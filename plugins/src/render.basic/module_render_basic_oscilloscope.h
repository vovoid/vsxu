class module_render_basic_oscilloscope : public vsx_module {
  // in
  vsx_module_param_float_array* data_in;
	vsx_module_param_float3* position;
	vsx_module_param_float3* size;
	vsx_module_param_float* angle;
	vsx_module_param_float* line_width;
	vsx_module_param_float3* rotation_axis;
	vsx_module_param_float4* color_rgb;
	// out
	vsx_module_param_render* render_result;
	// internal
	vsx_float_array* data; // our default value

public:


void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;oscilloscopes;simple_oscilloscope";
  info->in_param_spec = 
"\
data_in:float_array,\
spatial:complex\
{\
  position:float3,\
  angle:float?\
    smooth=2,\
  rotation_axis:float3,\
  size:float3\
},\
color:float4,\
line_width:float\
";
  
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
}


void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  data_in = (vsx_module_param_float_array*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT_ARRAY,"data_in");
  
	position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "position");
	position->set(0.0f, 0);
	position->set(0.0f, 1);
	position->set(0.0f, 2);
	
	size = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "size");
	size->set(1.0f,0);
	size->set(1.0f,1);
	angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "angle");
	angle->set(0.0f);

	line_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "line_width");
	line_width->set(2.0f);

	rotation_axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "rotation_axis");
	rotation_axis->set(1.0f, 0);
	rotation_axis->set(1.0f, 1);
	rotation_axis->set(0.0f, 2);
  color_rgb = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color");
  color_rgb->set(1.0f,0);
  color_rgb->set(1.0f,1);
  color_rgb->set(1.0f,2);
  color_rgb->set(1.0f,3);
	
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	render_result->set(0);
}

vsx_array<float> xposs;

void output(vsx_module_param_abs* param)
{
  VSX_UNUSED(param);
  loading_done = true;
  data = data_in->get_addr();
  if (!data) {
    render_result->set(0);
    return;
  }
  if (data->data->size()) {
    if (data->data->size()) {
    	glMatrixMode(GL_MODELVIEW);
    	glPushMatrix();
  	  glEnable(GL_LINE_SMOOTH);

    	glTranslatef(position->get(0),position->get(1),position->get(2));

    	glRotatef((float)angle->get()*360, rotation_axis->get(0), rotation_axis->get(1), rotation_axis->get(2));

    	glScalef(size->get(0), size->get(1), size->get(2));

    	glColor4f(color_rgb->get(0),color_rgb->get(1),color_rgb->get(2),color_rgb->get(3));
      glLineWidth(line_width->get());
    	glBegin(GL_LINE_STRIP);
      	if (xposs.size() != data->data->size())
      	{
          float mul = (2.0f/(float)(data->data->size()-1));
      	  for (unsigned long i = 0; i < data->data->size(); ++i) {
      	    xposs[i] = -1.0f + mul * (float)i;
      	  }
      	}
      	float* xps = xposs.get_pointer();
        for (unsigned long i = 0; i < data->data->size(); ++i) {
          glVertex2f(*xps,(*(data->data))[i]);
          ++xps;
        }  
    	glEnd();
    	glPopMatrix();
    } 	
  } 	
  render_result->set(1);
}

};

