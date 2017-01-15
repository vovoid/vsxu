#pragma once

#include <texture/vsx_texture.h>
#include <bitmap/vsx_bitmap_cache.h>
#include <tools/vsx_thread_pool.h>

class vsx_texture_gl_cache
{

  class vsx_texture_gl_cache_item
  {
  public:
    bool used = true;
    vsx_string<> filename;
    uint64_t bitmap_loader_hint;
    uint64_t hint;
    vsx_texture_gl* texture_gl;

    inline bool equals(const vsx_string<>& other_filename, uint64_t other_bitmap_loader_hint, uint64_t& other_hint)
    {
      if (vsx_string<>::s_equals(other_filename, filename)
          &&
          hint == other_hint
          &&
          bitmap_loader_hint == other_bitmap_loader_hint
          )
        return true;
      return false;
    }
  };

  vsx_nw_vector<vsx_texture_gl_cache_item*> items;
  vsx_lock items_lock;

  void create_item(vsx_string<>& filename, uint64_t bitmap_loader_hint, uint64_t hint, vsx_texture_gl* texture_gl)
  {
    vsx_texture_gl_cache_item* item = 0x0;
    items_lock.aquire();
    foreach (items, i)
      if (!items[i]->used)
      {
        items[i]->used = true;
        item = items[i];
        break;
      }

    if (!item)
    {
      item = new vsx_texture_gl_cache_item();
      items.push_back(item);
    }

    item->filename = filename;
    item->bitmap_loader_hint = bitmap_loader_hint;
    item->hint = hint;
    item->texture_gl = texture_gl;
    items_lock.release();
  }

  void recycle_item(vsx_texture_gl_cache_item* item)
  {
    foreach (items, i)
      if (items[i] == item)
        item->used = false;
  }

  vsx_texture_gl_cache_item* get_item(vsx_string<>& filename, uint64_t bitmap_loader_hint, uint64_t& hint)
  {
    foreach (items, i)
      if (items[i]->equals(filename, bitmap_loader_hint, hint))
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

  vsx_texture_gl* create(vsx_string<>& filename, uint64_t bitmap_loader_hint, uint64_t hint)
  {
    vsx_texture_gl* texture_gl = new vsx_texture_gl(true);
    texture_gl->references = 1;
    texture_gl->bitmap_loader_hint = bitmap_loader_hint;
    texture_gl->hint = hint;

    texture_gl->bitmap = vsx_bitmap_cache::get_instance()->aquire_create( filename, bitmap_loader_hint );

    create_item(filename, bitmap_loader_hint, hint, texture_gl);
    return texture_gl;
  }

  void dump_to_stdout()
  {
    foreach (items, i)
      vsx_printf(L"item filename: %hs %d\n", items[i]->filename.c_str(), items[i]->used);
  }

  bool has(vsx_string<>& filename, uint64_t bitmap_loader_hint, uint64_t hint)
  {
    foreach (items, i)
      if (items[i]->equals(filename, bitmap_loader_hint, hint))
        if (items[i]->used)
          return true;
    return false;
  }

  vsx_texture_gl* aquire(vsx_string<>& filename, vsx::filesystem* filesystem, bool reload_with_thread, uint64_t bitmap_loader_hint, uint64_t hint, bool reload = false)
  {
    vsx_texture_gl_cache_item* item = get_item(filename, bitmap_loader_hint, hint);
    req_error_v(item, "Invalid cache item", 0x0);

    if (!reload)
      item->texture_gl->references++;

    // reload data
    if (reload)
    {
      vsx_printf(L"reloading %hs\n", filename.c_str());
      item->texture_gl->uploaded_to_gl = false;
      item->texture_gl->bitmap = vsx_bitmap_cache::get_instance()->aquire_reload( filename, filesystem, reload_with_thread, bitmap_loader_hint );
      return item->texture_gl;
    }

    item->texture_gl->bitmap = vsx_bitmap_cache::get_instance()->aquire( filename, bitmap_loader_hint );
    return item->texture_gl;
  }

  vsx_texture_gl* aquire_create(vsx_string<>& filename, vsx::filesystem* filesystem, uint64_t bitmap_loader_hint, uint64_t hint)
  {
    if (has(filename, bitmap_loader_hint, hint))
      return aquire(filename, filesystem, false, bitmap_loader_hint, hint);
  }

  void destroy(vsx_texture_gl*& texture_gl)
  {
    req(texture_gl);

    vsx_texture_gl_cache_item* item = get_item(texture_gl);
    if (!item)
      VSX_ERROR_RETURN("Invalid cache item");

    vsx_bitmap_cache::get_instance()->destroy( item->texture_gl->bitmap );

    // decrease references
    item->texture_gl->references--;

    // still more references
    if (item->texture_gl->references)
    {
      texture_gl = 0x0;
      return;
    }

    item->texture_gl->unload();
    delete item->texture_gl;
    recycle_item(item);
    texture_gl = 0x0;
  }

  ~vsx_texture_gl_cache()
  {
    foreach (items, i)
      delete items[i];
  }

  static vsx_texture_gl_cache* get_instance()
  {
    static vsx_texture_gl_cache vtdc;
    return &vtdc;
  }
};
