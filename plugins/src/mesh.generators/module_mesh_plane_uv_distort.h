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


class module_mesh_plane_uv_distort : public vsx_module
{
public:
  // in
  vsx_module_param_float* x_res;
  vsx_module_param_float* z_res;

  vsx_module_param_float_sequence* p_x_shape;
  vsx_module_param_float_sequence* p_z_shape;

  vsx_module_param_float* x_shape_multiplier;
  vsx_module_param_float* z_shape_multiplier;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  int l_param_updates;
  int current_x_res;
  int current_z_res;

  // x_shape
  vsx::sequence::channel<vsx::sequence::value_float> seq_x_shape;
  float x_shape[8192];
  // z_shape
  vsx::sequence::channel<vsx::sequence::value_float> seq_z_shape;
  float z_shape[8192];


  void calc_shapes() {
    #define CALCS(var_name) \
    if (p_##var_name->updates)\
    {\
      seq_##var_name = p_##var_name->get();\
      p_##var_name->updates = 0;\
      seq_##var_name.reset();\
      for (int i = 0; i < 8192; ++i) {\
        var_name[i] = seq_##var_name.execute(1.0f/8192.0f).get_float() - 0.5f;\
      }\
    }

    CALCS(x_shape);
    CALCS(z_shape);

    #undef CALCS
  }


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;solid;plane_uv_distort";

    info->description = "";

    info->in_param_spec =
      "x_res:float?min=2,"
      "y_res:float?min=2,"
      "distortion:complex{"
        "x_shape:sequence,"
        "x_shape_multiplier:float,"
        "z_shape:sequence,"
        "z_shape_multiplier:float"
      "}"
    ;

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    l_param_updates = -1;
    loading_done = true;

    x_res = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"x_res");
    x_res->set(50.0f);

    z_res = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"y_res");
    z_res->set(50.0f);

    p_x_shape = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE, "x_shape");
    seq_x_shape.set_string( "0.5;1.000000;MC41MDAwMDA=|0.5;1.000000;MC41MDI5ODA=");
    p_x_shape->set(seq_x_shape);

    p_z_shape = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE, "z_shape");
    seq_z_shape.set_string( "0.5;1.000000;MC41MDAwMDA=|0.5;1.000000;MC41MDI5ODA=");
    p_z_shape->set(seq_z_shape);

    x_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "x_shape_multiplier");
    x_shape_multiplier->set(1.0f);

    z_shape_multiplier = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "z_shape_multiplier");
    z_shape_multiplier->set(1.0f);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");

    current_x_res = 0;
    current_z_res = 0;
  }

  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void run()
  {
    int new_res_x = (int)x_res->get();
    int new_res_z = (int)z_res->get();
    if (param_updates == 0) return;

    param_updates = 0;

    mesh->data->reset();
    calc_shapes();

    current_z_res = new_res_z;
    current_x_res = new_res_x;

    float x_shape_multiplier_f = x_shape_multiplier->get();
    float z_shape_multiplier_f = z_shape_multiplier->get();

    int vi = 0; // vertex index

    mesh->data->faces.reset_used();

    // optimized divisions
    float one_div_x_res = 1.0f / (float)(current_x_res-1);
    float one_div_z_res = 1.0f / (float)(current_z_res-1);

    for(int ix = 0; ix < current_x_res; ix++)
    {
      float ix1 = (float)ix * one_div_x_res;

      int x_index8192 = (int)round( 8192.0f * ix1 );

      int iz;
      for(iz = 0; iz < current_z_res; iz++)
      {
        // prepare values
        double iz1 = (float)iz * one_div_z_res;
        int z_index8192 = (int)round( 8192.0f * iz1 );
        // calculate skewed uv coord
        vsx_tex_coord2f uv_skew;
        uv_skew.s = (float)(ix1 + x_shape[x_index8192] * x_shape_multiplier_f);
        uv_skew.t = (float)(iz1 + z_shape[z_index8192] * z_shape_multiplier_f);

        mesh->data->vertices[vi] =  vsx_vector3<>
                                    (
                                      2.0f * ((float)ix1 - 0.5f),
                                      2.0f * ((float)iz1 - 0.5f),
                                      0
                                    );

        mesh->data->vertex_tex_coords[vi] = uv_skew;

        if (ix && iz)
        {
          vsx_face3 a;
          // c
          //
          // b   a (vi - 10)
          a.a = vi - current_z_res;
          a.b = vi-1;
          a.c = vi - current_z_res-1;
          mesh->data->faces.push_back(a);
          // b   c (vi)
          //
          //     a (vi - 10)
          a.a = vi - current_z_res;
          a.b = vi;
          a.c = vi-1;
          mesh->data->faces.push_back(a);
        }
        vi++;
      }
    }

    mesh->timestamp++;
    result->set_p(mesh);
  }
};
