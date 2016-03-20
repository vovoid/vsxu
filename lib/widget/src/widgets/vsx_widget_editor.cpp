#include <widgets/vsx_widget_editor.h>


vsx_widget_editor::vsx_widget_editor() {
  scrollbar_horiz = (vsx_widget_scrollbar*)add(new vsx_widget_scrollbar,"horiz");
  scrollbar_vert = (vsx_widget_scrollbar*)add(new vsx_widget_scrollbar,"vert");
  editor = (vsx_widget_base_edit*)add(new vsx_widget_base_edit,"edit");
  init_children();
  editor->size_from_parent = true;
  scrollbar_horiz->set_scroll_type(0);
  scrollbar_vert->set_scroll_type(1);
  scrollbar_horiz->set_control_value( &editor->scrollbar_pos_x );
  scrollbar_vert->set_control_value( &editor->scrollbar_pos_y );
  allow_move_y = allow_move_x = false;
}

void vsx_widget_editor::set_string(const vsx_string<>& str) {
  editor->set_string(str);
}
vsx_string<>vsx_widget_editor::get_string() {
  return editor->get_string();
}


void vsx_widget_editor::i_draw() {
  calc_size();

  float db15 = dragborder*2.5f;
  scrollbar_horiz->set_pos(vsx_vector3<>(-size.x*0.5,-size.y*0.5));
  scrollbar_horiz->set_size(vsx_vector3<>(target_size.x-db15, db15));
  scrollbar_horiz->set_window_size( editor->scroll_x_size );

  scrollbar_vert->set_pos(vsx_vector3<>(size.x*0.5-db15,-size.y*0.5+db15));
  scrollbar_vert->set_size(vsx_vector3<>(db15,target_size.y-scrollbar_horiz->size.y));
  scrollbar_vert->set_window_size( editor->scroll_y_size );

  editor->set_pos(vsx_vector3<>(-scrollbar_vert->size.x*0.5f,scrollbar_horiz->size.y*0.5f));
  editor->target_size.x = target_size.x-scrollbar_vert->size.x;
  editor->target_size.y = target_size.y-scrollbar_horiz->size.y;
}
