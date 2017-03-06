#pragma once

#include <filesystem/vsx_filesystem.h>
#include <tools/vsx_singleton.h>
#include <string/vsx_string_helper.h>
#include <list>

namespace vsx
{
namespace engine
{
namespace audiovisual
{
class pack_manager
{
  std::list< vsx_string<> > file_list;
  vsx_ma_vector< vsx::filesystem* > filesystem_list;

public:

  void load(vsx_string<> base_path, vsx_string<> visual_pack_path = "visual_packs")
  {
    vsx_string_helper::ensure_trailing_dir_separator(base_path);
    vsx::filesystem_helper::get_files_recursive(own_path + visual_pack_path, &file_list, "", "");
    for (auto it = file_list.begin(); it != file_list.end(); ++it)
    {
      vsx::filesystem* fs = new vsx::filesystem();
      fs->get_archive()->load( (*it)->c_str(), true );
      vsx_string<> vsx_string_helper::read_from_file("manifest.json", fs);

    }
  }

};
}
}
}
