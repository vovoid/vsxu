#ifndef VSX_BITMAP_HINT_H
#define VSX_BITMAP_HINT_H


class vsx_bitmap_loader_hint
{
public:
  bool flip_vertically = false;
  bool split_cubemap = false;

  bool equals(vsx_bitmap_loader_hint& other)
  {
    if (other.flip_vertically == flip_vertically && other.split_cubemap == split_cubemap)
      return true;
    return false;
  }

  vsx_bitmap_loader_hint()
  {}

  vsx_bitmap_loader_hint(bool flip_vertically_n, bool split_cubemap_n )
    :
      flip_vertically(flip_vertically_n),
      split_cubemap(split_cubemap_n)
  {}
};

#endif
