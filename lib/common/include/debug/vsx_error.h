#ifndef VSX_ERROR_H
#define VSX_ERROR_H

#include <vsx_platform.h>
#include "vsx_backtrace.h"
#include "vsx_memory.h"

// Error macro
#define VSX_ERROR_RETURN(s) \
{ \
  vsx_printf(L"**********************************************************************************\n");\
  vsx_printf(L"ERROR in %s#%d, %s:    %hs\n", WIDE1(__FILE__), __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_backtrace::print_backtrace();\
  return; \
}

#define VSX_ERROR_RETURN_S(s,s2) \
{ \
  vsx_printf(L"**********************************************************************************\n");\
  vsx_printf(L"ERROR in %s#%d, %s:    %s, %hs\n", WIDE1(__FILE__), __LINE__, __PRETTY_FUNCTION__, s, s2); \
  vsx_backtrace::print_backtrace();\
  return; \
}

#define VSX_ERROR_CONTINUE(s) \
{ \
  vsx_printf(L"**********************************************************************************\n");\
  vsx_printf(L"ERROR in %s#%d, %s:    %hs\n", WIDE1(__FILE__), __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_backtrace::print_backtrace();\
  continue; \
}


#define VSX_ERROR_EXIT(s,i) \
{ \
  vsx_printf(L"**********************************************************************************\n");\
  vsx_printf(L"ERROR in %s#%d, %s:    %hs\n", WIDE1(__FILE__), __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_printf(L"Exiting with error id %d\n", i);\
  vsx_backtrace::print_backtrace();\
  exit(i); \
}

#define VSX_ERROR_RETURN_V(s,iret) \
{ \
  vsx_printf(L"**********************************************************************************\n");\
  vsx_printf(L"ERROR in %s#%d, %s:    %hs\n", WIDE1(__FILE__), __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_backtrace::print_backtrace();\
  return iret; \
}

#define VSX_ERROR_INFO(s) \
{ \
  vsx_printf(L"**********************************************************************************\n");\
  vsx_printf(L"INFO in %s#%d, %s:    %s\n", WIDE1(__FILE__), __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_backtrace::print_backtrace();\
}

#define VSX_MEMORY_INFO(s) \
{ \
  vsx_printf(L"**********************************************************************************\n");\
  vsx_printf(L"INFO in %s#%d, %s:    %s\n", WIDE1(__FILE__), __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_memory::print_memory_info(); \
}

#endif
