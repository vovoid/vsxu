#include <string/vsx_string_helper.h>
#include <test/vsx_test.h>
#include <filesystem/archive/tree/vsx_filesystem_tree_writer.h>
#include <filesystem/archive/tree/vsx_filesystem_tree_serialize_string.h>
#include <filesystem/archive/tree/vsx_filesystem_tree_serialize_binary.h>
#include <filesystem/archive/tree/vsx_filesystem_tree_reader.h>

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  vsx_filesystem_tree_writer writer;

  writer.add_file("home/jaw/test1.txt", 1);
  writer.add_file("home/jaw/abc.txt", 2);
  writer.add_file("abc/xyz.txt", 1073741824 );
  writer.add_file("abc/def.txt", 2147483648 );

  writer.calculate_offsets();
  vsx_string<> result = vsx_filesystem_tree_serialize_string::serialize(writer);
  vsx_printf(L"%s\n", result.c_str());

  vsx_ma_vector<unsigned char> result_binary = vsx_filesystem_tree_serialize_binary::serialize(writer);

  vsx_filesystem_tree_reader reader;
  reader.initialize(result_binary.get_pointer());

  uint32_t reader_result = reader.get_payload_by_filename( "abc/def.txt" );
  test_assert( reader_result == 2147483648 );

  test_complete
  return 0;
}

