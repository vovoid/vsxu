//#include <string>

#include "_configuration.h"
#include "vsx_string.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"
#include "vsx_math_3d.h"
#include "thorn.h"
#include "plane.h"
#include "creature.h"

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
        sx = x_shape[index8192];//->execute(n_inc);//sin(pi/(num-1)*(float)(i)*5)*pow(((float)i/(float)num),2);
        sy = y_shape[index8192];//loft_y_seq->execute(n_inc);//sin(pi/(num-1)*(float)(i)*5)*pow(((float)i/(float)num),2);
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


