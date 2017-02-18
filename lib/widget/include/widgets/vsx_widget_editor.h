#pragma once

#include "vsx_widget_scrollbar.h"
#include "vsx_widget_panel.h"
#include "vsx_widget_base_edit.h"

class WIDGET_DLLIMPORT vsx_widget_editor : public vsx_widget_panel {
public:
  vsx_widget_scrollbar* scrollbar_horiz;
  vsx_widget_scrollbar* scrollbar_vert;
  vsx_widget_base_edit* editor;

  vsx_widget_editor();
  void set_string(const vsx_string<>& str);
  vsx_string<>get_string();

  virtual void i_draw();
};
