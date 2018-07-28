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

  void on_select(vsx_command_s& command);

public:
  size_t selected = 0;

  std::function<void(size_t, const vsx_string<>&)> on_selection = [](size_t value, const vsx_string<>& title){};
  vsx_widget_dropbox(vsx_string<> title);
  void show_menu();
  void recalculate_sizes();
  void reset_options();
  bool has_options();
  size_t get_option_count();
  void set_option_title(size_t index, vsx_string<> title);
  void add_option(int value, vsx_string<> title);
  void select_by_title(vsx_string<> title);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  size_t get_selected_index();
  vsx_string<> get_selected_title();
  void i_draw();

};
