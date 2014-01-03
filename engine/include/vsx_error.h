// Error macro
#define ERROR_RETURN(s) \
{ \
  vsx_printf("ERROR in %s on %d:    %s\n", __FILE__, __LINE__, s); \
  return; \
}

#define ERROR_RETURN_V(s,iret) \
{ \
  vsx_printf("ERROR in %s on %d:    %s\n", __FILE__, __LINE__, s); \
  return iret; \
}
