#pragma once
//#include <FTGL/ftgl.h>
//#include <vsx_font.h>

class font_outline_holder
{
  vsx_string<>file_name;
  size_t references;
  FTFont* font_inner;
  FTFont* font_outline;

  // binary data
  char* fdata;

public:

  font_outline_holder()
    :
      references(0),
      font_inner(0x0),
      font_outline(0x0),
      fdata(0x0)
  {}

  ~font_outline_holder()
  {
    unload();
  }

  inline void bind()
  {
    references++;
  }

  inline void unbind()
  {
    references--;
  }

  inline bool should_be_destroyed()
  {
    return references == 0;
  }

  FTFont* get_inner()
  {
    return font_inner;
  }

  FTFont* get_outline()
  {
    return font_outline;
  }

  vsx_string<>get_file_name()
  {
    return file_name;
  }

  void load(vsx::filesystem* filesystem, vsx_string<>font_file_name, vsx_font_outline_render_type type)
  {
    vsx::file* fp;

    if ((fp = filesystem->f_open(font_file_name.c_str())) == NULL)
      VSX_ERROR_RETURN_S(L"font not found: ",font_file_name.c_str());

    file_name = font_file_name;

    size_t size = filesystem->f_get_size(fp);

    fdata = (char*)malloc(size);
    filesystem->f_read((void*)fdata, size, fp);

    if (type & inner)
    {
      font_inner = new FTGLTextureFont((unsigned char*)fdata, size);
      font_inner->FaceSize(48);
      if (!font_inner->CharMap(ft_encoding_unicode))
        VSX_ERROR_RETURN("Could not set unicode freetype encoding");
    }
    if (type & outline)
    {
      font_outline = new FTGLOutlineFont((unsigned char*)fdata, size);
      font_outline->FaceSize(48);
      font_outline->CharMap(ft_encoding_unicode);
    }
    filesystem->f_close(fp);
  }

  void unload()
  {
    if (font_inner) {
      delete (FTFont*)font_inner;
      font_inner = 0;
    }
    if (font_outline) {
      delete (FTFont*)font_outline;
      font_outline = 0;
    }
    if (fdata)
      free(fdata);
  }
};


