#include <test/vsx_test.h>
#include <input/vsx_input_event_queue.h>

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  vsx_input_event_queue queue;

  vsx_printf(L"input_event size: %d\n", sizeof(vsx_input_event));

  test_complete
  return 0;
}

