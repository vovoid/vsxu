#pragma once

#include <vsx_widget.h>
#include <widgets/vsx_widget_label.h>
#include <widgets/vsx_widget_button.h>

class vsx_widget_checkbox
    : public vsx_widget
{
  vsx_widget_label* label = 0x0;
  vsx_widget_button* button = 0x0;

public:

  bool checked = false;

  vsx_widget_checkbox(vsx_string<> title)
  {
    label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
    label->set_pos( vsx_vector3f( -0.1, 0.0 ) );
    label->set_size( vsx_vector3f(0.1, 0.1) );
    label->set_font_size( 0.08f );
    label->title = title;
    label->halign = a_left;
    label->on_click =
      [this]()
      {
        checked = !checked;
        set_button_title_by_checked();
      };


    button = dynamic_cast<vsx_widget_button*>( add(new vsx_widget_button(), "button") );
    button->set_pos( vsx_vector3f( 0.1, 0.0));
    button->set_size( vsx_vector3f(0.05, 0.1));
    button->title = "[ ]";
    button->set_font_size( 0.08f );
    button->on_click =
      [this]()
      {
        checked = !checked;
        set_button_title_by_checked();
      };
  }

  void set_button_title_by_checked()
  {
    if (checked)
      button->title = "[x]";
    else
      button->title = "[ ]";
  }

  void recalculate_sizes()
  {
    label->set_size(vsx_vector3f(size.x - 0.05f , 0.1) );
    label->set_pos( vsx_vector3f( -0.025f, 0) );
    button->set_size( vsx_vector3f(0.05, 0.1));
    button->set_pos( vsx_vector3f( size.x * 0.5f - 0.025, 0.0));
  }

  void i_draw()
  {
    recalculate_sizes();
    draw_box_gradient(
          get_pos_p() - vsx_vector3f(size.x * 0.5f, size.y * 0.5f),
          size.x, size.y,
          vsx_widget_skin::get_instance()->get_color(1), vsx_widget_skin::get_instance()->get_color(1),
          vsx_widget_skin::get_instance()->get_color(1), vsx_widget_skin::get_instance()->get_color(1)
    );
  }

};
