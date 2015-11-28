#ifndef VSX_TEXTURE_DATA_CACHE_H
#define VSX_TEXTURE_DATA_CACHE_H

#include <map>
#include <texture/vsx_texture.h>



class vsx_texture_data_cache
{

  class vsx_texture_data_cache_item
  {
  public:
    bool used = true;
    vsx_string<> filename;
    vsx_texture_data_hint hint;
    vsx_texture_data* texture_data;

    inline bool equals(const vsx_string<>& other_filename, vsx_texture_data_hint& other_hint)
    {
      if (vsx_string<>::s_equals(other_filename, filename) && hint.equals(other_hint))
        return true;
      return false;
    }

  };

  vsx_nw_vector<vsx_texture_data_cache_item*> items;

  vsx_texture_data_cache_item* create_item()
  {
    foreach (items, i)
      if (!items[i]->used)
      {
        items[i]->used = true;
        return items[i];
      }
    vsx_texture_data_cache_item* item = new vsx_texture_data_cache_item();
    items.push_back(item);
    return item;
  }

  void recycle_item(vsx_texture_data_cache_item* item)
  {
    foreach (items, i)
      if (items[i] == item)
        item->used = false;
  }

  vsx_texture_data_cache_item* get_item(vsx_string<>& filename, vsx_texture_data_hint& hint)
  {
    foreach (items, i)
      if (items[i]->equals(filename, hint))
        return items[i];
    return 0;
  }

  vsx_texture_data_cache_item* get_item(vsx_texture_data* texture_data)
  {
    foreach (items, i)
      if (items[i]->texture_data == texture_data)
        return items[i];
    return 0;
  }


public:

  vsx_texture_data* create(vsx_string<>& filename,  vsx_texture_data_hint hint)
  {
    vsx_texture_data* texture_data = new vsx_texture_data(true);
    texture_data->references = 1;
    texture_data->hint = hint;

    vsx_texture_data_cache_item* item = create_item();
    item->hint = hint;
    item->filename = filename;
    item->texture_data = texture_data;

    return texture_data;
  }

  bool has(vsx_string<>& filename, vsx_texture_data_hint hint)
  {
    foreach (items, i)
      if (items[i]->equals(filename, hint))
        if (items[i]->used)
          return true;
    return false;
  }

  vsx_texture_data* aquire(vsx_string<>& filename, vsx_texture_data_hint hint)
  {
    vsx_texture_data_cache_item* item = get_item(filename, hint);
    if (!item)
      VSX_ERROR_RETURN_V("Invalid texture data item", 0x0);
    item->texture_data->references++;
    return item->texture_data;
  }

  void destroy(vsx_texture_data* texture_data)
  {
    vsx_texture_data_cache_item* item = get_item(texture_data);
    if (!item)
      VSX_ERROR_RETURN("Invalid texture data item");

    // decrease references
    item->texture_data->references--;

    // still more references
    if (item->texture_data->references)
      return;

    delete item->texture_data;
    recycle_item(item);
  }

  static vsx_texture_data_cache* get_instance()
  {
    static vsx_texture_data_cache vtdc;
    return &vtdc;
  }
};

#endif
