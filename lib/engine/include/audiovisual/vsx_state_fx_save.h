#pragma once

#include <vector>
#include <audiovisual/vsx_state.h>
#include <string/vsx_json_helper.h>

namespace vsx
{
namespace engine
{
namespace audiovisual
{

void fx_save(std::vector<state*>& states, vsx_string<> filename)
{
  vsx::json::array fx_array;
  foreach (states, i)
    fx_array.push_back(
      vsx::json::object{
        {"filename" , states[i]->filename.c_str() },
        {"fx_level", states[i]->fx_level}
      }
    );

  vsx::json result = vsx::json::object{
    {"visuals" , fx_array }
  };

  vsx::json_helper::save_json_to_file(
    filename,
    result
  );
}

void fx_load(std::vector<state*>& states, vsx_string<> filename)
{
  vsx::json json = vsx::json_helper::load_json_from_file(filename, vsx::filesystem::get_instance());
  vsx::json::array array = json["visuals"].array_items();
  foreach (array, i)
  {
    json::object obj = array[i].object_items();
    vsx_string<> filename = obj["filename"].string_value().c_str();
    foreach (states, si)
      if (states[si]->filename == filename)
      {
        states[si]->fx_level = (float)obj["fx_level"].number_value();
        break;
      }
  }
}

}
}
}
