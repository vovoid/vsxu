
class module_mesh_cloud_plane : public vsx_module
{
  // in
  vsx_module_param_float* size;
  vsx_module_param_float* angle;
  vsx_module_param_float* red;
  vsx_module_param_float* green;
  vsx_module_param_float* blue;
  vsx_module_param_float3* rotation_axis;

  // out
  vsx_module_param_render* render_result;
  vsx_module_param_mesh* mesh_result;

  // internal
  vsx_mesh* mesh;

public:

  bool init()
  {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;vovoid;cloud_plane";
    info->in_param_spec = "";
    info->out_param_spec = "mesh_result:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    VSX_UNUSED(in_parameters);
    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_result");
  }

  void run()
  {
    if (mesh->data->faces.get_used()) return;

    vsx_2dgrid_mesh gmesh;

    for (int x = 0; x < 50; ++x)
    {
      for (int y = 0; y < 50; ++y)
      {
        gmesh.vertices[x][y].coord = vsx_vector(((float)x-25)*0.8f,(float(rand()%1000))*0.0002f,(float(y)-25)*0.8f);
        gmesh.vertices[x][y].tex_coord = vsx_vector(((float)x)/50.0f,((float)y)/50.0f,0);
        gmesh.vertices[x][y].color = vsx_color((float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.0005f);
      }
    }

    for (int x = 0; x < 49; ++x)
    {
      for (int y = 0; y < 49; ++y)
      {
        gmesh.add_face(x,y+1,  x+1,y+1,  x,y);
        gmesh.add_face(x,y,  x+1,y+1,  x+1,y);
      }
    }

    gmesh.calculate_vertex_normals();

    gmesh.dump_vsx_mesh(mesh);
    mesh->data->calculate_face_centers();
    mesh->timestamp++;
    loading_done = true;

    mesh_result->set(mesh);
  }
};
