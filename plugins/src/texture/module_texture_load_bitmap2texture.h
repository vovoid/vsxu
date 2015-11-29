#include <texture/vsx_texture.h>

class module_texture_load_bitmap2texture : public vsx_module
{
  // in
  vsx_module_param_bitmap* bitmap_in;

  vsx_module_param_int* mipmaps_in;
  vsx_module_param_int* flip_vertical_in;

  // out
  vsx_module_param_texture* texture_out;

  // internal
  int mipmaps_cache;
  int flip_vertical_cache = 1;

  int bitmap_timestamp = 0;
  vsx_bitmap* bitmap;
  vsx_texture* texture = 0x0;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "texture;loaders;bitmap2texture";

    info->description =
      "";

    info->in_param_spec =
      "bitmap:bitmap,"
      "mipmaps:enum?yes|no,"
      "flip_vertical:enum?no|yes"
    ;

    info->out_param_spec =
      "texture:texture";

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    mipmaps_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"mipmaps");
    mipmaps_in->set(0);

    flip_vertical_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "flip_vertical");
    flip_vertical_in->set(flip_vertical_cache);

    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
  }

  void run()
  {
    if (!bitmap_in->get_addr())
    {
      texture_out->valid = false;
      return;
    }
    bitmap = bitmap_in->get();

    if (!bitmap->valid)
      return;

    bool reload = false;
    if (bitmap_timestamp != bitmap->timestamp)
      reload = true;
    if (mipmaps_cache != mipmaps_in->get())
      reload = true;
    if (flip_vertical_cache != flip_vertical_in->get())
      reload = true;
    req(reload);

    bitmap_timestamp = bitmap->timestamp;
    mipmaps_cache = mipmaps_in->get();
    flip_vertical_cache = flip_vertical_in->get();

    if (!texture)
    {
      texture = new vsx_texture;
      texture->texture_gl->init_opengl_texture_2d();
    }

    texture->texture_gl->hint.mipmaps = mipmaps_cache;

    // todo: convert bitmap to vsx_bitmap
    vsx_texture_gl_loader::upload_bitmap_2d(texture->texture_gl, bitmap, mipmaps_cache, flip_vertical_cache);
    texture_out->set(texture);
    loading_done = true;
  }

  void stop()
  {
    texture->texture_gl->unload();
  }

  void start()
  {
    texture->texture_gl->init_opengl_texture_2d();

    if (!bitmap_in->get_addr())
      return;

    bitmap = *(bitmap_in->get_addr());
    if (bitmap) {
      vsx_texture_gl_loader::upload_bitmap_2d(texture->texture_gl, bitmap, mipmaps_cache, flip_vertical_cache);
      texture_out->set(texture);
    }
  }

  void on_delete()
  {
    texture->texture_gl->unload();
    delete texture;
  }

};


