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

#include <highgui.h>
#include "input_video_camera.h"

int input_video_camera::count = 0;

input_video_camera::input_video_camera():
  module_video_input()
{
  count++;
  if(!isValid())
    message = "module||ERROR! Only 1 instance of Camera module is currently allowed";
}

bool input_video_camera::isValid()
{
  //FIXME: check for the available number of cameras instead of hardcoding it to 1 instance..
  //STATUS: issue put on hold because of upstream library not supporting it:
  //        https://code.ros.org/trac/opencv/ticket/935
  if (count > 1){
    return false;
  }
  return true;
}

input_video_camera::~input_video_camera()
{
  count--;
}

void input_video_camera::module_info(vsx_module_specification* info)
{
  info->in_param_spec = "";
    info->identifier = "vision;input;video_camera_input";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
  info->description = "Fetches camera data as bitmaps";
}

void input_video_camera::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  m_result = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  m_result->set_p(m_bitm);
}

void input_video_camera::worker()
{
  CvCapture* capture = cvCreateCameraCapture(0);//cvCaptureFromCAM(0);
  if(!capture || currentTask() != INITIALIZE_CAPTURE){
    message = "module||ERROR! Cannot initialize camera!!";
    addTask(CLEANUP_CAPTURE);
    return;
  }
  else message = "";

  IplImage *frame;
  Tasks task = INITIALIZE_CAPTURE;
 
  while( task != TERMINATE_CAPTURE ){

    // Fetch a frame if its asked for / couldnt fetch a previous frame / is the first time.
    if( task == FETCH_FRAME || task == IGNORE_FRAME || task == INITIALIZE_CAPTURE ){
      frame = cvQueryFrame(capture);
      if(frame){
        if( !m_bufferReady )
          initializeBuffers(frame->width,frame->height, frame->depth, frame->nChannels);

        cvConvertImage(frame,m_buffer[nextPage()], CV_CVTIMG_SWAP_RB);
        addTask(CONSUME_FRAME);
      }
      else addTask(IGNORE_FRAME);
    }

    task = currentTask();
  }

  //Cleanup
  cvReleaseCapture(&capture);
  freeBuffers();
  addTask(CLEANUP_CAPTURE);
}