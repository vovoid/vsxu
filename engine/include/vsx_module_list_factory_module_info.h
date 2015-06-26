#ifndef VSX_MODULE_LIST_FACTORY_MODULE_INFO_H
#define VSX_MODULE_LIST_FACTORY_MODULE_INFO_H

class vsx_module_list_factory_module_info
{
public:
  vsx_string<>name;
  void* cm; //create module func
  void* dm; //delete module func
  void* nm; //number of modules
  vsx_module_list_factory_module_info(vsx_string<>n_name, void *n_cm, void* n_dm, void* n_nm)
  {
    name = n_name;
    cm = n_cm;
    dm = n_dm;
    nm = n_nm;
  }
};




#endif
