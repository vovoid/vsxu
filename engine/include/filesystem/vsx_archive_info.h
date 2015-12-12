#ifndef VSX_FILESYSTEM_ARCHIVE_INFO
#define VSX_FILESYSTEM_ARCHIVE_INFO

namespace vsx_filesystem
{
    class archive_info {

      void* compressed_data;

    public:

      vsx_string<> filename;
      size_t archive_position;
      size_t compressed_size;
      void* uncompressed_data;
      size_t uncompressed_size;

      void* get_compressed_data()
      {
        return compressed_data;
      }

      void set_compressed_data(void* n)
      {
        compressed_data = n;
      }

      void clear_compressed_data()
      {
        if (compressed_data)
          free(compressed_data);
        compressed_data = 0;
      }

      void clear_uncompressed_data()
      {
        if (uncompressed_data)
          free(uncompressed_data);
        uncompressed_data = 0;
      }


      archive_info()
        :
        compressed_data(0x0),
        archive_position(0),
        compressed_size(0),
        uncompressed_data(0x0),
        uncompressed_size(0)
      {}

      ~archive_info()
      {
      }
    };

}

#endif