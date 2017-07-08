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


#include <time.h>


class module_mesh_planeworld : public vsx_module
{
  // in
  vsx_module_param_mesh* bspline_vertices_mesh;

  // out
  vsx_module_param_render* render_result;
  vsx_module_param_mesh* mesh_result;

  // internal
  vsx_mesh<>* mesh;
  vsx_bspline<vsx_vector3f> spline0;
  vsx_2dgrid_mesh gmesh;

public:
  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }
  void on_delete()
  {
    delete mesh;
    gmesh.cleanup();
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;vovoid;planeworld";

    info->in_param_spec =
      "bspline_vertices_mesh:mesh";

    info->out_param_spec =
      "mesh_result:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_result");
    bspline_vertices_mesh = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"bspline_vertices_mesh");
    srand ( (unsigned int)time(NULL) );
  }

  void run()
  {
    vsx_mesh<>** spline_mesh = bspline_vertices_mesh->get_addr();
    if (!spline_mesh) return;
    if (mesh->data->faces.get_used() && (*spline_mesh)->timestamp == mesh->timestamp) return;
    mesh->timestamp = (*spline_mesh)->timestamp;
    if ((*spline_mesh)->data->vertices.size() > 500)
    {
      for (unsigned long i = 0; i < 500; ++i) spline0.points[i] = (*spline_mesh)->data->vertices[i];
    }
    else {
      spline0.points.set_volatile();
      spline0.points.set_data((*spline_mesh)->data->vertices.get_pointer(), (*spline_mesh)->data->vertices.size() );
    }

    if (!spline0.points.size()) return;

    vsx_vector3<> spos;

    spline0.step(1);
    spos = spline0.get_current();

    //vsx_vector upv = vsx_vector(0,1);

    const float s = 0.008f;


    gmesh.vertices[0][0].coord = vsx_vector3<>(-1*s,-1*s);
    gmesh.vertices[0][1].coord = vsx_vector3<>(1*s,-1*s);
    gmesh.vertices[0][2].coord = vsx_vector3<>(1*s,1*s);
    gmesh.vertices[0][3].coord = vsx_vector3<>(-1*s,1*s);

    float step = 0.05f;
    float iterations_p = 1.0f/step;

    vsx_vector3<> old;
    vsx_vector3<> e;
    vsx_vector3<> d;
    vsx_matrix<float> ma;
    //-----
    for (unsigned long i = 0; i < spline0.points.size()*(int)iterations_p; ++i) {
      spline0.step(step);
      spos = spline0.get_current();

      e = spos-old;
      old = spos;
      e.normalize();

      ma.rotation_from_vectors(&e);

      gmesh.vertices[i+1][0].coord = ma.multiply_vector(gmesh.vertices[0][0].coord)+spos;
      gmesh.vertices[i+1][1].coord = ma.multiply_vector(gmesh.vertices[0][1].coord)+spos;
      gmesh.vertices[i+1][2].coord = ma.multiply_vector(gmesh.vertices[0][2].coord)+spos;
      gmesh.vertices[i+1][3].coord = ma.multiply_vector(gmesh.vertices[0][3].coord)+spos;

      gmesh.vertices[i+1][0].tex_coord = vsx_vector3<>(0.0f);
      gmesh.vertices[i+1][1].tex_coord = vsx_vector3<>(1,0);
      gmesh.vertices[i+1][2].tex_coord = vsx_vector3<>(1,1);
      gmesh.vertices[i+1][3].tex_coord = vsx_vector3<>(0,1);



      gmesh.vertices[i+1][0].color = vsx_color<>(1,1,1,0.5f);
      gmesh.vertices[i+1][1].color = vsx_color<>(1,1,1,0.5f);
      gmesh.vertices[i+1][2].color = vsx_color<>(1,1,1,0.5f);
      gmesh.vertices[i+1][3].color = vsx_color<>(1,1,1,0.5f);

      gmesh.add_face(i+1,1, i+1,3, i+1,0);
      gmesh.add_face(i+1,1, i+1,2, i+1,3);

    }

    gmesh.calculate_face_normals();
    gmesh.calculate_vertex_normals();

    gmesh.dump_vsx_mesh(mesh);

    mesh->data->calculate_face_centers();
    mesh_result->set(mesh);
    loading_done = true;
  }
};
