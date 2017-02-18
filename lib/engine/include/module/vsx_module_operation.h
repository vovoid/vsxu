#pragma once

#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>
#include <debug/vsx_error.h>

class vsx_module_operation
{
public:
  // internal name for the operation
  vsx_string<> handle;

  // pretty name of the operation for display in GUI - "dump to disk" or "reset"
  vsx_string<> name;

  // parameters
  int param_1_required;
  vsx_string<> param_1;
  vsx_string<> param_1_name;

  int param_2_required;
  vsx_string<> param_2;
  vsx_string<> param_2_name;

  int param_3_required;
  vsx_string<> param_3;
  vsx_string<> param_3_name;

  int param_4_required;
  vsx_string<> param_4;
  vsx_string<> param_4_name;

  vsx_module_operation()
    :
      param_1_required(false),
      param_2_required(false),
      param_3_required(false),
      param_4_required(false)
  {}

  vsx_string<> serialize()
  {
    vsx_string<> ret;
    ret = vsx_string_helper::base64_encode(handle) + "|";
    ret += vsx_string_helper::base64_encode(name) + "|";

    ret += vsx_string_helper::i2s(param_1_required) + "|";
    ret += vsx_string_helper::base64_encode(param_1) + "|";
    ret += vsx_string_helper::base64_encode(param_1_name) + "|";

    ret += vsx_string_helper::i2s(param_2_required) + "|";
    ret += vsx_string_helper::base64_encode(param_2) + "|";
    ret += vsx_string_helper::base64_encode(param_2_name) + "|";

    ret += vsx_string_helper::i2s(param_3_required) + "|";
    ret += vsx_string_helper::base64_encode(param_3) + "|";
    ret += vsx_string_helper::base64_encode(param_3_name) + "|";

    ret += vsx_string_helper::i2s(param_4_required) + "|";
    ret += vsx_string_helper::base64_encode(param_4) + "|";
    ret += vsx_string_helper::base64_encode(param_4_name);

    return ret;
  }

  void unserialize(vsx_string<>& in)
  {
    vsx_nw_vector< vsx_string<> > parts;
    vsx_string_helper::explode_single(in,'|', parts);
    if (parts.size() != 14)
      VSX_ERROR_RETURN("Wrong part count");

    handle = vsx_string_helper::base64_decode(parts[0]);
    name = vsx_string_helper::base64_decode(parts[1]);

    param_1_required = vsx_string_helper::s2i( parts[2]);
    param_1 = vsx_string_helper::base64_decode(parts[3]);
    param_1_name = vsx_string_helper::base64_decode(parts[4]);

    param_2_required = vsx_string_helper::s2i( parts[5]);
    param_2 = vsx_string_helper::base64_decode(parts[6]);
    param_2_name = vsx_string_helper::base64_decode(parts[7]);

    param_3_required = vsx_string_helper::s2i( parts[8]);
    param_3 = vsx_string_helper::base64_decode(parts[9]);
    param_3_name = vsx_string_helper::base64_decode(parts[10]);

    param_4_required = vsx_string_helper::s2i( parts[11]);
    param_4 = vsx_string_helper::base64_decode(parts[12]);
    param_4_name = vsx_string_helper::base64_decode(parts[13]);
  }
};

