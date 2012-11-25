/**
* @author Dinesh Manajipet, Vovoid Media Technologies - Copyright (C) 2012-2020
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "input_video.h"

module_video_input::module_video_input():
  vsx_module(),
  m_bufferReady(false),
  m_currentTask(module_video_input::INITIALIZE_CAPTURE), //Whenever the worker starts...
  m_currentPage(-1),  // We havent used any buffers yet.
  nFrames(0)
{
  // Initialize the mutex and the worker thread attributes:
  // We reuse these variables for all the threads.
  pthread_attr_init(&m_worker_attribute);
  pthread_attr_setdetachstate(&m_worker_attribute, PTHREAD_CREATE_JOINABLE);

  pthread_mutex_init(&m_mutex, NULL);
}

bool module_video_input::isValid()
{
  return false;
}

bool module_video_input::init()
{
  if(!isValid())
    return false;

  printf("Starting Capture...\n");

  //Not bothering with mutexes:
  // This method must not be called when a worker thread is alive.
  // As it Initializes the internal data structures.
  m_buffer[0] = 0;
  m_buffer[1] = 0;

  m_bitm.data = 0;
  m_bitm.bpp = 3;
  m_bitm.bformat = GL_RGB;
  m_bitm.valid = false;

  m_currentTask = INITIALIZE_CAPTURE;
  m_currentPage = -1;

  pthread_create(&m_worker, NULL, &startWorker , (void*)this);

  return true;
}

void module_video_input::release_capture()
{
  Tasks state = currentTask();
  if( state != INITIALIZE_CAPTURE && state != CLEANUP_CAPTURE ){
    // Command the worker thread to terminate
    addTask(TERMINATE_CAPTURE);
 
    // wait for the worker thread to shutdown
    void*  ret;
    pthread_join(m_worker,&ret);

  }
}

void module_video_input::initializeBuffers(int w, int h, int depth, int nChannels)
{
  for(int i = 0; i < N_BUFFERS; i++)
    m_buffer[i] = cvCreateImage(cvSize(w,h),depth,nChannels);
  m_bufferReady = true;
}

int module_video_input::currentPage()
{
  int buffer;

  pthread_mutex_lock(&m_mutex);
  buffer = m_currentPage;
  pthread_mutex_unlock(&m_mutex);

  return buffer;
}

int module_video_input::nextPage()
{
  return ( currentPage() + 1)%N_BUFFERS;
}


void module_video_input::flipPage()
{
  pthread_mutex_lock(&m_mutex);
  m_currentPage++;
  m_currentPage%=N_BUFFERS;
  pthread_mutex_unlock(&m_mutex);
}


void module_video_input::freeBuffers()
{
  m_bufferReady = false;
  for(int i = 0; i < N_BUFFERS; i++){
    if(m_buffer[i])
      cvReleaseImage(&m_buffer[i]);
    m_buffer[i] = 0;
  }
}

module_video_input::Tasks module_video_input::currentTask()
{
  Tasks state;

  pthread_mutex_lock(&m_mutex);
  state = m_currentTask;
  pthread_mutex_unlock(&m_mutex);

  return state;
}

void module_video_input::addTask(Tasks state)
{
  pthread_mutex_lock(&m_mutex);
  m_currentTask = state;
  pthread_mutex_unlock(&m_mutex);

}



void module_video_input::run()
{
  if ( currentTask() == CONSUME_FRAME ){
    nFrames++;
    int page = nextPage();

    m_bitm.data = m_buffer[page]->imageData;
    m_bitm.size_x = m_buffer[page]->width;
    m_bitm.size_y = m_buffer[page]->height;
    m_bitm.timestamp++;
    m_bitm.valid = true;

    m_result->set_p(m_bitm);
    loading_done = true;

    flipPage();
    addTask(FETCH_FRAME);
  }
}

void module_video_input::stop()
{
  printf("Capture stopped\n");
}

void module_video_input::on_delete()
{
  release_capture();
  printf("Capture deleted\n");
  //delete[] bitm.data;
}

module_video_input::~module_video_input()
{
  //Cleanup
  pthread_attr_destroy(&m_worker_attribute);
  pthread_mutex_destroy(&m_mutex);
}

