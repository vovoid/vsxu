#include <filesystem/vsx_filesystem_archive_vsx.h>
#include <filesystem/vsx_filesystem_helper.h>
#include <debug/vsx_error.h>
#include <tools/vsx_req_error.h>
#include <vsx_thread_pool.h>

// Lzma stuff
#include "../vsxfst/Common/MyInitGuid.h"
#include "../vsxfst/7zip/Compress/LZMA/LZMADecoder.h"
#include "../vsxfst/7zip/Compress/LZMA/LZMAEncoder.h"
#include "../vsxfst/LzmaRam.h"

extern "C"
{
  #include "../vsxfst/LzmaRamDecode.h"
}


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

    if ( 0x0 == handle->get_compressed_data() )
      VSX_ERROR_CONTINUE("Compressed data is NULL.");

    unsigned char* outBuffer = 0;
    size_t outSize;
    size_t outSizeProcessed;
    if (LzmaRamGetUncompressedSize((unsigned char*) handle->get_compressed_data() , handle->compressed_size, &outSize) != 0)
      VSX_ERROR_CONTINUE("LZMA Data Error Getting Uncompressed size");

    if (outSize != 0)
      outBuffer = (unsigned char*)malloc(outSize);

    handle->uncompressed_data = 0;
    if (outBuffer)
    {
      LzmaRamDecompress(handle->get_compressed_data(), handle->compressed_size, outBuffer, outSize, &outSizeProcessed, malloc, free);
      handle->uncompressed_size = outSizeProcessed;
      handle->uncompressed_data = outBuffer;
      handle->clear_compressed_data();
    }
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
    pooled_size += archive_files[i].compressed_size;

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

    vsx_string<>filebuf_name;
    while ((read_char = fgetc(archive_handle)) != 0)
      filebuf_name.push_back(read_char);

    size -= filebuf_name.size()+1;

    // save info about this file so we can open it later
    filesystem_archive_info file_info;
    file_info.filename = filebuf_name;
    file_info.archive_position = ftell(archive_handle)+1;
    file_info.compressed_size = size;

    if (load_data_multithreaded)
    {
      file_info.set_compressed_data( (unsigned char*)malloc(file_info.compressed_size) );
      size_t rb = fread(file_info.get_compressed_data(), 1, size, archive_handle);
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
    archive_files[i].clear_compressed_data();
    archive_files[i].clear_uncompressed_data();
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
    handle->size = archive_files[i].compressed_size;
    handle->mode = file::mode_read;

    // read compressed data from the filesystem (or cache)
    unsigned char* inBuffer;

    // TODO: move this into functions
    if ( archive_files[i].get_compressed_data() )
    {
      inBuffer = archive_files[i].get_compressed_data();
    }
    else
    {
      inBuffer = (unsigned char*)malloc(handle->size);

      lock.aquire();

      fseek(archive_handle, archive_files[i].archive_position-1,SEEK_SET);
      if (!fread(inBuffer,1,handle->size, archive_handle))
      {
        delete handle;

        lock.release();

        handle = 0x0;
        return;
      }

      lock.release();
    }

    // already uncompressed in RAM?
    if (archive_files[i].uncompressed_data && archive_files[i].uncompressed_size)
    {
      handle->size = archive_files[i].uncompressed_size;
      handle->file_data = archive_files[i].uncompressed_data;
      handle->file_data_volatile = true;
      return;
    }

    // decompress the data
    unsigned char* outBuffer = 0;
    size_t outSize;
    size_t outSizeProcessed;
    if (LzmaRamGetUncompressedSize((unsigned char*)inBuffer, archive_files[i].compressed_size, &outSize) != 0)
      VSX_ERROR_RETURN("LZMA Data Error Getting Uncompressed size");

    if (!outSize)
    {
      handle->size = 0;
      return;
    }

    if (outSize)
      outBuffer = (unsigned char*)malloc(outSize);

    handle->file_data = 0;

    if (outBuffer)
    {
      LzmaRamDecompress(inBuffer, archive_files[i].compressed_size, outBuffer, outSize, &outSizeProcessed, malloc, free);
      handle->size = outSizeProcessed;
      handle->file_data = outBuffer;
    }

    if (!archive_files[i].get_compressed_data())
      free(inBuffer);

    return;
  }
}

void filesystem_archive_vsx::file_open_write(const char* filename, file* &handle)
{
  handle->position = 0;
  handle->size = 0;
  handle->file_data = (unsigned char*)(new vsx_nw_vector<char>);
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

  (*(vsx_nw_vector<char>*)(handle->file_data)).push_back(0);

  file_add(
    handle->filename,
    &((*((vsx_nw_vector<char>*)(handle->file_data)))[0]),
    ((vsx_nw_vector<char>*)(handle->file_data))->size(),
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
    Byte* data = 0x0;
    FILE* fp = 0;
    fp = fopen(filename.c_str(),"rb");
    if (!fp)
      VSX_ERROR_CONTINUE( ("fp is not valid for file: "+filename).c_str() );

    fseek (fp, 0, SEEK_END);
    uint32_t data_size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    data = new Byte[data_size];

    if ( !fread(data, sizeof(char), data_size, fp) && data_size != 0 )
      VSX_ERROR_CONTINUE("Error reading file!");

    fclose(fp);

    // time to allocate ram for compression
    int32_t dictionary = 1 << 21;
    size_t outSize = (size_t)data_size / 20 * 21 + (1 << 16);
    size_t outSizeProcessed;
    Byte* outBuffer = 0;

    if (outSize != 0)
      outBuffer = (Byte*)MyAlloc(outSize);

    LzmaRamEncode(data, data_size, outBuffer, outSize, &outSizeProcessed, dictionary, SZ_FILTER_AUTO);
    info->compressed_size = outSizeProcessed;
    info->compressed_data = outBuffer;

    float compression_factor = ((float)outSizeProcessed / (float)data_size) * 100.0;
    vsx_printf(L" ** compressed %f        %s from %d to %d \n", compression_factor, filename.c_str(), data_size, outSizeProcessed);
  }
}


void filesystem_archive_vsx::file_add_all()
{
  size_t pooled_size = 0;
  vsx_nw_vector<filesystem_archive_info*>* pool = new vsx_nw_vector<filesystem_archive_info*>();
  foreach (archive_files, i)
  {
    req_continue(archive_files[i].uncompressed_size);

    bool is_last = i == (archive_files.size() - 1);
    pool->push_back(&archive_files[i]);
    pooled_size += archive_files[i].uncompressed_size;

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

  vsx_thread_pool::instance()->wait_all();

  // Write all to disk
  foreach (archive_files, i)
  {
    fseek(archive_handle,0,SEEK_END);
    uint32_t data_size = archive_files[i].compressed_size + archive_files[i].filename.size() + 1;
    fwrite(&data_size, sizeof(uint32_t),1,archive_handle);
    fputs(archive_files[i].filename.c_str(), archive_handle);
    char nn = 0;
    fwrite(&nn,sizeof(char),1,archive_handle);
    fwrite(archive_files[i].compressed_data, sizeof(Byte), archive_files[i].compressed_size, archive_handle);
  }
}



int filesystem_archive_vsx::file_add
(
  vsx_string<>filename,
  char* data,
  uint32_t data_size,
  vsx_string<>disk_filename,
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
    file_info.uncompressed_size = vsx::filesystem_helper::file_get_size(filename);
    archive_files.push_back(file_info);
    return 0;
  }

  vsx_string<> fopen_filename = filename;
  if (disk_filename != "")
    fopen_filename = disk_filename;

  FILE* fp = 0;
  if (data == 0)
  {
    fp = fopen(fopen_filename.c_str(),"rb");
    if (!fp)
      VSX_ERROR_RETURN_V("fp is not valid", 1);

    fseek (fp, 0, SEEK_END);
    data_size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    data = new char[data_size];

    if ( !fread(data, sizeof(char), data_size, fp) && data_size != 0 )
      VSX_ERROR_RETURN_V("Error reading file!", 2);
  }

  fseek(archive_handle,0,SEEK_END);

  // time to allocate ram for compression
  UInt32 dictionary = 1 << 21;
  size_t outSize = (size_t)data_size / 20 * 21 + (1 << 16);
  size_t outSizeProcessed;
  Byte *outBuffer = 0;

  if (outSize != 0)
    outBuffer = (Byte *)MyAlloc(outSize);

  LzmaRamEncode((Byte*)data, data_size, outBuffer, outSize, &outSizeProcessed, dictionary, SZ_FILTER_AUTO);
  data_size = outSizeProcessed+filename.size()+1;
  fwrite(&data_size,sizeof(uint32_t),1,archive_handle);
  fputs(filename.c_str(),archive_handle);
  char nn = 0;
  fwrite(&nn,sizeof(char),1,archive_handle);
  fwrite(outBuffer,sizeof(Byte),outSizeProcessed,archive_handle);
  delete outBuffer;

  filesystem_archive_info finfo;
  finfo.filename = filename;
  archive_files.push_back(finfo);

  if (fp) {
    delete data;
    fclose(fp);
  }
  return 0;
}


}
