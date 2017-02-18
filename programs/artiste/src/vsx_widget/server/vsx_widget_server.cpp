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
#include <gl_helper.h>
#include <map>
#include <list>
#include <vector>
#include <math.h>
#ifdef _WIN32
  #include <io.h>
#endif
#include <sys/types.h>
#if COMPILER != COMPILER_VISUAL_STUDIO
#include <dirent.h>
#endif
#include <filesystem/vsx_filesystem.h>
#include <command/vsx_command.h>
#include <command/vsx_command_client_server.h>
#include "vsx_font.h"
#include "vsx_param.h"
#include <module/vsx_module.h>
#include "vsx_version.h"
#include "vsx_platform.h"
#include <string/vsx_string_helper.h>
// local includes
#include "log/vsx_log_a.h"
#include "vsx_widget.h"
#include "vsx_widget_window.h"
#include "module_choosers/vsx_widget_module_chooser.h"
#include "module_choosers/vsx_widget_module_chooser_list.h"
#include "vsx_widget_comp.h"
#include "vsx_widget_server.h"
#include "vsx_widget_anchor.h"
#include "vsx_widget_connector_bezier.h"
#include "sequencer/vsx_widget_sequence_editor.h"
#include "sequencer/pool_manager/vsx_widget_seq_pool.h"
#include "controllers/vsx_widget_controller_base.h"
#include "controllers/vsx_widget_controller_editor.h"
#include "helpers/vsx_widget_note.h"
#include "artiste_desktop.h"
#include <texture/vsx_texture.h>
#include <filesystem/vsx_filesystem_helper.h>

// engine
#include <vsx_data_path.h>

// widget
#include <dialogs/dialog_query_string.h>
#include <dialogs/dialog_messagebox.h>
#include <widgets/vsx_widget_popup_menu.h>

// VSX_WIDGET_SERVER ***************************************************************************************************

vsx_widget_server::vsx_widget_server()
  : dump_commands(false)
{
  widget_type = VSX_WIDGET_TYPE_SERVER;
  cmd_out = 0;
  cmd_in = 0;
  server_type = VSX_WIDGET_SERVER_CONNECTION_TYPE_INTERNAL;
  init_run = false;
  support_scaling = false;
  selection = false;
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  client = 0;
  #endif

  log(" welcome to");
  log(" _   _  ___ _  __     _        ___  ___   ___");
  log(" \\\\  / //_   \\//      /  / /    /  /__/  /__/");
  log("  \\\\/  ___/ _/\\\\_    /__/ /__  /  /  \\  /  /");
  log("   "+vsx_string<>(VSXU_VERSION));
  log("-----------------------------------------------------------------");
  log(" (c) 2003-2011 vovoid || http://vovoid.com || http://vsxu.com");
}

void vsx_widget_server::init()
{
  help_text =
    "The server is selected:\n"
    "- left-double-click to get the\n"
    "module browser\n"
    "- ctrl+left-double_click to load a state\n"
  ;

  menu = add(new vsx_widget_popup_menu,name+".server_menu");
  support_interpolation = true;
  interpolation_speed = 3;
  title = name;
  menu->commands.adds(VSX_COMMAND_MENU,"new >;empty project","clear","");
  menu->commands.adds(VSX_COMMAND_MENU,"new >;visualization","state_template_visual","");
  menu->commands.adds(VSX_COMMAND_MENU,"new >;transition for vsxu player","state_template_fader","");
  menu->commands.adds(VSX_COMMAND_MENU,"open...                               [ctrl+leftmouse-doubleclick]","state_menu_load","");
  menu->commands.adds(VSX_COMMAND_MENU,"save as... ","state_menu_save","");
  menu->commands.adds(VSX_COMMAND_MENU,"compile >;music visual (.vsx, '_visuals/' dir) as...","m_package_export_visuals","");
  menu->commands.adds(VSX_COMMAND_MENU,"compile >;music visual fader (.vsx, '_visuals_faders/' dir) as...","m_package_export_visuals_faders","");
  menu->commands.adds(VSX_COMMAND_MENU,"compile >;general package (.vsx '_prods/' dir) as...","m_package_export_prods","");
  menu->commands.adds(VSX_COMMAND_MENU,"----------------------", "","");
  menu->commands.adds(VSX_COMMAND_MENU,"module browser...                              [left-double-click]", "show_module_browser","");
  menu->commands.adds(VSX_COMMAND_MENU,"module list...", "show_module_browser_list","");
  menu->commands.adds(VSX_COMMAND_MENU,"resource viewer...", "resource_menu","");
  menu->commands.adds(VSX_COMMAND_MENU,"----------------------", "","");
  menu->commands.adds(VSX_COMMAND_MENU,"create macro                               [alt+left-double-click]", "add_empty_macro","$mpos");
  menu->commands.adds(VSX_COMMAND_MENU,"create note", "add_note","$mpos");
  menu->commands.adds(VSX_COMMAND_MENU,"----------------------", "","");
  menu->commands.adds(VSX_COMMAND_MENU,"sequencer >;sequencer...","sequence_menu","");
  menu->commands.adds(VSX_COMMAND_MENU,"sequencer >;animation clips...","seq_pool_menu","");
  menu->commands.adds(VSX_COMMAND_MENU,"time >;rewind","rewind","");
  menu->commands.adds(VSX_COMMAND_MENU,"time >;play","play","");
  menu->commands.adds(VSX_COMMAND_MENU,"time >;stop","stop","");
  menu->commands.adds(VSX_COMMAND_MENU,"----------------------", "","");
  menu->commands.adds(VSX_COMMAND_MENU,"tools >;undo >;undo [Ctrl+Z]","undo","");
  menu->commands.adds(VSX_COMMAND_MENU,"tools >;undo >;disable","conf","automatic_undo 0");
  menu->commands.adds(VSX_COMMAND_MENU,"tools >;undo >;automatic","conf","automatic_undo 1");
  menu->commands.adds(VSX_COMMAND_MENU,"tools >;undo >;create rollback point","undo_s","");
  menu->commands.adds(VSX_COMMAND_MENU,"tools >;show modules that haven't loaded","get_module_status","");

  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;autoload last saved state >;no","conf","autoload_last_saved_state 0");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;autoload last saved state >;yes","conf","autoload_last_saved_state 1");

  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui smoothness >;none","conf","global_interpolation_speed 1000.0");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui smoothness >;slow","conf","global_interpolation_speed 0.5");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui smoothness >;normal","conf","global_interpolation_speed 1.0");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui smoothness >;quick","conf","global_interpolation_speed 2.0");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui keyboard movement speed >;very slow","conf","global_key_speed 1");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui keyboard movement speed >;slow","conf","global_key_speed 2");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui keyboard movement speed >;normal","conf","global_key_speed 3");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;none","conf","global_framerate_limit -1");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;2fps","conf","global_framerate_limit 2");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;5fps","conf","global_framerate_limit 5");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;10fps","conf","global_framerate_limit 10");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;25fps","conf","global_framerate_limit 25");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;30fps","conf","global_framerate_limit 30");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;50fps","conf","global_framerate_limit 50");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;60fps","conf","global_framerate_limit 60");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;80fps","conf","global_framerate_limit 80");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;85fps","conf","global_framerate_limit 85");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;90fps","conf","global_framerate_limit 90");
  menu->commands.adds(VSX_COMMAND_MENU,"configuration >;gui framerate limit >;100fps","conf","global_framerate_limit 100");
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  menu->commands.adds(VSX_COMMAND_MENU,"server >;connect to rendering server...","show_connect_dialog","");
  #endif
  if (server_type == VSX_WIDGET_SERVER_CONNECTION_TYPE_SOCKET)
  {
    menu->commands.adds(VSX_COMMAND_MENU,"server >;disconnect","dc","");
    menu->commands.adds(VSX_COMMAND_MENU,"server >;kill server", "system.shutdown","");
  } else
  {
    menu->commands.adds(VSX_COMMAND_MENU,"----------------------", "","");
    menu->commands.adds(VSX_COMMAND_MENU,"exit", "system.shutdown","");
  }

  if (state_name == "")
    state_name ="_default";

  menu->set_size(vsx_vector3<>(0.4f,0.5f));

  module_chooser = add(new vsx_widget_ultra_chooser,"module_browser");
  ((vsx_widget_ultra_chooser*)module_chooser)->server = this;

  module_chooser_list = add(new vsx_module_chooser_list,"module_browser_list");
  ((vsx_module_chooser_list*)module_chooser_list)->set_server(this);

  state_chooser = add(new vsx_widget_ultra_chooser,"state_browser");
  resource_chooser = add(new vsx_widget_ultra_chooser,"resource_browser");

  export_dialog_ext =
    add
    (
      new dialog_query_string(
        "visualization package export",
        "Filename to export to, will end up in visuals/ (ex. 'mypackage.vsx')|"
        "Title of the visual (ex. 'Starlight Aurora')|"
        "Your handle/nick/vsxu-id, (ex. 'jaw' or multiple: 'jaw, cor')|"
        "Your group/company (ex. 'vovoid')|"
        "Your country (ex. 'Sweden')|"
        "Your homepage (ex. 'http://vovoid.com')|"
        "Free text comments (max 300 characters)|"
      ),
      "package_visual_export"
    )
  ;

  export_dialog_state =
    add
    (
      new dialog_query_string(
        "save state",
        "Filename to save to, will end up in states/ (ex. 'my_state')|"
        "Title of the state (ex. 'My funky state')|"
        "Your handle/nick/vsxu-id, (ex. 'jaw' or multiple: 'jaw, cor')|"
        "Your group/company (ex. 'vovoid')|"
        "Your country (ex. 'Sweden')|"
        "Your homepage (ex. 'http://vovoid.com')|"
        "Free text comments (max 300 characters)|"
      ),
      "package_visual_export"
    )
  ;

  connect_dialog =
    add(new dialog_query_string("connect to server","hostname or ip"),"connect_dialog");

  // very extremely unfortunate for us the macros have to be in each and one of the
  // servers' browsers so we blatantly add pointers to them here, tee hee! ;) this to mimimize
  // the memory abuse but it will still be a lot :(
  // this is due to the fact that macros are stored in the client, not in the server
  // or should this be different? who knows..


  selection = false;

  set_size(vsx_vector3<>(10.0f,10.0f));
  color.a = 1.0;
  init_children();

  init_run = true;
  cmd_out->add_raw("get_module_list", VSX_COMMAND_GARBAGE_COLLECT);
  cmd_out->add_raw("get_list resources", VSX_COMMAND_GARBAGE_COLLECT);
  cmd_out->add_raw("get_list states", VSX_COMMAND_GARBAGE_COLLECT);
  cmd_out->add_raw("get_list prods", VSX_COMMAND_GARBAGE_COLLECT);
  cmd_out->add_raw("get_list visuals", VSX_COMMAND_GARBAGE_COLLECT);

  if (server_type == VSX_WIDGET_SERVER_CONNECTION_TYPE_INTERNAL)
  {
    if ( vsx_argvector::get_instance()->has_param_with_value("state") )
    {
      cmd_out->add_raw("state_load "+vsx_string_helper::base64_encode("states;"+vsx_argvector::get_instance()->get_param_value("state")), VSX_COMMAND_GARBAGE_COLLECT);
    } else
    {
      if (
          configuration.find("last_saved_state") != configuration.end()
          &&
          configuration.find("autoload_last_saved_state") != configuration.end()
          &&
          configuration["autoload_last_saved_state"] == "1"
          )
      {
        cmd_out->add_raw("state_load "+vsx_string_helper::base64_encode("states;" + vsx_string_helper::base64_decode(configuration["last_saved_state"])), VSX_COMMAND_GARBAGE_COLLECT);
      } else
      cmd_out->add_raw("state_load "+vsx_string_helper::base64_encode("states;_default"), VSX_COMMAND_GARBAGE_COLLECT);
    }
  }
  cmd_out->add_raw("get_state", VSX_COMMAND_GARBAGE_COLLECT);

  // set sequencer to 0, we gonna fill it later
  sequencer = 0;
  seq_pool = 0; // TODO: remove the auto-init

  mtex = vsx_texture_loader::load(
    vsx_widget_skin::get_instance()->skin_path_get() + "server.dds",
    vsx::filesystem::get_instance(),
    true, // threaded
    0,
    vsx_texture_gl::linear_interpolate_hint
  );

  if (server_type == VSX_WIDGET_SERVER_CONNECTION_TYPE_SOCKET)
    color.a = 1.0;
  init_run = true;
}

void vsx_widget_server::reinit()
{
  vsx_widget::reinit();
}

void vsx_widget_server::on_delete() {
  mtex.reset(nullptr);

  for (std::map<vsx_string<>,vsx_module_specification*>::iterator it = module_list.begin(); it != module_list.end(); ++it)
    delete (*it).second;

  for (size_t i = 0; i < module_infos_created_for_choosers.size(); i++)
  {
    delete module_infos_created_for_choosers[i];
  }
}

void vsx_widget_server::server_connect(vsx_string<>host, vsx_string<>port)
{
  VSX_UNUSED(port);
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    client = new vsx_command_list_client;
    cmd_in = client->get_command_list_in();
    cmd_out = client->get_command_list_out();
    client->client_connect(host);
    server_type = VSX_WIDGET_SERVER_CONNECTION_TYPE_SOCKET;
    support_scaling = true;
  #endif
}

// messages from the engine
void vsx_widget_server::vsx_command_process_f() {
  vsx_command_s *c = 0;

  if (server_type == VSX_WIDGET_SERVER_CONNECTION_TYPE_SOCKET)
  {
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    //printf("connection status: %d\n", client->get_connection_status());
    if (client->get_connection_status() == VSX_COMMAND_CLIENT_DISCONNECTED)
    {
      _delete();
    }
  #endif
  }
  VSXP_CLASS_LOCAL_INIT


  if (init_run) {
    // messages from the engine
    while ( (c = cmd_in->pop()) )
    {
      VSXP_S_BEGIN("w_server process_cmd_f")
      if (c->cmd == "vsxu_welcome") {
        server_version = c->parts[1];
        connection_id = c->parts[2];
      } else

      if (c->cmd == "component_create_ok")
      {
        // syntax:
        //  component_create_ok [name] [component_info] [x-pos] [y-pos] [size] [extra]
        vsx_widget* component = 0;
        vsx_widget* macro_parent = this;

        vsx_string<>real_name = c->cmd_data;
        vsx_string<>parent_name = "";

        vsx_nw_vector<vsx_string<> > add_c;
        vsx_string<>deli = ".";
        vsx_string_helper::explode(c->cmd_data, deli, add_c);
        if (add_c.size() > 1)
        {
          real_name = add_c[add_c.size()-1];
          add_c.reset_used(add_c.size()-1);
          vsx_string<> deli(".");
          parent_name = vsx_string_helper::implode(add_c, deli);

          vsx_widget* f = find_component(parent_name);
          if (f)
          {
            macro_parent = f;
          }
          else
          {
            command_q_f.addc(c);
            VSXP_S_END
            continue;
          }
        }

        component = macro_parent->add(new vsx_widget_component, c->cmd_data);
        comp_list[c->cmd_data] = component;
        if (!component)
        {
          VSXP_S_END
          return;
        }
        component->init();
        if (component->parent->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
          component->enabled = ((vsx_widget_component*)component->parent)->open;
          component->visible = ((vsx_widget_component*)component->parent)->open;
        }
        if (c->parts.size() >= 6)
        {
          if (c->parts[2] == "macro") {
            ((vsx_widget_component*)component)->old_size.y = ((vsx_widget_component*)component)->old_size.x = vsx_string_helper::s2f(c->parts[5]);
          }
          else
          {
            component->help_text = build_comp_helptext(c->parts[5]);
            ((vsx_widget_component*)component)->module_path = c->parts[5];
          }
        }
        component->set_size(vsx_vector3<>(0.05f*0.45f,0.05f*0.45f));
        if (c->parts.size() >= 7) {
          if (c->parts[6] == "out") {
            ((vsx_widget_component*)component)->not_movable = true;
          }
        }
        ((vsx_widget_component*)component)->server = this;
        ((vsx_widget_component*)component)->real_name = real_name;
        ((vsx_widget_component*)component)->parent_name = parent_name;

        component->set_pos(vsx_vector3<>(vsx_string_helper::s2f(c->parts[3]),vsx_string_helper::s2f(c->parts[4])));

        // send in the component_info
        command_q_b.add_raw("component_info "+component->name+" "+c->parts[2], true);
        component->vsx_command_queue_b(this);
        if (module_chooser->visible != 0)
          front(module_chooser);
      }
      else
      if (c->cmd == "component_rename_ok") {
        if (c->parts.size() == 3) {
          vsx_widget* dest = find_component(c->parts[1]);
          ((vsx_widget_component*)dest)->rename(c->parts[2]);
        }
      }
      else
      if (c->cmd == "note_create_ok") {
        vsx_widget* new_note = add(new vsx_widget_note,c->parts[1]);
        new_note->init_from_command(c);
        if (note_list.find(c->parts[1]) != note_list.end())
        {
          note_list[c->parts[1]]->_delete();
        }
        note_list[c->parts[1]] = new_note;
      }
      else
      if (c->cmd == "param_get_ok" || c->cmd == "pg64_ok") {
        // syntax:
        //  param_get [component] [param] [value] [gui-id]
        if (c->parts.size() == 5) {
        //printf("%s",c->raw.c_str());
          vsx_widget* t = find( vsx_string_helper::s2i(c->parts[4]) );
          if (t) {
            //printf("found comp\n");
            command_q_b.add(c);
            t->vsx_command_queue_b(this);
          }
        }
      }
      else
      if (c->cmd == "param_alias_ok") {
        // syntax:
        //          param_alias_ok [p_def] [-1=in / 1=out] [component] [parameter] [source_component] [source_parameter]
        param_alias_ok(c->parts[1],c->parts[2],c->parts[3],c->parts[4],c->parts[5],c->parts[6],c->parts[7]);
      }
      else
      if (c->cmd == "param_unalias_ok") {
        // syntax:
        //    param_unalias [-1/1] [component_name] [param_name]
        int anchor_io; // the io of the anchor
        if (c->parts[1] == "-1")
          anchor_io = -1;
        else
          anchor_io = 1;

        int anchor_count = 0; // number of anchors that we've found
        vsx_widget* dest = find_component(c->parts[2]);

        // go through our children to find the one that is asked for
        for (children_iter = dest->children.begin(); children_iter != dest->children.end(); ++children_iter) {
          // first, we single out the ones with the correct IO in case there's one with the same name
          if ((*children_iter)->widget_type == VSX_WIDGET_TYPE_ANCHOR)
          if (anchor_io == ((vsx_widget_anchor*)(*children_iter))->io)
          {
            // so if the name is ok
            if ((*children_iter)->name == c->parts[3]) {
              (*children_iter)->_delete();
            }
            else
            {
              // fix the vertical list
              // set its order to the right value
              ((vsx_widget_anchor*)(*children_iter))->a_order = anchor_count;
              ++anchor_count;
            }
          }
        } // for
        // ok set the component new-order right?

        if (anchor_io > 0)
          ((vsx_widget_component*)dest)->anchor_order[1] = anchor_count;
        else
          ((vsx_widget_component*)dest)->anchor_order[0] = anchor_count;
      }
      else
      if (c->cmd == "pa_ren_ok") {
        vsx_widget* dest = find_component(c->parts[1]);
        if (dest) {
          if (c->parts[4] == "-1") {
            vsx_widget* b = ((vsx_widget_component*)dest)->p_l_list_in[c->parts[2]];
            b->name = c->parts[3];
            ((vsx_widget_component*)dest)->p_l_list_in.erase(c->parts[2]);
            ((vsx_widget_component*)dest)->p_l_list_in[c->parts[3]] = b;
          } else {
            vsx_widget* b = ((vsx_widget_component*)dest)->p_l_list_out[c->parts[2]];
            b->name = c->parts[3];
            ((vsx_widget_component*)dest)->p_l_list_out.erase(c->parts[2]);
            ((vsx_widget_component*)dest)->p_l_list_out[c->parts[3]] = b;
          }
        }
      } else
      if (c->cmd == "clear_ok") {
        for (note_iter = note_list.begin(); note_iter != note_list.end(); ++note_iter) {
          (*note_iter).second->_delete();
        }
        note_list.clear();

        std::map<vsx_string<>, vsx_widget*>temp_ = comp_list;
        for (std::map<vsx_string<>, vsx_widget*>::iterator it = temp_.begin(); it != temp_.end(); ++it) {
          if (!((vsx_widget_component*)(*it).second)->internal_critical) {
            (*it).second->_delete();
          } else {
            (*it).second->target_pos.y = (*it).second->target_pos.x = 0.0f;
            (*it).second->interpolating_pos = true;
          }
        }
        if (seq_pool)
          seq_pool->message("clear");
        if (sequencer)
          sequencer->message("clear");
        server_message = "";
        state_name = "";
      } else
      if (c->cmd == "state_load_ok") {
        // just clear the server, the rest will come automatically..
        server_message = "";
        std::map<vsx_string<>, vsx_widget*>temp_ = comp_list;
        for (std::map<vsx_string<>, vsx_widget*>::iterator it = temp_.begin(); it != temp_.end(); ++it) {
          (*it).second->_delete();
        }
        vsx_string<>s_name = vsx_string_helper::base64_decode(c->parts[1]);

        if (s_name.find("states;") == 0)
        state_name = vsx_string_helper::str_replace<char>("states;", "", s_name, 1, 0);
        #ifdef VSX_DEBUG
        printf("state load ok received: %s \n",c->parts[1].c_str());
        #endif
        if (seq_pool)
        {
          seq_pool->init();
        }
      } else
      if (c->cmd == "component_delete_ok") {
        // syntax:
        //  component_delete_ok [component_name]
        // before we remove the component, check it's anchors to find out whom are aliased and delete everything!
        vsx_widget* dest = find_component(c->parts[1]);

        if (dest) dest->_delete();
      }
      else
      if (c->cmd == "component_assign_ok") {
        // syntax:
        //  0=component_assign_ok [1=macro_name] [2=master_component],[component],[component],... [3=pos_x] [4=pos_y]
        vsx_widget* dest_macro_component = find_component(c->parts[1]);
        vsx_string<>dest_name_prefix = "";
        if (!dest_macro_component) {
          // server / desktop
          dest_macro_component = this;
        }
        else
        dest_name_prefix = dest_macro_component->name;

        vsx_string<>deli = ",";
        vsx_nw_vector<vsx_string<> > comp_source;
        vsx_string_helper::explode(c->parts[2], deli, comp_source);
        std::list<vsx_widget_component*> components_list;

        for (unsigned long i = 0; i < comp_source.size(); ++i) {
          vsx_widget_component* t;
          t = (vsx_widget_component*)find_component(comp_source[i]);
          t->is_moving = true;
          components_list.push_back(t);
        }

        // calculate distance
        vsx_vector3<> master_pos = (*(components_list.begin()))->real_pos;
        vsx_vector3<> dst_pos(vsx_string_helper::s2f(c->parts[3]),vsx_string_helper::s2f(c->parts[4]));
        float max_size = 0.0f;

        for (std::list<vsx_widget_component*>::iterator it = components_list.begin(); it != components_list.end(); ++it)
        {
          // add the component to the list
          vsx_widget_component* component_moved = *it;

          vsx_string<>parent_name_prefix;
          if (component_moved->parent == this)
            parent_name_prefix = "";
          else
            parent_name_prefix = component_moved->parent->name;

          // get list of connections
          std::list<vsx_widget_connector_info*> in_connection_list;
          std::list<vsx_widget_connector_info*> out_connection_list;
          component_moved->get_connections_in_abs(&in_connection_list);
          component_moved->get_connections_out_abs(&out_connection_list);

          // disconnect
          component_moved->disconnect_abs();
          // move and deal with macros differently, renaming all children
          component_moved->parent->children.remove(component_moved);
          component_moved->parent = dest_macro_component;
          component_moved->cmd_parent = dest_macro_component;
          dest_macro_component->children.push_back(component_moved);

          component_moved->pos = component_moved->target_pos = dst_pos + component_moved->real_pos - master_pos;
          if ((float)fabs(component_moved->pos.x) > max_size) max_size = (float)fabs(component_moved->pos.x);
          if ((float)fabs(component_moved->pos.y) > max_size) max_size = (float)fabs(component_moved->pos.y);

          component_moved->rename_add_prefix(dest_name_prefix, parent_name_prefix);

          for(std::list<vsx_widget_connector_info*>::iterator it = in_connection_list.begin(); it != in_connection_list.end(); ++it) {
            ((vsx_widget_anchor*)(*it)->dest)->connect_far((vsx_widget_anchor*)(*it)->source,(*it)->order);
          }
          for(std::list<vsx_widget_connector_info*>::iterator it = out_connection_list.begin(); it != out_connection_list.end(); ++it) {
            ((vsx_widget_anchor*)(*it)->dest)->connect_far((vsx_widget_anchor*)(*it)->source,(*it)->order);
          }
        }
        max_size *= 2.2f;
        // now resize the macro accordingly
        if (dest_macro_component->size.x < max_size) {
          dest_macro_component->set_size(vsx_vector3<>(max_size,max_size));
          // send this to the engine so that when we dump this macro we'll get the proper size, yo!
          // this was fixed in 0.1.18
          cmd_out->add_raw("component_size "+c->parts[1]+" "+vsx_string_helper::f2s(max_size));
        }

        for (std::list<vsx_widget_component*>::iterator it = components_list.begin(); it != components_list.end(); ++it) {
          (*it)->is_moving = false;
        }
        for (std::list<vsx_widget*>::iterator itx = selected_list.begin(); itx != selected_list.end(); ++itx) {
          ((vsx_widget_component*)(*itx))->ethereal = false;
          ((vsx_widget_component*)(*itx))->ethereal_all = false;
        }
      } else
      if (
        c->cmd == "c_msg" ||
        c->cmd == "vsxl_cfr_ok" ||
        c->cmd == "vsxl_cfi_ok" ||
        c->cmd == "vsxl_cfl_s" ||
        c->cmd == "vsxl_pfr_ok" ||
        c->cmd == "vsxl_pfi_ok" ||
        c->cmd == "vsxl_pfl_s" ||
        c->cmd == "macro_dump_add" ||
        c->cmd == "macro_dump_complete" ||
        c->cmd == "component_clone_add" ||
        c->cmd == "component_clone_complete" ||
        c->cmd == "connections_order_ok" ||
        c->cmd == "in_param_spec" ||
        c->cmd == "out_param_spec" ||
        c->cmd == "param_connect_ok" ||
        c->cmd == "param_disconnect_ok" ||
        c->cmd == "param_connect_volatile" ||
        c->cmd == "module_operation_spec"
      )
      {
        //find in children
        vsx_widget* tc = find_component(c->parts[1]);
        if (tc) {
          command_q_b.add(c);
          tc->vsx_command_queue_b(this,true);
        }
        // ****************************************************************
        // This is useful for debugging out-of-order commands:
        //else
        //{
        //  printf("ARTISTE ERROR: could not find component %s\n", c->parts[1].c_str() );
        //}
        // ****************************************************************
      }
      else
      if (
            c->cmd == "pseq_l_dump_ok"
              ||
            c->cmd == "time_upd"
              ||
            c->cmd == "mseq_channel_ok"
              ||
            c->cmd == "pseq_list_add"
              ||
            c->cmd == "seq_list_ok"
        )
      {
        if (sequencer)
        {
          command_q_b.add(c);
          sequencer->vsx_command_queue_b(this,true);
        }
      }
      else
      if (c->cmd == "component_timing_ok") {
          vsx_widget* t = find( vsx_string_helper::s2i(c->parts[1]) );
          if (t) {
            command_q_b.add(c);
            t->vsx_command_queue_b(this);
          }
      } else
      if (c->cmd == "seq_pool")
      {
        if (c->parts[1] == "del")
        {
          if (sequencer)
          {
            command_q_b.addc(c);
            sequencer->vsx_command_queue_b(this,true);
          }
        }
        if (seq_pool)
        {
          command_q_b.addc(c);
          seq_pool->vsx_command_queue_b(this,true);
        }
      } else
      if (
        c->cmd == "pseq_p_ok" ||
        c->cmd == "pseq_r_ok")
      {
        vsx_widget* tc = find_component(c->parts[2]);
        if (tc) {
          command_q_b.addc(c);
          tc->vsx_command_queue_b(this,true);
        }
        if (sequencer) {
          command_q_b.addc(c);
          sequencer->vsx_command_queue_b(this,true);
        }
      } else
      if (c->cmd == "engine_dump_complete") {
        state_name = c->parts[1];
      } else
      if (c->cmd == "module_list") {
        // syntax:
        //         module_list [class=render/texture] [identifier] {information} {in_param_spec} {out_param_spec}
        // VARNING: inparamspec osv. r inte implemenenterat i servern!  <-- BUUU
        if (c->parts.size() > 2) {
          // init new module information holder
          vsx_module_specification* a;
          a = new vsx_module_specification;
          a->identifier = c->parts[2]; // xx;yy;zz
          a->description = vsx_string_helper::base64_decode(c->parts[3]); // long text
          // set component class (render, mesh, texture etc)
          a->component_class = c->parts[1];


          // add to module list and choosers
          module_list[c->parts[2]] = a;

          ((vsx_widget_ultra_chooser*)module_chooser)->module_tree->module_info = 0;
          ((vsx_widget_ultra_chooser*)module_chooser)->module_tree->add(c->parts[2], a);
          ((vsx_module_chooser_list*)module_chooser_list)->add_item(c->parts[2], a);
        }
      } else
      if (c->cmd == "module_list_end") {
        // add macros to the module list
        vsx_module_specification* a;
        std::list< vsx_string<> > mfiles;
        vsx_string<>base = vsx_data_path::get_instance()->data_path_get() + "macros";
        vsx::filesystem_helper::get_files_recursive(base, &mfiles, "", "");
        for (std::list< vsx_string<> >::iterator it = mfiles.begin(); it != mfiles.end(); ++it) {
          vsx_string<>ss = vsx_string_helper::str_replace<char>(
            " ",
            "\\ ",
            vsx_string_helper::str_replace<char>(
              "/",
              ";",
              vsx_string_helper::str_replace<char>(
                base,
                "",
                *it
              )
            )
          );
          a = new vsx_module_specification;
          module_infos_created_for_choosers.push_back(a);
          a->identifier = "macros;"+ss;
          //printf("a->ident: %s\n",a->identifier.c_str());
          a->component_class = "macro";
          //module_list["macros;"+ss] = a;
          ((vsx_widget_ultra_chooser*)module_chooser)->module_tree->module_info = 0;
          ((vsx_widget_ultra_chooser*)module_chooser)->module_tree->add(a->identifier,a);
          ((vsx_module_chooser_list*)module_chooser_list)->add_item(a->identifier, a);
        }
        ((vsx_widget_ultra_chooser*)module_chooser)->build_tree();
        ((vsx_module_chooser_list*)module_chooser_list)->build_tree();
        ((vsx_module_chooser_list*)module_chooser_list)->show();

        //PORTANT STUFF
        //todo:: add more info to the command and store it in the module_info object
        // module_class is not necesarry as that is part of the info sent to the client
        // when a component is created.
      } else
      if (c->cmd == "server_message") {
        server_message = vsx_string_helper::base64_decode(c->parts[1]);
      } else
      if (c->cmd == "states_list") {
        vsx_module_specification* a = new vsx_module_specification;
        module_infos_created_for_choosers.push_back(a);
        a->identifier = "states;"+vsx_string_helper::str_replace<char>(":20:"," ",c->parts[1]);
        a->component_class = "state";
        ((vsx_widget_ultra_chooser*)state_chooser)->module_tree->module_info = 0;
        ((vsx_widget_ultra_chooser*)state_chooser)->module_tree->add(a->identifier,a);
      } else
      if (c->cmd == "states_list_end") {
        ((vsx_widget_ultra_chooser*)state_chooser)->build_tree();
      } else
      if (c->cmd == "prods_list") {
        vsx_module_specification* a = new vsx_module_specification;
        module_infos_created_for_choosers.push_back(a);
        a->identifier = "prods;"+vsx_string_helper::str_replace<char>(":20:"," ",c->parts[1]);
        a->component_class = "prod";
        ((vsx_widget_ultra_chooser*)state_chooser)->module_tree->module_info = 0;
        ((vsx_widget_ultra_chooser*)state_chooser)->module_tree->add("prods;"+a->identifier,a);
      } else
      if (c->cmd == "prods_list_end") {
        ((vsx_widget_ultra_chooser*)state_chooser)->build_tree();
      } else
      if (c->cmd == "visuals_list") {
        vsx_module_specification* a = new vsx_module_specification;
        module_infos_created_for_choosers.push_back(a);
        a->identifier = "visuals;"+vsx_string_helper::str_replace<char>(":20:"," ",c->parts[1]);
        a->component_class = "visuals";
        ((vsx_widget_ultra_chooser*)state_chooser)->module_tree->module_info = 0;
        ((vsx_widget_ultra_chooser*)state_chooser)->module_tree->add("visuals;"+a->identifier,a);
      } else
      if (c->cmd == "visuals_list_end") {
        ((vsx_widget_ultra_chooser*)state_chooser)->build_tree();
      }
      if (c->cmd == "resources_list" && c->parts.size() == 2) {
        vsx_module_specification* a;
        a = new vsx_module_specification;
        module_infos_created_for_choosers.push_back(a);
        a->identifier = vsx_string_helper::str_replace<char>(":20:"," ",c->parts[1]);
        a->component_class = "resource";
        ((vsx_widget_ultra_chooser*)resource_chooser)->module_tree->module_info = 0;
        ((vsx_widget_ultra_chooser*)resource_chooser)->module_tree->add(a->identifier,a);
      } else
      if (c->cmd == "resources_list_end") {
        ((vsx_widget_ultra_chooser*)resource_chooser)->build_tree();
      } else
      if (c->cmd == "alert_fail") {
        vsx_vector3<> a;
        for (std::list <vsx_widget*>::iterator it = root->children.begin(); it != root->children.end(); ++it) {
          a = (*it)->pos;
        }
        vsx_widget* msg = root->add(new dialog_messagebox(c->parts[2],vsx_string_helper::base64_decode(c->parts[3])+"|"),"alert");
        msg->target_pos = msg->pos+alert_delta;
        if (msg->target_pos.x > 0.8) msg->target_pos.x = 0.2;
        if (msg->target_pos.y > 0.8) msg->target_pos.y = 0.2;
      } else
      if (c->cmd == "macro_prerun") {
        if (c->parts[1] == connection_id) {
          // woop
          vsx_command_list macro_commands(false);
          macro_commands.load_from_file(
            vsx_string_helper::str_replace<char>(
              ";",
              "/",
              vsx_data_path::get_instance()->data_path_get() +
              vsx_string_helper::str_replace<char>(
                "macros;",
                "macros/",
                c->parts[2],
                1
              )
            )
          );
          macro_commands.token_replace("$$name",c->parts[3]);
          macro_commands.parse();
          macro_commands.reset();
          vsx_command_s* mc = 0;
          // check the macro list to verify the existence of the componente we need for this macro
          bool components_existing = true;
          vsx_string<>failed_component = "";
          while ( (mc = macro_commands.get()) ) {
            if (mc->cmd == "component_create") {
              if (!(module_list.find(mc->parts[1]) != module_list.end())) {
                failed_component = mc->parts[2];
                components_existing = false;
              }
            }
          }
          if (components_existing) {
            macro_commands.reset();
            while ( (mc = macro_commands.get()) ) {
              if (mc->cmd == "macro_create" && mc->parts.size() < 4) {
                cmd_out->add_raw("macro_create "+mc->parts[1]+" "+c->parts[4]+" "+c->parts[5]+" "+mc->parts[2]);
              } else {
                if (mc->cmd != "")
                {
                  cmd_out->add( mc );
                }
              }
            }
          } else {
            a_focus->add(new dialog_messagebox("Error: Could not run macro","Server lacks necesarry module:|("+failed_component),"macro_create_error");
          }
        }
      }
      else log(">"+c->raw);
      VSXP_S_END
    }
  }

  bool run_volatile = true;
  while (run_volatile)
  {
    vsx_widget_component *comp;
    c = command_q_f.pop();
    if (c) {
      if (
        c->cmd == "pseq_p_ok" ||
        c->cmd == "pseq_r_ok")
      {
        vsx_widget* tc = find_component(c->parts[2]);
        if (tc) {
          command_q_b.addc(c);
          tc->vsx_command_queue_b(this);
          run_volatile = false;
        }
      } else
      if (c->cmd == "vsxl_pfi_ok") {
        vsx_widget* tc = find_component(c->parts[1]);
        if (tc) {
          command_q_b.addc(c);
          tc->vsx_command_queue_b(this);
          run_volatile = false;
        }
      } else
      if (c->cmd == "param_connect_volatile" ||
      c->cmd == "pcva"
      ) {
        //find in children
        if ( (comp = (vsx_widget_component*)find_component(c->parts[1])) ) {
          command_q_b.addc(c, VSX_COMMAND_GARBAGE_COLLECT);
          comp->vsx_command_queue_b(this,true);
          run_volatile = false;
        } else {
          command_q_f.add_front(c);
          run_volatile = false;
        }
      }
    }
    else run_volatile = false;
  }
  vsx_widget::vsx_command_queue_f();
}

// messages to the engine from other widgets
void vsx_widget_server::command_process_back_queue(vsx_command_s *t) {
  // process messages from the children, to the engine and to the system
  // cmd_out messages sent to engine
  if (
  t->cmd == "state_load"
  || t->cmd == "param_connect"
  || t->cmd == "param_disconnect"
  || t->cmd == "param_alias"
  || t->cmd == "param_unalias"
  || t->cmd == "connections_order"
  || t->cmd == "macro_create"
  || t->cmd == "component_create"
  || t->cmd == "component_assign"
  || t->cmd == "component_delete") {
    if (((vsx_artiste_desktop*)root)->auto_undo)
    undo_s();
  }

  if (t->cmd == "conf" || t->cmd == "system.shutdown") {
    command_q_b.add(t);
    return;
  }
  if (cmd_out)
  {
    if (t->cmd == "add_empty_macro") {
      cmd_out->add_raw("macro_create "+get_unique_name("empty")+" "+vsx_string_helper::str_replace<char>(","," ",t->cmd_data)+" 0.1");
    } else
    if (t->cmd == "add_note") {
      cmd_out->add_raw("note_create "+get_unique_name("note")+" "+t->cmd_data+",0.0 0.05,0.05,0.0 " + vsx_string_helper::base64_encode("text")+" 0.004");
    } else
    if (t->cmd == "delete_sequencer") {
      sequencer->_delete();
      sequencer = 0;
    } else
    if (t->cmd == "show_module_browser") {
      front(module_chooser);
      ((vsx_widget_ultra_chooser*)module_chooser)->message = "Drag and drop to create a module instance";
      ((vsx_widget_ultra_chooser*)module_chooser)->mode = 0;
      ((vsx_widget_ultra_chooser*)module_chooser)->show();
    }
    else
    if (t->cmd == "show_module_browser_list") {
      front(module_chooser_list);
      ((vsx_module_chooser_list*)module_chooser_list)->show();
    }
    else
    if (t->cmd == "show_connect_dialog")
    {
      ((dialog_query_string*)connect_dialog)->name = "connect";
      ((dialog_query_string*)connect_dialog)->show("127.0.0.1");
    }
    else
    if (t->cmd == "connect")
    {
      #if VSX_DEBUG
      printf("creating new server %s\n",t->parts[1].c_str());
      #endif
      // 1. create a new server widget
      vsx_widget* ns = add( new vsx_widget_server, "server "+t->parts[1] );
      ns->set_pos(vsx_vector3<>(1.0f,0.0f));
      ns->color.b = 255.0/255.0;
      ns->color.g = 200.0/255.0;
      ns->color.r = 200.0/255.0;
      ns->size.x = 2;
      ns->size.y = 2;

      ((vsx_widget_server*)ns)->server_connect(t->parts[1],"1234");
      ns->init();
      ns->set_size(vsx_vector3<>(2.0f,2.0f));
      // 2. tell the new server to connect its command lists
    } else
    // 1: menu choice is done
    if (t->cmd == "state_menu_save") {
      //((dialog_query_string*)export_dialog_state)->
      ((dialog_query_string*)export_dialog_state)->name = "state_save";
      #ifdef VSX_DEBUG
        printf("state_name is: %s\n",state_name.c_str());
      #endif
      ((dialog_query_string*)export_dialog_state)->show(state_name);
      return;
    }
    else
    if (t->cmd == "state_save") {
      state_name = t->parts[1];
      cmd_out->add_raw("meta_set "+t->parts[2]);
      cmd_out->add_raw("state_save "+t->parts[1]);
      command_q_b.add_raw("conf last_saved_state " + vsx_string_helper::base64_encode(t->parts[1]) );

      return;
    }
    else
    // 1: menu choice is done
    if (t->cmd.substr(0,14) == "package_export") {
      // do export
      vsx_string<>end = t->cmd.substr(-7);
      if (end != "_cancel") {
        vsx_string<>cc = t->cmd.substr(15); // visuals
        //::MessageBox(0, end.c_str(), cc.c_str(), MB_OK);
        cmd_out->add_raw("meta_set "+t->parts[2]);
        if (t->parts[1].substr(-4) != ".vsx") t->parts[1] += ".vsx";
        cmd_out->add_raw("package_export "+t->parts[1]+" "+cc+"/");
      }
    } else
    if (t->cmd.substr(0,16) == "m_package_export") {
      vsx_string<>cc = t->cmd.substr(16);
      ((dialog_query_string*)export_dialog_ext)->name = "package_export"+cc; // package_export_visuals
      ((dialog_query_string*)export_dialog_ext)->show();
    }
    else
    if (t->cmd == "state_template_fader") {
      cmd_out->add_raw("clear");
      cmd_out->add_raw("component_pos screen0 0.000000 0.003253");
      cmd_out->add_raw("component_create maths;arithmetics;binary;sub sub -0.208551 -0.009255");
      cmd_out->add_raw("param_set sub param1 0.99999243021011353000");
      cmd_out->add_raw("component_create renderers;basic;textured_rectangle textured_rectangle -0.114690 -0.008265");
      cmd_out->add_raw("param_set textured_rectangle facing_camera 0");
      cmd_out->add_raw("param_set textured_rectangle size 0.99999111890792847000");
      cmd_out->add_raw("param_set textured_rectangle angle 0.00000943537361308699");
      cmd_out->add_raw("component_create renderers;basic;textured_rectangle textured_rectangle1 -0.113667 -0.088924");
      cmd_out->add_raw("param_set textured_rectangle1 facing_camera 0");
      cmd_out->add_raw("component_create system;visual_fader visual_fader -0.306399 -0.086744");
      cmd_out->add_raw("note_create note101112111143 -0.286156,0.030803,0.000000 0.221093,0.127068,0.000000 ZmFkZV9wb3Nfb3V0IG1vdmVzIGZyb20gMCB0byAxLCB0aGlzIHdpbGwgaGF2ZSB0byBjb250cm9sIGEgY29tcGxldGUKZmFkZSBmcm9tIG9uZSB0ZXh0dXJlIHRvIGFub3RoZXIuIApJdCBzaG91bGQgZmFkZSBmcm9tIHRleHR1cmVfYV9vdXQgdG8gdGV4dHVyZV9iX291dCwKSXQgc2hvdWxkIGdvIGZyb20gQkxVRSB0byBSRUQKc28gd2hlbiAKICBmYWRlX3Bvc19vdXQgPSAwLjAgCiAgdGV4dHVyZV9hX291dCBzaG91bGQgYmUgMTAwJSB2aXNpYmxlLgoKQSBmZXcgcG9pbnRlcnMgb24gd2hhdCB0byBkbyBhbmQgbm90OgogLSBhdm9pZCB0ZXh0dXJlIGJ1ZmZlcnMgaWYgeW91IGNhbiwgdGhleSBjb25zdW1lIGxvdHMgb2YgcmFtCiAtIHBvdGVudGlhbGx5IDEwMCdzIG9mIHRoZXNlIHN0YXRlcyBjYW4gYmUgbG9hZGVkIGludG8gbWVtb3J5IHNvIHRyeSB0bwogICBrZWVwIHNpemUgZG93biAobm90IHVzaW5nIGFueSBvdGhlciB0ZXh0dXJlcyBldGMpCiAtIA== 0.008000");
      cmd_out->add_raw("param_connect textured_rectangle1 texture_in visual_fader texture_b_out");
      cmd_out->add_raw("param_connect textured_rectangle texture_in visual_fader texture_a_out");
      cmd_out->add_raw("param_connect textured_rectangle global_alpha sub sum");
      cmd_out->add_raw("param_connect sub param2 visual_fader fade_pos_out");
      cmd_out->add_raw("param_connect screen0 screen textured_rectangle1 render_out");
      cmd_out->add_raw("param_connect screen0 screen textured_rectangle render_out");
      return;
    } else
    if (t->cmd == "state_template_visual") {
      cmd_out->add_raw("clear");
      cmd_out->add_raw("component_create renderers;basic;colored_rectangle colored_rectangle -0.150612 0.014989");
      cmd_out->add_raw("param_set colored_rectangle size 1.00000000000000000000,0.03820488974452018700,0.00000000000000000000");
      cmd_out->add_raw("param_set colored_rectangle border_enable 0");
      cmd_out->add_raw("param_set colored_rectangle rotation_axis 0.00000088326197555944,0.00000000005320408350,0.99999994039535522000");
      cmd_out->add_raw("component_create sound;input_visualization_listener input_visualization_listener -0.305812 0.004698");
      cmd_out->add_raw("component_pos screen0 0.000000 0.003253");
      cmd_out->add_raw("note_create note2000100020 0.110675,-0.014352,0.000000 0.344248,0.250302,0.000000 LSB1c2Uga2V5Ym9hcmQga2V5cyB0byBuYXZpZ2F0ZSBkZXNrdG9wCiAgIHcgZSByICAodyAmIHIgem9vbSkKICAgcyBkIGYgIChzZGZlIHRvIG1vdmUpCi0gY3RybCtkb3VibGVjbGljayBvbiB0aGUgZGVza3RvcAogIHRvIGxvYWQgZXhhbXBsZXMKLSBkb3VibGVjbGljayB0aGUgZGVza3RvcCB0byBsb2FkIAogIGFkZGl0aW9uYWwgbW9kdWxlcwotIGJyaW5nIHVwIGx1bmEgKHRhYikgb3IgaG9sZCBtb3VzZQogIG92ZXIgYSBtb2R1bGU= 0.022000");
      cmd_out->add_raw("note_create note10111211121 -0.188187,0.038348,0.000000 0.076758,0.050000,0.000000 LSBkb3VibGVjbGljayB0aGUgWytdCi0gc291bmQgY29udHJvbHMgYW5nbGUKLSBob2xkIGN0cmwgJiByaWdodGNsaWNrIHRoZSB3aXJlIHRvIGRpc2Nvbm5lY3QKICAtIG9ubHkgd2hlbiBpdCdzIG9wZW4hCi0gcmlnaHQtY2xpY2sgYSBwYXJhbWV0ZXIgdG8gY2hvb3NlIGEgY29udHJvbGxlcgotIHJpZ2h0LWNsaWNrIHRoZSBtb2R1bGUgdG8gZ2V0IGEgbWVudQ== 0.004000");
      cmd_out->add_raw("note_create note01222322222 -0.331999,0.047805,0.000000 0.097616,0.050140,0.000000 VGhpcyBtb2R1bGUgaXMgZ3JhYmJpbmcgc291bmQgZnJvbSB0aGUgCnNvdW5kY2FyZCdzIHJlY29yZGluZyBzZXR0aW5nIGFuZCBwcm9jZXNzaW5nIGl0Ci0gdnUgaXMgYSB2b2x1bWUgbWV0ZXIsIGdlbmVyYWwgc291bmQgcG93ZXIgcmlnaHQgbm93Ci0gb2N0YXZlcyBpcyBodW1hbi1wZXJjZWl2ZWQgc291bmQgb2N0YXZlcywgdmVyeQogIHVzZWZ1bCBmb3IgcmVhY3RpbmcgdG8gbXVzaWMKLSB3YXZlIGlzIGFuIGFycmF5IGNvbnRhaW5pbmcgdGhlIHNvdW5kLCB1c2VmdWwKICBmb3IgcmVuZGVyaW5nIGFuIG9zY2lsbGF0b3IgKGNhbGxlZCBhIGZsb2F0IGFycmF5KQotIG5vcm1hbC9ocSBhcmUgYWxzbyBhcnJheXMgY29udGFpbmluZyBmdWxsCiAgc3BlY3RydW1zLCB5b3UgY2FuIHBpY2sgb3V0IGEgc2luZ2xlIHZhbHVlIGluIHRoZXNlIHdpdGggdGhlCiAgbW9kdWxlIGxvY2F0ZWQgYXQgbWF0aHM7YXJyYXk7ZmxvYXRfYXJyYXlfcGljaw== 0.004000");
      cmd_out->add_raw("param_connect screen0 screen colored_rectangle render_out");
      cmd_out->add_raw("param_connect colored_rectangle angle input_visualization_listener vu_l");
      return;
    } else
    // 1: menu choice, bring up chooser
    if (t->cmd == "state_menu_load") {
      front(state_chooser);
      ((vsx_widget_ultra_chooser*)state_chooser)->message = "Double click to load a project";
      ((vsx_widget_ultra_chooser*)state_chooser)->mode = 1;
      ((vsx_widget_ultra_chooser*)state_chooser)->command_receiver = this;
      ((vsx_widget_ultra_chooser*)state_chooser)->command = "state_load";
      ((vsx_widget_ultra_chooser*)state_chooser)->show();
      return;
    } else
    if (t->cmd == "resource_menu") {
      front(resource_chooser);
      ((vsx_widget_ultra_chooser*)resource_chooser)->message = "Resource viewer";
      ((vsx_widget_ultra_chooser*)resource_chooser)->mode = 1;
      ((vsx_widget_ultra_chooser*)resource_chooser)->command_receiver = this;
      ((vsx_widget_ultra_chooser*)resource_chooser)->command = "";
      ((vsx_widget_ultra_chooser*)resource_chooser)->show();
      return;
    }
    else
    if (t->cmd == "sequence_menu") {
      if (!sequencer) {
        sequencer = (vsx_widget*)add(new vsx_widget_sequence_editor,"Main Sequencer");
        sequencer->init();
        ((vsx_widget_sequence_editor*)sequencer)->set_server(this);
        ((vsx_widget_sequence_editor*)sequencer)->set_timeline_show_wave_data(true);
      }
      sequencer->visible = 1;
      front(sequencer);
    }
    else
    if (t->cmd == "seq_pool_menu") {
      if (!seq_pool) {
        seq_pool = (vsx_widget*)add(new vsx_widget_seq_pool_manager,name+"sequence_pool");
        seq_pool->init();
        ((vsx_widget_seq_pool_manager*)seq_pool)->set_server(this);
      }
      seq_pool->show();
      //seq_pool->set_render_type(render_3d);
      front(seq_pool);
    }
    else
    if (t->cmd == "macro_create_real") {
      t->cmd = "macro_create";
      t->parts[0] = "macro_create";
      t->raw = vsx_string_helper::str_replace<char>("macro_create_real", "macro_create",t->raw);
      cmd_out->add( t );
    } else
    if (t->cmd == "macro_create") {
      //printf("macro create %s\n",t->parts[1].c_str());
      if (t->parts[1] == "macros;empty" || t->cmd == "macro_create_real") {
        cmd_out->add_raw("macro_create "+t->parts[2]+" "+t->parts[3]+" "+t->parts[4]+" 0.1");
      } else {
        cmd_out->add_raw("macro_prerun "+connection_id+" "+t->parts[1]+" "+t->parts[2]+" "+t->parts[3]+" "+t->parts[4]+" 0.1");
      }
    } else
    if (t->cmd == "module_info_add") {
      // syntax:
      //         module_info_add [identifier] {information} {in_param_spec} {out_param_spec}
      if (t->parts.size() > 1) {
        bool duplicate = false;
        vsx_module_specification* a;
        a = new vsx_module_specification;
        module_infos_created_for_choosers.push_back(a);
        if (module_list.find(t->parts[1]) != module_list.end()) duplicate = true;
        module_list[t->parts[1]] = a;
        a->identifier = t->parts[1];
        a->component_class = "macro";
        if (!duplicate) {
          ((vsx_widget_ultra_chooser*)module_chooser)->module_tree->module_info = 0;
          ((vsx_widget_ultra_chooser*)module_chooser)->module_tree->add(t->parts[1],a);
          ((vsx_widget_ultra_chooser*)module_chooser)->build_tree();
        }
      }
    } else
    if (t->cmd == "alert_dialog") {
      root->add(new dialog_messagebox(t->parts[1],(vsx_string_helper::base64_decode(t->parts[2])+"|")),"alert");
    } else
    {
      LOG_A("adding copy:"+t->cmd)
      cmd_out->addc( t, VSX_COMMAND_GARBAGE_COLLECT );
    }
  }
}

void vsx_widget_server::param_alias_ok(vsx_string<>p_def, vsx_string<>io, vsx_string<>comp, vsx_string<>param, vsx_string<>source_comp, vsx_string<>source_param, vsx_string<>seven) {
  // 1. create new anchor
  // 2. copy info to it
  command_q_b.clear_delete();
  vsx_widget* dest = find_component(comp);
  if (io == "-1") {
    command_q_b.add_raw("ipsa "+comp+" "+p_def+" 1", VSX_COMMAND_GARBAGE_COLLECT);
    dest->vsx_command_queue_b(this);
  } else
  if (io == "1") {
    command_q_b.add_raw("opsa "+comp+" "+p_def+" 1", VSX_COMMAND_GARBAGE_COLLECT);
    dest->vsx_command_queue_b(this);
  }

  // find the anchor that should own the connector
  for (std::list<vsx_widget*>::iterator it = dest->children.begin(); it != dest->children.end(); ++it) {
    if ((*it)->widget_type == VSX_WIDGET_TYPE_ANCHOR) {
      ((vsx_widget_anchor*)*it)->alias = true;
    }
  }
  if (io == "-1") {
    vsx_widget_component* conn_dest = (vsx_widget_component*)find_component(source_comp);
    command_q_b.add_raw("pcva "+source_comp+" "+source_param+" "+comp+" "+param+" "+seven+" "+io, VSX_COMMAND_GARBAGE_COLLECT);
    conn_dest->vsx_command_queue_b(this);
  } else {
    vsx_widget_component* conn_dest = (vsx_widget_component*)find_component(comp);
    command_q_b.add_raw("pcva "+comp+" "+param+" "+source_comp+" "+source_param+" "+seven+" "+io, VSX_COMMAND_GARBAGE_COLLECT);
    conn_dest->vsx_command_queue_b(this);
    ((vsx_widget_component*)dest)->macro_fix_anchors();
  }
}

bool vsx_widget_server::event_key_down(uint16_t key)
{
  if (vsx_input_keyboard.pressed_ctrl())
  {
    if (key == 'z' || key == 'Z')
    {
      cmd_out->add_raw("undo");
    }
  }
  return true;
}

void vsx_widget_server::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  selection = false;
  delta_move = delta_zoom = 0.0f;
  selection_start = distance.center;
  mouse_click_position = vsx_input_mouse.position;

  if (button == 0)
  {
    selection_end = selection_start;
    if (vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_alt())
    {
      selection = true;
    } else
    if (!vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_alt())
    {
      for (std::list<vsx_widget*>::iterator it = selected_list.begin(); it != selected_list.end(); ++it)
      {
          ((vsx_widget_component*)(*it))->selected = false;
      }
      selected_list.clear();
    }
  }
  vsx_widget::event_mouse_down(distance,coords,button);
}

void vsx_widget_server::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);
  if (button == 0 && vsx_input_keyboard.pressed_alt() && !vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_ctrl())
  {
    command_q_b.add_raw("add_empty_macro "+vsx_string_helper::f2s(distance.center.x)+","+vsx_string_helper::f2s(distance.center.y));
    vsx_command_queue_b(this);
  } else
  if (button == 0 && vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_alt())
  {
    command_q_b.add_raw("state_menu_load");
    vsx_command_queue_b(this);
  } else
  if (button == 0 && !vsx_input_keyboard.pressed_ctrl() && !vsx_input_keyboard.pressed_shift() && !vsx_input_keyboard.pressed_alt())
  {
    front(module_chooser);
    ((vsx_widget_ultra_chooser*)state_chooser)->message = "";
    ((vsx_widget_ultra_chooser*)module_chooser)->mode = 0;
    ((vsx_widget_ultra_chooser*)module_chooser)->show();
  }
}

void vsx_widget_server::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  if (selection)
  {
    selection_end = distance.center;
    vsx_vector2f a;
    vsx_vector2f b;
    a = selection_start;
    b = selection_end;

    if (a.x < b.x) {
      float t = a.x;
      a.x = b.x;
      b.x = t;
    }
    if (a.y < b.y) {
      float t = a.y;
      a.y = b.y;
      b.y = t;
    }

    for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it) {
      if (
        ((*it)->pos.x < a.x) &&
        ((*it)->pos.y < a.y) &&
        ((*it)->pos.x > b.x) &&
        ((*it)->pos.y > b.y)
      ) {
        if ((*it)->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
          select_add(*it);
          a_focus = *it;
          k_focus = *it;
        }
      } else {
        if ((*it)->widget_type == VSX_WIDGET_TYPE_COMPONENT) {
          ((vsx_widget_component*)(*it))->selected = false;
          selected_list.remove(*it);
        }
      }
    }
    return;
  }


  if (!vsx_input_mouse.button_left && vsx_input_mouse.button_right)
  {
    if (vsx_input_mouse.position != mouse_click_position)
    {
      float dz = (vsx_input_mouse.position.y - mouse_click_position.y);
      camera.set_extra_movement( vsx_vector3f(0, 0, -dz) * 0.1f );
      vsx_mouse_control.hide_cursor();
      vsx_mouse_control.set_cursor_pos(mouse_click_position);
    }
    return;
  }

  if (server_type == VSX_WIDGET_SERVER_CONNECTION_TYPE_INTERNAL)
  if (vsx_input_mouse.button_left && !vsx_input_mouse.button_right)
  {
    if (vsx_input_mouse.position != mouse_click_position)
    {
      vsx_vector3f delta(
        - (vsx_input_mouse.position.x - mouse_click_position.x),
        - (vsx_input_mouse.position.y - mouse_click_position.y)
      );
      camera.set_extra_movement( delta * 0.1 );
      vsx_mouse_control.hide_cursor();
      vsx_mouse_control.set_cursor_pos(mouse_click_position);
    }
    return;
  }

  vsx_widget::event_mouse_move(distance,coords);
}

void vsx_widget_server::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  vsx_mouse_control.show_cursor();
  if (selection)
  {
    vsx_vector3f pp = parent->get_pos_p() - pos;
    vsx_vector3<> a = vsx_vector3f(selection_start.x, selection_start.y) - pp;
    vsx_vector3<> b = vsx_vector3f(selection_end.x, selection_end.y) - pp;
    for (std::list <vsx_widget*>::iterator it=children.begin(); it != children.end(); ++it)
    {
      if (
        ((*it)->pos.x > a.x) &&
        ((*it)->pos.y > a.y) &&
        ((*it)->pos.x < b.x) &&
        ((*it)->pos.y < b.y)
        )
      {
        if ((*it)->widget_type == VSX_WIDGET_TYPE_COMPONENT)
        select_add(*it);
      }
    }
    selection = false;
  }
  else
  {
    if (delta_zoom < 0.09f && delta_move < 0.09f)
    vsx_widget::event_mouse_up(distance,coords,button);
  }
}

void vsx_widget_server::event_mouse_wheel(float y)
{
  parent->event_mouse_wheel(y);
}

void vsx_widget_server::draw()
{
  glEnable(GL_BLEND);

  if (!init_run)
    return;

  if (!visible)
    return;

  float x = pos.x+parent->pos.x;
  float y = pos.y+parent->pos.y;

  if (!performance_mode || server_type == VSX_WIDGET_SERVER_CONNECTION_TYPE_SOCKET)
  {
    mtex->bind();
    glColor4f(color.r,color.g,color.b,color.a);
    if (server_type == VSX_WIDGET_SERVER_CONNECTION_TYPE_SOCKET)
    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    }

    glBegin(GL_QUADS);
      glTexCoord2f(0, 1);
      glVertex2f(x-size.x/1.6,y-size.y/1.6);
      glTexCoord2f(0, 0);
      glVertex2f(x-size.x/1.6,y+size.y/1.6);
      glTexCoord2f(1, 0);
      glVertex2f(x+size.x/1.6,y+size.y/1.6);
      glTexCoord2f(1, 1);
      glVertex2f(x+size.x/1.6,y-size.y/1.6);
    glEnd();
    mtex->_bind();
  }

  if (selection)
  {
    vsx_widget_skin::get_instance()->set_color_gl_a(0, 0.3);
    vsx_vector2f s_s = selection_start + vsx_vector2f(pos.x, pos.y);
    vsx_vector2f s_e = selection_end + vsx_vector2f(pos.x, pos.y);
    draw_box(s_s, s_e.x-s_s.x,s_e.y-s_s.y);
    glLineWidth(1);
    glColor4f(0.4,0.4,0.6,0.7);
    glBegin(GL_LINE_STRIP);
      glVertex2f(s_s.x,s_s.y);
      glVertex2f(s_e.x,s_s.y);
      glVertex2f(s_e.x,s_e.y);
      glVertex2f(s_s.x,s_e.y);
      glVertex2f(s_s.x,s_s.y);
    glEnd();
  }

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (server_message.size()) {
    font.print_center(vsx_vector3<>(x,y), server_message,0.03);
  }

  draw_children();

}



vsx_widget* vsx_widget_server::find_component(vsx_string<>name)
{
  // support "containers" here in the future
  if (comp_list.find(name) != comp_list.end())
    return comp_list[name];
  return 0;
}

void vsx_widget_server::select(vsx_widget* comp) {
  // search for the comp in the list, if found do nothing
  for (std::list<vsx_widget*>::iterator it = selected_list.begin(); it != selected_list.end(); ++it) {
    if (*it == comp) return;
  }
  for (std::list<vsx_widget*>::iterator it = selected_list.begin(); it != selected_list.end(); ++it) {
    ((vsx_widget_component*)(*it))->selected = false;
  }
  selected_list.clear();
  selected_list.push_back(comp);
  ((vsx_widget_component*)comp)->selected = true;
}

bool vsx_widget_server::select_add(vsx_widget* comp) {
  // is it already present?
  for (std::list<vsx_widget*>::iterator it = selected_list.begin(); it != selected_list.end(); ++it) {
    if (*it == comp) {
      return true;
    }
  }
  if (selected_list.size()) {
    if (comp->parent != (*(selected_list.begin()))->parent) return false;
  }
  selected_list.push_back(comp);
  ((vsx_widget_component*)comp)->selected = true;
  selected_list.unique();
  return true;
}

void vsx_widget_server::select_add_gui(vsx_widget* comp) {
    for (std::list<vsx_widget*>::iterator it = selected_list.begin(); it != selected_list.end(); ++it) {
      if (*it == comp) {
        selected_list.remove(comp);
        ((vsx_widget_component*)(comp))->selected = false;
        return;
      }
    }
    if (selected_list.size()) {
      if (comp->parent != (*(selected_list.begin()))->parent) return;
    }
    selected_list.push_back(comp);
    ((vsx_widget_component*)comp)->selected = true;
    selected_list.unique();
  }

vsx_string<>vsx_widget_server::get_unique_name(vsx_string<>name)
{
  vsx_string<> i_val;
  vsx_string<> delimiter = "_";
  vsx_nw_vector< vsx_string<> > parts;
  vsx_string_helper::explode(name, delimiter, parts);

  if (parts.size() == 1)
    parts.push_back("1");

  vsx_string<> &final_part = parts[parts.size()-1];
  foreach(final_part, i)
    if (final_part[i] >= '0' && final_part[i] <= '9')
      i_val.push_back(final_part[i]);

  if (!i_val.size())
      parts.push_back("0");

  size_t component_suffix_value = 1;
  if (i_val.size())
    component_suffix_value = vsx_string_helper::s2i(i_val);

  forever
  {
    parts[parts.size() - 1] = vsx_string_helper::i2s(component_suffix_value);
    vsx_string<> proposed_new_name = vsx_string_helper::implode(parts, delimiter);

    if (!find_component(proposed_new_name))
      return proposed_new_name;

    component_suffix_value++;
  }
}

vsx_string<>vsx_widget_server::build_comp_helptext(vsx_string<>path) {
  if (module_list.find(path) != module_list.end()) {
  return vsx_string<>("Module description:\n\n"+module_list[path]->description+"|||\n\
Module path:\n\
  "+path);
  }
  return vsx_string<>("");
}

