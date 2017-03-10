class module_mesh_import_vxm : public vsx_module
{
public:
  // in
  vsx_module_param_resource* filename;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;

  vsx_string<>current_filename;

  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;importers;vxm_importer";

    info->description =
      "VXM loader";

    info->in_param_spec =
      "filename:resource";

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = false;
    filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename->set("");
    current_filename = "";

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
  }

  void run()
  {
    if (filename->get() == current_filename)
      return;

    if (!vsx_string_helper::verify_filesuffix(filename->get(),"vxm"))
    {
      user_message = "module||ERROR in file name suffix! This is not a VXM mesh file!";
      return;
    }

    user_message = "module||ok";

    current_filename = filename->get();
    vsx::file *fp;

    if ((fp = engine_state->filesystem->f_open(current_filename.c_str())) == NULL)
      return;
    char tag[4] = {0,0,0,0};
    engine_state->filesystem->f_read((void*)&tag,sizeof(char) * 4,fp);
    vsx_string<>line;
    line = tag;

    if (line != "vxm")
    {
      user_message = "module||ERROR reading start tag! This is not a VXM mesh file!";
      engine_state->filesystem->f_close(fp);
      return;
    }


    size_t vert_size;
    engine_state->filesystem->f_read((void*)&vert_size,sizeof(size_t) * 1,fp);
    if (vert_size)
    {
      vsx_printf(L"vertex bytes: %ld\n",vert_size);
      void* vert_p = malloc(vert_size);
      engine_state->filesystem->f_read(vert_p,vert_size,fp);
      mesh->data->vertices.set_data( (vsx_vector3<>*)vert_p, vert_size / sizeof(vsx_vector3<>) );
    }

    size_t normals_size;
    engine_state->filesystem->f_read((void*)&normals_size,sizeof(size_t) * 1,fp);
    if (normals_size)
    {
      vsx_printf(L"normals bytes: %ld\n",normals_size);
      void* norm_p = malloc( normals_size);
      engine_state->filesystem->f_read(norm_p,normals_size,fp);
      mesh->data->vertex_normals.set_data((vsx_vector3<>*)norm_p,normals_size / sizeof(vsx_vector3<>));
    }

    size_t tex_coords_size;
    engine_state->filesystem->f_read((void*)&tex_coords_size,sizeof(size_t) * 1,fp);
    if (tex_coords_size)
    {
      vsx_printf(L"texcoord count: %ld\n",tex_coords_size);
      void* texcoords_p = malloc(tex_coords_size);
      engine_state->filesystem->f_read(texcoords_p,tex_coords_size,fp);
      mesh->data->vertex_tex_coords.set_data((vsx_tex_coord2f*)texcoords_p,tex_coords_size / sizeof(vsx_tex_coord2f));
    }

    size_t faces_size;
    engine_state->filesystem->f_read((void*)&faces_size,sizeof(size_t) * 1,fp);
    if (faces_size)
    {
      vsx_printf(L"face count: %ld\n",faces_size);
      void* faces_p = malloc(faces_size);
      engine_state->filesystem->f_read(faces_p,faces_size,fp);
      mesh->data->faces.set_data((vsx_face3*)faces_p,faces_size / sizeof(vsx_face3));
    }

    engine_state->filesystem->f_close(fp);
    loading_done = true;
    mesh->timestamp = (int)(engine_state->real_vtime*1000.0f);
    result->set_p(mesh);

  }
};
