#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_reader.h>
#include <filesystem/archive/vsxz/vsx_filesystem_archive_vsxz_writer.h>
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
  filesystem_archive_vsxz_writer archive;
  archive.create("test_filesystem_archive.vsxz");
  archive.add_file("test_filesystem_archive_file_1", "", false);
  archive.add_file("test_filesystem_archive_file_2", "", false);

  archive.close();

  filesystem_archive_vsxz_reader archive_load;

  archive_load.load("test_filesystem_archive.vsxz", false);

  test_assert(archive_load.is_file("test_filesystem_archive_file_1"));
  test_assert(archive_load.is_file("test_filesystem_archive_file_2"));
  archive_load.close();
}

void test_text_files()
{
  filesystem_archive_vsxz_writer archive;
  archive.create("test_filesystem_archive.vsxz");
  archive.add_file("test_filesystem_archive_file_1", "", false);
  archive.add_file("test_filesystem_archive_file_2", "", false);
  archive.add_string("test_string", "hello", false);
  archive.close();

  filesystem_archive_vsxz_reader archive_load;

  archive_load.load("test_filesystem_archive.vsxz", false);

  file* handle = new file;
  archive_load.file_open("test_string", handle);

  char* entire_string_file = (char*)handle->data.get_pointer();
  vsx_string<> test_string(entire_string_file, handle->data.size());
  test_assert( test_string == "hello");

  test_assert(archive_load.is_file("test_filesystem_archive_file_1"));
  test_assert(archive_load.is_file("test_filesystem_archive_file_2"));
  delete handle;
  archive_load.close();
}


void test_filesystem_archive_reader()
{
  filesystem my_filesystem;
  my_filesystem.get_archive()->load("test_filesystem_archive.vsxz", true);
  file* my_file =  my_filesystem.f_open("test_filesystem_archive_file_1");
  test_assert(my_file);
  char* entire_file = my_filesystem.f_gets_entire(my_file);

  vsx_string<> hello(entire_file, my_file->data.size());
  test_assert(hello.size() == 13);
  test_assert((hello == "hello world 1"));
  my_filesystem.f_close(my_file);
  my_filesystem.get_archive()->close();
}

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);
  build_up();

  test_plain_files();
  test_text_files();
  test_filesystem_archive_reader();

  teardown();

  test_complete

  return 0;
}

