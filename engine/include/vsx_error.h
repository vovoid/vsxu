// Error macro
#define ERROR_RETURN(s) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  return; \
}

#define ERROR_CONTINUE(s) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  continue; \
}


#define ERROR_EXIT(s,i) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  vsx_printf("Exiting with error id %d\n", i);\
  exit(i); \
}

#define ERROR_RETURN_V(s,iret) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("ERROR in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
  return iret; \
}

#define ERROR_INFO(s) \
{ \
  vsx_printf("**********************************************************************************\n");\
  vsx_printf("INFO in %s#%d, %s:    %s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, s); \
}
