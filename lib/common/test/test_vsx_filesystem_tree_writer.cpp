#include <string/vsx_string_helper.h>
#include <test/vsx_test.h>
#include <filesystem/archive/tree/vsx_filesystem_tree_writer.h>
#include <filesystem/archive/tree/vsx_filesystem_tree_serialize_string.h>

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  vsx_filesystem_tree_writer writer;

  writer.add_file("home/jaw/test1.txt", 1);
  writer.add_file("home/jaw/abc.txt", 2);
  writer.add_file("abc/xyz.txt", 4);
  writer.add_file("abc/def.txt", 3);

  writer.calculate_offsets();
  vsx_string<> result = vsx_filesystem_tree_serialize_string::serialize(writer);
  vsx_printf(L"%s\n", result.c_str());

  test_complete
  return 0;
}

