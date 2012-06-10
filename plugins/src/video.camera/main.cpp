/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
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

#include <pthread.h>
#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"

//#include <cv.h>
#include <highgui.h>

//thread data
pthread_mutex_t signal_mutex;
pthread_t worker_t;
pthread_attr_t attr;
bool m_isRunning;
bool frame_isReady;
IplImage *buffer = 0;

//TODO: implement the cleanup after memory is released

void* worker(void *ptr) {
    CvCapture* capture = cvCreateCameraCapture(0);
    bool running = true;
    IplImage *m_frame;
    vsx_bitmap *bitm = (vsx_bitmap*)ptr;

    while(running){
      m_frame = cvQueryFrame(capture);
      if(m_frame){
        if(!buffer)
          buffer = cvCreateImage(cvSize(m_frame->width,m_frame->height),
                                 m_frame->depth,m_frame->nChannels);
        //printf("getting camera data for %s \n", bitm.timestamp);

        pthread_mutex_lock(&signal_mutex);
        cvConvertImage(m_frame,buffer, CV_CVTIMG_SWAP_RB);
        frame_isReady = true;
        running = m_isRunning;
        pthread_mutex_unlock(&signal_mutex);
      }
    }
    cvReleaseCapture( &capture );
    pthread_exit(NULL);
  }

class module_video_camera : public vsx_module {
  vsx_module_param_bitmap* result1;
  // internal
  vsx_bitmap m_bitm;
  IplImage *m_buffer;

  int width, height;
  int current_frame;
  int previous_frame;

  // currently we enable the camera reading only for a single class.
  bool m_isValid;
public:
  // this stores the number of valid instances of Camera module
  static int count;

  module_video_camera():
    m_buffer(0){
      //Enable the camera reading only on a single object
      if(count >= 1){
        m_isValid = false;
        message = "module||ERROR! Only 1 instance of Camera module is currently allowed";
      }
      else {
        count++;
        m_isValid = true;
      }
      m_bitm.data = 0;
      m_bitm.bpp = 3;
      m_bitm.bformat = GL_RGB;
      m_bitm.valid = false;
  };
  ~module_video_camera(){
    //release_camera();
    if(m_isValid)
      count--;
  };


  bool init(){
    //Initialize the worker thread which does the job
    if(m_isValid){
      m_isRunning = true;
      frame_isReady = false;
      printf("Camera started\n");
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

      pthread_mutex_init(&signal_mutex, NULL);
      pthread_create(&worker_t, NULL, &worker , (void*)&m_bitm);
    }

  }

  void release_camera()
  {
    // Signal the worker thread to shutdown
    if(m_isValid){
      pthread_mutex_lock(&signal_mutex);
      m_isRunning = false;
      pthread_mutex_unlock(&signal_mutex);

      //Cleanup
      pthread_attr_destroy(&attr);
      pthread_mutex_destroy(&signal_mutex);

      // wait for the worker thread to shutdown
      void *  ret;
      pthread_join(worker_t,&ret);
    }
  }

  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "";
      info->identifier = "video;camera";
      info->out_param_spec = "bitmap:bitmap";
      info->component_class = "bitmap";
    info->description = "Fetches camera data as bitmaps";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    result1->set_p(m_bitm);
    previous_frame = -1;
  }

  void param_set_notify(const vsx_string& name)
  {
  }

  void run()
  {
    if (m_isValid){
      pthread_mutex_lock(&signal_mutex);
      if(frame_isReady){
          if(!m_buffer)
            m_buffer = cvCreateImage(cvSize(buffer->width,buffer->height),
                                    buffer->depth,buffer->nChannels);
          cvCopy(buffer,m_buffer);
          m_bitm.data = m_buffer->imageData;
          m_bitm.timestamp++;
          m_bitm.size_x = m_buffer->width;
          m_bitm.size_y = m_buffer->height;
          m_bitm.valid = true;

        result1->set_p(m_bitm);
        frame_isReady = false;
        loading_done = true;
      }
      pthread_mutex_unlock(&signal_mutex);
    }
  }
  void start() {
  }
  void stop() {
    printf("Camera stopped\n");
  }

  void on_delete()
  {
    printf("Camera deleted\n");
    release_camera();
    //delete[] bitm.data;
  }
};

int module_video_camera::count = 0;

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}

vsx_module* create_new_module(unsigned long module) {
  return (vsx_module*)(new module_video_camera);
}

void destroy_module(vsx_module* m,unsigned long module) {
  delete (module_video_camera*)m;
}

unsigned long get_num_modules() {
  return 1;
}