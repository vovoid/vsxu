#ifndef VSX_BITMAP_CACHE_H
#define VSX_BITMAP_CACHE_H

#include <map>
#include "loader/vsx_bitmap_loader_helper.h"


class vsx_bitmap_cache
{

  class vsx_bitmap_cache_item
  {
  public:
    bool used = true;
    vsx_string<> filename;
    vsx_bitmap_loader_hint hint;
    vsx_bitmap* bitmap;

    inline bool equals(const vsx_string<>& other_filename, vsx_bitmap_loader_hint& other_hint)
    {
      if (vsx_string<>::s_equals(other_filename, filename) && hint.equals(other_hint))
        return true;
      return false;
    }
  };

  vsx_nw_vector<vsx_bitmap_cache_item*> items;

  vsx_bitmap_cache_item* create_item()
  {
    foreach (items, i)
      if (!items[i]->used)
      {
        items[i]->used = true;
        return items[i];
      }
    vsx_bitmap_cache_item* item = new vsx_bitmap_cache_item();
    items.push_back(item);
    return item;
  }

  void recycle_item(vsx_bitmap_cache_item* item)
  {
    foreach (items, i)
      if (items[i] == item)
        item->used = false;
  }

  vsx_bitmap_cache_item* get_item(vsx_string<>& filename, vsx_bitmap_loader_hint& hint)
  {
    foreach (items, i)
      if (items[i]->equals(filename, hint))
        return items[i];
    return 0;
  }

  vsx_bitmap_cache_item* get_item(vsx_bitmap* bitmap)
  {
    foreach (items, i)
      if (items[i]->bitmap == bitmap)
        return items[i];
    return 0;
  }


public:

  vsx_bitmap* create(vsx_string<>& filename,  vsx_bitmap_loader_hint hint)
  {
    vsx_bitmap* bitmap = new vsx_bitmap(true);
    bitmap->references = 1;
    bitmap->hint = hint;

    vsx_bitmap_cache_item* item = create_item();
    item->hint = hint;
    item->filename = filename;
    item->bitmap = bitmap;

    return bitmap;
  }

  bool has(vsx_string<>& filename, vsx_bitmap_loader_hint hint)
  {
    foreach (items, i)
      if (items[i]->equals(filename, hint))
        if (items[i]->used)
          return true;
    return false;
  }

  vsx_bitmap* aquire(vsx_string<>& filename, vsx_bitmap_loader_hint hint)
  {
    vsx_bitmap_cache_item* item = get_item(filename, hint);
    if (!item)
      VSX_ERROR_RETURN_V("Invalid texture data item", 0x0);

    item->bitmap->references++;
    return item->bitmap;
  }

  vsx_bitmap* aquire_reload( vsx_string<>& filename, vsxf* filesystem, bool thread, vsx_bitmap_loader_hint hint)
  {
    vsx_bitmap* bitmap = aquire(filename, hint);
    vsx_bitmap_loader_helper::load(bitmap, bitmap->filename, filesystem, thread, hint );
    return bitmap;
  }

  vsx_bitmap* aquire_create(vsx_string<>& filename, vsx_bitmap_loader_hint hint)
  {
    if (has(filename, hint))
      return aquire(filename, hint);
    return create(filename, hint);
  }

  void destroy(vsx_bitmap*& bitmap)
  {
    vsx_bitmap_cache_item* item = get_item(bitmap);
    if (!item)
      VSX_ERROR_RETURN("Invalid texture data item");

    // decrease references
    item->bitmap->references--;

    // still more references
    if (item->bitmap->references)
      return;

    delete item->bitmap;
    recycle_item(item);
    bitmap = 0;
  }

  static vsx_bitmap_cache* get_instance()
  {
    static vsx_bitmap_cache vtdc;
    return &vtdc;
  }
};

#endif
