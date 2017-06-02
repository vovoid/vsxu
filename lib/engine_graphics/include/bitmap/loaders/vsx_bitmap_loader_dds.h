#pragma once

#include "vsx_bitmap_loader_base.h"
#include <bitmap/vsx_bitmap_transform.h>
#include <tools/vsx_singleton_counter.h>
#include <string/vsx_string_helper.h>

// original example code kindly supplied by Jon Watte

//  little-endian, of course
#define DDS_MAGIC 0x20534444

//  DDS_header.dwFlags
#define DDSD_CAPS                   0x00000001
#define DDSD_HEIGHT                 0x00000002
#define DDSD_WIDTH                  0x00000004
#define DDSD_PITCH                  0x00000008
#define DDSD_PIXELFORMAT            0x00001000
#define DDSD_MIPMAPCOUNT            0x00020000
#define DDSD_LINEARSIZE             0x00080000
#define DDSD_DEPTH                  0x00800000

//  DDS_header.sPixelFormat.dwFlags
#define DDPF_ALPHAPIXELS            0x00000001
#define DDPF_FOURCC                 0x00000004
#define DDPF_INDEXED                0x00000020
#define DDPF_RGB                    0x00000040

//  DDS_header.sCaps.dwCaps1
#define DDSCAPS_COMPLEX             0x00000008
#define DDSCAPS_TEXTURE             0x00001000
#define DDSCAPS_MIPMAP              0x00400000

//  DDS_header.sCaps.dwCaps2
#define DDSCAPS2_CUBEMAP            0x00000200
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000
#define DDSCAPS2_VOLUME             0x00200000

#define D3DFMT_DXT1 0x31545844
#define D3DFMT_DXT3 0x33545844
#define D3DFMT_DXT5 0x35545844

#define PF_IS_DXT1(pf) \
  ((pf.flags & DDPF_FOURCC) && \
   (pf.four_cc == D3DFMT_DXT1))

#define PF_IS_DXT3(pf) \
  ((pf.flags & DDPF_FOURCC) && \
   (pf.four_cc == D3DFMT_DXT3))

#define PF_IS_DXT5(pf) \
  ((pf.flags & DDPF_FOURCC) && \
   (pf.four_cc == D3DFMT_DXT5))


class vsx_bitmap_loader_dds
    : public vsx_bitmap_loader_base
{

  VSX_PACK_BEGIN
  struct dds_header
  {
    uint32_t    magic;
    uint32_t    size;
    uint32_t    flags;
    uint32_t    height;
    uint32_t    width;
    uint32_t    pitch_or_linear_size;
    uint32_t    depth;
    uint32_t    mip_map_count;
    uint32_t    reserved_1[ 11 ];

    struct
    {
      uint32_t    size;
      uint32_t    flags;
      uint32_t    four_cc;
      uint32_t    rgb_bit_count;
      uint32_t    r_bit_mask;
      uint32_t    g_bit_mask;
      uint32_t    b_bit_mask;
      uint32_t    alpha_bit_mask;
    } pixel_format;

    struct
    {
      uint32_t    caps_1;
      uint32_t    caps_2;
      uint32_t    dds_x;
      uint32_t    reserved;
    } caps;

    uint32_t    reserved_2;
  }
  VSX_PACK_END

  static bool worker_load_file(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx::file* file_handle, size_t cube_map_side)
  {
    req_error_v(file_handle, "file handle is null", 0);

    dds_header header;
    filesystem->f_read(&header, sizeof(header), file_handle);

    req_error_v(header.magic == DDS_MAGIC, "File does not start with \"DDS \"", false);
    req_error_v(header.size == 124, "Wrong header size", false);

    bitmap->compression = PF_IS_DXT1(header.pixel_format) ? vsx_bitmap::compression_dxt1 : vsx_bitmap::compression_none;

    if (bitmap->compression == vsx_bitmap::compression_none)
      bitmap->compression = PF_IS_DXT3(header.pixel_format) ? vsx_bitmap::compression_dxt3 : vsx_bitmap::compression_none;

    if (bitmap->compression == vsx_bitmap::compression_none)
      bitmap->compression = PF_IS_DXT5(header.pixel_format) ? vsx_bitmap::compression_dxt5 : vsx_bitmap::compression_none;

    req_error_v(bitmap->compression != vsx_bitmap::compression_none, "DDS loader only supports DXT1 or DXT3 or DXT5 compressed formats.", false);
    req_error_v(header.flags & DDSD_LINEARSIZE, "linear size", false);

    size_t bytes_per_block = 16;
    if (bitmap->compression == vsx_bitmap::compression_dxt1)
      bytes_per_block = 8;

    bitmap->width = header.width;
    bitmap->height = header.height;
    bitmap->channels = 4;

//    size_t row_size =
//        MAX(1, (header.width + 3) / 4) *
//        MAX(1, (header.height + 3) / 4) *
//        bytes_per_block
//      ;

    //if (row_size != header.pitch_or_linear_size)
    //  vsx_printf(L"WARNING!!!    DDS pitch or linear size wrong,      %hs\n", bitmap->filename.c_str());

    unsigned int x = header.width;
    unsigned int y = header.height;
    for (unsigned int mip_map_level = 0; mip_map_level < header.mip_map_count; ++mip_map_level)
    {
      size_t size = MAX(1, (x + 3) / 4) *  (MAX(1, (y + 3) / 4)) * bytes_per_block;

      bitmap->data_set(malloc(size), mip_map_level, cube_map_side, size);
      filesystem->f_read(bitmap->data_get(mip_map_level, cube_map_side), size, file_handle);

      x /= 2;
      y /= 2;
      if (!x)
        x = 1;
      if (!y)
        y = 1;
    }
    return true;
  }

  static bool worker_load_file_archive(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx::file* file_handle, size_t cube_map_side)
  {
    req_error_v(file_handle, "file handle is null", 0);

    size_t file_index = 0;
    dds_header* header;
    unsigned char* data = filesystem->f_data_get(file_handle);
    header = (dds_header*)data;
    file_index += sizeof(dds_header);

    req_error_v(header->magic == DDS_MAGIC, "File does not start with \"DDS \"", false);
    req_error_v(header->size == 124, "Wrong header size", false);

    bitmap->compression = PF_IS_DXT1(header->pixel_format) ? vsx_bitmap::compression_dxt1 : vsx_bitmap::compression_none;

    if (bitmap->compression == vsx_bitmap::compression_none)
      bitmap->compression = PF_IS_DXT3(header->pixel_format) ? vsx_bitmap::compression_dxt3 : vsx_bitmap::compression_none;

    if (bitmap->compression == vsx_bitmap::compression_none)
      bitmap->compression = PF_IS_DXT5(header->pixel_format) ? vsx_bitmap::compression_dxt5 : vsx_bitmap::compression_none;

    req_error_v( bitmap->compression != vsx_bitmap::compression_none, "DDS loader only supports DXT1 or DXT3 or DXT5 compressed formats.", false);
    req_error_v( header->flags & DDSD_LINEARSIZE, "linear size", false);

    size_t bytes_per_block = 16;
    if (bitmap->compression == vsx_bitmap::compression_dxt1)
      bytes_per_block = 8;

    bitmap->width = header->width;
    bitmap->height = header->height;
    bitmap->channels = 4;

    /*size_t row_size =
        MAX(1, (header->width + 3) / 4) *
        MAX(1, (header->height + 3) / 4) *
        bytes_per_block
      ;
    */
    /*req_error_v(
        row_size == header->pitch_or_linear_size,
        (
          vsx_string<>("size differes from pitch or linear size, header pitch:") +
          vsx_string_helper::i2s(header->pitch_or_linear_size) + " row size: " +
          vsx_string_helper::st2s(row_size)
        ).c_str()
        ,
        false
      );*/

    bitmap->data_mark_volatile();
    unsigned int x = header->width;
    unsigned int y = header->height;
    for( unsigned int mip_map_level = 0; mip_map_level < header->mip_map_count; ++mip_map_level )
    {
      size_t size = MAX( 1, (x + 3) / 4 ) *  (MAX( 1, (y + 3) / 4 )) * bytes_per_block;
      
      bitmap->data_set( data + file_index, mip_map_level, cube_map_side, size );
      file_index += size;
 
      x /= 2;
      y /= 2;
      if (!x)
        x = 1;
      if (!y)
        y = 1;
    }
    return true;
  }

  static void* worker(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename)
  {
    vsx::file* file_handle = filesystem->f_open(filename.c_str());
    if (!file_handle)
    {
      vsx_printf(L"%hs\n", vsx_string<>( ("error loading DDS texture: ") + filename).c_str());
      return 0;
    }

    bitmap->filename = filename;
    if (filesystem->get_archive() && filesystem->get_archive()->is_archive())
      worker_load_file_archive(bitmap, filesystem, file_handle, 0);
    else
      worker_load_file(bitmap, filesystem, file_handle, 0);
    filesystem->f_close(file_handle);


    if (bitmap->hint & vsx_bitmap::cubemap_load_files_hint)
      for (size_t i = 1; i < 6; i++)
      {
        vsx_string<> new_filename = filename;
        new_filename.replace("_0", "_"+vsx_string_helper::i2s((int)i));
        vsx::file* file_handle = filesystem->f_open(new_filename.c_str());
        worker_load_file(bitmap, filesystem, file_handle, i);
        filesystem->f_close(file_handle);
      }

    bitmap->timestamp = vsx_singleton_counter::get();
    bitmap->data_ready.fetch_add(1);

    return 0;
  }

  void load_internal(vsx_string<> filename, vsx::filesystem* filesystem, vsx_bitmap* bitmap, bool thread)
  {
    if (!thread)
      return (void)worker(bitmap, filesystem, filename);

    vsx_thread_pool::instance()->add(
      [=]
      (vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename)
      {
        worker(bitmap, filesystem, filename);
      },
      bitmap,
      filesystem,
      filename
    );
  }

public:

  static vsx_bitmap_loader_dds* get_instance()
  {
    static vsx_bitmap_loader_dds vtlj;
    return &vtlj;
  }

};
