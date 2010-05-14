#ifndef VSX_PARAM_VSXL_H
#define VSX_PARAM_VSXL_H


class vsx_engine;

//////////////////////////////////////////////////////////////////////

class vsx_param_vsxl_abs {
  public:
    virtual void* load(vsx_module_param_abs* engine_param, vsx_string program, int id = -1) = 0;
    virtual void execute() = 0;
    virtual ~vsx_param_vsxl_abs() {};
};


class vsx_param_vsxl_driver_abs {

public:
  vsx_module_param_abs* my_param;
  int id;

#ifndef VSX_NO_CLIENT
  vsx_module_param_interpolation_list* interpolation_list;
#endif
  vsx_engine* engine;
  void* comp;
  int status;
  int result;
  void* state;
  vsx_string script;
  virtual void* load(vsx_module_param_abs* engine_param,vsx_string program) = 0;
  virtual void run() = 0;
  virtual void unload() = 0;
  //virtual ~vsx_param_vsxl_driver_abs() = 0;
};





class vsx_param_vsxl : public vsx_param_vsxl_abs {
  vsx_param_vsxl_driver_abs* my_driver;
public:
	vsx_engine* engine;
  void* load(vsx_module_param_abs* engine_param, vsx_string program, int id = -1);
  vsx_param_vsxl_driver_abs* get_driver();
  void execute();
  vsx_param_vsxl();
  void unload();
};


#endif
