#include <module/vsx_module.h>

#include <texture/vsx_texture.h>
#include <bitmap/generators/vsx_bitmap_generator_blob.h>

class module_texture_texgen_blob : public vsx_module
{
  // in - function
  vsx_module_param_float* arms_in;
  vsx_module_param_float* attenuation_in;
  vsx_module_param_float* star_flower_in;
  vsx_module_param_float* angle_in;

  // in - options
  vsx_module_param_float4* color_in;
  vsx_module_param_int* alpha_in;
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
      "texture;generators;blob||!texture;particles;blob";

    info->description =
      "Generates blobs,stars or leaf\n"
      "depending on parameters.\n"
      "Play with the params :)"
    ;

    info->in_param_spec =
        "function:complex{"
          "arms:float,"
          "attenuation:float,"
          "star_flower:float,"
          "angle:float"
        "},"
        "options:complex{"
          "color:float4?default_controller=controller_col,"
          "alpha:enum?no|yes,"
          "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024|2048x2048"
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

    // bitmap generation
    arms_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"arms");
    arms_in->set(0.0f);

    attenuation_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"attenuation");
    attenuation_in->set(0.1f);

    star_flower_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"star_flower");

    angle_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

    color_in = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    color_in->set(1.0f,0);
    color_in->set(1.0f,1);
    color_in->set(1.0f,2);
    color_in->set(1.0f,3);

    alpha_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"alpha");
    alpha_in->set(0);

    // gl hints
    min_mag_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "min_mag_filter");
    min_mag_filter_in->set(1);

    anisotropic_filtering_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "anisotropic_filter");

    mipmaps_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmaps");

    mipmap_min_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmap_min_filter");
    mipmap_min_filter_in->set(1);

    // size
    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);

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

    vsx_string<> cache_handle =
      vsx_bitmap_generator_blob::generate_cache_handle(
        arms_in->get(),
        attenuation_in->get(),
        star_flower_in->get(),
        angle_in->get(),
        vsx_color<>(color_in->get(0), color_in->get(1), color_in->get(2), color_in->get(3)),
        alpha_in->get(),
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

    vsx_bitmap_generator_blob::generate_thread(
      texture->texture->bitmap,
      arms_in->get(),
      attenuation_in->get(),
      star_flower_in->get(),
      angle_in->get(),
      vsx_color<>(color_in->get(0), color_in->get(1), color_in->get(2), color_in->get(3)),
      alpha_in->get(),
      size_in->get()
    );
    worker_running = true;
  }

  void on_delete()
  {
    vsx_thread_pool::instance()->wait_all(10);

    if (texture)
    {
      vsx_texture_gl_cache::get_instance()->destroy( texture->texture );
      delete texture;
    }

    if (texture_old)
    {
      vsx_texture_gl_cache::get_instance()->destroy( texture_old->texture );
      delete texture_old;
    }
  }
};


