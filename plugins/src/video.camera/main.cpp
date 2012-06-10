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
int current_buffer = -1;

void* worker(void *ptr) {
    CvCapture* capture = cvCreateCameraCapture(0);
    bool running = true;
    IplImage *m_frame;
    IplImage **buffer = (IplImage**)ptr;

    while(running){
      m_frame = cvQueryFrame(capture);
      if(m_frame){
        if( buffer[0] == 0 || buffer[1] == 0 ){
          buffer[0] = cvCreateImage(cvSize(m_frame->width,m_frame->height),
                                 m_frame->depth,m_frame->nChannels);
          buffer[1] = cvCreateImage(cvSize(m_frame->width,m_frame->height),
                                 m_frame->depth,m_frame->nChannels);
        }

        //Convert the image to the actual form and put it in the actual current buffer
        cvConvertImage(m_frame,buffer[(current_buffer+1)%2], CV_CVTIMG_SWAP_RB);

        pthread_mutex_lock(&signal_mutex);
        running = m_isRunning;
        current_buffer = (current_buffer+1)%2;
        //When the module is asked to be deleted invalidate the current frame so that it wont be read in the main thread
        if(!running)
          current_buffer = -1;
        pthread_mutex_unlock(&signal_mutex);
      }
    }

    //Cleanup
    cvReleaseCapture( &capture );
    cvReleaseImage(&buffer[0]);
    cvReleaseImage(&buffer[1]);
    pthread_exit(NULL);
  }

class module_video_camera : public vsx_module {
  vsx_module_param_bitmap* result1;
  // internal
  vsx_bitmap m_bitm;
  IplImage* m_buffer[2];

  int width, height;
  int previous_frame;

  // currently we enable the camera reading only for a single class.
  bool m_isValid;
public:
  // this stores the number of valid instances of Camera module
  static int count;

  module_video_camera()
    {
      //Enable the camera reading only on a single object
      if(count >= 1){
        m_isValid = false;
        message = "module||ERROR! Only 1 instance of Camera module is currently allowed";
      }
      else {
        count++;
        m_buffer[0] = 0;
        m_buffer[1] = 0;
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
      current_buffer = -1;
      printf("Camera started\n");
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

      pthread_mutex_init(&signal_mutex, NULL);
      pthread_create(&worker_t, NULL, &worker , (void*)m_buffer);
    }

  }

  void release_camera()
  {
    // Signal the worker thread to shutdown
    if(m_isValid){
      pthread_mutex_lock(&signal_mutex);
      m_isRunning = false;
      pthread_mutex_unlock(&signal_mutex);

      // wait for the worker thread to shutdown
      void *  ret;
      pthread_join(worker_t,&ret);

      //Cleanup
      pthread_attr_destroy(&attr);
      pthread_mutex_destroy(&signal_mutex);

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
    int i;
    if (m_isValid){
      pthread_mutex_lock(&signal_mutex);
      i = current_buffer;
      pthread_mutex_unlock(&signal_mutex);

      if(i >= 0){
        m_bitm.data = m_buffer[i]->imageData;
        m_bitm.size_x = m_buffer[i]->width;
        m_bitm.size_y = m_buffer[i]->height;
        m_bitm.timestamp++;
        m_bitm.valid = true;

        result1->set_p(m_bitm);
        loading_done = true;
      }
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