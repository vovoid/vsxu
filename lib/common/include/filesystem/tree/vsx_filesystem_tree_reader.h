#pragma once

#include <container/vsx_ma_vector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>

class vsx_filesystem_tree_reader
{
  unsigned char* data_pointer = 0x0;

  void get_filename_payload_list_internal(vsx_nw_vector<vsx_string<>>& filenames, vsx_nw_vector<uint32_t>& payloads, unsigned char* start_point, vsx_string<> directory_base)
  {
    unsigned char* p = start_point;
    while (true)
    {
      if (!*p)
        break;

      vsx_string<> current_name;
      unsigned char string_length = *p & 0x7F;
      unsigned char is_dir = *p & 0x80;
      p++;
      uint16_t children_offset = 0;
      if (is_dir)
      {
        children_offset = ((uint16_t)*p) << 8;
        p++;
        children_offset += *p;
        p++;
      }

      for (size_t i = 0; i < string_length; i++)
      {
        current_name.push_back( *p );
        p++;
      }

      if (is_dir)
      {
        get_filename_payload_list_internal(filenames, payloads, data_pointer + children_offset, (directory_base.size()?directory_base + "/":vsx_string<>()) + current_name);
        continue;
      }

      filenames.push_back((directory_base.size()?directory_base + "/":vsx_string<>()) + current_name);

      uint32_t payload = ((uint32_t)(*p)) << 24;
      p++;
      payload += ((uint32_t)(*p)) << 16;
      p++;
      payload += ((uint32_t)(*p)) << 8;
      p++;
      payload += *p;
      p++;
      payloads.push_back(payload);
    }
  }

public:

  void initialize(unsigned char* p)
  {
    data_pointer = p;
  }

  void get_filename_payload_list(vsx_nw_vector<vsx_string<>>& filenames, vsx_nw_vector<uint32_t>& payloads)
  {
    get_filename_payload_list_internal(filenames, payloads, data_pointer, "");
  }

  uint32_t get_payload_by_filename(vsx_string<> filename)
  {
    if (!data_pointer)
      return 0;

    vsx_nw_vector<vsx_string<> > filename_parts;
    vsx_string_helper::explode_single(filename, '/', filename_parts);
    size_t filename_iterator = 0;
    unsigned char* p = data_pointer;
    while (true)
    {
      if (filename_iterator > filename_parts.size() - 1)
        return 0;

      if (!*p)
        return 0;

      unsigned char string_length = *p & 0x7F;
      unsigned char is_dir = *p & 0x80;
      p++;
      uint16_t children_offset = 0;
      if (is_dir)
      {
        children_offset = ((uint16_t)*p) << 8;
        p++;
        children_offset += *p;
        p++;
      }

      // compare string
      if (filename_parts[filename_iterator].size() == string_length)
      {
        char* cur_filename = filename_parts[filename_iterator].get_pointer();
        while (string_length)
        {
          if ( ((char)*p) != *cur_filename )
            break;
          p++;
          cur_filename++;
          string_length--;
        }
      }

      // jump to next
      if (string_length)
      {
        p += string_length;
        if (!is_dir)
          p += sizeof(uint32_t);
        continue;
      }

      // jump to next level
      if (is_dir)
      {
        p = data_pointer + children_offset;
        filename_iterator++;
        continue;
      }

      uint32_t result = ((uint32_t)(*p)) << 24;
      p++;
      result += ((uint32_t)(*p)) << 16;
      p++;
      result += ((uint32_t)(*p)) << 8;
      p++;
      result += *p;
      return result;
    }
    return 0;
  }
};
