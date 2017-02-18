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

  void add(vsx_string<> component, vsx_string<> parameter, vsx_string<> value, float length, size_t interpolation_type)
  {
    vsx_widget_sequence_clipboard_item item;
    item.component = component;
    item.parameter = parameter;
    item.value = value;
    item.time_offset = length;
    item.interpolation_type = interpolation_type;
    items.push_back(item);
  }
};

#endif
