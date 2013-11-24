#include <vsx_bspline.h>
// this should deliver a rotation matrix that is the rotation in a specific point
// on a spline defined by the vertices in bspline_mesh_in.
class module_bspline_matrix : public vsx_module
{
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

  void module_info(vsx_module_info* info)
  {
    info->identifier = "maths;bspline;bspline_matrix";
    info->in_param_spec = "\
      bspline_vertices_mesh:mesh,\
      b_pos:float";
    info->out_param_spec = "matrix_result:matrix,position:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    bspline_vertices_mesh = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"bspline_vertices_mesh");
    b_pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b_pos");
    matrix_result = (vsx_module_param_matrix*)out_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"matrix_result");
    matrix_result->set(matrix);
    position = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
    position->set(0,0);
    position->set(0,1);
    position->set(0,2);
    upv = vsx_vector(0,1);
    loading_done = true;
  }

  void run() {
    vsx_mesh** spline_mesh = bspline_vertices_mesh->get_addr();
    if (spline_mesh)
    {
      spline0.points.set_volatile();
      spline0.points.set_data((*spline_mesh)->data->vertices.get_pointer(),(*spline_mesh)->data->vertices.size());

    if (!spline0.points.size()) return;
    spline0.set_pos(b_pos->get()+0.1f);
      pos1 = spline0.calc_coord();
      spline0.step(0.1f);
      pos2 = spline0.calc_coord();
      e = pos2-pos1;
      e.normalize();

      vsx_quaternion q;
      q.x = e.x * (float)sin(1/2);
      q.y = e.y * (float)sin(1/2);
      q.z = e.z * (float)sin(1/2);
      q.w = (float)cos(1/2);
      q.normalize();

      matrix = q.matrix();


      matrix_result->set(matrix);
      position->set(pos2.x,0);
      position->set(pos2.y,1);
      position->set(pos2.z,2);
    }
    loading_done = true;
  }
};
