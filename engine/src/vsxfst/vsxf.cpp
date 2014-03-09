/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include <stdio.h>


// Lzma stuff
#include "Common/MyInitGuid.h"

#if defined(_WIN32)
#include <io.h>
#include <fcntl.h>
#define MY_SET_BINARY_MODE(file) setmode(fileno(file),O_BINARY)
#else
#define MY_SET_BINARY_MODE(file)
#endif

#include <sys/types.h>
#include <sys/stat.h>

#include "7zip/Compress/LZMA/LZMADecoder.h"
#include "7zip/Compress/LZMA/LZMAEncoder.h"

#include "vsx_string.h"
#include "vsx_avector.h"

#include "LzmaRam.h"

#include "vsx_platform.h"
#include "vsx_version.h"
#include "vsx_error.h"

#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
  #include <stdio.h>
  #include <stdlib.h>
  #include <string>
  #include <sys/stat.h>
  #include <unistd.h>
#endif

extern "C"
{
  #include "LzmaRamDecode.h"
}

#include "vsxfst.h"

#ifdef _WIN32
bool g_IsNT = false;
#endif


vsxf::vsxf()
{
  type = VSXF_TYPE_FILESYSTEM;
  archive_handle = 0;
  pthread_mutex_init(&mutex1, NULL);
}


void vsxf::set_base_path(vsx_string new_base_path)
{
  base_path = new_base_path;
}


vsx_string vsxf::get_base_path()
{
  return base_path;
}


void vsxf::archive_create(const char* filename)
{
  archive_name = filename;
  type = VSXF_TYPE_ARCHIVE;
  archive_handle = fopen(filename,"wb");
  const char header[5] = "VSXz";
  fwrite(header,sizeof(char),4,archive_handle);
}


vsx_avector<vsxf_archive_info>* vsxf::get_archive_files()
{
  return &archive_files;
}


void vsxf::archive_close()
{
  if (type != VSXF_TYPE_ARCHIVE)
    return;

  archive_name = "";

  if (archive_handle)
  {
    fclose(archive_handle);
    archive_handle = 0;
  }
  type = VSXF_TYPE_FILESYSTEM;
  archive_files.clear();
}

int vsxf::archive_add_file
(
  vsx_string filename,
  char* data,
  uint32_t data_size,
  vsx_string disk_filename
)
{
  if (!archive_handle)
    return 1;

  unsigned long i = 0;
  while (i < archive_files.size())
  {
    if (archive_files[i].filename == filename)
       return 1;
    ++i;
  }

  vsx_string fopen_filename = filename;
  if (disk_filename != "")
    fopen_filename = disk_filename;

  FILE* fp = 0;
  if (data == 0)
  {
    fp = fopen(fopen_filename.c_str(),"rb");
    if (!fp)
      ERROR_RETURN_V("fp is not valid", 1);

    fseek (fp, 0, SEEK_END);
    data_size = ftell(fp);
    fseek(fp,0,SEEK_SET);
    data = new char[data_size];

    if ( !fread(data, sizeof(char), data_size, fp) && data_size != 0 )
      ERROR_RETURN_V("Error reading file!", 2);
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

  vsxf_archive_info finfo;
  finfo.filename = filename;
  archive_files.push_back(finfo);

  if (fp)
  {
    delete data;
    fclose(fp);
  }
  return 0;
}

int vsxf::archive_load(const char* filename, bool preload_compressed_data)
{
  // precaution, we don't wanna waste RAM
  if (type == VSXF_TYPE_ARCHIVE)
    archive_close();

  archive_name = filename;
  archive_handle = fopen(filename,"rb");

  if (!archive_handle)
  {
    printf("error! \"%s\" is not a valid file handle!\n",filename);
    return 0;
  }

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
    ERROR_RETURN_V("VSXz Reading header size with fread failed!",2);

  vsx_string hs(header);
  if (hs != "VSXz")
    ERROR_RETURN_V("VSXz tag is wrong",2);

  while (fread(&size,sizeof(uint32_t),1,archive_handle) != 0)
  {
    char read_char;

    vsx_string filebuf_name;
    while ((read_char = fgetc(archive_handle)) != 0)
      filebuf_name.push_back(read_char);

    size -= filebuf_name.size()+1;

    // save info about this file so we can open it later
    vsxf_archive_info finfo;
    finfo.filename = filebuf_name;
    finfo.archive_position = ftell(archive_handle)+1;
    finfo.compressed_size = size;

    if (preload_compressed_data)
    {
      finfo.set_compressed_data( malloc(finfo.compressed_size) );
      size_t rb = fread(finfo.get_compressed_data(), 1, size, archive_handle);
      if (!rb)
        ERROR_EXIT("Could not read compressed data",100)
      archive_files.push_back(finfo);
      continue;
    }
    archive_files.push_back(finfo);
    // Default behaviour
    fseek(archive_handle, size, SEEK_CUR);
  }
  type = VSXF_TYPE_ARCHIVE;
  return 1;
}

bool vsxf::is_archive()
{
  return (type == VSXF_TYPE_ARCHIVE);
}

bool vsxf::is_archive_populated()
{
  return (archive_files.size() > 0);
}

bool vsxf::is_file(const char* filename)
{
  vsxf_handle *fp;
  fp = this->f_open(filename, "r");
  if (fp == NULL)
  {
    return false;
  }
  this->f_close(fp);
  return true;
}


bool vsxf::is_file(const vsx_string filename)
{
  vsxf_handle *fp;
  fp = this->f_open(filename.c_str(), "r");
  if (fp == NULL)
  {
    return false;
  }
  this->f_close(fp);
  return true;
}

void* vsxf::worker(void* p)
{
  vsx_avector<vsxf_archive_info*>* my_work_list = (vsx_avector<vsxf_archive_info*>*)p;

  for (size_t i = 0; i < my_work_list->size(); i++)
  {
    vsxf_archive_info* handle = (*my_work_list)[i];

    if ( 0x0 == handle->get_compressed_data() )
      ERROR_CONTINUE("Compressed data is NULL.");

    void* outBuffer = 0;
    size_t outSize;
    size_t outSizeProcessed;
    if (LzmaRamGetUncompressedSize((unsigned char*) handle->get_compressed_data() , handle->compressed_size, &outSize) != 0)
      ERROR_CONTINUE("LZMA Data Error Getting Uncompressed size");

    if (outSize != 0)
    {
      outBuffer = malloc(outSize);
    }
    handle->uncompressed_data = 0;
    if (outBuffer != 0)
    {
      LzmaRamDecompress((unsigned char*)handle->get_compressed_data(), handle->compressed_size, (unsigned char*)outBuffer, outSize, &outSizeProcessed, malloc, free);
      handle->uncompressed_size = outSizeProcessed;
      handle->uncompressed_data = outBuffer;
      handle->clear_compressed_data();
    }
  }


  pthread_exit(NULL);
  // the thread will die here.
  return 0;
}

void vsxf::archive_load_all_mt(const char* filename)
{
  // Options
  const size_t num_threads = 8;
  const size_t work_chunk_size = 1024*1024 * 5;

  // 1. load the archive into memory (compressed)
  archive_load(filename, true);

  // 2. Construct lists to work on.
  vsx_avector<vsxf_archive_info*> work_pool[num_threads];

//  size_t max_threads_needed = 0;s
  size_t cur_pool_id = 0;
  size_t cur_pooled_size = 0;
  for (size_t i = 0; i < archive_files.size(); i++)
  {
    work_pool[cur_pool_id].push_back(&archive_files[i]);
    cur_pooled_size += archive_files[i].compressed_size;
    if (cur_pooled_size > work_chunk_size)
    {
      cur_pooled_size = 0;
      cur_pool_id++;
      cur_pool_id &= num_threads-1;
    }
  }

//  vsx_printf("Going to use %d threads\n",)

  // 3. Fire off the threads
  pthread_t threads[num_threads];
  pthread_attr_t attrs[num_threads];
  for (size_t i = 0; i < num_threads; i++)
  {
    pthread_attr_init(&attrs[i]);
    pthread_create(&threads[i], &attrs[i], &worker, &work_pool[i]);
  }

  // 4. Wait for the threads to finish
  for (size_t i = 0; i < num_threads; i++)
  {
    pthread_join(threads[i],NULL);
    pthread_attr_destroy(&attrs[i]);
  }

}

vsxf_handle* vsxf::f_open(const char* filename, const char* mode)
{
  vsx_string i_filename(filename);

  // sanity check
  if (!i_filename.size())
    return NULL;

  vsxf_handle* handle = new vsxf_handle;

  // 1.  are we archive or filesystem?
  // 1a. archive:
  //     find the file location in the disk archive in the archive specification
  //     load/decompress filename from disk into ram
  // 1b. file:
  //     get a file descriptor from disk

  // lock the class

  if (type == VSXF_TYPE_FILESYSTEM)
  {
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
      i_filename = str_replace("\\","/",i_filename);
    #endif
    #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
      i_filename = str_replace("/","\\",i_filename);
    #endif

    handle->file_handle = fopen((base_path+i_filename).c_str(),mode);

    if (handle->file_handle == NULL)
    {
      delete handle;
      handle = 0x0;
    }
    return handle;
  }

  if (!archive_handle)
    ERROR_RETURN_V("archive handle not valid",0x0);

  vsx_string mode_search(mode);
  if (mode_search.find("r") != -1)
  {
    bool found = false;
    vsx_string fname(filename);
    unsigned long i = 0;
    while (!found && i < archive_files.size())
    {
      if (archive_files[i].filename == fname)
      {
        found = true;
        handle = new vsxf_handle;
        handle->filename = fname;
        handle->position = 0;
        handle->size = archive_files[i].compressed_size;
        handle->mode = VSXF_MODE_READ;

        // read compressed data from the filesystem (or cache)
        void* inBuffer;
        if ( archive_files[i].get_compressed_data() )
        {
          inBuffer = archive_files[i].get_compressed_data();
        }
        else
        {
          inBuffer = malloc(handle->size);

          if (archive_handle)
            aquire_lock();

          fseek(archive_handle, archive_files[i].archive_position-1,SEEK_SET);
          if (!fread(inBuffer,1,handle->size, archive_handle))
          {
            delete handle;

            if (archive_handle)
            release_lock();

            return NULL;
          }

          if (archive_handle)
          release_lock();
        }

        // If already uncompressed in RAM
        if (archive_files[i].uncompressed_data && archive_files[i].uncompressed_size)
        {
          handle->size = archive_files[i].uncompressed_size;
          handle->file_data = archive_files[i].uncompressed_data;
          return handle;
        }

        // decompress the data
        void* outBuffer = 0;
        size_t outSize;
        size_t outSizeProcessed;
        if (LzmaRamGetUncompressedSize((unsigned char*)inBuffer, archive_files[i].compressed_size, &outSize) != 0)
          ERROR_RETURN_V("LZMA Data Error Getting Uncompressed size", 0x0);

        if (outSize != 0)
        {
          outBuffer = malloc(outSize);
        }
        handle->file_data = 0;
        if (outBuffer != 0)
        {
          LzmaRamDecompress((unsigned char*)inBuffer, archive_files[i].compressed_size, (unsigned char*)outBuffer, outSize, &outSizeProcessed, malloc, free);
          handle->size = outSizeProcessed;
          handle->file_data = outBuffer;
        }

        // Only free in buffer if we created it
        if (!archive_files[i].get_compressed_data())
          free(inBuffer);

        return handle;
      }
      ++i;
    }
  } else
  if (mode_search.find("w") != -1)
  {
    handle->position = 0;
    handle->size = 0;
    handle->file_data = (void*)(new vsx_avector<char>);
    handle->filename = filename;
    handle->mode = VSXF_MODE_WRITE;


    return handle;
  }

  // safeguard
  delete handle;
  return 0;
}

void vsxf::f_close(vsxf_handle* handle)
{
  if (!handle)
    return;

  if (type == VSXF_TYPE_FILESYSTEM)
    fclose(handle->file_handle);

  if (type == VSXF_TYPE_ARCHIVE)
  {
    if (handle->mode == VSXF_MODE_WRITE)
    {
      (*(vsx_avector<char>*)(handle->file_data)).push_back(0);
      archive_add_file(
        handle->filename,
        &((*((vsx_avector<char>*)(handle->file_data)))[0]),
        ((vsx_avector<char>*)(handle->file_data))->size()
      );
    }
  }
  delete handle;
}

int vsxf::f_puts(const char* buf, vsxf_handle* handle)
{
  if (!handle)
    return 0;

  if (type == VSXF_TYPE_FILESYSTEM)
    return fputs(buf,handle->file_handle);

  if (type != VSXF_TYPE_ARCHIVE)
    return 0;

  if (handle->mode != VSXF_MODE_WRITE)
    return 0;

  int i = 0;
  while (buf[i])
  {
    (*(vsx_avector<char>*)(handle->file_data))[handle->position = handle->size++] = buf[i++];
  }

  return 0;
}

unsigned long vsxf::f_get_size(vsxf_handle* handle)
{
  if (type == VSXF_TYPE_FILESYSTEM)
  {
    unsigned long size;
    fseek (handle->file_handle, 0, SEEK_END);
    size = ftell(handle->file_handle);
    rewind(handle->file_handle);
    return size;
  }
  return handle->size;
}

char* vsxf::f_gets_entire(vsxf_handle* handle)
{
  unsigned long size = f_get_size(handle);
  char* buf = (char*)malloc(size+1);
  if (buf)
  {
    f_read((void*)buf, size, handle);
    buf[size] = 0;
    return buf;
  }
  ERROR_EXIT("Error allocating memory",1);
}

char* vsxf::f_gets(char* buf, unsigned long max_buf_size, vsxf_handle* handle)
{
  if (type == VSXF_TYPE_FILESYSTEM)
    return fgets(buf, max_buf_size, handle->file_handle);

  unsigned long i = 0;
  bool run = true;

  while (handle->position < handle->size && i < max_buf_size && run)
  {
    if (((char*)handle->file_data)[handle->position] == 0x0A)
    {
      run = false;
    }
    buf[i] = ((char*)handle->file_data)[handle->position];
    ++i;
    ++handle->position;
  }

  if (handle->position < handle->size)
  {
    buf[i] = 0;
  }

  if (i != 0)
  {
    return buf;
  }

  return 0x0;
}

int vsxf::f_read(void* buf, unsigned long num_bytes, vsxf_handle* handle)
{
  if (type == VSXF_TYPE_FILESYSTEM)
    return fread(buf,1,num_bytes,handle->file_handle);

  char* fd = (char*)handle->file_data;

  if (fd == 0)
    return 0;

  if (handle->position + num_bytes > handle->size)
  {
    num_bytes = handle->size - handle->position;
  }
  fd += handle->position;
  memcpy(buf, fd, num_bytes);
  handle->position += num_bytes;
  return num_bytes;
}

void create_directory(char* path)
{
	char dir_name[4096];
	char* p = path;
	char* q = dir_name;
  while( *p )
	{
    if ( DIRECTORY_SEPARATOR_CHAR == *p )
		{
      if (':' != *( p-1 ) )
			{
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    	 mkdir(dir_name,0700);
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
    	 CreateDirectory(dir_name, NULL);
#endif
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
#if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
	mkdir(dir_name,0700);
#endif
#if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
	CreateDirectory(dir_name, NULL);
#endif
}

vsx_string get_path_from_filename(vsx_string filename)
{
	vsx_string deli = "/";
	std::vector<vsx_string> results;
	explode(filename, deli, results);
	results.pop_back();
	return implode(results, deli);
}


vsx_string vsx_get_data_path()
{
  vsx_string base_path;
  #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
    char* home_dir = getenv ("HOME");
    base_path = vsx_string(home_dir)+"/.local/share/vsxu/";

    if (access(base_path.c_str(),0) != 0)
    {
      mkdir( (base_path).c_str(), 0700 );
      int r = symlink ( (base_path).c_str(), (vsx_string(home_dir)+"/vsxu").c_str() );
      (void)r;
    }

    base_path = base_path + vsxu_ver + "/";

    if (access(base_path.c_str(),0) != 0)
      mkdir( (base_path).c_str(),0700);

    base_path = base_path+"data/";

    if (access(base_path.c_str(),0) != 0)
    {
      mkdir( (base_path).c_str(),0700);
      mkdir( (base_path+"animations").c_str(),0700);
      mkdir( (base_path+"macros").c_str(),0700);
      mkdir( (base_path+"prods").c_str(),0700);
      mkdir( (base_path+"screenshots").c_str(),0700);
      mkdir( (base_path+"states").c_str(),0700);
      mkdir( (base_path+"visuals").c_str(),0700);
      mkdir( (base_path+"visuals_faders").c_str(),0700);
      mkdir( (base_path+"resources").c_str(),0700);
      // add symlinks to examples

      int r;
      r = symlink ( (PLATFORM_SHARED_FILES+"example-macros").c_str(), (base_path+"macros/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-states").c_str(), (base_path+"states/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-prods").c_str(), (base_path+"prods/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-visuals").c_str(), (base_path+"visuals/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-resources").c_str(), (base_path+"resources/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"example-faders").c_str(), (base_path+"visuals_faders/examples").c_str() );
      r = symlink ( (PLATFORM_SHARED_FILES+"debug-states").c_str(), (base_path+"states/debug").c_str() );
      (void)r;
    }
  #else // platform family = unix
    char* home_dir = getenv ("USERPROFILE");
    base_path = vsx_string(home_dir)+"\\vsxu\\";
    if (access(base_path.c_str(),0) != 0) mkdir((base_path).c_str());
    base_path = base_path+vsxu_ver+"\\";
    if (access(base_path.c_str(),0) != 0) mkdir( (base_path).c_str());
    base_path = base_path+"data\\";
    if (access(base_path.c_str(),0) != 0)
    {
      printf("trying to create base_path: %s\n",base_path.c_str());
      mkdir( (base_path).c_str());
      mkdir( (base_path+"animations").c_str());
      mkdir( (base_path+"macros").c_str());
      mkdir( (base_path+"prods").c_str());
      mkdir( (base_path+"screenshots").c_str());
      mkdir( (base_path+"states").c_str());
      mkdir( (base_path+"visuals").c_str());
      mkdir( (base_path+"visuals_faders").c_str());
      mkdir( (base_path+"resources").c_str());

      #ifdef VSXU_DEBUG
        //printf("xcopy command: %s\n",vsx_string("xcopy /E data "+base_path).c_str());
      printf("xcopy command: %s\n",vsx_string("xcopy /E \""+PLATFORM_SHARED_FILES+"\\example-macros\\\" "+base_path+"").c_str());
      #endif

      printf("%s",(vsx_string("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-macros\" \""+base_path+"macros\\examples\"").c_str());
      system((vsx_string("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-macros\" \""+base_path+"macros\\examples\"").c_str());
      system((vsx_string("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-states\" \""+base_path+"states\\examples\"").c_str());
      system((vsx_string("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-prods\" \""+base_path+"prods\\examples\"").c_str());
      system((vsx_string("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-visuals\" \""+base_path+"visuals\\examples\"").c_str());
      system((vsx_string("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-resources\" \""+base_path+"resources\\examples\"").c_str());
      system((vsx_string("xcopy /I /E \"")+PLATFORM_SHARED_FILES+"example-faders\" \""+base_path+"visuals_faders\\examples\"").c_str());
    }
  #endif
  return base_path;
}

vsx_string vsx_get_directory_separator()
{
  return vsx_string(DIRECTORY_SEPARATOR);
}
