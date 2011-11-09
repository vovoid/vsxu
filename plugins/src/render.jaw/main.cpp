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

//#include <string>

#include "_configuration.h"
#include <vsx_string.h>
#include <vsx_param.h>
#include <vsx_module.h>
#include <vsx_math_3d.h>
#include <time.h>
#include <vsx_bspline.h>
#include <vsx_grid_mesh.h>

#include "segmesh.h"
#include "planeworld.h"

class vsx_cloud_plane : public vsx_module {
  // in
  vsx_module_param_float* size;
  vsx_module_param_float* angle;
  vsx_module_param_float* red;
  vsx_module_param_float* green;
  vsx_module_param_float* blue;
  vsx_module_param_float3* rotation_axis;
  // out
  vsx_module_param_render* render_result;
  vsx_module_param_mesh* mesh_result;
  // internal
  vsx_mesh* mesh;
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
    info->identifier = "mesh;vovoid;cloud_plane";
    info->in_param_spec = "";
    info->out_param_spec = "mesh_result:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_result");
  }

  void simple_box() {
    vsx_2dgrid_mesh gmesh;
    for (int x = 0; x < 50; ++x) {
      for (int y = 0; y < 50; ++y) {
        gmesh.vertices[x][y].coord = vsx_vector(((float)x-25)*0.8f,(float(rand()%1000))*0.0002f,(float(y)-25)*0.8f);
        gmesh.vertices[x][y].tex_coord = vsx_vector(((float)x)/50.0f,((float)y)/50.0f,0);
        gmesh.vertices[x][y].color = vsx_color((float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.0005f);
      }
    }
    for (int x = 0; x < 49; ++x) {
      for (int y = 0; y < 49; ++y) {
        gmesh.add_face(x,y+1,  x+1,y+1,  x,y);
        gmesh.add_face(x,y,  x+1,y+1,  x+1,y);
      }
    }
    gmesh.calculate_vertex_normals();

    gmesh.dump_vsx_mesh(mesh);
    mesh->data->calculate_face_centers();
    mesh->timestamp++;
    loading_done = true;
  }

  void run() {
    if (mesh->data->faces.get_used()) return;
    simple_box();
    mesh_result->set(mesh);
  }
};


  const float incr = PI_FLOAT/15.0f;
  const float f_end = PI_FLOAT*2;

  const float b_end = 0.999999f;
  const float b_mult = 0.999999f/20.0f;
class vsx_module_thorn : public vsx_module {
  // in
  vsx_module_param_float* size;
  vsx_module_param_float* angle;
  vsx_module_param_float* red;
  vsx_module_param_float* green;
  vsx_module_param_float* blue;
  vsx_module_param_float3* rotation_axis;
  // out
  vsx_module_param_render* render_result;
  vsx_module_param_mesh* mesh_result;
  // internal
  GLUquadricObj *quadratic;
  vsx_mesh* mesh;


  float mfunc(float b) {
    float a = 0.2f-(float)fabs(0.2f*b*b);
    if (a < 0.0f) a = 0.00001f;
    return a;
  }

  float sfunc(float b) {
    return b*2;
  }

  float dfunc(float b) {
    return b*b;
  }

  float gfunc(float b) {
    return 0.4;
  }
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
  // make sure this code doesn't load any DLL's or build anything, that is, load the DLL in the run method

  // set the identifier. this should be dynamic, so in the real world
  info->identifier = "mesh;vovoid;thorn";

// i left this development renderer (in wich we test controller initializations)
// untouched so you can see how you can build this dynamically.
  info->in_param_spec = "";
  info->out_param_spec = "mesh_result:mesh";
//  info->out_param_spec = "render_out:render";
  info->component_class = "mesh";
}


void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_result");
  mesh_result->set(mesh);

}


void run() {
  if (mesh->data->faces.size()) return;

  unsigned long v_pos = 0;
  unsigned long f_pos = 0;
  vsx_vector vtemp;

  float r = 0.2f;
  for (float b = -0.999999f; b < b_end; b+=b_mult) {
    r = mfunc(b);

    vsx_vector p[8];
    p[0].y = sfunc(b+b_mult*2);
    p[1].y = sfunc(b+b_mult*2);
    p[2].y = sfunc(b+b_mult);
    p[3].y = sfunc(b);
    p[4].y = sfunc(b-b_mult);
    p[5].y = sfunc(b-b_mult);
    p[6].y = sfunc(b);
    p[7].y = sfunc(b+b_mult);
    vsx_vector i[4];
    i[0].y = sfunc(b+b_mult);
    i[1].y = sfunc(b+b_mult);
    i[2].y = sfunc(b);
    i[3].y = sfunc(b);
    vsx_vector fn[8];
    vsx_vector fi;
    vsx_vector vn[4];


    for (float a = incr; a < f_end+incr; a+=incr) {
      i[0].x = gfunc(b+b_mult)*mfunc(b+b_mult)*(float)cos(a-incr)+dfunc(b+b_mult);
      i[0].z = mfunc(b+b_mult)*(float)sin(a-incr);
      i[1].x = gfunc(b+b_mult)*mfunc(b+b_mult)*(float)cos(a)+dfunc(b+b_mult);
      i[1].z = mfunc(b+b_mult)*(float)sin(a);
      i[2].x = gfunc(b)*r*(float)cos(a)+dfunc(b);
      i[2].z = r*(float)sin(a);
      i[3].x = gfunc(b)*r*(float)cos(a-incr)+dfunc(b);
      i[3].z = r*(float)sin(a-incr);

      p[0].x = gfunc(b+2*b_mult)*mfunc(b+2*b_mult)*(float)cos(a-incr)+dfunc(b+2*b_mult);
      p[0].z = mfunc(b+2*b_mult)*(float)sin(a-incr);
      p[1].x = gfunc(b+2*b_mult)*mfunc(b+2*b_mult)*(float)cos(a)+dfunc(b+2*b_mult);
      p[1].z = mfunc(b+2*b_mult)*(float)sin(a);
      p[2].x = gfunc(b+b_mult)*mfunc(b+b_mult)*(float)cos(a+incr)+dfunc(b+b_mult);
      p[2].z = mfunc(b+b_mult)*(float)sin(a+incr);
      p[3].x = gfunc(b)*r*(float)cos(a+incr)+dfunc(b);
      p[3].z = r*(float)sin(a+incr);
      p[4].x = gfunc(b-b_mult)*mfunc(b-b_mult)*(float)cos(a)+dfunc(b-b_mult);
      p[4].z = mfunc(b-b_mult)*(float)sin(a);
      p[5].x = gfunc(b-b_mult)*mfunc(b-b_mult)*(float)cos(a-incr)+dfunc(b-b_mult);
      p[5].z = mfunc(b-b_mult)*(float)sin(a-incr);
      p[6].x = gfunc(b)*r*(float)cos(a-incr*2)+dfunc(b);
      p[6].z = r*(float)sin(a-incr*2);
      p[7].x = gfunc(b+b_mult)*mfunc(b+b_mult)*(float)cos(a-incr*2)+dfunc(b+b_mult);
      p[7].z = mfunc(b+b_mult)*(float)sin(a-incr*2.0f);
      if (b+b_mult < 1) {
        fn[0].assign_face_normal(&p[0],&i[0],&p[7]);
        fn[0].normalize();
        fn[1].assign_face_normal(&p[1],&i[0],&p[0]);
        fn[1].normalize();
        fn[2].assign_face_normal(&p[2],&i[1],&p[1]);
        fn[2].normalize();
      } else {
        fn[0].x = fn[0].y = fn[0].z = 0;
        fn[1].x = fn[1].y = fn[1].z = 0;
        fn[2].x = fn[2].y = fn[2].z = 0;
      }

      fn[3].assign_face_normal(&p[2],&p[3],&i[2]);
      fn[3].normalize();
      if (b-b_mult > -1) {
        fn[4].assign_face_normal(&p[3],&p[4],&i[2]);
        fn[4].normalize();
        fn[5].assign_face_normal(&i[2],&p[5],&i[3]);
        fn[5].normalize();
        fn[6].assign_face_normal(&i[3],&p[5],&p[6]);
        fn[6].normalize();
      } else {
        fn[4].x = fn[4].y = fn[4].z = 0;
        fn[5].x = fn[5].y = fn[5].z = 0;
        fn[6].x = fn[6].y = fn[6].z = 0;
      }

      fn[7].assign_face_normal(&i[0],&i[3],&p[6]);
      fn[7].normalize();
      fi.assign_face_normal(&i[1],&i[3],&i[0]);
      fi.normalize();

      vn[0] = fi+fn[0]+fn[1]+fn[7];
      vn[0].normalize();
      vn[1] = fi+fn[1]+fn[2]+fn[3];
      vn[1].normalize();
      vn[2] = fi+fn[3]+fn[4]+fn[5];
      vn[2].normalize();
      vn[3] = fi+fn[5]+fn[6]+fn[7];
      vn[3].normalize();

      mesh->data->vertices[v_pos].set(i[3].x,i[3].y,i[3].z);
      mesh->data->vertex_normals[v_pos].set(vn[3].x,vn[3].y,vn[3].z);
      mesh->data->vertex_colors[v_pos].set(vn[3].x,vn[3].y,vn[3].z,1);
      mesh->data->faces[f_pos].a = v_pos;
      ++v_pos;

      mesh->data->vertices[v_pos].set(i[0].x,i[0].y,i[0].z);
      mesh->data->vertex_normals[v_pos].set(vn[0].x,vn[0].y,vn[0].z);
      mesh->data->vertex_colors[v_pos].set(vn[0].x,vn[0].y,vn[0].z,1);
      mesh->data->faces[f_pos].b = v_pos;
      ++v_pos;

      mesh->data->vertices[v_pos].set(i[1].x,i[1].y,i[1].z);
      mesh->data->vertex_normals[v_pos].set(vn[1].x,vn[1].y,vn[1].z);
      mesh->data->vertex_colors[v_pos].set(vn[1].x,vn[1].y,vn[1].z,1);
      mesh->data->faces[f_pos].c = v_pos;
      ++v_pos;

      ++f_pos;



      mesh->data->vertices[v_pos].set(i[1].x,i[1].y,i[1].z);
      mesh->data->vertex_normals[v_pos].set(vn[1].x,vn[1].y,vn[1].z);
      mesh->data->vertex_colors[v_pos].set(vn[1].x,vn[1].y,vn[1].z,1);
      mesh->data->faces[f_pos].a = v_pos;
      ++v_pos;

      mesh->data->vertices[v_pos].set(i[2].x,i[2].y,i[2].z);
      mesh->data->vertex_normals[v_pos].set(vn[2].x,vn[2].y,vn[2].z);
      mesh->data->vertex_colors[v_pos].set(vn[2].x,vn[2].y,vn[2].z,1);
      mesh->data->faces[f_pos].b = v_pos;
      ++v_pos;

      mesh->data->vertices[v_pos].set(i[3].x,i[3].y,i[3].z);
      mesh->data->vertex_normals[v_pos].set(vn[3].x,vn[3].y,vn[3].z);
      mesh->data->vertex_colors[v_pos].set(vn[3].x,vn[3].y,vn[3].z,1);
      mesh->data->faces[f_pos].c = v_pos;
      ++v_pos;

      ++f_pos;

    }
  }

  mesh->timestamp++;
  mesh_result->set(mesh);

  loading_done = true;
}

};

/*

 This will be a specific case of a vsxu module in that we'll trick the main program into thinking
 we have as many modules as there are available salvation cabinets.
 We do so by returning a number when queried, wich vsxu will loop through creating one of each
 and running its [module_info] method to get the [info->identifier]-string.
 This is what ends up in the module browser tree, so we can create any number of virtual
 modules by just returning different identifiers. Of course the rest of the info needs
 to be altered as it suits us, perhaps you can find a way to accept float values into 
 salvation or something - then you have to dynamically create this (and the specification)
 in the module_info method.
 
*/



class vsx_module_segmesh_loft : public vsx_module {
  vsx_module_param_float* segments;
  vsx_module_param_float* length_p;
  vsx_module_param_segment_mesh* seg_mesh_in;
  vsx_module_param_segment_mesh* seg_mesh_out;

  vsx_module_param_sequence* param_x_shape;
  vsx_module_param_sequence* param_y_shape;
  vsx_module_param_sequence* param_z_shape;
  vsx_module_param_sequence* param_size_shape;

  vsx_2dgrid_mesh base_mesh;
  //vsx_2dgrid_mesh gmesh;
  vsx_sequence seq;

  long lx_t;
  long ly_t;
  long sides;
  float sx,sy;
  float n_inc;
  float length_inc;
  int i,j;
  bool first_run;

  // x_shape
  vsx_sequence seq_x_shape;
  float x_shape[8192];
  // y_shape
  vsx_sequence seq_y_shape;
  float y_shape[8192];
  // z_shape
  vsx_sequence seq_z_shape;
  float z_shape[8192];
  // size_shape
  vsx_sequence seq_size_shape;
  float sizes[8192];


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

    //CALCS(size_shape);
    CALCS(x_shape);
    CALCS(y_shape);
    CALCS(z_shape);

#undef CALCS
  }



public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;segmesh;segmesh_loft";
    info->in_param_spec = "segment_mesh_in:segment_mesh,"
        "loft_x:sequence,"
        "loft_y:sequence,"
        "loft_z:sequence,"
        "segments:float?min=2,"
        "length:float";
    info->out_param_spec = "segment_mesh_out:segment_mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    //printf("a\n");
    param_x_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"loft_x");
    //printf("a2\n");
    param_x_shape->set(seq_x_shape);
    param_y_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"loft_y");
    param_y_shape->set(seq_y_shape);

    param_z_shape = (vsx_module_param_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_SEQUENCE,"loft_z");
    param_z_shape->set(seq_z_shape);

    //printf("module_data: %d\n",loft_x->get_addr());

    //printf("b\n");
    segments = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"segments");
    segments->set(40);

    length_p = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"length");
    length_p->set(1.0f);

    //printf("c\n");
    seg_mesh_in = (vsx_module_param_segment_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"segment_mesh_in",true);
    //seg_mesh_in->set(base_mesh);
    //printf("d\n");
    seg_mesh_out = (vsx_module_param_segment_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"segment_mesh_out");
    seg_mesh_out->set_p(base_mesh);
    //printf("e\n");
    lx_t = -1;
    ly_t = -1;
    first_run = true;
  }

  void on_delete() {
    base_mesh.cleanup();
    //gmesh.cleanup();
  }

  void run()
  {
    //printf("segmesh_loft run\n");
    vsx_2dgrid_mesh* bb = seg_mesh_in->get_addr();
    if (bb) {
      //printf("gmesh vertices: %d\n",bb->vertices.size());
      //base_mesh.reset_vertices();
      //base_mesh.reset_faces();
      //gmesh = seg_mesh_in->get();
      //loft_x_seq = loft_x->get_addr();
      //loft_y_seq = loft_y->get_addr();
      sides = bb->vertices[0].size();
      //printf("sides: %d\n",sides);
      //if (lx_t != loft_x_seq->timestamp || ly_t != loft_y_seq->timestamp) {
        //printf("segmesh_loft run2 %f\n",segments->get());
        //lx_t = loft_x_seq->timestamp;
        //ly_t = loft_y_seq->timestamp;
        //loft_x_seq->reset();
        //loft_y_seq->reset();
      length_inc = length_p->get()/segments->get();
        //n_inc = 1.0f/(segments->get());
      calc_shapes();

      float zz = 0;
      long num_vertices = base_mesh.vertices.size();
      //if (num_vertices < 0) num_vertices = 0;
      for (unsigned long i = 0; i < segments->get(); ++i) {
        float ip = (float)i / (float)segments->get();
        int index8192 = (int)round(8192.0f*ip);
        sx = x_shape[index8192];//->execute(n_inc);//sin(PI/(num-1)*(float)(i)*5)*pow(((float)i/(float)num),2);
        sy = y_shape[index8192];//loft_y_seq->execute(n_inc);//sin(PI/(num-1)*(float)(i)*5)*pow(((float)i/(float)num),2);
        for (j = 0; j < (int)sides; ++j) {
          base_mesh.vertices[i][j].coord.x = bb->vertices[0][j].coord.x * sx;
          base_mesh.vertices[i][j].coord.y = bb->vertices[0][j].coord.y * sy;
          base_mesh.vertices[i][j].coord.z = zz * z_shape[index8192];
        }
        zz += length_inc;
      }
      if (segments->get() < base_mesh.vertices.size()) {
        base_mesh.vertices.reset_used((int)segments->get());
      }
        for (unsigned long i = num_vertices+1; i < floor(segments->get()); ++i) {
          for (j = 0; j < (int)sides; ++j) {
            base_mesh.vertices[i][j].color = vsx_color__((float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,1.0f);
          }
        }

        long face = num_vertices*sides;
        for (int i = num_vertices;  i < segments->get()-1; ++i) {
          for (int j = 0; j < (int)sides; ++j) {
            int j1 = j+1;
            if (j1 == (int)sides) j1 = 0;
            base_mesh.add_face(i+1,j1, i+1,j, i,j);
            base_mesh.add_face(i,j,  i,j1,  i+1,j1);
            //base_mesh.add_face(i+1,j1,  i,j, i,j1 );
            ++face;
          }
        }
        base_mesh.faces.reset_used(face);
      seg_mesh_out->set_p(base_mesh);
    }
    loading_done = true;
  }
};



//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifdef _WIN32
extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}
#endif


vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_thorn);
    case 1: return (vsx_module*)(new vsx_cloud_plane);
    case 2: return (vsx_module*)(new vsx_module_planeworld);
    case 3: return (vsx_module*)(new vsx_module_segmesh_map_bspline);
    case 4: return (vsx_module*)(new vsx_module_segmesh_shape_basic);
    case 5: return (vsx_module*)(new vsx_module_segmesh_loft);
    case 6: return (vsx_module*)(new vsx_module_segmesh_bspline_matrix);
  }
  return 0;
}


void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_thorn*)m; break;
    case 1: delete (vsx_cloud_plane*)m; break;
    case 2: delete (vsx_module_planeworld*)m; break;
    case 3: delete (vsx_module_segmesh_map_bspline*)m; break;
    case 4: delete (vsx_module_segmesh_shape_basic*)m; break;
    case 5: delete (vsx_module_segmesh_loft*)m; break;
    case 6: delete (vsx_module_segmesh_bspline_matrix*)m; break;
  }
}


unsigned long get_num_modules() {
  // in here you have to find out how many salvation modules are available and return
  // the correct number. The create_new_module will then create one of each and run
  //   vsx_module_plugin::module_info(vsx_module_info* info)
  // on it. The aim here is to make VSXU think that each and one of the available
  // slavation cabinets are a unique module.
    
  return 7;
}  


