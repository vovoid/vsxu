#pragma once


#include <container/vsx_nw_vector.h>
#include <string/vsx_string_helper.h>
#include "vsx_filesystem_tree_node.h"

class vsx_filesystem_tree_writer
{
  friend class vsx_filesystem_tree_serialize_string;
  friend class vsx_filesystem_tree_serialize_binary;

  vsx_filesystem_tree_node root_node;

  void add_file_internal(vsx_filesystem_tree_node* node, vsx_nw_vector< vsx_string<> > &parts, size_t index, uint32_t payload)
  {
    if (index > parts.size() - 1 )
      return;

    vsx_filesystem_tree_node* n = node->get_create(parts[index], payload);
    add_file_internal(n, parts, index + 1, payload);
  }

public:

  void add_file(vsx_string<> filename, uint32_t payload)
  {
    vsx_nw_vector< vsx_string<> > parts;
    vsx_string_helper::explode_single<char>(filename, '/', parts);
    add_file_internal(&root_node, parts, 0, payload);
  }

  void calculate_offsets()
  {
    uint16_t offset = 0;
    root_node.calculate_offsets(offset);
  }
};
