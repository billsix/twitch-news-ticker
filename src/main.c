/*
 * William Emerison Six
 *
 * Copyright 2016-2018 - William Emerison Six
 * All rights reserved
 * Distributed under Apache 2.0
 */

#include <stdbool.h>

#include <stdio.h>

#define NK_IMPLEMENTATION
#define NK_GLFW_GL3_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT
#define MAX_VERTEX_MEMORY 512 * 1024
#define MAX_ELEMENT_MEMORY 128 * 1024
#define GL3W_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "common.h"

#include "shader.h"

#include "main.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#define GL_MATRIX_STACK_IMPLEMENTATION
#include "gl-matrix-stack.h"

struct nk_glfw glfw = {0};
GLFWwindow *control_window;
GLFWwindow *news_window;

static GLuint textureID;

static GLuint textureLoc;

// show the nuklear GUIs or not.  pressing escape toggles
// it
bool guiEnable = true;
static int funLevel = 0; // can go up to 10

static void error_callback(int error, const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action,
                         int mods) {
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    // glfwSetWindowShouldClose(window, GLFW_TRUE);
    guiEnable = !guiEnable;
  }
}

const int news_window_width = 1920;
const int news_window_height = 250;
int farRight = 1920.0 / 2.0;

bool restart = false;


void draw_fun_o_meter_window();

#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char **argv) {

  struct nk_colorf bg;
  bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

  glfwSetErrorCallback(error_callback);
  /* Initialize the library */
  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  /* Create a windowed mode window and its OpenGL context */
  if (!(control_window =
            glfwCreateWindow(640, 480, "Bill's Twitch Control", NULL, NULL))) {
    glfwTerminate();
    return -1;
  }

  if (!(news_window =
        glfwCreateWindow(news_window_width, news_window_height, "Breaking NEWS", NULL, NULL))) {
    glfwTerminate();
    return -1;
  }

  glfwSetKeyCallback(control_window, key_callback);

  /* Make the window's context current */
  glfwMakeContextCurrent(control_window);

  // init GL3W
  {
    if (gl3w_init()) {
      printf("Could not init gl3w\n");
      return 1;
    }
  }

  // initialize OpenGL
  {
    GL_DEBUG_ASSERT();
    glClearColor(0.5, 0.5, 0.5, 1.0);
    GL_DEBUG_ASSERT();
    glClearDepth(1.0f);
    GL_DEBUG_ASSERT();
    glEnable(GL_DEPTH_TEST);
    GL_DEBUG_ASSERT();
    glDepthFunc(GL_LEQUAL);
    GL_DEBUG_ASSERT();


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_DEBUG_ASSERT();
  }

  // set viewport
  {
    int width = 0, height = 0;
    glfwGetFramebufferSize(control_window, &width, &height);
    glViewport(0, 0, width, height);
  }

  // nuklear context
  struct nk_context *ctx =
      nk_glfw3_init(&glfw, control_window, NK_GLFW3_INSTALL_CALLBACKS);
  {
    /* Load Fonts: if none of these are loaded a default font will be used  */
    /* Load Cursor: if you uncomment cursor loading please hide the cursor */
    {
      struct nk_font_atlas *atlas;
      nk_glfw3_font_stash_begin(&glfw, &atlas);
      /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas,
       * "../../../extra_font/DroidSans.ttf", 14, 0);*/
      /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas,
       * "../../../extra_font/Roboto-Regular.ttf", 16, 0);*/
      /*struct nk_font *future = nk_font_atlas_add_from_file(atlas,
       * "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
      /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas,
       * "../../../extra_font/ProggyClean.ttf", 12, 0);*/
      /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas,
       * "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
      /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas,
       * "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/
      nk_glfw3_font_stash_end(&glfw);
      /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
      /*nk_style_set_font(ctx, &roboto->handle)*/;
    }

    /* style.c */
    /*set_style(ctx, THEME_WHITE);*/
    /*set_style(ctx, THEME_RED);*/
    /*set_style(ctx, THEME_BLUE);*/
    /*set_style(ctx, THEME_DARK);*/
  }

  // The event loop.  Keep on truckin'.
  while (!glfwWindowShouldClose(control_window) &&
         !glfwWindowShouldClose(news_window)) {

    /* Poll for and process events */
    glfwPollEvents();

    // control window
    {
      /* Make the window's context current */
      glfwMakeContextCurrent(control_window);

      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(bg.r, bg.g, bg.b, bg.a);

      nk_glfw3_new_frame(&glfw);

      /* GUI */
      if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
                   NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_SCALABLE |
                       NK_WINDOW_MINIMIZABLE | NK_WINDOW_TITLE)) {
        enum { EASY, HARD };
        static int op = EASY;
        static int property = 20;
        nk_layout_row_static(ctx, 30, 80, 1);
        if (nk_button_label(ctx, "button"))
          fprintf(stdout, "button pressed\n");

        if (nk_button_label(ctx, "Fun Level 0 "))
          restart = true;
        if (nk_button_label(ctx, "Fun Level 1 "))
          funLevel = 1;
        if (nk_button_label(ctx, "Fun Level 3 "))
          funLevel = 3;
        if (nk_button_label(ctx, "Fun Level 5 "))
          funLevel = 5;
        if (nk_button_label(ctx, "Fun Level 7 "))
          funLevel = 7;
        if (nk_button_label(ctx, "Fun Level 9 "))
          funLevel = 9;

        nk_layout_row_dynamic(ctx, 30, 2);
        if (nk_option_label(ctx, "easy", op == EASY))
          op = EASY;
        if (nk_option_label(ctx, "hard", op == HARD))
          op = HARD;

        nk_layout_row_dynamic(ctx, 25, 1);
        nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

        nk_layout_row_dynamic(ctx, 20, 1);
        nk_label(ctx, "background:", NK_TEXT_LEFT);
        nk_layout_row_dynamic(ctx, 25, 1);
        if (nk_combo_begin_color(ctx, nk_rgb_cf(bg),
                                 nk_vec2(nk_widget_width(ctx), 400))) {
          nk_layout_row_dynamic(ctx, 120, 1);
          bg = nk_color_picker(ctx, bg, NK_RGBA);
          nk_layout_row_dynamic(ctx, 25, 1);
          bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f, 0.005f);
          bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f, 0.005f);
          bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f, 0.005f);
          bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f, 0.005f);
          nk_combo_end(ctx);
        }
      }
      nk_end(ctx);

      // render nuklear without changing global state
      {
        /* IMPORTANT: `nk_glfw3_render` modifies some global OpenGL state
         * with blending, scissor, face culling, depth test and viewport and
         * defaults everything back into a default state.
         * Make sure to either a.) save and restore or b.) reset your own state
         * after rendering the UI. */
        // Backup GL state
        GLint last_program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        GLint last_active_texture;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
        GLint last_array_buffer;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_element_array_buffer;
        glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,
                      &last_element_array_buffer);
        GLint last_vertex_array;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
        GLint last_blend_src;
        glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
        GLint last_blend_dst;
        glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
        GLint last_blend_equation_rgb;
        glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
        GLint last_blend_equation_alpha;
        glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
        GLint last_viewport[4];
        glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLint last_scissor_box[4];
        glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);

        // render nuklear
        nk_glfw3_render(&glfw, NK_ANTI_ALIASING_ON, MAX_VERTEX_MEMORY,
                        MAX_ELEMENT_MEMORY);

        // Restore modified GL state
        glUseProgram(last_program);
        glActiveTexture(last_active_texture);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindVertexArray(last_vertex_array);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb,
                                last_blend_equation_alpha);
        glBlendFunc(last_blend_src, last_blend_dst);
        if (last_enable_blend)
          glEnable(GL_BLEND);
        else
          glDisable(GL_BLEND);
        if (last_enable_cull_face)
          glEnable(GL_CULL_FACE);
        else
          glDisable(GL_CULL_FACE);
        if (last_enable_depth_test)
          glEnable(GL_DEPTH_TEST);
        else
          glDisable(GL_DEPTH_TEST);
        if (last_enable_scissor_test)
          glEnable(GL_SCISSOR_TEST);
        else
          glDisable(GL_SCISSOR_TEST);
        glViewport(last_viewport[0], last_viewport[1],
                   (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
        glScissor(last_scissor_box[0], last_scissor_box[1],
                  (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
      }
      glfwSwapBuffers(control_window);
    }

    draw_fun_o_meter_window();
  }

  // Cleanup
  glfwTerminate();
  return 0;
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


  void RenderText(const struct mat4_t *const matr, GLuint vao, GLuint vbo, char * text, float x, float y, float scale, float r, float g, float b)
  {
    // activate corresponding render state
  glUseProgram(fontProgramID);
  glUniform3f(glGetUniformLocation(fontProgramID, "textColor"), r,g,b);
  glUniformMatrix4fv(glGetUniformLocation(fontProgramID, "projection"), 1, GL_FALSE, matr->m);

  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vao);

  for (int i = 0; ; i++)
    {
      if (text[i] == 0)
        break;
      struct character ch = characters[text[i]];

      float xpos = x + ch.Bearing[0] * scale;
      float ypos = y - (ch.Size[1] - ch.Bearing[1]) * scale;

      float w = ch.Size[0] * scale;
      float h = ch.Size[1] * scale;
      // update VBO for each character
      float vertices[6][4] = {
        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos,     ypos,       0.0f, 1.0f },
        { xpos + w, ypos,       1.0f, 1.0f },

        { xpos,     ypos + h,   0.0f, 0.0f },
        { xpos + w, ypos,       1.0f, 1.0f },
        { xpos + w, ypos + h,   1.0f, 0.0f }
      };
      // render glyph texture over quad
      glBindTexture(GL_TEXTURE_2D, ch.TextureID);
      // update content of VBO memory
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
      x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
    }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void draw_fun_o_meter_window() {

  /* Make the window's context current */
  glfwMakeContextCurrent(news_window);

  // set viewport
  {
    int width = 0, height = 0;
    glfwGetFramebufferSize(news_window, &width, &height);
    glViewport(0, 0, width, height);
  }

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
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte-alignment restriction

        for (unsigned char c = 0; c < 128; c++)
          {
            // load character glyph
              if (FT_Load_Char(face, c, FT_LOAD_RENDER))
                {
                  printf("ERROR::FREETYTPE: Failed to load Glyph\n");
                  continue;
                }
              // generate texture
              unsigned int texture;
              glGenTextures(1, &texture);
              glBindTexture(GL_TEXTURE_2D, texture);
              glTexImage2D(
                           GL_TEXTURE_2D,
                           0,
                           GL_RED,
                           face->glyph->bitmap.width,
                           face->glyph->bitmap.rows,
                           0,
                           GL_RED,
                           GL_UNSIGNED_BYTE,
                           face->glyph->bitmap.buffer
                           );
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
                character.Bearing[1] =  face->glyph->bitmap_top;
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
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
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
        vec3_t s[] = {1.0, -1.0, 1.0};
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
        mat4_ortho(0.0, news_window_width, 0.0, news_window_height, -100.0, 100.0);
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

      const struct mat4_t *const matr = mat4_get_matrix(MODELVIEWPROJECTION);

      glDisable(GL_DEPTH_TEST);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

      // junk code
      {
        if (restart)
          {
            farRight = news_window_width / 2.0;
            restart = false;
          }
        farRight-= 3;
        RenderText(matr, fontVAO, fontVBO, "Pillow got Demoned", farRight, -20.0, 1.0, 0.0, 0.0, 0.0);
      }
      glEnable(GL_DEPTH_TEST);

    }
  }
  glfwSwapBuffers(news_window);
}
#ifdef __cplusplus
}
#endif
