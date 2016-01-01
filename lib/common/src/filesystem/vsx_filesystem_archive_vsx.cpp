#include <filesystem/vsx_filesystem_archive_vsx.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <debug/vsx_error.h>
#include <tools/vsx_req_error.h>
#include <tools/vsx_thread_pool.h>
#include <vsx_compression_lzma_old.h>

namespace vsx
{

void filesystem_archive_vsx::create(const char* filename)
{
  archive_name = filename;
  archive_handle = fopen(filename,"wb");
  const char header[5] = "VSXz";
  fwrite(header, sizeof(char), 4, archive_handle);
}


void filesystem_archive_vsx::load_all_worker(vsx_nw_vector<filesystem_archive_info*>* my_work_list)
{
  for (size_t i = 0; i < my_work_list->size(); i++)
  {
    filesystem_archive_info* handle = (*my_work_list)[i];

    req_continue(handle->compressed_data.size());

    handle->uncompressed_data = compression_lzma_old::uncompress(handle->compressed_data);
    handle->compressed_data.clear();
  }
}

void filesystem_archive_vsx::load_all()
{
  size_t pooled_size = 0;
  vsx_nw_vector<filesystem_archive_info*>* pool = new vsx_nw_vector<filesystem_archive_info*>();
  foreach (archive_files, i)
  {
    bool is_last = i == (archive_files.size() - 1);
    pool->push_back(&archive_files[i]);
    pooled_size += archive_files[i].compressed_data.get_sizeof();

    if (pooled_size <= work_chunk_size && !is_last)
      continue;

    vsx_thread_pool::instance()->add(
      [=](vsx_nw_vector<filesystem_archive_info*>* work_pool)
      {
        load_all_worker(work_pool);
        delete work_pool;
      },
      pool
    );

    if (!is_last)
      pool = new vsx_nw_vector<filesystem_archive_info*>();

    pooled_size = 0;
  }
  vsx_thread_pool::instance()->wait_all();
}


int filesystem_archive_vsx::load(const char* archive_filename, bool load_data_multithreaded)
{
  if (is_archive())
    close();

  archive_name = archive_filename;
  archive_handle = fopen(archive_filename,"rb");

  req_error_v(archive_handle, (vsx_string<>("could not open archive: ") + archive_name).c_str(), 0)

  // Find out total size
  fseek (archive_handle, 0, SEEK_END);
  unsigned long size = ftell(archive_handle);

  // Sanitize size
  if (size < 4)
    return 1;

  fseek(archive_handle,0,SEEK_SET);

  // Verify header
  char header[5];
  header[4] = 0;
  if (!fread(header,sizeof(char),4,archive_handle))
    VSX_ERROR_RETURN_V("VSXz Reading header size with fread failed!",2);

  vsx_string<> hs(header);
  if (hs != "VSXz")
    VSX_ERROR_RETURN_V("VSXz tag is wrong",2);

  while (fread(&size,sizeof(uint32_t), 1, archive_handle) != 0)
  {
    char read_char;

    vsx_string<> filebuf_name;
    while ((read_char = fgetc(archive_handle)) != 0)
      filebuf_name.push_back(read_char);

    size -= filebuf_name.size()+1;

    // save info about this file so we can open it later
    filesystem_archive_info file_info;
    file_info.filename = filebuf_name;
    file_info.archive_position = ftell(archive_handle)+1;

    if (load_data_multithreaded)
    {
      file_info.compressed_data.allocate( size );
      size_t rb = fread(file_info.compressed_data.get_pointer(), 1, size, archive_handle);
      if (!rb)
        VSX_ERROR_EXIT("Could not read compressed data",100)
      archive_files.push_back(file_info);
      continue;
    }
    archive_files.push_back(file_info);

    // Default behaviour
    fseek(archive_handle, size, SEEK_CUR);
  }

  if (load_data_multithreaded)
    load_all();

  return 1;
}



void filesystem_archive_vsx::close()
{
  req(is_archive());

  file_add_all();
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



bool filesystem_archive_vsx::is_archive()
{
  return (archive_handle != 0);
}

bool filesystem_archive_vsx::is_archive_populated()
{
  return (archive_files.size() > 0);
}

bool filesystem_archive_vsx::is_file(vsx_string<> filename)
{
  foreach(archive_files, i)
    if ( vsx_string<>::s_equals(filename, archive_files[i].filename) )
      return true;
  return false;
}

vsx_nw_vector<filesystem_archive_info>* filesystem_archive_vsx::files_get()
{
  return &archive_files;
}


void filesystem_archive_vsx::file_open_read(const char* filename, file* &handle)
{
  bool found = false;
  vsx_string<>fname(filename);
  unsigned long i = 0;
  while (!found && i < archive_files.size())
  {
    if (!vsx_string<>::s_equals(archive_files[i].filename, fname))
    {
      i++;
      continue;
    }

    found = true;
    handle->filename = fname;
    handle->position = 0;
    handle->size = archive_files[i].compressed_data.size();
    handle->mode = file::mode_read;


    // already uncompressed in RAM?
    if (archive_files[i].uncompressed_data.size())
    {
      handle->size = archive_files[i].uncompressed_data.size();
      handle->data = archive_files[i].uncompressed_data;
      handle->data.set_volatile();
      return;
    }

    // saturate compressed data
    if ( !archive_files[i].compressed_data.size() )
    {
      archive_files[i].compressed_data.allocate( archive_files[i].uncompressed_data.size() );
      lock.aquire();

      fseek(archive_handle, archive_files[i].archive_position-1,SEEK_SET);
      if (!fread(archive_files[i].compressed_data.get_pointer(), 1, handle->size, archive_handle))
      {
        delete handle;

        lock.release();

        handle = 0x0;
        return;
      }

      lock.release();
    }

    // decompress data
    archive_files[i].uncompressed_data = compression_lzma_old::uncompress( archive_files[i].compressed_data );
    handle->data = archive_files[i].uncompressed_data;
    handle->data.set_volatile();

    return;
  }
}

void filesystem_archive_vsx::file_open_write(const char* filename, file* &handle)
{
  handle->position = 0;
  handle->filename = filename;
  handle->mode = file::mode_write;
}

void filesystem_archive_vsx::file_open(const char* filename, const char* mode, file* &handle)
{
  req_error(archive_handle, "archive handle not valid");

  vsx_string<> mode_search(mode);

  if (mode_search.find("r") != -1)
    file_open_read(filename, handle);

  if (mode_search.find("w") != -1)
    file_open_write(filename, handle);
}

void filesystem_archive_vsx::file_close(file* handle)
{
  req(is_archive());
  req(handle->mode == file::mode_write);

  handle->data.push_back(0);

  file_add(
    handle->filename,
    "",
    false
  );
}


void filesystem_archive_vsx::file_add_all_worker(vsx_nw_vector<filesystem_archive_info*>* work_list)
{
  for (size_t i = 0; i < work_list->size(); i++)
  {
    filesystem_archive_info* info = (*work_list)[i];
    vsx_string<> filename = info->filename;
    FILE* fp = 0;
    fp = fopen(filename.c_str(),"rb");
    if (!fp)
      VSX_ERROR_CONTINUE( ("fp is not valid for file: "+filename).c_str() );

    vsx_ma_vector<unsigned char> data;

    fseek (fp, 0, SEEK_END);
    data.allocate(ftell(fp));
    fseek(fp,0,SEEK_SET);

    req(data.size());

    if ( !fread(data.get_pointer(), sizeof(char), data.get_sizeof(), fp) )
    {
      fclose(fp);
      VSX_ERROR_CONTINUE("Error reading file!");
    }

    fclose(fp);

    info->compressed_data = compression_lzma_old::compress( data );

    float compression_factor = ((float)info->compressed_data.size() / (float)data.size()) * 100.0;
    vsx_printf(L" ** compressed %f        %s from %d to %d \n", compression_factor, filename.c_str(), data.size(), info->compressed_data.size());
  }
}


void filesystem_archive_vsx::file_add_all()
{
  size_t pooled_size = 0;
  vsx_nw_vector<filesystem_archive_info*>* pool = new vsx_nw_vector<filesystem_archive_info*>();
  size_t files_to_process = 0;
  foreach (archive_files, i)
  {
    filesystem_archive_info &archive_file = archive_files[i];
    req_continue(archive_file.uncompressed_data.size());

    files_to_process++;
    bool is_last = i == (archive_files.size() - 1);

    pool->push_back(&archive_file);
    pooled_size += archive_file.uncompressed_data.size();

    if (pooled_size <= work_chunk_size && !is_last)
      continue;

    vsx_thread_pool::instance()->add(
      [=](vsx_nw_vector<filesystem_archive_info*>* work_pool)
      {
        file_add_all_worker(work_pool);
        delete work_pool;
      },
      pool
    );

    if (!is_last)
      pool = new vsx_nw_vector<filesystem_archive_info*>();

    pooled_size = 0;
  }

  req(files_to_process);

  vsx_thread_pool::instance()->wait_all();

  // Write all to disk
  foreach (archive_files, i)
  {
    fseek(archive_handle,0,SEEK_END);
    uint32_t data_size = archive_files[i].compressed_data.get_sizeof()+ archive_files[i].filename.size() + 1;
    fwrite(&data_size, sizeof(uint32_t),1,archive_handle);
    fputs(archive_files[i].filename.c_str(), archive_handle);
    char nn = 0;
    fwrite(&nn,sizeof(char),1,archive_handle);
    fwrite(archive_files[i].compressed_data.get_pointer(), sizeof(unsigned char), archive_files[i].compressed_data.get_sizeof(), archive_handle);
  }
}



int filesystem_archive_vsx::file_add
(
  vsx_string<> filename,
  vsx_string<> disk_filename,
  bool deferred_multithreaded
)
{
  req_v(archive_handle, 1);

  foreach(archive_files, i)
    if (archive_files[i].filename == filename)
      return 1;

  if (deferred_multithreaded)
  {
    filesystem_archive_info file_info;
    file_info.filename = filename;
    file_info.uncompressed_data.allocate( vsx::filesystem_helper::file_get_size(filename) );
    archive_files.push_back(file_info);
    return 0;
  }

  vsx_string<> fopen_filename = filename;
  if (disk_filename != "")
    fopen_filename = disk_filename;

  vsx_ma_vector<unsigned char> uncompressed_data;
  FILE* fp = fopen(fopen_filename.c_str(),"rb");

  if (!fp)
    VSX_ERROR_RETURN_V("fp is not valid", 1);

  fseek (fp, 0, SEEK_END);
  uncompressed_data.allocate(ftell(fp));
  fseek(fp,0,SEEK_SET);

  if ( !fread(uncompressed_data.get_pointer(), sizeof(char), uncompressed_data.size(), fp) )
    VSX_ERROR_RETURN_V("Error reading file!", 2);

  fseek(archive_handle,0,SEEK_END);

  vsx_ma_vector<unsigned char> compressed_data = compression_lzma_old::compress( uncompressed_data );

  uint32_t data_size = compressed_data.get_sizeof();
  fwrite(&data_size,sizeof(uint32_t),1,archive_handle);

  fputs(filename.c_str(),archive_handle);

  char nn = 0;
  fwrite(&nn,sizeof(char),1,archive_handle);

  fwrite(compressed_data.get_pointer(), sizeof(unsigned char), compressed_data.get_sizeof(), archive_handle);
  fclose(fp);

  filesystem_archive_info finfo;
  finfo.filename = filename;
  archive_files.push_back(finfo);

  return 0;
}


}
