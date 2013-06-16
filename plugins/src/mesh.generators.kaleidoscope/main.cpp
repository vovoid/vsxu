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


#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"

#ifndef VSXU_OPENGL_ES
class vsx_module_kaleido_star : public vsx_module
{
  // in
	vsx_module_param_float* hemispheric;
	vsx_module_param_texture* tex_a;
	//vsx_module_param_texture* tex_b;
	// out
	vsx_module_param_render* render_result;
	// internal
  vsx_vector cm;
  GLuint dlist;
  bool list_built;
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;vovoid;kaleidoscope";
  info->in_param_spec = "\
texture_a:texture,\
hemispheric:float";

  info->out_param_spec = "render_out:render";
  info->component_class = "render";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  list_built = false;
  hemispheric = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"hemispheric");
  hemispheric->set(0.0f);
  tex_a = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_a");
  //tex_b = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_b");
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  render_result->set(0);
  loading_done = true;
}

void output(vsx_module_param_abs* param) {
  VSX_UNUSED(param);
  if (tex_a) {

  	vsx_texture** ta;
  	ta = tex_a->get_addr();
    if (!ta) {
      render_result->set(0);
      return;
    }

  glDisable(GL_DEPTH_TEST);

  if (param_updates)
  {
    if (list_built)
    glDeleteLists(dlist,1);
    list_built = false;
    param_updates = 0;
  }
  (*ta)->bind();

  if (!list_built) {

    dlist = glGenLists(1);
    glNewList(dlist,GL_COMPILE);

  	static const int nRings = 6;
  	static const int nSectors = 16;
  	static const int tessU = 4, tessV = 4;
  	int i, j;

  	glColor3f(1, 1, 1);
    float hemispheric_v = hemispheric->get();

  	glBegin(GL_TRIANGLES);
  	for(j = 0; j < nRings; j++)
      for(i = 0; i < nSectors; i++)
      {
  			float dist0  =  (float)j      * (1.0f   / nRings);
  			float angle0 =  (float)i      * (2.0f * (float)PI / nSectors);
  			float dist1  = ((float)j + 1.0f) * (1.0f   / nRings);
  			float angle1 = ((float)i + 1.0f) * (2.0f * (float)PI / nSectors);

  			float uFact = j&1?1:-1;
  			float vFact = i&1?1:-1;
  			float uOffs = j&1?0:1;
  			float vOffs = i&1?0:1;

  			for(int k = 0; k < tessV; k++)
  				for(int l = 0; l < tessU; l++) {
  					float u0 = (float)l / tessU;
  					float v0 = (float)k / tessV;
  					float u1 = (float)(l + 1) / tessU;
  					float v1 = (float)(k + 1) / tessV;
  					float d0 = dist0 * (1 - u0) + dist1 * u0;
  					float a0 = angle0 * (1 - v0) + angle1 * v0;
  					float d1 = dist0 * (1 - u1) + dist1 * u1;
  					float a1 = angle0 * (1 - v1) + angle1 * v1;

  					float x0 = (float)cos(a0) * d0;
  					float y0 = (float)sin(a0) * d0;
  					float x1 = (float)cos(a0) * d1;
  					float y1 = (float)sin(a0) * d1;
  					float x2 = (float)cos(a1) * d1;
  					float y2 = (float)sin(a1) * d1;
  					float x3 = (float)cos(a1) * d0;
  					float y3 = (float)sin(a1) * d0;

  					u0 = u0 * uFact + uOffs;
  					v0 = v0 * vFact + vOffs;
  					u1 = u1 * uFact + uOffs;
  					v1 = v1 * vFact + vOffs;

  					float r0 = (float)sqrt(fabs(1.0f-(x0*x0+y0*y0)))*hemispheric_v;
  					float r2 = (float)sqrt(fabs(1.0f-(x2*x2+y2*y2)))*hemispheric_v;

  					glTexCoord2f(u0, v0/*d0*/);	glVertex3f(x0, y0, r0);
  					glTexCoord2f(u1, v0/*d1*/);	glVertex3f(x1, y1,(float)sqrt(fabs(1.0f-(x1*x1+y1*y1)))*hemispheric_v);
  					glTexCoord2f(u1, v1/*d1*/);	glVertex3f(x2, y2, r2);


  					glTexCoord2f(u1, v1/*d1*/);	glVertex3f(x2, y2,r2);
  					glTexCoord2f(u0, v1/*d0*/);	glVertex3f(x3, y3,(float)sqrt(fabs(1.0f-(x3*x3+y3*y3)))*hemispheric_v);
  					glTexCoord2f(u0, v0/*d0*/);	glVertex3f(x0, y0,r0);
  				}
  		}
  	glEnd();
  	glEndList();
    list_built = true;
  }
  else
  {
    glCallList(dlist);
  }

	(*ta)->_bind();

  }
  else render_result->set(0);

}
};
#endif


const int nRings = 6;
const int nSectors = 16;
const int tessU = 4, tessV = 4;
const int facecount = nRings*nSectors*tessV*tessU;

class vsx_module_kaleido_mesh : public vsx_module {
  //in
  vsx_module_param_float* hemispheric;
	// out
	vsx_module_param_mesh* mesh_result;
	// internal
	vsx_mesh* mesh;

public:

  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;generators;vovoid;kaleido_mesh";
    info->in_param_spec = "hemispheric:float";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
    loading_done = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    hemispheric = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"hemispheric");
    hemispheric->set(0.0f);
    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }

  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

void run() {

  if (mesh->data->vertices.get_used() && hemispheric->updates == 0) {
    // re-allocate
  } else {
    // just allocate
    hemispheric->updates = 0;
    unsigned long vert_pos = 0;
    unsigned long face_pos = 0;
  	int i, j;
  	float hemispheric_v = hemispheric->get();

  	for(j = 0; j < nRings; j++)
  		for(i = 0; i < nSectors; i++) {
  			float dist0  =  (float)j      * (1.0f   / nRings);
  			float angle0 =  (float)i      * (2.0f * (float)PI / nSectors);
  			float dist1  = ((float)j + 1.0f) * (1.0f   / nRings);
  			float angle1 = ((float)i + 1.0f) * (2.0f * (float)PI / nSectors);

  			float uFact = j&1?1:-1;
  			float vFact = i&1?1:-1;
  			float uOffs = j&1?0:1;
  			float vOffs = i&1?0:1;

  			for(int k = 0; k < tessV; k++)
  				for(int l = 0; l < tessU; l++) {
  					float u0 = (float)l / tessU;
  					float v0 = (float)k / tessV;
  					float u1 = (float)(l + 1) / tessU;
  					float v1 = (float)(k + 1) / tessV;
  					float d0 = dist0 * (1 - u0) + dist1 * u0;
  					float a0 = angle0 * (1 - v0) + angle1 * v0;
  					float d1 = dist0 * (1 - u1) + dist1 * u1;
  					float a1 = angle0 * (1 - v1) + angle1 * v1;

  					float x0 = (float)cos(a0) * d0;
  					float y0 = (float)sin(a0) * d0;
  					float x1 = (float)cos(a0) * d1;
  					float y1 = (float)sin(a0) * d1;
  					float x2 = (float)cos(a1) * d1;
  					float y2 = (float)sin(a1) * d1;
  					float x3 = (float)cos(a1) * d0;
  					float y3 = (float)sin(a1) * d0;

  					u0 = u0 * uFact + uOffs;
  					v0 = v0 * vFact + vOffs;
  					u1 = u1 * uFact + uOffs;
  					v1 = v1 * vFact + vOffs;

            float r0 = (float)sqrt(fabs(1.0f-(x0*x0+y0*y0)))*hemispheric_v;
            float r2 = (float)sqrt(fabs(1.0f-(x2*x2+y2*y2)))*hemispheric_v;


  					mesh->data->vertex_tex_coords[vert_pos].s = u0;
						mesh->data->vertex_tex_coords[vert_pos].t = v0;
						mesh->data->vertices[vert_pos].x = x0;
						mesh->data->vertices[vert_pos].y = y0;
						mesh->data->vertices[vert_pos].z = r0;
      			++vert_pos;
  					mesh->data->vertex_tex_coords[vert_pos].s = u1;
						mesh->data->vertex_tex_coords[vert_pos].t = v0;
						mesh->data->vertices[vert_pos].x = x1;
						mesh->data->vertices[vert_pos].y = y1;
						mesh->data->vertices[vert_pos].z = (float)sqrt(fabs(1.0f-(x1*x1+y1*y1)))*hemispheric_v;
      			++vert_pos;
  					mesh->data->vertex_tex_coords[vert_pos].s = u1;
						mesh->data->vertex_tex_coords[vert_pos].t = v1;
						mesh->data->vertices[vert_pos].x = x2;
						mesh->data->vertices[vert_pos].y = y2;
						mesh->data->vertices[vert_pos].z = r2;
      			++vert_pos;

      			mesh->data->faces[face_pos].a = vert_pos-3;
      			mesh->data->faces[face_pos].b = vert_pos-2;
      			mesh->data->faces[face_pos].c = vert_pos-1;
      			++face_pos;

      			//------------------------------------------------------

  					mesh->data->vertex_tex_coords[vert_pos].s = u1;
						mesh->data->vertex_tex_coords[vert_pos].t = v1;
						mesh->data->vertices[vert_pos].x = x2;
						mesh->data->vertices[vert_pos].y = y2;
						mesh->data->vertices[vert_pos].z = r2;
      			++vert_pos;
  					mesh->data->vertex_tex_coords[vert_pos].s = u0;
						mesh->data->vertex_tex_coords[vert_pos].t = v1;
						mesh->data->vertices[vert_pos].x = x3;
						mesh->data->vertices[vert_pos].y = y3;
						mesh->data->vertices[vert_pos].z = (float)sqrt(fabs(1.0f-(x3*x3+y3*y3)))*hemispheric_v;
      			++vert_pos;
  					mesh->data->vertex_tex_coords[vert_pos].s = u0;
						mesh->data->vertex_tex_coords[vert_pos].t = v0;
						mesh->data->vertices[vert_pos].x = x0;
						mesh->data->vertices[vert_pos].y = y0;
						mesh->data->vertices[vert_pos].z = r0;
      			++vert_pos;

      			mesh->data->faces[face_pos].a = vert_pos-3;
      			mesh->data->faces[face_pos].b = vert_pos-2;
      			mesh->data->faces[face_pos].c = vert_pos-1;
            ++face_pos;

				}
 		  loading_done = true;
 		  mesh->timestamp++;
		}
    mesh_result->set(mesh);
  }
}
};

//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch (module) {
    case 0: return (vsx_module*)(new vsx_module_kaleido_star);
    case 1: return (vsx_module*)(new vsx_module_kaleido_mesh);
  } // switch
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_kaleido_star*)m; break;
    case 1: delete (vsx_module_kaleido_mesh*)m; break;
  }
}

unsigned long get_num_modules() {
  return 2;
}
