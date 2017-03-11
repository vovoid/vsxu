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


#include "vsx_param.h"
#include <module/vsx_module.h>
#include <math/quaternion/vsx_quaternion.h>



//----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- DEPTH TEST --------------------------------------------------------
//-----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
class vsx_depth_test : public vsx_module {
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* status;
	// out
	vsx_module_param_render* render_result;
	// internal
public:

	void module_info(vsx_module_specification* info)
	{
  	info->identifier = "!renderers;opengl_modifiers;depth_test";
  	info->description = "Enables or disables depth testing\n\
for all below in this chain.\n\
This is useful when you have a mesh. \n\
If no depth testing the faces will be drawn in the order that they appear\n\
in the mesh which is almost never good. (unless you\n\
have satadding blend mode). However in order to use this\n\
you have to enable the depth mask which can be done\n\
with the sister module to this one - depth_mask.\n\
More hints...\n\
|||\n\
If you have faces that are semi-transarent,\n\
disable depth testing and use the transparency\n\
renderer.\n\
";

  	info->in_param_spec = "render_in:render,status:enum?DISABLED|ENABLED";
  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
  	info->tunnel = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
	  status = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"status");
		render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;
		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	}
	bool activate_offscreen() {
	  // comments?! what are those? :) real coders don't need comments. real coders can read opcodes in a hex editor.
	  if (status->get()) {
	    glEnable(GL_DEPTH_TEST);
	  } else {
	    glDisable(GL_DEPTH_TEST);
	  }  
	  return true;
	}  
	
  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
	  render_result->set(render_in->get());
	}
};



class vsx_module_kaleido_star : public vsx_module
{
  // in
  vsx_module_param_float* hemispheric;
  vsx_module_param_texture* tex_a;
  //vsx_module_param_texture* tex_b;
  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_vector3<> cm;
  GLuint dlist;
  bool list_built;
public:

void module_info(vsx_module_specification* info)
{
  info->identifier = "!renderers;vovoid;kaleidoscope";
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

    vsx_texture<>** ta;
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

        float uFact = j&1?1.0f:-1.0f;
        float vFact = i&1?1.0f:-1.0f;
        float uOffs = j&1?0.0f:1.0f;
        float vOffs = i&1?0.0f:1.0f;

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



//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- DEPTH MASK --------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------

class vsx_depth_mask : public vsx_module {
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* status;
	// out
	vsx_module_param_render* render_result;
	// internal
public:

	void module_info(vsx_module_specification* info)
	{
	  info->identifier = "!renderers;opengl_modifiers;depth_mask";
	  info->description = "Enables/Disables depth buffer writing.\nSee the help text for depth_test for more info.";
	  info->in_param_spec =	"render_in:render,status:enum?DISABLED|ENABLED";
	  info->out_param_spec = "render_out:render";
	  info->component_class = "render";
	  info->tunnel = true;
	}
	
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
	  status = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"status");
		render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;
		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	}
	bool activate_offscreen() {
	  if (status->get() == 1) {
	    glDepthMask(GL_TRUE);
	  } else {
	    glDepthMask(GL_FALSE);
	  }  
	  return true;
	}  
	
  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
	  render_result->set(render_in->get());
	}
};



class vsx_module_mesh_old_supershape : public vsx_module {
  // in
  vsx_module_param_float* x_num_segments;
  vsx_module_param_float* x_start;
  vsx_module_param_float* x_stop;
  vsx_module_param_float* x_a;
  vsx_module_param_float* x_b;
  vsx_module_param_float* x_n1;
  vsx_module_param_float* x_n2;
  vsx_module_param_float* x_n3;
  vsx_module_param_float* x_m;

  vsx_module_param_float* y_num_segments;
  vsx_module_param_float* y_start;
  vsx_module_param_float* y_stop;
  vsx_module_param_float* y_a;
  vsx_module_param_float* y_b;
  vsx_module_param_float* y_n1;
  vsx_module_param_float* y_n2;
  vsx_module_param_float* y_n3;
  vsx_module_param_float* y_m;

  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh<>* mesh;
  int n_segs;
  int l_param_updates;
public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier = "!mesh;solid;mesh_supershape";
    info->description = "";
    info->in_param_spec = "x:complex{"
                            "x_num_segments:float?min=2"
                            ",x_start:float?muin=-1.570794&muax=1.570794"
                            ",x_stop:float?muin=-1.570794&muax=1.570794"
                            ",x_a:float"
                            ",x_b:float"
                            ",x_n1:float"
                            ",x_n2:float"
                            ",x_n3:float"
                            ",x_m:float"
                          "}"
                          ",y:complex{"
                            "y_num_segments:float?min=2"
                            ",y_start:float?muin=-3.14159&muax=3.14159"
                            ",y_stop:float?muin=-3.14159&muax=3.14159"
                            ",y_a:float"
                            ",y_b:float"
                            ",y_n1:float"
                            ",y_n2:float"
                            ",y_n3:float"
                            ",y_m:float"
                          "}";

    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    x_num_segments = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_num_segments");
    x_num_segments->set(15.0f);
    x_start        = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_start");
    x_stop         = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_stop");
    x_start->set(-(float)HALF_PI);
    x_stop->set( (float)HALF_PI);
    x_a            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_a");
    x_a->set(1.0f);
    x_b            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_b");
    x_b->set(1.0f);
    x_n1           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_n1");
    x_n1->set(0.7f);
    x_n2           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_n2");
    x_n2->set(0.3f);
    x_n3           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_n3");
    x_n3->set(0.2f);
    x_m            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_m");
    x_m->set(2.0f);

    y_num_segments = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_num_segments");
    y_num_segments->set(15.0f);
    y_start        = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_start");
    y_stop         = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_stop");
    y_start->set(-(float)PI);
    y_stop->set((float)PI);
    y_a            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_a");
    y_a->set(1.0f);
    y_b            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_b");
    y_b->set(1.0f);
    y_n1           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_n1");
    y_n1->set(100.f);
    y_n2           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_n2");
    y_n2->set(100.f);
    y_n3           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_n3");
    y_n3->set(100.f);
    y_m            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_m");
    y_m->set(3.0f);


    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    result->set_p(mesh);
  }

  bool init() {
    mesh = new vsx_mesh<>;
    return true;
  }
  void on_delete()
  {
    delete mesh;
  }


  void run()
  {
    req(param_updates);
    param_updates = 0;
    mesh->data->vertices[0] = vsx_vector3<>(10);

    l_param_updates = param_updates;
    mesh->data->vertices.reset_used();
    mesh->data->faces.reset_used();
    int vi = 0; // vertex index

    // sanity checks
    float _x_start = x_start->get();

    float _x_stop = x_stop->get();

    if (_x_start > _x_stop) {
      float t = _x_start;
      _x_start = _x_stop;
      _x_stop = t;
    }

    float _y_start = y_start->get();

    float _y_stop = y_stop->get();

    if (_y_start > _y_stop) {
      float t = _y_start;
      _y_start = _y_stop;
      _y_stop = t;
    }


    double theta_step = (_x_stop - _x_start) / x_num_segments->get();
    double phi_step = (_y_stop - _y_start) / y_num_segments->get();
    int _x_num_segments = (int)x_num_segments->get();
    int _y_num_segments = (int)x_num_segments->get()+1;


    double _x_a = x_a->get();
    double _x_b = x_b->get();
    double _x_n1 = x_n1->get();
    double _x_n2 = x_n2->get();
    double _x_n3 = x_n3->get();
    double _x_m = x_m->get();

    double _y_a = y_a->get();
    double _y_b = y_b->get();
    double _y_n1 = y_n1->get();
    double _y_n2 = y_n2->get();
    double _y_n3 = y_n3->get();
    double _y_m = y_m->get();




    double theta = _x_start;
    for (int i = 0; i < _x_num_segments; i++) {
      double r1 = pow(
                  pow(
                    fabs((1.0f / _x_a) * cos(_x_m * theta / 4.0f))
                    , _x_n2)
                  +
                  pow(
                    fabs((1.0f / _x_b) * sin(_x_m * theta / 4.0f))
                    , _x_n3)
                , -1.0f/_x_n1);
      double phi = _y_start;
      for(int j = 0; j < _y_num_segments; j++) {

        double r2 = pow(
                    pow(
                      fabs((1.0f / _y_a) * cos(_y_m * phi / 4.0f))
                      , _y_n2)
                    +
                    pow(
                      fabs((1.0f / _y_b) * sin(_y_m * phi / 4.0f))
                      , _y_n3)
                  , -1.0f/_y_n1);

        vsx_vector3<> tmp_vec;
        tmp_vec.x = (float)(r1 * cos(phi) * r2 * cos(theta));
        tmp_vec.y = (float)(r1 * sin(phi) * r2 * cos(theta));
        tmp_vec.z = (float)(r2 * sin(theta));
        mesh->data->vertices[vi] = tmp_vec;
        mesh->data->vertex_normals[vi] = tmp_vec;
        mesh->data->vertex_colors[vi] = vsx_color<>(1, 1, 1, 1);
        phi += phi_step;
        vi++;
      }
      theta += theta_step;
    }


    for(int i = 0; i < _y_num_segments - 2; i++) {
      for(int j = 0; j < _x_num_segments; j++) {
        vsx_face3 a;
        a.a = i * _x_num_segments + j;
        a.b = (i + 1) * _x_num_segments + j;
        a.c = i * _x_num_segments + ((j + 1) % _x_num_segments);
        if (a.a > mesh->data->vertices.size()) a.a = 0;
        if (a.b > mesh->data->vertices.size()) a.b = 0;
        if (a.c > mesh->data->vertices.size()) a.c = 0;
        vsx_vector3<> aa = mesh->data->vertices[a.b] - mesh->data->vertices[a.a];
        vsx_vector3<> b = mesh->data->vertices[a.c] - mesh->data->vertices[a.a];
        vsx_vector3<> n;
        n.cross(aa,b);
        n.normalize();
        mesh->data->vertex_normals[a.a] = mesh->data->vertex_normals[a.b] = mesh->data->vertex_normals[a.c] = n;
        mesh->data->faces.push_back(a);
        a.a = i * _x_num_segments + ((j + 1) % _x_num_segments);
        a.b = (i + 1) * _x_num_segments + j;
        a.c = (i + 1) * _x_num_segments + ((j + 1) % _x_num_segments);
        aa = mesh->data->vertices[a.b] - mesh->data->vertices[a.a];
        b = mesh->data->vertices[a.c] - mesh->data->vertices[a.a];
        n.cross(aa,b);
        n.normalize();
        mesh->data->vertex_normals[a.a] = mesh->data->vertex_normals[a.b] = mesh->data->vertex_normals[a.c] = n;

        mesh->data->faces.push_back(a);
      }
    }
    mesh->timestamp++;
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
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
}

#ifndef MOD_CM
#define MOD_CM vsx_module__deprecated_cm
#define MOD_DM vsx_module__deprecated_dm
#define MOD_NM vsx_module__deprecated_nm
#endif


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  // as we have only one module available, don't look at the module variable, just return - for speed
  // otherwise you'd have something like,, switch(module) { case 0: break; }
  switch(module) {
    case 0: return (vsx_module*)(new vsx_depth_test);
    case 1: return (vsx_module*)(new vsx_depth_mask);
    case 2: return (vsx_module*)(new vsx_module_mesh_old_supershape);
    case 3: return (vsx_module*)(new vsx_module_kaleido_star);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_depth_test*)m; break;
    case 1: delete (vsx_depth_mask*)m; break;
    case 2: delete (vsx_module_mesh_old_supershape*)m; break;
    case 3: delete (vsx_module_kaleido_star*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment) {
  VSX_UNUSED(environment);
  return 4;
}  

