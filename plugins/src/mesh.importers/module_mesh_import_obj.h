#include <vsx_string_aux.h>


class module_mesh_import_obj : public vsx_module
{
public:
  // in
  vsx_module_param_resource* filename;
  vsx_module_param_int* preserve_uv_coords;

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
  info->identifier = "mesh;importers;obj_importer";
  info->description = "";
  info->in_param_spec = "filename:resource,preserve_uv_coords:enum?NO|YES";
  info->out_param_spec = "mesh:mesh";
  info->component_class = "mesh";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = false;
  filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  filename->set("");
  current_filename = "";

  preserve_uv_coords = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"preserve_uv_coords");
  preserve_uv_coords->set(1);

  result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
  first_run = true;
}

void run() {
  if (filename->get() != current_filename) {
    if (!verify_filesuffix(filename->get(),"obj")) {
      filename->set(current_filename);
      message = "module||ERROR! This is not a OBJ mesh file!";
      return;
    } else message = "module||ok";

    current_filename = filename->get();
    vsxf_handle *fp;

    if ((fp = engine->filesystem->f_open(current_filename.c_str(), "r")) == NULL)
    {
      return;
    }

    char buf[65535];
    vsx_string line;
    vsx_array<vsx_vector> vertices;
    vsx_array<vsx_vector> normals;
    vsx_array<vsx_tex_coord> texcoords;
    mesh->data->clear();

    int face_cur = 0;
    bool found_normals = false;
    bool found_texcoords = false;
    if (preserve_uv_coords->get()) {
      mesh->data->vertices.reset_used();
      mesh->data->vertex_tex_coords.reset_used();
      mesh->data->vertex_normals.reset_used();
      mesh->data->faces.reset_used();

      while (engine->filesystem->f_gets(buf,65535,fp)) {
        line = buf;
        if (line[line.size()-1] == 0x0A) line.pop_back();
        if (line[line.size()-1] == 0x0D) line.pop_back();
        if (line.size()) {
          vsx_avector<vsx_string> parts;
          vsx_string deli = " ";
          explode(line, deli, parts);
          if (parts[0] == "v") {
            vertices.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
          } else
          if (parts[0] == "vt") {
            vsx_tex_coord a;
            a.s = (s2f(parts[1]));
            a.t = (s2f(parts[2]));
            texcoords.push_back(a);
            found_texcoords = true;

          } else
          if (parts[0] == "vn") {
            normals.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
            found_normals = true;
          } else
          if (parts[0] == "f") {
              vsx_face ff;
              vsx_string deli2 = "/";


              vsx_avector<vsx_string> parts2;
              explode(parts[1], deli2, parts2);
              vsx_avector<vsx_string> parts3;
              explode(parts[2], deli2, parts3);
              vsx_avector<vsx_string> parts4;
              explode(parts[3], deli2, parts4);

              ff.c = face_cur;   //s2i(parts2[0])-1;
              ff.b = face_cur+1; //s2i(parts3[0])-1;
              ff.a = face_cur+2; //s2i(parts4[0])-1;


              int id;
              id = vsx_string_aux::s2i(parts2[0])-1; if (id < 0) id=0;
              mesh->data->vertices[ff.a] = vertices[id];
              id = vsx_string_aux::s2i(parts3[0])-1; if (id < 0) id=0;
              mesh->data->vertices[ff.b] = vertices[id];
              id = vsx_string_aux::s2i(parts4[0])-1; if (id < 0) id=0;
              mesh->data->vertices[ff.c] = vertices[id];

              if (found_texcoords && found_normals) {
                if (parts2[1] != "") {
                  mesh->data->vertex_tex_coords[ff.a] = texcoords[ vsx_string_aux::s2i(parts2[1])-1 ];
                  mesh->data->vertex_tex_coords[ff.b] = texcoords[ vsx_string_aux::s2i(parts3[1])-1 ];
                  mesh->data->vertex_tex_coords[ff.c] = texcoords[ vsx_string_aux::s2i(parts4[1])-1 ];
                }
                if (parts2[2] != "") {
                  mesh->data->vertex_normals[ff.a] = normals[ vsx_string_aux::s2i(parts2[2])-1 ];
                  mesh->data->vertex_normals[ff.b] = normals[ vsx_string_aux::s2i(parts3[2])-1 ];
                  mesh->data->vertex_normals[ff.c] = normals[ vsx_string_aux::s2i(parts4[2])-1 ];
                }
              } else
              if (found_normals) {
                if (parts2[2] != "") {
                  mesh->data->vertex_normals[ff.a] = normals[ vsx_string_aux::s2i(parts2[2])-1 ];
                  mesh->data->vertex_normals[ff.b] = normals[ vsx_string_aux::s2i(parts3[2])-1 ];
                  mesh->data->vertex_normals[ff.c] = normals[ vsx_string_aux::s2i(parts4[2])-1 ];
                }
              } else
              if (found_texcoords) {
                if (parts2[1] != "") {
                  mesh->data->vertex_tex_coords[ff.a] = texcoords[ vsx_string_aux::s2i(parts2[1])-1 ];
                  mesh->data->vertex_tex_coords[ff.b] = texcoords[ vsx_string_aux::s2i(parts3[1])-1 ];
                  mesh->data->vertex_tex_coords[ff.c] = texcoords[ vsx_string_aux::s2i(parts4[1])-1 ];
                }
              }


              face_cur += 3;
              mesh->data->faces.push_back(ff);
          }

        }
      }
    } else {

      while (engine->filesystem->f_gets(buf,65535,fp)) {
        line = buf;
        if (line[line.size()-1] == 0x0A) line.pop_back();
        if (line[line.size()-1] == 0x0D) line.pop_back();
        if (line.size()) {
          vsx_avector<vsx_string> parts;
          vsx_string deli = " ";
          explode(line, deli, parts);
          if (parts[0] == "v") {
            mesh->data->vertices.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
          } else
          if (parts[0] == "f") {
              vsx_face ff;
              vsx_string deli2 = "/";

              vsx_avector<vsx_string> parts2;
              explode(parts[1], deli2, parts2);
              vsx_avector<vsx_string> parts3;
              explode(parts[2], deli2, parts3);
              vsx_avector<vsx_string> parts4;
              explode(parts[3], deli2, parts4);

              ff.c = vsx_string_aux::s2i(parts2[0])-1;
              ff.b = vsx_string_aux::s2i(parts3[0])-1;
              ff.a = vsx_string_aux::s2i(parts4[0])-1;

              mesh->data->faces.push_back(ff);
          }

        }
      }
    }

    engine->filesystem->f_close(fp);
    loading_done = true;
    mesh->timestamp = (int)(engine->real_vtime*1000.0f);
    #ifdef VSXU_DEBUG
      //printf("mesh timestamp: %d\n", (int)mesh->timestamp);
    #endif
  }
  result->set_p(mesh);
}
};
