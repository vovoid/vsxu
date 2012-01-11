/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan "jaw" Wallmander, Vovoid Media Technologies - Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


typedef unsigned char uint8;

#define BLEND_NORMAL        0
#define BLEND_LIGHTEN       1
#define BLEND_DARKEN        2
#define BLEND_MULTIPLY      3
#define BLEND_AVERAGE       4
#define BLEND_ADD           5
#define BLEND_SUBTRACT      6
#define BLEND_DIFFERENCE    7
#define BLEND_NEGATION      8
#define BLEND_SCREEN        9
#define BLEND_EXCLUSION    10
#define BLEND_OVERLAY      11
#define BLEND_SOFT_LIGHT   12
#define BLEND_HARD_LIGHT   13
#define BLEND_COLOR_DODGE  14
#define BLEND_COLOR_BURN   15
#define BLEND_LINEAR_DODGE 16
#define BLEND_LINEAR_BURN  17
#define BLEND_LINEAR_LIGHT 18
#define BLEND_VIVID_LIGHT  19
#define BLEND_PIN_LIGHT    20
#define BLEND_HARD_MIX     21
#define BLEND_REFLECT      22
#define BLEND_GLOW         23
#define BLEND_PHOENIX      24

#define min(A,B) A<B?A:B
#define max(A,B) A>B?A:B

#define Blend_Normal(A,B) ((unsigned char)(A))
#define Blend_Lighten(A,B)  ((unsigned char)((B > A) ? B:A))
#define Blend_Darken(A,B) ((unsigned char)((B > A) ? A:B))
#define Blend_Multiply(A,B) ((unsigned char)((A * B) / 255))
#define Blend_Average(A,B)  ((unsigned char)((A + B) / 2))
#define Blend_Add(A,B)  ((unsigned char)((A + B > 255) ? 255:(A + B)))
#define Blend_Subtract(A,B) ((unsigned char)((A + B < 255) ? 0:(A + B - 255)))
#define Blend_Difference(A,B) ((unsigned char)(abs(A - B)))
#define Blend_Negation(A,B) ((unsigned char)(255 - abs(255 - A - B)))
#define Blend_Screen(A,B) ((unsigned char)(255 - (((255 - A) * (255 - B)) >> 8)))
#define Blend_Exclusion(A,B)  ((unsigned char)(A + B - 2 * A * B / 255))
#define Blend_Overlay(A,B)  ((unsigned char)((B < 128) ? (2 * A * B / 255):(255 - 2 * (255 - A) * (255 - B) / 255)))
#define Blend_Soft_Light(A,B)  ((uint8)((B < 128) ? (2*((A >> 1)+64)) * (B/255):(255 - (2*(255-((A >> 1) + 64))*(255-B)/255))))
#define Blend_Hard_Light(A,B)  (Blend_Overlay(B,A))
#define Blend_Color_Dodge(A,B) ((uint8)((A == 255) ? A:((B << 8 ) / (255 - A) > 255) ? 255:((B << 8 ) / (255 - A))))
#define Blend_Color_Burn(A,B)  ((uint8)((A == 0) ? 0:((255 - (((255 - B) << 8 ) / A)) < 0) ? 0:(255 - (((255 - B) << 8 ) / A))))
#define Blend_Linear_Dodge(A,B)  (Blend_Add(A,B))
#define Blend_Linear_Burn(A,B) (Blend_Subtract(A,B))
#define Blend_Linear_Light(A,B)  ((uint8)(A < 128) ? Blend_Linear_Burn((2 * A),B):Blend_Linear_Dodge((2 * (A - 128)),B))
#define Blend_Vivid_Light(A,B) ((uint8)(A < 128) ? Blend_Color_Burn((2 * A),B):Blend_Color_Dodge((2 * (A - 128)),B))
#define Blend_Pin_Light(A,B) ((uint8)(A < 128) ? Blend_Darken((2 * A),B):Blend_Lighten((2 *(A - 128)),B))
#define Blend_Hard_Mix(A,B)  ((uint8)(A < 255 - B) ? 0:255)
#define Blend_Reflect(A,B)  ((uint8)((B == 255) ? B:((A * A / (255 - B) > 255) ? 255:(A * A / (255 - B)))))
#define Blend_Glow(A,B) (Blend_Reflect(B,A))
#define Blend_Phoenix(A,B)  ((uint8)(min(A,B) - max(A,B) + 255))
#define Blend_Opacity(A,B,F,O)  ((uint8)(O * F(A,B) + (1 - O) * B))


/*#define fl_Blend_Normal(A,B) ((unsigned char)(A))
#define fl_Blend_Lighten(A,B)  ((unsigned char)((B > A) ? B:A))
#define fl_Blend_Darken(A,B) ((unsigned char)((B > A) ? A:B))*/
#define fl_Blend_Multiply(A,B) ((unsigned char)((A * B) / 255))
/*#define fl_Blend_Average(A,B)  ((unsigned char)((A + B) / 2))
#define fl_Blend_Add(A,B)  ((unsigned char)((A + B > 255) ? 255:(A + B)))
#define fl_Blend_Subtract(A,B) ((unsigned char)((A + B < 255) ? 0:(A + B - 255)))
#define fl_Blend_Difference(A,B) ((unsigned char)(abs(A - B)))
#define fl_Blend_Negation(A,B) ((unsigned char)(255 - abs(255 - A - B)))
#define fl_Blend_Screen(A,B) ((unsigned char)(255 - (((255 - A) * (255 - B)) >> 8)))
#define fl_Blend_Exclusion(A,B)  ((unsigned char)(A + B - 2 * A * B / 255))
#define fl_Blend_Overlay(A,B)  ((unsigned char)((B < 128) ? (2 * A * B / 255):(255 - 2 * (255 - A) * (255 - B) / 255)))
#define fl_Blend_Soft_Light(A,B)  ((uint8)((B < 128) ? (2*((A >> 1)+64)) * (B/255):(255 - (2*(255-((A >> 1) + 64))*(255-B)/255))))
#define fl_Blend_Hard_Light(A,B)  (Blend_Overlay(B,A))
#define fl_Blend_Color_Dodge(A,B) ((uint8)((A == 255) ? A:((B << 8 ) / (255 - A) > 255) ? 255:((B << 8 ) / (255 - A))))
#define fl_Blend_Color_Burn(A,B)  ((uint8)((A == 0) ? 0:((255 - (((255 - B) << 8 ) / A)) < 0) ? 0:(255 - (((255 - B) << 8 ) / A))))
#define fl_Blend_Linear_Dodge(A,B)  (Blend_Add(A,B))
#define fl_Blend_Linear_Burn(A,B) (Blend_Subtract(A,B))
#define fl_Blend_Linear_Light(A,B)  ((uint8)(A < 128) ? Blend_Linear_Burn((2 * A),B):Blend_Linear_Dodge((2 * (A - 128)),B))
#define fl_Blend_Vivid_Light(A,B) ((uint8)(A < 128) ? Blend_Color_Burn((2 * A),B):Blend_Color_Dodge((2 * (A - 128)),B))
#define fl_Blend_Pin_Light(A,B) ((uint8)(A < 128) ? Blend_Darken((2 * A),B):Blend_Lighten((2 *(A - 128)),B))
#define fl_Blend_Hard_Mix(A,B)  ((uint8)(A < 255 - B) ? 0:255)
#define fl_Blend_Reflect(A,B)  ((uint8)((B == 255) ? B:((A * A / (255 - B) > 255) ? 255:(A * A / (255 - B)))))
#define fl_Blend_Glow(A,B) (Blend_Reflect(B,A))
#define fl_Blend_Phoenix(A,B)  ((uint8)(min(A,B) - max(A,B) + 255))*/
#define fl_Blend_Opacity(A,B,F,O)  ((O * F(A,B) + (1 - O) * B))

// 0..24 blend types

class module_bitmap_blend : public vsx_module {
  
  // out
  vsx_module_param_bitmap* result1;
  // internal
  bool need_to_rebuild;

  int bitm_timestamp;

  pthread_t	worker_t;


  int p_updates;

public:
  vsx_bitmap bitm;
  vsx_bitmap *bitm1;
  vsx_bitmap *bitm2;
  // in
  vsx_module_param_bitmap* in1;
  vsx_module_param_bitmap* in2;

  vsx_module_param_int* filter_type;
  vsx_module_param_int* bitmap_type;

  vsx_module_param_float3* bitm1_ofs;
  vsx_module_param_float3* bitm2_ofs;
  vsx_module_param_float3* target_size;
  vsx_module_param_float* bitm2_opacity;
  
  int blend_type;
  
  vsx_bitmap*       work_bitmap;
  bool              worker_running;
  int               thread_state;
  int my_ref;

  // our worker thread, to keep the tough generating work off the main loop
  // this is a fairly simple operation, but when you want to generate fractals
  // and decode film, you could run into several seconds of processing time. 
  static void* worker(void *ptr)
  {
    module_bitmap_blend* mod = ((module_bitmap_blend*)ptr);
    vsx_bitmap* bitm = mod->work_bitmap;
    vsx_bitmap* bitm1 = mod->bitm1;
    vsx_bitmap* bitm2 = mod->bitm2;
    
    unsigned long x,y,ix,iy;

    for (x = 0; x < bitm->size_x * bitm->size_y; x++) ((vsx_bitmap_32bt*)bitm->data)[x] = 0;
    //TEEEEMP!
    //bitm->timestamp++;
    //bitm->valid = true;
    //mod->thread_state = 2;
    //return 0;
    // END TEEEEEMP
    
    bool ixbound = false, iybound = false;
    iy = 0;
    for (y = (unsigned long)mod->bitm1_ofs->get(1); y < bitm->size_y && !iybound; y++)
    {
      ix = 0;
      ixbound = false;
      for (x = (unsigned long)mod->bitm1_ofs->get(0); x < bitm->size_x && !ixbound; x++)
      {
        ((vsx_bitmap_32bt*)bitm->data)[x + y * bitm->size_x] = ((vsx_bitmap_32bt*)bitm1->data)[ix + iy * bitm1->size_x];
        ix++;
        if (ix >= bitm1->size_x) ixbound = true;
      }
      iy++;
      if (iy >= bitm1->size_y) iybound = true;
    }

    iybound = false;
    iy = 0;
    for (y = (unsigned long)mod->bitm2_ofs->get(1); y < bitm->size_y && !iybound; y++)
    {
      ix = 0;
      ixbound = false;
      for (x = (unsigned long)mod->bitm2_ofs->get(0); x < bitm->size_x && !ixbound; x++)
      {
        vsx_bitmap_32bt* data = (vsx_bitmap_32bt*)bitm->data;
        vsx_bitmap_32bt* data2 = (vsx_bitmap_32bt*)bitm2->data;

#define BLEND_FUNC(BLT) \
      for (int a = 0; a < 4; a++)\
      {\
        ((unsigned char*)&data[x + y * bitm->size_x])[a] = Blend_Opacity(((unsigned char*)&data2[ix + iy * bitm2->size_x])[a],((unsigned char*)&data[x + y * bitm->size_x])[a],BLT,mod->bitm2_opacity->get());\
      }\
      
  switch (mod->filter_type->get())
  {
    case BLEND_NORMAL       : BLEND_FUNC(Blend_Normal) break;
    case BLEND_LIGHTEN      : BLEND_FUNC(Blend_Lighten) break;
    case BLEND_DARKEN       : BLEND_FUNC(Blend_Darken) break;
    case BLEND_MULTIPLY     : BLEND_FUNC(Blend_Multiply) break;
    case BLEND_AVERAGE      : BLEND_FUNC(Blend_Average) break;
    case BLEND_ADD          : BLEND_FUNC(Blend_Add) break;
    case BLEND_SUBTRACT     : BLEND_FUNC(Blend_Subtract) break;
    case BLEND_DIFFERENCE   : BLEND_FUNC(Blend_Difference) break;
    case BLEND_NEGATION     : BLEND_FUNC(Blend_Negation) break;
    case BLEND_SCREEN       : BLEND_FUNC(Blend_Screen) break;
    case BLEND_EXCLUSION    : BLEND_FUNC(Blend_Exclusion) break;
    case BLEND_OVERLAY      : BLEND_FUNC(Blend_Overlay) break;
    case BLEND_SOFT_LIGHT   : BLEND_FUNC(Blend_Soft_Light) break;
    case BLEND_HARD_LIGHT   : BLEND_FUNC(Blend_Hard_Light) break;
    case BLEND_COLOR_DODGE  : BLEND_FUNC(Blend_Color_Dodge) break;
    case BLEND_COLOR_BURN   : BLEND_FUNC(Blend_Color_Burn) break;
    case BLEND_LINEAR_DODGE : BLEND_FUNC(Blend_Linear_Dodge) break;
    case BLEND_LINEAR_BURN  : BLEND_FUNC(Blend_Linear_Burn) break;
    case BLEND_LINEAR_LIGHT : BLEND_FUNC(Blend_Linear_Light) break;
    case BLEND_VIVID_LIGHT  : BLEND_FUNC(Blend_Vivid_Light) break;
    case BLEND_PIN_LIGHT    : BLEND_FUNC(Blend_Pin_Light) break;
    case BLEND_HARD_MIX     : BLEND_FUNC(Blend_Hard_Mix) break;
    case BLEND_REFLECT      : BLEND_FUNC(Blend_Reflect) break;
    case BLEND_GLOW         : BLEND_FUNC(Blend_Glow) break;
    case BLEND_PHOENIX      : BLEND_FUNC(Blend_Phoenix) break;
  }
      
      ix++;
      if (ix >= bitm2->size_x) ixbound = true;
    }
    iy++;
    if (iy >= bitm2->size_y) iybound = true;
  }

    bitm->timestamp++;
    bitm->valid = true;
    mod->thread_state = 2;
    //printf("blob thread done\n");
    // the thread will die here.
    return 0;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->in_param_spec = "bmp1:complex{in1:bitmap,bitm1_ofs:float3},bmp2:complex{in2:bitmap,bitm2_ofs:float3,bitm2_opacity:float},target_size:float3,\
blend_type:enum?BLEND_NORMAL|BLEND_LIGHTEN|BLEND_DARKEN|BLEND_MULTIPLY|BLEND_AVERAGE|BLEND_ADD|BLEND_SUBTRACT|BLEND_DIFFERENCE|BLEND_NEGATION|\
BLEND_SCREEN|BLEND_EXCLUSION|BLEND_OVERLAY|BLEND_SOFT_LIGHT|BLEND_HARD_LIGHT|BLEND_COLOR_DODGE|BLEND_COLOR_BURN|BLEND_LINEAR_DODGE|\
BLEND_LINEAR_BURN|BLEND_LINEAR_LIGHT|BLEND_VIVID_LIGHT|BLEND_PIN_LIGHT|BLEND_HARD_MIX|BLEND_REFLECT|BLEND_GLOW|BLEND_PHOENIX,bitmap_type:enum?integer|float";
    vsx_string nn;
    switch (blend_type)
    {
      case BLEND_NORMAL       :nn = "blend_normal"; break;
      case BLEND_LIGHTEN      :nn = "blend_lighten"; break;
      case BLEND_DARKEN       :nn = "blend_darken"; break;
      case BLEND_MULTIPLY     :nn = "blend_multiply"; break;
      case BLEND_AVERAGE      :nn = "blend_average"; break;
      case BLEND_ADD          :nn = "blend_add"; break;
      case BLEND_SUBTRACT     :nn = "blend_subtract"; break;
      case BLEND_DIFFERENCE   :nn = "blend_difference"; break;
      case BLEND_NEGATION     :nn = "blend_negation"; break;
      case BLEND_SCREEN       :nn = "blend_screen"; break;
      case BLEND_EXCLUSION    :nn = "blend_exclusion"; break;
      case BLEND_OVERLAY      :nn = "blend_overlay"; break;
      case BLEND_SOFT_LIGHT   :nn = "blend_soft_light"; break;
      case BLEND_HARD_LIGHT   :nn = "blend_hard_light"; break;
      case BLEND_COLOR_DODGE  :nn = "blend_color_dodge"; break;
      case BLEND_COLOR_BURN   :nn = "blend_color_burn"; break;
      case BLEND_LINEAR_DODGE :nn = "blend_linear_dodge"; break;
      case BLEND_LINEAR_BURN  :nn = "blend_linear_burn"; break;
      case BLEND_LINEAR_LIGHT :nn = "blend_linear_light"; break;
      case BLEND_VIVID_LIGHT  :nn = "blend_vivid_light"; break;
      case BLEND_PIN_LIGHT    :nn = "blend_pin_light"; break;
      case BLEND_HARD_MIX     :nn = "blend_hard_mix"; break;
      case BLEND_REFLECT      :nn = "blend_reflect"; break;
      case BLEND_GLOW         :nn = "blend_glow"; break;
      case BLEND_PHOENIX      :nn = "blend_phoenix"; break;
    }
    info->identifier = "bitmaps;filters;bitm_"+nn;
    info->out_param_spec = "bitmap:bitmap";
    info->component_class = "bitmap";
    info->description = "Multiplies bitmaps with each other\nMust be of same size!";
  }
  
  /*void param_set_notify(const vsx_string& name) {
    need_to_rebuild = true;
  };*/
  int timestamp1;
  int timestamp2;

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    thread_state = 0;
    worker_running = false;
    p_updates = -1;
    in1 = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"in1");
    in2 = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"in2");
    result1 = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    
    target_size = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"target_size");
    target_size->set(512.0f,0);
    target_size->set(512.0f,1);
    bitm1_ofs = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"bitm1_ofs");
    bitm2_ofs = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"bitm2_ofs");

    bitm2_opacity = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"bitm2_opacity");
    bitm2_opacity->set(1.0f);

    filter_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"blend_type");
    filter_type->set(blend_type);

    bitmap_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"bitmap_type");
    
    work_bitmap = &bitm;
    bitm.data = 0;
    bitm.bpp = 4;
    bitm.bformat = GL_RGBA;
    bitm.valid = false;
    my_ref = 0;
    bitm.ref = &my_ref;    
    bitm_timestamp = bitm.timestamp = rand();
    need_to_rebuild = true;
    //bitm.data = new vsx_bitmap_32bt[256*256];
    //bitm.size_y = bitm.size_x = 256;
    bitm.size_y = bitm.size_x = 0;
    timestamp1 = timestamp2 = -1;
    to_delete_data = 0;
    result1->set_p(bitm);
  }
  void *to_delete_data;

  void run() {
    bitm1 = in1->get_addr();
    bitm2 = in2->get_addr();
    // initialize our worker thread, we don't want to keep the renderloop waiting do we?
    if (!worker_running)
    if (bitm1 && bitm2 && !to_delete_data)
    {
      if (bitm1->valid && bitm2->valid)
      if (timestamp1 != bitm1->timestamp || timestamp2 != bitm2->timestamp || p_updates != param_updates) {
        *(bitm1->ref) = 1;
        *(bitm2->ref) = 1;
        //printf("both bitmaps are OK it seems %d - %d\n",bitm1->timestamp,timestamp1);
        p_updates = param_updates;
        bitm.valid = false;
        timestamp1 = bitm1->timestamp;
        timestamp2 = bitm2->timestamp;
        
        if (bitm.size_x != (unsigned long)target_size->get(0) || bitm.size_y != (unsigned long)target_size->get(1))
        {
          if (bitm.data != 0) to_delete_data = bitm.data;
          bitm.data = new vsx_bitmap_32bt[(int)target_size->get(0)*(int)target_size->get(1)];
          bitm.size_x = (int)target_size->get(0);
          bitm.size_y = (int)target_size->get(1);
          
        }
        
        //printf("creating thread\n");
        thread_state = 1;
        worker_running = true;
        pthread_create(&worker_t, NULL, &worker, (void*)this);
        //printf("done creating thread\n");
      }
    }
    if (thread_state == 2) {
      bitm1->ref = 0;
      bitm2->ref = 0;
      if (bitm.valid && bitm_timestamp != bitm.timestamp) {
        //pthread_join(worker_t,0);
        worker_running = false;
        // ok, new version
        //printf("uploading subplasma to param\n");
        bitm_timestamp = bitm.timestamp;
        result1->set_p(bitm);
        loading_done = true;
      }
      thread_state = 3;
    }
    if (thread_state == 3)
    if (to_delete_data && (*bitm.ref) == 0)
    {
      delete[] (vsx_bitmap_32bt*)to_delete_data;
      to_delete_data = 0;
    }
    
  }
  
  void on_delete() {
    // wait for thread to finish
    void* ret;
    pthread_join(worker_t,&ret);
    delete[] (vsx_bitmap_32bt*)bitm.data;
  }
};
