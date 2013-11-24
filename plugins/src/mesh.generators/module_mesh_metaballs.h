#include "metaballs/marching_cubes.h"
#include "metaballs/metaballs.h"

class module_mesh_metaballs : public vsx_module
{
  // in
  vsx_module_param_float* grid_size;
  vsx_module_param_float4* center_color;
  CMetaballs balls;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh* mesh;
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
    grid_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"grid_size");
    grid_size->set(20.0f);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    loading_done = true;
    first_run = true;
  }

  bool init()
  {
    mesh = new vsx_mesh;
    balls.vertices = &(mesh->data->vertices);
    balls.vertex_normals = &(mesh->data->vertex_normals);
    balls.vertex_tex_coords = &(mesh->data->vertex_tex_coords);
    balls.faces = &(mesh->data->faces);
    CMarchingCubes::BuildTables();
    balls.SetGridSize(20);
    i_grid_size = 20;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void run() {
    int l_grid_size = (int)floor(grid_size->get());
    if (i_grid_size != l_grid_size) {
      balls.SetGridSize(l_grid_size);
      i_grid_size = l_grid_size;
    }
    if (!(engine->dtime > 0.0f))
      return;

    float dd = engine->dtime;
    if (dd < 0) dd = 0;
    balls.Update(dd);

    balls.Render();

    mesh->timestamp++;
    result->set_p(mesh);
  }
};
