#ifndef VSX_TEXTURE_DATA_CACHE_H
#define VSX_TEXTURE_DATA_CACHE_H

#include <map>
#include <texture/vsx_texture.h>


class vsx_texture_data_cache
{
  std::map<vsx_string<>, vsx_texture_data*> t_glist;
public:

  vsx_texture_data* create(vsx_string<>& filename, size_t type)
  {
    vsx_texture_data* texture_data = new vsx_texture_data(type, true);
    t_glist[filename] = texture_data;
    texture_data->references = 1;
    return texture_data;
  }

  vsx_texture_data* aquire(vsx_string<>& filename)
  {
    vsx_texture_data* texture_data = t_glist[filename];
    texture_data->references++;
    return texture_data;
  }

  void destroy(vsx_string<>& filename)
  {
    vsx_texture_data* texture_data = t_glist[filename];
    texture_data->references--;
    if (!texture_data->references)
    {
      t_glist.erase(filename);
      delete texture_data;
    }
  }

  static vsx_texture_data_cache* get_instance()
  {
    static vsx_texture_data_cache vtdc;
    return &vtdc;
  }
};

#endif
