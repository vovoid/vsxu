#include <vsx_module.h>
#include <vsx_param.h>
#include <vsx_module_cache_helper.h>
#include <texture/vsx_texture.h>
#include <bitmap/vsx_bitmap_helper.h>
#include <bitmap/generators/vsx_bitmap_generator_blob.h>

class module_texture_texgen_blob : public vsx_module
{
  // in
  vsx_module_param_float* arms_in;
  vsx_module_param_float* attenuation_in;
  vsx_module_param_float* star_flower_in;
  vsx_module_param_float* angle_in;
  vsx_module_param_float4* color_in;
  vsx_module_param_int* alpha_in;
  vsx_module_param_int* size_in;

  vsx_module_param_int* mipmaps_in;
  vsx_module_param_int* anisotropic_filtering_in;
  vsx_module_param_int* min_mag_filter_in;
  vsx_module_param_int* mipmap_min_filter_in;

  // out
  vsx_module_param_texture* texture_out;

  // internal
  vsx_texture<>* texture = 0x0;
  vsx_texture<>* texture_old = 0x0;

  bool first = true;
  bool worker_running = false;
  bool delete_texture_old = false;

  float arms_cache = 0.0f;
  float attenuation_cache = 0.1f;
  float star_flower_cache = 0.0f;
  float angle_cache = 0.0f;
  int size_cache = 4;
  float color_r_cache = 1.0f;
  float color_g_cache = 1.0f;
  float color_b_cache = 1.0f;
  float color_a_cache = 1.0f;
  int alpha_cache = 0;
  int mipmaps_cache = 0;
  int anisotropic_filtering_cache = 0;
  int min_mag_filter_cache = 1;
  int mipmap_min_filter_cache = 1;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "texture;generators;blob||!texture;particles;blob";

    info->description =
      "Generates blobs,stars or leaf\n"
      "depending on parameters.\n"
      "Play with the params :)"
    ;

    info->in_param_spec =
        "settings:complex{"
          "arms:float,"
          "attenuation:float,"
          "star_flower:float,"
          "angle:float,"
          "color:float4?default_controller=controller_col,"
          "alpha:enum?no|yes"
        "},"
        "gl_hints:complex{"
          "min_mag_filter:enum?nearest|linear&nc=1,"
          "anisotropic_filter:enum?no|yes&nc=1,"
          "mipmaps:enum?no|yes&nc=1,"
          "mipmap_min_filter:enum?nearest|linear&nc=1"
        "},"
        "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048"
    ;

    info->out_param_spec =
      "texture:texture";

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    arms_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    arms_in->set(0.0f);

    attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    attenuation_in->set(attenuation_cache);

    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(size_cache);

    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");
    alpha_in->set(alpha_cache);

    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(color_r_cache,0);
    color_in->set(color_g_cache,1);
    color_in->set(color_b_cache,2);
    color_in->set(color_a_cache,3);

    star_flower_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");

    angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

    mipmaps_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmaps");
    anisotropic_filtering_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "anisotropic_filter");
    min_mag_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "min_mag_filter");
    min_mag_filter_in->set(1); // linear
    mipmap_min_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmap_min_filter");
    mipmap_min_filter_in->set(1);

    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");

    loading_done = true;
  }

  bool has_state_changed()
  {
    if (first)
    {
      first = false;
      return true;
    }
    cache_check_f(arms)
    cache_check_f(attenuation)
    cache_check(size)
    cache_check(alpha)
    cache_check_f(star_flower)
    cache_check_f(angle)
    cache_check(mipmaps)
    cache_check(anisotropic_filtering)
    cache_check(min_mag_filter)
    cache_check(mipmap_min_filter)

    if (
      (fabs(color_in->get(0) - color_r_cache) > 0.001f)
      ||
      (fabs(color_in->get(1) - color_g_cache) > 0.001f)
      ||
      (fabs(color_in->get(2) - color_b_cache) > 0.001f)
      ||
      (fabs(color_in->get(3) - color_a_cache) > 0.001f)
    )
    {
      color_r_cache = color_in->get(0);
      color_g_cache = color_in->get(1);
      color_b_cache = color_in->get(2);
      color_a_cache = color_in->get(3);
    }

    return false;
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

    req(has_state_changed());

    mipmaps_cache = mipmaps_in->get();
    mipmap_min_filter_cache = mipmap_min_filter_in->get();
    anisotropic_filtering_cache = anisotropic_filtering_in->get();
    min_mag_filter_cache = min_mag_filter_in->get();

    arms_cache = arms_in->get();
    attenuation_cache = attenuation_in->get();
    size_cache = size_in->get();
    alpha_cache = alpha_in->get();
    star_flower_cache = star_flower_in->get();
    angle_cache = angle_in->get();

    uint64_t hint = 0;
    hint |= vsx_texture_gl::anisotropic_filtering_hint * anisotropic_filtering_cache;
    hint |= vsx_texture_gl::generate_mipmaps_hint * mipmaps_cache;
    hint |= vsx_texture_gl::linear_interpolate_hint * min_mag_filter_cache;
    hint |= vsx_texture_gl::mipmap_linear_interpolate_hint * mipmap_min_filter_cache;

    vsx_string<> cache_handle =
      vsx_bitmap_generator_blob::generate_cache_handle(
        arms_cache,
        attenuation_cache,
        star_flower_cache,
        angle_cache,
        vsx_color<>(color_in->get(0), color_in->get(1), color_in->get(2), color_in->get(3)),
        alpha_cache,
        size_cache
      );

    //vsx_texture_gl_cache::get_instance()->dump_to_stdout();
    if (vsx_texture_gl_cache::get_instance()->has(cache_handle, 0, hint))
    {
      if (!texture)
        texture = new vsx_texture<>;

      texture->texture = vsx_texture_gl_cache::get_instance()->aquire(cache_handle, engine->filesystem, false, 0, hint, false );
      texture_out->set(texture);
      return;
    }


    if (texture)
      texture_old = texture;

    texture = new vsx_texture<>;



    texture->texture = vsx_texture_gl_cache::get_instance()->create(cache_handle, 0, hint);
    texture->texture->bitmap->filename = cache_handle;

    vsx_bitmap_generator_blob::load(
      texture->texture->bitmap,
      arms_cache,
      attenuation_cache,
      star_flower_cache,
      angle_cache,
      vsx_color<>(color_in->get(0), color_in->get(1), color_in->get(2), color_in->get(3)),
      alpha_cache,
      size_cache
    );
    worker_running = true;
  }

  void stop()
  {
  }

  void start()
  {
  }

  void on_delete()
  {
    vsx_thread_pool::instance()->wait_all();

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


