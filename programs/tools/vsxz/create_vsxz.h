#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_writer.h>

#include "filenames.h"

void create_vsxz()
{
  req(vsx_argvector::get_instance()->has_param_with_value("c"));
  req(vsx_argvector::get_instance()->has_param("z"));

  vsx_nw_vector< vsx_string<> > filenames;
  populate_filenames(filenames);

  vsx::filesystem_archive_vsxz_writer archive;

  if (vsx_argvector::get_instance()->has_param("nc"))
    archive.disable_compression();

  vsx_string<> archive_filename = vsx_argvector::get_instance()->get_param_value("c");
  archive.create(archive_filename.c_str());

  for (size_t i = 0; i < filenames.size(); i++)
  {
    req_continue(filenames[i].size());
    req_continue(!vsx::filesystem_helper::is_dir(filenames[i]));
    req_continue(filenames[i][0] != '#');

    vsx_printf(L"* adding: %hs \n", filenames[i].c_str() );
    archive.add_file( filenames[i], "", true);
  }

  archive.close();


  vsx_printf(L"-- successfully created the archive: %hs\n", archive_filename.c_str());

  // success
  exit(0);
}
