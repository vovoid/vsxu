class module_segmesh_loft : public vsx_module
{
  vsx_module_param_float* segments;
  vsx_module_param_float* length_p;
  vsx_module_param_segment_mesh* seg_mesh_in;
  vsx_module_param_segment_mesh* seg_mesh_out;

  vsx_module_param_float_sequence* param_x_shape;
  vsx_module_param_float_sequence* param_y_shape;
  vsx_module_param_float_sequence* param_z_shape;
  vsx_module_param_float_sequence* param_size_shape;

  vsx_2dgrid_mesh base_mesh;
  vsx::sequence::channel<vsx::sequence::value_float> seq;

  long lx_t;
  long ly_t;
  long sides;
  float sx,sy;
  float n_inc;
  float length_inc;
  int i,j;
  bool first_run;

  // x_shape
  vsx::sequence::channel<vsx::sequence::value_float> seq_x_shape;
  float x_shape[8192];

  // y_shape
  vsx::sequence::channel<vsx::sequence::value_float> seq_y_shape;
  float y_shape[8192];

  // z_shape
  vsx::sequence::channel<vsx::sequence::value_float> seq_z_shape;
  float z_shape[8192];

  // size_shape
  vsx::sequence::channel<vsx::sequence::value_float> seq_size_shape;
  float sizes[8192];


  void calc_shapes()
  {
    #define CALCS(var_name) \
    if (param_##var_name->updates)\
    {\
      seq_##var_name = param_##var_name->get();\
      param_##var_name->updates = 0;\
      seq_##var_name.reset();\
      for (int i = 0; i < 8192; ++i) {\
        var_name[i] = seq_##var_name.execute(1.0f/8192.0f).get_float();\
      }\
    }

    CALCS(x_shape);
    CALCS(y_shape);
    CALCS(z_shape);

    #undef CALCS
  }



public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;segmesh;segmesh_loft";

    info->in_param_spec =
      "segment_mesh_in:segment_mesh,"
      "loft_x:sequence,"
      "loft_y:sequence,"
      "loft_z:sequence,"
      "segments:float?min=2,"
      "length:float"
    ;

    info->out_param_spec =
      "segment_mesh_out:segment_mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    param_x_shape = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE,"loft_x");
    param_x_shape->set(seq_x_shape);
    param_y_shape = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE,"loft_y");
    param_y_shape->set(seq_y_shape);

    param_z_shape = (vsx_module_param_float_sequence*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE,"loft_z");
    param_z_shape->set(seq_z_shape);

    segments = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"segments");
    segments->set(40);

    length_p = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"length");
    length_p->set(1.0f);

    seg_mesh_in = (vsx_module_param_segment_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"segment_mesh_in",true);
    seg_mesh_out = (vsx_module_param_segment_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"segment_mesh_out");
    seg_mesh_out->set_p(base_mesh);
    lx_t = -1;
    ly_t = -1;
    first_run = true;
  }

  void on_delete() {
    base_mesh.cleanup();
  }

  void run()
  {
    vsx_2dgrid_mesh* bb = seg_mesh_in->get_addr();
    if (bb) 
    {
      sides = (long)bb->vertices[0].size();
      length_inc = length_p->get()/segments->get();
      calc_shapes();

      float zz = 0;
      long num_vertices = (long)base_mesh.vertices.size();
      for (unsigned long i = 0; i < segments->get(); ++i) {
        float ip = (float)i / (float)segments->get();
        int index8192 = (int)round(8192.0f*ip);
        sx = x_shape[index8192];
        sy = y_shape[index8192];
        for (j = 0; j < (int)sides; ++j) {
          base_mesh.vertices[i][j].coord.x = bb->vertices[0][j].coord.x * sx;
          base_mesh.vertices[i][j].coord.y = bb->vertices[0][j].coord.y * sy;
          base_mesh.vertices[i][j].coord.z = zz * z_shape[index8192];
        }
        zz += length_inc;
      }
      if (segments->get() < base_mesh.vertices.size()) {
        base_mesh.vertices.reset_used((int)segments->get());
      }
        for (unsigned long i = num_vertices+1; i < floor(segments->get()); ++i) {
          for (j = 0; j < (int)sides; ++j) {
            base_mesh.vertices[i][j].color = vsx_color<>((float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,1.0f);
          }
        }

        long face = num_vertices*sides;
        for (int i = num_vertices;  i < segments->get()-1; ++i) {
          for (int j = 0; j < (int)sides; ++j) {
            int j1 = j+1;
            if (j1 == (int)sides) j1 = 0;
            base_mesh.add_face(i+1,j1, i+1,j, i,j);
            base_mesh.add_face(i,j,  i,j1,  i+1,j1);
            ++face;
          }
        }
        base_mesh.faces.reset_used(face);
      seg_mesh_out->set_p(base_mesh);
    }
    loading_done = true;
  }
};
