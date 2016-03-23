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

#include <filesystem/vsx_filesystem.h>
#include "binds/gmCall.h"
#include "gm/gmThread.h"    // Game monkey script machine and thread
//#include "gm/gmCallScript.h" // Call script from C helper
//#include "gm/gmMathLib.h" // Call script from C helper

#include "vsx_param.h"

//#include <iostream>
#include "vsx_command.h"
#include "vsx_math_3d.h"
#include <module/vsx_module.h>
#include "vsx_param_interpolation.h"
#include "vsx_comp_abs.h"
#include "vsx_param_abstraction.h"
#include "vsx_param_sequence.h"
#include "vsx_param_sequence_list.h"
#include "vsx_sequence_pool.h"

#include <time/vsx_timer.h>
#include "vsx_engine.h"
#include "vsx_comp_vsxl.h"

class p_info {
public:
  vsx_module_param_abs* param;
  gmVariable variable;
  vsx_string<>name;
  unsigned long id;
};

class vsx_comp_vsxl_driver : public vsx_comp_vsxl_driver_abs {
  gmVariable vsx_vtime;
  gmVariable vsx_dtime;
public:
  std::vector<p_info*> p_list;
  gmMachine* machine;
  void *load(vsx_module_param_list* module_list,vsx_string<>program);
  void run();
  void unload();
  vsx_comp_vsxl_driver();
};

vsx_comp_vsxl_driver::vsx_comp_vsxl_driver() {
#ifndef VSXE_NO_GM
  machine = 0;
#endif
}

void vsx_comp_vsxl_driver::unload() {
#ifndef VSXE_NO_GM
  if (machine) {
    machine->ResetAndFreeMemory();
    delete machine;
  }
#endif
}


// load gaymonkay with new code
void *vsx_comp_vsxl_driver::load(vsx_module_param_list* module_list,vsx_string<>program)
{
#ifndef VSXE_NO_GM
  my_param_list = module_list;

  if (p_list.size() == 0) {
    // go through the module params and build gmvariables for them :)

    for (size_t i = 0;  i < module_list->id_vec.size(); ++i) {
      if (module_list->id_vec[i]->type == VSX_MODULE_PARAM_ID_FLOAT) {
        p_info* mp = new p_info;
        mp->param = module_list->id_vec[i];
        mp->name = module_list->id_vec[i]->name;
        mp->id = VSX_MODULE_PARAM_ID_FLOAT;
        p_list.push_back(mp);
      }
    }
  }

  if (!machine) {
    machine = new gmMachine;
    //gmBindMathLib(machine);
  } else {
    machine->ResetAndFreeMemory();
    delete machine;
    machine = new gmMachine;
    //gmBindMathLib(machine);
  }

  if (program == "") {
    script = "// available parameters:\n";
    for (std::vector<p_info*>::iterator it = p_list.begin(); it != p_list.end(); ++it) {
      script += "// "+(*it)->name+"\n";
    }
  script += "\n\
//global f = table(0.0);\n\
\n\
// vsx language component filter (acronyme is:vsxl_cf)\n\
global vsxl_cf = function()\n\
{\n\
  // set values like this:\n\
  //   global param_name = 10.0;\n\
};\n\
";
  } else {
    script = program;
  }
  // Compile and execute the script
  machine->ExecuteString(script.c_str(), 0, true);

#endif // no gm
}

void vsx_comp_vsxl_driver::run() {
#ifndef VSXE_NO_GM
  // Call a script function
  //printf("r");
  //std::cout << "realvalue" << ((vsx_engine*)engine)->vtime << std::endl;

  gmTableObject* gtable = machine->GetGlobals();
  //mytable.Set(machine, "fromstle", mt_var);
  for (std::vector<p_info*>::iterator it = p_list.begin(); it != p_list.end(); ++it) {
    (*it)->variable.SetFloat(((vsx_module_param_float*)(*it)->param)->get());
    gtable->Set(machine, (*it)->name.c_str(), (*it)->variable);
  }

  //gmVariable mt_var;
  //mt_var.SetFloat(0.2);
  vsx_vtime.SetFloat(((vsx_comp_abs*)comp)->r_engine_info->vtime);
  vsx_dtime.SetFloat(((vsx_comp_abs*)comp)->r_engine_info->dtime);
  machine->GetGlobals()->Set(machine, "_time", vsx_vtime);
  machine->GetGlobals()->Set(machine, "_dtime", vsx_dtime);
  //machine->GetGlobals()->Set(machine, "size_x", mt_var);
  gmCall call;

  machine->Execute(0);
  if(call.BeginGlobalFunction(machine, "vsxl_cf"))
  {
    //call.AddParamFloat(realvalue);
    //call.AddParamInt(valueB);
    call.End();
    //call.GetReturnedFloat(resultfloat);
  }

  //gmCallScript::SetMachine(machine);
  //if (gmCallScript::BeginGlobal("vsxl_cf"))
  //{
    //gmCallScript::Thread()->PushTable(&mytable);
    //gmCallScript::SetReturnTable(&newtable);
    //gmCallScript::SetReturnFloat(resultfloat);
    //gmCallScript::AddParamFloat(realvalue);

    //GameObject * object = new GameObject();
    //object->m_table = (void*)this;
    //gmCallScript::AddParamUser(object, GameObject::s_typeId);
    //object->m_userObject = gmCallScript::s_thread->PushNewUser(object, GameObject::s_typeId);
    //object->m_userObject = gmCallScript::s_thread->PushNewUser(object, GameObject::s_typeId);
    //gmCallScript::AddParamUser(object->m_userObject);
    //gmCallScript::AddParamUser((void*)this,GM_PVSXL);
    //gmCallScript::End();
  //}
  //((vsx_module_param_float*)my_param)->set_raw(resultfloat);

  gmVariable stringName;
  gmVariable retVar;

  //stringName.SetString(machine->AllocStringObject("size_x"));
  //retVar = machine->GetGlobals()->Get(stringName);

  for (std::vector<p_info*>::iterator it = p_list.begin(); it != p_list.end(); ++it) {
    stringName.SetString(machine->AllocStringObject((*it)->name.c_str()));
    //printf("var:: %f\n", gtable->Get(stringName).m_value.m_float);
    ((vsx_module_param_float*)(*it)->param)->set_raw(gtable->Get(stringName).m_value.m_float);
    //(*it)->variable.SetFloat(((vsx_module_param_float*)(*it)->param)->get());
    //gtable->Set(machine, (*it)->name, (*it)->variable);
  }


  //std::cout << "froo " << retVar.m_value.m_float << endl;
//  std::cout << "froo " << machine->GetGlobals()->Get(vsx_dtime).m_value.m_float << endl;
  //std::cout << "resultfloat: " << resultfloat << std::endl;
  machine->CollectGarbage(false);
#endif // no gm
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
vsx_comp_vsxl::vsx_comp_vsxl() {
  my_driver = 0;
}

/*void vsx_param_vsxl::unload() {
  my_driver->unload();
  delete my_driver;
} */
void vsx_comp_vsxl::unload() {
#ifndef VSXE_NO_GM
  if (my_driver)
  {
    my_driver->unload();
    delete my_driver;
    my_driver = 0;
  }
#endif
}

vsx_comp_vsxl::~vsx_comp_vsxl() {
  unload();
}

void* vsx_comp_vsxl::load(vsx_module_param_list* module_list, vsx_string<>program) {
#ifndef VSXE_NO_GM
  if (!my_driver) {
    my_driver = new vsx_comp_vsxl_driver;
  }
  my_driver->load(module_list, program);
  return my_driver;
#endif
}

vsx_comp_vsxl_driver_abs* vsx_comp_vsxl::get_driver() {
#ifndef VSXE_NO_GM
  return my_driver;
#endif
}

void vsx_comp_vsxl::execute() {
#ifndef VSXE_NO_GM
  my_driver->run();

#endif
}
