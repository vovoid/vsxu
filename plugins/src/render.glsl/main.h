#ifndef _VSXU_GLSL_
#define _VSXU_GLSL_

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */
//int explode(vsx_string& input, vsx_string& delimiter, vsx_avector<vsx_string>& results, int max_parts = 0);
//vsx_string str_replace(vsx_string search, vsx_string replace, vsx_string subject, int max_replacements = 0, int required_pos = -1);
#include <map>
#include "vsx_glsl.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
#if BUILDING_DLL
extern "C" {
_declspec(dllexport) vsx_module* create_new_module(unsigned long module);
_declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
_declspec(dllexport) unsigned long get_num_modules();
_declspec(dllexport) void set_environment_info(vsx_engine_environment* environment);
}
#endif

#endif /* _DLL_H_ */
