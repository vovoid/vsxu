// 1 bit = directory or file
// 7 bits = filename length
// 16 bits = jump to next block
// ...filename...
// 32 bits = payload

#pragma once

#include "vsx_filesystem_tree_writer.h"

class vsx_filesystem_tree_serialize_binary
{
  static void serialize_node(vsx_filesystem_tree_node* node, vsx_ma_vector<unsigned char>& result)
  {
    foreach (node->children, i)
    {
      vsx_filesystem_tree_node* cur_node = node->children[i];

      result.push_back(
          (unsigned char)cur_node->name.size()
          |
          (cur_node->children.size()?1:0) << 7
        );

      if (cur_node->children.size())
      {
        result.push_back( (0xff00 & cur_node->offset) >> 8 );
        result.push_back( 0x00ff & cur_node->offset);
      }

      foreach (cur_node->name, si)
        result.push_back( (unsigned char) cur_node->name[si]);

      if (!cur_node->children.size())
      {
        result.push_back( (unsigned char)((0xff000000 & cur_node->payload) >> 24) );
        result.push_back( (unsigned char)((0x00ff0000 & cur_node->payload) >> 16) );
        result.push_back( (unsigned char)((0x0000ff00 & cur_node->payload) >> 8) );
        result.push_back( (unsigned char)(0x000000ff & cur_node->payload) );
      }
    }

    result.push_back(0);

    foreach (node->children, i)
      serialize_node(node->children[i], result);
  }

public:

  static vsx_ma_vector<unsigned char> serialize(vsx_filesystem_tree_writer& writer)
  {
    writer.calculate_offsets();
    vsx_ma_vector<unsigned char> result;
    serialize_node(&writer.root_node, result);
    return result;
  }
};
