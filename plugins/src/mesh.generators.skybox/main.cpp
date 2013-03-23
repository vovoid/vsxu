/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
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

#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "pthread.h"
#include "vsx_math_3d.h"


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



class vsx_module_plugin : public vsx_module {
  // in
  vsx_module_param_bitmap* bitm_in;
  // out
  vsx_module_param_render* render_result;
  vsx_module_param_bitmap* positive_x;
  vsx_module_param_bitmap* negative_x;
  vsx_module_param_bitmap* positive_y;
  vsx_module_param_bitmap* negative_y;
  vsx_module_param_bitmap* positive_z;
  vsx_module_param_bitmap* negative_z;
  // internal
  bool              worker_running;
  bool              worker_ever_started;
  pthread_t         worker_t[6];
  pthread_attr_t    worker_t_attr[6];


public:
  vsx_bitmap* bitm;
  vsx_bitmap bitm_p;
  vsx_bitmap result_bitm[6];
  vsx_texture result_tex[6];
  int bitm_timestamp;
  int mapMode;

  int cylSizeX, cylSizeY;

  vsx_bitmap_32bt getColor(float u, float v)
  {

    vsx_bitmap_32bt vv = (vsx_bitmap_32bt)floor(v);
    vsx_bitmap_32bt uu = (vsx_bitmap_32bt)floor(u);
    vsx_bitmap_32bt c00 = ((vsx_bitmap_32bt*)bitm_p.data)[(vv    ) % cylSizeY * cylSizeX + (uu    ) % cylSizeX];
    vsx_bitmap_32bt c01 = ((vsx_bitmap_32bt*)bitm_p.data)[(vv    ) % cylSizeY * cylSizeX + (uu + 1) % cylSizeX];
    vsx_bitmap_32bt c10 = ((vsx_bitmap_32bt*)bitm_p.data)[(vv + 1) % cylSizeY * cylSizeX + (uu    ) % cylSizeX];
    vsx_bitmap_32bt c11 = ((vsx_bitmap_32bt*)bitm_p.data)[(vv + 1) % cylSizeY * cylSizeX + (uu + 1) % cylSizeX];

    /*if (((vsx_bitmap_32bt)v    ) % cylSizeY * cylSizeX + ((vsx_bitmap_32bt)u    ) % cylSizeX > 1024*2048)
    printf("u: %f v: %f\n",u,v);*/

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

    //  float sphU = (float)(atan2(vec[0], vec[2]) / PI + 1) * 0.5f * cylSizeX;
    float sphU = (float)(atan2(vec[0], vec[2]) / PI ) * 0.5f * cylSizeX;

    //  cylV = fmod(fabs(cylV), cylSizeY);
    //  cylU = fmod(cylU * 7, cylSizeX);
    //  cylV = fmod(cylV * 4, cylSizeY);

    return getColor(sphU, sphV);
  }

  void skyBoxFromMap(int plane)
  {
    int texSize = 512;
    int texSize1 = texSize - 1;

  //  cylSizeX = bitm->size_x-1;
  //  cylSizeY = bitm->size_y-1;
    cylSizeX = bitm->size_x;
    cylSizeY = bitm->size_y;

    int u, v;

    float vec[4];

  //  for(int plane = 0; plane < 6; plane++) {
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
  //            result_bitm[plane].data[v * texSize + u] = getColorCyl(vec);
            break;
            case MAP_SPHERE:
              //if ( plane)
              ((vsx_bitmap_32bt*)result_bitm[plane].data)[v * texSize + u] = getColorSph(vec);
            break;
          }
        }
      }
      ++result_bitm[plane].timestamp;
  //  }

  /*
    for(v = 0; v < texSize; v++) {
      for(u = 0; u < texSize; u++) {
        float vec[4] = {
          ((float)u / texSize1 - 0.5f) * 2,
          ((float)v / texSize1 - 0.5f) * 2,
          1,
          1};
        planes[0][v * texSize + u] = getColorCyl(vec);
      }
    }

    for(v = 0; v < texSize; v++) {
      for(u = 0; u < texSize; u++) {
        float vec[4] = {
          1,
          ((float)v / texSize1 - 0.5f) * 2,
          -((float)u / texSize1 - 0.5f) * 2,
          1};
        planes[1][v * texSize + u] = getColorCyl(vec);

      }
    }

    for(v = 0; v < texSize; v++) {
      for(u = 0; u < texSize; u++) {
        float vec[4] = {
          -((float)u / texSize1 - 0.5f) * 2,
          ((float)v / texSize1 - 0.5f) * 2,
          -1,
          1};
        planes[2][v * texSize + u] = getColorCyl(vec);
      }
    }

    for(v = 0; v < texSize; v++) {
      for(u = 0; u < texSize; u++) {
        float vec[4] = {
          -1,
          ((float)v / texSize1 - 0.5f) * 2,
          ((float)u / texSize1 - 0.5f) * 2,
          1};
        planes[3][v * texSize + u] = getColorCyl(vec);
      }
    }

    for(v = 0; v < texSize; v++) {
      for(u = 0; u < texSize; u++) {
        float vec[4] = {
          -((float)v / texSize1 - 0.5f) * 2,
          -1,
          ((float)u / texSize1 - 0.5f) * 2,
          1};
        planes[4][v * texSize + u] = getColorCyl(vec);
      }
    }

    for(v = 0; v < texSize; v++) {
      for(u = 0; u < texSize; u++) {
        float vec[4] = {
          ((float)v / texSize1 - 0.5f) * 2,
          1,
          ((float)u / texSize1 - 0.5f) * 2,
          1};
        planes[5][v * texSize + u] = getColorCyl(vec);
      }
    }
  */
  //  delete cylImg;
  //printf("done with plane %d\n",plane);
  }



  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;vovoid;skybox";
    info->in_param_spec = "bitmap:bitmap";
    info->out_param_spec =  "render_out:render,"
                            "bitmaps:complex{"
                              "positive_x:bitmap,"
                              "negative_x:bitmap,"
                              "positive_y:bitmap,"
                              "negative_y:bitmap,"
                              "positive_z:bitmap,"
                              "negative_z:bitmap"
                            "}";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mapMode = MAP_SPHERE;
    bitm_in = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"bitmap");
    //bitm_in->set(bitm);
    //bitm_timestamp = bitm.timestamp;
    bitm_timestamp = 0;//bitm.timestamp;

    positive_x = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_x");
    //positive_x->set_p(result_bitm[0]);
    negative_x = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_x");
    //negative_x->set_p(result_bitm[1]);

    positive_y = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_y");
    //positive_y->set_p(result_bitm[2]);
    negative_y = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_y");
    //negative_y->set_p(result_bitm[3]);

    positive_z = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_z");
    //positive_z->set_p(result_bitm[4]);
    negative_z = (vsx_module_param_bitmap*)out_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_z");
    //negative_z->set_p(result_bitm[5]);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
    worker_running = false;
    worker_ever_started = false;
  }
  
  static void* worker(void *data)
  {
    //printf("skybox thread starting, plane is: %d\n",((SB_THREAD_DATA*)data)->p_plane);
    SB_THREAD_DATA* idata = (SB_THREAD_DATA*)data;
    void *target = idata->target;
    ((vsx_module_plugin*)target)->skyBoxFromMap(idata->p_plane);
    delete idata;
    return 0;
  }

  void run()
  {
    //printf("skybox run\n");
    bitm = bitm_in->get_addr();
    if (bitm) {
      if (bitm->valid && bitm_timestamp != bitm->timestamp) {
        /*if (bitm->bpp == 3) {
          bitm_p.valid = true;
          vsx_bitmap_32bt b_c = (bitm->size_x-1) * (bitm->size_y-1);
          char* rb = (char*)bitm->data;
          bitm_p.data = new vsx_bitmap_32bt[b_c];
          for (vsx_bitmap_32bt i = 0; i < b_c; ++i) {
            bitm_p.data[i] = 0xFF000000 | (unsigned char)rb[i*3+2] << 16 | (unsigned char)rb[i*3+1] << 8 | (unsigned char)rb[i*3];
          }
        } else */
        if (bitm->bpp == 4) {
          bitm_p.data = bitm->data;
        }
        //printf("x: %d, y: %d\n",bitm->size_x, bitm->size_y);
        // ok, new version
        for (int i = 0; i < 6; ++i) {
          result_bitm[i].timestamp = 0;
          result_bitm[i].data = new vsx_bitmap_32bt[512*512];
          result_bitm[i].size_x = 512;
          result_bitm[i].size_y = 512;
    //      for (int j = 0; j < 512*512; ++j) result_bitm[i].data[j] = 0xFF000000+rand()%255;
          result_bitm[i].bpp = 4;
          result_bitm[i].bformat = GL_RGBA;

          SB_THREAD_DATA* h = new SB_THREAD_DATA;
          h->p_plane = i;
          h->target = (void*)this;
          pthread_attr_init(&worker_t_attr[i]);
          pthread_create(&worker_t[i], &worker_t_attr[i], &worker, (void*)h);
          worker_ever_started = true;
          //sched_param s_param;
          //int policy = 0;
          //s_param.sched_priority = 20;
          //pthread_setschedparam (worker_t[i],policy,&s_param);

          result_tex[i].init_opengl_texture();
    //      result_tex[i].upload_ram_bitmap(&result_bitm[i]);
    //      loading_done = true;
        }
        bitm_timestamp = bitm->timestamp;
      }

      if (!loading_done) {
        int count_finished = 0;
        for (int i = 0; i < 6; ++i)
        if (result_bitm[i].timestamp) ++count_finished;

        if (count_finished == 6) {
          for (int i = 0; i < 6; ++i) {
            //printf("uploading plane %d\n",i);
            result_tex[i].upload_ram_bitmap(&result_bitm[i]);
            result_bitm[i].valid = true;
            //delete[] result_bitm[i].data;
          }
  //        positive_x->set_p(result_bitm[5]);
  //        negative_x->set_p(result_bitm[4]);
  //        positive_y->set_p(result_bitm[3]);
  //        negative_y->set_p(result_bitm[2]);
  //        positive_z->set_p(result_bitm[1]);
  //        negative_z->set_p(result_bitm[0]);



          positive_x->set_p(result_bitm[0]);
          negative_x->set_p(result_bitm[2]);
          positive_y->set_p(result_bitm[4]);
          negative_y->set_p(result_bitm[5]);
          positive_z->set_p(result_bitm[3]);
          negative_z->set_p(result_bitm[1]);

          loading_done = true;
        }
      }
    }
  //  ((vsx_param_render*)out_parameter)->set(1);
    //printf("skybox run\n");
  }

  void output(vsx_module_param_abs* param)
  {
    if (loading_done)
    {
      if (param != render_result) return;
      //printf("skybox output\n");
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


      /*#define DRAWPLANE \
      glTexCoord2f(0.5f / 512.0f, 0.5f / 512.0f); \
      glVertex3f(1000, 1000, -1000); \
      glTexCoord2f(0.5f / 512.0f, 511.5f / 512.0f); \
      glVertex3f(1000, -1000, -1000); \
      glTexCoord2f(511.5f / 512.0f, 511.5f / 512.0f); \
      glVertex3f(-1000, -1000, -1000); \
      glTexCoord2f(511.5f / 512.0f, 0.5f / 512.0f); \
      glVertex3f(-1000, 1000, -1000)*/

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
      render_result->set(1);
      return;
    }
    render_result->set(0);
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
      result_tex[i].unload();
    }
  }

  void start()
  {
    if (!loading_done) {
      bitm_timestamp = -1;
    }
    for (int i = 0; i < 6; ++i) {
      //printf("uploading plane %d\n",i);
      result_tex[i].init_opengl_texture();
      result_tex[i].upload_ram_bitmap(&result_bitm[i]);
    }
  }

  void on_delete()
  {
    //if (bitm_p.valid) delete bitm_p.data;
    for (int i = 0; i < 6; ++i) {
      result_tex[i].unload();
      if (result_bitm[i].timestamp) delete[] (vsx_bitmap_32bt*)result_bitm[i].data;
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
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module, void* args) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_plugin);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_plugin*)m; break;
  }
}

unsigned long get_num_modules() {
  return 1;
}


