#pragma once

#include <filesystem/vsx_filesystem.h>
#include <string/vsx_string_helper.h>
#include <string/vsx_json_helper.h>
#include <audiovisual/vsx_state.h>

namespace vsx
{
namespace engine
{
namespace audiovisual
{

class pack
{
  vsx_ma_vector< state* > states;
  vsx::filesystem* fs = 0x0;

public:

  pack(vsx_string<> filename)
  {
    fs = new vsx::filesystem();
    fs->get_archive()->load( filename.c_str(), true );
    vsx::json json = vsx::json_helper::load_json_from_file("manifest.json", fs);
    vsx_string<> name = json["name"].string_value().c_str();

    vsx::json::array array = json["visuals"].array_items();
    foreach (array, i)
      states.push_back(
        new state(
          array[i].object_items(),
          fs
        )
      );
  }

  void add_states(std::vector<state*>& result)
  {
    foreach (states, i)
      result.push_back(states[i]);
  }

};
}
}
}
