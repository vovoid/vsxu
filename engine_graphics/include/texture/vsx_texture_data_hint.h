#ifndef VSX_TEXTURE_DATA_HINT_H
#define VSX_TEXTURE_DATA_HINT_H


class vsx_texture_data_hint
{
public:
  bool flip_vertically = false;
  bool split_cubemap = false;

  bool equals(vsx_texture_data_hint& other)
  {
    if (other.flip_vertically == flip_vertically && other.split_cubemap == split_cubemap)
      return true;
    return false;
  }

  vsx_texture_data_hint()
  {}

  vsx_texture_data_hint(bool flip_vertically_n, bool split_cubemap_n )
    :
      flip_vertically(flip_vertically_n),
      split_cubemap(split_cubemap_n)
  {}
};

#endif
