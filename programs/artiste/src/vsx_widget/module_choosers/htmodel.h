class HTModel {
private:
    HTModel* parent; // our parent, if we're root this is NULL

    double      length;  // distance between node and children
    int         nodes;    // number of nodes


    int ntype; // if 0, single, if 1, composite
    HTCoord*             z     ; // Euclidian coordinates
    double globalWeight;  // sum of children weight
    bool is_this_root;
public:
    HTNode*               node; // encapsulated HTNode
    double               weight;  // part of space taken
    std::vector<HTModel*> children;
    HTModel* root; // the root of the tree's model
    HTModel* mroot;
    double lp;
    double lk;

  /* --- Constructor --- */

    /**
     * Constructor.
     *
     * @param root    the root of the real tree
     */
    HTModel(HTNode* noderoot) {
      is_this_root = true;
      lp = 0.1;
      lk = 0.18;
      parent = 0;
      length = lp;
      nodes = 0;
      z = new HTCoord();
      weight = 1;
      mroot = this;
      this->root = 0;

      if (noderoot->isLeaf()) {
        // single
          root = new HTModel(mroot,0,noderoot, 0,0);
      } else {
        // composite
          root = new HTModel(mroot,0,noderoot, 0,1);
      }
      root->layoutHyperbolicTree();
    }

    HTModel(HTModel* n_mroot, HTModel* s_root, HTNode *node, HTModel *parent,int type) {
      is_this_root = false;
      mroot = n_mroot;
      root = s_root;
      weight = 1.0;
      if (root == 0)
        root = this;
      this->ntype = type;
      this->node = node;
      this->parent = parent;
      mroot->incrementNumberOfNodes();

      z = new HTCoord();

      globalWeight = 0.0;

      if (type) {

        HTNode* childNode = 0;
        HTModel* child = 0;

        for (unsigned long i = 0; i < node->children.size(); i++) {
            childNode = (HTNode*) node->children[i];
            if (childNode->isLeaf()) {
                child = new HTModel(mroot,root,childNode, this, 0);
            } else {
                child = new HTModel(mroot,root,childNode, this, 1);
            }
            children.push_back(child);
        }
        // here the down of the tree is built, so we can compute the weight
        computeWeight();
      }
    }

    void layoutHyperbolicTree() {
        HTSector* sector = new HTSector();
        double eps = 0.02;
        double d = sqrt(1 - (eps * eps));
        sector->A = new HTCoord(d, eps);
        sector->B = new HTCoord(d, -eps);
        layout(sector, root->getLength());
        delete sector->A;
        delete sector->B;
        delete sector;
    }


    void layout(HTSector* sector, double length) {
      // Nothing to do for the root node
      if (parent == 0) {
      }
      else
      {
        HTCoord zp = parent->getCoordinates();

        double angle = sector->getBisectAngle();

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
            HTCoord h(getCoordinates());
            h.x = -h.x;
            h.y = -h.y;
            sector->translate(h);
        }

        float nbrChild = (float)children.size();
        double l1 = (1.2 - mroot->getLength());
        double l2 = cos((20.0 * PI) / (2.6 * nbrChild + 38.0));
        length = mroot->getLength() + (l1 * l2);
        if (length > 0.9f) length = 0.9f;

        double   alpha = sector->getAngle();
        double   omega = sector->A->arg();
        HTCoord* K     = new HTCoord(sector->A);

        // It may be interesting to sort children by weight instead
        for (unsigned long i = 0; i < children.size(); ++i) {
            HTModel* child = children[i];
            HTSector* childSector = new HTSector();
            childSector->A = new HTCoord(K);
            omega += (alpha * (child->getWeight() / globalWeight));
            K->x = cos(omega);
            K->y = sin(omega);
            childSector->B = new HTCoord(K);
            child->layout(childSector, length);
            delete childSector->A;
            delete childSector->B;
            delete childSector;
        }
        delete K;
      }
    }



    ~HTModel() {
      if (is_this_root)
        delete root;

      for (std::vector<HTModel*>::iterator it = children.begin(); it != children.end(); ++it) {
  //      (*it)->delete_();
        delete *it;
      }
      delete z;
    }

/*    void delete_() {
      for (std::vector<HTModel*>::iterator it = children.begin(); it != children.end(); ++it) {
        (*it)->delete_();
        delete *it;
      }
      delete z;
    }
*/


  /* --- Length --- */

    /**
     * Returns the distance between a node and its children
     * in the hyperbolic space.
     *
     * @return    the distance
     */

     bool isLeaf() {
      return node->isLeaf();
    }

    double getLength() {
        return length;
    }

    void setLengthPoincare() {
        length = mroot->lp;
    }

    void setLengthKlein() {
        length = mroot->lk;
    }

    vsx_string<>getName() {
        return node->getName();
    }

    double getWeight() {
        return weight;
    }

    HTCoord* getCoordinates() {
        return z;
    }




    void computeWeight() {
        for (unsigned long i = 0; i < children.size(); i++) {
          globalWeight += children[i]->getWeight();
        }
        if (globalWeight != 0.0) {
          weight += log(globalWeight);
        }
    }




  /* --- Number of nodes --- */

    /**
     * Increments the number of nodes.
     */
    void incrementNumberOfNodes() {
        nodes++;
    }

    HTNode* getNode() {
        return node;
    }
    /**
     * Returns the number of nodes.
     *
     * @return    the number of nodes
     */
    int getNumberOfNodes() {
        return nodes;
    }
}; // HTModel
