#pragma once
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>

namespace vsx
{

class filesystem_archive_vsxz_info
{

public:

  void get_info(const char* archive_filename, vsx_nw_vector< vsx_string<> >& result);

};

}
