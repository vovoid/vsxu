#include <stdio.h>
#include <stdint.h>
#include <vsx_compression_lzham.h>
#include <lib/compression/thirdparty/lzham-sdk/include/lzham.h>
#include <string/vsx_string.h>

namespace vsx
{

vsx_ma_vector<unsigned char> compression_lzham::compress(vsx_ma_vector<unsigned char> &uncompressed_data)
{
  vsx_ma_vector<unsigned char> compressed_data;

  lzham_compress_params comp_params = {
    sizeof(lzham_compress_params),
    LZHAM_MAX_DICT_SIZE_LOG2_X86,
    LZHAM_COMP_LEVEL_DEFAULT,
    LZHAM_DEFAULT_TABLE_UPDATE_RATE,
    -1,
    0,
    0,
    NULL,
    64,
    64
  };

  lzham_compress_state_ptr state = lzham_compress_init(&comp_params);

  lzham_compress_status_t status;
  vsx_ma_vector<unsigned char> out_buf;
  out_buf.allocate(1024*1024);
  size_t in_buf_ofs = 0;
  forever
  {
    size_t out_buf_size = 1024*1024;
    size_t in_buf_size = uncompressed_data.get_sizeof() - in_buf_ofs;

    status = lzham_compress(
      state,
      uncompressed_data.get_pointer() + in_buf_ofs,
      &in_buf_size,
      out_buf.get_pointer(),
      &out_buf_size,
      1
    );

    in_buf_ofs += in_buf_size;

    if (out_buf_size)
    {
      out_buf.reset_used(out_buf_size);
      foreach (out_buf, i)
        compressed_data.push_back(out_buf[i]);
    }

    if (status >= LZHAM_COMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
      break;
  }

  if (status > 3)
    vsx_printf(L"LZHAM compression failed with status code %d\n", status);

  lzham_compress_deinit(state);
  return compressed_data;
}

void compression_lzham::uncompress(
    vsx_ma_vector<unsigned char> &uncompressed_data,
    vsx_ma_vector<unsigned char> &compressed_data
)
{
  lzham_decompress_params decomp_params = {
      sizeof(lzham_decompress_params),
      LZHAM_MAX_DICT_SIZE_LOG2_X86,
      LZHAM_DEFAULT_TABLE_UPDATE_RATE,
      0,
      0,
      NULL,
      64,
      64
    };

  size_t dest_len = uncompressed_data.get_sizeof();
  lzham_decompress_memory(&decomp_params, uncompressed_data.get_pointer(), &dest_len, compressed_data.get_pointer(), compressed_data.get_sizeof(), 0x0);


  /*
  lzham_decompress_state_ptr state = lzham_decompress_init(&decomp_params);

  lzham_decompress_status_t status;
  vsx_ma_vector<unsigned char> out_buf;
  out_buf.allocate(1024*1024);
  size_t original_size = uncompressed_data.get_sizeof();
  uncompressed_data.reset_used();

  size_t in_buf_ofs = 0;
  uint out_buf_written = 0;

  while (out_buf_written < original_size)
  {
    size_t out_buf_size = 1024*1024;
    size_t in_buf_size = compressed_data.get_sizeof() - in_buf_ofs;

    status =
      lzham_decompress(
        state,
        compressed_data.get_pointer() + in_buf_ofs,
        &in_buf_size,
        out_buf.get_pointer(),
        &out_buf_size,
        1
      );

    in_buf_ofs += in_buf_size;

    if (out_buf_size)
    {
      out_buf.reset_used(out_buf_size);
      foreach (out_buf, i)
        uncompressed_data.push_back(out_buf[i]);

      out_buf_written += out_buf_size;
    }


    if (status >= LZHAM_DECOMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
      break;
  }

  if (status > LZHAM_DECOMP_STATUS_FIRST_SUCCESS_OR_FAILURE_CODE)
    vsx_printf(L"LZHAM decompression failed, status code: %d\n", status);
  lzham_decompress_deinit(state);*/
}

vsx_ma_vector<unsigned char> compression_lzham::uncompress(vsx_ma_vector<unsigned char> &compressed_data, size_t original_size)
{
  vsx_ma_vector<unsigned char> uncompressed_data;
  reqrv(original_size, uncompressed_data);
  uncompressed_data.allocate(original_size - 1);
  uncompress(uncompressed_data, compressed_data);
  return uncompressed_data;
}



}
