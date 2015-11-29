/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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


class module_bitmap_add_noise : public vsx_module
{
  // in
  vsx_module_param_bitmap* bitmap_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  float time;
  vsx_bitmap* bitm;
  vsx_bitmap t_bitm;

  int buf, frame;
  vsx_bitmap_32bt *data_a;
  vsx_bitmap_32bt *data_b;
  int bitm_timestamp;
  vsx_bitmap bitmap;
  bool first, worker_running, t_done;
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  int my_ref;

public:



  static void* noise_worker(void *ptr)
  {
    int i_frame = -1;
    bool buf = false;
    vsx_bitmap_32bt *p;
    while (((module_bitmap_add_noise*)ptr)->worker_running) {
      if (i_frame != ((module_bitmap_add_noise*)ptr)->frame) {
        // time to run baby
        if (buf) p = ((module_bitmap_add_noise*)ptr)->data_a;
        else p = ((module_bitmap_add_noise*)ptr)->data_b;

        unsigned long b_c = ((module_bitmap_add_noise*)ptr)->bitmap.width * ((module_bitmap_add_noise*)ptr)->bitmap.height;

        vsx_bitmap& t_bitmap = ((module_bitmap_add_noise*)ptr)->t_bitm;
        if (t_bitmap.storage_format == vsx_bitmap::byte_storage && t_bitmap.channels == 4)
        {
          for (size_t x = 0; x < b_c; ++x)
          {
              p[x] = ((vsx_bitmap_32bt*)((module_bitmap_add_noise*)ptr)->t_bitm.data)[x] | rand() << 8  | (unsigned char)rand(); //bitm->data[x + y*result_bitm.size_x]
          }
        }
        ((module_bitmap_add_noise*)ptr)->bitmap.valid = true;
        ((module_bitmap_add_noise*)ptr)->bitmap.data = p;
        ++((module_bitmap_add_noise*)ptr)->bitmap.timestamp;
        buf = !buf;
        i_frame = ((module_bitmap_add_noise*)ptr)->frame;
      }
      Sleep(100);
    }
    ((module_bitmap_add_noise*)ptr)->t_done = true;
    return 0;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "bitmaps;modifiers;add_noise";
    info->in_param_spec = "bitmap_in:bitmap";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "Adds noise to the bitmap.";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    bitmap.width = 0;
    bitmap.height = 0;
    bitmap.channels = 4;
    bitmap.storage_format = vsx_bitmap::byte_storage;
    my_ref = 0;
    first = true;
    worker_running = false;
    buf = 0;
    frame = 0;
    t_done = false;

    //--------------------------------------------------------------------------------------------------

    bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap_in");

    //--------------------------------------------------------------------------------------------------

    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  }

  void run()
  {
    if (!bitmap_in->get_addr())
      return;

    bitm = *(bitmap_in->get_addr());
    if (!bitm)
    {
      worker_running = false;
      pthread_join(worker_t,0);
      bitmap_out->valid = false;
      return;
    }

    t_bitm = *bitm;

    if (bitmap.width != bitm->width && bitmap.height != bitm->height)
    {
      if (worker_running)
      pthread_join(worker_t,0);
      worker_running = false;

      // need to realloc
      if (bitmap.valid)
        delete[] (vsx_bitmap_32bt*)bitmap.data;

      data_a = new vsx_bitmap_32bt[bitm->width*bitm->height];
      data_b = new vsx_bitmap_32bt[bitm->width*bitm->height];
      bitmap.data = data_a;
      bitmap.valid = true;
      bitmap.width = bitm->width;
      bitmap.height = bitm->height;

      pthread_attr_init(&worker_t_attr);
      pthread_create(&worker_t, &worker_t_attr, &noise_worker, (void*)this);
      sched_param s_param;
      int policy = 0;
      s_param.sched_priority = 20;
      pthread_setschedparam (worker_t,policy,&s_param);
      worker_running = true;
    }
    ++frame;

    bitmap_out->set(&bitmap);
  }

  void on_delete()
  {
    if (worker_running)
    {
      worker_running = false;
      bitmap_out->valid = false;
      pthread_join(worker_t,0);
    }
    delete[] data_a;
    delete[] data_b;

    if (bitmap.valid)
    {
      delete[] (vsx_bitmap_32bt*)bitmap.data;
    }
  }

};


