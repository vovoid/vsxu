#include <test/vsx_test.h>
#include <input/vsx_input_event_queue.h>
#include <input/vsx_input_event_queue_reader.h>

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  vsx_input_event_queue queue;

  vsx_input_event vsx_event(vsx_input_event::type_keyboard);
  vsx_event.keyboard.scan_code = 1;
  vsx_event.keyboard.pressed = true;
  queue.add(vsx_event);

  vsx_input_event_queue_reader reader(&queue);
  vsx_input_event* consumed_event = reader.consume();
  test_assert(consumed_event->type == vsx_input_event::type_keyboard);
  test_assert(consumed_event->keyboard.pressed == true);
  test_assert(consumed_event->keyboard.scan_code == 1);

  vsx_input_event_queue_reader* reader_heap = new vsx_input_event_queue_reader(&queue);
  test_assert(reader_heap == 0x0);

  test_complete
  return 0;
}

