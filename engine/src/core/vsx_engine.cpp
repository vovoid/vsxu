#include <vsx_platform.h>

#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
  #include <io.h>
#endif

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <dlfcn.h>
  #include <syslog.h>
#endif

#include <dirent.h>
#include <sys/types.h>
#include "vsx_string.h"
#include "vsx_log.h"
#include "vsx_engine.h"
#include "vsx_master_sequencer/vsx_master_sequence_channel.h"
//#include "vsx_module_dll_info.h"
#include "vsx_note.h"

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#endif

#ifdef VSXU_ENGINE_STATIC
#include "vsx_module_static_factory.h"
#endif

#ifndef VSXE_NO_GM
#include "gm/gmThread.h"    // Game monkey script machine and thread
#include "gm/gmMachine.h"
#include "binds/gmCall.h"
#include "binds/gmMathLib.h"
#include "scripting/vsx_param_vsxl.h"
#include "scripting/vsx_comp_vsxl.h"
#include "scripting/vsxl_engine.h"
#endif

#include <vector>


#ifdef VSXU_ENGINE_INFO_STATIC
vsx_module_engine_info vsx_engine::engine_info;
#endif

using namespace std;

void vsx_engine::constructor_set_default_values()
{
  module_list = 0x0;
  valid = false;
  no_send_client_time = false;
  g_timer_amp = 1.0f;
  engine_info.amp = 1.0f;
  engine_info.vtime = 0;
  engine_info.dtime = 0;
  engine_info.real_vtime = 0;
  engine_info.real_dtime = 0;
  engine_info.filesystem = &filesystem;
  engine_info.request_play = 0;
  engine_info.request_stop = 0;
  engine_info.request_rewind = 0;
  engine_info.request_set_time = -0.01f;
  vsxl = 0;
  lastsent = 0;
  sequence_pool.set_engine((void*)this);
  last_e_state = current_state = VSX_ENGINE_STOPPED;
  // on unix/linux, resources are now stored in ~/.vsxu/data/resources
  filesystem.set_base_path(vsx_get_data_path());
  frame_cfp_time = 0.0f;
  last_m_time_synch = 0;
  first_start = true;
  stopped = true;
  // rendering hints
  render_hint_module_output_only = false;
  frame_dcount = 0;
  frame_dtime = 0;
  frame_dprev = -1;
  frame_dfps = 0;
  frame_d = 50;
  component_name_autoinc = 0;
}

vsx_engine::vsx_engine()
{
  constructor_set_default_values();
}

vsx_engine::vsx_engine(vsx_string path)
{
  constructor_set_default_values();
  log_dir = path;
}


vsx_engine::~vsx_engine()
{
  stop();
  commands_internal.clear(true);
  commands_res_internal.clear(true);
  commands_out_cache.clear(true);
  i_clear(0,true);
}


vsx_module_list_abs* vsx_engine::get_module_list()
{
  return module_list;
}

void vsx_engine::set_module_list( vsx_module_list_abs* new_module_list )
{
  module_list = new_module_list;
  engine_info.module_list = (void*) new_module_list;
}

int vsx_engine::get_engine_state()
{
  return current_state;
}

int vsx_engine::get_modules_left_to_load()
{
  return modules_left_to_load;
}

int vsx_engine::get_modules_loaded()
{
  return modules_loaded;
}



vsx_sequence_pool* vsx_engine::get_sequence_pool()
{
  return &sequence_pool;
}

void vsx_engine::set_no_send_client_time(bool new_value)
{
  no_send_client_time = new_value;
}

bool vsx_engine::get_commands_internal_status()
{
  if (commands_internal.count() == 0)
    return true;
  return false;
}

float vsx_engine::get_frame_elapsed_time()
{
  return g_timer.atime() - frame_start_time;
}

vsx_module_engine_info* vsx_engine::get_engine_info()
{
  return &engine_info;
}

void vsx_engine::reset_time()
{
  g_timer.start();
}

vsx_module_param_abs* vsx_engine::get_in_param_by_name(vsx_string module_name, vsx_string param_name)
{
  if (!valid)
    return 0x0;
  vsx_comp* c = get_component_by_name(module_name);
  if (c) {
    vsx_engine_param* p = c->get_params_in()->get_by_name(param_name);
    if (p) return p->module_param;
  }
  return 0x0;
}

void vsx_engine::reset_input_events()
{
  if (!valid) return;
  engine_info.num_input_events = 0;
}

vsx_comp* vsx_engine::get_component_by_name(vsx_string label)
{
  if (forge_map.find(label) != forge_map.end())
  {
    return forge_map[label];
  }
  return 0;
}

vsx_comp* vsx_engine::get_by_id(unsigned long id)
{
  return forge[id];
}


void vsx_engine::input_event(vsx_engine_input_event &new_input_event)
{
  if (!valid) return;
  if (engine_info.num_input_events < VSX_ENGINE_INPUT_EVENT_BUFSIZE)
  {
    engine_info.input_events[engine_info.num_input_events] = new_input_event;
    engine_info.num_input_events++;
  }
}

int vsx_engine::i_load_state(vsx_command_list& load1,vsx_string *error_string, vsx_string info_filename)
{
  if (!valid) return 2;
  LOG("i_load_state 1")
  vsx_command_list load2,loadr2;
  load1.reset();
  vsx_command_s* mc = 0;
  // check the macro list to verify the existence of the componente we need for this macro
  //bool components_existing = true;
  vsx_string failed_component = "";
  while ( (mc = load1.get()) )
  {
    if (mc->cmd == "component_create")
    {
      // verify that the module is present and can be loaded
      if
      (
          !module_list->find( mc->parts[1] )
      )
      {
        failed_component = mc->parts[2];
        //components_existing = false;
        if (error_string) *error_string = "VSX Engine could not find or load module: "+mc->parts[1];
        printf( "%s\n",vsx_string(
                  "**************************************************\n"
                  "Notice: \n\tVSX Engine could not load module: "
                  "'"+mc->parts[1]+"'"
                  "\n\tThis prevented the project: \n\t\t'"+
                  info_filename+"'\n"+
                  "\tfrom loading.\n"
                  "\tThis is most likely from your GPU/drivers do not support some feature needed.\n"
                  "\tPlease consider upgrading your hardware or drivers.\n"
                  "\tIf you are a developer, this might mean some modules didn't compile properly.\n"
                  "**************************************************\n\n").c_str() );

        LOG3("Module missing in engine: "+mc->parts[1]);
        return 1;
      }
    }
  }
  static vsx_string sld("state_load_done");
  load1.add_raw(sld);
  load1.reset();
  //if (components_existing)
  {
    LOG("i_load_state: all modules are available, proceeding with load")

    LOG("stopping")
    stop();
    LOG("i_clear")
    i_clear();
    LOG("start()")

    //load2.add_raw("clear");
    //process_message_queue(&load2,&loadr2,true);
    start();
    LOG("i_load_state pre processing_message_queue")

    process_message_queue(&load1,&loadr2,true);
    LOG("i_load_state post processing_message_queue")
    load2.clear(true);
    loadr2.clear(true);
  }
  load1.clear(true);
  current_state = VSX_ENGINE_LOADING;
  g_timer.start();
  modules_loaded = 0;
  modules_left_to_load = 0;
  return 0;
}

int vsx_engine::load_state(vsx_string filename, vsx_string *error_string)
{
  if (!valid) return 2;
  LOG("load_state 1")
  filesystem.set_base_path("");
  if (filesystem.is_archive())
  {
    LOG("vsx_engine::load_state closing filesystem archive")
    filesystem.archive_close();
  }


  LOG("load_state 2")
  vsx_command_list load1;
  load1.filesystem = &filesystem;
  vsx_string i_filename = filename;
  LOG("load_state 3")

  bool is_archive = false;
  if (filename.size() >= 4) {
    LOG("load_state 4")
    if (filename.substr(filename.size()-4,4) == ".vsx") {
      LOG("file is .VSX\n")
      filesystem.archive_load(filename.c_str());
      if (filesystem.is_archive_populated()) {
        is_archive = true;
        // state has to lie first in the archive!!!
        LOG("engine loading archive: "+filename)
        i_filename = "_states/_default";//filesystem.archive_files[0].filename;
      } else
      { filesystem.archive_close(); return 0; }
    }
  }
  LOG("engine loading state: "+i_filename);
  load1.load_from_file(i_filename,true);
  LOG("load_state after")
#ifdef VSXU_MAC_XCODE
  syslog(LOG_ERR,"load1.count() = %d\n", load1.count());
#endif

  if (!is_archive)
  filesystem.set_base_path(vsx_get_data_path());
  int res = i_load_state(load1,error_string,filename);
  load1.clear(true);

  return res;
}

vsx_comp* vsx_engine::add(vsx_string label)
{
  if (!valid) return 0x0;
  if (!forge_map[label])
  {
    vsx_comp* comp = new vsx_comp;
    comp->engine_owner = (void*)this;
    comp->name = label;
    forge.push_back(comp);

    // is this a child of a macro?
    vector<vsx_string> c_parts;
    vsx_string deli = ".";
    explode(label, deli, c_parts);
    if (c_parts.size() > 1) {
      // ok, we have a macro
      c_parts.pop_back();
      vsx_string macro_name = implode(c_parts,deli);
      if (vsx_comp* macro_comp = get_component_by_name(macro_name)) {
        comp->parent = macro_comp;
        macro_comp->children.push_back(comp);
      }
    }
    forge_map[label] = comp;
    return comp;
  }
  return 0x0;
}

// send our current time to the client
void vsx_engine::tell_client_time(vsx_command_list *cmd_out)
{
  if (!valid) return;
  if (no_send_client_time) return;
  #ifndef VSX_NO_CLIENT
    bool send = false;

    if (lastsent < 0 || lastsent > 0.05 ) {
      send = true;
      lastsent = 0;
    }
    if (current_state != last_e_state) send = true;

    if (send) {
      cmd_out->add_raw("time_upd " + f2s(engine_info.vtime)+" "+i2s(current_state));
    }
    last_e_state = current_state;
  #endif
}

// set engine speed
void vsx_engine::set_speed(float spd)
{
  if (!valid) return;
  #ifndef VSX_DEMO_MINI
    g_timer_amp = spd;
  #endif
}

// set internal float parameter
void vsx_engine::set_float_array_param(int id, vsx_engine_float_array* float_array)
{
  if (!valid) return;
  engine_info.param_float_arrays[id] = float_array;
}

// set FX level amplification (sound, etc)
void vsx_engine::set_amp(float amp)
{
  if (!valid) return;
  #ifndef VSX_DEMO_MINI
    engine_info.amp = amp;
  #endif
}

// start the engine and sending all the modules the start signal
bool vsx_engine::start()
{
  // a few assertions
  if (0x0 == module_list) return false;

  if (!stopped) return false;
  if (stopped) stopped = false;
  if (first_start)
  {
    valid = true;
    sequence_list.set_engine(this);
    first_start = false;

    log("trying to add screen",0);

    // create a new component for the screen
    vsx_comp* comp = new vsx_comp;
    comp->internal_critical = true;
    comp->engine_owner = (void*)this;
    comp->identifier = "outputs;screen";
    comp->load_module("outputs;screen");
    comp->component_class += ":critical";
    comp->name="screen0";
    comp->engine_info(&engine_info);

    // add this to our forge and forge_map
    forge.push_back(comp);
    forge_map["screen0"] = comp;
    // add to outputs
    outputs.push_back(comp);
    // set validity
  }
  for (std::vector<vsx_comp*>::iterator it = forge.begin(); it != forge.end(); ++it)
  {
    (*it)->start();
  }
  m_timer.start();
  g_timer.start();
  lastsent = 0;
  return true;
}
// stop the engine
bool vsx_engine::stop()
{
  if (!valid) return false;
  #ifndef VSX_DEMO_MINI
    if (!stopped)
    {
      for (unsigned long i = 0; i < forge.size(); ++i) {
        forge[i]->stop();
      }
      stopped = true;
      return true;
    }
    return false;
  #endif
}

void vsx_engine::set_constant_frame_progression(float new_frame_cfp_time)
{
  frame_cfp_time = new_frame_cfp_time;
}

void vsx_engine::play()
{
  if (!valid) return;
  current_state = VSX_ENGINE_PLAYING;
  g_timer.start();
}

//############## R E N D E R #######################################################################
bool vsx_engine::render()
{
  if (!valid) return false;
   // check for time control requests from the modules
  if (engine_info.request_play == 1 && current_state != VSX_ENGINE_LOADING) {
    current_state = VSX_ENGINE_PLAYING;
    engine_info.request_play = 0;
  }

  if (engine_info.request_stop == 1) {
    current_state = VSX_ENGINE_STOPPED;
    engine_info.request_stop = 0;
  }

  if (engine_info.request_rewind == 1) {
    current_state = VSX_ENGINE_REWIND;
    engine_info.request_rewind = 0;
  }

  if (current_state == VSX_ENGINE_STOPPED && engine_info.request_set_time > 0.0f)
  {
    float dd = engine_info.vtime - engine_info.request_set_time;
    if (dd > 0) {
      engine_info.dtime = -dd;
    } else {
      engine_info.dtime = fabs(dd);
    }
  }

  //vsx_command_s cm; cm.process_garbage();
    //printf("d1");
  if (!stopped) {
    frame_timer.start();
    // here we set the global time of the engine.
    // In case we need this module to do something else - capture to file
    // we would need to set dtime to the length of each frame, and add vtime
    // with dtime here to stretch the time correctly.
    /*if (reset_time->get() == 0) {
      //printf("reset_time\n");
      d_time = -v_time;
      //v_time = 0;
      reset_time->set(1);
    } else { */
      //if (time_multiplier->get() < 0.000001) {time_multiplier->set(0.000001);}
    /*}*/
    //engine->dtime = d_time;
    //engine->vtime = v_time;

    //
    //printf("engine rendering new frame\n");


    //d_time = (1000.0f/avi_fps)*0.001f;
    //engine_info.dtime = 0;
    float gtime = (float)g_timer.dtime();
    if (frame_cfp_time != 0.0f) gtime = frame_cfp_time;
    d_time = gtime * g_timer_amp;
    engine_info.real_dtime = d_time;
    engine_info.real_vtime += d_time;
    //
    if (current_state == VSX_ENGINE_LOADING) {
      frame_start_time = g_timer.atime();
    }


    float d_time_i = d_time;
    float dt = 0;
    // this is the fmod time synchronizer

    if (frame_cfp_time == 0.0f)
    {
      for (unsigned long i = 0; i < outputs.size(); i++) {
        vsx_engine_param* param;

        param = outputs[i]->get_params_out()->get_by_name("_st");

            //else {
              //param = dest->get_params_in()->get_by_name(c->parts[2]);
              //printf("size: %d\n",dest->get_params_in()->param_name_list.size());
            //}
        if (param) {
          vsx_module_param_float* fp = (vsx_module_param_float*)param->module_param;
          dt = fp->get();
          if (dt != -1.0f) {
            //printf("getting time: %f\n", dt);
            // we're getting some time from the module
            if (current_state == VSX_ENGINE_PLAYING) {
              if (last_m_time_synch == 0) {
                g_timer.start();
  //              d_time_i = dt-0.06;//dt - frame_prev_time;
                if (engine_info.vtime == 0)
                d_time_i = dt;//dt-0.06;//dt - frame_prev_time;
                //printf("f___dt: %f\n");
                //engine_info.vtime = dt;//frame_prev_time = dt;
                last_m_time_synch = 1;
              } else {
                d_time_i = d_time;
                //d_time_i = dt - frame_prev_time;
                //engine_info.vtime = dt;//
                //if (d_time_i == 0)
                //frame_prev_time = dt;
                //d_time_i = d_time;
              }
            } else {
              d_time_i = 0;
            }
            //engine_info.vtime = dt;//frame_prev_time = dt;
            //engine_info.dtime = d_time_i;
            //d_time_i = dt;
          } else {
            d_time_i = 0;
          }
        }
              //printf("aaaaa %s\n",param->get_name().c_str());
              //vsx_string value = param->get_string();
      }
    }
    //if (e_state == VSX_ENGINE_PLAYING) {
      //printf("dt: %f\n",dt);
      //printf("dt: %f\n",engine_info.vtime);
      //printf("time diff: %f\n",dt-(engine_info.vtime));
      //printf("engine.dtime: %f\n",engine_info.dtime);
    //} //else printf("------STOPPED\n");


#ifndef VSX_NO_CLIENT
    if (current_state == VSX_ENGINE_REWIND) {
      engine_info.dtime = -engine_info.vtime;
      engine_info.vtime = 0;
      g_timer.start();
      current_state = VSX_ENGINE_STOPPED;
    } else
#endif
    if (current_state == VSX_ENGINE_PLAYING) {
      engine_info.dtime = d_time_i;// * time_multiplier->get();
      engine_info.vtime += engine_info.dtime;
    } else {
      engine_info.vtime += engine_info.dtime;
    }

    if (current_state == VSX_ENGINE_STOPPED) last_m_time_synch = 0;
    //if (avi_play && e_state == VSX_ENGINE_STOPPED) {
      //printf("closing AVI\n");
      //CloseAvi(avi);
      //printf("done closing AVI\n");
      //avi_play = false;
    //}
    engine_info.state = current_state;
    //printf("engine state: %d\n",e_state);

    lastsent += engine_info.dtime;

    //printf("ed %f ev %f | ",engine_info.dtime, engine_info.vtime);


    if (frame_dcount++ > frame_d) {frame_dcount = 0; frame_dtime = 0; frame_dprev = engine_info.vtime; }
    else {
      frame_dtime+=(engine_info.vtime-frame_dprev);
      frame_dprev = engine_info.vtime;
      frame_dfps = 1/(frame_dtime/frame_dcount);
    }
    sequence_list.run(engine_info.dtime);
    //if (seq_pool
#ifndef VSX_NO_CLIENT
    interpolation_list.run(m_timer.dtime());
#endif

    sequence_pool.run(engine_info.dtime);

    //engine_info.dtime = dtime;
    //engine_info.vtime = vtime;
    //++frame_tcount;
    //printf("d2");

    // go through the outputs - actual rendering
    for (unsigned long i = 0; i < outputs.size(); i++) {
      outputs[i]->prepare();
    }
    for(std::vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it) {
      (*it)->reset_frame_status();
    }

    // reset every component
    if (current_state == VSX_ENGINE_LOADING)
    {
      modules_left_to_load = 0;
      modules_loaded = 0;
      for(vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it)
      {
        if ((*it)->component_class != "macro")
        if ((*it)->module)
        {
          if (!(*it)->module->loading_done)
          {
            vsx_module_info foom;
            (*it)->module->module_info(&foom);
            //#ifdef VSXU_ARTISTE
            LOG3(vsx_string("waiting for module: ")+foom.identifier+" with name: "+(*it)->name);

            //#endif
            ++modules_left_to_load;
          } else ++modules_loaded;
        }
      }	//else printf("component without module\n");

      //printf("i_count: %d   %d\n",commands_internal.count(),modules_left_to_load);
      if (modules_left_to_load == 0 && commands_internal.count() == 0) {
        current_state = VSX_ENGINE_PLAYING;
      }
    }

    //printf("MODULES LEFT TO LOAD: %d\n",i);
    engine_info.dtime = 0;
    last_frame_time = (float)frame_timer.dtime();

    // reset input events counter
    reset_input_events();
    return true;
  }
  // reset input events counter
  reset_input_events();
  return false;
}

void vsx_engine::redeclare_in_params(vsx_comp* comp, vsx_command_list *cmd_out) {
  // 1. get all connections in a list
  //printf("+++redeclaring\n");
  list<vsx_engine_param_connection_info*> abs_connections_in;
  vsx_engine_param_list* in = comp->get_params_in();
  in->get_abs_connections(&abs_connections_in);

  // dump out the sequences for those params that have such
  std::map<vsx_string, vsx_string> sequences;
  std::map<vsx_string, vsx_string> values;
  for (unsigned long i = 0; i < in->param_id_list.size(); ++i) {
    if (in->param_id_list[i]->sequence) {
      sequences[in->param_id_list[i]->name] = sequence_list.dump_param(in->param_id_list[i]);
      sequence_list.remove_param_sequence(in->param_id_list[i]);
    } else
    {
      values[in->param_id_list[i]->name] = in->param_id_list[i]->get_string();
    }
  }

  comp->re_init_in_params();
  cmd_out->add_raw("in_param_spec "+comp->name+" "+comp->in_param_spec+" c");
  comp->module->redeclare_in = false;
  in = comp->get_params_in();

  // repopulate the sequences / values
  for (unsigned long i = 0; i < in->param_id_list.size(); ++i) {
    if (sequences.find(in->param_id_list[i]->name) != sequences.end()) {
      sequence_list.inject_param(in->param_id_list[i], comp, sequences[in->param_id_list[i]->name]);
    } else
    {
      if (values.find(in->param_id_list[i]->name) != values.end()) {
        in->param_id_list[i]->set_compound_string(values[in->param_id_list[i]->name]);
      }
    }
  }

  for (list<vsx_engine_param_connection_info*>::iterator it2 = abs_connections_in.begin(); it2 != abs_connections_in.end(); ++it2) {
    //printf("trying to connect %s\n",(*it2)->dest_name.c_str());
    vsx_engine_param* dparam = in->get_by_name((*it2)->dest_name);

    if (dparam) {
      int order = dparam->connect((*it2)->src);
      //printf("internal order: %d\n",order);
#ifndef VSX_DEMO_MINI
      cmd_out->add_raw("param_connect_volatile "+comp->name+" "+dparam->name+" "+(*it2)->src->owner->component->name+" "+(*it2)->src->name+" "+i2s(order));
#endif
    }
  }
}

void vsx_engine::redeclare_out_params(vsx_comp* comp, vsx_command_list *cmd_out) {
  // 1. get all connections in a list
  #ifdef VSXU_DEBUG
  printf("+++redeclaring out for component named: %s\n",comp->name.c_str());
  #endif
  list<vsx_engine_param_connection_info*> abs_connections_out;
  vsx_engine_param_list* out = comp->get_params_out();
  out->get_abs_connections(&abs_connections_out);

  // will nuke all the internal params.
  comp->re_init_out_params();
#ifndef VSX_DEMO_MINI
  cmd_out->add_raw("out_param_spec "+comp->name+" "+comp->out_param_spec+" c");
#endif
  #ifdef VSXU_DEBUG
    //printf("outparamspec: %s\n",("out_param_spec "+comp->name+" "+comp->out_param_spec+" c").c_str());
  #endif
  comp->module->redeclare_out = false;
  out = comp->get_params_out();

  for (list<vsx_engine_param_connection_info*>::iterator it2 = abs_connections_out.begin(); it2 != abs_connections_out.end(); ++it2) {
    #ifdef VSXU_DEBUG
      //printf("(*it2)->dest_name: %s\n",(*it2)->dest_name.c_str());
      //printf("(*it2)->src_name:  %s    %s\n",(*it2)->src_name.c_str(),(*it2)->dest_name.c_str());
    #endif
    vsx_engine_param* dparam = out->get_by_name((*it2)->src_name);
    if (dparam) {
      int order = (*it2)->dest->connect(dparam);
//      printf("internal command: %d\n",order);
#ifndef VSX_DEMO_MINI
      vsx_string dest_comp_name = (*it2)->dest->owner->component->name;
      vsx_string srcn = (*it2)->src_name;
      vsx_string cn = comp->name;
      vsx_string dpn = dparam->name;
      vsx_string os = i2s(order);
      cmd_out->add_raw("param_connect_volatile "+dest_comp_name+" "+srcn+" "+cn+" "+dpn+" "+os);
#endif
    }
  }
}

void vsx_engine::process_message_queue_redeclare(vsx_command_list *cmd_out_res)
{
  for (vector<vsx_comp*>::iterator it = forge.begin(); it < forge.end(); ++it) {
    if ((*it)->module) {
      if ((*it)->module->redeclare_in) {
        redeclare_in_params(*it,cmd_out_res);
      }
      if ((*it)->module->redeclare_out) {
        redeclare_out_params(*it,cmd_out_res);
      }
      if ((*it)->module->message.size()) {
        cmd_out_res->add_raw("c_msg "+(*it)->name+" "+base64_encode((*it)->module->message));
        (*it)->module->message = "";
      }
    }
  }
}

void vsx_engine::set_ignore_per_frame_time_limit(bool new_value)
{

}

//############## M E S S A G E   P R O C E S S O R #################################################
void vsx_engine::process_message_queue(vsx_command_list *cmd_in, vsx_command_list *cmd_out_res, bool exclusive, bool ignore_timing)
{
  if (!valid) return;
  // service commands
  LOG("process_message_queue 1")

  commands_res_internal.clear(true);
  tell_client_time(cmd_out_res);
  vsx_command_s *c = 0;
  if (!exclusive) {
    while ( (c = commands_out_cache.pop()) ) cmd_out_res->add(c);
  }
  // check for module requests

  if (current_state == VSX_ENGINE_LOADING)
  {
    process_message_queue_redeclare(cmd_out_res);
  }

  // handle exclusivity
  if (exclusive) cmd_in->set_type(1);
  // add the incoming commands to our own list for buffering (to continue on next frame if we don't have time to do them all)
  while ( (c = cmd_in->pop()) ) {
    commands_internal.add(c);
  }
  //commands_internal.reset();

  //---------------------------------------
  double total_time = 0.0;

  #define FAIL(header, message) 	cmd_out->add_raw(vsx_string("alert_fail ")+base64_encode(#header)+" Error "+base64_encode(#message))

  vsx_command_timer.start();
  //bool run = true;
  vsx_command_list* cmd_out = cmd_out_res;
  while (total_time < 0.01)
  //while (1)
  {
    c = commands_internal.pop();
    if (!c) break;
    //LOG3(vsx_string("cmd_in: ")+c->cmd+" ::: "+c->raw);
    //printf("%s\n", vsx_string(vsx_string("cmd_in: ")+c->cmd+" ::: "+c->raw).c_str());
    //printf("c type %d\n",c->type);
    if (c->type == 1)
      cmd_out = &commands_res_internal;
    //else
//    	cmd_out = cmd_out_res;

    #define cmd c->cmd
    #define cmd_data c->cmd_data

    #include "vsx_engine_messages/vsx_saveload.h"
    #include "vsx_engine_messages/vsx_em_comp.h"
    #include "vsx_engine_messages/vsx_connections.h"
    #include "vsx_engine_messages/vsx_parameters.h"
    #include "vsx_engine_messages/vsx_sequencer.h"
    #include "vsx_engine_messages/vsx_em_macro.h"
    #include "vsx_engine_messages/vsx_seq_pool.h"
    #include "vsx_engine_messages/vsx_engine_time.h"
    #include "vsx_engine_messages/vsx_em_script.h"
    #ifndef VSX_NO_CLIENT
      #include "vsx_engine_messages/vsx_note.h"
    #endif
    #include "vsx_engine_messages/vsx_em_system.h"

    #undef cmd
    #undef cmd_data

    if (current_state != VSX_ENGINE_LOADING)
    {
      process_message_queue_redeclare(cmd_out_res);
    }


    total_time+=vsx_command_timer.dtime();
    // internal garbage collection
    (*(c->garbage_pointer)).remove(c);
    delete c;
  }

} // process_comand_queue

void vsx_engine::send_state_to_client(vsx_command_list *cmd_out) {
#ifndef VSX_DEMO_MINI
  #ifndef SAVE_PRODUCTION
  if (filesystem.type != VSXF_TYPE_FILESYSTEM)  {
    cmd_out->add_raw("server_message "+base64_encode("Packages not possible to edit, ask author for state!"));
    return;
  }
  #endif
  vsx_command_list temp_conn;
  vsx_command_list temp_conn_alias;
  for (unsigned long i = 0; i < forge.size(); ++i) {
    vsx_string xs,ys;
    xs = f2s(forge[i]->position.x);
    ys = f2s(forge[i]->position.y);
    vsx_string command = "component_create_ok "+forge[i]->name+" "+forge[i]->component_class+" "+xs+" "+ys+" ";

    if (forge[i]->component_class == "macro")
      command += f2s(forge[i]->size);
      else
      command += forge[i]->module_info->identifier;
    if (forge[i]->module_info->output) command += " out";
    if (forge[i]->component_class != "macro") {
      forge[i]->get_params_in()->dump_aliases_and_connections_rc(&temp_conn);
      forge[i]->get_params_out()->dump_aliases_rc(&temp_conn_alias);
    }
    cmd_out->add_raw(command);
    cmd_out->add_raw("in_param_spec "+forge[i]->name+" "+forge[i]->in_param_spec);
    cmd_out->add_raw("out_param_spec "+forge[i]->name+" "+forge[i]->out_param_spec);
    //send vsxl presence
    for (unsigned long ii = 0; ii < forge[i]->in_parameters->count(); ++ii) {
      vsx_engine_param* param = forge[i]->get_params_in()->get_by_id(ii);
      if (param->module_param->vsxl_modifier) {
        cmd_out->add_raw("vsxl_pfi_ok "+forge[i]->name+" "+param->name);
      }
    }
    if (forge[i]->vsxl_modifier) {
      cmd_out->add_raw("vsxl_cfi_ok "+forge[i]->name);
    }
  }
  // conns
  vsx_command_s* outc;
  temp_conn_alias.reset();
  while ( (outc = temp_conn_alias.get()) ) {
    cmd_out->addc(outc);
  }
  temp_conn.reset();
  while ( (outc = temp_conn.get()) ) {
    cmd_out->addc(outc);
  }
  //printf("************ GETTING SEQUENCES ************");
  sequence_list.get_sequences(cmd_out);
  //printf("state sent to client end++\n");
  //cmd_out->add_raw("server_message "+base64_encode(""));
  // notes
  for (note_iter = note_map.begin(); note_iter != note_map.end(); note_iter++)
  cmd_out->add_raw(vsx_string((*note_iter).second.serialize()));
#endif
}

float vsx_engine::get_last_frame_time()
{
  return last_frame_time;
}

double vsx_engine::get_fps() {
  #ifndef VSX_DEMO_MINI
  return frame_dfps;
  #endif
}

void vsx_engine::i_clear(vsx_command_list *cmd_out,bool clear_critical) {
#ifndef VSX_DEMO_MINI
  //if (filesystem.type == VSXF_TYPE_ARCHIVE) filesystem.archive_close();

  std::map<vsx_string,vsx_comp*> forge_map_save;
  std::vector<vsx_comp*> forge_save;
  for (std::map<vsx_string,vsx_comp*>::iterator fit = forge_map.begin(); fit != forge_map.end(); ++fit) {
    if (!(*fit).second->internal_critical || clear_critical)
    {
      LOG("component deleting: "+(*fit).second->name);


      std::map<vsx_module_param_abs*, std::list<vsx_channel*> >::iterator out_map_channels_iter;
      std::map<vsx_module_param_abs*, std::list<vsx_channel*> > temp_map = (*fit).second->out_map_channels;
      for (out_map_channels_iter = temp_map.begin(); out_map_channels_iter != temp_map.end(); ++out_map_channels_iter) {
        std::list<vsx_channel*>::iterator it;
        for (it = (*out_map_channels_iter).second.begin(); it != (*out_map_channels_iter).second.end(); ++it) {
          (*it)->component->disconnect((*it)->my_param->name,(*fit).second,(*out_map_channels_iter).first->name);
        }
      }
    LOG("delete step 2")
      // ! 3:: remove aliases of other components that have aliased our params and connections (this does this)
      for (std::vector<vsx_engine_param*>::iterator it = (*fit).second->get_params_in()->param_id_list.begin(); it != (*fit).second->get_params_in()->param_id_list.end(); ++it) {

        if ((*it)->sequence) {
          sequence_list.remove_param_sequence((*it));
          //cmd_out->add_raw("pseq_p_ok remove "+(*fit).second->name+" "+(*it)->name);
        }
#ifndef VSX_NO_CLIENT
        interpolation_list.remove(*it);
#endif
      }
      // remove aliases AND connections
    LOG("delete step 2b");
      (*fit).second->get_params_in()->unalias_aliased();
      (*fit).second->get_params_out()->unalias_aliased();
    LOG("delete step 3");
      if ((*fit).second->module_info->output)
      outputs.remove((*fit).second);
    LOG("delete step 4\n");
      LOG("del "+(*fit).second->name);
      if ((*fit).second->component_class != "macro")
      if ( module_list->find((*fit).second->identifier) )
      {
        LOG("unloading "+(*fit).second->name);
        (*fit).second->unload_module();
      }
      delete ((*fit).second);
      LOG("done deleting"´)
    }
    else
    {
      (*fit).second->position.x = 0;
      (*fit).second->position.y = 0;
      forge_save.push_back((*fit).second);
      forge_map_save[(*fit).first] = (*fit).second;
    }
  }
  LOG("clearing forge");
  forge.clear();
  LOG("clearing forge map");
  forge_map.clear();
  note_map.clear();
  forge = forge_save;
  forge_map = forge_map_save;

  sequence_pool.clear();
  sequence_list.clear_master_sequences();

  //printf("forge save size: %d\n",forge.size());
  last_m_time_synch = 0;
  engine_info.vtime = 0;
  engine_info.dtime = 0;
  engine_info.real_vtime = 0;
  current_state = VSX_ENGINE_STOPPED;
  if (filesystem.is_archive())
  {
    if (cmd_out)
    {
      //printf("clear not filesystem\n");
      filesystem.archive_close();
      send_state_to_client(cmd_out);
    }
  }
#endif
}

void vsx_engine::unload_state() {
  i_clear();
}

int vsx_engine::get_state_as_commandlist(vsx_command_list &savelist)
{
  #ifndef VSX_NO_CLIENT
  vsx_command_list tmp_comp;
  vsx_command_list tmp_param_set;
  vsx_command_list tmp_connections;
  vsx_command_list tmp_aliases;
  if (meta_information.size()) tmp_comp.add_raw("meta_set "+base64_encode(meta_information));
  for (forge_map_iter = forge_map.begin(); forge_map_iter != forge_map.end(); ++forge_map_iter) {
    vsx_comp* comp = (*forge_map_iter).second;
    if (((*forge_map_iter).second->component_class == "macro"))
    tmp_comp.add_raw(vsx_string("macro_create ")+(*forge_map_iter).first+" "+f2s(comp->position.x)+" "+f2s(comp->position.y)+" "+f2s((*forge_map_iter).second->size));
    else
    {
      if ((*forge_map_iter).first != "screen0")
      tmp_comp.add_raw("component_create "+comp->identifier+" "+comp->name+" "+f2s(comp->position.x)+" "+f2s(comp->position.y));
      else
      tmp_comp.add_raw("cpp screen0 "+f2s(((*forge_map_iter).second)->position.x)+" "+f2s(((*forge_map_iter).second)->position.y));
      comp->get_params_in()->dump_aliases_and_connections("", &tmp_connections);
      comp->get_params_out()->dump_aliases("", &tmp_aliases);
    }
    for (unsigned long i = 0; i < comp->get_params_in()->param_id_list.size(); ++i)
    {
      bool run = true;
      if (comp->get_params_in()->param_id_list[i]->channel)
      if ((comp->get_params_in()->param_id_list[i]->channel->connections.size()))
      run = false;
      vsx_engine_param* param = comp->get_params_in()->param_id_list[i];
      if (run)
      {
        // check for sequence
        vsx_string ss = sequence_list.dump_param(param);
        if (ss != "")
        {
          // sequence is controlling, we need no init value.
          tmp_comp.add_raw(vsx_string("pseq_p inject ")+comp->name+" "+param->name+" "+ss);
        } else
        {
          // or dump the value
          //printf("component name: %sparam name:\n",comp->name.c_str(),param->module_param->name.c_str());
          //printf("name:%s\nval: %s\ndef: %s\n",param->module_param->name.c_str(),param->module_param->get_string().c_str(),param->module_param->get_default_string().c_str());
          vsx_string pval = param->get_string();
          //printf("val: %s  default: %s\n",pval.c_str(),param->get_default_string().c_str());
          if (!param->alias) {
            if (
              pval !=
              param->get_default_string()
            )
            {
              if (
                  param->module_param->type == VSX_MODULE_PARAM_ID_STRING
                  ||
                  param->module_param->type == VSX_MODULE_PARAM_ID_RESOURCE
                )
              {
                tmp_comp.add_raw(vsx_string("ps64 ")+comp->name+" "+param->name+" "+base64_encode(pval));
              } else
              {
                tmp_comp.add_raw(vsx_string("param_set ")+comp->name+" "+param->name+" "+pval);
              }
            }
          }
        }
        // dump the param vsxl filter
      }
      //printf("running pre vsxlmf %s\n", param->name.c_str());
      #ifndef VSXE_NO_GM
      if (param->module_param->vsxl_modifier) {
        //printf("vsxl modifier present\n");
        vsx_param_vsxl_driver_abs* driver;
        driver = (vsx_param_vsxl_driver_abs*)((vsx_param_vsxl*)param->module_param->vsxl_modifier)->get_driver();
        tmp_comp.add_raw(vsx_string("vsxl_pfi ")+comp->name+" "+param->name+" "+i2s(driver->id)+" "+base64_encode(driver->script));
      }
      #endif
    }
    #ifndef VSXE_NO_GM
    if (comp->vsxl_modifier) {
      vsx_comp_vsxl_driver_abs* driver;
      driver = (vsx_comp_vsxl_driver_abs*)((vsx_comp_vsxl*)comp->vsxl_modifier)->get_driver();
      tmp_comp.add_raw(vsx_string("vsxl_cfi ")+comp->name+" "+base64_encode(driver->script));
    }
    #endif
  }
  for (note_iter = note_map.begin(); note_iter != note_map.end(); note_iter++)
  {
    tmp_connections.add_raw((*note_iter).second.serialize_state());
  }

  vsx_command_s* outc;
  tmp_comp.reset();
  while ( (outc = tmp_comp.get()) ) {
    savelist.addc(outc);
  }
  tmp_aliases.reset();
  while ( (outc = tmp_aliases.pop_back()) ) {
    savelist.addc(outc);
  }
  tmp_connections.reset();
  while ( (outc = tmp_connections.pop_back()) ) {
    savelist.addc(outc);
  }
  // dump the sequence pool
  sequence_pool.dump_to_command_list(savelist);
  // dump the master sequences with their connections to the sequence pools
  sequence_list.dump_master_channels_to_command_list(savelist);
  #endif
  return 0;
}

// [base].[name]
// empty.comp1
// comp1

int vsx_engine::rename_component(vsx_string old_identifier, vsx_string new_base, vsx_string new_name)
{
#ifndef VSX_NO_CLIENT
  //printf("new base: %s\n",new_base.c_str());
  // first we need to split up the name so we have the old base and the old name
  vsx_string old_base;
  vsx_string old_name;
  std::vector<vsx_string> parts;
  vsx_string deli = ".";
  explode(old_identifier,deli,parts);
  old_name = parts[parts.size()-1];
  parts.pop_back();
  if (parts.size())
  old_base = implode(parts,deli);
  else old_base = "";
  //printf("old_name: %s\nold_base: %s\nnew base: %s\nnew name: %s\n",old_name.c_str(),old_base.c_str(),new_base.c_str(),new_name.c_str());
  // we have the basic names set up now find the component
  vsx_comp* t = get_component_by_name(old_identifier);
  if (!t) return 0;
  // if we don't want to either move or rename
  if (new_base == "$") new_base = old_base;
  if (new_name == "$") new_name = old_name;

  // if we don't move anything, no reason to change parent
  bool assign_first = (new_base != old_base);
  vsx_comp* dest = 0;
  if (assign_first) dest = get_component_by_name(new_base);

  int max_loop = 0;
  if (t->component_class == "macro") max_loop = 0; else max_loop = 1;

  std::list<vsx_string> macro_comps;
  std::list<vsx_comp*> macro_comp_p;
  std::map<vsx_string,vsx_comp*>::iterator m_i = forge_map.find(old_identifier);
  bool first = true;
  bool drun = true;
  // loop and find all components we need to rename
  int runs = 0;
  while (drun) {
    if (m_i != forge_map.end()) {
      vsx_string tt = (*m_i).first;
      if (tt.find(old_identifier) == 0 || first) {
        if (first || tt[old_identifier.size()] == vsx_string(".")) {
          first = false;
          macro_comps.push_back(tt);
          macro_comp_p.push_back((*m_i).second);
        }
      } else drun = false;
      ++m_i;
    } else drun = false;
    ++runs;
    if (max_loop != 0)
    if (runs >= max_loop) drun = false;
  }

  /*vsx_string parent_name;
  if (t->parent) {
    parent_name = t->parent->name;
  } else parent_name = "";*/
  vsx_string new_name_ = "";
  // do the actual renaming
  std::list<vsx_comp*>::iterator it_c = macro_comp_p.begin();

  for (std::list<vsx_string>::iterator it2 = macro_comps.begin(); it2 != macro_comps.end(); ++it2)
  {
    forge_map.erase(*it2);
    if (new_base.size()) {
      if (old_base.size()) {
        //printf("p1\n");
        // moving from macro to macro
        new_name_ = new_base+"."+str_replace(old_name,new_name,str_replace(old_base+".","",*it2,1,0),1,0);
      }
      else {
        //printf("p2 %s  %s   %s\n",old_name.c_str(),new_name.c_str(),(*it2).c_str());
        // moving from root to macro
        new_name_ = new_base+"."+str_replace(old_name,new_name,*it2,1,0);
      }
    }
    else {
      // moving the component to the root (server)
      // from macro to root
      if (old_base.size()) {
        // "old_base.component_name" -> "new_component_name"
        //printf("p3 %s \n",str_replace(old_base+".","",*it2,1,0).c_str());
        new_name_ = str_replace(old_name,new_name,str_replace(old_base+".","",*it2,1,0),1,0);
        new_name = new_name;
      } else {
        // plain renaming
        //printf("p4\n");
        new_name_ = str_replace(old_name,new_name,*it2,1,0);
      }
    }
    //printf("new name is: %s\n",new_name_.c_str());
    forge_map[new_name_] = *it_c;
    (*it_c)->name = new_name_;
    ++it_c;
  }

  // actually move the component
  if (assign_first) {
    //printf("moving component\n");
    if (t->parent) {
      ((vsx_comp*)t->parent)->children.remove(t);
    }
    if (dest)
    t->parent = dest;
    else
    t->parent = 0;
  }
  return 1;
  #else
  return 0;
#endif
}


extern "C" vsx_engine* create_engine()
{
  return new vsx_engine;
}

// VSX_ENGINE
