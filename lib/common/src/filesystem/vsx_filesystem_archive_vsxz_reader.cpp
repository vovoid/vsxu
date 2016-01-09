#include <filesystem/archive/vsx_filesystem_archive_vsxz_reader.h>
#include <tools/vsx_thread_pool.h>
#include <vsx_compression_lzma.h>

namespace vsx
{


void filesystem_archive_vsxz_reader::load_worker( vsx_ma_vector<uncompress_info>* uncompress_work_list, size_t start_index, size_t end_index )
{
  for (size_t i = start_index; i < end_index; i++)
  {
    vsx_ma_vector<unsigned char> compressed_data;
    compressed_data.set_volatile();
    compressed_data.set_data(
      (*uncompress_work_list)[i].compressed_data,
      (*uncompress_work_list)[i].file_info->compressed_size
    );

    vsx_ma_vector<unsigned char> uncompressed_data;
    uncompressed_data.set_volatile();
    uncompressed_data.set_data(
      (*uncompress_work_list)[i].uncompressed_data,
      (*uncompress_work_list)[i].file_info->uncompressed_size
    );
    compression_lzma::uncompress(uncompressed_data, compressed_data);
  }
}

bool filesystem_archive_vsxz_reader::load(const char* archive_filename, bool load_data_multithreaded)
{
  mmap = filesystem_mmap::create(archive_filename);
  req_v(mmap, false);
  req_v(mmap->size > sizeof(vsxz_header) + sizeof(vsxz_header_file_info), false);
  header = (vsxz_header*)mmap->data;

  req_v(header->identifier[0] == 'V', false);
  req_v(header->identifier[1] == 'S', false);
  req_v(header->identifier[2] == 'X', false);
  req_v(header->identifier[3] == 'Z', false);

  req_v(header->compressed_uncompressed_data_size, false);
  uncompressed_data.allocate(header->compressed_uncompressed_data_size - 1);


  vsx_ma_vector<uncompress_info> uncompress_work_list;
  req_v(header->file_count_compressed, false);
  uncompress_work_list.allocate(header->file_count_compressed - 1);

  unsigned char* current_file_table_pos = mmap->data + sizeof(vsxz_header);
  unsigned char* compressed_data_base_pos = current_file_table_pos + header->file_info_table_size;
  size_t work_list_index = 0;
  uint32_t compressed_data_offset = 0;
  uint32_t uncompressed_data_offset = 0;
  for_n (header->file_count, i)
  {
    vsxz_header_file_info* info = (vsxz_header_file_info*)current_file_table_pos;
    if (info->compression_type)
    {
      uncompress_work_list[work_list_index].compressed_data = compressed_data_base_pos + compressed_data_offset;
      uncompress_work_list[work_list_index].uncompressed_data = uncompressed_data.get_pointer() + uncompressed_data_offset;
      uncompress_work_list[work_list_index].file_info = info;
      work_list_index++;
      uncompressed_data_offset += info->uncompressed_size;
    }
    compressed_data_offset += info->compressed_size;
    current_file_table_pos += info->filename_size + sizeof(vsxz_header_file_info);
  }

  uint32_t data_aggregation = 0;
  size_t prev_index = 0;
  size_t i = 0;
  for(; i < uncompress_work_list.size(); i++)
  {
    data_aggregation += uncompress_work_list[i].file_info->uncompressed_size;
    req_continue(data_aggregation > 1024*1024);
    data_aggregation = 0;

    vsx_thread_pool::instance()->add(
      [=]
      (
        vsx_ma_vector<uncompress_info>* uncompress_work_list,
        size_t start_index,
        size_t end_index
      )
      {
        load_worker(uncompress_work_list, start_index, end_index);
      },
      &uncompress_work_list,
      prev_index,
      i
    );
    prev_index = i;
  }

  if (data_aggregation)
    vsx_thread_pool::instance()->add(
      [=]
      (
        vsx_ma_vector<uncompress_info>* uncompress_work_list,
        size_t start_index,
        size_t end_index
      )
      {
        load_worker(uncompress_work_list, start_index, end_index);
      },
      &uncompress_work_list,
      prev_index,
      i
    );


  vsx_thread_pool::instance()->wait_all();

  return true;
}

void filesystem_archive_vsxz_reader::file_open(const char* filename, file* &handle)
{
  uint32_t compressed_offset = 0;
  uint32_t uncompressed_offset = 0;
  vsxz_header_file_info* file_info = get_file_info(filename, compressed_offset, uncompressed_offset);
  req(file_info);
  handle->data.set_volatile();
  if (file_info->compression_type)
  {
    handle->data.set_volatile();
    handle->data.set_data(uncompressed_data.get_pointer() + uncompressed_offset, file_info->uncompressed_size);
    return;
  }
  unsigned char* data_pos = mmap->data + sizeof(vsxz_header) + header->file_info_table_size + compressed_offset;
  handle->data.set_data(data_pos, file_info->uncompressed_size);
}

void filesystem_archive_vsxz_reader::close()
{
  filesystem_mmap::destroy(mmap);
  header = 0x0;
  uncompressed_data.clear();
}


bool filesystem_archive_vsxz_reader::is_archive()
{
  return header != 0x0;
}


bool filesystem_archive_vsxz_reader::is_archive_populated()
{
  req_v(header, false);
  return header->file_count > 0;
}

vsxz_header_file_info* filesystem_archive_vsxz_reader::get_file_info(const char* filename, uint32_t &compressed_offset, uint32_t &uncompressed_offset)
{
  unsigned char* current_pos = mmap->data + sizeof(vsxz_header);
  for_n(header->file_count, i)
  {
    vsxz_header_file_info* info = (vsxz_header_file_info*)current_pos;
    current_pos += sizeof(vsxz_header_file_info);
    size_t matching_characters = 0;
    size_t fi = 0;
    size_t filename_length = strlen(filename);
    for(; (fi < info->filename_size) && (fi < filename_length); fi++)
    {
      char current_char = (char)*(current_pos);

      if (current_char != filename[fi])
        break;

      matching_characters++;
      current_pos++;
    }

    if (matching_characters == info->filename_size)
      return info;

    compressed_offset += info->compressed_size;
    if (info->compression_type)
      uncompressed_offset += info->uncompressed_size;

    // next file
    current_pos += info->filename_size - fi;
  }

  return 0x0;
}

bool filesystem_archive_vsxz_reader::is_file(vsx_string<> filename)
{
  uint32_t compressed_offset = 0;
  uint32_t uncompressed_offset = 0;
  return get_file_info(filename.c_str(), compressed_offset, uncompressed_offset) != 0x0;
}


vsx_nw_vector<filesystem_archive_file_read>* filesystem_archive_vsxz_reader::files_get()
{
  return 0;
}



}

