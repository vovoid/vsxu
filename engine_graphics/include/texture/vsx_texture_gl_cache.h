#ifndef VSX_TEXTURE_GL_CACHE_H
#define VSX_TEXTURE_GL_CACHE_H

#include <texture/vsx_texture.h>
#include "vsx_texture_data_cache.h"

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

    texture_gl->texture_data = vsx_texture_data_cache::get_instance()->aquire_create(
      filename,
      vsx_texture_data_hint(
        hint.data_flip_vertically,
        hint.data_split_cubemap
      )
    );

    vsx_texture_gl_cache_item* item = create_item();
    item->hint = hint;
    item->filename = filename;
    item->texture_gl = texture_gl;

    vsx_printf(L"vsx_texture_gl_cache::create %s,  %lx \n", item->filename.c_str(), item->texture_gl);

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

  vsx_texture_gl* aquire(vsx_string<>& filename, vsxf* filesystem, bool reload_with_thread, vsx_texture_gl_hint hint)
  {
    vsx_texture_gl_cache_item* item = get_item(filename, hint);
    if (!item)
      VSX_ERROR_RETURN_V("Invalid texture glitem", 0x0);

    item->texture_gl->references++;

    vsx_texture_data_hint data_hint(
      hint.data_flip_vertically,
      hint.data_split_cubemap
    );

    // mark for re-upload as soon as data has loaded
    if (hint.cache_data_reload)
    {
      item->texture_gl->uploaded_to_gl = false;
      item->texture_gl->texture_data = vsx_texture_data_cache::get_instance()->aquire_reload( filename, filesystem, reload_with_thread, data_hint );
    } else
    item->texture_gl->texture_data = vsx_texture_data_cache::get_instance()->aquire( filename, data_hint );

    vsx_printf(L"vsx_texture_gl_cache::aquire %s,  %lx  references: %d\n", item->filename.c_str(), item->texture_gl, item->texture_gl->references);
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

    vsx_printf(L"vsx_texture_gl_cache::destroy %s,  %lx  references: %d\n", item->filename.c_str(), item->texture_gl, item->texture_gl->references);

    // still more references
    if (item->texture_gl->references)
      return;

    vsx_printf(L"    - deleting %s,  %lx \n", item->filename.c_str(), item->texture_gl);

    item->texture_gl->unload();
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

#endif
