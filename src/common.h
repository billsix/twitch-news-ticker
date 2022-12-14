/*
 * William Emerison Six
 *
 * Copyright 2016-2018 - William Emerison Six
 * All rights reserved
 * Distributed under Apache 2.0
 */

#ifndef COMMON_H
#define COMMON_H 1

#define M_PI acos(-1.0)

#ifdef __cplusplus
#define BEGIN_C_DECLS extern "C" {
#define END_C_DECLS }
#else
#define BEGIN_C_DECLS
#define END_C_DECLS
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#elif CMAKE_CONFIG
// do nothing
#else // Visual Studio
#define FONT_DIR "font/"
#define SHADER_DIR "shaders/"
#define TEXTURE_DIR "textures/"
#endif

#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <tchar.h>
#include <windows.h>
#endif

#include "gl3w.h"
#include <GLFW/glfw3.h>
#include <assert.h>

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <signal.h>

// nuklear defs
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#include "nuklear.h"
#include "nuklear_glfw_gl3.h"

#include "stb_image.h"

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

#define NOOP() (void)0

// Uncomment the following flag to debug OpenGL calls
#define DEBUG 1

#ifdef DEBUG
#define GL_DEBUG_ASSERT()                                                      \
  {                                                                            \
    GLenum error = glGetError();                                               \
    if (error != GL_NO_ERROR) {                                                \
      switch (error) {                                                         \
      case GL_INVALID_ENUM:                                                    \
        printf("An unacceptable value is specified for an enumerated "         \
               "argument. The offending command is ignored and has no "        \
               "other side effect than to set the error flag.\n");             \
        break;                                                                 \
      case GL_INVALID_VALUE:                                                   \
        printf("A numeric argument is out of range. The offending "            \
               "command is ignored and has no other side effect than "         \
               "to set the error flag.\n");                                    \
        break;                                                                 \
      case GL_INVALID_OPERATION:                                               \
        printf("The specified operation is not allowed in the current "        \
               "state. The offending command is ignored and has no "           \
               "other side effect than to set the error flag.\n");             \
        break;                                                                 \
      case GL_INVALID_FRAMEBUFFER_OPERATION:                                   \
        printf("The framebuffer object is not complete. The offending "        \
               "command is ignored and has no other side effect than "         \
               "to set the error flag.\n");                                    \
        break;                                                                 \
      case GL_OUT_OF_MEMORY:                                                   \
        printf("There is not enough memory left to execute the "               \
               "command. The state of the GL is undefined, except for "        \
               "the state of the error flags, after this error is "            \
               "recorded.\n");                                                 \
        break;                                                                 \
      case GL_STACK_UNDERFLOW:                                                 \
        printf("An attempt has been made to perform an operation that "        \
               "would cause an internal stack to underflow.\n");               \
        break;                                                                 \
      case GL_STACK_OVERFLOW:                                                  \
        printf("An attempt has been made to perform an operation that "        \
               "would cause an internal stack to overflow. \n");               \
        break;                                                                 \
      }                                                                        \
      assert(error != GL_NO_ERROR);                                            \
      raise(SIGINT);                                                           \
    }                                                                          \
  }
#else
#define GL_DEBUG_ASSERT() NOOP()
#endif

#define BUFFER_OFFSET(offset) ((void *)(offset))

#endif
