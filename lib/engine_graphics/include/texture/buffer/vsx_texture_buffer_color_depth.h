#pragma once

#include "vsx_texture_buffer_base.h"

class vsx_texture_buffer_color_depth
    : public vsx_texture_buffer_base
{
public:

  // See http://www.opengl.org/wiki/Framebuffer_Object_Examples

  void init
  (
    vsx_texture<>* texture,
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture, // use floating point channels (8-bit is default)
    bool alpha, // support alpha channel or not
    bool multisample, // enable MSAA anti aliasing
    bool linear_filter, // linear min/mag texture filter
    GLuint existing_depth_texture_id // depth buffer from other buffer, 0 to ignore
  )
  {
    if ( !has_buffer_support() )
      VSX_ERROR_RETURN("No FBO support");

    int i_width = width;
    int i_height = height;
    is_multisample = multisample;


    if (multisample)
      glEnable(GL_MULTISAMPLE);

    vsx_gl_state::get_instance()->clear_errors();


    // save the previous FBO (stack behaviour)
    int prev_buf_l;
    prev_buf_l = vsx_gl_state::get_instance()->framebuffer_bind_get();

    GLuint texture_storage_type;

    if (float_texture)
      texture_storage_type = alpha?GL_RGBA16F_ARB:GL_RGB16F_ARB;
    else
      texture_storage_type = alpha?GL_RGBA8:GL_RGB8;


    // MIN / MAG filter
    GLint min_mag = linear_filter?GL_LINEAR:GL_NEAREST;


    // Generate Color Texture
    glGenTextures(1, &frame_buffer_fbo_attachment_texture);
    vsx_gl_state::get_instance()->accumulate_errors();

    if (multisample)
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, frame_buffer_fbo_attachment_texture);
    else
      glBindTexture(GL_TEXTURE_2D, frame_buffer_fbo_attachment_texture);
    vsx_gl_state::get_instance()->accumulate_errors();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    vsx_gl_state::get_instance()->accumulate_errors();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    vsx_gl_state::get_instance()->accumulate_errors();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag);
    vsx_gl_state::get_instance()->accumulate_errors();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag);
    vsx_gl_state::get_instance()->accumulate_errors();

    if (multisample)
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, texture_storage_type, i_width, i_height, GL_FALSE);
    else
      //NULL means reserve texture memory, but texels are undefined
      glTexImage2D(GL_TEXTURE_2D, 0, texture_storage_type, i_width, i_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
    vsx_gl_state::get_instance()->accumulate_errors();








    // Generate Depth Texture
    if (existing_depth_texture_id != 0)
    {
      depth_buffer_handle = existing_depth_texture_id;
      depth_buffer_local = false;
    }
    else
    {
      vsx_gl_state::get_instance()->accumulate_errors();

      glGenTextures(1, &depth_buffer_handle);
      vsx_gl_state::get_instance()->accumulate_errors();

      if (multisample)
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_buffer_handle);
      else
        glBindTexture(GL_TEXTURE_2D, depth_buffer_handle);

      vsx_gl_state::get_instance()->accumulate_errors();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      vsx_gl_state::get_instance()->accumulate_errors();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      vsx_gl_state::get_instance()->accumulate_errors();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_mag);
      vsx_gl_state::get_instance()->accumulate_errors();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, min_mag);
      vsx_gl_state::get_instance()->accumulate_errors();

      glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
      vsx_gl_state::get_instance()->accumulate_errors();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
      vsx_gl_state::get_instance()->accumulate_errors();

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
      vsx_gl_state::get_instance()->accumulate_errors();

      if (multisample)
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_DEPTH_COMPONENT24, i_width, i_height, GL_FALSE);
      else
        //NULL means reserve texture memory, but texels are undefined
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, i_width, i_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

      vsx_gl_state::get_instance()->accumulate_errors();

      depth_buffer_local = true;
    }




    // GENERATE FBO
    //-------------------------
    glGenFramebuffersEXT(1, &frame_buffer_handle);
    vsx_gl_state::get_instance()->accumulate_errors();

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_handle);
    vsx_gl_state::get_instance()->accumulate_errors();



    // Attach textures to the FBO


    //Attach 2D texture to this FBO
    if (multisample)
      glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_2D_MULTISAMPLE,
        frame_buffer_fbo_attachment_texture,
        0/*mipmap level*/
      );
    else
      glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_2D,
        frame_buffer_fbo_attachment_texture,
        0/*mipmap level*/
      );
    vsx_gl_state::get_instance()->accumulate_errors();


    //-------------------------
    //Attach depth texture to FBO
    if (multisample)
      glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_DEPTH_ATTACHMENT_EXT,
        GL_TEXTURE_2D_MULTISAMPLE,
        depth_buffer_handle,
        0
      );
    else
      glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_DEPTH_ATTACHMENT_EXT,
        GL_TEXTURE_2D,
        depth_buffer_handle,
        0 //mipmap level
      );

    //-------------------------

    vsx_gl_state::get_instance()->accumulate_errors();


    // Create Render buffer for blitting back to texture
    if (multisample)
    {
      glGenTextures(1, &frame_buffer_blit_color_texture);
      glBindTexture(GL_TEXTURE_2D, frame_buffer_blit_color_texture);

      if (float_texture)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, i_width, i_height, 0, GL_RGBA, GL_FLOAT, NULL);
      else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_width, i_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLfloat)min_mag);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLfloat)min_mag);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

      // create a normal fbo and attach texture to it
      glGenFramebuffersEXT(1, &frame_buffer_blit_handle);
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_blit_handle);

      // attach the texture to the FBO
      glFramebufferTexture2DEXT(
        GL_FRAMEBUFFER_EXT,
        GL_COLOR_ATTACHMENT0_EXT,
        GL_TEXTURE_2D,
        frame_buffer_blit_color_texture,
        0
      );

      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, frame_buffer_handle);
      vsx_gl_state::get_instance()->accumulate_errors();

    }



    //Does the GPU support current FBO configuration?
    GLenum status;
    status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch(status)
    {
       case GL_FRAMEBUFFER_COMPLETE_EXT:
        if (multisample)
          texture->texture->gl_id = frame_buffer_blit_color_texture;
        else
        {
          color_buffer_handle = frame_buffer_fbo_attachment_texture;
          texture->texture->gl_id = frame_buffer_fbo_attachment_texture;
        }


        if (multisample)
          texture->texture->gl_type = GL_TEXTURE_2D_MULTISAMPLE;
        else
          texture->texture->gl_type = GL_TEXTURE_2D;

        texture->texture->uploaded_to_gl = true;
        this->width = width;
        this->height = height;
        valid_fbo = true; // valid for capturing
        break;
      default:
        vsx_printf(L"Error initializing FBO, status = %d\n", status);
      break;
    }
    vsx_gl_state::get_instance()->framebuffer_bind(prev_buf_l);
  }

  void reinit
  (
    vsx_texture<>* texture,
    int width, // width in pixels
    int height, // height in pixels
    bool float_texture, // use floating point channels (8-bit is default)
    bool alpha, // support alpha channel or not
    bool multisample, // enable MSAA anti aliasing
    bool linear_filter, // linear min/mag texture filter
    GLuint existing_depth_texture_id
  )
  {
    deinit(texture);
    init
    (
      texture,
      width,
      height,
      float_texture,
      alpha,
      multisample,
      linear_filter,
      existing_depth_texture_id
    );
  }

  void deinit(vsx_texture<>* texture)
  {
    if (!frame_buffer_handle)
      return;

    //Delete resources
    glDeleteTextures(1, &color_buffer_handle);

    if (depth_buffer_local)
      glDeleteTextures(1, &depth_buffer_handle);

    depth_buffer_handle = 0;
    depth_buffer_local = 0;

    //Bind 0, which means render to back buffer, as a result, fb is unbound
    if ( vsx_gl_state::get_instance()->framebuffer_bind_get() == frame_buffer_handle )
      vsx_gl_state::get_instance()->framebuffer_bind(0);

    glDeleteFramebuffersEXT(1, &frame_buffer_handle);
    frame_buffer_handle = 0;

    if (texture->texture->gl_type == GL_TEXTURE_2D_MULTISAMPLE)
    {
      glDeleteTextures(1, &frame_buffer_blit_color_texture);
      frame_buffer_blit_color_texture = 0;
      glDeleteFramebuffersEXT(1, &frame_buffer_blit_handle);
      frame_buffer_blit_handle = 0;
    }

    valid_fbo = false;
    texture->texture->gl_id = 0;
    texture->texture->gl_type = 0;
    texture->texture->uploaded_to_gl = false;
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

    if ( is_multisample )
    {
      glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, frame_buffer_handle);
      glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, frame_buffer_blit_handle);
      glBlitFramebufferEXT(
        0,
        0,
        width - 1,
        height - 1,
        0,
        0,
        width - 1,
        height - 1,
        GL_COLOR_BUFFER_BIT,
        GL_NEAREST
      );
    }
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
