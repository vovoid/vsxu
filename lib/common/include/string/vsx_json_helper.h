#pragma once

#include <string/vsx_string_helper.h>
#include <string>
#include <string/vsx_json.h>

namespace vsx
{
namespace json_helper
{

inline vsx::json load_json_from_file(vsx_string<> filename, vsx::filesystem* filesystem)
{
  std::string json_error;
  vsx_string<> buf = vsx_string_helper::read_from_file(filename, filesystem);
  vsx::json result = vsx::json::parse(buf.c_str(), json_error);
  return result;
}

inline void save_json_to_file(vsx_string<> filename, vsx::json& json)
{
  std::string buf;
  json.dump(buf);
  vsx_string_helper::write_to_file(filename, vsx_string<>(buf.c_str()) );
}

}
}
