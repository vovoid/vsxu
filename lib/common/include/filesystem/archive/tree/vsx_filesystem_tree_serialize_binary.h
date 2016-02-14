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
      result.push_back(node->children[i]->name.size() | (node->children[i]->children.size()?1:0) << 8 );
      result.push_back( 0xff00 & node->children[i]->offset >> 8 );
      result.push_back( 0x00ff & node->children[i]->offset);
      foreach (node->children[i]->name, si)
        result.push_back( (unsigned char) node->children[i]->name[si]);
      result.push_back( 0xff000000 & node->children[i]->payload >> 24);
      result.push_back( 0x00ff0000 & node->children[i]->payload >> 16);
      result.push_back( 0x0000ff00 & node->children[i]->payload >> 8);
      result.push_back( 0x000000ff & node->children[i]->payload);
    }

    foreach (node->children, i)
      result += serialize_node(node->children[i], result);
  }

public:

  static vsx_ma_vector<unsigned char> serialize(vsx_filesystem_tree_writer& writer)
  {
    vsx_ma_vector<unsigned char> result;
    serialize_node(&writer.root_node, result);
    return result;
  }
};
