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


class face_holder
{
public:
  double dist;
  int id;
  bool operator < (const face_holder& a) {
      return dist > a.dist;
  }
};


class module_mesh_render_zsort : public vsx_module
{
  // in
  vsx_module_param_texture* tex_a;
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_int* vertex_colors;

  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_mesh<>** mesh;
  vsx_texture<>** ta;
  bool m_normals, m_tex, m_colors;
  vsx_matrix<float> mod_mat, proj_mat;
  vsx_nw_vector_nd<face_holder> f_distances;
  vsx_nw_vector_nd<vsx_face3> f_result;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;mesh;mesh_transparency_render";

    info->in_param_spec =
      "tex_a:texture,"
      "mesh_in:mesh,"
      "vertex_colors:enum?no|yes";

    info->out_param_spec =
      "render_out:render";

    info->component_class =
      "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    tex_a = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"tex_a");
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    vertex_colors = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"vertex_colors");
    vertex_colors->set(0);
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
  }


  static int fpartition(face_holder* a, int first, int last)
  {
    face_holder pivot = a[first];
    int lastS1 = first;
    int firstUnknown = first + 1;
    while (firstUnknown <= last) {
      if (a[firstUnknown] < pivot) {
        lastS1++;
        face_holder b = a[firstUnknown];
        a[firstUnknown] = a[lastS1];
        a[lastS1] = b;
      }
      firstUnknown++;
    }
    face_holder b = a[first];
    a[first] = a[lastS1];
    a[lastS1] = b;
    return lastS1;
  }

  static void fquicksort(face_holder* a, int first, int last)
  {
    if (first < last) {
      int pivotIndex = fpartition(a, first, last);
      fquicksort(a, first, pivotIndex - 1);
      fquicksort(a, pivotIndex + 1, last);
    }
  }

  static void fquicksort(face_holder* a, int aSize)
  {
    fquicksort(a, 0, aSize - 1);
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);

    ta = tex_a->get_addr();
    mesh = mesh_in->get_addr();
    if (!mesh) return;
    if ( !(*mesh)->data->faces.get_used() ) return;


    glGetFloatv(GL_MODELVIEW_MATRIX, mod_mat.m);
    glGetFloatv(GL_PROJECTION_MATRIX, proj_mat.m);

    glPushMatrix();
    glLoadIdentity();
    glMultMatrixf(proj_mat.m);
    glMultMatrixf(mod_mat.m);
    glGetFloatv(GL_PROJECTION_MATRIX, mod_mat.m);
    glPopMatrix();

    vsx_vector3<> center(0.0f);
    vsx_vector3<> deep(0,0,1);
    vsx_vector3<> istart = mod_mat.multiply_vector(center);
    vsx_vector3<> end = mod_mat.multiply_vector(deep);
    vsx_vector3<> sort_vec = end - istart;

    if (!(*mesh)->data->face_centers.size())
    {
      (*mesh)->data->calculate_face_centers();
    }

    // loop through all the faces and calculate the distance from the camera
    for (unsigned long i = 0; i < (*mesh)->data->face_centers.size(); ++i)
    {
      f_distances[i].dist = (*mesh)->data->face_centers[i].dot_product(&sort_vec);
      f_distances[i].id = i;
    }

    fquicksort(f_distances.get_pointer(),f_distances.size());
    for (unsigned long i = 0; i < (*mesh)->data->face_centers.size(); ++i)
    {
      f_result[i] = (*mesh)->data->faces[f_distances[i].id];
    }


    if (vertex_colors->get())
    {
      glEnable(GL_COLOR_MATERIAL);
    }

    if (ta)
    {
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();

      if ((*ta)->get_transform())
        (*ta)->get_transform()->transform();

      (*ta)->bind();
    }

    if ((*mesh)->data->vertex_colors.get_used())
    {
      glColorPointer(4,GL_FLOAT,0,(*mesh)->data->vertex_colors.get_pointer());
      glEnableClientState(GL_COLOR_ARRAY);
      m_colors = true;
    } else m_colors = false;

    if ((*mesh)->data->vertex_normals.get_used())
    {
      glNormalPointer(GL_FLOAT,0,(*mesh)->data->vertex_normals.get_pointer());
      glEnableClientState(GL_NORMAL_ARRAY);
      m_normals = true;
    } else m_normals = false;

    if ((*mesh)->data->vertex_tex_coords.get_used())
    {
      glTexCoordPointer(2,GL_FLOAT,0,(*mesh)->data->vertex_tex_coords.get_pointer());
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      m_tex = true;
    } else m_tex = false;

    glVertexPointer(3,GL_FLOAT,0,(*mesh)->data->vertices.get_pointer());
    glEnableClientState(GL_VERTEX_ARRAY);
      glDrawElements(GL_TRIANGLES,f_result.get_used()*3,GL_UNSIGNED_INT,f_result.get_pointer());
    glDisableClientState(GL_VERTEX_ARRAY);

    if (m_normals) {
      glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (m_tex) {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (m_colors) {
      glDisableClientState(GL_COLOR_ARRAY);
    }
    if (ta) (*ta)->_bind();
    if (vertex_colors->get()) {
      glDisable(GL_COLOR_MATERIAL);
    }
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();

    render_result->set(1);
    return;
  }
};



