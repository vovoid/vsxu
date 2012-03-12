/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
//#include "vsx_string_lib.h"
#include "vsx_avector.h"
#include "vsxfst.h"

class vsx_module_file_chooser : public vsx_module {
  // in
	vsx_module_param_resource* directory_path;
	vsx_module_param_float* file_id;
	// out
	vsx_module_param_resource* filename_result;
	vsx_module_param_float* filename_count;
	// internal
  int p_updates;
  vsx_avector<vsx_string> files_list;
public:
  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "directory_path:resource?default_controller=controller_edit,file_id:float";
    info->identifier = "system;filesystem;file_chooser";
    info->out_param_spec = "filename_result:resource,filename_count:float";
    info->component_class = "system";
    info->description = "chooses either of the files in a directory\nspecified by path";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    directory_path = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"directory_path");
    directory_path->set("resources");
    
    filename_result = (vsx_module_param_resource*)out_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename_result");
    filename_result->set("");
    filename_count = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"filename_count");
    filename_count->set(0.0f);
    file_id = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"file_id");
    file_id->set(0.0f);
    p_updates = 0;
    loading_done = true;
  }
  vsx_string old_path;
  void run() {
    if (p_updates != param_updates) {
      p_updates = param_updates;
      if (directory_path->get() != "") {
        if (old_path != directory_path->get()) {
          old_path = directory_path->get();
          std::list<vsx_string> files;
          //vsxfst tt;
          get_files_recursive(directory_path->get(), &files);

          files_list.reset_used(0);
          for (std::list<vsx_string>::iterator it = files.begin(); it != files.end(); ++it) {
            if ((*it).find(".svn/") == -1)
            files_list.push_back(*it);
          }
        }
      }
      if (files_list.size()) {
      	filename_count->set((float)files_list.size());
        unsigned long fid = (unsigned long)floor(file_id->get());
        if (fid >= files_list.size()) fid = files_list.size()-1;
        filename_result->set(files_list[fid]);
      }
    }
  }
};


class vsx_module_resource_to_string : public vsx_module {
  // in
	vsx_module_param_resource* resource_in;
	// out
	vsx_module_param_string* string_out;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "string;converters;res_to_str||string;res_to_str";
    info->in_param_spec = "resource_in:resource";
    info->out_param_spec = "string_out:string";
    info->component_class = "system";
    info->description = "converts a resource string to a normal string";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    resource_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"resource_in");
    resource_in->set("");
    string_out = (vsx_module_param_string*)out_parameters.create(VSX_MODULE_PARAM_ID_STRING,"string_out");
    string_out->set("");
    loading_done = true;
  }
  void run() {
  	string_out->set(resource_in->get());
  }
};



//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_file_chooser);
    case 1: return (vsx_module*)(new vsx_module_resource_to_string);
  }
  return 0;  
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_file_chooser*)m; break;
    case 1: delete (vsx_module_resource_to_string*)m; break;
  }
}

unsigned long get_num_modules() {
  return 2;
}  

