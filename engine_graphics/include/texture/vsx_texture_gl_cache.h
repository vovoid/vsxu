#ifndef VSX_TEXTURE_GL_CACHE_H
#define VSX_TEXTURE_GL_CACHE_H

#include <texture/vsx_texture.h>


class vsx_texture_gl_cache
{

  class vsx_texture_gl_cache_item
  {
  public:
    bool used = true;
    vsx_string<> filename;
    vsx_texture_gl_hint hint;
    vsx_texture_gl* texture_gl;

    inline bool equals(const vsx_string<>& other_filename, vsx_texture_gl_hint& other_hint)
    {
      if (vsx_string<>::s_equals(other_filename, filename) && hint.equals(other_hint))
        return true;
      return false;
    }

  };

  vsx_nw_vector<vsx_texture_gl_cache_item*> items;

  vsx_texture_gl_cache_item* create_item()
  {
    foreach (items, i)
      if (!items[i]->used)
      {
        items[i]->used = true;
        return items[i];
      }
    vsx_texture_gl_cache_item* item = new vsx_texture_gl_cache_item();
    items.push_back(item);
    return item;
  }

  void recycle_item(vsx_texture_gl_cache_item* item)
  {
    foreach (items, i)
      if (items[i] == item)
        item->used = false;
  }

  vsx_texture_gl_cache_item* get_item(vsx_string<>& filename, vsx_texture_gl_hint& hint)
  {
    foreach (items, i)
      if (items[i]->equals(filename, hint))
        return items[i];
    return 0;
  }

  vsx_texture_gl_cache_item* get_item(vsx_texture_gl* texture_gl)
  {
    foreach (items, i)
      if (items[i]->texture_gl == texture_gl)
        return items[i];
    return 0;
  }


public:

  vsx_texture_gl* create(vsx_string<>& filename,  vsx_texture_gl_hint hint)
  {
    vsx_texture_gl* texture_gl = new vsx_texture_gl(true);
    texture_gl->references = 1;
    texture_gl->hint = hint;

    vsx_texture_gl_cache_item* item = create_item();
    item->hint = hint;
    item->filename = filename;
    item->texture_gl = texture_gl;

    return texture_gl;
  }

  bool has(vsx_string<>& filename, vsx_texture_gl_hint hint)
  {
    foreach (items, i)
      if (items[i]->equals(filename, hint))
        if (items[i]->used)
          return true;
    return false;
  }

  vsx_texture_gl* aquire(vsx_string<>& filename, vsx_texture_gl_hint hint)
  {
    vsx_texture_gl_cache_item* item = get_item(filename, hint);
    if (!item)
      VSX_ERROR_RETURN_V("Invalid texture glitem", 0x0);
    item->texture_gl->references++;
    return item->texture_gl;
  }

  void destroy(vsx_texture_gl*& texture_gl)
  {
    vsx_texture_gl_cache_item* item = get_item(texture_gl);
    if (!item)
      VSX_ERROR_RETURN("Invalid texture gl item");

    vsx_texture_data_cache::get_instance()->destroy( item->texture_gl->texture_data );

    // decrease references
    item->texture_gl->references--;

    // still more references
    if (item->texture_gl->references)
      return;

    delete item->texture_gl;
    recycle_item(item);
    texture_gl = 0;
  }

  static vsx_texture_gl_cache* get_instance()
  {
    static vsx_texture_gl_cache vtdc;
    return &vtdc;
  }
};
/*class vsx_texture_gl_cache
{
  std::map<vsx_string<>, vsx_texture_gl*> t_glist;

public:


  bool has(vsx_string<>& filename, vsx_texture_gl_hint hint)
  {
    if (t_glist.find(filename) == t_glist.end())
      return false;
    return true;
  }

  vsx_texture_gl* create(vsx_string<>& filename, vsx_texture_gl_hint hint)
  {
    vsx_texture_gl* texture_gl = new vsx_texture_gl(true);
    texture_gl->references = 1;
    t_glist[filename] = texture_gl;
    return texture_gl;
  }

  vsx_texture_gl* aquire(vsx_string<>& filename, vsx_texture_gl_hint hint)
  {
    vsx_texture_gl* texture_gl = t_glist[filename];
    texture_gl->references++;
//    vsx_printf(L"texture_gl_cache: aquiring %s, references: %d\n", filename.c_str(), texture_gl->references);
    return texture_gl;
  }

  void destroy(vsx_string<>& filename, vsx_texture_gl_hint hint)
  {
    vsx_texture_gl* texture_gl = t_glist[filename];
    texture_gl->references--;
//    vsx_printf(L"texture_gl_cache: destroying %s, references: %d\n", filename.c_str(), texture_gl->references);
    if (!texture_gl->references)
    {
//      vsx_printf(L"texture_gl_cache: deleting %s\n", filename.c_str(), texture_gl->references);
      t_glist.erase(filename);
      delete texture_gl;
    }
  }



  static vsx_texture_gl_cache* get_instance()
  {
    static vsx_texture_gl_cache vtlc;
    return &vtlc;
  }
};*/

#endif
