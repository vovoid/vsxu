#include <thread>
#include <inttypes.h>
#include <vsx_platform.h>

#if PLATFORM == PLATFORM_LINUX
  #include <unistd.h>
#endif

#include <stdlib.h>

#include <profiler/vsx_profiler_manager.h>

std::atomic_uint_fast64_t run_threads;

void thread_producer()
{
  vsx_profiler_manager::get_instance()->init_profiler();

  vsx_profiler* p = vsx_profiler_manager::get_instance()->get_profiler();
  while ( run_threads.load() )
  {
    p->maj_begin();
    p->sub_begin("1 test1");
    std::this_thread::sleep_for(std::chrono::microseconds(20));
    p->sub_begin("1 test2");
    std::this_thread::sleep_for(std::chrono::microseconds(200));
    p->sub_end();
    p->sub_end();
    p->maj_end();
  }
  vsx_printf(L"exiting p1\n");
}

void thread_producer2()
{
  vsx_profiler* p = vsx_profiler_manager::get_instance()->get_profiler();
  while ( run_threads.load() )
  {
    p->maj_begin();
    p->sub_begin("2 test1");
    std::this_thread::sleep_for(std::chrono::microseconds(100));
    p->sub_begin("2 test2");
    std::this_thread::sleep_for(std::chrono::microseconds(300));
    p->sub_end();
    std::this_thread::sleep_for(std::chrono::microseconds(50));
    p->sub_end();
    p->maj_end();
  }
  vsx_printf(L"exiting p2\n");
}



int main()
{
  run_threads = 1;

  std::thread t1 = std::thread( [](){thread_producer();});
  std::thread t2 = std::thread( [](){thread_producer2();});

  std::this_thread::sleep_for(std::chrono::seconds(20));

  run_threads.fetch_sub(1);

  t1.join();
  t2.join();

  return 0;
}
