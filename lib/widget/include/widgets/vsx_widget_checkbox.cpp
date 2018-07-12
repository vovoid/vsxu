#include "vsx_widget_checkbox.h"

vsx_widget_checkbox::vsx_widget_checkbox(vsx_string<> title)
{
  allow_move_x = false;
  allow_move_y = false;

  label = dynamic_cast<vsx_widget_label*>( add(new vsx_widget_label(), "label") );
  label->set_pos( vsx_vector3f( -0.1f, 0.0 ) );
  label->set_size( vsx_vector3f(0.1f, 0.1f) );
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
  button->set_pos( vsx_vector3f( 0.1f, 0.0));
  button->set_size( vsx_vector3f(0.05f, 0.1f));
  button->title = "[ ]";
  button->set_font_size( 0.08f );
  button->on_click =
    [this]()
    {
      checked = !checked;
      set_button_title_by_checked();
    };
}

void vsx_widget_checkbox::set_button_title_by_checked()
{
  if (checked)
    button->title = "[x]";
  else
    button->title = "[ ]";
}

void vsx_widget_checkbox::set_checked(bool value)
{
  checked = value;
  set_button_title_by_checked();
}

void vsx_widget_checkbox::recalculate_sizes()
{
  label->set_size(vsx_vector3f(size.x - 0.05f , size.y) );
  label->set_pos( vsx_vector3f( -0.025f, 0) );
  label->set_font_size(font_size);
  button->set_size( vsx_vector3f(0.2f * size.x, size.y));
  button->set_pos( vsx_vector3f( size.x * 0.5f - 0.2f * size.x, 0.0));
  button->set_font_size(font_size);
}

void vsx_widget_checkbox::event_mouse_up(vsx_widget_distance distance, vsx_widget_coords coords, int button)
{
  checked = !checked;
  set_button_title_by_checked();
}

void vsx_widget_checkbox::i_draw()
{
  recalculate_sizes();
  draw_box_gradient(
        get_pos_p() - vsx_vector3f(size.x * 0.5f, size.y * 0.5f),
        size.x, size.y,
        vsx_widget_skin::get_instance()->get_color(1), vsx_widget_skin::get_instance()->get_color(1),
        vsx_widget_skin::get_instance()->get_color(1), vsx_widget_skin::get_instance()->get_color(1)
        );
}
