class module_segmesh_shape_basic : public vsx_module
{
  vsx_module_param_int* shape_type;
  vsx_module_param_float* sides;
  vsx_module_param_segment_mesh* seg_mesh;
  vsx_module_param_float* size;
  vsx_2dgrid_mesh gmesh;
  float incr;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;segmesh;shape;segmesh_shape_basic";
    info->in_param_spec = "shape_type:enum?sphere|rectangle,sides:float,size:float";
    info->out_param_spec = "segment_mesh:segment_mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    shape_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"shape_type");
    size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"size");
    size->set(1);
    sides = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"sides");
    sides->set(16);
    seg_mesh = (vsx_module_param_segment_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"segment_mesh");
    seg_mesh->set_p(gmesh);
  }

  void run()
  {
    //printf("shape type: %d\n",shape_type->get());
    gmesh.reset_vertices();
    float ss = size->get();
    switch (shape_type->get()) {
      case 0:
        {
          incr = PI_FLOAT*2.0f/sides->get();
          for (float a = 0; a < sides->get(); ++a) {
            gmesh.vertices[0][(int)a].coord = vsx_vector((float)cos(a*incr)*ss,(float)sin(a*incr)*ss,0);
            gmesh.vertices[0][(int)a].color = vsx_color((float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,0.8f);
            //vsx_color__(0.5,0.5,0.5,0.5);
          }
        }
      break;
    }; // end switch
    //vsx_2dgrid_mesh b = gmesh;
    //printf("base sides: %d\n",gmesh.vertices[0].size());
    seg_mesh->set_p(gmesh);
    loading_done = true;
  }

  void on_delete() {
    gmesh.cleanup();
  }
};

