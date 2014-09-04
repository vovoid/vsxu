#include "vsx_gl_global.h"
#include "vsx_gl_state.h"
#include "vsx_font_outline.h"
#include "ftgl/FTGLPolygonFont.h"
#include "ftgl/FTGLBitmapFont.h"
#include "ftgl/FTGLOutlineFont.h"
#include "ftgl/FTGLTextureFont.h"


class font_outline_holder
{
  size_t references;
  vsx_font_outline_render_type render_type;
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

  void load(vsxf* filesystem, vsx_string font_path, vsx_font_outline_render_type type)
  {
    vsxf_handle *fp;

    if ((fp = filesystem->f_open(font_path.c_str(), "rb")) == NULL)
      VSX_ERROR_RETURN_S("font not found: ",font_path.c_str());

    unsigned long size = filesystem->f_get_size(fp);

    fdata = (char*)malloc(size);
    unsigned long bread = filesystem->f_read((void*)fdata, size, fp);

    if (type & inner)
    {
      font_inner = new FTGLTextureFont((unsigned char*)fdata, size);
      font_inner->FaceSize(24);
      font_inner->CharMap(ft_encoding_unicode);
    }
    if (type & outline)
    {
      font_outline = new FTGLOutlineFont((unsigned char*)fdata, size);
      font_outline->FaceSize(24);
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

class outline_font_cache
{
  std::map<vsx_string,font_outline_holder* > cache;

  font_outline_holder* find_and_bind(vsx_string path)
  {
    if (cache.find(path) == cache.end())
      return 0x0;

    font_outline_holder* font_holder = cache[path];
    font_holder->bind();
    return font_holder;
  }

  font_outline_holder* create_and_bind(vsxf* filesystem, vsx_string path, vsx_font_outline_render_type type)
  {
    font_outline_holder* font_holder = new font_outline_holder;

    font_holder->load(filesystem, path, type);
    font_holder->bind();
    cache[path] = font_holder;
  }


public:

  font_outline_holder* get( vsxf* filesystem, vsx_string path, vsx_font_outline_render_type type)
  {
    font_outline_holder* font_holder = find_and_bind( path );
    if (font_holder)
      return font_holder;

    return create_and_bind(filesystem, path, type);
  }

  void recycle( font_outline_holder* holder )
  {
    holder->unbind();
    if (holder->should_be_destroyed())
      delete holder;
  }

  static outline_font_cache* get_instance()
  {
    static outline_font_cache ofc;
    return &ofc;
  }

};



int vsx_font_outline::process_lines()
{
  if (!font_holder)
    return 0;

  vsx_string deli = "\n";
  vsx_avector<vsx_string> t_lines;
  explode(text, deli, t_lines);
  lines.clear();
  for (unsigned long i = 0; i < t_lines.size(); ++i)
  {
    float x1, y1, z1, x2, y2, z2;
    lines[i].string = t_lines[i];
    FTFont* font = ((font_outline_holder*)font_holder)->get_inner();
    font->BBox(t_lines[i].c_str(), x1, y1, z1, x2, y2, z2);
    lines[i].size_x = x2 - x1;
    lines[i].size_y = y2 - y1;
  }
  return 1;
}

void vsx_font_outline::load_font(vsx_string font_path)
{
  if (!filesystem)
    VSX_ERROR_RETURN("filesystem not set");

  if (font_holder)
    unload();

  font_holder = outline_font_cache::get_instance()->get( filesystem, font_path, render_type );

}

void vsx_font_outline::unload()
{
  if (0x0 == font_holder)
    return;

  outline_font_cache::get_instance()->recycle( (font_outline_holder*)font_holder);
  font_holder = 0x0;
}

void vsx_font_outline::render_lines(void* font_inner_p, void* font_outline_p)
{
  FTFont* font_inner = (FTFont*) font_inner_p;
  FTFont* font_outline = (FTFont*) font_outline_p;

  float ypos = 0;
  for (unsigned long i = 0; i < lines.size(); ++i)
  {
    gl_state->matrix_push();
      if (align == 0)
        gl_state->matrix_translate_f( 0, ypos, 0 );

      if (align == 1)
        gl_state->matrix_translate_f( -lines[i].size_x*0.5f,ypos, 0 );

      if (align == 2)
        gl_state->matrix_translate_f( -lines[i].size_x,ypos,0 );

      glColor4f(color.r, color.g, color.b, color.a);
      font_inner->Render(lines[i].string.c_str());

      if (render_type & outline)
      {
        glColor4f(color_outline.r, color_outline.g, color_outline.b, color_outline.a);
        font_outline->Render(lines[i].string.c_str());
      }
    gl_state->matrix_pop();
    ypos += leading;
  }
}

void vsx_font_outline::render()
{
  if (!font_holder)
    VSX_ERROR_RETURN("Font not initialized");

  FTFont* font_inner = ((font_outline_holder*)font_holder)->get_inner();
  FTFont* font_outline = ((font_outline_holder*)font_holder)->get_outline();

  if (render_type & outline && !font_outline)
    VSX_ERROR_RETURN("Font Outline not initialized");

  if (gl_state == 0x0)
    gl_state = vsx_gl_state::get_instance();

  gl_state->matrix_mode (VSX_GL_MODELVIEW_MATRIX );
  gl_state->matrix_push();
  float pre_linew = gl_state->line_width_get();

    if (render_type & inner)
      glEnable(GL_TEXTURE_2D);

    gl_state->line_width_set( outline_thickness );
    render_lines(font_inner, font_outline);

    if (render_type & inner)
      glDisable(GL_TEXTURE_2D);

  gl_state->line_width_set( pre_linew );
  gl_state->matrix_pop();
}
