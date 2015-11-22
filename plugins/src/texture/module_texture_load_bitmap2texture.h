class module_texture_load_bitmap2texture : public vsx_module
{
  // in
  vsx_module_param_bitmap* bitmap_in;
  vsx_module_param_int* mipmaps_in;

  // out
  vsx_module_param_texture* texture_out;

  // internal
  float time;
  vsx_bitmap* bitm;
  int bitm_timestamp;
  vsx_texture* texture;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "texture;loaders;bitmap2texture";

    info->description =
      "";

    info->in_param_spec =
      "bitmap:bitmap,mipmaps:enum?yes|no";

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

    bitm_timestamp = 0;
    texture = new vsx_texture;
    texture->texture_gl->init_opengl_texture_2d();

    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
    loading_done = true;
  }

  void run()
  {
    if (!bitmap_in->get_addr())
    {
      texture_out->valid = false;
      return;
    }

    bitm = *(bitmap_in->get_addr());

    if (bitm->valid && bitm_timestamp != bitm->timestamp)
    {
      bitm_timestamp = bitm->timestamp;
      vsx_texture_gl_loader::upload_bitmap_2d(texture->texture_gl, bitm, mipmaps_in->get() == 0);
      texture_out->set(texture);
    }
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

    bitm = *(bitmap_in->get_addr());
    if (bitm) {
      vsx_texture_gl_loader::upload_bitmap_2d(texture->texture_gl, bitm, mipmaps_in->get() == 0);
      texture_out->set(texture);
    }
  }

  void on_delete()
  {
    texture->texture_gl->unload();
    delete texture;
  }

};


