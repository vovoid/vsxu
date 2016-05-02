const int nRings = 6;
const int nSectors = 16;
const int tessU = 4, tessV = 4;
const int facecount = nRings*nSectors*tessV*tessU;

class module_mesh_kaleidoscope : public vsx_module
{
  //in
  vsx_module_param_float* hemispheric;
  // out
  vsx_module_param_mesh* mesh_result;
  // internal
  vsx_mesh<>* mesh;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;generators;vovoid;kaleido_mesh";

    info->in_param_spec =
      "hemispheric:float";

    info->out_param_spec =
      "mesh_out:mesh";

    info->component_class =
      "mesh";

  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    hemispheric = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"hemispheric");
    hemispheric->set(0.0f);
    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
  }

  bool init() {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

void run() {

  if (mesh->data->vertices.get_used() && hemispheric->updates == 0) {
    // re-allocate
  } else {
    // just allocate
    hemispheric->updates = 0;
    unsigned long vert_pos = 0;
    unsigned long face_pos = 0;
    int i, j;
    float hemispheric_v = hemispheric->get();

    for(j = 0; j < nRings; j++)
      for(i = 0; i < nSectors; i++) {
        float dist0  =  (float)j      * (1.0f   / nRings);
        float angle0 =  (float)i      * (2.0f * (float)PI / nSectors);
        float dist1  = ((float)j + 1.0f) * (1.0f   / nRings);
        float angle1 = ((float)i + 1.0f) * (2.0f * (float)PI / nSectors);

        float uFact = j&1?1.0f:-1.0f;
        float vFact = i&1?1.0f:-1.0f;
        float uOffs = j&1?0.0f:1.0f;
        float vOffs = i&1?0.0f:1.0f;

        for(int k = 0; k < tessV; k++)
          for(int l = 0; l < tessU; l++) {
            float u0 = (float)l / tessU;
            float v0 = (float)k / tessV;
            float u1 = (float)(l + 1) / tessU;
            float v1 = (float)(k + 1) / tessV;
            float d0 = dist0 * (1 - u0) + dist1 * u0;
            float a0 = angle0 * (1 - v0) + angle1 * v0;
            float d1 = dist0 * (1 - u1) + dist1 * u1;
            float a1 = angle0 * (1 - v1) + angle1 * v1;

            float x0 = (float)cos(a0) * d0;
            float y0 = (float)sin(a0) * d0;
            float x1 = (float)cos(a0) * d1;
            float y1 = (float)sin(a0) * d1;
            float x2 = (float)cos(a1) * d1;
            float y2 = (float)sin(a1) * d1;
            float x3 = (float)cos(a1) * d0;
            float y3 = (float)sin(a1) * d0;

            u0 = u0 * uFact + uOffs;
            v0 = v0 * vFact + vOffs;
            u1 = u1 * uFact + uOffs;
            v1 = v1 * vFact + vOffs;

            float r0 = (float)sqrt(fabs(1.0f-(x0*x0+y0*y0)))*hemispheric_v;
            float r2 = (float)sqrt(fabs(1.0f-(x2*x2+y2*y2)))*hemispheric_v;


            mesh->data->vertex_tex_coords[vert_pos].s = u0;
            mesh->data->vertex_tex_coords[vert_pos].t = v0;
            mesh->data->vertices[vert_pos].x = x0;
            mesh->data->vertices[vert_pos].y = y0;
            mesh->data->vertices[vert_pos].z = r0;
            ++vert_pos;
            mesh->data->vertex_tex_coords[vert_pos].s = u1;
            mesh->data->vertex_tex_coords[vert_pos].t = v0;
            mesh->data->vertices[vert_pos].x = x1;
            mesh->data->vertices[vert_pos].y = y1;
            mesh->data->vertices[vert_pos].z = (float)sqrt(fabs(1.0f-(x1*x1+y1*y1)))*hemispheric_v;
            ++vert_pos;
            mesh->data->vertex_tex_coords[vert_pos].s = u1;
            mesh->data->vertex_tex_coords[vert_pos].t = v1;
            mesh->data->vertices[vert_pos].x = x2;
            mesh->data->vertices[vert_pos].y = y2;
            mesh->data->vertices[vert_pos].z = r2;
            ++vert_pos;

            mesh->data->faces[face_pos].a = vert_pos-3;
            mesh->data->faces[face_pos].b = vert_pos-2;
            mesh->data->faces[face_pos].c = vert_pos-1;
            ++face_pos;

            //------------------------------------------------------

            mesh->data->vertex_tex_coords[vert_pos].s = u1;
            mesh->data->vertex_tex_coords[vert_pos].t = v1;
            mesh->data->vertices[vert_pos].x = x2;
            mesh->data->vertices[vert_pos].y = y2;
            mesh->data->vertices[vert_pos].z = r2;
            ++vert_pos;
            mesh->data->vertex_tex_coords[vert_pos].s = u0;
            mesh->data->vertex_tex_coords[vert_pos].t = v1;
            mesh->data->vertices[vert_pos].x = x3;
            mesh->data->vertices[vert_pos].y = y3;
            mesh->data->vertices[vert_pos].z = (float)sqrt(fabs(1.0f-(x3*x3+y3*y3)))*hemispheric_v;
            ++vert_pos;
            mesh->data->vertex_tex_coords[vert_pos].s = u0;
            mesh->data->vertex_tex_coords[vert_pos].t = v0;
            mesh->data->vertices[vert_pos].x = x0;
            mesh->data->vertices[vert_pos].y = y0;
            mesh->data->vertices[vert_pos].z = r0;
            ++vert_pos;

            mesh->data->faces[face_pos].a = vert_pos-3;
            mesh->data->faces[face_pos].b = vert_pos-2;
            mesh->data->faces[face_pos].c = vert_pos-1;
            ++face_pos;

        }
      loading_done = true;
      mesh->timestamp++;
    }
    mesh_result->set(mesh);
  }
}
};
