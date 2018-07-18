#include <filesystem/archive/vsx/vsx_filesystem_archive_vsx_reader.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <string/vsx_string_helper.h>
#include <debug/vsx_error.h>
#include <tools/vsx_req_error.h>
#include <tools/vsx_thread_pool.h>
#include <vsx_compression_lzma_old.h>

namespace vsx
{


void filesystem_archive_vsx_reader::load_all_worker(vsx_nw_vector<filesystem_archive_file_read*>* work_list)
{
  for (size_t i = 0; i < work_list->size(); i++)
  {
    filesystem_archive_file_read* handle = (*work_list)[i];

    req_continue(handle->compressed_data.size());

    handle->uncompressed_data = compression_lzma_old::uncompress(handle->compressed_data);
    handle->compressed_data.clear();
  }
}

void filesystem_archive_vsx_reader::load_all()
{
  size_t pooled_size = 0;
  vsx_nw_vector<filesystem_archive_file_read*>* pool = new vsx_nw_vector<filesystem_archive_file_read*>();
  foreach (archive_files, i)
  {
    bool is_last = i == (archive_files.size() - 1);
    pool->push_back(&archive_files[i]);
    pooled_size += archive_files[i].compressed_data.get_sizeof();

    if (pooled_size <= work_chunk_size && !is_last)
      continue;

    vsx_thread_pool::instance()->add(
      [=](vsx_nw_vector<filesystem_archive_file_read*>* work_pool)
      {
        load_all_worker(work_pool);
        delete work_pool;
      },
      pool
    );

    if (!is_last)
      pool = new vsx_nw_vector<filesystem_archive_file_read*>();

    pooled_size = 0;
  }
  vsx_printf(L"waiting for threads...\n");
  vsx_thread_pool::instance()->wait_all(100);
  vsx_printf(L"done waiting\n");
}


bool filesystem_archive_vsx_reader::load(const char* archive_filename, bool load_data_multithreaded)
{
  if (is_archive())
    close();

  archive_name = archive_filename;
  archive_handle = fopen(archive_filename,"rb");

  req_error_v(archive_handle, (vsx_string<>("could not open archive: ") + archive_name).c_str(), false);

  // Find out total size
  fseek (archive_handle, 0, SEEK_END);
  size_t size = ftell(archive_handle);

  // Sanitize size
  reqrv(size > 4, false);

  fseek(archive_handle,0,SEEK_SET);

  // Verify header
  char header[5];
  header[4] = 0;
  if (!fread(header,sizeof(char),4,archive_handle))
    VSX_ERROR_RETURN_V("VSXz Reading header size with fread failed!",0);

  vsx_string<> hs(header);
  if (hs != "VSXz")
    VSX_ERROR_RETURN_V("VSXz tag is wrong",0);

  while (fread(&size,sizeof(uint32_t), 1, archive_handle) != 0)
  {
    char read_char;

    vsx_string<> filebuf_name;
    while ((read_char = fgetc(archive_handle)) != 0)
      filebuf_name.push_back(read_char);

    size -= filebuf_name.size()+1;

    // save info about this file so we can open it later
    filesystem_archive_file_read file_info;
    file_info.filename = filebuf_name;
    file_info.archive_position = ftell(archive_handle)+1;
    file_info.compressed_data.allocate( size - 1 );

    if (load_data_multithreaded)
    {
      size_t rb = fread(file_info.compressed_data.get_pointer(), 1, size, archive_handle);
      if (!rb)
        VSX_ERROR_EXIT("Could not read compressed data",100)
      archive_files.move_back(std::move(file_info));
      continue;
    }
    archive_files.move_back(std::move(file_info));

    // Default behaviour
    fseek(archive_handle, (long)size, SEEK_CUR);
  }

  if (load_data_multithreaded)
    load_all();

  return true;
}


void filesystem_archive_vsx_reader::files_get(vsx_nw_vector<filesystem_archive_file_read>& files)
{
  foreach (archive_files, i)
    files.push_back( archive_files[i] );
  foreach (files, i)
  {
    files[i].compressed_data.set_volatile();
    files[i].uncompressed_data.set_volatile();
  }
}

void filesystem_archive_vsx_reader::file_open(const char* filename, file* &handle)
{
  req_error(archive_handle, "archive handle not valid");

  bool found = false;
  vsx_string<> fname(filename);
  unsigned long i = 0;
  while (!found && i < archive_files.size())
  {
    filesystem_archive_file_read& archive_file = archive_files[i];
    if (!vsx_string<>::s_equals(archive_file.filename, fname))
    {
      i++;
      continue;
    }

    found = true;
    handle->filename = fname;
    handle->size = 0;
    handle->position = 0;

    // already uncompressed in RAM?
    if (archive_file.uncompressed_data.size())
    {
      handle->size = archive_file.uncompressed_data.size();
      handle->data.set_volatile();
      handle->data.set_data(archive_file.uncompressed_data.get_pointer(), archive_file.uncompressed_data.size() );
      return;
    }

    // saturate compressed data
    lock.aquire();
      fseek(archive_handle, (long)archive_file.archive_position - 1, SEEK_SET);
      if (!fread(archive_file.compressed_data.get_pointer(), 1, archive_file.compressed_data.get_sizeof(), archive_handle))
      {
        delete handle;

        lock.release();

        handle = 0x0;
        return;
      }
    lock.release();

    // decompress data
    archive_file.uncompressed_data = compression_lzma_old::uncompress( archive_file.compressed_data );
    handle->data.set_volatile();
    handle->data.set_data( archive_file.uncompressed_data.get_pointer(), archive_file.uncompressed_data.get_sizeof());
    handle->size = archive_file.uncompressed_data.size();

    return;
  }
}

void filesystem_archive_vsx_reader::close()
{
  req(is_archive());

  archive_name = "";
  fclose(archive_handle);
  archive_handle = 0;

  for (size_t i = 0; i < archive_files.size(); i++)
  {
    archive_files[i].compressed_data.clear();
    archive_files[i].uncompressed_data.clear();
  }

  archive_files.clear();
}



bool filesystem_archive_vsx_reader::is_archive()
{
  return (archive_handle != 0);
}

bool filesystem_archive_vsx_reader::is_archive_populated()
{
  return (archive_files.size() > 0);
}

bool filesystem_archive_vsx_reader::is_file(vsx_string<> filename)
{
  foreach(archive_files, i)
    if ( vsx_string<>::s_equals(filename, archive_files[i].filename) )
      return true;
  return false;
}


}
