#ifndef VSX_WIDGET_SKIN_H
#define VSX_WIDGET_SKIN_H

#include <vsx_gl_global.h>
#include <vsx_widget_dllimport.h>
#include <command/vsx_command_list.h>
#include <string/vsx_string.h>
#include <color/vsx_color.h>

class vsx_widget_skin
{
  vsx_string<>skin_path;
  vsx_color<> skin_colors[20];

public:

  void set_color_gl(const size_t &index)
  {
    glColor4fv(&skin_colors[index].r);
  }

  void set_color_gl_a(const size_t &index, float a) 
  {
    glColor4f(
      skin_colors[index].r,
      skin_colors[index].g,
      skin_colors[index].b,
      skin_colors[index].a * a
    );
  }

  vsx_color<> get_color(size_t index) 
  {
    return skin_colors[index];
  }

  vsx_color<> get_color(size_t index, float rm, float gm = 1.0, float bm = 1.0, float am = 1.0)
  {
    return vsx_color<>(
      skin_colors[index].r * rm,
      skin_colors[index].g * gm,
      skin_colors[index].b * bm,
      skin_colors[index].a * am
    );
  }

  void skin_path_set(vsx_string<>n)
  {
    skin_path = n;
  }

  vsx_string<>& skin_path_get() 
  {
    return skin_path;
  }

  void init()
  {
    vsx_command_s* mc = 0;
    vsx_command_list skin_conf(true);
    skin_conf.load_from_file(skin_path+"skin.conf",true,4);

    skin_conf.reset();
    while ( (mc = skin_conf.get()) ) {
      if (mc->cmd == "color") {
        vsx_nw_vector< vsx_string<> > parts;
        vsx_string<>deli = ",";
        vsx_string_helper::explode(mc->parts[2],deli, parts);
        vsx_color<> p;
        p.r = vsx_string_helper::s2f(parts[0]);
        p.g = vsx_string_helper::s2f(parts[1]);
        p.b = vsx_string_helper::s2f(parts[2]);
        p.a = vsx_string_helper::s2f(parts[3]);
        skin_colors[ vsx_string_helper::s2i(mc->parts[1]) ] = p;
      }
    }
    skin_conf.clear_delete();
  }

private:
  WIDGET_DLLIMPORT static vsx_widget_skin instance;
public:
  static vsx_widget_skin* get_instance() 
  {
    return &instance;
  }

};



#endif
