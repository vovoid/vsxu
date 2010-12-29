#ifndef VSX_TEXTURE_LIB_H
#define VSX_TEXTURE_LIB_H
#ifdef VSXU_EXE
  #include <map>
#endif
#include "vsx_texture_info.h"
#include "vsx_string.h"
#ifndef VSX_TEXTURE_NO_RT
  #ifndef VSX_TEXTURE_NO_RT_PBUFFER
    #ifndef VSXU_OPENGL_ES
      #include "render_texture.h"
    #endif
  #endif
#endif
#include "vsx_bitmap.h"


#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#define VSX_TEXTURE_DLLIMPORT
#else
  #if defined(VSX_ENG_DLL)
    #define VSX_TEXTURE_DLLIMPORT __declspec (dllexport)
  #else 
    #define VSX_TEXTURE_DLLIMPORT __declspec (dllimport)
  #endif
#endif


class vsx_texture {
#ifdef VSXU_EXE
  static std::map<vsx_string, vsx_texture_info> t_glist;
#else
  static void* t_glist;
#endif
  GLint prev_buf;
  bool use_fbo;
  GLuint framebuffer_id;
  GLuint depthbuffer_id;
  GLuint colorBuffer, depthBuffer, tex_fbo;

public:
  bool locked; // this is if another texture gets a texture already in the list, to prevent it from unloading.
                // if not locked it can safely delete it. This is an approximation of course, but should work
                // in most cases.
  // name of the texture
  vsx_string name;
  // is this valid for binding:
  bool valid;

#if !defined(VSX_TEXTURE_NO_RT) && !defined(VSX_TEXTURE_NO_RT_PBUFFER) && !defined(VSXU_OPENGL_ES)
  RenderTexture *rt; // render texture (used when acting as a pbuffer)
#else
  void *rt;
#endif
	vsx_transform_obj* transform_obj; // transformation

  // our texture info
  vsx_texture_info texture_info;

  // pbuffer related functions
  VSX_TEXTURE_DLLIMPORT void init_buffer(int width, int height, bool float_texture = false); // run once
  VSX_TEXTURE_DLLIMPORT void init_buffer_render(int width, int height); // run once, needs support in hardware
  VSX_TEXTURE_DLLIMPORT void deinit_buffer(); // remove the buffer
  VSX_TEXTURE_DLLIMPORT void reinit_buffer(int width, int height, bool float_texture = false); // run in stop/start or when changing resolution
  VSX_TEXTURE_DLLIMPORT void reinit_buffer_render(int width, int height); // run in stop/start or when changing resolution

  bool get_fbo_status() { return use_fbo;}

#if ((!defined(VSX_TEXTURE_NO_RT)) && (!defined(VSX_TEXTURE_NO_RT_PBUFFER))) && (!defined(VSXU_OPENGL_ES))
  RenderTexture* get_rt() { return rt; }
#else
  void* get_rt() { return rt; }
#endif

  // allocate an openGL texture ID
  VSX_TEXTURE_DLLIMPORT void init_opengl_texture();

  // reuploads all textures in the t_glist so you don't have to bother :)
  // just use thie in the start function of the module and all should be ok unless you use
  // the buffer. then see above.
  VSX_TEXTURE_DLLIMPORT void reinit_all_active();
  VSX_TEXTURE_DLLIMPORT void unload_all_active();

  // upload a bitmap from RAM to the GPU
  // as an openGL texture. requires that init_opengl_texture
  // has been run.
#ifdef VSXU_OPENGL_ES
#define GL_BGRA_EXT 0
#endif
  VSX_TEXTURE_DLLIMPORT void upload_ram_bitmap(vsx_bitmap* vbitmap,bool mipmaps = false, bool upside_down = true);
  VSX_TEXTURE_DLLIMPORT void upload_ram_bitmap(unsigned long* data, unsigned long size_x, unsigned long size_y,bool mipmaps = false, int bpp = 4, int bpp2 = GL_BGRA_EXT, bool upside_down = true);

  // load a tga file in the same thread as ours (why would anyone use tga when png's around? anyway..)
//  void load_tga(vsx_string name, bool mipmaps = true);

  void* pti_l; // needed by the communication between the png thread and the texture. internal stuff.
  // load a png in the same thread as ours.
  VSX_TEXTURE_DLLIMPORT void load_png(vsx_string fname, bool mipmaps = true);
  VSX_TEXTURE_DLLIMPORT void load_png_thread(vsx_string fname, bool mipmaps = true);
  VSX_TEXTURE_DLLIMPORT void load_jpeg(vsx_string fname, bool mipmaps = true);

  // update the transform object with a new transformation
 	void set_transform(vsx_transform_obj* new_transform_obj) {
		if(transform_obj == new_transform_obj) return;
		if(transform_obj) delete transform_obj;
		transform_obj = new_transform_obj;
	}
	// return the transformation
	vsx_transform_obj* get_transform(){return transform_obj;}

  VSX_TEXTURE_DLLIMPORT void begin_capture();
  VSX_TEXTURE_DLLIMPORT void end_capture();

  // use this to bind the texture.
  VSX_TEXTURE_DLLIMPORT bool bind();
  // use this when you're done with the texture
  VSX_TEXTURE_DLLIMPORT void _bind();

  // use this to always set texture coordinates properly.
  VSX_TEXTURE_DLLIMPORT void texcoord2f(float x, float y);

  // constructors
  
  vsx_texture()
  {
    pti_l = 0;
    locked = false;
    rt = 0;
    valid = false;
    transform_obj = new vsx_transform_neutral;
  }
  vsx_texture(int id, int type);


  VSX_TEXTURE_DLLIMPORT void unload();
};


#endif
