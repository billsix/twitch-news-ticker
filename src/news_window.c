#include <stdbool.h>

#include <stdio.h>

#include "common.h"

#include "shader.h"

#include "main.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "gl-matrix-stack.h"

#include "news_window.h"

static GLFWwindow *news_window;

const int news_window_width = 1980;
const int news_window_height = 1080;
int farRight = 1920.0 / 2.0;

bool restart = false;

static GLuint textureID;

static GLuint textureLoc;

GLFWwindow *news_window_create() {

  // GLFWmonitor *monitor = glfwGetPrimaryMonitor();

  int count;
  GLFWmonitor **monitors = glfwGetMonitors(&count);
  const GLFWvidmode *mode = glfwGetVideoMode(monitors[1]);

  glfwWindowHint(GLFW_RED_BITS, mode->redBits);
  glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
  glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
  glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

  // needed so that a click doesn't make the window minimized
  glfwWindowHint(GLFW_AUTO_ICONIFY, GL_FALSE);

  return (news_window = glfwCreateWindow(mode->width, mode->height, "My Title",
                                         monitors[1], NULL));
}

bool news_window_should_close() { return glfwWindowShouldClose(news_window); }

void news_window_set_viewport() {
  int width = 0, height = 0;
  glfwGetFramebufferSize(news_window, &width, &height);
  glViewport(0, 0, width, height);
}

// for font loading
struct character {
  unsigned int TextureID; // ID handle of the glyph texture
  int Size[2];            // Size of glyph
  int Bearing[2];         // Offset from baseline to left/top of glyph
  unsigned int Advance;   // Offset to advance to next glyph
};

struct character characters[128];

static GLuint funometerProgramID;
static GLuint fontProgramID;

static void RenderText(const struct mat4_t *const matr, GLuint vao, GLuint vbo,
                       char *text, float x, float y, float scale, float r,
                       float g, float b) {
  // activate corresponding render state
  glUseProgram(fontProgramID);
  glUniform3f(glGetUniformLocation(fontProgramID, "textColor"), r, g, b);
  glUniformMatrix4fv(glGetUniformLocation(fontProgramID, "projection"), 1,
                     GL_FALSE, matr->m);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vao);

  for (int i = 0;; i++) {
    if (text[i] == 0)
      break;
    struct character ch = characters[text[i]];

    float xpos = x + ch.Bearing[0] * scale;
    float ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

    float w = ch.Size[0] * scale;
    float h = ch.Size[1] * scale;
    // update VBO for each character
    float vertices[6][4] = {
        {xpos, ypos + h, 0.0f, 0.0f},    {xpos, ypos, 0.0f, 1.0f},
        {xpos + w, ypos, 1.0f, 1.0f},

        {xpos, ypos + h, 0.0f, 0.0f},    {xpos + w, ypos, 1.0f, 1.0f},
        {xpos + w, ypos + h, 1.0f, 0.0f}};
    // render glyph texture over quad
    glBindTexture(GL_TEXTURE_2D, ch.TextureID);
    // update content of VBO memory
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    // render quad
    glDrawArrays(GL_TRIANGLES, 0, 6);
    // now advance cursors for next glyph (note that advance is number of 1/64
    // pixels)
    x += (ch.Advance >> 6) *
         scale; // bitshift by 6 to get value in pixels (2^6 = 64)
  }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void news_window_draw() {
  /* Make the window's context current */
  glfwMakeContextCurrent(news_window);

  news_window_set_viewport();

  static GLfloat texture_vertices[] = {// triangle 1
                                       -1.0, -1.0, 1.0, -1.00, -1.0, 1.0,
                                       // triangle 2
                                       1.0, 1.0, -1.0, 1.0, 1.0, -1.0};

  static bool firstExecution = true;

  static GLuint VAO;
  static GLuint vertex_buffer;
  static GLuint mvpMatrixLoc;

  static GLuint fontVAO;
  static GLuint fontVBO;

  if (firstExecution) {
    // load font
    {
      FT_Library ft;
      if (FT_Init_FreeType(&ft)) {
        printf("ERROR::FREETYPE: Could not init FreeType Library \n");
        exit(1);
      }

      FT_Face face;
      if (FT_New_Face(ft, FONT_DIR "TiroGurmukhi-Regular.ttf", 0, &face)) {
        printf("ERROR::FREETYPE: Failed to load font\n");
        exit(1);
      }

      FT_Set_Pixel_Sizes(face, 0, 64);

      if (FT_Load_Char(face, 'X', FT_LOAD_RENDER)) {
        printf("ERROR::FREETYTPE: Failed to load Glyph\n");
        exit(1);
      }

      {
        glPixelStorei(GL_UNPACK_ALIGNMENT,
                      1); // disable byte-alignment restriction

        for (unsigned char c = 0; c < 128; c++) {
          // load character glyph
          if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            printf("ERROR::FREETYTPE: Failed to load Glyph\n");
            continue;
          }
          // generate texture
          unsigned int texture;
          glGenTextures(1, &texture);
          glBindTexture(GL_TEXTURE_2D, texture);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
                       face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE,
                       face->glyph->bitmap.buffer);
          // set texture options
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
          // now store character for later use

          struct character character;
          {
            character.TextureID = texture;
            character.Size[0] = face->glyph->bitmap.width;
            character.Size[1] = face->glyph->bitmap.rows;
            character.Bearing[0] = face->glyph->bitmap_left;
            character.Bearing[1] = face->glyph->bitmap_top;
            character.Advance = face->glyph->advance.x;
          }

          characters[c] = character;
        }
      }

      FT_Done_Face(face);
      FT_Done_FreeType(ft);

      // font VAO/VBO
      {
        glGenVertexArrays(1, &fontVAO);
        glGenBuffers(1, &fontVBO);
        glBindVertexArray(fontVAO);
        glBindBuffer(GL_ARRAY_BUFFER, fontVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL,
                     GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
      }
    }

    // load textures
    {
      int width, height, numberOf8BitComponentsPerPixel;
      unsigned char *breakingnews =
          stbi_load(TEXTURE_DIR "breakingnews.png", &width, &height,
                    &numberOf8BitComponentsPerPixel, 0);

      const GLuint mode =
          numberOf8BitComponentsPerPixel == 4 ? GL_RGBA : GL_RGB;

      // Create one OpenGL texture
      {
        glGenTextures(1, &textureID);
        GL_DEBUG_ASSERT();
        glBindTexture(GL_TEXTURE_2D, textureID);
        GL_DEBUG_ASSERT();
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GL_DEBUG_ASSERT();
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        GL_DEBUG_ASSERT();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        GL_DEBUG_ASSERT();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        GL_DEBUG_ASSERT();
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        GL_DEBUG_ASSERT();
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        GL_DEBUG_ASSERT();

        glTexImage2D(GL_TEXTURE_2D, 0, mode, width, height, 0, mode,
                     GL_UNSIGNED_BYTE, breakingnews);
        GL_DEBUG_ASSERT();
        GL_DEBUG_ASSERT();
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        GL_DEBUG_ASSERT();
        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        GL_DEBUG_ASSERT();
        glGenerateMipmap(GL_TEXTURE_2D);
      }
      stbi_image_free(breakingnews);
    }

    glGenVertexArrays(1, &VAO);
    GL_DEBUG_ASSERT();

    glBindVertexArray(VAO);
    GL_DEBUG_ASSERT();

    glGenBuffers(1, &vertex_buffer);
    {
      // populate vertex buffer
      {
        // This will identify our vertex buffer
        // Generate 1 buffer, put the resulting identifier in vertexbuffer
        GL_DEBUG_ASSERT();
        // The following commands will talk about our 'vertexbuffer' buffer
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        GL_DEBUG_ASSERT();
        // Give our vertices to OpenGL.
        glBufferData(GL_ARRAY_BUFFER, sizeof(texture_vertices),
                     texture_vertices, GL_STATIC_DRAW);
        GL_DEBUG_ASSERT();
      }
    }

    enum Attribute_IDS { vPosition = 0 };

    // set the vertex data
    {
      glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
      GL_DEBUG_ASSERT();
      glEnableVertexAttribArray(vPosition);
      GL_DEBUG_ASSERT();
      glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0,
                            BUFFER_OFFSET(0));
      GL_DEBUG_ASSERT();
    }

    // load shaders
    {

      // for the window
      {
        GLuint vertexShaderID =
            compile_shader(GL_VERTEX_SHADER, SHADER_DIR "funometer.vert");

        GLuint fragmentShaderID =
            compile_shader(GL_FRAGMENT_SHADER, SHADER_DIR "funometer.frag");

        funometerProgramID = link_shaders(vertexShaderID, fragmentShaderID);
        GL_DEBUG_ASSERT();

        mvpMatrixLoc = glGetUniformLocation(funometerProgramID, "mvpMatrix");
        GL_DEBUG_ASSERT();
        textureLoc =
            glGetUniformLocation(funometerProgramID, "breakingNewsTexture");

        // clean up
        glDeleteShader(vertexShaderID);
        GL_DEBUG_ASSERT();
        glDeleteShader(fragmentShaderID);
        GL_DEBUG_ASSERT();
      }
      // for the fonts
      {
        GLuint vertexShaderID =
            compile_shader(GL_VERTEX_SHADER, SHADER_DIR "font.vert");

        GLuint fragmentShaderID =
            compile_shader(GL_FRAGMENT_SHADER, SHADER_DIR "font.frag");

        fontProgramID = link_shaders(vertexShaderID, fragmentShaderID);
        GL_DEBUG_ASSERT();

        // clean up
        glDeleteShader(vertexShaderID);
        GL_DEBUG_ASSERT();
        glDeleteShader(fragmentShaderID);
        GL_DEBUG_ASSERT();
      }
    }

    firstExecution = !firstExecution;
  }

  glUseProgram(funometerProgramID);

  // draw scene
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 1.0, 0.0, 1.0);

    // render chyron
    {
      // projection
      {
        enum matrixType m = PROJECTION;
        mat4_identity(m);
      }
      // view
      {
        enum matrixType m = VIEW;
        mat4_identity(m);
      }
      // model
      {
        enum matrixType m = MODEL;
        mat4_identity(m);
      }

      glBindVertexArray(VAO);
      GL_DEBUG_ASSERT();

      // model
      {
        enum matrixType m = MODEL;
        mat4_identity(m);
      }

      {
        vec3_t t[] = {0.0, -0.8, 0.0};
        mat4_translate(MODEL, t);
        vec3_t s[] = {1.0, -0.2, 1.0};
        mat4_scale(MODEL, s);
      }

      const struct mat4_t *const matr = mat4_get_matrix(MODELVIEWPROJECTION);

      glUniformMatrix4fv(mvpMatrixLoc, 1, GL_FALSE, matr->m);
      GL_DEBUG_ASSERT();

      glActiveTexture(GL_TEXTURE0);
      GL_DEBUG_ASSERT();
      glUniform1i(textureLoc, 0);
      GL_DEBUG_ASSERT();
      glBindTexture(GL_TEXTURE_2D, textureID);
      GL_DEBUG_ASSERT();

      static GLuint numVertices = ARRAY_SIZE(texture_vertices) / 2;

      // Draw the triangles !
      glDrawArrays(GL_TRIANGLES, 0, numVertices);
      GL_DEBUG_ASSERT();
      glUseProgram(0);
      glBindVertexArray(0);
    }
    // render text
    {
      // projection
      {
        enum matrixType m = PROJECTION;
        mat4_ortho(0.0, news_window_width, 0.0, news_window_height, -100.0,
                   100.0);
      }
      // view
      {
        enum matrixType m = VIEW;
        mat4_identity(m);
      }
      // model
      {
        enum matrixType m = MODEL;
        mat4_identity(m);
      }

      vec3_t t[] = {0.0, -430, 0.0};
      mat4_translate(MODEL, t);

      const struct mat4_t *const matr = mat4_get_matrix(MODELVIEWPROJECTION);

      glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // junk code
      {
        RenderText(matr, fontVAO, fontVBO, main_comment, -900.0, -20.0, 1.0,
                   0.0, 0.0, 0.0);
      }

      // junk code
      {
        if (restart) {
          farRight = news_window_width / 2.0;
          restart = false;
        }
        farRight -= 8;
        RenderText(matr, fontVAO, fontVBO,
                   "Queshon: HAHA MRPILLOWFORT MORE LIKE MRPILLOWDEMONED",
                   farRight, -100.0, 1.0, 1.0, 1.0, 1.0);
      }
      glEnable(GL_DEPTH_TEST);
    }
  }
  glfwSwapBuffers(news_window);
}
