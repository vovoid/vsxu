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

#include <vsxfst.h>
#include "module_video_file.h"

module_video_file::module_video_file():
  module_video_input(),
  m_filename(0)
{
  isValid();
}

void module_video_file::module_info(vsx_module_info* info)
{
  info->in_param_spec = "filename:resource";
    info->identifier = "video;file";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
  info->description = "Fetches frame data from a ffmpeg compatible video file as bitmaps.";
}

void module_video_file::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  m_filename = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"filename");
  m_filename->set("");
  result = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  result->set_p(m_bitm);
  previous_frame = -1;
}

void module_video_file::param_set_notify(const vsx_string& name)
{
  if(name == "filename" && isValid()){
    release_capture();
    init();
  }
}

bool module_video_file::isValid()
{
  if(m_filename){
    // The module is valid if the file exists.
    if (FILE *f = fopen((vsx_get_data_path() + m_filename->get()).c_str(), "r")){
      printf("Opening %s ...\n", (vsx_get_data_path() + m_filename->get()).c_str());
      m_isValid = true;
      fclose(f);
    }
    else printf( "Cannot open %s !!\n", (vsx_get_data_path() + m_filename->get()).c_str() );
  }
  else m_isValid = false;
  return m_isValid;
}

void* module_video_file::worker()
{
  // Create a fresh capture device
  CvCapture* capture = cvCaptureFromFile((vsx_get_data_path() + m_filename->get()).c_str());
  bool running = true, invalidFrame = false;
  int nextBuffer;
  IplImage *frame;

  // To maintain the needed fps
  double delay = 1000000.0/cvGetCaptureProperty(capture,CV_CAP_PROP_FPS);

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
      running = m_isRunning;
      m_currentBuffer = nextBuffer;
      //When the module is asked to be deleted or if the frame cannot be captured,
      //invalidate the current frame so that it wont be read in the main thread
      if( (!running) || invalidFrame ){
        m_currentBuffer = -1;
        invalidFrame = false;
      }
    pthread_mutex_unlock(&m_mutex);
    usleep(delay);
  }

  //Cleanup
  cvReleaseCapture(&capture);
  freeBuffers();
}