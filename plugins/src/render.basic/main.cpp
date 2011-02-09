#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"
#include "vsx_math_3d.h"


class vsx_module_render_basic_colored_rectangle : public vsx_module {
  // in
	vsx_module_param_float3* position;
	vsx_module_param_float3* size;
	vsx_module_param_float* angle;
	vsx_module_param_float3* rotation_axis;
	vsx_module_param_float4* color_rgb;
	vsx_module_param_int* border;
	vsx_module_param_float* border_width;
	vsx_module_param_float4* border_color;
	// out
	vsx_module_param_render* render_result;
	// internal
	
public:
	
	void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;basic;colored_rectangle||renderers;examples;simple";
    info->description = "\
Renders a filled rectangle\n\
with optional border.\n\
Supports size (x+y), rotating around an axis,\n\
position and color (rgba).\
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
  /* &min=0&help=`This controls the angle of the line\nrotated around the [rotation_axis].\nHint: Hook it to an oscillator or similar\nfor some fun twisting action!`,\*/
//  info->in_param_spec = "spatial:complex{position:float3,angle:float,rotation_axis:float3,size:float3},color:float4";
    info->in_param_spec = "spatial:complex{position:float3,angle:float,rotation_axis:float3,size:float3},border:complex{border_enable:enum?no|yes,border_width:float,border_color:float4},color:float4";
  
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

  	border = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "border_enable");
  	border_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "border_width");
		border_width->set(1.5f);

    border_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "border_color");
    border_color->set(0.0f,0);
    border_color->set(0.0f,1);
    border_color->set(0.0f,2);
    border_color->set(1.0f,3);

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

  void output(vsx_module_param_abs* param) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glTranslatef(position->get(0),position->get(1),position->get(2));
    glRotatef((float)angle->get()*360, rotation_axis->get(0), rotation_axis->get(1), rotation_axis->get(2));
    
    glScalef(size->get(0), size->get(1), size->get(2));

    #ifndef VSXU_OPENGL_ES_2_0
      glColor4f(color_rgb->get(0),color_rgb->get(1),color_rgb->get(2),color_rgb->get(3));
    #endif
    #ifdef VSXU_OPENGL_ES
      const GLfloat square_vertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f,  -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f,   1.0f, 0.0f
      };
      const GLfloat square_colors[] = {
        color_rgb->get(0), color_rgb->get(1),color_rgb->get(2),color_rgb->get(3),
        color_rgb->get(0), color_rgb->get(1),color_rgb->get(2),color_rgb->get(3),
        color_rgb->get(0), color_rgb->get(1),color_rgb->get(2),color_rgb->get(3),
        color_rgb->get(0), color_rgb->get(1),color_rgb->get(2),color_rgb->get(3),
      };
      #ifdef VSXU_OPENGL_ES_1_0
        glDisableClientState(GL_COLOR_ARRAY);
        glVertexPointer(2, GL_FLOAT, 0, square_vertices);
        glEnableClientState(GL_VERTEX_ARRAY);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
      #endif
      #ifdef VSXU_OPENGL_ES_2_0
        vsx_es_begin();
        glEnableVertexAttribArray(0);
        vsx_es_set_default_arrays((GLvoid*)&square_vertices, (GLvoid*)&square_colors);
        //glVertexAttribPointer(0,2,GL_FLOAT, GL_FALSE, 0, squareVertices);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        vsx_es_end();
      #endif
    #endif

    #ifndef VSXU_OPENGL_ES
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
        glLineWidth(border_width->get());
        glBegin(GL_LINE_STRIP);
          glColor4f(border_color->get(0),border_color->get(1),border_color->get(2),border_color->get(3));
          glVertex3f(-1, -1.0f, 0);
          glVertex3f(-1, 1.0f, 0);
          glVertex3f( 1, 1.0f, 0);
          glVertex3f( 1, -1.0f, 0);
          glVertex3f(-1, -1.0f, 0);
        glEnd();
      }
    #endif
  	glPopMatrix();
    render_result->set(1);
    loading_done = true;
  }
};

//###############################
// MODULE RENDER LINE

class vsx_module_render_line : public vsx_module {
  // in
  vsx_module_param_float3* point_a;
  vsx_module_param_float3* point_b;
  vsx_module_param_float4* color_a;
  vsx_module_param_float4* color_b;
  vsx_module_param_float* width;
  // out
  vsx_module_param_render* render_result;
  // internal
	
public:
	
	void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;basic;render_line";
    info->description = "Renders a line\n"
          "with width and start/stop color.";

    info->in_param_spec = "spatial:complex{"
                            "point_a:float3,"
                            "point_b:float3,"
                            "color_a:float4,"
                            "color_b:float4,"
                            "width:float}";
      
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    point_a = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "point_a");
    point_a->set(0.0f, 0);
    point_a->set(0.0f, 1);
    point_a->set(0.0f, 2);

    point_b = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "point_b");
    point_b->set(1.0f, 0);
    point_b->set(0.0f, 1);
    point_b->set(0.0f, 2);

    color_a = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color_a");
    color_a->set(1.0f,0);
    color_a->set(1.0f,1);
    color_a->set(1.0f,2);
    color_a->set(1.0f,3);

    color_b = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color_b");
    color_b->set(1.0f,0);
    color_b->set(1.0f,1);
    color_b->set(1.0f,2);
    color_b->set(0.0f,3);
    
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "width");
    width->set(1.0f);
    
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
  } 

	void output(vsx_module_param_abs* param) {
#ifdef VSXU_OPENGL_ES
	GLfloat line_vertices[] = {
		point_a->get(0), point_a->get(1), point_a->get(2),
		point_b->get(0), point_b->get(1), point_b->get(2),
	};

  #ifdef VSXU_OPENGL_ES_1_0
    GLfloat line_colors[] = {
      color_a->get(0),color_a->get(1),color_a->get(2),color_a->get(3),
      color_b->get(0),color_b->get(1),color_b->get(2),color_b->get(3),
    };
    glVertexPointer(3, GL_FLOAT, 0, line_vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(4, GL_FLOAT, 0, line_colors);
    glEnableClientState(GL_COLOR_ARRAY);
  #endif
  #ifdef VSXU_OPENGL_ES_2_0
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT, GL_FALSE, 0, line_vertices);
	#endif
	
	glDrawArrays(GL_LINE_STRIP, 0, 2);
#endif
#ifndef VSXU_OPENGL_ES
    glBegin(GL_LINES);
      glColor4f(color_a->get(0),color_a->get(1),color_a->get(2),color_a->get(3));
      glVertex3f(point_a->get(0), point_a->get(1), point_a->get(2));   
  
      glColor4f(color_b->get(0),color_b->get(1),color_b->get(2),color_b->get(3));
      glVertex3f(point_b->get(0), point_b->get(1), point_b->get(2));
    glEnd();
#endif
  	render_result->set(1);
    loading_done = true;
  }
};



class vsx_module_simple_with_texture : public vsx_module {
  // in
	vsx_module_param_float3* position;
	vsx_module_param_float* opacity;
	vsx_module_param_float* size;
  vsx_module_param_float* x_aspect;
	vsx_module_param_texture* tex_inf;
	vsx_module_param_float* angle;
	vsx_module_param_float4* color_multiplier;
	vsx_module_param_float4* color_center;
	vsx_module_param_float4* color_a;
	vsx_module_param_float4* color_b;
	vsx_module_param_float4* color_c;
	vsx_module_param_float4* color_d;
	vsx_module_param_float3* tex_coord_a;
	vsx_module_param_float3* tex_coord_b;
  vsx_module_param_int* facing_camera;
	// out
	vsx_module_param_render* render_result;
	
	float tax, tay, tbx, tby;
  vsx_color cm;

public:


void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;basic;textured_rectangle||renderers;examples;simple_with_texture";
  info->in_param_spec = "spatial:complex{position:float3,size:float,angle:float,x_aspect_ratio:float,tex_coord_a:float3,tex_coord_b:float3,facing_camera:enum?no|yes},color:complex{global_alpha:float,color_multiplier:float4,color_center:float4,color_a:float4,color_b:float4,color_c:float4,color_d:float4},texture_in:texture";
  info->description = 
"Renders a textured rectangle.\n"
"It has the following featurs:\n"
"- facing camera (billboard)\n"
"- variable texture aspect ratio\n"
"- colors in center and 4 corners";
    
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
}

GLfloat blobMat[16];
GLfloat blobVec0[4];
GLfloat blobVec1[4];


void beginBlobs() {
	glEnable(GL_TEXTURE_2D);
	GLfloat tmpMat[16];
  #ifdef VSXU_OPENGL_ES
    // TODO
  #else
    glGetFloatv(GL_MODELVIEW_MATRIX, blobMat);
  #endif
  
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	#ifdef VSXU_OPENGL_ES
	  // TODO
  #else
  glGetFloatv(GL_PROJECTION_MATRIX, tmpMat);
  #endif
	tmpMat[3] = 0;
	tmpMat[7] = 0;
	tmpMat[11] = 0;
	tmpMat[12] = 0;
	tmpMat[13] = 0;
	tmpMat[14] = 0;
	tmpMat[15] = 1;

	v_norm(tmpMat);
	v_norm(tmpMat + 4);
	v_norm(tmpMat + 8);
	
	glLoadIdentity();
	glMultMatrixf(tmpMat);

	blobMat[3] = 0;
	blobMat[7] = 0;
	blobMat[11] = 0;
	blobMat[12] = 0;
	blobMat[13] = 0;
	blobMat[14] = 0;
	blobMat[15] = 1;

	v_norm(blobMat);
	v_norm(blobMat + 4);
	v_norm(blobMat + 8);
	
	glMultMatrixf(blobMat);
  #ifdef VSXU_OPENGL_ES_2_0
    // TODO
    
  #else
    glGetFloatv(GL_PROJECTION_MATRIX, blobMat);
  #endif
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	//inv_mat(blobMat, blobMatInv);

	GLfloat upLeft[] = {-0.5f, 0.5f, 0.0f, 1.0f};
	GLfloat upRight[] = {0.5f, 0.5f, 0.0f, 1.0f};

	mat_vec_mult(blobMat, upLeft, blobVec0);
	mat_vec_mult(blobMat, upRight, blobVec1);
}
void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
	opacity = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "global_alpha");
	opacity->set(1.0f);

	facing_camera = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "facing_camera");
	
	x_aspect = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "x_aspect_ratio");
	x_aspect->set(1.0f);

	size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size");
	size->set(1.0f);
	// critical parameter, without this it won't run
	tex_inf = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in",true,true);

	angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "angle");
	
	color_multiplier = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_multiplier");
	color_center = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_center");
	color_a = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_a");
	color_b = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_b");
	color_c = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_c");
	color_d = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_d");
	color_multiplier->set(1,0);
	color_multiplier->set(1,1);
	color_multiplier->set(1,2);
	color_multiplier->set(1,3);

	color_center->set(1,0);
	color_center->set(1,1);
	color_center->set(1,2);
	color_center->set(1,3);

	color_a->set(1,0);
	color_a->set(1,1);
	color_a->set(1,2);
	color_a->set(1,3);

	color_b->set(1,0);
	color_b->set(1,1);
	color_b->set(1,2);
	color_b->set(1,3);
	
	color_c->set(1,0);
	color_c->set(1,1);
	color_c->set(1,2);
	color_c->set(1,3);

	color_d->set(1,0);
	color_d->set(1,1);
	color_d->set(1,2);
	color_d->set(1,3);

	tex_coord_a = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"tex_coord_a");
	tex_coord_b = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"tex_coord_b");
  tex_coord_a->set(0,0);
  tex_coord_a->set(0,1);
  tex_coord_a->set(0,2);
  tex_coord_b->set(1,0);
  tex_coord_b->set(1,1);
  tex_coord_b->set(0,2);

	position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  render_result->set(0);
}	
void output(vsx_module_param_abs* param) {
	//if (tex_inf)
  {
	//printf("simple_renderer_texture::output\n");
	vsx_texture** t_inf;
	t_inf = tex_inf->get_addr();
  if (!t_inf) {
    render_result->set(0);
    return;
  }
  if (!((*t_inf)->valid)) {
    render_result->set(0);
    return;
  }
  //printf("rtr %d",t_inf->rt);
  
//	int texture_id = t_inf->texture_info.get_id();
//	int texture_type = t_inf->texture_info.get_type();
	vsx_transform_obj& texture_transform = *(*t_inf)->get_transform();
	
	float obj_size = size->get();

//	glMatrixMode(GL_PROJECTION);
//	glPushMatrix();
//	glLoadIdentity();
//	gluPerspective(90, 1, 0.1, 10);
//	glTranslatef(0, 0, -1.0);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();

	if ((*t_inf)->get_transform()) texture_transform();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
//	glLoadIdentity();
	
	glTranslatef(position->get(0),position->get(1),position->get(2));

	glRotatef((float)angle->get()*360, 0, 0, 1);

	glScalef(obj_size*x_aspect->get(), obj_size, obj_size);

//	GLboolean oldTexStatus = glIsEnabled(texture_type);
//	GLboolean oldDepStatus = glIsEnabled(GL_DEPTH_TEST);
//	glEnable(texture_type);
//	glDisable(GL_DEPTH_TEST);
//	glBindTexture(texture_type, texture_id);
  (*t_inf)->bind();
  float alpha = opacity->get();
  if (alpha < 0) alpha = 0;

  
  cm.r = color_multiplier->get(0);
  cm.g = color_multiplier->get(1);
  cm.b = color_multiplier->get(2);
  cm.a = color_multiplier->get(3)*opacity->get();
  tax = tex_coord_a->get(0);
  tay = tex_coord_a->get(1);
  tbx = tex_coord_b->get(0);
  tby = tex_coord_b->get(1);
  if (facing_camera->get()) {
    beginBlobs();

    GLfloat tmpVec0[] = {blobVec0[0]*2, blobVec0[1]*2, blobVec0[2]*2};
    GLfloat tmpVec1[] = {blobVec1[0]*2, blobVec1[1]*2, blobVec1[2]*2};
		#ifdef VSXU_OPENGL_ES
			GLfloat fan_vertices[] = {
				0,0,0,
				-tmpVec0[0], -tmpVec0[1], -tmpVec0[2],
				tmpVec1[0],  tmpVec1[1], tmpVec1[2],
				tmpVec0[0],  tmpVec0[1], tmpVec0[2],
				-tmpVec1[0], -tmpVec1[1], -tmpVec1[2],
				-tmpVec0[0], -tmpVec0[1], -tmpVec0[2],
			};

      #ifdef VSXU_OPENGL_ES_1_0
        GLfloat fan_colors[] = {
          cm.r*color_center->get(0), cm.g*color_center->get(1) , cm.b*color_center->get(2),cm.a*color_center->get(3),
          cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3),
          cm.r*color_b->get(0), cm.g*color_b->get(1)   , cm.b*color_b->get(2),cm.a*color_b->get(3),
          cm.r*color_c->get(0), cm.g*color_c->get(1)   , cm.b*color_c->get(2),cm.a*color_c->get(3),
          cm.r*color_d->get(0), cm.g*color_d->get(1)   , cm.b*color_d->get(2),cm.a*color_d->get(3),
          cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3),
        };
        GLfloat fan_texcoords[] = {
          (tbx-tax)*0.5+tax,(tby-tay)*0.5+tay,
          tax, tby,
          tax, tay,
          tbx, tay,
          tbx, tby,
          tax, tby,
        };
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, fan_vertices);
        glColorPointer(4, GL_FLOAT, 0, fan_colors);
        glTexCoordPointer(2, GL_FLOAT, 0, fan_texcoords);
      #endif

      #ifdef VSXU_OPENGL_ES_2_0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, fan_vertices);
      #endif

			glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
		#endif
		#ifndef VSXU_OPENGL_ES
			glBegin(GL_TRIANGLE_FAN);
				glColor4f(cm.r*color_center->get(0), cm.g*color_center->get(1) , cm.b*color_center->get(2),cm.a*color_center->get(3));  (*t_inf)->texcoord2f((tbx-tax)*0.5f+tax,(tby-tay)*0.5f+tay);
				glVertex3f(0,0,0);
      
				glColor4f(cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3));	(*t_inf)->texcoord2f(tax, tby);
				glVertex3f(-tmpVec0[0], -tmpVec0[1], -tmpVec0[2]);
				
				glColor4f(cm.r*color_b->get(0), cm.g*color_b->get(1)   , cm.b*color_b->get(2),cm.a*color_b->get(3));	(*t_inf)->texcoord2f(tax, tay);
				glVertex3f(tmpVec1[0],  tmpVec1[1], tmpVec1[2]);
      
				glColor4f(cm.r*color_c->get(0), cm.g*color_c->get(1)   , cm.b*color_c->get(2),cm.a*color_c->get(3));	(*t_inf)->texcoord2f(tbx, tay);
				glVertex3f( tmpVec0[0],  tmpVec0[1], tmpVec0[2]);
      
				glColor4f(cm.r*color_d->get(0), cm.g*color_d->get(1)   , cm.b*color_d->get(2),cm.a*color_d->get(3));	(*t_inf)->texcoord2f(tbx, tby);
				glVertex3f(-tmpVec1[0], -tmpVec1[1], -tmpVec1[2]);
      
				glColor4f(cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3));	(*t_inf)->texcoord2f(tax, tby);
				glVertex3f(-tmpVec0[0], -tmpVec0[1], -tmpVec0[2]);
			glEnd();
		#endif
  }
  else
  {
		#ifdef VSXU_OPENGL_ES
			GLfloat fan_vertices[] = {
				0,0,
				-1,-1,
				-1,1,
				1,1,
				1,-1,
				-1,-1,
			};

      #ifdef VSXU_OPENGL_ES_1_0
        GLfloat fan_colors[] = {
          cm.r*color_center->get(0), cm.g*color_center->get(1) , cm.b*color_center->get(2),cm.a*color_center->get(3),
          cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3),
          cm.r*color_b->get(0), cm.g*color_b->get(1)   , cm.b*color_b->get(2),cm.a*color_b->get(3),
          cm.r*color_c->get(0), cm.g*color_c->get(1)   , cm.b*color_c->get(2),cm.a*color_c->get(3),
          cm.r*color_d->get(0), cm.g*color_d->get(1)   , cm.b*color_d->get(2),cm.a*color_d->get(3),
          cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3),
        };
        GLfloat fan_texcoords[] = {
          (tbx-tax)*0.5+tax,(tby-tay)*0.5+tay,
          tax, tay,
          tax, tby,
          tbx, tby,
          tbx, tay,
          tax, tay,
        };
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, fan_vertices);
        glColorPointer(4, GL_FLOAT, 0, fan_colors);
        glTexCoordPointer(2, GL_FLOAT, 0, fan_texcoords);
      #endif

      #ifdef VSXU_OPENGL_ES_2_0
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, fan_vertices);
      #endif
      
			glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
		#endif
		#ifndef VSXU_OPENGL_ES
			glBegin(GL_TRIANGLE_FAN);
				glColor4f(cm.r*color_center->get(0), cm.g*color_center->get(1) , cm.b*color_center->get(2),cm.a*color_center->get(3));  (*t_inf)->texcoord2f((tbx-tax)*0.5f+tax,(tby-tay)*0.5f+tay);
				glVertex3i(0,0,0);
				glColor4f(cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3));	(*t_inf)->texcoord2f(tax, tay);	glVertex3f(-1, -1, 0);
				glColor4f(cm.r*color_b->get(0), cm.g*color_b->get(1)   , cm.b*color_b->get(2),cm.a*color_b->get(3));	(*t_inf)->texcoord2f(tax, tby);	glVertex3f(-1,  1, 0);
				glColor4f(cm.r*color_c->get(0), cm.g*color_c->get(1)   , cm.b*color_c->get(2),cm.a*color_c->get(3));	(*t_inf)->texcoord2f(tbx, tby);	glVertex3f( 1,  1, 0);
				glColor4f(cm.r*color_d->get(0), cm.g*color_d->get(1)   , cm.b*color_d->get(2),cm.a*color_d->get(3));	(*t_inf)->texcoord2f(tbx, tay);	glVertex3f( 1, -1, 0);
				glColor4f(cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3));	(*t_inf)->texcoord2f(tax, tay);	glVertex3f(-1, -1, 0);
			glEnd();
		#endif
  }

  (*t_inf)->_bind();

	glPopMatrix();
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

  render_result->set(1);
  return;
  }
  render_result->set(0);
}

};


//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
#if BUILDING_DLL
vsx_module* MOD_CM(unsigned long module) {
  switch (module) {
    case 0: return (vsx_module*)new vsx_module_simple_with_texture;
    case 1: return (vsx_module*)new vsx_module_render_basic_colored_rectangle;
    case 2: return (vsx_module*)new vsx_module_render_line;
  } // switch
  return 0;
}


void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_simple_with_texture*)m; break;
    case 1: delete (vsx_module_render_basic_colored_rectangle*)m; break;
    case 2: delete (vsx_module_render_line*)m; break;
  }
}


unsigned long MOD_NM() {
  return 3;
}  

#endif
