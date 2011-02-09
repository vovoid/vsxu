#ifndef _VSX_MODULE_SOUND_H_
#define _VSX_MODULE_SOUND_H_

#if BUILDING_DLL
extern "C" {
_declspec(dllexport) vsx_module* create_new_module(unsigned long module);
_declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
_declspec(dllexport) unsigned long get_num_modules();
void on_unload_library();
}
#endif

#endif /* _DLL_H_ */
