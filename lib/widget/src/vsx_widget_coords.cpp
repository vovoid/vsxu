#include "vsx_widget_coords.h"


#include <vsx_gl_global.h>
#include <vsx_gl_state.h>

double vsx_widget_coords::modelMatrix[16];
double vsx_widget_coords::projMatrix[16];
int vsx_widget_coords::viewport[4];

vsx_vector3<> vsx_vec_viewport()
{
  return vsx_vector3f(
    (float)vsx_gl_state::get_instance()->viewport_get_width(),
    (float)vsx_gl_state::get_instance()->viewport_get_height()
  );
}


void vsx_widget_coords::init(float x, float y)
{
  GLfloat wx=x,wy,wz;
  glGetDoublev(GL_PROJECTION_MATRIX,projMatrix);
  glGetDoublev(GL_MODELVIEW_MATRIX,modelMatrix);
  glGetIntegerv(GL_VIEWPORT, viewport);
  wy=viewport[3]-y;
  wx = x;
  glReadPixels((int)x,(int)y,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&wz);

  GLdouble xx,yy,zz;
  gluUnProject(wx,wy,wz,modelMatrix,projMatrix,viewport,&xx,&yy,&zz);

  // world
  world_global.x = (float)xx;
  world_global.y = (float)yy;
  world_global.z = 0.0f;
  world_local.x = world_local.y = 0.0f;
  // screen
  screen_global.x = x;
  screen_global.y = y;
  screen_global.z = 0.0f;
  screen_global = screen_global / vsx_vec_viewport();
  screen_global.y = 1-screen_global.y;
}
