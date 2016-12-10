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

#include "vsx_gl_global.h"

#include <map>
#include <list>
#include <vector>
#include <math.h>
#include <command/vsx_command.h>
#include <command/vsx_command_list.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include <filesystem/vsx_filesystem.h>
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <texture/vsx_texture.h>

// local includes
#include "vsx_widget_comp.h"
#include "vsx_widget_anchor.h"
#include "vsx_widget_server.h"
#include "controllers/vsx_widget_controller_base.h"
#include "controllers/vsx_widget_controller_editor.h"
#include "widgets/vsx_widget_popup_menu.h"
#include "vsx_data_path.h"
#include <gl_helper.h>

// widget
#include <dialogs/dialog_query_string.h>
#include <dialogs/dialog_messagebox.h>


bool vsx_widget_component::show_titles = true;
bool vsx_widget_component::ethereal_all = false;

int vsx_widget_component::inside_xy_l(vsx_vector3<> &test, vsx_vector3<> &global) {
  if (ethereal || (ethereal_all && !macro)) return 0;
  return vsx_widget::inside_xy_l(test, global);
}

void vsx_widget_component::command_process_back_queue(vsx_command_s *t)
{
  if (t->cmd == "pseq_p_ok")
  {
    if (p_l_list_in.find(t->parts[3]) != p_l_list_in.end())
    {
      command_q_b.add(t);
      p_l_list_in[t->parts[3]]->vsx_command_queue_b(this);
    } else server->command_q_f.addc_front(t);
    return;
  } else
  if (
    t->cmd == "vsxl_pfr_ok" ||
    t->cmd == "param_connect_ok" ||
    t->cmd == "param_disconnect_ok" ||
    t->cmd == "connections_order_ok" ||
    t->cmd == "vsxl_pfl_s"
  ) {
    if (p_l_list_in.find(t->parts[2]) != p_l_list_in.end())
    {
      command_q_b.add(t);
      p_l_list_in[t->parts[2]]->vsx_command_queue_b(this);
    }
    return;
  } else
  if (t->cmd == "vsxl_pfi_ok") {
    if (p_l_list_in.find(t->parts[2]) != p_l_list_in.end())
    {
      command_q_b.add(t);
      p_l_list_in[t->parts[2]]->vsx_command_queue_b(this);
    } else
    {
      server->command_q_f.addc(t);
    }
    return;
  } else
  if (t->cmd == "pca") {
    if (t->parts[6] == "-1")
    {
      if (p_l_list_in.find(t->parts[2]) != p_l_list_in.end())
      {
        command_q_b.add(t);
        p_l_list_in[t->parts[2]]->vsx_command_queue_b(this);
      }
    } else {
      if (p_l_list_out.find(t->parts[2]) != p_l_list_out.end())
      {
        command_q_b.add(t);
        p_l_list_out[t->parts[2]]->vsx_command_queue_b(this);
      }
    }
  } else
  if (t->cmd == "c_msg")
  {
    vsx_string<>s = vsx_string_helper::base64_decode(t->parts[2]);
    vsx_string<>deli = "&&";

    deli = "||";
    vsx_nw_vector <vsx_string<> > parts;
    vsx_string_helper::explode(s,deli,parts);
    if (parts[0] == "module")
    {
      if (parts[1] == "ok")
      {
        message = "";
      } else {
        message = parts[1];
        message_time = 6.0f;
      }
    } else
    {
    }
    return;
  } else
  if (t->cmd == "add_empty_macro")
  {
    command_q_b.add_raw("macro_create macros;empty "+name+"."+"empty"+" "+vsx_string_helper::str_replace<char>(","," ",t->cmd_data)+" 0.1");
    server->vsx_command_queue_b(this);
    return;
  } else
  if (t->cmd == "macro_dump")
  {
    command_q_b.add_raw("macro_dump "+name+" "+t->parts[1]);
    server->vsx_command_queue_b(this);
    return;
  } else
  if (t->cmd == "macro_dump_add")
  {
    printf("macro_dump_add %s\n", vsx_string_helper::base64_decode(t->parts[2]).c_str());
    macro_commands.add_raw( vsx_string_helper::base64_decode(t->parts[2]));
    return;
  } else
  if (t->cmd == "macro_dump_complete")
  {
    printf("macro dump complete\n");
    vsx_string<> macro_commands_string = macro_commands.get_as_string();
    vsx_string_helper::write_to_file(
        vsx_data_path::get_instance()->data_path_get() + "macros/"+t->parts[2],
        macro_commands_string
      );
    macro_commands.clear_delete();
    command_q_b.add_raw("module_info_add macros;"+t->parts[2]);
    server->vsx_command_queue_b(this);
    return;
  } else
  if (t->cmd == "component_clone_add") {
    printf("component_clone_add %s\n", vsx_string_helper::base64_decode(t->parts[2]).c_str());
    macro_commands.add_raw( vsx_string_helper::base64_decode(t->parts[2]));
    return;
  } else
  if (t->cmd == "component_clone_complete")
  {
    printf("component clone complete %s\n",t->raw.c_str());
    macro_commands.parse();

    macro_commands.token_replace("$$name",((vsx_widget_server*)server)->get_unique_name(t->parts[1]));
    vsx_command_s* mc = 0;
    macro_commands.reset();
    bool is_part_of_macro = false;
    while ((mc = macro_commands.get()))
    {
      if (mc->cmd == "macro_create" && mc->parts.size() < 5)
      {
        is_part_of_macro = true; // the following components are part of a macro
        command_q_b.add_raw("macro_create_real "+mc->parts[1]+" "+t->parts[3]+" "+t->parts[4]+" "+mc->parts[2]);
      } else
      if (mc->cmd == "macro_create" && mc->parts.size() < 6)
      {
        command_q_b.add_raw("macro_create_real "+mc->parts[1]+" "+mc->parts[2]+" "+mc->parts[3]+" "+mc->parts[4]);
      }
      else if (mc->cmd == "component_create")
        {
          if (is_part_of_macro)
            // child of macro, should retain its position
            command_q_b.add_raw("component_create "+mc->parts[1]+" "+mc->parts[2]+" "+mc->parts[3]+" "+mc->parts[4]);
          else
            // move it to the new position, it's just a module being cloned
            command_q_b.add_raw("component_create "+mc->parts[1]+" "+mc->parts[2]+" "+t->parts[3]+" "+t->parts[4]);
          //
        }
      else
        {
          if (mc->cmd != "")
          command_q_b.addc(mc);
        }
    }
    server->vsx_command_queue_b(this);
    macro_commands.clear_delete();
    return;
  } else
  if (t->cmd == "macro_saveas")
  {
    ((dialog_query_string*)name_dialog)->show("");
    return;
  } else
  if (t->cmd == "macro_toggle")
  {
    macro_toggle();
  } else
  if (t->cmd == "component_delete_menu")
  {
    if (component_type != "screen")
    {
      command_q_b.add_raw("component_delete "+name);
      server->vsx_command_queue_b(this);

      if (((vsx_widget_server*)(server))->selected_list.size())
      for (std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin(); itx != ((vsx_widget_server*)server)->selected_list.end(); ++itx) {
        if ((*itx) != this)
        if (!((vsx_widget_component*)(*itx))->internal_critical) {
          command_q_b.add_raw("component_delete "+(*itx)->name);
          server->vsx_command_queue_b(this);
        }
      }
    }
    return;
  } else
  if (t->cmd == "in_param_spec" || t->cmd == "out_param_spec" || t->cmd == "ipsa" || t->cmd == "opsa")
  {
    bool fix_anchors = false;
    if (t->parts.size() < 3) return;
    int l_io = 0;
    if (t->cmd == "in_param_spec" || t->cmd == "ipsa") {
      l_io = -1;
      if (t->cmd != "ipsa")
      if (p_l_list_in.size()) {
        anchor_order[0] = 0;
        t_list = p_l_list_in;
        p_l_list_in.clear();
        fix_anchors = true;
      }
    }
    else
    {
      if (t->cmd != "opsa")
      if (p_l_list_out.size()) {
        anchor_order[1] = 0;
        t_list = p_l_list_out;
        p_l_list_out.clear();
        fix_anchors = true;
      }
      l_io = 1;
    }
    if (fix_anchors)
    {
      for (std::map<vsx_string<>, vsx_widget*>::iterator it = t_list.begin(); it != t_list.end(); it++)
      {
        if ( ((*it).second)->widget_type == VSX_WIDGET_TYPE_ANCHOR)
        {
          ((vsx_widget_anchor*)((*it).second))->anchor_order[0] = 0;
          ((vsx_widget_anchor*)((*it).second))->anchor_order[1] = 0;
        }
      }
    }

    // p[0]: in_param_spec
    // p[1]: osc2
    // p[2]: actual command
    vsx_nw_vector <vsx_string<> > add_c;
    /*
    complex:surround_sound_data[blob:center
    ,
    complex:rear_data[blob:Rleft
    ,
    blob:Rright]
    ,
    complex:front_data[blob:Fleft
    ,
    blob:Fright]]
    ,
    float:result2
    */

    // THIS CODE IS A MESS! :(
    vsx_string<>cd = t->parts[2];
    //printf("comp_param_spec_parsing %s----:::\n",cd.c_str());
    vsx_string<>cm = "";
    vsx_string<>cms = "";
    int state = 0;
    //float ypos = size.y/2;
    for (size_t i = 0; i < cd.size(); ++i)
    {
      if (state == 0 && cd[i] != ',' && cd[i] != '{')
      cm += cd[i];

      if (cd[i] == '{') { ++state; }

      if (state > 0)
      {
        // add to the command to be sent further in
        if ( (cd[i] != '{' && cd[i] != '}') || state > 1)
        cms += cd[i];
      }
      if (cd[i] == '}') { state--; }

      if ((cd[i] == ',' || i == cd.size()-1) && state == 0 ) {
        // we have our first part covered, and the command-sub
        // 1. find the name of this anchor
        add_c.clear();
        vsx_string<>deli = ":";
        vsx_string_helper::explode(cm, deli, add_c, -1);
        vsx_widget_anchor *anchor = 0;
        // see if there's an anchor we can use
        if (t_list.find(add_c[0]) != t_list.end()) {
          anchor = (vsx_widget_anchor*)t_list[add_c[0]];
          if (l_io < 0) {
            anchor->a_order = anchor_order[0]++;
            p_l_list_in[add_c[0]] = t_list[add_c[0]];
          }
          else
          {
            ((vsx_widget_anchor*)anchor)->a_order = anchor_order[1]++;
            p_l_list_out[add_c[0]] = t_list[add_c[0]];
          }
          t_list.erase(add_c[0]);
        } else
        {
          // no previous anchor found, create a new one
          anchor = (vsx_widget_anchor*)add(new vsx_widget_anchor,add_c[0]);
          // extra type info split
          vsx_nw_vector<vsx_string<> > type_info;
          vsx_string<>type_deli = "?";
          vsx_string_helper::explode(add_c[1],type_deli,type_info);
          if (type_info.size() == 2)
          {
            ((vsx_widget_anchor*)anchor)->p_type_suffix = type_info[1];
          }
          ((vsx_widget_anchor*)anchor)->p_type = type_info[0];
          ((vsx_widget_anchor*)anchor)->p_desc = cm;
          ((vsx_widget_anchor*)anchor)->io = l_io;
          ((vsx_widget_anchor*)anchor)->anchor_order[0] = 0;
          ((vsx_widget_anchor*)anchor)->anchor_order[1] = 0;
          if (t->parts.size() == 4) if (t->parts[3] == "1") {
            ((vsx_widget_anchor*)anchor)->alias = true;
          }

          ((vsx_widget_anchor*)anchor)->component = this;
          if (add_c[0] != "complex") {
            if (l_io < 0) {
              ((vsx_widget_component*)((vsx_widget_anchor*)anchor)->component)->p_l_list_in[add_c[0]] = anchor;
            }
            else
            {
              ((vsx_widget_component*)((vsx_widget_anchor*)anchor)->component)->p_l_list_out[add_c[0]] = anchor;
            }
          }

          if (l_io < 0)
          {
            ((vsx_widget_anchor*)anchor)->a_order = anchor_order[0]++;
          }
          else
          {
            ((vsx_widget_anchor*)anchor)->a_order = anchor_order[1]++;
          }

          anchor->size.x = anchor->size.y = 0.0f;
          anchor->target_size.x = anchor->target_size.y = 0.05/6;
          anchor->interpolating_size = true;
        }

        if (cms.size()) {
          //printf("cms = %s\n",cms.c_str());
          ((vsx_widget_anchor*)anchor)->p_def += add_c[1];
          ((vsx_widget_anchor*)anchor)->p_def += "[";
          if (t->parts.size() == 4) {
            if (t->parts[3] == "c") cms += " c";
          }
          command_q_b.add_raw(t->cmd+" "+add_c[0]+" "+cms, VSX_COMMAND_GARBAGE_COLLECT);
          anchor->vsx_command_queue_b(this);
          ((vsx_widget_anchor*)anchor)->p_def += "]";
        } else ((vsx_widget_anchor*)anchor)->p_def = add_c[1];
        cms = "";
        cm = "";
      }
    }
    for (std::map<vsx_string<>, vsx_widget*>::iterator it = t_list.begin(); it != t_list.end(); ++it)
    {
      (*it).second->_delete();
    }

    if (!macro)
    {
      // scale component according to anchor count
      int largest_num_anchors;
      if (anchor_order[0] > anchor_order[1]) largest_num_anchors = anchor_order[0];
      else
      largest_num_anchors = anchor_order[1];
      if (largest_num_anchors < 3) largest_num_anchors = 3;
      target_size.y = target_size.x = 0.05f * 0.45 / 2.0f * (float)(largest_num_anchors-1);
      interpolating_size = true;
      size.y = size.x = 0.0f;
    }

    init_children();
    return;
  }


  if (t->cmd == "module_operation_spec")
  {
    vsx_nw_vector< vsx_string<> > parts;
    vsx_string_helper::explode_single(t->parts[2], '#', parts);

    foreach (parts, i)
    {
      vsx_module_operation* operation = new vsx_module_operation;
      operation->unserialize( parts[i] );
      module_operations.push_back(operation);
    }

    if (module_operations.size())
      foreach (module_operations, i)
        menu->commands.adds(VSX_COMMAND_MENU, module_operations[i]->name, "module_operation_show_" + vsx_string_helper::i2s(i), "");

    if (module_operations.size())
      module_operations_dialog =
        add
        (
          new dialog_query_string(
              "Perform Operation",
              "Param 1|Param 2|Param 3|Param 4"
          ),
          "module_operation_perform_"
        );

    ((vsx_widget_popup_menu*)menu)->init_extra_commands();
    return;
  }

  if (!macro && t->cmd.substr(0,17 + 5) == "module_operation_show_")
  {
    size_t index = vsx_string_helper::s2i( t->cmd.substr(17 + 5 ,1) );
    vsx_module_operation* operation = module_operations[index];

    dynamic_cast<dialog_query_string*>(module_operations_dialog)->set_label(0, "(unused)");
    dynamic_cast<dialog_query_string*>(module_operations_dialog)->set_label(1, "(unused)");
    dynamic_cast<dialog_query_string*>(module_operations_dialog)->set_label(2, "(unused)");
    dynamic_cast<dialog_query_string*>(module_operations_dialog)->set_label(3, "(unused)");

    operation->param_1 = "";
    operation->param_2 = "";
    operation->param_3 = "";
    operation->param_4 = "";

    if (operation->param_1_name.size())
      dynamic_cast<dialog_query_string*>(module_operations_dialog)->set_label(0, operation->param_1_name);
    if (operation->param_2_name.size())
      dynamic_cast<dialog_query_string*>(module_operations_dialog)->set_label(1, operation->param_2_name);
    if (operation->param_3_name.size())
      dynamic_cast<dialog_query_string*>(module_operations_dialog)->set_label(2, operation->param_3_name);
    if (operation->param_4_name.size())
      dynamic_cast<dialog_query_string*>(module_operations_dialog)->set_label(3, operation->param_4_name);
    dynamic_cast<dialog_query_string*>(module_operations_dialog)->name = "module_operation_perform_"+vsx_string_helper::i2s(index);
    dynamic_cast<dialog_query_string*>(module_operations_dialog)->show();
    return;
  }

  if (t->cmd.substr(0, 25) == "module_operation_perform_" && t->parts.size() == 3)
  {
    size_t index = vsx_string_helper::s2i( t->cmd.substr(25 ,1) );
    vsx_string<> parameters = vsx_string_helper::base64_decode(t->parts[2]);
    vsx_nw_vector< vsx_string<> > parts;
    vsx_string_helper::explode_single(parameters, '|', parts);
    vsx_module_operation* operation = module_operations[index];
    operation->param_1 = t->parts[1];
    operation->param_2 = parts[0];
    operation->param_3 = parts[1];
    operation->param_4 = parts[2];

    command_q_b.add_raw("module_operation_perform "+name+" "+operation->serialize());
    server->vsx_command_queue_b(this);
    return;
  }

  if (t->cmd == "param_connect_volatile") {
    bool failed = false;
    vsx_widget_component* a = (vsx_widget_component *)((vsx_widget_server*)server)->find_component(t->parts[3]);
    if (a) {
      if (a->p_l_list_out.find(t->parts[4]) != a->p_l_list_out.end())
      {
        if (p_l_list_in.find(t->parts[2]) != p_l_list_in.end()) {
          // seems all we need is out there, make the connection!
          command_q_b.add_raw(
            "param_connect_ok "+
            name+" "+
            p_l_list_in[t->parts[2]]->name+" "+
            a->name+" "+
            a->p_l_list_out[t->parts[4]]->name+" "+
            t->parts[5],
            VSX_COMMAND_GARBAGE_COLLECT
          );
          //
          p_l_list_in[t->parts[2]]->vsx_command_queue_b(this);
        } else failed = true;
      } else failed = true;
    } else failed = true;

    if (failed) {
        server->command_q_f.addc_front(t);
    }
    return;
  } else
  // param connect volatile for aliases, purely internal stuff :3
  if (t->cmd == "pcva") {
    bool failed = false;
    vsx_widget_component* a = (vsx_widget_component *)((vsx_widget_server*)server)->find_component(t->parts[3]);
    if (a) {
      if (t->parts[6] == "-1") {
        if (a->p_l_list_in.find(t->parts[4]) != a->p_l_list_in.end())
        {
          if (p_l_list_in.find(t->parts[2]) != p_l_list_in.end()) {
            // seems all we need is out there, make the connection!
            command_q_b.add_raw(
              "pca "+
              name+" "+
              p_l_list_in[t->parts[2]]->name+" "+
              a->name+" "+
              a->p_l_list_in[t->parts[4]]->name+" "+
              t->parts[5]+" "+
              t->parts[6],
              VSX_COMMAND_GARBAGE_COLLECT
            );
            p_l_list_in[t->parts[2]]->vsx_command_queue_b(this);
          } else failed = true;
        } else failed = true;
      } else {
        if (a->p_l_list_out.find(t->parts[4]) != a->p_l_list_out.end())
        {
          if (p_l_list_out.find(t->parts[2]) != p_l_list_out.end()) {
            // seems all we need is out there, make the connection!
            command_q_b.add_raw(
              "pca "+
              name+" "+
              p_l_list_out[t->parts[2]]->name+" "+
              a->name+" "+
              a->p_l_list_out[t->parts[4]]->name+" "+
              t->parts[5]+" "+
              t->parts[6],
              VSX_COMMAND_GARBAGE_COLLECT
            );
            p_l_list_out[t->parts[2]]->vsx_command_queue_b(this);
          } else failed = true;
        } else failed = true;
      }
    } else failed = true;

    if (failed) {
      server->command_q_f.addc(t);
    }
    return;
  } else
  if (t->cmd == "vsxl_load_script") {
    // vsxl param filter load
    command_q_b.add_raw("vsxl_cfl "+name);
    server->vsx_command_queue_b(this);
  } else
  if (t->cmd == "vsxl_remove_script") {
    // vsxl param filter load
    command_q_b.add_raw("vsxl_cfr "+name);
    server->vsx_command_queue_b(this);
  } else
  if (t->cmd == "vsxl_cfi_ok") {
    vsxl_filter = true;
  } else
  if (t->cmd == "vsxl_cfr_ok") {
    vsxl_filter = false;
  } else
  if (t->cmd == "vsxl_cfl_s")
  {
    vsx_widget* tt = add(new vsx_widget_controller_editor,name+".edit");
    ((vsx_widget_controller_editor*)tt)->target_param = name;
    ((vsx_widget_controller_editor*)tt)->return_command = "ps64";
    ((vsx_widget_controller_editor*)tt)->return_component = this;
    tt->set_render_type(render_3d);
    tt->widget_type = VSX_WIDGET_TYPE_CONTROLLER;
    tt->set_font_size(0.002);
    tt->set_border(0.0005);
    tt->set_pos(vsx_vector3<>(0,-0.08f));

    ((vsx_widget_controller_editor*)tt)->return_command = "vsxl_cfi";
    ((vsx_widget_controller_editor*)tt)->return_component = this;
    ((vsx_widget_controller_editor*)tt)->load_text( vsx_string_helper::base64_decode(t->parts[2]));
  } else
  if (t->cmd == "vsxl_cfi") {
    command_q_b.add_raw("vsxl_cfi "+name+" "+t->parts[1]);
    server->vsx_command_queue_b(this);
  } else
  if (t->cmd == "component_info")
  {
    if (t->parts.size() >= 3) {
      vsx_nw_vector<vsx_string<> > parts;
      // critical:screen:small
      vsx_string<>deli = ":";
      vsx_string_helper::explode(t->parts[2], deli, parts);

      if (parts.size() > 1)
      for (unsigned long i = 1; i < parts.size(); ++i) {
        if (parts[i] == "critical")
        {
          internal_critical = true;
        }
        else
        {
          if (parts[i] == "small")
          {
            target_size.x = size.x *= 0.45;
            target_size.y = size.y *= 0.45;
          }
        }
      }

      help_text = "Module type: "+parts[0]+"\n"+help_text;

      component_type = parts[0];

      mtex = vsx_texture_loader::load(
        vsx_widget_skin::get_instance()->skin_path_get() + "component_types/"+component_type+".dds",
        vsx::filesystem::get_instance(),
        true, // threaded
        0,
        vsx_texture_gl::linear_interpolate_hint
      );

      mtex_overlay = 0x0;
      if (component_type == "macro")
        mtex_overlay = vsx_texture_loader::load(
          vsx_widget_skin::get_instance()->skin_path_get() +"component_types/"+component_type+"_overlay.png",
          vsx::filesystem::get_instance(),
          true, // threaded
          vsx_bitmap::flip_vertical_hint,
          vsx_texture_gl::linear_interpolate_hint | vsx_texture_gl::generate_mipmaps_hint
        );

      if (!internal_critical)
      {
        menu = add(new vsx_widget_popup_menu,".comp_menu");
        menu->size.x = 0.32;
        if (component_type != "macro") {
        menu->commands.adds(VSX_COMMAND_MENU, "add/edit VSXL filter script", "vsxl_load_script","");
        menu->commands.adds(VSX_COMMAND_MENU, "remove VSXL filter script", "vsxl_remove_script","");
        menu->commands.adds(VSX_COMMAND_MENU, "----------------------", "","");
        } else
        {
          macro = true;
          menu->commands.adds(VSX_COMMAND_MENU, "add module... [ left-double-click ]", "add_component","");
          menu->commands.adds(VSX_COMMAND_MENU, "add empty macro [alt+left-double-click]", "add_empty_macro","$mpos");
          menu->commands.adds(VSX_COMMAND_MENU, "save macro...","macro_saveas","");
          menu->commands.adds(VSX_COMMAND_MENU, "open/close [alt + right-click]", "macro_toggle","");
          support_scaling = true;
          name_dialog = add(new dialog_query_string("name of macro","ex: my_macros;my_macro1"),"macro_dump");
          menu->commands.adds(VSX_COMMAND_MENU, "----------------------", "","");
        }
        menu->commands.adds(VSX_COMMAND_MENU, "delete [ del ]", "component_delete_menu","");

      }
      if (module_operations.size())
        foreach (module_operations, i)
          menu->commands.adds(VSX_COMMAND_MENU, module_operations[i]->name, "module_operation_"+vsx_string_helper::i2s(i), "");


      if (menu)
        menu->init();

      title = real_name;
    }
    return;
  }
  command_q_b.add(t);
}

void vsx_widget_component::vsx_command_process_f()
{
  vsx_command_s *c = 0;
  while ( (c = command_q_f.pop()) )
  {
    if (c->cmd == "init")
      init_run = true;
  }
  if (macro)
  vsx_widget::vsx_command_queue_f();
}

void vsx_widget_component::init()
{
  if (init_run) return;
  vsxl_filter = false;
  internal_critical = false;
  open = false;
  macro_overlay_opacity = 0.5f;
  deleting = 0;
  is_moving = false;
  support_interpolation = true;
  color.r = 1.0f;
  color.g = 1.0f;
  color.b = 1.0f;
  color.a = 1.0f;
  ethereal = false;
  selected = false;
  not_movable = false;

  init_children();
  command_q_f.add_raw("init", true);
  support_scaling = false;
  anchor_order[0] = 0;
  anchor_order[1] = 0;
  widget_type = VSX_WIDGET_TYPE_COMPONENT;
  parent_name = "";
  init_run = true;
  macro = false;
  scaled = false;
  transform_state = 0;
  size_min.x = 0.1f;

  mtex_blob = vsx_texture_loader::load(
    vsx_widget_skin::get_instance()->skin_path_get() + "interface_extras/connection_blob.png",
    vsx::filesystem::get_instance(),
    true, // threaded
    vsx_bitmap::flip_vertical_hint,
    vsx_texture_gl::linear_interpolate_hint
  );
}

void vsx_widget_component::reinit()
{
  vsx_widget::reinit();
}

void vsx_widget_component::draw()
{
  if (deleting == 1 && size.x < 0.00001f)
  {
    perform_delete();
    deleting++;
    return;
  }

  if (!enabled) return;
  vsx_vector3<> pp = parent->get_pos_p();
  size.y = size.x;
  glPushMatrix();
  #define FF 1.2f
  #define FF2 1.8f
  #define FFDIV 0.24f


    const GLshort squareTexcoords[] = {
      0, 1,
      1, 1,
      0, 0,
      1, 0
    };

    const GLfloat squareVertices[] = {
      -FF,   -FF,
      FF,   -FF,
      -FF,   FF,
      FF,   FF,
    };

    const GLfloat grid_lines[] = {
      -FF, -FF+1.0f*FFDIV,
        0, -FF+1.0f*FFDIV,
        0, -FF+1.0f*FFDIV,
       FF, -FF+1.0f*FFDIV,

      -FF, -FF+2.0f*FFDIV,
        0, -FF+2.0f*FFDIV,
        0, -FF+2.0f*FFDIV,
       FF, -FF+2.0f*FFDIV,

      -FF, -FF+3.0f*FFDIV,
        0, -FF+3.0f*FFDIV,
        0, -FF+3.0f*FFDIV,
       FF, -FF+3.0f*FFDIV,

      -FF, -FF+4.0f*FFDIV,
        0, -FF+4.0f*FFDIV,
        0, -FF+4.0f*FFDIV,
       FF, -FF+4.0f*FFDIV,

      -FF, -FF+5.0f*FFDIV,
        0, -FF+5.0f*FFDIV,
        0, -FF+5.0f*FFDIV,
       FF, -FF+5.0f*FFDIV,

      -FF, -FF+6.0f*FFDIV,
        0, -FF+6.0f*FFDIV,
        0, -FF+6.0f*FFDIV,
       FF, -FF+6.0f*FFDIV,

      -FF, -FF+7.0f*FFDIV,
        0, -FF+7.0f*FFDIV,
        0, -FF+7.0f*FFDIV,
       FF, -FF+7.0f*FFDIV,

      -FF, -FF+8.0f*FFDIV,
        0, -FF+8.0f*FFDIV,
        0, -FF+8.0f*FFDIV,
       FF, -FF+8.0f*FFDIV,

      -FF, -FF+9.0f*FFDIV,
        0, -FF+9.0f*FFDIV,
        0, -FF+9.0f*FFDIV,
       FF, -FF+9.0f*FFDIV,

      -FF+1.0f*FFDIV, -FF,
      -FF+1.0f*FFDIV,   0,
      -FF+1.0f*FFDIV,   0,
      -FF+1.0f*FFDIV,  FF,

      -FF+2.0f*FFDIV, -FF,
      -FF+2.0f*FFDIV,   0,
      -FF+2.0f*FFDIV,   0,
      -FF+2.0f*FFDIV,  FF,

      -FF+3.0f*FFDIV, -FF,
      -FF+3.0f*FFDIV,   0,
      -FF+3.0f*FFDIV,   0,
      -FF+3.0f*FFDIV,  FF,

      -FF+4.0f*FFDIV, -FF,
      -FF+4.0f*FFDIV,   0,
      -FF+4.0f*FFDIV,   0,
      -FF+4.0f*FFDIV,  FF,

      -FF+5.0f*FFDIV, -FF,
      -FF+5.0f*FFDIV,   0,
      -FF+5.0f*FFDIV,   0,
      -FF+5.0f*FFDIV,  FF,

      -FF+6.0f*FFDIV, -FF,
      -FF+6.0f*FFDIV,   0,
      -FF+6.0f*FFDIV,   0,
      -FF+6.0f*FFDIV,  FF,

      -FF+7.0f*FFDIV, -FF,
      -FF+7.0f*FFDIV,   0,
      -FF+7.0f*FFDIV,   0,
      -FF+7.0f*FFDIV,  FF,

      -FF+8.0f*FFDIV, -FF,
      -FF+8.0f*FFDIV,   0,
      -FF+8.0f*FFDIV,   0,
      -FF+8.0f*FFDIV,  FF,

      -FF+9.0f*FFDIV, -FF,
      -FF+9.0f*FFDIV,   0,
      -FF+9.0f*FFDIV,   0,
      -FF+9.0f*FFDIV,  FF,
    };

    #define GRID_ALPHA 0.4f
    const GLfloat grid_colors[] = {
      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,
      // ----
      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

      1.0f,1.0f,1.0f,0.0f,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,GRID_ALPHA,
      1.0f,1.0f,1.0f,0.0f,

    };
    #undef GRID_ALPHA


    glVertexPointer(2, GL_FLOAT, 0, squareVertices);
    glTexCoordPointer(2, GL_SHORT, 0, squareTexcoords);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    if (ethereal)
    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glMatrixMode(GL_MODELVIEW);
      glPushMatrix();
        glColor4f(0.4,0.4,0.5,0.7);
        glTranslatef(real_pos.x+pp.x,real_pos.y+pp.y,real_pos.z);
        glScaled(size.x/2,size.y/2,1);
        mtex->bind();
          glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        mtex->_bind();
      glPopMatrix();
      glColor4f(1,1,1,0.3);
    } else
    {
      glColor4f(
        color.r,
        color.g,
        color.b,
        color.a
      );
    }
    if (macro)
    {
      glColor3f(color.r,color.g,color.b);
    }

    glTranslatef(pos.x+pp.x,pos.y+pp.y,pos.z);
    glScaled(size.x/2, size.y/2, 1);
    mtex->bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    mtex->_bind();
    if (macro)
    {
      glColor4f(1.0,1.0,1.0,macro_overlay_opacity);
      mtex_overlay->bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      mtex_overlay->_bind();
    }


    if (selected)
    {
      if (macro && open)
      {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glVertexPointer(2, GL_FLOAT, 0, grid_lines);
        glColorPointer(4, GL_FLOAT, 0, grid_colors);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glDrawArrays(GL_LINES, 0, 4 * 9 * 2);
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
      else
      {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glColor4f(1.0f, 1.0f, 1.0f,0.7f);
      mtex_blob->bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      mtex_blob->_bind();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      }
    }
    #undef FF
    #undef FF2
    glColor3f(0,1,1);
    if (vsxl_filter)
    {
      glLineWidth(4);
      glBegin(GL_LINE_STRIP);
        glVertex2f(-0.95,-0.95);
        glVertex2f(-0.95, 0.95);
        glVertex2f( 0.95, 0.95);
        glVertex2f( 0.95,-0.95);
        glVertex2f(-0.95,-0.95);
      glEnd();
    }
    if (message.size()) {
      glColor4f(1.0f,0.0f,0.0f,0.3f);
      glBegin(GL_QUADS);
        glVertex2f(-0.75,-0.75);
        glVertex2f(0.75,-0.75);
        glVertex2f(0.75,0.75);
        glVertex2f(-0.75,0.75);
      glEnd();
    }

    if (macro)
    if (!open)
    {
      glColor4f(0,1,1,0.4*color.a);
      glLineWidth(2);
      glBegin(GL_LINE_STRIP);
        glVertex2f(-0.95,-0.95);
        glVertex2f(0.95,-0.95);
        glVertex2f(0.95,0.95);
        glVertex2f(-0.95,0.95);
        glVertex2f(-0.95,-0.95);
      glEnd();
    }
    if (ethereal) {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glPopMatrix();
  if (a_focus == this) {
    pre_draw_children();
    draw_children();
  }


  if (show_titles)
  {
    vsx_vector3<> t = pos + pp;
    float font_size = size.y*0.3;
    float max_size = 0.006;
    if (selected) max_size = 0.012;
    if (font_size > max_size) font_size = max_size;
    t.y -= size.y*0.5;

    font.print_center(t, real_name,font_size);
    t.y += size.y*0.5;
    if (message.size())
    {
      if (message_time > 0.0f) {
        message_time -= vsx_widget_time::get_instance()->get_dtime();
        glColor4f(0,0,0,0.8);
        vsx_vector3<> rp = t;
        rp.x -= 0.008f*10;
        draw_box(rp, 0.008*20, -0.008*10);
        rp.x += 0.0008;
        rp.y -= 0.001;
        font.color = vsx_color<>(1,0.5,0.5,1.0f-fmod(vsx_widget_time::get_instance()->get_time()*2.0f,1.0f));
        font.print(rp, "\nModule status:\n"+message, 0.008);
      } else
      if (m_o_focus == this && !mouse_down_l && !mouse_down_r) {
        glColor4f(0,0,0,0.8);
        vsx_vector3f rp = t + vsx_vector3f(message_pos.x, message_pos.y);
        rp.y -= 0.004;
        draw_box(rp, 0.004*20, -0.004*10);
        rp.x += 0.0008;
        rp.y -= 0.001;
        font.color = vsx_color<>(1,0.5,0.5,color.a);
        font.print(rp, "\nModule status:\n"+message, 0.004);
      }
    }
    font.color = vsx_color<>(1.0f,1.0f,1.0f,1.0f);
  }
  if (a_focus != this) {
    pre_draw_children();
    draw_children();
  }
}

void vsx_widget_component::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);

  message_pos = distance.center;
}

void vsx_widget_component::event_mouse_wheel(float y)
{
  if (macro && open && vsx_input_keyboard.pressed_alt())
  {
    macro_overlay_opacity += -y * 0.1f;
    if (macro_overlay_opacity < 0.0f) macro_overlay_opacity = 0.0f;
    if (macro_overlay_opacity > 1.0f) macro_overlay_opacity = 1.0f;
  } else
  {
    server->event_mouse_wheel(y);
  }
}

void vsx_widget_component::macro_close() {
  for (std::list <vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
    if ((*it)->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
      vsx_widget_component* c = ((vsx_widget_component*)(*it));
      if (c->macro && c->open) {
        c->macro_toggle();
      }
    }
  }
}

void vsx_widget_component::macro_toggle() {
  if (open) {
    macro_close();
  }
  open = !open;
  for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
    if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
      (*children_iter)->enabled = open;
      (*children_iter)->visible = open;
      if (!open) {
        (*children_iter)->pos.x = 0;
        (*children_iter)->pos.y = 0;
      } else {
        (*children_iter)->interpolating_pos = true;
      }
    }
  }
  support_scaling = open;
  if (!open) {
    old_size = target_size;
    int largest_num_anchors;
    if (anchor_order[0] > anchor_order[1]) largest_num_anchors = anchor_order[0];
    else
    largest_num_anchors = anchor_order[1];
    if (largest_num_anchors < 3) largest_num_anchors = 3;
    target_size.y = target_size.x = 0.05f * 0.45 / 2.0f * (float)(largest_num_anchors-1);
    interpolating_size = true;
  } else
  {
    target_size = old_size;
    interpolating_size = true;
  }
  ((vsx_widget_server*)server)->select(this);
}

void vsx_widget_component::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  menu_temp_disable = true;
  if (!vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_shift() && button == 2)
    ((vsx_widget_server*)server)->select(this);

  if (macro && open && button == 0)
    support_scaling = true;
  else
      support_scaling = false;

  if (vsx_input_keyboard.pressed_shift() && vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_alt() && button == 0)
  {
    if (!((vsx_widget_server*)server)->select_add(this))
        return;

    if (((vsx_widget_server*)(this->server))->selected_list.size())
      for (std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin(); itx != ((vsx_widget_server*)server)->selected_list.end(); ++itx)
      {
        ((vsx_widget_component*)(*itx))->real_pos = ((vsx_widget_component*)(*itx))->target_pos;
        ((vsx_widget_component*)(*itx))->ethereal = true;
        ethereal_all = true;
        move_time = vsx_widget_time::get_instance()->get_time();
        transform_state = COMPONENT_MOVE;
      }
  }
  else
  if (vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_shift() && button == 0) {
    real_pos = target_pos;
    ethereal = true;
    move_time = vsx_widget_time::get_instance()->get_time();
    transform_state = COMPONENT_MOVE;
  }
  else
  if (vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_shift() && button == 0) {
    ((vsx_widget_server*)server)->select_add_gui(this);
  }
  else
  if (vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_ctrl() && button == 2) {
    if (component_type == "macro") {
      macro_toggle();
      parent->front(this);
      a_focus = this;
      m_focus = this;
      support_scaling = open;
      return;
    } else
    if (parent->widget_type == VSX_WIDGET_TYPE_COMPONENT)
    ((vsx_widget_component*)parent)->macro_toggle();
    return;
  } else
  if (vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_ctrl() && button == 0 && component_type == "macro" && open) {
    if (support_scaling) {
      if (transform_state == COMPONENT_SCALE) scaled = true;
      move_time = vsx_widget_time::get_instance()->get_time();
      transform_state = COMPONENT_SCALE;
    }
  } else
  if (button == 0) {
    if (transform_state == 0) {
      move_time = vsx_widget_time::get_instance()->get_time();
      transform_state = COMPONENT_MOVE;
      scaled = false;
    }
    ((vsx_widget_server*)server)->select(this);
  }
  menu_temp_disable = false;
  vsx_widget::event_mouse_down(distance,coords,button);
}

void vsx_widget_component::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);
  if (component_type == "macro") {
    if (button == 0 && vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_ctrl()) {
      command_q_b.add_raw("add_empty_macro "+vsx_string_helper::f2s(distance.center.x)+","+vsx_string_helper::f2s(distance.center.y));
      vsx_command_queue_b(this);
    } else

    if (vsx_input_keyboard.pressed_alt() && button == 2 && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift()) {
      return;
    } else
    if (button == 0 && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_alt()) {
      if (open) {
        ((vsx_widget_server*)server)->front(((vsx_widget_server*)server)->module_chooser);
        ((vsx_widget_server*)server)->module_chooser->show();
      }
    }
  } else {
    if (!vsx_input_keyboard.pressed_alt() && button == 0 && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift()) {
      vsx_vector3<> pp = get_pos_p();
      root->move_camera(vsx_vector3<>(pp.x, pp.y, 1.2f+size.x*3.0f));
      transform_state = -1;
      ((vsx_widget_server*)server)->select(this);
      return;
    }
  }
}

void vsx_widget_component::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  if (ethereal) {
    // do stuff
    if (vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift() && vsx_input_keyboard.pressed_alt()) {
      command_q_b.add_raw("component_clone "+name+" "+name+"_clone "+vsx_string_helper::f2s(target_pos.x)+" "+vsx_string_helper::f2s(target_pos.y));
      server->vsx_command_queue_b(this);
      target_pos = real_pos;
      interpolating_pos = true;
      ethereal = false;
    } else
    if (vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_alt()) {
      // component assign code
      vsx_widget_distance l_distance;
      vsx_widget* dest_macro_component = root->find_component(coords,l_distance,true);
      if (dest_macro_component) {
        bool failed = true;
        if (dest_macro_component != parent) {
        failed = false;
          if (dest_macro_component->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
            if (((vsx_widget_component*)dest_macro_component)->component_type != "macro") {
              failed = true;
            }
            if (!((vsx_widget_component*)dest_macro_component)->open)
              failed = true;
          }
        }

        if (!failed) {
          // build a move-to-macro command :3
          vsx_nw_vector <vsx_string<> > comps;
          bool run = true;
          // move ourselves to the beginning
          ((vsx_widget_server*)server)->selected_list.remove((vsx_widget*)this);
          ((vsx_widget_server*)server)->selected_list.push_front((vsx_widget*)this);
          // iterate the selected list
          std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin();
          while (itx != ((vsx_widget_server*)server)->selected_list.end() && run) {
            bool name_ok = true;
            if (dest_macro_component->widget_type == VSX_WIDGET_TYPE_SERVER)
              if (((vsx_widget_server*)server)->find_component(((vsx_widget_component*)(*itx))->real_name))
                name_ok = false;

            if (dest_macro_component->widget_type == VSX_WIDGET_TYPE_COMPONENT
                && ((vsx_widget_server*)server)->find_component(dest_macro_component->name+"."+((vsx_widget_component*)(*itx))->real_name))
              name_ok = false;
            if (((vsx_widget_component*)(*itx))->not_movable)
              a_focus->add(new dialog_messagebox("Error: Component wasn't moved","This component can not be moved: '"+((vsx_widget_component*)(*itx))->real_name+"'||This is due to the component either being locked or of a special type||such as out-bound components."),"foo");

            if (name_ok) {
              comps.push_back((*itx)->name);
            }
            else
            {
              a_focus->add(new dialog_messagebox("Error: Could not move component","There is already a component with this name.||Was trying to move '"+((vsx_widget_component*)(*itx))->real_name+"' into '"+dest_macro_component->name+"'||Rename one of the components and try again!"),"foo");
              comps.reset_used();
              run = false;
              ((vsx_widget_component*)(*itx))->target_pos = ((vsx_widget_component*)(*itx))->real_pos;
            }
            ++itx;
          }
          if (comps.size()) {
            ((vsx_widget_server*)server)->selected_list.remove(this);
            ((vsx_widget_server*)server)->selected_list.push_front(this);
            vsx_string<> deli(",");
            vsx_string<>comps_s = vsx_string_helper::implode(comps, deli);

            command_q_b.add_raw("component_assign "+dest_macro_component->name+" "+comps_s+" "+vsx_string_helper::f2s(l_distance.center.x)+" "+vsx_string_helper::f2s(l_distance.center.y));
            server->vsx_command_queue_b(this);
          }
        } else
        for (std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin(); itx != ((vsx_widget_server*)server)->selected_list.end(); ++itx) {
          ((vsx_widget_component*)(*itx))->target_pos = ((vsx_widget_component*)(*itx))->real_pos;
          ((vsx_widget_component*)(*itx))->interpolating_pos = true;
          ((vsx_widget_component*)(*itx))->ethereal = false;
          ((vsx_widget_component*)(*itx))->ethereal_all = false;
        }
      }
    }
    ethereal = false;
    ethereal_all = false;
  } else {
    if (transform_state == COMPONENT_MOVE) {
      if (support_interpolation)
      move(target_pos.x,target_pos.y,target_pos.z);
      else
      move(pos.x,pos.y,pos.z);
      server_move_notify(true);
    } else
    if (support_scaling)
    if (transform_state == COMPONENT_SCALE) {
      resize_to(target_size);
      server_scale_notify(true);
      scaled = false;
    }
  }
  transform_state = 0;
  vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_component::move(double x, double y, double z) {
  if (transform_state <= 0) return;
  vsx_vector3<> a;
  a.x = x-pos.x;
  a.y = y-pos.y;
  a.z = 0;
  vsx_widget* sel_first = 0;
  if (!ethereal)
  if (((vsx_widget_server*)(this->server))->selected_list.size())
  {
    sel_first = *(((vsx_widget_server*)(this->server))->selected_list.begin());
    if (sel_first && parent != sel_first->parent) return;
    for (std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin(); itx != ((vsx_widget_server*)server)->selected_list.end(); ++itx) {
      if ((*itx)->parent == sel_first->parent) {
        (*itx)->vsx_widget::move((*itx)->pos.x+a.x,(*itx)->pos.y+a.y,(*itx)->pos.z);
        ((vsx_widget_component*)(*itx))->server_move_notify(false);
      }
    }
  }
  if (ethereal) {
    constrained_y = constrained_x = false;
  }
  vsx_widget::move(x,y,z);
  if (ethereal) {
    constrained_y = constrained_x = true;
  } else
  server_move_notify(false);
}

void vsx_widget_component::resize_to(vsx_vector3<> to_size) {
  to_size.y = to_size.x;
  printf("tosizex: %f\n", to_size.x);
  if (parent->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
    if (parent->size.x > size_min.x*1.1) {
      if (to_size.x > parent->size.x*0.95) to_size.y = to_size.x = parent->size.x*0.95;
    }
  }
  server_scale_notify(false);
  vsx_widget::resize_to(to_size);
}

void vsx_widget_component::server_move_notify(bool force) {
  if (vsx_widget_time::get_instance()->get_time() - move_time > 1 || force)
  {
    command_q_b.add_raw("cpp "+name+" "+vsx_string_helper::f2s(pos.x)+" "+vsx_string_helper::f2s(pos.y));
    server->vsx_command_queue_b(this);
    move_time = vsx_widget_time::get_instance()->get_time();
  }
}

void vsx_widget_component::server_scale_notify(bool force)
{
  if (vsx_widget_time::get_instance()->get_time() - move_time > 1 || force)
  {
    command_q_b.add_raw("component_size "+name+" "+vsx_string_helper::f2s(target_size.x));
    server->vsx_command_queue_b(this);
    move_time = vsx_widget_time::get_instance()->get_time();
    scaled = true;
  }
}

void vsx_widget_component::macro_fix_anchors(bool override) {
  if (!override)
  if (!macro) return;
  int anchor_orders = 0;
  if (marked_for_deletion) return;
  for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
    if ((*it)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      if (((vsx_widget_anchor*)(*it))->io == 1 && !(*it)->marked_for_deletion) {
        ((vsx_widget_anchor*)(*it))->a_order = anchor_orders;
        ++anchor_orders;
      }
    }
  }
  anchor_order[1] = anchor_orders;
  anchor_orders = 0;
  for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
    if ((*it)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      if (((vsx_widget_anchor*)(*it))->io == -1 && !(*it)->marked_for_deletion) {
        ((vsx_widget_anchor*)(*it))->a_order = anchor_orders;
        ++anchor_orders;
      }
    }
  }
  anchor_order[0] = anchor_orders;
}

vsx_string<>vsx_widget_component::alias_get_unique_name_in(vsx_string<>base_name, int tried) {
  if (tried == 0) {
    // first run!
    if (p_l_list_in.find(base_name) ==  p_l_list_in.end()) {
      return base_name;
    } else {
      // oops, trouble!
      return alias_get_unique_name_in(base_name,tried+1);
    }
  } else {
    // ok, we already know we need to do stuff
    if (p_l_list_in.find(base_name+"_"+vsx_string_helper::i2s(tried)) ==  p_l_list_in.end()) {
      return base_name+"_"+vsx_string_helper::i2s(tried);
    } else {
      // oops, trouble!
      return alias_get_unique_name_in(base_name,tried+1);
    }
  }
}

vsx_string<>vsx_widget_component::alias_get_unique_name_out(vsx_string<>base_name, int tried) {
  if (tried == 0) {
    // first run!
    if (p_l_list_out.find(base_name) ==  p_l_list_out.end()) {
      return base_name;
    } else {
      // oops, trouble!
      return alias_get_unique_name_out(base_name,tried+1);
    }
  } else {
    // ok, we already know we need to do stuff
    if (p_l_list_out.find(base_name+"_"+vsx_string_helper::i2s(tried)) ==  p_l_list_out.end()) {
      return base_name+"_"+vsx_string_helper::i2s(tried);
    } else {
      // oops, trouble!
      return alias_get_unique_name_out(base_name,tried+1);
    }
  }
}

void vsx_widget_component::hide_all_complex_anchors_but_me(vsx_widget* open_anchor)
{
  if ( ((vsx_widget_anchor*)open_anchor)->io == -1)
  {
    for (p_l_list_in_iter = p_l_list_in.begin(); p_l_list_in_iter != p_l_list_in.end(); p_l_list_in_iter++)
    {
      if ( ((vsx_widget_anchor*)(*p_l_list_in_iter).second)->p_type[0] == 'c') // dumb slow string identifiers MUST DIE!
      {
        if ( ((vsx_widget_anchor*)(*p_l_list_in_iter).second)->p_type == "complex")
        {
          if (open_anchor == (*p_l_list_in_iter).second)
          {
            // open
            if ( ((vsx_widget_anchor*)(*p_l_list_in_iter).second)->tree_open)
              ((vsx_widget_anchor*)(*p_l_list_in_iter).second)->toggle(1);
            else
            ((vsx_widget_anchor*)(*p_l_list_in_iter).second)->toggle(2);
          } else
          {
            // close
            ((vsx_widget_anchor*)(*p_l_list_in_iter).second)->toggle(1);
          }
        }
      }
    }
  } else
  {
    for (p_l_list_out_iter = p_l_list_out.begin(); p_l_list_out_iter != p_l_list_out.end(); p_l_list_out_iter++)
    {
      if ( ((vsx_widget_anchor*)(*p_l_list_out_iter).second)->p_type[0] == 'c') // dumb slow stroutg identifiers MUST DIE!
      {
        if ( ((vsx_widget_anchor*)(*p_l_list_out_iter).second)->p_type == "complex")
        {
          if (open_anchor == (*p_l_list_out_iter).second)
          {
            // open
            if ( ((vsx_widget_anchor*)(*p_l_list_out_iter).second)->tree_open)
              ((vsx_widget_anchor*)(*p_l_list_out_iter).second)->toggle(1);
            else
            ((vsx_widget_anchor*)(*p_l_list_out_iter).second)->toggle(2);
          } else
          {
            // close
            ((vsx_widget_anchor*)(*p_l_list_out_iter).second)->toggle(1);
          }
        }
      }
    }
  }
}


void vsx_widget_component::rename(vsx_string<>new_name, bool partial_name) {
  ((vsx_widget_server*)server)->comp_list.erase(name);
  if (partial_name) {
    if (parent->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
      // part of a macro
      new_name = parent->name+"."+new_name;
    }
    if (macro) {
      rename_add_prefix(new_name,name);
    }
  }
  ((vsx_widget_server*)server)->comp_list[new_name] = this;
  name = new_name;
  vsx_nw_vector<vsx_string<> > add_c;
  vsx_string<>deli = ".";
  vsx_string_helper::explode(new_name,deli,add_c,2);

  vsx_nw_vector<vsx_string<> > comp_realname;
  deli = ";";
  vsx_string_helper::explode(add_c[add_c.size()-1],deli,comp_realname);

  real_name = comp_realname[comp_realname.size()-1];
}

void vsx_widget_component::rename_add_prefix(vsx_string<>prefix, vsx_string<>old_name_remove)
{
  for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it)
  {
    if ((*it)->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
      ((vsx_widget_component*)(*it))->rename_add_prefix(prefix,old_name_remove);
    }
  }
  vsx_string<>new_name = name;
  if (old_name_remove != "")
    vsx_string_helper::str_remove_equal_prefix(new_name, old_name_remove, ".");

  vsx_nw_vector< vsx_string<> > prefix_list;
  vsx_nw_vector< vsx_string<> > name_list;
  vsx_string<>deli = ".";
  vsx_string_helper::explode(prefix, deli, prefix_list);
  vsx_string_helper::explode(new_name, deli, name_list);
  foreach(name_list, i)
    prefix_list.push_back(name_list[i]);

  new_name = vsx_string_helper::implode(prefix_list, deli);
  rename(new_name,false);
}

// separate this in 2 methods!
void vsx_widget_component::get_connections_in_abs(std::list<vsx_widget_connector_info*>* mlist) {
//  printf("comp, get_connections_abs %s\n",name.c_str());
  for (std::map<vsx_string<>, vsx_widget*>::iterator it = p_l_list_in.begin(); it != p_l_list_in.end(); ++it) {
    if (((*it).second)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)((*it).second))->get_connections_abs((*it).second, mlist);
    }
  }
}

void vsx_widget_component::get_connections_out_abs(std::list<vsx_widget_connector_info*>* mlist) {
//  printf("comp, get_connections_abs %s\n",name.c_str());
  for (std::map<vsx_string<>, vsx_widget*>::iterator it = p_l_list_out.begin(); it != p_l_list_out.end(); ++it) {
    if (((*it).second)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)((*it).second))->get_connections_abs((*it).second, mlist);
    }
  }
}

void vsx_widget_component::disconnect_abs() {
  for (std::map<vsx_string<>, vsx_widget*>::iterator it = p_l_list_in.begin(); it != p_l_list_in.end(); ++it) {
    if (((*it).second)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)((*it).second))->disconnect_abs();
    }
  }
  for (std::map<vsx_string<>, vsx_widget*>::iterator it = p_l_list_out.begin(); it != p_l_list_out.end(); ++it) {
    if (((*it).second)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)((*it).second))->disconnect_abs();
    }
  }
}


void vsx_widget_component::begin_delete()
{
  deleting = 1;
  target_size.x  = 0.0f;
  interpolating_size = true;
}


void vsx_widget_component::perform_delete()
{
  command_q_b.add_raw("component_delete "+name);
  server->vsx_command_queue_b(this);
}

bool vsx_widget_component::event_key_down(uint16_t key)
{
  if (key == VSX_SCANCODE_DELETE)
  {
    if (((vsx_widget_server*)(this->server))->selected_list.size())
    for (std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin(); itx != ((vsx_widget_server*)server)->selected_list.end(); ++itx) {
      if (!((vsx_widget_component*)(*itx))->internal_critical) {
        ((vsx_widget_component*)(*itx))->begin_delete();
      }
    }
  }
  if (key == VSX_SCANCODE_Q) {
    show_titles = !show_titles;
  }
  return true;
}

void vsx_widget_component::before_delete() {
  ((vsx_widget_server*)(server))->selected_list.remove(this);
  ((vsx_widget_server*)server)->comp_list.erase(name);

}

void vsx_widget_component::on_delete()
{
  mtex.reset(nullptr);
  mtex_blob.reset(nullptr);
  mtex_overlay.reset(nullptr);
}
