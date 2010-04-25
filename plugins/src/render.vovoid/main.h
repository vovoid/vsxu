#ifndef _DLL_H_
#define _DLL_H_

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */


class vsx_module_plugin : public vsx_module {
  // in
  vsx_module_param_bitmap* bitm_in;
	// out
	vsx_module_param_render* render_result;
	vsx_module_param_bitmap* positive_x;
	vsx_module_param_bitmap* negative_x;
	vsx_module_param_bitmap* positive_y;
	vsx_module_param_bitmap* negative_y;
	vsx_module_param_bitmap* positive_z;
	vsx_module_param_bitmap* negative_z;
	// internal
	

public:
	vsx_bitmap* bitm;
	vsx_bitmap bitm_p;
	vsx_bitmap result_bitm[6];
	vsx_texture result_tex[6];
	int bitm_timestamp;
	int mapMode;
	
  int cylSizeX, cylSizeY;

	unsigned long getColor(float u, float v);
  unsigned long getColorSph(float *vec);
	void skyBoxFromMap(int plane);


  void module_info(vsx_module_info* info);
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
	void run();
	void output(vsx_module_param_abs* param);
	void stop();
	void start();
	void on_delete();
};


extern "C" {
_declspec(dllexport) vsx_module* create_new_module(unsigned long module);
_declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
_declspec(dllexport) unsigned long get_num_modules();
}


#endif /* _DLL_H_ */
