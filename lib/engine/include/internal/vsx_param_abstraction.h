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


#ifndef VSX_PARAM_ABSTRACTION_H
#define VSX_PARAM_ABSTRACTION_H

#include "vsx_comp_channel.h"
#include "vsx_comp_abs.h"
/*

modulen fyller i en v�ldigt simpel lista utan logik i (klass)
sedan tar komponenten hand om den listan och bygger en parallell lista med avancerade funktioner och det
blir denna som �r den som anv�nds igenom hela systemet, f�r b�de statiska parametrar och alias etc.
*/

/*class vsx_engine_param;
class vsx_engine_param_list;
class vsx_channel;*/

// this connection class is intended to hold the visual connection, not the real connection.
// so this connection can be between 2 aliases for instance.
class vsx_engine_param;

class vsx_engine_param_connection {
public:
  bool alias_connection;
  vsx_engine_param* owner;
  int connection_order; // connection order id
  vsx_engine_param* src;
  vsx_engine_param* dest;
  vsx_channel_connection_info* channel_connection;
};



class vsx_engine_param_connection_info {
public:
  int connection_order;
  int localorder;
  size_t num_dest_connections;
  vsx_engine_param* src;  // can be volatile!
  vsx_string<>src_name;
  vsx_engine_param* dest; // can be volatile!
  vsx_string<>dest_name;
  vsx_channel_connection_info* channel_connection;
};


// an engine param can have one alias and a number of connections.

class vsx_engine_param {
public:
  vsx_module* module;
  vsx_comp_abs* component;

// the core param, for both aliases and real access
  vsx_module_param_abs* module_param;
  vsx_channel* channel;

  // our owner/parent (can be used to find component ownership also)
  vsx_engine_param_list* owner;

  // linked list backwards
  vsx_engine_param* alias_parent;

  // linked list forwards
  //vsx_engine_param* alias_child;

  // the one we're aliasing
  vsx_engine_param* alias_owner;

  bool critical; // copied from the module param to save ->'s
  bool all_required;
  bool sequence; // is a sequence active for this parameter?

  vsx_string<>name; // name of our anchor
  vsx_string<>spec; // specification string, filled out by the module in the form :float

  bool alias; // is this an alias or a source anchor?

  // expose this param when running engine in a module?
  int external_expose;

  // we save here our own connections so we can disconnect properly when an alias is removed
  std::vector<vsx_engine_param_connection*> connections;
  vsx_engine_param_connection* get_conn_by_dest(vsx_engine_param* dest);
  vsx_engine_param_connection* get_conn_by_src(vsx_engine_param* src);

  bool unalias();
  bool disconnect();

  // disconnect all connections in this and aliased without disconnecting the engine
  void disconnect_abs_connections();

  // dump/serialize values
  void dump_aliases_and_connections(vsx_string<>base_macro, vsx_command_list* command_result);
  void dump_aliases_and_connections_rc(vsx_command_list* command_result); // direct
  void dump_aliases(vsx_string<>base_macro, vsx_command_list* command_result);
  void dump_aliases_rc(vsx_command_list* command_result);
  void dump_connections(vsx_string<>base_macro, vsx_command_list* command_result);
  void get_abs_connections(std::list<vsx_engine_param_connection_info*>* abs_connections, vsx_engine_param* dest);
  void dump_pflags(vsx_command_list* command_result);

// order, also pretty yeah...
  void rebuild_orders(std::vector<int>* new_order);


// connect to another parameter in the system
  int connect(vsx_engine_param* src);
  vsx_engine_param* alias_to_level(vsx_engine_param* dest);
  int connect_abs(vsx_engine_param* src, vsx_channel_connection_info* channel_connection, int order, int position = 1);
  int connect_far_abs(vsx_engine_param_connection_info* info, int order,vsx_engine_param* referrer = 0);
  int disconnect(vsx_engine_param* src, bool lowlevel = true);
  bool delete_conn(vsx_engine_param_connection* conn);

// for values that can be represented as string
  vsx_string<> get_string();
  vsx_string<> get_save_string();
  vsx_string<> get_default_string();
  void set_string(vsx_string<>data);
  void set_string_index(vsx_string<>data, int index = 0);
  void clean_up_module_param(vsx_module_param_abs* param);

  vsx_string<>get_type_name();

// constructor
  vsx_engine_param();
// destructor
  ~vsx_engine_param();

};

class vsx_engine_param_list {
  vsx_string<>single_param_spec(vsx_string<>param_name, int startpos = 0);
  vsx_module_param_list* module_param_list;
public:
  // input or output type
  int io; // -1 or 1
  // our owner - our component
  vsx_comp_abs* component;
  // name mapped onto the param
  std::map<vsx_string<>, vsx_engine_param*> param_name_list;
  // a plain list of the parameters
  std::vector<vsx_engine_param*> param_id_list;
  // init - run once from the component to build engine_params from module_params
  void init(vsx_module_param_list* module_param_list);

  // delete a param from our list
  void delete_param(vsx_engine_param* param);

  //---------------------------------------
  // Various Dump Routines
  //---------------------------------------
  // dump commands describing our aliases/connections
  void dump_aliases_and_connections(vsx_string<>base_macro, vsx_command_list* command_result);
  void dump_aliases_and_connections_rc(vsx_command_list* command_result);
  void dump_aliases(vsx_string<>base_macro, vsx_command_list* command_result);
  void dump_aliases_rc(vsx_command_list* command_result);
  void dump_connections(vsx_string<>base_macro, vsx_command_list* command_result);
  void dump_param_values(vsx_string<>my_name, vsx_command_list* command_result);
  // returns information about abstract (engine_param) destination and real source (engine_param) of each real
  // connection - that is, not the aliases
  void get_abs_connections(std::list<vsx_engine_param_connection_info*>* abs_connections);

  //---------------------------------------
  // Aliasing
  //---------------------------------------
  // alias another component into us - mimic/alias the param found in src
  int alias(vsx_engine_param* src, vsx_string<>name, int order = -1);
  bool unalias(vsx_string<>name);
  vsx_string<>alias_get_unique_name(vsx_string<>base_name, int tried = 0); // find a free name for the alias
  bool alias_rename(vsx_string<>ren_name,vsx_string<>to_name);

  //---------------------------------------
  // Connections
  //---------------------------------------
  // Disconnect all connections in this and aliased without disconnecting the engine
  void disconnect_abs_connections();

  // clear our parameters from other param_list's
  void unalias_aliased();

  // changes the order of the parameters in this list and maps the connections onto the channel recursively
  int order(vsx_string<>param, vsx_string<>new_order);

  //---------------------------------------

  // returns an engine_param by name
  vsx_engine_param* get_by_name(vsx_string<>name);

  vsx_string<>get_name_by_param(vsx_engine_param* param);

  vsx_engine_param* get_by_id(unsigned long id) {
    return param_id_list[id];
//		return module_param_list->id_vec[id];
  }

  size_t count() {
    return param_id_list.size();
  }
  ~vsx_engine_param_list();
};


#endif
