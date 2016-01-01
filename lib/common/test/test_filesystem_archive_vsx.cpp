#include <filesystem/vsx_filesystem_archive_vsx.h>
#include <string/vsx_string_helper.h>
#include <time/vsx_timer.h>
#include <vsx_argvector.h>
#include <test/vsx_test.h>

void test_plain_files()
{
  FILE* file = fopen("/tmp/test_filesystem_archive_file_1", "wb");
  fputs("hello world 1", file);
  fclose(file);

  file = fopen("/tmp/test_filesystem_archive_file_2", "wb");
  fputs("hello world 2", file);
  fclose(file);

  vsx::filesystem_archive_vsx archive;
  archive.create("/tmp/test_filesystem_archive.vsx");
  archive.file_add("/tmp/test_filesystem_archive_file_1", "", false);
  archive.file_add("/tmp/test_filesystem_archive_file_2", "", false);

  archive.close();
  //unlink("/tmp/test_filesystem_archive_file_1");
  //unlink("/tmp/test_filesystem_archive_file_2");

  vsx::filesystem_archive_vsx archive_load;

  archive_load.load("/tmp/test_filesystem_archive.vsx", false);

  test_assert(archive_load.is_file("/tmp/test_filesystem_archive_file_1"));

  //unlink("/tmp/test_filesystem_archive.vsx");

}

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  test_plain_files();

  return 0;
}

