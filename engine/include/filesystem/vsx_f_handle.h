#ifndef VSX_FILESYSTEM_HANDLE_H
#define VSX_FILESYSTEM_HANDLE_H

namespace vsx_filesystem {

    class handle {
    public:
      vsx_string<> filename;
      size_t position;   // position in the data stream
      size_t size;       // size of the data stream in bytes
      int mode;        // 0 = undefined,  1 = read, 2 = write
      void* file_data; // in the case of VSXF_TYPE_ARCHIVE this is the actual decompressed file in RAM
      // don't mess with this! the file class will handle it..
      bool file_data_volatile;
      FILE* file_handle;

      handle()
        :
          position(0),
          size(0),
          mode(0),
          file_data(0),
          file_data_volatile(false),
          file_handle(0)
      {}

      ~handle()
      {
        if (!file_data)
          return;

        if (mode == VSXF_MODE_WRITE)
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

#endif