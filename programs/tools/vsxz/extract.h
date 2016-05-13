#include <vsx_argvector.h>
#include <filesystem/vsx_filesystem.h>
#include <filesystem/archive/vsx/vsx_filesystem_archive_vsx_reader.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_reader.h>
#include <perf/vsx_perf.h>

void extract()
{
  req(vsx_argvector::get_instance()->has_param_with_value("x"));

  vsx::filesystem filesystem;

  bool dry_run = vsx_argvector::get_instance()->has_param("dry");

  vsx_perf perf_cpu_instructions;

  if ( vsx_argvector::get_instance()->has_param("m") )
    perf_cpu_instructions.cpu_instructions_start();

  // Sanitize current_path
  if (!current_path.size())
    VSX_ERROR_EXIT("Fatal error: current_path is empty",1);

  if (access(current_path.c_str(),W_OK))
    VSX_ERROR_EXIT("current_path is not accessible for writing",1);

  vsx_string<> filename = vsx_argvector::get_instance()->get_param_value("x");

  // Sanitize file
  if (!filesystem.is_file(filename))
    VSX_ERROR_EXIT( (vsx_string<>("Invalid archive supplied,")+filename).c_str(), 1);

  if ( vsx_argvector::get_instance()->has_param("m") )
    perf_cpu_instructions.cpu_instructions_begin();

  filesystem.get_archive()->load( filename.c_str(), vsx_argvector::get_instance()->has_param("mt") );

  // Sanitize archive
  if (!filesystem.get_archive()->is_archive_populated())
    VSX_ERROR_EXIT("Archive contains no files or failed to load", 2);

  vsx_nw_vector<vsx::filesystem_archive_file_read> archive_files;
  filesystem.get_archive()->files_get(archive_files);
  for (unsigned long i = 0; i < archive_files.size(); ++i)
  {
    vsx_string<> out_filename = archive_files[i].filename;
    vsx_string<> out_dir = vsx_string_helper::path_from_filename(out_filename);
    vsx_string<> full_out_path = current_path + "/" + out_filename;
    vsx_string<> out_directory = current_path + "/" + out_dir;

    if (!dry_run)
      vsx::filesystem_helper::create_directory( out_directory.c_str() );

    vsx::file* fpi = filesystem.f_open(archive_files[i].filename.c_str());
    if (!fpi)
    {
      continue;
      VSX_ERROR_EXIT( (vsx_string<>("Internal Error: fpi invalid when reading ")+archive_files[i].filename).c_str(), 3);
    }

    FILE* fpo = 0;
    if (!dry_run)
    {
      fpo = fopen(full_out_path.c_str(), "wb");
      if (!fpo)
        VSX_ERROR_EXIT( (vsx_string<>("Internal Error: fpo invalid when opening file for writing: ")+full_out_path).c_str(), 3 );
    }

    char* buf = filesystem.f_gets_entire(fpi);
    if (!buf)
    {
      if (!dry_run)
      fclose(fpo);
      VSX_ERROR_EXIT("Internal Error: buf invalid", 4);
    }

    if (!dry_run)
      fwrite(buf,sizeof(char),fpi->size,fpo);

    if (!dry_run)
      fclose(fpo);
    filesystem.f_close(fpi);
  }

  if ( vsx_argvector::get_instance()->has_param("m") )
  {
    uint64_t cpu_instructions = perf_cpu_instructions.cpu_instructions_end();
    vsx_printf(
      L"decompression took:\n"
      " - %f seconds at 3.0 GHz"
      " - %lld CPU instructions\n",
      (double)cpu_instructions / 3000000000.0,
      cpu_instructions
    );
    perf_cpu_instructions.cpu_instructions_stop();
  }

  filesystem.get_archive()->close();

  // success
  exit(0);
}
