#include <string/vsx_string_helper.h>


class module_mesh_import_robj : public vsx_module
{
public:
  // in
  vsx_module_param_resource* filename;
  vsx_module_param_int* preserve_uv_coords;
  vsx_module_param_int* center_object;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  bool first_run;
  int n_rays;
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
      "mesh;importers;robj_importer";

    info->description = "";

    info->in_param_spec =
      "filename:resource,"
      "preserve_uv_coords:enum?NO|YES,"
      "center_object:enum?NO|YES"
    ;

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

    preserve_uv_coords = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"preserve_uv_coords");
    preserve_uv_coords->set(1);

    center_object = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"center_object");
    center_object->set(0);

    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
  }

  void run()
  {
    if (
        filename->get() == current_filename
        &&
        !param_updates
        )
      return;

    param_updates = 0;

    if (!vsx_string_helper::verify_filesuffix(filename->get(),"robj"))
    {
      filename->set(current_filename);
      user_message = "module||ERROR! This is not a ROBJ mesh file!";
      return;
    }
    else
      user_message = "module||ok";

    current_filename = filename->get();
    vsx::file *fp;

    if ((fp = engine_state->filesystem->f_open(current_filename.c_str())) == NULL)
    {
      VSX_ERROR_RETURN("Could not open file");
    }

    char buf[65535];
    vsx_string<>line;
    vsx_ma_vector<vsx_vector3<> > vertices;
    vsx_ma_vector<vsx_vector3<> > normals;
    vsx_ma_vector<vsx_tex_coord2f> texcoords;
    vsx_ma_vector<vsx_tex_coord2f> texcoords_2;

    int face_cur = 0;
    bool found_normals = false;
    bool found_texcoords = false;
    if (preserve_uv_coords->get())
    {
      mesh->data->vertices.reset_used();
      mesh->data->vertex_tex_coords.reset_used();
      mesh->data->vertex_normals.reset_used();
      mesh->data->faces.reset_used();

      while (engine_state->filesystem->f_gets(buf,65535,fp))
      {
        line = buf;

        if (line[line.size()-1] == 0x0A) line.pop_back();
        if (line[line.size()-1] == 0x0D) line.pop_back();

        if (!line.size())
          continue;

        vsx_nw_vector< vsx_string<> > parts;
        vsx_string<>deli = " ";
        vsx_string_helper::explode(line, deli, parts);
        if (parts[0] == "v") {
          vertices.push_back(vsx_vector3<>(vsx_string_helper::s2f(parts[1]),vsx_string_helper::s2f(parts[2]),vsx_string_helper::s2f(parts[3])));
        } else
        if (parts[0] == "vt") {
          vsx_tex_coord2f a;
          a.s = (vsx_string_helper::s2f(parts[1]));
          a.t = (vsx_string_helper::s2f(parts[2]));
          texcoords.push_back(a);
          found_texcoords = true;
          continue;
        }

        if (parts[0] == "vt2")
        {
          vsx_tex_coord2f a;
          a.s = (vsx_string_helper::s2f(parts[1]));
          a.t = (vsx_string_helper::s2f(parts[2]));
          texcoords_2.push_back(a);
          found_texcoords = true;
          continue;
        }


        if (parts[0] == "vn") {
          normals.push_back(vsx_vector3<>(vsx_string_helper::s2f(parts[1]),vsx_string_helper::s2f(parts[2]),vsx_string_helper::s2f(parts[3])));
          found_normals = true;
        } else
        if (parts[0] == "f") {
            vsx_face3 ff;
            vsx_string<>deli2 = "/";


            vsx_nw_vector< vsx_string<> > parts2;
            vsx_string_helper::explode(parts[1], deli2, parts2);
            vsx_nw_vector< vsx_string<> > parts3;
            vsx_string_helper::explode(parts[2], deli2, parts3);
            vsx_nw_vector< vsx_string<> > parts4;
            vsx_string_helper::explode(parts[3], deli2, parts4);

            ff.c = face_cur;
            ff.b = face_cur+1;
            ff.a = face_cur+2;


            int id;
            id = vsx_string_helper::s2i(parts2[0])-1; if (id < 0) id=0;
            mesh->data->vertices[ff.a] = vertices[id];
            id = vsx_string_helper::s2i(parts3[0])-1; if (id < 0) id=0;
            mesh->data->vertices[ff.b] = vertices[id];
            id = vsx_string_helper::s2i(parts4[0])-1; if (id < 0) id=0;
            mesh->data->vertices[ff.c] = vertices[id];



            const size_t tex_coord_multiplier = 2;

            if (found_texcoords && found_normals) {
              if (parts2[1] != "")
              {
                mesh->data->vertex_tex_coords[ff.a * tex_coord_multiplier ] =
                    texcoords[
                      vsx_string_helper::s2i( parts2[1] ) - 1
                    ];

                mesh->data->vertex_tex_coords[ff.b * tex_coord_multiplier ] =
                    texcoords[
                      vsx_string_helper::s2i( parts3[1] ) - 1
                    ];
                mesh->data->vertex_tex_coords[ff.c * tex_coord_multiplier ] =
                    texcoords[
                      vsx_string_helper::s2i( parts4[1] ) - 1
                    ];
                if (tex_coord_multiplier > 1)
                {
                  mesh->data->vertex_tex_coords[ff.a * tex_coord_multiplier + 1 ] =
                      texcoords_2[
                        vsx_string_helper::s2i( parts2[3] ) - 1
                      ];

                  mesh->data->vertex_tex_coords[ff.b * tex_coord_multiplier + 1 ] =
                      texcoords_2[
                        vsx_string_helper::s2i( parts3[3] ) - 1
                      ];
                  mesh->data->vertex_tex_coords[ff.c * tex_coord_multiplier + 1 ] =
                      texcoords_2[
                        vsx_string_helper::s2i( parts4[3] ) - 1
                      ];
                }
              }
              if (parts2[2] != "")
              {
                mesh->data->vertex_normals[ff.a] = normals[vsx_string_helper::s2i(parts2[2])-1];
                mesh->data->vertex_normals[ff.b] = normals[vsx_string_helper::s2i(parts3[2])-1];
                mesh->data->vertex_normals[ff.c] = normals[vsx_string_helper::s2i(parts4[2])-1];
              }
            } else
            if (found_normals) {
              if (parts2[2] != "") {
                mesh->data->vertex_normals[ff.a] = normals[ vsx_string_helper::s2i(parts2[2])-1 ];
                mesh->data->vertex_normals[ff.b] = normals[ vsx_string_helper::s2i(parts3[2])-1 ];
                mesh->data->vertex_normals[ff.c] = normals[ vsx_string_helper::s2i(parts4[2])-1 ];
              }
            } else
            if (found_texcoords) {
              if (parts2[1] != "") {
                mesh->data->vertex_tex_coords[ff.a] = texcoords[ vsx_string_helper::s2i(parts2[1])-1 ];
                mesh->data->vertex_tex_coords[ff.b] = texcoords[ vsx_string_helper::s2i(parts3[1])-1 ];
                mesh->data->vertex_tex_coords[ff.c] = texcoords[ vsx_string_helper::s2i(parts4[1])-1 ];
              }
            }


            face_cur += 3;
            mesh->data->faces.push_back(ff);
        }


      }
    } else {

      while (engine_state->filesystem->f_gets(buf,65535,fp))
      {
        line = buf;
        if (line[line.size()-1] == 0x0A) line.pop_back();
        if (line[line.size()-1] == 0x0D) line.pop_back();
        if (line.size())
        {
          vsx_nw_vector< vsx_string<> > parts;
          vsx_string<>deli = " ";
          vsx_string_helper::explode(line, deli, parts);
          if (parts[0] == "v")
          {
            mesh->data->vertices.push_back(vsx_vector3<>(vsx_string_helper::s2f(parts[1]),vsx_string_helper::s2f(parts[2]),vsx_string_helper::s2f(parts[3])));
          } else
          if (parts[0] == "f")
          {
              vsx_face3 ff;
              vsx_string<>deli2 = "/";

              vsx_nw_vector< vsx_string<> > parts2;
              vsx_string_helper::explode(parts[1], deli2, parts2);
              vsx_nw_vector< vsx_string<> > parts3;
              vsx_string_helper::explode(parts[2], deli2, parts3);
              vsx_nw_vector< vsx_string<> > parts4;
              vsx_string_helper::explode(parts[3], deli2, parts4);

              ff.c = vsx_string_helper::s2i(parts2[0])-1;
              ff.b = vsx_string_helper::s2i(parts3[0])-1;
              ff.a = vsx_string_helper::s2i(parts4[0])-1;

              mesh->data->faces.push_back(ff);
          }
        }
      }
    }

    if (center_object->get())
    {
      // calculate middle of object and move accordingly
      vsx_vector3<> minima;
      vsx_vector3<> maxima;

      vsx_vector3<>* vs_p;
      vs_p = &mesh->data->vertices[0];

      minima = vs_p[0];
      maxima = vs_p[0];

      // investigate
      for (size_t i = 0; i < mesh->data->vertices.size(); i++)
      {
        if (vs_p[i].x < minima.x)
          minima.x = vs_p[i].x;
        if (vs_p[i].x > maxima.x)
          maxima.x = vs_p[i].x;

        if (vs_p[i].y < minima.y)
          minima.y = vs_p[i].y;
        if (vs_p[i].y > maxima.y)
          maxima.y = vs_p[i].y;

        if (vs_p[i].z < minima.z)
          minima.z = vs_p[i].z;
        if (vs_p[i].z > maxima.z)
          maxima.z = vs_p[i].z;
      }

      vsx_vector3<> midpoint
      (
        -(minima.x + (maxima.x - minima.x) * 0.5),
        -(minima.y + (maxima.y - minima.y) * 0.5),
        -(minima.z + (maxima.z - minima.z) * 0.5)
      );

      vsx_vector3<> move
      (
        midpoint.x,
        midpoint.y,
        midpoint.z
      );

      // performe adjustment
      for (size_t i = 0; i < mesh->data->vertices.size(); i++)
      {
        vs_p[i] += move;
      }
    }

    engine_state->filesystem->f_close(fp);
    loading_done = true;
    mesh->timestamp = (int)(engine_state->real_vtime*1000.0f);

    result->set_p(mesh);
  }
};
