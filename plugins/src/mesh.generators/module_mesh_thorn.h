const float incr = PI_FLOAT/15.0f;
const float f_end = PI_FLOAT*2;

const float b_end = 0.999999f;
const float b_mult = 0.999999f/20.0f;

class module_mesh_thorn : public vsx_module
{
  // in
  vsx_module_param_float* size;
  vsx_module_param_float* angle;
  vsx_module_param_float* red;
  vsx_module_param_float* green;
  vsx_module_param_float* blue;
  vsx_module_param_float3* rotation_axis;
  // out
  vsx_module_param_mesh* mesh_result;
  // internal
  GLUquadricObj *quadratic;
  vsx_mesh* mesh;


  float mfunc(float b) {
    float a = 0.2f-(float)fabs(0.2f*b*b);
    if (a < 0.0f) a = 0.00001f;
    return a;
  }

  float sfunc(float b) {
    return b*2;
  }

  float dfunc(float b) {
    return b*b;
  }

  float gfunc(float b)
  {
    VSX_UNUSED(b);
    return 0.4;
  }
public:
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
  info->identifier = "mesh;vovoid;thorn";
  info->in_param_spec = "";
  info->out_param_spec = "mesh_result:mesh";
  info->component_class = "mesh";
}


void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  VSX_UNUSED(in_parameters);
  mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_result");
  mesh_result->set(mesh);

}


void run()
{
  if (mesh->data->faces.size()) return;

  unsigned long v_pos = 0;
  unsigned long f_pos = 0;
  vsx_vector vtemp;

  float r = 0.2f;
  for (float b = -0.999999f; b < b_end; b+=b_mult) {
    r = mfunc(b);

    vsx_vector p[8];
    p[0].y = sfunc(b+b_mult*2);
    p[1].y = sfunc(b+b_mult*2);
    p[2].y = sfunc(b+b_mult);
    p[3].y = sfunc(b);
    p[4].y = sfunc(b-b_mult);
    p[5].y = sfunc(b-b_mult);
    p[6].y = sfunc(b);
    p[7].y = sfunc(b+b_mult);
    vsx_vector i[4];
    i[0].y = sfunc(b+b_mult);
    i[1].y = sfunc(b+b_mult);
    i[2].y = sfunc(b);
    i[3].y = sfunc(b);
    vsx_vector fn[8];
    vsx_vector fi;
    vsx_vector vn[4];


    for (float a = incr; a < f_end+incr; a+=incr) {
      i[0].x = gfunc(b+b_mult)*mfunc(b+b_mult)*(float)cos(a-incr)+dfunc(b+b_mult);
      i[0].z = mfunc(b+b_mult)*(float)sin(a-incr);
      i[1].x = gfunc(b+b_mult)*mfunc(b+b_mult)*(float)cos(a)+dfunc(b+b_mult);
      i[1].z = mfunc(b+b_mult)*(float)sin(a);
      i[2].x = gfunc(b)*r*(float)cos(a)+dfunc(b);
      i[2].z = r*(float)sin(a);
      i[3].x = gfunc(b)*r*(float)cos(a-incr)+dfunc(b);
      i[3].z = r*(float)sin(a-incr);

      p[0].x = gfunc(b+2*b_mult)*mfunc(b+2*b_mult)*(float)cos(a-incr)+dfunc(b+2*b_mult);
      p[0].z = mfunc(b+2*b_mult)*(float)sin(a-incr);
      p[1].x = gfunc(b+2*b_mult)*mfunc(b+2*b_mult)*(float)cos(a)+dfunc(b+2*b_mult);
      p[1].z = mfunc(b+2*b_mult)*(float)sin(a);
      p[2].x = gfunc(b+b_mult)*mfunc(b+b_mult)*(float)cos(a+incr)+dfunc(b+b_mult);
      p[2].z = mfunc(b+b_mult)*(float)sin(a+incr);
      p[3].x = gfunc(b)*r*(float)cos(a+incr)+dfunc(b);
      p[3].z = r*(float)sin(a+incr);
      p[4].x = gfunc(b-b_mult)*mfunc(b-b_mult)*(float)cos(a)+dfunc(b-b_mult);
      p[4].z = mfunc(b-b_mult)*(float)sin(a);
      p[5].x = gfunc(b-b_mult)*mfunc(b-b_mult)*(float)cos(a-incr)+dfunc(b-b_mult);
      p[5].z = mfunc(b-b_mult)*(float)sin(a-incr);
      p[6].x = gfunc(b)*r*(float)cos(a-incr*2)+dfunc(b);
      p[6].z = r*(float)sin(a-incr*2);
      p[7].x = gfunc(b+b_mult)*mfunc(b+b_mult)*(float)cos(a-incr*2)+dfunc(b+b_mult);
      p[7].z = mfunc(b+b_mult)*(float)sin(a-incr*2.0f);
      if (b+b_mult < 1) {
        fn[0].assign_face_normal(&p[0],&i[0],&p[7]);
        fn[0].normalize();
        fn[1].assign_face_normal(&p[1],&i[0],&p[0]);
        fn[1].normalize();
        fn[2].assign_face_normal(&p[2],&i[1],&p[1]);
        fn[2].normalize();
      } else {
        fn[0].x = fn[0].y = fn[0].z = 0;
        fn[1].x = fn[1].y = fn[1].z = 0;
        fn[2].x = fn[2].y = fn[2].z = 0;
      }

      fn[3].assign_face_normal(&p[2],&p[3],&i[2]);
      fn[3].normalize();
      if (b-b_mult > -1) {
        fn[4].assign_face_normal(&p[3],&p[4],&i[2]);
        fn[4].normalize();
        fn[5].assign_face_normal(&i[2],&p[5],&i[3]);
        fn[5].normalize();
        fn[6].assign_face_normal(&i[3],&p[5],&p[6]);
        fn[6].normalize();
      } else {
        fn[4].x = fn[4].y = fn[4].z = 0;
        fn[5].x = fn[5].y = fn[5].z = 0;
        fn[6].x = fn[6].y = fn[6].z = 0;
      }

      fn[7].assign_face_normal(&i[0],&i[3],&p[6]);
      fn[7].normalize();
      fi.assign_face_normal(&i[1],&i[3],&i[0]);
      fi.normalize();

      vn[0] = fi+fn[0]+fn[1]+fn[7];
      vn[0].normalize();
      vn[1] = fi+fn[1]+fn[2]+fn[3];
      vn[1].normalize();
      vn[2] = fi+fn[3]+fn[4]+fn[5];
      vn[2].normalize();
      vn[3] = fi+fn[5]+fn[6]+fn[7];
      vn[3].normalize();

      mesh->data->vertices[v_pos].set(i[3].x,i[3].y,i[3].z);
      mesh->data->vertex_normals[v_pos].set(vn[3].x,vn[3].y,vn[3].z);
      mesh->data->vertex_colors[v_pos].set(vn[3].x,vn[3].y,vn[3].z,1);
      mesh->data->faces[f_pos].a = v_pos;
      ++v_pos;

      mesh->data->vertices[v_pos].set(i[0].x,i[0].y,i[0].z);
      mesh->data->vertex_normals[v_pos].set(vn[0].x,vn[0].y,vn[0].z);
      mesh->data->vertex_colors[v_pos].set(vn[0].x,vn[0].y,vn[0].z,1);
      mesh->data->faces[f_pos].b = v_pos;
      ++v_pos;

      mesh->data->vertices[v_pos].set(i[1].x,i[1].y,i[1].z);
      mesh->data->vertex_normals[v_pos].set(vn[1].x,vn[1].y,vn[1].z);
      mesh->data->vertex_colors[v_pos].set(vn[1].x,vn[1].y,vn[1].z,1);
      mesh->data->faces[f_pos].c = v_pos;
      ++v_pos;

      ++f_pos;



      mesh->data->vertices[v_pos].set(i[1].x,i[1].y,i[1].z);
      mesh->data->vertex_normals[v_pos].set(vn[1].x,vn[1].y,vn[1].z);
      mesh->data->vertex_colors[v_pos].set(vn[1].x,vn[1].y,vn[1].z,1);
      mesh->data->faces[f_pos].a = v_pos;
      ++v_pos;

      mesh->data->vertices[v_pos].set(i[2].x,i[2].y,i[2].z);
      mesh->data->vertex_normals[v_pos].set(vn[2].x,vn[2].y,vn[2].z);
      mesh->data->vertex_colors[v_pos].set(vn[2].x,vn[2].y,vn[2].z,1);
      mesh->data->faces[f_pos].b = v_pos;
      ++v_pos;

      mesh->data->vertices[v_pos].set(i[3].x,i[3].y,i[3].z);
      mesh->data->vertex_normals[v_pos].set(vn[3].x,vn[3].y,vn[3].z);
      mesh->data->vertex_colors[v_pos].set(vn[3].x,vn[3].y,vn[3].z,1);
      mesh->data->faces[f_pos].c = v_pos;
      ++v_pos;

      ++f_pos;

    }
  }

  mesh->timestamp++;
  mesh_result->set(mesh);

  loading_done = true;
}

};
