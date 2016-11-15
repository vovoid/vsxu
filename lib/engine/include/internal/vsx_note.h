/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include <math/vector/vsx_vector3_helper.h>

class vsx_note {
public:
	vsx_string<>text; // base64-encoded, we don't care to unpack it
  vsx_vector3<float> pos;
  vsx_vector3<float> size;
	vsx_string<>name;
	float font_size;

	bool set(vsx_command_s* c) {
		if (c->parts.size() == 6) {
			//        0       1    2  3     4 
			//  note_create name pos size text
			name = c->parts[1];
      pos = vsx_vector3_helper::from_string<float>(c->parts[2]);
      size = vsx_vector3_helper::from_string<float>(c->parts[3]);
			text = c->parts[4];
      font_size = vsx_string_helper::s2f(c->parts[5]);
			return true;
		}
		return false;
	}

	const vsx_string<>serialize() {
		//        0       1    2  3     4 
		//  note_create name pos size text
    return "note_create_ok "+name+" "+ vsx_vector3_helper::to_string(pos) +" "+vsx_vector3_helper::to_string(size) +" "+text+" "+vsx_string_helper::f2s(font_size);
	}
	vsx_string<>serialize_state() {
		//        0       1    2  3     4 
		//  note_create name pos size text
    return "note_create "+name+" "+ vsx_vector3_helper::to_string(pos) +" "+vsx_vector3_helper::to_string(size) +" " + text + " " + vsx_string_helper::f2s(font_size);
	}
};
