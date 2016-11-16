#include <tools/vsx_thread_pool.h>
#include <string/vsx_string_helper.h>
#include <time/vsx_timer.h>
#include <vsx_argvector.h>
#include <test/vsx_test.h>

class foo
{
public:
  vsx_thread_pool pool;
};

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  foo* f = new foo;

  threaded_task
    {
      for_n(i, 0, 32)
        vsx_printf(L"hello world from thread 1\n");
    }
  threaded_task_end;

  threaded_task
    {
      test_assert(!vsx_thread_pool::instance()->wait_all(100));
      for_n(i, 0, 32)
        vsx_printf(L"hello world from thread 2\n");
    }
  threaded_task_end;

  threaded_task_wait_all(100);

  delete f;

  test_complete
  return 0;
}

