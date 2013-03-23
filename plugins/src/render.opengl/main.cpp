/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "_configuration.h"
//#define VSX_TEXTURE_NO_GLPNG
//#include <string>
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"
#include "vsx_quaternion.h"



const unsigned int matrix_target[] = {
GL_MODELVIEW,
GL_PROJECTION,
GL_TEXTURE};

const unsigned int matrix_target_get[] = {
GL_MODELVIEW_MATRIX,
GL_PROJECTION_MATRIX,
GL_TEXTURE_MATRIX};

const unsigned int sfactors[] = {
GL_ZERO,
GL_ONE,
GL_DST_COLOR,
GL_ONE_MINUS_DST_COLOR,
GL_SRC_ALPHA,
GL_ONE_MINUS_SRC_ALPHA,
GL_DST_ALPHA,
GL_ONE_MINUS_DST_ALPHA,
#ifndef VSXU_OPENGL_ES
GL_CONSTANT_COLOR_EXT,
GL_ONE_MINUS_CONSTANT_COLOR_EXT,
GL_CONSTANT_ALPHA_EXT,
GL_ONE_MINUS_CONSTANT_ALPHA_EXT,
#endif
GL_SRC_ALPHA_SATURATE};
const unsigned int dfactors[] = {
GL_ZERO,
GL_ONE,
GL_SRC_COLOR,
GL_ONE_MINUS_SRC_COLOR,
GL_SRC_ALPHA,
GL_ONE_MINUS_SRC_ALPHA,
GL_DST_ALPHA,
GL_ONE_MINUS_DST_ALPHA,
#ifndef VSXU_OPENGL_ES
GL_CONSTANT_COLOR_EXT,
GL_ONE_MINUS_CONSTANT_COLOR_EXT,
GL_CONSTANT_ALPHA_EXT,
GL_ONE_MINUS_CONSTANT_ALPHA_EXT
#endif
};





#ifdef VSXU_OPENGL_ES
//-----------------------------------------------------------------------
//---- OPENGL ES UTILS --------------------------------------------------
//-----------------------------------------------------------------------

void gluPerspective(double fovy, double aspect, double zNear, double zFar)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	double xmin, xmax, ymin, ymax;

	ymax = zNear * tan(fovy * M_PI / 360.0);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;


	glFrustumf(xmin, xmax, ymin, ymax, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glDepthMask(GL_TRUE);
}

void gluOrtho2D(GLfloat x1,GLfloat y1, GLfloat width, GLfloat height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrthof( x1, width, height, y1, 1, 0 );
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDepthMask(GL_FALSE);
}

void gluLookAt(GLfloat eyex, GLfloat eyey, GLfloat eyez,
							 GLfloat centerx, GLfloat centery, GLfloat centerz,
							 GLfloat upx, GLfloat upy, GLfloat upz)
{
	GLfloat m[16];
	GLfloat x[3], y[3], z[3];
	GLfloat mag;

	/* Make rotation matrix */

	/* Z vector */
	z[0] = eyex - centerx;
	z[1] = eyey - centery;
	z[2] = eyez - centerz;
	mag = sqrt(z[0] * z[0] + z[1] * z[1] + z[2] * z[2]);
	if (mag) {                   /* mpichler, 19950515 */
		z[0] /= mag;
		z[1] /= mag;
		z[2] /= mag;
	}

	/* Y vector */
	y[0] = upx;
	y[1] = upy;
	y[2] = upz;

	/* X vector = Y cross Z */
	x[0] = y[1] * z[2] - y[2] * z[1];
	x[1] = -y[0] * z[2] + y[2] * z[0];
	x[2] = y[0] * z[1] - y[1] * z[0];

	/* Recompute Y = Z cross X */
	y[0] = z[1] * x[2] - z[2] * x[1];
	y[1] = -z[0] * x[2] + z[2] * x[0];
	y[2] = z[0] * x[1] - z[1] * x[0];

	/* mpichler, 19950515 */
	/* cross product gives area of parallelogram, which is < 1.0 for
	 * non-perpendicular unit-length vectors; so normalize x, y here
	 */
	mag = sqrt(x[0] * x[0] + x[1] * x[1] + x[2] * x[2]);
	if (mag) {
		x[0] /= mag;
		x[1] /= mag;
		x[2] /= mag;
	}
	mag = sqrt(y[0] * y[0] + y[1] * y[1] + y[2] * y[2]);
	if (mag) {
		y[0] /= mag;
		y[1] /= mag;
		y[2] /= mag;
	}

#define M(row,col)  m[col*4+row]
	M(0, 0) = x[0];
	M(0, 1) = x[1];
	M(0, 2) = x[2];
	M(0, 3) = 0.0;
	M(1, 0) = y[0];
	M(1, 1) = y[1];
	M(1, 2) = y[2];
	M(1, 3) = 0.0;
	M(2, 0) = z[0];
	M(2, 1) = z[1];
	M(2, 2) = z[2];
	M(2, 3) = 0.0;
	M(3, 0) = 0.0;
	M(3, 1) = 0.0;
	M(3, 2) = 0.0;
	M(3, 3) = 1.0;
#undef M
	glMultMatrixf(m);

	/* Translate Eye to Origin */
	glTranslatef(-eyex, -eyey, -eyez);
}
#endif


//-----------------------------------------------------------------------
//---- GL COLOR ---------------------------------------------------------
//-----------------------------------------------------------------------
class vsx_gl_color : public vsx_module {
  // in
  vsx_module_param_float4* color;
	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_color";
  info->description = "Sets the color for modules\n\
that don't do this themselves.";
  info->in_param_spec =
    "render_in:render,"
    "color:float4?default_controller=controller_col"
  ;
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true; // always run this
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color");
  color->set(1,0);
  color->set(1,1);
  color->set(1,2);
  color->set(1,3);
	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen() {
  // save current matrix
  glColor4f(color->get(0),color->get(1),color->get(2),color->get(3));
  return true;
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}
};



class vsx_module_blend_mode : public vsx_module {
  GLint prev_src, prev_dst;
  GLboolean isblend;
  // in
	vsx_module_param_int* sfactor;
	vsx_module_param_int* dfactor;
	vsx_module_param_float4* blend_color;
	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
	GLfloat col[4];
public:



void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;blend_mode";
  info->description = "Changes the way pixels are blended.\n\
Default is normal transparency:\n\
  GL_SRC_ALPHA,\n\
  GL_ONE_MINUS_SRC_ALPHA\n\
To get sat adding, choose:\n\
  GL_SRC_ALPHA,\n\
  GL_ONE";

  info->in_param_spec =
"render_in:render,"
"source_blend:enum?ZERO|ONE|DST_COLOR|ONE_MINUS_DST_COLOR|SRC_ALPHA|ONE_MINUS_SRC_ALPHA|DST_ALPHA|ONE_MINUS_DST_ALPHA|CONSTANT_COLOR_EXT|ONE_MINUS_CONSTANT_COLOR_EXT|CONSTANT_ALPHA_EXT|ONE_MINUS_CONSTANT_ALPHA_EXT|SRC_ALPHA_SATURATE&nc=1,"
"dest_blend:enum?ZERO|ONE|SRC_COLOR|ONE_MINUS_SRC_COLOR|SRC_ALPHA|ONE_MINUS_SRC_ALPHA|DST_ALPHA|ONE_MINUS_DST_ALPHA|CONSTANT_COLOR_EXT|ONE_MINUS_CONSTANT_COLOR_EXT|CONSTANT_ALPHA_EXT|ONE_MINUS_CONSTANT_ALPHA_EXT&nc=1,"
"blend_color:float4?default_controller=controller_col"
;
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  sfactor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"source_blend");
  sfactor->set(4);
  dfactor = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"dest_blend");
  dfactor->set(5);
  blend_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"blend_color");
  blend_color->set(1,0);
  blend_color->set(1,1);
  blend_color->set(1,2);
  blend_color->set(1,3);
  render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_in->set(0);
  render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}


bool activate_offscreen() {
  glGetIntegerv(GL_BLEND_SRC, &prev_src);
  glGetIntegerv(GL_BLEND_DST, &prev_dst);
#if !defined(__APPLE__)
  if (GLEW_EXT_blend_color)
  glGetFloatv(GL_BLEND_COLOR_EXT,col);
#endif
  isblend = glIsEnabled(GL_BLEND);
	glEnable(GL_BLEND);
	glBlendFunc(sfactors[sfactor->get()], dfactors[dfactor->get()]);
#if !defined(__APPLE__)
    if (GLEW_EXT_blend_color)
    glBlendColor(blend_color->get(0),blend_color->get(1),blend_color->get(2),blend_color->get(3));
#endif
	return true;
}

void deactivate_offscreen() {
	glBlendFunc(prev_src,prev_dst);
#if !defined(__APPLE__)
  if (GLEW_EXT_blend_color)
  glBlendColor(col[0],col[1],col[2],col[3]);
#endif
	if (!isblend)
	glDisable(GL_BLEND);
}


void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

};
//----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- GL LIGHTING -------------------------------------------------------
//-----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

const unsigned int lights[] = {GL_LIGHT0,GL_LIGHT1,GL_LIGHT2,GL_LIGHT3,GL_LIGHT4,GL_LIGHT5,GL_LIGHT6,GL_LIGHT7};

class vsx_light : public vsx_module {
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



bool activate_offscreen() {
  if (enabled->get()) {
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


void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

};

//----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- GL POLYGON MODE ---------------------------------------------------
//-----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

#ifndef VSXU_OPENGL_ES
unsigned int rendermodes[] = {GL_POINT,GL_LINE,GL_FILL};

class vsx_polygon_mode : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* front;
  GLint p_mode[2];
  vsx_module_param_int* back;
  vsx_module_param_int* smooth;
  GLboolean p_smooth;
  // out
  vsx_module_param_render* render_result;
  // internal
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;rendering_mode";
  info->description = "Changes the way triangles are drawn\n - either as points, lines (wireframe\nor solid (default)";

  info->in_param_spec =
"\
render_in:render,\
back_facing:enum?points|lines|solid,\
front_facing:enum?points|lines|solid,\
smooth_edges:enum?no|yes\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

  front = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"front_facing");
  front->set(2);
  back = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"back_facing");
  back->set(2);
  smooth = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"smooth_edges");
  smooth->set(0);
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen() {
  glGetIntegerv(GL_POLYGON_MODE,&p_mode[0]);
  glPolygonMode(GL_FRONT, rendermodes[front->get()]);
  glPolygonMode(GL_BACK, rendermodes[back->get()]);
  glGetBooleanv(GL_POLYGON_SMOOTH,&p_smooth);
  if (smooth->get()) {
    glHint(GL_POLYGON_SMOOTH_HINT,GL_NICEST);
    glEnable(GL_POLYGON_SMOOTH);
  }
  return true;
}

void deactivate_offscreen() {
  glPolygonMode(GL_FRONT, p_mode[0]);
  glPolygonMode(GL_BACK, p_mode[1]);
  if (p_smooth != smooth->get()) {
    if (p_smooth) glEnable(GL_POLYGON_SMOOTH);
    else
    glDisable(GL_POLYGON_SMOOTH);
  }
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}
};
#endif
unsigned int faces_list[] = {GL_FRONT,GL_BACK,GL_FRONT_AND_BACK};

//----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- GL MATERIAL PARAM -------------------------------------------------
//-----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
class vsx_material_param : public vsx_module {
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* faces_affected;

	vsx_module_param_float4  *ambient_reflectance,
                           *diffuse_reflectance,
                           *specular_reflectance,
                           *emission_intensity;
  vsx_module_param_float *specular_exponent;
	// out
	vsx_module_param_render* render_result;
	// internal
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat emission[4];
	GLfloat spec_exp;

public:

	void module_info(vsx_module_info* info)
	{
	  info->identifier = "renderers;opengl_modifiers;material_param";
	  info->description = "Modifies the way triangles process\n"
												"light from a lightsource.";
	  info->in_param_spec =	"render_in:render,"
													"faces_affected:enum?front_facing|back_facing|front_and_back,"
													"properties:complex{"
                            "ambient_reflectance:float4?default_controller=controller_col,"
                            "diffuse_reflectance:float4?default_controller=controller_col,"
                            "specular_reflectance:float4?default_controller=controller_col,"
                            "emission_intensity:float4?default_controller=controller_col,"
													  "specular_exponent:float"
													"}";
	  info->out_param_spec = "render_out:render";
	  info->component_class = "render";
	  info->tunnel = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
		render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
	  faces_affected = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"faces_affected");
	  faces_affected->set(2);
	  ambient_reflectance = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"ambient_reflectance");
	  ambient_reflectance->set(0.2f,0);
	  ambient_reflectance->set(0.2f,1);
	  ambient_reflectance->set(0.2f,2);
	  ambient_reflectance->set(1.0f,3);

	  diffuse_reflectance = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"diffuse_reflectance");
	  diffuse_reflectance->set(0.8f,0);
	  diffuse_reflectance->set(0.8f,1);
	  diffuse_reflectance->set(0.8f,2);
	  diffuse_reflectance->set(1.0f,3);

	  specular_reflectance = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"specular_reflectance");
	  specular_reflectance->set(0.0f,0);
	  specular_reflectance->set(0.0f,1);
	  specular_reflectance->set(0.0f,2);
	  specular_reflectance->set(1.0f,3);

	  emission_intensity = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"emission_intensity");
	  emission_intensity->set(0.0f,0);
	  emission_intensity->set(0.0f,1);
	  emission_intensity->set(0.0f,2);
	  emission_intensity->set(1.0f,3);

	  specular_exponent = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"specular_exponent");

		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	}


	bool activate_offscreen() {
	  unsigned int ff = faces_list[faces_affected->get()];
		//GLfloat ambient[4];
		//GLfloat diffuse[4];
		//GLfloat specular[4];
		//GLfloat emission[4];
		//GLfloat spec_exp;
		glGetMaterialfv(ff,GL_AMBIENT,&ambient[0]);
		glGetMaterialfv(ff,GL_DIFFUSE,&diffuse[0]);
		glGetMaterialfv(ff,GL_SPECULAR,&specular[0]);
		glGetMaterialfv(ff,GL_EMISSION,&emission[0]);
		glGetMaterialfv(ff,GL_SHININESS,&spec_exp);

	  /*mm[0] = value->get(0);
	  mm[1] = value->get(1);
	  mm[2] = value->get(2);
	  mm[3] = value->get(3);*/
	  glMaterialfv(ff,GL_AMBIENT,ambient_reflectance->get_addr());
	  glMaterialfv(ff,GL_DIFFUSE,diffuse_reflectance->get_addr());
	  glMaterialfv(ff,GL_SPECULAR,specular_reflectance->get_addr());
	  glMaterialfv(ff,GL_EMISSION,emission_intensity->get_addr());
	  glMaterialfv(ff,GL_SHININESS,specular_exponent->get_addr());
	  return true;
	}

	void deactivate_offscreen() {
	  unsigned int ff = faces_list[faces_affected->get()];
	  glMaterialfv(ff,GL_AMBIENT		,&ambient[0]);
	  glMaterialfv(ff,GL_DIFFUSE		,&diffuse[0]);
	  glMaterialfv(ff,GL_SPECULAR		,&specular[0]);
	  glMaterialfv(ff,GL_EMISSION		,&emission[0]);
	  glMaterialfv(ff,GL_SHININESS	,&spec_exp);
	}

	void output(vsx_module_param_abs* param) {
	  render_result->set(render_in->get());
	}

};



//----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- DEPTH SETTING --------------------------------------------------------
//-----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
class vsx_depth_buffer : public vsx_module {
  // in
	vsx_module_param_render* render_in;
	vsx_module_param_int* depth_test;
	vsx_module_param_int* depth_mask;
	// out
	vsx_module_param_render* render_result;
	// internal
public:

	void module_info(vsx_module_info* info)
	{
  	info->identifier = "renderers;opengl_modifiers;depth_buffer";
  	info->description = "Enables or disables depth testing\n\
for all below in this chain.\n\
This is useful when you have a mesh. \n\
If no depth testing the faces will be drawn in the order that they appear\n\
in the mesh which is almost never good. (unless you\n\
have satadding blend mode). However in order to use this\n\
you have to enable the depth mask which can be done\n\
with the sister module to this one - depth_mask.\n\
More hints...\n\
|||\n\
If you have faces that are semi-transarent,\n\
disable depth testing and use the transparency\n\
renderer.\n\
";

  	info->in_param_spec = "render_in:render,depth_test:enum?DISABLED|ENABLED,depth_mask:enum?DISABLED|ENABLED";
  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
  	info->tunnel = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
	  depth_test = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"depth_test");
	  depth_mask = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"depth_mask");
		render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	}
	GLboolean old_depth_mask;
	GLboolean old_depth_test;
	bool reset_test, reset_mask;
	bool activate_offscreen() {
		reset_mask = false;
		reset_test = false;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &old_depth_mask);
		if (depth_mask->get() != old_depth_mask) {
			reset_mask = true;
			glDepthMask(!old_depth_mask);
		}

	  old_depth_test = glIsEnabled(GL_DEPTH_TEST);
	  if (depth_test->get() != old_depth_test) {
	  	reset_test = true;
	  	if (old_depth_test)
	    glDisable(GL_DEPTH_TEST);
	    else
			{
				glEnable(GL_DEPTH_TEST);
			}
	  }
	  return true;
	}

	void deactivate_offscreen() {
		if (reset_mask) glDepthMask(!old_depth_mask);
		if (reset_test)
		{
			if (old_depth_test)
	    glEnable(GL_DEPTH_TEST);
	    else
	    glDisable(GL_DEPTH_TEST);
		}
	}

	void output(vsx_module_param_abs* param) {
	  render_result->set(render_in->get());
	}
};


//----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- DEPTH FUNCTION --------------------------------------------------------
//-----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

class vsx_depth_func : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* depth_func;
  // out
  vsx_module_param_render* render_result;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;depth_function";
    info->description = "";

    info->in_param_spec = "render_in:render,depth_func:enum?NEVER|LESS|EQUAL|LESS_OR_EQUAL|GREATER|NOT_EQUAL|GREATER_OR_EQUAL|ALWAYS";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    depth_func = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"depth_func");
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  }
  GLint old_depth_func;
  bool reset_func;
  bool activate_offscreen() {
    reset_func = false;
    glGetIntegerv(GL_DEPTH_FUNC, &old_depth_func);
    if (depth_func->get() + GL_NEVER != old_depth_func) {
      reset_func = true;
      glDepthFunc(depth_func->get() + GL_NEVER);
    }
    return true;
  }

  void deactivate_offscreen() {
    if (reset_func) glDepthFunc(old_depth_func);
  }

  void output(vsx_module_param_abs* param) {
    render_result->set(render_in->get());
  }
};


//----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- DEPTH CLEAR --------------------------------------------------------
//-----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
class vsx_depth_buffer_clear : public vsx_module {
  // in
	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
public:

	void module_info(vsx_module_info* info)
	{
  	info->identifier = "renderers;opengl_modifiers;depth_buffer_clear";
  	info->description = "";

  	info->in_param_spec = "render_in:render";
  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
  	info->tunnel = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
		render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	}
	bool activate_offscreen() {
    glClear(GL_DEPTH_BUFFER_BIT);
	  return true;
	}

	void deactivate_offscreen() {
	}

	void output(vsx_module_param_abs* param) {
	  render_result->set(render_in->get());
	}
};

//----------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- BUFFER CLEAR --------------------------------------------------------
//-----------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------
class vsx_buffer_clear : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* clear_color_buffer;
  vsx_module_param_int* clear_depth_buffer;
  vsx_module_param_float4* clear_color;
  // out
  vsx_module_param_render* render_result;
  // internal
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;buffer_clear";
    info->description = "";

    info->in_param_spec =
      "render_in:render,"
      "color_buffer:enum?no|yes,"
      "clear_color:float4?default_controller=controller_col,"
      "depth_buffer:enum?no|yes"
    ;
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    clear_color_buffer = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"color_buffer");
    clear_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"clear_color");
    clear_depth_buffer = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"depth_buffer");
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  }

  bool activate_offscreen()
  {
    glClearColor
    (
      clear_color->get(0),
      clear_color->get(1),
      clear_color->get(2),
      clear_color->get(3)
    );
    glClear(
          GL_DEPTH_BUFFER_BIT * clear_depth_buffer->get()
          |
          GL_COLOR_BUFFER_BIT * clear_color_buffer->get()
    );
    return true;
  }

  void deactivate_offscreen() {
  }

  void output(vsx_module_param_abs* param) {
    render_result->set(render_in->get());
  }
};


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- BACKFACE CULLING --------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


class vsx_backface_cull : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* status;
  // out
  vsx_module_param_render* render_result;
  // internal
public:


void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;backface_culling";
  info->description = "Enables/Disables backfacing faces.";
  info->in_param_spec =
"render_in:render,status:enum?DISABLED|ENABLED";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  status = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"status");
	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}
bool activate_offscreen() {
  // comments?! what are those? :) real coders don't need comments. real coders read opcodes in a hex editor.
  if (status->get()) {
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

  } else {
    glDisable(GL_CULL_FACE);
  }
  return true;
}

void deactivate_offscreen() {
  glDisable(GL_CULL_FACE);
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}
};
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//--- FOG ---------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


class vsx_fog : public vsx_module {
  // in
  vsx_module_param_render* render_in;
  vsx_module_param_int* status;
  vsx_module_param_float4* fog_color;
  vsx_module_param_float* fog_start;
  vsx_module_param_float* fog_end;
  // out
  vsx_module_param_render* render_result;
  // internal
public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_fog";
  info->description = "Enables the fog built into OpenGL";
  info->in_param_spec =
"render_in:render,status:enum?DISABLED|ENABLED,fog_color:float4?default_controller=controller_col,fog_start:float,fog_end:float";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  status = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"status");
  fog_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"fog_color");
  fog_color->set(0,0);
  fog_color->set(0,1);
  fog_color->set(0,2);
  fog_color->set(0,3);
  fog_start = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fog_start");
  fog_start->set(1.0f);
  fog_end = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fog_end");
  fog_end->set(5.0f);
	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}
bool activate_offscreen() {
  GLfloat fogColor[] = {fog_color->get(0), fog_color->get(1), fog_color->get(2), fog_color->get(3)};
	#ifndef VSXU_OPENGL_ES
  glFogi (GL_FOG_MODE, GL_LINEAR);   // Fog fade using exponential function
	#endif
  glFogfv (GL_FOG_COLOR, fogColor);   // Set the fog color
  glFogf (GL_FOG_DENSITY, 0.35f);   // Set the density, don't make it too high.
  //glHint (GL_FOG_HINT, GL_NICEST);
  glFogf(GL_FOG_START, fog_start->get());				// Fog Start Depth
  glFogf(GL_FOG_END, fog_end->get());				// Fog End Depth
  // comments?! what are those? :) real coders don't need comments. real coders read opcodes in a hex editor.
  if (status->get()) {
    glEnable (GL_FOG);   // turn on fog, otherwise you won't see any
  } else {
    glDisable(GL_FOG);
  }
  return true;
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

void deactivate_offscreen() {
  glDisable(GL_FOG);
}
};

//-----------------------------------------------------------------------
//--- FREELOOK CAMERA ---------------------------------------------------
//-----------------------------------------------------------------------
class vsx_freelook_camera : public vsx_module {
  GLfloat tmpMat[16];
  // in
  vsx_module_param_float3* position;
  vsx_module_param_float3* rotation;
  vsx_module_param_float3* upvector;
  vsx_module_param_float* fov;
  vsx_module_param_float* near_clipping;
  vsx_module_param_float* far_clipping;

  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal

public:


void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;cameras;freelook_camera";
  info->description = "A camera that has a position and a rotation\n\
axis set by the user.\nUseful for static cameras.";
  info->in_param_spec =
"\
render_in:render,\
camera:complex{\
  position:float3,\
  rotation:float3,\
  upvector:float3,\
  fov:float,\
  near_clipping:float,\
  far_clipping:float,\
}\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
  rotation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"rotation");
  upvector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"upvector");
  fov = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fov");
  near_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"near_clipping");
  far_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"far_clipping");

  position->set(0,0);
  position->set(0,1);
  position->set(1,2);

  rotation->set(0,0);
  rotation->set(-1,1);
  rotation->set(0,2);
  upvector->set(0,0);
  upvector->set(1,1);
  upvector->set(0,2);

  fov->set(90.0f);
  near_clipping->set(0.01f);
  far_clipping->set(2000.0f);

  render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen() {
  glGetFloatv(GL_PROJECTION_MATRIX, tmpMat);
  glMatrixMode(GL_PROJECTION);
  //glLoadIdentity();
  gluPerspective(fov->get(), 1.0, fabs(near_clipping->get()), far_clipping->get());
	gluLookAt(
	  position->get(0),
	  position->get(1),
	  position->get(2),

	  rotation->get(0)+position->get(0),
	  rotation->get(1)+position->get(1),
	  rotation->get(2)+position->get(2),

	  upvector->get(0),
	  upvector->get(1),
	  upvector->get(2)
	);
  glMatrixMode(GL_MODELVIEW);
  return true;
}

void deactivate_offscreen() {
  // reset the matrix to previous value
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(tmpMat);
}


void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

};


//-----------------------------------------------------------------------
//--- TARGET CAMERA -----------------------------------------------------
//-----------------------------------------------------------------------

void set_correct_perspective(float fov, float near_clip, float far_clip)
{
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  float screenx = (float)fabs((float)(viewport[2]));
  float screeny = (float)fabs((float)(viewport[3]));
  gluPerspective(fov, screenx/screeny, fabs(near_clip), far_clip);
}

class vsx_target_camera : public vsx_module {
  GLfloat tmpMat_proj[16];
  GLfloat tmpMat_model[16];
  // in
  vsx_module_param_float3* position;
  vsx_module_param_float3* destination;
  vsx_module_param_float3* upvector;
  vsx_module_param_float* fov;
  vsx_module_param_float* near_clipping;
  vsx_module_param_float* far_clipping;
  vsx_module_param_int* perspective_correct;
  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal

public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;cameras;target_camera";
  info->description = "A camera that looks from one \nposition to a destination.\n\
Useful if you want to follow an object\n\
with the camera.";
  info->in_param_spec =
"\
render_in:render,\
camera:complex{\
  position:float3,\
  destination:float3,\
  upvector:float3,\
  fov:float,\
  near_clipping:float,\
  far_clipping:float,\
  perspective_correct:enum?no|yes\
}\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
  destination = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"destination");
  upvector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"upvector");
  fov = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fov");
  near_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"near_clipping");
  far_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"far_clipping");
  perspective_correct = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"perspective_correct");

  position->set(0,0);
  position->set(0,1);
  position->set(1,2);

  destination->set(0,0);
  destination->set(0,1);
  destination->set(0,2);
  upvector->set(0,0);
  upvector->set(1,1);
  upvector->set(0,2);

  fov->set(90.0f);
  near_clipping->set(0.01f);
  far_clipping->set(2000.0f);

	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen() {
  glGetFloatv(GL_PROJECTION_MATRIX, tmpMat_proj);
  glGetFloatv(GL_MODELVIEW_MATRIX, tmpMat_model);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
	if (perspective_correct->get())
	  set_correct_perspective(fov->get(),near_clipping->get(),far_clipping->get());
	else
  gluPerspective(fov->get(), 1.0, fabs(near_clipping->get()), far_clipping->get());
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(
	  position->get(0),
	  position->get(1),
	  position->get(2),

	  destination->get(0),
	  destination->get(1),
	  destination->get(2),

	  upvector->get(0),
	  upvector->get(1),
	  upvector->get(2)
	);
  glMatrixMode(GL_MODELVIEW);
  return true;
}

void deactivate_offscreen() {
  // reset the matrix to previous value
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMultMatrixf(tmpMat_proj);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glMultMatrixf(tmpMat_model);
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

};

//-----------------------------------------------------------------------
//---- ORBIT CAMERA -----------------------------------------------------
//-----------------------------------------------------------------------


class vsx_orbit_camera : public vsx_module {
  // in
	GLfloat tmpMat_proj[16];
	GLfloat tmpMat_model[16];


  vsx_module_param_float3* rotation;
  vsx_module_param_float* distance;
  vsx_module_param_float3 * destination;
  vsx_module_param_float3 * upvector;
  vsx_module_param_float* fov;
  vsx_module_param_float* near_clipping;
  vsx_module_param_float* far_clipping;

  vsx_module_param_int* perspective_correct;

	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
	vsx_vector rotation_;

public:






void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;cameras;orbit_camera";
  info->description = "\
A camera always looking at the\n\
destination, with a distance\n\
and a rotation vector. Useful if you\n\
want to orbit an object like in starlight aurora.";
  info->in_param_spec =
"render_in:render,\
camera:complex{\
rotation:float3,\
distance:float,\
destination:float3,\
upvector:float3,\
fov:float,\
near_clipping:float,\
far_clipping:float,\
perspective_correct:enum?no|yes\
}\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true;
}


void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  rotation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"rotation");
  distance = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"distance");
  destination = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"destination");
  upvector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"upvector");
  fov = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fov");
  near_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"near_clipping");
  far_clipping = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"far_clipping");
  perspective_correct = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"perspective_correct");

  rotation->set(0,0);
  rotation->set(0,1);
  rotation->set(1,2);

  distance->set(2);
  destination->set(0,0);
  destination->set(0,1);
  destination->set(0,2);
  upvector->set(0,0);
  upvector->set(1,1);
  upvector->set(0,2);

  fov->set(90.0f);
  near_clipping->set(0.01f);
  far_clipping->set(2000.0f);

	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen() {
 	glGetFloatv(GL_PROJECTION_MATRIX, tmpMat_proj);
 	glGetFloatv(GL_MODELVIEW_MATRIX, tmpMat_model);


	glMatrixMode(GL_PROJECTION);
  float dist = distance->get();
  glLoadIdentity();
  if (perspective_correct->get())
  set_correct_perspective(fov->get(),near_clipping->get(),far_clipping->get());
  else
  gluPerspective(fov->get(), 1.0f, fabs(near_clipping->get()), far_clipping->get());
  rotation_.x = rotation->get(0);
  rotation_.y = rotation->get(1);
  rotation_.z = rotation->get(2);
  rotation_.normalize();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
#ifdef VSXU_OPENGL_ES
	glRotatef(-90,0,0,1);
#endif
	gluLookAt(
	  rotation_.x*dist+destination->get(0),
	  rotation_.y*dist+destination->get(1),
	  rotation_.z*dist+destination->get(2),

	  destination->get(0),
	  destination->get(1),
	  destination->get(2),

	  upvector->get(0),
	  upvector->get(1),
	  upvector->get(2)
	);
  return true;
}

void deactivate_offscreen() {
  // reset the matrix to previous value
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(tmpMat_proj);
  glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(tmpMat_model);
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

};

//-----------------------------------------------------------------------
//---- GL ORTHO 2D ------------------------------------------------------
//-----------------------------------------------------------------------

class vsx_gl_orto_2d : public vsx_module {
  // in
  GLfloat tmpMat[16];

  vsx_module_param_float3* scale;

  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal

public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_ortho2d";
  info->description = "";

  info->in_param_spec =
"\
render_in:render,\
scale:float3\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true; // always run this
}


void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  scale = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"scale");

  scale->set(1,0);
  scale->set(1,1);
  scale->set(1,2);

	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen() {
  // save current matrix
 	glGetFloatv(GL_PROJECTION_MATRIX, tmpMat);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
  gluOrtho2D(0, 1, 0, 1);
	return true;
}

void deactivate_offscreen() {
  // reset the matrix to previous value
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(tmpMat);
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}
};


//-----------------------------------------------------------------------
//--- GET VIEWPORT SIZE -------------------------------------------------
//-----------------------------------------------------------------------


class vsx_viewport_size : public vsx_module {
  GLint viewport[4];
  // in
  // out
  vsx_module_param_float* vx;
  vsx_module_param_float* vy;
  // internal

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "system;viewport_size";
    info->description = "Gets current viewport size. Hook\nit to the screen or a texture buffer.";
    info->in_param_spec = "";
    info->out_param_spec = "vx:float,vy:float";
    info->component_class = "render";
    info->tunnel = true;
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    vx = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"vx");
    vx->set(0.0f);
    vy = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"vy");
    vy->set(0.0f);
  }

  void run()
  {
    glGetIntegerv(GL_VIEWPORT, viewport);
    vx->set( (float)fabs((float)(viewport[2])) );
    vy->set( (float)fabs((float)(viewport[3])) );
  }

};






//-----------------------------------------------------------------------
//---- GL SCALE ---------------------------------------------------------
//-----------------------------------------------------------------------


class vsx_gl_scale : public vsx_module {
  // in
	GLfloat tmpMat[16];

  vsx_module_param_float3* scale;
  vsx_module_param_int* matrix;

	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal

public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_scale";
  info->description = "Scales everything up using \n\
OpenGL matrix transformations. \n\
Useful to either scale or squish\n\
an object/scene.\n\
Experiment with this in combination with\n\
gl_translate and gl_rotate.";

  info->in_param_spec =
"\
render_in:render,\
scale:float3,\
matrix:enum?MODELVIEW|PROJECTION|TEXTURE\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true; // always run this
}


void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  scale = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"scale");
  matrix = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"matrix");
  matrix->set(0);

  scale->set(1,0);
  scale->set(1,1);
  scale->set(1,2);

	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen() {
  // save current matrix
  switch(matrix->get()) {
  	case 0:
 		glGetFloatv(GL_MODELVIEW_MATRIX, tmpMat);
		glMatrixMode(GL_MODELVIEW);
		break;
  	case 1:
 		glGetFloatv(GL_PROJECTION_MATRIX, tmpMat);
		glMatrixMode(GL_PROJECTION);
		break;
  	case 2:
 		glGetFloatv(GL_TEXTURE_MATRIX, tmpMat);
		glMatrixMode(GL_TEXTURE);
		break;
  }
	glScalef(scale->get(0),scale->get(1),scale->get(2));
	return true;
}

void deactivate_offscreen() {
  // reset the matrix to previous value
  switch(matrix->get()) {
  	case 0:
		glMatrixMode(GL_MODELVIEW);
		break;
  	case 1:
		glMatrixMode(GL_PROJECTION);
		break;
  	case 2:
		glMatrixMode(GL_TEXTURE);
		break;
  }
	glLoadIdentity();
	glMultMatrixf(tmpMat);
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

};


//-----------------------------------------------------------------------
//---- GL TRANSLATE -----------------------------------------------------
//-----------------------------------------------------------------------

class vsx_gl_translate : public vsx_module {
  // in
  GLfloat tmpMat[16];

  vsx_module_param_float3* translation;

  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal

public:

void module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_translate";
  info->description = "Moves or \"translates\"\n\
an object in x,y,z directions.\n\
Use this if you want to move\n\
an object around or if the \n\
renderer doesn't have a position\n\
parameter. (Some don't as \n\
this module exists, it's more\n\
efficcient to use matrix transforms.";
  info->in_param_spec =
"\
render_in:render,\
translation:float3\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true; // always run this
}


void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  translation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"translation");

	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool activate_offscreen() {
  // save current matrix
 	glGetFloatv(GL_MODELVIEW_MATRIX, tmpMat);
	glMatrixMode(GL_MODELVIEW);
	glTranslatef(translation->get(0),translation->get(1),translation->get(2));
	return true;
}

void deactivate_offscreen() {
  // reset the matrix to previous value
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(tmpMat);
}

void output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

};


//-----------------------------------------------------------------------
//---- GL ROTATE --------------------------------------------------------
//-----------------------------------------------------------------------
class vsx_gl_rotate : public vsx_module {
  // in
  GLfloat tmpMat[16];

  vsx_module_param_float3* axis;
  vsx_module_param_float* angle;

  vsx_module_param_render* render_in;
  // out
  vsx_module_param_render* render_result;
  // internal
public:
  void module_info(vsx_module_info* info);
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters);
  bool activate_offscreen();
  void deactivate_offscreen();
//  void run();
  void output(vsx_module_param_abs* param);
};


void vsx_gl_rotate::module_info(vsx_module_info* info)
{
  info->identifier = "renderers;opengl_modifiers;gl_rotate";
  info->description = "Multiplies the current modelview\n\
matrix with a rotation around an \n\
axis. \n\
I.e. rotates anything connected \n\
via it.";
  info->in_param_spec =
"\
render_in:render,\
axis:float3,\
angle:float\
";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
  info->tunnel = true; // always run this
}


void vsx_gl_rotate::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = true;
  axis = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"axis");
  axis->set(0,0);
  axis->set(0,1);
  axis->set(1,2);
  angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"angle");

	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
}

bool vsx_gl_rotate::activate_offscreen() {
  // save current matrix
 	glGetFloatv(GL_MODELVIEW_MATRIX, tmpMat);
	glMatrixMode(GL_MODELVIEW);
	vsx_vector bb(axis->get(0),axis->get(1),axis->get(2));
	bb.normalize();
	glRotatef(angle->get()*360.0f,bb.x,bb.y,bb.z);
  return true;
}

void vsx_gl_rotate::deactivate_offscreen() {
  // reset the matrix to previous value
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(tmpMat);
}

void vsx_gl_rotate::output(vsx_module_param_abs* param) {
  render_result->set(render_in->get());
}

//-----------------------------------------------------------------------
//---- GL ROTATE QUATERNION ---------------------------------------------
//-----------------------------------------------------------------------

class vsx_gl_rotate_quat : public vsx_module {
  // in
	GLfloat tmpMat[16];

  vsx_module_param_quaternion* rotation;
  vsx_module_param_int* matrix_target_l;
  vsx_module_param_int* invert_rotation;

	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_result;
	// internal
public:


  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;gl_rotate_quat";
    info->description = "Multiplies the current modelview\n"
      "matrix with a rotation defined \n"
      "by a quaternion. \n"
      "I.e. rotates anything connected \n"
      "via it."
    ;
    info->in_param_spec =
      "render_in:render,"
      "rotation:quaternion,"
      "matrix_target:enum?MODELVIEW|PROJECTION|TEXTURE,"
      "invert_rotation:enum?NO|YES"
    ;
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true; // always run this
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    rotation = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"rotation");
    rotation->set(0,0);
    rotation->set(0,1);
    rotation->set(0,2);
    rotation->set(1,3);
    matrix_target_l = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"matrix_target");

    invert_rotation = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"invert_rotation");
    invert_rotation->set(0);

  	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

  	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  }
  vsx_quaternion bb;

  bool activate_offscreen() {
    // save current matrix
    glGetFloatv(matrix_target_get[matrix_target_l->get()], tmpMat);
    glMatrixMode(matrix_target[matrix_target_l->get()]);

  	bb.x = rotation->get(0);
  	bb.y = rotation->get(1);
  	bb.z = rotation->get(2);
  	bb.w = rotation->get(3);
    bb.normalize();

  	vsx_matrix mat;
    if (invert_rotation->get())
    {
      vsx_matrix mat2;
      mat2 = bb.matrix();
      mat.assign_inverse( &mat2 );
    } else
    {
      mat = bb.matrix();
    }
  	glMultMatrixf(mat.m);
    return true;
  }

  void deactivate_offscreen() {
    // reset the matrix to previous value
    glMatrixMode(matrix_target[matrix_target_l->get()]);
  	glLoadIdentity();
  	glMultMatrixf(tmpMat);
  }

  void output(vsx_module_param_abs* param) {
    render_result->set(render_in->get());
  }
};



//-----------------------------------------------------------------------
//---- GL MATRIX GET -----------------------------------------------
//-----------------------------------------------------------------------
class vsx_module_gl_matrix_get : public vsx_module {
  // in
	//GLfloat tmpMat[16];
	vsx_module_param_render* render_in;
	vsx_module_param_int* matrix_target;
	// out
	vsx_module_param_render* render_result;
  vsx_module_param_matrix* matrix_out;
	// internal
	vsx_matrix matrix;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;gl_matrix_get";
    info->description = "Gets the current matrix from openGL";
    info->in_param_spec =
  "\
  render_in:render,\
  matrix_target:enum?MODELVIEW|PROJECTION|TEXTURE";
    info->out_param_spec = "render_out:render,matrix_out:matrix";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    matrix_target = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"matrix_target");
  	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
  	render_in->set(0);
    matrix_out = (vsx_module_param_matrix*)out_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"matrix_out");
    matrix_out->set(matrix);
  	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  	render_result->set(1);
  }


  void output(vsx_module_param_abs* param) {
    if (param == render_result) {
      // save current matrix
 	    glGetFloatv(matrix_target_get[matrix_target->get()], matrix.m);
   	  matrix_out->set(matrix);
      render_result->set(1);
    }
  }
};


//-----------------------------------------------------------------------
//---- GL MATRIX MULTIPLY -----------------------------------------------
//-----------------------------------------------------------------------
class vsx_gl_matrix_multiply : public vsx_module {
  // in
	GLfloat tmpMat[16];

  vsx_module_param_matrix* matrix_in;
	vsx_module_param_render* render_in;
	vsx_module_param_int* matrix_target_l;
	// out
	vsx_module_param_render* render_result;
	// internal
	vsx_matrix* mm;
	int active;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;opengl_modifiers;gl_matrix_multiply";
    info->description = "If a module outputs a matrix\n\
as a result of a mathematical \n\
operation and you want to use it\n\
to transform a scene, this is the module\n\
for you.\n\
\n\
Try it with the bspline_matrix math\n\
module for some interresting results.";
    info->in_param_spec =
  "\
  render_in:render,\
  matrix_in:matrix,\
  matrix_target:enum?MODELVIEW|PROJECTION|TEXTURE";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    info->tunnel = true; // always run this
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    //vsx_matrix foo;
    matrix_in = (vsx_module_param_matrix*)in_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"matrix_in");
    //matrix_in->set(foo);
    matrix_target_l = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"matrix_target");
  	render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");

  	render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  }

  bool activate_offscreen() {
    // save current matrix
    active = false;
    mm = matrix_in->get_addr();
    if (mm) {
      active = true;
      glGetFloatv(matrix_target_get[matrix_target_l->get()],tmpMat);
      glMatrixMode(matrix_target[matrix_target_l->get()]);
      glMultMatrixf(mm->m);
    }
    return true;
  }

  void deactivate_offscreen() {
    // reset the matrix to previous value
    if (active)
    switch (matrix_target_l->get()) {
      case 0:
    	glMatrixMode(GL_MODELVIEW);
    	glLoadIdentity();
    	glMultMatrixf(tmpMat);
    	break;
      case 1:
    	glMatrixMode(GL_PROJECTION);
    	glLoadIdentity();
    	glMultMatrixf(tmpMat);
    	break;
      case 2:
    	glMatrixMode(GL_TEXTURE);
    	glLoadIdentity();
    	glMultMatrixf(tmpMat);
    	break;
    }
  }

  void output(vsx_module_param_abs* param) {
    render_result->set(render_in->get());
  }
};


//-----------------------------------------------------------------------
//---- GL_NORMALIZE -----------------------------------------------------
//-----------------------------------------------------------------------
class vsx_gl_normalize : public vsx_module {
  // in
	vsx_module_param_render* render_in;
	// out
	vsx_module_param_render* render_out;
	// internal
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "renderers;opengl_modifiers;auto_normalize";
    info->description = "";
    info->in_param_spec = "render_in:render";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    loading_done = true;
    info->tunnel = true; // always run this
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_in->set(0);
    render_out->set(0);
  }

	bool activate_offscreen() {
    glEnable(GL_NORMALIZE);
    return true;
  }

	void deactivate_offscreen() {
    glDisable(GL_NORMALIZE);
  }
};

//-----------------------------------------------------------------------
//---- GL_LINE_WIDTH ----------------------------------------------------
//-----------------------------------------------------------------------
class vsx_gl_line_width : public vsx_module {
  // in
	vsx_module_param_render* render_in;
  vsx_module_param_float* width;
	// out
	vsx_module_param_render* render_out;
	// internal
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "renderers;opengl_modifiers;gl_line_width";
    info->description = "";
    info->in_param_spec = "render_in:render,width:float";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    loading_done = true;
    info->tunnel = true; // always run this
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"width");
    width->set(1.0f);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_in->set(0);
    render_out->set(0);
  }
  GLfloat prev_width;
	bool activate_offscreen() {
    glGetFloatv(GL_LINE_WIDTH,&prev_width);
    glLineWidth(width->get());
    return true;
  }

	void deactivate_offscreen() {
    glLineWidth(prev_width);
  }
};


class vsx_texture_bind : public vsx_module {
  // in
	vsx_module_param_render* render_in;
  vsx_module_param_texture* tex_in;
	// out
	vsx_module_param_render* render_out;
	// internal
	vsx_texture** t_tex;

public:
  void module_info(vsx_module_info* info) {
    info->identifier = "texture;opengl;texture_bind";
    info->description = "Uses OpenGL to bind a texture.\nAll connected to it (unless \n\
    it declares its own textures) \nwill use the texture \nprovided in the tex_in.";
    info->in_param_spec = "render_in:render,tex_in:texture";
    info->out_param_spec = "render_out:render";
    info->component_class = "texture";
    loading_done = true;
    info->tunnel = true;
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_in->set(0);
    render_out->set(0);
    tex_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"tex_in");
    //tex_in->set(&i_tex);
  }

	bool activate_offscreen() {
    t_tex = tex_in->get_addr();
    if (t_tex)
    {
     	glMatrixMode(GL_TEXTURE);
     	glPushMatrix();
      vsx_transform_obj& texture_transform = *(*t_tex)->get_transform();
     	texture_transform();

      glMatrixMode(GL_MODELVIEW);


      (*t_tex)->bind();

    }
    return true;
  }

	void deactivate_offscreen() {
    if (t_tex) {
      (*t_tex)->_bind();
      glMatrixMode(GL_TEXTURE);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
    }
  }
};



//******************************************************************************
//*** F A C T O R Y ************************************************************
//******************************************************************************

#ifndef _WIN32
#define __declspec(a)
#endif

extern "C" {
__declspec(dllexport) vsx_module* create_new_module(unsigned long module, void* args);
__declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
__declspec(dllexport) unsigned long get_num_modules();
}


vsx_module* create_new_module(unsigned long module, void* args) {
  // as we have only one module available, don't look at the module variable, just return - for speed
  // otherwise you'd have something like,, switch(module) { case 0: break; }
  switch(module) {
    case 0 : return (vsx_module*)(new vsx_module_blend_mode);
    case 1 : return (vsx_module*)(new vsx_orbit_camera);
    case 2 : return (vsx_module*)(new vsx_target_camera);
    case 3 : return (vsx_module*)(new vsx_freelook_camera);
    case 4 : return (vsx_module*)(new vsx_gl_translate);
    case 5 : return (vsx_module*)(new vsx_depth_buffer);
    case 6 : return (vsx_module*)(new vsx_gl_rotate);
    case 7 : return (vsx_module*)(new vsx_light);
    case 8 : return (vsx_module*)(new vsx_polygon_mode);
    case 9 : return (vsx_module*)(new vsx_material_param);
    case 10: return (vsx_module*)(new vsx_gl_scale);
    case 11: return (vsx_module*)(new vsx_gl_matrix_multiply);
    case 12: return (vsx_module*)(new vsx_gl_color);
    case 13: return (vsx_module*)(new vsx_gl_orto_2d);
    case 14: return (vsx_module*)(new vsx_fog);
    case 15: return (vsx_module*)(new vsx_backface_cull);
    case 16: return (vsx_module*)(new vsx_gl_rotate_quat);
    case 17: return (vsx_module*)(new vsx_gl_normalize);
    case 18: return (vsx_module*)(new vsx_module_gl_matrix_get);
    case 19: return (vsx_module*)(new vsx_gl_line_width);
    case 20: return (vsx_module*)(new vsx_depth_buffer_clear);
    case 21: return (vsx_module*)(new vsx_depth_func);
		case 22: return (vsx_module*)(new vsx_texture_bind);
    case 23: return (vsx_module*)(new vsx_viewport_size);
    case 24: return (vsx_module*)(new vsx_buffer_clear);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_blend_mode*)m; break;
    case 1: delete (vsx_orbit_camera*)m; break;
    case 2: delete (vsx_target_camera*)m; break;
    case 3: delete (vsx_freelook_camera*)m; break;
    case 4: delete (vsx_gl_translate*)m; break;
    case 5: delete (vsx_depth_buffer*)m; break;
    case 6: delete (vsx_gl_rotate*)m; break;
    case 7: delete (vsx_light*)m; break;
    case 8: delete (vsx_polygon_mode*)m; break;
    case 9: delete (vsx_material_param*)m; break;
    case 10: delete (vsx_gl_scale*)m; break;
    case 11: delete (vsx_gl_matrix_multiply*)m; break;
    case 12: delete (vsx_gl_color*)m; break;
    case 13: delete (vsx_gl_orto_2d*)m; break;
    case 14: delete (vsx_fog*)m; break;
    case 15: delete (vsx_backface_cull*)m; break;
    case 16: delete (vsx_gl_rotate_quat*)m; break;
    case 17: delete (vsx_gl_normalize*)m; break;
    case 18: delete (vsx_module_gl_matrix_get*)m; break;
    case 19: delete (vsx_gl_line_width*)m; break;
    case 20: delete (vsx_depth_buffer_clear*)m; break;
    case 21: delete (vsx_depth_func*)m; break;
		case 22: delete (vsx_texture_bind*)m; break;
    case 23: delete (vsx_viewport_size*)m; break;
    case 24: delete (vsx_buffer_clear*)m; break;
  }
}

unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  glewInit();
  return 25;
}

