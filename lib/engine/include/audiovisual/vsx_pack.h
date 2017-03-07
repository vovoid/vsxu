#pragma once

#include <filesystem/vsx_filesystem.h>
#include <string/vsx_string_helper.h>
#include <string/vsx_json_helper.h>
#include <audiovisual/vsx_state_info.h>

namespace vsx
{
namespace engine
{
namespace audiovisual
{

class pack
{
  vsx_ma_vector< state_info* > states;
  vsx::filesystem* fs = 0x0;

public:

  void load(vsx_string<> filename)
  {
    fs = new vsx::filesystem();
    fs->get_archive()->load( filename.c_str(), true );
    vsx::json json = vsx::json_helper::json_from_file("manifest.json", fs);
    foreach(json["visuals"].array_items(), i)
      states.push_back( new state_info( json["visuals"].array_items()[i].object_value(), fs ) );



  }
};
}
}
}
