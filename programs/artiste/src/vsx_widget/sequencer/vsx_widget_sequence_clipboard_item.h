#ifndef VSX_WIDGET_SEQUENCE_CLIPBOARD_ITEM_H
#define VSX_WIDGET_SEQUENCE_CLIPBOARD_ITEM_H


class vsx_widget_sequence_clipboard_item
{
public:
  vsx_string<> component;
  vsx_string<> parameter;
  vsx_string<> value;
  float time_offset;
  size_t interpolation_type;
};

#endif
