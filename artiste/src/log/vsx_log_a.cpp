#include "vsx_string.h"
#ifdef __WIN32
#include <io.h>
#endif
#include "vsx_log_a.h"

int log_level = 1;
vsx_string log_dir;
int a_first = 1; 

void log_a(vsx_string message, int level)
{
	if (log_level < level) return;
	FILE* fpo = 0;
	if (a_first) {
		fpo = fopen((log_dir+"vsxu_artiste.debug.log").c_str(),"w");
		a_first = 0;
	} else
		fpo = fopen((log_dir+"vsxu_artiste.debug.log").c_str(),"a");
	if (!fpo) return;
	fprintf(fpo, "%s\n", message.c_str() );
	fclose(fpo);	
}
