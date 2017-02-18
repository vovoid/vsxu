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

//#ifndef VSXE_NO_GM
#include <filesystem/vsx_filesystem.h>
#include "gm/gmMachine.h"
#include "binds/gmCall.h"
#include "binds/gmMathLib.h"
#include "gm/gmVariable.h"
//#include "gm/gmThread.h"    // Game monkey script machine and thread
//#include "gm/gmCall.h" // Call script from C helper
//#include "gm/gmCallScript.h" // Call script from C helper
#include "binds/gmMathLib.h" // Call script from C helper
//#endif
//#include <iostream>
#include <map>
#include "vsx_param.h"
#include <time/vsx_timer.h>
#include "vsx_command.h"
#include "vsx_math_3d.h"
#include <module/vsx_module.h>
#include "vsx_param_interpolation.h"
#include "vsx_comp_abs.h"
#include "vsx_param_abstraction.h"
#include "vsx_param_sequence.h"
#include "vsx_param_sequence_list.h"
#include "vsx_sequence_pool.h"

#include "vsxl_engine.h"
#include "vsx_engine.h"
#include "vsx_param_vsxl.h"


//int func_id = 0;

class vsx_param_vsxl_driver_float : public vsx_param_vsxl_driver_abs {
  float realvalue, resultfloat;
//	gmMachine* machine;
//	gmVariable vsx_vtime;
//	gmVariable vsx_dtime;
public:
  void *load(vsx_module_param_abs* engine_param, vsx_string<>program);
  void run();
  void unload();
	vsx_param_vsxl_driver_float() {
	  //machine = 0;
	  id = -1;
	}
};

//------------------------------------------------------------------------------
//--- V S X L   D R I V E R   A B S --------------------------------------------
//------------------------------------------------------------------------------
gmType GM_PVSXL;

struct GameObject
{
  void* m_table; // Contain the table functionality
  gmUserObject * m_userObject;
  static gmType s_typeId; // Store our user type
};

gmType GameObject::s_typeId = GM_NULL;

/*static bool GM_CDECL GCTrace(gmMachine * a_machine, gmUserObject* a_object, gmGarbageCollector* a_gc, const int a_workLeftToDo, int& a_workDone)
{
#ifndef VSXE_NO_GM
//  printf("gctrace\n");
  GM_ASSERT(a_object->m_userType == GameObject::s_typeId);
  GameObject * object = (GameObject *) a_object->m_user;
  //if(object->m_table) a_gc->GetNextObject(object->m_table);
  a_workDone + 2; // contents + this
#endif
  return true;
}

static void GM_CDECL GCDestruct(gmMachine * a_machine, gmUserObject* a_object)
{
#ifndef VSXE_NO_GM
//  printf("gcdestruct\n");
  GM_ASSERT(a_object->m_userType == GameObject::s_typeId);
  GameObject * object = (GameObject *) a_object->m_user;
  object->m_table = NULL;
  delete object;
#endif
}
static void GM_CDECL AsString(gmUserObject * a_object, char* a_buffer, int a_bufferLen)
{
}
*/
//------------------------------------------------------------------------------



void vsx_param_vsxl_driver_float::unload() {
#ifndef VSXE_NO_GM
//  printf("unload\n");
  /*if (machine) {

    machine->ResetAndFreeMemory();
    delete machine;
  } */
#endif
}

// load gaymonkay with new code
void* vsx_param_vsxl_driver_float::load(vsx_module_param_abs* engine_param,vsx_string<>program) {
#ifndef VSXE_NO_GM
  //printf("float::load1\n");
  my_param = engine_param;

   /*else {
  	printf("float::load4\n");

    machine->ResetAndFreeMemory();
    delete machine;
  	printf("float::load5\n");

    machine = new gmMachine;
    GameObject::s_typeId = machine->CreateUserType("GameObject");
  	printf("float::load6\n");
    machine->RegisterUserCallbacks(GameObject::s_typeId,GCTrace, GCDestruct,AsString);
    gmBindMathLib(machine);
  	printf("float::load7\n");
  }*/
  //gmBindMathLib(machine);
  if (id == -1) {
  	id = engine->vsxl->pf_id;
  	engine->vsxl->pf_id++;
  }
  printf("float::load8\n");
  if (program == "") { //engine_param->name
  script = "// this script modifies the float param called \""+engine_param->name+"\"\n\
// it's generated and maintained by the engine.\n\
// you have to put your code in the vsxl_pf function.\n\
// when saving, your program will be sent to the engine, compiled and implemented in realtime.\n\
//\n\
// this is an example variable that stays put until next frame - useful for doing interpolation\n\
//global f = table(0.0);\n\
\n\
// vsx language parameter filter (acronyme is:vsxl_pf)\n\
global vsxl_pf"+vsx_string_helper::i2s(id)+" = function(param)\n\
{\n\
  // param is the param ready to use in code, i.e. param += 1;\n\
  // param_abs can be used as parameter to the vsxu parameter functions - stuff that you ask vsxu to do.\n\
  // a float doesn't have much of this, but it's still there to follow standard :)  \n\
//  f[0] = f[0]+0.01;\n\
//  if (param < 0.0) {\n\
//    return 0.0;\n\
//  } else {\n\
    return param;\n\
//  }\n\
};\n\
";
  } else {
  	printf("load_8b\n");
    script = program;
  }
  // Compile and execute the script
  //MessageBox(0, "pre-execute", "status", MB_OK);
  printf("load_9\n");
  engine->vsxl->machine.ExecuteString(script.c_str(), NULL, false, NULL);
  printf("load_a\n");
  //MessageBox(0, "post-execute", "status", MB_OK);
  return this;
#endif
}

void vsx_param_vsxl_driver_float::run() {
#ifndef VSXE_NO_GM
  // Call a script function
  realvalue = ((vsx_module_param_float*)my_param)->get_internal();
  //std::cout << "realvalue" << ((vsx_engine*)engine)->vtime << std::endl;

  //gmTableObject mytable;
  //gmVariable mt_var;
  //mt_var.SetFloat(0.2);
  //mytable.Set(machine, "fromstle", mt_var);

  //vsx_vtime.SetFloat(0.12);
  engine->vsxl->vsx_vtime.SetFloat(((vsx_comp_abs*)comp)->r_engine_info->vtime);
  engine->vsxl->vsx_dtime.SetFloat(((vsx_comp_abs*)comp)->r_engine_info->dtime);
  engine->vsxl->machine.GetGlobals()->Set(&(engine->vsxl->machine), "_time", engine->vsxl->vsx_vtime);
  engine->vsxl->machine.GetGlobals()->Set(&(engine->vsxl->machine), "_dtime", engine->vsxl->vsx_dtime);

  gmCall call;

	engine->vsxl->machine.Execute(0);
	if(call.BeginGlobalFunction(&(engine->vsxl->machine), ("vsxl_pf"+vsx_string_helper::i2s(id)).c_str()))
  {
    call.AddParamFloat(realvalue);
    //call.AddParamInt(valueB);
    call.End();
    call.GetReturnedFloat(resultfloat);
  }

  /*gmCallScript::SetMachine(machine);
  if (gmCallScript::BeginGlobal(("vsxl_pf"+vsx_string_helper::i2s(id)).c_str()))
  {
    gmCallScript::SetReturnFloat(resultfloat);
    gmCallScript::AddParamFloat(realvalue);

    //GameObject * object = new GameObject();
    //object->m_table = (void*)this;
    //gmCallScript::AddParamUser(object, GameObject::s_typeId);
    //object->m_userObject = gmCallScript::s_thread->PushNewUser(object, GameObject::s_typeId);
    //object->m_userObject = gmCallScript::s_thread->PushNewUser(object, GameObject::s_typeId);
    //gmCallScript::AddParamUser(object->m_userObject);
    //gmCallScript::AddParamUser((void*)this,GM_PVSXL);
    gmCallScript::End();
  }*/
  ((vsx_module_param_float*)my_param)->set_raw(resultfloat);

  //gmVariable stringName;
  //gmVariable retVar;

  //stringName.SetString(machine->AllocStringObject("_dtime"));
  //retVar = machine->GetGlobals()->Get(stringName);

  //std::cout << "froo " << retVar.m_value.m_float << endl;
//  std::cout << "froo " << machine->GetGlobals()->Get(vsx_dtime).m_value.m_float << endl;
  //std::cout << "resultfloat: " << resultfloat << std::endl;
  engine->vsxl->machine.CollectGarbage();
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
vsx_param_vsxl::vsx_param_vsxl() {
  my_driver = 0;
}

void vsx_param_vsxl::unload() {
#ifndef VSXE_NO_GM
  my_driver->unload();
  delete my_driver;
#endif
}


void* vsx_param_vsxl::load(vsx_module_param_abs* engine_param, vsx_string<>program, int id) {
#ifndef VSXE_NO_GM
//printf("load1\n");
  if (!my_driver) {
  	//printf("load2\n");
    if (engine_param->type == VSX_MODULE_PARAM_ID_FLOAT) {
    	//printf("load3\n");
      my_driver = new vsx_param_vsxl_driver_float;
      my_driver->engine = engine;
      //printf("load4\n");
    }
  }

  if (!engine->vsxl) {
  	engine->vsxl = new vsxl_engine;
  	engine->vsxl->init();
  }

  if (id != -1) {
  	my_driver->id = id;
  	engine->vsxl->pf_id++;
  }
  my_driver->load(engine_param, program);
  return my_driver;
#endif
}

vsx_param_vsxl_driver_abs* vsx_param_vsxl::get_driver() {
  return my_driver;
}

void vsx_param_vsxl::execute() {
#ifndef VSXE_NO_GM
  my_driver->run();
#endif
}
