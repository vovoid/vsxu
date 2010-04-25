#ifndef VSX_THORN_H
#define VSX_THORN_H

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
  float mfunc(float b);
  float sfunc(float b);
  float dfunc(float b);
  float gfunc(float b);
  vsx_mesh mesh;
public:
  void module_info(vsx_module_info* info);
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
	void run();
};


#endif
