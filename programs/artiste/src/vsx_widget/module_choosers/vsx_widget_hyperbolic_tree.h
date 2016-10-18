class vsx_widget_hyperbolic_tree : public vsx_widget {
private:
    HTModel*    model;  // the tree model

    HTCoord   sOrigin;  // origin of the screen plane
    HTCoord   sMax;  // max point in the screen plane

    int       border;

    int ntype; // type - 0 = leaf, 1 = composite

    double ray[4];

    bool    fastMode; // fast mode
    bool    longNameMode; // long name mode
    bool    transNotCorrected;
    bool    kleinMode; // klein mode
    double  projType; // projection type
//------------------------------------------------------------------------------


    vsx_widget_hyperbolic_tree* father;  // father of this node
    vsx_widget_hyperbolic_tree* brother;  // brother of this node

    std::vector<vsx_widget_hyperbolic_tree*> i_children;
    std::map<vsx_widget_hyperbolic_tree*,HTGeodesic*> geodesics;

public:

  HTCoord*            ze;  // current euclidian coords
  HTCoord*            oldZe;  // old euclidian coords
  HTCoord*            zs;  // current screen coords
  HTGeodesic* mygeodesic;
  std::unique_ptr<vsx_texture<>> mtex = 0x0;

  vsx_widget_hyperbolic_tree* selected;  // selected node
  HTModel*         node;  // encapsulated HTModelNode
  vsx_color<> color;
  vsx_vector3<> pos, size;
  // previously static
  vsx_widget_hyperbolic_tree* root;  // drawing model
  vsx_widget_hyperbolic_tree* draw_root;  // the root of the drawing tree

  int NBR_FRAMES; // number of intermediates

  void on_delete()
  {
    if (ze)
      delete ze;
    if (zs)
      delete zs;
    if (oldZe)
      delete oldZe;

    for (std::map<vsx_widget_hyperbolic_tree*,HTGeodesic*>::iterator it = geodesics.begin(); it != geodesics.end(); it++ )
    {
      delete (*it).second;
    }

    mtex.reset(nullptr);
  }

  // create the base maintainer class
  vsx_widget_hyperbolic_tree(
      vsx_widget_hyperbolic_tree* s_root,
      vsx_widget_hyperbolic_tree* s_drawroot,
      HTModel* model
  )
  {
    VSX_UNUSED(s_drawroot);
    if (s_root == 0)
      root = this;
    selected = 0;
    root = this;
    mygeodesic = 0;
    ntype = 0;

    NBR_FRAMES = 5;
    fastMode = false;
    longNameMode = false;
    transNotCorrected = false;
    kleinMode = false;
    border = 0;
    projType = -1.0;
    ze = 0;
    zs = 0;
    oldZe = 0;
    this->model = model;
    HTModel* model_root = model->root;

    if (model_root->isLeaf()) {
      // single
      draw_root = (vsx_widget_hyperbolic_tree*)add(new vsx_widget_hyperbolic_tree(root, 0, 0, model_root, 0), "ht");
    } else {
      // composite
      draw_root = (vsx_widget_hyperbolic_tree*)add(new vsx_widget_hyperbolic_tree(root, 0, 0, model_root, 1), "ht");
    }

    mtex = vsx_texture_loader::load(
        vsx_widget_skin::get_instance()->skin_path_get() + "label.png",
        vsx::filesystem::filesystem::get_instance(),
        true, // threaded
        vsx_bitmap::no_hint,
        vsx_texture_gl::linear_interpolate_hint
      );
  }

  // constructor for child elements
  vsx_widget_hyperbolic_tree(vsx_widget_hyperbolic_tree* s_root, vsx_widget_hyperbolic_tree* s_drawroot, vsx_widget_hyperbolic_tree* father, HTModel* node, int type)
  {
    selected = 0;
    mygeodesic = 0;
    root = s_root;
    if (s_drawroot == 0)
      draw_root = this;
    else
      draw_root = s_drawroot;

    projType = 0;
    this->father = father;
    this->node = node;
    kleinMode = false;

    ze = new HTCoord(node->getCoordinates());
    oldZe = new HTCoord(ze);
    zs = new HTCoord();

    ntype = type;

    if (type)
    {
      HTModel* childNode = 0;
      vsx_widget_hyperbolic_tree* child = 0;
      vsx_widget_hyperbolic_tree* brother = 0;
      bool first = true;
      bool second = false;
      for (unsigned long i = 0; i < node->children.size(); i++)
      {
        childNode = node->children[i];
        if (childNode->isLeaf()) {
          child = new vsx_widget_hyperbolic_tree(root,draw_root,this, childNode, 0);
        } else {
          child = new vsx_widget_hyperbolic_tree(root,draw_root,this, childNode, 1);
        }
        addChild(child);
        if (first) {
          brother = child;
          first = false;
          second = true;
        } else if (second) {
          child->setBrother(brother);
          brother->setBrother(child);
          brother = child;
          second = false;
        } else {
          child->setBrother(brother);
          brother = child;
        }
      }
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  void addChild(vsx_widget_hyperbolic_tree* child) {
      children.push_back(child);
      i_children.push_back(child);
      geodesics[child] = new HTGeodesic(getCoordinates(), child->getCoordinates());
      geodesics[child]->zpos = pos.z;
  }

  int getBorder() {
    return border;
  }

  void refreshScreenCoordinates() {
      sMax.x = (size.x)/2;
      sMax.y = (size.y)/2;
      sOrigin.x = pos.x;
      sOrigin.y = pos.y;
      draw_root->refreshScreenCoordinates(sOrigin, sMax);
  }

  HTCoord getSOrigin() {
      return sOrigin;
  }
  HTCoord getSMax() {
      return sMax;
  }

  void drawBranches() {
      draw_root->drawBranches_();
  }

  void drawNodes() {
      draw_root->drawNodes_();
  }

    void translate(HTCoord* zs, HTCoord* ze) {

        HTCoord zst;
        HTCoord zet;

        if (kleinMode) {
             zs->kToP(&zst);
             ze->kToP(&zet);
        } else if (projType != 0.0) {
            zst = zs->zToP(projType);
            zet = ze->zToP(projType);
        } else {
          zst.copy(zs);
          zet.copy(ze);
        }

        if (transNotCorrected) {
            HTCoord* v = new HTCoord();
            double de = zet.d2();
            double ds = zst.d2();
            double dd = 1.0 - de * ds;
            v->x = (zet.x * ( 1.0 - ds) - zst.x * (1.0 - de)) / dd;
            v->y = (zet.y * ( 1.0 - ds) - zst.y * (1.0 - de)) / dd;
            if (v->isValid()) {
              draw_root->translate(v);
            }
        } else {
            HTCoord* zo = new HTCoord(draw_root->getOldCoordinates());
            zo->x = - zo->x;
            zo->y = - zo->y;
            HTCoord* zs2 = new HTCoord(zst);

            zs2->translate(zo);

            HTCoord t;
            double de = zet.d2();
            double ds = zs2->d2();
            double dd = 1.0 - de * ds;
            t.x = (zet.x * ( 1.0 - ds) - zs2->x * (1.0 - de)) / dd;
            t.y = (zet.y * ( 1.0 - ds) - zs2->y * (1.0 - de)) / dd;

            if (t.isValid()) {

                HTCoord alpha;
                alpha.x = 1 + (zo->x * t.x) + (zo->y * t.y);
                alpha.y = (zo->x * t.y) - (zo->y * t.x);
                HTCoord beta;
                beta.x = zo->x + t.x;
                beta.y = zo->y + t.y;

                draw_root->specialTrans(alpha, beta);
             }
             delete zo;
             delete zs2;
        }
    }

    void translateCorrected(HTCoord* zs, HTCoord* ze) {

        HTCoord* zo = new HTCoord(draw_root->getOldCoordinates());
        zo->x = - zo->x;
        zo->y = - zo->y;
        HTCoord* zs2 = new HTCoord(zs);

        zs2->translate(zo);

        HTCoord* t = new HTCoord();
        double de = ze->d2();
        double ds = zs2->d2();
        double dd = 1.0 - de * ds;
        t->x = (ze->x * ( 1.0 - ds) - zs2->x * (1.0 - de)) / dd;
        t->y = (ze->y * ( 1.0 - ds) - zs2->y * (1.0 - de)) / dd;

        if (t->isValid()) {

            HTCoord alpha;
            alpha.x = 1 + (zo->x * t->x) + (zo->y * t->y);
            alpha.y = (zo->x * t->y) - (zo->y * t->x);

            HTCoord beta;
            beta.x = zo->x + t->x;
            beta.y = zo->y + t->y;

            draw_root->specialTrans(alpha, beta);
        }
        delete zs2;
        delete t;
        delete zo;
    }

    /**
     * Signal that the translation ended.
     */
    void endTranslation() {
        draw_root->endTranslation_();
    }

    /**
     * Translate the hyperbolic tree so that the given node
     * is put at the origin of the hyperbolic tree.
     *
     * @param node    the given HTDrawNode
     */

    /**
     * Restores the hyperbolic tree to its origin.
     */
    void restore() {
        draw_root->restore_();
    }

    /**
     * Sets the fast mode, where nodes are no more drawed.
     *
     * @param mode    setting on or off.
     */
    void set_fastMode(bool mode) {
        if (mode != fastMode) {
            fastMode = mode;
            draw_root->set_fastMode_(mode);
        }
    }

    void set_fastMode_(bool mode) {
        if (mode != fastMode) {
            fastMode = mode;
        }
    }


    bool getFastMode() {
        return fastMode;
    }

    /**
     * Sets the long name mode, where full names are drawn.
     *
     * @param mode    setting on or off.
     */
    void set_longNameMode(bool mode) {
        if (mode != longNameMode) {
            longNameMode = mode;
            draw_root->set_longNameMode_(mode);
        }
    }

    bool getLongNameMode() {
        return longNameMode;
    }

    void transNotCorrectedMode(bool mode) {
        transNotCorrected = mode;
    }

    bool getTransNotCorrectedMode() {
        return transNotCorrected;
    }

    void setQuadMode(bool mode) {
        draw_root->setQuadMode(mode);
    }

    /**
     * Sets the klein mode.
     *
     * @param mode    setting on or off.
     */
    void set_kleinMode(bool mode)
    {
      VSX_UNUSED(mode);
    }

    /**
     * Change projection type.
     *
     * @param z    the new value of proj type
     */
    void changeProjType(double z)
    {
        if (z != projType) {
            HTCoord* zo = new HTCoord(draw_root->getCoordinates());

            double oldProjType = projType;
            projType = z;
            draw_root->changeProjType_(z);

            restore();
            HTCoord* zz = new HTCoord();
            zo->pToZ(zz,oldProjType);
            delete zo;

            HTCoord* ze = zz->zToP(z);
            HTCoord* tcz = new HTCoord();
            translateCorrected(tcz, ze);
            delete tcz;
            endTranslation();
            delete ze;
            delete zz;
        }
    }

    double getProjType() {
        return projType;
    }
// FIN NEU

  /* --- Node searching --- */

    /**
     * Returns the node (if any) whose screen coordinates' zone
     * contains thoses given in parameters.
     *
     * @param zs    the given screen coordinate
     * @return      the searched HTDrawNode if found;
     *              <CODE>null</CODE> otherwise
     */
    vsx_widget_hyperbolic_tree* findNode(HTCoord zs) {
        return draw_root->findNode(zs);
    }

//----------------------------------------------------------------------------------------------------------------------

    void setPos(float x, float y) {
      draw_root->ze->x = x;
      draw_root->ze->y = y;
    }
    void setBrother(vsx_widget_hyperbolic_tree* brother) {
        this->brother = brother;
    }
    HTModel* getHTModelNode() {
        return node;
    }
    vsx_string<>getName() {
        return node->getName();
    }
    HTCoord* getCoordinates() {
        return ze;
    }
    HTCoord* getOldCoordinates() {
        return oldZe;
    }
    HTCoord* getScreenCoordinates() {
        return zs;
    }
    void refreshScreenCoordinates(HTCoord sOrigin, HTCoord sMax) {
        if (kleinMode) {
          HTCoord s; ze->pToK(&s);
            zs->projectionEtoS(s, sOrigin, sMax);
        } else if (projType != 0.0) {
          HTCoord a;
          ze->pToZ(&a,projType);
            zs->projectionEtoS(a, sOrigin, sMax);
        } else {
            zs->projectionEtoS(ze, sOrigin, sMax);
        }

        if (ntype) {
          for (unsigned long i = 0; i < children.size(); i++) {
            i_children[i]->refreshScreenCoordinates(sOrigin, sMax);

            if (geodesics.find(i_children[i]) != geodesics.end())
            geodesics[i_children[i]]->refreshScreenCoordinates(sOrigin, sMax);

          }
        }
    }


    void drawNodes_() {
        if (fastMode == false) {
          // add text drawing HERE
            for (unsigned long i = 0; i < i_children.size(); i++) {
                i_children[i]->drawNodes_();
            }
        }
    }


  int getSpace() {
        int dF = -1;
        int dB = -1;

        if (father != 0) {
            HTCoord zF = father->getScreenCoordinates();
            dF = (int)round(zs->getDistance(zF));
        }
        if (brother != 0) {
            HTCoord* zB = brother->getScreenCoordinates();
          dB = (int)round(zs->getDistance(zB));
        }

        if ((dF == -1) && (dB == -1)) {
            return -1;
        } else if (dF == -1) {
            return dB;
        } else if (dB == -1) {
            return dF;
        } else {
          if (dF < dB) return dF; else return dB;
        }
    }

    void specialTrans(HTCoord alpha, HTCoord beta) {
        ze->copy(oldZe);
        ze->specialTrans(alpha, beta);

        if (!ntype) return;

        for (unsigned long i = 0; i < children.size(); i++) {
            i_children[i]->specialTrans(alpha, beta);

            if (geodesics.find(i_children[i]) != geodesics.end())
            geodesics[i_children[i]]->rebuild();
        }
    }

    void translate(HTCoord dest) {
        ze->copy(oldZe);
        ze->translate(dest);


        if (!ntype) return;
        for (unsigned long i = 0; i < i_children.size(); i++) {
            i_children[i]->translate(dest);
            if (geodesics.find(i_children[i]) != geodesics.end())
            geodesics[i_children[i]]->rebuild();
        }


    }
    void endTranslation_() {
      oldZe->copy(ze);
      if (!ntype) return;

      for (unsigned long i = 0; i < i_children.size(); i++) {
        i_children[i]->endTranslation_();
      }
    }

    void restore_() {
        HTCoord* orig = node->getCoordinates();
        ze->x = orig->x;
        ze->y = orig->y;
        oldZe->copy(ze);
      if (!ntype) return;

      for (unsigned long i = 0; i < i_children.size(); i++) {
            i_children[i]->restore_();
            if (geodesics.find(i_children[i]) != geodesics.end())
            geodesics[i_children[i]]->rebuild();
      }
    }


    void set_longNameMode_(bool mode) {
        if (mode != longNameMode) {
            longNameMode = mode;
        }
    }

    void set_kleinMode_(bool mode) {
        if (mode != kleinMode) {
            kleinMode = mode;
        }
    }

    void changeProjType_(double z) {
        if (z != projType) {
            projType = z;
        }
    }

  vsx_widget_hyperbolic_tree* findNode(HTCoord* zs)
  {
    VSX_UNUSED(zs);
    return 0;
  }




  /* --- Drawing --- */



    void translateToOrigin(vsx_widget_hyperbolic_tree* node) {
      HTCoord zn = node->getCoordinates();
      zn.x+=0.001;
      zn.y+=0.001;
      HTCoord zf;
      translate(&zn,&zf);
    }

      void drawBranches_() {
        if (ntype)
        {
          for (unsigned long i = 0; i < children.size(); i++) {
            vsx_widget_hyperbolic_tree* child = i_children[i];
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
                  i_children[i]->node->node->color.r,
                  i_children[i]->node->node->color.g,
                  i_children[i]->node->node->color.b,
                  bs*draw_root->color.a * i_children[i]->node->node->color.a
                );
                if (child->ntype) box_size*=0.8; else box_size*=1.2;

                double diffx = box_size * i_children[i]->node->node->size.x;
                double diffy = box_size * i_children[i]->node->node->size.y;

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
                font.print_center(vsx_vector3<>(gg->b.x,gg->b.y-diffy),i_children[i]->node->node->getName(),diffy*0.8);

                if (root->selected == i_children[i]) {
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


      vsx_widget_hyperbolic_tree* findNode__(HTCoord zs) {
        if (ntype) {
          for (int i = children.size()-1; i >= 0; --i) {
            vsx_widget_hyperbolic_tree* child = i_children[i];
            if (geodesics.find(child) != geodesics.end()) {
              HTGeodesic* gg = geodesics[i_children[i]];

              double box_size = draw_root->ze->d(gg->zb);
              if (box_size > 1) box_size = 1;
              if (box_size > 0.00001) {
                if (child->ntype) box_size*=0.8; else box_size*=1.2;
                double diffx = 1.3*box_size*i_children[i]->node->node->size.x;
                double diffy = box_size*i_children[i]->node->node->size.y;

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

      vsx_widget_hyperbolic_tree* findNode(vsx_string<>designator) {
        if (this == root) return draw_root->findNode(designator);
        vsx_nw_vector <vsx_string<> > add_c;
        vsx_string<>deli = ";";
        vsx_string_helper::explode(designator,deli,add_c,-1);
        vsx_string<>search = add_c[0];
        for (std::vector<vsx_widget_hyperbolic_tree*>::iterator it = i_children.begin(); it != i_children.end(); ++it) {
          if ((*it)->node->node->getName() == search) {
            if (add_c.size() > 1) {
              add_c.remove_index(0);
              vsx_string<> semicolon(";");
              vsx_string<>new_add_name = vsx_string_helper::implode(add_c, semicolon);
              return (*it)->findNode(new_add_name);
            } else {
              return *it;
            }
          }
        }
        return 0;
      }


};
