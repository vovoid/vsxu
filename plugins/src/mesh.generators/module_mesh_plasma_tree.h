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

#include <math/vsx_bspline.h>
#include <math/quaternion/vsx_quaternion.h>
#include <math/quaternion/vsx_quaternion_helper.h>
#include <math/vector/vsx_vector4.h>
#include <graphics/vsx_mesh.h>
#include <math/vsx_math.h>
#include <tools/vsx_foreach.h>
#include <math/vsx_tween_values.h>
#include <tools/vsx_debug_dump.h>

class plasma_tree_generator
{
  void generate_leaf(
      vsx_mesh<>& mesh,
      size_t leaf_steps,
    vsx_vector3f leaf_cross,
    float growth,
      float step_factor,

    vsx_vector3f start_pos,
    vsx_quaternion<float> base_rot_quat,
    float rotationx,
    float rotationy,
    float vector_length
  )
  {
    vsx_quaternion<float> x_rot;
    x_rot.from_axis_angle(vsx_vector3f(1,0,0), rotationx );
    vsx_quaternion<float> z_rot;
    z_rot.from_axis_angle(vsx_vector3f(0,0,1), rotationy );
    vsx_quaternion<float> step_rot;
    step_rot *= x_rot;
    step_rot *= z_rot;

    vsx_vector3f current_pos = start_pos;

    vsx_vector3f cross = leaf_cross * growth;
    vsx_vector3f cross_result = base_rot_quat.transform(cross);

    vsx_bspline<vsx_vector3f> spline_vertices;
    spline_vertices.points.push_back(current_pos);
    spline_vertices.points.push_back(current_pos);
    vsx_bspline<vsx_vector3f> spline_cross;
    spline_cross.points.push_back(cross_result);
    spline_cross.points.push_back(cross_result);


    for_n(i, 0, leaf_steps)
    {
      vsx_vector3f direction = base_rot_quat.transform( vsx_vector3f(0,1,0) );
      direction.normalize();
      direction *= vector_length;
      current_pos += direction;

      // save position for stem
      spline_vertices.points.push_back(current_pos);
      spline_cross.points.push_back(cross_result);
      base_rot_quat *= step_rot;
      cross_result = base_rot_quat.transform(cross);
      vector_length *= step_factor;
    }

    // build leaf mesh
    mesh.data->vertices.push_back( spline_vertices.get(0.01f) - spline_cross.get(0.01f) );
    mesh.data->vertices.push_back( spline_vertices.get(0.01f) + spline_cross.get(0.01f) );
    mesh.data->vertex_colors.push_back(vsx_color<float>(1,1,1,1));
    mesh.data->vertex_colors.push_back(vsx_color<float>(1,1,1,1));
    mesh.data->vertex_normals.push_back(vsx_vector3f(0,1,0));
    mesh.data->vertex_normals.push_back(vsx_vector3f(0,1,0));
    mesh.data->vertex_tex_coords.push_back(vsx_tex_coord2f(0.0f,0));
    mesh.data->vertex_tex_coords.push_back(vsx_tex_coord2f(1.0f,0));

    size_t tesselation_steps = 20;
    float one_div_tess = growth / (float)tesselation_steps;

    for_n(i, 0, tesselation_steps)
    {
      float t = (float)i * one_div_tess;
      t = vsx::tween_values<float>::in_linear(t, 0.01f,1.0);
      mesh.data->vertices.push_back( spline_vertices.get(t) - spline_cross.get(t));
      mesh.data->vertices.push_back( spline_vertices.get(t) + spline_cross.get(t));
      mesh.data->vertex_colors.push_back(vsx_color<float>(1,1,1,1));
      mesh.data->vertex_colors.push_back(vsx_color<float>(1,1,1,1));
      mesh.data->vertex_normals.push_back(vsx_vector3f(0,1,0));
      mesh.data->vertex_normals.push_back(vsx_vector3f(0,1,0));
      mesh.data->vertex_tex_coords.push_back(vsx_tex_coord2f(0.0f,t));
      mesh.data->vertex_tex_coords.push_back(vsx_tex_coord2f(1.0f,t));

      GLuint vertices_count = (GLuint)mesh.data->vertices.size();
      mesh.data->faces.push_back(vsx_face3(vertices_count-1, vertices_count-4, vertices_count-2 ));
      mesh.data->faces.push_back(vsx_face3(vertices_count-1, vertices_count-3, vertices_count-4 ));
    }
  }

public:

  /**
   * @brief generate
   * @param mesh
   * @param step_factor 0.0..1.5
   * @param start_vector_length 0.9
   * @param stem_steps 10
   * @param leaf_steps 9
   * @param leaf_size 2.0
   * @param growth 0..2.0
   * @param rotation_x 0.14
   * @param rotation_y 0.14 or -0.14
   * @param leaf_rotation_x -0.5
   * @param leaf_rotation_y -0.07
   * @param leaf_rotation_z 0.33
   * @param stem_cross {0.08, 0.06, -0.18}
   * @param leaf_cross {0.34, -0.16, 0.04}
   * @param base_rotation {0,0,0,1}
   */
  void generate(
      vsx_mesh<>& mesh,
      float step_factor,
      float start_vector_length,
      size_t stem_steps,
      size_t leaf_steps,
      float leaf_size,
      float growth,
      float rotation_x,
      float rotation_y,
      float leaf_rotation_x,
      float leaf_rotation_y,
      float leaf_rotation_z,
      vsx_vector3f stem_cross,
      vsx_vector3f leaf_cross,
      vsx_quaternionf base_rotation
  )
  {
    vsx_quaternion<float> base_rot_quat = base_rotation;
    vsx_quaternion<float> leaf_base_rot_quat_forward;
    vsx_quaternion<float> leaf_base_rot_quat_backward;

    vsx_quaternion<float> x_rot;
    vsx_quaternion<float> z_rot;
    x_rot.from_axis_angle(vsx_vector3f(1,0,0), rotation_x * (0.5f + vsx::tween_values<float>::in_sin(growth, 0.5, 1.0) ) );
    z_rot.from_axis_angle(vsx_vector3f(0,0,1), rotation_y * (0.5f + vsx::tween_values<float>::in_sin(growth, 0.5, 1.0) ) );

    vsx_quaternion<float> step_rot;
    step_rot *= x_rot;
    step_rot *= z_rot;

    vsx_quaternion<float> leaf_x_rot;
    vsx_quaternion<float> leaf_y_rot;
    vsx_quaternion<float> leaf_z_rot;

    vsx_quaternion<float> leaf_step_rot_forward;
    leaf_x_rot.from_axis_angle(vsx_vector3f(1,0,0), rotation_x * 0.5f * growth);
    leaf_y_rot.from_axis_angle(vsx_vector3f(0,0,1), leaf_rotation_z * 0.5f * growth);
    leaf_z_rot.from_axis_angle(vsx_vector3f(0,0,1), rotation_y * 0.5f * growth);
    leaf_step_rot_forward *= leaf_x_rot;
    leaf_step_rot_forward *= leaf_z_rot;
    leaf_step_rot_forward *= leaf_y_rot;

    vsx_quaternion<float> leaf_step_rot_backward;
    leaf_x_rot.from_axis_angle(vsx_vector3f(1,0,0), -rotation_x * 0.5f * growth);
    leaf_y_rot.from_axis_angle(vsx_vector3f(0,0,1), -leaf_rotation_z * 0.5f * growth);
    leaf_z_rot.from_axis_angle(vsx_vector3f(0,0,1), -rotation_y * 0.5f * growth);
    leaf_step_rot_backward *= leaf_x_rot;
    leaf_step_rot_backward *= leaf_z_rot;
    leaf_step_rot_backward *= leaf_y_rot;

    vsx_vector3f current_pos;
    float vector_length = start_vector_length;

    vsx_vector3f cross = stem_cross * growth;
    vsx_vector3f cross_result = base_rot_quat.transform(cross);

    vsx_bspline<vsx_vector3f> spline_vertices;
    spline_vertices.points.push_back(vsx_vector3f(0,0,0));
    spline_vertices.points.push_back(vsx_vector3f(0,0,0));
    vsx_bspline<vsx_vector3f> spline_cross;
    spline_cross.points.push_back(cross_result);
    spline_cross.points.push_back(cross_result);

    float total_length = 0.0f;
    vsx_ma_vector< float > leaf_positions;

    vsx_bspline< vsx_vector4f > leaf_rotations_forward;
    vsx_bspline< vsx_vector4f > leaf_rotations_backward;
    vsx_ma_vector< float > leaf_sizes;

    for_n(i, 0, stem_steps)
    {
      vsx_vector3f direction = base_rot_quat.transform( vsx_vector3f(0,1,0) );
      direction.normalize();
      direction *= vector_length;
      current_pos += direction;

      // leafs
      if (i)
      {
        total_length += vector_length;
        leaf_positions.push_back(total_length);
        leaf_rotations_forward.points.push_back( (
                                                   base_rot_quat *
                                                   leaf_base_rot_quat_forward).get_vector4() );
        leaf_rotations_backward.points.push_back( (
                                                    base_rot_quat *
                                                    leaf_base_rot_quat_backward).get_vector4() );
        leaf_sizes.push_back( vector_length * step_factor );
        leaf_base_rot_quat_forward *= leaf_step_rot_forward;
        leaf_base_rot_quat_backward *= leaf_step_rot_backward;
      }

      // save position for stem
      spline_vertices.points.push_back(current_pos);
      spline_cross.points.push_back(cross_result);

      // calculate new rotations
      base_rot_quat *= step_rot;
      cross_result = base_rot_quat.transform(cross);
      vector_length *= step_factor;
    }

    // normalize leaf positions
    foreach (leaf_positions, i)
      leaf_positions[i] = leaf_positions[i] / total_length;

    // build stem mesh
    mesh.data->vertices.push_back( spline_vertices.get(0.01f) - spline_cross.get(0.01f) );
    mesh.data->vertices.push_back( spline_vertices.get(0.01f) + spline_cross.get(0.01f) );
    mesh.data->vertex_colors.push_back(vsx_color<float>(1,1,1,1));
    mesh.data->vertex_colors.push_back(vsx_color<float>(1,1,1,1));
    mesh.data->vertex_normals.push_back(vsx_vector3f(0,1,0));
    mesh.data->vertex_normals.push_back(vsx_vector3f(0,1,0));
    mesh.data->vertex_tex_coords.push_back(vsx_tex_coord2f(0.0f,0));
    mesh.data->vertex_tex_coords.push_back(vsx_tex_coord2f(1.0f,0));

    size_t tesselation_steps = 20;
    float one_div_tess = growth / (float)tesselation_steps;

    for_n(i, 0, tesselation_steps)
    {
      float t = (float)i * one_div_tess;
      t = vsx::tween_values<float>::in_linear(t, 0.01f, 1.0);
      mesh.data->vertices.push_back( spline_vertices.get(t) - spline_cross.get(t));
      mesh.data->vertices.push_back( spline_vertices.get(t) + spline_cross.get(t));
      mesh.data->vertex_colors.push_back(vsx_color<float>(1,1,1,1));
      mesh.data->vertex_colors.push_back(vsx_color<float>(1,1,1,1));
      mesh.data->vertex_normals.push_back(vsx_vector3f(0,1,0));
      mesh.data->vertex_normals.push_back(vsx_vector3f(0,1,0));
      mesh.data->vertex_tex_coords.push_back(vsx_tex_coord2f(0.0f,t));
      mesh.data->vertex_tex_coords.push_back(vsx_tex_coord2f(1.0f,t));


      GLuint vertices_count = (GLuint)mesh.data->vertices.size();
      mesh.data->faces.push_back(vsx_face3(vertices_count-1, vertices_count-4, vertices_count-2 ));
      mesh.data->faces.push_back(vsx_face3(vertices_count-1, vertices_count-3, vertices_count-4 ));
    }

    // build leafs
    foreach (leaf_positions, i)
    {
      generate_leaf(
            mesh,
            leaf_steps,
          leaf_cross,
          growth,
            step_factor,

          spline_vertices.get(leaf_positions[i] * growth),
          vsx_quaternionf(leaf_rotations_forward.get(leaf_positions[i] * growth)),
          leaf_rotation_x,
          leaf_rotation_y,
          leaf_sizes[i] * leaf_size
        );
      generate_leaf(
            mesh,
            leaf_steps,
          leaf_cross,
            growth,
            step_factor,

          spline_vertices.get(leaf_positions[i] * growth),
          vsx_quaternionf(leaf_rotations_backward.get(leaf_positions[i] * growth)),
          leaf_rotation_x,
          leaf_rotation_y,
          leaf_sizes[i] * leaf_size
        );
    }
  }
};


class module_mesh_plasma_tree: public vsx_module
{
public:
  // in
  vsx_module_param_float* step_factor;
  vsx_module_param_float* start_vector_length;
  vsx_module_param_float* stem_steps;
  vsx_module_param_float* leaf_steps;
  vsx_module_param_float* leaf_size;
  vsx_module_param_float* growth;
  vsx_module_param_float* rotation_x;
  vsx_module_param_float* rotation_y;
  vsx_module_param_float* leaf_rotation_x;
  vsx_module_param_float* leaf_rotation_y;
  vsx_module_param_float* leaf_rotation_z;
  vsx_module_param_float3* stem_cross;
  vsx_module_param_float3* leaf_cross;
  vsx_module_param_quaternion* base_rotation;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  plasma_tree_generator generator;
  bool first_run;

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;solid;plasma_tree";

    info->description =
      "Plasma tree";

    info->in_param_spec =
      "step_factor:float,"
      "start_vector_length:float,"
      "stem_steps:float,"
      "leaf_steps:float,"
      "leaf_size:float,"
      "growth:float,"
        "rotation:complex"
        "{"
          "rotation_x:float,"
          "rotation_y:float,"
          "leaf_rotation_x:float,"
          "leaf_rotation_y:float,"
          "leaf_rotation_z:float"
        "},"
      "stem_cross:float3,"
      "leaf_cross:float3,"
      "base_rotation:quaternion"
    ;

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    step_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_factor");
    step_factor->set(0.62f);

    start_vector_length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "start_vector_length");
    start_vector_length->set(1.0f);

    stem_steps = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "stem_steps");
    stem_steps->set(5);

    leaf_steps = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "leaf_steps");
    leaf_steps->set(5);

    leaf_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "leaf_size");
    leaf_size->set(1.0f);

    growth = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "growth");
    growth->set(1.0f);

    rotation_x = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "rotation_x");
    rotation_x->set(0.1f);

    rotation_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "rotation_y");
    rotation_y->set(0.1f);

    leaf_rotation_x = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "leaf_rotation_x");
    leaf_rotation_x->set(0.1f);

    leaf_rotation_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "leaf_rotation_y");
    leaf_rotation_y->set(0.1f);

    leaf_rotation_z = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "leaf_rotation_z");
    leaf_rotation_z->set(0.0f);

    stem_cross = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "stem_cross");
    stem_cross->set(0.1f, 0);
    stem_cross->set(0, 1);
    stem_cross->set(0, 2);

    leaf_cross = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "leaf_cross");
    leaf_cross->set(0.1f, 0);
    leaf_cross->set(0, 1);
    leaf_cross->set(0, 2);


    base_rotation = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION, "base_rotation");

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH, "mesh");
    first_run = true;
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
    req(
      first_run
      ||
          step_factor->updates
          ||
          start_vector_length->updates
          ||
          stem_steps->updates
          ||
          leaf_steps->updates
          ||
          leaf_size->updates
          ||
          growth->updates
          ||
          rotation_x->updates
          ||
          rotation_y->updates
          ||
          leaf_rotation_x->updates
          ||
          leaf_rotation_y->updates
          ||
          leaf_rotation_z->updates
          ||
          stem_cross->updates
          ||
          leaf_cross->updates
          ||
      base_rotation->updates
    );

    step_factor->updates = 0;
    start_vector_length->updates = 0;
    stem_steps->updates = 0;
    leaf_steps->updates = 0;
    leaf_size->updates = 0;
    growth->updates = 0;
    rotation_x->updates = 0;
    rotation_y->updates = 0;
    leaf_rotation_x->updates = 0;
    leaf_rotation_y->updates = 0;
    leaf_rotation_z->updates = 0;
    stem_cross->updates = 0;
    leaf_cross->updates = 0;
    base_rotation->updates = 0;


    mesh->data->vertices.reset_used();
    mesh->data->faces.reset_used();
    mesh->data->vertex_normals.reset_used();
    mesh->data->vertex_colors.reset_used();
    mesh->data->vertex_tex_coords.reset_used();

    generator.generate(
          *mesh,
          step_factor->get(),
          start_vector_length->get(),
          (size_t)stem_steps->get(),
          (size_t)leaf_steps->get(),
          leaf_size->get(),
          growth->get(),
          rotation_x->get(),
          rotation_y->get(),
          leaf_rotation_x->get(),
          leaf_rotation_y->get(),
          leaf_rotation_z->get(),
          vsx_vector3f( stem_cross->get(0), stem_cross->get(1), stem_cross->get(2) ),
          vsx_vector3f( leaf_cross->get(0), leaf_cross->get(1), leaf_cross->get(2) ),
          vsx_quaternionf(base_rotation->get(0), base_rotation->get(1), base_rotation->get(2), base_rotation->get(3))
        );

    first_run = false;
    mesh->timestamp++;
    result->set_p(mesh);
  }
};
