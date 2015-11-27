#include <texture/vsx_texture.h>
#include <texture/vsx_texture_data_loader_png.h>

class module_texture_load_png_cubemap : public vsx_module
{
  // in
  float time;
  vsx_module_param_resource* filename_in;
  vsx_module_param_int* reload_in;

  // out
  vsx_module_param_bitmap* bitmap_out;
  vsx_module_param_texture* texture_out;

  // internal
  vsx_texture* texture;
  vsx_string<>current_filename;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "texture;loaders;png_cubemap_load";

    info->description =
      "Loads a PNG image from\n"
      "disk and outputs a \n"
      " - VSXu bitmap \n "
      "  and\n"
      " - texture.\n"
      "Texture is only loaded when used.\n"
      "This is to preserve memory."
    ;

    info->in_param_spec =
      "filename:resource,reload:enum?no|yes";

    info->out_param_spec =
      "texture:texture";

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = false;

    filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename_in->set("");
    current_filename = "";

    reload_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reload");

    // out
    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");

    texture = 0x0;

    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
    texture_out->valid = false;
  }


  void run()
  {
    if (texture && texture->texture_data->data_ready)
    {
      loading_done = true;
      message = "module||ok";
    }

    if (!(current_filename != filename_in->get() || reload_in->get() == 1))
      return;

    reload_in->set(0);

    if (!verify_filesuffix(filename_in->get(),"png")) {
      filename_in->set(current_filename);
      message = "module||ERROR! This is not a PNG image file!";
      return;
    }

    current_filename = filename_in->get();

    if (texture)
    {
      texture->texture_gl->unload();
      delete texture;
    }

    texture = vsx_texture_data_loader_png::get_instance()->load_cube_thread(current_filename, engine->filesystem, false);
    texture_out->set(texture);
  }

  void output(vsx_module_param_abs* param)
  {
  }

  void stop() {
  }

  void start() {
  }

  void on_delete() {
    if (texture) {
      texture->texture_gl->unload();
      delete texture;
    }
  }

};


