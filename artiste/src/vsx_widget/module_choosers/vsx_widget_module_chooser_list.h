#ifndef VSX_NO_CLIENT
#ifndef VSX_MODULE_CHOOSER_LIST_H
#define VSX_MODULE_CHOOSER_LIST_H



class vsx_module_chooser_list : public vsx_widget_window {
  vsx_string i_hint;
  //std::map<vsx_string, vsx_widget*> fields;
  vsx_widget *edit;
  vsx_widget *search;
  
  std::vector<vsx_string> i_rows;

public:
  void vsx_command_process_b(vsx_command_s *t);
  
  void show(vsx_string value);
  void show();
  void init() {};
  void i_draw();
	bool event_key_down(signed long key, bool alt = false, bool ctrl = false, bool shift = false);
	void add_item(vsx_string name,vsx_module_info* m_info);
	void build_tree();
	void set_server(vsx_widget* serv);
  vsx_module_chooser_list();
};

#endif
#endif
