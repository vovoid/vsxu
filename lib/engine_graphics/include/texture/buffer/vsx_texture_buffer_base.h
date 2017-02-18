#pragma once

#include <texture/vsx_texture.h>

// FBO functions-------------------------------------------------------------
// FBO is used to capture rendering output into a texture rather than to the
// screen for re-use in other rendering operations.

class vsx_texture_buffer_base
{
protected:
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

  int width;
  int height;

  // to prevent double-begins, lock the buffer
  bool capturing_to_buffer;
  bool is_multisample;

public:

  vsx_texture_buffer_base()
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
      viewport_size{0,0,0,0},
      capturing_to_buffer(false),
      is_multisample(false)
  {}


  // query if the hardware has Frame Buffer Object support
  static bool has_buffer_support()
  {
    if (!GLEW_EXT_framebuffer_object)
      glewInit();

    if (!GLEW_EXT_framebuffer_object)
      VSX_ERROR_RETURN_V("GLEW_EXT_framebuffer_object missing", false);

    if (!GLEW_EXT_framebuffer_blit)
      VSX_ERROR_RETURN_V("GLEW_EXT_framebuffer_blit missing", false);

    return true;
  }

  bool is_valid()
  {
    return valid_fbo;
  }

  GLuint get_depth_buffer_handle()
  {
    return depth_buffer_handle;
  }


  virtual void init
  (
    vsx_texture<>* texture,
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture, // use floating point channels (8-bit is default)
    bool alpha, // support alpha channel or not
    bool multisample, // enable MSAA anti aliasing
    bool linear_filter, // linear min/mag texture filter
    GLuint existing_depth_texture_id // depth buffer from other buffer, 0 to ignore
  ) = 0;

  virtual void reinit
  (
    vsx_texture<>* texture,
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture, // use floating point channels (8-bit is default)
    bool alpha, // support alpha channel or not
    bool multisample, // enable MSAA anti aliasing
    bool linear_filter, // linear min/mag texture filter
    GLuint existing_depth_texture_id // depth buffer from other buffer, 0 to ignore
  ) = 0;

  virtual void deinit(vsx_texture<>* texture) = 0;

  // begin capturing render output into the frame buffer object
  virtual void begin_capture_to_buffer() = 0;

  // end the capturing render output into the frame buffer object
  virtual void end_capture_to_buffer() = 0;

};
