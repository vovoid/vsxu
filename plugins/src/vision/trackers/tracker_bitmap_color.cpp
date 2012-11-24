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

#include <cv.h>
#include "tracker_bitmap_color.h"


void tracker_bitmap_color::module_info(vsx_module_info* info)
{
  info->identifier = "vision;trackers;bitmap_color_tracker";
  info->description = "Tracks the centroid of a colored blob in the input bitmap.\nSpecify the ranges of the input color in the HSV scale using color1, color2.";
  //info->out_param_spec = "blob_position:float3,filtered_output:bitmap";
  //TODO: Add a proper debug output which can be disabled
  info->out_param_spec = "blob_position:float3";
  info->in_param_spec = "bitmap:bitmap,color1:float3,color2:float3";
  info->component_class = "parameters"; //TODO: Add a new component_class for vision objects
}

void tracker_bitmap_color::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  in_bitmap = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  in_color1 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"color1");
  in_color2 = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"color2");

  result_position = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"blob_position");
  //filtered_output = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"filtered_output");

  result_position->set(0,0);
  result_position->set(0,1);
  result_position->set(0,2);
  
  m_previousTimestamp = 0;
  //filtered_output->set_p(m_bitm);
  loading_done = true;
}

void tracker_bitmap_color::run()
{
  vsx_bitmap *bmp = in_bitmap->get_addr();

  //Check if there is any new image to process
  if(!bmp || !bmp->valid ||!bmp->timestamp || bmp->timestamp == m_previousTimestamp){
    printf("Skipping frame\n");
    return;
  }

  m_previousTimestamp = bmp->timestamp;

  //for a camera input, we currently assume the image is composed of 8bit RGB pixels and reconstruct the OpenCV image
  IplImage *img_input, *img_HSV, *img_threshold;
  img_input = cvCreateImageHeader(cvSize(bmp->size_x,bmp->size_y),8,3);
  img_input->imageData = (char*)bmp->data;

  img_HSV = cvCreateImage(cvSize(bmp->size_x,bmp->size_y),8,3);
  img_threshold = cvCreateImage(cvSize(bmp->size_x,bmp->size_y),8,1);

  //Get the image in HSV color space
  cvCvtColor(img_input,img_HSV,CV_RGB2HSV);

  //Threshold the image based on the supplied range of colors
  cvInRangeS( img_HSV,
              cvScalar( (int)(in_color1->get(0)*255), (int)(in_color1->get(1)*255), (int)(in_color1->get(2)*255) ),
              cvScalar( (int)(in_color2->get(0)*255), (int)(in_color2->get(1)*255), (int)(in_color2->get(2)*255) ),
              img_threshold );

  //keep the image for debugging purposes
  //cvCvtColor(img_threshold,img_HSV, CV_GRAY2RGB);
  //Now the math
  //1)Get the moments
  CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
  cvMoments(img_threshold,moments,1);
  double moment10 = cvGetSpatialMoment(moments,1,0);
  double moment01 = cvGetSpatialMoment(moments,0,1);
  double area = cvGetCentralMoment(moments,0,0);

  //2)Calculate the positions
  double posX =  moment10/area;
  double posY = moment01/area;

  //3) Normalize the positions
  posX = posX/bmp->size_x;
  posY = posY/bmp->size_y;

  //Finally set the result
  printf("Position: (%f,%f)\n",posX,posY);
  result_position->set(posX,0);
  result_position->set(posX,1);

  /*
  m_bitm = *bmp;
  m_bitm.data = img_HSV->imageData;;
  filtered_output->set_p(m_bitm);
  //loading_done = true;
  */

  //Release the allocated resources
  //TODO : Allocate and release resources on demand/only when the data changes.
  //       else reuse the previously allocated structures.
  delete moments;
  cvReleaseImageHeader(&img_input);
  cvReleaseImage(&img_HSV);
  cvReleaseImage(&img_threshold);
}