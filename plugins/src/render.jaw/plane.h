#ifndef VSX_CLOUD_PLANE_H
#define VSX_CLOUD_PLANE_H

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
  vsx_mesh mesh;
  void simple_box();
public:
  vsx_string salvation_module_name;
  void module_info(vsx_module_info* info);
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
	void run();
  ~vsx_cloud_plane();
};



#endif

