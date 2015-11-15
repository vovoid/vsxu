#ifndef VSX_TEXTURE_BUFFER_COLOR_H
#define VSX_TEXTURE_BUFFER_COLOR_H

#include "vsx_texture_buffer_base.h"

class vsx_texture_buffer_color
    : public vsx_texture_buffer_base
{
public:
  void init
  (
    vsx_texture* texture,
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    bool multisample = false, // enable MSAA anti aliasing
    GLuint existing_depth_texture_id = 0
  )
  {
    prev_buf = 0;
    int i_width = width;
    int i_height = height;

    if ( !has_buffer_support() )
    {
      //printf("vsx_texture error: No FBO available!\n");
      return;
    }

    // save the previous FBO (stack behaviour)
    int prev_buf_l;
    prev_buf_l = vsx_gl_state::get_instance()->framebuffer_bind_get();
  //  glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, (GLint *)&prev_buf_l);

    GLuint texture_storage_type;

    if (float_texture)
    {
      texture_storage_type = alpha?GL_RGBA16F_ARB:GL_RGB16F_ARB;
    }
    else
    {
      texture_storage_type = alpha?GL_RGBA8:GL_RGB8;
    }

    vsx_gl_state::get_instance()->clear_errors();

    //RGBA8 2D texture, 24 bit depth texture, 256x256
    glGenTextures(1, &frame_buffer_fbo_attachment_texture);
    vsx_gl_state::get_instance()->accumulate_errors();
    glBindTexture(GL_TEXTURE_2D, frame_buffer_fbo_attachment_texture);
    vsx_gl_state::get_instance()->accumulate_errors();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    vsx_gl_state::get_instance()->accumulate_errors();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    vsx_gl_state::get_instance()->accumulate_errors();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    vsx_gl_state::get_instance()->accumulate_errors();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    vsx_gl_state::get_instance()->accumulate_errors();
    //NULL means reserve texture memory, but texels are undefined
    glTexImage2D(GL_TEXTURE_2D, 0, texture_storage_type, i_width, i_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    vsx_gl_state::get_instance()->accumulate_errors();

    //-------------------------
    glGenFramebuffersEXT(1, &frame_buffer_handle);
    vsx_gl_state::get_instance()->accumulate_errors();
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_handle);
    vsx_gl_state::get_instance()->accumulate_errors();
    //Attach 2D texture to this FBO
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, frame_buffer_fbo_attachment_texture, 0/*mipmap level*/);
    vsx_gl_state::get_instance()->accumulate_errors();
    //-------------------------
    //Does the GPU support current FBO configuration?
    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status)
    {
       case GL_FRAMEBUFFER_COMPLETE_EXT:
        texture->texture_gl->gl_id = frame_buffer_fbo_attachment_texture;
        texture->texture_gl->gl_type = GL_TEXTURE_2D;
        texture->texture_gl->uploaded_to_gl = true;
        this->width = width;
        this->height = height;
        valid_fbo = true; // valid for capturing
        break;
      default:
      break;
    }
    vsx_gl_state::get_instance()->framebuffer_bind(prev_buf_l);
  }

  void reinit
  (
    vsx_texture* texture,
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture = false, // use floating point channels (8-bit is default)
    bool alpha = true, // support alpha channel or not
    bool multisample = false, // enable MSAA anti aliasing
    GLuint existing_depth_texture_id = 0
  )
  {
    deinit(texture);
    init
    (
      texture,
      width,
      height,
      float_texture,
      alpha
    );

  }

  void deinit(vsx_texture* texture)
  {
    //Delete resources
    glDeleteTextures(1, &frame_buffer_fbo_attachment_texture);
    depth_buffer_handle = 0;
    depth_buffer_local = 0;
    glDeleteFramebuffersEXT(1, &frame_buffer_handle);

    valid_fbo = false;
    texture->texture_gl->gl_id = 0;
    texture->texture_gl->gl_type = 0;
    texture->texture_gl->uploaded_to_gl = false;
  }

  void begin_capture_to_buffer()
  {
    if (!valid_fbo)
      VSX_ERROR_RETURN("trying to capture to an invalid buffer");

    if (capturing_to_buffer)
      VSX_ERROR_RETURN("trying to capture to a buffer already capturing");

    vsx_gl_state::get_instance()->viewport_get( &viewport_size[0] );

    prev_buf = vsx_gl_state::get_instance()->framebuffer_bind_get();
    glPushAttrib(GL_ALL_ATTRIB_BITS );
    vsx_gl_state::get_instance()->matrix_get_v( VSX_GL_PROJECTION_MATRIX, buffer_save_matrix[0].m );
    vsx_gl_state::get_instance()->matrix_get_v( VSX_GL_MODELVIEW_MATRIX, buffer_save_matrix[1].m );
    vsx_gl_state::get_instance()->matrix_get_v( VSX_GL_TEXTURE_MATRIX, buffer_save_matrix[2].m );


    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_PROJECTION_MATRIX );
    vsx_gl_state::get_instance()->matrix_load_identity();
    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_MODELVIEW_MATRIX );
    vsx_gl_state::get_instance()->matrix_load_identity();
    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_TEXTURE_MATRIX );
    vsx_gl_state::get_instance()->matrix_load_identity();

    buffer_save_blend = vsx_gl_state::get_instance()->blend_get();
    vsx_gl_state::get_instance()->blend_set(1);

    vsx_gl_state::get_instance()->framebuffer_bind(frame_buffer_handle);
    vsx_gl_state::get_instance()->viewport_set(0, 0, width, height);

    capturing_to_buffer = true;
  }


  void end_capture_to_buffer()
  {
    if (!valid_fbo)
      VSX_ERROR_RETURN("trying to capture to an invalid buffer");

    if (!capturing_to_buffer)
      VSX_ERROR_RETURN("trying to stop capturing to a buffer never capturing");

    vsx_gl_state::get_instance()->framebuffer_bind(prev_buf);
    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_PROJECTION_MATRIX );
    vsx_gl_state::get_instance()->matrix_load_identity();
    vsx_gl_state::get_instance()->matrix_mult_f( buffer_save_matrix[0].m );
    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_MODELVIEW_MATRIX );
    vsx_gl_state::get_instance()->matrix_load_identity();
    vsx_gl_state::get_instance()->matrix_mult_f( buffer_save_matrix[1].m );
    vsx_gl_state::get_instance()->matrix_mode( VSX_GL_TEXTURE_MATRIX );
    vsx_gl_state::get_instance()->matrix_load_identity();
    vsx_gl_state::get_instance()->matrix_mult_f( buffer_save_matrix[2].m );
    vsx_gl_state::get_instance()->blend_set(buffer_save_blend);

    glPopAttrib();

    vsx_gl_state::get_instance()->viewport_set( &viewport_size[0] );

    capturing_to_buffer = false;
  }


};

#endif
