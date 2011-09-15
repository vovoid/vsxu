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
#include "vsx_widget_base.h"
#include "window/vsx_widget_window.h"
#include "vsx_widget_desktop.h"
#include "vsx_widget_anchor.h"
#include <vsx_command_client_server.h>
#include "vsx_widget_server.h"
#include "module_choosers/vsx_widget_module_chooser.h"
#include "vsx_widget_connector.h"
#include "helpers/vsx_widget_assistant.h"
//#include "vsx_widget_object_inspector.h"
#include "controllers/vsx_widget_base_controller.h"
#include "controllers/vsx_widget_controller.h"
#include "controllers/vsx_widget_controller_seq.h"
#include "controllers/vsx_widget_controller_ab.h"
#include "controllers/vsx_widget_editor.h"
//#include "vsx_widget_controller_ab.h"
//#include "vsx_widget_controller_seq.h"
#include "vsx_widget_comp.h"
#include "dialogs/vsx_widget_window_statics.h"
using namespace std;

bool vsx_widget_anchor::drag_status = false;
bool vsx_widget_anchor::clone_value = false;
vsx_vector vsx_widget_anchor::drag_position;
vsx_widget_anchor* vsx_widget_anchor::drag_anchor;
vsx_widget_anchor* vsx_widget_anchor::drag_clone_anchor;

//-- b a c k e n d -----------------------------------------------------------------------------------------------------


void vsx_widget_anchor::vsx_command_process_b(vsx_command_s *t) {
//    t->parse();
	if (t->cmd == "anchor_unalias") {
		command_q_b.add_raw("param_unalias "+i2s(io)+" "+component->name+" "+name);
	} else
	if (t->cmd == "connections_order_ok") {
		//syntax:
		//  connections_order_ok [component] [anchor] [specification]
		std::vector<vsx_string> order_list;
		vsx_string deli = ",";
		split_string(t->parts[3],deli,order_list);
		// set up a mapping list with the connections
		std::map<int,vsx_widget*> connection_map;
		for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
			if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) {
				connection_map[((vsx_widget_connector*)*children_iter)->order] = *children_iter;
			}
		}
		//std::list<vsx_widget*> cstack;

		int c = 0;
//      printf("new order: %s",t->parts[3].c_str());
		for (std::vector<vsx_string>::iterator it = order_list.begin(); it != order_list.end(); ++it) {
//      printf("oldorder: %d",((vsx_widget_connector*)connection_map[c])->order);
//      printf("order: %s",(*it).c_str());
//        cstack.push_back(connection_map[s2i(*it)]);
		((vsx_widget_connector*)connection_map[s2i(*it)])->order = c;
		((vsx_widget_connector*)connection_map[s2i(*it)])->move(vsx_vector(0));
		//((vsx_widget_connector*)connection_map[s2i(*it)])->pos.x = 0;
		//((vsx_widget_connector*)connection_map[s2i(*it)])->pos.y = 0;
	//printf("order-name: %s",((vsx_widget_connector*)connection_map[c])->destination->name.c_str());
		++c;
	}
} else
if (t->cmd == "connections_order_int") {
	std::map<float, vsx_widget*> position_map;
	// create the order-map. we're lucky as the maps always put the contents in order <3
	for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
		if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) {
			position_map[((vsx_widget_connector*)*children_iter)->real_pos.y] = *children_iter;
		}
	}
	std::vector<vsx_string> order_list;
	for (std::map<float,vsx_widget*>::reverse_iterator it = position_map.rbegin(); it != position_map.rend(); ++it) {
		order_list.push_back(i2s(((vsx_widget_connector*)((*it).second))->order));
	}
	vsx_string order_list_finished = implode(order_list,",");
	command_q_b.add_raw("connections_order "+component->name+" "+name+" "+order_list_finished);
	((vsx_widget_component*)component)->server->vsx_command_queue_b(this);
} else
if (t->cmd == "param_connect_ok") {
	//printf("param_connect_ok running order: %s\n",t->parts[5].c_str());
	// syntax:
	//   param_connect_ok [component] [param] [dest-comp] [dest-param] [order]
	// we're the first 2 parts of the command, so just find the other component and create the link
	// first find the server.

	vsx_widget_component *b = (vsx_widget_component*)((vsx_widget_server*)((vsx_widget_component*)component)->server)->find_component(t->parts[3]);
	if (b) {
		// ok now find the other anchor
		if (b->p_l_list_out.find(t->parts[4]) != b->p_l_list_out.end()) {
			vsx_widget_anchor *ba = (vsx_widget_anchor*)b->p_l_list_out[t->parts[4]];
			connect_far(ba,s2i(t->parts[5]));
			/*
			// delete controllers
			for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
				if ((*children_iter)->type == VSX_WIDGET_TYPE_CONTROLLER) root->command_q_f.add("delete",(*children_iter)->id);
			}
			int c_order = s2i(t->parts[5]);
			// add new connector
			vsx_widget *tt = add(new vsx_widget_connector,name+":conn");
			vsx_vector u;
			vsx_vector uu = get_pos_p();
			((vsx_widget_connector*)tt)->alias_conn = false;
			if (ba->alias && io == -1 && ba->io == io) {
				ba->alias_parent = this;
				((vsx_widget_connector*)tt)->alias_conn = true;
			}
			if (alias && io == 1 && ba->io == io) {
//        printf("setting alias parent for %s to %s\n",name.c_str(),ba->name.c_str());
				alias_parent = ba;
				alias_for_component = t->parts[3];
				alias_for_anchor = t->parts[4];
				((vsx_widget_connector*)tt)->alias_conn = true;
			}
			//u.x = world.x-pos.x;
			//u.y = world.y-pos.y;
			tt->size.x = 1;
			tt->size.y = 1;
			tt->init();
			((vsx_widget_connector*)tt)->order = c_order;
			//if (io == -1)
			//if (connections.size())
			//if (c_order == -1) {
				//for (std::list<vsx_widget*>::iterator it = connections.begin(); it != connections.end(); ++it) {
					//if (!(*it)->marked_for_deletion) {
						//++((vsx_widget_connector*)(*it))->order;
					//}
				//}
			//((vsx_widget_connector*)tt)->order = 0;
			//}

			//if (c_order > connections.size()) {
				//((vsx_widget_connector*)tt)->order = connections.size();
			//}
			((vsx_widget_connector*)tt)->receiving_focus = true;
			((vsx_widget_connector*)tt)->destination = ba;
			((vsx_widget_connector*)tt)->open = conn_open;
			connections.push_back(tt);

			ba->connectors.push_back(tt);
//        printf("%s builds connection to %s \n",name.c_str(),ba->name.c_str());
		} else printf("connection failed, command is: %s",t->raw.c_str());
		*/
		}
	}
	return;
} else

if (t->cmd == "pca") {
	//printf("pca running\n");
	// syntax:
	//   pca [component] [param] [dest-comp] [dest-param] [order]
	// we're the first 2 parts of the command, so just find the other component and create the link
	// first find the server.
	vsx_widget_component *b = (vsx_widget_component*)((vsx_widget_server*)((vsx_widget_component*)component)->server)->find_component(t->parts[3]);
	if (b) {
		// ok now find the other anchor
		vsx_widget_anchor *ba = 0;
		if (t->parts[6] == "-1") {
			if (b->p_l_list_in.find(t->parts[4]) != b->p_l_list_in.end()) {
				ba = (vsx_widget_anchor*)b->p_l_list_in[t->parts[4]];
			}// else printf("connection failed, command is: %s",t->raw.c_str());
		} else {
			if (b->p_l_list_out.find(t->parts[4]) != b->p_l_list_out.end()) {
				ba = (vsx_widget_anchor*)b->p_l_list_out[t->parts[4]];
			}// else printf("connection failed, command is: %s",t->raw.c_str());
		}
		// ok, all we need is available
		if (ba) {
			// delete controllers
			delete_controllers();
			// add new connector
			vsx_widget *tt = add(new vsx_widget_connector,name+":conn");
			vsx_vector u;
			vsx_vector uu = get_pos_p();
			((vsx_widget_connector*)tt)->alias_conn = false;
			if (ba->alias && io == -1 && ba->io == io) {
				ba->alias_parent = this;
				((vsx_widget_connector*)tt)->alias_conn = true;
			}
			if (alias && io == 1 && ba->io == io) {
				alias_parent = ba;
				alias_for_component = t->parts[3];
				alias_for_anchor = t->parts[4];
				((vsx_widget_connector*)tt)->alias_conn = true;
			}
			tt->size.x = 1;
			tt->size.y = 1;
			tt->init();
			((vsx_widget_connector*)tt)->order = s2i(t->parts[5]);
			((vsx_widget_connector*)tt)->receiving_focus = true;
			((vsx_widget_connector*)tt)->destination = ba;
			((vsx_widget_connector*)tt)->open = conn_open;

			ba->connectors.push_back(tt);
			connections.push_back(tt);
		}
	}
	return;
} else
if (t->cmd == "param_disconnect_ok") {
	// syntax:
	//  param_disconnect_ok [component] [anchor] [destination_component] [destination-anchor]
	for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
		if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR)
		if (((vsx_widget_anchor*)((vsx_widget_connector*)(*children_iter))->destination)->component->name == t->parts[3])
		{
			if (((vsx_widget_anchor*)((vsx_widget_connector*)(*children_iter))->destination)->name == t->parts[4])
			{
				// this is the one. KILL IT!

/*            printf("my-name: %s\n",name.c_str());
				printf("delete-name: %s\n",(*children_iter)->name.c_str());
				printf("parent name: %s\n",(*children_iter)->parent->name.c_str());
				printf("delete id: %d",(*children_iter)->id);*/
//            (*children_iter)->parent->children.remove(*children_iter);
 //       k_focus = (*children_iter)->parent;
 //       m_focus = (*children_iter)->parent;
//        a_focus = (*children_iter)->parent;
//            (*children_iter)->parent          l_list.erase(t->name);
//          glist.erase(t->name);
//          delete *children_iter;
				(*children_iter)->_delete();
				return;
			}
		}
	}
} else
if (t->cmd == "in_param_spec" || t->cmd == "out_param_spec") {
	int l_io = 0;
	if (t->cmd == "in_param_spec") l_io = -1; else l_io = 1;
	// p[0]: in_param_spec
	// p[1]: osc2
	// p[2]: actual command
	if (t->parts.size() == 4) {
		if (t->parts[3] == "c") {
			if (l_io == -1)
			anchor_order[0] = 0;
			else
			anchor_order[1] = 0;
		}
	}
	std::vector<vsx_string> add_c;
//    anchor_order[0] = 0;
//    anchor_order[1] = 0;
	vsx_string cd = t->parts[2];
		//printf("\nanchparsing %s\n",t->parts[2].c_str());
	vsx_string cm = "";
	vsx_string cms = "";
	int state = 0;
	//float ypos = size.y/2-size.y*0.8/2;
	for (int i = 0; i < cd.size(); ++i) {
		if (state == 0 && cd[i] != ',' && cd[i] != '{')
		cm += cd[i];

		if (cd[i] == '{') { ++state; }

		if (state > 0)
		{
			// add to the command to be sent further in
			if (cd[i] != '{' && cd[i] != '}' || state > 1)
			cms += cd[i];
		}
		if (cd[i] == '}') { state--; }

		if ((cd[i] == ',' || i == cd.size()-1) && state == 0 ) {
			// we have our first part covered, and the command-sub
			// 1. find the name of this anchor

			//printf("internal anchor parsing of %s",cm.c_str());
			add_c.clear();
			vsx_string deli = ":";
			split_string(cm,deli,add_c,-1);
			//printf("adding component %s\n",add_c[0].c_str());
			vsx_widget *tt = 0;
			// look for old anchor
			if ( ((vsx_widget_component*)component)->t_list.find(add_c[0]) != ((vsx_widget_component*)component)->t_list.end()) {
				//printf("found old anchor: %s\n",add_c[0].c_str());
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
			} else {
				// extra type info split
				std::vector<vsx_string> type_info;
				vsx_string type_deli = "?";
				split_string(add_c[1],type_deli,type_info,2);
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
					//if (add_c[1] != "complex")
          ((vsx_widget_component*)((vsx_widget_anchor*)tt)->component)->p_l_list_in[add_c[0]] = tt;
					((vsx_widget_anchor*)tt)->a_order = anchor_order[0]++;
				}
				else
				{
					//if (add_c[1] != "complex")
          ((vsx_widget_component*)((vsx_widget_anchor*)tt)->component)->p_l_list_out[add_c[0]] = tt;
					((vsx_widget_anchor*)tt)->a_order = anchor_order[1]++;
					//printf("new anchor has order %d",((vsx_widget_anchor*)tt)->order);
				}
			}

//          ((vsx_widget_anchor*)tt)->grid_open = false;
//          p_def+=",";
			if (cms.size()) {
				p_def += add_c[1];
				((vsx_widget_anchor*)tt)->p_def += add_c[1];
				((vsx_widget_anchor*)tt)->p_def += "[";

				command_q_b.add_raw(t->cmd+" "+add_c[0]+" "+cms);
				tt->vsx_command_queue_b(this);
				((vsx_widget_anchor*)tt)->p_def += "]";
			} else {
				p_def += add_c[1];
				((vsx_widget_anchor*)tt)->p_def += add_c[1];
			}
			if (i != cd.size()-1) p_def += ",";

			//ypos -= size.x;
			cms = "";
			cm = "";
		}
	}
	init_children();
	return;
} else
if (t->cmd == "vsxl_load_script") {
	// vsxl param filter load
	command_q_b.add_raw("vsxl_pfl "+component->name+" "+name);
	component->vsx_command_queue_b(this);
} else
if (t->cmd == "vsxl_remove_script") {
	// vsxl param filter load
	command_q_b.add_raw("vsxl_pfr "+component->name+" "+name);
	component->vsx_command_queue_b(this);
} else
if (t->cmd == "vsxl_pfi_ok") {
	//printf("setting vsxl filter to true\n");
	vsxl_filter = true;
} else
if (t->cmd == "vsxl_pfr_ok") {
	vsxl_filter = false;
} else
if (t->cmd == "vsxl_pfl_s") {
	//printf("adding editor\n");
	vsx_widget* tt = add(new vsx_widget_editor,name+".edit");
	((vsx_widget_editor*)tt)->target_param = name;
	((vsx_widget_editor*)tt)->return_command = "ps64";
	((vsx_widget_editor*)tt)->return_component = this;
	tt->widget_type = VSX_WIDGET_TYPE_CONTROLLER;
	tt->set_render_type(VSX_WIDGET_RENDER_3D);
	tt->set_font_size(0.002);
	tt->set_border(0.0005);


	//printf("done adding editor %s\n",t->parts[2].c_str());
	#ifndef VSXU_PLAYER
	tt->title = "VSXL [param filter] : "+component->name+"->"+name;
	#endif
	((vsx_widget_editor*)tt)->return_command = "vsxl_pfi";
	((vsx_widget_editor*)tt)->return_component = this;
	((vsx_widget_editor*)tt)->load_text(base64_decode(t->parts[3]));

} else
if (t->cmd == "pseq_p" && t->cmd_data == "remove") {
	command_q_b.add_raw("pseq_p remove "+component->name+" "+name);
	parent->vsx_command_queue_b(this);
} else
if (t->cmd == "pseq_p_ok") {
	if (t->parts[1] == "list" || t->parts[1] == "init") {
		init_menu(false);
		sequenced = true;
		delete_controllers();
	} else
	if (t->parts[1] == "remove") {
		init_menu(true);
		sequenced = false;
	}
} else
if (t->cmd == "vsxl_pfi") {
	command_q_b.add_raw("vsxl_pfi "+component->name+" "+name+" -1 "+t->parts[1]);
	component->vsx_command_queue_b(this);
} else
if (t->cmd == "pseq_a_m") {
	command_q_b.add_raw("pseq_p add "+component->name+" "+name);
	component->vsx_command_queue_b(this);
	command_q_b.add_raw("sequence_menu");
	component->vsx_command_queue_b(this);
} else
if (t->cmd == "seq_pool_add") {
	command_q_b.add_raw("seq_pool add_param "+component->name+" "+name);
	component->vsx_command_queue_b(this);
} else
/*  if (t->cmd == "pseq_i") {
	vsx_widget* tt = add(new vsx_widget_param_sequence,name+".sequence");
	((vsx_widget_param_sequence*)tt)->target_param = name;
	((vsx_widget_param_sequence*)tt)->target_comp = component->name;
	((vsx_widget_param_sequence*)tt)->init();
} else  */

if (t->cmd == "controller_seq_edit") {
	for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
    if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONTROLLER) return;
  }
	vsx_widget* tt = add(new vsx_widget_controller_sequence,name+".seq_edit");
	((vsx_widget_controller_sequence*)tt)->target_param=name;
	((vsx_widget_controller_sequence*)tt)->init();
	tt->pos.x = tt->target_pos.x -= tt->target_size.x * 0.6f;
	return;
} else

if (t->cmd == "controller_edit" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) {
	//printf("adding editor\n");
	vsx_widget* tt = add(new vsx_widget_editor,name+".edit");
	tt->widget_type = VSX_WIDGET_TYPE_CONTROLLER;
//  #ifndef VSXU_PLAYER
//    tt->title = "Edit string parameter: "+component->name+"->"+name;
//    #endif
	((vsx_widget_editor*)tt)->target_param = name;
	((vsx_widget_editor*)tt)->return_command = "ps64";
	((vsx_widget_editor*)tt)->return_component = this;
	tt->set_render_type(VSX_WIDGET_RENDER_3D);
	tt->set_font_size(0.002);
	tt->set_border(0.0005);

	command_q_b.add_raw("pg64 "+component->name+" "+name+" "+i2s(tt->id));
	component->vsx_command_queue_b(this);
	//printf("done adding editor\n");
//    a_focus = tt;
//    k_focus = tt;
} else
if (t->cmd == "tg") {
	toggle();
} else
if (t->cmd.find("controller") != -1) {
	if (sequenced || connections.size()) {
		command_q_b.add_raw("alert_dialog Information "+base64_encode("This is controlled by a sequence and thus can't be controlled this way."));
		parent->vsx_command_queue_b(this);
		return;
	}
	if (t->cmd == "controller_knob" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) {
		vsx_widget* tt = add(new vsx_widget_knob,name+".knob");
		((vsx_widget_knob*)tt)->target_param=name;
		((vsx_widget_knob*)tt)->param_spec = &options;
		((vsx_widget_knob*)tt)->init();
		a_focus = tt;
		k_focus = tt;
		return;
	} else
	if (t->cmd == "controller_ab" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) {
		vsx_widget* tt = add(new vsx_widget_controller_ab,name+".rotation_ab");
		((vsx_widget_controller_ab*)tt)->target_param = name;
		((vsx_widget_controller_ab*)tt)->param_spec = &options;
		((vsx_widget_controller_ab*)tt)->init();
		((vsx_widget_controller_ab*)tt)->type = s2i(t->cmd_data);
		a_focus = tt;
		k_focus = tt;
	} else
	if (t->cmd == "controller_col" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) {
		vsx_widget* tt = add(new vsx_widget_controller_color,name+".rotation_col");
		if (p_type == "float3") ((vsx_widget_controller_color*)tt)->type = 0;
		else ((vsx_widget_controller_color*)tt)->type = 1;
		((vsx_widget_controller_color*)tt)->target_param=name;
		((vsx_widget_controller_color*)tt)->init();
		a_focus = tt;
		k_focus = tt;
	} else
	if (t->cmd == "controller_pad" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) {
		vsx_widget* tt = add(new vsx_widget_controller_pad,name+".pad");
		if (p_type == "float3") ((vsx_widget_controller_pad*)tt)->ptype = 0;
		else ((vsx_widget_controller_pad*)tt)->ptype = 1;
		((vsx_widget_knob*)tt)->param_spec = &options;
		((vsx_widget_controller_pad*)tt)->init();
	} else
	if (t->cmd == "controller_slider" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) {
		vsx_widget* tt = add(new vsx_widget_channel,name+".mixer");
		((vsx_widget_channel*)tt)->target_param=name;
		((vsx_widget_channel*)tt)->param_spec = &options;
		((vsx_widget_channel*)tt)->init();
		a_focus = tt;
		k_focus = tt;
	} else
	if (t->cmd == "controller_slider_multi" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) {
			vsx_widget* tt = add(new vsx_widget_mixer,name+".mixer");
			((vsx_widget_mixer*)tt)->target_param=name;
			((vsx_widget_mixer*)tt)->nummixers = s2i(t->cmd_data);
			((vsx_widget_mixer*)tt)->param_spec = &options;
			((vsx_widget_mixer*)tt)->init();
		a_focus = tt;
		k_focus = tt;
	} else
	if (t->cmd == "controller_resource") {
		//printf("bringing up chooser\n");
		vsx_widget_ultra_chooser* chooser= (vsx_widget_ultra_chooser*)(((vsx_widget_server*)((vsx_widget_component*)component)->server)->resource_chooser);
		((vsx_widget_server*)((vsx_widget_component*)component)->server)->front(chooser);
		chooser->mode = 1;
		#ifndef VSXU_PLAYER
		chooser->message = "CHOOSE A RESOURCE BY DOUBLE-CLICKING ON IT";
		#endif
		chooser->command = "chooser_ok";
		chooser->command_receiver = this;
		chooser->center_on_item("resources");
		chooser->show();
	}
} else
if (t->cmd == "settings_dialog" && !find_child_by_type(VSX_WIDGET_TYPE_CONTROLLER)) {
		vsx_widget* tt = add(new vsx_widget_dialog,name+"."+t->parts.at(1));
		((vsx_widget_dialog*)tt)->target_param=name;
		((vsx_widget_dialog*)tt)->in_param_spec=t->parts.at(2);
		tt->pos.x = -size.x*4;
		((vsx_widget_dialog*)tt)->init();
} else
if (t->cmd == "enum") {
	command_q_b.add_raw("param_set "+component->name+" "+alias_owner->name+" "+t->parts[1]);
	component->vsx_command_queue_b(this);
} else
if (t->cmd == "chooser_ok") {
	//std::vector<vsx_string> parts;
	//vsx_string deli = ";";
	//explode(t->parts[1],deli, parts);
	vsx_string ns = base64_encode("resources/"+str_replace(";","/",base64_decode(t->parts[1])));
	//if (parts[0] == "resources") {
		command_q_b.add_raw("ps64 "+component->name+" "+alias_owner->name+" "+ns);
		component->vsx_command_queue_b(this);
	//}
} else
if (t->cmd == "param_set_interpolate") {
	if (p_type == "complex")
	{
		command_q_b.add_raw("param_set_interpolate " + component->name + " " + t->parts[3] + " " + t->parts[1]+ " "+t->parts[2]);
		component	->vsx_command_queue_b(this);
	}
	else
	{
		//printf("parts 2 : %s\n", t->parts[2].c_str());
		command_q_b.add_raw("param_set_interpolate " + component->name + " " + name + " " + t->parts[1]+ " "+t->parts[2]);
		component->vsx_command_queue_b(this);
	}
} else
if (t->cmd == "param_get_ok") {
	display_value = base64_decode(t->parts[3]);
	//printf("val: %s\n",display_value.c_str());
	std::vector<vsx_string> parts;
	vsx_string deli = ",";
	if (p_type == "float") {
		display_value = f2s(s2f(display_value),5);
	}
	if (p_type == "float3") {
		explode(display_value,deli, parts);
		if (parts.size() == 3)
		display_value = f2s(s2f(parts[0]),5)+","+f2s(s2f(parts[1]),5)+","+f2s(s2f(parts[2]),5);
	}
	if (p_type == "float4") {
		explode(display_value,deli, parts);
		if (parts.size() == 4)
		display_value = f2s(s2f(parts[0]),5)+","+f2s(s2f(parts[1]),5)+","+f2s(s2f(parts[2]),5)+","+f2s(s2f(parts[3]),5);
	}
	if (p_type == "enum") {

		int ii = s2i(display_value);
		//printf("ii: %d\n",ii);
		if (ii >= 0 && ii < (int)enum_items.size())
		display_value = enum_items[s2i(display_value)];
		else
		display_value = "";
	}
	if (display_value != "")
	display_value += " ";
} else
if (t->cmd == "param_set_default" && p_type != "complex") {
	command_q_b.add_raw(t->cmd+" "+component->name+" "+name);
} else
if (t->cmd == "param_set" || t->cmd == "ps64") {
	if (p_type != "complex")
	{
		//cout << t->parts[0] + " " + component->name + " " + t->parts[3] + " " + t->parts[1]<<endl;
		if (t->parts.size() == 4) {
			command_q_b.add_raw(t->parts[0] + " " + component->name + " " + t->parts[3] + " " + t->parts[1]);
			component->vsx_command_queue_b(this);
		} else
		if (t->parts.size() == 3) {
			#ifdef VSX_DEBUG
			printf("sending paramset to server\n");
			#endif
			command_q_b.add_raw(t->parts[0] + " " + component->name + " " + t->parts[1] + " " + t->parts[2]);
			component->vsx_command_queue_b(this);
		}
	}
	else
	{
//        printf("command: %s\n",t->raw.c_str());
//        cout << t->parts[0] + " " + component->name + " " + name + " " + t->parts[1]<<endl;
		command_q_b.add_raw(t->parts[0] + " " + component->name + " " + name + " " + t->parts[1]);
		component->vsx_command_queue_b(this);
	}
} else
if (t->cmd == "param_get" || t->cmd == "pg64") {

	command_q_b.add_raw(t->parts[0] + " " + component->name + " " + t->parts[1] + " " + t->parts[2]);
	component->vsx_command_queue_b(this);
} else
vsx_widget::vsx_command_process_b(t);
}

bool vsx_widget_anchor::connect(vsx_widget* other_anchor) {
	// we know that
	// - we're not the same type as the other component,
	// - we're the same type (wether or not a complex or normal)
//    printf("anchor connect\n");
	if (io == 1) {
		return ((vsx_widget_anchor *)other_anchor)->connect(this);
	}
	if (p_type != "complex") {
		if (((vsx_widget_anchor*)other_anchor)->component->name == component->name) return false;
		#ifdef VSX_DEBUG
		printf("connecting other %s\n",name.c_str());
		#endif

		for (map<vsx_string,vsx_string>::iterator ito = options.begin(); ito != options.end(); ++ito) {
			#ifdef VSX_DEBUG
			printf("options: %s\n",(*ito).second.c_str());
			#endif
		}

		if (options.find("nc") != options.end()) {
			a_focus->add(new dialog_messagebox("Error: connecting","Target parameter '"+name+"' is a config-only parameter."),"foo");
			return false;
		}

		for (std::list <vsx_widget*>::iterator it = connections.begin(); it != connections.end(); ++it) {
			if ( ((vsx_widget_connector*)*it)->destination == other_anchor) {
				//command_q_b.add_raw("alert_dialog foo error "+base64_encode("Double connection!"));
				return false;
			}
		}
		/*if (((vsx_widget_anchor*)other_anchor)->component->parent != component->parent) {
			command_q_b.add_raw("param_connect_far "+component->name+" "+name+" "+((vsx_widget_anchor *)other_anchor)->component->name+" "+other_anchor->name);
		} else*/
		command_q_b.add_raw("param_connect "+component->name+" "+name+" "+((vsx_widget_anchor *)other_anchor)->component->name+" "+other_anchor->name);
		parent->vsx_command_queue_b(this);
	}
	if (!alias) connectors.clear();
	return false;
}// else printf("no connectors\n");


void vsx_widget_anchor::before_delete() {
	#ifdef VSX_DEBUG
	printf("before_delete anchor: %s\n",name.c_str());
	#endif
		// go through our children and
		if (io == -1)
		for (std::list <vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
			if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*it)->marked_for_deletion) {
				vsx_widget_connector* c = (vsx_widget_connector*)(*it);
//          printf("left dealing with %s\n",c->name.c_str());
				if (c->destination) {
					if (((vsx_widget_anchor*)c->destination)->io == io) ((vsx_widget_anchor*)c->destination)->_delete();
				}
//          ((vsx_widget_anchor*)c->destination)->connectors.remove(c);
				//c->destination = 0;
				c->_delete();
				((vsx_widget_component*)component)->macro_fix_anchors();
			}
		}
	if (connectors.size()) {
		std::list <vsx_widget*> connectors2 = connectors;
		#ifdef VSX_DEBUG
		printf("connectors > 1 %d\n",connectors2.size());
		#endif
		for (std::list <vsx_widget*>::iterator it = connectors2.begin(); it != connectors2.end(); ++it) {
//        if ((*it)->type == "vsx_widget_connector") {
				((vsx_widget_connector*)(*it))->destination = 0;
//          printf("connector: %s\n",(*it)->name.c_str());
//          printf("io: %d\n",io);
				 //(*it)->parent->children.remove((*it));
				//(*it)->marked_for_deletion = true;
				if (((vsx_widget_anchor*)(*it)->parent)->io == 1)
				{
//            printf("connector: %s\n",(*it)->name.c_str());
					#ifdef VSX_DEBUG
					printf("parent: %s\n",(*it)->parent->name.c_str());
					#endif
					if (((vsx_widget_anchor*)(*it)->parent)->alias && ((vsx_widget_anchor*)(*it)->parent)->component->parent != component->parent) {
						#ifdef VSX_DEBUG
						printf("asking %s to delete itself\n",(*it)->parent->name.c_str());
						#endif
						(*it)->parent->_delete();
						#ifdef VSX_DEBUG
						printf("anchor fix %s\n",((vsx_widget_component*)((vsx_widget_anchor*)(*it)->parent)->component)->name.c_str());
						#endif
						((vsx_widget_component*)((vsx_widget_anchor*)(*it)->parent)->component)->macro_fix_anchors();
					}
				}

				//((vsx_widget_connector*)(*it))->before_delete();
				//((vsx_widget_connector*)(*it))->on_delete();
				//delete (*it);
				#ifdef VSX_DEBUG
				printf("before _delete for connector\n");
				#endif
				(*it)->_delete();
			}
//      }
		//if (!component->marked_for_deletion)
		connectors.clear();
	}
	if (io == 1)
	((vsx_widget_component*)component)->p_l_list_out.erase(name);
	else
	((vsx_widget_component*)component)->p_l_list_in.erase(name);
}

void vsx_widget_anchor::update_connection_order() {
//  printf("update_connection_order");
  if (marked_for_deletion) return;
  std::map<int,vsx_widget*> connection_map;
  if (children.size()) {
    for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it) {
      if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*it)->marked_for_deletion) {
        connection_map[((vsx_widget_connector*)*it)->order] = *it;
      }
    }
    // go through these in the sorted order and update the order
    int c = 0;
    for (std::map<int,vsx_widget*>::iterator it2 = connection_map.begin(); it2 != connection_map.end(); ++it2) {
      ((vsx_widget_connector*)(*it2).second)->order = c;
      ++c;
    }
  }
}

void vsx_widget_anchor::get_connections_abs(vsx_widget* base, std::list<vsx_widget_connector_info*>* mlist) {
  //printf("anchor, get_connections_abs %s\n",name.c_str());
  if (io == -1) {
    //printf("connsize: %d\n",connections.size());
    for (unsigned long i = 0; i < connections.size(); ++i) {
      //printf("koko %d\n",i);
      bool found = false;
      std::list<vsx_widget*>::iterator it;
      for (std::list<vsx_widget*>::iterator itb = connections.begin(); itb != connections.end(); ++itb) {
        if (((vsx_widget_connector*)(*itb))->order == (int)i) {
          it = itb;
          found = true;
        }
      }
      //
      if (found)
      if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*it)->marked_for_deletion) {
//        printf("dest: %s\n",((vsx_widget_connector*)*it)->destination->name.c_str());
        if (((vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination)->io != io)
        {
          if (!((vsx_widget_component*)((vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination)->component)->is_moving) {
            // ok, we found a real connection, stop here
            vsx_widget_connector_info* connector_info = new vsx_widget_connector_info;
            connector_info->dest = base;
            connector_info->source = ((vsx_widget_connector*)*it)->destination;
            connector_info->order = ((vsx_widget_connector*)*it)->order;
            mlist->push_back(connector_info);
          }
        } else
        if (((vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination)->alias) {
          ((vsx_widget_anchor*)(((vsx_widget_connector*)*it)->destination))->get_connections_abs(base, mlist);
        }
      }
    }
  } else {
    /*for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); it++) {
      //
      printf("outconnector %s\n",(*it)->name.c_str());
      if ((*it)->type == "vsx_widget_connector") {
        printf("dest: %s\n",((vsx_widget_connector*)*it)->destination->name.c_str());
        if (((vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination)->io != io) {
          printf("outconnector:2\n");
          // ok, we found a real connection, stop here
          vsx_widget_connector_info* connector_info = new vsx_widget_connector_info;
          connector_info->dest = base;
          connector_info->source = ((vsx_widget_connector*)*it)->destination;
          connector_info->order = ((vsx_widget_connector*)*it)->order;
          out_list->push_back(connector_info);
        } else
        if (((vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination)->alias) {
          ((vsx_widget_anchor*)(((vsx_widget_connector*)*it)->destination))->get_connections_abs(base, in_list,out_list);
        }
      }
    }*/
    for (std::list <vsx_widget*>::iterator it = connectors.begin(); it != connectors.end(); ++it) {
//      printf("outconnector %s\n",(*it)->name.c_str());
      if (!(*it)->marked_for_deletion) {
        if (((vsx_widget_anchor*)((vsx_widget_connector*)*it)->parent)->io != io)
        {
          if (!((vsx_widget_component*)((vsx_widget_anchor*)((vsx_widget_connector*)*it)->parent)->component)->is_moving) {
            // these are connections, add them to the outlist
            vsx_widget_connector_info* connector_info = new vsx_widget_connector_info;
            connector_info->source = base;
            connector_info->dest = ((vsx_widget_connector*)*it)->parent;
            connector_info->order = ((vsx_widget_connector*)*it)->order;
            mlist->push_back(connector_info);
          }
        } else
        if (((vsx_widget_anchor*)((vsx_widget_connector*)*it)->parent)->alias) {
          ((vsx_widget_anchor*)(((vsx_widget_connector*)*it)->parent))->get_connections_abs(base, mlist);
        }
      }
    }
  }
}

void vsx_widget_anchor::disconnect_abs() {
//  printf("anchor::disconnect_abs %s\n",name.c_str());
  if (io == -1) {
    for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
      //
      if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR)
      if (!((vsx_widget_component*)((vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination)->component)->is_moving)
      {
        //printf("abs_disconnect -1 %s\n",(*it)->name.c_str());
        (*it)->_delete();
      }
    }
  } else {
    std::list <vsx_widget*> connectors_temp = connectors;
    for (std::list <vsx_widget*>::iterator it = connectors_temp.begin(); it != connectors_temp.end(); ++it) {
//      printf("abs_disconnect +1 %s\n",(*it)->name.c_str());
      if (!((vsx_widget_component*)((vsx_widget_anchor*)((vsx_widget_connector*)*it)->parent)->component)->is_moving)
      {
        if (
            ((vsx_widget_anchor*)(*it)->parent)->alias
          &&
            (((vsx_widget_anchor*)(*it)->parent)->component->parent !=
            ((vsx_widget_anchor*)(((vsx_widget_connector*)(*it))->destination))->component->parent)
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

void vsx_widget_anchor::fix_connection_order() {
  //std::vector<vsx_widget*> ll;
  //int c = 0;
  //for (std::list<vsx_widget*>::iterator it = connections.begin(); it != connections.end(); ++it) {
//    if ((*it)->type == "vsx_widget_connector") {
//      ((vsx_widget_connector*)(*it))->order = c;
//      ++c;
//    }
//  }
}

void vsx_widget_anchor::param_connect_abs(vsx_string c_component, vsx_string c_param, int c_order) {
    //printf("param_connect_abs running order: %d\n",c_order);
    //printf("connections.size = %d\n",connections.size());
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
        for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
          if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) {
            if ( ((vsx_widget_connector*)(*children_iter))->destination == (vsx_widget*)other_anchor) return;
          }
          if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONTROLLER) (*children_iter)->_delete();//root->command_q_f.add("delete",(*children_iter)->id);
        }
        // add new connection
        vsx_widget_connector *connection = (vsx_widget_connector *)add(new vsx_widget_connector,name+":conn");

        connection->alias_conn = false;
        if (other_anchor->alias && io == -1 && other_anchor->io == io) {
          other_anchor->alias_parent = this;
          connection->alias_conn = true;
        }
        if (alias && io == 1 && other_anchor->io == io) {
  //        printf("setting alias parent for %s to %s\n",name.c_str(),ba->name.c_str());
          alias_parent = other_anchor;
          alias_for_component = c_component;
          alias_for_anchor = c_param;
          connection->alias_conn = true;
        }

        //u.x = world.x-pos.x;
        //u.y = world.y-pos.y;
        connection->size.x = 1;
        connection->size.y = 1;
        connection->init();

        connection->order = c_order;
				/*
        if (!connections.size() && io == -1) {
        	if (c_order == -1) {

        	} else
        	if (c_order == -2) {
        		printf("got c_order == -2\n");
        		c_order = 0;
        	} else
        	{
          	((vsx_widget_connector*)tt)->order = c_order;
        	}
        }

        if (io == -1 && connections.size())
        {
					if (c_order == -1)
					{
						for (std::list<vsx_widget*>::iterator it = connections.begin(); it != connections.end(); ++it)
						{
							if (!(*it)->marked_for_deletion)
							{
								++((vsx_widget_connector*)(*it))->order;
							}
						}
						c_order = ((vsx_widget_connector*)tt)->order = 0;
					}
					else
					if (c_order == -2)
					{
						printf("got c_order == -2\n");
						c_order = ((vsx_widget_connector*)tt)->order = connections.size();
					}
					else
					{
						//((vsx_widget_connector*)tt)->order = c_order;
						//for (std::list<vsx_widget*>::iterator it = connections.begin(); it != connections.end(); ++it)
						//{
							//if (!(*it)->marked_for_deletion)
							//{
								//if (((vsx_widget_connector*)(*it))->order != c_order && ((vsx_widget_connector*)(*it))->order > c_order)
								//{
								//	++((vsx_widget_connector*)(*it))->order;
								//}
							//}
							//++((vsx_widget_connector*)tt)->order;
						//}
					}
        }
				*/
        /*if (((vsx_widget_connector*)tt)->order > (int)connections.size()) {
          ((vsx_widget_connector*)tt)->order = connections.size();
        }*/
        connection->receiving_focus = true;
        connection->destination = other_anchor;
        connection->open = conn_open;
        connections.push_back(connection);

        other_anchor->connectors.push_back(connection);
//        printf("%s builds connection to %s \n",name.c_str(),ba->name.c_str());
      } //else printf("connection failed, command is: %s",t->raw.c_str());
    }
    return;
}

void vsx_widget_anchor::connect_far(vsx_widget_anchor* src, int corder, vsx_widget_anchor* referrer) {
  //printf("vsx_widget_anchor::connect_far %s owned by %s order %d\n",name.c_str(),component->name.c_str(),corder);
  // 1. find out the common name for us, to see if we're at the end of the alias chain
  vsx_string src_name = src->component->name;
  vsx_string dest_name = component->name;
//  printf("src_name = %s\n",src_name.c_str());
//  printf("dest_name = %s\n",dest_name.c_str());
  str_remove_equal_prefix(&src_name, &dest_name, ".");
//  printf("src_name = %s\n",src_name.c_str());
//  printf("dest_name = %s\n",dest_name.c_str());
//  printf("'''''''''''\n");
  if (src_name == "" && src->alias) {
//    printf("moment 22\n");
//    cout << "alias_parent name: "+src->alias_parent->name <<endl;
    return connect_far(src->alias_parent,corder);
  } else
  if (dest_name == "" && alias) {
//    printf("moment dest 22 -- alias parent is %s\n",alias_parent->name.c_str());
    int num_conn = 0;
    for (std::list<vsx_widget*>::iterator cit = children.begin(); cit != children.end(); ++cit)
    if ((*cit)->widget_type == VSX_WIDGET_TYPE_CONNECTOR)  ++num_conn;

    //if (!referrer) {
      if (corder+1 > num_conn/2)
      {
        //printf("second half of alias\n");
        // find wich of the connections in the alias-parent go here
        int oo = 0;
        for (std::list<vsx_widget*>::iterator it = alias_parent->children.begin(); it != alias_parent->children.end(); ++it) {
          if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) {
            if (((vsx_widget_connector*)(*it))->destination == this) {
              oo = ((vsx_widget_connector*)(*it))->order;
              //printf("found oo, %d\n",oo);
              //it = alias_parent->children.end();
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
        //printf("first half of alias\n");
        int oo = 0;
        for (std::list<vsx_widget*>::iterator it = alias_parent->children.begin(); it != alias_parent->children.end(); ++it) {
          if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) {
            if (((vsx_widget_connector*)(*it))->destination == this) {
              oo = ((vsx_widget_connector*)(*it))->order;
              //printf("found oo %d\n",oo);
              //it = alias_parent->children.end();
            }
          }
        }
        //if (oo == 0)
        //return alias_parent->connect_far(src,0,this);
        //else
        return alias_parent->connect_far(src, oo-1, this);
      }
    //} else {

//      return alias_parent->connect_far(src,corder,this);
//    }
  }
  vsx_string deli = ".";
  std::vector<vsx_string> dest_name_parts;
  explode(dest_name,deli,dest_name_parts);
  dest_name_parts.pop_back();
  dest_name = implode(dest_name_parts,deli);

  std::vector<vsx_string> src_name_parts;
  explode(src_name,deli,src_name_parts);
  src_name_parts.pop_back();
  src_name = implode(src_name_parts,deli);

  if (dest_name_parts.size() == 0) {
    // check if we can make a clean connection to the src
    // if not, ask src to build to this level.
    vsx_widget_anchor* new_src = src;
    if (src_name != "") {
//      printf("we need the src to alias itself down to our level\n");
      new_src = src->alias_to_level(this);
    }
    // we need the src to alias itself down to our level
    if (new_src) {
      int num_connections = 0;

      for (std::list<vsx_widget*>::iterator cit = children.begin(); cit != children.end(); ++cit)
      if ((*cit)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*cit)->marked_for_deletion) {
        //if (corder == 0) {
          //++(((vsx_widget_connector*)(*cit))->order);
        //}
        ++num_connections;
      }
      if (!referrer) {
//        cout << "aparam_connect_ok 0 0 "+new_src->component->name+" "+new_src->name+" "+i2s(num_connections) << endl;
//        if (corder == num_connections) --corder;
        param_connect_abs(new_src->component->name,new_src->name,corder);
        //command_q_b.add_raw("param_connect_ok 0 0 "+new_src->component->name+" "+new_src->name+" "+i2s(corder));
        //vsx_command_queue_b(this);
        //fix_connection_order();
      } else {
        if (corder == -1) {
//          cout << "bparam_connect_ok 0 0 "+new_src->component->name+" "+new_src->name+" -1" << endl;
    //      command_q_b.add_raw("param_connect_ok 0 0 "+new_src->component->name+" "+new_src->name+" -1");
          param_connect_abs(new_src->component->name,new_src->name,-1);
  //        vsx_command_queue_b(this);
          //fix_connection_order();
        } else {
//          cout << "cparam_connect_ok 0 0 "+new_src->component->name+" "+new_src->name+" "+i2s(corder) << endl;
          param_connect_abs(new_src->component->name,new_src->name,corder);
//          vsx_command_queue_b(this);
          //fix_connection_order();
        }
      }
    }
  } else {
    // go through our list of connections to find an alias
    vsx_widget_anchor* our_alias = 0;
    vsx_widget_connector* our_connection = 0;
    for (std::list<vsx_widget*>::iterator it = children.begin(); it != children.end(); ++it) {
      //
      if ((*it)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*it)->marked_for_deletion) {
//        printf("dest: %s\n",((vsx_widget_connector*)*it)->destination->name.c_str());

        if (((vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination)->alias
        &&  ((vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination)->io == io
        ) {
          our_alias = (vsx_widget_anchor*)((vsx_widget_connector*)*it)->destination;
          our_connection = (vsx_widget_connector*)(*it);
        }
      }
    }

    if (our_alias) {
      int neworder;
//      printf("our_connection_order: %d  order : %d\n",our_connection->order,order);
      if (corder >= 0) {
        if (our_connection->order > corder) neworder = -1;  // put it in the very beginning
        else neworder = -2; // put it in the end
      } else neworder = corder;
      // alias already exists, send our request further down the chain
      //printf("alias already exists, send our request further down the chain\n");
      our_alias->connect_far(src,neworder,this);
    } else {

      // we need to create this alias
      //printf("creating new alias %s\n",("alias_"+name).c_str());
//      printf("our p_def is: %s\n",p_def.c_str());
//      std::cout << "param_alias " << "alias_"+name+":"+p_def+" " << i2s(io)+" " << component->parent->name+" " << "alias_"+name+" "+component->name+" " << name << std::endl;
      int num_connections = 0;
      for (std::list<vsx_widget*>::iterator cit = children.begin(); cit != children.end(); ++cit)
      {
      	if ((*cit)->widget_type == VSX_WIDGET_TYPE_CONNECTOR && !(*cit)->marked_for_deletion)
      	{
      		++num_connections;
      	}
      }
      vsx_string newname = ((vsx_widget_component*)component->parent)->alias_get_unique_name_out("alias_"+name);
      ((vsx_widget_server*)((vsx_widget_component*)component)->server)->param_alias_ok(
        newname+":"+p_def,
        i2s(io),
        component->parent->name,
        newname,
        component->name,
        name, i2s(corder)
      );
      // i2s(num_connections++)
      // FIX PLZ
      vsx_widget_anchor* new_alias = ((vsx_widget_anchor*)((vsx_widget_component*)component->parent)->p_l_list_in["alias_"+name]);
      new_alias->connect_far(src, corder,this);
    }
  }
}

vsx_widget_anchor* vsx_widget_anchor::alias_to_level(vsx_widget_anchor* dest) {
//  printf("alias_to_level %s\n",dest->name.c_str());
  vsx_string src_name = component->name;
  vsx_string dest_name = dest->component->name;
//  printf("src_name = %s\n",src_name.c_str());
//  printf("dest_name = %s\n",dest_name.c_str());
  str_remove_equal_prefix(&src_name, &dest_name, ".");
//  printf("src_name = %s\n",src_name.c_str());
//  printf("dest_name = %s\n",dest_name.c_str());
//  printf("'''''''''''\n");

  vsx_string deli = ".";

  std::vector<vsx_string> src_name_parts;
  explode(src_name,deli,src_name_parts);
  src_name_parts.pop_back();
  src_name = implode(src_name_parts,deli);
  vsx_widget_anchor* alias_found = 0;
  for (std::list <vsx_widget*>::iterator it = connectors.begin(); it != connectors.end(); ++it) {
    if (!((vsx_widget_anchor*)((vsx_widget_connector*)(*it))->parent)->marked_for_deletion)
    if (((vsx_widget_anchor*)((vsx_widget_connector*)(*it))->parent)->alias)
    if (((vsx_widget_anchor*)((vsx_widget_connector*)(*it))->parent)->alias_parent == this)
    alias_found = (vsx_widget_anchor*)((vsx_widget_connector*)(*it))->parent;
  }

  if (alias_found) {
    return alias_found->alias_to_level(dest);
  } else

  if (src_name_parts.size()) {
    vsx_string newname = ((vsx_widget_component*)component->parent)->alias_get_unique_name_out("alias_"+name);
      ((vsx_widget_server*)((vsx_widget_component*)component)->server)->param_alias_ok(
        newname+":"+p_def,
        i2s(io),
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

map<vsx_string,vsx_string> parse_url_params(vsx_string input, char major='&', char minor='=', char sublevelbegin='(', char sublevelend=')');

map<vsx_string,vsx_string> parse_url_params(vsx_string input, char major, char minor, char sublevelbegin, char sublevelend)
{
  map<vsx_string,vsx_string> values;
  int startpos=0, sublevel=0;
  vsx_string key="",val="",strip="";
  int p=0;

  for (int i=0;i<input.size();++i)
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
            if (val[val.size()-1]==')') val.pop_back();//str_replace(val.replace(val.size()-1,1,"");
            if (val[0]=='(') val = val.substr(1);//replace(0,1,"");
            //cout << "key: "<<key<<", val: "<<val<<endl;
            values[key]=val;
          }
        }
        startpos=i+1;
      }
  }
  return values;
}

void vsx_widget_anchor::init_menu(bool include_controllers) {
  if (menu) menu->_delete();
  vsx_widget_popup_menu* menu_ = new vsx_widget_popup_menu;
    menu_->size.x = 0.2;
    menu_->size.y = 0.5;
  if (alias) {
    menu_->commands.adds(VSX_COMMAND_MENU, "unalias", "anchor_unalias","");
  }
  if (io==-1)
  if (p_type == "float") {
    if (!default_controller.size()) default_controller = "controller_knob";
    if (include_controllers) {
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
  }
  else if (p_type == "float3")
  {
    if (!default_controller.size()) default_controller = "controller_slider_multi 3";
    if (include_controllers) {
      menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
      menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
      menu_->commands.adds(VSX_COMMAND_MENU, "sliders", "controller_slider_multi","3");
      menu_->commands.adds(VSX_COMMAND_MENU, "rotation axis sphere", "controller_ab","3");
      menu_->commands.adds(VSX_COMMAND_MENU, "color", "controller_col","");
      menu_->commands.adds(VSX_COMMAND_MENU, "pad", "controller_pad","");
    }
  }
  else if (p_type == "float4")
  {
    if (!default_controller.size()) default_controller = "controller_slider_multi 4";
    //menu = add(new vsx_widget_popup_menu,".anchor_menu");
    if (include_controllers) {
      menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
      menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
      menu_->commands.adds(VSX_COMMAND_MENU, "sliders", "controller_slider_multi","4");
      menu_->commands.adds(VSX_COMMAND_MENU, "color", "controller_col","");
    }
  }
  else if (p_type == "quaternion")
  {
    if (!default_controller.size()) default_controller = "controller_slider_multi 4";
    //menu = add(new vsx_widget_popup_menu,".anchor_menu");
    if (include_controllers) {
      menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
      menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
      menu_->commands.adds(VSX_COMMAND_MENU, "sliders", "controller_slider_multi","4");
      menu_->commands.adds(VSX_COMMAND_MENU, "rotation axis sphere", "controller_ab","4");
    }
    menu_->commands.adds(VSX_COMMAND_MENU, "--Sequencer-----------", "","");
    menu_->commands.adds(VSX_COMMAND_MENU, "add/edit sequence", "pseq_a_m","");
    menu_->commands.adds(VSX_COMMAND_MENU, "remove sequence", "pseq_p","remove");
  }
  else if (p_type == "resource") {
    if (!default_controller.size()) default_controller = "controller_resource";
    if (include_controllers) {
      menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
      menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
      menu_->commands.adds(VSX_COMMAND_MENU, "resource library", "controller_resource","");
      menu_->commands.adds(VSX_COMMAND_MENU, "text editor", "controller_edit","");
    }
  }
  else if (p_type == "string") {
    if (!default_controller.size()) default_controller = "controller_edit";
    if (include_controllers) {
      menu_->commands.adds(VSX_COMMAND_MENU, "set default value", "param_set_default","");
      menu_->commands.adds(VSX_COMMAND_MENU, "--Controllers--------", "","");
      menu_->commands.adds(VSX_COMMAND_MENU, "text editor", "controller_edit","");
      if (default_controller == "controller_resource") {
        menu_->commands.adds(VSX_COMMAND_MENU, "resource library", "controller_resource","");
      }
    }
  }
  else if (p_type == "sequence") {
    menu_->commands.adds(VSX_COMMAND_MENU, "editor", "controller_seq_edit","");
  }
  else if (p_type == "complex")
  {
    menu_->commands.adds(VSX_COMMAND_MENU, "open/close (left-double-click)", "tg","");
    dialogs=parse_url_params(p_type_suffix);
    for (map<vsx_string,vsx_string>::iterator it = dialogs.begin(); it != dialogs.end(); ++it)
    {
      //cout << (*it).first.substr(0,7)<<endl;
      if (((vsx_string)(*it).first).substr(0,7) == "dialog_")
      {
        vsx_string name=((vsx_string)(*it).first).substr(7);
        menu_->commands.adds(VSX_COMMAND_MENU, "Dialogs;"+name, "settings_dialog",name+" "+(*it).second);
      }
    }
  }
  else 
  if (p_type == "enum")
  {
    //menu = add(new vsx_widget_popup_menu,".anchor_menu");
    vsx_string deli_p = "&";
    vector<vsx_string> parts;
    explode(p_type_suffix,deli_p,parts);

    vsx_string deli = "|";
    vector<vsx_string> enumerations;
    explode(parts[0],deli,enumerations);
    for (unsigned long i = 0; i < enumerations.size(); ++i) {
      menu_->commands.adds(VSX_COMMAND_MENU, enumerations[i],"enum",i2s(i));
      enum_items.push_back(enumerations[i]);
    }
  }
  if (!menu_->commands.count()) {
    menu_->_delete();
  } else
  {
    menu = add(menu_,"menu");
    menu->init();
  }
}

void vsx_widget_anchor::init() {
	if (init_run) return;
	alias_owner = this;
	support_interpolation = true;
	alias_parent = 0;
	set_glow(false);
	// process options
	options = parse_url_params(p_type_suffix);
	help_text = "Data type:  "+p_type+"\n";
	if (options.find("min") != options.end()) {
		help_text += "Minimum value: "+options["min"]+"\n";
	}
	if (options.find("max") != options.end()) {
		help_text += "Maximum value:  "+options["max"]+"\n";
	}

	if (options.find("help") != options.end()) {
		help_text += "\nParameter info (from the module):\n"+base64_decode(options["help"]);
	}
#ifdef VSXU_PLAYER
	color = vsx_color__(0.4,0.5,0.6,0.4);
	#ifdef VSX_DEBUG
	printf("p_type: %s\n",p_type.c_str());
	#endif
	if (p_type == "complex") {
		color = vsx_color__(216.0f/255.0f,76.0f/255.0f,202.0f/255.0f,0.8f);
	} else
	if (p_type == "float") {
		color = vsx_color__(20.0f/255.0f,121.0f/255.0f,72.0f/255.0f,0.8f);
	} else
	if (p_type == "float3") {
		color = vsx_color__(64.0f/255.0f,190.0f/255.0f,78.0f/255.0f,0.8f);
	} else
	if (p_type == "float4") {
		color = vsx_color__(142.0f/255.0f,49.0f/255.0f,168.0f/255.0f,0.8f);
	} else
	if (p_type == "texture") {
		color = vsx_color__(15.0f/255.0f,99.0f/255.0f,206.0f/255.0f,0.8f);
	} else
	if (p_type == "render") {
		color = vsx_color__(236.0f/255.0f,102.0f/255.0f,127.0f/255.0f,0.8f);
	}
#endif
//     && ((vsx_widget_component*)component)->component_type != "macro"
//    cout << "p_type_suffix: " << p_type_suffix << endl;
	dialogs=parse_url_params(p_type_suffix);
	for (map<vsx_string,vsx_string>::iterator it = dialogs.begin(); it != dialogs.end(); ++it)
	{
		if ((*it).first == "default_controller")
		{
			//cout << (*it).second << endl;
			default_controller = (*it).second;
		}
	}

	menu = 0;
	init_menu(true); // include controllers in the menu
	init_children();
	anchor_order[0] = 0;
	anchor_order[1] = 0;

	title = name+":"+p_type;
	//if (p_type == "render" || p_type == "complex" || p_type == "float" || p_type == "float3" || p_type == "float4" ||
	//p_type == "texture" || p_type == "enum" || p_type == "bitmap" || p_type == "string" || p_type == "float_array" || p_type =="sequence")
#ifndef VSXU_PLAYER
	mtex_d.load_png(skin_path+"datatypes/"+p_type+".png");
	mtex_blob.load_png(skin_path+"interface_extras/highlight_blob.png");
	mtex_blob_small.load_png(skin_path+"interface_extras/connection_blob.png");
#endif
	//else
	//mtex_d.load_tga(skin_path+"datatypes/"+p_type+".tga");
	color.r = 1.0f;
	color.g = 1.0f;
	color.b = 1.0f;
	color.a = 1.0f;
	size.y = size.x = 0;
	conn_open = false;
	tree_open = false;
//    myf.background = true;
//    myf.background_color.a = 0.5;
	text_size = 0;
	init_run = true;
	if (parent->widget_type == VSX_WIDGET_TYPE_ANCHOR)
	visible = ((vsx_widget_anchor*)parent)->tree_open;
}

void vsx_widget_anchor::reinit() {
	//if (p_type == "render" || p_type == "complex" || p_type == "float" || p_type == "float3" || p_type == "float4" ||
	//p_type == "texture" || p_type == "enum" || p_type == "bitmap" || p_type == "string")
#ifndef VSXU_PLAYER
	mtex_d.load_png(skin_path+"datatypes/"+p_type+".png");
#endif
	//else
	//mtex_d.load_tga(skin_path+"datatypes/"+p_type+".tga");
	//mtex_d.load_tga(skin_path+"datatypes/"+p_type+".tga");
//    mtex.load_png_thread(skin_path+"component_types/"+component_type+".png");
	vsx_widget::reinit();
}

void vsx_widget_anchor::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
	//printf("hej\n");
	/*if (button != 0) {
		//
		return;
	}*/

	if (button == 0) {
		m_focus = this;
		a_focus = this;
		k_focus = this;
		parent->front(this);

		//glutSetCursor(GLUT_CURSOR_CROSSHAIR);
		if (p_type != "complex") {
			if (t) {
				((vsx_widget_connector*)t)->receiving_focus = true;
        t->_delete();
				//children.remove(t);
				//delete t;
				t = 0;
			}
			if (ctrl && alt)
			{
				clone_value = true;
				drag_anchor = 0;
				drag_clone_anchor = this;
			} else
			{
				t = add(new vsx_widget_connector,name+":ct");
				((vsx_widget_connector*)t)->receiving_focus = false;
				t->size = distance.center;
				t->init();
			}
			//((vsx_widget_connector*)t)->dim_my_alpha = 1.0f;
		} else
		{
			if (parent == component)
			((vsx_widget_component*)component)->hide_all_complex_anchors_but_me(this);
			else
				toggle();
		}
	} else {
			//printf("mousedown1\n");
			//t->_delete();
			drag_status = false;
			((vsxu_assistant*)((vsx_widget_desktop*)root)->assistant)->temp_show();
			vsx_widget_connector::dim_alpha = 1.0f;
			vsx_widget_connector::receiving_focus = true;
			if (t) {
        t->_delete();
				//children.remove(t);
				//delete t;
				t = 0;
			}
			//((vsx_widget_connector*)t)->receiving_focus = true;
			//children.remove(t);
			//delete t;
			//t = 0;
		//printf("mousedown2\n");
		if (alias) {
			if (!menu) {
				menu = add(new vsx_widget_popup_menu,".alias_menu");

				menu->commands.adds(VSX_COMMAND_MENU, "unalias", "anchor_unalias","");
				//menu->size.x = 0.3;
				//menu->size.y = 0.5;
				menu->title = name;
				menu->init();
			}
		}

		//for (children_iter=children.begin(); children_iter != children.end(); children_iter++) (*children_iter)->visible = 0;
		//glutSetCursor(GLUT_CURSOR_CROSSHAIR);
			vsx_widget::event_mouse_down(distance,coords,button);
	}
	//printf("end of mousedown\n");
	//parent->event_mouse_down(x-pos.x,y-pos.y,button);
	//printf("mdown done\n");
}

void vsx_widget_anchor::fix_anchors() {
  int i = 0;
  for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
    if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)(*children_iter))->fix_anchors();
      ((vsx_widget_anchor*)(*children_iter))->a_order = i;
      ++i;
    }
  }
}

void vsx_widget_anchor::set_glow(bool glow_status) {
  if (options.find("nc") != options.end()) {
		return;
  }
	draw_glow = glow_status;
}

void vsx_widget_anchor::toggle(int override) {
	if (p_type != "complex") return;
	if (override == 1) tree_open = true; else
	if (override == 2) tree_open = false;
	if (tree_open) {
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

				if (((vsx_widget_anchor*)(*it))->p_type != "complex") {
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
}

void vsx_widget_anchor::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
	if (button == 0)
	{
		//if (p_type == "complex") {
			//toggle();
			//((vsx_widget_server*)((vsx_widget_component*)component)->server)->select(component);
		//} else
		//if (p_type == "enum") {
			//vsx_widget_3d_grid_comp::event_mouse_down(world,screen,GLUT_RIGHT_BUTTON);
//		} else
		{
			conn_open = !conn_open;
			int c = 0;
			for (children_iter=children.begin(); children_iter != children.end(); ++children_iter)
			{
				if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONNECTOR) {
					if (((vsx_widget_connector*)(*children_iter))->destination) {
						++c;
						((vsx_widget_connector*)(*children_iter))->open = conn_open;
					}
				}
			}

			if (!c) {
				if (ctrl) {
					command_q_b.add_raw("seq_pool_add");
    			this->vsx_command_queue_b(this);
				} else
				if (alt) {
					command_q_b.add_raw("pseq_a_m");
					this->vsx_command_queue_b(this);
				}
				else
				if (shift) {
				} else
				{
					if (sequenced)
					{
						command_q_b.add_raw("pseq_a_m");
						this->vsx_command_queue_b(this);
					} else
					if (default_controller.size()) {
						command_q_b.add_raw(default_controller);
						vsx_command_queue_b(this);
					}
				}
			}
		}
	}
}

bool vsx_widget_anchor::event_key_down(signed long key, bool alt, bool ctrl, bool shift) {
  //printf("anchor keydown\n");
  return true;
}

void vsx_widget_anchor::get_value() {
  if (io == -1)
  command_q_b.add_raw("param_get " + component->name+" "+name+" "+i2s(id));
  else
  {
    if (p_type != "render")
    command_q_b.add_raw("pgo " + component->name+" "+name+" "+i2s(id));
  }
  component->vsx_command_queue_b(this);
}

void vsx_widget_anchor::delete_controllers() {
  for (children_iter = children.begin(); children_iter != children.end(); ++children_iter) {
    if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_CONTROLLER) (*children_iter)->_delete();//root->command_q_f.add("delete",(*children_iter)->id);
  }
}

void vsx_widget_anchor::enumerate_children_get(int override) {
  for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
  {
    //if ((*it)->type == VSX_WIDGET_TYPE_CONTROLLER) get_val = false;
    if ((*it)->widget_type == VSX_WIDGET_TYPE_ANCHOR)
    {
      if (((vsx_widget_anchor*)(*it))->p_type != "complex") {

        ((vsx_widget_anchor*)(*it))->display_value_t = 2;
        ((vsx_widget_anchor*)(*it))->text_size = 2;
        bool gvl = false;
        if (((vsx_widget_anchor*)(*it))->io == 1) gvl = true;
        else
        if (((vsx_widget_anchor*)(*it))->io == -1 && ((vsx_widget_anchor*)(*it))->connections.size()) gvl = true;

        if (gvl || override == 1)
        ((vsx_widget_anchor*)(*it))->get_value();
      }
    }
  }
}

void vsx_widget_anchor::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords) {
  //if (((vsx_widget_component*)component)->selected)
  //glutSetCursor(GLUT_CURSOR_CROSSHAIR);
  if (m_o_focus != this) {
//    bool get_val = true;
  //if (get_val) {
    get_value();
    display_value_t = 2;
    text_size = 2;
  //}
  }
  parent->front(this);
}

void vsx_widget_anchor::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) {
  //printf("mouse move\n");
  text_size = 2;
  //glutSetCursor(GLUT_CURSOR_CROSSHAIR);
  //vsx_vector uu = get_pos_p();
  //vsx_vector world2 = world - uu;
  //world2.z = pos.z;
  if (t || clone_value) {
    drag_status = true;
    drag_coords = coords;
    ((vsxu_assistant*)((vsx_widget_desktop*)root)->assistant)->temp_hide();
    if (t)
    vsx_widget_connector::dim_alpha = 0.25f;

    drag_anchor = this;
    if (t)
    {
    	t->size.x = distance.center.x;
    	t->size.y = distance.center.y;
    }
    vsx_widget_distance distance2;
    search_anchor = root->find_component(coords,distance2);
    if (search_anchor) {
      if (search_anchor->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      //printf("tt-name: %s\n",tt->name.c_str());
        if (!((vsx_widget_anchor*)search_anchor)->tree_open) {
          ((vsx_widget_anchor*)search_anchor)->toggle();
        }
        ((vsx_widget_anchor*)search_anchor)->text_size = 2.2;
        ((vsx_widget_anchor*)search_anchor)->display_value_t = 2.0f;
      }
    }
  }
}

void vsx_widget_anchor::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
	//printf("anchor mouse up\n");
	// see if the component the mouse is over is a valid receiver
	//vsx_vector pv = get_pos_p();
	//vsx_vector uu = parent->get_pos_p();
	//vsx_vector p = u - uu;
	if (button == 0) {
		if (p_type == "enum")
		{
			if (menu) {
				//printf("menu up\n");
				front(menu);
				menu->visible = 2;
				coords.screen_global.x -= menu->size.x;
				menu->pos = menu->target_pos = coords.screen_global;
			}
		} else
		if (p_type == "sequence")
		{
			command_q_b.add_raw("controller_seq_edit");
			this->vsx_command_queue_b(this);
		}
		drag_status = false;
		((vsxu_assistant*)((vsx_widget_desktop*)root)->assistant)->temp_show();
		vsx_widget_connector::dim_alpha = 1.0f;
		vsx_widget_connector::receiving_focus = true;
		if (clone_value)
		{
			if (search_anchor)
			{
				if (search_anchor->widget_type == VSX_WIDGET_TYPE_ANCHOR)
				{
					if (p_type == ((vsx_widget_anchor*)search_anchor)->p_type)
					{
						vsx_string cm = vsx_string("param_set_interpolate ")+display_value+vsx_string("10.0 ")+search_anchor->name;
						//printf("sending param update %s\n",cm.c_str());
						command_q_b.add_raw(cm);
						search_anchor->vsx_command_queue_b(this);
					}
				}
			}
		}
		clone_value = false;
		drag_clone_anchor = 0;
		if (t) {
			//children.remove(t);
			//delete t;
      t->_delete();
			t = 0;
			//vsx_widget *tt = 0;//!!!root->find_component(world,screen,true);
			if (search_anchor) {
//          printf("tt %s\n",tt->name.c_str());
				if (search_anchor->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
					//make it so that the connector is owned by the in-anchor
					//check that the type of input is not the same as ours
					//printf("ioooo %d\n",((vsx_widget_anchor*)tt)->io);
					//printf("name: %s\n",((vsx_widget_anchor*)tt)->name.c_str());
					if (((vsx_widget_anchor*)search_anchor)->io != io)
					{
						// 1. is the param_def string equal to ours?
						if (p_type == ((vsx_widget_anchor*)search_anchor)->p_type)
						{
							if (((vsx_widget_anchor*)search_anchor)->io == -1) {
								connect(search_anchor);
							} else {
								((vsx_widget_anchor*)search_anchor)->connect(this);
							}
						}
					}
				} else
				if (search_anchor->widget_type == VSX_WIDGET_TYPE_COMPONENT && ((vsx_widget_component*)search_anchor)->component_type == "macro" && search_anchor == component->parent && m_focus == this) {
					// syntax:
					//   param_alias [component] [parameter] [source_component] [source_parameter] [-1=in / 1=out]
					vsx_string tp;
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

void vsx_widget_anchor::pre_draw() {
	if (visible > 0) {
		//printf("pre draw\n");
		if (t) if (a_focus != this) {
			((vsx_widget_connector*)t)->receiving_focus = true;
      t->_delete();
			//children.remove(t);
			//delete t;
			t = 0;
		}

		// dim down component and other helpful stuff if dragging connection
		if (clone_value) {
			component->color.a = 0.05f;
			if (drag_anchor)
			if (drag_anchor != this) {
				if (drag_anchor->io == io) {
					if (drag_anchor->p_type == p_type) {
						if (drag_anchor->component != component) {
							set_glow(true);
							color.a = 1.0f;
							component->color.a = 0.7f;
							drag_size_mul = 1.2f;
						} else color.a = 0.55f;
					} else color.a = 0.35f;
				} else color.a = 0.05f;
			}
		} else
		if (drag_status) {
			//printf("drag status\n");
			// if (component->color.a == 1.0f)  <-- <jaw> WHY an if here?
				component->color.a = 0.05f;
			//component->color.a -= 1.0f;
			//if (component->color.a < 0.1f) component->color.a = 0.1f;
			if (drag_anchor != this) {
				if (drag_anchor->io != io) {
					if (drag_anchor->p_type == p_type) {
						if (drag_anchor->component != component) {
							set_glow(true);
							color.a = 1.0f;
							component->color.a = 0.7f;
							drag_size_mul = 1.2f;
						} else color.a = 0.55f;
					} else color.a = 0.35f;
				} else color.a = 0.05f;
			}
		} else {
			drag_size_mul = 1.0f;
			set_glow(false);
			component->color.a = 1.0f;
			color.a = 1.0;
		}

		pp = parent->get_pos_p();
		if (parent->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
			if ( ((vsx_widget_component*)component)->deleting  )
			{
				target_size.x = 0.0f;
				interpolating_size = true;
				target_size.y = size.x;
			}
			else
			{
				target_size.x = 0.05f*0.15f*drag_size_mul;
				//if (size.x > 0.15f*0.05f) size.x = 0.15f*0.05f;
				target_size.y = size.x;
			}
			pos.y = parent->size.y/2-(float)a_order*target_size.y*1.4f/drag_size_mul;
			pos.x = (float)io*parent->size.x*0.5f;
		} else {
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
#ifdef VSXU_PLAYER
		sx = (size.x/2.0f);
		sy = (size.x/2.0f);
#else
		sx = 1.8f*(size.x/2.0f);
		sy = 1.8f*(size.x/2.0f);
#endif
		if (!interpolating_size)
		{
			target_size = size;
		}
		target_pos = pos;
	}
}

void vsx_widget_anchor::draw() {
	if (visible > 0)
	{
		color.gl_color();
#ifndef VSXU_PLAYER
		mtex_d.bind();
#endif
		draw_box_texf(ax,ay,pos.z,sx,sy);
#ifndef VSXU_PLAYER
		mtex_d._bind();
#endif


		if (vsxl_filter) {
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
		if (sequenced) {
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
		if (a_focus == this) {
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

		text_size -= dtime;
		if (drag_status) text_size = 0.5f;
		display_value_t -= dtime;
		if (display_value_t < 0) display_value_t = 0;
		if (text_size < 0) text_size = 0;
		float d_size = display_value_t > 0.8f?0.8f:display_value_t;
		float t_size = text_size > 0.5f?0.5f:text_size;

		//if (((vsx_widget_component*)component)->selected)
		float sx06 = size.x * 0.6f;

		myf_size = myf.get_size(name,size.x*(0.3+t_size));
		myf_pos = vsx_vector(pos.x+pp.x+sx06,pos.y+pp.y-size.y*0.5);
		glColor4f(0.0f,0.0f,0.0f,0.5f*color.a);
		myf.color.a = color.a;
		if (io == -1) {
			if (!drag_status) draw_box(myf_pos, myf_size.x, myf_size.y);
			myf.print(myf_pos, name,size.x*(0.3+t_size));
		}
		else {
			myf_pos.x -= size.x;
			if (!drag_status)
			draw_box(myf_pos-vsx_vector(myf_size.x) , myf_size.x, myf_size.y);
			myf.print_right(myf_pos, name,size.x*(0.3+t_size));
		}
		if (d_size > 0.3)
		if (frames%7 == 1) {
			if (m_o_focus == this) {
				if (p_type == "complex" && tree_open) {
					for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
					{
						if ((*it)->widget_type == VSX_WIDGET_TYPE_ANCHOR)
						{
							if (((vsx_widget_anchor*)(*it))->p_type != "complex") {
								((vsx_widget_anchor*)(*it))->display_value_t = 2;
								((vsx_widget_anchor*)(*it))->text_size = 2;

								bool gvl = false;
								if (((vsx_widget_anchor*)(*it))->io == 1) gvl = true;
								else
								if (((vsx_widget_anchor*)(*it))->io == -1 && ((vsx_widget_anchor*)(*it))->connections.size()) gvl = true;
								if (gvl)
								((vsx_widget_anchor*)(*it))->get_value();
							}
						}
					}
				}
				display_value_t = text_size = 2;
			}
			bool gv = false;
			if (io == 1) gv = true;
			else
			if (io == -1 && connections.size()) gv = true;
			if (gv)
			get_value();
		}
		myf.color.a = color.a;
//      myf.background_color.a = color.a;
		myf_size = myf.get_size(display_value,size.x*(d_size));
		//glColor4f(0.0f, 0.0f, 0.0f, 0.3f*color.a);
		if (d_size > 0.1) {
			if (io == -1)
			{
				myf_pos.x -= size.x;
				//draw_box(myf_pos-vsx_vector(myf_size.x), myf_size.x, myf_size.y);
				myf.print_right(myf_pos, display_value,size.x*(d_size));
			}
			else
			{
				myf_pos.x += size.x;
				//draw_box(myf_pos, myf_size.x, myf_size.y);
				myf.print(myf_pos, display_value,size.x*(d_size));
			}
		}
	}
	if (draw_glow) {
	glColor4f(0.8,0.8,1,0.8*(0.5+sin(time*15)*0.5)+0.2f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glPushMatrix();
		glTranslatef(ax,ay,pos.z);
		glRotatef(time*100,0,0,1);
		mtex_blob.bind();
			draw_box_texf(0,0,0,0.03f,0.03f);
		mtex_blob._bind();
		glPopMatrix();
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	pre_draw_children();
	draw_children();
	if (clone_value && drag_clone_anchor == this)
	{
		mtex_blob_small.bind();
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
			//printf("screen aspect: %f\n",screen_aspect);
			glColor4f(0.5f,1,1,1);
			draw_box_tex_c(drag_coords.world_global, 0.01f*(1.0f - fmod(time * 10.0f, 1.0f)), 0.01f*(1.0f - fmod(time * 10.0f, 1.0f)));
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		mtex_blob_small._bind();
	}
}

vsx_widget_anchor::vsx_widget_anchor() {
	alias = vsxl_filter = sequenced = tree_open = false;
	alias_for_component = alias_for_anchor = "";
	menu = search_anchor = t = 0;
	display_value_t = 0.0f;
	widget_type = VSX_WIDGET_TYPE_ANCHOR;
}

vsx_vector vsx_widget_anchor::get_pos_p() {
	if (!visible) return parent->get_pos_p();

	vsx_vector t;
	t.x = pos.x;
	t.y = pos.y;
	t.z = pos.z;
	vsx_vector tt;
	if (parent != this) tt = parent->get_pos_p();
	t.z = pos.z;
	return t + tt;
}
#endif
