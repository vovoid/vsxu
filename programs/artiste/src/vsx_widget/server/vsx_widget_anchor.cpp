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
#include <vsx_param_helper.h>
#include <texture/vsx_texture.h>

// local includes
#include "vsx_widget.h"
#include "widgets/vsx_widget_popup_menu.h"
#include "vsx_widget_window.h"
#include "vsx_widget_anchor.h"
#include "vsx_widget_server.h"
#include "module_choosers/vsx_widget_module_chooser.h"
#include "vsx_widget_connector_bezier.h"
#include "helpers/vsx_widget_assistant.h"
#include "controllers/vsx_widget_controller_base.h"
#include "controllers/vsx_widget_controller_seq.h"
#include "controllers/vsx_widget_controller_ab.h"
#include "controllers/vsx_widget_controller_editor.h"
#include "controllers/vsx_widget_controller_dialog.h"
#include "controllers/vsx_widget_controller_knob.h"
#include "controllers/vsx_widget_controller_channel.h"
#include "controllers/vsx_widget_controller_mixer.h"
#include "controllers/vsx_widget_controller_color.h"
#include "controllers/vsx_widget_controller_pad.h"
#include "vsx_widget_comp.h"
#include <gl_helper.h>

#include "artiste_desktop.h"

// widget
#include <dialogs/dialog_messagebox.h>


bool vsx_widget_anchor::drag_status = false;
bool vsx_widget_anchor::clone_value = false;
vsx_vector3<> vsx_widget_anchor::drag_position;
vsx_widget_anchor* vsx_widget_anchor::drag_anchor;
vsx_widget_anchor* vsx_widget_anchor::drag_clone_anchor;

//-- b a c k e n d -----------------------------------------------------------------------------------------------------


void vsx_widget_anchor::command_process_back_queue(vsx_command_s *t) 
{
  // command:
  //   anchor_unalias
  //
  // source:
  //   menu; ours
  if (t->cmd == "anchor_unalias")
  {
    command_q_b.add_raw("param_unalias "+vsx_string_helper::i2s(io)+" "+component->name+" "+name);
    return;
  }





  // command:
  //   connections_order_ok [component] [anchor] [specification]
  //
  // source:
  //   engine
  if (t->cmd == "connections_order_ok")
  {
    vsx_nw_vector<vsx_string<> > order_list;
    vsx_string<>deli = ",";
    vsx_string_helper::explode(t->parts[3], deli, order_list);
    // set up a mapping list with the connections
    std::map<int,vsx_widget*> connection_map;
    for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) 
    {
      if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) 
      {
        connection_map[((vsx_widget_connector_bezier*)*children_iter)->order] = *children_iter;
      }
    }

    int c = 0;
    foreach(order_list, i)
    {
      ((vsx_widget_connector_bezier*)connection_map[ vsx_string_helper::s2i( order_list[i] ) ])->order = c;
      ((vsx_widget_connector_bezier*)connection_map[ vsx_string_helper::s2i( order_list[i] ) ])->move(vsx_vector3<>(0.0f));
      ++c;
    }
    return;
  }





  // command:
  //   connections_order_int 1
  //
  // source:
  //   vsx_widget_connector_bezier
  if (t->cmd == "connections_order_int")
  {
    std::map<float, vsx_widget*> position_map;
    
    // create the order-map. we're lucky as the maps always put the contents in order <3
    for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) 
    {
      if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) {
        position_map[((vsx_widget_connector_bezier*)*children_iter)->real_pos.y] = *children_iter;
      }
    }
    vsx_nw_vector<vsx_string<> > order_list;
    for (std::map<float,vsx_widget*>::reverse_iterator it = position_map.rbegin(); it != position_map.rend(); ++it) 
    {
      order_list.push_back(vsx_string_helper::i2s(((vsx_widget_connector_bezier*)((*it).second))->order));
    }
    vsx_string<> deli(",");
    vsx_string<>order_list_finished = vsx_string_helper::implode(order_list, deli);
    command_q_b.add_raw("connections_order "+component->name+" "+name+" "+order_list_finished);
    ((vsx_widget_component*)component)->server->vsx_command_queue_b(this);
    return;
  }





  // command:
  //   param_connect_ok [component] [param] [dest-comp] [dest-param] [order]
  // source:
  //   engine
  if (t->cmd == "param_connect_ok")
  {
    // we're the first 2 parts of the command, so just find the other component and create the link
    // first find the server.

    vsx_widget_component *b = (vsx_widget_component*)((vsx_widget_server*)((vsx_widget_component*)component)->server)->find_component(t->parts[3]);
    if (b) 
    {
      // ok now find the other anchor
      if (b->p_l_list_out.find(t->parts[4]) != b->p_l_list_out.end()) 
      {
        vsx_widget_anchor *ba = (vsx_widget_anchor*)b->p_l_list_out[t->parts[4]];
        connect_far( ba, vsx_string_helper::s2i(t->parts[5]) );
      }
    }
    return;
  }





  // command:
  //   pca [component] [param] [dest-comp] [dest-param] [order]
  // source:
  //   vsx_widget_component
  if (t->cmd == "pca")
  {
    // we're the first 2 parts of the command, so just find the other component and create the link
    // first find the server.
    vsx_widget_component *b = (vsx_widget_component*)((vsx_widget_server*)((vsx_widget_component*)component)->server)->find_component(t->parts[3]);
    if (b) 
    {
      // ok now find the other anchor
      vsx_widget_anchor *ba = 0;
      if (t->parts[6] == "-1") 
      {
        if (b->p_l_list_in.find(t->parts[4]) != b->p_l_list_in.end()) 
        {
          ba = (vsx_widget_anchor*)b->p_l_list_in[t->parts[4]];
        }
      } else 
      {
        if (b->p_l_list_out.find(t->parts[4]) != b->p_l_list_out.end()) 
        {
          ba = (vsx_widget_anchor*)b->p_l_list_out[t->parts[4]];
        }
      }
      // ok, all we need is available
      if (ba) 
      {
        // delete controllers
        delete_controllers();
        // add new connector
        vsx_widget *tt = add(new vsx_widget_connector_bezier,name+":conn");

        ((vsx_widget_connector_bezier*)tt)->alias_conn = false;
        if (ba->alias && io == -1 && ba->io == io) 
        {
          ba->alias_parent = this;
          ((vsx_widget_connector_bezier*)tt)->alias_conn = true;
        }
        if (alias && io == 1 && ba->io == io) 
        {
          alias_parent = ba;
          alias_for_component = t->parts[3];
          alias_for_anchor = t->parts[4];
          ((vsx_widget_connector_bezier*)tt)->alias_conn = true;
        }
        tt->size.x = 1;
        tt->size.y = 1;
        tt->init();
        ((vsx_widget_connector_bezier*)tt)->order = vsx_string_helper::s2i(t->parts[5]);
        ((vsx_widget_connector_bezier*)tt)->receiving_focus = true;
        ((vsx_widget_connector_bezier*)tt)->destination = ba;
        ((vsx_widget_connector_bezier*)tt)->open = conn_open;

        ba->connectors.push_back(tt);
        connections.push_back(tt);
      }
    }
    return;
  }





  // command:
  //   param_disconnect_ok [component] [anchor] [destination_component] [destination-anchor]
  // source:
  //   engine
  if (t->cmd == "param_disconnect_ok")
  {
    for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) 
    {
      if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR)
      if (((vsx_widget_anchor*)((vsx_widget_connector_bezier*)(*children_iter))->destination)->component->name == t->parts[3])
      {
        if (((vsx_widget_anchor*)((vsx_widget_connector_bezier*)(*children_iter))->destination)->name == t->parts[4])
        {
          // this is the one. KILL IT!
          (*children_iter)->_delete();
          return;
        }
      }
    }
    return;
  }





  if (t->cmd == "in_param_spec" || t->cmd == "out_param_spec")
  {
    // determine in or out
    int l_io = 0;
    if (t->cmd == "in_param_spec")
      l_io = -1;
    else
      l_io = 1;


    // p[0]: in_param_spec
    // p[1]: osc2
    // p[2]: actual command
    if (t->parts.size() == 4) 
    {
      if (t->parts[3] == "c") 
      {
        if (l_io == -1)
        anchor_order[0] = 0;
        else
        anchor_order[1] = 0;
      }
    }
    vsx_nw_vector<vsx_string<> > add_c;
    vsx_string<>cd = t->parts[2];
    vsx_string<>cm = "";
    vsx_string<>cms = "";
    int state = 0;

    for (size_t i = 0; i < cd.size(); ++i) 
    {
      if (state == 0 && cd[i] != ',' && cd[i] != '{')
      {
        cm += cd[i];
      }

      if (cd[i] == '{') 
      { 
        ++state;
      }

      if (state > 0)
      {
        // add to the command to be sent further in
        if ( (cd[i] != '{' && cd[i] != '}') || state > 1)
        {
          cms += cd[i];
        }
      }
      if (cd[i] == '}') 
      { 
        state--;
      }

      if ((cd[i] == ',' || i == cd.size()-1) && state == 0 ) 
      {
        // we have our first part covered, and the command-sub
        // 1. find the name of this anchor
        add_c.clear();
        vsx_string<>deli = ":";
        vsx_string_helper::explode(cm, deli, add_c, -1);
        vsx_widget *tt = 0;
        // look for old anchor
        if ( ((vsx_widget_component*)component)->t_list.find(add_c[0]) != ((vsx_widget_component*)component)->t_list.end()) 
        {
          tt = ((vsx_widget_component*)component)->t_list[add_c[0]];
          if (l_io < 0) {
            ((vsx_widget_anchor*)tt)->a_order = anchor_order[0]++;
            ((vsx_widget_component*)component)->p_l_list_in[add_c[0]] = ((vsx_widget_component*)component)->t_list[add_c[0]];
          }
          else
          {
            ((vsx_widget_anchor*)tt)->a_order = anchor_order[1]++;
            ((vsx_widget_component*)component)->p_l_list_out[add_c[0]] = ((vsx_widget_component*)component)->t_list[add_c[0]];
          }
          ((vsx_widget_component*)component)->t_list.erase(add_c[0]);
        } 
        else 
        {
          // extra type info split
          vsx_nw_vector<vsx_string<> > type_info;
          vsx_string<>type_deli = "?";
          vsx_string_helper::explode(add_c[1],type_deli,type_info,2);
          tt = add(new vsx_widget_anchor,add_c[0]);
          if (type_info.size() == 2)
            ((vsx_widget_anchor*)tt)->p_type_suffix = type_info[1];

          ((vsx_widget_anchor*)tt)->p_type = type_info[0];
          if (!l_io)
            tt->pos.x = -size.x*1.5*0.8;
          else
            tt->pos.x = size.x*1.5*0.8;

          //tt->pos.y = ypos;
          tt->size.x = size.x*0.5;
          tt->size.y = size.x*0.5;
          tt->visible = 0;
          ((vsx_widget_anchor*)tt)->p_desc = cm;
          ((vsx_widget_anchor*)tt)->io = l_io;
          ((vsx_widget_anchor*)tt)->component = component;
          ((vsx_widget_anchor*)tt)->anchor_order[0] = 0;
          ((vsx_widget_anchor*)tt)->anchor_order[1] = 0;

          if (l_io < 0)
          {
            ((vsx_widget_component*)((vsx_widget_anchor*)tt)->component)->p_l_list_in[add_c[0]] = tt;
            ((vsx_widget_anchor*)tt)->a_order = anchor_order[0]++;
          }
          else
          {
            ((vsx_widget_component*)((vsx_widget_anchor*)tt)->component)->p_l_list_out[add_c[0]] = tt;
            ((vsx_widget_anchor*)tt)->a_order = anchor_order[1]++;
          }
        }

        if (cms.size()) 
        {
          p_def += add_c[1];
          ((vsx_widget_anchor*)tt)->p_def += add_c[1];
          ((vsx_widget_anchor*)tt)->p_def += "[";

          command_q_b.add_raw(t->cmd+" "+add_c[0]+" "+cms, true);
          tt->vsx_command_queue_b(this);
          ((vsx_widget_anchor*)tt)->p_def += "]";
        } 
        else 
        {
          p_def += add_c[1];
          ((vsx_widget_anchor*)tt)->p_def += add_c[1];
        }
        if (i != cd.size()-1) 
        {
          p_def += ",";
        }

        cms = "";
        cm = "";
      }
    }
    init_children();
    return;
  }





  if (t->cmd == "vsxl_load_script")
  {
    // vsxl param filter load
    command_q_b.add_raw("vsxl_pfl "+component->name+" "+name);
    component->vsx_command_queue_b(this);
    return;
  }





  if (t->cmd == "vsxl_remove_script") 
  {
    // vsxl param filter load
    command_q_b.add_raw("vsxl_pfr "+component->name+" "+name);
    component->vsx_command_queue_b(this);
    return;
  } 





  if (t->cmd == "vsxl_pfi_ok") 
  {
    vsxl_filter = true;
    return;
  } 





  if (t->cmd == "vsxl_pfr_ok") 
  {
    vsxl_filter = false;
    return;
  }





  if (t->cmd == "vsxl_pfl_s")
  {
    vsx_widget* tt = add(new vsx_widget_controller_editor,name+".edit");
    ((vsx_widget_controller_editor*)tt)->target_param = name;
    ((vsx_widget_controller_editor*)tt)->return_command = "ps64";
    ((vsx_widget_controller_editor*)tt)->return_component = this;
    tt->widget_type = VSX_WIDGET_TYPE_CONTROLLER;
    tt->set_render_type(render_3d);
    tt->set_font_size(0.002);
    tt->set_border(0.0005);
    tt->title = "VSXL [param filter] : "+component->name+"->"+name;
    ((vsx_widget_controller_editor*)tt)->return_command = "vsxl_pfi";
    ((vsx_widget_controller_editor*)tt)->return_component = this;
    ((vsx_widget_controller_editor*)tt)->load_text( vsx_string_helper::base64_decode(t->parts[3]));
    return;
  } 



  if (t->cmd == "pseq_p" && t->cmd_data == "remove") 
  {
    command_q_b.add_raw("pseq_p remove "+component->name+" "+name);
    parent->vsx_command_queue_b(this);
    return;
  } 





  if (t->cmd == "pseq_p_ok") 
  {
    if (t->parts[1] == "list" || t->parts[1] == "init") 
    {
      init_menu(false);
      sequenced = true;
      delete_controllers();
    } 
    else
    if (t->parts[1] == "remove") 
    {
      init_menu(true);
      sequenced = false;
    }
    return;
  }





  if (t->cmd == "vsxl_pfi")
  {
    command_q_b.add_raw("vsxl_pfi "+component->name+" "+name+" -1 "+t->parts[1]);
    component->vsx_command_queue_b(this);
    return;
  }



  if (t->cmd == "pseq_a_m")
  {
    command_q_b.add_raw("pseq_p add "+component->name+" "+name);
    component->vsx_command_queue_b(this);
    command_q_b.add_raw("sequence_menu");
    component->vsx_command_queue_b(this);
    return;
  }





  if (t->cmd == "seq_pool_add")
  {
    command_q_b.add_raw("seq_pool add_param "+component->name+" "+name);
    component->vsx_command_queue_b(this);
    return;
  }





  if (t->cmd == "pflag")
  {
    command_q_b.add_raw("pflag "+component->name+" "+name+" "+t->parts[1]+" "+t->parts[2]);
    component->vsx_command_queue_b(this);
    return;
  }





  if (t->cmd == "controller_seq_edit") 
  {
    for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) 
      if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONTROLLER)
        return;
    vsx_widget* tt = add(new vsx_widget_controller_sequence,name+".seq_edit");
    ((vsx_widget_controller_sequence*)tt)->target_param = name;
    ((vsx_widget_controller_sequence*)tt)->init();
    ((vsx_widget_controller_sequence*)tt)->set_param_type( vsx_param_helper::param_id_from_string(p_type) );
    tt->pos.x = tt->target_pos.x -= tt->target_size.x * 0.6f;
    return;
  }





  if (t->cmd == "controller_edit" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER))
  {
    vsx_widget* tt = add(new vsx_widget_controller_editor,name+".edit");
    tt->widget_type = VSX_WIDGET_TYPE_CONTROLLER;
    ((vsx_widget_controller_editor*)tt)->target_param = name;
    ((vsx_widget_controller_editor*)tt)->return_command = "ps64";
    ((vsx_widget_controller_editor*)tt)->return_component = this;
    tt->set_render_type(render_3d);
    tt->set_font_size(0.002);
    tt->set_border(0.0005);

    command_q_b.add_raw("pg64 "+component->name+" "+name+" "+vsx_string_helper::i2s(tt->id));
    component->vsx_command_queue_b(this);
    return;
  }





  if (t->cmd == "tg")
  {
    toggle();
    return;
  }





  if (t->cmd.find("controller") != -1)
  {
    if (sequenced || connections.size()) 
    {
      command_q_b.add_raw("alert_dialog Information " + vsx_string_helper::base64_encode("This is controlled by a sequence and thus can't be controlled this way."));
      parent->vsx_command_queue_b(this);
      return;
    }
    if (t->cmd == "controller_knob" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) 
    {
      vsx_widget* tt = add(new vsx_widget_controller_knob,name+".knob");
      ((vsx_widget_controller_knob*)tt)->target_param=name;
      ((vsx_widget_controller_knob*)tt)->param_spec = &options;
      ((vsx_widget_controller_knob*)tt)->init();
      a_focus = tt;
      k_focus = tt;
      return;
    } else
    if (t->cmd == "controller_ab" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) 
    {
      vsx_widget* tt = add(new vsx_widget_controller_ab,name+".rotation_ab");
      ((vsx_widget_controller_ab*)tt)->target_param = name;
      ((vsx_widget_controller_ab*)tt)->param_spec = &options;
      ((vsx_widget_controller_ab*)tt)->init();
      ((vsx_widget_controller_ab*)tt)->set_tuple_type( vsx_string_helper::s2i(t->cmd_data) );
      a_focus = tt;
      k_focus = tt;
    } else
    if (t->cmd == "controller_col" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) 
    {
      vsx_widget* tt = add(new vsx_widget_controller_color,name+".rotation_col");
      if (p_type == "float3") ((vsx_widget_controller_color*)tt)->type = 0;
      else ((vsx_widget_controller_color*)tt)->type = 1;
      ((vsx_widget_controller_color*)tt)->target_param=name;
      ((vsx_widget_controller_color*)tt)->init();
      a_focus = tt;
      k_focus = tt;
    } else
    if (t->cmd == "controller_pad" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) 
    {
      vsx_widget* tt = add(new vsx_widget_controller_pad,name+".pad");
      if (p_type == "float3") ((vsx_widget_controller_pad*)tt)->ptype = 0;
      else ((vsx_widget_controller_pad*)tt)->ptype = 1;
      ((vsx_widget_controller_knob*)tt)->param_spec = &options;
      ((vsx_widget_controller_pad*)tt)->init();
    } else
    if (t->cmd == "controller_slider" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) 
    {
      vsx_widget* tt = add(new vsx_widget_controller_channel,name+".mixer");
      ((vsx_widget_controller_channel*)tt)->target_param=name;
      ((vsx_widget_controller_channel*)tt)->param_spec = &options;
      ((vsx_widget_controller_channel*)tt)->init();
      a_focus = tt;
      k_focus = tt;
    } else
    if (t->cmd == "controller_slider_multi" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) 
    {
      vsx_widget* tt = add(new vsx_widget_controller_mixer,name+".mixer");
      ((vsx_widget_controller_mixer*)tt)->target_param=name;
      ((vsx_widget_controller_mixer*)tt)->nummixers = vsx_string_helper::s2i(t->cmd_data);
      ((vsx_widget_controller_mixer*)tt)->param_spec = &options;
      ((vsx_widget_controller_mixer*)tt)->init();
      a_focus = tt;
      k_focus = tt;
    } else
    if (t->cmd == "controller_resource") 
    {
      vsx_widget_ultra_chooser* chooser= (vsx_widget_ultra_chooser*)(((vsx_widget_server*)((vsx_widget_component*)component)->server)->resource_chooser);
      ((vsx_widget_server*)((vsx_widget_component*)component)->server)->front(chooser);
      chooser->mode = 1;
        chooser->message = "CHOOSE A RESOURCE BY DOUBLE-CLICKING ON IT";
      chooser->command = "chooser_ok";
      chooser->command_receiver = this;
      chooser->center_on_item("resources");
      chooser->show();
    }
    return;
  }





  if (t->cmd == "settings_dialog" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER))
  {
    vsx_widget* tt = add(new vsx_widget_controller_dialog,name+"."+t->parts[1]);
    ((vsx_widget_controller_dialog*)tt)->target_param=name;
    ((vsx_widget_controller_dialog*)tt)->in_param_spec=t->parts[2];
    tt->pos.x = -size.x*4;
    ((vsx_widget_controller_dialog*)tt)->init();
    return;
  }





  if (t->cmd == "enum")
  {
    command_q_b.add_raw("param_set "+component->name+" "+alias_owner->name+" "+t->parts[1]);
    component->vsx_command_queue_b(this);
    return;
  }





  if (t->cmd == "chooser_ok")
  {
    vsx_string<> ns = vsx_string_helper::base64_encode(
      "resources/" +
      vsx_string_helper::str_replace<char>(
        ";",
        "/",
        vsx_string_helper::base64_decode(t->parts[1])
      )
    );
    command_q_b.add_raw("ps64 "+component->name+" "+alias_owner->name+" "+ns);
    component->vsx_command_queue_b(this);
    return;
  }





  if (t->cmd == "param_set_default" && p_type != "complex")
  {
    command_q_b.add_raw(t->cmd+" "+component->name+" "+name);
    return;
  }





  if (t->cmd == "param_set_interpolate")
  {
    if (p_type == "complex")
    {
      command_q_b.add_raw("param_set_interpolate " + component->name + " " + t->parts[3] + " " + t->parts[1]+ " "+t->parts[2]);
      component	->vsx_command_queue_b(this);
    }
    else
    {
      command_q_b.add_raw("param_set_interpolate " + component->name + " " + name + " " + t->parts[1]+ " "+t->parts[2]);
      component->vsx_command_queue_b(this);
    }
    return;
  }





  if (t->cmd == "param_set" || t->cmd == "ps64")
  {
    if (p_type != "complex")
    {
      if (t->parts.size() == 4) 
      {
        command_q_b.add_raw(t->parts[0] + " " + component->name + " " + t->parts[3] + " " + t->parts[1]);
        component->vsx_command_queue_b(this);
      }
      else
      if (t->parts.size() == 3) 
      {
        command_q_b.add_raw(t->parts[0] + " " + component->name + " " + t->parts[1] + " " + t->parts[2]);
        component->vsx_command_queue_b(this);
      }
    }
    else
    {
      command_q_b.add_raw(t->parts[0] + " " + component->name + " " + name + " " + t->parts[1]);
      component->vsx_command_queue_b(this);
    }
    return;
  }





  if (t->cmd == "param_get" || t->cmd == "pg64")
  {
    command_q_b.add_raw(t->parts[0] + " " + component->name + " " + t->parts[1] + " " + t->parts[2]);
    component->vsx_command_queue_b(this);
    return;
  }





  if (t->cmd == "param_get_ok")
  {
    display_value = vsx_string_helper::base64_decode(t->parts[3]);
    vsx_nw_vector<vsx_string<> > parts;
    vsx_string<>deli = ",";
    if (p_type == "float")
    {
      display_value = vsx_string_helper::f2s(vsx_string_helper::s2f(display_value),5);
    }
    if (p_type == "float3")
    {
      vsx_string_helper::explode(display_value, deli, parts);
      if (parts.size() == 3)
        display_value = vsx_string_helper::f2s(vsx_string_helper::s2f(parts[0]),5)+","+vsx_string_helper::f2s(vsx_string_helper::s2f(parts[1]),5)+","+vsx_string_helper::f2s(vsx_string_helper::s2f(parts[2]),5);
    }
    if (p_type == "float4")
    {
      vsx_string_helper::explode(display_value,deli, parts);
      if (parts.size() == 4)
        display_value = vsx_string_helper::f2s(vsx_string_helper::s2f(parts[0]),5)+","+vsx_string_helper::f2s(vsx_string_helper::s2f(parts[1]),5)+","+vsx_string_helper::f2s(vsx_string_helper::s2f(parts[2]),5)+","+vsx_string_helper::f2s(vsx_string_helper::s2f(parts[3]),5);
    }
    if (p_type == "enum")
    {
      int ii = vsx_string_helper::s2i(display_value);

      if (ii >= 0 && ii < (int)enum_items.size())
      {
        display_value = enum_items[ vsx_string_helper::s2i(display_value) ];
      }
      else
      {
        display_value = "";
      }
    }
    if (display_value != "")
    {
      display_value += " ";
    }
    return;
  }


  vsx_widget::command_process_back_queue(t);
}


bool vsx_widget_anchor::connect(vsx_widget* other_anchor) {
  // we know that
  // - we're not the same type as the other component,
  // - we're the same type (wether or not a complex or normal)

  if (io == 1) 
  {
    return ((vsx_widget_anchor *)other_anchor)->connect(this);
  }
  if (p_type != "complex") 
  {
    if (((vsx_widget_anchor*)other_anchor)->component->name == component->name)
      return false;

    if (options.find("nc") != options.end())
    {
      a_focus->add(new dialog_messagebox("Error: connecting","Target parameter '"+name+"' is a config-only parameter."),"foo");
      return false;
    }

    for (std::list <vsx_widget*>::iterator it = connections.begin(); it != connections.end(); ++it) 
    {
      if ( ((vsx_widget_connector_bezier*)*it)->destination == other_anchor) 
      {
        return false;
      }
    }
    command_q_b.add_raw("param_connect "+component->name+" "+name+" "+((vsx_widget_anchor *)other_anchor)->component->name+" "+other_anchor->name);
    parent->vsx_command_queue_b(this);
  }
  if (!alias) connectors.clear();
  return false;
}


void vsx_widget_anchor::before_delete() 
{
  // go through our children and
  if (io == -1)
  {
    for (std::list <vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
      if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*it)->marked_for_deletion) {
        vsx_widget_connector_bezier* c = (vsx_widget_connector_bezier*)(*it);
        if (c->destination) 
        {
          if (((vsx_widget_anchor*)c->destination)->io == io) 
          {
            ((vsx_widget_anchor*)c->destination)->_delete();
          }
        }
        c->_delete();
        ((vsx_widget_component*)component)->macro_fix_anchors();
      }
    }
  }
  if (connectors.size()) 
  {
    std::list <vsx_widget*> connectors2 = connectors;
    for (std::list <vsx_widget*>::iterator it = connectors2.begin(); it != connectors2.end(); ++it) 
    {
      ((vsx_widget_connector_bezier*)(*it))->destination = 0;
      if (((vsx_widget_anchor*)(*it)->parent)->io == 1)
      {
        if (((vsx_widget_anchor*)(*it)->parent)->alias && ((vsx_widget_anchor*)(*it)->parent)->component->parent != component->parent) 
        {
          (*it)->parent->_delete();
          ((vsx_widget_component*)((vsx_widget_anchor*)(*it)->parent)->component)->macro_fix_anchors();
        }
      }
      (*it)->_delete();
    }
    connectors.clear();
  }
  if (io == 1)
  {
    ((vsx_widget_component*)component)->p_l_list_out.erase(name);
  }
  else
  {
    ((vsx_widget_component*)component)->p_l_list_in.erase(name);
  }
}


void vsx_widget_anchor::on_delete()
{
  mtex_d.reset(nullptr);
  mtex_blob.reset(nullptr);
  mtex_blob_small.reset(nullptr);
}


void vsx_widget_anchor::update_connection_order() 
{
  if (marked_for_deletion) return;
  std::map<int,vsx_widget*> connection_map;
  if (children.size()) 
  {
    for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it) 
    {
      if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*it)->marked_for_deletion) 
      {
        connection_map[((vsx_widget_connector_bezier*)*it)->order] = *it;
      }
    }
    // go through these in the sorted order and update the order
    int c = 0;
    for (std::map<int,vsx_widget*>::iterator it2 = connection_map.begin(); it2 != connection_map.end(); ++it2) 
    {
      ((vsx_widget_connector_bezier*)(*it2).second)->order = c;
      ++c;
    }
  }
}

void vsx_widget_anchor::get_connections_abs(vsx_widget* base, std::list<vsx_widget_connector_info*>* mlist) 
{
  if (io == -1) 
  {
    for (unsigned long i = 0; i < connections.size(); ++i) 
    {
      bool found = false;
      std::list<vsx_widget*>::iterator it;
      for (std::list<vsx_widget*>::iterator itb = connections.begin(); itb != connections.end(); ++itb) 
      {
        if (((vsx_widget_connector_bezier*)(*itb))->order == (int)i) 
        {
          it = itb;
          found = true;
        }
      }
      if (found && (*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*it)->marked_for_deletion) 
      {
        if (((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->destination)->io != io)
        {
          if (!((vsx_widget_component*)((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->destination)->component)->is_moving) 
          {
            // ok, we found a real connection, stop here
            vsx_widget_connector_info* connector_info = new vsx_widget_connector_info;
            connector_info->dest = base;
            connector_info->source = ((vsx_widget_connector_bezier*)*it)->destination;
            connector_info->order = ((vsx_widget_connector_bezier*)*it)->order;
            mlist->push_back(connector_info);
          }
        } else
        if (((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->destination)->alias) 
        {
          ((vsx_widget_anchor*)(((vsx_widget_connector_bezier*)*it)->destination))->get_connections_abs(base, mlist);
        }
      }
    }
  } else 
  {

    for (std::list <vsx_widget*>::iterator it = connectors.begin(); it != connectors.end(); ++it) 
    {
      if (!(*it)->marked_for_deletion) 
      {
        if (((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->parent)->io != io)
        {
          if (!((vsx_widget_component*)((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->parent)->component)->is_moving) 
          {
            // these are connections, add them to the outlist
            vsx_widget_connector_info* connector_info = new vsx_widget_connector_info;
            connector_info->source = base;
            connector_info->dest = ((vsx_widget_connector_bezier*)*it)->parent;
            connector_info->order = ((vsx_widget_connector_bezier*)*it)->order;
            mlist->push_back(connector_info);
          }
        } else
        if (((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->parent)->alias) 
        {
          ((vsx_widget_anchor*)(((vsx_widget_connector_bezier*)*it)->parent))->get_connections_abs(base, mlist);
        }
      }
    }
  }
}

void vsx_widget_anchor::disconnect_abs() 
{
  if (io == -1) 
  {
    for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) 
    {
      if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR)
      if (!((vsx_widget_component*)((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->destination)->component)->is_moving)
      {
        (*it)->_delete();
      }
    }
  } else 
  {
    std::list <vsx_widget*> connectors_temp = connectors;
    for (std::list <vsx_widget*>::iterator it = connectors_temp.begin(); it != connectors_temp.end(); ++it) 
    {
      if (!((vsx_widget_component*)((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->parent)->component)->is_moving)
      {
        if (
          ((vsx_widget_anchor*)(*it)->parent)->alias
          &&
          (((vsx_widget_anchor*)(*it)->parent)->component->parent !=
          ((vsx_widget_anchor*)(((vsx_widget_connector_bezier*)(*it))->destination))->component->parent)
        )
        {
          (*it)->parent->_delete();
          ((vsx_widget_component*)((vsx_widget_anchor*)(*it)->parent)->component)->macro_fix_anchors();
        }
        (*it)->_delete();
      }
    }
  }
}


void vsx_widget_anchor::param_connect_abs(vsx_string<>c_component, vsx_string<>c_param, int c_order) {
  // syntax:
  //   param_connect_ok [component] [param] [dest-comp] [dest-param] [order]
  // we're the first 2 parts of the command, so just find the other component and create the link
  // first find the server.
  vsx_widget_component *other_component = (vsx_widget_component*)((vsx_widget_server*)((vsx_widget_component*)component)->server)->find_component(c_component);
  if (other_component)
  {
    // ok now find the other anchor
    if (other_component->p_l_list_out.find(c_param) != other_component->p_l_list_out.end())
    {
      vsx_widget_anchor *other_anchor = (vsx_widget_anchor*)other_component->p_l_list_out[c_param];
      // delete controllers & check for existing connections
      for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) 
      {
        if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) 
        {
          if ( ((vsx_widget_connector_bezier*)(*children_iter))->destination == (vsx_widget*)other_anchor) return;
        }
        if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONTROLLER) (*children_iter)->_delete();
      }

      // add new connection
      vsx_widget_connector_bezier *connection = (vsx_widget_connector_bezier *)add(new vsx_widget_connector_bezier,name+":conn");

      connection->alias_conn = false;
      if (other_anchor->alias && io == -1 && other_anchor->io == io) 
      {
        other_anchor->alias_parent = this;
        connection->alias_conn = true;
      }
      if (alias && io == 1 && other_anchor->io == io) 
      {
        alias_parent = other_anchor;
        alias_for_component = c_component;
        alias_for_anchor = c_param;
        connection->alias_conn = true;
      }
      connection->size.x = 1;
      connection->size.y = 1;
      connection->init();

      connection->order = c_order;

      connection->receiving_focus = true;
      connection->destination = other_anchor;
      connection->open = conn_open;
      connections.push_back(connection);

      other_anchor->connectors.push_back(connection);
    }
  }
  return;
}

void vsx_widget_anchor::connect_far(vsx_widget_anchor* src, int corder, vsx_widget_anchor* referrer) 
{
  // 1. find out the common name for us, to see if we're at the end of the alias chain
  vsx_string<> component_name_source = src->component->name;
  vsx_string<> component_name_destination = component->name;
  vsx_string_helper::str_remove_equal_prefix(component_name_source, component_name_destination, ".");

  if (component_name_source == "" && src->alias)
    return connect_far(src->alias_parent, corder);

  if (component_name_destination == "" && alias)
  {
    int num_conn = 0;
    for (std::list<vsx_widget*>::iterator cit = children.begin(); cit != children.end(); ++cit)
    if ((*cit)->widget_type == VSX_WIDGET_TYPE_CONNECTOR)  ++num_conn;

    if (corder+1 > num_conn/2)
    {
      // find wich of the connections in the alias-parent go here
      int oo = 0;
      for (std::list<vsx_widget*>::iterator it = alias_parent->children.begin(); it != alias_parent->children.end(); ++it) 
      {
        if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) 
        {
          if (((vsx_widget_connector_bezier*)(*it))->destination == this) 
          {
            oo = ((vsx_widget_connector_bezier*)(*it))->order;
          }
        }
      }
      // second half of the alias, THIS IS AN APPROXIMATION
      return alias_parent->connect_far(src,oo,this);
    }
    else
    {
      // first half of the alias, THIS IS AN APPROXIMATION
      // find wich of the connections in the alias-parent go here
      int oo = 0;
      for (std::list<vsx_widget*>::iterator it = alias_parent->children.begin(); it != alias_parent->children.end(); ++it) 
      {
        if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) 
        {
          if (((vsx_widget_connector_bezier*)(*it))->destination == this) 
          {
          oo = ((vsx_widget_connector_bezier*)(*it))->order;
          }
        }
      }
      return alias_parent->connect_far(src, oo-1, this);
    }
  }
  vsx_string<>deli = ".";
  vsx_nw_vector<vsx_string<> > dest_name_parts;
  vsx_string_helper::explode(component_name_destination,deli,dest_name_parts);
  dest_name_parts.pop_back();
  component_name_destination = vsx_string_helper::implode(dest_name_parts,deli);

  vsx_nw_vector<vsx_string<> > src_name_parts;
  vsx_string_helper::explode(component_name_source,deli,src_name_parts);
  src_name_parts.pop_back();
  component_name_source = vsx_string_helper::implode(src_name_parts,deli);

  if (dest_name_parts.size() == 0)
  {
    // check if we can make a clean connection to the src
    // if not, ask src to build to this level.
    vsx_widget_anchor* new_src = src;
    if (component_name_source != "")
    {
      new_src = src->alias_to_level(this);
    }

    // we need the src to alias itself down to our level
    if (new_src) 
    {
      int num_connections = 0;

      for (std::list<vsx_widget*>::iterator cit = children.begin(); cit != children.end(); ++cit)
      if ((*cit)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*cit)->marked_for_deletion) 
      {
        ++num_connections;
      }
      if (!referrer) 
      {
        param_connect_abs(new_src->component->name,new_src->name,corder);
      } else 
      {
        if (corder == -1) 
        {
          param_connect_abs(new_src->component->name,new_src->name,-1);
        } else 
        {
          param_connect_abs(new_src->component->name,new_src->name,corder);
        }
      }
    }
  } else 
  {
    // go through our list of connections to find an alias
    vsx_widget_anchor* our_alias = 0;
    vsx_widget_connector_bezier* our_connection = 0;
    for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) 
    {
      //
      if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*it)->marked_for_deletion) 
      {
        if (
          ((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->destination)->alias
          &&  ((vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->destination)->io == io
        ) 
        {
          our_alias = (vsx_widget_anchor*)((vsx_widget_connector_bezier*)*it)->destination;
          our_connection = (vsx_widget_connector_bezier*)(*it);
        }
      }
    }

    if (our_alias) 
    {
      int neworder;
      if (corder >= 0) 
      {
        if (our_connection->order > corder) neworder = -1;  // put it in the very beginning
        else neworder = -2; // put it in the end
      } else neworder = corder;
      // alias already exists, send our request further down the chain
      our_alias->connect_far(src,neworder,this);
    } else 
    {

      // we need to create this alias
      int num_connections = 0;
      for (std::list<vsx_widget*>::iterator cit = children.begin(); cit != children.end(); ++cit)
      {
        if ((*cit)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*cit)->marked_for_deletion)
        {
          ++num_connections;
        }
      }
      vsx_string<>newname = ((vsx_widget_component*)component->parent)->alias_get_unique_name_out("alias_"+name);
      ((vsx_widget_server*)((vsx_widget_component*)component)->server)->param_alias_ok(
        newname+":"+p_def,
        vsx_string_helper::i2s(io),
        component->parent->name,
        newname,
        component->name,
        name, vsx_string_helper::i2s(corder)
      );

      vsx_widget_anchor* new_alias = ((vsx_widget_anchor*)((vsx_widget_component*)component->parent)->p_l_list_in["alias_"+name]);
      new_alias->connect_far(src, corder,this);
    }
  }
}

vsx_widget_anchor* vsx_widget_anchor::alias_to_level(vsx_widget_anchor* dest) 
{
  vsx_string<>src_name = component->name;
  vsx_string<>dest_name = dest->component->name;
  vsx_string_helper::str_remove_equal_prefix(src_name, dest_name, ".");

  vsx_string<>deli = ".";

  vsx_nw_vector<vsx_string<> > src_name_parts;
  vsx_string_helper::explode(src_name,deli,src_name_parts);
  src_name_parts.pop_back();
  src_name = vsx_string_helper::implode(src_name_parts,deli);
  vsx_widget_anchor* alias_found = 0;
  for (std::list <vsx_widget*>::iterator it = connectors.begin(); it != connectors.end(); ++it) 
  {
    if (!((vsx_widget_anchor*)((vsx_widget_connector_bezier*)(*it))->parent)->marked_for_deletion)
    if (((vsx_widget_anchor*)((vsx_widget_connector_bezier*)(*it))->parent)->alias)
    if (((vsx_widget_anchor*)((vsx_widget_connector_bezier*)(*it))->parent)->alias_parent == this)
    alias_found = (vsx_widget_anchor*)((vsx_widget_connector_bezier*)(*it))->parent;
  }

  if (alias_found) 
  {
    return alias_found->alias_to_level(dest);
  } else
  if (src_name_parts.size()) 
  {
    vsx_string<>newname = ((vsx_widget_component*)component->parent)->alias_get_unique_name_out("alias_"+name);
    ((vsx_widget_server*)((vsx_widget_component*)component)->server)->param_alias_ok(
      newname+":"+p_def,
      vsx_string_helper::i2s(io),
      component->parent->name,
      newname,
      component->name,
      name, "0"
    );
    return ((vsx_widget_anchor*)((vsx_widget_component*)component->parent)->p_l_list_out[newname])->alias_to_level(dest);
  }
  return this;
}

//-- p u r e   g u i   s t u f f ---------------------------------------------------------------------------------------

std::map<vsx_string<>,vsx_string<> > parse_url_params(vsx_string<>input, char major='&', char minor='=', char sublevelbegin='(', char sublevelend=')');

std::map<vsx_string<>,vsx_string<> > parse_url_params(vsx_string<>input, char major, char minor, char sublevelbegin, char sublevelend)
{
  VSX_UNUSED(major);
  VSX_UNUSED(minor);
  VSX_UNUSED(sublevelbegin);
  VSX_UNUSED(sublevelend);

  std::map<vsx_string<>,vsx_string<> > values;
  int startpos=0, sublevel=0;
  vsx_string<>key="",val="",strip="";
  int p=0;

  for (size_t i=0;i<input.size();++i)
  {
    if (input[i]=='(') ++sublevel;
    if (input[i]==')') --sublevel;
    if ((input[i]=='&' && sublevel==0) || i+1==input.size())
    {
      int x;
      i+1==input.size()?x=i+1:x=i-startpos;
      strip=input.substr(startpos,x);
      p=strip.find('=');
      if (p != -1)
      {
        key=strip.substr(0,p);
        if (key!="")
        {
          val=strip.substr(p+1);
          if (val[val.size()-1]==')') val.pop_back();
          if (val[0]=='(') val = val.substr(1);
          values[key]=val;
        }
      }
      startpos=i+1;
    }
  }
  return values;
}

void vsx_widget_anchor::init_menu(bool include_controllers) 
{
  if (menu)
    menu->_delete();
  vsx_widget_popup_menu* menu_ = new vsx_widget_popup_menu;
  menu_->size.x = 0.2;
  menu_->size.y = 0.5;
  if (alias) 
  {
    menu_->commands.adds(VSX_COMMAND_MENU, "unalias", "anchor_unalias","");
  }
  if (io == -1)
  {
    if (p_type == "float") 
    {
      if (!default_controller.size())
        default_controller = "controller_knob";

      if (include_controllers) 
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
        menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers---------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "knob", "controller_knob","");
        menu_->commands.adds(VSX_COMMAND_MENU, "slider", "controller_slider","");
      }
      menu_->commands.adds(VSX_COMMAND_MENU, "--Animation Clip ------", "","");
      menu_->commands.adds(VSX_COMMAND_MENU, "animation clip;add to current", "seq_pool_add","");
      menu_->commands.adds(VSX_COMMAND_MENU, "--Sequencer-----------", "","");
      menu_->commands.adds(VSX_COMMAND_MENU, "sequencer;add/edit sequence", "pseq_a_m","");
      menu_->commands.adds(VSX_COMMAND_MENU, "sequencer;remove sequence", "pseq_p","remove");
      menu_->commands.adds(VSX_COMMAND_MENU, "--VSXL scripting------", "","");
      menu_->commands.adds(VSX_COMMAND_MENU, "add/edit filter script", "vsxl_load_script","");
      menu_->commands.adds(VSX_COMMAND_MENU, "remove filter script", "vsxl_remove_script","");
      if (!alias)
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "--Sub-engine export--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "Disable export", "pflag","external_expose 0");
        menu_->commands.adds(VSX_COMMAND_MENU, "Enable export", "pflag","external_expose 1");
      }
    }
    else 
    if (p_type == "float3")
    {
      if (!default_controller.size()) 
      {
        default_controller = "controller_slider_multi 3";
      }
      if (include_controllers) 
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
        menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "sliders", "controller_slider_multi","3");
        menu_->commands.adds(VSX_COMMAND_MENU, "rotation axis sphere", "controller_ab","3");
        menu_->commands.adds(VSX_COMMAND_MENU, "color", "controller_col","");
        menu_->commands.adds(VSX_COMMAND_MENU, "pad", "controller_pad","");
      }
      if (!alias)
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "--Sub-engine export--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "Disable export", "pflag","external_expose 0");
        menu_->commands.adds(VSX_COMMAND_MENU, "Enable export", "pflag","external_expose 1");
      }
    }
    else if (p_type == "float4")
    {
      if (!default_controller.size())
      {
        default_controller = "controller_slider_multi 4";
      }

      if (include_controllers) 
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
        menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "sliders", "controller_slider_multi","4");
        menu_->commands.adds(VSX_COMMAND_MENU, "color", "controller_col","");
      }
      if (!alias)
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "--Sub-engine export--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "Disable export", "pflag","external_expose 0");
        menu_->commands.adds(VSX_COMMAND_MENU, "Enable export", "pflag","external_expose 1");
      }
    }
    else if (p_type == "quaternion")
    {
      if (!default_controller.size()) 
      {
        default_controller = "controller_slider_multi 4";
      }
      
      if (include_controllers) 
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
        menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "sliders", "controller_slider_multi","4");
        menu_->commands.adds(VSX_COMMAND_MENU, "rotation axis sphere", "controller_ab","4");
      }
      if (!alias)
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "--Sub-engine export--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "Disable export", "pflag","external_expose 0");
        menu_->commands.adds(VSX_COMMAND_MENU, "Enable export", "pflag","external_expose 1");
      }
    }
    else if (p_type == "resource")
    {
      if (!default_controller.size()) 
      {
        default_controller = "controller_resource";
      }
      if (include_controllers) 
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
        menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "resource library", "controller_resource","");
        menu_->commands.adds(VSX_COMMAND_MENU, "text editor", "controller_edit","");
      }
      if (!alias)
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "--Sub-engine export--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "Disable export", "pflag","external_expose 0");
        menu_->commands.adds(VSX_COMMAND_MENU, "Enable export", "pflag","external_expose 1");
      }
    }
    else 
    if (p_type == "string")
    {
      if (!default_controller.size()) 
      {
        default_controller = "controller_edit";
      }
      if (include_controllers) 
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
        menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "text editor", "controller_edit","");
        if (default_controller == "controller_resource") 
        {
          menu_->commands.adds(VSX_COMMAND_MENU, "resource library", "controller_resource","");
        }

        menu_->commands.adds(VSX_COMMAND_MENU, "--Animation Clip ------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "animation clip;add to current", "seq_pool_add","");
        menu_->commands.adds(VSX_COMMAND_MENU, "--Sequencer-----------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "sequencer;add/edit sequence", "pseq_a_m","");
        menu_->commands.adds(VSX_COMMAND_MENU, "sequencer;remove sequence", "pseq_p","remove");
      }
      if (!alias)
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "--Sub-engine export--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "Disable export", "pflag","external_expose 0");
        menu_->commands.adds(VSX_COMMAND_MENU, "Enable export", "pflag","external_expose 1");
      }
    }
    else
    if (p_type == "float_sequence" || p_type == "string_sequence")
    {
      menu_->commands.adds(VSX_COMMAND_MENU, "editor", "controller_seq_edit","");
      if (!alias)
      {
        menu_->commands.adds(VSX_COMMAND_MENU, "--Sub-engine export--------", "","");
        menu_->commands.adds(VSX_COMMAND_MENU, "Disable export", "pflag","external_expose 0");
        menu_->commands.adds(VSX_COMMAND_MENU, "Enable export", "pflag","external_expose 1");
      }
    }
    else 
    if (p_type == "complex")
    {
      menu_->commands.adds(VSX_COMMAND_MENU, "open/close (left-double-click)", "tg","");
      dialogs=parse_url_params(p_type_suffix);
      for (std::map<vsx_string<>,vsx_string<> >::iterator it = dialogs.begin(); it != dialogs.end(); ++it)
      {
        if (((vsx_string<>)(*it).first).substr(0,7) == "dialog_")
        {
          vsx_string<>name=((vsx_string<>)(*it).first).substr(7);
          menu_->commands.adds(VSX_COMMAND_MENU, "Dialogs;"+name, "settings_dialog",name+" "+(*it).second);
        }
      }
    }
    else if (p_type == "enum")
    {
      //menu = add(new vsx_widget_popup_menu,".anchor_menu");
      vsx_string<>deli_p = "&";
      vsx_nw_vector<vsx_string<> > parts;
      vsx_string_helper::explode(p_type_suffix,deli_p,parts);

      vsx_string<>deli = "|";
      vsx_nw_vector<vsx_string<> > enumerations;
      vsx_string_helper::explode(parts[0],deli,enumerations);
      foreach(enumerations, i)
      {
        menu_->commands.adds(VSX_COMMAND_MENU, enumerations[i],"enum",vsx_string_helper::i2s(i));
        enum_items.push_back(enumerations[i]);
      }
    }
  }

  if (!menu_->commands.count()) 
  {
    menu_->_delete();
  } else
  {
    menu = add(menu_,"menu");
    menu->init();
  }
}

void vsx_widget_anchor::init() 
{
  if (init_run) return;
  alias_owner = this;
  support_interpolation = true;
  alias_parent = 0;
  set_glow(false);

  // process options
  options = parse_url_params(p_type_suffix);
  help_text = "Data type:  "+p_type+"\n";
  if (options.find("min") != options.end()) 
  {
    help_text += "Minimum value: "+options["min"]+"\n";
  }
  if (options.find("max") != options.end()) 
  {
    help_text += "Maximum value:  "+options["max"]+"\n";
  }

  if (options.find("help") != options.end()) 
  {
    help_text += "\nParameter info (from the module):\n"+vsx_string_helper::base64_decode(options["help"]);
  }
  
  dialogs=parse_url_params(p_type_suffix);
  // default is to allow connections to this anchor
  forbid_connections = false;
  for (std::map<vsx_string<>,vsx_string<> >::iterator it = dialogs.begin(); it != dialogs.end(); ++it)
  {
    if ((*it).first == "default_controller")
    {
      default_controller = (*it).second;
    }
    if ((*it).first == "nc")
    {
      forbid_connections = true;
    }
  }

  menu = 0;
  init_menu(true); // include controllers in the menu
  init_children();
  anchor_order[0] = 0;
  anchor_order[1] = 0;

  title = name+":"+p_type;

  mtex_d = vsx_texture_loader::load(
    vsx_widget_skin::get_instance()->skin_path_get() + "datatypes/"+p_type+".dds",
    vsx::filesystem::get_instance(),
    true, // threaded
    0,
    vsx_texture_gl::linear_interpolate_hint
  );

  mtex_blob = vsx_texture_loader::load(
    vsx_widget_skin::get_instance()->skin_path_get() + "interface_extras/highlight_blob.png",
    vsx::filesystem::get_instance(),
    true, // threaded
    vsx_bitmap::flip_vertical_hint,
    vsx_texture_gl::linear_interpolate_hint
  );

  mtex_blob_small = vsx_texture_loader::load(
    vsx_widget_skin::get_instance()->skin_path_get() + "interface_extras/connection_blob.png",
    vsx::filesystem::get_instance(),
    true, // threaded
    vsx_bitmap::flip_vertical_hint,
    vsx_texture_gl::linear_interpolate_hint
  );

  color.r = 1.0f;
  color.g = 1.0f;
  color.b = 1.0f;
  color.a = 1.0f;
  size.y = size.x = 0;
  conn_open = false;
  tree_open = false;
  text_size = 0;
  init_run = true;
  if (parent->widget_type == VSX_WIDGET_TYPE_ANCHOR)
  visible = ((vsx_widget_anchor*)parent)->tree_open;
} // init

void vsx_widget_anchor::reinit() 
{
  vsx::filesystem filesystem;
  vsx_widget::reinit();
}


void vsx_widget_anchor::fix_anchors() 
{
  int i = 0;
  for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) 
  {
    if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_ANCHOR) 
    {
      ((vsx_widget_anchor*)(*children_iter))->fix_anchors();
      ((vsx_widget_anchor*)(*children_iter))->a_order = i;
      ++i;
    }
  }
}

void vsx_widget_anchor::set_glow(bool glow_status) 
{
  if (options.find("nc") != options.end()) 
  {
    return;
  }
  draw_glow = glow_status;
}

void vsx_widget_anchor::toggle(int override) 
{
  if (p_type != "complex") return;

  if (override == 1) 
  {
    tree_open = true; 
  }
  else
  if (override == 2) 
  {
    tree_open = false;
  }
  if (tree_open) 
  {
    hide_children();
    tree_open = false;
  }
  else
  {
    for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
    {
      if ((*it)->widget_type == VSX_WIDGET_TYPE_ANCHOR)
      {
        (*it)->visible = 1;

        if (((vsx_widget_anchor*)(*it))->p_type != "complex") 
        {
          (*it)->show_children();
          // go through to show any other things
          for (std::list<vsx_widget*>::iterator it2 = (*it)->children.begin(); it2 != (*it)->children.end(); ++it2) {
            (*it2)->show_children();
          }
        }
      }
    }
    tree_open = true;
    enumerate_children_get(1);
  }
} // toggle


bool vsx_widget_anchor::event_key_down(uint16_t key)
{
  VSX_UNUSED(key);
  return true;
}

void vsx_widget_anchor::get_value() 
{
  if (io == -1)
  {
    command_q_b.add_raw("param_get " + component->name+" "+name+" "+vsx_string_helper::i2s(id));
  }
  else
  {
    if (p_type != "render")
    {
      command_q_b.add_raw("pgo " + component->name+" "+name+" "+vsx_string_helper::i2s(id));
    }
  }
  component->vsx_command_queue_b(this);
}

bool vsx_widget_anchor::get_drag_status()
{
  return drag_status;
}


void vsx_widget_anchor::clone_our_value_to_other_anchor_via_server( const vsx_widget_anchor* other_anchor )
{
  if (!other_anchor)
    return;

  if (search_anchor->widget_type != VSX_WIDGET_TYPE_ANCHOR)
    return;

  if (p_type != ((vsx_widget_anchor*)other_anchor)->p_type)
    return;

  // send clone message to server
  command_q_b.add_raw
  (
    vsx_string<>("param_clone_value ") +
      component->name + " " +
      name + " " +
      other_anchor->component->name +" " +
      other_anchor->name
  );
  ((vsx_widget_component*)component)->server->vsx_command_queue_b(this);
}

void vsx_widget_anchor::delete_controllers() 
{
  for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) 
  {
    if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONTROLLER) (*children_iter)->_delete();
  }
}

void vsx_widget_anchor::enumerate_children_get(int override) 
{
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
  {
    if ((*it)->widget_type == VSX_WIDGET_TYPE_ANCHOR)
    {
      if (((vsx_widget_anchor*)(*it))->p_type != "complex") 
      {
        ((vsx_widget_anchor*)(*it))->display_value_t = 2;
        ((vsx_widget_anchor*)(*it))->text_size = 2;
        bool gvl = false;
        if (((vsx_widget_anchor*)(*it))->io == 1) 
        {
          gvl = true;
        }
        else
        if (((vsx_widget_anchor*)(*it))->io == -1 && ((vsx_widget_anchor*)(*it))->connections.size()) 
        {
          gvl = true;
        }

        if (gvl || override == 1)
        {
          ((vsx_widget_anchor*)(*it))->get_value();
        }
      }
    }
  }
}













void vsx_widget_anchor::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  if (button == 0)
  {
    m_focus = this;
    a_focus = this;
    k_focus = this;
    parent->front(this);

    if (p_type != "complex")
    {
      if (temp_drag_connector)
      {
        ((vsx_widget_connector_bezier*)temp_drag_connector)->receiving_focus = true;
        temp_drag_connector->_delete();
        temp_drag_connector = 0;
      }
      if (vsx_input_keyboard.pressed_ctrl() && vsx_input_keyboard.pressed_alt())
      {
        clone_value = true;
        drag_anchor = 0;
        drag_clone_anchor = this;
      } else
      {
        if (forbid_connections) return;
        if (io == -1 && connections.size() && p_type != "render") return;
        temp_drag_connector = add(new vsx_widget_connector_bezier,name+":ct");
        ((vsx_widget_connector_bezier*)temp_drag_connector)->receiving_focus = false;
        temp_drag_connector->size = vsx_vector3f(distance.center.x, distance.center.y);
        temp_drag_connector->init();
      }
    } else
    {
      if (parent == component)
      {
        ((vsx_widget_component*)component)->hide_all_complex_anchors_but_me(this);
      }
      else
      {
        toggle();
      }
    }
  } else
  {
    drag_status = false;
    ((vsxu_assistant*)((vsx_artiste_desktop*)root)->assistant)->temp_show();
    vsx_widget_connector_bezier::dim_alpha = 1.0f;
    vsx_widget_connector_bezier::receiving_focus = true;
    if (temp_drag_connector)
    {
      temp_drag_connector->_delete();
      temp_drag_connector = 0;
    }
    if (alias)
    {
      if (!menu)
      {
        menu = add(new vsx_widget_popup_menu,".alias_menu");
        menu->commands.adds(VSX_COMMAND_MENU, "unalias", "anchor_unalias","");
        menu->title = name;
        menu->init();
      }
    }
    vsx_widget::event_mouse_down(distance,coords,button);
  }
} // event_mouse_down



void vsx_widget_anchor::event_mouse_double_click(vsx_widget_distance distance, vsx_widget_coords coords, int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);

  if (button == 0)
  {
    {
      // if render type == render
      if (p_type == "render")
      {
        conn_open = !conn_open;
        int c = 0;
        for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
        {
          if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR)
          {
            if (((vsx_widget_connector_bezier*)(*children_iter))->destination)
            {
              ++c;
              ((vsx_widget_connector_bezier*)(*children_iter))->open = conn_open;
            }
          }
        }
        return;
      }

      if (p_type == "float_sequence" || p_type == "string_sequence")
      {
        command_q_b.add_raw("controller_seq_edit");
        this->vsx_command_queue_b(this);
        return;
      }


      if (vsx_input_keyboard.pressed_ctrl())
      {
        command_q_b.add_raw("seq_pool_add");
        this->vsx_command_queue_b(this);
      } else
      if (vsx_input_keyboard.pressed_alt())
      {
        command_q_b.add_raw("pseq_a_m");
        this->vsx_command_queue_b(this);
      }
      else
      if (vsx_input_keyboard.pressed_shift())
      {
      }
      else
      {
        if (sequenced)
        {
          command_q_b.add_raw("pseq_a_m");
          this->vsx_command_queue_b(this);
        } else
        if (default_controller.size())
        {
          command_q_b.add_raw(default_controller);
          vsx_command_queue_b(this);
        }
      }
    }
  }
}


void vsx_widget_anchor::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) 
{
  text_size = 2;
  if (temp_drag_connector || clone_value)
  {
    drag_status = true;
    drag_coords = coords;
    ((vsxu_assistant*)((vsx_artiste_desktop*)root)->assistant)->temp_hide();
    if (temp_drag_connector)
    {
      vsx_widget_connector_bezier::dim_alpha = 0.25f;
    }

    drag_anchor = this;
    if (temp_drag_connector)
    {
      temp_drag_connector->size.x = distance.center.x;
      temp_drag_connector->size.y = distance.center.y;
    }
    vsx_widget_distance distance2;
    search_anchor = root->find_component(coords,distance2);
    if (search_anchor) {
      if (search_anchor->widget_type == VSX_WIDGET_TYPE_ANCHOR) 
      {
        if (!((vsx_widget_anchor*)search_anchor)->tree_open) 
        {
          ((vsx_widget_anchor*)search_anchor)->toggle();
        }
        ((vsx_widget_anchor*)search_anchor)->text_size = 2.2;
        ((vsx_widget_anchor*)search_anchor)->display_value_t = 2.0f;
      }
    }
  }
}

void vsx_widget_anchor::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(coords);
  if (m_o_focus != this)
  {
    get_value();
    display_value_t = 2;
    text_size = 2;
  }
  parent->front(this);
}


void vsx_widget_anchor::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button) 
{
  // see if the component the mouse is over is a valid receiver
  if (button == 0) 
  {
    if (p_type == "enum")
    {
      if (menu) 
      {
        front(menu);
        menu->visible = 2;
        coords.screen_global.x -= menu->size.x;
        menu->pos = menu->target_pos = coords.screen_global;
      }
    }
    drag_status = false;
    ((vsxu_assistant*)((vsx_artiste_desktop*)root)->assistant)->temp_show();
    vsx_widget_connector_bezier::dim_alpha = 1.0f;
    vsx_widget_connector_bezier::receiving_focus = true;



    // handle cloning of values
    if (clone_value)
    {
      clone_our_value_to_other_anchor_via_server((vsx_widget_anchor*)search_anchor);
      clone_value = false;
      drag_clone_anchor = 0x0;
    }




    if (temp_drag_connector)
    {
      temp_drag_connector->_delete();
      temp_drag_connector = 0;
      if (search_anchor) 
      {
        if (search_anchor->widget_type == VSX_WIDGET_TYPE_ANCHOR) 
        {
          //make it so that the connector is owned by the in-anchor
          //check that the type of input is not the same as ours

          if
          (
            ((vsx_widget_anchor*)search_anchor)->io != io
            &&
            ((vsx_widget_anchor*)search_anchor)->forbid_connections == false
            &&
            p_type == ((vsx_widget_anchor*)search_anchor)->p_type
          )
          {
            if (((vsx_widget_anchor*)search_anchor)->io == -1)
            {
              connect(search_anchor);
            }
            else
            {
              ((vsx_widget_anchor*)search_anchor)->connect(this);
            }
          }
        } else
        if (search_anchor->widget_type == VSX_WIDGET_TYPE_COMPONENT && ((vsx_widget_component*)search_anchor)->component_type == "macro" && search_anchor == component->parent && m_focus == this) 
        {
          // syntax:
          //   param_alias [component] [parameter] [source_component] [source_parameter] [-1=in / 1=out]
          vsx_string<>tp;
          if (io > 0) tp = "1"; else tp = "-1";
          // TODO: support for complex parameters that should generate a lot more commands on this level.
          // in this case all aliasing has to be done at once so only one command..
          // so to do this all the aliases should be done like 1||2||3||4
          command_q_b.add_raw("param_alias alias_"+p_desc+" "+tp+" "+search_anchor->name+" alias_"+name+" "+component->name+" "+name+" ");
          parent->vsx_command_queue_b(this);
        }
      }// if tt
    }
  }
  vsx_widget::event_mouse_up(distance,coords,button);
}















void vsx_widget_anchor::pre_draw() 
{
  if (visible > 0) 
  {
    if (temp_drag_connector) if (a_focus != this)
    {
      ((vsx_widget_connector_bezier*)temp_drag_connector)->receiving_focus = true;
      temp_drag_connector->_delete();
      temp_drag_connector = 0;
    }

    // dim down component and other helpful stuff if dragging connection
    if (clone_value) 
    {
      component->color.a = 0.05f;
      if (drag_anchor && drag_anchor != this)
      {
        if (drag_anchor->io == io) 
        {
          if (drag_anchor->p_type == p_type) 
          {
            if (drag_anchor->component != component) 
            {
              set_glow(true);
              color.a = 1.0f;
              component->color.a = 0.7f;
              drag_size_mul = 1.2f;
            } else color.a = 0.55f;
          } else color.a = 0.35f;
        } else color.a = 0.05f;
      }
    } else
    if (drag_status) 
    {
      component->color.a = 0.05f;
      if (drag_anchor != this) 
      {
        if (drag_anchor->io != io) 
        {
          if (drag_anchor->p_type == p_type) 
          {
            if (drag_anchor->component != component) 
            {
              set_glow(true);
              color.a = 1.0f;
              component->color.a = 0.7f;
              drag_size_mul = 1.2f;
            } else color.a = 0.55f;
          } else color.a = 0.35f;
        } else color.a = 0.05f;
      }
    } else 
    {
      drag_size_mul = 1.0f;
      set_glow(false);
      component->color.a = 1.0f;
      color.a = 1.0;
    }

    pp = parent->get_pos_p();
    if (parent->widget_type == VSX_WIDGET_TYPE_COMPONENT) 
    {
      if ( ((vsx_widget_component*)component)->deleting )
      {
        target_size.x = 0.0f;
        interpolating_size = true;
        target_size.y = size.x;
      }
      else
      {
        target_size.x = 0.05f*0.15f*drag_size_mul;
        target_size.y = size.x;
      }
      pos.y = parent->size.y/2-(float)a_order*target_size.y*1.4f/drag_size_mul;
      pos.x = (float)io*parent->size.x*0.5f;
    } 
    else
    {
      size.x = parent->size.x*0.8;
      size.y = size.x;

      pos.x = (float)io*parent->size.x*1.4f;
      pos.y = parent->size.y/2-(float)a_order*size.y*1.4f;
      glLineWidth(1.0f);
      glBegin(GL_LINE_STRIP);
        glColor4f(1.0f,1.0f,1.0f,color.a);
        glVertex3f(pp.x,pp.y,parent->pos.z);
        glVertex3f(pp.x+pos.x, pp.y+pos.y, pos.z);
      glEnd();
    }
    ax = pp.x+pos.x;
    ay = pp.y+pos.y;
    sx = 1.8f*(size.x/2.0f);
    sy = 1.8f*(size.x/2.0f);
    if (!interpolating_size)
    {
      target_size = size;
    }
    target_pos = pos;
  }
}

void vsx_widget_anchor::draw() 
{
  if (visible > 0)
  {
    glColor4f(
      color.r,
      color.g,
      color.b,
      color.a
    );

    mtex_d->bind();
    draw_box_texf_correct(ax,ay,pos.z,sx,sy);
    mtex_d->_bind();

    if (vsxl_filter) 
    {
      glLineWidth(2.0);
      glColor3f(0,1,1);
      glBegin(GL_LINE_STRIP);
        glVertex3f(pp.x+pos.x+size.x/2,pp.y+pos.y-size.y/2,pos.z);
        glVertex3f(pp.x+pos.x+size.x/2,pp.y+pos.y+size.y/2,pos.z);
        glVertex3f(pp.x+pos.x-size.x/2,pp.y+pos.y+size.y/2,pos.z);
        glVertex3f(pp.x+pos.x-size.x/2, pp.y+pos.y-size.y/2,pos.z);
        glVertex3f(pp.x+pos.x+size.x/2,pp.y+pos.y-size.y/2,pos.z);
      glEnd();
    }
    if (sequenced) 
    {
      glLineWidth(2.0);
      glColor3f(0,1,0);
      glBegin(GL_LINE_STRIP);
        glVertex3f(pp.x+pos.x+size.x/2.5,pp.y+pos.y-size.y/2.5,pos.z);
        glVertex3f(pp.x+pos.x+size.x/2.5,pp.y+pos.y+size.y/2.5,pos.z);
        glVertex3f(pp.x+pos.x-size.x/2.5,pp.y+pos.y+size.y/2.5,pos.z);
        glVertex3f(pp.x+pos.x-size.x/2.5, pp.y+pos.y-size.y/2.5,pos.z);
        glVertex3f(pp.x+pos.x+size.x/2.5,pp.y+pos.y-size.y/2.5,pos.z);
      glEnd();
    }
    if (a_focus == this) 
    {
      glColor3f(1,1,1);
      glLineWidth(2);
      glBegin(GL_LINE_STRIP);
        glVertex3f(pp.x+pos.x+size.x/1.95,pp.y+pos.y-size.y/1.95,pos.z);
        glVertex3f(pp.x+pos.x+size.x/1.95,pp.y+pos.y+size.y/1.95,pos.z);
        glVertex3f(pp.x+pos.x-size.x/1.95,pp.y+pos.y+size.y/1.95,pos.z);
        glVertex3f(pp.x+pos.x-size.x/1.95, pp.y+pos.y-size.y/1.95,pos.z);
        glVertex3f(pp.x+pos.x+size.x/1.95,pp.y+pos.y-size.y/1.95,pos.z);
      glEnd();
    }

    text_size -= vsx_widget_time::get_instance()->get_dtime();
    if (drag_status) text_size = 0.5f;
    display_value_t -= vsx_widget_time::get_instance()->get_dtime();
    if (display_value_t < 0) display_value_t = 0;
    if (text_size < 0) text_size = 0;
    float d_size = display_value_t > 0.8f?0.8f:display_value_t;
    float t_size = text_size > 0.5f?0.5f:text_size;

    float sx06 = size.x * 0.6f;

    myf_size = font.get_size(name,size.x*(0.3+t_size));
    myf_pos = vsx_vector3<>(pos.x+pp.x+sx06,pos.y+pp.y-size.y*0.5);
    glColor4f(0.0f,0.0f,0.0f,0.5f*color.a);
    font.color.a = color.a;
    if (io == -1)
    {
      if (!drag_status) draw_box(myf_pos, myf_size.x, myf_size.y);
      font.print(myf_pos, name,size.x*(0.3+t_size));
    }
    else 
    {
      myf_pos.x -= size.x;
      if (!drag_status)
      draw_box(myf_pos-vsx_vector3<>(myf_size.x) , myf_size.x, myf_size.y);
      font.print_right(myf_pos, name,size.x*(0.3+t_size));
    }
    if (d_size > 0.3 && frames%7 == 1) 
    {
      if (m_o_focus == this) 
      {
        if (p_type == "complex" && tree_open) 
        {
          for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
          {
            if ((*it)->widget_type == VSX_WIDGET_TYPE_ANCHOR)
            {
              if (((vsx_widget_anchor*)(*it))->p_type != "complex") 
              {
                ((vsx_widget_anchor*)(*it))->display_value_t = 2;
                ((vsx_widget_anchor*)(*it))->text_size = 2;

                bool gvl = false;
                if (((vsx_widget_anchor*)(*it))->io == 1) 
                {
                  gvl = true;
                }
                else
                if (((vsx_widget_anchor*)(*it))->io == -1 && ((vsx_widget_anchor*)(*it))->connections.size()) 
                {
                  gvl = true;
                }
                if (gvl)
                {
                  ((vsx_widget_anchor*)(*it))->get_value();
                }
              }
            }
          }
        }
        display_value_t = text_size = 2;
      }
      bool gv = false;
      if (io == 1) 
      {
        gv = true;
      }
      else
      if (io == -1 && connections.size()) 
      {
        gv = true;
      }
      if (gv)
      {
        get_value();
      }
    }
    font.color.a = color.a;
    myf_size = font.get_size(display_value,size.x*d_size*0.8);
    if (d_size > 0.1) 
    {
      if (io == -1)
      {
        myf_pos.x -= size.x;
        font.print_right(myf_pos, display_value,size.x*(d_size)*0.8);
      }
      else
      {
        myf_pos.x += size.x;
        font.print(myf_pos, display_value,size.x*(d_size)*0.8);
      }
    }
  }
  if (draw_glow) 
  {
    glColor4f(0.8,0.8,1,0.8*(0.5+sin(vsx_widget_time::get_instance()->get_time()*15)*0.5)+0.2f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPushMatrix();
    glTranslatef(ax,ay,pos.z);
    glRotatef(vsx_widget_time::get_instance()->get_time() * 100,0,0,1);
    mtex_blob->bind();
      draw_box_texf(0,0,0,0.03f,0.03f);
    mtex_blob->_bind();
    glPopMatrix();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }
  pre_draw_children();
  draw_children();
  if (clone_value && drag_clone_anchor == this)
  {
    mtex_blob_small->bind();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      glColor4f(0.5f,1,1,1);
      draw_box_tex_c(drag_coords.world_global, 0.01f*(1.0f - fmod(vsx_widget_time::get_instance()->get_time()
                                                                  * 10.0f, 1.0f)),
                     0.01f*(1.0f - fmod(vsx_widget_time::get_instance()->get_time() * 10.0f, 1.0f)));
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mtex_blob_small->_bind();
  }
}

vsx_widget_anchor::vsx_widget_anchor() 
{
  alias = vsxl_filter = sequenced = tree_open = false;
  alias_for_component = alias_for_anchor = "";
  menu = search_anchor = temp_drag_connector = 0;
  display_value_t = 0.0f;
  widget_type = VSX_WIDGET_TYPE_ANCHOR;
}

vsx_vector3<> vsx_widget_anchor::get_pos_p()
{
  if (!visible) return parent->get_pos_p();

  vsx_vector3<> t;
  t.x = pos.x;
  t.y = pos.y;
  t.z = pos.z;
  vsx_vector3<> tt;
  if (parent != this) tt = parent->get_pos_p();
  t.z = pos.z;
  return t + tt;
}
