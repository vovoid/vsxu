#ifndef VSX_MODULE_DLL_INFO_H_
#define VSX_MODULE_DLL_INFO_H_
#include <vsx_platform.h>


//-----internal:
//#ifdef VSXU_EXE
class module_dll_info {
public:
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
	// dll handle
  HMODULE module_handle;
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
	// for linux / apple: pointer to dlopen handle
	// for mobile devices: pointer to a vsx_string by which to create the module
	void* module_handle;
#endif
  int module_id;
  bool hidden_from_gui;
};  
//-
//#endif

#endif /*VSX_MODULE_DLL_INFO_H_*/
