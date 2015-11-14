#ifndef VSX_TEXTURE_GL_CACHE_H
#define VSX_TEXTURE_GL_CACHE_H

#include <texture/vsx_texture.h>

class vsx_texture_gl_cache
{
  std::map<vsx_string<>, vsx_texture_gl*> t_glist;

public:

  void unload_all_active()
  {
    for (std::map<vsx_string<>, vsx_texture_gl*>::iterator it = t_glist.begin(); it != t_glist.end(); ++it)
    {
      glDeleteTextures(1,&((*it).second->gl_id));

      (*it).second->gl_id = 0;
      (*it).second->uploaded_to_gl = false;
    }
  }

  void reinit_all_active()
  {
    //std::map<vsx_string<>, vsx_texture_gl> temp_glist = t_glist;
    //vsx_string<>tname;
    //for (std::map<vsx_string<>, vsx_texture_gl*>::iterator it = temp_glist.begin(); it != temp_glist.end(); ++it)
    //{
      //TODO: mark for re-upload deferred
    //}
  }

  bool has(vsx_string<>& filename)
  {
    if (t_glist.find(filename) == t_glist.end())
      return false;
    return true;
  }

  vsx_texture_gl* create(vsx_string<>& filename)
  {
    vsx_texture_gl* texture_gl = new vsx_texture_gl(true);
    texture_gl->references = 1;
    t_glist[filename] = texture_gl;
    return texture_gl;
  }

  vsx_texture_gl* aquire(vsx_string<>& filename)
  {
    vsx_texture_gl* texture_gl = t_glist[filename];
    texture_gl->references++;
    vsx_printf(L"texture_gl_cache: aquiring %s, references: %d\n", filename.c_str(), texture_gl->references);
    return texture_gl;
  }

  void destroy(vsx_string<>& filename)
  {
    vsx_texture_gl* texture_gl = t_glist[filename];
    texture_gl->references--;
    vsx_printf(L"texture_gl_cache: destroying %s, references: %d\n", filename.c_str(), texture_gl->references);
    if (!texture_gl->references)
    {
      vsx_printf(L"texture_gl_cache: deleting %s\n", filename.c_str(), texture_gl->references);
      t_glist.erase(filename);
      delete texture_gl;
    }
  }



  static vsx_texture_gl_cache* get_instance()
  {
    static vsx_texture_gl_cache vtlc;
    return &vtlc;
  }
};

#endif
