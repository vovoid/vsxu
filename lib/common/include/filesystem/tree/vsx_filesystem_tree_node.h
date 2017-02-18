#pragma once

#include <vsx_platform.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>


VSX_PACK_BEGIN
struct vsx_filesystem_tree_node_storage_header
{
  uint8_t filename_length_mask;
  uint16_t child_node_offset;
}
VSX_PACK_END

VSX_PACK_BEGIN
struct vsx_filesystem_tree_node_storage_footer
{
  uint32_t payload;
}
VSX_PACK_END

class vsx_filesystem_tree_node
{
  friend class vsx_filesystem_tree_serialize_string;
  friend class vsx_filesystem_tree_serialize_binary;
  friend class vsx_filesystem_tree_writer;

  vsx_nw_vector< vsx_filesystem_tree_node* > children;
  vsx_string<> name;
  uint32_t payload = 0;

  // for calculating offset
  uint16_t total_block_size = 0;
  uint16_t offset = 0;

  int quick_sort_partition(vsx_filesystem_tree_node** a, int first, int last)
  {
    vsx_filesystem_tree_node* pivot = a[first];
    int lastS1 = first;
    int firstUnknown = first + 1;
    while (firstUnknown <= last) {
      if (a[firstUnknown]->name < pivot->name) {
        lastS1++;
        vsx_filesystem_tree_node* b = a[firstUnknown];
        a[firstUnknown] = a[lastS1];
        a[lastS1] = b;
      }
      firstUnknown++;
    }
    vsx_filesystem_tree_node* b = a[first];
    a[first] = a[lastS1];
    a[lastS1] = b;
    return lastS1;
  }

  void quick_sort(vsx_filesystem_tree_node** a, int first, int last)
  {
    if (first < last) {
      int pivotIndex = quick_sort_partition(a, first, last);
      quick_sort(a, first, pivotIndex - 1);
      quick_sort(a, pivotIndex + 1, last);
    }
  }

  void sort()
  {
    quick_sort(children.get_pointer(), 0, (int)children.size() - 1);
  }

public:

  void calculate_offsets(uint16_t& target_offset)
  {
    offset = target_offset;

    foreach (children, i)
    {
      if (children[i]->children.size())
        target_offset += (uint16_t)children[i]->name.size() + sizeof(vsx_filesystem_tree_node_storage_header);
      else
        target_offset += (uint16_t)children[i]->name.size() + 1 + sizeof(vsx_filesystem_tree_node_storage_footer);
    }
    target_offset += 1;

    foreach (children, i)
      children[i]->calculate_offsets(target_offset);
  }

  vsx_filesystem_tree_node* get_create(vsx_string<> new_name, uint32_t payload)
  {
    // already existing
    foreach (children, i)
      if (children[i]->name == new_name)
        return children[i];

    // new node
    vsx_filesystem_tree_node* new_node = new vsx_filesystem_tree_node();
    new_node->name = new_name;
    new_node->payload = payload;
    children.push_back(new_node);
    sort();
    return new_node;
  }

  ~vsx_filesystem_tree_node()
  {
    foreach (children, i)
      delete children[i];
  }
};
