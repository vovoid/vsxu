#include <filesystem/archive/vsx/vsx_filesystem_archive_vsx_writer.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <debug/vsx_error.h>
#include <tools/vsx_req_error.h>
#include <tools/vsx_thread_pool.h>
#include <vsx_compression_lzma_old.h>

namespace vsx
{

void filesystem_archive_vsx_writer::create(const char* filename)
{
  archive_filename = filename;
  archive_handle = fopen(filename,"wb");
  const char header[5] = "VSXz";
  fwrite(header, sizeof(char), 4, archive_handle);
}

void filesystem_archive_vsx_writer::file_add_all_worker(vsx_nw_vector<filesystem_archive_file_write*>* work_list)
{
  for (size_t i = 0; i < work_list->size(); i++)
  {
    filesystem_archive_file_write* info = (*work_list)[i];
    vsx_string<> filename = info->filename;

    if (!info->data.size())
      info->data = filesystem_helper::read(filename);

    info->compressed_data = compression_lzma_old::compress( info->data );
  }
}

void filesystem_archive_vsx_writer::archive_files_saturate_all()
{
  foreach (archive_files, i)
  {
    filesystem_archive_file_write &archive_file = archive_files[i];
    if (archive_file.data.size())
      continue;
    req_continue(archive_file.operation == filesystem_archive_file_write::operation_add);
    archive_file.data = filesystem_helper::read(archive_file.source_filename);
  }
}


void filesystem_archive_vsx_writer::file_add_all()
{
  size_t pooled_size = 0;
  vsx_nw_vector<filesystem_archive_file_write*>* pool = new vsx_nw_vector<filesystem_archive_file_write*>();
  size_t files_to_process = 0;
  foreach (archive_files, i)
  {
    filesystem_archive_file_write &archive_file = archive_files[i];
    req_continue(archive_file.data.size());
    req_continue(archive_file.operation == filesystem_archive_file_write::operation_add);

    files_to_process++;
    bool is_last = i == (archive_files.size() - 1);

    pool->push_back(&archive_files[i]);
    pooled_size += archive_file.data.size();

    if (pooled_size <= work_chunk_size && !is_last)
      continue;

    vsx_thread_pool::instance()->add(
      [=](vsx_nw_vector<filesystem_archive_file_write*>* work_pool)
      {
        file_add_all_worker(work_pool);
        delete work_pool;
      },
      pool
    );
    pool = 0x0;
    if (!is_last)
      pool = new vsx_nw_vector<filesystem_archive_file_write*>();

    pooled_size = 0;
  }

  if (pool)
    vsx_thread_pool::instance()->add(
      [=](vsx_nw_vector<filesystem_archive_file_write*>* work_pool)
      {
        file_add_all_worker(work_pool);
        delete work_pool;
      },
      pool
    );

  vsx_thread_pool::instance()->wait_all(100);

  req(files_to_process);

  // Write all to disk
  foreach (archive_files, i)
  {
    fseek(archive_handle,0,SEEK_END);
    uint32_t data_size = (uint32_t)(archive_files[i].compressed_data.get_sizeof()+ archive_files[i].filename.size() + 1);
    fwrite(&data_size, sizeof(uint32_t),1,archive_handle);
    fputs(archive_files[i].filename.c_str(), archive_handle);
    char nn = 0;
    fwrite(&nn,sizeof(char),1,archive_handle);
    fwrite(archive_files[i].compressed_data.get_pointer(), sizeof(unsigned char), archive_files[i].compressed_data.get_sizeof(), archive_handle);
  }
}



void filesystem_archive_vsx_writer::add_file
(
  vsx_string<> filename,
  vsx_string<> disk_filename,
  bool deferred_multithreaded
)
{
  req(archive_handle);

  foreach(archive_files, i)
    if (archive_files[i].filename == filename)
      return;

  if (deferred_multithreaded)
  {
    filesystem_archive_file_write file_info;
    file_info.source_filename = disk_filename;
    file_info.filename = filename;
    file_info.operation = filesystem_archive_file_write::operation_add;
    archive_files.push_back(file_info);
    return;
  }

  vsx_string<> fopen_filename = filename;
  if (disk_filename != "")
    fopen_filename = disk_filename;

  vsx_ma_vector<unsigned char> uncompressed_data = filesystem_helper::read(fopen_filename);

  file_compress_and_add_to_archive(filename, uncompressed_data);

  filesystem_archive_file_write file_info;
  file_info.filename = filename;
  archive_files.move_back(std::move(file_info));
}

void filesystem_archive_vsx_writer::file_compress_and_add_to_archive(vsx_string<> filename, vsx_ma_vector<unsigned char>& uncompressed_data)
{
  vsx_ma_vector<unsigned char> compressed_data = compression_lzma_old::compress( uncompressed_data );

  uint32_t data_size = (uint32_t)(compressed_data.get_sizeof() + filename.size() + 1);

  fseek(archive_handle,0,SEEK_END);
  fwrite(&data_size,sizeof(uint32_t),1,archive_handle);
  fputs(filename.c_str(), archive_handle);

  char nn = 0;
  fwrite(&nn,sizeof(char),1,archive_handle);
  fwrite(compressed_data.get_pointer(), sizeof(unsigned char), compressed_data.get_sizeof(), archive_handle);
}

void filesystem_archive_vsx_writer::add_string(vsx_string<> filename, vsx_string<> payload, bool deferred_multithreaded)
{
  filesystem_archive_file_write file_info;
  file_info.filename = filename;
  file_info.operation = filesystem_archive_file_write::operation_add;

  foreach (payload, i)
    file_info.data[i] = (unsigned char)payload[i];

  if (deferred_multithreaded)
  {
    archive_files.move_back(std::move(file_info));
    return;
  }

  file_compress_and_add_to_archive(filename, file_info.data);
  archive_files.move_back(std::move(file_info));
}

void filesystem_archive_vsx_writer::close()
{
  req(archive_handle);

  archive_files_saturate_all();
  file_add_all();
  archive_filename = "";
  fclose(archive_handle);
  archive_handle = 0;

  for (size_t i = 0; i < archive_files.size(); i++)
  {
    archive_files[i].compressed_data.clear();
    archive_files[i].data.clear();
  }

  archive_files.clear();
}

}
