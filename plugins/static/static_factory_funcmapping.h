vsx_module* vsx_module_bitmap_generators_cm(unsigned long module, void* args);
void vsx_module_bitmap_generators_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_bitmap_generators_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_bitmap_loaders_cm(unsigned long module, void* args);
void vsx_module_bitmap_loaders_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_bitmap_loaders_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_bitmap_modifiers_cm(unsigned long module, void* args);
void vsx_module_bitmap_modifiers_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_bitmap_modifiers_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module__deprecated_cm(unsigned long module, void* args);
void vsx_module__deprecated_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module__deprecated_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_math_cm(unsigned long module, void* args);
void vsx_module_math_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_math_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_math_oscillators_cm(unsigned long module, void* args);
void vsx_module_math_oscillators_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_math_oscillators_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_mesh_generators_cm(unsigned long module, void* args);
void vsx_module_mesh_generators_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_mesh_generators_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_mesh_importers_cm(unsigned long module, void* args);
void vsx_module_mesh_importers_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_mesh_importers_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_mesh_modifiers_cm(unsigned long module, void* args);
void vsx_module_mesh_modifiers_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_mesh_modifiers_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_mesh_render_cm(unsigned long module, void* args);
void vsx_module_mesh_render_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_mesh_render_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_outputs_screen_opengl_cm(unsigned long module, void* args);
void vsx_module_outputs_screen_opengl_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_outputs_screen_opengl_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_particlesystem_generators_cm(unsigned long module, void* args);
void vsx_module_particlesystem_generators_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_particlesystem_generators_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_particlesystem_modifiers_cm(unsigned long module, void* args);
void vsx_module_particlesystem_modifiers_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_particlesystem_modifiers_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_particlesystem_render_cm(unsigned long module, void* args);
void vsx_module_particlesystem_render_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_particlesystem_render_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_render_basic_cm(unsigned long module, void* args);
void vsx_module_render_basic_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_render_basic_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_render_glsl_cm(unsigned long module, void* args);
void vsx_module_render_glsl_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_render_glsl_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_render_gravity_lines_cm(unsigned long module, void* args);
void vsx_module_render_gravity_lines_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_render_gravity_lines_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_render_opengl_cm(unsigned long module, void* args);
void vsx_module_render_opengl_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_render_opengl_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_render_text_cm(unsigned long module, void* args);
void vsx_module_render_text_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_render_text_nm(vsx_module_engine_environment* environment);

//vsx_module* vsx_module_selectors_cm(unsigned long module, void* args);
//void vsx_module_selectors_dm(vsx_module* m, unsigned long module);
//unsigned long vsx_module_selectors_nm(vsx_module_engine_environment* environment);

#ifdef VSXU_AUDIO_SINK
  vsx_module* vsx_module_sound_sink_cm(unsigned long module, void* args);
  void vsx_module_sound_sink_dm(vsx_module* m, unsigned long module);
  unsigned long vsx_module_sound_sink_nm(vsx_module_engine_environment* environment);
#else
  vsx_module* vsx_module_sound_rtaudio_cm(unsigned long module, void* args);
  void vsx_module_sound_rtaudio_dm(vsx_module* m, unsigned long module);
  unsigned long vsx_module_sound_rtaudio_nm(vsx_module_engine_environment* environment);
#endif

vsx_module* vsx_module_string_cm(unsigned long module, void* args);
void vsx_module_string_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_string_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_system_cm(unsigned long module, void* args);
void vsx_module_system_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_system_nm(vsx_module_engine_environment* environment);

vsx_module* vsx_module_texture_cm(unsigned long module, void* args);
void vsx_module_texture_dm(vsx_module* m, unsigned long module);
unsigned long vsx_module_texture_nm(vsx_module_engine_environment* environment);

//vsx_module* vsx_module_vision_cm(unsigned long module, void* args);
//void vsx_module_vision_dm(vsx_module* m, unsigned long module);
//unsigned long vsx_module_vision_nm(vsx_module_engine_environment* environment);
