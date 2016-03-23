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



// calculates a rotation from 2 id's into a position and quaternion
class module_mesh_calc_attachment : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* id_a;
  vsx_module_param_float* id_b;

  // out
  vsx_module_param_float3* position;
  vsx_module_param_quaternion* rotation;

  // internal

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;modifiers;pickers;mesh_attach_picker";

    info->description =
      "Builds a matrix/quaternion from 2 vertex id's\nand positions an object there.";

    info->in_param_spec =
      "mesh_in:mesh,"
      "id_a:float,"
      "id_b:float"
    ;

    info->out_param_spec =
      "position:float3,"
      "rotation:quaternion"
    ;

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");

    id_a = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"id_a");
    id_a->set(0.0f);

    id_b = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"id_b");
    id_b->set(0.0f);

    loading_done = true;
    position = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
    rotation = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"rotation");

    set_default_values();
  }

  void set_default_values()
  {
    position->set(0.0f,0);
    position->set(0.0f,1);
    position->set(0.0f,2);
    rotation->set(0.0f,0);
    rotation->set(0.0f,1);
    rotation->set(0.0f,2);
    rotation->set(1.0f,3);
  }

  void run()
  {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p) return;

    long id_a_ = (unsigned long)floor(id_a->get());
    long id_b_ = (unsigned long)floor(id_b->get());
    if (!(*p)->data) return;

    if (id_a_ < 0) id_a_ += (*p)->data->vertices.size();
    if (id_b_ < 0) id_b_ += (*p)->data->vertices.size();

    if (
      (unsigned long)id_a_ < (*p)->data->vertices.size()
      &&
      (unsigned long)id_b_ < (*p)->data->vertices.size()
      &&
      (unsigned long)id_a_ < (*p)->data->vertex_normals.size()
      &&
      (unsigned long)id_b_ < (*p)->data->vertex_normals.size()
      )
    {
      // 1. calculate vector
      vsx_vector3<> k,n,c;
      vsx_matrix<float> m,mr;

      k = (*p)->data->vertices[id_b_] - (*p)->data->vertices[id_a_];
      k.normalize();
      n = (*p)->data->vertex_normals[id_a_];
      n.normalize();

      c.cross(k,n);
      c.normalize();

      k.cross(c,n);
      k.normalize();

      m.m[0] = n.x; m.m[4] = n.y; m.m[8]  = n.z;
      m.m[1] = k.x; m.m[5] = k.y; m.m[9]  = k.z;
      m.m[2] = c.x; m.m[6] = c.y; m.m[10] = c.z;

      mr.assign_inverse(&m);



      //m.rotation_from_vectors(&k, &(p->data->vertex_normals[id_a_]) );
      vsx_quaternion<> q;
      q.from_matrix(&mr);
      q.normalize();
      position->set((*p)->data->vertices[id_a_].x,0);
      position->set((*p)->data->vertices[id_a_].y,1);
      position->set((*p)->data->vertices[id_a_].z,2);
      rotation->set(q.x, 0);
      rotation->set(q.y, 1);
      rotation->set(q.z, 2);
      rotation->set(q.w, 3);
    } else {
      set_default_values();
    }

  }

  void on_delete() {
  }
};
