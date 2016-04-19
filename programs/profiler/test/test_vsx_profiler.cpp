#include <thread>
#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <vsx_platform.h>

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
    usleep(20);
    p->sub_begin("1 test2");
    usleep(200);
    p->sub_end();
    p->sub_end();
    p->maj_end();
  }
  vsx_printf(L"exiting p1\n");
  pthread_exit(0);
}

void thread_producer2()
{
  vsx_profiler* p = vsx_profiler_manager::get_instance()->get_profiler();
  while ( run_threads.load() )
  {
    p->maj_begin();
    p->sub_begin("2 test1");
    usleep(100);
    p->sub_begin("2 test2");
    usleep(300);
    p->sub_end();
    usleep(50);
    p->sub_end();
    p->maj_end();
  }
  vsx_printf(L"exiting p2\n");
  pthread_exit(0);
}



int main()
{
  unsigned int procs = 0;
  run_threads = 1;
  pthread_t *thrs;

  // Getting number of CPUs
  procs = std::thread::hardware_concurrency();
  vsx_printf(L"This system has %d cores available\n", procs);

  thrs = (pthread_t*)malloc( sizeof( pthread_t ) * procs );
  if (thrs == NULL)
  {
    perror( "malloc" );
    return -1;
  }

  std::thread t1 = std::thread( [](){thread_producer();});
  std::thread t2 = std::thread( [](){thread_producer2();});


  sleep( 20 );

  run_threads.fetch_sub(1);

  t1.join();
  t2.join();
  free( thrs );

  return 0;
}
