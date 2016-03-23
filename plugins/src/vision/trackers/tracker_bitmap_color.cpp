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
#include "tracker_bitmap_color.h"

#define FILTER_NONE 0
#define FILTER_HSV 1
#define FILTER_HSV_THRESHOLD 2
#define FILTER_HSV_THRESHOLD_RGB 3
#include <bitmap/vsx_bitmap.h>

tracker_bitmap_color::tracker_bitmap_color():
  m_previousTimestamp(0),
  m_compute_debug_out(false),
  vsx_module()
{
  m_img[FILTER_NONE] = 0;
  m_img[FILTER_HSV] = 0;
  m_img[FILTER_HSV_THRESHOLD] = 0;
  m_img[FILTER_HSV_THRESHOLD_RGB] = 0;
  m_moments = (CvMoments*)malloc(sizeof(CvMoments));
}

tracker_bitmap_color::~tracker_bitmap_color()
{
  free(m_moments);
}

void tracker_bitmap_color::initialize_buffers( int w, int h )
{
  //Do nothing, If the previous image and input image are same as the dimensions,
  if(m_img[FILTER_NONE] && m_img[FILTER_NONE]->width == w && m_img[FILTER_NONE]->height == h) return;
  else release_buffers();

  //for a camera input, assume the image is composed of 8bit RGB pixels.
  //Also for the input image, the just the headers are sufficient.
  m_img[FILTER_NONE] = cvCreateImageHeader(cvSize(w,h),8,3);

  m_img[FILTER_HSV] = cvCreateImage(cvSize(w,h),8,3);
  m_img[FILTER_HSV_THRESHOLD] = cvCreateImage(cvSize(w,h),8,1);
  m_img[FILTER_HSV_THRESHOLD_RGB] = cvCreateImage(cvSize(w,h),8,3);
}

void tracker_bitmap_color::on_delete()
{
  release_buffers();
}


void tracker_bitmap_color::release_buffers()
{
  //allocates buffers if needed
  if(!m_img[FILTER_NONE] || !m_img[FILTER_HSV] || !m_img[FILTER_HSV_THRESHOLD] || !m_img[FILTER_HSV_THRESHOLD_RGB])
    return;

  cvReleaseImageHeader(&m_img[FILTER_NONE]);
  cvReleaseImage(&m_img[FILTER_HSV]);
  cvReleaseImage(&m_img[FILTER_HSV_THRESHOLD]);
  cvReleaseImage(&m_img[FILTER_HSV_THRESHOLD_RGB]);

  m_img[FILTER_NONE] = 0;
  m_img[FILTER_HSV] = 0;
  m_img[FILTER_HSV_THRESHOLD] = 0;
  m_img[FILTER_HSV_THRESHOLD_RGB] = 0;
}


void tracker_bitmap_color::module_info(vsx_module_specification* info)
{
  info->identifier = "vision;trackers;bitmap_color_tracker";
  info->description = "Tracks the centroid of a colored blob in the input bitmap.\n\
Specify the ranges of the input color in the HSV scale using color1, color2.";
  info->out_param_spec = "blob_position:float3,debug_output:bitmap";
  
  //info->out_param_spec = "blob_position:float3";
  info->in_param_spec = "bitmap:bitmap,color1:float3,color2:float3";
  info->component_class = "parameters"; //TODO: Add a new component_class for vision objects
}

void tracker_bitmap_color::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  in_bitmap = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  in_color1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"color1");
  in_color2 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"color2");

  out_centroid = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"blob_position");
  out_debug = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"debug_output");

  out_centroid->set(0,0);
  out_centroid->set(0,1);
  out_centroid->set(0,2);

  out_debug->set_p(m_debug);
  loading_done = true;
}

void tracker_bitmap_color::run()
{
  vsx_bitmap *bmp = in_bitmap->get_addr();

  //Check if there is any new image to process
  if(!(bmp && bmp->valid && bmp->timestamp && bmp->timestamp != m_previousTimestamp)){
#ifdef VSXU_DEBUG
    printf("Skipping frame after %d \n",m_previousTimestamp);
#endif
    return;
  }

  m_previousTimestamp = bmp->timestamp;  
  initialize_buffers(bmp->size_x, bmp->size_y);

  //Grab the input image
  m_img[FILTER_NONE]->imageData = (char*)bmp->data;

  //1)filter the image to the HSV color space
  cvCvtColor(m_img[FILTER_NONE],m_img[FILTER_HSV],CV_RGB2HSV);

  //2Threshold the image based on the supplied range of colors
  cvInRangeS( m_img[FILTER_HSV],
              cvScalar( (int)(in_color1->get(0)*255), (int)(in_color1->get(1)*255), (int)(in_color1->get(2)*255) ),
              cvScalar( (int)(in_color2->get(0)*255), (int)(in_color2->get(1)*255), (int)(in_color2->get(2)*255) ),
              m_img[FILTER_HSV_THRESHOLD] );

  //3)Now the math to find the centroid of the "thresholded image"
  //3.1)Get the moments
  cvMoments(m_img[FILTER_HSV_THRESHOLD],m_moments,1);
  double moment10 = cvGetSpatialMoment(m_moments,1,0);
  double moment01 = cvGetSpatialMoment(m_moments,0,1);
  double area = cvGetCentralMoment(m_moments,0,0);

  //3.2)Calculate the positions
  double posX =  moment10/area;
  double posY = moment01/area;

  //3.3) Normalize the positions
  posX = posX/bmp->size_x;
  posY = posY/bmp->size_y;

  //Finally set the result
#ifdef VSXU_DEBUG
  printf("Position: (%f,%f)\n",posX,posY);
#endif
  out_centroid->set(posX,0);
  out_centroid->set(posY,1);

  //Calculate the debug output only if requested
  if(m_compute_debug_out){
    m_compute_debug_out = false;
    cvCvtColor(m_img[FILTER_HSV_THRESHOLD],m_img[FILTER_HSV_THRESHOLD_RGB], CV_GRAY2RGB);

    m_debug = *bmp;
    m_debug.data = m_img[FILTER_HSV_THRESHOLD_RGB]->imageData;
    out_debug->set_p(m_debug);
  }
}

void tracker_bitmap_color::output(vsx_module_param_abs* param)
{
  if(param->name == "debug_output")
    m_compute_debug_out = true;
}
