#ifndef _DLL_H_
#define _DLL_H_

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */



// ******************************************************

class module_bitmap_add_noise : public vsx_module {
  // in
	vsx_module_param_bitmap* bitmap_in;
	float time;
	
	// out
	vsx_module_param_bitmap* result1;
	// internal
	vsx_bitmap* bitm;
  vsx_bitmap t_bitm;

	int buf, frame;
	unsigned long *data_a;
	unsigned long *data_b;
	int bitm_timestamp;
	vsx_bitmap result_bitm;
	bool first, worker_running, t_done;
  pthread_t					worker_t;
  pthread_attr_t		worker_t_attr;
	
public:
  virtual void module_info(vsx_module_info* info);
	virtual void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
	virtual void run();
	virtual void start();
	virtual void stop();
	void on_delete();
};


extern "C" {
_declspec(dllexport) vsx_module* create_new_module(unsigned long module);
_declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
_declspec(dllexport) unsigned long get_num_modules();

}


#endif /* _DLL_H_ */
