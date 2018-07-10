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

  union
  {
    bool checked = false;
    bool value;
  };

  vsx_widget_checkbox(vsx_string<> title);

  void set_button_title_by_checked();

  void set_checked(bool value);

  void recalculate_sizes();

  void i_draw();

};
