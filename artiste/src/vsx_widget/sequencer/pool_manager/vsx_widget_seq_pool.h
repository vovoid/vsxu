#ifndef VSX_WIDGET_SEQ_POOL_H_
#define VSX_WIDGET_SEQ_POOL_H_



class vsx_widget_seq_pool_manager : public vsx_widget_window {
  vsx_string i_hint;

  // widgets
  vsx_widget *edit;
  vsx_widget *search;
  vsx_widget *button_add;
  vsx_widget *button_del;
  vsx_widget *button_clone;
  vsx_widget *button_toggle_edit;
  // dialogs
  vsx_widget *name_dialog;
  vsx_widget *clone_name_dialog;
  // sequencer
  vsx_widget *sequencer;

public:
  void vsx_command_process_b(vsx_command_s *t);

  void show(vsx_string value);
  void show();
  void init();
  void i_draw();
	bool event_key_down(signed long key, bool alt = false, bool ctrl = false, bool shift = false);
	void set_server(vsx_widget* serv);
  vsx_widget_seq_pool_manager();
};


#endif /*VSX_WIDGET_SEQ_POOL_H_*/
