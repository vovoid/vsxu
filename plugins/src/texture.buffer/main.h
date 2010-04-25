#ifndef VSX_MODULE_TEXTURE_H
#define VSX_MODULE_TEXTURE_H


extern "C" {
_declspec(dllexport) vsx_module* create_new_module(unsigned long module);
_declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
_declspec(dllexport) unsigned long get_num_modules();
}

#endif

