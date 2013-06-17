class module_mesh_import_vxm : public vsx_module
{
public:
  // in
  vsx_module_param_resource* filename;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh* mesh;
  bool first_run;
  int n_rays;

  vsx_string current_filename;

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
  info->identifier = "mesh;importers;vxm_importer";
  info->description = "VXM loader";
  info->in_param_spec = "filename:resource";
  info->out_param_spec = "mesh:mesh";
  info->component_class = "mesh";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = false;
  filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  filename->set("");
  current_filename = "";

  result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
  result->set_p(mesh);
  first_run = true;
}

  void run()
  {
    if (filename->get() != current_filename) {
      if (!verify_filesuffix(filename->get(),"vxm")) {
        filename->set(current_filename);
        message = "module||ERROR! This is not a OBJ mesh file!";
        return;
      } else message = "module||ok";

      current_filename = filename->get();
      vsxf_handle *fp;
      //printf("a\n");
      if ((fp = engine->filesystem->f_open(current_filename.c_str(), "r")) == NULL)
        return;
      char tag[4];
      engine->filesystem->f_read((void*)&tag,sizeof(char) * 4,fp);
      vsx_string line;
      line = tag;
      //printf("vxm line read: %s\n",line.c_str());
      if (line == "vxm")
      {
        //printf("found vxm file\n");
        size_t vert_size;
        engine->filesystem->f_read((void*)&vert_size,sizeof(size_t) * 1,fp);
        if (vert_size)
        {
          //printf("vertex bytes: %d\n",vert_size);
          void* vert_p = malloc(vert_size);
          engine->filesystem->f_read(vert_p,vert_size,fp);
          mesh->data->vertices.set_data((vsx_vector*)vert_p,vert_size / sizeof(vsx_vector));
        }

        size_t normals_size;
        engine->filesystem->f_read((void*)&normals_size,sizeof(size_t) * 1,fp);
        if (normals_size)
        {
          //printf("normals bytes: %d\n",normals_size);
          void* norm_p = malloc( normals_size);
          engine->filesystem->f_read(norm_p,normals_size,fp);
          mesh->data->vertex_normals.set_data((vsx_vector*)norm_p,normals_size / sizeof(vsx_vector));
        }

        size_t tex_coords_size;
        engine->filesystem->f_read((void*)&tex_coords_size,sizeof(size_t) * 1,fp);
        if (tex_coords_size)
        {
          //printf("texcoord count: %d\n",tex_coords_size);
          void* texcoords_p = malloc(tex_coords_size);
          engine->filesystem->f_read(texcoords_p,tex_coords_size,fp);
          mesh->data->vertex_tex_coords.set_data((vsx_tex_coord*)texcoords_p,tex_coords_size / sizeof(vsx_tex_coord));
        }

        size_t faces_size;
        engine->filesystem->f_read((void*)&faces_size,sizeof(size_t) * 1,fp);
        if (faces_size)
        {
          //printf("face count: %d\n",faces_size);
          void* faces_p = malloc(faces_size);
          engine->filesystem->f_read(faces_p,faces_size,fp);
          mesh->data->faces.set_data((vsx_face*)faces_p,faces_size / sizeof(vsx_face));
        }
      }
      engine->filesystem->f_close(fp);
      loading_done = true;
      mesh->timestamp++;
    }
  }
};
