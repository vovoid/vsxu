/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#ifndef VSX_TEXTURE_H
#define VSX_TEXTURE_H
#include <vsx_platform.h>
#include <engine_graphics_dllimport.h>
#include <string/vsx_string.h>
#include <vsxg.h>
#include <vsx_bitmap.h>
#include <vsx_matrix.h>

#include "vsx_texture_data.h"
#include "vsx_texture_gl.h"
#include "vsx_texture_transform_base.h"

// Frame Buffer Types (see frame_buffer_type)
#define VSX_TEXTURE_BUFFER_TYPE_RENDER_BUFFER 1
#define VSX_TEXTURE_BUFFER_TYPE_COLOR 2
#define VSX_TEXTURE_BUFFER_TYPE_COLOR_DEPTH 3

class vsx_texture
{
  GLint prev_buf;

  // FBO
  bool valid_fbo;
  int frame_buffer_type;
  GLuint frame_buffer_handle;

  GLuint color_buffer_handle;

  GLuint depth_buffer_handle;
  bool depth_buffer_local;

  // Render buffer variables
  GLuint render_buffer_color_handle;
  GLuint render_buffer_depth_handle;

  // temporary fbo handle to hold destination tex
  GLuint frame_buffer_blit_color_texture;
  GLuint frame_buffer_fbo_attachment_texture;
  GLuint frame_buffer_blit_handle;

  // save state for buffer capture
  vsx_matrix<> buffer_save_matrix[3];
  int buffer_save_blend;
  int viewport_size[4];

  // to prevent double-begins, lock the buffer
  bool capturing_to_buffer;

public:

  vsx_string<> filename;

  // transformation object
  vsx_texture_transform_base* transform_obj;
  vsx_texture_data* texture_data;
  vsx_texture_gl* texture_gl;

  // FBO functions-------------------------------------------------------------
  // FBO is used to capture rendering output into a texture rather than to the
  // screen for re-use in other rendering operations.

  // query if the hardware has Frame Buffer Object support
  VSX_ENGINE_GRAPHICS_DLLIMPORT bool has_buffer_support();

  // init an offscreen feedback possible buffer
  VSX_ENGINE_GRAPHICS_DLLIMPORT void init_render_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    bool multisample = false // enable anti-aliasing
  );

  // run in stop/start or when changing resolution
  VSX_ENGINE_GRAPHICS_DLLIMPORT void reinit_render_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    bool multisample = false // enable anti-aliasing
  );
  VSX_ENGINE_GRAPHICS_DLLIMPORT void deinit_render_buffer();



  // init an offscreen feedback possible buffer
  VSX_ENGINE_GRAPHICS_DLLIMPORT void init_color_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true // support alpha channel or not
  );

  // run in stop/start or when changing resolution
  VSX_ENGINE_GRAPHICS_DLLIMPORT void reinit_color_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true // support alpha channel or not
  );

  VSX_ENGINE_GRAPHICS_DLLIMPORT void deinit_color_buffer();


  // init an offscreen feedback possible buffer
  VSX_ENGINE_GRAPHICS_DLLIMPORT void init_color_depth_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    GLuint existing_depth_texture_id = 0,
    bool multisample = false
  );

  // run in stop/start or when changing resolution
  VSX_ENGINE_GRAPHICS_DLLIMPORT void reinit_color_depth_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    GLuint existing_depth_texture_id = 0
  );

  VSX_ENGINE_GRAPHICS_DLLIMPORT void deinit_color_depth_buffer();

  // remove/delete the buffer
  VSX_ENGINE_GRAPHICS_DLLIMPORT void deinit_buffer();



  // begin capturing render output into the frame buffer object
  VSX_ENGINE_GRAPHICS_DLLIMPORT void begin_capture_to_buffer();

  // end the capturing render output into the frame buffer object
  VSX_ENGINE_GRAPHICS_DLLIMPORT void end_capture_to_buffer();

  VSX_ENGINE_GRAPHICS_DLLIMPORT bool is_valid_capture_buffer();

  VSX_ENGINE_GRAPHICS_DLLIMPORT GLuint get_depth_buffer_handle();


  // TODO: put these in a class vsx_texture_gl_loader
  // upload a bitmap from RAM to the GPU
  // as an openGL texture. requires that init_opengl_texture
  // has been run.
  VSX_ENGINE_GRAPHICS_DLLIMPORT void upload_ram_bitmap_1d(void* data, unsigned long size, bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT);
  VSX_ENGINE_GRAPHICS_DLLIMPORT void upload_ram_bitmap_2d(vsx_bitmap* vbitmap,bool mipmaps = false, bool upside_down = true);
  VSX_ENGINE_GRAPHICS_DLLIMPORT void upload_ram_bitmap_2d(void* data, unsigned long size_x, unsigned long size_y,bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT, bool upside_down = true);

  // assumes width is 6x height (maps in order: -x, z, x, -z, -y, y
  VSX_ENGINE_GRAPHICS_DLLIMPORT void upload_ram_bitmap_cube(void* data, unsigned long size_x, unsigned long size_y,bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT, bool upside_down = true);

  void set_transform(vsx_texture_transform_base* new_transform_obj)
  {
    transform_obj = new_transform_obj;
  }

  vsx_texture_transform_base* get_transform()
  {
    return transform_obj;
  }

  inline bool is_valid()
  {
    if (!texture_gl)
      return false;

    if (!texture_gl->gl_id)
      return false;

    return true;
  }

  // use this to load/unload the texture from vram
  VSX_ENGINE_GRAPHICS_DLLIMPORT void upload_gl();
  VSX_ENGINE_GRAPHICS_DLLIMPORT void unload_gl();


  // use this to bind the texture.
  VSX_ENGINE_GRAPHICS_DLLIMPORT bool bind();

  // use this when you're done with the texture
  VSX_ENGINE_GRAPHICS_DLLIMPORT void _bind();

  // constructors

  vsx_texture(bool attached_to_cache = false)
    :
      prev_buf(0),
      valid_fbo(false),
      frame_buffer_type(0),
      frame_buffer_handle(0),
      color_buffer_handle(0),
      depth_buffer_handle(0),
      depth_buffer_local(true),
      render_buffer_color_handle(0),
      render_buffer_depth_handle(0),
      frame_buffer_blit_color_texture(0),
      frame_buffer_fbo_attachment_texture(0),
      frame_buffer_blit_handle(0),
      buffer_save_blend(0),
      viewport_size({0,0,0,0}),
      capturing_to_buffer(false),
      transform_obj(0x0),
      texture_data(0x0),
      texture_gl(0x0)
  {
    if (!attached_to_cache)
    {
      texture_gl = new vsx_texture_gl(true);
      texture_data = new vsx_texture_data(0, true);
    }
  }

  ~vsx_texture()
  {
    if (texture_data && !texture_data->attached_to_cache)
      delete texture_data;

    if (texture_gl && !texture_gl->attached_to_cache)
      delete texture_gl;
  }
};

#include "vsx_texture_transform_helper.h"
#include "vsx_texture_data_loader_helper.h"

#endif
