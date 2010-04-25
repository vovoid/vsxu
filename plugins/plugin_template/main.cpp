#include "vsxfst.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"
#include "vsx_math_3d.h"



class vsx_module_template : public vsx_module {
  // in
	vsx_module_param_float3* position;
	vsx_module_param_float3* size;
	vsx_module_param_float* angle;
	vsx_module_param_float3* rotation_axis;
	vsx_module_param_float4* color_rgb;
	vsx_module_param_int* border;
	// out
	vsx_module_param_render* render_result;
	// internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "templates;simple";
    /*
     * Parameter syntax:
     * 
     * [name]:[type][?[flag][=value]]
     * 
     * Name and type are required.
     * Flag can be many things, controllers or options on how the GUI should
     * treat it.
     * 
     * NonConnection flag:
     * 		nc=1
     * 
     * 		This will make it impossible to connect the parameter to any other 
     * 		module. That is, only the human operating VSXu can set it. 
     * 		Use this when you do operations that involve large chunks of memory
     *    or heavy one-time precalculations.
     * 
     * Setting Default Controller flags (opened when double-clicking the param):
     * 		default_controller=[value]
     * 
     * 		Different values (controllers) for different parameters are available:
     *  
     *  	controller_knob				float
     *		controller_slider			float, float3, float4
     *  	controller_edit  			string/resource  
     *		controller_resource 	resource (to use it with strings, make it default)
     * 
     * 		Example:
     * 		  To make a knob default (when double-clicking) go like:
     * 		  angle:float?default_controller=controller_knob
     *  
     *  
     *   
     * 
     */
    info->description = "\
Template module showing all\n\
possible data types and tricks\n\
you can do in modules.\n\
";
  /*
  dialog_Test1=(\
    comp=false&\
    size_x=7&\
    size_y=4&\
    control_knob=(\
      pos_x=-0.5&\
      pos_y=0&\
      param=size&\
      type=float&\
      hint=MyHint\
    )&\
    control_slider=(\
      pos_x=0.5&\
      pos_y=0&\
      param=angle&\
      type=float\
    )\
  )\
  */
  /*// &min=0&help=`This controls the angle of the line\nrotated around the [rotation_axis].\nHint: Hook it to an oscillator or similar\nfor some fun twisting action!`,\*/
//  info->in_param_spec = "spatial:complex{position:float3,angle:float,rotation_axis:float3,size:float3},color:float4";
    info->in_param_spec = "\
spatial:complex{position:float3,angle:float,rotation_axis:float3,size:float3,border:enum?no|yes},color:float4";
  
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
  	position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "position");
  	position->set(0.0f, 0);
  	position->set(0.0f, 1);
  	position->set(0.0f, 2);
	
  	size = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "size");
  	size->set(1.0f,0);
  	size->set(0.3f,1);
  	angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "angle");
  	angle->set(0.0f);

  	border = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "border");
	
  	rotation_axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "rotation_axis");
  	rotation_axis->set(1.0f, 0);
  	rotation_axis->set(1.0f, 1);
  	rotation_axis->set(0.0f, 2);
    color_rgb = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color");
    color_rgb->set(1.0,0);
    color_rgb->set(1.0,1);
    color_rgb->set(1.0,2);
    color_rgb->set(1.0,3);

  	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  	render_result->set(0);
  }	

  void output(vsx_module_param_abs* param) {
  	glMatrixMode(GL_MODELVIEW);
  	glPushMatrix();
  	glTranslatef(position->get(0),position->get(1),position->get(2));

  	glRotatef((float)angle->get()*360, rotation_axis->get(0), rotation_axis->get(1), rotation_axis->get(2));

  	glScalef(size->get(0), size->get(1), size->get(2));

  	glColor4f(color_rgb->get(0),color_rgb->get(1),color_rgb->get(2),color_rgb->get(3));

  	glBegin(GL_QUADS);
  	  glTexCoord2f(0.0f,0.0f);
      glVertex3f(-1.0f, -1.0f, 0.0f);
  	  glTexCoord2f(0.0f,1.0f);
      glVertex3f(-1.0f,  1.0f, 0.0f);
  	  glTexCoord2f(1.0f,1.0f);
      glVertex3f( 1.0f,  1.0f, 0.0f);
  	  glTexCoord2f(1.0f,0.0f);
      glVertex3f( 1.0f, -1.0f, 0.0f);
  	glEnd();
  	
  	if (border->get()) {
    	glEnable(GL_LINE_SMOOTH);
    	glLineWidth(1.5);
    	glEnable(GL_BLEND);
    	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    	glBegin(GL_LINE_STRIP);
    		glColor3f(0, 0, 0);
    		glVertex3f(-2, -0.4f, 0);
    		glVertex3f(-2, -0.2f, 0);
    		glVertex3f( 2, -0.2f, 0);
    		glVertex3f( 2, -0.4f, 0);
    		glVertex3f(-2, -0.4f, 0);
    	glEnd();
    }

  	glPopMatrix();
    render_result->set(1);
    loading_done = true;
  }

};



//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////

vsx_module* create_new_module(unsigned long module) {
  switch (module) {
    case 0: return (vsx_module*)new vsx_module_template;
  } // switch
  return 0;
}



void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_template*)m; break;
  }
}


unsigned long get_num_modules() {
  return 2;
}  

