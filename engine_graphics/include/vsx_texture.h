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


#ifndef VSX_TEXTURE_LIB_H
#define VSX_TEXTURE_LIB_H
#include <map>
#include <vsx_texture_info.h>
#include <vsx_string.h>
#include <vsx_bitmap.h>
#include <vsxfst.h>
#include <vsx_math_3d.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #define VSX_TEXTURE_DLLIMPORT
#else
  #if defined(VSX_ENG_DLL)
    #define VSX_TEXTURE_DLLIMPORT __declspec (dllexport)
  #else 
    #define VSX_TEXTURE_DLLIMPORT __declspec (dllimport)
  #endif
#endif


// Frame Buffer Types (see frame_buffer_type)
#define VSX_TEXTURE_BUFFER_TYPE_RENDER_BUFFER 1
#define VSX_TEXTURE_BUFFER_TYPE_COLOR 2
#define VSX_TEXTURE_BUFFER_TYPE_COLOR_DEPTH 3


class vsx_texture
{
  static std::map<vsx_string, vsx_texture_info> t_glist;

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
  GLuint frame_buffer_blit_handle;

  // save state for buffer capture
  vsx_matrix buffer_save_matrix[3];


  int original_transform_obj;
  void* gl_state;


public:
  // this is if another texture gets a texture already in the list, to prevent it from unloading.
  // if not locked it can safely delete it. This is an approximation of course, but should work
  // in most cases.
  bool locked;

  // name of the texture
  vsx_string name;

  // is this valid for binding:
  bool valid;

  // transformation object
  vsx_transform_obj* transform_obj;

  // our texture info
  vsx_texture_info texture_info;

  // GL State
  void set_gl_state(void* n);

  // FBO functions-------------------------------------------------------------
  // FBO is used to capture rendering output into a texture rather than to the
  // screen for re-use in other rendering operations.

  // query if the hardware has Frame Buffer Object support
  VSX_TEXTURE_DLLIMPORT bool has_buffer_support();




  // init an offscreen feedback possible buffer
  VSX_TEXTURE_DLLIMPORT void init_render_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    bool multisample = false // enable anti-aliasing
  );

  // run in stop/start or when changing resolution
  VSX_TEXTURE_DLLIMPORT void reinit_render_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    bool multisample = false // enable anti-aliasing
  );
private:
  VSX_TEXTURE_DLLIMPORT void deinit_render_buffer();



public:
  // init an offscreen feedback possible buffer
  VSX_TEXTURE_DLLIMPORT void init_color_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true // support alpha channel or not
  );

  // run in stop/start or when changing resolution
  VSX_TEXTURE_DLLIMPORT void reinit_color_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true // support alpha channel or not
  );

private:
  VSX_TEXTURE_DLLIMPORT void deinit_color_buffer();



public:
  // init an offscreen feedback possible buffer
  VSX_TEXTURE_DLLIMPORT void init_color_depth_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    GLuint existing_depth_texture_id = 0
  );

  // run in stop/start or when changing resolution
  VSX_TEXTURE_DLLIMPORT void reinit_color_depth_buffer
  (
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    GLuint existing_depth_texture_id = 0
  );

private:
  VSX_TEXTURE_DLLIMPORT void deinit_color_depth_buffer();



public:
  // remove/delete the buffer
  VSX_TEXTURE_DLLIMPORT void deinit_buffer();



  // begin capturing render output into the frame buffer object
  VSX_TEXTURE_DLLIMPORT void begin_capture_to_buffer();

  // end the capturing render output into the frame buffer object
  VSX_TEXTURE_DLLIMPORT void end_capture_to_buffer();


  VSX_TEXTURE_DLLIMPORT GLuint get_depth_buffer_handle();


  // General texture functions-------------------------------------------------
  // allocate an openGL texture ID
  VSX_TEXTURE_DLLIMPORT void init_opengl_texture_1d();
  VSX_TEXTURE_DLLIMPORT void init_opengl_texture_2d();
  VSX_TEXTURE_DLLIMPORT void init_opengl_texture_cubemap();

  // reuploads all textures in the t_glist so you don't have to bother :)
  // just use thie in the start function of the module and all should be ok unless you use
  // the buffer. then see above.
  VSX_TEXTURE_DLLIMPORT void reinit_all_active();
  VSX_TEXTURE_DLLIMPORT void unload_all_active();

  // upload a bitmap from RAM to the GPU
  // as an openGL texture. requires that init_opengl_texture
  // has been run.
  VSX_TEXTURE_DLLIMPORT void upload_ram_bitmap_1d(void* data, unsigned long size, bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT);
  VSX_TEXTURE_DLLIMPORT void upload_ram_bitmap_2d(vsx_bitmap* vbitmap,bool mipmaps = false, bool upside_down = true);
  VSX_TEXTURE_DLLIMPORT void upload_ram_bitmap_2d(void* data, unsigned long size_x, unsigned long size_y,bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT, bool upside_down = true);

  // assumes width is 6x height (maps in order: -x, z, x, -z, -y, y
  VSX_TEXTURE_DLLIMPORT void upload_ram_bitmap_cube(void* data, unsigned long size_x, unsigned long size_y,bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT, bool upside_down = true);

  void* pti_l; // needed by the communication between the png thread and the texture. internal stuff.
  // load a png in the same thread as ours.
  VSX_TEXTURE_DLLIMPORT void load_png(vsx_string fname, bool mipmaps = true, vsxf* filesystem = 0x0);
  VSX_TEXTURE_DLLIMPORT void load_png_thread(vsx_string fname, bool mipmaps = true);
  VSX_TEXTURE_DLLIMPORT void load_jpeg(vsx_string fname, bool mipmaps = true);
  VSX_TEXTURE_DLLIMPORT void load_png_cubemap(vsx_string fname, bool mipmaps = true, vsxf* filesystem = 0x0);

  // update the transform object with a new transformation
  void set_transform(vsx_transform_obj* new_transform_obj) {
    if(transform_obj == new_transform_obj) return;
    if(transform_obj) delete transform_obj;
    transform_obj = new_transform_obj;
    original_transform_obj = 0;
  }
  // return the transformation
  vsx_transform_obj* get_transform()
  {
    return transform_obj;
  }

  inline void begin_transform()
  {
    if ( !transform_obj->is_transform() ) return;
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    (*transform_obj)();
  }

  inline void end_transform()
  {
    if ( !transform_obj->is_transform() ) return;
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
  }

  // use this to bind the texture.
  VSX_TEXTURE_DLLIMPORT bool bind();
  // use this when you're done with the texture
  VSX_TEXTURE_DLLIMPORT void _bind();

  // use this to always set texture coordinates properly.
  VSX_TEXTURE_DLLIMPORT void texcoord2f(float x, float y);

  // constructors

  VSX_TEXTURE_DLLIMPORT vsx_texture();
  VSX_TEXTURE_DLLIMPORT vsx_texture(int id, int type);
  ~vsx_texture()
  {
    if (original_transform_obj)
    delete transform_obj;
  }

  VSX_TEXTURE_DLLIMPORT void unload();
};


#endif
