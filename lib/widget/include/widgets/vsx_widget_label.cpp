#include "vsx_widget_label.h"

bool vsx_widget_label::inside_xyd(vsx_vector3<> world, vsx_vector3<> screen)
{
  VSX_UNUSED(world);
  VSX_UNUSED(screen);

  return false;
}

void vsx_widget_label::init()
{
  halign = a_center;
  font_size = size.y = 0.014f;
  set_render_type(vsx_widget_render_type::render_2d);
}

void vsx_widget_label::event_mouse_down(vsx_widget_distance distance, vsx_widget_coords coords, int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  outside = false;
  m_focus = this;
}

void vsx_widget_label::event_mouse_up(vsx_widget_distance distance, vsx_widget_coords coords, int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  VSX_UNUSED(button);
  req(!outside);

  on_click();
}

void vsx_widget_label::event_mouse_move(vsx_widget_distance distance, vsx_widget_coords coords)
{
  VSX_UNUSED(coords);
  outside =
      !(
        (distance.corner.x > 0)
        &&
        (distance.corner.x < target_size.x)
        &&
        (distance.corner.y > 0)
        &&
        (distance.corner.y < target_size.y)
        );
}

void vsx_widget_label::draw_debug(vsx_vector3f position)
{
  draw_box_gradient(
        position - vsx_vector3f(size.x * 0.5f, size.y * 0.5f),
        size.x, size.y,
        vsx_widget_skin::get_instance()->get_color(3), vsx_widget_skin::get_instance()->get_color(3),
        vsx_widget_skin::get_instance()->get_color(4), vsx_widget_skin::get_instance()->get_color(4)
        );
}

void vsx_widget_label::i_draw()
{
  req(visible);

  vsx_vector3<> position = parent->get_pos_p() + pos;

  //    draw_debug(position);

  glColor3f(1,1,1);
  position.y -= font_size*0.5f;
  switch ((h_align)halign) {
  case a_left:
    position.x -= size.x * 0.5f;
    font.print(position, title,font_size);
    break;
  case a_center:
    font.print_center(position, title,font_size);
    break;
  case a_right:
    font.print_right(position, title,font_size);
    break;
  }
}
