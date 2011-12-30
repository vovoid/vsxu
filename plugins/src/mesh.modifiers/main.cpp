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
#include <vsx_param.h>
#include <vsx_module.h>
#include <vsx_math_3d.h>
#include <vsx_float_array.h>
#include <vsx_quaternion.h>
#include <pthread.h>

/*
 * MESH DATA REFERENCE
  vsx_array<vsx_vector> vertices;
  vsx_array<vsx_vector> vertex_normals;
  vsx_array<vsx_color> vertex_colors;
  vsx_array<vsx_tex_coord> vertex_tex_coords;
  vsx_array<vsx_face> faces;
  vsx_array<vsx_vector> face_normals;
  vsx_array<vsx_vector> vertex_tangents; // tangent space, for normal mapping
  vsx_array<vsx_vector> face_centers; // centers of the faces - the average of each face's v1+v2+v3/3
 */

// HOW TO:
// When writing a mesh modifier you should consider using poiters and volatile arrays.
// However, this means you have to set the output mesh to mesh_empty (below), otherwise
// the receiver might still have direct pointers to data we don't know is there.
// This small snippet in the beginning of the run() method does the trick:
//     
// if (!p) 
// {
//   mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
//   mesh_out->set_p(mesh_empty);
//   prev_timestamp = 0xFFFFFFFF;
//   return;
// }   


// TODO: optimize the mesh_quat_rotate to also use volatile arrays for speed
// TODO: optimize the inflation mesh modifier to use volatile arrays for passthru arrays
// TODO: add a real spheremapping module
// TODO: quaternion rotation from 2 vertex id's: vector from point to point, normal, crossproduct = matrix -> quaternion
// global static empty mesh
vsx_mesh mesh_empty;

class vsx_module_mesh_dummy : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;dummies;mesh_dummy";
    info->description = "dummy for mesh (useful in macros)";
    info->in_param_spec = "mesh_in:mesh";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    loading_done = true;
  }

  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (p)
    {
      mesh_out->set_p(*p);
    }
  }
};

class vsx_module_mesh_vertex_picker : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* id;
	// out
	vsx_module_param_float3* vertex;
	vsx_module_param_float3* normal;
	vsx_module_param_float4* color;
	vsx_module_param_float3* texcoords;
	vsx_module_param_mesh* passthru;
	// internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;pickers;mesh_vertex_picker";
    info->description = "Picks out a vertex from a mesh";
    info->in_param_spec = "mesh_in:mesh,id:float";
    info->out_param_spec = "vertex:float3,normal:float3,color:float4,texcoords:float3,passthru:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    passthru = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"passthru");
    id = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"id");
    id->set(0.0f);
    loading_done = true;
    vertex = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"vertex");
    normal = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"normal");

    color = (vsx_module_param_float4*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
    texcoords = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"texcoords");
    set_default_values();
  }

  void set_default_values() {
    vertex->set(0.0f,0);
    vertex->set(0.0f,1);
    vertex->set(0.0f,2);
    normal->set(0.0f,0);
    normal->set(0.0f,1);
    normal->set(0.0f,2);
    color->set(0.0f,0);
    color->set(0.0f,1);
    color->set(0.0f,2);
    color->set(0.0f,3);
    texcoords->set(0.0f,0);
    texcoords->set(0.0f,1);
    texcoords->set(0.0f,2);
  }

  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (p) {
      passthru->set(*p);
      long id_ = (unsigned long)floor(id->get());
      if (!(*p)->data) return;

      if (id_ < 0) id_ += (*p)->data->vertices.size();

      if ((unsigned long)id_ < (*p)->data->vertices.size())
      {
        vertex->set((*p)->data->vertices[id_].x,0);
        vertex->set((*p)->data->vertices[id_].y,1);
        vertex->set((*p)->data->vertices[id_].z,2);

        if ((*p)->data->vertex_normals.size() > (unsigned long)id_)
        {
          //printf("id: %d\n",(int)id_);
          normal->set((*p)->data->vertex_normals[id_].x,0);
          normal->set((*p)->data->vertex_normals[id_].y,1);
          normal->set((*p)->data->vertex_normals[id_].z,2);
        }
        if ((*p)->data->vertex_colors.size() > (unsigned long)id_)
        {
          color->set((*p)->data->vertex_colors[id_].x,0);
          color->set((*p)->data->vertex_colors[id_].y,1);
          color->set((*p)->data->vertex_colors[id_].z,2);
          color->set((*p)->data->vertex_colors[id_].z,3);
        }

        if ((*p)->data->vertex_tex_coords.size() > (unsigned long)id_)
        {
          texcoords->set((*p)->data->vertex_tex_coords[id_].s,0);
          texcoords->set((*p)->data->vertex_tex_coords[id_].t,1);
        }
      } else {
        set_default_values();
      }
    }
  }
};

class vsx_module_mesh_to_float3_arrays : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  // out
  vsx_module_param_float3_array* vertices;
  vsx_module_param_float3_array* vertex_normals;
  vsx_module_param_float3_array* vertex_tangents;
  vsx_module_param_float3_array* face_normals;
  vsx_module_param_float3_array* face_centers;
  // internal
  vsx_array<vsx_vector> int_empty_array;
  vsx_float3_array int_vertices;
  vsx_float3_array int_vertex_normals;
  vsx_float3_array int_vertex_tangents;
  vsx_float3_array int_face_normals;
  vsx_float3_array int_face_centers;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;converters;mesh_to_float3_arrays";
    info->description = "Pick out the vector data from a mesh";
    info->in_param_spec = "mesh_in:mesh";
    info->out_param_spec = "vertices:float3_array,"
      "vertex_normals:float3_array,"
//      "vertex_tangents:float3_array,"
      "face_normals:float3_array,"
      "face_centers:float3_array";
    info->component_class = "mesh";
  }
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    vertices = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "vertices");
    vertex_normals = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "vertex_normals");
    //vertex_tangents = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "vertex_tangents");
    face_normals = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "face_normals");
    face_centers = (vsx_module_param_float3_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3_ARRAY, "face_centers");

    int_vertices.data = &int_empty_array;
    vertices->set_p(int_vertices);

    int_vertex_normals.data = &int_empty_array;
    vertex_normals->set_p(int_vertex_normals);

    //int_vertex_tangents.data = &int_empty_array;
    //vertex_tangents->set_p(int_vertex_tangents);

    int_face_normals.data = &int_empty_array;
    face_normals->set_p(int_face_normals);

    int_face_centers.data = &int_empty_array;
    face_centers->set_p(int_face_centers);
  }
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (p)
    {
      int_vertices.data = &((*p)->data->vertices);
      vertices->set_p(int_vertices);

      int_vertex_normals.data = &((*p)->data->vertex_normals);
      vertex_normals->set_p(int_vertex_normals);

      //int_vertex_tangents.data = &((*p)->data->vertex_tangents);
      //vertex_tangents->set_p(int_vertex_tangents);

      int_face_normals.data = &((*p)->data->face_normals);
      face_normals->set_p(int_face_normals);

      int_face_normals.data = &((*p)->data->face_normals);
      face_normals->set_p(int_face_normals);

      int_face_centers.data = &((*p)->data->face_centers);
      face_centers->set_p(int_face_centers);
    }
  }
};




// calculates a rotation from 2 id's into a position and quaternion
class vsx_module_mesh_calc_attachment : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* id_a;
  vsx_module_param_float* id_b;
  // out
  vsx_module_param_float3* position;
  vsx_module_param_quaternion* rotation;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;pickers;mesh_attach_picker";
    info->description = "Builds a matrix/quaternion from 2 vertex id's\nand positions an object there.";
    info->in_param_spec = "mesh_in:mesh,"
    "id_a:float,"
    "id_b:float";
    info->out_param_spec = "position:float3,"
    "rotation:quaternion";
    info->component_class = "mesh";
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

  void set_default_values() {
    position->set(0.0f,0);
    position->set(0.0f,1);
    position->set(0.0f,2);
    rotation->set(0.0f,0);
    rotation->set(0.0f,1);
    rotation->set(0.0f,2);
    rotation->set(1.0f,3);
  }

  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (p) {
      
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
        vsx_vector k,n,c;
        vsx_matrix m,mr;

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
        vsx_quaternion q;
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
  }

  void on_delete() {
  }
};




class vsx_module_mesh_quat_rotate : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_quaternion* quat_in;
  vsx_module_param_int* invert_rotation;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;
  vsx_quaternion q;
public:

  bool init() {
    mesh = new vsx_mesh;
    return true;
  }
  
  void on_delete()
  {
    delete mesh;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;transforms;mesh_rotate_quat";
    info->description = "Rotates mesh by Quaternion";
    info->in_param_spec = "mesh_in:mesh,quat_in:quaternion,invert_rotation:enum?no|yes";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    quat_in = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_in");
    quat_in->set(0.0,0);
    quat_in->set(0.0,1);
    quat_in->set(0.0,2);
    quat_in->set(1.0,3);
    invert_rotation = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "invert_rotation");
    invert_rotation->set(0);
    //id = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"id");
    //id->set(0.0f);
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }
  unsigned long prev_timestamp;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      q.x = quat_in->get(0);
      q.y = quat_in->get(1);
      q.z = quat_in->get(2);
      q.w = quat_in->get(3);
      q.normalize();
      vsx_matrix mat;
      if ( invert_rotation->get())
      {
        vsx_matrix mat2 = q.matrix();
        mat.assign_inverse(&mat2);
      } else
      {
        mat = q.matrix();
      }
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
      {
        mesh->data->vertices[i] = mat.multiply_vector((*p)->data->vertices[i]);
      }
      for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++)
      {
        mesh->data->vertex_normals[i] = mat.multiply_vector((*p)->data->vertex_normals[i]);
      }
/*
      vsx_array<vsx_vector> vertices;
      vsx_array<vsx_vector> vertex_normals;
      vsx_array<vsx_color> vertex_colors;
      vsx_array<vsx_tex_coord> vertex_tex_coords;
      vsx_array<vsx_face> faces;
*/
      for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];
      mesh->timestamp++;
      mesh_out->set_p(mesh);
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      param_updates = 0;
    }
  }
};

class vsx_module_mesh_quat_rotate_around_vertex : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_quaternion* quat_in;
  vsx_module_param_float* vertex_rot_id;
  vsx_module_param_float3* offset_pos;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;
  vsx_quaternion q;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;transforms;mesh_rotate_quat_around_vertex";
    info->description = "Rotates mesh around vertex id by Quaternion";
    info->in_param_spec = "mesh_in:mesh,quat_in:quaternion,vertex_rot_id:float,offset_pos:float3";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    quat_in = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "quat_in");
    quat_in->set(0.0,0);
    quat_in->set(0.0,1);
    quat_in->set(0.0,2);
    quat_in->set(1.0,3);
    vertex_rot_id = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "vertex_rot_id");
    offset_pos = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "offset_pos");
    //id = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"id");
    //id->set(0.0f);
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    
    prev_timestamp = 0xFFFFFFFF;
    //mesh->data->vertices.reset_used(0);
    //mesh->data->vertex_normals.reset_used(0);
  }
  unsigned long prev_timestamp;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (!p) 
    {
      mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
      mesh_out->set(&mesh_empty);
      prev_timestamp = 0xFFFFFFFF;
      return;
    }   
    
    if (p && (param_updates || prev_timestamp != (*p)->timestamp) && (*p)->data->vertices.size()) {
      q.x = quat_in->get(0);
      q.y = quat_in->get(1);
      q.z = quat_in->get(2);
      q.w = quat_in->get(3);
      float len = (float)(sqrt(
        q.x*q.x +
        q.y*q.y +
        q.z*q.z +
        q.w*q.w
      ));
      if (len != 0.0f)
      {
        len = 1.0f / len;
        q.x *= len;
        q.y *= len;
        q.z *= len;
        q.w *= len;
      }
      vsx_matrix mat = q.matrix();


      vsx_vector neg_vec;
      int n_i = (int)floor( vertex_rot_id->get() ) % (*p)->data->vertices.size();
      neg_vec = (*p)->data->vertices[n_i];
      vsx_vector ofs_vec;
      ofs_vec.x = offset_pos->get(0);
      ofs_vec.y = offset_pos->get(1);
      ofs_vec.z = offset_pos->get(2);
      ofs_vec += neg_vec;

      //mesh->data->vertices.reset_used(0);
      //mesh->data->vertex_normals.reset_used(0);

      unsigned long end = (*p)->data->vertices.size();
      vsx_vector* vs_p = &(*p)->data->vertices[0];//.get_pointer();
      mesh->data->vertices.allocate(end);
      mesh->data->vertices.reset_used(end);
      vsx_vector* vs_d = mesh->data->vertices.get_pointer();



      for (unsigned long i = 0; i < end; i++)
      {
        //(*vs_d).multiply_matrix_other_vec(&mat.m[0],*vs_p - neg_vec);// = mat.multiply_vector((*p)->data->vertices[i]);
        //(*vs_d) += ofs_vec;
        //vs_p++;
        //vs_d++;
        //mesh->data->vertices[i] = mat.multiply_vector((*p)->data->vertices[i]);
        //mesh->data->vertices[i].multiply_matrix_other_vec(&mat.m[0],(*p)->data->vertices[i] - neg_vec);// = mat.multiply_vector((*p)->data->vertices[i]);
        //mesh->data->vertices[i] += ofs_vec;

        vs_d[i].multiply_matrix_other_vec(&mat.m[0],vs_p[i] - neg_vec);// = mat.multiply_vector((*p)->data->vertices[i]);
        vs_d[i] += ofs_vec;
      }

      end = (*p)->data->vertex_normals.size();
      mesh->data->vertex_normals.allocate(end);
      mesh->data->vertex_normals.reset_used(end);
      vs_d = mesh->data->vertex_normals.get_pointer();
      vs_p = (*p)->data->vertex_normals.get_pointer();

      for (unsigned long i = 0; i < end; i++)
      {
        //(*vs_d).multiply_matrix_other_vec(&mat.m[0],*vs_p);
        //mesh->data->vertex_normals[i].multiply_matrix_other_vec(&mat.m[0],(*p)->data->vertex_normals[i]);// = mat.multiply_vector((*p)->data->vertex_normals[i]);
        vs_d[i].multiply_matrix_other_vec(&mat.m[0],vs_p[i]);// = mat.multiply_vector((*p)->data->vertex_normals[i]);
        //mesh->data->vertex_normals[i] = mat.multiply_vector((*p)->data->vertex_normals[i]);
//        vs_p++;
        //vs_d++;
      }
/*
      vsx_array<vsx_vector> vertices;
      vsx_array<vsx_vector> vertex_normals;
      vsx_array<vsx_color> vertex_colors;
      vsx_array<vsx_tex_coord> vertex_tex_coords;
      vsx_array<vsx_face> faces;
*/
      if (prev_timestamp != (*p)->timestamp)
      {
        mesh->data->vertex_tex_coords.set_volatile();
        mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

        mesh->data->vertex_tangents.set_volatile();
        mesh->data->vertex_tangents.set_data((*p)->data->vertex_tangents.get_pointer(), (*p)->data->vertex_tangents.size());

        mesh->data->vertex_colors.set_volatile();
        mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

        mesh->data->faces.set_volatile();
        mesh->data->faces.set_data((*p)->data->faces.get_pointer(), (*p)->data->faces.size());
      }
      mesh->timestamp++;
      mesh_out->set_p(mesh);
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      prev_timestamp = (*p)->timestamp;
      param_updates = 0;
    }
  }
};

class vsx_module_mesh_translate : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* translation;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;transforms;mesh_translate";
    info->description = "Translates/moves mesh";
    info->in_param_spec = "mesh_in:mesh,translation:float3";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    translation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "translation");
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }
  unsigned long prev_timestamp;
  vsx_vector v;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (!p) 
    {
      mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
      mesh_out->set(&mesh_empty);
      prev_timestamp = 0xFFFFFFFF;
      return;
    }   

    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      v.x = translation->get(0);
      v.y = translation->get(1);
      v.z = translation->get(2);
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      unsigned long end = (*p)->data->vertices.size();
      vsx_vector* vs_p = &(*p)->data->vertices[0];//.get_pointer();
      mesh->data->vertices.allocate(end);
      mesh->data->vertices.reset_used(end);
      vsx_vector* vs_d = mesh->data->vertices.get_pointer();


      for (unsigned int i = 0; i < end; i++)
      {
        //mesh->data->vertices[i] = (*p)->data->vertices[i] + v;
        vs_d[i] = vs_p[i] + v;
      }
/*
      vsx_array<vsx_vector> vertices;
      vsx_array<vsx_vector> vertex_normals;
      vsx_array<vsx_color> vertex_colors;
      vsx_array<vsx_tex_coord> vertex_tex_coords;
      vsx_array<vsx_face> faces;
*/

      //if (prev_timestamp != (*p)->timestamp)
      //{
      mesh->data->vertex_normals.set_volatile();
      mesh->data->vertex_normals.set_data((*p)->data->vertex_normals.get_pointer(), (*p)->data->vertex_normals.size());

      mesh->data->vertex_tex_coords.set_volatile();
      mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

      mesh->data->vertex_tangents.set_volatile();
      mesh->data->vertex_tangents.set_data((*p)->data->vertex_tangents.get_pointer(), (*p)->data->vertex_tangents.size());

      mesh->data->vertex_colors.set_volatile();
      mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

      mesh->data->faces.set_volatile();
      mesh->data->faces.set_data((*p)->data->faces.get_pointer(), (*p)->data->faces.size());
//      }


      //for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      //for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];
      mesh->timestamp++;
      mesh_out->set_p(mesh);
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      param_updates = 0;
    }
  }
};

class vsx_module_mesh_scale : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* scale;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;transforms;mesh_scale";
    info->description = "Scales mesh";
    info->in_param_spec = "mesh_in:mesh,scale:float3";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    scale = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "scale");
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }
  unsigned long prev_timestamp;
  vsx_vector v;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (!p) 
    {
      mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
      mesh_out->set(&mesh_empty);
      prev_timestamp = 0xFFFFFFFF;
      return;
    }   
    
    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      v.x = scale->get(0);
      v.y = scale->get(1);
      v.z = scale->get(2);
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      unsigned long end = (*p)->data->vertices.size();
      vsx_vector* vs_p = &(*p)->data->vertices[0];
      mesh->data->vertices.allocate(end);
      mesh->data->vertices.reset_used(end);
      vsx_vector* vs_d = mesh->data->vertices.get_pointer();


      for (unsigned int i = 0; i < end; i++)
      {
        //mesh->data->vertices[i] = (*p)->data->vertices[i] + v;
        vs_d[i] = vs_p[i] * v;
      }
/*
      vsx_array<vsx_vector> vertices;
      vsx_array<vsx_vector> vertex_normals;
      vsx_array<vsx_color> vertex_colors;
      vsx_array<vsx_tex_coord> vertex_tex_coords;
      vsx_array<vsx_face> faces;
*/

      //if (prev_timestamp != (*p)->timestamp)
      //{
      mesh->data->vertex_normals.set_volatile();
      mesh->data->vertex_normals.set_data((*p)->data->vertex_normals.get_pointer(), (*p)->data->vertex_normals.size());

      mesh->data->vertex_tex_coords.set_volatile();
      mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

      mesh->data->vertex_tangents.set_volatile();
      mesh->data->vertex_tangents.set_data((*p)->data->vertex_tangents.get_pointer(), (*p)->data->vertex_tangents.size());

      mesh->data->vertex_colors.set_volatile();
      mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

      mesh->data->faces.set_volatile();
      mesh->data->faces.set_data((*p)->data->faces.get_pointer(), (*p)->data->faces.size());
//      }


      //for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      //for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];
      mesh->timestamp++;
      mesh_out->set_p(mesh);
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      param_updates = 0;
    }
  }
};


class vsx_module_mesh_translate_edge_wraparound : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* translation;
  vsx_module_param_float3* edge_min;
  vsx_module_param_float3* edge_max;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;transforms;mesh_translate_edge_wraparound";
    info->description = "Translates/moves mesh";
    info->in_param_spec = "mesh_in:mesh,translation:float3,edge_min:float3,edge_max:float3";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    translation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "translation");
    edge_min = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "edge_min");
    edge_min->set(-1.0f, 0);
    edge_min->set(-1.0f, 1);
    edge_min->set(-1.0f, 2);
    edge_max = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "edge_max");
    edge_max->set(1.0f, 0);
    edge_max->set(1.0f, 1);
    edge_max->set(1.0f, 2);
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }
  unsigned long prev_timestamp;
  vsx_vector v;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (!p)
    {
      mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
      mesh_out->set(&mesh_empty);
      prev_timestamp = 0xFFFFFFFF;
      return;
    }

    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      v.x = translation->get(0);
      v.y = translation->get(1);
      v.z = translation->get(2);
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      unsigned long end = (*p)->data->vertices.size();
      vsx_vector* vs_p = &(*p)->data->vertices[0];//.get_pointer();
      mesh->data->vertices.allocate(end);
      mesh->data->vertices.reset_used(end);
      vsx_vector* vs_d = mesh->data->vertices.get_pointer();

      float eminx = edge_min->get(0);
      float eminy = edge_min->get(1);
      float eminz = edge_min->get(2);
      
      float emaxx = edge_max->get(0);
      float emaxy = edge_max->get(1);
      float emaxz = edge_max->get(2);

      

      for (unsigned int i = 0; i < end; i++)
      {
        vs_d[i] = vs_p[i] + v;
        if (vs_d[i].x > emaxx) vs_d[i].x = eminx + fmod(vs_d[i].x,emaxx);
        if (vs_d[i].y > emaxy) vs_d[i].y = eminy + fmod(vs_d[i].y,emaxy);
        if (vs_d[i].z > emaxz) vs_d[i].z = eminz + fmod(vs_d[i].z,emaxz);
      }
/*
      vsx_array<vsx_vector> vertices;
      vsx_array<vsx_vector> vertex_normals;
      vsx_array<vsx_color> vertex_colors;
      vsx_array<vsx_tex_coord> vertex_tex_coords;
      vsx_array<vsx_face> faces;
*/

      //if (prev_timestamp != (*p)->timestamp)
      //{
      mesh->data->vertex_normals.set_volatile();
      mesh->data->vertex_normals.set_data((*p)->data->vertex_normals.get_pointer(), (*p)->data->vertex_normals.size());

      mesh->data->vertex_tex_coords.set_volatile();
      mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

      mesh->data->vertex_tangents.set_volatile();
      mesh->data->vertex_tangents.set_data((*p)->data->vertex_tangents.get_pointer(), (*p)->data->vertex_tangents.size());

      mesh->data->vertex_colors.set_volatile();
      mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

      mesh->data->faces.set_volatile();
      mesh->data->faces.set_data((*p)->data->faces.get_pointer(), (*p)->data->faces.size());
//      }


      //for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      //for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];
      mesh->timestamp++;
      mesh_out->set_p(mesh);
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      param_updates = 0;
    }
  }
};

class vsx_module_mesh_noise : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* noise_amount;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;

  vsx_avector<vsx_vector> random_distort_points;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;deformers;mesh_noise";
    info->description = "Trashes / applies noise to mesh";
    info->in_param_spec = "mesh_in:mesh,noise_amount:float3";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    noise_amount = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "noise_amount");
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }
  unsigned long prev_timestamp;
  vsx_vector v;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      v.x = noise_amount->get(0);
      v.y = noise_amount->get(1);
      v.z = noise_amount->get(2);
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      if (random_distort_points.size() != (*p)->data->faces.size())
      {
        // inefficient, yes, but meshes don't change datasets that often..
        for (size_t i = 0; i < (*p)->data->faces.size(); i++)
        {
          random_distort_points[i].x = rand()%1000 * 0.001-0.5f;
          random_distort_points[i].y = rand()%1000 * 0.001-0.5f;
          random_distort_points[i].z = rand()%1000 * 0.001-0.5f;
          // thought of normalizing here but we'll do that later so doesn't matter really
        }
      }

      // we need to decouple the faces
      size_t i_vertex_iter = 0;
      size_t i_face_iter = 0;
      for (size_t face_iterator = 0; face_iterator < (*p)->data->faces.size(); face_iterator++)
      {
        mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].a] + random_distort_points[face_iterator] * v;
        mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].a];
        mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].a];
        mesh->data->faces[i_face_iter].a = i_vertex_iter;
        i_vertex_iter++;
        mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].b] + random_distort_points[face_iterator] * v;
        mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].b];
        mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].b];
        mesh->data->faces[i_face_iter].b = i_vertex_iter;
        i_vertex_iter++;
        mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].c] + random_distort_points[face_iterator] * v;
        mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].c];
        mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].c];
        mesh->data->faces[i_face_iter].c = i_vertex_iter;
        i_vertex_iter++;
        i_face_iter++;
      }

      //for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
      //{
      //  mesh->data->vertices[i] = (*p)->data->vertices[i] + v;
      //}
/*
      vsx_array<vsx_vector> vertices;
      vsx_array<vsx_vector> vertex_normals;
      vsx_array<vsx_color> vertex_colors;
      vsx_array<vsx_tex_coord> vertex_tex_coords;
      vsx_array<vsx_face> faces;
*/
      //for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      //for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      //for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];
      mesh->timestamp++;
      mesh_out->set_p(mesh);
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      param_updates = 0;
    }
  }
};

class vsx_module_mesh_rain_down : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* start;
  vsx_module_param_float* floor_level;
  vsx_module_param_float* explosion_factor;
  vsx_module_param_float* landing_fluffiness;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;
  unsigned long prev_timestamp;
  vsx_vector v;
  float prev_start;
  vsx_array<float> vertex_weight_array;
  vsx_array<float> vertex_explosion_array_x;
  vsx_array<float> vertex_explosion_array_z;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;deformers;mesh_rain_down";
    info->description = "Slowly rains the mesh down on the floor";
    info->in_param_spec = "mesh_in:mesh,start:float,floor_level:float,explosion_factor:float,landing_fluffiness:float";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    start = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "start");
    start->set(0.0f);
    landing_fluffiness = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "landing_fluffiness");
    landing_fluffiness->set(1.0f);
    explosion_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "explosion_factor");
    explosion_factor->set(0.0f);
    floor_level = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "floor_level");
    floor_level->set(-1.0f);
    prev_start = -1.0f;
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }

  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (!p) 
    {
      mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
      mesh_out->set(&mesh_empty);
      prev_timestamp = 0xFFFFFFFF;
      return;
    }   
    if (((start->get() > 0.0f))) {
      if (prev_start <= 0.0f) // should cover all instances
      {
        // splice the mesh into faces for our personal use.
        // we need to decouple the faces
        size_t i_vertex_iter = 0;
        size_t i_face_iter = 0;
        size_t i_vertex_weight_iter = 0;
        mesh->data->vertices.unset_volatile();
        mesh->data->vertex_normals.unset_volatile();
        mesh->data->vertex_tex_coords.unset_volatile();
        mesh->data->faces.unset_volatile();
        for (size_t face_iterator = 0; face_iterator < (*p)->data->faces.size(); face_iterator++)
        {
          vsx_vector a,b,c,ab,ac;
          mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].a];
          a = mesh->data->vertices[i_vertex_iter];
          mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].a];
          mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].a];
          mesh->data->faces[i_face_iter].a = i_vertex_iter;
          i_vertex_iter++;
          mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].b];
          b = mesh->data->vertices[i_vertex_iter];
          mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].b];
          mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].b];
          mesh->data->faces[i_face_iter].b = i_vertex_iter;
          i_vertex_iter++;
          mesh->data->vertices[i_vertex_iter] = (*p)->data->vertices[(*p)->data->faces[face_iterator].c];
          c = mesh->data->vertices[i_vertex_iter];
          mesh->data->vertex_normals[i_vertex_iter] = (*p)->data->vertex_normals[(*p)->data->faces[face_iterator].c];
          mesh->data->vertex_tex_coords[i_vertex_iter] = (*p)->data->vertex_tex_coords[(*p)->data->faces[face_iterator].c];
          mesh->data->faces[i_face_iter].c = i_vertex_iter;
          ab = b-a;
          ac = c-a;
          a.cross(ab,ac);
          float weight=pow(a.length(),0.2f);
          //weight = pow(weight, 5.0f)*5.0f;
          vsx_vector exp_dist;

          exp_dist.x = (float)(rand()%1000) * 0.001f-0.5f;
          exp_dist.z = (float)(rand()%1000) * 0.001f-0.5f;
          exp_dist.normalize();
          exp_dist *= (float)(rand()%1000) * 0.001f-0.5f;
          float explosion_x = exp_dist.x;
          float explosion_z = exp_dist.z;
          vertex_weight_array[i_vertex_weight_iter] = weight;
          vertex_explosion_array_x[i_vertex_weight_iter] = explosion_x;
          vertex_explosion_array_z[i_vertex_weight_iter] = explosion_z;
          i_vertex_weight_iter++;
          vertex_weight_array[i_vertex_weight_iter] = weight;
          vertex_explosion_array_x[i_vertex_weight_iter] = explosion_x;
          vertex_explosion_array_z[i_vertex_weight_iter] = explosion_z;
          i_vertex_weight_iter++;
          vertex_weight_array[i_vertex_weight_iter] = weight;
          vertex_explosion_array_x[i_vertex_weight_iter] = explosion_x;
          vertex_explosion_array_z[i_vertex_weight_iter] = explosion_z;
          i_vertex_weight_iter++;

          i_vertex_iter++;
          i_face_iter++;
        }
      }
      vsx_vector* vp = mesh->data->vertices.get_pointer();
      float* v_weight_p = vertex_weight_array.get_pointer();
      float* v_ex_p = vertex_explosion_array_x.get_pointer();
      float* v_ez_p = vertex_explosion_array_z.get_pointer();
      float dtx = engine->dtime;
      float speed = start->get()*2.0f;
      float spdtx = speed * dtx;
      float spdtx_exp = spdtx*explosion_factor->get();
      float fluffiness = landing_fluffiness->get()*10.0f;
      float fl = floor_level->get();
      for (size_t i = 0; i < mesh->data->vertices.size(); i++)
      {
        float floor = fl-(*v_weight_p) * fluffiness;
        (*vp).y += spdtx * ( (floor - (*vp).y * (*v_weight_p)));

        //(*pp).speed.y += ay*((cy - (*pp).pos.y) * orig_size);

        if ((*vp).y < floor) (*vp).y = floor; else
        {
          (*vp).x += (*v_ex_p)*spdtx_exp;
          (*vp).z += (*v_ez_p)*spdtx_exp;
          (*v_ex_p)*=1.0f-0.01f*spdtx;
          (*v_ez_p)*=1.0f-0.01f*spdtx;
        }
        vp++;
        v_weight_p++;
        v_ex_p++;
        v_ez_p++;
      }
      mesh->timestamp++;
      param_updates = 0;
    } else
    {
      mesh->data->vertices.set_volatile();
      mesh->data->vertices.set_data((*p)->data->vertices.get_pointer(), (*p)->data->vertices.size());

      mesh->data->vertex_normals.set_volatile();
      mesh->data->vertex_normals.set_data((*p)->data->vertex_normals.get_pointer(), (*p)->data->vertex_normals.size());

      mesh->data->vertex_tex_coords.set_volatile();
      mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

      mesh->data->vertex_tangents.set_volatile();
      mesh->data->vertex_tangents.set_data((*p)->data->vertex_tangents.get_pointer(), (*p)->data->vertex_tangents.size());

      mesh->data->vertex_colors.set_volatile();
      mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

      mesh->data->faces.set_volatile();
      mesh->data->faces.set_data((*p)->data->faces.get_pointer(), (*p)->data->faces.size());
      mesh->timestamp = (*p)->timestamp;
    }
    mesh_out->set_p(mesh);
    prev_start = start->get();
    prev_timestamp = (*p)->timestamp;
  }
};

class vsx_module_mesh_deformers_random_normal_distort : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* distortion_factor;
  vsx_module_param_int* distort_normals;
  vsx_module_param_int* distort_vertices;
  vsx_module_param_float* vertex_distortion_factor;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;
  vsx_array<vsx_vector> normals_dist_array;

public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;deformers;mesh_normal_randistort";
    info->description = "Distorts the normals of the mesh, useful for crapping up the lighting of a mesh";
    info->in_param_spec =
      "mesh_in:mesh,"
      "distortion_factor:float3,"
      "distort_normals:enum?no|yes,"
      "distort_vertices:enum?no|yes,"
      "vertex_distortion_factor:float"
      ;
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    distortion_factor = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "distortion_factor");
    loading_done = true;
    distort_normals = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "distort_normals");
    distort_normals->set(1);
    distort_vertices = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "distort_vertices");
    distort_vertices->set(0);
    vertex_distortion_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "vertex_distortion_factor");
    vertex_distortion_factor->set(1.0f);
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    prev_timestamp = 0xFFFFFFFF;
  }
  unsigned long int prev_timestamp;
  vsx_vector v, v_;
  float vertex_distortion_factor_;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (!p) 
    {
      mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
      mesh_out->set(&mesh_empty);
      prev_timestamp = 0xFFFFFFFF;
      return;
    }
    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      
      if (normals_dist_array.size() != (*p)->data->vertices.size())
      {
        // inefficient, yes, but meshes don't change datasets that often..
        normals_dist_array.reset_used();
        for (size_t i = 0; i < (*p)->data->vertices.size(); i++)
        {
          normals_dist_array[i].x = rand()%1000 * 0.001;
          normals_dist_array[i].y = rand()%1000 * 0.001;
          normals_dist_array[i].z = rand()%1000 * 0.001;
          // thought of normalizing here but we'll do that later so doesn't matter really
          // IT's A REALLY BAD IDEA TO NORMALIZE HERE!!!!!
          // WHAT WAS I THINKING
        }
      }

      v.x = distortion_factor->get(0);
      v.y = distortion_factor->get(1);
      v.z = distortion_factor->get(2);

      float v1x = 1.0f - v.x;
      float v1y = 1.0f - v.y;
      float v1z = 1.0f - v.z;

      if (
          (
            vertex_distortion_factor->get() == vertex_distortion_factor_
          )
          && 
          (
            v_ == v
          ) 
          && 
          (
            prev_timestamp != 0xFFFFFFFF
          )
          &&
          (
           prev_timestamp == (*p)->timestamp
          )
        ) 
      {
        #ifdef VSXU_DEBUG
        //printf("return at line %d\n", __LINE__);
        #endif
        return;
      }
      #ifdef VSXU_DEBUG
        //printf("running meshdf\n");
      #endif

      v_ = v;
      vertex_distortion_factor_ = vertex_distortion_factor->get();
      //mesh->data->vertices.reset_used(0);
      //mesh->data->vertex_normals.reset_used(0);
      //mesh->data->vertex_tex_coords.reset_used(0);
      //mesh->data->vertex_colors.reset_used(0);
      //mesh->data->faces.reset_used(0);

      //for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++) { mesh->data->vertices[i] = (*p)->data->vertices[i]; }
/*
      vsx_array<vsx_vector> vertices;
      vsx_array<vsx_vector> vertex_normals;
      vsx_array<vsx_color> vertex_colors;
      vsx_array<vsx_tex_coord> vertex_tex_coords;
      vsx_array<vsx_face> faces;
*/
      bool normal_transform_enabled = false;
      bool vertex_transform_enabled = false;

      if (1 == distort_normals->get())
      {
        vsx_vector* ndap = normals_dist_array.get_pointer();
        vsx_vector* vnp = (*p)->data->vertex_normals.get_pointer();
        mesh->data->vertex_normals.unset_volatile();
        mesh->data->vertex_normals.allocate( (*p)->data->vertex_normals.size() );
        vsx_vector* vnd = mesh->data->vertex_normals.get_pointer();
        for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++)
        {
          // linear interpolation here
          vnd->x = ndap->x * v.x + v1x * vnp->x;
          vnd->y = ndap->y * v.y + v1y * vnp->y;
          vnd->z = ndap->z * v.z + v1z * vnp->z;
          vnd->normalize();
          vnp++;
          vnd++;
          ndap++;
        }
        if ((*p)->data->vertex_normals.size())
          normal_transform_enabled = true;
      }

      if (1 == distort_vertices->get())
      {
        v *= vertex_distortion_factor->get();
        vsx_vector* ndap = normals_dist_array.get_pointer();
        vsx_vector* vp = (*p)->data->vertices.get_pointer();
        mesh->data->vertices.unset_volatile();
        mesh->data->vertices.allocate( (*p)->data->vertices.size() );
        vsx_vector* vd = mesh->data->vertices.get_pointer();

        for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
        {
          // similar linear interpolation here
          vd->x = ndap->x * v.x + vp->x;
          vd->y = ndap->y * v.y + vp->y;
          vd->z = ndap->z * v.z + vp->z;
          vp++;
          vd++;
          ndap++;
        }
        
        if ((*p)->data->vertices.size())
          vertex_transform_enabled = true;
      }
      {
        if (!vertex_transform_enabled)
        {
          mesh->data->vertices.set_volatile();
          mesh->data->vertices.set_data((*p)->data->vertices.get_pointer(), (*p)->data->vertices.size());
        }
        if (!normal_transform_enabled)
        {
          mesh->data->vertex_normals.set_volatile();
          mesh->data->vertex_normals.set_data((*p)->data->vertex_normals.get_pointer(), (*p)->data->vertex_normals.size());
        }

        mesh->data->vertex_tex_coords.set_volatile();
        mesh->data->vertex_tex_coords.set_data((*p)->data->vertex_tex_coords.get_pointer(), (*p)->data->vertex_tex_coords.size());

        mesh->data->vertex_tangents.set_volatile();
        mesh->data->vertex_tangents.set_data((*p)->data->vertex_tangents.get_pointer(), (*p)->data->vertex_tangents.size());

        mesh->data->vertex_colors.set_volatile();
        mesh->data->vertex_colors.set_data((*p)->data->vertex_colors.get_pointer(), (*p)->data->vertex_colors.size());

        mesh->data->faces.set_volatile();
        mesh->data->faces.set_data((*p)->data->faces.get_pointer(), (*p)->data->faces.size());
      }
      
      mesh->timestamp++;
      mesh_out->set(mesh);
      prev_timestamp = (*p)->timestamp;
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      param_updates = 0;
    } 
  }
};

class vsx_module_mesh_compute_tangents : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  // out
  vsx_module_param_quaternion_array* tangents;
  // internal
  vsx_mesh* mesh;
  vsx_quaternion_array i_tangents;
  vsx_array<vsx_quaternion> data;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;helpers;mesh_compute_tangents";
    info->description = "Computes tangent space for the mesh";
    info->in_param_spec = "mesh_in:mesh";
    info->out_param_spec = "tangents:quaternion_array";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    loading_done = true;
    tangents = (vsx_module_param_quaternion_array*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION_ARRAY,"tangents");
    i_tangents.data = &data;
    tangents->set_p(i_tangents);
  }
  unsigned long prev_timestamp;
  vsx_vector v;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (p && (prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;


      if ((*p)->data->vertex_tangents.size())
      {
        i_tangents.data = &(*p)->data->vertex_tangents;
      }
      else
      {
        //printf("calculating tangents\n");
      //void CalculateTangentArray(long vertexCount, const Point3D *vertex, const Vector3D *normal,
              //const Point2D *texcoord, long triangleCount, const Triangle *triangle, Vector4D *tangent)
      //{
          //Vector3D *tan1 = new Vector3D[vertexCount * 2];
          //Vector3D *tan2 = tan1 + vertexCount;
//          ClearMemory(tan1, vertexCount * sizeof(Vector3D) * 2);

      data.allocate((*p)->data->vertices.size());
      data.reset_used((*p)->data->vertices.size());
      data.memory_clear();
      vsx_quaternion* vec_d = data.get_pointer();

/*          for (unsigned long i = 0; i < (*p)->data->vertices.size(); i++) {
            data[i].x = 0;
            data[i].y = 0;
            data[i].z = 0;
          }
*/
          for (unsigned long a = 0; a < (*p)->data->faces.size(); a++)
          {
            long i1 = (*p)->data->faces[a].a;
            long i2 = (*p)->data->faces[a].b;
            long i3 = (*p)->data->faces[a].c;

            const vsx_vector& v1 = (*p)->data->vertices[i1];
            const vsx_vector& v2 = (*p)->data->vertices[i2];
            const vsx_vector& v3 = (*p)->data->vertices[i3];

            const vsx_tex_coord& w1 = (*p)->data->vertex_tex_coords[i1];
            const vsx_tex_coord& w2 = (*p)->data->vertex_tex_coords[i2];
            const vsx_tex_coord& w3 = (*p)->data->vertex_tex_coords[i3];

            float x1 = v2.x - v1.x;
            float x2 = v3.x - v1.x;
            float y1 = v2.y - v1.y;
            float y2 = v3.y - v1.y;
            float z1 = v2.z - v1.z;
            float z2 = v3.z - v1.z;

            float s1 = w2.s - w1.s;
            float s2 = w3.s - w1.s;
            float t1 = w2.t - w1.t;
            float t2 = w3.t - w1.t;

            float r = 1.0f / (s1 * t2 - s2 * t1);
            vsx_quaternion sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
            //vsx_vector sdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,(s1 * z2 - s2 * z1) * r);

            vec_d[i1] += sdir;
            vec_d[i2] += sdir;
            vec_d[i3] += sdir;

            //tan2[i1] += tdir;
            //tan2[i2] += tdir;
            //tan2[i3] += tdir;
          }
          for (unsigned long a = 0; a < data.size(); a++)
          {
              vsx_vector& n = (*p)->data->vertex_normals[a];
              vsx_quaternion& t = vec_d[a];

              // Gram-Schmidt orthogonalize
              //vec_d[a] = (t - n * t.dot_product(&n) );
              vec_d[a] = (t - n * t.dot_product(&n) );
              vec_d[a].normalize();

              // Calculate handedness
              //tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
          }
      }
    }
  }
};

class vsx_module_mesh_deformers_mesh_vertex_move : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  float p_index;
  vsx_module_param_float* index;
  vsx_vector p_offset;
  vsx_module_param_float3* offset;
  float p_falloff_range;
  vsx_module_param_float* falloff_range;
  unsigned long p_timestamp;
  // out
  vsx_module_param_mesh* mesh_out;
  // internal
  vsx_mesh* mesh;
  vsx_quaternion q;
  vsx_avector<unsigned long> moved_vertices;
  vsx_avector<int> vertices_needing_normal_calc;
  float falloff;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;deformers;mesh_vertex_move";
    info->description = "Moves a single vertex with falloff";
    info->in_param_spec = "mesh_in:mesh,index:float,offset:float3,falloff_range:float";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    index = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "index");
    offset = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "offset");
    falloff_range = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "falloff_range");
    falloff_range->set(0.0f);
    //id = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"id");
    //id->set(0.0f);
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }

  int first_index;
  void do_falloff(float i_falloff, unsigned long ind, bool final = false)
  {
    if (ind > mesh->data->vertices.size()) return;
    //printf("i_falloff = %f ind %d", i_falloff, ind);
    float pp;
    if (falloff == 0.0f) pp = 0.0f;
    else
    {


      pp = i_falloff / falloff;
    }
    //printf(" pp = %f\n", pp);

    //float add;
    //if ((falloff+1)-i_falloff > 1.0f) add = 1.0f;
    //else add = (falloff+1)-i_falloff;
    //if (p < 1.0f)
    moved_vertices.push_back(ind);

    if (!final)
    {
      // find all faces connected to the index-vertex
      for (unsigned long i = 0; i < mesh->data->faces.size(); i++)
      {
        if (
            mesh->data->faces[i].a == ind ||
            mesh->data->faces[i].b == ind ||
            mesh->data->faces[i].c == ind)
        {
          unsigned long a = mesh->data->faces[i].a;
          unsigned long b = mesh->data->faces[i].b;
          unsigned long c = mesh->data->faces[i].c;
          // for each face, check all index points - if not already moved, move it and recurse on that index
          bool found = false;
          unsigned long j = 0;
          if (a != ind)
          {
            while (!found && j < moved_vertices.size())
            {
              if (moved_vertices[j] == a) found = true;
              j++;
            }
            if (!found) {
              vsx_vector dist = (*p)->data->vertices[a] - (*p)->data->vertices[first_index];
              float len = dist.length();
              if (len > falloff)
              do_falloff(len, a,true);
              else
              do_falloff(len, a);
            }
          }

          if (b != ind)
          {
            found = false; j = 0;
            while (!found && j < moved_vertices.size())
            {
              if (moved_vertices[j] == b) found = true;
              j++;
            }
            if (!found) {
              vsx_vector dist = (*p)->data->vertices[b] - (*p)->data->vertices[first_index];
              float len = dist.length();
              if (len > falloff)
              do_falloff(len, b,true);
              else
              do_falloff(len, b);
            }
          }

          found = false; j = 0;
          if (c != ind)
          {
            while (!found && j < moved_vertices.size())
            {
              if (moved_vertices[j] == c) found = true;
              j++;
            }
            if (!found) {
              vsx_vector dist = (*p)->data->vertices[c] - (*p)->data->vertices[first_index];
              float len = dist.length();
              if (len > falloff)
              do_falloff(len, c,true);
              else
              do_falloff(len, c);
            }
          }
        } // if ind in face
      }
    }
    if (pp <= 1.0f)
    {
      // find distance between this index and center vertex

      mesh->data->vertices[ind] = (*p)->data->vertices[ind] + vsx_vector(offset->get(0),offset->get(1),offset->get(2))*(1.0f-pp);
      //printf("moved %d\n", ind);
    }
    vertices_needing_normal_calc.push_back(ind);
  }
  vsx_mesh** p;
  void run() {
    p = mesh_in->get_addr();
    if (!p) return;
    bool run = false;
    if (p_index != index->get()) run = true;
    if (!run) if (p_offset != vsx_vector(offset->get(0), offset->get(1), offset->get(2))) run = true;
    if (p_falloff_range != falloff_range->get()) run = true;

    if ((*p)->timestamp != p_timestamp || run) {
      p_timestamp = (*p)->timestamp;
      p_falloff_range = falloff_range->get();
      p_offset = vsx_vector(offset->get(0), offset->get(1), offset->get(2));
      p_index = index->get();
      //mesh->timestamp++;
      //printf("run %d\n",param_updates);
      //param_updates = 0;
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);
      for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
      {
        mesh->data->vertices[i] = (*p)->data->vertices[i];
      }
      for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++)
      {
        mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      }
      //printf("%d\n", __LINE__);
/*
      vsx_array<vsx_vector> vertices;
      vsx_array<vsx_vector> vertex_normals;
      vsx_array<vsx_color> vertex_colors;
      vsx_array<vsx_tex_coord> vertex_tex_coords;
      vsx_array<vsx_face> faces;
*/
      for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];
      //for (int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      unsigned long ind = (int)floor(index->get());
      if (ind > mesh->data->vertices.size()-1) ind = mesh->data->vertices.size()-1;
      // add to this vector now
      //printf("%d\n", __LINE__);
      vertices_needing_normal_calc.clear();
      moved_vertices.clear();
      //printf("%d\n", __LINE__);
      //mesh->data->vertices[ind] = mesh->data->vertices[ind] + vsx_vector(offset->get(0),offset->get(1),offset->get(2));
      //vertices_needing_normal_calc.push_back(ind);
      falloff = falloff_range->get();
      first_index = ind;
      //moved_vertices.push_back(ind);
      //printf("%d\n", __LINE__);
      do_falloff(0.0f, ind);
      //printf("%d\n", __LINE__);
      //bool f_first = true;
      // 3.5
      // 2.5
      // 1.5
      // 0.5
      //printf("%d\n", __LINE__);
      // find all faces that affect this vertex, calculate their normals and add to compound normal
      for (unsigned long k = 0; k < vertices_needing_normal_calc.size(); k++)
      //for (unsigned long indi = 0; indi < mesh->data->vertices.size(); indi++)
      {
        unsigned long indi = vertices_needing_normal_calc[k];
        vsx_vector norm_accum;
        for (unsigned long i = 0; i < mesh->data->faces.size(); i++)
        {
          if (
              mesh->data->faces[i].a == indi ||
              mesh->data->faces[i].b == indi ||
              mesh->data->faces[i].c == indi)
          {
            norm_accum = norm_accum + mesh->data->get_face_normal(i);
          }
        }
        norm_accum.normalize();
        mesh->data->vertex_normals[indi] = norm_accum;
      }
      //printf("%d\n", __LINE__);
      mesh_out->set_p(mesh);
    }
  }
};

// mesh inflation by CoR
// optimized 2010-01 by jaw

#define printVector(a,b,c)
#define printFloat(a,b,c)

class vsx_module_mesh_inflate : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* steps_per_second;
  vsx_module_param_float* step_size;
  vsx_module_param_float* gas_amount;
  vsx_module_param_float* gas_expansion_factor;
  vsx_module_param_float* grid_stiffness_factor;
  vsx_module_param_float* damping_factor;
  vsx_module_param_float* material_weight;
  vsx_module_param_float* lower_boundary;
  // out
  vsx_module_param_mesh* mesh_out;
  vsx_module_param_float* volume_out;
  // internal
  vsx_mesh* mesh;
  vsx_array<vsx_vector> faceLengths;
  vsx_array<float> faceAreas;
  vsx_array<vsx_vector> verticesSpeed;

  bool debug;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
  
  void _printVector(const char* name, int faceInd, vsx_vector vec) {
    if(debug) printf("%d %s: %f %f %f \n", faceInd, name, vec.x, vec.y, vec.z);
  }
  void _printFloat(const char* name, int faceInd, float f) {
    if(debug) printf("%d %s: %f \n", faceInd, name, f);
  }


  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;deformers;mesh_inflate";
    info->description = "Inflates a mesh";
    info->in_param_spec = "mesh_in:mesh,"
              "steps_per_second:float,"
              "step_size:float,"
              "gas_amount:float,"
              "gas_expansion_factor:float,"
              "grid_stiffness_factor:float,"
              "damping_factor:float,"
              "material_weight:float,"
              "lower_boundary:float";

    info->out_param_spec = "mesh_out:mesh,volume_out:float";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");

    steps_per_second = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "steps_per_second");
    step_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_size");
    gas_amount = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "gas_amount");
    gas_expansion_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "gas_expansion_factor");
    grid_stiffness_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "grid_stiffness_factor");
    damping_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "damping_factor");
    material_weight = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "material_weight");
    lower_boundary = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "lower_boundary");


    steps_per_second->set(100.0f);
    step_size->set(0.01f);
    gas_amount->set(1.0f);
    gas_expansion_factor->set(1.0f);
    grid_stiffness_factor->set(1.0f);
    damping_factor->set(0.98f);
    material_weight->set(0.00f);
    lower_boundary->set(-150.00f);


    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    volume_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"volume_out");
    volume_out->set(0.0f);
    prev_timestamp = 0xFFFFFFFF;
  }

  unsigned long prev_timestamp;
  vsx_vector v;
  float dtimeRest;

  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (!p) 
    {
      mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
      mesh_out->set(&mesh_empty);
      prev_timestamp = 0xFFFFFFFF;
      return;
    }

    debug = false;
    bool newMeshLoaded = false;


    //after a mesh change clone the mesh
    if (p && (prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
      {
        mesh->data->vertices[i] = (*p)->data->vertices[i] + v;
        verticesSpeed[i] = vsx_vector(0, 0, 0);
      }

      for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];

      //calc and store original face lengths
      faceLengths.reset_used();
     	vsx_vector normal;
     	vsx_vector len;
     	vsx_vector hypVec;
      for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) {
      	vsx_face& f = mesh->data->faces[i];
      	vsx_vector& v0 = mesh->data->vertices[f.a];
      	vsx_vector& v1 = mesh->data->vertices[f.b];
      	vsx_vector& v2 = mesh->data->vertices[f.c];
      	//calc face area
        normal.assign_face_normal(&v0, &v1, &v2);
        float area = normal.length() / 2.0f;
        faceAreas[i] = area;
        //facelengths a, b, c stored in vector x, y, z
        len.x = (v1 - v0).length();
        len.y = (v2 - v1).length();
        len.z = (v0 - v2).length();
        faceLengths.push_back(len);
      }
      mesh->timestamp++;
      param_updates = 0;

      newMeshLoaded = true;
      dtimeRest = 0.0f;
    }

    float stepSize = step_size->get();
    //float stepsPerSecond = steps_per_second->get();
    float gasExpansionFactor = gas_expansion_factor->get();
    float gridStiffnessFactor = grid_stiffness_factor->get();
    float dampingFactor =  damping_factor->get();
    float materialWeight = material_weight->get();
    float lowerBoundary = lower_boundary->get();

    //use engine->dtime; and dtimeRest
    //to repeat the calculation several times ((dtime + rest) * stepsPerSecond)



    //calculate volume
    float volume = 0.0f;
    vsx_face* face_p = mesh->data->faces.get_pointer();
    vsx_vector* vertex_p = mesh->data->vertices.get_pointer();
    vsx_vector* faces_length_p = faceLengths.get_pointer();

    verticesSpeed.allocate(mesh->data->vertices.size());
    vsx_vector* vertices_speed_p = verticesSpeed.get_pointer();

    float onedivsix = (1.0f / 6.0f);
    for(unsigned int i = 0; i < mesh->data->faces.size(); i++) {
    	vsx_face& f = face_p[i];//mesh->data->faces[i];
    	vsx_vector& v0 = vertex_p[f.a];
    	vsx_vector& v2 = vertex_p[f.b];
    	vsx_vector& v1 = vertex_p[f.c];

      volume += (v0.x * (v1.y - v2.y) +
           v1.x * (v2.y - v0.y) +
           v2.x * (v0.y - v1.y)) * (v0.z + v1.z + v2.z) * onedivsix;

    }

    //default gas_amount to volume of a new mesh i.e. no pressure
    if(newMeshLoaded) {
      gas_amount->set(volume);
    }
    float pressure = (gas_amount->get() - volume) / volume;

    //mesh->data->face_normals.reset_used(0);
    //mesh->data->vertex_normals.reset_used(0);


    //calculate face areas, normals, forces and add to speed
    for(unsigned int i = 0; i < mesh->data->faces.size(); i++) {
    	vsx_face& f = face_p[i];//mesh->data->faces[i];
    	vsx_vector& v0 = vertex_p[f.a];//mesh->data->vertices[f.a];
    	vsx_vector& v1 = vertex_p[f.b];//mesh->data->vertices[f.b];
    	vsx_vector& v2 = vertex_p[f.c];//mesh->data->vertices[f.c];

              printVector("v0", i, v0);
              printVector("v1", i, v1);
              printVector("v2", i, v2);

    	//vsx_vector normal = mesh->data->get_face_normal(i);
      vsx_vector a = vertex_p[face_p[i].b] - vertex_p[face_p[i].a];
      vsx_vector b = vertex_p[face_p[i].c] - vertex_p[face_p[i].a];
      vsx_vector normal;
      normal.cross(a,b);



              printVector("normal", i, normal);

      //float len = normal.length();
      //float area = len / 2;

              printFloat("length", i, len);
              printFloat("area", i, len);

      vsx_vector edgeA = (v1 - v0);
      vsx_vector edgeB = (v2 - v1);
      vsx_vector edgeC = (v0 - v2);

              printVector("edgeA", i, edgeA);
              printVector("edgeB", i, edgeB);
              printVector("edgeC", i, edgeC);

      float lenA = edgeA.length();
      float lenB = edgeB.length();
      float lenC = edgeC.length();

              printFloat("lenA", i, lenA);
              printFloat("lenB", i, lenB);
              printFloat("lenC", i, lenC);


      float edgeForceA = (faces_length_p[i].x - lenA) / faces_length_p[i].x;
      float edgeForceB = (faces_length_p[i].y - lenB) / faces_length_p[i].y;
      float edgeForceC = (faces_length_p[i].z - lenC) / faces_length_p[i].z;

              printFloat("edgeForceA", i, edgeForceA);
              printFloat("edgeForceB", i, edgeForceB);
              printFloat("edgeForceC", i, edgeForceC);

      float edgeAccA = edgeForceA / lenA;
      float edgeAccB = edgeForceB / lenB;
      float edgeAccC = edgeForceC / lenC;

              printFloat("edgeAccA", i, edgeAccA);
              printFloat("edgeAccB", i, edgeAccB);
              printFloat("edgeAccC", i, edgeAccC);

      vsx_vector accA = edgeA * edgeAccA;
      vsx_vector accB = edgeB * edgeAccB;
      vsx_vector accC = edgeC * edgeAccC;

              printVector("accA", i, accA);
              printVector("accB", i, accB);
              printVector("accC", i, accC);

      vertices_speed_p[f.a] -= (accA - accC) * gridStiffnessFactor;
      vertices_speed_p[f.b] -= (accB - accA) * gridStiffnessFactor;
      vertices_speed_p[f.c] -= (accC - accB) * gridStiffnessFactor;

      //applying pressure to areas of faces
      vsx_vector pressureAcc = normal * pressure * gasExpansionFactor;
      vertices_speed_p[f.a] -= pressureAcc;
      vertices_speed_p[f.b] -= pressureAcc;
      vertices_speed_p[f.c] -= pressureAcc;

      //apply material weight
      vertices_speed_p[f.a].y -= materialWeight;
      vertices_speed_p[f.b].y -= materialWeight;
      vertices_speed_p[f.c].y -= materialWeight;

    }

    //apply speeds to vertices
    for(unsigned int i = 0; i < mesh->data->vertices.size(); i++) {
      vertex_p[i] += vertices_speed_p[i] * stepSize;
      if(vertex_p[i].y < lowerBoundary) {
        vertex_p[i].y = lowerBoundary;
      }
      vertices_speed_p[i] = vertices_speed_p[i] * dampingFactor;
    }


    mesh->data->vertex_normals.allocate(mesh->data->vertices.size());
    mesh->data->vertex_normals.memory_clear();
    vsx_vector* vertex_normals_p = mesh->data->vertex_normals.get_pointer();
    /*for(unsigned int i = 0; i < mesh->data->vertices.size(); i++) {
      mesh->data->vertex_normals[i] = vsx_vector(0, 0, 0);
    }*/

    //TODO: create vertex normals, for rendering... should be a separate module...
    for(unsigned int i = 0; i < mesh->data->faces.size(); i++) {
      vsx_vector a = vertex_p[face_p[i].b] - vertex_p[face_p[i].a];
      vsx_vector b = vertex_p[face_p[i].c] - vertex_p[face_p[i].a];
      vsx_vector normal;
      normal.cross(a,b);

    	//vsx_vector normal = mesh->data->get_face_normal(i);
    	normal = -normal;
    	normal.normalize();
    	vertex_normals_p[face_p[i].a] += normal;
    	vertex_normals_p[face_p[i].b] += normal;
    	vertex_normals_p[face_p[i].c] += normal;
    }

    volume_out->set(volume);

    //printf("***************Pressure %f     ", pressure);
    //printf(" Volume %f\n", volume);

    mesh_out->set_p(mesh);
  }
};

#undef printVector
#undef printFloat



class vertex_holder {
public:
  float dist;
  int id;
  bool operator < (const vertex_holder& a) {
      return dist > a.dist;
  }
};

class vsx_module_mesh_vertex_distance_sort : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* distance_to;
  // out
  vsx_module_param_mesh* mesh_out;
  vsx_module_param_float_array* original_ids;
  // internal
  vsx_mesh* mesh;
  vsx_array<vertex_holder*> distances;

  // previous id maintanence
  vsx_float_array i_ids;
  vsx_array<float> ids_data;

  //*******************************************************************************
  //*******************************************************************************
  //*******************************************************************************
  //*******************************************************************************

  // fast square root routine from nvidia
  
  #define FP_BITS(fp) (*(unsigned int *)&(fp))

  unsigned int fast_sqrt_table[0x10000];  // declare table of square roots

  typedef union FastSqrtUnion
  {
    float f;
    unsigned int i;
  } FastSqrtUnion;

  void  build_sqrt_table()
  {
    unsigned int i;
    FastSqrtUnion s;

    for (i = 0; i <= 0x7FFF; i++)
    {

      // Build a float with the bit pattern i as mantissa
      //  and an exponent of 0, stored as 127

      s.i = (i << 8) | (0x7F << 23);
      s.f = (float)sqrt(s.f);

      // Take the square root then strip the first 7 bits of
      //  the mantissa into the table

      fast_sqrt_table[i + 0x8000] = (s.i & 0x7FFFFF);

      // Repeat the process, this time with an exponent of 1,
      //  stored as 128

      s.i = (i << 8) | (0x80 << 23);
      s.f = (float)sqrt(s.f);

      fast_sqrt_table[i] = (s.i & 0x7FFFFF);
    }
  }

  inline float fastsqrt(float n)
  {
    if (FP_BITS(n) == 0)
      return 0.0;                 // check for square root of 0

    FP_BITS(n) = fast_sqrt_table[(FP_BITS(n) >> 8) & 0xFFFF] | ((((FP_BITS(n) - 0x3F800000) >> 1) + 0x3F800000) & 0x7F800000);

    return n;
  }
  
  void quicksort(vertex_holder** a, int left, int right) {
      int i = left, j = right;
      vertex_holder *tmp;
      float pivot = a[(left + right) / 2]->dist;

      /* partition */
      while (i <= j) {
            while (a[i]->dist < pivot)
                  i++;
            while (a[j]->dist > pivot)
                  j--;
            if (i <= j) {
                  tmp = a[i];
                  a[i] = a[j];
                  a[j] = tmp;
                  i++;
                  j--;
            }
      };

      /* recursion */
      if (left < j)
            quicksort(a, left, j);
      if (i < right)
            quicksort(a, i, right);
}
  

public:

  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    // clear out the distances list
    delete mesh;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;vertices;modifiers;mesh_vertex_distance_sort";
    info->description = "Sorts vertices by distance to a point\n"
                        " - camera/eye for instance";
    info->in_param_spec = "mesh_in:mesh,distance_to:float3";
    info->out_param_spec = "mesh_out:mesh,original_ids:float_array";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    distance_to = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "distance_to");
    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    original_ids = (vsx_module_param_float_array*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"original_ids");
    i_ids.data = &ids_data;
    original_ids->set_p(i_ids);
    
    build_sqrt_table();
  }


  unsigned long prev_timestamp;
  void run() {
    vsx_mesh** p = mesh_in->get_addr();
    if (p && (param_updates || prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;

      if (!(*p)->data->vertices.size()) return;
      // if new mesh from upstream, import it into ours...
      //---
      float dtx = distance_to->get(0);
      float dty = distance_to->get(1);
      float dtz = distance_to->get(2);
      //---
      //  init pointer iterators
      size_t vertex_count = (*p)->data->vertices.size();
      //printf("distances arreay size: %d      mesh data size: %d\n", distances.size(), vertex_count);
      if (distances.size() < vertex_count)
      {
        #ifdef VSXU_DEBUG
        printf("allocating more memory for vertex holders...\n");
        #endif
        distances.allocate(vertex_count-1);
        for (size_t i = 0; i < vertex_count; i++)
        {
          distances[i] = new vertex_holder;
        }
      }
      //-----------------------------------------------------------
      vertex_holder** vf = distances.get_pointer();
      vsx_vector* vp = (*p)->data->vertices.get_pointer();
      //
      
      for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
      {
        float x = fabs(dtx - (*vp).x);
        float y = fabs(dty - (*vp).y);
        float z = fabs(dtz - (*vp).z);
        // yo pythagoras!
        (*vf)->dist = sqrtf(x*x + y*y + z*z);
        (*vf)->id = i;
        vf++;
        vp++;
      }
      // sort the arrays
      quicksort(distances.get_pointer(),0,vertex_count-1);
      // put it back into our private mesh, payload order is calculated by id
      mesh->data->vertices.allocate(vertex_count);
      vertex_holder** ddp = distances.get_pointer();
      vsx_vector* dp = mesh->data->vertices.get_end_pointer();
      vsx_vector* ds = (*p)->data->vertices.get_pointer();
      size_t i = 0;
      while (i < vertex_count)
      {
        //printf("id: %d  %f\n", (*ddp)->id,(*ddp)->dist);
        *dp = ds[(*ddp)->id];
        ids_data[i] = (float)(*ddp)->id;
        dp--;
        ddp++;
        i++;
      }
      // finally set output params
      mesh->timestamp++;
      mesh_out->set_p(mesh);

      param_updates = 0;
    }
  }
};

//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------



//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module) {
  switch(module)
  {
    case 0: return (vsx_module*)(new vsx_module_mesh_vertex_picker);
    case 1: return (vsx_module*)(new vsx_module_mesh_quat_rotate);
    case 2: return (vsx_module*)(new vsx_module_mesh_deformers_mesh_vertex_move);
    case 3: return (vsx_module*)(new vsx_module_mesh_translate);
    case 4: return (vsx_module*)(new vsx_module_mesh_to_float3_arrays);
    case 5: return (vsx_module*)(new vsx_module_mesh_compute_tangents);
    case 6: return (vsx_module*)(new vsx_module_mesh_inflate);
    case 7: return (vsx_module*)(new vsx_module_mesh_deformers_random_normal_distort);
    case 8: return (vsx_module*)(new vsx_module_mesh_noise);
    case 9: return (vsx_module*)(new vsx_module_mesh_quat_rotate_around_vertex);
    case 10: return (vsx_module*)(new vsx_module_mesh_dummy);
    case 11: return (vsx_module*)(new vsx_module_mesh_scale);
    case 12: return (vsx_module*)(new vsx_module_mesh_rain_down);
    case 13: return (vsx_module*)(new vsx_module_mesh_calc_attachment);
    case 14: return (vsx_module*)(new vsx_module_mesh_vertex_distance_sort);
    case 15: return (vsx_module*)(new vsx_module_mesh_translate_edge_wraparound);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module)
  {
    case 0: delete (vsx_module_mesh_vertex_picker*)m; break;
    case 1: delete (vsx_module_mesh_quat_rotate*)m; break;
    case 2: delete (vsx_module_mesh_deformers_mesh_vertex_move*)m; break;
    case 3: delete (vsx_module_mesh_translate*)m; break;
    case 4: delete (vsx_module_mesh_to_float3_arrays*)m; break;
    case 5: delete (vsx_module_mesh_compute_tangents*)m; break;
    case 6: delete (vsx_module_mesh_inflate*)m; break;
    case 7: delete (vsx_module_mesh_deformers_random_normal_distort*)m; break;
    case 8: delete (vsx_module_mesh_noise*)m; break;
    case 9: delete (vsx_module_mesh_quat_rotate_around_vertex*)m; break;
    case 10: delete (vsx_module_mesh_dummy*)m; break;
    case 11: delete (vsx_module_mesh_scale*)m; break;
    case 12: delete (vsx_module_mesh_rain_down*)m; break;
    case 13: delete (vsx_module_mesh_calc_attachment*)m; break;
    case 14: delete (vsx_module_mesh_vertex_distance_sort*)m; break;
    case 15: delete (vsx_module_mesh_translate_edge_wraparound*)m; break;
  }
}

unsigned long get_num_modules() {
  return 16;
}
