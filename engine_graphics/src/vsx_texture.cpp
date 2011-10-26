/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "vsx_string.h"

#ifdef VSXU_EXE
  #include <map>
  #include "vsxfst.h"
#endif
#define VSX_NOGLUT_LOCAL
#include "vsx_gl_global.h"
#undef VSX_NOGLUT_LOCAL
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#ifndef VSX_TEXTURE_NO_GLPNG
#include "vsxg.h"
#include <stdlib.h>
#ifdef _WIN32
	#include "pthread.h"
#else
	#include "pthread.h"
#endif
#endif
#ifdef VSXU_MAC_XCODE
#include <syslog.h>
#endif

#ifdef VSXU_EXE
std::map<vsx_string, vsx_texture_info> vsx_texture::t_glist;
#else
void* vsx_texture::t_glist;
#endif


vsx_texture::vsx_texture(int id, int type) {
  pti_l = 0;
  rt = 0;
  texture_info.ogl_id = id;
  texture_info.ogl_type = type;
  transform_obj = new vsx_transform_neutral;
  valid = true;
  locked = false;
  #ifndef VSXU_OPENGL_ES
  glewInit();
  #endif
}

void vsx_texture::init_opengl_texture() {
  GLuint tex_id;
  glGenTextures(1, &tex_id);
	//printf("init_opengl_texture: GL Error was: %x\n", glGetError());
  texture_info.ogl_id = tex_id;
  texture_info.ogl_type = GL_TEXTURE_2D;
}

void vsx_texture::init_buffer(int width, int height, bool float_texture) {
#ifndef VSX_TEXTURE_NO_RT
  locked = false;
  prev_buf = 0;
#ifndef VSXU_OPENGL_ES
  glewInit();
#endif
  int i_width = width;
  int i_height = height;
  //printf("GL vendor string: %s---\n",glGetString(GL_VENDOR));

#ifdef VSXU_OPENGL_ES
	use_fbo = true;
#endif
#ifndef VSXU_OPENGL_ES
  use_fbo = GLEW_EXT_framebuffer_object;
#endif
  //if (vsx_string((char*)glGetString(GL_VENDOR)) == vsx_string("ATI Technologies Inc."))
  //{
	  // use_fbo = false;
  //}
  if (use_fbo) {
#ifdef VSXU_OPENGL_ES_1_0
    GLint prev_buf_l;
    GLuint tex_id;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, (GLint *)&prev_buf_l);

    glGenFramebuffersOES(1, &framebuffer_id);
			printf("%d GL Error was: %x\n", __LINE__,glGetError());
		glGenTextures(1, &tex_id);
			printf("%d GL Error was: %x\n", __LINE__,glGetError());
//		glGenRenderbuffersOES(1, &depthbuffer_id);
//		glGenRenderbuffersOES(1, &stencil_rb);
  //  * Bind the framebuffer object:
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer_id);
			printf("%d GL Error was: %x\n", __LINE__,glGetError());
//    * Setup the color buffer for use as a renderable texture:
		glBindTexture(GL_TEXTURE_2D, tex_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_width, i_height, 0,
								 GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			printf("%d GL Error was: %x\n", __LINE__,glGetError());
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			printf("%d GL Error was: %x\n", __LINE__,glGetError());
		glFramebufferTexture2DOES(GL_FRAMEBUFFER_OES,
															GL_COLOR_ATTACHMENT0_OES,
															GL_TEXTURE_2D, tex_id, 0);
			printf("%d GL Error was: %x\n", __LINE__,glGetError());
//    * Initialize the depth buffer:
	/*	glBindRenderbufferOES(GL_RENDERBUFFER_OES, depthbuffer_id);
		glRenderbufferStorageOES(GL_RENDERBUFFER_OES,
														 GL_DEPTH_COMPONENT16_OES, i_width, i_height);
		glFramebufferRenderbufferOES(GL_FRAMEBUFFER_OES,
																 GL_DEPTH_ATTACHMENT_OES,
																 GL_RENDERBUFFER_OES, depthbuffer_id);*/
	  texture_info.ogl_id = tex_id;
	  texture_info.ogl_type = GL_TEXTURE_2D;
	  texture_info.size_x = width;
	  texture_info.size_y = height;

		//////
		glBindFramebufferOES(GL_FRAMEBUFFER_OES, prev_buf_l);
#endif
#ifndef VSXU_OPENGL_ES

    GLint prev_buf_l;
    GLuint tex_id;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, (GLint *)&prev_buf_l);


    // multi sampled color buffer
    glGenRenderbuffersEXT(1, &colorBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, colorBuffer);

    if (float_texture)
    glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, 4, GL_RGBA16F_ARB, width, height);
    else
    glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, 4, GL_RGBA8, width, height);

    // multi sampled depth buffer
    glGenRenderbuffersEXT(1, &depthBuffer);
    glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthBuffer);
    glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, 4, GL_DEPTH_COMPONENT, width, height);

    // create fbo for multi sampled content and attach depth and color buffers to it
    glGenFramebuffersEXT(1, &framebuffer_id);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer_id);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_RENDERBUFFER_EXT, colorBuffer);
    glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthBuffer);

    // create texture
    glGenTextures(1, &tex_id);
    glBindTexture(GL_TEXTURE_2D, tex_id);
    if (float_texture)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, i_width, i_height, 0, GL_RGBA, GL_FLOAT, NULL);
    else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_width, i_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set your texture parameters here if required ...

    // create final fbo and attach texture to it
    glGenFramebuffersEXT(1, &tex_fbo);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, tex_fbo);
    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex_id, 0);



    /*glGenFramebuffersEXT(1, &framebuffer_id);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer_id);
    glGenTextures(1, &tex_id);
    // Setup texture
		glBindTexture(GL_TEXTURE_2D, tex_id);
		if (float_texture)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, i_width, i_height, 0, GL_RGBA, GL_FLOAT, NULL);
		else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, i_width, i_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
*/
/*	  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL,0);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	  //glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	  //float rMaxAniso;
	  //glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &rMaxAniso);
	  //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, rMaxAniso);

	  /*glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, tex_id, 0);

	  glGenRenderbuffersEXT(1, &depthbuffer_id);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, depthbuffer_id);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, i_width, i_height);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, depthbuffer_id);
    */
	  texture_info.ogl_id = tex_id;
	  texture_info.ogl_type = GL_TEXTURE_2D;
	  texture_info.size_x = width;
	  texture_info.size_y = height;
	  glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prev_buf_l);
#endif
	} else {

#if !defined(VSX_TEXTURE_NO_RT_PBUFFER)
	  rt = new RenderTexture("rgba=8 depth tex2D depthTex2D ctt");
	  rt->Initialize(i_width, i_height);
	  texture_info.ogl_id = rt->GetTextureID();
	  texture_info.ogl_type = rt->GetTextureTarget();
	  //rt->Initialize(true, true, false, false, false,texture_wrap, 8, 8, 8, 8,RenderTexture::RT_COPY_TO_TEXTURE);
#endif
  }
  valid = true;
#endif
}

void vsx_texture::init_buffer_render(int width, int height) {
#ifndef VSX_TEXTURE_NO_RT
#if !defined(VSX_TEXTURE_NO_RT_PBUFFER)
  glewInit();
  rt = new RenderTexture("rgba tex2D rtt");
	rt->Initialize(width, height);
	texture_info.ogl_id = rt->GetTextureID();
	texture_info.ogl_type = rt->GetTextureTarget();
	/*
	rt = new RenderTexture(width, height);
  rt->Initialize(true, false, false, false, false, 8, 8, 8, 8,RenderTexture::RT_RENDER_TO_TEXTURE);
  */
  valid = true;
#endif
#endif
}

void vsx_texture::deinit_buffer() {
#ifndef VSX_TEXTURE_NO_RT
  if (use_fbo) {
#ifdef VSXU_OPENGL_ES_1_0
  	glDeleteRenderbuffersOES(1,&depthbuffer_id);
  	glDeleteTextures(1,&texture_info.ogl_id);
    glDeleteFramebuffersOES(1, &framebuffer_id);
#endif

#ifndef VSXU_OPENGL_ES
		//printf("removing FBO\n");
  	glDeleteRenderbuffersEXT(1,&depthbuffer_id);
  	glDeleteTextures(1,&texture_info.ogl_id);
    glDeleteFramebuffersEXT(1, &framebuffer_id);
#endif
  }
#if !defined(VSX_TEXTURE_NO_RT_PBUFFER)
  else
  if (rt) {
    delete (RenderTexture*)rt;
    rt = 0;
    valid = false;
  }
#endif
#endif
}

void vsx_texture::reinit_buffer(int width, int height, bool float_texture) {
#ifndef VSX_TEXTURE_NO_RT
  //if (rt) {
//      delete (RenderTexture*)rt;
//    }
  deinit_buffer();
  init_buffer(width,height,float_texture);
#endif
}

void vsx_texture::reinit_buffer_render(int width, int height) {
#ifndef VSX_TEXTURE_NO_RT
#if !defined(VSX_TEXTURE_NO_RT_PBUFFER)
  if (rt) {
    delete rt;
    rt = new RenderTexture();
    rt->Initialize(true, true, false, false, false, 8, 8, 8, 8,RenderTexture::RT_RENDER_TO_TEXTURE);
    valid = true;
    //rt->Reset(width, height);
  }  else {
    init_buffer_render(width,height);
  }
#endif
#endif
}

void vsx_texture::begin_capture() {
#ifndef VSX_TEXTURE_NO_RT
  if (use_fbo) {
    if (locked) printf("locked\n");
    if (locked) return;
#ifdef VSXU_OPENGL_ES_1_0
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_OES, (GLint *)&prev_buf);
#endif
#ifndef VSXU_OPENGL_ES
    glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, (GLint *)&prev_buf);
    glPushAttrib(GL_ALL_ATTRIB_BITS );
#endif
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    GLfloat one_array[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat zero_array[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    #ifndef VSXU_OPENGL_ES_2_0
    glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,&one_array[0]);
    glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,&one_array[0]);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,&zero_array[0]);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,&zero_array[0]);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,&one_array[0]);

    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHT1);
    glDisable(GL_LIGHT2);
    glDisable(GL_LIGHT3);
    #endif
    glEnable(GL_BLEND);
#ifdef VSXU_OPENGL_ES_1_0
    glBindTexture(GL_TEXTURE_2D,0);
//			printf("framebuffer_id: %d\n");
    glBindFramebufferOES(GL_FRAMEBUFFER_OES, framebuffer_id);
#endif
#ifndef VSXU_OPENGL_ES
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, framebuffer_id);
#endif
    locked = true;

    glViewport(0,0,(int)texture_info.size_x, (int)texture_info.size_y);

    #ifdef __APPLE__
    #endif
  }
#if !defined(VSX_TEXTURE_NO_RT_PBUFFER)
  else
  if (rt)
  rt->BeginCapture();
#endif
#endif
}

void vsx_texture::end_capture() {
#ifndef VSX_TEXTURE_NO_RT
  if (use_fbo) {
    if (locked) {
      #ifndef VSXU_OPENGL_ES_2_0
        glBindFramebufferEXT(GL_READ_FRAMEBUFFER_EXT, framebuffer_id);
        glBindFramebufferEXT(GL_DRAW_FRAMEBUFFER_EXT, tex_fbo);
        glBlitFramebufferEXT(0, 0, texture_info.size_x, texture_info.size_x, 0, 0, texture_info.size_x, texture_info.size_x, GL_COLOR_BUFFER_BIT, GL_NEAREST);
      #endif


#ifdef VSXU_OPENGL_ES_1_0
      glBindFramebufferOES(GL_FRAMEBUFFER_OES, prev_buf);
#endif
#ifndef VSXU_OPENGL_ES
      glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, prev_buf);
#endif
      glMatrixMode(GL_MODELVIEW);
      glPopMatrix();
      glMatrixMode(GL_TEXTURE);
      glPopMatrix();
      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
#ifndef VSXU_OPENGL_ES_2_0
      glPopAttrib();
#endif
      locked = false;
    }

  }
#if !defined(VSX_TEXTURE_NO_RT_PBUFFER)
  else
  if (rt)
  rt->EndCapture();
#endif
#endif
}

void vsx_texture::unload_all_active() {
#ifdef VSXU_EXE
  for (std::map<vsx_string, vsx_texture_info>::iterator it = t_glist.begin(); it != t_glist.end(); ++it) {
    glDeleteTextures(1,&((*it).second.ogl_id));
  }
#endif
}

void vsx_texture::reinit_all_active() {
#ifdef VSXU_EXE
  //printf("reiniting all active textures\n");
  std::map<vsx_string, vsx_texture_info> temp_glist = t_glist;
  vsx_string tname;
  for (std::map<vsx_string, vsx_texture_info>::iterator it = temp_glist.begin(); it != temp_glist.end(); ++it) {
//    if ((*it).second.type == 0) {
      //tname = (*it).first;
      //printf("attempting to reload tga %s\n",tname.c_str());
      //t_glist.erase(tname);
      //load_tga(tname);
//    } else
    if ((*it).second.type == 1) {
      tname = (*it).first;
    //  printf("attempting to reload png %s\n",tname.c_str());
      t_glist.erase(tname);
      load_png(tname);
    }
  }
#endif
  //printf("END reiniting all active textures\n");
}


void vsx_texture::upload_ram_bitmap(vsx_bitmap* vbitmap,bool mipmaps, bool upside_down) {
#ifndef VSX_TEXTURE_NO_R_UPLOAD
  //printf("uploading ram bitmap\n");
	upload_ram_bitmap(vbitmap->data, vbitmap->size_x, vbitmap->size_y,mipmaps,vbitmap->bpp, vbitmap->bformat,upside_down);
#endif
}

void vsx_texture::upload_ram_bitmap(void* data, unsigned long size_x, unsigned long size_y, bool mipmaps, int bpp, int bpp2, bool upside_down) {
#ifndef VSX_TEXTURE_NO_R_UPLOAD
	if (!mipmaps) {
	  if ((float)size_x/(float)size_y != 1.0) {
			#ifdef VSXU_OPENGL_ES
			texture_info.ogl_type = GL_TEXTURE_2D;
			#endif
			#ifndef VSXU_OPENGL_ES  // TODO: add support for irregularly shaped textures
				// irregularly shaped texture
				glewInit();
				if (GLEW_ARB_texture_rectangle) {
          #if defined(VSXU_DEBUG)
					printf("GL_TEXTURE_RECTANGLE_EXT 1\n");
          #endif
					texture_info.ogl_type = GL_TEXTURE_RECTANGLE_ARB;
					mipmaps = false;
				} else
				{
          #if defined(VSXU_DEBUG)
          printf("GL_TEXTURE_MIPMAP FALLBACK 1\n");
          #endif
					texture_info.ogl_type = GL_TEXTURE_2D;
					mipmaps = true;
				}
			#endif
	  } else
	  {
      #if defined(VSXU_DEBUG)
      printf("no mipmaps, GL_TEXTURE_2D\n");
      #endif
			texture_info.ogl_type = GL_TEXTURE_2D;
		}
	} else {
    #if defined(VSXU_DEBUG)
    printf("mipmaps, GL_TEXTURE_2D\n");
    #endif
	  texture_info.ogl_type = GL_TEXTURE_2D;
	//printf("GL_TEXTURE_2D 2\n");
	}
	GLboolean oldStatus = glIsEnabled(texture_info.ogl_type);
	//printf("%d GL Error was: %x\n", __LINE__,glGetError());
	glEnable(texture_info.ogl_type);
	//printf("%d GL Error was: %x\n", __LINE__,glGetError());
	glBindTexture(texture_info.ogl_type, texture_info.ogl_id);
	//printf("Texture id is %d\n",texture_info.ogl_id);
	//printf("%d GL Error was: %x\n", __LINE__,glGetError());
	glTexParameteri(texture_info.ogl_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//printf("%d GL Error was: %x\n", __LINE__,glGetError());
	glTexParameteri(texture_info.ogl_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//printf("%d GL Error was: %x\n", __LINE__,glGetError());

  if (upside_down) {
    if (bpp == GL_RGBA32F_ARB)
    {
      GLfloat* data2 = new GLfloat[size_x * size_y * 4];
      int dy = 0;
      int sxbpp = size_x*4;
      for (int y = size_y-1; y >= 0; --y) {
        for (unsigned long x = 0; x < size_x*4; ++x) {
          data2[dy*sxbpp + x] = ((GLfloat*)data)[y*sxbpp + x];
        }
        ++dy;
      }
      data = (GLfloat*)data2;
    } else
    {
      unsigned char* data2 = new unsigned char[size_x * size_y * bpp];
      int dy = 0;
      int sxbpp = size_x*bpp;
      for (int y = size_y-1; y >= 0; --y) {
        for (unsigned long x = 0; x < size_x*bpp; ++x) {
          data2[dy*sxbpp + x] = ((unsigned char*)data)[y*sxbpp + x];
        }
        ++dy;
      }
      data = (unsigned long*)data2;
    }
  }
  //glTexParameteri(texture_info.ogl_type, GL_TEXTURE_WRAP_S, GL_CLAMP);
  //glTexParameteri(texture_info.ogl_type, GL_TEXTURE_WRAP_T, GL_CLAMP);

  //glTexParameteri(texture_info.ogl_type, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  //glTexParameteri(texture_info.ogl_type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  //printf("before upload %d\n",bpp);
  if (mipmaps)
  {
    //printf("texture: trying to use mipmaps\n");
#ifdef VSXU_OPENGL_ES
		printf("texture: running glTExImage2D\n");
		glTexImage2D(texture_info.ogl_type, 0, GL_RGBA, size_x, size_y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		printf("%d GL Error was: %x\n", __LINE__,glGetError());
		printf("GL NOError is: %d\n", GL_NO_ERROR);
#endif
#ifndef VSXU_OPENGL_ES
    if (bpp == GL_RGBA32F_ARB)
    gluBuild2DMipmaps(texture_info.ogl_type,bpp,size_x,size_y,bpp2,GL_FLOAT,data);
    else
    gluBuild2DMipmaps(texture_info.ogl_type,bpp,size_x,size_y,bpp2,GL_UNSIGNED_BYTE,data);
#endif
  }
  else
  {
    //printf("NO mipmaps");
#ifndef VSXU_OPENGL_ES
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAX_LEVEL,0);
#endif
    
    // no compression
    if (bpp == GL_RGBA32F_ARB)
    {
      glTexImage2D(texture_info.ogl_type, 0,bpp , size_x, size_y, 0, bpp2, GL_FLOAT, data);
    } else
    {
      glTexImage2D(texture_info.ogl_type, 0,bpp , size_x, size_y, 0, bpp2, GL_UNSIGNED_BYTE, data);
    }

    // use compression
    /*if (bpp == 3)
    glTexImage2D(texture_info.ogl_type, 0,GL_COMPRESSED_RGB_S3TC_DXT1_EXT , size_x, size_y, 0, bpp2, GL_UNSIGNED_BYTE, data);
    else
    glTexImage2D(texture_info.ogl_type, 0,GL_COMPRESSED_RGBA_S3TC_DXT5_EXT , size_x, size_y, 0, bpp2, GL_UNSIGNED_BYTE, data);
    // end compression block
    */
  }
  if (upside_down) {
    if (bpp == GL_RGBA32F_ARB)
    {
  	delete[] (GLfloat*)data;
    } else
    {
      delete[] (unsigned long*)data;
    }
  }
  //printf("after upload\n");

  this->texture_info.size_x = size_x;
  this->texture_info.size_y = size_y;
  if(!oldStatus) glDisable(texture_info.ogl_type);
  valid = true;
#endif
}

  void vsx_texture::load_png(vsx_string fname, bool mipmaps) {
#ifndef VSX_TEXTURE_NO_GLPNG
#ifdef VSXU_EXE
    if (t_glist.find(fname) != t_glist.end()) {
      //printf("already found png: %s\n",fname.c_str());
      locked = true;
      texture_info = t_glist[fname];
      return;
    } else
#endif
    {
      locked = false;
      //printf("processing png: %s\n",fname.c_str());
      vsxf filesystem;
      pngRawInfo* pp = new pngRawInfo;
      if (pngLoadRaw(fname.c_str(), pp, &filesystem))
      {
        this->name = fname;
        init_opengl_texture();
        if (pp->Components == 1)
        upload_ram_bitmap((unsigned long*)(pp->Data),pp->Width,pp->Height,mipmaps,3,GL_RGB);
        if (pp->Components == 2)
        upload_ram_bitmap((unsigned long*)(pp->Data),pp->Width,pp->Height,mipmaps,4,GL_RGBA);
        if (pp->Components == 3)
        upload_ram_bitmap((unsigned long*)(pp->Data),pp->Width,pp->Height,mipmaps,pp->Components,GL_RGB);
        if (pp->Components == 4)
        upload_ram_bitmap((unsigned long*)(pp->Data),pp->Width,pp->Height,mipmaps,pp->Components,GL_RGBA);
        free(pp->Data);
        texture_info.type = 1; // png
        //printf("name: %s\n",fname.c_str());
#ifdef VSXU_EXE
        t_glist[fname] = texture_info;
#endif
      }
      delete pp;
    }
#endif
  }

  void vsx_texture::load_jpeg(vsx_string fname, bool mipmaps) {
    #ifdef VSXU_EXE
    #ifndef VSX_TEXTURE_NO_JPEG
      CJPEGTest cj;
      vsx_string ret;
      vsxf filesystem;
      cj.LoadJPEG(fname,ret,&filesystem);
      upload_ram_bitmap((unsigned long*)cj.m_pBuf, cj.GetResX(), cj.GetResY(), mipmaps, 3, GL_RGB);
    #endif
    #endif
  }

#ifndef VSX_TEXTURE_NO_GLPNG
  // PNG THREAD STUFF
  typedef struct {
    pngRawInfo*       pp;
    int               thread_state;
    pthread_t					worker_t;
    pthread_attr_t		worker_t_attr;
    vsx_bitmap        bitmap;
    vsx_string       filename;
    bool              mipmaps;
  } pti; // png thread info

  // thread run by the load_png_thread
  void* png_worker(void *ptr) {

    ((pti*)((vsx_texture*)ptr)->pti_l)->pp = new pngRawInfo;
    vsxf filesystem;
    if (pngLoadRaw(((pti*)((vsx_texture*)ptr)->pti_l)->filename.c_str(), ((pti*)((vsx_texture*)ptr)->pti_l)->pp,&filesystem))
    {
      ((pti*)((vsx_texture*)ptr)->pti_l)->thread_state = 2;
    }
    return 0;
  }
#endif
  // load a png but put the heavy processing in a thread instead. RECOMMENDED!
  void vsx_texture::load_png_thread(vsx_string fname, bool mipmaps) {
#ifndef VSX_TEXTURE_NO_GLPNG
#ifndef VSX_TEXTURE_NO_GLPNG_THREAD
    if (t_glist.find(fname) != t_glist.end()) {
      locked = true;
      texture_info = t_glist[fname];
      this->name = fname;
      return;
    } else
    {
      locked = false;
      if (pti_l) {
        if (((pti*)pti_l)->thread_state == 1) {
          long* aa;
          pthread_join(((pti*)pti_l)->worker_t, (void **)&aa);
        }
        free(((pti*)pti_l)->pp->Data);
        free(pti_l);
      }
      this->name = fname;
      valid = false;
      pti* pt = new pti;
      pt->filename = fname;
      pt->mipmaps = mipmaps;
      pthread_attr_init(&pt->worker_t_attr);
      pt->thread_state = 1;
      pti_l = (void*)pt;
      pthread_create(&(pt->worker_t), &(pt->worker_t_attr), &png_worker, (void*)this);
    }
#endif
#endif
  }


//void vsx_texture::load_tga(vsx_string fname, bool mipmaps) {
/*  if (t_glist.find(name) != t_glist.end()) {
    locked = true;
    texture_info = t_glist[name];
    return;
  } else
  {
    //printf("::loading tga %s\n",name.c_str());
    locked = false;
    std::ifstream fis;
    fis.open(name.c_str(), std::ios_base::binary | std::ios_base::in);
    //	Sleep(100);
    //	bool test = fis.is_open();
    	char dummy[100];
    	fis.read(dummy, 12);
    	this->name = name;

    	signed short width, height, bits;
    	bits = 24;
    	fis.read((char*)&width, 2);
    	fis.read((char*)&height, 2);
    	fis.read((char*)&bits, 1);
    	fis.read(dummy,1);

    	char* data = new char[width * height * 4];

    	if(bits == 24) {
    		for(int y = height - 1; y >= 0; y--)
    			for(int x = 0; x < width; ++x) {
    				fis.read(data + y * width * 4 + x * 4, 3);
    				data[y * width * 4 + x * 4 + 3] = (char)0xFF;
    			}
    	}

    	if(bits == 32) {
//      	std::cout << "tga is 32bit " << name << std::endl;
    		for(int y = height - 1; y >= 0; y--)
    			fis.read(data + y * width * 4, width * 4);
    	}

    	fis.close();

//    	std::cout << "tga bits: " << bits << std::endl;
//    	std::cout << "width of tga: " << width << std::endl;
//    	std::cout << "height of tga: " << height << std::endl;

      GLuint tex_id;

  		glGenTextures(1, &tex_id);
//  		printf("GL texture ID: %d\n",tex_id);
  		GLboolean oldStatus = glIsEnabled(GL_TEXTURE_2D);
  		texture_info.ogl_id = tex_id;
  		texture_info.ogl_type = GL_TEXTURE_2D;
  //		vsx_texture_info tex_info;
//
//  		tex
//     = new vsx_texture_info(tex_id, GL_TEXTURE_2D);
      texture_info.type = 0; // tga
    	t_glist[name] = texture_info;
//    	texture_info = tex_info;

  		glEnable(GL_TEXTURE_2D);
  		glBindTexture(GL_TEXTURE_2D, tex_id);

      if (mipmaps) {
//    		std::cout << "using mipmaps" << std::endl;
    		gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA8,width,height,GL_BGRA_EXT,GL_UNSIGNED_BYTE,data);
    		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  		}
      else {
//  			std::cout << "not using mipmaps" << std::endl;
  //			gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA8,width,height,GL_BGRA_EXT,GL_UNSIGNED_BYTE,data);
    		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, data);
    //		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      }

//    	std::cout << "tex-id " << tex_id << std::endl;
  		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  		//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    	if(!oldStatus) glDisable(GL_TEXTURE_2D);
//  		glDisable(texture_info->get_type());
  		free(data);
  		valid = true;
		}*/
  //	return (unsigned long*)data;
//  }

  bool vsx_texture::bind() {
#ifndef VSX_TEXTURE_NO_RT
#if !defined(VSX_TEXTURE_NO_RT_PBUFFER)
    if (rt) {
      printf("binding rt\n");
      rt->Bind();
      rt->EnableTextureTarget();
    } else
#endif
#endif
    {
      #ifndef VSX_TEXTURE_NO_GLPNG
			#ifndef VSX_TEXTURE_NO_GLPNG_THREAD
        if (pti_l)
        if (((pti*)pti_l)->thread_state == 2) {
          if (texture_info.ogl_id != 0)
          unload();
          init_opengl_texture();
          pngRawInfo* pp = (pngRawInfo*)(((pti*)pti_l)->pp);
          if (pp->Components == 1)
          upload_ram_bitmap((unsigned long*)(pp->Data),pp->Width,pp->Height,false,3,GL_RGB); else
          if (pp->Components == 2)
          upload_ram_bitmap((unsigned long*)(pp->Data),pp->Width,pp->Height,false,4,GL_RGBA); else
          if (pp->Components == 3)
          upload_ram_bitmap((unsigned long*)(pp->Data),pp->Width,pp->Height,false,pp->Components,GL_RGB); else
          if (pp->Components == 4)
          upload_ram_bitmap((unsigned long*)(pp->Data),pp->Width,pp->Height,false,pp->Components,GL_RGBA);
          free((((pti*)pti_l)->pp)->Data);
          texture_info.type = 1; // png
          t_glist[name] = texture_info;
          //pthread_join(((pti*)pti_l)->worker_t,0);
          valid = true;
          delete (pti*)pti_l;
          pti_l = 0;
        }
      #endif
      #endif
      if (texture_info.ogl_id == 0) {
        return false;
      }
    	glEnable(texture_info.ogl_type);
    	glBindTexture(texture_info.ogl_type,texture_info.ogl_id);
      //printf("binding texture\n");
    	return true;
   	}
		return false;
  }
  void vsx_texture::_bind() {
#if !defined(VSX_TEXTURE_NO_RT_PBUFFER)
#ifndef VSX_TEXTURE_NO_RT
    if (rt) {
      rt->DisableTextureTarget();
    } else
#endif
#endif
    {
      if (texture_info.ogl_id == 0) return;
  		glDisable(texture_info.ogl_type);
 		}
  }

  void vsx_texture::texcoord2f(float x, float y) {
#ifdef VSXU_OPENGL_ES
		printf("NO vsx_texture::texcoord2f support on OpenGL ES!\n");
#else
    if (texture_info.ogl_type == GL_TEXTURE_RECTANGLE_EXT) {
      glTexCoord2i((GLuint)(x*texture_info.size_x),(GLuint)(y*texture_info.size_y));
      //printf("b %d",(GLuint)(x*texture_info.size_x));
    } else {
      //printf("a");
      glTexCoord2f(x,y);
    }
#endif
  }

  void vsx_texture::unload() {
#if defined(VSXU_EXE) && !defined(VSXU_PLAYER)
  //printf("deleting GL texture 0\n");
    if (texture_info.ogl_id != 0)	{
      if (name != "" && t_glist.find(name) != t_glist.end())
      {
        if (!locked) {
          t_glist.erase(name);
          //printf("deleting GL texture\n");
          glDeleteTextures(1,&(texture_info.ogl_id));
        }
      } else
      if (locked) {
        glDeleteTextures(1,&(texture_info.ogl_id));
        //GLenum ii = glGetError();
        //printf("deletion error was: %d\n",ii);
      }
      texture_info.ogl_id = 0;
      texture_info.ogl_type = 0;
      valid = false;
    }
#else
    if (texture_info.ogl_id != 0)	{
      glDeleteTextures(1,&(texture_info.ogl_id));
      texture_info.ogl_id = 0;
      texture_info.ogl_type = 0;
      valid = false;
    }
#endif
  }


// OLD PNG CODE.
      /*GLuint tex_id;
    	pngInfo info;
  		glGenTextures(1, &tex_id);
  		GLboolean oldStatus = glIsEnabled(GL_TEXTURE_2D);
  		vsx_texture_info* tex_info = new vsx_texture_info(tex_id, GL_TEXTURE_2D);
    	t_glist[name] = *tex_info;
    	texture_info = *tex_info;
     	this->name = name;

  		glEnable(GL_TEXTURE_2D);
  		glBindTexture(GL_TEXTURE_2D, tex_id);
      if (!pngLoad(name.c_str(), PNG_NOMIPMAP, PNG_ALPHA, &info)) {
  			printf("Can't load file");
  			return;
  		}
      size_x = info.Width;
      size_y = info.Height;
      if (mipmaps) {
//    		std::cout << "using mipmaps" << std::endl;
    		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  		}
      else {
//  			std::cout << "not using mipmaps" << std::endl;
    		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

      }
    	if(!oldStatus) glDisable(GL_TEXTURE_2D);
      valid = true;
      */

      /*
      png_structp png_ptr;
      png_infop info_ptr;
      unsigned int sig_read = 0;
      png_uint_32 width, height;
      int bit_depth, color_type, interlace_type;
      FILE *fp;

      if ((fp = fopen(name.c_str(), "rb")) == NULL)
        return;
      png_voidp user_error_ptr;
      png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
        0, 0, 0);

      if (png_ptr == NULL)
      {
        fclose(fp);
        return;
      }
      info_ptr = png_create_info_struct(png_ptr);
      if (info_ptr == NULL)
      {
        fclose(fp);
        png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
        return ;
      }
      if (setjmp(png_jmpbuf(png_ptr)))
      {
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);
        return ;
      }
      png_init_io(png_ptr, fp);
      png_read_png(png_ptr, info_ptr, 0, png_voidp_NULL);
      png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
      fclose(fp);
      */

