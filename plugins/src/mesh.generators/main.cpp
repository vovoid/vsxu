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
#include "vsx_sequence.h"
#include "vsx_bspline.h"

class vsx_module_mesh_rand_points : public vsx_module {
  // in
  vsx_module_param_float* rand_seed;
	vsx_module_param_float* num_points;
	vsx_module_param_float3* scaling;
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	vsx_vector old_scaling;
	bool first_run;
  vsx_rand rand;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;vertices;random_vertices";
    info->description = "Generates [num_points] vertices\nconfined within the box\ndefined by [scaling]\ndelivered as a mesh.\n"
      "Colors, faces etc. are not genarated.\n"
      "Primarily for use with particle systems\n"
      "and point renderers and data source for\n"
      "the b-spline generator.";
    info->out_param_spec = "mesh:mesh";
    info->in_param_spec =
  "\
  rand_seed:float,\
  num_points:float,\
  scaling:float3?nc=1\
  ";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    rand_seed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    num_points = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_points");
    num_points->set(100);
    scaling = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"scaling");
    scaling->set(1,0);
    scaling->set(1,1);
    scaling->set(1,2);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
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
    //if (mesh->data->vertices.size() != (int)num_points) {

      //mesh->data->vertices.reset_used();
    if (first_run || param_updates) {
      rand.srand( (int)rand_seed->get() );
      //printf("generating random points\n");
      int i;
      for (i = 0; i < (int)num_points->get(); ++i) {
        mesh->data->vertices[i].x = (rand.frand()-0.5f)*scaling->get(0);
        mesh->data->vertices[i].y = (rand.frand()-0.5f)*scaling->get(1);
        mesh->data->vertices[i].z = (rand.frand()-0.5f)*scaling->get(2);
      }
      mesh->data->vertices.reset_used(i);
      first_run = false;
      param_updates = 0;
      mesh->timestamp++;
      result->set_p(mesh);
    } /*else {
      if (num_points->get() < mesh->data->vertices.size()) {
        mesh->data->vertices.reset_used((int)num_points->get());
      } else
      if (num_points->get() > mesh->data->vertices.size()) {
        for (int i = mesh->data->vertices.size(); i < (int)num_points->get(); ++i) {
          mesh->data->vertices[i].x = ((rand()%10000)*0.0001-0.5)*scaling->get(0);
          mesh->data->vertices[i].y = ((rand()%10000)*0.0001-0.5)*scaling->get(1);
          mesh->data->vertices[i].z = ((rand()%10000)*0.0001-0.5)*scaling->get(2);
        }
      }
    }*/
      //printf("randMesh done %d\n",mesh->data->vertices.size());
    //  }
  }
};

class vsx_module_mesh_bspline_vertices : public vsx_module {
  // in
  vsx_module_param_mesh* source;
  vsx_module_param_float* density;
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh* mesh;
  vsx_bspline spline0;
  bool first_run;
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
    info->identifier = "mesh;vertices;bspline_vertices";
    info->description = "";
    info->out_param_spec = "mesh:mesh";
    info->in_param_spec =
  "\
  source:mesh,\
  density:float\
  ";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    source = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH, "source" );
    
    density = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "density" );
    density->set(10);
    
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH, "mesh" );

    first_run = true;
    spline0.init(vsx_vector(0), 0.7f, 0.3f, 0.6f);
  }

  void run() {
    vsx_mesh** in = source->get_addr();
    if (!in) return;
    //if (mesh.data->vertices.size() != (int)num_points) {
    spline0.points.set_volatile();
    spline0.points.set_data((*in)->data->vertices.get_pointer(), (*in)->data->vertices.size() );
    spline0.set_pos(0.0f);
    spline0.step(1);
    int idens = (int)floor(density->get());
    float step = 1.0f / (float)idens;
    if ((*in)->timestamp != mesh->timestamp) first_run = true;
    
    if (first_run || param_updates) {
      //printf("generating random points\n");
      int i;
      for (i = 0; i < (int)((*in)->data->vertices.size()-1) * idens; ++i) {
        spline0.step(step);
        mesh->data->vertices[i] = spline0.calc_coord();
      }
      mesh->data->vertices.reset_used(i);
      first_run = false;
      param_updates = 0;
      mesh->timestamp = (*in)->timestamp;
      result->set(mesh);
    } /*else {
      if (num_points->get() < mesh.data->vertices.size()) {
        mesh.data->vertices.reset_used((int)num_points->get());
      } else
      if (num_points->get() > mesh.data->vertices.size()) {
        for (int i = mesh.data->vertices.size(); i < (int)num_points->get(); ++i) {
          mesh.data->vertices[i].x = ((rand()%10000)*0.0001-0.5)*scaling->get(0);
          mesh.data->vertices[i].y = ((rand()%10000)*0.0001-0.5)*scaling->get(1);
          mesh.data->vertices[i].z = ((rand()%10000)*0.0001-0.5)*scaling->get(2);
        }
      }
    }*/
      //printf("randMesh done %d\n",mesh.data->vertices.size());
    //  }
  }
};

class vsx_module_mesh_lightning_vertices : public vsx_module {
  // in
  vsx_module_param_float* rand_seed;
  vsx_module_param_float* length;
  vsx_module_param_float* lifetime_mod;
  vsx_module_param_float* num_points;
  vsx_module_param_float3* scaling;
  vsx_module_param_mesh* mesh_a;
  vsx_module_param_mesh* mesh_b;
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh* mesh;
  vsx_vector old_scaling;
  bool first_run;
  unsigned long id_a;
  unsigned long id_b;
  unsigned long lifetime;
  vsx_vector delta;
  vsx_vector start;
  vsx_rand rand;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;vertices;lightning_vertices";
    info->description = "Generates a line with random offset\n"
      "vertices from one mesh to another.";
    info->out_param_spec = "mesh:mesh";
    info->in_param_spec =
      "rand_seed:float,"
      "lifetime:float,"
      "length:float,"
      "mesh_a:mesh,"
      //"mesh_b:mesh,"
      "num_points:float,"
      "scaling:float3?nc=1"
    ;
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    rand_seed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"length");
    length->set(1.0f);
    lifetime_mod = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"lifetime");
    lifetime_mod->set(1.0f);
    num_points = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_points");
    num_points->set(100);
    scaling = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"scaling");
    scaling->set(1,0);
    scaling->set(1,1);
    scaling->set(1,2);
    mesh_a = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_a");
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
    lifetime = 0;
  }

  bool init() {
    mesh = new vsx_mesh;
    return true;
  }
  
  void on_delete()
  {
    delete mesh;
  }

  void run()
  {
    //if (mesh->data->vertices.size() != (int)num_points) {
    vsx_mesh** p_a = mesh_a->get_addr();
    //vsx_mesh* p_b = mesh_b->get_addr();
    if (p_a)// && p_b)
    {
      //mesh->data->vertices.reset_used();
      if (lifetime <= 0)
      {

        id_a = rand.rand() % (*p_a)->data->vertices.size();
        //id_b = rand() % p_b->data->vertices.size();
        //printf("id_a: %d\n", (int)id_a);
        lifetime = (int) ((float)(rand.rand() % 10) * lifetime_mod->get()) + 5;
        //delta = p_b->data->vertices[id_b] - p_a->data->vertices[id_a];
        delta = (*p_a)->data->vertex_normals[id_a] * rand.frand() * length->get();
        start = (*p_a)->data->vertices[id_a];
        delta *= 1.0f / (float)((int)num_points->get());
      }

      int i;
      start = (*p_a)->data->vertices[id_a];
      float one_div_num_points = 1.0f / (float)((int)num_points->get());
      float sx = scaling->get(0) * length->get();
      float sy = scaling->get(1) * length->get();
      float sz = scaling->get(2) * length->get();
      for (i = 0; i < (int)num_points->get(); ++i)
      {
        //printf("generating point %d %f %f %f\n",i,delta.x, delta.y, delta.z);
        mesh->data->vertices[i].x = start.x + (rand.frand()-0.5f)*sx * sin((float)i * one_div_num_points * PI);
        mesh->data->vertices[i].y = start.y + (rand.frand()-0.5f)*sy * sin((float)i * one_div_num_points * PI);
        mesh->data->vertices[i].z = start.z + (rand.frand()-0.5f)*sz * sin((float)i * one_div_num_points * PI);
        float c = 1.0f - (float)i * one_div_num_points;
        mesh->data->vertex_colors[i].r = c;
        mesh->data->vertex_colors[i].g = c;
        mesh->data->vertex_colors[i].b = c;
        mesh->data->vertex_colors[i].a = 1.0f;
        start = mesh->data->vertices[i];
        start += delta;
      }
      mesh->data->vertices.reset_used(i);
      lifetime--;
      result->set(mesh);
      /*
        srand( (int)rand_seed->get() );
        //printf("generating random points\n");
        int i;
        for (i = 0; i < (int)num_points->get(); ++i)
        {
          mesh->data->vertices[i].x = ((rand()%10000)*0.0001f-0.5f)*scaling->get(0);
          mesh->data->vertices[i].y = ((rand()%10000)*0.0001f-0.5f)*scaling->get(1);
          mesh->data->vertices[i].z = ((rand()%10000)*0.0001f-0.5f)*scaling->get(2);
        }
        mesh->data->vertices.reset_used(i);
        first_run = false;
        param_updates = 0;
        mesh->timestamp++;
        result->set_p(mesh);
      }*/
    }
  }

  ~vsx_module_mesh_lightning_vertices()
  {
  }
};

class vsx_module_mesh_rays : public vsx_module {
  // in
	vsx_module_param_float* num_rays;
	vsx_module_param_float* limit_ray_size;
	vsx_module_param_float4* center_color;

	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	bool first_run;
	int n_rays;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;particles;mesh_rays";
    info->description = "A bunch of random triangles with edges alpha = 0";
    info->in_param_spec = "num_rays:float,center_color:float4,options:complex{limit_ray_size:float}";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    num_rays = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_rays");
    num_rays->set(40);
    n_rays = 40;
    limit_ray_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"limit_ray_size");
    limit_ray_size->set(-1.0f);

    center_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"center_color");
    center_color->set(0.5f,0);
    center_color->set(0.5f,1);
    center_color->set(0.5f,2);
    center_color->set(1,3);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
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
    mesh->data->vertices[0] = vsx_vector(0);
    mesh->data->vertex_colors[0] = vsx_color__(center_color->get(0),center_color->get(1),center_color->get(2),center_color->get(3));

    if (first_run || n_rays != (int)num_rays->get() || limit_ray_size->updates) {
    	limit_ray_size->updates = 0;
      mesh->data->vertex_tex_coords[0].s = 0;
      mesh->data->vertex_tex_coords[0].t = 0;
      mesh->data->vertices.reset_used();
      mesh->data->faces.reset_used();
      //printf("generating random points\n");
      for (int i = 1; i < (int)num_rays->get(); ++i) {
        mesh->data->vertices[i*2].x = (rand()%10000)*0.0001f-0.5f;
        mesh->data->vertices[i*2].y = (rand()%10000)*0.0001f-0.5f;
        mesh->data->vertices[i*2].z = (rand()%10000)*0.0001f-0.5f;
        mesh->data->vertex_colors[i*2] = vsx_color__(0,0,0,0);
        mesh->data->vertex_tex_coords[i*2].s = 0.0f;
        mesh->data->vertex_tex_coords[i*2].t = 1.0f;
        if (limit_ray_size->get() > 0.0f ) {
        	mesh->data->vertices[i*2+1].x = mesh->data->vertices[i*2].x+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
        	mesh->data->vertices[i*2+1].y = mesh->data->vertices[i*2].y+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
        	mesh->data->vertices[i*2+1].z = mesh->data->vertices[i*2].z+((rand()%10000)*0.0001f-0.5f)*limit_ray_size->get();
        } else {
        	mesh->data->vertices[i*2+1].x = (rand()%10000)*0.0001f-0.5f;
        	mesh->data->vertices[i*2+1].y = (rand()%10000)*0.0001f-0.5f;
        	mesh->data->vertices[i*2+1].z = (rand()%10000)*0.0001f-0.5f;
        }

        mesh->data->vertex_colors[i*2+1] = vsx_color__(0,0,0,0);
        mesh->data->vertex_tex_coords[i*2+1].s = 1.0f;
        mesh->data->vertex_tex_coords[i*2+1].t = 0.0f;
        mesh->data->faces[i-1].a = 0;
        mesh->data->faces[i-1].b = i*2;
        mesh->data->faces[i-1].c = i*2+1;
        n_rays = (int)num_rays->get();
      }
      first_run = false;
      mesh->timestamp++;
      result->set_p(mesh);
    }
    /*else {
      if (num_points->get() < mesh->data->vertices.size()) {
        mesh->data->vertices.reset_used((int)num_points->get());
      } else
      if (num_points->get() > mesh->data->vertices.size()) {
        for (int i = mesh->data->vertices.size(); i < (int)num_points->get(); ++i) {
          mesh->data->vertices[i].x = (rand()%10000)*0.0001*scaling->get(0);
          mesh->data->vertices[i].y = (rand()%10000)*0.0001*scaling->get(1);
          mesh->data->vertices[i].z = (rand()%10000)*0.0001*scaling->get(2);
        }

      }
    }
      printf("randMesh done %d\n",mesh->data->vertices.size());*/

    //  }
  }
};

class vsx_module_mesh_disc : public vsx_module {
  // in
  vsx_module_param_float* num_segments;
  vsx_module_param_float* width;
  vsx_module_param_float* diameter;
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	bool first_run;
	int n_segs;
	int l_param_updates;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;particles;mesh_disc";
    info->description = "A disc. simple as that.";
    info->in_param_spec = "num_segments:float,diameter:float,border_width:float";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;
    num_segments = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_segments");
    num_segments->set(20);
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"border_width");
    width->set(1);
    diameter = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"diameter");
    diameter->set(1);
    n_segs = 20;

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
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
    if (l_param_updates != param_updates) first_run = true;
    mesh->data->vertices[0] = vsx_vector(0);
    //mesh->data->vertex_colors[0] = vsx_color__(center_color->get(0),center_color->get(1),center_color->get(2),center_color->get(3));
    if (first_run || n_segs != num_segments->get()) {
      l_param_updates = param_updates;
      //printf("generating random points\n");
      mesh->data->vertices.reset_used();
      mesh->data->faces.reset_used();
      float inc = (float)(PI*2/(double)((int)num_segments->get()));
      float t_inc = 1.0f/(float)((int)num_segments->get());
      float t = 0.0f;
      float ip = 0.0f;
      float dia = diameter->get();
      for (int i = 0; i < (int)num_segments->get(); ++i) {
        //mesh->data->vertex_tex_coords[i] = vsx_vector__(0,0,0);
        int i4 = i*4;
        mesh->data->vertices[i4].x = (float)cos(ip)*dia;
        mesh->data->vertices[i4].y = 0.0f;
        mesh->data->vertices[i4].z = (float)sin(ip)*dia;
        mesh->data->vertex_colors[i4] = vsx_color__(1,1,1,1);
        mesh->data->vertex_tex_coords[i4].s = t;
        mesh->data->vertex_tex_coords[i4].t = 0;

        mesh->data->vertices[i4+1].x = (float)cos(ip)*(dia+0.15f*width->get());
        mesh->data->vertices[i4+1].y = 0.0f;
        mesh->data->vertices[i4+1].z = (float)sin(ip)*(dia+0.15f*width->get());
        mesh->data->vertex_colors[i4+1] = vsx_color__(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+1].s = t;
        mesh->data->vertex_tex_coords[i4+1].t = 1;

        mesh->data->vertices[i4+2].x = (float)cos(ip+inc)*(dia+0.15f*width->get());
        mesh->data->vertices[i4+2].y = 0.0f;
        mesh->data->vertices[i4+2].z = (float)sin(ip+inc)*(dia+0.15f*width->get());
        mesh->data->vertex_colors[i4+2] = vsx_color__(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+2].s = t+t_inc;
        mesh->data->vertex_tex_coords[i4+2].t = 1;

        mesh->data->vertices[i4+3].x = (float)cos(ip+inc)*dia;
        mesh->data->vertices[i4+3].y = 0.0f;
        mesh->data->vertices[i4+3].z = (float)sin(ip+inc)*dia;
        mesh->data->vertex_colors[i4+3] = vsx_color__(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+3].s = t+t_inc;
        mesh->data->vertex_tex_coords[i4+3].t = 0;

        vsx_face a;
        a.a = i4+2; a.b = i4; a.c = i4+1;
        mesh->data->faces.push_back(a);
        a.a = i4+2; a.b = i4+3; a.c = i4;
        mesh->data->faces.push_back(a);
        ip += inc;
        t += t_inc;
      }
      first_run = false;
      n_segs = (int)num_segments->get();
      mesh->timestamp++;
      result->set_p(mesh);
    }
    /*else {
      if (num_points->get() < mesh->data->vertices.size()) {
        mesh->data->vertices.reset_used((int)num_points->get());
      } else
      if (num_points->get() > mesh->data->vertices.size()) {
        for (int i = mesh->data->vertices.size(); i < (int)num_points->get(); ++i) {
          mesh->data->vertices[i].x = (rand()%10000)*0.0001*scaling->get(0);
          mesh->data->vertices[i].y = (rand()%10000)*0.0001*scaling->get(1);
          mesh->data->vertices[i].z = (rand()%10000)*0.0001*scaling->get(2);
        }

      }
    }
      printf("randMesh done %d\n",mesh->data->vertices.size());*/

    //  }
  }
};

void eval2D(double a, double b, float m,float n1,float n2,float n3,float phi,float &x,float &y){
  double r;
  double t1,t2;
  //double a=1,b=1;

  t1 = cos(m * phi / 4) / a;
  t1 = fabs(t1);
  t1 = pow(t1,n2);

  t2 = sin(m * phi / 4) / b;
  t2 = fabs(t2);
  t2 = pow(t2,n3);

  r = pow(t1+t2,1/n1);
  if (fabs(r) == 0) {
    x = 0;
    y = 0;
  } else {
    r = 1 / r;
    x = (float)(r * cos(phi));
    y = (float)(r * sin(phi));
  }
}

inline void eval3D(double a, double b, float m,float n1,float n2,float n3,float phi,float theta,float &x,float &y,float &z)
{
  double r;
  double t1,t2;
  //double a=1,b=1;

  eval2D(a,b, m,n1,n2,n3,phi,x,y);

  t1 = cos(m * theta / 4) / a;
  t1 = fabs(t1);
  t1 = pow(t1,n2);

  t2 = sin(m * theta / 4) / b;
  t2 = fabs(t2);
  t2 = pow(t2,n3);

  r = pow(t1+t2,1/n1);
  if (fabs(r) == 0) {
    x = 0;
    y = 0;
    z = 0;
  } else {
    r = 1 / r;
    x *= (float)(r * cos(theta));
    y *= (float)(r * cos(theta));
    z = (float) (r * sin(theta));
  }

}

class vsx_module_mesh_supershape : public vsx_module {
  // in
  vsx_module_param_float* x_num_segments;
  vsx_module_param_float* x_start;
  vsx_module_param_float* x_stop;
  vsx_module_param_float* y_start;
  vsx_module_param_float* y_stop;
  vsx_module_param_float* x_a;
  vsx_module_param_float* x_b;
  vsx_module_param_float* x_n1;
  vsx_module_param_float* x_n2;
  vsx_module_param_float* x_n3;
  vsx_module_param_float* x_m;

	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	bool first_run;
	int n_segs;
	int l_param_updates;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_solid_supershape";
    info->description = "";
    info->in_param_spec = "x:complex{"
                            "x_num_segments:float?min=2"
                            ",x_start:float?muin=-1.570794&muax=1.570794"
                            ",x_stop:float?muin=-1.570794&muax=1.570794"
                            ",y_start:float?muin=-1.570794&muax=1.570794"
                            ",y_stop:float?muin=-1.570794&muax=1.570794"
                            ",x_a:float"
                            ",x_b:float"
                            ",x_n1:float"
                            ",x_n2:float"
                            ",x_n3:float"
                            ",x_m:float"
                          "}";

    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    x_num_segments = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_num_segments");
    x_num_segments->set(40.0f);
    x_start        = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_start");
    x_stop         = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_stop");
    x_start->set((float)-HALF_PI);
    x_stop->set((float)HALF_PI);
    x_a            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_a");
    x_a->set(1.0f);
    x_b            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_b");
    x_b->set(1.0f);
    x_n1           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_n1");
    x_n1->set(1.0f);
    x_n2           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_n2");
    x_n2->set(1.0f);
    x_n3           = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_n3");
    x_n3->set(1.0f);
    x_m            = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_m");
    x_m->set(1.0f);
    y_start        = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_start");
    y_stop         = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_stop");
    y_start->set((float)-PI);
    y_stop->set((float)PI);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
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
    if (l_param_updates != param_updates) first_run = true;
    mesh->data->vertices[0] = vsx_vector(10);

    if (first_run) {
      l_param_updates = param_updates;
      //printf("generating supershape mesh\n");
      mesh->data->vertices.reset_used();
      mesh->data->faces.reset_used();
			int vi = 0; // vertex index

			// sanity checks
			float _x_start = x_start->get();
			/*if (_x_start < -HALF_PI) _x_start = -HALF_PI;
			if (_x_start > HALF_PI) _x_start = HALF_PI;*/

			float _x_stop = x_stop->get();
			/*if (_x_stop < -HALF_PI) _x_stop = -HALF_PI;
			if (_x_stop > HALF_PI) _x_stop = HALF_PI;*/

			if (_x_start > _x_stop) {
				float t = _x_start;
				_x_start = _x_stop;
				_x_stop = t;
			}

			float _y_start = y_start->get();
			/*if (_y_start < -PI) _y_start = -PI;
			if (_y_start > PI) _y_start = PI;*/

			float _y_stop = y_stop->get();
			/*if (_y_stop < -PI) _y_stop = -PI;
			if (_y_stop > PI) _y_stop = PI;*/

			if (_y_start > _y_stop) {
				float t = _y_start;
				_y_start = _y_stop;
				_y_stop = t;
			}


			float theta_step = (_x_stop - _x_start) / x_num_segments->get();
			float phi_step = (_y_stop - _y_start) / x_num_segments->get();
			int _x_num_segments = (int)x_num_segments->get();
			int _y_num_segments = (int)x_num_segments->get();


			double _x_a = x_a->get();
			double _x_b = x_b->get();
			float _x_n1 = x_n1->get();
			float _x_n2 = x_n2->get();
			float _x_n3 = x_n3->get();
			float _x_m = x_m->get();

			/*double _y_a = y_a->get();
			double _y_b = y_b->get();
			double _y_n1 = y_n1->get();
			double _y_n2 = y_n2->get();
			double _y_n3 = y_n3->get();
			double _y_m = y_m->get();*/

      float scale = 1.0f;


      float x1=0,y1=0,z1=0;
      float x2=0,y2=0,z2=0;
      float x3=0,y3=0,z3=0;
//      float x4=0,y4=0,z4=0;

      float phi = _y_start;

      vsx_avector<vsx_vector> prev_row_normals;
      vsx_avector<vsx_vector*> prev_row_threes;
      vsx_vector prev_norm;

      for (int i = 0; i < _x_num_segments+1; i++) {
        float theta = (float)_x_start;

        /*double r1 = pow(
                    pow(
                      fabs((1.0f / _x_a) * cos(_x_m * theta / 4.0f))
                      , _x_n2)
                    +
                    pow(
                      fabs((1.0f / _x_b) * sin(_x_m * theta / 4.0f))
                      , _x_n3)
                  , -1.0f/_x_n1);*/
        for(int j = 0; j < _y_num_segments+1; j++) {
          //eval3D(double a, double b, float m,float n1,float n2,float n3,float phi,float theta,float &x,float &y,float &z)

	        eval3D(_x_a, _x_b, _x_m,_x_n1,_x_n2,_x_n3,phi       ,theta         ,x1,y1,z1);
	        eval3D(_x_a, _x_b, _x_m,_x_n1,_x_n2,_x_n3,phi       ,theta+theta_step,x2,y2,z2);
	        eval3D(_x_a, _x_b,_x_m,_x_n1,_x_n2,_x_n3,phi+phi_step,theta+theta_step,x3,y3,z3);
	        //eval3D(_x_m,_x_n1,_x_n2,_x_n3,phi+phi_step,theta         ,x4,y4,z4);
	        // Calc normal ->
	        // create vectors
	        float v1x=x1-x2; float v2x=x2-x3;
	        float v1y=y1-y2; float v2y=y2-y3;
	        float v1z=z1-z2; float v2z=z2-z3;
	          // Get cross product of vectors
	        float nx = (v1y * v2z) - (v1z * v2y);
	        float ny = (v1z * v2x) - (v1x * v2z);
	        float nz = (v1x * v2y) - (v1y * v2x);
	        // Normalise final vector
	        float vLen = (float)sqrt( (nx * nx) + (ny * ny) + (nz * nz) );
	        vsx_vector norm(-nx/vLen,  -ny/vLen,  -nz/vLen);
	        mesh->data->vertex_normals[vi] = norm;
	        mesh->data->vertices[vi] = vsx_vector(x1*scale, y1*scale, z1*scale);
	        vi++;
          //mesh->data->vertex_normals[vi] = norm;
          //mesh->data->vertices[vi] = vsx_vector(x4*scale, y4*scale, z4*scale);

          //vi++;
          /*mesh->data->vertex_normals[vi] = norm;
          mesh->data->vertices[vi] = vsx_vector(x3*scale, y3*scale, z3*scale);
          vi++;
          mesh->data->vertex_normals[vi] = norm;
          mesh->data->vertices[vi] = vsx_vector(x4*scale, y4*scale, z4*scale);
          vi++;*/
          //Gl.glNormal3f(nx/vLen,  ny/vLen,  nz/vLen);
          //Gl.glVertex3f(x1*scale, y1*scale, z1*scale);
          //Gl.glVertex3f(x2*scale, y2*scale, z2*scale);
          //Gl.glVertex3f(x3*scale, y3*scale, z3*scale);
          //Gl.glVertex3f(x4*scale, y4*scale, z4*scale);
          //theta+=addTheta;

          // per-vertex normals (try)
          if (i > 0 && j > 0)
          {
            vsx_vector sum_norm = norm + prev_norm + prev_row_normals[j] + prev_row_normals[j-1];
            sum_norm.normalize();
            mesh->data->vertex_normals[vi-2] = sum_norm;
            mesh->data->vertex_normals[vi-3] = sum_norm;
            //printf("i: %d j: %d\n", i,j);
            //mesh->data->vertex_normals[vi-_x_num_segments-2] = sum_norm;
            //(*prev_row_threes[j]) = sum_norm;
          }
          prev_norm = norm;

          if (i > 0 && j > 0)
          {
            vsx_face a;
            a.a = vi - 1; // (0)
            a.b = vi - _x_num_segments-1;
            a.c = vi - _x_num_segments-2;
            mesh->data->faces.push_back(a);
            a.a = vi-2;
            a.b = vi-1;
            a.c = vi - _x_num_segments-2;
            mesh->data->faces.push_back(a);

  	        //a.a = vi-2; // 3
  	        //a.b = vi-3; // 2
  	        //a.c = vi-4; // 1
  	        //mesh->data->faces.push_back(a);
//            a.a = vi-2; // 3
            //a.b = vi-4; // 1
            //a.c = vi-1; // 4
            //mesh->data->faces.push_back(a);
          }
          theta += theta_step;

          prev_row_normals[j] = norm;
          //prev_row_threes[j] = &mesh->data->vertex_normals[vi-2];
					/*double r2 = pow(
											pow(
												fabs((1.0f / _y_a) * cos(_y_m * phi / 4.0f))
												, _y_n2)
											+
											pow(
											  fabs((1.0f / _y_b) * sin(_y_m * phi / 4.0f))
											  , _y_n3)
										, -1.0f/_y_n1);

			    vsx_vector tmp_vec;//(sin(angle) * rad, y, cos(angle) * rad);
			    tmp_vec.x = r1 * cos(phi) * r2 * cos(theta);
			    tmp_vec.y = r1 * sin(phi) * r2 * cos(theta);
			    tmp_vec.z = r2 * sin(theta);
			    //printf("%f %f %f\n", tmp_vec.x, tmp_vec.y, tmp_vec.z);
			    mesh->data->vertices[vi] = tmp_vec;
			    mesh->data->vertex_normals[vi] = tmp_vec;
			    mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
			    phi += phi_step;
			    vi++;*/
				}
				phi += phi_step;
			}

			/*vsx_vector tmp_vec = vsx_vector(0, 1, 0);
	    mesh->data->vertices[vi] = tmp_vec;
	    mesh->data->vertex_normals[vi] = tmp_vec;
	    mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
	  	vi++;
			tmp_vec = vsx_vector(0, -1, 0);
	    mesh->data->vertices[vi] = tmp_vec;
	    mesh->data->vertex_normals[vi] = tmp_vec;
	    mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
	  	vi++;*/
	  	 /*
	  	for(int i = 0; i < _y_num_segments - 2; i++) {
				for(int j = 0; j < _x_num_segments; j++) {
					vsx_face a;
					a.a = i * _x_num_segments + j;
					a.b = (i + 1) * _x_num_segments + j;
					a.c = i * _x_num_segments + ((j + 1) % _x_num_segments);
					vsx_vector aa = mesh->data->vertices[a.b] - mesh->data->vertices[a.a];
					vsx_vector b = mesh->data->vertices[a.c] - mesh->data->vertices[a.a];
					vsx_vector n;
					n.cross(aa,b);
					n.normalize();
					mesh->data->vertex_normals[a.a] = mesh->data->vertex_normals[a.b] = mesh->data->vertex_normals[a.c] = n;
			    //printf("%d %d %d\n", a.a, a.b, a.c);
					//mesh->data->faces.push_back(a);
					a.a = i * _x_num_segments + ((j + 1) % _x_num_segments);
					a.b = (i + 1) * _x_num_segments + j;
					a.c = (i + 1) * _x_num_segments + ((j + 1) % _x_num_segments);
			    //printf("%d %d %d\n", a.a, a.b, a.c);
					aa = mesh->data->vertices[a.b] - mesh->data->vertices[a.a];
					b = mesh->data->vertices[a.c] - mesh->data->vertices[a.a];
					//vsx_vector n;
					n.cross(aa,b);
					n.normalize();
					mesh->data->vertex_normals[a.a] = mesh->data->vertex_normals[a.b] = mesh->data->vertex_normals[a.c] = n;

					mesh->data->faces.push_back(a);
				}
	  	}*/
			first_run = false;
			mesh->timestamp++;
	    result->set_p(mesh);
    }

    //  }
  }
};

class vsx_module_mesh_planes : public vsx_module {
  // in
  vsx_module_param_float* num_planes;
  vsx_module_param_float* space_between;
  vsx_module_param_float* diameter;
  vsx_module_param_float3* normals;
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	bool first_run;
	int n_segs;
	int l_param_updates;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_planes";
    info->description = "Multiple planes layered on top of each other";
    info->in_param_spec = "num_planes:float,space_between:float,diameter:float,normals:float3";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;
    num_planes = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_planes");
    num_planes->set(20.0f);
    space_between = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"space_between");
    space_between->set(0.05f);
    diameter = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"diameter");
    diameter->set(1.0f);

    normals = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"normals");
    normals->set(0.0f,0);
    normals->set(1.0f,1);
    normals->set(0.0f,2);

    n_segs = 20;

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
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
    if (l_param_updates != param_updates) first_run = true;
    mesh->data->vertices[0] = vsx_vector(0);
    //mesh->data->vertex_colors[0] = vsx_color__(center_color->get(0),center_color->get(1),center_color->get(2),center_color->get(3));
    if (first_run || n_segs != num_planes->get()) {
      l_param_updates = param_updates;
      //printf("generating random points\n");
      mesh->data->vertices.reset_used();
      mesh->data->faces.reset_used();
      float inc = space_between->get();
      vsx_vector vertex_normals = vsx_vector(normals->get(0),normals->get(1),normals->get(2));
      //float t = 0.0f;
      float ip = 0.0f;
      //float dia = diameter->get();
      for (int i = 0; i < (int)num_planes->get(); ++i) {
        //mesh->data->vertex_tex_coords[i] = vsx_vector__(0,0,0);
        int i4 = i*4;
        mesh->data->vertices[i4].x = -1.0f*diameter->get();
        mesh->data->vertices[i4].y = -1.0f*diameter->get();
        mesh->data->vertices[i4].z = ip;
        mesh->data->vertex_colors[i4] = vsx_color__(1,1,1,1);
        mesh->data->vertex_tex_coords[i4] = vsx_tex_coord__(0,0);
        mesh->data->vertex_normals[i4] = vertex_normals;

        mesh->data->vertices[i4+1].x = 1.0f*diameter->get();
        mesh->data->vertices[i4+1].y = -1.0f*diameter->get();
        mesh->data->vertices[i4+1].z = ip;
        mesh->data->vertex_colors[i4+1] = vsx_color__(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+1] = vsx_tex_coord__(1,0);
        mesh->data->vertex_normals[i4+1] = vertex_normals;

        mesh->data->vertices[i4+2].x = 1.0f*diameter->get();
        mesh->data->vertices[i4+2].y = 1.0f*diameter->get();
        mesh->data->vertices[i4+2].z = ip;
        mesh->data->vertex_colors[i4+2] = vsx_color__(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+2] = vsx_tex_coord__(1,1);
        mesh->data->vertex_normals[i4+2] = vertex_normals;

        mesh->data->vertices[i4+3].x = -1.0f*diameter->get();
        mesh->data->vertices[i4+3].y = 1.0f*diameter->get();
        mesh->data->vertices[i4+3].z = ip;
        mesh->data->vertex_colors[i4+3] = vsx_color__(1,1,1,1);
        mesh->data->vertex_tex_coords[i4+3] = vsx_tex_coord__(0,1);
        mesh->data->vertex_normals[i4+3] = vertex_normals;

        vsx_face a;
        a.a = i4+2; a.b = i4; a.c = i4+1;
        mesh->data->faces.push_back(a);
        a.a = i4+2; a.b = i4+3; a.c = i4;
        mesh->data->faces.push_back(a);
        ip += inc;
      }
      first_run = false;
      n_segs = (int)num_planes->get();
    }
    result->set_p(mesh);
  }
};

class vsx_module_mesh_box : public vsx_module {
  // in
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	bool first_run;
	int l_param_updates;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_box";
    info->description = "";
    info->in_param_spec = "";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
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
    if (!first_run) return;
    vsx_face a;
		//right
    mesh->data->vertices[0] = vsx_vector( 0.5f,-0.5f, 0.5f);
    mesh->data->vertices[1] = vsx_vector( 0.5f, 0.5f, 0.5f);
    mesh->data->vertices[2] = vsx_vector( 0.5f, 0.5f,-0.5f);
    mesh->data->vertices[3] = vsx_vector( 0.5f,-0.5f,-0.5f);
    a.a = 2;
    a.b = 1;
    a.c = 0;
    mesh->data->faces.push_back(a);
    a.a = 3;
    a.b = 2;
    a.c = 0;
    mesh->data->faces.push_back(a);
		//left
    mesh->data->vertices[4] = vsx_vector(-0.5f,-0.5f, 0.5f);
    mesh->data->vertices[5] = vsx_vector(-0.5f, 0.5f, 0.5f);
    mesh->data->vertices[6] = vsx_vector(-0.5f, 0.5f,-0.5f);
    mesh->data->vertices[7] = vsx_vector(-0.5f,-0.5f,-0.5f);
    a.a = 4;
    a.b = 5;
    a.c = 6;
    mesh->data->faces.push_back(a);
    a.a = 4;
    a.b = 6;
    a.c = 7;
    mesh->data->faces.push_back(a);
		// bottom
    mesh->data->vertices[ 8] = vsx_vector(-0.5f,-0.5f,-0.5f);
    mesh->data->vertices[ 9] = vsx_vector(-0.5f,-0.5f, 0.5f);
    mesh->data->vertices[10] = vsx_vector( 0.5f,-0.5f, 0.5f);
    mesh->data->vertices[11] = vsx_vector( 0.5f,-0.5f,-0.5f);
    a.a = 10;
    a.b = 9;
    a.c = 8;
    mesh->data->faces.push_back(a);
    a.a = 11;
    a.b = 10;
    a.c = 8;
    mesh->data->faces.push_back(a);

		// top
    mesh->data->vertices[12] = vsx_vector(-0.5f, 0.5f,-0.5f);
    mesh->data->vertices[13] = vsx_vector(-0.5f, 0.5f, 0.5f);
    mesh->data->vertices[14] = vsx_vector( 0.5f, 0.5f, 0.5f);
    mesh->data->vertices[15] = vsx_vector( 0.5f, 0.5f,-0.5f);
    a.a = 12;
    a.b = 13;
    a.c = 14;
    mesh->data->faces.push_back(a);
    a.a = 12;
    a.b = 14;
    a.c = 15;
    mesh->data->faces.push_back(a);

		// near
    mesh->data->vertices[16] = vsx_vector(-0.5f,-0.5f,-0.5f);
    mesh->data->vertices[17] = vsx_vector(-0.5f, 0.5f,-0.5f);
    mesh->data->vertices[18] = vsx_vector( 0.5f, 0.5f,-0.5f);
    mesh->data->vertices[19] = vsx_vector( 0.5f,-0.5f,-0.5f);
    a.a = 16;
    a.b = 17;
    a.c = 18;
    mesh->data->faces.push_back(a);
    a.a = 16;
    a.b = 18;
    a.c = 19;
    mesh->data->faces.push_back(a);

		// far
    mesh->data->vertices[20] = vsx_vector(-0.5f,-0.5f, 0.5f);
    mesh->data->vertices[21] = vsx_vector(-0.5f, 0.5f, 0.5f);
    mesh->data->vertices[22] = vsx_vector( 0.5f, 0.5f, 0.5f);
    mesh->data->vertices[23] = vsx_vector( 0.5f,-0.5f, 0.5f);
    a.a = 22;
    a.b = 21;
    a.c = 20;
    mesh->data->faces.push_back(a);
    a.a = 23;
    a.b = 22;
    a.c = 20;
    mesh->data->faces.push_back(a);




    first_run = false;
    mesh->timestamp++;
    result->set_p(mesh);
  }
};

class vsx_module_mesh_grid : public vsx_module {
  // in
  vsx_module_param_float* power_of_two_size;
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh* mesh;
  bool first_run;
  int l_param_updates;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_grid";
    info->description = "";
    info->in_param_spec = "power_of_two_size:float";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;
    power_of_two_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"power_of_two_size");
    power_of_two_size->set(5);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
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
    if (!first_run && param_updates == 0) return;
    param_updates = 0;
    size_t width = (size_t)pow(2, (size_t)power_of_two_size->get());

    float onedivwidth = 1.0 / (float)width;
    float halfonedivwidth = -0.5f;
    for (size_t x = 0; x < width; x++)
    {
      for (size_t z = 0; z < width; z++)
      {
        size_t ipos = x + z*width;
        mesh->data->vertices[ipos].x = halfonedivwidth + ((float)x) * onedivwidth;
        mesh->data->vertices[ipos].z = halfonedivwidth + ((float)z) * onedivwidth;
        mesh->data->vertices[ipos].y = 0.0;
        mesh->data->vertex_normals[ipos].x = 0.0;
        mesh->data->vertex_normals[ipos].z = 0.0;
        mesh->data->vertex_normals[ipos].y = 1.0;
        mesh->data->vertex_tex_coords[ipos].s = ((float)x) * onedivwidth;
        mesh->data->vertex_tex_coords[ipos].t = ((float)z) * onedivwidth;
      }
    }
    for (int x = 1; x < width; x++)
    {
      for (int z = 1; z < width; z++)
      {
        vsx_face a;
        a.a = x-1 +  (z - 1) * width;
        a.b = x   +  (z - 1) * width;
        a.c = x-1 +  (z    ) * width;
        mesh->data->faces.push_back(a);
        a.a = x   +  (z - 1) * width;
        a.b = x-1 +  (z    ) * width;
        a.c = x   +  (z    ) * width;
        mesh->data->faces.push_back(a);
      }
    }

    first_run = false;
    mesh->timestamp++;
    result->set_p(mesh);
  }
};

class vsx_module_mesh_sphere : public vsx_module {
  // in
  vsx_module_param_float* num_sectors;
  vsx_module_param_float* num_stacks;
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
	int l_param_updates;
	int current_num_stacks;
	int current_num_sectors;

public: 
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_sphere";
    info->description = "";
    info->in_param_spec = "num_sectors:float?min=2,num_stacks:float?min=2";
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    num_sectors = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_sectors");
    num_sectors->set(6.0f);

    num_stacks = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_stacks");
    num_stacks->set(4.0f);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    current_num_stacks = 0;
    current_num_sectors = 0;
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


    int new_num_stacks = (int)num_stacks->get();
    int new_num_sectors = (int)num_sectors->get();

    if (current_num_stacks == new_num_stacks && current_num_sectors == new_num_sectors) return;

  	mesh->data->reset();

    current_num_sectors = new_num_sectors;
    current_num_stacks = new_num_stacks;

    int vi = 0; // vertex index

		for(int i = 1; i < current_num_stacks; i++) {
			double angle_stack = (double)i / current_num_stacks * PI;
			float rad = (float)sin(angle_stack);
			float y = (float)cos(angle_stack);
			for(int j = 0; j < current_num_sectors; j++) {
				double angle = (double)j / current_num_sectors * 2 * PI;
		    vsx_vector tmp_vec((float)sin(angle) * rad, y, (float)cos(angle) * rad);
		    //printf("%f %f %f\n", tmp_vec.x, tmp_vec.y, tmp_vec.z);
		    mesh->data->vertices[vi] = tmp_vec;
		    mesh->data->vertex_normals[vi] = tmp_vec;
		    mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
		    vi++;
			}
		}

		vsx_vector tmp_vec = vsx_vector(0, 1, 0);
    mesh->data->vertices[vi] = tmp_vec;
    mesh->data->vertex_normals[vi] = tmp_vec;
    mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
  	vi++;
		tmp_vec = vsx_vector(0, -1, 0);
    mesh->data->vertices[vi] = tmp_vec;
    mesh->data->vertex_normals[vi] = tmp_vec;
    mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
  	vi++;

  	for(int i = 0; i < current_num_stacks - 2; i++) {
			for(int j = 0; j < current_num_sectors; j++) {
				vsx_face a;
				a.a = i * current_num_sectors + j;
				a.b = (i + 1) * current_num_sectors + j;
				a.c = i * current_num_sectors + ((j + 1) % current_num_sectors);
		    //printf("%d %d %d\n", a.a, a.b, a.c);
				mesh->data->faces.push_back(a);
				a.a = i * current_num_sectors + ((j + 1) % current_num_sectors);
				a.b = (i + 1) * current_num_sectors + j;
				a.c = (i + 1) * current_num_sectors + ((j + 1) % current_num_sectors);
		    //printf("%d %d %d\n", a.a, a.b, a.c);
				mesh->data->faces.push_back(a);
			}
  	}

		for(int j = 0; j < current_num_sectors; j++) {
			vsx_face a;
			a.a = vi - 2;
			a.b = 0 * current_num_sectors + j;
			a.c = 0 * current_num_sectors + ((j + 1) % current_num_sectors);
	    //printf("%d %d %d\n", a.a, a.b, a.c);
			mesh->data->faces.push_back(a);
			a.a = vi - 1;
			a.b = (current_num_stacks - 2) * current_num_sectors + ((j + 1) % current_num_sectors);
			a.c = (current_num_stacks - 2) * current_num_sectors + j;
	    //printf("%d %d %d\n", a.a, a.b, a.c);
			mesh->data->faces.push_back(a);
		}

  	//printf("%d\n", vi);
		mesh->timestamp++;
    result->set_p(mesh);
  }
};

class vsx_module_mesh_abstract_hand : public vsx_module {
  // in
  vsx_module_param_float* num_sectors;
  vsx_module_param_float* num_stacks;
  vsx_module_param_float* z_length;
  vsx_module_param_sequence* param_x_shape;
  vsx_module_param_sequence* param_y_shape;
  vsx_module_param_sequence* param_z_shape;
  vsx_module_param_sequence* param_size_shape_x;
  vsx_module_param_sequence* param_size_shape_y;
  vsx_module_param_float* x_shape_multiplier;
  vsx_module_param_float* y_shape_multiplier;
  vsx_module_param_float* z_shape_multiplier;
  vsx_module_param_float* size_shape_x_multiplier;
  vsx_module_param_float* size_shape_y_multiplier;
  // out
  vsx_module_param_mesh* result;
  vsx_module_param_float* last_vertex_index;
  // internal
  vsx_mesh* mesh;
  int l_param_updates;
  int current_num_stacks;
  int current_num_sectors;

  // x_shape
  vsx_sequence seq_x_shape;
  float x_shape[8192];
  // y_shape
  vsx_sequence seq_y_shape;
  float y_shape[8192];
  // z_shape
  vsx_sequence seq_z_shape;
  float z_shape[8192];
  // size_shape_x
  vsx_sequence seq_size_shape_x;
  float size_shape_x[8192];
  // size_shape_y
  vsx_sequence seq_size_shape_y;
  float size_shape_y[8192];


  void calc_shapes() {
#define CALCS(var_name) \
    if (param_##var_name->updates)\
    {\
      seq_##var_name = param_##var_name->get();\
      param_##var_name->updates = 0;\
      seq_##var_name.reset();\
      for (int i = 0; i < 8192; ++i) {\
        var_name[i] = seq_##var_name.execute(1.0f/8192.0f);\
      }\
    }

    CALCS(size_shape_x);
    CALCS(size_shape_y);
    CALCS(x_shape);
    CALCS(y_shape);
    CALCS(z_shape);

#undef CALCS
  }


public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_super_banana";
    info->description = "";
    info->in_param_spec = "num_sectors:float?min=2,"
                          "num_stacks:float?min=2,"
                          "shape:complex{"
                            "x_shape:sequence,"
                            "x_shape_multiplier:float,"
                            "y_shape:sequence,"
                            "y_shape_multiplier:float,"
                            "z_shape:sequence,"
                            "z_shape_multiplier:float"
                          "},"
                          "size:complex{"
                            "size_shape_x:sequence,"
                            "size_shape_x_multiplier:float,"
                            "size_shape_y:sequence,"
                            "size_shape_y_multiplier:float"
                          "}"
        ;
    info->out_param_spec = "mesh:mesh,"
                           "last_vertex_index:float";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    param_x_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "x_shape");
    param_y_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "y_shape");
    param_z_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "z_shape");
    param_x_shape->set(seq_x_shape);
    param_y_shape->set(seq_y_shape);
    param_z_shape->set(seq_z_shape);

    x_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "x_shape_multiplier"); x_shape_multiplier->set(1.0f);
    y_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "y_shape_multiplier"); y_shape_multiplier->set(1.0f);
    z_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "z_shape_multiplier"); z_shape_multiplier->set(1.0f);
    size_shape_x_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size_shape_x_multiplier"); size_shape_x_multiplier->set(1.0f);
    size_shape_y_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size_shape_y_multiplier"); size_shape_y_multiplier->set(1.0f);

    param_size_shape_x = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "size_shape_x");
    param_size_shape_x->set(seq_size_shape_x);

    param_size_shape_y = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "size_shape_y");
    param_size_shape_y->set(seq_size_shape_y);

    num_sectors = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_sectors");
    num_sectors->set(6.0f);

    num_stacks = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_stacks");
    num_stacks->set(4.0f);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    
    
    last_vertex_index = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "last_vertex_index");

    current_num_stacks = 0;
    current_num_sectors = 0;
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
    int new_num_stacks = (int)num_stacks->get();
    int new_num_sectors = (int)num_sectors->get();
    if (param_updates == 0) return;

    param_updates = 0;
//    if (current_num_stacks == new_num_stacks && current_num_sectors == new_num_sectors) return;

    mesh->data->reset();
    calc_shapes();

    current_num_sectors = new_num_sectors;
    current_num_stacks = new_num_stacks;

    float x_shape_multiplier_f = x_shape_multiplier->get();
    float y_shape_multiplier_f = y_shape_multiplier->get();
    float z_shape_multiplier_f = z_shape_multiplier->get();
    float size_shape_x_multiplier_f = size_shape_x_multiplier->get();
    float size_shape_y_multiplier_f = size_shape_y_multiplier->get();


    int vi = 0; // vertex index

    mesh->data->faces.reset_used();

    float one_div_num_stacks = 1.0f / (float)current_num_stacks;
    //float one_div_num_sectors = 1.0f / (float)current_num_sectors;
    float one_div_num_sectors_minus_one = 1.0f / (float)(current_num_sectors - 1);

    //for(int i = 1; i < current_num_stacks; i++)
    for(int i = 0; i < current_num_stacks; i++)
    {
      // banana extends in z direction
      // x and y are the roundness
      float ip = (float)i * one_div_num_stacks;
      int index8192 = (int)round(8192.0f*ip);

      vsx_vector circle_base_pos = vsx_vector(
                                                x_shape[index8192] * x_shape_multiplier_f,
                                                y_shape[index8192] * y_shape_multiplier_f,
                                                z_shape[index8192] * z_shape_multiplier_f
                                                );
      int j;
      for(j = 0; j < current_num_sectors; j++)
      {
        double j1 = (float)j * one_div_num_sectors_minus_one;
        vsx_vector tmp_vec(
            circle_base_pos.x + cos(j1 * TWO_PI) * size_shape_x[index8192] * size_shape_x_multiplier_f,
            circle_base_pos.y + sin(j1 * TWO_PI) * size_shape_y[index8192] * size_shape_y_multiplier_f,
            circle_base_pos.z
        );
        mesh->data->vertices[vi] = tmp_vec;
        mesh->data->vertex_normals[vi] = tmp_vec - circle_base_pos;
        mesh->data->vertex_normals[vi].normalize();
        mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
        mesh->data->vertex_tex_coords[vi].s = j1;
        mesh->data->vertex_tex_coords[vi].t = ip;

        if (i && j)
        {
          vsx_face a;
          // c
          //
          // b   a (vi - 10)
          a.a = vi - current_num_sectors;
          a.b = vi-1;
          a.c = vi - current_num_sectors-1;
          mesh->data->faces.push_back(a);
          // b   c (vi)
          //
          //     a (vi - 10)
          a.a = vi - current_num_sectors;
          a.b = vi;
          a.c = vi-1;
          mesh->data->faces.push_back(a);
        }
        vi++;
      }
      /*if (i > 1 && i < current_num_stacks-1)
      {
        vsx_face a;
        // c
        //
        // b   a (vi - 10)
        a.c = vi - current_num_sectors;
        a.b = vi - current_num_sectors-1;
        a.a = vi-1;
        mesh->data->faces.push_back(a);
        // b   c (vi)
        //
        //     a (vi - 10)
        a.c = vi - current_num_sectors;
        a.b = vi-1;
        a.a = vi;
        mesh->data->faces.push_back(a);
      }*/
    }

      /*


      double i1 = (double)(i-1) / (current_num_stacks-1);
      float global_shape = sin(i1 * 3.14159);
      double angle_stack = (double)i / current_num_stacks * PI;
      float rad = (float)sin(angle_stack);
      float y = (float)cos(angle_stack);
      //double i1 = (double)i / current_num_sectors;
      for(int j = 0; j < current_num_sectors; j++)
      {
        double j1 = (double)j / current_num_sectors;
        double angle = j1 *  PI;
        float s = (float)sin(angle) * rad;
        vsx_vector tmp_vec(
              s*growth->get() * global_shape,
              y * global_shape,
              ( (float)cos(angle) * rad  +  sin(s * 2.14) * bend->get()) * global_shape
        );
        //printf("%f %f %f\n", tmp_vec.x, tmp_vec.y, tmp_vec.z);
        mesh->data->vertices[vi] = tmp_vec;
        mesh->data->vertex_normals[vi] = tmp_vec;
        mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
        vi++;
      }*/

    /*vsx_vector tmp_vec = vsx_vector(0, 1, 0);
    mesh->data->vertices[vi] = tmp_vec;
    mesh->data->vertex_normals[vi] = tmp_vec;
    mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
    vi++;
    tmp_vec = vsx_vector(0, -1, 0);
    mesh->data->vertices[vi] = tmp_vec;
    mesh->data->vertex_normals[vi] = tmp_vec;
    mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);
    vi++;*/

/*    for(int i = 0; i < (current_num_stacks - 2); i++) {
      for(int j = 0; j < current_num_sectors; j++) {
        vsx_face a;
        a.a = i * current_num_sectors + j;
        a.b = (i + 1) * current_num_sectors + j;
        a.c = i * current_num_sectors + ((j + 1) % current_num_sectors);
        //printf("%d %d %d\n", a.a, a.b, a.c);
        mesh->data->faces.push_back(a);
        a.a = i * current_num_sectors + ((j + 1) % current_num_sectors);
        a.b = (i + 1) * current_num_sectors + j;
        a.c = (i + 1) * current_num_sectors + ((j + 1) % current_num_sectors);
        //printf("%d %d %d\n", a.a, a.b, a.c);
        mesh->data->faces.push_back(a);
      }
    }

    for(int j = 0; j < current_num_sectors; j++) {
      vsx_face a;
      a.a = vi - 2;
      a.b = 0 * current_num_sectors + j;
      a.c = 0 * current_num_sectors + ((j + 1) % current_num_sectors);
      //printf("%d %d %d\n", a.a, a.b, a.c);
      mesh->data->faces.push_back(a);
      a.a = vi - 1;
      a.b = (current_num_stacks - 2) * current_num_sectors + ((j + 1) % current_num_sectors);
      a.c = (current_num_stacks - 2) * current_num_sectors + j;
      //printf("%d %d %d\n", a.a, a.b, a.c);
      mesh->data->faces.push_back(a);
    }
*/
    //printf("%d\n", vi);
    last_vertex_index->set( (float)vi );
    mesh->timestamp++;
    result->set_p(mesh);
  }
};

class vsx_module_mesh_torus_knot : public vsx_module {
  // in
  vsx_module_param_float* num_sectors;
  vsx_module_param_float* num_stacks;
  vsx_module_param_float* p;
  vsx_module_param_float* q;
  vsx_module_param_float* phi_offset;
  vsx_module_param_float* z_length;
  vsx_module_param_sequence* param_x_shape;
  vsx_module_param_sequence* param_y_shape;
  vsx_module_param_sequence* param_z_shape;
  vsx_module_param_sequence* param_size_shape_x;
  vsx_module_param_sequence* param_size_shape_y;
  //vsx_module_param_float* x_shape_multiplier;
  //vsx_module_param_float* y_shape_multiplier;
  //vsx_module_param_float* z_shape_multiplier;
  vsx_module_param_float* size_shape_x_multiplier;
  vsx_module_param_float* size_shape_y_multiplier;
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh* mesh;
  int l_param_updates;
  int current_num_stacks;
  int current_num_sectors;

  // x_shape
  vsx_sequence seq_x_shape;
  float x_shape[8192];
  // y_shape
  vsx_sequence seq_y_shape;
  float y_shape[8192];
  // z_shape
  vsx_sequence seq_z_shape;
  float z_shape[8192];
  // size_shape_x
  vsx_sequence seq_size_shape_x;
  float size_shape_x[8192];
  // size_shape_y
  vsx_sequence seq_size_shape_y;
  float size_shape_y[8192];


  void calc_shapes() {
#define CALCS(var_name) \
    if (param_##var_name->updates)\
    {\
      seq_##var_name = param_##var_name->get();\
      param_##var_name->updates = 0;\
      seq_##var_name.reset();\
      for (int i = 0; i < 8192; ++i) {\
        var_name[i] = seq_##var_name.execute(1.0f/8192.0f);\
      }\
    }

    CALCS(size_shape_x);
    CALCS(size_shape_y);
    CALCS(x_shape);
    CALCS(y_shape);
    CALCS(z_shape);

#undef CALCS
  }


public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;solid;mesh_torus_knot";
    info->description = "";
    info->in_param_spec = "num_sectors:float?min=2,"
        "num_stacks:float?min=2,"
        "p:float,"
        "q:float,"
        "phi_offset:float,"
        "size:complex{"
        "size_shape_x:sequence,"
        "size_shape_x_multiplier:float,"
        "size_shape_y:sequence,"
        "size_shape_y_multiplier:float"
        "}"
        ;
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    param_x_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "x_shape");
    param_y_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "y_shape");
    param_z_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "z_shape");
    param_x_shape->set(seq_x_shape);
    param_y_shape->set(seq_y_shape);
    param_z_shape->set(seq_z_shape);

    //x_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "x_shape_multiplier"); x_shape_multiplier->set(1.0f);
    //y_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "y_shape_multiplier"); y_shape_multiplier->set(1.0f);
    //z_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "z_shape_multiplier"); z_shape_multiplier->set(1.0f);
    size_shape_x_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size_shape_x_multiplier"); size_shape_x_multiplier->set(1.0f);
    size_shape_y_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size_shape_y_multiplier"); size_shape_y_multiplier->set(1.0f);

    param_size_shape_x = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "size_shape_x");
    param_size_shape_x->set(seq_size_shape_x);

    param_size_shape_y = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE, "size_shape_y");
    param_size_shape_y->set(seq_size_shape_y);

    num_sectors = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_sectors");
    num_sectors->set(6.0f);

    num_stacks = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_stacks");
    num_stacks->set(4.0f);

    p = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"p");
    p->set(4.0f);
    q = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"q");
    q->set(3.0f);
    phi_offset = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"phi_offset");
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");

    current_num_stacks = 0;
    current_num_sectors = 0;
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

    if (param_updates == 0) return;

    param_updates = 0;

    int new_num_stacks = (int)num_stacks->get();
    int new_num_sectors = (int)num_sectors->get();

//    if (current_num_stacks == new_num_stacks && current_num_sectors == new_num_sectors) return;

    mesh->data->reset();
    calc_shapes();

    current_num_sectors = new_num_sectors;
    current_num_stacks = new_num_stacks;

    //float x_shape_multiplier_f = x_shape_multiplier->get();
    //float y_shape_multiplier_f = y_shape_multiplier->get();
    //float z_shape_multiplier_f = z_shape_multiplier->get();
    float size_shape_x_multiplier_f = size_shape_x_multiplier->get();
    float size_shape_y_multiplier_f = size_shape_y_multiplier->get();


    int vi = 0; // vertex index

    float Q = q->get();
    float P = p->get();
    float phiofs = phi_offset->get();

    mesh->data->faces.reset_used();
    float one_div_num_stacks = 1.0f / (float)(current_num_stacks);

    //int num_vertices = current_num_stacks * (current_num_sectors-1);

    for(int i = 0; i < current_num_stacks; i++)
    {
      // banana extends in z direction
      // x and y are the roundness
      float ip = (float)i * one_div_num_stacks;
      float ip2 = (float)(i+1) * one_div_num_stacks;
      float phi = TWO_PI * ip;
      float phi2 = TWO_PI * ip2;

      // knot vertex pos

      float r = 0.5f * (2.0f + sin( Q * phi ) );



      int index8192 = (int)round(8192.0f*ip) % 8192;

      vsx_vector circle_base_pos = vsx_vector(
                                                r * cos ( P * phi + phiofs),
                                                r * cos ( Q * phi + phiofs),
                                                r * sin ( P * phi + phiofs)
                                                //x_shape[index8192] * x_shape_multiplier_f,
                                                //y_shape[index8192] * y_shape_multiplier_f,
                                                //z_shape[index8192] * z_shape_multiplier_f
                                                );
      vsx_vector circle_base_pos_phi2 = vsx_vector(
                                                r * cos ( P * phi2 +phiofs),
                                                r * cos ( Q * phi2 +phiofs),
                                                r * sin ( P * phi2 +phiofs)
                                                );

      // rotation calculation
      vsx_vector T = circle_base_pos_phi2 - circle_base_pos;
      vsx_vector N = circle_base_pos_phi2 + circle_base_pos;
      vsx_vector B;
      B.cross(T, N);
      N.cross(B, T);
      B.normalize();
      N.normalize();


      float j1_div_num_sectors = 1.0f / (float)(current_num_sectors);
      int j;
      for(j = 0; j < current_num_sectors; j++)
      {
        double j1 = (float)j * j1_div_num_sectors;

        //newpoint = point.x * N + point.y * B;

        float px = cos(j1 * TWO_PI) * size_shape_x[index8192] * size_shape_x_multiplier_f;
        float py = sin(j1 * TWO_PI) * size_shape_y[index8192] * size_shape_y_multiplier_f;

        vsx_vector tmp_vec(
            circle_base_pos.x,
            circle_base_pos.y,
            circle_base_pos.z
        );

        tmp_vec += N * px + B * py;

        mesh->data->vertices[vi] = tmp_vec;
        mesh->data->vertex_normals[vi] = tmp_vec - circle_base_pos;
        mesh->data->vertex_normals[vi].normalize();
        mesh->data->vertex_colors[vi] = vsx_color(1, 1, 1, 1);

        if (i && j)
        {
          vsx_face a;
          // c                      current row
          //
          // b   a (vi - 10)        prev row


          a.c = vi - current_num_sectors;
          a.b = vi - current_num_sectors-1;
          a.a = vi-1;
          //if (i==1) {
            //if (j > 1) mesh->data->faces.push_back(a);
          //}
          //else
          //if (j)
          mesh->data->faces.push_back(a);

          // b   c (vi)
          //
          //     a (vi - 10)
          a.c = vi - current_num_sectors;
          a.b = vi-1;
          a.a = vi;
          mesh->data->faces.push_back(a);
        }
        vi++;
      }
      if (i > 1 && i < current_num_stacks-1)
      {
        //vi--;
        vsx_face a;
        // (vi-1)
        // a
        //
        // b   c (vi - 10)
//        if (rand()%5 == 2)

        //int ji = vi % num_vertices;//

        {
          a.c = vi - current_num_sectors ;
          a.b = vi - current_num_sectors - 1;
          a.a = vi - 1;
          mesh->data->faces.push_back(a);
        }
        {
          a.b = vi - current_num_sectors;
          a.c = vi;
          a.a = vi - 1;
          mesh->data->faces.push_back(a);
        }
        // (vi-1)
        // b   a (vi)
        //
        //     c (vi - 10)



        //a.c = vi - current_num_sectors+1;
        //a.b = vi;
        //a.a = vi - current_num_sectors;
        //mesh->data->faces.push_back(a);
        //vi++;
      }



      /*if (i > 1)// && i < current_num_stacks-1)
      {
        vsx_face a;
        // c
        //
        // b   a (vi - 10)
        a.c = vi - current_num_sectors;
        a.b = vi - current_num_sectors-1;
        a.a = vi-1;
        mesh->data->faces.push_back(a);
        // b   c (vi)
        //
        //     a (vi - 10)
        a.c = vi - current_num_sectors;
        a.b = vi-1;
        a.a = vi;
        mesh->data->faces.push_back(a);
      }*/
    }

    //if (rand()%4 == 2)
    for(int j = 0; j < current_num_sectors-1; j++)
    {
      if (j)
      {
        vsx_face a;
        // c                      current row
        //
        // b   a (vi - 10)        prev row
        a.c = vi - current_num_sectors;
        a.b = vi - current_num_sectors-1;
        a.a = j-1;
        mesh->data->faces.push_back(a);
        // b   c (vi)
        //
        //     a (vi - 10)
        a.c = vi - current_num_sectors;
        a.b = j-1;
        a.a = j;
        mesh->data->faces.push_back(a);
      }
      vi++;
    }
    {
      vsx_face a;
      a.c = vi - current_num_sectors ;
      a.b = vi - current_num_sectors - 1;
      a.a = current_num_sectors - 1;
      mesh->data->faces.push_back(a);
    }
    /*{
      vsx_face a;
      a.c = 0;
      a.b = vi - current_num_sectors - 1;
      a.a = current_num_sectors -2;
      mesh->data->faces.push_back(a);
    }*/
    //printf("vi: %d\n", vi);
    // vi=4829
    // vi-current_num_sectors == 4819

    // id=4818, 0, 8


    //printf("%d\n", vi);
    mesh->timestamp++;
    result->set_p(mesh);
  }
};

class vsx_module_mesh_needle : public vsx_module {
  // in
	vsx_module_param_float* num_points;
	vsx_module_param_float* size;
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;

public:
	void module_info(vsx_module_info* info)
	{
	  info->identifier = "mesh;vertices;needle";
	  info->description = "";
	  info->out_param_spec = "mesh:mesh";
	  info->in_param_spec =
	"\
num_points:float,\
size:float\
";
	  info->component_class = "mesh";
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
		num_points = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_points");
		size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"size");
		num_points->set(5);
		size->set(1);

		result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
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
	  if (!param_updates) return;  param_updates = 0;
	  float istart = 0;
	  for (int i = 0; i < 5; ++i) {
	    mesh->data->vertices[i].x = istart;
	    mesh->data->vertices[i].y = 0;
	    mesh->data->vertices[i].z = 0;
	    mesh->data->vertex_colors[i].r = 1;
	    mesh->data->vertex_colors[i].g = size->get()/5.0f;
	    mesh->data->vertex_colors[i].b = 1;
	    mesh->data->vertex_colors[i].a = 1;
	    istart += size->get()/5.0f;
	  }
	  mesh->timestamp++;

	}
};

class vsx_module_mesh_ribbon : public vsx_module {
  // in
  vsx_module_param_float3* start_point;
  vsx_module_param_float3* end_point;
  vsx_module_param_float3* up_vector;
  vsx_module_param_float* width;
  vsx_module_param_float* skew_amp;
  vsx_module_param_float* time_amp;
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh* mesh;
  int l_param_updates;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;generators;ribbon";
    info->description = "";
    info->in_param_spec =
        "start_point:float3,"
        "end_point:float3,"
        "up_vector:float3,"
        "width:float,"
        "skew_amp:float,"
        "time_amp:float"
        ;
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;
    start_point = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"start_point");
    end_point = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"end_point");
    up_vector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"up_vector");
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"width");
    skew_amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"skew_amp");
    skew_amp->set(1.0f);
    time_amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time_amp");
    time_amp->set(1.0f);
    width->set(0.1f);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    mesh = new vsx_mesh;
  }

  bool init() {
    return true;
  }

  void on_delete()
  {
    if (mesh)
    delete mesh;
    mesh = 0;
  }

  void run()
  {
    //if (l_param_updates != param_updates) first_run = true;
    mesh->data->vertices[0] = vsx_vector(0);

    vsx_vector a(start_point->get(0), start_point->get(1), start_point->get(2));
    vsx_vector b(end_point->get(0), end_point->get(1), end_point->get(2));
    vsx_vector up(up_vector->get(0), up_vector->get(1), up_vector->get(2));
    up *= width->get();


    vsx_vector pos = a;
    vsx_vector diff = b-a;
    vsx_vector diff_n = diff;
    diff_n.normalize();

    vsx_vector normal;
    vsx_vector up_n = up;
    up_n.normalize();
    normal.cross(diff_n, up_n);

    vsx_vector up_side = normal;
    up_side *= up.length();

    float t = engine->vtime * time_amp->get();

#define COUNT 20.0f
    diff *= 1.0f / COUNT;
    float skew_amount = skew_amp->get();
    //     i=0   1   2   3   4   5   6   7   8   9
    // /\    0   2   4   6   8   10  12  14  16  18
    // ||    x---x---x---x---x---x---x---x---x---x
    // up   a|0/1|2/3| / | / | / | / | / | / | / |b  ----> diff
    //       x---x---x---x---x---x---x---x---x---x
    //       1   3   5   7   9   11  13  15  17  19

    mesh->data->faces.reset_used();
    for (int i = 0; i < (int)COUNT; i++)
    {
      int i2 = i << 2;
      float it = (float)i / COUNT;
      float ft = sin(it * 3.14159f + t) * sin(-it * 5.18674f - t);// + ( (float)(rand()%1000) * 0.0003f);
      float thick = sin(it * 3.14159f);
      vsx_vector skew = up * ft * skew_amount * thick;

      mesh->data->vertices[i2    ] = pos + up * thick + skew;
      mesh->data->vertices[i2 + 1] = pos - up * thick + skew;

      mesh->data->vertices[i2 + 2] = pos + skew + up_side * thick;
      mesh->data->vertices[i2 + 3] = pos + skew - up_side * thick;

      mesh->data->vertex_normals[i2    ] = normal;
      mesh->data->vertex_normals[i2 + 1] = normal;
      mesh->data->vertex_normals[i2 + 2] = normal;
      mesh->data->vertex_normals[i2 + 3] = normal;

      pos += diff;

      mesh->data->vertex_colors[i2] = vsx_color(1, 1, 1, 1);
      mesh->data->vertex_colors[i2+1] = vsx_color(1, 1, 1, 1);
      mesh->data->vertex_colors[i2+2] = vsx_color(1, 1, 1, 1);
      mesh->data->vertex_colors[i2+3] = vsx_color(1, 1, 1, 1);

      mesh->data->vertex_tex_coords[i2]   = vsx_tex_coord__(it, 0);
      mesh->data->vertex_tex_coords[i2+1] = vsx_tex_coord__(it, 1);
      mesh->data->vertex_tex_coords[i2+2] = vsx_tex_coord__(it, 0);
      mesh->data->vertex_tex_coords[i2+3] = vsx_tex_coord__(it, 1);

      if (i)
      {
        vsx_face a;
        //printf("i2: %d\n", i2);

        a.a = i2;
        a.b = i2 - 3;
        a.c = i2 - 4;
        mesh->data->faces.push_back(a);

        a.a = i2;
        a.b = i2 + 1;
        a.c = i2 - 3;
        mesh->data->faces.push_back(a);

        a.a = i2 + 2;
        a.b = i2 - 1;
        a.c = i2 - 2;
        mesh->data->faces.push_back(a);

        a.a = i2 + 2;
        a.b = i2 + 3;
        a.c = i2 - 1;
        mesh->data->faces.push_back(a);


      }
    }
#undef COUNT



      mesh->timestamp++;
      result->set(mesh);
    //}
    /*else {
      if (num_points->get() < mesh->data->vertices.size()) {
        mesh->data->vertices.reset_used((int)num_points->get());
      } else
      if (num_points->get() > mesh->data->vertices.size()) {
        for (int i = mesh->data->vertices.size(); i < (int)num_points->get(); ++i) {
          mesh->data->vertices[i].x = (rand()%10000)*0.0001*scaling->get(0);
          mesh->data->vertices[i].y = (rand()%10000)*0.0001*scaling->get(1);
          mesh->data->vertices[i].z = (rand()%10000)*0.0001*scaling->get(2);
        }

      }
    }
      printf("randMesh done %d\n",mesh->data->vertices.size());*/

    //  }
  }
};

class vsx_module_mesh_ribbon_cloth : public vsx_module {
  // in
  vsx_module_param_float3* start_point;
  vsx_module_param_float3* end_point;
  vsx_module_param_float3* up_vector;
  vsx_module_param_float* width;
  vsx_module_param_float* skew_amp;
  vsx_module_param_float* time_amp;
  vsx_module_param_float* damping_factor;
  vsx_module_param_float* step_size;
  vsx_module_param_float* stiffness;
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh* mesh;
  int l_param_updates;
  bool regen;
  vsx_array<vsx_vector> face_lengths;
  vsx_array<vsx_vector> vertices_speed;
  vsx_array<vsx_vector> vertices_orig;
  int num_runs;
  vsx_vector prev_pos;
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
    info->identifier = "mesh;generators;ribbon_cloth";
    info->description = "";
    info->in_param_spec =
        "start_point:float3,"
        "end_point:float3,"
        "damping_Factor:float,"
        "step_size:float,"
        "stiffness:float,"
        ;
    info->out_param_spec = "mesh:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;
    start_point = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"start_point");
    end_point = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"end_point");

    damping_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"damping_Factor");
    damping_factor->set(0.5f);
    
    step_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"step_size");
    step_size->set(1.0f);

    stiffness = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"stiffness");
    stiffness->set(0.8f);

    up_vector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"up_vector");
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"width");
    skew_amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"skew_amp");
    skew_amp->set(1.0f);
    
    time_amp = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time_amp");
    time_amp->set(1.0f);
    width->set(0.1f);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    regen = true;
    num_runs = 0;
  }


  void run()
  {
    mesh->data->vertices[0] = vsx_vector(0);

    vsx_vector a(start_point->get(0), start_point->get(1), start_point->get(2));
    vsx_vector b(end_point->get(0), end_point->get(1), end_point->get(2));
    vsx_vector up(up_vector->get(0), up_vector->get(1), up_vector->get(2));
    up *= width->get();


    vsx_vector pos = vsx_vector(0.0f,0.0f,0.0f);
    vsx_vector diff = b-a;
    vsx_vector diff_n = diff;
    diff_n.normalize();

    vsx_vector normal;
    vsx_vector up_n = up;
    up_n.normalize();
    normal.cross(diff_n, up_n);

    vsx_vector up_side = normal;
    up_side *= up.length();

    float t = engine->vtime * time_amp->get();

#define COUNT 20.0f
    diff *= (0.4f*0.1f) / COUNT;
    float skew_amount = skew_amp->get();
    //     i=0   1   2   3   4   5   6   7   8   9
    // /\    0   2   4   6   8   10  12  14  16  18
    // ||    x---x---x---x---x---x---x---x---x---x
    // up   a|0/1|2/3| / | / | / | / | / | / | / |b  ----> diff
    //       x---x---x---x---x---x---x---x---x---x
    //       1   3   5   7   9   11  13  15  17  19

    vsx_vector addpos;
    
    if (regen)
    {
      prev_pos = a;
      vertices_speed.allocate((int)COUNT*4);
      for (int i = 0; i < (int)COUNT*4; i++)
      {
        vertices_speed[i].x = 0.0f;
        vertices_speed[i].y = -0.04f;
        vertices_speed[i].z = 0.0f;
      }
      
      regen = false;
      mesh->data->faces.reset_used();
      for (int i = 0; i < (int)COUNT; i++)
      {
        int i2 = i << 1;
        float it = (float)i / COUNT;
        float ft = sin(it * 3.14159f + t) * sin(-it * 5.18674f - t);// + ( (float)(rand()%1000) * 0.0003f);
        float thick = 0.58f*0.11f;//sin(it * 3.14159f);
        vsx_vector skew = up * ft * skew_amount * thick;

        mesh->data->vertices[i2    ] = pos + up * thick + skew;
        mesh->data->vertices[i2 + 1] = pos - up * thick + skew;
        vertices_orig[i2] = mesh->data->vertices[i2    ];
        vertices_orig[i2+1] = mesh->data->vertices[i2   +1];

        mesh->data->vertex_normals[i2    ] = normal;
        mesh->data->vertex_normals[i2 + 1] = normal;

        pos += diff;

        mesh->data->vertex_colors[i2] = vsx_color(1, 1, 1, 1);
        mesh->data->vertex_colors[i2+1] = vsx_color(1, 1, 1, 1);

        mesh->data->vertex_tex_coords[i2]   = vsx_tex_coord__(it, 0);
        mesh->data->vertex_tex_coords[i2+1] = vsx_tex_coord__(it, 1);

        //vertices_speed[i2] = vsx_vector(0, 0, 0);
        //vertices_speed[i2+1] = vsx_vector(0, 0, 0);

        vsx_vector len;
        if (i>1)
        {
          vsx_face f;
          //printf("i2: %d\n", i2);
          //
          //i2-2      i2
          //
          //i2-1      i2+1
          {
            f.a = i2;
            f.b = i2 - 1;
            f.c = i2 - 2;
            mesh->data->faces.push_back(f);

            vsx_vector v0 = mesh->data->vertices[f.a];
            vsx_vector v1 = mesh->data->vertices[f.b];
            vsx_vector v2 = mesh->data->vertices[f.c];

            len.x = fabs( (v1 - v0).length()+(float)(rand()%1000)*0.0001f);
            len.y = fabs( (v2 - v1).length()+(float)(rand()%1000)*0.0001f);
            len.z = fabs( (v0 - v2).length()+(float)(rand()%1000)*0.00005f);
            #define TRESH 0.04f
            if (len.x < TRESH) len.x = TRESH;
            if (len.y < TRESH) len.y = TRESH;
            if (len.z < TRESH) len.z = TRESH;

            face_lengths.push_back(len);
          }
  
          {
            f.a = i2-1;
            f.b = i2;
            f.c = i2+1;
            /*f.a = i2 + 1;
            f.b = i2 - 1;
            f.c = i2;*/
            mesh->data->faces.push_back(f);

            vsx_vector v0 = mesh->data->vertices[f.a];
            vsx_vector v1 = mesh->data->vertices[f.b];
            vsx_vector v2 = mesh->data->vertices[f.c];

            len.x = fabs( (v1 - v0).length()+(float)(rand()%1000)*0.0001f );
            len.y = fabs( (v2 - v1).length()+(float)(rand()%1000)*0.0001f );
            len.z = fabs( (v0 - v2).length()+(float)(rand()%1000)*0.00005f );
            #define TRESH 0.04f
            if (len.x < TRESH) len.x = TRESH;
            if (len.y < TRESH) len.y = TRESH;
            if (len.z < TRESH) len.z = TRESH;
            
            
            face_lengths.push_back(len);
          }
        }
      }
  #undef COUNT
    }

    //vertices_speed.allocate( mesh->data->vertices.size() );
    float fcount = 1.0f / (float)mesh->data->faces.size();
    float dirx = -b.x*0.05f;
    float dirz = -b.z*0.05f;
    float stepsizemultiplier = 0.02f * step_size->get();
    float gravity_pull = -0.01f;
    vsx_face* face_p = mesh->data->faces.get_pointer();
    vsx_vector* vertices_speed_p = vertices_speed.get_pointer();
    vsx_vector* faces_length_p = face_lengths.get_pointer();
    vsx_vector* vertex_p = mesh->data->vertices.get_pointer();

    for (int j = 0; j < 8; j++)
    {
      vsx_face* face_p_it = face_p;
      for(unsigned int i = 0; i < mesh->data->faces.size(); i++) {
        // face fetching
        unsigned long fa = (*face_p_it).a;
        unsigned long fb = (*face_p_it).b;
        unsigned long fc = (*face_p_it).c;
        face_p_it++;
        // ---
        
        vsx_vector v0 = vertex_p[fa];
        vsx_vector v1 = vertex_p[fb];
        vsx_vector v2 = vertex_p[fc];
        
        vsx_vector edgeA = (v1 - v0);
        vsx_vector edgeB = (v2 - v1);
        vsx_vector edgeC = (v0 - v2);

        float lenA = edgeA.length();
        float lenB = edgeB.length();
        float lenC = edgeC.length();

        if (lenA < 0.0001f) lenA = 0.0001f;
        if (lenB < 0.0001f) lenB = 0.0001f;
        if (lenC < 0.0001f) lenC = 0.0001f;
        float edgeForceA = (face_lengths[i].x - lenA) / faces_length_p[i].x;
        float edgeForceB = (face_lengths[i].y - lenB) / faces_length_p[i].y;
        float edgeForceC = (face_lengths[i].z - lenC) / faces_length_p[i].z;
        float edgeAccA = edgeForceA / lenA;
        float edgeAccB = edgeForceB / lenB;
        float edgeAccC = edgeForceC / lenC;
        vsx_vector accA = edgeA * edgeAccA;
        vsx_vector accB = edgeB * edgeAccB;
        vsx_vector accC = edgeC * edgeAccC;

        float ii = 1.0f - (float)i * fcount;

        vertices_speed_p[fa] -= (accA - accC)*stiffness->get();// * 0.8f;//(0.10f+0.9*ii);
        vertices_speed_p[fb] -= (accB - accA)*stiffness->get();// * 0.8f;//(0.10f+0.9*ii);
        vertices_speed_p[fc] -= (accC - accB)*stiffness->get();// * 0.8f;//(0.10f+0.9*ii);

        vertices_speed_p[fa].y += gravity_pull;
        vertices_speed_p[fb].y += gravity_pull;
        vertices_speed_p[fc].y += gravity_pull;
        float sp2 = pow(sin(ii*1.57f),3.0f)*2.0f;

        vertices_speed_p[fa].x -= dirx*sp2;
        vertices_speed_p[fb].x -= dirx*sp2;
        vertices_speed_p[fc].x -= dirx*sp2;

        vertices_speed_p[fa].z -= dirz*sp2;
        vertices_speed_p[fb].z -= dirz*sp2;
        vertices_speed_p[fc].z -= dirz*sp2;
      }
      vsx_vector mdist = a-prev_pos;  // prev_pos-------->a
      float mdl = mdist.length();
      if (mdl > 0.07f)
      {
        // prev_pos    <---0.1f a
        mdist.normalize();
        mdist.x = -mdist.x;
        mdist.y = -mdist.y;
        mdist.z = -mdist.z;
        //printf("newlength: %f\n", mdl-0.01f);
        addpos = mdist*(mdl-0.07f);
        //printf("addpos: %f, %f, %f\n", addpos.x, addpos.y, addpos.z);
        for(unsigned long i = 4; i < mesh->data->vertices.size(); i++) {
          vertex_p[i] -= addpos;
        }
      }
      prev_pos = a;
      for(unsigned long i = 0; i < 4; i++)
      {
        mesh->data->vertices[i] = a;
      }
      
      for(unsigned long i = 4; i < mesh->data->vertices.size(); i++) {
        //mesh->data->vertices[i] -= addpos;
        vertex_p[i] += (vertices_speed[i] * stepsizemultiplier);
        if (mesh->data->vertices[i].y < 0.0f) mesh->data->vertices[i].y = 0.0f;
        vertices_speed[i] = vertices_speed[i] * damping_factor->get();
      }
    }
    for(unsigned long i = 0; i < mesh->data->faces.size(); i++) {
      vsx_vector a = mesh->data->vertices[mesh->data->faces[i].b] - mesh->data->vertices[mesh->data->faces[i].a];
      vsx_vector b = mesh->data->vertices[mesh->data->faces[i].c] - mesh->data->vertices[mesh->data->faces[i].a];
      vsx_vector normal;
      normal.cross(a,b);

      normal = -normal;
      normal.normalize();
      mesh->data->vertex_normals[mesh->data->faces[i].a] = normal;
      mesh->data->vertex_normals[mesh->data->faces[i].b] = normal;
      mesh->data->vertex_normals[mesh->data->faces[i].c] = normal;
    }

    mesh->timestamp++;
    result->set(mesh);
    num_runs++;
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
    case 0: return (vsx_module*)(new vsx_module_mesh_needle);
    case 1: return (vsx_module*)(new vsx_module_mesh_rand_points);
    case 2: return (vsx_module*)(new vsx_module_mesh_rays);
    case 3: return (vsx_module*)(new vsx_module_mesh_disc);
    case 4: return (vsx_module*)(new vsx_module_mesh_planes);
    case 5: return (vsx_module*)(new vsx_module_mesh_box);
    case 6: return (vsx_module*)(new vsx_module_mesh_sphere);
    case 7: return (vsx_module*)(new vsx_module_mesh_supershape);
    case 8: return (vsx_module*)(new vsx_module_mesh_ribbon);
    case 9: return (vsx_module*)(new vsx_module_mesh_abstract_hand);
    case 10: return (vsx_module*)(new vsx_module_mesh_torus_knot);
    case 11: return (vsx_module*)(new vsx_module_mesh_lightning_vertices);
    case 12: return (vsx_module*)(new vsx_module_mesh_ribbon_cloth);
    case 13: return (vsx_module*)(new vsx_module_mesh_bspline_vertices);
    case 14: return (vsx_module*)(new vsx_module_mesh_grid);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_mesh_needle*)m; break;
    case 1: delete (vsx_module_mesh_rand_points*)m; break;
    case 2: delete (vsx_module_mesh_rays*)m; break;
    case 3: delete (vsx_module_mesh_disc*)m; break;
    case 4: delete (vsx_module_mesh_planes*)m; break;
    case 5: delete (vsx_module_mesh_box*)m; break;
    case 6: delete (vsx_module_mesh_sphere*)m; break;
    case 7: delete (vsx_module_mesh_supershape*)m; break;
    case 8: delete (vsx_module_mesh_ribbon*)m; break;
    case 9: delete (vsx_module_mesh_abstract_hand*)m; break;
    case 10: delete (vsx_module_mesh_torus_knot*)m; break;
    case 11: delete (vsx_module_mesh_lightning_vertices*)m; break;
    case 12: delete (vsx_module_mesh_ribbon_cloth*)m; break;
    case 13: delete (vsx_module_mesh_bspline_vertices*)m; break;
    case 14: delete (vsx_module_mesh_grid*)m; break;
  }
} 

unsigned long get_num_modules() {
  return 15;
}

