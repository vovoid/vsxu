#pragma once

#include <tools/vsx_singleton.h>
#include <string/vsx_string.h>

class vsx_gl_vendor
    : public vsx::singleton<vsx_gl_vendor>
{
  bool init_run = false;

  // vendors
  bool amd = false;
  bool nvidia = false;
  bool intel = false;
  bool unknown = false;

public:

  void init();
  vsx_string<> get_vendor_prefix();
  bool is_amd();
  bool is_intel();

};
