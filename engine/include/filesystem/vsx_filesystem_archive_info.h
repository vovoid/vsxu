#pragma once

namespace vsx_filesystem
{
  class archive_info
  {
  public:

    vsx_string<> filename;
    size_t archive_position = 0;

    size_t compressed_size = 0;
    void* compressed_data = 0x0;

    size_t uncompressed_size = 0;
    void* uncompressed_data = 0x0;

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
  };
}

