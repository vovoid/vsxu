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


#include "_configuration.h"
#include <vsx_param.h>
#include <vsx_module.h>
#include <pthread.h>


#define MAP_CYLINDER 0
#define MAP_SPHERE 1


typedef struct {
  void* target;
  int p_plane;
} SB_THREAD_DATA;


void inline v_smult(GLfloat *v, float s){
  v[0] *= s;
  v[1] *= s;
  v[2] *= s;
}

float v_rlen(float *v){
  return 1.0f / (GLfloat)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}



class module_render_skybox : public vsx_module
{
  // in
  vsx_module_param_bitmap* bitmap_in;

  // out
  vsx_module_param_render* render_out;
  vsx_module_param_bitmap* positive_x_out;
  vsx_module_param_bitmap* negative_x_out;
  vsx_module_param_bitmap* positive_y_out;
  vsx_module_param_bitmap* negative_y_out;
  vsx_module_param_bitmap* positive_z_out;
  vsx_module_param_bitmap* negative_z_out;

  // internal
  bool              worker_running = false;
  bool              worker_ever_started = false;
  pthread_t         worker_t[6];
  vsx_bitmap* bitmap;
  vsx_bitmap bitm_p;
  vsx_bitmap result_bitm[6];
  vsx_texture<> result_tex[6];
  int bitmap_in_timestamp = 0;
  int mapMode = MAP_SPHERE;
  int cylSizeX, cylSizeY;

public:


  vsx_bitmap_32bt getColor(float u, float v)
  {

    vsx_bitmap_32bt vv = (vsx_bitmap_32bt)floor(v);
    vsx_bitmap_32bt uu = (vsx_bitmap_32bt)floor(u);
    vsx_bitmap_32bt c00 = ((vsx_bitmap_32bt*)bitm_p.data)[(vv    ) % cylSizeY * cylSizeX + (uu    ) % cylSizeX];
    vsx_bitmap_32bt c01 = ((vsx_bitmap_32bt*)bitm_p.data)[(vv    ) % cylSizeY * cylSizeX + (uu + 1) % cylSizeX];
    vsx_bitmap_32bt c10 = ((vsx_bitmap_32bt*)bitm_p.data)[(vv + 1) % cylSizeY * cylSizeX + (uu    ) % cylSizeX];
    vsx_bitmap_32bt c11 = ((vsx_bitmap_32bt*)bitm_p.data)[(vv + 1) % cylSizeY * cylSizeX + (uu + 1) % cylSizeX];

    float fracU = (u + 10000) - (int)(u + 10000);
    float fracV = (v + 10000) - (int)(v + 10000);

    vsx_bitmap_32bt a, r, g, b;
    vsx_bitmap_32bt a00, a01, a10, a11;
    vsx_bitmap_32bt r00, r01, r10, r11;
    vsx_bitmap_32bt g00, g01, g10, g11;
    vsx_bitmap_32bt b00, b01, b10, b11;

    #define sep_col(col, a, r, g, b)\
      a = (((col) >> 24) & 255);\
      r = (((col) >> 16) & 255);\
      g = (((col) >> 8) & 255);\
      b = (((col) >> 0) & 255);

    #define interpol(x, x00, x01, x10, x11, fracU, fracV)\
      x = (vsx_bitmap_32bt)(x00 * (1.0f - fracU) * (1.0f - fracV) + \
          x01 * (fracU) * (1.0f - fracV) + \
          x10 * (1.0f - fracU) * (fracV) + \
          x11 * (fracU) * (fracV));

    sep_col(c00, a00, r00, g00, b00);
    sep_col(c01, a01, r01, g01, b01);
    sep_col(c10, a10, r10, g10, b10);
    sep_col(c11, a11, r11, g11, b11);

    interpol(a, a00, a01, a10, a11, fracU, fracV);
    interpol(r, r00, r01, r10, r11, fracU, fracV);
    interpol(g, g00, g01, g10, g11, fracU, fracV);
    interpol(b, b00, b01, b10, b11, fracU, fracV);

    return (a << 24) | (r << 16) | (g << 8) | b;
  }

  vsx_bitmap_32bt getColorSph(float *vec)
  {
    v_norm(vec);

    float sphV = (float)((acos(-vec[1]) / PI) ) * cylSizeY;

    float vec2[4] = {
      vec[0],
      0,
      vec[2],
      1
    };
    float scale = v_rlen(vec2);
    v_smult(vec, scale);

    float sphU = (float)(atan2(vec[0], vec[2]) / PI ) * 0.5f * cylSizeX;


    return getColor(sphU, sphV);
  }

  void skyBoxFromMap(int plane)
  {
    int texSize = 512;
    int texSize1 = texSize - 1;

    cylSizeX = bitmap->width;
    cylSizeY = bitmap->height;

    int u, v;

    float vec[4];

      for(v = 0; v < texSize; ++v) {
        for(u = 0; u < texSize; ++u) {
          switch(plane) {
            case 0:
              vec[0] = ((float)u / texSize1 - 0.5f) * 2;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2;
              vec[2] = 1;
              vec[3] = 1;
            break;
            case 1:
              vec[0] = 1;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2;
              vec[2] = -((float)u / texSize1 - 0.5f) * 2;
              vec[3] = 1;
            break;
            case 2:
              vec[0] = -((float)u / texSize1 - 0.5f) * 2;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2;
              vec[2] = -1;
              vec[3] = 1;
            break;
            case 3:
              vec[0] = -1;
              vec[1] = ((float)v / texSize1 - 0.5f) * 2;
              vec[2] = ((float)u / texSize1 - 0.5f) * 2;
              vec[3] = 1;
            break;
            case 4:
              vec[0] = -((float)v / texSize1 - 0.5f) * 2;
              vec[1] = -1;
              vec[2] = ((float)u / texSize1 - 0.5f) * 2;
              vec[3] = 1;
            break;
            case 5:
              vec[0] = ((float)v / texSize1 - 0.5f) * 2;
              vec[1] = 1;
              vec[2] = ((float)u / texSize1 - 0.5f) * 2;
              vec[3] = 1;
            break;
          }

          switch(mapMode) {
            case MAP_CYLINDER:
            break;
            case MAP_SPHERE:
              ((vsx_bitmap_32bt*)result_bitm[plane].data)[v * texSize + u] = getColorSph(vec);
            break;
          }
        }
      }
      result_bitm[plane].timestamp++;
      __sync_fetch_and_add( &result_bitm[plane].data_ready, 1);
  }



  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "renderers;vovoid;skybox";

    info->in_param_spec =
      "bitmap:bitmap";

    info->out_param_spec =
      "render_out:render,"
      "bitmaps:complex"
      "{"
        "positive_x:bitmap,"
        "negative_x:bitmap,"
        "positive_y:bitmap,"
        "negative_y:bitmap,"
        "positive_z:bitmap,"
        "negative_z:bitmap"
      "}"
    ;

    info->component_class =
      "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    bitmap_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");

    positive_x_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_x");
    negative_x_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_x");

    positive_y_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_y");
    negative_y_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_y");

    positive_z_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_z");
    negative_z_out = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_z");

    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_out->set(0);
  }
  
  static void* worker(void *data)
  {
    SB_THREAD_DATA* idata = (SB_THREAD_DATA*)data;
    void *target = idata->target;
    ((module_render_skybox*)target)->skyBoxFromMap(idata->p_plane);
    delete idata;
    return 0;
  }

  void run()
  {
    if (!bitmap_in->get_addr())
      return;

    bitmap = *(bitmap_in->get_addr());
    if (bitmap_in_timestamp != bitmap->timestamp)
    {
      if (bitmap->channels == 4)
        bitm_p.data[0] = bitmap->data[0];

      // ok, new version
      for (int i = 0; i < 6; ++i)
      {
        result_bitm[i].timestamp = 0;
        result_bitm[i].data[0] = malloc( sizeof(vsx_bitmap_32bt) * 512 * 512);
        result_bitm[i].width = 512;
        result_bitm[i].height = 512;
        result_bitm[i].channels = 4;
        result_bitm[i].storage_format = vsx_bitmap::byte_storage;

        SB_THREAD_DATA* h = new SB_THREAD_DATA;
        h->p_plane = i;
        h->target = (void*)this;
        pthread_create(&worker_t[i], NULL, &worker, (void*)h);
        worker_ever_started = true;

        result_tex[i].texture->init_opengl_texture_2d();
      }
      bitmap_in_timestamp = bitmap->timestamp;
    }

    if (!loading_done) {
      int count_finished = 0;
      for (int i = 0; i < 6; ++i)
      if (result_bitm[i].timestamp) ++count_finished;

      if (count_finished == 6) {
        for (int i = 0; i < 6; ++i)
          vsx_texture_gl_loader::upload_bitmap_2d(result_tex[i].texture, &result_bitm[i], true);

        positive_x_out->set(&result_bitm[0]);
        negative_x_out->set(&result_bitm[2]);
        positive_y_out->set(&result_bitm[4]);
        negative_y_out->set(&result_bitm[5]);
        positive_z_out->set(&result_bitm[3]);
        negative_z_out->set(&result_bitm[1]);

        loading_done = true;
      }
    }
  }

  void output(vsx_module_param_abs* param)
  {
    if (loading_done)
    {
      if (param != render_out) return;
      float a = 0.5f/512.0f;
      float b = 511.5f/512.0f;

      #define DRAWPLANE \
      glTexCoord2f(a, b); \
      glVertex3f(1000, 1000, -1000); \
      glTexCoord2f(a, a); \
      glVertex3f(1000, -1000, -1000); \
      glTexCoord2f(b, a); \
      glVertex3f(-1000, -1000, -1000); \
      glTexCoord2f(b, b); \
      glVertex3f(-1000, 1000, -1000)


      glColor3f(1.0f, 1.0f, 1.0f);

      glPushMatrix();

      glRotatef(180,0,0,1);
      glRotatef(180,0,1,0);

      result_tex[0].bind();
      glBegin(GL_QUADS);
          DRAWPLANE;
      glEnd();
      result_tex[0]._bind();


      glRotatef(90, 0, 1, 0);
      result_tex[1].bind();
      glBegin(GL_QUADS);
          DRAWPLANE;
      glEnd();
      result_tex[1]._bind();


      glRotatef(90, 0, 1, 0);
      result_tex[2].bind();

      glBegin(GL_QUADS);
          DRAWPLANE;
      glEnd();
      result_tex[2]._bind();

      glRotatef(90, 0, 1, 0);
      result_tex[3].bind();
      glBegin(GL_QUADS);
          DRAWPLANE;
      glEnd();
      result_tex[3]._bind();

      glRotatef(90, 1, 0, 0);
      result_tex[4].bind();
      glBegin(GL_QUADS);
          DRAWPLANE;
      glEnd();
      result_tex[4]._bind();


      result_tex[5].bind();
      glRotatef(180, 1, 0, 0);
      glBegin(GL_QUADS);
          DRAWPLANE;
      glEnd();
      result_tex[5]._bind();
      glPopMatrix();
      render_out->set(1);
      return;
    }
    render_out->set(0);
  }

  void stop()
  {
    if (!loading_done) {
      if (worker_ever_started)
      {
        for (int i = 0; i < 6; ++i) {
          pthread_join(worker_t[i],0);
        }
      }
    }
    for (int i = 0; i < 6; ++i) {
      result_tex[i].texture->unload();
    }
  }

  void start()
  {
    if (!loading_done) {
      bitmap_in_timestamp = -1;
    }
    for (int i = 0; i < 6; ++i) {
      result_tex[i].texture->init_opengl_texture_2d();
      vsx_texture_gl_loader::upload_bitmap_2d(result_tex[i].texture, &result_bitm[i], true);
    }
  }

  void on_delete()
  {
    for (int i = 0; i < 6; ++i) {
      result_tex[i].texture->unload();
      if (result_bitm[i].timestamp)
        free(result_bitm[i].data[0]);
    }
  }
};


//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules(vsx_engine_environment* environment);
}


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);
  switch(module) {
    case 0: return (vsx_module*)(new module_render_skybox);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (module_render_skybox*)m; break;
  }
}

unsigned long MOD_NM(vsx_engine_environment* environment) {
  VSX_UNUSED(environment);
  return 1;
}


