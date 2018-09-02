#include "vsx_gl_vendor.h"
#include <vsx_gl_global.h>
#include <string/vsx_string.h>

void vsx_gl_vendor::init()
{
  req(!init_run);
  char *str;
  str = (char*)glGetString(GL_VENDOR);
  vsx_string<> vendor_str(str);
  vendor_str.make_lowercase();
  if (vendor_str.find("ati") >= 0)
  {
    amd = true;
    return;
  }

  if (vendor_str.find("nvidia") >= 0)
  {
    nvidia = true;
    return;
  }

  if (vendor_str.find("intel") >= 0)
  {
    intel = true;
    return;
  }
  unknown = true;
}

vsx_string<> vsx_gl_vendor::get_vendor_prefix()
{
  if (amd)
    return "amd";
  if (intel)
    return "intel";
  if (nvidia)
    return "nvidia";
  return "unknown";
}

bool vsx_gl_vendor::is_amd()
{
  if (!init_run)
  {
    vsx_printf(L"ERROR: FATAL: init has not been run!\n");
    exit(1);
  }
  return amd;
}

bool vsx_gl_vendor::is_intel()
{
  if (!init_run)
  {
    vsx_printf(L"ERROR: FATAL: init has not been run!\n");
    exit(1);
  }
  return intel;

}

