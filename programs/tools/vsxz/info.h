#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_info.h>
#include <vsx_argvector.h>

void info()
{
  req(vsx_argvector::get_instance()->has_param_with_value("info"));

  vsx::filesystem_archive_vsxz_info archive_info;
  vsx_nw_vector< vsx_string<> > result;
  archive_info.get_info( vsx_argvector::get_instance()->get_param_value("info").c_str(), result );
  foreach (result, i)
    vsx_printf(L"%hs", result[i].c_str());

  exit(0);
}
