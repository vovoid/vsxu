
// commented modules are not part of the default build  
VSXM_SF_ADD_MODULE("bitmap.generators",vsx_module_bitmap_generators_cm,vsx_module_bitmap_generators_dm,vsx_module_bitmap_generators_nm);
  VSXM_SF_ADD_MODULE("bitmap.loaders",vsx_module_bitmap_loaders_cm,vsx_module_bitmap_loaders_dm,vsx_module_bitmap_loaders_nm);
  VSXM_SF_ADD_MODULE("bitmap.modifiers",vsx_module_bitmap_modifiers_cm,vsx_module_bitmap_modifiers_dm,vsx_module_bitmap_modifiers_nm);
  VSXM_SF_ADD_MODULE("_deprecated",vsx_module__deprecated_cm,vsx_module__deprecated_dm,vsx_module__deprecated_nm);
  VSXM_SF_ADD_MODULE("math",vsx_module_math_cm,vsx_module_math_dm,vsx_module_math_nm);
  VSXM_SF_ADD_MODULE("math.oscillators",vsx_module_math_oscillators_cm,vsx_module_math_oscillators_dm,vsx_module_math_oscillators_nm);
  VSXM_SF_ADD_MODULE("mesh.generators",vsx_module_mesh_generators_cm,vsx_module_mesh_generators_dm,vsx_module_mesh_generators_nm);
  VSXM_SF_ADD_MODULE("mesh.importers",vsx_module_mesh_importers_cm,vsx_module_mesh_importers_dm,vsx_module_mesh_importers_nm);
  VSXM_SF_ADD_MODULE("mesh.modifiers",vsx_module_mesh_modifiers_cm,vsx_module_mesh_modifiers_dm,vsx_module_mesh_modifiers_nm);
  VSXM_SF_ADD_MODULE("mesh.render",vsx_module_mesh_render_cm,vsx_module_mesh_render_dm,vsx_module_mesh_render_nm);
  VSXM_SF_ADD_MODULE("outputs.screen_opengl",vsx_module_outputs_screen_opengl_cm,vsx_module_outputs_screen_opengl_dm,vsx_module_outputs_screen_opengl_nm);
  VSXM_SF_ADD_MODULE("particlesystem.generators",vsx_module_particlesystem_generators_cm,vsx_module_particlesystem_generators_dm,vsx_module_particlesystem_generators_nm);
  VSXM_SF_ADD_MODULE("particlesystem.modifiers",vsx_module_particlesystem_modifiers_cm,vsx_module_particlesystem_modifiers_dm,vsx_module_particlesystem_modifiers_nm);
  VSXM_SF_ADD_MODULE("particlesystem.render",vsx_module_particlesystem_render_cm,vsx_module_particlesystem_render_dm,vsx_module_particlesystem_render_nm);
  VSXM_SF_ADD_MODULE("render.basic",vsx_module_render_basic_cm,vsx_module_render_basic_dm,vsx_module_render_basic_nm);
  VSXM_SF_ADD_MODULE("render.glsl",vsx_module_render_glsl_cm,vsx_module_render_glsl_dm,vsx_module_render_glsl_nm);
  VSXM_SF_ADD_MODULE("render.gravity_lines",vsx_module_render_gravity_lines_cm,vsx_module_render_gravity_lines_dm,vsx_module_render_gravity_lines_nm);
  VSXM_SF_ADD_MODULE("render.opengl",vsx_module_render_opengl_cm,vsx_module_render_opengl_dm,vsx_module_render_opengl_nm);
  VSXM_SF_ADD_MODULE("render.text",vsx_module_render_text_cm,vsx_module_render_text_dm,vsx_module_render_text_nm);
  #ifdef VSXU_AUDIO_SINK
    VSXM_SF_ADD_MODULE("sound.sink",vsx_module_sound_sink_cm,vsx_module_sound_sink_dm,vsx_module_sound_sink_nm);
  #else
    VSXM_SF_ADD_MODULE("sound.rtaudio", vsx_module_sound_rtaudio_cm, vsx_module_sound_rtaudio_dm, vsx_module_sound_rtaudio_nm);
  #endif
  VSXM_SF_ADD_MODULE("string",vsx_module_string_cm,vsx_module_string_dm,vsx_module_string_nm);
  VSXM_SF_ADD_MODULE("system",vsx_module_system_cm,vsx_module_system_dm,vsx_module_system_nm);
  VSXM_SF_ADD_MODULE("texture",vsx_module_texture_cm,vsx_module_texture_dm,vsx_module_texture_nm);
  // VSXM_SF_ADD_MODULE("selectors", vsx_module_selectors_cm,vsx_module_selectors_dm,vsx_module_selectors_nm);
  //  VSXM_SF_ADD_MODULE("vision",vsx_module_vision_cm,vsx_module_vision_dm,vsx_module_vision_nm);
