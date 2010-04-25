#ifndef VSX_NO_CLIENT
#include "vsx_gl_global.h"
//#include <string>
//#include <iostream>
#include <iomanip>
#include <map>
#include <list>
#include <vector>
#include <math.h>
//#include <sstream>
//#include <fstream>
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsxfst.h"
#include "vsx_mouse.h"
#include "vsx_param.h"
#include "vsx_module.h"
// local includes
#include "vsx_widget_base.h"
//#include "vsx_widget_anchor.h"
//#include "vsx_widget_server.h"
//#include "vsx_widget_connector.h"
//#include "vsx_widget_comp.h"
#include "lib/vsx_widget_lib.h"
#include "vsx_timer.h"
#include "vsxfst.h"
#include "vsx_widget_base_controller.h"
#include "vsx_widget_controller.h"
#include "vsx_widget_controller_ab.h"


void vsx_widget_controller_ab::init() {
  type = VSX_WIDGET_TYPE_CONTROLLER;
  type = 3; // float3 datatype
  get_value();
  quadratic=gluNewQuadric();										      // Create A Pointer To The Quadric Object
	gluQuadricNormals(quadratic, GLU_SMOOTH);						// Create Smooth Normals
	gluQuadricTexture(quadratic, GL_TRUE);							// Create Texture Coords
	vsx_widget::init();
  constrained_x = false;
  constrained_y = false;
  set_pos(vsx_vector(-sizeunit*1.2));
  set_size(vsx_vector(0.02f, 0.02f));
	//rot = 0;

	ball_size.y = ball_size.x = size.x/2;

	dest_quat.s.X = 0;
	dest_quat.s.Y = 1;
	dest_quat.s.Z = 0;
	dest_quat.s.W = 0;

	Transform.M[0] = 1.0f;
	Transform.M[1] = 0.0f;
	Transform.M[2] = 0.0f;
	Transform.M[3] = 0.0f;

	Transform.M[4] = 0.0f;
	Transform.M[5] = 1.0f;
	Transform.M[6] = 0.0f;
	Transform.M[7] = 0.0f;

	Transform.M[8] = 0.0f;
	Transform.M[9] = 0.0f;
	Transform.M[10] = 1.0f;
	Transform.M[11] = 0.0f;

	Transform.M[12] = 0.0f;
	Transform.M[13] = 0.0f;
	Transform.M[14] = 0.0f;
	Transform.M[15] = 1.0f;


	LastRot.M[0] = 1.0f;
	LastRot.M[1] = 0.0f;
	LastRot.M[2] = 0.0f;

	LastRot.M[3] = 0.0f;
	LastRot.M[4] = 1.0f;
	LastRot.M[5] = 0.0f;

	LastRot.M[6] = 0.0f;
	LastRot.M[7] = 0.0f;
	LastRot.M[8] = 1.0f;


	ThisRot.M[0] = 1.0f;
	ThisRot.M[1] = 0.0f;
	ThisRot.M[2] = 0.0f;

	ThisRot.M[3] = 0.0f;
	ThisRot.M[4] = 1.0f;
	ThisRot.M[5] = 0.0f;

	ThisRot.M[6] = 0.0f;
	ThisRot.M[7] = 0.0f;
	ThisRot.M[8] = 1.0f;


	int_rot.M[0] = 1.0f;
	int_rot.M[1] = 0.0f;
	int_rot.M[2] = 0.0f;
	int_rot.M[3] = 0.0f;

	int_rot.M[4] = 0.0f;
	int_rot.M[5] = 1.0f;
	int_rot.M[6] = 0.0f;
	int_rot.M[7] = 0.0f;

	int_rot.M[8] = 0.0f;
	int_rot.M[9] = 0.0f;
	int_rot.M[10] = 1.0f;
	int_rot.M[11] = 0.0f;

	int_rot.M[12] = 0.0f;
	int_rot.M[13] = 0.0f;
	int_rot.M[14] = 0.0f;
	int_rot.M[15] = 1.0f;

  /*label=add(new vsx_widget_3d_label,name+".label");
  label->size.y=sizeunit*0.25;
  label->size.x=sizeunit;
  label->pos.y=sizeunit*0.5+label->size.y*0.5;
  ((vsx_widget_3d_label*)label)->init();*/



  mtex.load_png(skin_path+"controllers/sphere.png");
  generate_menu();
  menu->init();
  ArcBall = new ArcBall_t(0,0);
  // set the arcball in usable mode
  MousePt.s.Y = MousePt.s.X = 0;
  //ArcBall->click(&MousePt);
  //MousePt.s.Y = MousePt.s.X = 0.001;
  Quat4fT     ThisQuat;
  ArcBall->drag(&MousePt, &ThisQuat);						// Update End Vector And Get Rotation As Quaternion

  Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);		// Convert Quaternion Into Matrix3fT
  Matrix3fMulMatrix3f(&ThisRot, &LastRot);				// Accumulate Last Rotation Into This One
  //float v1[3];
  dest_quat.s.X = 0;
  dest_quat.s.Y = 0;
  dest_quat.s.Z = 1;
  dest_quat.s.W = 1;
  //bgcolor.r = 0.1;
  //bgcolor.g = 0.1;
  //bgcolor.b = 0.1;
  //v1[0] = 0;
  //v1[1] = 1;
  //v1[2] = 0;
  //mat_vec_mult3x3(&ThisRot.M[0], &v1[0], &v2[0]);
  //v_norm(&v2[0]);
  //Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);
  //v2_int[0] = v2[0];
  //v2_int[1] = v2[1];
  //v2_int[2] = v2[2];
  //int_rot = Transform;
  LastRot = ThisRot;
}

void vsx_widget_controller_ab::draw() {
  if (!visible) return;
  parentpos = parent->get_pos_p();
  //Matrix3fT   NewRot;



  //Matrix4fSetRotationFromMatrix3f(&Transform, &ThisRot);

  if (smoothness>=0)
  {
    //Matrix3fSetRotationFromQuat4f(&NewRot, &dest_quat);
    //QuatFromMat(&NewRot, inter_quat );
    float tt = dtime*smoothness;
    if (tt > 1) tt = 1;
    Quat4fT temp_quat;
    QuatSlerp(&inter_quat, &dest_quat,tt, &temp_quat);
    inter_quat = temp_quat;


    /*float temp = inter_quat.T[0]*(1-tt)+dest_quat.T[0]*tt;
    inter_quat.T[0] = temp;
    temp = inter_quat.T[1]*(1-tt)+dest_quat.T[1]*tt;
    inter_quat.T[1] = temp;
    temp = inter_quat.T[2]*(1-tt)+dest_quat.T[2]*tt;
    inter_quat.T[2] = temp;
    temp = inter_quat.T[3]*(1-tt)+dest_quat.T[3]*tt;
    inter_quat.T[3] = temp;
    printf("interquat 0: %f\n",inter_quat.T[0]);
    printf("interquat 1: %f\n",inter_quat.T[1]);
    printf("interquat 2: %f\n",inter_quat.T[2]);
    printf("interquat 3: %f\n",inter_quat.T[3]);
*/

/*    float len = 1.0 / (float)sqrt(inter_quat.s.X*inter_quat.s.X + inter_quat.s.Y*inter_quat.s.Y+
    inter_quat.s.Z*inter_quat.s.Z+ inter_quat.s.W*inter_quat.s.W);
    inter_quat.s.X *= len;
    inter_quat.s.Y *= len;
    inter_quat.s.Z *= len;
    inter_quat.s.W *= len;
*/

    /*for (int i = 0; i < 16; ++i) {
      temp = int_rot.M[i]*(1-tt)+Transform.M[i]*tt;
      int_rot.M[i]=temp;
    }
    temp = v2_int[0]*(1-tt)+v2[0]*tt;
    v2_int[0]=temp;
    temp = v2_int[1]*(1-tt)+v2[1]*tt;
    v2_int[1]=temp;
    temp = v2_int[2]*(1-tt)+v2[2]*tt;
    v2_int[2]=temp;

    float a = 1.0f/sqrt(int_rot.M[0]*int_rot.M[0] + int_rot.M[1]*int_rot.M[1] + int_rot.M[2]*int_rot.M[2]);
    int_rot.M[0]*= a;
    int_rot.M[1]*= a;
    int_rot.M[2]*= a;

    a = 1.0f/sqrt(int_rot.M[4]*int_rot.M[4] + int_rot.M[5]*int_rot.M[5] + int_rot.M[6]*int_rot.M[6]);
    int_rot.M[4]*= a;
    int_rot.M[5]*= a;
    int_rot.M[6]*= a;

    a = 1.0f/sqrt(int_rot.M[8]*int_rot.M[8] + int_rot.M[9]*int_rot.M[9] + int_rot.M[10]*int_rot.M[10]);
    int_rot.M[8]*= a;
    int_rot.M[9]*= a;
    int_rot.M[10]*= a;
    int_rot.M[3] = 0;
    int_rot.M[7] = 0;
    int_rot.M[11] = 0;
    int_rot.M[12] = 0;
    int_rot.M[13] = 0;
    int_rot.M[14] = 0;
    int_rot.M[15] = 1;*/

    Matrix3fSetRotationFromQuat4f(&NewRot, &inter_quat);
  } else {
    Matrix3fSetRotationFromQuat4f(&NewRot, &dest_quat);
    v2_int[0] = v2[0];
    v2_int[1] = v2[1];
    v2_int[2] = v2[2];
    int_rot = Transform;
  }

  Matrix4fSetRotationFromMatrix3f(&Transform, &NewRot);
  int_rot = Transform;

  glColor3f(1,1,1);
  /*glLineWidth(2);
	glBegin(GL_LINES);
	   	  glVertex3f(parentpos.x,parentpos.y,pos.z);
	   	  glColor4f(1,1,1,0);
	   	  glVertex3f(parentpos.x+pos.x,parentpos.y+pos.y,pos.z);
	glEnd();*/

  vsx_widget_base_controller::draw();

/*	glPushMatrix();
  	glTranslatef(parentpos.x+pos.x, parentpos.y+pos.y, pos.z);
  	glRotatef(rot,v2_int[0],v2_int[1],v2_int[2]);
  	rot += 0.8;
    glColor3f(1.0,1.0,1.0);
	glPopMatrix();*/

	glPushMatrix();
    glTranslatef(parentpos.x+pos.x, parentpos.y+pos.y, pos.z);
    glMultMatrixf(int_rot.M);

    glLineWidth(3);
    glBegin(GL_LINE_STRIP);
      glColor4f(1.0,1.0,1.0,1.0);
  	   	  glVertex3f(0,-size.x/2*0.87,0);
      glColor4f(1.0,1.0,1.0,0.0);
  	   	  glVertex3f(0,0,0);
      glColor4f(1.0,1.0,1.0,1.0);
  	   	  glVertex3f(0,size.x/2*0.87,0);
  	glEnd();

    glBegin(GL_LINE_STRIP);
      glColor4f(1.0,0.5,1.0,1.0);
  	   	  glVertex3f(-size.x/2*0.87,0,0);
      glColor4f(1.0,1.0,1.0,0.0);
  	   	  glVertex3f(0,0,0);
      glColor4f(1.0,0.5,1.0,1.0);
  	   	  glVertex3f(size.x/2*0.87,0,0);
  	glEnd();

    glBegin(GL_LINE_STRIP);
      glColor4f(1.0,0.5,1.0,1.0);
  	   	  glVertex3f(0,0,-size.x/2*0.87);
      glColor4f(1.0,1.0,1.0,0.0);
  	   	  glVertex3f(0,0,0);
      glColor4f(1.0,0.5,1.0,1.0);
  	   	  glVertex3f(0,0,size.x/2*0.87);
  	glEnd();

    glLineWidth(1);

    if (v2[2] <= 0) {
      glColor4f(1.0f,1.0f,1.0f,0.7f);
      glCullFace(GL_FRONT);
      glEnable(GL_CULL_FACE);
      mtex.bind();
        gluSphere(quadratic,size.x/2*0.8,10,10);
      mtex._bind();
      glDisable(GL_CULL_FACE);
    }

  	glPushMatrix();
  	glColor4f(1.0f,1.0f,1.0f,1.0f);
  	glTranslatef(0,size.x/2*0.9,0);
  	gluSphere(quadratic,size.x/2*0.09,3,3);
    glPopMatrix();

  // after line

    if (v2[2] > 0) {
      glColor4f(1.0f,1.0f,1.0f,0.7f);
      glCullFace(GL_FRONT);
      glEnable(GL_CULL_FACE);
      mtex.bind();
        gluSphere(quadratic,size.x/2*0.8,10,10);
      mtex._bind();
      glDisable(GL_CULL_FACE);
    }
	glPopMatrix();
}

void vsx_widget_controller_ab::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button) {

    if (controlling && button == 0)
    {
      mouse.set_cursor_pos(remPointer.x,remPointer.y);
    }
    mouse.show_cursor();
		LastRot = ThisRot;
		controlling = false;
		vsx_widget::event_mouse_up(distance,coords,button);
  //vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_controller_ab::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
//  printf("vsx_widget_controller_ab::event_mouse_down\n");
//  world = world-pos;
    //MousePt.s.X = world.x;
    //MousePt.s.Y = world.y;
  if (button == 0) {
    MousePt.s.X = distance.center.x/size.x;
    MousePt.s.Y = distance.center.y/size.y;
    //printf("vsx_widget_controller_ab::event_mouse_down %f %f\n",MousePt.s.X, MousePt.s.Y);
//		isDragging = true;
		m_focus = this;
		a_focus = this;
		k_focus = this;
		ArcBall->click(&MousePt);
    //float k1=(world.x-pos.x)/size.x;
    //float k2=(world.y-pos.y)/size.y;
    if (sqrt(MousePt.s.X*MousePt.s.X + MousePt.s.Y*MousePt.s.Y) < 0.4f) {
      controlling = true;
      remPointer=mouse.position;
      mouse.hide_cursor();
    }
  }

  vsx_widget::event_mouse_down(distance,coords,button);
}

void vsx_widget_controller_ab::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) {
  if (mouse.position!=remPointer && controlling) {
//    world = world-parentpos-pos;
    Point2fT    oldMousePt = MousePt;
    //Point2fT    new_m_p;
    //ArcBall->click(&MousePt);

    Quat4fT ThisQuat;
    //new_m_p.s.X = (world.x/size.x-MousePt.s.X);
    //new_m_p.s.Y = (world.y/size.y-MousePt.s.Y);
    MousePt.s.X = distance.center.x/size.x;
    MousePt.s.Y = distance.center.y/size.y;


    /*if (MousePt.s.X > 0.9) MousePt.s.X = 0.9;
    if (MousePt.s.X < -0.9) MousePt.s.X = -0.9;
    if (MousePt.s.Y > 0.9) MousePt.s.Y = 0.9;
    if (MousePt.s.Y < -0.9) MousePt.s.Y = -0.9;*/
    //printf("vsx_widget_controller_ab::event_mouse_move %f %f\n",new_m_p.s.X, new_m_p.s.Y);

    /*Quat4fT ThisQuat2;

    float sin_a    = sin( 0.5 );
    float cos_a    = cos( 0.5 );

    float sin_lat  = sin( MousePt.s.Y );
    float cos_lat  = cos( MousePt.s.Y );

    float sin_long = sin( MousePt.s.X );
    float cos_long = cos( MousePt.s.X );

    ThisQuat2.s.X       = sin_a * cos_lat * sin_long;
    ThisQuat2.s.Y       = sin_a * sin_lat;
    ThisQuat2.s.Z       = sin_a * sin_lat * cos_long;
    ThisQuat2.s.W       = cos_a;*/


    //ArcBall->drag(&new_m_p, &ThisQuat);
    ArcBall->drag(&MousePt, &ThisQuat);
    MousePt = oldMousePt;
    /*dest_quat.s.X += ThisQuat.s.X;
    dest_quat.s.Y += ThisQuat.s.Y;
    dest_quat.s.Z += ThisQuat.s.Z;
    dest_quat.s.W += ThisQuat.s.W;*/

    //Quat4fMulQuat4f(&dest_quat, &ThisQuat);

    //printf("quat 1: %f\n",dest_quat.T[0]);

    /*Vector3fT qav, qbv, va, vb, vc;
    qav.s.X = ThisQuat.s.X;
    qav.s.Y = ThisQuat.s.Y;
    qav.s.Z = ThisQuat.s.Z;

    qbv.s.X = dest_quat.s.X;
    qbv.s.Y = dest_quat.s.Y;
    qbv.s.Z = dest_quat.s.Z;

    dest_quat.s.W = Vector3fDot ( &qav, &qbv );
    Vector3fCross(&va, &qav, &qbv);
    vb.s.X = qav.s.X*dest_quat.s.W;
    vb.s.Y = qav.s.Y*dest_quat.s.W;
    vb.s.Z = qav.s.Z*dest_quat.s.W;

    vc.s.X = qbv.s.X*ThisQuat.s.W;
    vc.s.Y = qbv.s.Y*ThisQuat.s.W;
    vc.s.Z = qbv.s.Z*ThisQuat.s.W;

    dest_quat.s.X = va.s.X+vb.s.X+vc.s.X;
    dest_quat.s.Y = va.s.Y+vb.s.Y+vc.s.Y;
    dest_quat.s.Z = va.s.Z+vb.s.Z+vc.s.Z;

    float len = 1.0 / (float)sqrt(dest_quat.s.X*dest_quat.s.X + dest_quat.s.Y*dest_quat.s.Y+
    dest_quat.s.Z*dest_quat.s.Z+ dest_quat.s.W*dest_quat.s.W);
    dest_quat.s.X *= len;
    dest_quat.s.Y *= len;
    dest_quat.s.Z *= len;
    dest_quat.s.W *= len;*/


    /*dest_quat.s.X += ThisQuat.s.X;
    dest_quat.s.Y += ThisQuat.s.Y;
    dest_quat.s.Z += ThisQuat.s.Z;
    dest_quat.s.W += ThisQuat.s.W;
    Quat4fT ThisQuat2 = dest_quat;*/
    //float len = 1.0 / (float)sqrt(ThisQuat2.s.X*ThisQuat2.s.X + ThisQuat2.s.Y*ThisQuat2.s.Y+
    //ThisQuat2.s.Z*ThisQuat2.s.Z+ ThisQuat2.s.W*ThisQuat2.s.W); //
    //float len = 1.0 / (float)sqrt(ThisQuat2.s.X*ThisQuat2.s.X + ThisQuat2.s.Z*ThisQuat2.s.Z); //
    //ThisQuat2.s.X *= len;
    //ThisQuat2.s.Z *= len;
    //len = 1.0 / (float)sqrt(ThisQuat2.s.Y*ThisQuat2.s.Y + ThisQuat2.s.W*ThisQuat2.s.W); //
    //ThisQuat2.s.Y *= len;
    //ThisQuat2.s.W *= len;
    //dest_quat = ThisQuat2;
    //printf("qatx %f %f %f %f\n",ThisQuat2.s.X,ThisQuat2.s.Y,ThisQuat2.s.Z,ThisQuat2.s.W);

    //Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);
    //Matrix3fMulMatrix3f(&ThisRot, &LastRot);

    //Matrix3fSetRotationFromQuat4f(&ThisRot, &dest_quat);

    //Matrix3fMulMatrix3f(&ThisRot, &LastRot);


    Matrix3fSetRotationFromQuat4f(&ThisRot, &ThisQuat);
    Matrix3fMulMatrix3f(&ThisRot, &LastRot);


//    Matrix3fSetRotationFromQuat4f(&ThisRot, &dest_quat);
//    Matrix3fMulMatrix3f(&ThisRot, &LastRot);

    float v1[3];
    v1[0] = 0;
    v1[1] = 1;
    v1[2] = 0;

    //float mm[9];
    //mm[0] = ThisRot.M[0];
    //mm[1] = ThisRot.M[1];
    mat_vec_mult3x3(&ThisRot.M[0], &v1[0], &v2[0]);
    v_norm(&v2[0]);

    /*
    //attempt to normalize the quaternion
    float len = 1.0 / (float)sqrt(dest_quat.s.X*dest_quat.s.X + dest_quat.s.Y*dest_quat.s.Y+
    dest_quat.s.Z*dest_quat.s.Z+ dest_quat.s.W*dest_quat.s.W);
    dest_quat.s.X *= len;
    dest_quat.s.Y *= len;
    dest_quat.s.Z *= len;
    dest_quat.s.W *= len;
    */

    QuatFromMat(&ThisRot, dest_quat );




    //printf("v2:  %f, %f, %f\n",v2[0],v2[1],v2[2]);
    if (type == 3) {
      if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(v2[0])+","+f2s(v2[1])+","+f2s(v2[2])  + " " + f2s(smoothness) + " "+target_param);
      else command_q_b.add_raw("param_set "+ f2s(v2[0])+","+f2s(v2[1])+","+f2s(v2[2]) + " "+target_param);
    } else
    {
      if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(dest_quat.s.X)+","+f2s(dest_quat.s.Y)+","+f2s(dest_quat.s.Z)+","+f2s(dest_quat.s.W)  + " " + f2s(smoothness) + " "+target_param);
      else command_q_b.add_raw("param_set "+ f2s(dest_quat.s.X)+","+f2s(dest_quat.s.Y)+","+f2s(dest_quat.s.Z)+","+f2s(dest_quat.s.W)  + " " + target_param);
    }
    parent->vsx_command_queue_b(this);

    LastRot = ThisRot;

    //vsx_vector p=(world-(parent->get_pos_p()+pos))/size;
    //deltamove=clickpos-p;
    //clickpos=(remWorld-pos)/size;
    mouse.set_cursor_pos(remPointer.x,remPointer.y);
  } else
  if (!controlling)
  vsx_widget_base_controller::event_mouse_move(distance, coords);
}

void vsx_widget_controller_ab::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  mouse.show_cursor();
  _delete();
}

void vsx_widget_controller_ab::vsx_command_process_b(vsx_command_s *t) {
  if (t->cmd == "pg64_ok") {
    std::vector<vsx_string> parts;
    vsx_string deli = ",";
    t->parts[3] = base64_decode(t->parts[3]);
    explode(t->parts[3],deli, parts);
    if (parts.size() == 4) {
      dest_quat.s.X = inter_quat.s.X = s2f(parts[0]);
      dest_quat.s.Y = inter_quat.s.Y = s2f(parts[1]);
      dest_quat.s.Z = inter_quat.s.Z = s2f(parts[2]);
      dest_quat.s.W = inter_quat.s.W = s2f(parts[3]);
      Matrix3fSetRotationFromQuat4f(&LastRot, &dest_quat);
    }
  } else vsx_widget_base_controller::vsx_command_process_b(t);
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

vsx_widget_controller_color::vsx_widget_controller_color() {
	angle = angle_dest = 0;
	color_x = color_y = 1;
  type = 0;
}

void vsx_widget_controller_color::init() {
  type = VSX_WIDGET_TYPE_CONTROLLER;
  base_size = 0.8;
  alpha_down = angle_down = color_down = false;
  set_size(vsx_vector(0.03,0.03));
  set_pos(vsx_vector(-0.02));
	bgcolor.r = 0;
	bgcolor.g = 0;
	bgcolor.b = 0;
	get_value();
	generate_menu();
	menu->init();
}

void vsx_widget_controller_color::calculate_color() {
  color.hsv(angle_dest,color_x,color_y);
  //color.a = 1;
}

void vsx_widget_controller_color::send_to_server() {
  if (type == 0) {
    if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(color.r)+","+f2s(color.g)+","+f2s(color.b)  + " " + f2s(smoothness) + " "+target_param);
    else command_q_b.add_raw("param_set "+ f2s(color.r)+","+f2s(color.g)+","+f2s(color.b) + " "+target_param);
  } else {
    if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(color.r)+","+f2s(color.g)+","+f2s(color.b)+","+f2s(color.a)  + " " + f2s(smoothness) + " "+target_param);
    else command_q_b.add_raw("param_set "+ f2s(color.r)+","+f2s(color.g)+","+f2s(color.b)+","+f2s(color.a) + " "+target_param);
  }
  parent->vsx_command_queue_b(this);
}

void vsx_widget_controller_color::vsx_command_process_b(vsx_command_s *t) {
	// 0=pg64_ok [1=module_name] [2=param] MC4yMTI4OTE5OTU5MDY4Mjk4MzM5OCwwLjAwMDAwMDAwMDAwMDAwOTI5NjU3LDAuOTk5OTk5ODgwNzkwNzEwNDQ5MjIsMS4wMDAwMDAwMDAwMDAwMDAwMDAwMA== 2054
  if (t->cmd == "pg64_ok") {
    std::vector<vsx_string> parts;
    vsx_string deli = ",";
    t->parts[3] = base64_decode(t->parts[3]);
    explode(t->parts[3],deli, parts);
    if (parts.size() == 3) {
      vsx_color t;
      color.r = s2f(parts[0]);
      color.g = s2f(parts[1]);
      color.b = s2f(parts[2]);
      color.get_hsv(&t);
      angle_dest = t.h;
      color_x = t.s;
      color_y = t.v;
    } else
    if (parts.size() == 4) {
      vsx_color t;
      color.r = s2f(parts[0]);
      color.g = s2f(parts[1]);
      color.b = s2f(parts[2]);
      //printf("col in: %f %f %f\n",color.r, color.g, color.b);

      color.get_hsv(&t);
      angle_dest = t.h;
      //printf("col out: %f %f %f\n",t.h, t.s, t.v);
      color_x = t.s;
      color_y = t.v;
      color.a = s2f(parts[3]);
    }

    set_value(s2f(t->parts[3]));

  //cout <<"got value: "<<t->parts[3]<<endl;}
  //else if (t->cmd=="delete" && !owned) _delete();

  } else vsx_widget_base_controller::vsx_command_process_b(t);
}

void vsx_widget_controller_color::draw() {
  if (!visible) return;
  parentpos = parent->get_pos_p();
  vsx_widget_base_controller::draw();

	glPushMatrix();
	glTranslatef(parentpos.x+pos.x, parentpos.y+pos.y, pos.z);
	float sx = size.x*0.5;
	float sy = size.y*0.5;
	float gridc = 70.0;
	float gx = size.x/gridc;
	float gy = size.y/gridc;
	float gi = 1.0/gridc;

	float ss = size.x*base_size;
	float wheel_h = size.x*(1-base_size);

	float cur_angle = angle = angle_dest;
	cur_angle += 0.5;
	if (cur_angle >= 1.0) cur_angle -=1.0;
	for (float i = 0; i < 1.0; i+=gi) {
      glBegin(GL_QUADS);
        vsx_color aa;
        aa.hsv(cur_angle,1,1); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+i*ss,sy,0);
        glVertex3f(-sx+i*ss,sy-wheel_h,0);
        cur_angle += gi;
        if (cur_angle >= 1.0) cur_angle -=1.0;

        aa.hsv(cur_angle,1,1); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+(i+gi)*ss,sy-wheel_h,0);
        glVertex3f(-sx+(i+gi)*ss,sy,0);
      glEnd();
  }
  glColor4f(0,0,0,1);
 	glBegin(GL_LINE_STRIP);
 	  glVertex3f(-sx+0.5*ss,sy,0);
 	  glVertex3f(-sx+0.5*ss,sy-wheel_h*0.2,0);
  glEnd();

  glColor4f(1,1,1,1);
 	glBegin(GL_LINE_STRIP);
 	  glVertex3f(-sx+0.5*ss,sy-wheel_h*0.8,0);
 	  glVertex3f(-sx+0.5*ss,sy-wheel_h,0);
  glEnd();

	cur_angle -= 0.5;
	if (cur_angle < 0.0) cur_angle +=1.0;

  gridc = 10.0;
  gi = 1.0 / gridc;
  gx = size.x/gridc;
  gy = size.y/gridc;

	for (float i = 0; i < 1.0; i+=gi) {
	  for (float j = 0; j < 1.0; j+=gi) {
      glBegin(GL_QUADS);
        vsx_color aa;
        aa.hsv(cur_angle,i,j); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+i*ss,-sy+j*ss,0);

        aa.hsv(cur_angle,i+gi,j); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+(i+gi)*ss,-sy+j*ss,0);

        aa.hsv(cur_angle,i+gi,j+gi); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+(i+gi)*ss,-sy+(j+gi)*ss,0);

        aa.hsv(cur_angle,i,j+gi); glColor3f(aa.r, aa.g, aa.b);
        glVertex3f(-sx+i*ss,-sy+(j+gi)*ss,0);
      glEnd();
	  }
  }


  glColor4f(color.r, color.g, color.b, color.a);
  glBegin(GL_QUADS);
    glVertex3f(sx-0.2*size.x,sy-0.2*size.y,0);
    glVertex3f(sx,sy-0.2*size.y,0);
    glVertex3f(sx,sy,0);
    glVertex3f(sx-0.2*size.x,sy,0);
  //glEnd();

  glColor4f(color.r, color.g, color.b, 1.0);
  //glBegin(GL_QUADS);
    glVertex3f(sx-0.2*size.x,sy-0.2*size.y,0);
    glVertex3f(sx,sy-0.2*size.y,0);
    glColor4f(color.r, color.g, color.b, 0);
    glVertex3f(sx,-sy,0);
    glVertex3f(sx-0.2*size.x,-sy,0);
  glEnd();

  // color_x
  glColor4f(1,1,1,0.7);
 	glBegin(GL_LINE_STRIP);
 	  glVertex3f(-sx+ss*color_x,-sy,0);
 	  glVertex3f(-sx+ss*color_x,-sy+ss,0);
  glEnd();

  // color_y
 	glBegin(GL_LINE_STRIP);
 	  glVertex3f(-sx,-sy+ss*color_y,0);
 	  glVertex3f(-sx+ss,-sy+ss*color_y,0);
  glEnd();

  // alpha
 	glBegin(GL_LINE_STRIP);
 	  glVertex3f(-sx+ss,-sy+ss*color.a,0);
 	  glVertex3f(sx,-sy+ss*color.a,0);
  glEnd();




	/*
  glBegin(GL_QUADS);
    vsx_vector aa;
    aa.hsv(0.0,0.0,0.0); glColor3f(aa.r, aa.g, aa.b);
    //glColor3f(0,0,0);
    glVertex3f(-sx,-sy,0);

    glVertex3f(sx,-sy,0);

    aa.hsv(0.0,1.0,1.0); glColor3f(aa.r, aa.g, aa.b);

//    aa.hsl(1,1,1);

    glVertex3f(sx,sy,0);

    aa.hsv(0.0,0.0,1.0); glColor3f(aa.r, aa.g, aa.b);

    //aa.hsl(1,0,1);
    glVertex3f(-sx,sy,0);
  glEnd();*/
  glPopMatrix();
}

void vsx_widget_controller_color::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) {
  //vsx_vector world2 = world-get_pos_p();
  //printf("gruu2\n");
  if (color_down) {
    color_x = (distance.center.x+size.x/2)/(size.x*base_size);
    if (color_x < 0) color_x = 0;
    if (color_x > 1) color_x = 1;

    color_y = (distance.center.y+size.y/2)/(size.y*base_size);
    if (color_y < 0) color_y = 0;
    if (color_y > 1) color_y = 1;
    calculate_color();
    send_to_server();
  } else
  if (angle_down) {
    if (mouse.position!=remPointer) {
      //printf("wx  %f  cx  %f\n",world2.x, clickpos.x);
      angle_dest -= (distance.center.x-remWorld.x)*20;
      while (angle_dest > 1) angle_dest -= 1.0;
      while (angle_dest < 0) angle_dest += 1.0;
      mouse.set_cursor_pos(remPointer.x,remPointer.y);
      calculate_color();
      send_to_server();
    }
  } else
  if (alpha_down) {
    color.a = (distance.center.y+size.y/2)/(size.y*base_size);
    if (color.a < 0) color.a = 0;
    if (color.a > 1) color.a = 1;
    calculate_color();
    send_to_server();
  }
  else
  vsx_widget_base_controller::event_mouse_move(distance,coords);
}

void vsx_widget_controller_color::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  if (angle_down || alpha_down) mouse.show_cursor();
  alpha_down = angle_down = color_down = false;
  vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_controller_color::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  //clickpos.x += size.x/2;
  //clickpos.y += size.y/2;
  color_down = angle_down = alpha_down = false;
  //world = world-pos;
  remWorld = distance.center;//-get_pos_p();
/*  a_focus = this;
  m_focus = this;
  k_focus = this;*/
  //vsx_vector p = parent->get_pos_p();
  //p += pos;
  //printf("wx : %f\n",world.x);
  if (button == 0) {
    if (((distance.center.x > -size.x/2) && (distance.center.x < -size.x/2+base_size*size.x))
    && ((distance.center.y > -size.y/2) && (distance.center.y < -size.y/2+base_size*size.y)))
    {
      //printf("color down\n");
      color_down = true;
    } else
    if (((distance.center.x > -size.x/2) && (distance.center.x < -size.x/2+base_size*size.x))
    && ((distance.center.y > -size.y/2+base_size*size.y) && (distance.center.y < size.y/2)))
    {
      //printf("angle down\n");
      angle_down = true;
      remPointer=mouse.position;
      mouse.hide_cursor();
    } else
    if (((distance.center.x > -size.x/2+base_size*size.x) && (distance.center.x < size.x/2))
    && ((distance.center.y > -size.y/2) && (distance.center.y < -size.y/2+base_size*size.y)))
    {
      //printf("alpha down\n");
      alpha_down = true;
      //mouse.hide_cursor();
    }
  }
  if (color_down) event_mouse_move(distance,coords);
  if (alpha_down) event_mouse_move(distance,coords);
//  vsx_widget_3d_grid_comp::event_mouse_down(world,screen,button);
  vsx_widget_base_controller::event_mouse_down(distance,coords,button);
}

void vsx_widget_controller_color::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
	_delete();
}

bool vsx_widget_controller_color::event_key_down(signed long key, bool alt, bool ctrl, bool shift) {
  switch (key) {
    case 'f':
      angle_dest+= 0.01;
      if (angle_dest > 1.0) angle_dest -= 1.0;
      if (angle_dest < 0.0) angle_dest += 1.0;
      calculate_color();
      send_to_server();
      return false;
    break;
    case 's':
      angle_dest-= 0.01;
      if (angle_dest > 1.0) angle_dest -= 1.0;
      if (angle_dest < 0.0) angle_dest += 1.0;
      calculate_color();
      send_to_server();
      return false;
    break;
  }
  return true;
}

//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------

vsx_widget_controller_pad::vsx_widget_controller_pad() {
//	angle = angle_dest = 0;
//	color_x = color_y = 1;
  widget_type = 0;
  a.x = 0;
  a.y = 0;

  b.x = 1;
  b.y = 1;

  //c.x = 1;
  //c.y = -1;

  //c.x = -1;
  //c.y = -1;
  widget_type = VSX_WIDGET_TYPE_CONTROLLER;
  coord_type = VSX_WIDGET_COORD_CENTER;
  set_size(vsx_vector(0.1,0.1));
  set_pos(vsx_vector(-0.06));

  //size.x = 0.07;
	//size.y = 0.07;
	//pos.x = -0.06;
	generate_menu();
	menu->init();
  drawing = false;
}

void vsx_widget_controller_pad::init() {
	prev_pos = 0;
	draw_area = size.x*0.5-sizeunit*1.3*0.5f;
  knob_x1 = add(new vsx_widget_knob,"x1");
  knob_x1->set_pos(vsx_vector(-draw_area-sizeunit*0.25,0));
  ((vsx_widget_knob*)knob_x1)->target_param = "x1";
  ((vsx_widget_knob*)knob_x1)->bgcolor.a=0;
  ((vsx_widget_knob*)knob_x1)->owned=true;
  ((vsx_widget_knob*)knob_x1)->drawconnection=false;
  ((vsx_widget_knob*)knob_x1)->isolate=true;
  knob_x1->set_size(vsx_vector(sizeunit*0.5,sizeunit*0.5));
  knob_x1->init();

  knob_y1 = add(new vsx_widget_knob,"y1");
  knob_y1->set_pos(vsx_vector(0,draw_area+sizeunit*0.25));
  ((vsx_widget_knob*)knob_y1)->bgcolor.a=0;
  ((vsx_widget_knob*)knob_y1)->owned=true;
  ((vsx_widget_knob*)knob_y1)->drawconnection=false;
  ((vsx_widget_knob*)knob_y1)->isolate=true;
  ((vsx_widget_knob*)knob_y1)->target_param = "y1";
  knob_y1->set_size(vsx_vector(sizeunit*0.5,sizeunit*0.5));
  knob_y1->init();

  knob_x2 = add(new vsx_widget_knob,"x2");
  knob_x2->set_pos(vsx_vector(draw_area+sizeunit*0.25,0));
  ((vsx_widget_knob*)knob_x2)->target_param = "x2";
  ((vsx_widget_knob*)knob_x2)->bgcolor.a=0;
  ((vsx_widget_knob*)knob_x2)->owned=true;
  ((vsx_widget_knob*)knob_x2)->drawconnection=false;
  ((vsx_widget_knob*)knob_x2)->isolate=true;
  ((vsx_widget_knob*)knob_x2)->target_value = 1.0;
  ((vsx_widget_knob*)knob_x2)->value = 1.0;
  knob_x2->set_size(vsx_vector(sizeunit*0.5,sizeunit*0.5));
  knob_x2->init();

  knob_y2 = add(new vsx_widget_knob,"y2");
  knob_y2->set_pos(vsx_vector(0,-draw_area-sizeunit*0.38));
  ((vsx_widget_knob*)knob_y2)->bgcolor.a=0;
  ((vsx_widget_knob*)knob_y2)->owned=true;
  ((vsx_widget_knob*)knob_y2)->drawconnection=false;
  ((vsx_widget_knob*)knob_y2)->isolate=true;
  ((vsx_widget_knob*)knob_y2)->target_param = "y2";
  ((vsx_widget_knob*)knob_y2)->value = 1.0;
  ((vsx_widget_knob*)knob_y2)->target_value = 1.0;
  knob_y2->set_size(vsx_vector(sizeunit*0.5,sizeunit*0.5));
  knob_y2->init();

}

void vsx_widget_controller_pad::draw() {
  if (!visible) return;
  parentpos = parent->get_pos_p();
  vsx_widget_base_controller::draw();
  glColor4f(1.0f,1.0f,1.0f,0.2f);
  draw_box_c(parentpos+pos, draw_area,draw_area);
  int start_prevs = prev_pos-300;
  int stop_prevs = prev_pos;
  if (start_prevs < 0) start_prevs += 300;
  if (start_prevs >= prev_draw.size()) start_prevs = 0;

  //float inc = 1.0f / ((float)stop_prevs - (float)start_prevs);
  //float alpha = 0.0f;
  glLineWidth(1.0f);
  glBegin(GL_LINE_STRIP);
  bool run = true;
  int i = start_prevs;
  int j = 0;
  while (run)
  {
  //for (int i = start_prevs; i < stop_prevs; i++) {
  	glColor4f(1,1,1,prev_draw[i].z);
  	glVertex2f(prev_draw[i].x+parentpos.x+pos.x,prev_draw[i].y+parentpos.y+pos.y);
  	i++; j++;
  	if (i >= prev_draw.size()) run = false;
  	if (i >= 300) i = 0;
  	if (j >= 300) run = false;
  }
  glEnd();
}

void vsx_widget_controller_pad::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  if (
  		button == 0
  		&& fabs(distance.center.x) < draw_area
  		&& fabs(distance.center.y) < draw_area
  		)
  {
  	drawing = true;
  	event_mouse_move(distance,coords);
  }
  else
  drawing = false;
  //if (!drawing)
  vsx_widget_base_controller::event_mouse_down(distance,coords,button);
}

void vsx_widget_controller_pad::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
  drawing = false;
  if (button != 0)
  vsx_widget_base_controller::event_mouse_up(distance,coords,button);
}

void vsx_widget_controller_pad::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) {
  if (drawing) {
  	float xx = ((distance.center.x+draw_area)/(draw_area*2));
  	if (xx < 0.0f) xx = 0.0f;
  	if (xx > 1.0f) xx = 1.0f;
  	tracer.x = ((vsx_widget_knob*)knob_x1)->value + xx * (((vsx_widget_knob*)knob_x2)->value - ((vsx_widget_knob*)knob_x1)->value);
//  	printf("xx: %f\n",tracer.x);
  	float yy = (1-(distance.center.y+draw_area)/(draw_area*2));
  	if (yy < 0.0f) yy = 0.0f;
  	if (yy > 1.0f) yy = 1.0f;
  	tracer.y = ((vsx_widget_knob*)knob_y1)->value + yy * (((vsx_widget_knob*)knob_y2)->value - ((vsx_widget_knob*)knob_y1)->value);
  	//printf("yy: %f\n",tracer.y);
  	prev_draw[prev_pos].x = distance.center.x;
  	prev_draw[prev_pos].y = distance.center.y;
  	if (prev_draw[prev_pos].x < -draw_area) prev_draw[prev_pos].x = -draw_area;
  	if (prev_draw[prev_pos].y < -draw_area) prev_draw[prev_pos].y = -draw_area;
  	if (prev_draw[prev_pos].x > draw_area) prev_draw[prev_pos].x = draw_area;
  	if (prev_draw[prev_pos].y > draw_area) prev_draw[prev_pos].y = draw_area;
  	prev_draw[prev_pos].z = 2.0f;
  	prev_pos++;
  	if (prev_pos > 300) prev_pos = 0; // cyclic buffer
  	for (int i = 0; i < prev_draw.size(); i++)
  	{
    	prev_draw[i].z -= 2.0f/300.0f;
  		if (prev_draw[i].z < 0.0f) prev_draw[i].z = 0.0f;
  	}

    //tracer.x = xx;//(distance.center.x)/draw_area;
    //tracer.y = (distance.center.y)/draw_area;
    send_to_server();
  }
  else vsx_widget_base_controller::event_mouse_move(distance,coords);
}
void vsx_widget_controller_pad::vsx_command_process_b(vsx_command_s *t) {
	if (t->cmd == "param_set_interpolate") {
		printf("param_set_interp\n");
		if (t->parts[3] == "x1") {
			printf("setting new x1\n");
			a.x = s2f(t->parts[1]);
		}
		if (t->parts[3] == "y1") a.y = s2f(t->parts[1]);
		if (t->parts[3] == "x2") b.x = s2f(t->parts[1]);
		if (t->parts[3] == "y2") b.y = s2f(t->parts[1]);
	}  vsx_widget_base_controller::vsx_command_process_b(t);
}


void vsx_widget_controller_pad::send_to_server() {
  if (ptype == 0) {
    if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(tracer.r)+","+f2s(tracer.g)+","+f2s(tracer.b)  + " " + f2s(smoothness) + " "+target_param);
    else command_q_b.add_raw("param_set "+ f2s(tracer.r)+","+f2s(tracer.g)+","+f2s(tracer.b) + " "+target_param);
  } else {
    if (smoothness>=0) command_q_b.add_raw("param_set_interpolate "+ f2s(tracer.r)+","+f2s(tracer.g)+","+f2s(tracer.b)+","+f2s(tracer.a)  + " " + f2s(smoothness) + " "+target_param);
    else command_q_b.add_raw("param_set "+ f2s(tracer.r)+","+f2s(tracer.g)+","+f2s(tracer.b)+","+f2s(tracer.a) + " "+target_param);
  }
  parent->vsx_command_queue_b(this);
}


//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------
/*
vsx_widget_param_sequence::vsx_widget_param_sequence() {
}


void vsx_widget_param_sequence::init() {
  if (init_run) return;
  init_children();
  init_run = true;
  num_rows = 20;
  float fs = 0.002; // character size (font units)
  float char_w = fs*2/5; // character width
  size.x = 48*char_w;
	size.y = fs*(float)num_rows;
	pos.x = -0.04;
	cur_x = 0;
	cur_y = 0;
	scroll_y = 0;

  target_size = size;
  target_pos = pos;

	bgcolor = skin_color[1];
	darkshade = 1.0;
	title = "sequence editor";
	//printf("seqedit id: %d\n",id);
	command_q_b.add_raw("pseq_g "+target_comp+" "+target_param+" "+i2s(id));
  parent->vsx_command_queue_b(this);
  edit_mode = false;
}

void vsx_widget_param_sequence::draw() {
  if (!visible) return;
  vsx_widget_base_controller::draw();
  parentpos = parent->get_pos_p();
  vsx_vector pp;
  float fs = 0.002; // character size (font units)
  float char_w = fs*2/5; // character width
  pp.x = pos.x + parentpos.x-size.x/2;
  pp.y = pos.y + parentpos.y+size.y/2-fs;
  pp.z = pos.z;
  glPushMatrix();
  glTranslatef(pp.x,pp.y,pp.z);
  float opx = pos.x + parentpos.x-size.x/2;
  float field_chars;
  float field_width;
  float field_startx,fsxi;
  float field_starty,fsyi;

  myf.print(vsx_vector(0),"row  delay smooth value","ascii",fs);
  //pp.y -= fs;

  int i_row = 0; // internal row counter, for placing highlights
  field_starty = 1;
  vsx_string s;// && ri-scroll_y < num_rows
  for (int ri = scroll_y; ri < slist.size() && ri-scroll_y < num_rows-1; ++ri) {
  //for (std::vector<pseq_info>::iterator it = slist.begin(); it != slist.end(); ++it) {

    // ROW NUMBER
    field_startx = 0;
    field_chars = 5;
    field_width = field_chars * char_w;
    fsxi = field_startx*char_w;
    fsyi = -field_starty*fs;
    // box background
    glColor3f(skin_color[2].r,skin_color[2].g,skin_color[2].b);
    glBegin(GL_QUADS);      glVertex3f(fsxi,                fsyi,0);  glVertex3f(fsxi+field_width,    fsyi,0);   glVertex3f(fsxi+field_width,    fsyi+fs,0);      glVertex3f(fsxi,                fsyi+fs,0);    glEnd();
    // vertical line
    glColor3f(1,1,1);
    glBegin(GL_QUADS);      glVertex3f(fsxi+field_width,            fsyi, 0);      glVertex3f(fsxi+field_width+fs*0.05,     fsyi, 0);      glVertex3f(fsxi+field_width+fs*0.05,     fsyi+fs, 0);      glVertex3f(fsxi+field_width,     fsyi+fs, 0);    glEnd();
    // data
    s = i2s(slist[ri].id);
    myf.print(vsx_vector(fsxi,fsyi),str_pad(s,"0",5,STR_PAD_LEFT),"ascii",fs);
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------------------------
    // DELAY
    field_startx += field_chars;
    field_chars = 5;
    field_width = field_chars * char_w;
    fsxi = field_startx*char_w;
    fsyi = -field_starty*fs;
    // box background
    if (cur_x == 0 && cur_y == i_row)
    glColor3f(skin_color[2].r*1.7,skin_color[2].g*1.7,skin_color[2].b*1.7); else
    glColor3f(skin_color[2].r,skin_color[2].g,skin_color[2].b);
    glBegin(GL_QUADS);      glVertex3f(fsxi,                fsyi,0);  glVertex3f(fsxi+field_width,    fsyi,0);   glVertex3f(fsxi+field_width,    fsyi+fs,0);      glVertex3f(fsxi,                fsyi+fs,0);    glEnd();
    // vertical line
    glColor3f(1,1,1);
    glBegin(GL_QUADS);      glVertex3f(fsxi+field_width,            fsyi, 0);      glVertex3f(fsxi+field_width+fs*0.05,     fsyi, 0);      glVertex3f(fsxi+field_width+fs*0.05,     fsyi+fs, 0);      glVertex3f(fsxi+field_width,     fsyi+fs, 0);    glEnd();
    // data
    if (cur_x == 0 && cur_y == i_row && edit_mode)
    myf.print(vsx_vector(fsxi,fsyi),str_pad(edit_str+"_"," ",5,STR_PAD_RIGHT,STR_PAD_OVERFLOW_LEFT),"ascii",fs);
    else
    myf.print(vsx_vector(fsxi,fsyi),str_pad(f2s(slist[ri].delay)," ",5,STR_PAD_LEFT),"ascii",fs);

    // SMOOTHESS / INTERPOLATION
    field_startx += field_chars;
    field_chars = 8;
    field_width = field_chars * char_w;
    fsxi = field_startx*char_w;
    fsyi = -field_starty*fs;
    // box background
    if (cur_x == 1 && cur_y == i_row)
    glColor3f(skin_color[2].r*1.7,skin_color[2].g*1.7,skin_color[2].b*1.7); else
    glColor3f(skin_color[2].r,skin_color[2].g,skin_color[2].b);
    glBegin(GL_QUADS);      glVertex3f(fsxi,                fsyi,0);  glVertex3f(fsxi+field_width,    fsyi,0);   glVertex3f(fsxi+field_width,    fsyi+fs,0);      glVertex3f(fsxi,                fsyi+fs,0);    glEnd();
    // vertical line
    glColor3f(1,1,1);
    glBegin(GL_QUADS);      glVertex3f(fsxi+field_width,            fsyi, 0);      glVertex3f(fsxi+field_width+fs*0.05,     fsyi, 0);      glVertex3f(fsxi+field_width+fs*0.05,     fsyi+fs, 0);      glVertex3f(fsxi+field_width,     fsyi+fs, 0);    glEnd();
    // data
    if (cur_x == 1 && cur_y == i_row && edit_mode)
    myf.print(vsx_vector(fsxi,fsyi),str_pad(edit_str+"_"," ",8,STR_PAD_RIGHT,STR_PAD_OVERFLOW_LEFT),"ascii",fs);
    else
    myf.print(vsx_vector(fsxi,fsyi),str_pad(f2s(slist[ri].interpolation)," ",8,STR_PAD_LEFT),"ascii",fs);

    // VALUE
    field_startx += field_chars;
    field_chars = 30;
    field_width = field_chars * char_w;
    fsxi = field_startx*char_w;
    fsyi = -field_starty*fs;
    // box background
    if (cur_x == 2 && cur_y == i_row)
    glColor3f(skin_color[2].r*1.7,skin_color[2].g*1.7,skin_color[2].b*1.7); else
    glColor3f(skin_color[2].r,skin_color[2].g,skin_color[2].b);
    glBegin(GL_QUADS);      glVertex3f(fsxi,                fsyi,0);  glVertex3f(fsxi+field_width,    fsyi,0);   glVertex3f(fsxi+field_width,    fsyi+fs,0);      glVertex3f(fsxi,                fsyi+fs,0);    glEnd();
    // vertical line
    glColor3f(1,1,1);
    glBegin(GL_QUADS);      glVertex3f(fsxi+field_width,            fsyi, 0);      glVertex3f(fsxi+field_width+fs*0.05,     fsyi, 0);      glVertex3f(fsxi+field_width+fs*0.05,     fsyi+fs, 0);      glVertex3f(fsxi+field_width,     fsyi+fs, 0);    glEnd();
    // data
    if (cur_x == 2 && cur_y == i_row && edit_mode)
    myf.print(vsx_vector(fsxi,fsyi),str_pad(edit_str+"_"," ",120,STR_PAD_RIGHT,STR_PAD_OVERFLOW_LEFT),"ascii",fs);
    else
    myf.print(vsx_vector(fsxi,fsyi),slist[ri].value,"ascii",fs);


//    pp.x += fw*5;

//    myf.print(pp,str_pad(f2s((*it).delay),' ',5,STR_PAD_LEFT),"ascii",fs);
//    glBegin(GL_QUADS);
//      glColor3f(1,1,1);
//      glVertex3f(pp.x+fw*5,pp.y,pp.z);
//      glVertex3f(pp.x+fw*5.08,pp.y,pp.z);
//      glVertex3f(pp.x+fw*5.08,pp.y+fs,pp.z);
//      glVertex3f(pp.x+fw*5,pp.y+fs,pp.z);
//    glEnd();

//    pp.x += fw*5;
//    glBegin(GL_QUADS);
//      glColor3f(skin_color[2].b,skin_color[2].r,skin_color[2].g);
//      glVertex3f(pp.x,pp.y,pp.z);
//      glVertex3f(pp.x+fw*8,pp.y,pp.z);
//      glVertex3f(pp.x+fw*8,pp.y+fs,pp.z);
//      glVertex3f(pp.x,pp.y+fs,pp.z);
//    glEnd();
//    glBegin(GL_QUADS);
//      glColor3f(1,1,1);
//      glVertex3f(pp.x+fw*8,pp.y,pp.z);
//      glVertex3f(pp.x+fw*8.08,pp.y,pp.z);
//      glVertex3f(pp.x+fw*8.08,pp.y+fs,pp.z);
//      glVertex3f(pp.x+fw*8,pp.y+fs,pp.z);
//    glEnd();

//    myf.print(pp,str_pad(f2s((*it).interpolation),' ',8,STR_PAD_LEFT)+"","ascii",fs);
//    pp.x += fw*8;
//  myf.print(pp,(*it).value,"ascii",fs);

    //pp.y -= fs;
    ++field_starty;
    ++i_row;
  }
  glPopMatrix();
}

void vsx_widget_param_sequence::vsx_command_process_b(vsx_command_s *t) {
  pseq_info mp;
  if (t->cmd == "pseql_r") {
    if (t->parts[2] == "a") {
      mp.value = base64_decode(t->parts[3]);
      mp.delay = s2f(t->parts[4]);
      mp.interpolation = s2f(t->parts[5]);
      mp.id = slist.size();
      slist.push_back(mp);
      event_key_down(-103,true,false,false);
    } else
    if (t->parts[2] == "u") {
      //printf("psqlr u %s\n",t->parts[4].c_str());
      int id = s2i(t->parts[6]);
      slist[id].value = base64_decode(t->parts[3]);
      slist[id].delay = s2f(t->parts[4]);
      slist[id].interpolation = s2f(t->parts[5]);
      slist[id].id = s2i(t->parts[6]);

      //event_key_down(-103,true,false,false);
    }
  } else
  if (t->cmd == "pseq_d") {
    //"pseq_d "+controller_id+" "+base64_encode((*it).command)+" "+i2s(row)+" "+f2s((*it).delay));
     //float time_accum;
  //vsx_string cmd;
  //float delay;
    mp.value = base64_decode(t->parts[2]);
    mp.delay = s2f(t->parts[4]);
    mp.interpolation = s2f(t->parts[5]);
    mp.id = slist.size();
    slist.push_back(mp);
  }
}

bool vsx_widget_param_sequence::event_key_down(signed long key, bool alt, bool ctrl, bool shift) {
  bool returnval = true;
  if (edit_mode) returnval = false;
  switch (key) {
    case 13:
      edit_mode = !edit_mode;
      if (edit_mode) {
        if (cur_x == 0) {
          edit_str = f2s(slist[cur_y+scroll_y].delay);
        } else
        if (cur_x == 1) {
          edit_str = f2s(slist[cur_y+scroll_y].interpolation);
        } else
        if (cur_x == 2) {
          edit_str = slist[cur_y+scroll_y].value;
        }
      } else {
        bool send_mesg = true;
        if (cur_x == 0) {
          server_message("pseql u",base64_encode(slist[cur_y+scroll_y].value)+" "+edit_str+" "+f2s(slist[cur_y+scroll_y].interpolation)+" "+i2s(slist[cur_y+scroll_y].id));
        } else
        if (cur_x == 1) {
          server_message("pseql u",base64_encode(slist[cur_y+scroll_y].value)+" "+f2s(slist[cur_y+scroll_y].delay)+" "+edit_str+" "+i2s(slist[cur_y+scroll_y].id));
        } else
        if (cur_x == 2) {
          server_message("pseql u",base64_encode(edit_str)+" "+f2s(slist[cur_y+scroll_y].delay)+" "+f2s(slist[cur_y+scroll_y].interpolation)+" "+i2s(slist[cur_y+scroll_y].id));
        }
        if (send_mesg) {
        }
      }
      if (alt) edit_mode = !edit_mode;
    break;
    // arrow left
    case -100:
      --cur_x;
      if (cur_x < 0) cur_x = 0;
      break;
    // arrow right
    case -102:
      ++cur_x;
      if (cur_x > 2) cur_x = 2;
      break;
    // arrow up
    case -101:
      --cur_y;
      if (cur_y < 0) {
        cur_y = 0;
        --scroll_y;
        if (scroll_y < 0) scroll_y = 0;
      }
      break;
    // arrow down
    case -103:
      ++cur_y;
      if (cur_y > slist.size()-scroll_y-1 || cur_y > num_rows-2) {
        if (ctrl) {
          pseq_info* pd = &slist[scroll_y+cur_y-1];
          server_message("pseql a",pd->value+" "+f2s(pd->delay)+" "+f2s(pd->interpolation));
          --cur_y;
          //++scroll_y;
          //if (scroll_y + cur_y > slist.size()) --scroll_y;
        } else {
          //printf("hoho\n");
          ++scroll_y;
          if (scroll_y + cur_y > slist.size()) --scroll_y;
          --cur_y;
        }
      }
      break;
    case 8:
      if (edit_mode)
      if (edit_str.size())
      edit_str.pop_back();//erase(edit_str.size()-1,1);
      break;
    case 27:
      if (edit_mode) {
        edit_mode = false;
      }
    break;
    default:
      if (edit_mode) {
        if (key > 20)
        edit_str += (char)key;
      } else {
        edit_str = (char)key;
        edit_mode = true;
      }
    break;
  }
  return returnval;

}
*/
#endif
