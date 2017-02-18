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
#include <bitmap/vsx_bitmap.h>

class module_bitmap_to_particlesystem : public vsx_module
{
public:

  // in
  vsx_module_param_bitmap* bitmap_in;
  vsx_module_param_float* size_in;
  vsx_module_param_float* blob_size_in;
  vsx_module_param_float* random_weight_in;

  // out
  vsx_module_param_particlesystem* particlesystem_out;

  // internal
  vsx_bitmap* bitmap;
  int bitm_timestamp;

  vsx_particlesystem<> particles;

  int p_updates;
  bool first;

  void module_info(vsx_module_specification* info)
  {
    info->identifier = "particlesystems;generators;bitmap2particlesystem";
    info->in_param_spec =
      "bitmap_in:bitmap,"
      "size:float,"
      "blobsize:float,"
      "random_weight:float"
    ;
    info->out_param_spec = "particlesystem_out:particlesystem";
    info->component_class = "particlesystem";
    info->description = "";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {

    particles.timestamp = 0;
    particles.particles = new vsx_ma_vector< vsx_particle<> >;
    loading_done = true;
    first = true;
    p_updates = param_updates;

    //--------------------------------------------------------------------------------------------------

    bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap_in");

    size_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"size");
    size_in->set(5.0f);

    blob_size_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"blobsize");
    blob_size_in->set(0.1f);

    random_weight_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"random_weight");
    random_weight_in->set(0.5f);

    //--------------------------------------------------------------------------------------------------

    particlesystem_out = (vsx_module_param_particlesystem*)out_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particlesystem_out");
    particlesystem_out->set_p(particles);
  }

  void param_set_notify(const vsx_string<>& name)
  {
    VSX_UNUSED(name);
    first = true;
  }

  void run()
  {
    if (!bitmap_in->get_addr())
      return;

    bitmap = *(bitmap_in->get_addr());

    if (p_updates != param_updates)
    {
      first = true;
      p_updates = param_updates;
    }

    unsigned int i = 0;
    unsigned int width = bitmap->width;
    float space = size_in->get()/(float)width;
    float dest = -size_in->get()*0.5f;
    if (first) {
      for (size_t y = 0; y < bitmap->height; ++y)
      {
        for (size_t x = 0; x < width; ++x)
        {
          (*particles.particles)[i].pos.x = dest+(float)x*space+random_weight_in->get()*(-0.5f+(float)(rand()%1000)/1000.0f);
          (*particles.particles)[i].pos.y = 0;
          (*particles.particles)[i].pos.z = dest+(float)y*space+random_weight_in->get()*(-0.5f+(float)(rand()%1000)/1000.0f);
          ++i;
        }
      }
      first = false;
    }

    i = 0;
    if (bitmap->storage_format == vsx_bitmap::byte_storage && bitmap->channels == 4)
    {
      for (size_t y = 0; y < bitmap->height; ++y)
      {
        size_t cc = y*width;
        for (size_t x = 0; x < width; ++x)
        {
          (*particles.particles)[i].color.b = ((float)(unsigned char)((((uint32_t*)bitmap->data_get())[cc+x]&0x00FF0000)>>16))/255.0f;
          (*particles.particles)[i].color.g = ((float)(unsigned char)((((uint32_t*)bitmap->data_get())[cc+x]&0x0000FF00)>>8))/255.0f;
          (*particles.particles)[i].color.r = ((float)(unsigned char)((((uint32_t*)bitmap->data_get())[cc+x]&0x000000FF)))/255.0f;
          (*particles.particles)[i].color.a = 1.0f;
          if ((*particles.particles)[i].color.b < 0.01f && (*particles.particles)[i].color.g < 0.01f && (*particles.particles)[i].color.r < 0.01f)
          (*particles.particles)[i].size = 0.0f;
          else
          (*particles.particles)[i].size = (*particles.particles)[i].orig_size = blob_size_in->get();

          (*particles.particles)[i].speed.x = 0;
          (*particles.particles)[i].speed.y = 0;
          (*particles.particles)[i].speed.z = 0;
          (*particles.particles)[i].time = 0;
          (*particles.particles)[i].lifetime = 1000000000;
          ++i;
        }
      }
    }
    particlesystem_out->set_p(particles);
  }

  ~module_bitmap_to_particlesystem()
  {
    delete particles.particles;
  }

};

