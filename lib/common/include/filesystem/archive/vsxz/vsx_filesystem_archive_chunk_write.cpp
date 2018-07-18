#include <filesystem/archive/vsxz/vsx_filesystem_archive_chunk_write.h>
#include <vsx_compression_lzham.h>
#include <vsx_compression_lzma.h>

void vsx::filesystem_archive_chunk_write::compress()
{
  req(uncompressed_data.size());

  threaded_task {

    vsx_printf(L"compressing chunk %d\n", file_info_table[0].chunk);
    vsx_ma_vector< unsigned char> uncompressed_data_readback;

    vsx_perf perf;
    perf.cpu_instructions_start();

    // LZHAM
    compressed_data = vsx::compression_lzham::compress( uncompressed_data );
    perf.cpu_instructions_begin();
    uncompressed_data_readback = vsx::compression_lzham::uncompress(compressed_data, uncompressed_data.size() );
    long long cpu_instructions_lzham = perf.cpu_instructions_end();

    // LZMA
    compression_type = compression_lzma;
    compressed_data = vsx::compression_lzma::compress( uncompressed_data );
    perf.cpu_instructions_begin();
    uncompressed_data_readback = vsx::compression_lzma::uncompress(compressed_data, uncompressed_data.size() );
    long long cpu_instructions_lzma = perf.cpu_instructions_end();

    if (cpu_instructions_lzham < cpu_instructions_lzma)
    {
      compressed_data = vsx::compression_lzham::compress( uncompressed_data );
      compression_type = compression_lzham;
    }

    perf.cpu_instructions_stop();
    vsx_printf(L"compressing chunk %d [DONE]\n", file_info_table[0].chunk);
  } threaded_task_end;
}
