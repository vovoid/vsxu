#pragma once

#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <vsx_argvector.h>

void populate_filenames(vsx_nw_vector< vsx_string<> >& filenames)
{
  // Sanitize the presence of -f / -fl
  if (!vsx_argvector::get_instance()->has_param_with_value("f") && !vsx_argvector::get_instance()->has_param_with_value("fl"))
    VSX_ERROR_EXIT("Error, you must supply files to be added to the archive", 100);

  vsx_string<> filenames_string;
  if (vsx_argvector::get_instance()->has_param_with_value("f"))
    filenames_string = vsx_argvector::get_instance()->get_param_value("f");

  if (vsx_argvector::get_instance()->has_param_with_value("fl"))
  {
    vsx_string<> file_list_file = vsx_argvector::get_instance()->get_param_value("fl");
    if (!vsx::filesystem::get_instance()->is_file(file_list_file))
      VSX_ERROR_EXIT("Error, can not read file list file...", 101);
    filenames_string = vsx_string_helper::read_from_file<1024*1024>( file_list_file );
  }

  vsx_string<>deli = ":";
  vsx_string_helper::explode(filenames_string, deli, filenames);

  if (!filenames.size())
    VSX_ERROR_EXIT("There seems to be no entries in the file list", 101);

  vsx_printf(L"File names:\n %s\n", filenames_string.c_str());

  // Make sure we can read all the files
  for (size_t i = 0; i < filenames.size(); i++)
    if (access(filenames[i].c_str(),R_OK))
      VSX_ERROR_EXIT( ( vsx_string<>("Pre-check failed, error accessing file: ") + filenames[i]).c_str()  ,1);

}

