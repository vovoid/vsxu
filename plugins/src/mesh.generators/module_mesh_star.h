typedef struct
{
  vsx_quaternion<> pos;
  float speed;
} star_line;

class star_worm
{
public:
  float size;
  vsx_color<> color;
  vsx_quaternion<> phs;
  vsx_quaternion<> freq;
  vsx_nw_vector_nd<star_line> lines;
  star_worm() {
    for (int i = 0; i < 20; ++i) {
      star_line line;
      line.pos.x = 0;
      line.pos.y = 0;
      line.pos.z = 0;
      line.pos.w = 1;
      line.speed = (float)(i+3)*0.1f;

      lines.push_back(line);

    }
  }
};

class module_mesh_star : public vsx_module {
  // in
  vsx_module_param_float_array* paths;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  bool first_run;

  float trail_length;

  vsx_nw_vector<star_worm*> worms;
  vsx_float_array* spectrum;

public:

  bool init() {
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
      "mesh;particles;mesh_star";

    info->description = "";

    info->in_param_spec =
      "paths:float_array";

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    paths = (vsx_module_param_float_array*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"paths");
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    first_run = true;
  }

  void run()
  {
    if (first_run) {
      for (int j = 0; j < 100; ++j) {
        star_worm* my_worm = new star_worm;
        my_worm->size = 0.0f;
        my_worm->freq.x = (rand()%10000)*0.0001f-0.5f;
        my_worm->freq.y = (rand()%10000)*0.0001f-0.5f;
        my_worm->freq.z = (rand()%10000)*0.0001f-0.5f;
        my_worm->freq.w = (rand()%10000)*0.0001f-0.5f;

        my_worm->phs.x = (rand()%10000)*0.0001f-0.5f;
        my_worm->phs.y = (rand()%10000)*0.0001f-0.5f;
        my_worm->phs.z = (rand()%10000)*0.0001f-0.5f;
        my_worm->phs.w = (rand()%10000)*0.0001f-0.5f;
        my_worm->color = vsx_color<>(
            (rand()%10000)*0.0001f, 
            (rand()%10000)*0.0001f,
            (rand()%10000)*0.0001f,
            0.5f
          );
        worms.push_back(my_worm);
      }
      first_run = false;
    }
    spectrum = paths->get_addr();
    vsx_quaternion<> quat;
    int jj = 0;
    for (unsigned long i = 0; i < worms.size(); ++i) {
      quat.x = sin(worms[i]->freq.x*engine_state->vtime+worms[i]->phs.x);
      quat.y = sin(worms[i]->freq.y*engine_state->vtime+worms[i]->phs.y);
      quat.z = sin(worms[i]->freq.z*engine_state->vtime+worms[i]->phs.z);
      quat.w = sin(worms[i]->freq.w*engine_state->vtime+worms[i]->phs.w);
      quat.normalize();
      mesh->data->vertices[jj] = vsx_vector3<>(0.0f);
      mesh->data->vertex_colors[jj] = vsx_color<>();
      ++jj;
      for (unsigned long k = 0; k < worms[i]->lines.size(); ++k) {
        vsx_quaternion<> qq = worms[i]->lines[k].pos;
        float tt = engine_state->dtime * worms[i]->lines[k].speed;
        worms[i]->lines[k].pos.slerp(qq, quat, tt);
        vsx_matrix<float> mat2 = worms[i]->lines[k].pos.matrix();
        mesh->data->vertices[jj] = mat2.multiply_vector(vsx_vector3<>(1.0f,0,0));
        mesh->data->vertex_colors[jj] = worms[i]->color;
        ++jj;
      }
    }
    mesh->timestamp++;
    result->set_p(mesh);
  }
};
