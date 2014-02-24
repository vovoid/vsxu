#include <inttypes.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <linux/unistd.h>
#include <sys/syscall.h>

#include "../include/vsx_profiler.h"

volatile __attribute__((aligned(64))) int64_t run_threads = 1;

void *thread_producer( void *arg )
{
  VSX_UNUSED(arg);
  vsx_profiler* p = vsx_profiler_manager::get_instance()->get_profiler();
  while ( __sync_fetch_and_add( &run_threads, 0) )
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
  vsx_printf("exiting p1\n");
  pthread_exit(0);
  return NULL;
}

void *thread_producer2( void *arg )
{
  VSX_UNUSED(arg);
  vsx_profiler* p = vsx_profiler_manager::get_instance()->get_profiler();
  while ( __sync_fetch_and_add( &run_threads, 0) )
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
  vsx_printf("exiting p2\n");
  pthread_exit(0);
  return NULL;
}



int main()
{
  int procs = 0;
  int i;
  pthread_t *thrs;

  // Getting number of CPUs
  procs = (int)sysconf( _SC_NPROCESSORS_ONLN );
  if (procs < 0)
  {
    perror( "sysconf" );
    return -1;
  }

  typedef char       tag[36];

  vsx_printf("This system has %d cores available\n", procs);

  thrs = (pthread_t*)malloc( sizeof( pthread_t ) * procs );
  if (thrs == NULL)
  {
    perror( "malloc" );
    return -1;
  }

  pthread_create(
    &thrs[0],
    NULL,
    thread_producer,
    0x0
  );

  pthread_create(
    &thrs[1],
    NULL,
    thread_producer2,
    0x0
  );


  sleep( 20 );

  __sync_fetch_and_sub( &run_threads, 1);


  for (i = 0; i < 2; i++)
  {
    pthread_join( thrs[i], NULL );
  }

  free( thrs );

  return 0;
}
