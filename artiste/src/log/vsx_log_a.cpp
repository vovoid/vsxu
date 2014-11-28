/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <string/vsx_string.h>
#ifdef __WIN32
#include <io.h>
#endif
#include "vsx_log_a.h"

int log_level = 1;
vsx_string<>log_dir;
int a_first = 1; 

void log_a(vsx_string<>message, int level)
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
