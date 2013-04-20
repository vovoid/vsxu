/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "vsx_gl_global.h"
#include "vsx_math_3d.h"
#include "vsx_param.h"
#include "vsx_module.h"

#include <vfw.h>
class module_bitmap_avi : public vsx_module {
  // in
  vsx_module_param_string *filename;
  vsx_module_param_float *time;
  vsx_module_param_int *drive;
	
	// out
	vsx_module_param_bitmap* result1;
	// internal
	bool need_to_rebuild;
	
	vsx_bitmap bitm;

  PAVIFILE m_aviFile;
  PAVISTREAM streamVid;
  PGETFRAME getFrame;
  DWORD dwRate;
  DWORD dwScale;

	int bitm_timestamp;
  int width, height;
  int num_frames; 
  int current_frame;
  int previous_frame;
public:

  
  vsx_bitmap*       work_bitmap;

  
  module_bitmap_avi()
  {
    m_aviFile = NULL;
    getFrame = NULL;
    streamVid = NULL;
    width = 0;
    height = 0;
    num_frames = 0;
    dwRate = 0;
    dwScale = 0;
    current_frame=0;
  }

  int load_avi_file()
  {
    bitm.valid=false;
    const char *avifilename = filename->get().c_str();
    HRESULT res = AVIFileOpen(&m_aviFile, avifilename, OF_READ, NULL);
    if (res!=AVIERR_OK)
    {
      printf("Couldn't open avi file %s\n",filename->get().c_str());
      return 0;
    }

    res = AVIFileGetStream(m_aviFile, &streamVid, streamtypeVIDEO, 0);
    if (res!=AVIERR_OK)
    {
      AVIFileRelease(m_aviFile);
      m_aviFile = NULL;
      streamVid = NULL;
      printf("Couldn't get stream");
      return 0;
    }

    LONG format_length = 0;

    AVIStreamReadFormat(streamVid,0,NULL,&format_length);

    //if format_data is not a reasonable size, fail
    if (format_length>128)
    {
      printf("Format data too big");
      return 0;
    }
    //make room for at least 128 bytes, sizeof(int) aligned
    int format_data[(128/sizeof(int)) + 1];

    AVIStreamReadFormat(streamVid,0,format_data,&format_length);
        
    BITMAPINFOHEADER *bi = (BITMAPINFOHEADER *)format_data;

    //only 24 bit output is supported
    if (bi->biBitCount!=24)
    {
      printf("Bitcount %d not supported",bi->biBitCount);
      return 0;
    }
    
    // Create the PGETFRAME
    getFrame = AVIStreamGetFrameOpen(streamVid,NULL);
	
    //unable to decode the .avi?
    if (getFrame==NULL)
    {
      printf("AVIStreamGetFrameOpen returned NULL");
      return 0;
    }
        
    // Define the length of the video (necessary for loop reading)
		// and its size.
		num_frames = AVIStreamLength(streamVid);
      
    if (num_frames<1)
    {
      printf("Zero frames");
      return 0;
    }
    
    AVISTREAMINFO psi;
    AVIStreamInfo(streamVid, &psi, sizeof(AVISTREAMINFO));

    width  = psi.rcFrame.right - psi.rcFrame.left;
		height = psi.rcFrame.bottom - psi.rcFrame.top;

    dwRate = psi.dwRate;
    dwScale = psi.dwScale;

    bitm.bpp=(int)(bi->biBitCount/8);
    bitm.bformat=GL_BGR;
    bitm.size_x=width;
    bitm.size_y=height;
    bitm.valid=true;

    return 1;
  }

  double get_frame_rate() {return (double)dwRate / (double)dwScale;}

  void *get_frame_data(int frame_num)
  {
    if (num_frames<=0)
      return NULL;

    if (frame_num<0)
      frame_num = (frame_num % num_frames) + num_frames;
    else
      if (frame_num>=num_frames)
        frame_num = frame_num % num_frames;

  	LPBITMAPINFOHEADER lpbi = NULL;
	
		lpbi = (LPBITMAPINFOHEADER)AVIStreamGetFrame(getFrame, frame_num);
									
  	if (lpbi)
	  {
      //frame pixel data is 40 bytes past the bitmapinfoheader
      void *bitmap = ((unsigned char *)lpbi) + 40; 

      return bitmap;
    }

    return NULL;
  }

  void release_avi()
  {
    if (getFrame != NULL)
    {
      try
      {
        AVIStreamGetFrameClose(getFrame);
      }
      catch (...)
      {
        printf("AVIStreamGetFrameClose error");
      }
      getFrame = NULL;
	  }

    if (streamVid != NULL)
    {
      try
      {
        AVIStreamRelease(streamVid);
      }
      catch (...)
      {
        printf("AVIStreamRelease error");
      }
      streamVid = NULL;
    }

    if (m_aviFile!=NULL)
    {
      try
      {
        AVIFileRelease(m_aviFile);
      }
      catch (...)
      {
        printf("AVIFileRelease error");
      }
      m_aviFile = NULL;
    }
  }

  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "filename:resource,drive:enum?engine_time|parameter,time:float";
      info->identifier = "bitmaps;loaders;avi";
      info->out_param_spec = "bitmap:bitmap";
      info->component_class = "bitmap";
    info->description = "Plays AVI video";
  }
  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {

    filename = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"filename");
    filename->set("");
    //load_avi_file();
    drive = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"drive");
    drive->set(0);
    time = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time");

  	result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    result1->set_p(bitm);
    work_bitmap = &bitm;
    bitm.data = 0;
    bitm.bpp = 3;
    bitm.bformat = GL_RGBA;
    bitm.valid = false;
    bitm_timestamp = bitm.timestamp = rand();
    need_to_rebuild = true;
    previous_frame = -1;
  }
  
  void param_set_notify(const vsx_string& name) 
  {
    if (name=="filename")
      load_avi_file();
  }
  
  
  void run()
  {
  	if (drive->get() == 0)
    current_frame = (int)(engine->vtime * get_frame_rate());
    else
    current_frame = (int)(time->get() * get_frame_rate());
    if (current_frame != previous_frame) {
      //printf("r-");
      bitm.data=(long unsigned int*)get_frame_data(current_frame);
      bitm.timestamp++;
      previous_frame = current_frame;
     // printf("t-");
    }
    if (bitm.valid && bitm_timestamp != bitm.timestamp)
    {
      bitm_timestamp = bitm.timestamp;
      result1->set(bitm);
      loading_done = true;
    }
    if (current_frame>num_frames) current_frame=0;
  }
  void start() {}  
  
  void stop() {}
  
  void on_delete()
  {
    release_avi();
    delete[] bitm.data;
  }
};




extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}
 
vsx_module* create_new_module(unsigned long module) {
  return (vsx_module*)(new module_bitmap_avi);
}

void destroy_module(vsx_module* m,unsigned long module) {
  delete (module_bitmap_avi*)m;
}

unsigned long get_num_modules() {
  return 1;
}  



