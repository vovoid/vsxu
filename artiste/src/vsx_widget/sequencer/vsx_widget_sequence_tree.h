#ifndef VSX_WIDGET_SEQUENCE_TREE_H
#define VSX_WIDGET_SEQUENCE_TREE_H

#include "widgets/vsx_widget_base_edit.h"

class vsx_widget_sequence_tree : public vsx_widget_editor
{
  vsx_widget_sequence_editor* sequence_editor;

public:

  void set_sequence_editor( vsx_widget_sequence_editor* editor )
  {
    sequence_editor = editor;
  }

  void extra_init()
  {
    editor->enable_syntax_highlighting = false;
    editor->mirror_mouse_double_click_object = this;
  }

  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
  {
    VSX_UNUSED(distance);
    VSX_UNUSED(coords);
    VSX_UNUSED(button);

    sequence_editor->toggle_channel_visible(editor->get_line(editor->selected_line));
  }
};


#endif // VSX_WIDGET_SEQUENCE_TREE_H
