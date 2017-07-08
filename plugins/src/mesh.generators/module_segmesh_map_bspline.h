class module_segmesh_map_bspline : public vsx_module
{
  // in
  vsx_module_param_segment_mesh* seg_mesh_in;
  vsx_module_param_mesh* bspline_vertices_mesh;
  vsx_module_param_float* b_pos;
  vsx_module_param_float* length;
  // out
  vsx_module_param_render* render_result;
  vsx_module_param_mesh* mesh_result;
  // internal
  vsx_bspline<vsx_vector3f> spline0;
  // the mesh we're gonna put on the path
  vsx_2dgrid_mesh *base_mesh;
  // our building tool
  vsx_2dgrid_mesh gmesh;

  int i,j;
  float stime;
  float sides;
  int num;

  // the step for each spline segment - defined as number of length/segments
  float spline_step;

  vsx_vector3<> old;
  vsx_vector3<> e;
  vsx_vector3<> f;
  vsx_vector3<> d;
  vsx_matrix<float> ma;
  vsx_vector3<> old_vec;
  vsx_vector3<> spos;
  vsx_vector3<> upv;
  // the resulting mesh
  vsx_mesh<>* result_mesh;
public:

  bool init()
  {
    result_mesh = new vsx_mesh<>;
    return true;
  }
  void on_delete()
  {
    delete result_mesh;
    gmesh.cleanup();
  }


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;segmesh;map_segmesh_bspline";

    info->in_param_spec =
      "seg_mesh_in:segment_mesh,"
      "bspline_vertices_mesh:mesh,"
      "b_pos:float,"
      "length:float"
    ;

    info->out_param_spec =
      "mesh_result:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    sides = 8;
    stime = 0.0f;

    seg_mesh_in = (vsx_module_param_segment_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"seg_mesh_in");

    bspline_vertices_mesh = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"bspline_vertices_mesh");

    b_pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b_pos");
    b_pos->set(1);

    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"length");
    length->set(0.1f);

    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_result");
    mesh_result->set_p(result_mesh);

    spline0.points[0] = vsx_vector3<>(0.0f);

    upv = vsx_vector3<>(0,1);
  }

  void run()
  {
    base_mesh = seg_mesh_in->get_addr();
    if (!base_mesh) return;

    vsx_mesh<>** spline_mesh = bspline_vertices_mesh->get_addr();

    if (!spline_mesh) return;

    stime = b_pos->get();

    spline0.points.set_volatile();
    spline0.points.set_data((*spline_mesh)->data->vertices.get_pointer(),(*spline_mesh)->data->vertices.size());
    if (!spline0.points.size()) return;
    spline0.set_pos(stime);

    num = (int)base_mesh->vertices.size();
    sides = (float)base_mesh->vertices[0].size();
    spline_step = length->get()/((float)num+1.0f);

    spline0.step(spline_step);
    spos = spline0.get_current();
    e = spos-old;
    e.normalize();
    old = spos;


    for (i = 0; i < num; ++i)
    {
      spline0.step(spline_step);
      spos = spline0.get_current();

      e = spos-old;
      e.normalize();

      ma.rotation_from_vectors(&e);
      old = spos;

      for (j = 0; j < (int)sides; ++j)
      {
        gmesh.vertices[i][j].coord = ma.multiply_vector(base_mesh->vertices[i][j].coord)+spos;
        gmesh.vertices[i][j].color = vsx_color<>(0.5f,0.5f,0.5f,1.0f);
      }
    }

    if (!gmesh.faces.size())
    {
      for (int i = 0;  i < num-2; ++i)
      {
        for (int j = 0; j < (int)sides; ++j)
        {
          int j1 = j+1;
          if (j1 == (int)sides) j1 = 0;
          gmesh.add_face(i+1,j1, i+1,j, i,j);
          gmesh.add_face(i,j, i,j1, i+1,j1);
        }
      }
    }
    gmesh.calculate_face_normals();
    gmesh.calculate_vertex_normals();
    gmesh.dump_vsx_mesh(result_mesh);

    mesh_result->set_p(result_mesh);
    loading_done = true;
    return;
  }
};

