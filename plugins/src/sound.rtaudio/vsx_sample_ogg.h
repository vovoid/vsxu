#ifndef VSX_SAMPLE_OGG_H
#define VSX_SAMPLE_OGG_H

#include "vsx_sample.h"
#include "ogg_vorbis.c"

class vsx_sample_ogg : public vsx_sample
{
  vsxf* filesystem;

public:

  vsx_sample_ogg()
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

    // file size
    size_t file_size = filesystem->f_get_size(fp);

    // allocate ram to read the file
    void* temp_storage = malloc(file_size);
    if (!temp_storage)
      return;

    // load muffins
    filesystem->f_read( temp_storage, file_size, fp);

    short *result;

    // decode the ogg stream
    // (unsigned char *mem, int len, int *channels, short **output);
    int num_channels;
    int samples_loaded = stb_vorbis_decode_memory
        (
          (unsigned char*)temp_storage,
          file_size,
          &num_channels,
          &result
        );

    free( temp_storage );

    if (-1 == samples_loaded)
    {
      vsx_printf("error loading ogg file, not a vorbis stream or other error...\n");
      return;
    }

    data.set_volatile();
    data.set_data(result,samples_loaded * num_channels);

    filesystem->f_close(fp);
  }

};


#endif // VSX_SAMPLE_OGG_H
