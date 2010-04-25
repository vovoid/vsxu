#define VSX_NOGLUT_LOCAL
#include "vsx_gl_global.h"
#undef VSX_NOGLUT_LOCAL
#include "vsx_math_3d.h"

void draw_box(vsx_vector pos, float width, float height) {
	glBegin(GL_QUADS);
			glVertex2f(pos.x, pos.y+height);
			glVertex2f(pos.x, pos.y);
			glVertex2f(pos.x+width, pos.y);
 			glVertex2f(pos.x+width, pos.y+height);
	glEnd();
}
void draw_box_c(vsx_vector pos, float width, float height) {
	glBegin(GL_QUADS);
			glVertex2f(pos.x-width, pos.y+height);
			glVertex2f(pos.x-width, pos.y-height);
			glVertex2f(pos.x+width, pos.y-height);
 			glVertex2f(pos.x+width, pos.y+height);
	glEnd();
}
void draw_box_tex_c(vsx_vector pos, float width, float height) {
  glBegin(GL_QUADS);
  	glTexCoord2f(0, 1);
  	glVertex2f(pos.x-width, pos.y+height);
  	glTexCoord2f(0, 0);
		glVertex2f(pos.x-width, pos.y-height);
  	glTexCoord2f(1, 0);
		glVertex2f(pos.x+width, pos.y-height);
  	glTexCoord2f(1,1);
		glVertex2f(pos.x+width, pos.y+height);
	glEnd();
}
void draw_box_tex(vsx_vector pos, float width, float height) {
  glBegin(GL_QUADS);
  	glTexCoord2f(0, 1);
  	glVertex2f(pos.x, pos.y+height);
  	glTexCoord2f(0, 0);
		glVertex2f(pos.x, pos.y);
  	glTexCoord2f(1, 0);
		glVertex2f(pos.x+width, pos.y);
  	glTexCoord2f(1,1);
		glVertex2f(pos.x+width, pos.y+height);
	glEnd();
}

void draw_box_tex(float pos_x, float pos_y, float pos_z, float width, float height) {
  glBegin(GL_QUADS);
  	glTexCoord2f(0, 1);
  	glVertex2f(pos_x-width, pos_y+height);
  	glTexCoord2f(0, 0);
		glVertex2f(pos_x-width, pos_y-height);
  	glTexCoord2f(1, 0);
		glVertex2f(pos_x+width, pos_y-height);
  	glTexCoord2f(1,1);
		glVertex2f(pos_x+width, pos_y+height);
	glEnd();
}
void draw_box_gradient(vsx_vector pos, float width, float height, vsx_color a, vsx_color b, vsx_color c, vsx_color d) {
	glBegin(GL_QUADS);
    a.gl_color();
			glVertex2f(pos.x, pos.y+height);
		b.gl_color();
			glVertex2f(pos.x, pos.y);
		c.gl_color();
			glVertex2f(pos.x+width, pos.y);
		d.gl_color();
 			glVertex2f(pos.x+width, pos.y+height);
	glEnd();
}

void draw_box_border(vsx_vector pos, vsx_vector size, float dragborder) {
  glBegin(GL_QUADS);
  	glVertex2f(pos.x,pos.y+size.y);
  	glVertex2f(pos.x+dragborder, pos.y+size.y);
  	glVertex2f(pos.x+dragborder, pos.y);
  	glVertex2f(pos.x,pos.y);
  
  	glVertex2f(pos.x+size.x,pos.y+size.y);
  	glVertex2f(pos.x+size.x-dragborder, pos.y+size.y);
  	glVertex2f(pos.x+size.x-dragborder, pos.y);
  	glVertex2f(pos.x+size.x,pos.y);
  
  	glVertex2f(pos.x, pos.y+size.y-dragborder);
  	glVertex2f(pos.x+size.x, pos.y+size.y-dragborder);
  	glVertex2f(pos.x+size.x, pos.y+size.y);
  	glVertex2f(pos.x, pos.y+size.y);
  
  	glVertex2f(pos.x, pos.y+dragborder);
  	glVertex2f(pos.x+size.x, pos.y+dragborder);
  	glVertex2f(pos.x+size.x, pos.y);
  	glVertex2f(pos.x, pos.y);
  glEnd();
}

vsx_vector vsx_vec_viewport() {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  return vsx_vector(viewport[2],viewport[3],0);  
}
