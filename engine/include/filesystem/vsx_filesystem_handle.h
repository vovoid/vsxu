#pragma once

namespace vsx_filesystem
{
  class file_handle
  {
  public:
    vsx_string<> filename;
    size_t position = 0;   // position in the data stream
    size_t size = 0;       // size of the data stream in bytes

    // TODO
    enum mode
    {
      mode_none = 0,
      mode_read = 1,
      mode_write = 2
    };


    int current_mode = 0;        // 0 = undefined,  1 = read, 2 = write
    void* file_data = 0; // in the case of VSXF_TYPE_ARCHIVE this is the actual decompressed file in RAM
                     // don't mess with this! the file class will handle it..
    bool file_data_volatile = false;
    FILE* handle = 0x0;

    ~file_handle()
    {
      req(file_data);

      if (current_mode == mode_write)
      {
        delete (vsx_nw_vector<char>*)file_data;
        return;
      }


      if (file_data_volatile)
        return;

      free(file_data);
    }
  };

}

