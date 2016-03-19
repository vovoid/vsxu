#pragma once

#include <map>
#include <tools/vsx_thread_pool.h>
#include <tools/vsx_lock.h>
#include "vsx_bitmap_loader.h"

class vsx_bitmap_cache
{

  class vsx_bitmap_cache_item
  {
  public:
    bool used = true;
    vsx_string<> filename;
    uint64_t hint;
    vsx_bitmap* bitmap;

    inline bool equals(const vsx_string<>& other_filename, uint64_t& other_hint)
    {
      if (vsx_string<>::s_equals(other_filename, filename) && hint == other_hint)
        return true;
      return false;
    }
  };

  vsx_nw_vector<vsx_bitmap_cache_item*> items;
  vsx_lock lock;

  vsx_bitmap_cache_item* create_item()
  {
  lock.aquire();
    foreach (items, i)
      if (!items[i]->used)
      {
        items[i]->used = true;
        lock.release();
        return items[i];
      }
    vsx_bitmap_cache_item* item = new vsx_bitmap_cache_item();
    items.push_back(item);
    lock.release();
    return item;
  }

  void recycle_item(vsx_bitmap_cache_item* item)
  {
    lock.aquire();
    foreach (items, i)
      if (items[i] == item)
        item->used = false;
    lock.release();
  }

  vsx_bitmap_cache_item* get_item(vsx_string<>& filename, uint64_t& hint)
  {
  lock.aquire();
    foreach (items, i)
      if (items[i]->equals(filename, hint))
      {
        vsx_bitmap_cache_item* item = items[i];
        lock.release();
        return item;
      }
    lock.release();
    return 0;
  }

  vsx_bitmap_cache_item* get_item(vsx_bitmap* bitmap)
  {
    lock.aquire();
    foreach (items, i)
      if (items[i]->bitmap == bitmap)
      {
        vsx_bitmap_cache_item* item = items[i];
        lock.release();
        return item;
      }
    lock.release();
    return 0;
  }

public:

  vsx_bitmap* create(vsx_string<>& filename, uint64_t hint)
  {
    vsx_bitmap* bitmap = new vsx_bitmap();
    bitmap->attached_to_cache = true;
    bitmap->references = 1;
    bitmap->hint = hint;

    vsx_bitmap_cache_item* item = create_item();
    item->hint = hint;
    item->filename = filename;
    item->bitmap = bitmap;

    return bitmap;
  }

  bool has(vsx_string<>& filename, uint64_t hint)
  {
    lock.aquire();
    foreach (items, i)
      if (items[i]->equals(filename, hint))
        if (items[i]->used)
        {
          lock.release();
          return true;
        }
    lock.release();
    return false;
  }

  vsx_bitmap* aquire(vsx_string<>& filename, uint64_t hint)
  {
    vsx_bitmap_cache_item* item = get_item(filename, hint);
    if (!item)
      VSX_ERROR_RETURN_V("Invalid texture data item", 0x0);

    item->bitmap->references++;
    return item->bitmap;
  }

  vsx_bitmap* aquire_reload( vsx_string<>& filename, vsx::filesystem* filesystem, bool thread, uint64_t hint)
  {
    vsx_bitmap_cache_item* item = get_item(filename, hint);
    vsx_bitmap* bitmap = item->bitmap;
    vsx_bitmap_loader::reload(bitmap, bitmap->filename, filesystem, thread, hint );
    return bitmap;
  }

  vsx_bitmap* aquire_create(vsx_string<>& filename, uint64_t hint)
  {
    if (has(filename, hint))
      return aquire(filename, hint);
    return create(filename, hint);
  }

  void destroy(vsx_bitmap*& bitmap)
  {
    if (!bitmap->attached_to_cache)
      VSX_ERROR_RETURN("Trying to destroy a non-cached bitmap...");

    vsx_bitmap_cache_item* item = get_item(bitmap);

    if (!item)
      VSX_ERROR_RETURN("Bitmap not found in cache");

    // decrease references
    item->bitmap->references--;

    // still more references
    if (item->bitmap->references)
      return;

    delete item->bitmap;
    recycle_item(item);
    bitmap = 0;
  }

  ~vsx_bitmap_cache()
  {
    foreach (items, i)
      delete items[i];
  }

  static vsx_bitmap_cache* get_instance()
  {
    static vsx_bitmap_cache vtdc;
    return &vtdc;
  }
};
