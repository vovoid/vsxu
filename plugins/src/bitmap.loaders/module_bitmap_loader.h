/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Vovoid Media Technologies AB Copyright (C) 2003-2015
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <vsx_module_cache_helper.h>
#include <texture/vsx_texture.h>
#include <bitmap/vsx_bitmap.h>

class module_bitmap_load: public vsx_module
{
  // in
  vsx_module_param_resource* filename_in;

  vsx_module_param_int* reload_in;

  vsx_module_param_int* flip_vertical_in;
  vsx_module_param_int* cubemap_split_6_1_in;
  vsx_module_param_int* cubemap_sphere_map_in;
  vsx_module_param_int* cubemap_load_files_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  vsx_string<> filename_cache;
  vsx_bitmap* bitmap = 0x0;

  int flip_vertical_cache = 0;
  int cubemap_split_6_1_cache = 0;
  int cubemap_sphere_map_cache = 0;
  int cubemap_load_files_cache = 0;

  const char* module_name;
  const char* file_suffix;
  const char* file_suffix_uppercase;

public:

  module_bitmap_load(
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
      vsx_string<>("bitmaps;loaders;") + module_name;

    info->description =
      vsx_string<>("Loads a ") + file_suffix_uppercase + " image from\n"
      "disk and outputs a:\n"
      " - bitmap \n "
    ;

    info->in_param_spec =
      "filename:resource,"
      "reload:enum?no|yes&nc=1,"
      "bitmap_loading_hints:complex{"
        "flip_vertical:enum?no|yes&nc=1"
        "cubemap_split_6_1:enum?no|yes&nc=1,"
        "cubemap_sphere_map:enum?no|yes&nc=1,"
        "cubemap_load_files:enum?no|yes&nc=1"
      "}"
    ;

    info->out_param_spec =
      "bitmap:bitmap";

    info->component_class =
      "bitmap";
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

    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP, "bitmap");
  }

  bool has_state_changed()
  {
    if (reload_in->get())
      return true;

    cache_check(filename)

    // bitmap
    cache_check(flip_vertical)
    cache_check(cubemap_split_6_1)
    cache_check(cubemap_sphere_map)
    cache_check(cubemap_load_files)

    return false;
  }


  void run()
  {
    if (bitmap && bitmap->data_ready)
    {
      bitmap_out->set(bitmap);
      loading_done = true;
    }

    req( has_state_changed() );

    bool reload = reload_in->get() > 0;
    reload_in->set(0);

    if (!vsx_string_helper::verify_filesuffix(filename_in->get(), file_suffix))
    {
      user_message = vsx_string<>("module||ERROR! This is not a ") + file_suffix_uppercase + " image file!";
      return;
    }

    uint64_t bitmap_loader_hint = 0;
    bitmap_loader_hint |= vsx_bitmap::flip_vertical_hint * flip_vertical_cache;
    bitmap_loader_hint |= vsx_bitmap::cubemap_load_files_hint * cubemap_load_files_cache;
    bitmap_loader_hint |= vsx_bitmap::cubemap_sphere_map * cubemap_sphere_map_cache;
    bitmap_loader_hint |= vsx_bitmap::cubemap_split_6_1_hint* cubemap_split_6_1_cache;

    if (bitmap && reload)
    {
      bitmap = vsx_bitmap_cache::get_instance()->aquire_reload( filename_cache, engine_state->filesystem, true, bitmap_loader_hint );
      return;
    }

    if (bitmap)
    {
      vsx_bitmap_cache::get_instance()->destroy(bitmap);
      bitmap = 0;
    }

    if (!bitmap)
      bitmap = vsx_bitmap_cache::get_instance()->
        aquire_create( filename_cache, bitmap_loader_hint );

    vsx_bitmap_loader::load(bitmap, filename_cache, engine_state->filesystem, true, bitmap_loader_hint );
  }

  void on_delete()
  {
    vsx_thread_pool::instance()->wait_all(10);

    if (bitmap)
      vsx_bitmap_cache::get_instance()->destroy(bitmap);
  }

};


