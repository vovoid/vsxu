/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"
#include "vsx_quaternion.h"

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------


typedef struct  {
  vsx_quaternion pos;
  float speed;
} star_line;

class star_worm {
public:
  float size;
  vsx_color color;
  vsx_quaternion phs;
  vsx_quaternion freq;
  vsx_avector_nd<star_line> lines;
  star_worm() {
    for (int i = 0; i < 20; ++i) {
      star_line line;
      line.pos.x = 0;
      line.pos.y = 0;
      line.pos.z = 0;
      line.pos.w = 1;
      line.speed = (float)(i+3)*0.1f;//(rand()%10000)*0.0001f*20.0f;

      lines.push_back(line);

    }
  }
};

class vsx_module_mesh_star : public vsx_module {
  // in
	vsx_module_param_float_array* paths;

	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	bool first_run;

	float trail_length;

	vsx_avector<star_worm*> worms;
	vsx_float_array* spectrum;

public:

  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;particles;mesh_star";
    info->description = "";
    info->in_param_spec = "paths:float_array";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
  	paths = (vsx_module_param_float_array*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"paths");
  	result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
  }

	void run()
	{
    if (first_run) {
      for (int j = 0; j < 100; ++j) {
        star_worm* my_worm = new star_worm;
        my_worm->size = 0.0f;
        my_worm->freq.x = (rand()%10000)*0.0001-0.5;
        my_worm->freq.y = (rand()%10000)*0.0001-0.5;
        my_worm->freq.z = (rand()%10000)*0.0001-0.5;
        my_worm->freq.w = (rand()%10000)*0.0001-0.5;

        my_worm->phs.x = (rand()%10000)*0.0001-0.5;
        my_worm->phs.y = (rand()%10000)*0.0001-0.5;
        my_worm->phs.z = (rand()%10000)*0.0001-0.5;
        my_worm->phs.w = (rand()%10000)*0.0001-0.5;
        my_worm->color = vsx_color__((rand()%10000)*0.0001,(rand()%10000)*0.0001,(rand()%10000)*0.0001,0.5f);
//        vsx_quaternion quat;
//        quat.x = sin(worms[j]->freq.x*engine->vtime+worms[j]->phs.x);
//        quat.y = sin(worms[j]->freq.y*engine->vtime+worms[j]->phs.y);
//        quat.z = sin(worms[j]->freq.z*engine->vtime+worms[j]->phs.z);
//        quat.w = sin(worms[j]->freq.w*engine->vtime+worms[j]->phs.w);
        //for (int k = 0; k < worms[j]->lines.size(); ++k) {
//          worms[j]->lines[k].pos=quat;
//        }

        /*for (int k = 0; k < my_worm->lines.size(); ++k) {
          my_worm->lines.pos
        }*/
        worms.push_back(my_worm);
      }
      first_run = false;
    }
    spectrum = paths->get_addr();
    vsx_quaternion quat;
    int jj = 0;
    for (unsigned long i = 0; i < worms.size(); ++i) {
      //float size = 1.0f;
      //if (spectrum) {
//        size = (*(spectrum->data))[i*(spectrum->data->size()/worms.size()-1)]+0.04f;
        //size = (*(spectrum->data))[i*2];
      //}
      quat.x = sin(worms[i]->freq.x*engine->vtime+worms[i]->phs.x);
      quat.y = sin(worms[i]->freq.y*engine->vtime+worms[i]->phs.y);
      quat.z = sin(worms[i]->freq.z*engine->vtime+worms[i]->phs.z);
      quat.w = sin(worms[i]->freq.w*engine->vtime+worms[i]->phs.w);
      quat.normalize();
      //vsx_matrix mat = quat.matrix();
      //mesh->data->vertices[jj] = mat.multiply_vector(vsx_vector__(1,0,0));
      //mesh->data->vertex_colors[jj] = vsx_color__(1.0f,1.0f,1.0f,1.0f);
      mesh->data->vertices[jj] = vsx_vector(0);
      mesh->data->vertex_colors[jj] = vsx_color();
      ++jj;
      for (unsigned long k = 0; k < worms[i]->lines.size(); ++k) {
        vsx_quaternion qq = worms[i]->lines[k].pos;
        float tt = engine->dtime * worms[i]->lines[k].speed;
        //float ttt = engine->dtime;
        worms[i]->lines[k].pos.slerp(qq, quat, tt);
        vsx_matrix mat2 = worms[i]->lines[k].pos.matrix();
        //worms[i]->size = 1.0f;
        //worms[i]->size = worms[i]->size*(1-tt) + size*ttt;
        mesh->data->vertices[jj] = mat2.multiply_vector(vsx_vector(1.0f,0,0));
        mesh->data->vertex_colors[jj] = worms[i]->color;//vsx_color__(0.1f,0.3f,0.1f,1.0f);
        ++jj;
      }
    }
    mesh->timestamp++;

    /*if (first_run || n_rays != (int)num_rays->get()) {
      mesh->data->vertex_tex_coords[0] = vsx_vector__(0,0,0);
      mesh->data->vertices.reset_used();
      mesh->data->faces.reset_used();
      //printf("generating random points\n");
      for (int i = 1; i < (int)num_rays->get(); ++i) {
        mesh->data->vertices[i*2].x = (rand()%10000)*0.0001-0.5;
        mesh->data->vertices[i*2].y = (rand()%10000)*0.0001-0.5;
        mesh->data->vertices[i*2].z = (rand()%10000)*0.0001-0.5;
        mesh->data->vertex_colors[i*2] = vsx_color__(0,0,0,0);
        mesh->data->vertex_tex_coords[i*2] = vsx_vector__(0,1,0);
        mesh->data->vertices[i*2+1].x = (rand()%10000)*0.0001-0.5;
        mesh->data->vertices[i*2+1].y = (rand()%10000)*0.0001-0.5;
        mesh->data->vertices[i*2+1].z = (rand()%10000)*0.0001-0.5;
        mesh->data->vertex_colors[i*2+1] = vsx_color__(0,0,0,0);
        mesh->data->vertex_tex_coords[i*2+1] = vsx_vector__(1,0,0);
        mesh->data->faces[i-1].a = 0;
        mesh->data->faces[i-1].b = i*2;
        mesh->data->faces[i-1].c = i*2+1;
        n_rays = (int)num_rays->get();
      }
      first_run = false;

    } */
    result->set_p(mesh);
  }
};

//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_mesh_star);
  }
  return 0;
}


void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_mesh_star*)m; break;
  }
}


unsigned long get_num_modules() {
  return 1;
}



