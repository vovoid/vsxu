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

#include <filesystem/vsx_filesystem.h>
#include <highgui.h>
#include "input_video_file.h"
#include "vsx_data_path.h"

input_video_file::input_video_file():
  module_video_input(),
  m_filename(0)
{
}

void input_video_file::module_info(vsx_module_specification* info)
{
  info->in_param_spec = "filename:resource";
    info->identifier = "vision;input;video_file_input";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
  info->description = "Fetches frame data from a ffmpeg compatible video file as bitmaps.";
}

void input_video_file::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  m_filename = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"filename");
  m_filename->set("");
  m_result = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  m_result->set_p(m_bitm);
}

void input_video_file::param_set_notify(const vsx_string<>& name)
{
  if(name == "filename" && isValid()){
    release_capture();
    init();
  }
}

bool input_video_file::isValid()
{
  if( !m_filename || m_filename->get() == "")
    return false;

  if (FILE *f = fopen(( vsx_data_path::get_instance()->data_path_get() + m_filename->get()).c_str(), "r")){
    fclose(f);
    return true;
  }

  printf( "Cannot open %s !!\n", ( vsx_data_path::get_instance()->data_path_get() + m_filename->get()).c_str() );
  return false;
}

void input_video_file::worker()
{
  // Create a fresh capture device
  CvCapture* capture = cvCaptureFromFile(( vsx_data_path::get_instance()->data_path_get() + m_filename->get()).c_str());
  if(!capture || currentTask() != INITIALIZE_CAPTURE ){
    message = "module||ERROR! Cannot initialize video file reader!!";
    addTask(CLEANUP_CAPTURE);
    return;
  }
  else message = "";

  IplImage *frame;
  Tasks task = INITIALIZE_CAPTURE;
  //double delay = 1000000.0/cvGetCaptureProperty(capture,CV_CAP_PROP_FPS); //To maintain the needed fps

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
