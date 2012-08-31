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


#include "module_video_camera.h"

int module_video_camera::count = 0;

module_video_camera::module_video_camera():
  module_video_input()
{
  count++;
  if(!isValid())
    message = "module||ERROR! Only 1 instance of Camera module is currently allowed";
}

bool module_video_camera::isValid()
{
  //FIXME: check for the available number of cameras instead of hardcoding it to 1 instance..
  //STATUS: issue put on hold because of upstream library not supporting it:
  //        https://code.ros.org/trac/opencv/ticket/935
  if (count > 1){
    m_isValid = false;
  }
  return m_isValid;
}

module_video_camera::~module_video_camera()
{
  count--;
}

void module_video_camera::module_info(vsx_module_info* info)
{
  info->in_param_spec = "";
    info->identifier = "video;camera";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
  info->description = "Fetches camera data as bitmaps";
}

void module_video_camera::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  result = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  result->set_p(m_bitm);
  previous_frame = -1;
}

void* module_video_camera::worker()
{
  CvCapture* capture = cvCreateCameraCapture(0);//cvCaptureFromCAM(0);
  bool running = true, invalidFrame = false;
  int nextBuffer;
  IplImage *frame;

  while(capture && running){
    nextBuffer = (m_currentBuffer+1)%N_BUFFERS;
    frame = cvQueryFrame(capture);

    if(frame){
      if( !m_buffersReady )
        initializeBuffers(frame->width,frame->height, frame->depth, frame->nChannels);

      //Convert the image to the actual form and put it in the next buffer to be read
      cvConvertImage(frame,m_buffer[nextBuffer], CV_CVTIMG_SWAP_RB);
    }
    else invalidFrame = true;

    pthread_mutex_lock(&m_mutex);

      running = m_isRunning; //Should we grab another frame?
      m_currentBuffer = nextBuffer; //Which buffer should we read

      //When the module is asked to be deleted or if the frame cannot be captured,
      //invalidate the current frame so that it wont be read in the main thread
      if( (!running) || invalidFrame){
        m_currentBuffer = -1;
        invalidFrame = false;
      }
    pthread_mutex_unlock(&m_mutex);
  }

  //Cleanup
  cvReleaseCapture(&capture);
  freeBuffers();
}