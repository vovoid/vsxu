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


inline void eval2D(double a, double b, float m,float n1,float n2,float n3,float phi,float &x,float &y){
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

class module_mesh_supershape : public vsx_module
{
public:
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
  vsx_mesh<>* mesh;
  int n_segs;
  int l_param_updates;


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;solid;mesh_solid_supershape";

    info->description = "";

    info->in_param_spec =
      "x:complex{"
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
      "}"
    ;

    info->out_param_spec =
      "mesh:mesh";

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
  }

  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  vsx_nw_vector< vsx_vector3<> > prev_row_normals;

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

    float scale = 1.0f;


    float x1=0,y1=0,z1=0;
    float x2=0,y2=0,z2=0;
    float x3=0,y3=0,z3=0;

    float phi = _y_start;

    prev_row_normals.reset_used();

    vsx_vector3<> prev_norm;

    for (int i = 0; i < _x_num_segments+1; i++)
    {
      float theta = (float)_x_start;

      for(int j = 0; j < _y_num_segments+1; j++)
      {

        eval3D(_x_a, _x_b, _x_m,_x_n1,_x_n2,_x_n3,phi       ,theta         ,x1,y1,z1);
        eval3D(_x_a, _x_b, _x_m,_x_n1,_x_n2,_x_n3,phi       ,theta+theta_step,x2,y2,z2);
        eval3D(_x_a, _x_b,_x_m,_x_n1,_x_n2,_x_n3,phi+phi_step,theta+theta_step,x3,y3,z3);

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
        vsx_vector3<> norm(-nx/vLen,  -ny/vLen,  -nz/vLen);
        mesh->data->vertex_normals[vi] = norm;
        mesh->data->vertices[vi] = vsx_vector3<>(x1*scale, y1*scale, z1*scale);
        vi++;

        // per-vertex normals (try)
        if (i > 0 && j > 0)
        {
          vsx_vector3<> sum_norm = norm + prev_norm + prev_row_normals[j] + prev_row_normals[j-1];
          sum_norm.normalize();
          mesh->data->vertex_normals[vi-2] = sum_norm;
          mesh->data->vertex_normals[vi-3] = sum_norm;
        }
        prev_norm = norm;

        if (i > 0 && j > 0)
        {
          vsx_face3 a;
          a.a = vi - 1; // (0)
          a.b = vi - _x_num_segments-1;
          a.c = vi - _x_num_segments-2;
          mesh->data->faces.push_back(a);
          a.a = vi-2;
          a.b = vi-1;
          a.c = vi - _x_num_segments-2;
          mesh->data->faces.push_back(a);

        }
        theta += theta_step;

        prev_row_normals[j] = norm;
      }
      phi += phi_step;
    }

    mesh->timestamp++;
    result->set_p(mesh);
  }
};
