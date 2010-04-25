/*
 *  vsx_module_static.cpp
 *
 */
#ifdef VSXU_MODULES_STATIC
#include "vsx_param.h"

#include "vsx_module.h"
#include "vsx_module_static.h"
#include "vsx_log.h"
#include "../../plugin_base/outputs.screen/main.cpp"
#include "../../plugin_base/render.basic/main.cpp"
#include "../../plugin_base/maths.parameters.oscillators.oscillator/main.cpp"
#include "../../plugin_base/texture.bitmaps-lib/main.cpp"
#include "../../plugin_base/mesh.generators.simple-lib/main.cpp"
#include "../../plugin_base/render.mesh/main.cpp"
#include "../../plugin_base/render.opengl-lib/main.cpp"
#include "../../plugin_base/maths.basic/main.cpp"
#include "../../plugin_base/texture.buffer/main.cpp"
#include "../../plugin_base/mesh.generators.kaleidoscope/main.cpp"
#include "../../plugin_base/particles.modifiers.standard-lib/main.cpp"
#include "../../plugin_base/particles.generators/main.cpp"
#include "../../plugin_base/bitmap.texgen/main.cpp"
#include "../../plugin_base/render.particlesystem/main.cpp"
#include "../../plugin_base/sound.itunes/main.cpp"
#include "../../plugin_base/render.jaw-lib/thorn.h"
#include "../../plugin_base/render.jaw-lib/plane.h"
#include "../../plugin_base/render.jaw-lib/creature.h"
#include "../../plugin_base/system/main.cpp"
#include "../../plugin_base/_deprecated/main.cpp"
#include "../../plugin_base/mesh.generators.metaballs/main.cpp"
#include "../../plugin_base/render.oscilloscope/main.cpp"
#include "../../plugin_base/render.opengl.advanced-lib/main.cpp"
#include "../../plugin_base/render.gravity_lines/main.cpp"

vsx_module* create_named_module(vsx_string name)
{
	// 20 modules!
  #define ADD_MODULE(mn) if (name == #mn)	return (vsx_module*)new mn
  ADD_MODULE(vsx_module_output_screen);
	ADD_MODULE(vsx_module_render_basic_colored_rectangle);
	ADD_MODULE(vsx_module_render_line);
	ADD_MODULE(vsx_module_simple_with_texture);
	ADD_MODULE(vsx_module_plugin_maths_oscillator);
	ADD_MODULE(vsx_module_plugin_maths_oscillators_float_sequencer);
	if (name == "bitmaps;loaders;png_bitm_load")
	{
		module_load_png* m = new module_load_png;
		m->m_type = 0;
		return (vsx_module*)m;
	}
	if (name == "texture;loaders;png_tex_load")
	{
		module_load_png* m = new module_load_png;
		m->m_type = 1;
		return (vsx_module*)m;
	}
	if (name == "bitmaps;loaders;jpeg_bitm_load")
	{
		module_load_jpeg* m = new module_load_jpeg;
		m->m_type = 0;
		return (vsx_module*)m;
	}
	if (name == "texture;loaders;jpeg_tex_load")
	{
		module_load_jpeg* m = new module_load_jpeg;
		m->m_type = 1;
		return (vsx_module*)m;
	}
	ADD_MODULE(texture_loaders_bitmap2texture);
	
	ADD_MODULE(vsx_module_mesh_needle);
	ADD_MODULE(vsx_module_mesh_rand_points);
	ADD_MODULE(vsx_module_mesh_rays);
	ADD_MODULE(vsx_module_mesh_disc);
	ADD_MODULE(vsx_module_mesh_planes);
	ADD_MODULE(vsx_module_mesh_box);
	ADD_MODULE(vsx_module_mesh_sphere);
	ADD_MODULE(vsx_module_mesh_supershape);
	//18
	ADD_MODULE(vsx_module_mesh_render_line);
	ADD_MODULE(vsx_module_render_mesh);
	
	ADD_MODULE(vsx_module_blend_mode);
	ADD_MODULE(vsx_orbit_camera);
	ADD_MODULE(vsx_target_camera);
	ADD_MODULE(vsx_freelook_camera);
	ADD_MODULE(vsx_gl_translate);
	ADD_MODULE(vsx_depth_buffer);
	ADD_MODULE(vsx_gl_rotate);
	ADD_MODULE(vsx_light);
	ADD_MODULE(vsx_material_param);
	ADD_MODULE(vsx_gl_scale);
	ADD_MODULE(vsx_gl_matrix_multiply);
	ADD_MODULE(vsx_gl_color);
	ADD_MODULE(vsx_gl_orto_2d);
	ADD_MODULE(vsx_fog);
	ADD_MODULE(vsx_backface_cull);
	ADD_MODULE(vsx_gl_rotate_quat);
	ADD_MODULE(vsx_gl_normalize);
	ADD_MODULE(vsx_module_gl_matrix_get);
	ADD_MODULE(vsx_gl_line_width);
	ADD_MODULE(vsx_depth_buffer_clear);
	ADD_MODULE(vsx_depth_func);
	ADD_MODULE(vsx_texture_bind);

ADD_MODULE(module_3float_to_float3);
ADD_MODULE(module_4float_to_float4);
ADD_MODULE(vsx_module_4f_hsv_to_rgb_f4);
ADD_MODULE(vsx_float_dummy);
ADD_MODULE(vsx_float3_dummy);
ADD_MODULE(vsx_float_array_pick);
ADD_MODULE(vsx_arith_add);
ADD_MODULE(vsx_arith_sub);
ADD_MODULE(vsx_arith_mult);
ADD_MODULE(vsx_arith_div);
ADD_MODULE(vsx_arith_min);
ADD_MODULE(vsx_arith_max);
ADD_MODULE(vsx_arith_pow);
ADD_MODULE(vsx_arith_round);
ADD_MODULE(vsx_arith_floor);
ADD_MODULE(vsx_arith_ceil);
ADD_MODULE(vsx_float_accumulator);
ADD_MODULE(vsx_float3_accumulator);
ADD_MODULE(vsx_float4_accumulator);
ADD_MODULE(module_vector_add);
ADD_MODULE(module_vector_add_float);
ADD_MODULE(module_vector_mul_float);
ADD_MODULE(module_float_to_float3);
ADD_MODULE(vsx_float_abs);
ADD_MODULE(vsx_float_sin);
ADD_MODULE(vsx_float_cos);
ADD_MODULE(vsx_bool_and);
ADD_MODULE(vsx_bool_or);
ADD_MODULE(vsx_bool_nor);
ADD_MODULE(vsx_bool_xor);
ADD_MODULE(vsx_bool_not);
ADD_MODULE(module_vec4_mul_float);
ADD_MODULE(vsx_bool_nand);
ADD_MODULE(module_float4_add);
ADD_MODULE(vsx_float_array_average);
ADD_MODULE(vsx_arith_mod);
ADD_MODULE(vsx_module_f4_hsl_to_rgb_f4);
ADD_MODULE(vsx_float3to3float);
ADD_MODULE(vsx_float_limit);
ADD_MODULE(module_vector_4float_to_quaternion);
ADD_MODULE(vsx_float_interpolate);
ADD_MODULE(module_quaternion_rotation_accumulator_2d);
ADD_MODULE(module_vector_normalize);
ADD_MODULE(module_vector_cross_product);
ADD_MODULE(module_vector_dot_product);
ADD_MODULE(module_vector_from_points);
ADD_MODULE(module_quaternion_slerp_2);
ADD_MODULE(module_quaternion_mul);
ADD_MODULE(vsx_float_accumulator_limits);
	// 90
ADD_MODULE(vsx_module_rendered_texture_single);
ADD_MODULE(vsx_module_texture_translate);
ADD_MODULE(vsx_module_texture_scale);
ADD_MODULE(vsx_module_texture_rotate);
ADD_MODULE(vsx_module_texture_parameter);
	// 95
	ADD_MODULE(vsx_module_kaleido_mesh);
	ADD_MODULE(vsx_module_plugin_wind);
	ADD_MODULE(vsx_module_particle_size_noise);
	ADD_MODULE(vsx_module_plugin_gravity);
	ADD_MODULE(vsx_module_particle_floor);
	ADD_MODULE(vsx_module_plugin_fluid);
	if (name == "module_bitmap_blob_a") {
		module_bitmap_blob* b = new module_bitmap_blob; b->c_type = 0; return (vsx_module*)b;
	}
	if (name == "module_bitmap_blob_b") {
		module_bitmap_blob* b = new module_bitmap_blob; b->c_type = 1; return (vsx_module*)b;
	}
	ADD_MODULE(module_bitmap_add_noise);
	ADD_MODULE(module_bitmap_plasma);
	ADD_MODULE(module_bitmap_subplasma);
	if (name.find("module_bitmap_blend_") == 0) {
		LOG(name.substr(20));
		int i = s2i(name.substr(20));
    module_bitmap_blend* b = new module_bitmap_blend;
    b->blend_type = i;
    return (vsx_module*)b;
	}
	ADD_MODULE(vsx_module_particle_gen_simple);
	ADD_MODULE(vsx_module_particle_gen_mesh);
	ADD_MODULE(module_render_particlesystem);
	ADD_MODULE(module_render_particlesystem_c);
	ADD_MODULE(module_render_particlesystem_sparks);
	ADD_MODULE(vsx_listener);
  #if !defined (VSXU_OPENGL_ES)
  	ADD_MODULE(vsx_module_texture_blur);
    ADD_MODULE(vsx_module_visual_fader);
	  ADD_MODULE(vsx_polygon_mode);
  #endif
  ADD_MODULE(vsx_module_thorn);
  ADD_MODULE(vsx_cloud_plane);
  ADD_MODULE(vsx_module_planeworld);
  ADD_MODULE(vsx_module_segmesh_map_bspline);
  ADD_MODULE(vsx_module_segmesh_shape_basic);
  ADD_MODULE(vsx_module_segmesh_loft);
  ADD_MODULE(vsx_module_segmesh_bspline_matrix);
	ADD_MODULE(vsx_module_system_time);
	ADD_MODULE(vsx_module_block_chain);
	ADD_MODULE(vsx_depth_test);
	ADD_MODULE(vsx_depth_mask);
	ADD_MODULE(vsx_module_metaballs);
	ADD_MODULE(vsx_module_oscilloscope);
	ADD_MODULE(vsx_build_cubemap_texture);
	ADD_MODULE(vsx_texture_coord_gen);

	ADD_MODULE(vsx_module_gravlines);
	ADD_MODULE(vsx_module_gravity_ribbon);
	ADD_MODULE(vsx_module_gravity_ribbon_particles);
	
	return 0;
}

#endif // VSXU_MODULES_STATIC
