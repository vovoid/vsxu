#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_writer.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <vsx_compression_lzma.h>
#include <vsx_compression_lzham.h>
#include <tools/vsx_thread_pool.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_header.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_chunk_write.h>
#include <filesystem/vsx_filesystem_identifier.h>
#include <filesystem/tree/vsx_filesystem_tree_serialize_binary.h>

namespace vsx
{
void filesystem_archive_vsxz_writer::create(const char* filename)
{
  archive_filename = filename;
}

void filesystem_archive_vsxz_writer::add_file
(
  vsx_string<> filename,
  vsx_string<> disk_filename,
  bool deferred_multithreaded
)
{
  VSX_UNUSED(deferred_multithreaded);
  filesystem_archive_file_write file_info;
  file_info.operation = filesystem_archive_file_write::operation_add;
  file_info.filename = filename;
  file_info.source_filename = disk_filename;
  if (!disk_filename.size())
    file_info.source_filename = filename;

  archive_files.move_back(std::move(file_info));
}


void filesystem_archive_vsxz_writer::add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded)
{
  VSX_UNUSED(deferred_multithreaded);
  filesystem_archive_file_write file_info;
  file_info.filename = filename;
  req(payload.size());
  file_info.data.allocate( payload.size() - 1 );
  file_info.operation = filesystem_archive_file_write::operation_add;

  foreach (payload, i)
    file_info.data[i] = (unsigned char)payload[i];

  archive_files.move_back(std::move(file_info));
}


void filesystem_archive_vsxz_writer::archive_files_saturate_all()
{
  vsx_printf(L"reading all files from disk...\n");
  foreach (archive_files, i)
  {
    filesystem_archive_file_write &archive_file = archive_files[i];
    vsx_printf(L"reading all files from disk: %hs\n", archive_file.filename.c_str());
    if (archive_file.data.size())
      continue;
    req_continue(archive_file.operation == filesystem_archive_file_write::operation_add);
    archive_file.data = filesystem_helper::read(archive_file.source_filename);
  }
  vsx_printf(L"reading all files from disk [DONE]\n");
}



void filesystem_archive_vsxz_writer::close()
{
  req(archive_files.size());

  vsx_ma_vector<bool> is_processed;
  is_processed.allocate(archive_files.size() - 1);
  is_processed.memory_clear();

  // Read all files from disk
  archive_files_saturate_all();


  // Calculate compression ratios for all large files
  vsx_ma_vector<float> compression_ratios;
  compression_ratios.allocate(archive_files.size() - 1);
  compression_ratios.memory_clear();

  foreach (archive_files, i)
  {

    // look for largest file
    size_t id_found = 0;
    int64_t max_size_found = -1;
    foreach (archive_files, si)
    {
      if (is_processed[si])
        continue;

      int64_t cur_size = archive_files[si].data.size();
      if (cur_size > max_size_found)
      {
        id_found = si;
        max_size_found = cur_size;
      }
    }
    is_processed[id_found] = true;

    if (archive_files[id_found].data.size() <= 1024*1024)
      break;


    compression_ratios[id_found] = 1.0f;
    req_continue(do_compress);
    vsx_thread_pool::instance()->add( [&](float& ratio, size_t ai)
      {
        vsx_printf(L"calculating ratio for %hs\n", archive_files[ai].filename.c_str());
        vsx_ma_vector<unsigned char> lzma_compressed = vsx::compression_lzma::compress( archive_files[ai].data );
        float lzma_ratio = (float)(lzma_compressed.size()) / (float)(archive_files[ai].data.size());
        vsx_ma_vector<unsigned char> lzham_compressed = vsx::compression_lzham::compress( archive_files[ai].data );
        float lzham_ratio = (float)(lzham_compressed.size()) / (float)(archive_files[ai].data.size());
        if (lzham_ratio < lzma_ratio)
          ratio = lzham_ratio;
        else
          ratio = lzma_ratio;
        vsx_printf(L"calculating ratio for %hs [DONE]\n", archive_files[ai].filename.c_str());
      },
      compression_ratios[id_found],
      id_found
    );
  }

  vsx_thread_pool::instance()->wait_all(100);


  // Add files to compression chunks
  const size_t max_chunks = 9;
  filesystem_archive_chunk_write chunks[max_chunks];
  is_processed.memory_clear();

  foreach (archive_files, i)
  {
    // look for largest file
    size_t id_found = 0;
    int64_t max_size_found = -1;
    foreach (archive_files, si)
    {
      if (is_processed[si])
        continue;

      int64_t cur_size = archive_files[si].data.size();
      if (cur_size > max_size_found)
      {
        id_found = si;
        max_size_found = cur_size;
      }
    }

    is_processed[id_found] = true;

    // handle text file
    if (filesystem_identifier::is_text_file(archive_files[id_found].data))
    {
      chunks[1].add_file( &archive_files[id_found] );
      is_processed[id_found] = true;
      vsx_printf(L"adding file %hs to chunk 1\n", archive_files[id_found].filename.c_str());
      continue;
    }

    // if file is large and has bad compression ratio add to uncompressed chunk
    if (
      !do_compress
      ||
      (
        archive_files[id_found].data.size() > 1024*1024
        &&
        compression_ratios[id_found] > 0.8
      )
    )
    {
      vsx_printf(L"adding file %hs to chunk 0\n", archive_files[id_found].filename.c_str());
      chunks[0].add_file( &archive_files[id_found] );
      continue;
    }


    // find out which chunk has least data in it
    size_t target_chunk = 2;
    uint64_t min_size = 0xffffffffffffffff;
    for_n(ichunk, 2, max_chunks)
    {
      if (!chunks[ichunk].is_size_above_treshold())
      {
        target_chunk = ichunk;
        break;
      }

      if (chunks[ichunk].uncompressed_data.size() < min_size)
      {
        target_chunk = ichunk;
        min_size = chunks[ichunk].uncompressed_data.size();
      }
    }

    vsx_printf(L"adding file %hs to chunk %lld\n", archive_files[id_found].filename.c_str(), target_chunk);
    chunks[target_chunk].add_file( &archive_files[id_found] );
  }

  foreach (is_processed, i)
    if (!is_processed[i])
      vsx_printf(L"*** file id %ld not processed! (%hs)\n",(long)i, archive_files[i].filename.c_str());

  // Set chunk id in all file info structs
  for_n(i, 0, max_chunks)
    chunks[i].set_chunk_id((uint8_t)i);

  // Add to file tree
  vsx_filesystem_tree_writer tree;
  size_t file_list_index = 1;

  for_n(i, 0, max_chunks)
    chunks[i].add_to_tree( tree, file_list_index);

  // Compress chunks
  for_n(i, 1, max_chunks)
    chunks[i].compress();
  threaded_task_wait_all(100);

  // Count valid chunks
  size_t valid_chunk_index = 2;
  for (; valid_chunk_index < max_chunks; valid_chunk_index++)
    if (!chunks[valid_chunk_index].has_files())
      break;

  // Serialize tree
  vsx_ma_vector<unsigned char> tree_data = vsx_filesystem_tree_serialize_binary::serialize(tree);

  // Calculate & fill in header
  vsxz_header header;
  header.file_count = (uint32_t)archive_files.size();
  header.chunk_count = (uint32_t)valid_chunk_index;

  for_n(i, 0, max_chunks)
    header.compression_uncompressed_memory_size += chunks[i].get_compressed_uncompressed_size();

  header.file_count = (uint32_t)archive_files.size();
  header.tree_size = (uint32_t)tree_data.size();



  // Write archive to disk
  FILE* file = fopen(archive_filename.c_str(),"wb");

  {
    // header:
    fwrite(&header, sizeof(vsxz_header), 1, file);

    // tree:
    fwrite(tree_data.get_pointer(), 1, tree_data.get_sizeof(), file);

    // file info table:
    for_n(i, 0, max_chunks)
      chunks[i].write_file_info_table(file);

    // chunk info table:
    chunks[0].write_chunk_info_table(file, true);
    chunks[1].write_chunk_info_table(file, true);
    for_n(i, 2, max_chunks)
      chunks[i].write_chunk_info_table(file);

    // compressed data:
    for_n(i, 0, max_chunks)
      chunks[i].write_data(file);
  }
  fclose(file);
}

}

