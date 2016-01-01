#include <filesystem/vsx_filesystem_archive_vsx.h>
#include <string/vsx_string_helper.h>
#include <time/vsx_timer.h>
#include <vsx_argvector.h>
#include <test/vsx_test.h>

using namespace vsx;

void build_up()
{
  FILE* file = fopen("test_filesystem_archive_file_1", "wb");
  fputs("hello world 1", file);
  fclose(file);

  file = fopen("test_filesystem_archive_file_2", "wb");
  fputs("hello world 2", file);
  fclose(file);
}

void teardown()
{
  unlink("test_filesystem_archive_file_1");
  unlink("test_filesystem_archive_file_2");
  unlink("test_filesystem_archive.vsx");
}

void test_plain_files()
{
  filesystem_archive_vsx archive;
  archive.create("test_filesystem_archive.vsx");
  archive.file_add("test_filesystem_archive_file_1", "", false);
  archive.file_add("test_filesystem_archive_file_2", "", false);

  archive.close();

  filesystem_archive_vsx archive_load;

  archive_load.load("test_filesystem_archive.vsx", false);

  test_assert(archive_load.is_file("test_filesystem_archive_file_1"));
  test_assert(archive_load.is_file("test_filesystem_archive_file_2"));
}

void test_singleton_archive_prohibited()
{
  filesystem_archive* archive = filesystem::get_instance()->get_archive();
  test_assert(!archive);
}

void test_filesystem_archive()
{
  filesystem my_filesystem;
  my_filesystem.get_archive()->load("test_filesystem_archive.vsx", true);
  file* my_file =  my_filesystem.f_open("test_filesystem_archive_file_1", "r");
  test_assert(my_file);
  char* entire_file = my_filesystem.f_gets_entire(my_file);

  vsx_string<> hello(entire_file);
  test_assert(hello.size() == 14);
  my_filesystem.f_close(my_file);
}

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);
  build_up();

  test_singleton_archive_prohibited();
  test_plain_files();
  test_filesystem_archive();

  teardown();

  return 0;
}

