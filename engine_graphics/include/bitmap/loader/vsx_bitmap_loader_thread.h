#pragma once

#include <pthread.h>
#include "vsx_bitmap_loader_thread_info.h"

class vsx_bitmap_loader_thread
{
public:
  pthread_t					worker_t;
  pthread_attr_t		worker_t_attr;

  vsx_bitmap_loader_thread(void *(*worker) (void *), vsx_texture_loader_thread_info* thread_info)
  {
    pthread_attr_init(&worker_t_attr);
    thread_info->thread = (void*)this;
    pthread_create(&(worker_t), &(worker_t_attr), worker, (void*)thread_info);
  }
};
