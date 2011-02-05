#ifndef VSX_CREATURE_H
#define VSX_CREATURE_H

#include "vsx_bspline.h"
#include "vsx_grid_mesh.h"

class vsx_module_segmesh_shape_basic : public vsx_module {
  vsx_module_param_int* shape_type;
  vsx_module_param_float* sides;
  vsx_module_param_segment_mesh* seg_mesh;
  vsx_module_param_float* size;
  vsx_2dgrid_mesh gmesh;
  float incr;
public:
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  void run();
  void on_delete();
};

//---------------------------------------------------------------------


//---------------------------------------------------------------------

// this should deliver a rotation matrix that is the rotation in a specific point
// on a spline defined by the vertices in bspline_mesh_in.

class vsx_module_segmesh_bspline_matrix : public vsx_module {
  // in
  vsx_module_param_mesh* bspline_vertices_mesh;
  vsx_module_param_float* b_pos;
  // out
  vsx_module_param_matrix* matrix_result;
  vsx_module_param_float3* position;
  // internal
  vsx_bspline spline0;
  vsx_matrix matrix;
  vsx_vector pos1, pos2, e, upv;
public:
  void module_info(vsx_module_info* info);
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
	void run();
};
//---------------------------------------------------------------------

class vsx_module_segmesh_map_bspline : public vsx_module {
  // in
	vsx_module_param_segment_mesh* seg_mesh_in;
	vsx_module_param_mesh* bspline_vertices_mesh;
	vsx_module_param_float* b_pos;
	vsx_module_param_float* length;
	// out
	vsx_module_param_render* render_result;
	vsx_module_param_mesh* mesh_result;
	// internal
  vsx_bspline spline0;
  // the points from which we constuct the b-spline
  vsx_mesh* spline_mesh;
  // the mesh we're gonna put on the path
  vsx_2dgrid_mesh *base_mesh;
  // our building tool
  vsx_2dgrid_mesh gmesh;
  
  int i,j;
  float stime;
  float sides;
  int num;
  
  // the step for each spline segment - defined as number of length/segments
  float spline_step;
  
  vsx_vector old;
  vsx_vector e;
  vsx_vector f;
  vsx_vector d;
  vsx_matrix ma;
  vsx_vector old_vec;
  vsx_vector spos;
  vsx_vector upv;
  // the resulting mesh
  vsx_mesh result_mesh;
public:
  void module_info(vsx_module_info* info);
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
	void run();
	void on_delete();
  ~vsx_module_segmesh_map_bspline();
};




class vsx_module_planeworld : public vsx_module {
  // in
  vsx_module_param_mesh* bspline_vertices_mesh;
	// out
	vsx_module_param_render* render_result;
	vsx_module_param_mesh* mesh_result;
	// internal
  vsx_mesh mesh;
  vsx_mesh* spline_mesh;
  vsx_bspline spline0;
  vsx_2dgrid_mesh gmesh;

public:
  void module_info(vsx_module_info* info);
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
	void run();
  void on_delete();
  ~vsx_module_planeworld();
};



#endif
