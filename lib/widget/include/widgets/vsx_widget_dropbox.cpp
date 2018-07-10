#include "vsx_widget_dropbox.h"

void vsx_widget_dropbox::on_select(vsx_command_s &command)
{
  int value = vsx_string_helper::s2i(command.cmd);
  foreach (options, i)
    if (options[i].value == value)
    {
      selected = i;
      label->title = options[selected].title;
      on_selection(value, options[selected].title);
      return;
    }
}

vsx_widget_dropbox::vsx_widget_dropbox(vsx_string<> title)
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
    show_menu();
  };

  button = dynamic_cast<vsx_widget_button*>( add(new vsx_widget_button(), "button") );
  button->set_pos( vsx_vector3f( 0.1f, 0.0f));
  button->set_size( vsx_vector3f(0.05f, 0.1f));
  button->title = "\x1f";
  button->set_font_size( 0.08f );
  button->on_click =
      [this]()
  {
    show_menu();
  };

  menu = add(new vsx_widget_popup_menu, ".comp_menu");
  menu->init();
  menu->size.x = 0.2f;
  dynamic_cast<vsx_widget_popup_menu*>(menu)->row_size_by_font_size( 0.08f );

  dynamic_cast<vsx_widget_popup_menu*>(menu)->on_selection =
      [this](vsx_command_s& command)
  {
    this->on_select(command);
  };
}

void vsx_widget_dropbox::show_menu()
{
  menu->set_render_type(render_type);
  menu->size.x = size.x;
  menu->target_size.x = size.x;
  menu->set_pos( get_pos_p() + vsx_vector3f(-size.x * 0.5f, -size.y * 0.5f) );
  menu->show();
}

void vsx_widget_dropbox::recalculate_sizes()
{
  //    label->set_size(vsx_vector3f(size.x - 0.05f , 0.1f) );
  //    label->set_pos( vsx_vector3f( -0.025f, 0.0f) );
  //    button->set_size( vsx_vector3f(0.05f, 0.1f));
  //    button->set_pos( vsx_vector3f( size.x * 0.5f - 0.025f, 0.0f));

  label->set_size(vsx_vector3f(size.x - 0.05f , size.y) );
  label->set_pos( vsx_vector3f( -0.025f, 0) );
  label->set_font_size(font_size);
  button->set_size( vsx_vector3f(0.2f * size.x, size.y));
  button->set_pos( vsx_vector3f( size.x * 0.5f - 0.1f * size.x, 0.0));
  button->set_font_size(font_size);
  dynamic_cast<vsx_widget_popup_menu*>(menu)->row_size_by_font_size( font_size );
}

void vsx_widget_dropbox::reset_options()
{
  options.reset_used();
  dynamic_cast<vsx_widget_popup_menu*>(menu)->clear();
}

bool vsx_widget_dropbox::has_options()
{
  return options.size() > 0;
}

size_t vsx_widget_dropbox::get_option_count()
{
  return options.size();
}

void vsx_widget_dropbox::set_option_title(size_t index, vsx_string<> title)
{
  options[index].title = title;
}

void vsx_widget_dropbox::add_option(int value, vsx_string<> title)
{
  options.push_back(option(value, title));
  menu->commands.adds(VSX_COMMAND_MENU, title, vsx_string_helper::i2s(value), "");
  dynamic_cast<vsx_widget_popup_menu*>(menu)->init_extra_commands();
}

void vsx_widget_dropbox::select_by_title(vsx_string<> title)
{
  selected = 0;
  foreach (options, i)
    if (options[i].title == title)
    {
      selected = i;
      return;
    }
}

void vsx_widget_dropbox::event_mouse_up(vsx_widget_distance distance, vsx_widget_coords coords, int button)
{
  show_menu();
}

size_t vsx_widget_dropbox::get_selected_index()
{
  return selected;
}

vsx_string<> vsx_widget_dropbox::get_selected_title()
{
  return options[selected].title;
}

void vsx_widget_dropbox::i_draw()
{
  req(options.size());
  recalculate_sizes();
  label->title = options[selected].title;
  draw_box_gradient(
        get_pos_p() - vsx_vector3f(size.x * 0.5f, size.y * 0.5f),
        size.x, size.y,
        vsx_widget_skin::get_instance()->get_color(1), vsx_widget_skin::get_instance()->get_color(0),
        vsx_widget_skin::get_instance()->get_color(0), vsx_widget_skin::get_instance()->get_color(1)
        );
}
