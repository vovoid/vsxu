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
#include "vsx_param.h"
#include <module/vsx_module.h>
#include <vsx_font.h>
#include <FTGL/ftgl.h>

typedef struct
{
  float size_x, size_y;
  vsx_string<>string;
} text_info;

class vsx_module_text_s : public vsx_module {
  FTFont* ftfont;
  FTFont* ftfont2;
  vsx_string<>cur_font;
  int cur_render_type;
  float cur_glyph_size;

  // in
  vsx_module_param_float* glyph_size;
  vsx_module_param_float* size;
  vsx_module_param_float* angle;
  vsx_module_param_float* red;
  vsx_module_param_float* green;
  vsx_module_param_float* blue;
  vsx_module_param_float3* rotation_axis;
  vsx_module_param_string* text_in;
  vsx_module_param_resource* font_in;
  vsx_module_param_int* render_type;
  vsx_module_param_int* align;
  vsx_module_param_float* leading;
  vsx_module_param_float* limit_line;

  vsx_module_param_float* text_alpha;
  vsx_module_param_float4* outline_color;
  vsx_module_param_float* outline_alpha;
  vsx_module_param_float* outline_thickness;
  // out
  vsx_module_param_render* render_result;
  // internal

  vsx_nw_vector<text_info> lines;

  vsx_gl_state* gl_state;

public:


void module_info(vsx_module_specification* info)
{

  info->identifier =
    "renderers;text;text_s";

  info->in_param_spec =
    "text_in:string,"
    "font_in:resource?nc=1,"
    "render_type:enum?BITMAP|POLYGON,"
    "align:enum?LEFT|CENTER|RIGHT,"
    "limits:complex"
    "{"
      "limit_line:float"
    "},"
    "appearance:complex"
    "{"
      "glyph_size:float,"
      "size:float,"
      "leading:float,"
      "angle:float,"
      "rotation_axis:float3,"
      "text_alpha:float,"
      "outline_alpha:float,"
      "outline_color:float4,"
      "outline_thickness:float,"
      "color:complex"
      "{"
        "red:float,"
        "green:float,"
        "blue:float"
      "}"
    "}"
  ;

  info->out_param_spec =
    "render_out:render";

  info->component_class =
    "render";
}

bool declare_run;
void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  declare_run = false;
  size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size");
  size->set(1.0f);
  angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "angle");
  angle->set(0.0f);

  text_in = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING, "text_in");
  text_in->set("Vovoid VSX Ultra");
  text_in->updates = 1;
  font_in = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE, "font_in");
  font_in->set("resources/fonts/pala.ttf");
  cur_font = "";

  limit_line = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "limit_line");
  limit_line->set(-1.0f);


  leading = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "leading");
  leading->set(1.0f);

  glyph_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "glyph_size");
  glyph_size->set(24.0f);
  cur_glyph_size = 24.0f;
  render_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"render_type");
  render_type->set(0);
  align = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"align");
  align->set(0);
  cur_render_type = 0;

  ftfont = 0;
  ftfont2 = 0;

  rotation_axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "rotation_axis");
  rotation_axis->set(0.0f, 0);
  rotation_axis->set(1.0f, 1);
  rotation_axis->set(0.0f, 2);
  red = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "red");
  red->set(1.0f);
  green = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "green");
  green->set(1.0f);
  blue = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "blue");
  blue->set(1.0f);

  text_alpha = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "text_alpha");
  text_alpha->set(1.0);
  outline_alpha = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "outline_alpha");
  outline_alpha->set(0.5);
  outline_thickness = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "outline_thickness");
  outline_thickness->set(3.0);
  outline_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "outline_color");
  outline_color->set(0.0f, 0);
  outline_color->set(0.0f, 1);
  outline_color->set(0.0f, 2);
  outline_color->set(0.0f, 3);

  render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  render_result->set(0);
  declare_run = true;

  gl_state = vsx_gl_state::get_instance();
}


  int process_lines()
  {
    if (!ftfont) return 0;
    vsx_string<>deli = "\n";
    vsx_nw_vector< vsx_string<> > t_lines;
    vsx_string_helper::explode(text_in->get(), deli, t_lines);
    lines.clear();
    for (unsigned long i = 0; i < t_lines.size(); ++i)
    {
      float x1, y1, z1, x2, y2, z2;
      lines[i].string = t_lines[i];
      ftfont->BBox(t_lines[i].c_str(), x1, y1, z1, x2, y2, z2);
      lines[i].size_x = x2 - x1;
      lines[i].size_y = y2 - y1;
    }
    return 1;
  }

  void param_set_notify(const vsx_string<>& name)
  {
    if (!declare_run)
      return;

    if (name == "font_in" || name == "glyph_size")
    {
      setup_font();
      if (ftfont)
      {
        process_lines();
      }
    }
  }

  void setup_font()
  {
    if
    (
      (cur_font != font_in->get()) ||
      (cur_render_type !=render_type->get()) ||
      (cur_glyph_size != glyph_size->get())
    )
    {
      vsx::file *fp;
      if ((fp = engine_state->filesystem->f_open(font_in->get().c_str())) == NULL)
      {
        printf("font not found: %s\n",cur_font.c_str());
        return;
      }
      cur_font = font_in->get();
      cur_render_type = render_type->get();
      cur_glyph_size = glyph_size->get();

      if (ftfont) {
        delete ftfont;
        ftfont = 0;
      }
      if (ftfont2) {
        delete ftfont2;
        ftfont2 = 0;
      }
      unsigned long size = engine_state->filesystem->f_get_size(fp);
      char* fdata = (char*)malloc(size);
      unsigned long bread = engine_state->filesystem->f_read((void*)fdata, size, fp);
      if (bread == size)
      {
        switch (cur_render_type)
        {
          case 0:
            ftfont = new FTGLTextureFont((unsigned char*)fdata, size);
          break;
          case 1:
            ftfont = new FTGLPolygonFont((unsigned char*)fdata, size);
            ftfont2 = new FTGLOutlineFont((unsigned char*)fdata, size);
          break;
        }
        ftfont->FaceSize((unsigned int)round(cur_glyph_size));
        ftfont->CharMap(ft_encoding_unicode);
        if (ftfont2) {
          ftfont2->FaceSize((unsigned int)round(cur_glyph_size));
          ftfont2->CharMap(ft_encoding_unicode);
        }
        loading_done = true;
      }
      engine_state->filesystem->f_close(fp);
    }
  }

  void run()
  {
    setup_font();
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    if (text_in->updates)
    {
      if (process_lines())
      text_in->updates = 0;
    }
    if (text_alpha->get() <= 0)
      return;

    if (!ftfont)
    {
      user_message = "module||error loading font "+cur_font;
      return;
    }

    if (text_in->get() == "_")
      return;


    float obj_size = size->get();

    gl_state->matrix_mode (VSX_GL_MODELVIEW_MATRIX );
    gl_state->matrix_push();

    gl_state->matrix_rotate_f( (float)angle->get()*360, rotation_axis->get(0), rotation_axis->get(1), rotation_axis->get(2) );

    if (obj_size < 0)
      obj_size = 0;

    gl_state->matrix_scale_f( obj_size*0.8*0.01, obj_size*0.01, obj_size*0.01 );

    int l_align = align->get();
    float l_leading = leading->get();
    float ypos = 0;

    if (cur_render_type == 0)
      glEnable(GL_TEXTURE_2D);

    glColor4f(red->get(),green->get(),blue->get(),text_alpha->get());

    for (unsigned long i = 0; i < lines.size(); ++i)
    {
      float ll = limit_line->get();
      if (ll != -1.0f)
      {
        if (trunc(ll) != i) continue;
      }
      gl_state->matrix_push();
      if (l_align == 0)
      {
        gl_state->matrix_translate_f( 0, ypos, 0 );
      } else
      if (l_align == 1)
      {
        gl_state->matrix_translate_f( -lines[i].size_x*0.5f,ypos,0 );
      }
      if (l_align == 2)
      {
        gl_state->matrix_translate_f( -lines[i].size_x,ypos,0 );
      }

      if (cur_render_type == 1)
      {
        if (outline_alpha->get() > 0.0f && ftfont2) {
          float pre_linew;
          pre_linew = gl_state->line_width_get();
          gl_state->line_width_set( outline_thickness->get() );
          glColor4f(outline_color->get(0),outline_color->get(1),outline_color->get(2),outline_alpha->get()*outline_color->get(3));
          ftfont2->Render(lines[i].string.c_str());
          gl_state->line_width_set( pre_linew );
        }
        glColor4f(red->get(),green->get(),blue->get(),text_alpha->get());
      }

      ftfont->Render(lines[i].string.c_str());
      gl_state->matrix_pop();
      ypos += l_leading;
    }

    if (cur_render_type == 0)
      glDisable(GL_TEXTURE_2D);


    gl_state->matrix_pop();

    render_result->set(1);
    loading_done = true;
  }

  void stop()
  {
    if (ftfont)
    {
      delete ftfont;
      ftfont = 0;
    }
  }

  void start()
  {
    cur_font = "";
    setup_font();
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
__declspec(dllexport) unsigned long get_num_modules(vsx_module_engine_environment* environment);
}

#ifndef MOD_CM
#define MOD_CM vsx_module_render_text_cm
#define MOD_DM vsx_module_render_text_dm
#define MOD_NM vsx_module_render_text_nm
#endif


vsx_module* MOD_CM(unsigned long module, void* args)
{
  VSX_UNUSED(args);

  // as we have only one module available, don't look at the module variable, just return - for speed
  // otherwise you'd have something like,, switch(module) { case 0: break; }
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_text_s);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module)
{
  switch(module) {
    case 0: delete (vsx_module_text_s*)m; break;
  }
}

unsigned long MOD_NM(vsx_module_engine_environment* environment)
{
  VSX_UNUSED(environment);
  // we have only one module. it's id is 0
  return 1;
}
