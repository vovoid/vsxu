#ifndef VSX_GL_HELPER_H
#define VSX_GL_HELPER_H


void draw_box(vsx_vector pos, float width, float height);
void draw_box_c(vsx_vector pos, float width, float height);
void draw_box_tex(vsx_vector pos, float width, float height);
void draw_box_tex_c(vsx_vector pos, float width, float height);
void draw_box_tex(float pos_x, float pos_y, float pos_z, float width, float height);
void draw_box_gradient(vsx_vector pos, float width, float height, vsx_color a, vsx_color b, vsx_color c, vsx_color d);
void draw_box_border(vsx_vector pos, vsx_vector size, float dragborder);
vsx_vector vsx_vec_viewport();



#endif
