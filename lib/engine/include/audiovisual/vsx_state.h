#pragma once

#include <vsx_engine.h>
#include <string/vsx_json.h>
#include <vsx_module_list_manager.h>

namespace vsx
{
namespace engine
{
namespace audiovisual
{

class state {

  vsx_command_list cmd_in;
  vsx_command_list cmd_out;

public:
  float fx_level = 1.0f;
  float speed = 1.0f;

  vsx::filesystem* filesystem = 0x0;

  vsx_string<> filename;
  vsx_string<> name;
  vsx_string<> author;

  vsx_engine* engine = 0x0;

  bool need_stop = false;
  bool need_reload = false;

  state(const state &other)
    :
      cmd_in(false),
      cmd_out(false)
  {
    fx_level = other.fx_level;
    speed = other.speed;
    engine = other.engine;
    name = other.name;
    cmd_in = other.cmd_in;
    cmd_out = other.cmd_out;
    need_stop = other.need_stop;
    need_reload = other.need_reload;
  }

  state(vsx::json::object json_object, vsx::filesystem* filesystem)
    :
      cmd_in(false),
      cmd_out(false)
  {
    this->filesystem = filesystem;
    this->filename = json_object["filename"].string_value().c_str();
    this->name = json_object["name"].string_value().c_str();
    this->author = json_object["author"].string_value().c_str();
  }

  state()
    :
      cmd_in(false),
      cmd_out(false)
  {
  }

  int init()
  {
    reqrv(!engine, 1);
    engine = new vsx_engine( vsx_module_list_manager::get()->module_list );
    engine->set_no_send_client_time( true );
    engine->start();
    engine->reset_time();
    vsx_string<> error;
    if (filesystem)
      return engine->load_state_filesystem( filename, &error, filesystem );
    return engine->load_state( filename, &error );
  }

  ~state()
  {
    req(engine);
    engine->stop();
    delete engine;
  }

  void stop()
  {
    req(engine);
    engine->unload_state();
  }

  void start()
  {
    req(engine);
    engine->start();
    vsx_string<> error;
    if (filesystem)
      engine->load_state_filesystem( filename, &error, filesystem );
    engine->load_state( filename, &error );
  }

  bool done_loading()
  {
    reqrv(engine, false);
    return
        engine->get_modules_left_to_load() == 0;
  }

  void adjust_fx_level(float change)
  {
    req(engine);
    fx_level = CLAMP(fx_level + change, 0.1f, 15.0f);
    engine->set_amp(fx_level);
  }

  void adjust_speed(float change)
  {
    speed = CLAMP(speed * change, 0.1f, 15.0f);
    engine->set_speed( speed );
  }

  void set_float_array_param(int id, vsx_module_engine_float_array& data )
  {
    req(engine);
    engine->set_float_array_param(id, &data);
  }

  vsx_string<> system_message_get()
  {
    return engine->system_message_get();
  }

  void render()
  {
    req(engine);
    engine->process_message_queue(&cmd_in, &cmd_out);
    engine->render();
    cmd_out.clear_normal();
  }

  vsx_string<> get_name()
  {
    if (filesystem)
      return name;

    reqrv(engine, "");
    return engine->get_meta_information(0);
  }

  vsx_string<> get_author()
  {
    if (filesystem)
      return author;

    reqrv(engine, "");
    return engine->get_meta_information(1);
  }

};

}
}
}
