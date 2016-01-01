/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <stdio.h>
#include "GL/glfw.h"
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <filesystem/vsx_filesystem.h>
#include <stdlib.h>
#include "vsx_gl_state.h"

int main(int argc, char* argv[])
{
  int     width, height, running, frames, x, y;

  // Initialise GLFW
  glfwInit();

  // Open OpenGL window
  if( !glfwOpenWindow( 100, 100, 0,0,0,0,16,0, start_fullscreen?GLFW_FULLSCREEN:GLFW_WINDOW ) ) // GLFW_FULLSCREEN
  {
    printf("Error! Could not create an OpenGL context. Please check your GPU drivers...\n");
    glfwTerminate();
    return 0;
  }
  if (start_fullscreen) glfwEnable( GLFW_MOUSE_CURSOR );
  app_init(0);

  glfwEnable(GLFW_AUTO_POLL_EVENTS);

  // Enable sticky keys
  glfwEnable( GLFW_STICKY_KEYS );
  glfwSwapInterval(1);
  // Main loop
  running = GL_TRUE;
  frames = 0;

  // Get window size (may be different than the requested size)
  glfwGetWindowSize( &width, &height );
  height = height > 0 ? height : 1;

  // Set viewport
  glViewport( 0, 0, width, height );

  glMatrixMode(GL_MODELVIEW);
  vsx_matrix result_our;
  vsx_matrix result_opengl;

  vsx_matrix identity;
  for (size_t i = 0; i < 16; i++) identity.m[i] = rand()%1000*0.001f;
  ///////////////////////////////////////////////////////////////////////////
  // test matrix multiplication
  vsx_matrix trans;
  for (size_t i = 0; i < 16; i++) trans.m[i] = rand()%1000*0.001f;
  // 1.1 our own mult
  result_our.multiply( &trans, &identity);
  // 1.2 opengl mult
  glLoadIdentity();
  glMultMatrixf( identity.m );
  glMultMatrixf( trans.m );
  glGetFloatv(GL_MODELVIEW_MATRIX, result_opengl.m);

  printf("Test Matrix Multiplications\n");
  printf("our    opengl\n");
  for (size_t i = 0; i < 16; i++)
  {
    printf("%f ",result_our.m[i]);
    printf("%f ",result_opengl.m[i]);
    if (result_our.m[i] == result_opengl.m[i]) printf(" OK!");
    printf("\n");
  }
  printf("---end of test---\n");
  fflush(stdout);

  ///////////////////////////////////////////////////////////////////////////
  // test matrix translation
  matrix_mode( VSX_GL_MODELVIEW_MATRIX );
  core_matrix[ VSX_GL_MODELVIEW_MATRIX ] = trans;
  matrix_translate_f(0.2816, -1.028, 7.819);
  result_our = core_matrix[ VSX_GL_MODELVIEW_MATRIX ];

  // 1.1 our own mult

  // 1.2 opengl mult
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf( trans.m );
  glTranslatef(0.2816, -1.028, 7.819);
  glGetFloatv(GL_MODELVIEW_MATRIX, result_opengl.m);

  printf("Test Matrix Translation\n");
  printf("our    opengl\n");
  for (size_t i = 0; i < 16; i++)
  {
    printf("%f ",result_our.m[i]);
    printf("%f ",result_opengl.m[i]);
    if (result_our.m[i] == result_opengl.m[i]) printf(" OK!");
    printf("\n");
  }
  printf("---end of test---\n");
  fflush(stdout);

  ///////////////////////////////////////////////////////////////////////////
  // test matrix rotation
  matrix_mode( VSX_GL_MODELVIEW_MATRIX );
  core_matrix[ VSX_GL_MODELVIEW_MATRIX ] = trans;
  matrix_rotate_f(77.2, 0.2816, -1.028, 7.819);
  result_our = core_matrix[ VSX_GL_MODELVIEW_MATRIX ];

  // 1.1 our own mult

  // 1.2 opengl mult
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf( trans.m );
  glRotatef(77.2, 0.2816, -1.028, 7.819);
  glGetFloatv(GL_MODELVIEW_MATRIX, result_opengl.m);

  printf("Test Matrix Rotation");
  printf("our    opengl\n");
  for (size_t i = 0; i < 16; i++)
  {
    printf("%f ",result_our.m[i]);
    printf("%f ",result_opengl.m[i]);
    if (result_our.m[i] == result_opengl.m[i]) printf(" OK!");
    printf("\n");
  }
  printf("---end of test---\n");
  fflush(stdout);


  ///////////////////////////////////////////////////////////////////////////
  // test gluperspective with aspect = 1
  printf("Test gluPerspective with aspect = 1\n");
  // 2.1 our own
  matrix_glu_perspective(60.0, 1.0, 0.01, 2000.0);
  result_our = core_matrix[VSX_GL_PROJECTION_MATRIX];

  // 2.2 opengl
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,1.0,0.01,2000.0);
  glGetFloatv(GL_PROJECTION_MATRIX, result_opengl.m);

  printf("our    opengl\n");
  for (size_t i = 0; i < 16; i++)
  {
    printf("%f ",result_our.m[i]);
    printf("%f ",result_opengl.m[i]);
    if (result_our.m[i] == result_opengl.m[i]) printf(" OK!");

    printf("\n");
  }
  printf("---end of test---\n");
  fflush(stdout);

  // 3. test gluperspective with aspect = 0.87
  printf("Test gluPerspective with aspect = 0.87\n");
  // 2.1 our own
  matrix_glu_perspective(60.0, 0.87, 0.01, 2000.0);
  result_our = core_matrix[VSX_GL_PROJECTION_MATRIX];

  // 2.2 opengl
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0,0.87,0.01,2000.0);
  glGetFloatv(GL_PROJECTION_MATRIX, result_opengl.m);

  printf("our    opengl\n");
  for (size_t i = 0; i < 16; i++)
  {
    printf("%f ",result_our.m[i]);
    printf("%f ",result_opengl.m[i]);
    if (result_our.m[i] == result_opengl.m[i]) printf(" OK!");

    printf("\n");
  }
  printf("---end of test---\n");
  fflush(stdout);

  ///////////////////////////////////////////////////////////////////////////
  // test gluLookat with center = 0,0,0

  printf("Test gluLookAt #1\n");
  // 2.1 our own
  matrix_mode( VSX_GL_MODELVIEW_MATRIX );
  matrix_load_identity();
  matrix_glu_lookat(
    -1.87,  0.37, -0.2,
    0.0,    0.0,   0.0,
    0.0,    1.0,   0.0
  );
  result_our = core_matrix[VSX_GL_MODELVIEW_MATRIX];

  // 2.2 opengl
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(
    -1.87,  0.37, -0.2,
    0.0,    0.0,   0.0,
    0.0,    1.0,   0.0
  );
  glGetFloatv(GL_MODELVIEW_MATRIX, result_opengl.m);

  printf("our    opengl\n");
  for (size_t i = 0; i < 16; i++)
  {
    printf("%f ",result_our.m[i]);
    printf("%f ",result_opengl.m[i]);
    if (result_our.m[i] == result_opengl.m[i]) printf(" OK!");

    printf("\n");
  }
  printf("---end of test---\n");
  fflush(stdout);

  ///////////////////////////////////////////////////////////////////////////
  // test glOrtho

  printf("Test glOrtho\n");
  // 2.1 our own
  matrix_mode( VSX_GL_MODELVIEW_MATRIX );
  matrix_load_identity();
  matrix_ortho(
        -0.5,0.5,
        -0.5,0.5,
        0.01,2.0
        );
  result_our = core_matrix[VSX_GL_MODELVIEW_MATRIX];

  // 2.2 opengl
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glOrtho(
    -0.5,0.5,
    -0.5,0.5,
    0.01,2.0
  );
  glGetFloatv(GL_MODELVIEW_MATRIX, result_opengl.m);

  printf("our    opengl\n");
  for (size_t i = 0; i < 16; i++)
  {
    printf("%f ",result_our.m[i]);
    printf("%f ",result_opengl.m[i]);
    if (result_our.m[i] == result_opengl.m[i]) printf(" OK!");

    printf("\n");
  }
  printf("---end of test---\n");
  fflush(stdout);

  ///////////////////////////////////////////////////////////////////////////
  // test gluOrtho2D

  printf("Test gluOrtho2D\n");
  // 2.1 our own
  matrix_glu_ortho_2d(
        -0.5,0.5,
        -0.5,0.5
        );
  result_our = core_matrix[VSX_GL_PROJECTION_MATRIX];

  // 2.2 opengl
  gluOrtho2D(
    -0.5,0.5,
    -0.5,0.5
  );
  glGetFloatv(GL_PROJECTION_MATRIX, result_opengl.m);

  printf("our    opengl\n");
  for (size_t i = 0; i < 16; i++)
  {
    printf("%f ",result_our.m[i]);
    printf("%f ",result_opengl.m[i]);
    if (result_our.m[i] == result_opengl.m[i]) printf(" OK!");

    printf("\n");
  }
  printf("---end of test---\n");
  fflush(stdout);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}











