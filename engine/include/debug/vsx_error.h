#ifndef VSX_ERROR_H
#define VSX_ERROR_H

#include "vsx_backtrace.h"

// Error macro
#define ERROR_RETURN(s) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_backtrace::print_backtrace();\
  return; \
}

#define ERROR_RETURN_S(s,s2) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s, %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s, s2); \
  vsx_backtrace::print_backtrace();\
  return; \
}

#define ERROR_CONTINUE(s) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_backtrace::print_backtrace();\
  continue; \
}


#define ERROR_EXIT(s,i) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_printf("Exiting with error id %d\n", i);\
  vsx_backtrace::print_backtrace();\
  exit(i); \
}

#define ERROR_RETURN_V(s,iret) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_backtrace::print_backtrace();\
  return iret; \
}

#define ERROR_INFO(s) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("INFO in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_backtrace::print_backtrace();\
}

#endif
