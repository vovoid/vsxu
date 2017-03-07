#pragma once

#include <string/vsx_string_helper.h>
#include <string>
#include <string/vsx_json.h>

namespace vsx
{
namespace json_helper
{

inline vsx::json json_from_file(vsx_string<> filename, vsx::filesystem* filesystem)
{
  std::string json_error;
  vsx_string<> buf = vsx_string_helper::read_from_file(filename, filesystem);
  return vsx::json::parse(buf.c_str(), json_error);
}

}
}
