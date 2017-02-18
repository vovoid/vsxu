#include <module/vsx_module.h>
#include <vsx_param.h>
#include <texture/vsx_texture.h>
#include <bitmap/vsx_bitmap_helper.h>

class module_texture_load_bitmap2texture : public vsx_module
{
  // in
  vsx_module_param_bitmap* bitmap_in;

  vsx_module_param_int* flip_vertical_in;
  vsx_module_param_int* mipmaps_in;
  vsx_module_param_int* anisotropic_filtering_in;
  vsx_module_param_int* min_mag_filter_in;
  vsx_module_param_int* mipmap_min_filter_in;

  // out
  vsx_module_param_texture* texture_out;

  // internal
  vsx_bitmap* source_bitmap = 0x0;
  vsx_bitmap bitmap;
  vsx_texture<>* texture = 0x0;
  int bitmap_timestamp = 0;
  int flip_vertical_cache = 1;
  int mipmaps_cache = 0;
  int anisotropic_filtering_cache = 0;
  int min_mag_filter_cache = 1;
  int mipmap_min_filter_cache = 1;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "texture;loaders;bitmap2texture";

    info->description =
      "";

    info->in_param_spec =
      "bitmap:bitmap,"
      "data_hints:complex{"
        "flip_vertical:enum?no|yes&nc=1,"
      "},"
      "gl_hints:complex{"
        "min_mag_filter:enum?nearest|linear&nc=1,"
        "anisotropic_filter:enum?no|yes&nc=1,"
        "mipmaps:enum?no|yes&nc=1,"
        "mipmap_min_filter:enum?nearest|linear&nc=1"
      "}"
    ;

    info->out_param_spec =
      "texture:texture";

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");

    flip_vertical_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "flip_vertical");
    flip_vertical_in->set(flip_vertical_cache);

    mipmaps_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmaps");
    anisotropic_filtering_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "anisotropic_filter");
    min_mag_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "min_mag_filter");
    min_mag_filter_in->set(1); // linear
    mipmap_min_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmap_min_filter");
    mipmap_min_filter_in->set(1);

    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
  }

  void run()
  {
    if (!bitmap_in->get_addr())
    {
      texture_out->valid = false;
      bitmap_timestamp = 0;
      return;
    }
    source_bitmap = bitmap_in->get();
    req(source_bitmap);
    req(source_bitmap->is_valid());

    bool reload = false;
    if (bitmap_timestamp != source_bitmap->timestamp)
      reload = true;

    if (flip_vertical_cache != flip_vertical_in->get())
      reload = true;

    if (mipmaps_in->get() != mipmaps_cache)
      reload = true;

    if (anisotropic_filtering_in->get() != anisotropic_filtering_cache)
      reload = true;

    if (min_mag_filter_in->get() != min_mag_filter_cache)
      reload = true;

    if (mipmap_min_filter_in->get() != mipmap_min_filter_cache)
      reload = true;

    req(reload);

    bitmap_timestamp = source_bitmap->timestamp;
    flip_vertical_cache = flip_vertical_in->get();
    mipmaps_cache = mipmaps_in->get();
    mipmap_min_filter_cache = mipmap_min_filter_in->get();
    anisotropic_filtering_cache = anisotropic_filtering_in->get();
    min_mag_filter_cache = min_mag_filter_in->get();

    if (!texture)
    {
      texture = new vsx_texture<>;
      texture->texture->init_opengl_texture_2d();
    }

    uint64_t hint = 0;
    hint |= vsx_texture_gl::anisotropic_filtering_hint * anisotropic_filtering_cache;
    hint |= vsx_texture_gl::generate_mipmaps_hint * mipmaps_cache;
    hint |= vsx_texture_gl::linear_interpolate_hint * min_mag_filter_cache;
    hint |= vsx_texture_gl::mipmap_linear_interpolate_hint * mipmap_min_filter_cache;
    texture->texture->hint = hint;

    texture->unload_gl();
    texture->texture->init_opengl_texture_2d();

    if (!flip_vertical_cache)
    {
      bitmap.data_free_all();
      texture->texture->bitmap = source_bitmap;
      vsx_texture_gl_loader::upload_2d( texture->texture );
      texture->texture->bitmap = 0x0;
    }

    if (flip_vertical_cache)
    {
      bitmap.data_free_all();
      vsx_bitmap_helper::copy(*source_bitmap, bitmap);
      vsx_bitmap_transform::get_instance()->flip_vertically(&bitmap);
      texture->texture->bitmap = &bitmap;
      vsx_texture_gl_loader::upload_2d( texture->texture );
      texture->texture->bitmap = 0x0;
    }

    texture_out->set(texture);
    loading_done = true;
  }

  void stop()
  {
    req(texture);
    texture->unload_gl();
  }

  void start()
  {
    if (texture)
      texture->texture->init_opengl_texture_2d();

    if (!bitmap_in->get_addr())
      return;

    source_bitmap = *(bitmap_in->get_addr());
    if (source_bitmap) {
      texture->texture->hint |= vsx_texture_gl::generate_mipmaps_hint * mipmaps_cache;
      vsx_texture_gl_loader::upload_bitmap_2d(texture->texture, &bitmap, flip_vertical_cache);
      texture_out->set(texture);
    }
  }

  void on_delete()
  {
    req(texture);
    texture->texture->unload();
    delete texture;
  }
};


