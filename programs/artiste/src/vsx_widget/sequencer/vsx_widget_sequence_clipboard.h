#ifndef VSX_WIDGET_SEQUENCE_CLIPBOARD_H
#define VSX_WIDGET_SEQUENCE_CLIPBOARD_H

#include "vsx_widget_sequence_clipboard_item.h"

class vsx_widget_sequence_clipboard
{
  vsx_nw_vector< vsx_widget_sequence_clipboard_item > items;

public:

  vsx_nw_vector< vsx_widget_sequence_clipboard_item >* items_get_ptr()
  {
    return &items;
  }

  void clear()
  {
    items.clear();
  }

  void add(vsx_string<> component, vsx_string<> parameter, float value)
  {
    vsx_widget_sequence_clipboard_item item;
    item.component = component;
    item.parameter = parameter;
    item.value = value;
    items.push_back(item);
  }
};

#endif
