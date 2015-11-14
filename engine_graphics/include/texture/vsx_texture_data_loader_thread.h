#ifndef VSX_TEXTURE_LOADER_THREAD_H
#define VSX_TEXTURE_LOADER_THREAD_H

#include <pthread.h>

class vsx_texture_data_loader_thread
{
public:
  pthread_t					worker_t;
  pthread_attr_t		worker_t_attr;
};

#endif
