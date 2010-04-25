#ifndef VSX_COMP_VSXL_H
#define VSX_COMP_VSXL_H


class vsx_comp_vsxl_driver_abs {
public:
  vsx_module_param_list* my_param_list;
  void* comp;
  vsx_string script;
  virtual void* load(vsx_module_param_list* module_list,vsx_string program) = 0;
  virtual bool run() = 0;
  virtual bool unload() = 0;
  virtual ~vsx_comp_vsxl_driver_abs() {};
};

class vsx_comp_vsxl {
  vsx_comp_vsxl_driver_abs* my_driver;
public:
  void* load(vsx_module_param_list* module_list, vsx_string program);
  vsx_comp_vsxl_driver_abs* get_driver();
  bool execute();
  vsx_comp_vsxl();
  void unload();
  ~vsx_comp_vsxl();
};




#endif
