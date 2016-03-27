#pragma once

#include "vsx_filesystem_tree_writer.h"

class vsx_filesystem_tree_serialize_string
{

  static vsx_string<> serialize_node(vsx_filesystem_tree_node* node)
  {
    vsx_string<> result;
    if (node->children.size())
      result += "offset: "+vsx_string_helper::i2s(node->offset)+"\n";

    foreach (node->children, i)
    {
      result += node->children[i]->name + "    -     payload: " + vsx_string_helper::i2s(node->children[i]->payload);
      if (node->children[i]->children.size())
        result += "     offset:  " + vsx_string_helper::i2s( node->children[i]->offset );
      result += "\n";
    }

    foreach (node->children, i)
      result += serialize_node(node->children[i]);

    return result;
  }

public:

  static vsx_string<> serialize(vsx_filesystem_tree_writer& writer)
  {
    writer.calculate_offsets();
    return serialize_node(&writer.root_node);
  }
};
