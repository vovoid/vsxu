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


class module_mesh_render_line : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_int* center;
  vsx_module_param_int* override_base_color;
  vsx_module_param_float4* base_color;
  vsx_module_param_float4* base_color_add;
  vsx_module_param_float4* center_color;
  vsx_module_param_float4* center_color_add;
  vsx_module_param_float* line_width;

  // out
  vsx_module_param_render* render_out;

  // internal
  vsx_mesh<>** mesh;
  vsx_matrix<float> ma;
  vsx_vector3<> upv;

  GLuint dlist;
  bool list_built;
  GLfloat prev_width;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;mesh;mesh_line_render";

    info->description =
      "Draws lines between points in a mesh.\n"
      "Faces data is ignored.\n"
      "If vertex colors are undefined you\n"
      "can override the colors - then the\n"
      "[base_color_add] parameter is ignored.";

    info->in_param_spec =
      "mesh_in:mesh,"
      "base_options:complex"
      "{"
        "line_width:float,"
        "override_base_color:enum?no|yes,"
        "base_color:float4,"
        "base_color_add:float4"
      "},"
      "center_options:complex{"
        "each_to_center:enum?no|yes,"
        "center_color:float4,"
        "center_color_add:float4"
      "}"
    ;

    info->out_param_spec =
      "render_out:render";

    info->component_class =
      "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    override_base_color = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"override_base_color");
    override_base_color->set(0);
    line_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"line_width");
    line_width->set(1.0f);
    base_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color");
    base_color->set(1.0f,0);
    base_color->set(1.0f,1);
    base_color->set(1.0f,2);
    base_color->set(1.0f,3);
    base_color_add = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color_add");
    base_color_add->set(0.0f,0);
    base_color_add->set(0.0f,1);
    base_color_add->set(0.0f,2);
    base_color_add->set(0.0f,3);
    center_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"center_color");
    center_color->set(1.0f,0);
    center_color->set(1.0f,1);
    center_color->set(1.0f,2);
    center_color->set(1.0f,3);
    center_color_add = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"center_color_add");
    center_color_add->set(0.0f,0);
    center_color_add->set(0.0f,1);
    center_color_add->set(0.0f,2);
    center_color_add->set(0.0f,3);
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    center = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"each_to_center");
    center->set(0);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    loading_done = true;
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);

    glGetFloatv(GL_LINE_WIDTH,&prev_width);
    glLineWidth(line_width->get());
    glEnable(GL_LINE_SMOOTH);

    mesh = mesh_in->get_addr();
    if (!mesh) return;


    if (center->get())
    {
      vsx_color<> l_center_color = vsx_color<>(center_color->get(0)+center_color_add->get(0),center_color->get(1)+center_color_add->get(1),center_color->get(2)+center_color_add->get(2),center_color->get(3)+center_color_add->get(3));
      vsx_color<> main_color = vsx_color<>(base_color->get(0)+base_color_add->get(0),base_color->get(1)+base_color_add->get(1),base_color->get(2)+base_color_add->get(2),base_color->get(3)+base_color_add->get(3));

      glBegin(GL_LINES);
      if (override_base_color->get())
      {

        for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i)
        {
          glColor4f(
            l_center_color.r,
            l_center_color.g,
            l_center_color.b,
            l_center_color.a
          );
          glVertex3f(0.0f,0.0f,0.0f);
          glColor4f(
            main_color.r,
            main_color.g,
            main_color.b,
            main_color.a
          );
          glVertex3f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z);
        }
      }
      else
      {
        for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i)
        {
          glColor4f(
            l_center_color.r,
            l_center_color.g,
            l_center_color.b,
            l_center_color.a
          );
            glVertex3f(0.0f,0.0f,0.0f);
            glColor4f(
              (*mesh)->data->vertex_colors[i].r,
              (*mesh)->data->vertex_colors[i].g,
              (*mesh)->data->vertex_colors[i].b,
              (*mesh)->data->vertex_colors[i].a
            );

            glVertex3f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z);
        }
      }
    } // center->get()
    else
    {
      glBegin(GL_LINE_STRIP);
      if (override_base_color->get())
      {
        glColor4f(base_color->get(0)+base_color_add->get(0),base_color->get(1)+base_color_add->get(1),base_color->get(2)+base_color_add->get(2),base_color->get(3)+base_color_add->get(3));
        for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i)
        {
          glVertex3f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z);
        }
      }
      else
      {
        for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i)
        {
          if ((*mesh)->data->vertex_colors.size())
          {
            glColor4f(
              (*mesh)->data->vertex_colors[i].r,
              (*mesh)->data->vertex_colors[i].g,
              (*mesh)->data->vertex_colors[i].b,
              (*mesh)->data->vertex_colors[i].a
            );
          }
          glVertex3f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z);
        }
      }
    }
    glEnd();
    render_out->set(1);
  }

};
