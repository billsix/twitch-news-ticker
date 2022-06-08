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

#include "news_window.h"

struct nk_glfw glfw = {0};
GLFWwindow *control_window;


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

  if (!news_window_create()) {
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
         !news_window_should_close()) {

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

    news_window_draw();
  }

  // Cleanup
  glfwTerminate();
  return 0;
}



#ifdef __cplusplus
}
#endif
