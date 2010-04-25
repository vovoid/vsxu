#ifndef VSX_FONT_H
#define VSX_FONT_H

//#include "ftgl/FTGLExtrdFont.h"
//#include "ftgl/FTGLOutlineFont.h"
//#include "ftgl/FTGLPolygonFont.h"

struct vsx_font_info {
  int type; // 0 = texture (old) 1 = new (FtGL)
  vsx_texture* texture;
};  

class vsx_font {
  vsx_vector ep;
  void reinit(vsx_font_info* f_info,vsx_string font);
  bool list_built;
  GLuint dlist;
  //vsx_string old_string;
  //float old_size;
  float dx, dy, dz;
  vsx_string base_path;
  int i;
  float ch, cw, size_s;
  float ddx,ddy;
  char* stc;
  char* colc;
  float sx,sy,ex,ey;

  void init_vars() {
    mode_2d = false;
    //old_size = 0;
    //dlist = glGenLists(1);
    list_built = false;
//    outline_transparency = 0;
    my_font_info = 0;
    //background = false;
    color.r = 1;
    color.g = 1;
    color.b = 1;
    color.a = 1;
    align = 1.0;
  }
public:
  vsx_font_info* init(vsx_string font);

  vsx_avector<vsx_color> syntax_colors;
  // is in 2d mode?
  bool mode_2d;
  // keeping track of our font
  vsx_font_info* my_font_info;
  float outline_transparency;
  // is it translucent or not?
//  bool background;
  vsx_color color;
  double align;
  
  
  vsx_vector print(vsx_vector p, const vsx_string& str, const float size, const vsx_string colors = "");  
  vsx_vector print(vsx_vector p, const vsx_string& str, const vsx_string& font, float size, const vsx_string colors = "");  
  vsx_vector print_center(vsx_vector p, const vsx_string& str, float size);
  //vsx_vector print_center(vsx_vector p, const vsx_string& str, const vsx_string& font, float size);
  vsx_vector print_right(vsx_vector p, const vsx_string& str, float size);
  //vsx_vector print_right(vsx_vector p, const vsx_string& str, const vsx_string& font, float size);
  //vsx_vector get_size(vsx_vector p, const vsx_string& str, const vsx_string& font, float size);
  vsx_vector get_size(const vsx_string& str, float size);
  void reinit_all_active();
  
  vsx_font(vsx_string path) {
    base_path = path;
    init_vars();
  }

  vsx_font() {
    init_vars();
  }
};

#endif
