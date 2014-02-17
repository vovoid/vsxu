#include <inttypes.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <sched.h>
#include <linux/unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include "../include/vsx_fifo.h"
#include "../include/vsx_profiler.h"

#define INC_TO 1000000

vsx_fifo<> fifo;

int producer_still_running = 1;
int64_t producer_sum;
int64_t consumer_sum;

uint64_t producer_start;
uint64_t consumer_end;
int64_t num_consumed = 0;

volatile __attribute__((aligned(64))) int64_t run_thread = 1;

void *thread_producer( void *arg )
{
  VSX_UNUSED(arg);
  vsx_profiler* p = vsx_profiler_manager::get_instance()->get_profiler();
  while ( __sync_fetch_and_add( &run_thread, 0) )
  {
    vsx_printf("running producer1 %d\n", run_thread);
    p->begin("1 test1");
    usleep(20);
    p->begin("1 test2");
    usleep(200);
    p->end();
    p->end();
  }
  vsx_printf("exiting p1\n");
  pthread_exit(0);
  return NULL;
}

void *thread_producer2( void *arg )
{
  VSX_UNUSED(arg);
  vsx_profiler* p = vsx_profiler_manager::get_instance()->get_profiler();
  while ( __sync_fetch_and_add( &run_thread, 0) )
  {
    vsx_printf("running producer2 %d\n", run_thread);
    p->begin("2 test1");
    usleep(100);
    p->begin("2 test2");
    usleep(300);
    p->end();
    usleep(50);
    p->end();
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


  sleep( 2 );

  __sync_fetch_and_sub( &run_thread, 1);


  for (i = 0; i < 2; i++)
  {
    vsx_printf("joining iterator %d\n", i);
    pthread_join( thrs[i], NULL );
  }

  free( thrs );

  return 0;
}
