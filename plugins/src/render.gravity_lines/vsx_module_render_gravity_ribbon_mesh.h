class vsx_module_render_gravity_ribbon_mesh : public vsx_module
{
  // in
  vsx_module_param_float4* color0;
  vsx_module_param_float4* color1;

  vsx_module_param_float* mesh_id_start;
  vsx_module_param_float* mesh_id_count;

  vsx_module_param_float* friction;
  vsx_module_param_float* step_length;
  vsx_module_param_float* ribbon_width;
  vsx_module_param_float* length;
  vsx_module_param_float* reset_pos;
  vsx_module_param_mesh* in_mesh;
  vsx_module_param_matrix* modelview_matrix;
  vsx_module_param_float3* upvector;

  // out
  vsx_module_param_render* render_result;
  vsx_module_param_mesh* mesh_result;

  // internal
  vsx_avector<gravity_strip*> gr;
  gravity_strip* grp;
  vsx_mesh** mesh;
  vsx_mesh* mesh_out;

  vsx_matrix modelview_matrix_no_connection;
  float last_update;
  unsigned long prev_num_vertices;

public:

  bool init()
  {
    mesh_out = new vsx_mesh;
    last_update = 0;
    return true;
  }

  void on_delete()
  {
    delete mesh_out;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "renderers;mesh;render_mesh_ribbon";

    info->in_param_spec =
      "in_mesh:mesh,"
      "mesh_settings:complex"
      "{"
        "mesh_id_start:float,"
        "mesh_id_count:float,"
      "},"
      "params:complex"
      "{"
        "ribbon_width:float,"
        "length:float,"
        "friction:float,"
        "step_length:float,"
        "color0:float4,"
        "color1:float4,"
        "reset_pos:float,"
        "modelview_matrix:matrix,"
        "upvector:float3"
      "}"
    ;

    info->out_param_spec =
      "render_out:render,mesh_out:mesh";

    info->component_class =
      "render";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    prev_num_vertices = 0;
    mesh_id_start = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "mesh_id_start");
    mesh_id_start->set(0.0f);
    mesh_id_count = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "mesh_id_count");
    mesh_id_count->set(0.0f);
    in_mesh = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"in_mesh");

    upvector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "upvector");
    upvector->set(0.0f);
    upvector->set(0.0f);
    upvector->set(1.0f);

    color0 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color0");
    color0->set(1.0f,0);
    color0->set(1.0f,1);
    color0->set(1.0f,2);
    color0->set(0.3f,3);

    color1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color1");
    color1->set(1.0f,0);
    color1->set(1.0f,1);
    color1->set(1.0f,2);
    color1->set(1.0f,3);

    modelview_matrix = (vsx_module_param_matrix*)in_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"modelview_matrix");



    // parameters for the effect
    friction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "friction");
    friction->set(1);

    step_length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_length");
    step_length->set(10);

    ribbon_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "ribbon_width");
    ribbon_width->set(0.2f);

    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "length");
    length->set(1.0f);

    reset_pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "reset_pos");
    reset_pos->set(-1.0f);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);

    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }

  void output(vsx_module_param_abs* param)
  {
    mesh = in_mesh->get_addr();

    // sanity checks
    if (!mesh)
      return;

    if (!(*mesh)->data->vertices.size())
      return;

    if (prev_num_vertices != (unsigned long)mesh_id_count->get())
    {
      // remove all the old ones
      for (unsigned long i = prev_num_vertices; i < (unsigned long)mesh_id_count->get(); ++i)
      {
        gr[i] = new gravity_strip;
        gr[i]->init();
        gr[i]->init_strip();
      }
      prev_num_vertices = (int)mesh_id_count->get();
    }

    size_t mesh_index = (size_t)mesh_id_start->get() % (*mesh)->data->vertices.size();

    vsx_matrix* matrix_result = modelview_matrix->get_addr();
    if (!matrix_result)
    {
      matrix_result = &modelview_matrix_no_connection;
      glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix_no_connection.m);
    }

    if (param == render_result)
    {
      for (unsigned long i = 0; i < prev_num_vertices; ++i)
      {
        gr[i]->width = ribbon_width->get();
        gr[i]->length = length->get();
        gr[i]->friction = friction->get();
        gr[i]->color0[0] = color0->get(0);
        gr[i]->color0[1] = color0->get(1);
        gr[i]->color0[2] = color0->get(2);
        gr[i]->color0[3] = color0->get(3);

        gr[i]->color1[0] = color1->get(0);
        gr[i]->color1[1] = color1->get(1);
        gr[i]->color1[2] = color1->get(2);
        gr[i]->step_freq = 10.0f * step_length->get();

        if (reset_pos->get() > 0.0f)
        {
          gr[i]->reset_pos(
            (*mesh)->data->vertices[mesh_index].x,
            (*mesh)->data->vertices[mesh_index].y,
            (*mesh)->data->vertices[mesh_index].z
          );
        } else
        {
          gr[i]->update(
            engine->dtime,
            (*mesh)->data->vertices[mesh_index].x,
            (*mesh)->data->vertices[mesh_index].y,
            (*mesh)->data->vertices[mesh_index].z
          );
        }
        gr[i]->render();
        mesh_index++;
        mesh_index = mesh_index % (*mesh)->data->vertices.size();
      }
    }
    else
    {
      float ilength = length->get();
      if (ilength > 1.0f) ilength = 1.0f;
      if (ilength < 0.01f) ilength = 0.01f;

      int num2 = BUFF_LEN * (int)(ilength * 8.0f * (float)prev_num_vertices);

      // allocate mesh memory for all parts
      mesh_out->data->faces.allocate(num2);
      mesh_out->data->vertices.allocate(num2);
      mesh_out->data->vertex_normals.allocate(num2);
      mesh_out->data->vertex_tex_coords.allocate(num2);
      mesh_out->data->faces.reset_used(num2);
      mesh_out->data->vertices.reset_used(num2);
      mesh_out->data->vertex_normals.reset_used(num2);
      mesh_out->data->vertex_tex_coords.reset_used(num2);

      vsx_face*      fs_d = mesh_out->data->faces.get_pointer();
      vsx_vector*    vs_d = mesh_out->data->vertices.get_pointer();
      vsx_vector*    ns_d = mesh_out->data->vertex_normals.get_pointer();
      vsx_tex_coord* ts_d = mesh_out->data->vertex_tex_coords.get_pointer();
      int generated_vertices = 0;
      int generated_faces = 0;

      generated_faces = 0;
      generated_vertices = 0;
      generated_vertices = 0;
      generated_vertices = 0;
      vsx_vector upv;
      upv.x = upvector->get(0);
      upv.y = upvector->get(1);
      upv.z = upvector->get(2);
      for (unsigned long i = 0; i < prev_num_vertices; ++i)
      {
        gr[i]->width = ribbon_width->get();
        gr[i]->length = length->get();
        gr[i]->friction = friction->get();
        gr[i]->color0[0] = color0->get(0);
        gr[i]->color0[1] = color0->get(1);
        gr[i]->color0[2] = color0->get(2);
        gr[i]->color0[3] = color0->get(3);

        gr[i]->color1[0] = color1->get(0);
        gr[i]->color1[1] = color1->get(1);
        gr[i]->color1[2] = color1->get(2);
        gr[i]->step_freq = 10.0f * step_length->get();

        if (reset_pos->get() > 0.0f)
        {
          gr[i]->reset_pos(
            (*mesh)->data->vertices[mesh_index].x,
            (*mesh)->data->vertices[mesh_index].y,
            (*mesh)->data->vertices[mesh_index].z
          );
        } else
        {
          gr[i]->update(
            engine->dtime,
            (*mesh)->data->vertices[mesh_index].x,
            (*mesh)->data->vertices[mesh_index].y,
            (*mesh)->data->vertices[mesh_index].z
          );
        }

        gr[i]->generate_mesh(*mesh_out,fs_d, vs_d, ns_d, ts_d, matrix_result, &upv, generated_vertices, generated_faces);
        mesh_index++;
        mesh_index = mesh_index % (*mesh)->data->vertices.size();
      }

      mesh_out->data->faces.reset_used(generated_faces);
      mesh_out->data->vertices.reset_used(generated_vertices);
      mesh_out->data->vertex_normals.reset_used(generated_vertices);
      mesh_out->data->vertex_tex_coords.reset_used(generated_vertices);

      mesh_result->set_p(mesh_out);
    }
    render_result->set(1);
  }
};
