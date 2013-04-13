/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
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

#ifndef VSX_NO_CLIENT
#include "vsx_gl_global.h"

#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsxfst.h"
#include "vsx_param.h"
#include "vsx_module.h"
// local includes
#include "lib/vsx_widget_lib.h"
#include "vsx_widget_comp.h"
#include "vsx_widget_anchor.h"
#include <vsx_command_client_server.h>
#include "vsx_widget_server.h"
//#include "vsx_widget_module_chooser.h"
#include "dialogs/vsx_widget_window_statics.h"
#include "controllers/vsx_widget_base_controller.h"
#include "controllers/vsx_widget_editor.h"

#include <GL/glfw.h>

using namespace std;
// VSX_WIDGET_COMPONENT **************************************************************************************************>
// VSX_WIDGET_COMPONENT **************************************************************************************************>
// VSX_WIDGET_COMPONENT **************************************************************************************************>
// VSX_WIDGET_COMPONENT **************************************************************************************************>

bool vsx_widget_component::show_titles = true;
bool vsx_widget_component::ethereal_all = false;

int vsx_widget_component::inside_xy_l(vsx_vector &test, vsx_vector &global) {
  if (ethereal || (ethereal_all && !macro)) return 0;
  return vsx_widget::inside_xy_l(test, global);
}

void vsx_widget_component::vsx_command_process_b(vsx_command_s *t)
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
      //printf("adding command to server again, not found\n");
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
    vsx_string s = base64_decode(t->parts[2]);
    vsx_string deli = "&&";

    deli = "||";
    std::vector<vsx_string> parts;
    explode(s,deli,parts);
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
    printf("empty: %s\n",t->cmd_data.c_str());
    command_q_b.add_raw("macro_create macros;empty "+name+"."+"empty"+" "+str_replace(","," ",t->cmd_data)+" 0.1");
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
    printf("macro_dump_add %s\n",base64_decode(t->parts[2]).c_str());
    macro_commands.add_raw(base64_decode(t->parts[2]));
    return;
  } else
  if (t->cmd == "macro_dump_complete")
  {
    printf("macro dump complete\n");
    macro_commands.save_to_file(vsx_get_data_path()+"macros/"+t->parts[2]);
    macro_commands.clear();
    command_q_b.add_raw("module_info_add macros;"+t->parts[2]);
    server->vsx_command_queue_b(this);
    return;
  } else
  if (t->cmd == "component_clone_add") {
    printf("component_clone_add %s\n",base64_decode(t->parts[2]).c_str());
    macro_commands.add_raw(base64_decode(t->parts[2]));
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
        //printf("component_clone_internal: %s\n", vsx_string("macro_create_real "+mc->parts[1]+" "+t->parts[3]+" "+t->parts[4]+" "+mc->parts[2]).c_str());
      } else
      if (mc->cmd == "macro_create" && mc->parts.size() < 6)
      {
        command_q_b.add_raw("macro_create_real "+mc->parts[1]+" "+mc->parts[2]+" "+mc->parts[3]+" "+mc->parts[4]);
        //printf("component_clone_internal: %s\n", vsx_string("macro_create_real "+mc->parts[1]+" "+t->parts[3]+" "+t->parts[4]+" "+mc->parts[2]).c_str());
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
      //if
    }
    server->vsx_command_queue_b(this);
    macro_commands.clear();
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
        //((vsx_widget_component*)(*itx))->component_type != "screen") {
          command_q_b.add_raw("component_delete "+(*itx)->name);
          server->vsx_command_queue_b(this);
        }
      }
    }
    return;
    //for (std::list<vsx_widget*>::iterator it=children.begin(); it != children.end(); it++)
    //{
      // 1. disconnect everything on the serverside
      // 2. delete the components, the gui will handle visual connections when the server is finished.
      //    this is done as eeach vsx_component goes through all anchors on the in-side and plain deletes them.
      //    then it goes through the anchors on its outside, using the reversemap to find the anchors in charge
      //    and ask them to delete.
    //}
  //{
  } else
  if (t->cmd == "in_param_spec" || t->cmd == "out_param_spec" || t->cmd == "ipsa" || t->cmd == "opsa")
  {
    //printf("widget_comp::vsx_command_process_b command in_param_spec: param part 1: %s----:::\n",t->raw.c_str());
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
      for (std::map<vsx_string, vsx_widget*>::iterator it = t_list.begin(); it != t_list.end(); it++)
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
    std::vector<vsx_string> add_c;
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
    //printf("param_spec_parsing %d\n",l_io);

    // THIS CODE IS A MESS! :(
    vsx_string cd = t->parts[2];
    //printf("comp_param_spec_parsing %s----:::\n",cd.c_str());
    vsx_string cm = "";
    vsx_string cms = "";
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
        vsx_string deli = ":";
        split_string(cm,deli,add_c,-1);
        vsx_widget_anchor *anchor = 0;
        //printf("comp_p1\n");
        // see if there's an anchor we can use
        if (t_list.find(add_c[0]) != t_list.end()) {
          anchor = (vsx_widget_anchor*)t_list[add_c[0]];
          //printf("anchor order 1++ %s\n",tt->name.c_str());
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
          //printf("addc0 %s\n\n\n",cm.c_str());
          std::vector<vsx_string> type_info;
          vsx_string type_deli = "?";
          split_string(add_c[1],type_deli,type_info);
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
              //printf("addint to p_list_in\n");
              ((vsx_widget_component*)((vsx_widget_anchor*)anchor)->component)->p_l_list_in[add_c[0]] = anchor;
            }
            else
            {
              //printf("addint to p_list_out\n");
              ((vsx_widget_component*)((vsx_widget_anchor*)anchor)->component)->p_l_list_out[add_c[0]] = anchor;
            }
          }

          if (l_io < 0)
          {
            //printf("anchor order 2++ %s\n",tt->name.c_str());
            ((vsx_widget_anchor*)anchor)->a_order = anchor_order[0]++;
          }
          else
          {
            //printf("anchor order 2++ %s\n",tt->name.c_str());
            ((vsx_widget_anchor*)anchor)->a_order = anchor_order[1]++;
            //printf("new anchor has order %d",((vsx_widget_anchor*)tt)->order);
          }

          //tt->pos.y = ypos;

          anchor->size.x = anchor->size.y = 0.0f;
          anchor->target_size.x = anchor->target_size.y = 0.05/6;
          anchor->interpolating_size = true;

          //tt->size.x = size.x/6;
          //tt->size.y = size.x/6;
          //printf("comp_p2\n");
          //printf("comp_p3\n");
        }

        if (cms.size()) {
          //printf("cms = %s\n",cms.c_str());
          ((vsx_widget_anchor*)anchor)->p_def += add_c[1];
          ((vsx_widget_anchor*)anchor)->p_def += "[";
          if (t->parts.size() == 4) {
            if (t->parts[3] == "c") cms += " c";
          }
          command_q_b.add_raw(t->cmd+" "+add_c[0]+" "+cms);
          anchor->vsx_command_queue_b(this);
          ((vsx_widget_anchor*)anchor)->p_def += "]";
          //((vsx_widget_anchor*)tt)->fix_anchors();
        } else ((vsx_widget_anchor*)anchor)->p_def = add_c[1];
        //ypos -= (size.x/6)*1.5;
        cms = "";
        cm = "";
      }
    }
    for (std::map<vsx_string, vsx_widget*>::iterator it = t_list.begin(); it != t_list.end(); ++it)
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

    //printf("num anchors: %d\n", largest_num_anchors);
    init_children();
    return;
  } else
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
            t->parts[5]
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
        //printf("negative io! i'm %s\n",name.c_str());
        if (a->p_l_list_in.find(t->parts[4]) != a->p_l_list_in.end())
        {
         // printf("found 1\n");
          if (p_l_list_in.find(t->parts[2]) != p_l_list_in.end()) {
           // printf("found 2\n");
            // seems all we need is out there, make the connection!
            command_q_b.add_raw(
              "pca "+
              name+" "+
              p_l_list_in[t->parts[2]]->name+" "+
              a->name+" "+
              a->p_l_list_in[t->parts[4]]->name+" "+
              t->parts[5]+" "+
              t->parts[6]
            );
            p_l_list_in[t->parts[2]]->vsx_command_queue_b(this);
          } else failed = true;
        } else failed = true;
      } else {
        //printf("positive io! i'm %s\n",name.c_str());
        if (a->p_l_list_out.find(t->parts[4]) != a->p_l_list_out.end())
        {
          //printf("found 1\n");
          if (p_l_list_out.find(t->parts[2]) != p_l_list_out.end()) {
            //printf("found 2\n");
            // seems all we need is out there, make the connection!
            command_q_b.add_raw(
              "pca "+
              name+" "+
              p_l_list_out[t->parts[2]]->name+" "+
              a->name+" "+
              a->p_l_list_out[t->parts[4]]->name+" "+
              t->parts[5]+" "+
              t->parts[6]
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
    vsx_widget* tt = add(new vsx_widget_editor,name+".edit");
    ((vsx_widget_editor*)tt)->target_param = name;
    ((vsx_widget_editor*)tt)->return_command = "ps64";
    ((vsx_widget_editor*)tt)->return_component = this;
    tt->set_render_type(VSX_WIDGET_RENDER_3D);
    tt->widget_type = VSX_WIDGET_TYPE_CONTROLLER;
    tt->set_font_size(0.002);
    tt->set_border(0.0005);
    tt->set_pos(vsx_vector(0,-0.08f));

    ((vsx_widget_editor*)tt)->return_command = "vsxl_cfi";
    ((vsx_widget_editor*)tt)->return_component = this;
    ((vsx_widget_editor*)tt)->load_text(base64_decode(t->parts[2]));

    //tt->title = "VSXL [compfilter] : "+name;
    //((vsx_widget_2d_editor*)tt)->return_command = "vsxl_cfi";
    //((vsx_widget_2d_editor*)tt)->return_component = this;
    //((vsx_widget_2d_editor*)tt)->load_text(base64_decode(t->parts[2]));
  } else
  if (t->cmd == "vsxl_cfi") {
    command_q_b.add_raw("vsxl_cfi "+name+" "+t->parts[1]);
    server->vsx_command_queue_b(this);
  } else
  if (t->cmd == "component_info")
  {
    if (t->parts.size() >= 3) {
      std::vector<vsx_string> parts;
      // critical:screen:small
      vsx_string deli = ":";
      explode(t->parts[2],deli,parts);

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

      mtex.load_png(skin_path+"component_types/"+component_type+".png");
      if (component_type == "macro")
      {
        // load overlay texture
        mtex_overlay.load_png(skin_path+"component_types/"+component_type+"_overlay.png");
      }

      if (!internal_critical)
      {
        menu = add(new vsx_widget_popup_menu,".comp_menu");
        //if (parts.size() > 1) {
          //target_size.x = size.x *= 0.45;
          //target_size.y = size.y *= 0.45;
        //}
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
    {
      //command_q_b.add("get_in_param_spec",name);
      //command_q_b.add("get_out_param_spec",name);
      //command_q_b.add_raw("get_in_param_connections "+name);
      //command_q_b.add_raw("get_component_info "+name);
      //parent->vsx_command_queue_b(this);
      init_run = true;
    }
    //delete c;
    //c = 0;
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
  color.r = 1.0f;//0.0001f;//(double)(rand()%1000)/1000.0;
  color.g = 1.0f;//(double)(rand()%1000)/1000.0;
  color.b = 1.0f;//(double)(rand()%1000)/1000.0;
  color.a = 1.0f;
  ethereal = false;
  selected = false;
  not_movable = false;

  init_children();
  //printf("vsx_widget_component::init()\n");
  command_q_f.add("init","");
  support_scaling = false;
  anchor_order[0] = 0;
  anchor_order[1] = 0;
  widget_type = VSX_WIDGET_TYPE_COMPONENT;
  parent_name = "";
  //myf.background_color.a = 0.5;
  //myf.background = true;
  myf.mode_2d = true;
  init_run = true;
  macro = false;
  scaled = false;
  transform_state = 0;
  size_min.x = 0.1f;

  mtex_blob.load_png(skin_path+"interface_extras/connection_blob.png");
}

void vsx_widget_component::reinit()
{
  #ifndef VSXU_PLAYER
  mtex.load_png(skin_path+"component_types/"+component_type+".png");
  #endif
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
  vsx_vector pp = parent->get_pos_p();
  size.y = size.x;
  glPushMatrix();
  #define FF 1.2f
  #define FF2 1.8f
  #define FFDIV 0.24f


    const GLshort squareTexcoords[] = {
      0, 0,
      1, 0,
      0, 1,
      1, 1
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
        #ifndef VSXU_PLAYER
          mtex.bind();
        #endif
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        #ifndef VSXU_PLAYER
          mtex._bind();
        #endif
      glPopMatrix();
      glColor4f(1,1,1,0.3);
    } else
    {
      color.gl_color();
    }
    if (macro)
    {
      glColor3f(color.r,color.g,color.b);
    }

    glTranslatef(pos.x+pp.x,pos.y+pp.y,pos.z);
    glScaled(size.x/2, size.y/2, 1);
    mtex.bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    mtex._bind();
    if (macro)
    {
      glColor4f(1.0,1.0,1.0,macro_overlay_opacity);
      mtex_overlay.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      mtex_overlay._bind();
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
      mtex_blob.bind();
      glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      mtex_blob._bind();
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
    //glLoadIdentity();
    if (ethereal) {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glPopMatrix();
  if (a_focus == this) {
    pre_draw_children();
    draw_children();
  }
  if (show_titles) {
    vsx_vector t = pos + pp;
    float font_size = size.y*0.3;
    float max_size = 0.006;
    if (selected) max_size = 0.012;
    if (font_size > max_size) font_size = max_size;
    t.y -= size.y*0.5;

    //myf.background = true;
    //myf.background_color.a = color.a*0.4f;
    myf.print_center(t, real_name,font_size);
    t.y += size.y*0.5;
    if (message.size())
    {
      if (message_time > 0.0f) {
        message_time -= dtime;
        glColor4f(0,0,0,0.8);
        vsx_vector rp = t;
        rp.x -= 0.008f*10;
        //rp.y -= 0.004;
        draw_box(rp, 0.008*20, -0.008*10);
        rp.x += 0.0008;
        rp.y -= 0.001;
        //myf.background = true;
        myf.color = vsx_color__(1,0.5,0.5,1.0f-fmod(time*2.0f,1.0f));
        myf.print(rp, "\nModule status:\n"+message, 0.008);
      } else
      if (m_o_focus == this && !mouse_down_l && !mouse_down_r) {
        glColor4f(0,0,0,0.8);
        vsx_vector rp = t + message_pos;
        rp.y -= 0.004;
        draw_box(rp, 0.004*20, -0.004*10);
        rp.x += 0.0008;
        rp.y -= 0.001;
        //myf.background = false;
        myf.color = vsx_color__(1,0.5,0.5,color.a);
        myf.print(rp, "\nModule status:\n"+message, 0.004);
      }
    }
    myf.color = vsx_color(1.0f,1.0f,1.0f,1.0f);
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
  if (macro && open && alt)
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
  //printf("distance.center.x %f\n",distance.center.x);
  if (!alt && !shift && button == 2) {
    ((vsx_widget_server*)server)->select(this);
  }
  if (macro && open && button == 0)
  support_scaling = true; else support_scaling = false;
  //if (ctrl && !alt && !shift && button == GLUT_RIGHT_BUTTON) {
    //if (component_type != "screen") {
//        command_q_b.add_raw("component_delete "+name);
//        server->vsx_command_queue_b(this);
//      }
  //} else
  if (shift && ctrl && !alt && button == 0) {
    if (!((vsx_widget_server*)server)->select_add(this)) return;

    if (((vsx_widget_server*)(this->server))->selected_list.size())
    for (std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin(); itx != ((vsx_widget_server*)server)->selected_list.end(); ++itx) {
      ((vsx_widget_component*)(*itx))->real_pos = ((vsx_widget_component*)(*itx))->target_pos;
      ((vsx_widget_component*)(*itx))->ethereal = true;
      ethereal_all = true;
      move_time = time;
      transform_state = COMPONENT_MOVE;
    }
//      real_pos = pos;
//      ethereal = true;
  } else
  if (ctrl && alt && !shift && button == 0) {
    real_pos = target_pos;
    ethereal = true;
    move_time = time;
    transform_state = COMPONENT_MOVE;
  } else
  if (ctrl && !alt && !shift && button == 0) {
    ((vsx_widget_server*)server)->select_add_gui(this);
  }
  else
  if (alt && !shift && !ctrl && button == 2) {
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
  if (alt && !ctrl && button == 0 && component_type == "macro" && open) {
    if (support_scaling) {
      //printf("supporting scaling\n");
      if (transform_state == COMPONENT_SCALE) scaled = true;
      move_time = time;
      transform_state = COMPONENT_SCALE;
    }// else printf("doesn't support scaling\n");
//      printf("component scale transform state set\n");
  } else
  if (button == 0) {
    if (transform_state == 0) {
      //printf("moving\n");
      move_time = time;
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
   //if (ctrl && button == GLUT_RIGHT_BUTTON) {
     //if (component_type != "screen") {
//         command_q_b.add_raw("component_delete "+name);
//         server->vsx_command_queue_b(this);
//       }
//     } else
  if (component_type == "macro") {
    if (button == 0 && alt && !shift && !ctrl) {
      command_q_b.add_raw("add_empty_macro "+f2s(distance.center.x)+","+f2s(distance.center.y));
      vsx_command_queue_b(this);
    } else

    if (alt && button == 2 && !ctrl && !shift) {
      //macro_toggle();
      return;
    } else
    if (button == 0 && !ctrl && !shift && !alt) {
      if (open) {
        ((vsx_widget_server*)server)->front(((vsx_widget_server*)server)->module_chooser);
        ((vsx_widget_server*)server)->module_chooser->show();
      }
    }
  } else {
    if (!alt && button == 0 && !ctrl && !shift) {
      vsx_vector pp = get_pos_p();
      root->move_camera(vsx_vector(pp.x, pp.y, 1.2f+size.x*3.0f));
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
    if (ctrl && !shift && alt) {
      command_q_b.add_raw("component_clone "+name+" "+name+"_clone "+f2s(target_pos.x)+" "+f2s(target_pos.y));
      server->vsx_command_queue_b(this);
      target_pos = real_pos;
      interpolating_pos = true;
      ethereal = false;
    } else
    if (ctrl && shift && !alt) {
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

//        vsx_vector my_pos = target_pos;
    //    printf("eth2\n");
        if (!failed) {
          // build a move-to-macro command :3
          //vsx_vector ttpos = world-tt->get_pos_p();//-(get_pos_p());
          //printf("local pos %f %f\n",ttpos.x,ttpos.y);
      //    printf("eth3\n");
          std::vector<vsx_string> comps;
          bool run = true;
          // move ourselves to the beginning
          ((vsx_widget_server*)server)->selected_list.remove((vsx_widget*)this);
          ((vsx_widget_server*)server)->selected_list.push_front((vsx_widget*)this);
          // iterate the selected list
          std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin();
          while (itx != ((vsx_widget_server*)server)->selected_list.end() && run) {
            // check if the destination component (check name) exists
        //    printf("eth4\n");
            bool name_ok = true;
            if (dest_macro_component->widget_type == VSX_WIDGET_TYPE_SERVER)
              if (((vsx_widget_server*)server)->find_component(((vsx_widget_component*)(*itx))->real_name))
                name_ok = false;
          //  printf("name move sequence: %s.%s ",tt->name.c_str(),((vsx_widget_component*)(*itx))->real_name.c_str());
            if (dest_macro_component->widget_type == VSX_WIDGET_TYPE_COMPONENT
                && ((vsx_widget_server*)server)->find_component(dest_macro_component->name+"."+((vsx_widget_component*)(*itx))->real_name))
              name_ok = false;
            if (((vsx_widget_component*)(*itx))->not_movable)
              a_focus->add(new dialog_messagebox("Error: Component wasn't moved","This component can not be moved: '"+((vsx_widget_component*)(*itx))->real_name+"'||This is due to the component either being locked or of a special type||such as out-bound components."),"foo");

            if (name_ok) {
            //  printf("pushing back\n");
              comps.push_back((*itx)->name);
            }
            else
            {
              a_focus->add(new dialog_messagebox("Error: Could not move component","There is already a component with this name.||Was trying to move '"+((vsx_widget_component*)(*itx))->real_name+"' into '"+dest_macro_component->name+"'||Rename one of the components and try again!"),"foo");
              comps.empty();
              run = false;
              ((vsx_widget_component*)(*itx))->target_pos = ((vsx_widget_component*)(*itx))->real_pos;
  //            itx = ((vsx_widget_server*)server)->selected_list.end();
            }
            ++itx;
          }
  //        printf("eth5\n");
          if (comps.size()) {
    //        printf("eth6\n");
            ((vsx_widget_server*)server)->selected_list.remove(this);
            ((vsx_widget_server*)server)->selected_list.push_front(this);
            vsx_string comps_s = implode(comps,",");

            command_q_b.add_raw("component_assign "+dest_macro_component->name+" "+comps_s+" "+f2s(l_distance.center.x)+" "+f2s(l_distance.center.y));
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
    //printf("up done\n");
  } else {
    //printf("transform_state: %d\n",transform_state);
    if (transform_state == COMPONENT_MOVE) {
      time += 4;
      if (support_interpolation)
      move(target_pos.x,target_pos.y,target_pos.z);
      else
      move(pos.x,pos.y,pos.z);
      time -=4;
      server_move_notify();
    } else
    if (support_scaling)
    if (transform_state == COMPONENT_SCALE) {
      time += 4;
      resize_to(target_size);
      time -=4;
      scaled = false;
    }
  }
  transform_state = 0;
  vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_component::move(double x, double y, double z) {
  //printf("comp_move\n");
  //if (macro && open) return;
  if (transform_state <= 0) return;
  vsx_vector a;
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
        ((vsx_widget_component*)(*itx))->server_move_notify();
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
  server_move_notify();
}

void vsx_widget_component::resize_to(vsx_vector to_size) {
  to_size.y = to_size.x;
  printf("tosizex: %f\n", to_size.x);
  if (parent->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
    if (parent->size.x > size_min.x*1.1) {
      if (to_size.x > parent->size.x*0.95) to_size.y = to_size.x = parent->size.x*0.95;
    }
  }
  server_scale_notify();
  vsx_widget::resize_to(to_size);
}

void vsx_widget_component::server_move_notify() {
  if (time - move_time > 1) {
    command_q_b.add_raw("cpp "+name+" "+f2s(pos.x)+" "+f2s(pos.y));
    server->vsx_command_queue_b(this);
    move_time = time;
  }
}

void vsx_widget_component::server_scale_notify() {
  if (time - move_time > 1) {
    command_q_b.add_raw("component_size "+name+" "+f2s(target_size.x));
    server->vsx_command_queue_b(this);
    move_time = time;
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

vsx_string vsx_widget_component::alias_get_unique_name_in(vsx_string base_name, int tried) {
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
    if (p_l_list_in.find(base_name+"_"+i2s(tried)) ==  p_l_list_in.end()) {
      return base_name+"_"+i2s(tried);
    } else {
      // oops, trouble!
      return alias_get_unique_name_in(base_name,tried+1);
    }
  }
}

vsx_string vsx_widget_component::alias_get_unique_name_out(vsx_string base_name, int tried) {
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
    if (p_l_list_out.find(base_name+"_"+i2s(tried)) ==  p_l_list_out.end()) {
      return base_name+"_"+i2s(tried);
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


void vsx_widget_component::rename(vsx_string new_name, bool partial_name) {
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
  std::vector<vsx_string> add_c;
  vsx_string deli = ".";
  split_string(new_name,deli,add_c,2);

  std::vector<vsx_string> comp_realname;
  deli = ";";
  split_string(add_c[add_c.size()-1],deli,comp_realname);

  real_name = comp_realname[comp_realname.size()-1];
}

void vsx_widget_component::rename_add_prefix(vsx_string prefix, vsx_string old_name_remove)
{
  for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it)
  {
    if ((*it)->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
      ((vsx_widget_component*)(*it))->rename_add_prefix(prefix,old_name_remove);
    }
  }
  vsx_string new_name = name;
//  printf("name = %s\n",name.c_str());
//  printf("old_name_remove = %s\n",old_name_remove.c_str());
  vsx_string old_name = old_name_remove;
  if (old_name_remove != "") {
    str_remove_equal_prefix(&new_name,&old_name, ".");
    //new_name = prefix+str_replace(old_name_remove,"",name);
  }
//  printf("prefix = %s new_name = %s\n",prefix.c_str(), new_name.c_str());
  std::list<vsx_string> prefix_list;
  std::list<vsx_string> name_list;
  vsx_string deli = ".";
  explode(prefix, deli, prefix_list);
  explode(new_name, deli, name_list);
  for (std::list<vsx_string>::iterator it = name_list.begin(); it != name_list.end(); ++it)
  {
    prefix_list.push_back(*it);
  }
  //prefix_list.merge(name_list);

  new_name = implode(prefix_list, deli);

//  printf("prefix = %s new_name = %s\n",prefix.c_str(), new_name.c_str());
  //new_name = prefix+new_name;
  rename(new_name,false);
}

// separate this in 2 methods!
void vsx_widget_component::get_connections_in_abs(std::list<vsx_widget_connector_info*>* mlist) {
//  printf("comp, get_connections_abs %s\n",name.c_str());
  for (std::map<vsx_string, vsx_widget*>::iterator it = p_l_list_in.begin(); it != p_l_list_in.end(); ++it) {
    if (((*it).second)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)((*it).second))->get_connections_abs((*it).second, mlist);
    }
  }
}

void vsx_widget_component::get_connections_out_abs(std::list<vsx_widget_connector_info*>* mlist) {
//  printf("comp, get_connections_abs %s\n",name.c_str());
  for (std::map<vsx_string, vsx_widget*>::iterator it = p_l_list_out.begin(); it != p_l_list_out.end(); ++it) {
    if (((*it).second)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)((*it).second))->get_connections_abs((*it).second, mlist);
    }
  }
}

void vsx_widget_component::disconnect_abs() {
  for (std::map<vsx_string, vsx_widget*>::iterator it = p_l_list_in.begin(); it != p_l_list_in.end(); ++it) {
    if (((*it).second)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)((*it).second))->disconnect_abs();
    }
  }
  for (std::map<vsx_string, vsx_widget*>::iterator it = p_l_list_out.begin(); it != p_l_list_out.end(); ++it) {
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
  /*for (std::list<vsx_widget*>::iterator itx = children.begin(); itx != children.end(); itx++)
  {
    (*itx)->target_size.y = (*itx)->target_size.x = 0.0f;
    (*itx)->interpolating_size = true;
  }*/
}


void vsx_widget_component::perform_delete()
{
  command_q_b.add_raw("component_delete "+name);
  server->vsx_command_queue_b(this);
}

bool vsx_widget_component::event_key_down(signed long key, bool alt, bool ctrl, bool shift)
{
  VSX_UNUSED(alt);
  VSX_UNUSED(ctrl);
  VSX_UNUSED(shift);
  if (abs(key) == GLFW_KEY_DEL)
  {
    if (((vsx_widget_server*)(this->server))->selected_list.size())
    for (std::list<vsx_widget*>::iterator itx = ((vsx_widget_server*)server)->selected_list.begin(); itx != ((vsx_widget_server*)server)->selected_list.end(); ++itx) {
      if (!((vsx_widget_component*)(*itx))->internal_critical) {
        ((vsx_widget_component*)(*itx))->begin_delete();
      }
    }
  }
  if (key == (signed long)'q') {
    show_titles = !show_titles;
  }
  return true;
}

void vsx_widget_component::before_delete() {
  ((vsx_widget_server*)(server))->selected_list.remove(this);
  ((vsx_widget_server*)server)->comp_list.erase(name);
}
#endif
