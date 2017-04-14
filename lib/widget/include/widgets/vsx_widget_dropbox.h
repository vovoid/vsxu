#pragma once

#include <vsx_widget.h>
#include <widgets/vsx_widget_button.h>
#include <widgets/vsx_widget_label.h>
#include <widgets/vsx_widget_popup_menu.h>

class vsx_widget_dropbox
    : public vsx_widget
{

  class option
  {
  public:
    int value = 0;
    vsx_string<> title;

    option()
    {}

    option(int value, vsx_string<> title)
    {
      this->value = value;
      this->title = title;
    }
  };

  vsx_nw_vector<option> options;

  vsx_widget_label* label = 0x0;
  vsx_widget_button* button = 0x0;

  void on_select(vsx_command_s& command)
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

public:
  size_t selected = 0;

  std::function<void(size_t, vsx_string<>&)> on_selection = [](size_t value, vsx_string<>& title){};

  vsx_widget_dropbox(vsx_string<> title)
  {
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

  void show_menu()
  {
    menu->set_render_type(render_type);
    menu->size.x = size.x;
    menu->target_size.x = size.x;
    menu->set_pos( get_pos_p() + vsx_vector3f(-size.x * 0.5f, -size.y * 0.5f) );
    menu->show();
  }

  void recalculate_sizes()
  {
    label->set_size(vsx_vector3f(size.x - 0.05f , 0.1f) );
    label->set_pos( vsx_vector3f( -0.025f, 0.0f) );
    button->set_size( vsx_vector3f(0.05f, 0.1f));
    button->set_pos( vsx_vector3f( size.x * 0.5f - 0.025f, 0.0f));
  }

  void add_option(int value, vsx_string<> title)
  {
    options.push_back(option(value, title));
    menu->commands.adds(VSX_COMMAND_MENU, title, vsx_string_helper::i2s(value), "");
    dynamic_cast<vsx_widget_popup_menu*>(menu)->init_extra_commands();
  }

  void i_draw()
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

};
