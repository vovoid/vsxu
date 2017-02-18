#include <command/vsx_command_list.h>
#include <string/vsx_string_helper.h>
#include <time/vsx_timer.h>
#include <vsx_argvector.h>
#include <test/vsx_test.h>

using namespace vsx;

void test_legacy()
{
  filesystem my_filesystem;
  my_filesystem.get_archive()->load("/tmp/dolphin.vsx", true);
  vsx_command_list command_list(true);
  command_list.set_filesystem(&my_filesystem);
  command_list.load_from_file("_states/_default", true);
  vsx_command_s* command;
  forever
  {
    req_break(command = command_list.get());
    vsx_printf(L"command: %s\n", command->raw.c_str());
  }
  my_filesystem.get_archive()->close();
}

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  test_legacy();

  return 0;
}

