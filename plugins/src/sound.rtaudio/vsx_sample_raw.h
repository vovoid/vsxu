#ifndef VSX_SAMPLE_RAW_H
#define VSX_SAMPLE_RAW_H

#include "vsx_sample.h"

class vsx_sample_raw : public vsx_sample
{
  vsxf* filesystem;

public:

  vsx_sample_raw()
  :
  filesystem(0x0)
  {
  }

  void set_filesystem(vsxf* n)
  {
    filesystem = n;
  }

  void load_filename(vsx_string filename)
  {
    if (!filesystem)
      return;

    vsxf_handle *fp;
    if
    (
      (fp = filesystem->f_open(filename.c_str(), "r"))
      ==
      NULL
    )
      return;

    size_t file_size = filesystem->f_get_size(fp);
    data.reset_used();
    data.allocate_bytes( file_size );
    filesystem->f_read( (void*)data.get_pointer(), file_size, fp);
    filesystem->f_close(fp);
  }

};


#endif // VSX_SAMPLE_RAW_H
