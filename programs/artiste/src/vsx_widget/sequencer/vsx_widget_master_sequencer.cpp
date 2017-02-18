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

#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_gl_global.h"
#include <texture/vsx_texture.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include <filesystem/vsx_filesystem.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
// local includes
#include "vsx_widget.h"
#include "vsx_widget_sequence_editor.h"
#include "vsx_widget_sequence_channel.h"
#include "vsx_widget_timeline.h"
#include "widgets/vsx_widget_base_edit.h"

#define VSX_ENGINE_STOPPED 0
#define VSX_ENGINE_PLAYING 1
#define VSX_ENGINE_REWIND 2
