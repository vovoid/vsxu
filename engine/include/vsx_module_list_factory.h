#ifndef VSX_MODULE_LIST_FACTORY_H
#define VSX_MODULE_LIST_FACTORY_H

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #define VSX_MODULE_LIST_DLLIMPORT
#else
  #if defined(VSX_ENG_DLL)
    #define VSX_MODULE_LIST_DLLIMPORT __declspec (dllexport)
  #else
    #define VSX_MODULE_LIST_DLLIMPORT __declspec (dllimport)
  #endif
#endif


extern "C" {
 VSX_MODULE_LIST_DLLIMPORT vsx_module_list_abs* vsx_module_list_factory_create(vsx_string arguments, bool print_help);
 VSX_MODULE_LIST_DLLIMPORT void vsx_module_list_factory_destroy( vsx_module_list_abs* object );
}

#endif
