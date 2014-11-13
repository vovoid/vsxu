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

#ifndef VSX_LOG_A_H_
#define VSX_LOG_A_H_

// logging
extern int log_level;
extern vsx_string<>log_dir;
void log_a(vsx_string<>message, int level = 1);
#ifdef VSXU_DEBUG
#define LOG_A(mess) log_a(mess);
#define LOG2_A(mess, lvl) 
#else
#define LOG_A(mess)
#define LOG2_A(mess, lvl)
#endif
//log_a(mess, lvl);

  

#endif /*VSX_LOG_A_H_*/
