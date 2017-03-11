#pragma once

#include <filesystem/vsx_filesystem.h>
#include <tools/vsx_singleton.h>
#include <string/vsx_string_helper.h>
#include <list>
#include <audiovisual/vsx_pack.h>

namespace vsx
{
namespace engine
{
namespace audiovisual
{

class pack_manager
{
  vsx_ma_vector< pack* > packs;

public:

  void load(vsx_string<> base_path, vsx_string<> visual_pack_path = "visuals_packs")
  {
    vsx_string_helper::ensure_trailing_dir_separator(base_path);
    std::list< vsx_string<> > file_list;
    vsx::filesystem_helper::get_files_recursive(base_path + visual_pack_path, &file_list, ".vsxz", "");
    for (auto it = file_list.begin(); it != file_list.end(); ++it)
      packs.push_back( new pack(*it));
  }

  void add_states(std::vector<state*>& result)
  {
    foreach (packs, i)
      packs[i]->add_states(result);
  }

};
}
}
}
