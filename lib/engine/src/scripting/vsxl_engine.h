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

#ifndef VSXL_ENGINE_H_
#define VSXL_ENGINE_H_

class vsxl_engine {
public:
	gmMachine machine;
	gmVariable vsx_vtime;
	gmVariable vsx_dtime;
	int pf_id; // counter to make unique parameter filter functions
	int cf_id;
	vsxl_engine() : pf_id(0), cf_id(0)
	{}
	void init() {
    //machine = new gmMachine;
  	printf("float::load2\n");
    //GameObject::s_typeId = machine->CreateUserType("GameObject");
  	printf("float::load2\n");
    //machine->RegisterUserCallbacks(GameObject::s_typeId,GCTrace, GCDestruct,AsString);
  	printf("float::load2\n");
  	gmBindMathLib(&machine);
    printf("float::load3\n");		
	}
};

#endif /*VSXL_ENGINE_H_*/
