/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "Common/MyWindows.h"
#include "Common/MyInitGuid.h"

#include <stdio.h>

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


vsxf::vsxf() {
  type = VSXF_TYPE_FILESYSTEM;
  archive_handle = 0;
}

void vsxf::set_base_path(vsx_string new_base_path)
{
  base_path = new_base_path;
}

void vsxf::archive_create(const char* filename) {
#ifndef VSXF_DEMO
  archive_name = filename;
  type = VSXF_TYPE_ARCHIVE;
  archive_handle = fopen(filename,"wb");
  const char header[5] = "VSXz";
  fwrite(header,sizeof(char),4,archive_handle);
#endif
}

vsx_avector<vsxf_archive_info>* vsxf::get_archive_files()
{
  return &archive_files;
}

void vsxf::archive_close() {
  if (type == VSXF_TYPE_ARCHIVE)
  {
    archive_name = "";
    if (archive_handle) {
      fclose(archive_handle);
      archive_handle = 0;
    }
    type = VSXF_TYPE_FILESYSTEM;
    archive_files.clear();
  }
}

  int vsxf::archive_add_file(
                             vsx_string filename, 
                             char* data, 
                             uint32_t data_size, 
                             vsx_string disk_filename
  ) {
#ifndef VSXF_DEMO
    if (!archive_handle) return 1;
    unsigned long i = 0;
    while (i < archive_files.size()) {
      if (archive_files[i].filename == filename) {
        return 1;
      }
      ++i;
    }
    vsx_string fopen_filename = filename;
    if (disk_filename != "") fopen_filename = disk_filename;
    printf("vsxz adding file: %s\n", fopen_filename.c_str());
    FILE* fp = 0;
    if (data == 0) {
      fp = fopen(fopen_filename.c_str(),"rb");
      if (fp) {
        fseek (fp, 0, SEEK_END);
        data_size = ftell(fp);
        fseek(fp,0,SEEK_SET);
        data = new char[data_size];
        if (!fread(data,sizeof(char),data_size,fp)) { return 2;};
        // write it to disk
        fseek(archive_handle,0,SEEK_END);
      }
    }
    fseek(archive_handle,0,SEEK_END);
    // time to allocate ram for compression
    UInt32 dictionary = 1 << 21;
    size_t outSize = (size_t)data_size / 20 * 21 + (1 << 16);
    size_t outSizeProcessed;
    Byte *outBuffer = 0;
    if (outSize != 0) outBuffer = (Byte *)MyAlloc(outSize);
    //dictionary = 1 << 21;
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


    if (fp) {
      delete data;
      fclose(fp);
    }
#endif
    return 0;
  }

  int vsxf::archive_load(const char* filename) {
    // precaution, we don't wanna waste RAM
    if (type == VSXF_TYPE_ARCHIVE) archive_close();
    archive_name = filename;
    archive_handle = fopen(filename,"rb");
    if (!archive_handle) {
      printf("error! \"%s\" is not a valid file handle!\n",filename);
      return 0;
    }
    fseek (archive_handle, 0, SEEK_END);
    unsigned long size = ftell(archive_handle);
    fseek(archive_handle,0,SEEK_SET);
    if (size < 4) return 1;
    char header[5];
    header[4] = 0;
    if (!fread(header,sizeof(char),4,archive_handle)) return 2;
    //printf("header: %s\n",header);
    vsx_string hs(header);
    if (hs != "VSXz") return 2;
    //printf("archive seems ok...\n");
    //printf("binary pos is: %d\n",ftell(archive_handle));
    while (fread(&size,sizeof(uint32_t),1,archive_handle) != 0) {
      //char filebuf[4096];
      char aa;
      //int pos = 0;
      vsx_string filebuf_name;
      while ((aa = fgetc(archive_handle)) != 0)  filebuf_name.push_back(aa);
      aa = fgetc(archive_handle);
      size -= filebuf_name.size()+1;
      //printf("filename stored is: %s\n",filebuf_name.c_str());

      //printf("binary pos is: %d\n",ftell(archive_handle));
      //printf("binary size is: %d\n",size);

      // save info about this file so we can open it later
      vsxf_archive_info finfo;
      finfo.filename = filebuf_name;
      finfo.position = ftell(archive_handle);
      finfo.size = size;
      archive_files.push_back(finfo);

      fseek(archive_handle,size-1,SEEK_CUR);
    }
    fclose(archive_handle);
    archive_handle = 0;
    type = VSXF_TYPE_ARCHIVE;
    return 1;
  };

bool vsxf::is_archive()
{
  return (type == VSXF_TYPE_ARCHIVE);
}

bool vsxf::is_archive_populated()
{
  return (archive_files.size() > 0);
}

  vsxf_handle* vsxf::f_open(const char* filename, const char* mode) {
    vsxf_handle* handle = new vsxf_handle;
    // 1.  are we archive or filesystem?
    // 1a. archive:
    //     find the file location in the disk archive in the archive specification
    //     load/decompress filename from disk into ram
    // 1b. file:
    //     get a file descriptor from disk
    if (type == VSXF_TYPE_FILESYSTEM) {
    	vsx_string i_filename(filename);
      #if PLATFORM_FAMILY == PLATFORM_FAMILY_UNIX
        i_filename = str_replace("\\","/",i_filename);
      #endif
      #if PLATFORM_FAMILY == PLATFORM_FAMILY_WINDOWS
        i_filename = str_replace("/","\\",i_filename);
      #endif
      #ifdef VSXU_DEBUG
        printf("vsxf::f_open %s base_path: %s\n\n", filename,base_path.c_str() );
      #endif
      handle->file_handle = fopen((base_path+i_filename).c_str(),mode);
      if (handle->file_handle == NULL) {
        delete handle;
        return NULL;
      } else
      return handle;
    } else {
      FILE* l_handle = fopen(archive_name.c_str(),"rb");

      vsx_string mode_search(mode);
      if (mode_search.find("r") != -1) {
        bool found = false;
        vsx_string fname(filename);
        unsigned long i = 0;

        while (!found && i < archive_files.size()) {
          //printf("trying file: %s\n",archive_files[i].filename.c_str());
          if (archive_files[i].filename == fname) {
            found = true;
            handle = new vsxf_handle;
            handle->filename = fname;
            handle->position = 0;
            handle->size = archive_files[i].size;
            handle->mode = VSXF_MODE_READ;
            // decompress the data into the filehandle
            //printf("nhandle size: %d\n",handle->size);
            void* inBuffer = malloc(handle->size);
            fseek(l_handle,archive_files[i].position-1,SEEK_SET);
            if (!fread(inBuffer,1,handle->size,l_handle))
            {
              delete handle;
              return NULL;
            }
            void* outBuffer = 0;
            size_t outSize;
            size_t outSizeProcessed;
            if (LzmaRamGetUncompressedSize((unsigned char*)inBuffer, archive_files[i].size, &outSize) != 0)
            {
              printf("vsxf: lzma data error!");
            }
            if (outSize != 0)
            {
              outBuffer = malloc(outSize);
            }
            handle->file_data = 0;
            if (outBuffer != 0)
            {
              LzmaRamDecompress((unsigned char*)inBuffer, archive_files[i].size, (unsigned char*)outBuffer, outSize, &outSizeProcessed, malloc, free);
              handle->size = outSizeProcessed;
              //printf("decompressed %d into %d bytes\n",handle->size, outSizeProcessed);
              handle->file_data = outBuffer;
              //printf("byte %s\n",outBuffer);
              //FILE* ff = fmemopen (void *outBuffer, outSize, "r");
            }
            free(inBuffer);
            fclose(l_handle);
            return handle;
          }
          ++i;
        }
      } else
      if (mode_search.find("w") != -1) {
        handle->position = 0;
        handle->size = 0;
        handle->file_data = (void*)(new vsx_avector<char>);
        handle->filename = filename;
        handle->mode = VSXF_MODE_WRITE;
        return handle;
      }
      delete handle;
      return 0;
    }
    delete handle;
    return 0;
  }

  void vsxf::f_close(vsxf_handle* handle) {
    if (handle) {
      if (type == VSXF_TYPE_FILESYSTEM) fclose(handle->file_handle);
      if (type == VSXF_TYPE_ARCHIVE) {
        if (handle->mode == VSXF_MODE_WRITE) {
          (*(vsx_avector<char>*)(handle->file_data)).push_back(0);
          archive_add_file(handle->filename, &((*((vsx_avector<char>*)(handle->file_data)))[0]),((vsx_avector<char>*)(handle->file_data))->size());
        }
      }
      delete handle;
    }
  }

  int vsxf::f_puts(const char* buf, vsxf_handle* handle) {
#ifndef VSXF_DEMO
    //printf("fputs: %s\n",buf);
    if (!handle) return 0;
    if (type == VSXF_TYPE_FILESYSTEM) {
      return fputs(buf,handle->file_handle);
    }
    if (type == VSXF_TYPE_ARCHIVE) {
      //int run = 1;
      int i = 0;
      if (handle->mode == VSXF_MODE_WRITE) {
        while (buf[i]) {
          //printf("i: %d\n",handle->position);
          (*(vsx_avector<char>*)(handle->file_data))[handle->position = handle->size++] = buf[i++];
          /*
          char aa[2];
          aa[0] = (*(vsx_avector<char>*)(handle->file_data))[handle->position];
          aa[1] = 0;
          printf("char added: %s",aa);
          */
        } // while
      } // if
    }
#endif
		return 0;
  }

  unsigned long vsxf::f_get_size(vsxf_handle* handle) {
    if (type == VSXF_TYPE_FILESYSTEM) {
      unsigned long size;
      fseek (handle->file_handle, 0, SEEK_END);
      size = ftell(handle->file_handle);
      rewind(handle->file_handle);
      return size;
    } else {
      return handle->size;
    }
  }

  char* vsxf::f_gets_entire(vsxf_handle* handle) {
    unsigned long size = f_get_size(handle);
    char* buf = (char*)malloc(size+1);
    if (buf) {
      f_read((void*)buf, size, handle);
      //printf("size aquired: %d\n",read_size);
      buf[size] = 0;
      return buf;
    } else return 0;
  }

  char* vsxf::f_gets(char* buf, unsigned long max_buf_size, vsxf_handle* handle) {
    //printf("f_gets\n");
    if (type == VSXF_TYPE_FILESYSTEM) {
      return fgets(buf, max_buf_size, handle->file_handle);
    } else {
      unsigned long i = 0;
      bool run = true;
      //printf("handle->position: %d\n",handle->position);
      //printf("handle->size: %d\n",handle->size);
      while (handle->position < handle->size && i < max_buf_size && run) {
        if (((char*)handle->file_data)[handle->position] == 0x0A) {
          run = false;
        }
        buf[i] = ((char*)handle->file_data)[handle->position];
        ++i;
        ++handle->position;
      }
      if (handle->position < handle->size) {
        buf[i] = 0;
      }
      if (i != 0) {
        return buf;
      }
      return (char*)0;
    }
  }

  int vsxf::f_read(void* buf, unsigned long num_bytes, vsxf_handle* handle) {
    if (type == VSXF_TYPE_FILESYSTEM) {
      //printf("reading up to %d bytes\n",num_bytes);
      //printf("stream pos: %d\n",ftell(handle->file_handle));
      unsigned long read_bytes = fread(buf,1,num_bytes,handle->file_handle);
      //printf("ferror was: %d\n",ferror(handle->file_handle));
      return read_bytes;
    } else {
      char* fd = (char*)handle->file_data;
      if (fd == 0) return 0;
      if (handle->position + num_bytes > handle->size) {
        num_bytes = handle->size - handle->position;
      }
      fd+=handle->position;
      memcpy(buf,fd,num_bytes);
      handle->position += num_bytes;
      return num_bytes;
    }
  }

// OTHER FUNCTIONS

void create_directory(char* path)
{
	char dir_name[4096];
	char* p = path;
	char* q = dir_name;
	while(*p)
	{
		if (('\\' == *p) || ('/' == *p))
		{
			if (':' != *(p-1))
			{
#ifdef __linux__
    	 mkdir(dir_name,0700);
#endif
#if defined(_WIN32)
    	 CreateDirectory(dir_name, NULL);
#endif
			}
		}
		*q++ = *p++;
		*q = '\0';
	}
#ifdef __linux__
	mkdir(dir_name,0700);
#endif
#if defined(_WIN32)
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
    //struct stat st;
    char* home_dir = getenv ("HOME");
    base_path = vsx_string(home_dir)+"/.vsxu/";
    if (access(base_path.c_str(),0) != 0) mkdir( (base_path).c_str(),0700);
    base_path = base_path+vsxu_ver+"/";
    if (access(base_path.c_str(),0) != 0) mkdir( (base_path).c_str(),0700);
    base_path = base_path+"data/";
    #ifdef VSXU_DEBUG
      printf("base path: %s\n", base_path.c_str() );
    #endif
    if (access(base_path.c_str(),0) != 0)
    {
      mkdir( (base_path).c_str(),0700);
      mkdir( (base_path+"macros").c_str(),0700);
      mkdir( (base_path+"states").c_str(),0700);
      mkdir( (base_path+"prods").c_str(),0700);
      mkdir( (base_path+"visuals").c_str(),0700);
      mkdir( (base_path+"visuals_faders").c_str(),0700);
      mkdir( (base_path+"resources").c_str(),0700);
      // add symlinks to examples

      symlink ( (PLATFORM_SHARED_FILES+"example-macros").c_str(), (base_path+"macros/examples").c_str() );
      symlink ( (PLATFORM_SHARED_FILES+"example-states").c_str(), (base_path+"states/examples").c_str() );
      symlink ( (PLATFORM_SHARED_FILES+"example-prods").c_str(), (base_path+"prods/examples").c_str() );
      symlink ( (PLATFORM_SHARED_FILES+"example-visuals").c_str(), (base_path+"visuals/examples").c_str() );
      symlink ( (PLATFORM_SHARED_FILES+"example-resources").c_str(), (base_path+"resources/examples").c_str() );
      symlink ( (PLATFORM_SHARED_FILES+"example-faders").c_str(), (base_path+"visual_faders/examples").c_str() );
      #if (VSXU_DEBUG)
        symlink ( (PLATFORM_SHARED_FILES+"debug-states").c_str(), (base_path+"states/debug").c_str() );
      #endif
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
      mkdir( (base_path+"macros").c_str());
      mkdir( (base_path+"states").c_str());
      mkdir( (base_path+"prods").c_str());
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
