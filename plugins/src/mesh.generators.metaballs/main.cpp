#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "marching_cubes.h"
#include "metaballs.h"
#include "main.h"
#include "vsx_math_3d.h"
#include "vsx_timer.h"


class vsx_module_metaballs : public vsx_module {
  // in
	vsx_module_param_float* grid_size;
	vsx_module_param_float4* center_color;
	CMetaballs balls;
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh mesh;
	bool first_run;
	int i_grid_size;
public:
void module_info(vsx_module_info* info)
{
  info->identifier = "mesh;solid;metaballs";
  info->description = "Generates a metaballs mesh";
  info->in_param_spec = "grid_size:float?nc=1";
  info->out_param_spec = "mesh:mesh";
  info->component_class = "mesh";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
	CMarchingCubes::BuildTables();

  balls.SetGridSize(20);
  i_grid_size = 20;
  grid_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"grid_size");
  grid_size->set(20.0f);
//	num_rays = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"num_rays");
//	num_rays->set(40);
//	n_rays = 40;
/*	center_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"center_color");
	center_color->set(0.5,0);
	center_color->set(0.5,1);
	center_color->set(0.5,2);
	center_color->set(1,3);*/
	result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
  result->set_p(mesh);
  first_run = true;
}

void run() {
  int l_grid_size = (int)floor(grid_size->get());
  if (i_grid_size != l_grid_size) {
    balls.SetGridSize(l_grid_size);
    i_grid_size = l_grid_size;
  }
  if (engine->dtime != 0.0f) {
  	float dd = engine->dtime;
  	if (dd < 0) dd = 0;
  vsx_timer timer;
  timer.start();
  balls.Update(dd);
		
		//printf("update took: %f s\n", timer.dtime());
  timer.start();
    balls.Render();
		
  //printf("render took: %f s\n", timer.dtime());
  mesh.data->vertices = balls.vertices;
  mesh.data->vertex_normals = balls.vertex_normals;
  mesh.data->vertex_tex_coords = balls.vertex_tex_coords;
  mesh.data->faces = balls.faces;
  mesh.timestamp++;
  }
  result->set_p(mesh);
}
// mesh data is cleared on deletion since we're just stealing pointers here
};






//******************************************************************************************
//******************************************************************************************
//******************************************************************************************

#if BUILDING_DLL
vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_metaballs);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_metaballs*)m; break;
  }
}


unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 1;
}

#endif

