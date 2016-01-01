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

#include <math/vsx_rand.h>
#include <bitmap/vsx_bitmap.h>

unsigned char catmullrom_interpolate(int v0, int v1, int v2, int v3, float xx)
{
  int a =v0 - v1;
  int P =v3 - v2 - a;
  int Q =a - P;
  int R =v2 - v0;
  int t=(int)(v1+xx*(R+xx*(Q+xx*P)));
  if (t > 255) return 255; else
  if (t < 0) return 0; else
  return (unsigned char)t;
}

class module_bitmap_subplasma : public vsx_module
{
public:
  // in
  vsx_module_param_float* rand_seed_in;
  vsx_module_param_int* size_in;
  vsx_module_param_int* amplitude_in;

  // out
  vsx_module_param_bitmap* bitmap_out;

  // internal
  vsx_bitmap bitmap;
  pthread_t					worker_t;
  int p_updates = -1;
  bool worker_running = false;
  int i_size = 0;
  void *to_delete_data = 0x0;

  static void* worker(void *ptr)
  {
    int x,y;
    module_bitmap_subplasma* module = ((module_bitmap_subplasma*)ptr);
    unsigned char* SubPlasma = new unsigned char[module->i_size * module->i_size];
    for (x = 0; x < module->i_size*module->i_size; ++x) { SubPlasma[x] = 0; }
    int np=2 << module->amplitude_in->get();

    unsigned int musize = module->i_size-1;
    unsigned int mmu = (unsigned int)(((float)module->i_size/(float)np));
    unsigned int mm1 = mmu-1;
    unsigned int mm2 = mmu*2;
    float mmf = (float)mmu;

    vsx_rand rand;

    rand.srand((int)module->rand_seed_in->get());
    for (y=0; y < np; y++)
      for (x=0; x < np; x++)
        SubPlasma[x*mmu+y*mmu*module->i_size] = rand.rand();

    for (y=0; y<np; y++)
      for (x=0; x<module->i_size; x++)
      {
        int p=x&(~mm1);
        int zy=y*mmu*module->i_size;
        SubPlasma[x+zy] = catmullrom_interpolate(
          SubPlasma[((p-mmu)&musize)+zy],
          SubPlasma[((p   )&musize)+zy],
          SubPlasma[((p+mmu)&musize)+zy],
          SubPlasma[((p+mm2)&musize)+zy],
          (x&mm1)/mmf);
      }

    int sl = module->size_in->get()+3;
    for (y=0; y<module->i_size; y++)
      for (x=0; x<module->i_size; x++) {
        int p=y&(~(mm1));
        SubPlasma[x+y*module->i_size] = catmullrom_interpolate(
          SubPlasma[x+(((p-mmu)&musize)<<sl)],
          SubPlasma[x+(((p   )&musize)<<sl)],
          SubPlasma[x+(((p+mmu)&musize)<<sl)],
          SubPlasma[x+(((p+mm2)&musize)<<sl)],
          (y&(mm1))/mmf);
      }

    vsx_bitmap_32bt *p = (vsx_bitmap_32bt*)((module_bitmap_subplasma*)ptr)->bitmap.data_get();

    for (x = 0; x < module->i_size * (module->i_size); ++x, p++)
    {
      *p = 0xFF000000 | ((vsx_bitmap_32bt)SubPlasma[x]) << 16 | (vsx_bitmap_32bt)SubPlasma[x] << 8 | (vsx_bitmap_32bt)SubPlasma[x];// | 0 * 0x00000100 | 0;

    }

    delete[] SubPlasma;
    module->bitmap.timestamp++;
    __sync_fetch_and_add( &(module->bitmap.data_ready), 1 );
    return 0;
  }

  void module_info(vsx_module_info* info)
  {
    info->in_param_spec =
      "rand_seed:float,"
      "size:enum?8x8|16x16|32x32|64x64|128x128|256x256|512x512|1024x1024,"
      "amplitude:enum?2|4|8|16|32|64|128|256|512"
    ;
    info->identifier = "bitmaps;generators;subplasma";
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description =
      "Generates a plasma bitmap\n"
      "Thanks to BoyC of Conspiracy \n"
      "for the base code of this!";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    rand_seed_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"rand_seed");
    rand_seed_in->set(4.0f);

    size_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"size");
    size_in->set(4);

    amplitude_in = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"amplitude");

    bitmap_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
  }

  void run()
  {
    if (to_delete_data)
    {
      free(to_delete_data);
      to_delete_data = 0;
    }

    if (bitmap.data_ready && worker_running)
    {
      pthread_join(worker_t,0);
      worker_running = false;
      bitmap_out->set(&bitmap);
      loading_done = true;
    }

    req(p_updates != param_updates);
    req(!worker_running);
    p_updates = param_updates;

    if (i_size != 8 << size_in->get())
    {
      i_size = 8 << size_in->get();
      if (bitmap.data_get())
        to_delete_data = bitmap.data_get();
      bitmap.data_set( malloc( sizeof(vsx_bitmap_32bt) * i_size * i_size) );

      bitmap.width  = i_size;
      bitmap.height = i_size;
    }

    worker_running = true;
    pthread_create(&worker_t, NULL, &worker, (void*)this);
  }

  void on_delete() {
    if (worker_running)
      pthread_join(worker_t,NULL);
  }
};
