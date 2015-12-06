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

#include <texture/vsx_texture.h>
#include <bitmap/vsx_bitmap.h>

class module_bitmap_load: public vsx_module
{
  // in
  vsx_module_param_resource* filename_in;
  vsx_module_param_int* reload_in;
  vsx_module_param_int* flip_vertical_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  vsx_string<> current_filename;
  vsx_bitmap* bitmap = 0x0;
  int flip_vertical_cache = 0;

  const char* module_name;
  const char* file_suffix;
  const char* file_suffix_uppercase;
  uint64_t bitmap_loader_extra_hint;

public:

  module_bitmap_load(
      const char* module_name_n,
      const char* file_suffix_n,
      const char* file_suffix_uppercase_n,
      uint64_t bitmap_loader_extra_hint_n = 0
  )
    :
      module_name(module_name_n),
      file_suffix(file_suffix_n),
      file_suffix_uppercase(file_suffix_uppercase_n),
      bitmap_loader_extra_hint(bitmap_loader_extra_hint_n)
  {
  }

  void module_info(vsx_module_info* info)
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
      "flip_vertical:enum?no|yes&nc=1"
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
    current_filename = "";

    reload_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "reload");
    flip_vertical_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "flip_vertical");

    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP, "bitmap");
  }


  void run()
  {
    if (bitmap && bitmap->data_ready)
    {
      bitmap_out->set(bitmap);
      loading_done = true;
    }

    bool reload = false;

    if (current_filename != filename_in->get())
      reload = true;

    if (reload_in->get())
      reload = true;

    if (flip_vertical_in->get() != flip_vertical_cache)
      reload = true;

    if (!reload)
      return;

    flip_vertical_cache = flip_vertical_in->get();

    bool do_reload = reload_in->get();
    reload_in->set(0);

    if (!verify_filesuffix(filename_in->get(),file_suffix))
    {
      filename_in->set(current_filename);
      message = vsx_string<>("module||ERROR! This is not a ") + file_suffix_uppercase + " image file!";
      return;
    }

    current_filename = filename_in->get();

    uint64_t bitmap_loader_hint = 0;
    bitmap_loader_hint |= vsx_bitmap::flip_vertical_hint * flip_vertical_cache;
    bitmap_loader_hint |= bitmap_loader_extra_hint;

    if (bitmap && do_reload)
    {
      bitmap = vsx_bitmap_cache::get_instance()->aquire_reload( current_filename, engine->filesystem, true, bitmap_loader_hint );
      return;
    }

    if (bitmap)
    {
      vsx_bitmap_cache::get_instance()->destroy(bitmap);
      bitmap = 0;
    }

    if (!bitmap)
      bitmap = vsx_bitmap_cache::get_instance()->
        aquire_create( current_filename, bitmap_loader_hint );

    vsx_bitmap_loader_helper::load(bitmap, current_filename, engine->filesystem, true, bitmap_loader_hint );
  }

  void on_delete()
  {
    if (bitmap)
      vsx_bitmap_cache::get_instance()->destroy(bitmap);
  }

};


