#include <module/vsx_module.h>
#include <vsx_param.h>
#include <texture/vsx_texture.h>

class module_texture_load: public vsx_module
{
  // in
  vsx_module_param_resource* filename_in;

  vsx_module_param_int* reload_in;

  vsx_module_param_int* flip_vertical_in;
  vsx_module_param_int* cubemap_split_6_1_in;
  vsx_module_param_int* cubemap_sphere_map_in;
  vsx_module_param_int* cubemap_load_files_in;

  vsx_module_param_int* mipmaps_in;
  vsx_module_param_int* anisotropic_filtering_in;
  vsx_module_param_int* min_mag_filter_in;
  vsx_module_param_int* mipmap_min_filter_in;

  // out
  vsx_module_param_texture* texture_out;

  // internal
  vsx_string<> filename_cache;
  std::unique_ptr<vsx_texture<>> texture;
  std::unique_ptr<vsx_texture<>> texture_to_destroy;

  // data hints
  int flip_vertical_cache = 0;
  int cubemap_split_6_1_cache = 0;
  int cubemap_sphere_map_cache = 0;
  int cubemap_load_files_cache = 0;

  // gl hints
  int mipmaps_cache = 0;
  int anisotropic_filtering_cache = 0;
  int min_mag_filter_cache = 1;
  int mipmap_min_filter_cache = 1;

  const char* module_name;
  const char* file_suffix;
  const char* file_suffix_uppercase;

public:

  module_texture_load(
      const char* module_name_n,
      const char* file_suffix_n,
      const char* file_suffix_uppercase_n
  )
    :
      module_name(module_name_n),
      file_suffix(file_suffix_n),
      file_suffix_uppercase(file_suffix_uppercase_n)
  {
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      vsx_string<>("texture;loaders;")+ module_name;

    info->description =
      vsx_string<>("Loads a ") + file_suffix_uppercase + " image from\n"
      "disk and outputs a\n"
      "texture.\n"
    ;

    info->in_param_spec =
      "filename:resource,"
      "reload:enum?no|yes&nc=1,"
      "bitmap_loading_hints:complex{"
        "flip_vertical:enum?no|yes&nc=1,"
        "cubemap_split_6_1:enum?no|yes&nc=1,"
        "cubemap_sphere_map:enum?no|yes&nc=1,"
        "cubemap_load_files:enum?no|yes&nc=1"
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
    filename_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename_in->set("");
    filename_cache = "";

    reload_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reload");

    flip_vertical_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "flip_vertical");
    cubemap_split_6_1_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "cubemap_split_6_1");
    cubemap_sphere_map_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "cubemap_sphere_map");
    cubemap_load_files_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "cubemap_load_files");

    mipmaps_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmaps");
    anisotropic_filtering_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "anisotropic_filter");
    min_mag_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "min_mag_filter");
    min_mag_filter_in->set(1); // linear
    mipmap_min_filter_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "mipmap_min_filter");
    mipmap_min_filter_in->set(1);

    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
    texture_out->valid = false;
  }

  bool has_parameters_changed()
  {
    if (reload_in->get())
      return true;

    #define cache_check(n) \
      if ( n##_in->get() != n##_cache) { \
        n##_cache = n##_in->get(); \
        return true; \
      }

    cache_check(filename)

    // bitmap
    cache_check(flip_vertical)
    cache_check(cubemap_split_6_1)
    cache_check(cubemap_sphere_map)
    cache_check(cubemap_load_files)

    // gl
    cache_check(mipmaps)
    cache_check(anisotropic_filtering)
    cache_check(min_mag_filter)
    cache_check(mipmap_min_filter)

    return false;
  }

  void run()
  {
    if (texture)
    if (texture->texture)
    if (texture->texture->bitmap)
    if (texture->texture->bitmap->data_ready)
    {
      if (texture_to_destroy)
        texture_to_destroy.reset(nullptr);
      loading_done = true;
      user_message = "module||ok";
    }

    req(has_parameters_changed());

    bool reload = reload_in->get();
    reload_in->set(0);

    if (!vsx_string_helper::verify_filesuffix(filename_in->get(), file_suffix))
    {
      user_message = vsx_string<>("module||ERROR! This is not a ") + file_suffix_uppercase + " image file!";
      return;
    }

    filename_cache = filename_in->get();

    if (texture && !reload)
      texture_to_destroy = std::move(texture);

    uint64_t bitmap_loader_hint = 0;
    bitmap_loader_hint |= vsx_bitmap::flip_vertical_hint * flip_vertical_cache;
    bitmap_loader_hint |= vsx_bitmap::cubemap_load_files_hint * cubemap_load_files_cache;
    bitmap_loader_hint |= vsx_bitmap::cubemap_sphere_map * cubemap_sphere_map_cache;
    bitmap_loader_hint |= vsx_bitmap::cubemap_split_6_1_hint* cubemap_split_6_1_cache;

    uint64_t hint = 0;
    hint |= vsx_texture_gl::anisotropic_filtering_hint * anisotropic_filtering_cache;
    hint |= vsx_texture_gl::generate_mipmaps_hint * mipmaps_cache;
    hint |= vsx_texture_gl::linear_interpolate_hint * min_mag_filter_cache;
    hint |= vsx_texture_gl::mipmap_linear_interpolate_hint * mipmap_min_filter_cache;

    texture = std::move(vsx_texture_loader::load(
      filename_cache,
      engine_state->filesystem,
      true, // threaded
      bitmap_loader_hint,
      hint,
      reload
    ));
    texture_out->set(texture.get());
  }

  void on_delete()
  {
    vsx_thread_pool::instance()->wait_all(10);

    if (texture)
      texture.reset(nullptr);
  }

};


