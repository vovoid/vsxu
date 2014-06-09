#include "vsx_gl_global.h"
#include "vsx_gl_state.h"


#include "ftgl/FTGLPolygonFont.h"
#include "ftgl/FTGLBitmapFont.h"
#include "ftgl/FTGLOutlineFont.h"
#include "ftgl/FTGLTextureFont.h"

#include "vsx_font_outline.h"





int vsx_font_outline::process_lines()
{
  if (!ftfont)
    return 0;

  vsx_string deli = "\n";
  vsx_avector<vsx_string> t_lines;
  explode(text, deli, t_lines);
  lines.clear();
  for (unsigned long i = 0; i < t_lines.size(); ++i)
  {
    float x1, y1, z1, x2, y2, z2;
    lines[i].string = t_lines[i];
    ((FTFont*)ftfont)->BBox(t_lines[i].c_str(), x1, y1, z1, x2, y2, z2);
    lines[i].size_x = x2 - x1;
    lines[i].size_y = y2 - y1;
  }
  return 1;
}

void vsx_font_outline::load_font(vsx_string font_path)
{
  if (!filesystem)
    ERROR_RETURN("filesystem not set");

  vsxf_handle *fp;

  if ((fp = filesystem->f_open(font_path.c_str(), "rb")) == NULL)
    ERROR_RETURN_S("font not found: ",cur_font.c_str());

  if (ftfont) {
    delete (FTFont*)ftfont;
    ftfont = 0;
  }
  if (ftfont2) {
    delete (FTFont*)ftfont2;
    ftfont2 = 0;
  }
  unsigned long size = filesystem->f_get_size(fp);

  char* fdata = (char*)malloc(size);
  unsigned long bread = filesystem->f_read((void*)fdata, size, fp);
  if (bread == size)
  {
    switch (render_type)
    {
      case 0:
        ftfont = new FTGLTextureFont((unsigned char*)fdata, size);
      break;
      case 1:
        ftfont = new FTGLPolygonFont((unsigned char*)fdata, size);
        ftfont2 = new FTGLOutlineFont((unsigned char*)fdata, size);
      break;
    }
    ((FTFont*)ftfont)->FaceSize((unsigned int)round(glyph_size));
    ((FTFont*)ftfont)->CharMap(ft_encoding_unicode);
    if (ftfont2)
    {
      ((FTFont*)ftfont2)->FaceSize((unsigned int)round(glyph_size));
      ((FTFont*)ftfont2)->CharMap(ft_encoding_unicode);
      vsx_printf("loaded font 2-------------------------------------------------------------------------------\n");
    }

  }
  filesystem->f_close(fp);
}

void vsx_font_outline::render()
{
  if (!ftfont)
    ERROR_RETURN("Font not initialized");

  if (gl_state == 0x0)
    gl_state = vsx_gl_state::get_instance();



  gl_state->matrix_mode (VSX_GL_MODELVIEW_MATRIX );
  gl_state->matrix_push();


//  gl_state->matrix_scale_f( 0.8*0.01, 0.01, 0.01 );

  int l_align = align;
  float l_leading = leading;
  float ypos = 0;

  if (render_type == 0)
    glEnable(GL_TEXTURE_2D);

  glColor4f(color.r, color.g, color.b, color.a);

  for (unsigned long i = 0; i < lines.size(); ++i)
  {
    gl_state->matrix_push();
    if (l_align == 0)
    {
      gl_state->matrix_translate_f( 0, ypos, 0 );
    } else
    if (l_align == 1)
    {
      gl_state->matrix_translate_f( -lines[i].size_x*0.5f,ypos, 0 );
    }
    if (l_align == 2)
    {
      gl_state->matrix_translate_f( -lines[i].size_x,ypos,0 );
    }

    ((FTFont*)ftfont)->Render(lines[i].string.c_str());

    if (render_type == 1)
    {
      if (!ftfont2)
      {
        gl_state->matrix_pop();
        gl_state->matrix_pop();
        ERROR_RETURN("Font Outline not initialized");
      }

      float pre_linew = gl_state->line_width_get();

      gl_state->line_width_set( outline_thickness );
      glColor4f(color_outline.r, color_outline.g, color_outline.b, color_outline.a);
      ((FTFont*)ftfont2)->Render(lines[i].string.c_str());
      gl_state->line_width_set( pre_linew );
      glColor4f(color.r, color.g, color.b, color.a);
    }

    gl_state->matrix_pop();
    ypos += l_leading;
  }

  if (render_type == 0)
    glDisable(GL_TEXTURE_2D);

  gl_state->matrix_pop();
}
