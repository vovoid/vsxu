#ifndef VSX_TEXTURE_GL_LOADER_HINT_H
#define VSX_TEXTURE_GL_LOADER_HINT_H


class vsx_texture_gl_loader_hint
{
public:
  // data permutations
  bool data_flip_vertically = false;
  bool data_split_cubemap = false;

  bool mipmaps = false;
  bool linear_interpolate = true;

  // meta hints
  bool cache_data_reload = false;

  bool equals(vsx_texture_gl_loader_hint& other)
  {
    if (
        other.data_flip_vertically == data_flip_vertically
        &&
        other.data_split_cubemap == data_split_cubemap
        &&
        other.mipmaps == mipmaps
        &&
        other.linear_interpolate == linear_interpolate
    )
      return true;
    return false;
  }

  vsx_texture_gl_loader_hint()
  {
  }

  vsx_texture_gl_loader_hint(
      bool data_flip_vertically_n,
      bool data_split_cubemap_n,
      bool mipmaps_n,
      bool linear_interpolate_n
  )
    :
      data_flip_vertically(data_flip_vertically_n),
      data_split_cubemap(data_split_cubemap_n),
      mipmaps(mipmaps_n),
      linear_interpolate(linear_interpolate_n)
  {}
};

#endif
