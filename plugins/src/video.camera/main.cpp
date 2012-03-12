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

#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"
//#include "system/pthread/pthread.h"
#include "main.h"

//#include <cv.h>
#include <highgui.h>

class module_video_webcam : public vsx_module {
  vsx_module_param_bitmap* result1;
  // internal
  CvCapture* m_capture;
  vsx_bitmap bitm;
  IplImage *m_frame;

  int width, height;
  int current_frame;
  int previous_frame;
public:
  module_video_webcam():
    m_capture(0),
    m_frame(0){
  }
  ~module_video_webcam(){
    release_webcam();
  }

  bool init(){
    m_capture = cvCreateCameraCapture(0);
  }

  void release_webcam()
  {
    if(m_capture)
      cvReleaseCapture( &m_capture );
    m_capture = 0;
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
    result1->set_p(bitm);
    bitm.data = 0;
    bitm.bpp = 3;
    bitm.bformat = GL_RGB;
    bitm.valid = false;
    previous_frame = -1;
  }

  void param_set_notify(const vsx_string& name) 
  {
  }

  void run()
  {
    m_frame = cvQueryFrame(m_capture);
    if(m_frame){
      cvConvertImage(m_frame,m_frame, CV_CVTIMG_SWAP_RB);
      bitm.data = m_frame->imageData;
      bitm.timestamp++;
      bitm.size_x = m_frame->width;
      bitm.size_y = m_frame->height;
      bitm.valid = true;
      result1->set(bitm);
      //printf("getting webcam data for %s \n", bitm.timestamp);
      loading_done = true;
    }
  }
  void start() {
    printf("Webcam on start\n");
  }
  void stop() {
    printf("Webcam on stop\n");
  }

  void on_delete()
  {
    printf("Webcam on delete\n");
    release_webcam();
    //delete[] bitm.data;
  }
};


#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}

vsx_module* create_new_module(unsigned long module) {
  return (vsx_module*)(new module_video_webcam);
}

void destroy_module(vsx_module* m,unsigned long module) {
  delete (module_video_webcam*)m;
}

unsigned long get_num_modules() {
  return 1;
}