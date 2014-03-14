/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#include "vsx_gl_global.h"
#include <gl_helper.h>
#include <map>
#include <list>
#include <vector>
#include <math.h>
#include "vsx_texture_info.h"
#include "vsx_texture.h"
#include "vsx_command.h"
#include "vsx_font.h"
#include "vsxfst.h"
#include "vsx_param.h"
#include "vsx_module.h"
// local includes
#include "log/vsx_log_a.h"
#include "vsx_widget_base.h"
#include "window/vsx_widget_window.h"
#include "vsx_widget_desktop.h"
#include <vsx_command_client_server.h>
#include "server/vsx_widget_server.h"
#include "server/vsx_widget_comp.h"
#include "vsx_widget_module_chooser.h"
#include "helpers/vsx_widget_assistant.h"
#include "helpers/vsx_widget_object_inspector.h"
#include "dialogs/vsx_widget_window_statics.h"

using namespace std;

HTNode::HTNode() {
//  module_info = 0;
  value = 0;
  color.r = 0.8;
  color.g = 0.8 ;
  color.b = 1;
  color.a = 1;
  size.x = 0.13;
  size.y = 0.025;
}

HTNode::~HTNode() {
  for (std::vector<HTNode*>::iterator it = children.begin(); it != children.end(); ++it) {
    delete *it;
  }
}

vsx_command_list module_chooser_colors;
std::map<vsx_string,vsx_color> mc_colors;
std::map<vsx_string,vsx_color> mc_r_colors;


// this function will build a tree structure out of a string
// string syntax: texture;generators;simple;jaw's_own_texgen
//
HTNode* HTNode::add(vsx_string add_name,vsx_module_info* m_info) {
  std::vector<vsx_string> add_c;
  vsx_string deli = ";";
  split_string(add_name,deli,add_c,-1);
  if (!module_info) {
    // 0ptimiz3d
    module_info = m_info;
  //  module_info = new vsx_module_info;
  //  module_info->identifier = add_name;
  }

  //vsx_string deli2 = ":";
  //vsx_avector<vsx_string> c_parts;
  //explode(module_info->component_class,deli2,c_parts);
  vsx_string comp_class = module_info->component_class;
  //printf("comp_class: %s\n",comp_class.c_str());

  if (add_c.size() > 1) {
    // we're not the last part of the string
    // we start by looking in our children to see if the supernode already exists
    HTNode* search_result = 0;
    vsx_string this_name = add_c[0];
    for (unsigned long i = 0; i < children.size(); i++) {
      if (children[i]->name == this_name) {
        search_result = children[i];
        i = children.size()+1;
      }
    }
    add_c.erase(add_c.begin());
    vsx_string new_add_name;
    if (add_c.size())
    new_add_name = implode(add_c,vsx_string(";"));
    if (search_result) {
      // ask the child to do some creation
      search_result->module_info = module_info;
      search_result->add(new_add_name,m_info);
    } else
    {
      // the child we need to send further instructions to doesn't exist, so create it
      HTNode* b = new HTNode();
      children.push_back(b);
      b->name = this_name;
      b->value = 0;
      //b->size.x = 0.1;
      //b->size.y = 0.02;
      b->module_info = module_info;

      if (mc_colors.find(comp_class) != mc_colors.end()) {
        b->color = mc_colors[comp_class];
      } else
      if (mc_colors.find("default") != mc_colors.end()) {
        b->color = mc_colors["default"];
      } else
      {
        b->color.r = 0.8;
        b->color.g = 0.8;
        b->color.b = 0.8;
      }
      return b->add(new_add_name,m_info);
    }
  } else {
      size.x *= 1.05;
      size.y *= 1.05;
      if (size.x > 0.2) size.x = 0.2;
      if (size.y > 0.1) size.y = 0.1;
    // we've only been told to create this, so do that and nothing more.
    HTNode* b = new HTNode();
    //b->size.x = 0.1;
    //b->size.y = 0.02;
    if (module_info->component_class == "resource") {
      std::vector<vsx_string> parts;
      vsx_string deli = ".";
      explode(add_name,deli, parts);
      if (parts.size() > 1) {
        if (mc_r_colors.find(parts[parts.size()-1]) != mc_r_colors.end()) {
          b->color = mc_r_colors[parts[parts.size()-1]];
        } else {
          b->color.r = 0.5;
          b->color.g = 0.5;
          b->color.b = 0.5;
        }
      } else {
        if (mc_colors.find("resources_") != mc_colors.end()) {
          b->color = mc_colors["resources_"];
        } else {
          b->color.r = 0.5;
          b->color.g = 0.5;
          b->color.b = 0.5;
        }
      }
    } else
    if (mc_colors.find(comp_class+"_") != mc_colors.end()) {
      b->color = mc_colors[comp_class+"_"];
    } else
    if (mc_colors.find("default_") != mc_colors.end()) {
      b->color = mc_colors["default_"];
    } else
    {
      b->color.r = 1;
      b->color.g = 1;
      b->color.b = 1;
    }
    b->name = str_replace("\\ "," ",add_name);
    b->value = 1;
    b->module_info = module_info;
    children.push_back(b);
    return b;
  }
  return 0;
}

bool HTNode::isLeaf() {
  if (value) return true; else return false;
//  if (!children.size()) return true; else return false;
}


//----------------------------------------------------------------------------------------------

//double HTModel::lp;
//double HTModel::lk;
//HTModel* HTModel::root;
//HTModel* HTModel::mroot;

    HTModel::HTModel(HTNode* noderoot) {
      lp = 0.1;
      lk = 0.18;
//      lp = 0.1;
//      lk = 0.08;
      parent = 0;
      length = lp;
      nodes = 0;
      z = new HTCoord();
      weight = 1;
      mroot = this;
      this->root = 0;

        if (noderoot->isLeaf()) {
          // single
          //printf("SINGLE MODELROOT\n");
            this->root = new HTModel(mroot,0,noderoot, 0,0);
        } else {
          //printf("COMPOSITE MODELROOT\n");
          // composite
            this->root = new HTModel(mroot,0,noderoot, 0,1);
        }
        this->root->layoutHyperbolicTree();
    }


    void HTModel::layoutHyperbolicTree() {
      //printf("layoutHyperbolicTree\n");
        HTSector* sector = new HTSector();
        double eps = 0.02;
        double d = sqrt(1 - (eps * eps));
        sector->A = new HTCoord(d, eps);
        sector->B = new HTCoord(d, -eps);
        layout(sector, root->getLength());
    }


    void HTModel::layout(HTSector* sector, double length) {
      // Nothing to do for the root node
      if (parent == 0) {
//        printf("parent is 0, so i'm returning\n");
          //return;
      }
      else
      {
        HTCoord zp = parent->getCoordinates();

        double angle = sector->getBisectAngle();
        //printf("sector->getBisectAngle() = %f\n",angle);

        // We first start as if the parent was the origin.
        // We still are in the hyperbolic space.
        z->x = length * cos(angle);
        z->y = length * sin(angle);

        // Then translate by parent's coordinates
        z->translate(zp);
      }
      if (ntype) {
        if (parent != 0) {
            sector->translate(parent->getCoordinates());
            HTCoord* h = new HTCoord(getCoordinates());
            h->x = -h->x;
            h->y = -h->y;
            sector->translate(h);
        }

        float nbrChild = (float)children.size();
        //printf("root->getlength() = %d\n",root->getLength());
        double l1 = (1.2 - mroot->getLength());
        //if (l1 > 0.4) l1 = 0.4;
        double l2 = cos((20.0 * PI) / (2.6 * nbrChild + 38.0));
        //printf("l1: %f\n",l1);
        //printf("l2: %f\n",l2);
        length = mroot->getLength() + (l1 * l2);
        //if (length > 0.7f) length = 0.7f;
        if (length > 0.9f) length = 0.9f;

        double   alpha = sector->getAngle();
        double   omega = sector->A->arg();
        HTCoord* K     = new HTCoord(sector->A);

        // It may be interesting to sort children by weight instead
        for (unsigned long i = 0; i < children.size(); ++i) {
        //for (Enumeration e = children(); e.hasMoreElements(); ) {
            HTModel* child = children[i];
            HTSector* childSector = new HTSector();
            childSector->A = new HTCoord(K);
            omega += (alpha * (child->getWeight() / globalWeight));
            K->x = cos(omega);
            K->y = sin(omega);
            childSector->B = new HTCoord(K);
            child->layout(childSector, length);
            //delete K;
        }
      }
    }


void vsx_widget_hyperbolic_tree::translateToOrigin(vsx_widget_hyperbolic_tree* node) {
  HTCoord zn = node->getCoordinates();
  zn.x+=0.001;
  zn.y+=0.001;
  HTCoord zf;
  //cout << "zn x y: " << zn.x << " " << zn.y << endl;
  translate(&zn,&zf);
  //endpoint = zn;
//  refreshScreenCoordinates();
}


//vsx_widget_hyperbolic_tree* vsx_widget_hyperbolic_tree::selected;
///vsx_widget_hyperbolic_tree* vsx_widget_hyperbolic_tree::root;  // drawing model
///vsx_widget_hyperbolic_tree* vsx_widget_hyperbolic_tree::draw_root;  // the root of the drawing tree

  void vsx_widget_hyperbolic_tree::drawBranches_() {
    if (ntype)
    {
      for (unsigned long i = 0; i < children.size(); i++) {
        vsx_widget_hyperbolic_tree* child = children[i];
        if (!child->mygeodesic) {
          if (geodesics.find(child) != geodesics.end()) {
            child->mygeodesic = geodesics[child];
          }
        }


          HTGeodesic* gg = child->mygeodesic;
            double x = gg->b.x-0.5;
            double y = gg->b.y-0.5;
            double box_size = 1-sqrt(x*x + y*y)*2;

            box_size = 5*(box_size);

          if (box_size > 1) box_size = 1;
            float bs = box_size*box_size*box_size;
          gg->intensity = draw_root->color.a*0.5f;
          gg->draw();

          if (box_size > 0.2) {
            if (bs > 1) bs = 1;
            bs = 1.0f;
            root->mtex->bind();
            glColor4f(
              children[i]->node->node->color.r,
              children[i]->node->node->color.g,
              children[i]->node->node->color.b,
              bs*draw_root->color.a*children[i]->node->node->color.a
            );
            if (child->ntype) box_size*=0.8; else box_size*=1.2;

            double diffx = box_size*children[i]->node->node->size.x;
            double diffy = box_size*children[i]->node->node->size.y;

            glBegin(GL_QUADS);
              glTexCoord2f(0,1);
              glVertex3f(gg->b.x-diffx*1.2, gg->b.y - diffy*1.5,0);
              glTexCoord2f(1,1);
              glVertex3f(gg->b.x+diffx*1.2, gg->b.y - diffy*1.5,0);
              glTexCoord2f(1,0);
              glVertex3f(gg->b.x+diffx*1.2, gg->b.y+diffy*0.5,0);
              glTexCoord2f(0,0);
              glVertex3f(gg->b.x-diffx*1.2, gg->b.y+diffy*0.5,0);
            glEnd();
            root->mtex->_bind();
            glColor4f(0,0,0,draw_root->color.a);
            font.color.a = draw_root->color.a;
            font.print_center(vsx_vector(gg->b.x,gg->b.y-diffy),children[i]->node->node->getName(),diffy*0.8);

            if (root->selected == children[i]) {
              glColor4f(1,1,1,draw_root->color.a);
              float lb = 0.01; // line border
              glBegin(GL_LINE_STRIP);
                glVertex3f(gg->b.x-diffx-lb, gg->b.y+lb,0);
                glVertex3f(gg->b.x+diffx+lb, gg->b.y+lb,0);
                glVertex3f(gg->b.x+diffx+lb, gg->b.y-diffy-lb,0);
                glVertex3f(gg->b.x-diffx-lb, gg->b.y-diffy-lb,0);
                glVertex3f(gg->b.x-diffx-lb, gg->b.y+lb,0);
              glEnd();
            }
          }
          child->drawBranches_();
        }
      }

  }


  vsx_widget_hyperbolic_tree* vsx_widget_hyperbolic_tree::findNode__(HTCoord zs) {
    if (ntype) {
      for (int i = children.size()-1; i >= 0; --i) {
        vsx_widget_hyperbolic_tree* child = children[i];
        if (geodesics.find(child) != geodesics.end()) {
          HTGeodesic* gg = geodesics[children[i]];

          double box_size = draw_root->ze->d(gg->zb);
          if (box_size > 1) box_size = 1;
          if (box_size > 0.00001) {
            if (child->ntype) box_size*=0.8; else box_size*=1.2;
            double diffx = 1.3*box_size*children[i]->node->node->size.x;
            double diffy = box_size*children[i]->node->node->size.y;

            if (
              (zs.x > gg->b.x-diffx)
              &&
              (zs.x < gg->b.x+diffx)
              &&
              (zs.y > gg->b.y-diffy)
              &&
              (zs.y < gg->b.y+diffy)
              )
              {

                vsx_widget_hyperbolic_tree* search_result = 0;
                if (child->ntype) {
                  search_result = child->findNode__(zs);
                }
                if (search_result) return search_result; else
                return child;
              } else {
                vsx_widget_hyperbolic_tree* search_result = 0;
                if (child->ntype) {
                   search_result = child->findNode__(zs);
                   if (search_result) return search_result;
                }
              }

          }
        }
      }
    }
    return 0;
  }

  vsx_widget_hyperbolic_tree* vsx_widget_hyperbolic_tree::findNode(vsx_string designator) {
    if (this == root) return draw_root->findNode(designator);
    std::vector<vsx_string> add_c;
    vsx_string deli = ";";
    split_string(designator,deli,add_c,-1);
    vsx_string search = add_c[0];
    //printf("addc size %d\n",add_c.size());
    for (std::vector<vsx_widget_hyperbolic_tree*>::iterator it = children.begin(); it != children.end(); ++it) {
      //cout << (*it)->node->node->getName() << endl;
      if ((*it)->node->node->getName() == search) {
        if (add_c.size() > 1) {
          add_c.erase(add_c.begin());
          vsx_string new_add_name = implode(add_c,";");
          return (*it)->findNode(new_add_name);
        } else {
          return *it;
        }
      }
    }
    return 0;
  }

    void vsx_widget_hyperbolic_tree::changeProjType(double z) {
        if (z != projType) {
            HTCoord* zo = new HTCoord(draw_root->getCoordinates());

            double oldProjType = projType;
            projType = z;
            //model.layoutHyperbolicTree();
            draw_root->changeProjType_(z);

            restore();
            HTCoord* zz = new HTCoord();
            zo->pToZ(zz,oldProjType);

            translateCorrected(new HTCoord(), zz->zToP(z));
            endTranslation();
            delete zz;
        }
    }


//----------------------------------------------------------------------------------------------------------------------

int vsx_widget_ultra_chooser::inside_xy_l(vsx_vector &test, vsx_vector &global)
{
  VSX_UNUSED(test);
  VSX_UNUSED(global);

  if (visible >= 0.0f)
  {
    return coord_type;
  }
  else
  {
    return 0;
  }
}

void vsx_widget_ultra_chooser::init() {
}


void vsx_widget_ultra_chooser::center_on_item(vsx_string target) {
  vsx_widget_hyperbolic_tree* t = treedraw->findNode(target);
  if (t) {
    treedraw->translateToOrigin(t);
    treedraw->endTranslation();
    endpoint = clickpoint;
  }
}

void vsx_widget_ultra_chooser::cancel_drop() {
  visible = 1;
  drag_module = false;
  drag_dropped = false;
  allow_move = false;
  ((vsxu_assistant*)((vsx_widget_desktop*)root)->assistant)->temp_show();
}

bool vsx_widget_ultra_chooser::event_key_down(signed long key, bool alt, bool ctrl, bool shift)
{
  VSX_UNUSED(alt);
  VSX_UNUSED(ctrl);
  VSX_UNUSED(shift);

  //printf("%d\n",key);
  /*if (ctrl) {
    if (key == 'r') { size.x += 0.1; size.y += 0.1;}
    if (key == 'w') { size.x -= 0.1; size.y -= 0.1;}
    treedraw->size.x = size.x;
    treedraw->size.y = size.y;

  }*/
  if (key == 27) {
    if (drag_dropped) {
      cancel_drop();
    } else
    hide();
  } else
  // home
  if (key == -36 || key == 'a') {
    HTCoord h;// = treedraw->draw_root->getCoordinates();
    //h.x -= treedraw->draw_root->ze->x;
    //h.y -= treedraw->draw_root->ze->y;
    //printf("gruuu\n");
    //treedraw->setPos(0,0);
    //treedraw->refreshScreenCoordinates();
    //startpoint = h;

    ///treedraw->translateToOrigin(treedraw->draw_root);
    ///treedraw->endTranslation();
    ///endpoint = clickpoint;

    endpoint = h;
    h = treedraw->draw_root->getCoordinates();
    h.x+=0.001;
    h.y+=0.001;
    clickpoint = startpoint = h;




    //endpoint = treedraw->draw_root->getCoordinates();
    //endpoint.x += clickpoint.x;
    //endpoint.y += clickpoint.y;

    //endpoint = treedraw->draw_root->getOldCoordinates();
    //endpoint.y = 0;
    //endpoint = h;//treedraw->draw_root->getCoordinates();
    //anim_x = 1;
    //treedraw->draw_root->restore();

  }
  return true;
}

void vsx_widget_ultra_chooser::command_process_back_queue(vsx_command_s *t) {
  if (t->cmd == "cancel" || t->cmd == "component_create_name_cancel") {
    visible = 1;
    m_focus = this;
    k_focus = this;
    a_focus = this;
    drag_dropped = false;
    ((vsxu_assistant*)((vsx_widget_desktop*)root)->assistant)->temp_show();
    parent->front(this);
  } else
  if (t->cmd == "component_create_name") {
    visible = 1;
    m_focus = this;
    k_focus = this;
    a_focus = this;
    drag_dropped = false;
    ((vsxu_assistant*)((vsx_widget_desktop*)root)->assistant)->temp_show();
    //printf("ident: %s\n",treedraw->selected->node->node->module_info->identifier.c_str());
    // check wich type it is
    if (treedraw->selected->node->node->module_info->component_class == "macro") {
      //printf("macro in browser\n");
      // everything else will be contained in this macro, so modify the macro_name var
      vsx_string local_macro_name = t->cmd_data;
      // first create the macro
      command_q_b.add_raw("macro_create "+treedraw->selected->node->node->module_info->identifier+" "+macro_name+local_macro_name+" "+f2s(drop_pos.x)+" "+f2s(drop_pos.y));
      // in here, send all the commands contained in the macro to the server..
      // this is terrible yes, but it will hopefully work :)
      parent->vsx_command_queue_b(this);

    } else
    {
      // syntax:
      //  component_create math_logic;oscillator_dlux macro1.my_oscillator 0.013 0.204
      command_q_b.add_raw("component_create "+treedraw->selected->node->node->module_info->identifier+" "+macro_name+t->cmd_data+" "+f2s(drop_pos.x)+" "+f2s(drop_pos.y));
      parent->vsx_command_queue_b(this);
      ((vsxu_assistant*)((vsx_widget_desktop*)root)->assistant)->temp_show();
    }
  }
}

bool doubleclix;

void vsx_widget_ultra_chooser::event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);
  VSX_UNUSED(button);

  LOG_A("begin doubleclix\n");
  if (!treedraw) return;
  allow_move = false;
  HTCoord test_coord;
  test_coord.x = distance.corner.x;
  test_coord.y = distance.corner.y;
  vsx_widget_hyperbolic_tree* test = treedraw->draw_root->findNode__(test_coord);
  //if (test) {
    //treedraw->endTranslation();
    if (mode == 0) {
      startpoint.projectionStoE(0.5f*size.x, 0.5f*size.y,treedraw->getSOrigin(),treedraw->getSMax());
      clickpoint = startpoint;
    }
    if (test) {
      LOG_A("test");
      //endpoint = test->getCoordinates();
      //startpoint.projectionStoE(pos.x, pos.y,treedraw->getSOrigin(),treedraw->getSMax());
      //treedraw->translateToOrigin(test);
      //treedraw->endTranslation();
      //endpoint = clickpoint;
      //startpoint.x = 0;
      //startpoint.y = 0;
      //endpoint = test->getCoordinates();
      if (mode == 0) {
        HTCoord zn = test->getCoordinates();
        zn.x+=0.001;
        zn.y+=0.001;



        HTCoord zf;
        //treedraw->translate(&zn,&zf);
        //treedraw->endTranslation();
        endpoint = zf;
        clickpoint = startpoint = zn;
        interpolation_speed = 3;

        //endpoint.x = -endpoint.x;
        //endpoint.y = -endpoint.y;
      }

      // act on impulse!
      if (mode == 1)
      if (test->node->node->isLeaf()) {
        LOG_A("command_q_b.add_raw("+command+" "+test->node->node->module_info->identifier+");");
        command_q_b.add_raw(command+" "+base64_encode(test->node->node->module_info->identifier));
        LOG_A("command_receiver->vsx_command_queue_b(this);")
        command_receiver->vsx_command_queue_b(this);
        LOG_A("((vsx_window_object_inspector*)object_inspector)->unload();")
        ((vsx_window_object_inspector*)object_inspector)->unload();
        LOG_A("hide();")
        hide();
        return;
      }

    }
    else {
      if (mode == 0) {
        startpoint.projectionStoE(distance.corner.x, distance.corner.y,treedraw->getSOrigin(),treedraw->getSMax());
        clickpoint = startpoint;
        endpoint.x = 0;
        endpoint.y = 0;
      }
    }
    //endpoint.projectionStoE(screen.x, screen.y,treedraw->getSOrigin(),treedraw->getSMax());
    //endpoint.projectionStoE(1-screen.x, screen.y,treedraw->getSOrigin(),treedraw->getSMax());
/*    moved = false;
    treedraw->endTranslation();
    startpoint.projectionStoE(0.5, 0.5,treedraw->getSOrigin(),treedraw->getSMax());
    clickpoint = startpoint;
    endpoint.projectionStoE(1-screen.x, 1-screen.y,treedraw->getSOrigin(),treedraw->getSMax());
    cout << startpoint.x << endl;
    cout << endpoint.x << endl;*/
    doubleclix = true;
    LOG_A("end doubleclix\n");
//    startpoint.projectionStoE(pos.x , pos.y, treedraw->getSOrigin(), treedraw->getSMax());
//    clickpoint = startpoint;
//    endpoint.projectionStoE(screen.x , screen.y, treedraw->getSOrigin(), treedraw->getSMax());
    //anim_x = 0;
    //animating = true;
  //}
}

void vsx_widget_ultra_chooser::event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords)
{
  VSX_UNUSED(coords);

  HTCoord test_coord;
  test_coord.x = distance.corner.x;
  test_coord.y = distance.corner.y;
  vsx_widget_hyperbolic_tree* test = treedraw->draw_root->findNode__(test_coord);
  draw_tooltip = 0;
  tooltip_text = "";
  if (test) {
    if (test->node->node->isLeaf()) {
      if (test->node->node->module_info->description != "") {
        tooltip_text = test->node->node->module_info->description;
        if (tooltip_text.size() > 1) {
          tooltip_text = test->node->node->module_info->identifier + "\n---------------\n" + tooltip_text;
          draw_tooltip = 1;
          tooltip_pos = distance.corner;
          tooltip_pos.y-=0.01;
          tooltip_pos.z = pos.z;
        }
      }
    }
  }
}

void vsx_widget_ultra_chooser::event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(coords);

  //printf("chooser mouse down\n");
  if (drag_module) { cancel_drop(); return; }
  HTCoord test_coord;
  test_coord.x = distance.corner.x;
  test_coord.y = distance.corner.y;
  vsx_widget_hyperbolic_tree* test = treedraw->draw_root->findNode__(test_coord);

  k_focus = this;
  if (!drag_dropped)
  if (button == 2) {
    hide();
    return;
  }
  if (drag_dropped && button == 2) {
//    visible = 1;
    cancel_drop();
  }
  drag_dropped = false;
  if (!treedraw) return;
  if (animating) {
    treedraw->endTranslation();
    animating = false;
  }
  a_focus = this;
  m_focus = this;
  moved = false;

  if (((dialog_query_string*)name_dialog)->visible) {
    ((dialog_query_string*)name_dialog)->visible = 0;
    visible = 1;
  }
  if (!test) treedraw->selected = 0;
//  if (button == GLUT_LEFT_BUTTON)
  if (!moved && test) {
    if (test->node->node->isLeaf())
    {
      //printf("drag module!\n");
      drag_pos = distance.corner;
      drag_module = true;
      treedraw->selected = test;
      //printf("clicknode: %s\n",test->node->node->module_info->identifier.c_str());
      if (server) {
        //printf("setting new helptext\n");
        help_text = ((vsx_widget_server*)server)->build_comp_helptext(test->node->node->module_info->identifier);
        ++help_timestamp;
      }
      std::vector<vsx_string> parts;
      vsx_string deli = ";";
      explode(test->node->node->module_info->identifier,deli, parts);
      if (test->node->node->module_info->component_class == "state") {
        drag_module = false;
      } else
      if (test->node->node->module_info->component_class == "resource") {
        drag_module = false;
        ((vsx_window_object_inspector*)object_inspector)->load_file("resources/"+str_replace("\\ "," ",str_replace(";","/",test->node->node->module_info->identifier)));
      } else {
        //((vsx_window_object_inspector*)object_inspector)->unload();
      }
    }
    // create a component here
  }
  if (!doubleclix)
  if (!drag_module) {
    interpolation_speed = 6;
    treedraw->endTranslation();
    startpoint.projectionStoE(distance.corner.x, distance.corner.y,treedraw->getSOrigin(),treedraw->getSMax());
    endpoint = clickpoint = startpoint;
    //endpoint.projectionStoE(screen.x, screen.y,treedraw->getSOrigin(),treedraw->getSMax());
  }
}

void vsx_widget_ultra_chooser::event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords)
{
  //printf("mouse move begin\n");
  if (hide_) return;
  if (!treedraw) return;
  if (!allow_move) return;
  if (drag_module) {

    draw_tooltip = 0;
    drag_pos.z = 0;
    //printf("distance.corner.x: %f\n",distance.corner.x);
    //printf("distance.corner.x: %f\n",drag_pos.x);
    //printf("distance: %f\n",drag_pos.distance(distance.corner));
        vsx_widget_distance l_distance;
        vsx_widget* tt = root->find_component(coords,l_distance);
        if (tt)
        //printf("name over: %s\n", tt->name.c_str());
    if (drag_pos.distance(distance.corner) > 0.0015 || drag_dropped) {
//          printf("drag gronk\n");
      drag_dropped = true;
      visible = 0.15;
      drag_pos = distance.corner;

      ((vsxu_assistant*)((vsx_widget_desktop*)root)->assistant)->temp_hide();
    }
    return;
  }
  moved = true;

  v_anim_endpoint.x = distance.corner.x*2;
  v_anim_endpoint.y = distance.corner.y*2;
  endpoint.projectionStoE(distance.corner.x, distance.corner.y,treedraw->getSOrigin(),treedraw->getSMax());
  //printf("mouse move end\n");
}


void vsx_widget_ultra_chooser::event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button)
{
  VSX_UNUSED(distance);
  VSX_UNUSED(button);

  if (!treedraw) return;
  //if (button == GLUT_LEFT_BUTTON)
  {
    allow_move = true;
    drag_module = false;
    if (drag_dropped) {
      drag_module = false;
      visible = 0;
      if (mode == 0) {
        vsx_widget_distance l_distance;
        vsx_widget* tt = root->find_component(coords,l_distance);
        if (tt) {
            //printf("tt->name = %s\n",tt->name.c_str());
          bool macro = false;
          if (tt->widget_type == VSX_WIDGET_TYPE_COMPONENT) if (((vsx_widget_component*)tt)->component_type == "macro") macro = true;
          if (macro) macro_name = tt->name+"."; else macro_name = "";
          if (tt->widget_type == VSX_WIDGET_TYPE_SERVER || macro) {
            //vsx_vector a = tt->get_pos_p();
            //drop_pos = distance.center - a;
            drop_pos = l_distance.center;
            //printf("drop_pos: %f %f\n",drop_pos.x, drop_pos.y);
            //printf("x,y = %f , %f\n", drop_pos.x, drop_pos.y);
            if (treedraw->selected) {
              if (server) {
                drag_dropped = false;
                if (ctrl)
                ((dialog_query_string*)name_dialog)->show(((vsx_widget_server*)server)->get_unique_name(treedraw->selected->getName()));
                else
                {
                  command_q_b.add_raw("component_create_name "+((vsx_widget_server*)server)->get_unique_name(treedraw->selected->getName()));
                  vsx_command_queue_b(this);
                  visible = 1;
                  drag_dropped = false;
                  return;
                }
              }
              visible = 0.3;
            } else {
              visible = 1;
              drag_dropped = false;
            }
          }
        }
      }
    }
  }
}

void vsx_widget_ultra_chooser::draw_2d()
{
  float dtime = vsx_widget_time::get_instance()->get_dtime();
  vsx_vector a(0.5f*size.x,0.5f*size.y);
  if (animating) {
    anim_x += dtime*2;
    double lx = anim_endpoint.x - anim_startpoint.x;
    double ly = anim_endpoint.y - anim_startpoint.y;
    if (anim_x < 0.5) {
      anim_curpoint.x = lx*2*anim_x*anim_x;
      anim_curpoint.y = ly*2*anim_x*anim_x;
      treedraw->translate(&anim_curpoint, &anim_startpoint);
    } else {
      if (anim_x < 1) {
        double a = 0.5 - fabs(anim_x-0.5);
        anim_curpoint.x = lx-lx*2*(a*a);
        anim_curpoint.y = ly-ly*2*(a*a);
        treedraw->translate(&anim_curpoint, &anim_startpoint);
        treedraw->endTranslation();
      } else {
        treedraw->endTranslation();
        animating = false;
        anim_x = 0;
      }
    }
  }
  // SHOW ME YOUR HIDE!
  if (show_)
  if (visible != 1) {
    visible += dtime*6;
    if (visible > 1.0f) {
      visible = 1;
      show_ = false;
      a_focus = this;
      k_focus = this;
    }
  }
  if (hide_)
  if (visible != 0) {
    visible -= dtime*6;
    if (visible < 0.0f) {
      visible = 0;
      hide_ = false;
      a_focus = parent;
      k_focus = parent;
    }
  }
  if (visible != 0)
  {
    // interpolation magic
    float tt = dtime*interpolation_speed * vsx_widget_global_interpolation::get_instance()->get();
    if (tt > 1) tt = 1;

  /*float ft = tt * 3.1415927;
  float ff = (1 - cos(ft)) * .5;

  clickpoint.x = clickpoint.x*(1-ff) + endpoint.x*ff;
  clickpoint.y = clickpoint.y*(1-ff) + endpoint.y*ff;
    */
    if (clickpoint.x != endpoint.x && clickpoint.y != endpoint.y)
    {
      doubleclix = false;
      clickpoint.x = clickpoint.x*(1-tt)+endpoint.x*tt;
      clickpoint.y = clickpoint.y*(1-tt)+endpoint.y*tt;

      if (endpoint.isValid()) {
        treedraw->translate(&startpoint,&clickpoint);
        treedraw->endTranslation();
        startpoint = clickpoint;
      }
    }

    //printf("drawing\n");
    if (!performance_mode)
    {
    float nu = 20;
    glBegin(GL_TRIANGLE_FAN);
      glColor4f(0 , 0.5 , 0.6 , visible);
      glVertex2f(a.x,a.y);

      //glColor4f(0,0.5,0.6,0.9*visible);
      glColor4f(0,0,0,0);

      float n = 2*PI/nu;
      for (float i = 0; i <= nu; i++) {
        glVertex3f(size.x*cos(n*i)/2+0.5f,size.y*sin(n*i)/2+0.5f,0);
      }
    glEnd();
    }

/*    glColor4f(1,1,1,0.4*visible);
    glBegin(GL_LINE_STRIP);
      for (float i = 0; i <= nu; i++) {
        glVertex3f(size.x*cos(n*i)/2+pos.x,size.y*sin(n*i)/2+pos.y,0);
      }
    glEnd();*/
    if (treedraw) {
      treedraw->draw_root->color.a = visible;
      glLineWidth(1.0f);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE);
      treedraw->refreshScreenCoordinates();
      treedraw->drawBranches();
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      if (drag_dropped) {
        if (visible != 1) {
          glColor4f(1,1,1,0.5);
          float dragbox = 0.01;
          glBegin(GL_QUADS);
            glVertex3f(drag_pos.x - dragbox ,drag_pos.y - dragbox, 0);
            glVertex3f(drag_pos.x + dragbox ,drag_pos.y - dragbox, 0);
            glVertex3f(drag_pos.x + dragbox ,drag_pos.y + dragbox, 0);
            glVertex3f(drag_pos.x - dragbox ,drag_pos.y + dragbox, 0);
          glEnd();
        }
      }

      if (first) {
        first = false;
        startpoint.projectionStoE(0.0, 0.0, treedraw->getSOrigin(), treedraw->getSMax());
        endpoint.projectionStoE(0.01, 0.01,treedraw->getSOrigin(),treedraw->getSMax());
        treedraw->translate(&startpoint,&endpoint);
        treedraw->endTranslation();
      }
    } /*else {
      //glColor4f(1,1,1,visible);
      //myf.print_center(pos, "DIVE NOT ACTIVE: NO DATA","ascii",0.02);
    }*/
    glColor4f(1,1,1,visible);
    font.color.a = visible*0.3;
    if (message != "")
    font.print_center(a, message,0.035);
    if (drag_dropped) {
      font.color.a = (sin(PI + vsx_widget_time::get_instance()->get_time() *13)+1)*0.4f+0.2f;
      font.print_center(vsx_vector(a.x,a.y+0.16), "* Dropping "+treedraw->selected->node->node->module_info->identifier+" *",0.025);
      font.print_center(vsx_vector(a.x,a.y+0.13), "Hold the mouse button, move/drag the module where you want to place it.",0.025);
      font.print_center(vsx_vector(a.x,a.y+0.1), "Then release the left mouse button to drop it.",0.025);
      font.print_center(vsx_vector(a.x,a.y-0.15), "Press right mouse button or ESC to cancel drop.",0.035);



//\n\n
    }
    if (draw_tooltip) {
      font.color.a = 0.0f;
      vsx_vector sz = font.get_size(tooltip_text, 0.025);
      //sz = sz-tooltip_pos;
      glColor4f(0.0f,0.0f,0.0f,0.6f*visible);
      draw_box(vsx_vector(tooltip_pos.x,tooltip_pos.y+0.025*1.05), sz.x, -sz.y);
      font.color.a = 1.0f*visible;
      font.print(tooltip_pos, tooltip_text,0.022);
    }
  }
  draw_children_2d();
}

void vsx_widget_ultra_chooser::show() {
  if (!object_inspector) {
    object_inspector = find("object_inspector");
  }
  a_focus = this;
  k_focus = this;
  show_ = true;
  hide_ = false;
}

void vsx_widget_ultra_chooser::hide() {
  //((vsx_window_object_inspector*)object_inspector)->unload();
  hide_ = true;
  show_ = false;
}

vsx_widget_ultra_chooser::vsx_widget_ultra_chooser()
{
  server = 0;
  object_inspector = 0;
  widget_type = VSX_WIDGET_TYPE_ULTRA_CHOOSER;
  animating = false;
  show_ = false;
  hide_ = false;
  first = true;
  drag_dropped = false;
  draw_tooltip = false;
  drag_module = false;
  module_tree = new HTNode;
  interpolation_speed = 7;
  coord_type = VSX_WIDGET_COORD_CORNER;
  coord_related_parent = false;
  mode = 0; // normal component chooser
  mymodel = 0;
  treedraw = 0;
  treedraw = 0;
  size.x = 1.0f;
  size.y = 1.0f;
  visible = 0;
  topmost = true;
  font.mode_2d = true;
  name_dialog = add(new dialog_query_string("name of component","Choose a unique name for your component"),"component_create_name");
  init_children();
  set_render_type(VSX_WIDGET_RENDER_2D);

//  printf("%s\n\n",(root->skin_path+"module_chooser.conf").c_str());
  module_chooser_colors.load_from_file(root->skin_path+"module_chooser.conf",true,4);

  if (!module_chooser_colors.count()) {
    module_chooser_colors.add_raw("ccolor default 0.2,0.2,0.6,1");
    module_chooser_colors.add_raw("ccolor default_ 0.2,0.2,0.6,1");
    module_chooser_colors.add_raw("ccolor resource 0.8,0.4,0.4,0.6");
    module_chooser_colors.add_raw("ccolor resource_ 0.5,0.5,0.5,1");
    module_chooser_colors.add_raw("ccolor macro 0.3,0.8,0.8,0.3");
    module_chooser_colors.add_raw("ccolor macro_ 0.5,0.9,0.9,1");
    module_chooser_colors.add_raw("ccolor render 1,0.4,0.3,0.5");
    module_chooser_colors.add_raw("ccolor render_ 1,0.4,0.3,1");
    module_chooser_colors.add_raw("ccolor texture 0,0.4,0.8,0.8");
    module_chooser_colors.add_raw("ccolor texture_ 0.5,0.5,1,1");
    module_chooser_colors.add_raw("ccolor texture_surface 0.7,0,1,0.8");
    module_chooser_colors.add_raw("ccolor texture_surface_ 0.9,0.2,1,1");
    module_chooser_colors.add_raw("ccolor bitmap 1,0,0.5,0.5");
    module_chooser_colors.add_raw("ccolor bitmap_ 1,0,0.5,1");
    module_chooser_colors.add_raw("ccolor parameters 0.8,0.5,0.6,0.3");
    module_chooser_colors.add_raw("ccolor parameters_ 0.8,0.5,0.6,1");
    module_chooser_colors.add_raw("ccolor mesh 0.6,0.7,0.7,1");
    module_chooser_colors.add_raw("ccolor mesh_ 0.6,0.7,0.7,0.5");
    module_chooser_colors.add_raw("rcolor jpg 0,1,0,1");
    module_chooser_colors.add_raw("rcolor png 1,0,1,1");
  }
  module_chooser_colors.reset();

  vsx_command_s* mc = 0;
  while ( (mc = module_chooser_colors.get()) ) {
    if (mc->cmd == "ccolor") {
      std::vector<vsx_string> parts;
      vsx_string deli = ",";
      explode(mc->parts[2],deli, parts);
      vsx_color p;
      p.r = s2f(parts[0]);
      p.g = s2f(parts[1]);
      p.b = s2f(parts[2]);
      p.a = s2f(parts[3]);
      mc_colors[mc->parts[1]] = p;
    } else
    if (mc->cmd == "rcolor") {
      std::vector<vsx_string> parts;
      vsx_string deli = ",";
      explode(mc->parts[2],deli, parts);
      vsx_color p;
      p.r = s2f(parts[0]);
      p.g = s2f(parts[1]);
      p.b = s2f(parts[2]);
      p.a = s2f(parts[3]);
      mc_r_colors[mc->parts[1]] = p;
    }
  }
  module_chooser_colors.clear();
  help_text = "This is the browser:\n\
- right-click to close\n\
- click+drag or double-click to create\n\
  or choose an item\n\
- some items are previewable:\n\
  left-click them once to see a preview\n\
- press [home] to return to the middle\n\
- left-double-click to travel to\n\
  a new destination";
}

vsx_widget_ultra_chooser::~vsx_widget_ultra_chooser() {
}

void vsx_widget_ultra_chooser::on_delete() {
  //printf("deleting chooser\n");
  mc_colors.clear();
  mc_r_colors.clear();
  delete mymodel;
  delete treedraw;
  delete module_tree;
}


void vsx_widget_ultra_chooser::build_tree() {
  if (mymodel && treedraw) {
    delete mymodel;
    treedraw->_delete();
  }
  allow_move = true;
  mymodel = new HTModel(module_tree);
  treedraw = new vsx_widget_hyperbolic_tree(0,0,mymodel);
  treedraw->size.x = size.x;
  treedraw->size.y = size.y;
  treedraw->pos = vsx_vector(0.5f*size.x, 0.5f*size.y);
  treedraw->changeProjType(0);
  treedraw->mtex = new vsx_texture;

  vsxf filesystem;
  treedraw->mtex->load_png(skin_path+"label.png", true, &filesystem);
  treedraw->mtex->bind_load_gl();
}
void vsx_widget_ultra_chooser::reinit() {
  vsxf filesystem;
  treedraw->mtex->load_png(skin_path+"label.png", true, &filesystem);
}

