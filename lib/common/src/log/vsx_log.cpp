#include <string/vsx_string.h>
#include <vsx_platform.h>
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    #include <io.h>
#else
  // unix or other
  #include <syslog.h>
#endif
#include <log/vsx_log.h>

//int log_level = 1;
//vsx_string<>log_dir;
//int first = 1;

void log(vsx_string<>message, int level)
{
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    openlog ("vsx_engine", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog(LOG_ERR,"%d: %s", level, message.c_str());
  #else
    printf("LOG: %s\n", message.c_str() );
    /*
    if (log_level < level) return;
    printf("%s\n",message.c_str());
    FILE* fpo = 0;
    if (first) {
      fpo = fopen((log_dir+"vsx_engine.debug.log").c_str(),"w");
      first = 0;
    } else
      fpo = fopen((log_dir+"vsx_engine.debug.log").c_str(),"a");
    if (!fpo) return;
    fprintf(fpo, "%s\n", message.c_str() );
    fclose(fpo);*/
  #endif
}
