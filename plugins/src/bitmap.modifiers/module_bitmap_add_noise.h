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
public:

  // in
  vsx_module_param_bitmap* bitmap_in;

  // out
  vsx_module_param_bitmap* result1;

  // internal
  float time;
  vsx_bitmap* bitm;
  vsx_bitmap t_bitm;

  int buf, frame;
  vsx_bitmap_32bt *data_a;
  vsx_bitmap_32bt *data_b;
  int bitm_timestamp;
  vsx_bitmap result_bitm;
  bool first, worker_running, t_done;
  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  int my_ref;


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

        unsigned long b_c = ((module_bitmap_add_noise*)ptr)->result_bitm.size_x * ((module_bitmap_add_noise*)ptr)->result_bitm.size_y;

        if (((module_bitmap_add_noise*)ptr)->t_bitm.bformat == GL_RGBA)
        {
          for (size_t x = 0; x < b_c; ++x)
          {
              p[x] = ((vsx_bitmap_32bt*)((module_bitmap_add_noise*)ptr)->t_bitm.data)[x] | rand() << 8  | (unsigned char)rand(); //bitm->data[x + y*result_bitm.size_x]
          }
        }
        ((module_bitmap_add_noise*)ptr)->result_bitm.valid = true;
        ((module_bitmap_add_noise*)ptr)->result_bitm.data = p;
        ++((module_bitmap_add_noise*)ptr)->result_bitm.timestamp;
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
    info->description = "";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    result_bitm.size_x = 0;
    result_bitm.size_y = 0;
    result_bitm.bpp = 4;
    result_bitm.bformat = GL_RGBA;
    my_ref = 0;
    first = true;
    worker_running = false;
    buf = 0;
    frame = 0;
    t_done = false;

    //--------------------------------------------------------------------------------------------------

    bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap_in");

    //--------------------------------------------------------------------------------------------------

    result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");

  }

  void run()
  {
    bitm = bitmap_in->get_addr();
    if (!bitm)
    {
      worker_running = false;
      pthread_join(worker_t,0);
      result1->valid = false;
      return;
    }

    t_bitm = *bitm;

    if (result_bitm.size_x != bitm->size_x && result_bitm.size_y != bitm->size_y)
    {
      if (worker_running)
      pthread_join(worker_t,0);
      worker_running = false;

      // need to realloc
      if (result_bitm.valid) delete[] (vsx_bitmap_32bt*)result_bitm.data;
      data_a = new vsx_bitmap_32bt[bitm->size_x*bitm->size_y];
      data_b = new vsx_bitmap_32bt[bitm->size_x*bitm->size_y];
      result_bitm.data = data_a;
      result_bitm.valid = true;
      result_bitm.size_x = bitm->size_x;
      result_bitm.size_y = bitm->size_y;

      pthread_attr_init(&worker_t_attr);
      pthread_create(&worker_t, &worker_t_attr, &noise_worker, (void*)this);
      sched_param s_param;
      int policy = 0;
      s_param.sched_priority = 20;
      pthread_setschedparam (worker_t,policy,&s_param);
      worker_running = true;
    }
    ++frame;

    result1->set_p(result_bitm);
  }

  void on_delete()
  {
    if (worker_running)
    {
      worker_running = false;
      result1->valid = false;
      pthread_join(worker_t,0);
    }
    delete[] data_a;
    delete[] data_b;

    if (result_bitm.valid)
    {
      delete[] (vsx_bitmap_32bt*)result_bitm.data;
    }
  }

};


