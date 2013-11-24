const unsigned int lights[] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};

class module_render_gl_light_directional : public vsx_module
{
  GLfloat ar[4];

  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* light_id;
  vsx_module_param_int* enabled;
  vsx_module_param_float3* position;
  vsx_module_param_float4* ambient_color;
  vsx_module_param_float4* diffuse_color;
  vsx_module_param_float4* specular_color;
  // out
  vsx_module_param_render* render_result;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;light_directional";
    info->description =
    "A light always hitting the triangles\n"
    "from the same direction given\n"
    "in [position]. This module modifies \n"
    "one lightsource, you can have 8 lights\n"
    "(nested) at the same time depending\n"
    "on your OpenGL drivers.\n";

    info->in_param_spec =
      "render_in:render,"
      "properties:complex{"
        "light_id:enum?0|1|2|3|4|5|6|7,"
        "enabled:enum?NO|YES,"
        "position:float3,"
        "ambient_color:float4?default_controller=controller_col,"
        "diffuse_color:float4?default_controller=controller_col,"
        "specular_color:float4?default_controller=controller_col"
      "}"
    ;
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;

    light_id = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"light_id");
    enabled = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"enabled");
    position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
    position->set(0,0);
    position->set(0,1);
    position->set(1,2);

    ambient_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"ambient_color");
    ambient_color->set(0,0);
    ambient_color->set(0,1);
    ambient_color->set(0,2);
    ambient_color->set(1,3);

    diffuse_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"diffuse_color");
    diffuse_color->set(1,0);
    diffuse_color->set(1,1);
    diffuse_color->set(1,2);
    diffuse_color->set(1,3);

    specular_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"specular_color");
    specular_color->set(1,0);
    specular_color->set(1,1);
    specular_color->set(1,2);
    specular_color->set(1,3);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  }



bool activate_offscreen()
{
  if (enabled->get())
  {
    ar[0] = position->get(0);
    ar[1] = position->get(1);
    ar[2] = position->get(2);
    ar[3] = 0;
    glLightfv(lights[light_id->get()],GL_POSITION,ar);

    ar[0] = ambient_color->get(0);
    ar[1] = ambient_color->get(1);
    ar[2] = ambient_color->get(2);
    ar[3] = ambient_color->get(3);
    glLightfv(lights[light_id->get()],GL_AMBIENT,ar);

    ar[0] = diffuse_color->get(0);
    ar[1] = diffuse_color->get(1);
    ar[2] = diffuse_color->get(2);
    ar[3] = diffuse_color->get(3);
    glLightfv(lights[light_id->get()],GL_DIFFUSE,ar);

    ar[0] = specular_color->get(0);
    ar[1] = specular_color->get(1);
    ar[2] = specular_color->get(2);
    ar[3] = specular_color->get(3);
    glLightfv(lights[light_id->get()],GL_SPECULAR,ar);
    glEnable(lights[light_id->get()]);
    glEnable(GL_LIGHTING);
  }
  return true;
}

void deactivate_offscreen() {
  if (enabled->get()) {
    glDisable(lights[light_id->get()]);
    glDisable(GL_LIGHTING);
  }
}


void output(vsx_module_param_abs* param)
{
  VSX_UNUSED(param);
  render_result->set(render_in->get());
}

};
