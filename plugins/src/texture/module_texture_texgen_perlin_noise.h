#include <module/vsx_module.h>

#include <texture/vsx_texture.h>
#include <bitmap/generators/vsx_bitmap_generator_perlin_noise.h>

class module_texture_texgen_perlin_noise : public vsx_module
{
  // in - function
  vsx_module_param_float* rand_seed_in;
  vsx_module_param_float* perlin_strength_in;
  vsx_module_param_int* octave_in;
  vsx_module_param_int* frequency_in;

  // in - function_blob
  vsx_module_param_int* blob_enable_in;
  vsx_module_param_float* blob_arms_in;
  vsx_module_param_float* blob_attenuation_in;
  vsx_module_param_float* blob_star_flower_in;
  vsx_module_param_float* blob_angle_in;

  // in - options
  vsx_module_param_float4* color_in;
  vsx_module_param_int* alpha_in;
  vsx_module_param_int* bitmap_type_in;
  vsx_module_param_int* size_in;

  // in - rendering_hints
  vsx_module_param_int* min_mag_filter_in;
  vsx_module_param_int* anisotropic_filtering_in;
  vsx_module_param_int* mipmaps_in;
  vsx_module_param_int* mipmap_min_filter_in;

  // out
  vsx_module_param_texture* texture_out;

  // internal
  vsx_texture<>* texture = 0x0;
  vsx_texture<>* texture_old = 0x0;

  bool first = true;
  bool worker_running = false;
  bool delete_texture_old = false;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "texture;generators;perlin_noise"
    ;

    info->description =
      "Perlin Noise (clouds) generator"
    ;

    info->in_param_spec =
        "function:complex"
        "{"
          "rand_seed:float,"
          "perlin_strength:float,"
          "octave:enum?1|2|3|4|5|6|7|8|9|10|11|12|13|14|15|16&nc=1,"
          "frequency:enum?1|2|3|4|5|6|7|8&nc=1,"
        "},"
        "function_blob:complex"
        "{"
          "enable_blob:enum?no|yes&nc=1,"
          "arms:float,"
          "attenuation:float,"
          "star_flower:float,"
          "angle:float"
        "},"
        "options:complex{"
          "color:float4?default_controller=controller_col,"
          "alpha:enum?no|yes&nc=1,"
          "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048&nc=1,"
          "bitmap_type:enum?integer|float&nc=1"
        "},"
        "rendering_hints:complex{"
          "min_mag_filter:enum?nearest|linear&nc=1,"
          "anisotropic_filter:enum?no|yes&nc=1,"
          "mipmaps:enum?no|yes&nc=1,"
          "mipmap_min_filter:enum?nearest|linear&nc=1"
        "},"
    ;

    info->out_param_spec =
      "texture:texture";

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    // function
    rand_seed_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    rand_seed_in->set(4.0f);

    perlin_strength_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"perlin_strength");
    perlin_strength_in->set(1.0f);

    octave_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"octave");
    octave_in->set(0);

    frequency_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"frequency");
    frequency_in->set(0);

    // function blob
    blob_enable_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"enable_blob");
    blob_arms_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    blob_attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    blob_star_flower_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");
    blob_angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

    // options
    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(1.0f, 0);
    color_in->set(1.0f, 1);
    color_in->set(1.0f, 2);
    color_in->set(1.0f, 3);

    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");

    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);

    bitmap_type_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"bitmap_type");

    // rendering_hints
    min_mag_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "min_mag_filter");
    min_mag_filter_in->set(1); // linear

    anisotropic_filtering_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "anisotropic_filter");

    mipmaps_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmaps");

    mipmap_min_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmap_min_filter");
    mipmap_min_filter_in->set(1);

    // out
    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
  }

  void run()
  {
    if (worker_running && !texture->texture->bitmap->data_ready)
      return;

    if (worker_running && texture->texture->bitmap->data_ready)
    {
      texture_out->set(texture);
      worker_running = false;
      if (texture_old)
      {
        vsx_texture_gl_cache::get_instance()->destroy( texture_old->texture );
        delete texture_old;
        texture_old = 0x0;
      }
      return;
    }

    req(param_updates);
    param_updates = 0;

    uint64_t hint = 0;
    hint |= vsx_texture_gl::anisotropic_filtering_hint * anisotropic_filtering_in->get();
    hint |= vsx_texture_gl::generate_mipmaps_hint * mipmaps_in->get();
    hint |= vsx_texture_gl::linear_interpolate_hint * min_mag_filter_in->get();
    hint |= vsx_texture_gl::mipmap_linear_interpolate_hint * mipmap_min_filter_in->get();

    vsx_string<> cache_handle = vsx_bitmap_generator_perlin_noise::generate_cache_handle(
        blob_enable_in->get(),
        blob_arms_in->get(),
        blob_attenuation_in->get(),
        blob_star_flower_in->get(),
        blob_angle_in->get(),
        rand_seed_in->get(),
        octave_in->get(),
        frequency_in->get(),
        perlin_strength_in->get(),
        alpha_in->get(),
        vsx_color<>(color_in->get(0),color_in->get(1),color_in->get(2),color_in->get(3)),
        bitmap_type_in->get(),
        size_in->get()
      );

    if (vsx_texture_gl_cache::get_instance()->has(cache_handle, 0, hint))
    {
      if (texture)
        texture_old = texture;

      texture = new vsx_texture<>(true);

      texture->texture = vsx_texture_gl_cache::get_instance()->aquire(cache_handle, engine_state->filesystem, false, 0, hint, false );
      texture_out->set(texture);
      return;
    }

    if (texture)
      texture_old = texture;

    texture = new vsx_texture<>(true);

    texture->texture = vsx_texture_gl_cache::get_instance()->create(cache_handle, 0, hint);
    texture->texture->bitmap->filename = cache_handle;

    vsx_bitmap_generator_perlin_noise::generate_thread(
        texture->texture->bitmap,
        blob_enable_in->get(),
        blob_arms_in->get(),
        blob_attenuation_in->get(),
        blob_star_flower_in->get(),
        blob_angle_in->get(),
        rand_seed_in->get(),
        octave_in->get(),
        frequency_in->get(),
        perlin_strength_in->get(),
        alpha_in->get(),
        vsx_color<>(color_in->get(0),color_in->get(1),color_in->get(2),color_in->get(3)),
        bitmap_type_in->get(),
        size_in->get()
    );

    worker_running = true;
  }

  void on_delete()
  {
    vsx_thread_pool::instance()->wait_all(10);

    if (texture_old)
    {
      vsx_texture_gl_cache::get_instance()->destroy( texture_old->texture );
      delete texture_old;
    }
    if (texture)
    {
      vsx_texture_gl_cache::get_instance()->destroy( texture->texture );
      delete texture;
    }
  }
};

