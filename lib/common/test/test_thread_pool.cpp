#include <tools/vsx_thread_pool.h>
#include <string/vsx_string_helper.h>
#include <time/vsx_timer.h>
#include <vsx_argvector.h>


int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  vsx_thread_pool::instance()->add(
    [=]()
    {
      for_n(512,i)
        vsx_printf(L"hello world from thread 1\n");
    }
  );

  vsx_thread_pool::instance()->add(
    [=]()
    {
      for_n(512,i)
        vsx_printf(L"hello world from thread 2\n");
    }
  );

  return 0;
}

