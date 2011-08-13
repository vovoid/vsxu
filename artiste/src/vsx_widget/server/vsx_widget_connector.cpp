#ifndef VSX_NO_CLIENT
#include "vsx_gl_global.h"
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_math_3d.h"
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsxfst.h"
// local includes
#include "lib/vsx_widget_lib.h"
#include "vsx_widget_anchor.h"
#include "vsx_widget_desktop.h"
#include "vsx_widget_comp.h"

#include "vsx_widget_connector.h"

// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************
// VSX_WIDGET_CONNECTOR ************************************************************************************************

bool vsx_widget_connector::receiving_focus;
float vsx_widget_connector::dim_alpha = 1.0f;

void vsx_widget_connector::vsx_command_process_b(vsx_command_s *t) {
  if (t->cmd == "disconnect") {
    // syntax:
    //   param_unalias [-1/1] [component] [param_name]
    if (alias_conn) {
      vsx_string bb;
      if (((vsx_widget_anchor*)parent)->io == -1) bb = "-1"; else bb = "1";
      if (((vsx_widget_anchor*)parent)->io == -1)
      command_q_b.add_raw("param_unalias "+bb+" "+((vsx_widget_component*)((vsx_widget_anchor*)destination)->component)->name+" "+destination->name);
      else
      command_q_b.add_raw("param_unalias "+bb+" "+((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->name+" "+parent->name);
    } else
    {
      command_q_b.add_raw(
        "param_disconnect "+
        ((vsx_widget_anchor*)parent)->component->name+" "+
        parent->name+" "+
        ((vsx_widget_anchor*)destination)->component->name+" "+
        ((vsx_widget_anchor*)destination)->name
      );
    }
    parent->vsx_command_queue_b(this);
  }
}

int vsx_widget_connector::inside_xy_l(vsx_vector &test, vsx_vector &global)
{
  vsx_vector world = test-global;
  if (!visible) return false;
  if (((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->ethereal_all) return false;
  if (!receiving_focus) return false;
  if (destination)
  if (!((vsx_widget_anchor*)parent)->component->visible || !((vsx_widget_anchor*)destination)->component->visible) return false;
  if (open && order != -1) {
    float s = parent->size.x * 0.2f;
    pv.y = world.y - (parent->size.y/2 - ((float)order)*s*2.2);
    pv.x = world.x + parent->size.y*0.7;
      if ( fabs(pv.x) < s && fabs(pv.y) < s)
      {
        return coord_type;
      }
    }
    if (alt || !ctrl) return 0;
    if (alt && ctrl) return 0;

    //if (transparent) return 0;
    double dx = (ex-sx);
    double dy = (ey-sy);
    vsx_vector x0, x1, x2, t, t2;

    x0.x = world.x;
    x0.y = world.y;

    x2.x = dx;
    x2.y = dy;
      t.cross(x2,x1-x0); // t = x2 x x0
      //t.cross((ex-sx),x0); // t = x2 x x0

      t2 = x2;// - x1; // t2 = x2 - x1
//      double plen = t.norm() / t2.norm();
      //pv = parent->get_pos_p()+pos;
      //world.x += pv.x;
      //world.y += pv.y;
      //double plen = fabs( (ex-sx)*(sy-world.y)-(sx-world.x)*(ey-sy)) / sqrt( (ex - sx)*(ex - sx) + (ey-sy)*(ey-sy) );
      //return 0;
      double divi = sqrt( (ex - sx)*(ex - sx) + (ey-sy)*(ey-sy) );
      if (divi == 0.0) return 0;
      double plen = fabs( (ex-sx)*(world.y)-(world.x)*(ey-sy)) / divi;
      t = x2 - x0;
      double dt2 = t.norm()*t.norm();
      double p2 = plen * plen;
      double plen2 = sqrt(dt2-p2);
      t = x1 - x0;
      dt2 = t.norm()*t.norm();
      double plen3 = sqrt(dt2-p2);
      //printf("plen: %f\n",plen);
      // phase 1 complete
//      if (guu)
      //printf("ff %f %f ::: %f %f %f\n",plen2,plen3,(plen2+plen3),x2.norm(),plen);
    //
      if ((plen2+plen3) <= x2.norm()+0.001 && plen < 0.005) {
        //if (guu) {
          //printf("xyd true %s\n",parent->name.c_str());
        //}
        return coord_type;
      }
  //    if ((plen2+plen3) <= x2.norm() && plen < 0.015) return true;

    //if (guu)
    //printf("xyd false %s\n",parent->name.c_str());
    return 0;
  }
  void vsx_widget_connector::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button) {
    //printf("connector mdown %s\n",name.c_str());
    //if (!open) {
      //if (alt || !ctrl) return false;
      //if (alt && ctrl) return false;
    //}
//    smx = 0;
//    smy = distance.center.y;
//printf("name %s\n",name.c_str());
		//MessageBox(0, "hej_connector", "lala", MB_OK);
		if (button == 0)
		{
    	mouse_down_l = 1;
    	m_focus = this;
    	a_focus = this;
  	}
    //printf("my order is %d\n",order);
//    mouse_down = world;
    //vsx_widget::event_mouse_down(distance,coords,button);
  }

void vsx_widget_connector::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords) {
  move_d(vsx_vector(0.0f,(distance.center.y-mouse_down_pos.center.y)));
}

void vsx_widget_connector::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  if (open) {
    command_q_b.add_raw("connections_order_int 1");
    parent->vsx_command_queue_b(this);
    set_pos(vsx_vector(0));
  }
  vsx_widget::event_mouse_up(distance,coords,button);
}

void vsx_widget_connector::draw() {
  float alpha = 1.0f;
  if (marked_for_deletion) return;

        if (   ( (vsx_widget_component*) ((vsx_widget_anchor*)parent)->component)->deleting)
        {
        	return;
        }
        if (destination)
        if (   ( (vsx_widget_component*) ((vsx_widget_anchor*)destination)->component)->deleting)
        {
        	return;
        }


  if (destination) {
    if (!((vsx_widget_anchor*)destination)->component->visible) return;
      if (
        ((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->open
        && ((vsx_widget_anchor*)parent)->io == -1
        ) {
        alpha = 0.1f;
      }
  } //else return;
  pv = parent->get_pos_p();
  if (!parent->visible)
  {
    open = false;
    ((vsx_widget_anchor*)parent)->conn_open = false;
  }

  if (((vsx_widget_anchor*)parent)->io == -1)
  if (open && order != -1) {
    float s = parent->size.x * 0.2;

    pv.x = pos.x + pv.x - parent->size.y*0.7;
    pv.y = pos.y + pv.y + parent->size.y/2 - ((float)order)*s*2.2;
    pv.z = pos.z;
    if (a_focus == this)
    glColor4f(1,0.5,0.5,alpha);
      else
    glColor4f(0.8,0.8,0.8,alpha);

    /*glBegin(GL_QUADS);
      glVertex3f(pv.x-s,pv.y-s,pv.z);
      glVertex3f(pv.x+s,pv.y-s,pv.z);
      glVertex3f(pv.x+s,pv.y+s,pv.z);
      glVertex3f(pv.x-s,pv.y+s,pv.z);
    glEnd();*/
  }

  if (!destination) {
    //printf("a1:%s:%d\n",name.c_str(),destination);
    ex = pv.x+size.x;
    ey = pv.y+size.y;
  } else {
    vsx_vector dv = destination->get_pos_p();
    ex = dv.x;
    ey = dv.y;
  }


  glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  mtex_blob.bind();
    draw_box_texf(pv.x,pv.y,0,0.004f,0.004f);
    draw_box_texf(ex,ey,0,0.004f,0.004f);
  mtex_blob._bind();

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glEnable(GL_LINE_SMOOTH);
	glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
	glLineWidth(3.0f);
 	glBegin(GL_LINES);
  sx = pv.x;
  sy = pv.y;
        //float dx = ex-sx;
        //float dy = ey-sy;
//          float k = -(1/(dx/dy));
        //float k = -(1/(dx/dy));
        //float k1 = 1.0f/k;
        //glVertex3f(sx,sy,pos.z);
        //1 = sqrt((xk)^2 + (x/k)^2)
        //sqrt(1) = (xk)^2 + (x/k)^2

        //1 = xk+x*1/k
        //1 = x(k+1/k)
        //1/x = k+1/k
        //float x = 0.1f/sqrt(k*k+k1*k1);
        //schinkenteppichkartoffelnsalatbesteckrostshutzfarbe

        //glVertex3f(sx+x,sy+x*k1,pos.z);

 	   	  glVertex3f(sx,sy,pos.z);
        glVertex3f(ex,ey,pos.z);
  glEnd();
  glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  if (m_focus == this || a_focus == this || k_focus == this) {
      glLineWidth(1.0f+fmod(time*20.0f,5.0f));//(float)((int)(time*2000) % 1000)/200 + 1.0);
  } else
  glLineWidth(1.0f);

 	glBegin(GL_LINES);

				if (k_focus == this)
				glColor4f(1,0.5,0.5,alpha);
				else
				{
					if (parent == a_focus || destination == a_focus)
					glColor4f(1,1,1,alpha);
						else
					glColor4f(1,1,1,alpha*(dim_alpha+dim_my_alpha));
				}
 	   	  glVertex3f(sx,sy,pos.z);
        glVertex3f(ex,ey,pos.z);

        //if (macro_child) {
        /*if (destination)
        if (((vsx_widget_anchor*)destination)->component->parent->widget_type != VSX_WIDGET_TYPE_COMPONENT)
        if ( (((vsx_widget_anchor*)parent)->io = -1) ) {
          // if the angle of the line is
          //if ((sx < ex) && (sy > ey)) {
//            transparent = true;
//            glColor4f(1,1,1,0);
          //} else
        	transparent = false;
      	}*/
   	glEnd();
  	glDisable(GL_LINE_SMOOTH);
	//if (!performance_mode)
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  real_pos = pv;
  draw_children();
}

void vsx_widget_connector::before_delete() {
	//return;
    //printf("ondelete %s\n",name.c_str());
  marked_for_deletion = true;
//    if (marked_for_deletion) return;
  ((vsx_widget_anchor*)parent)->connections.remove(this);
  if (destination) {
    ((vsx_widget_anchor*)destination)->connectors.remove(this);
      // see if the destination is an alias that needs to go as well
    vsx_widget_anchor* dest = (vsx_widget_anchor*)destination;
    if (dest)
    if (dest->io != 1)
    if (dest->alias && dest->io == ((vsx_widget_anchor*)parent)->io) {
      dest->_delete();
      ((vsx_widget_component*)dest->component)->macro_fix_anchors();
    }
      //printf("ondelete 2\n");
      // denna var bortkommenterad, gr det nt?
      //((vsx_widget_anchor*)destination)->connectors.remove(this);
      //printf("ondelete 3\n");
    destination = 0;
  }
//    if (((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->component_type != "macro")
}

void vsx_widget_connector::on_delete() {
	//return;
  if (!global_delete)
  ((vsx_widget_anchor*)parent)->update_connection_order();
}

void vsx_widget_connector::init() {
  order = 0;
  dim_my_alpha = 0.0f;
  open = false;
  if (init_run) return;
//    mdx = mdy = 0;
  widget_type = VSX_WIDGET_TYPE_CONNECTOR;
  title = "connector";
  coord_type = VSX_WIDGET_COORD_CORNER;
	transparent = false;
  menu = add(new vsx_widget_popup_menu,".connector_menu");
  menu->commands.adds(VSX_COMMAND_MENU, "disconnect", "disconnect","");
  menu->size.x = 0.3;
  menu->size.y = 0.5;
  init_children();
  mtex_blob.load_png(skin_path+"interface_extras/connection_blob.png");


  constrained_x = false;
  constrained_y = false;
  constrained_z = false;

  if (((vsx_widget_component*)((vsx_widget_anchor*)parent)->component)->component_type == "macro") macro_child = true;
  init_run = true;
  receiving_focus = true;
}





// constructor ----->
vsx_widget_connector::vsx_widget_connector()
{
  alias_conn = false;
  visible = 1;
  destination = 0;
  receiving_focus = true;
  support_interpolation = true;
}

bool vsx_widget_connector::event_key_down(signed long key, bool alt, bool ctrl, bool shift) {
  if (key == 127) {
    command_q_b.add_raw("disconnect");
    vsx_command_queue_b(this);
  }
  return true;
}
#endif
