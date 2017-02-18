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


class vertex_holder
{
public:
  float dist;
  int id;
  bool operator < (const vertex_holder& a) {
      return dist > a.dist;
  }
};

class module_mesh_vertex_distance_sort : public vsx_module
{
public:
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float3* distance_to;

  // out
  vsx_module_param_mesh* mesh_out;
  vsx_module_param_float_array* original_ids;

  // internal
  vsx_mesh<>* mesh;
  vsx_ma_vector<vertex_holder*> distances;

  // previous id maintanence
  vsx_float_array i_ids;
  vsx_ma_vector<float> ids_data;

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

  void quicksort(vertex_holder** a, int left, int right)
  {
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


  bool init() {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    // clear out the distances list
    delete mesh;
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;vertices;modifiers;mesh_vertex_distance_sort";

    info->description =
      "Sorts vertices by distance to a point\n"
      " - camera/eye for instance"
    ;

    info->in_param_spec =
      "mesh_in:mesh,"
      "distance_to:float3"
    ;

    info->out_param_spec =
      "mesh_out:mesh,"
      "original_ids:float_array"
    ;

    info->component_class =
      "mesh";
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
    vsx_mesh<>** p = mesh_in->get_addr();
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
      vsx_vector3<>* vp = (*p)->data->vertices.get_pointer();
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
      vsx_vector3<>* dp = mesh->data->vertices.get_end_pointer();
      vsx_vector3<>* ds = (*p)->data->vertices.get_pointer();
      size_t i = 0;
      while (i < vertex_count)
      {
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
