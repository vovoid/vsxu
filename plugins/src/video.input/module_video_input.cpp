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


#include "module_video_input.h"

module_video_input::module_video_input():
  vsx_module(),
  m_buffersReady(false),
  m_currentBuffer(-1),  // We havent used any buffers yet.
  m_isValid(true),     //The derived class constructors will do the appropriate validation checks
  m_isRunning(false)  //The base class has to start the worker threads after validation
{

  //Enable the capture reading only on a single object
  m_buffer[0] = 0;
  m_buffer[1] = 0;

  m_bitm.data = 0;
  m_bitm.bpp = 3;
  m_bitm.bformat = GL_RGB;
  m_bitm.valid = false;
}

bool module_video_input::init()
{
  if(m_isValid){
    m_isRunning = true;
    m_currentBuffer = -1;

    printf("Capture started\n");
    pthread_attr_init(&m_worker_attribute);
    pthread_attr_setdetachstate(&m_worker_attribute, PTHREAD_CREATE_JOINABLE);

    pthread_mutex_init(&m_mutex, NULL);
    pthread_create(&m_worker, NULL, &startWorker , (void*)this);
    return true;
  }
  return false;
}

void module_video_input::release_capture()
{
  // Signal the worker thread to shutdown
  if(m_isValid && m_isRunning){
    pthread_mutex_lock(&m_mutex);
    m_isRunning = false;
    pthread_mutex_unlock(&m_mutex);

    // wait for the worker thread to shutdown
    void *  ret;
    pthread_join(m_worker,&ret);

    //Cleanup
    pthread_attr_destroy(&m_worker_attribute);
    pthread_mutex_destroy(&m_mutex);
  }
}

void module_video_input::initializeBuffers(int w, int h, int depth, int nChannels)
{
  for(int i = 0; i < N_BUFFERS; i++)
    m_buffer[i] = cvCreateImage(cvSize(w,h),depth,nChannels);
  m_buffersReady = true;
}

void module_video_input::freeBuffers()
{
  m_buffersReady = false;
  for(int i = 0; i < N_BUFFERS; i++){
    cvReleaseImage(&m_buffer[i]);
    m_buffer[i] = 0;
  }
}


void module_video_input::run()
{
  int currentBuffer;
  if (m_isValid){
    pthread_mutex_lock(&m_mutex);
    currentBuffer = m_currentBuffer;
    pthread_mutex_unlock(&m_mutex);

    if(currentBuffer >= 0){
      m_bitm.data = m_buffer[currentBuffer]->imageData;
      m_bitm.size_x = m_buffer[currentBuffer]->width;
      m_bitm.size_y = m_buffer[currentBuffer]->height;
      m_bitm.timestamp++;
      m_bitm.valid = true;

      result->set_p(m_bitm);
      loading_done = true;
    }
  }
}

void module_video_input::stop()
{
  printf("Capture stopped\n");
}

void module_video_input::on_delete()
{
  printf("Capture deleted\n");
  release_capture();
  //delete[] bitm.data;
}
