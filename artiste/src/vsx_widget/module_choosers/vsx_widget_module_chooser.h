#ifndef VSX_NO_CLIENT
#ifndef VSX_WIDGET_MODULE_CHOOSER_H
#define VSX_WIDGET_MODULE_CHOOSER_H


const static double EPSILON = 1.0E-10; // epsilon
const static int ZONE_LENGTH = 4; // size of the zone


class HTCoord {
private:
public:
  double x;
  double y;
  double z;
  
  /* --- Constructor --- */
    HTCoord() {
      x = 0.0;
      y = 0.0;
      z = 0.0;
    }

    /**
     * Constructor.
     * x = 0 and y = 0.
     */

    /**
     * Constructor copying the given euclidian point.
     *
     * @param z    the euclidian point to copy 
     */
    HTCoord(HTCoord* z) {
        this->copy(z);
    }

    HTCoord(float x, float y) {
        this->x = x;
        this->y = y;
    }

    /**
     * Constructor fixing x and y.
     *
     * @param x    the x coord
     * @param y    the y coord
     */
    HTCoord(double x, double y) {
        this->x = x;
        this->y = y;
    }


  /* --- Copy --- */

    /**
     * Copy the given HTCoord into this HTCoord.
     *
     * @param z    the HTCoord to copy
     */
    void copy(HTCoord* z) {
        this->x = z->x;
        this->y = z->y;
    }



  /* --- Projections --- */

    /**
     * Progects from Screen to Euclidian.
     * 
     * @param x        the x screen coordinate
     * @param y        the y screen coordinate
     * @param sOrigin   the origin of the screen plane
     * @param sMax      the (xMax, yMax) point in the screen plane
     */
    void projectionStoE(float x, float y, HTCoord sOrigin, HTCoord sMax) {
        this->x = (double) (x - sOrigin.x) / (double) sMax.x;
        this->y = -((double) (y - sOrigin.y) / (double) sMax.y);
    } 

    void projectionEtoS(HTCoord ze, HTCoord sOrigin, HTCoord sMax) {
        x = (float) ze.x * sMax.x + sOrigin.x;
        y = - (float) ze.y * sMax.y + sOrigin.y;
    }
    bool contains(HTCoord zs) {
        float length = getDistance(zs);
        if (length <= ZONE_LENGTH) {
            return true;
        } else {
            return false;
        }
    }
    float getDistance(HTCoord z) {
        float d2 = (z.x - x) * (z.x - x) + (z.y - y) * (z.y - y);
        return  round(sqrt(d2));
    }

  /* --- Validation --- */

    /**
     * Is this coordinate in the hyperbolic disc ?
     *
     * @return    <CODE>true</CODE> if this point is in;
     *            <CODE>false</CODE> otherwise
     */
    bool isValid() {
        return (this->d2() < 1.0);
    }
 

  /* --- Transformation --- */

    /*
     * Some complex computing formula :
     *
     * arg(z)  = atan(y / x) if x > 0
     *         = atan(y / x) + Pi if x < 0
     *
     * d(z)    = Math.sqrt((z.x * z.x) + (z.y * z.y)) 
     *
     * conj(z) = | z.x
     *           | - z.y
     *
     * a * b   = | (a.x * b.x) - (a.y * b.y)
     *           | (a.x * b.y) + (a.y * b.x)
     *
     * a / b   = | ((a.x * b.x) + (a.y * b.y)) / d(b)
     *           | ((a.y * b.x) - (a.x * b.y)) / d(b)
     */
     
    /**
     * Conjugate the complex.
     */
    void conjugate() {
        y = -y;
    }

    /**
     * Multiply this coordinate by the given coordinate.
     *
     * @param z    the coord to multiply with
     */
    void multiply(HTCoord z) {
        double tx = x;
        double ty = y;
        x = (tx * z.x) - (ty * z.y);
        y = (tx * z.y) + (ty * z.x);
    }
    
    /**
     * Divide this coordinate by the given coordinate.
     *
     * @param z    the coord to divide with
     */
    void divide(HTCoord z) {
        double d = z.d2();
        double tx = x;
        double ty = y;
        x = ((tx * z.x) + (ty * z.y)) / d;
        y = ((ty * z.x) - (tx * z.y)) / d;
    }

    /**
     * Substracts the second coord to the first one
     * and put the result in this HTCoorE
     * (this = a - b).
     *
     * @param a    the first coord
     * @param b    the second coord
     */
    void sub(HTCoord a, HTCoord b) {
        x = a.x - b.x;
        y = a.y - b.y;
    }

    /**
     * Returns the angle between the x axis and the line
     * passing throught the origin O and this point.
     * The angle is given in radians.
     *
     * @return    the angle, in radians
     */
    double arg() {
        double a = atan(y / x);
        if (x < 0) {
            a += pi;
        } else if (y < 0) {
            a += 2 * pi;
        }
        return a;
    }

    /**
     * Returns the square of the distance from the origin 
     * to this point.
     *
     * @return    the square of the distance
     */
    double d2() {
        double d2 = (x * x) + (y * y);
        return d2;
    }

    /**
     * Returns the distance from the origin 
     * to this point.
     *
     * @return    the distance
     */
    double d() {
        return sqrt(d2());
    }

    /**
     * Returns the distance from this point
     * to the point given in parameter.
     *
     * @param p    the other point
     * @return     the distance between the 2 points
     */
    double d(HTCoord p) {
        return sqrt((p.x - x) * (p.x - x) + (p.y - y) * (p.y - y));
    }

    /**
     * Translate this Euclidian point 
     * by the coordinates of the given Euclidian point.
     * 
     * @param t    the translation coordinates
     */
    void translate(HTCoord t) {
        // z = (z + t) / (1 + z * conj(t))
        
        // first the denominator
        double denX = (x * t.x) + (y * t.y) + 1.0;
        double denY = (y * t.x) - (x * t.y) ;    

        // and the numerator
        double numX = x + t.x;
        double numY = y + t.y;

        // then the division (bell)
        double dd = (denX * denX) + (denY * denY);
        x = ((numX * denX) + (numY * denY)) / dd;
        y = ((numY * denX) - (numX * denY)) / dd;
    }

    void pToK(HTCoord* k) {
    //HTCoord* (HTCoord* k) {
        //HTCoord* k = new HTCoord();
        double d = 2.0 / (1.0 + d2());
        k->x = (d * x);
        k->y = (d * y);
        //return k;
    }

    void kToP(HTCoord* k) {
    //HTCoord* kToP() {
      //  HTCoord* k = new HTCoord();
        double d = 1.0 / (1.0 + sqrt(1 - d2()));
        k->x = (d * x);
        k->y = (d * y);
//        return k;
    }

// NEU
    void pToZ(HTCoord* k, double z) {
        //HTCoord* k = new HTCoord();
        double d = 2.0 / (2.0 - z * (1.0 - d2()));
        k->x = (d * x);
        k->y = (d * y);
        //return k;
    }


    HTCoord* zToP(double z) {
        HTCoord* k = new HTCoord();
        double s = (d2() - z + sqrt((z*z*d2()) - (2*z*d2()) + 1)) / 
                   (1 - d2());
        double y1 = x * (1 + s);
        double y2 = y * (1 + s);
        double y3 = z + s;
        k->x = y1 / (1+y3);
        k->y = y2 / (1+y3);
        return k;
    }
// FIN NEU
    
    void specialTrans(HTCoord alpha, HTCoord beta) {
        // z = (alpha * z + beta) / (conj(alpha) + conj(beta)*z)
 
        double dx = (this->x * beta.x) + (this->y * beta.y) + alpha.x;
        double dy = (this->y * beta.x) - (this->x * beta.y) - alpha.y;
        double d = (dx * dx) + (dy * dy);
        
        double tx = (this->x * alpha.x) - (this->y * alpha.y) + beta.x;
        double ty = (this->x * alpha.y) + (this->y * alpha.x) + beta.y;

        x = ((tx * dx) + (ty * dy)) / d;
        y = ((ty * dx) - (tx * dy)) / d;
    }
};



/**
 * The HTGeodesic class implements a geodesic 
 * linking to points in the Poincarre model.
 *
 * @author Christophe Bouthier [bouthier@loria.fr]
 * @version 1.0
 */

    const int LINE    = 0;       // draw a line
    const int ARC     = 1;       // draw an arc

class HTGeodesic {

//    private static final double EPSILON = 1.0E-10; // epsilon
private:

    int      type; // type of the geodesic


    //CURVE
    double  r;  // ray of the geod
    double  alpha;  // alpha factor
    double  beta;  // beta factor

    int     n;   // numer of segment in the approximation
    std::vector<HTCoord*> tz;
    std::vector<HTCoord*> ts;
    
    bool  kleinMode;
    double  projType;
    
    bool quadMode;

public:
  // intensity = alpha
  double intensity;
    HTCoord* za; // first point (Euclidian)
    HTCoord* zb; // second point (Euclidian)
    HTCoord* zc; // control point (Euclidian)
    HTCoord* zo; // center of the geodesic;

    float      d;
    HTCoord a; // first point (on the screen)
    HTCoord b; // second point (on the screen)
    HTCoord c; // control point (on the screen)

  float zpos;
  /* --- Constructor --- */

    /**
     * Constructor.
     *
     * @param za       the first point
     * @param zb       the second point
     */
    HTGeodesic(HTCoord* za, HTCoord* zb) {
      quadMode = false;
      type = LINE;
      r = 0.0; alpha = 0.0; beta = 0.0;
      n = 3;
      for (int i = 0; i < 2*n+1; ++i) {
        tz.push_back(new HTCoord());
        ts.push_back(new HTCoord());
      }
      projType = 0.0;
      d = 0;
      kleinMode = false;
//      a = b = c = 0;
      
        this->za    = za;
        this->zb    = zb;
      
        zc = new HTCoord();
        zo = new HTCoord();

        rebuild();
    }


  /* --- Refresh --- */

    /**
     * Refresh the screen coordinates of this node.
     *
     * @param sOrigin   the origin of the screen plane
     * @param sMax      the (xMax, yMax) point in the screen plane
     */
    void refreshScreenCoordinates(HTCoord sOrigin, HTCoord sMax) {
        if (kleinMode) {
          HTCoord ak; 
            za->pToK(&ak);
            a.projectionEtoS(ak, sOrigin, sMax);
          
            za->pToK(&ak);
            b.projectionEtoS(ak, sOrigin, sMax);
        } else if (projType != 0.0) {
          HTCoord a;
          za->pToZ(&a,projType);
            a.projectionEtoS(a, sOrigin, sMax);
          zb->pToZ(&a,projType);
            b.projectionEtoS(a, sOrigin, sMax);
          zc->pToZ(&a,projType);
            c.projectionEtoS(a, sOrigin, sMax);

            d = ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
            if (quadMode) {
              printf("ERROR, we're not in quadmode!\n");
                if (d != 0) {
                    //QUAD
//                    curve.setCurve(a.x, a.y, c.x, c.y, b.x, b.y);
                    //QUAD
                }
            } else {
                //CURVE
                HTCoord g;
                for (unsigned long i = 0; i < tz.size(); ++i) {
                  tz[i]->pToZ(&g,projType); // sets g to new value
                    ts[i]->projectionEtoS(g, sOrigin, sMax);
                }
                //CURVE
            }
        } else {
          //printf("za.x = %f %f %f %f %f %f\n",za->x,za->y,zb->x,zb->y,zc->x,zc->y);
            a.projectionEtoS(za, sOrigin, sMax);
            b.projectionEtoS(zb, sOrigin, sMax);
            c.projectionEtoS(zc, sOrigin, sMax);
          //printf("smax %f %f\n",sMax.x,sMax.y);

            d = ((a.x - b.x) * (a.x - b.x)) + ((a.y - b.y) * (a.y - b.y));
            if (quadMode) {
                if (d != 0) {
                printf("ERROR, we're not in quadmode!\n");
                    //QUAD
//                    curve.setCurve(a.x, a.y, c.x, c.y, b.x, b.y);
                    //QUAD
                }
            } else {
                //CURVE
//                printf("generating a curve\n");
                for (unsigned long i = 0; i < tz.size(); ++i) {
                  
                     ts[i]->projectionEtoS(tz[i], sOrigin, sMax);
                }
                //CURVE
            }
        }
    }


  /* --- Rebuild --- */

    /**
     * Builds the geodesic.
     */
    void rebuild() {
      if (! kleinMode) {
        if ( // za == origin
             (fabs(za->d()) < EPSILON) || 
             
             // zb == origin
             (fabs(zb->d()) < EPSILON) || 
             
             // za = lambda.zb
             (fabs((za->x / zb->x) - (za->y / zb->y)) < EPSILON) )
        {    
            type = LINE;
        } else {
            type = ARC;

            double da = 1 + za->x * za->x + za->y * za->y;
            double db = 1 + zb->x * zb->x + zb->y * zb->y;
            double dd = 2 * (za->x * zb->y - zb->x * za->y);
 
            zo->x = (zb->y * da - za->y * db) / dd;
            zo->y = (za->x * db - zb->x * da) / dd;

            double det = (zb->x - zo->x) * (za->y - zo->y) - 
                         (za->x - zo->x) * (zb->y - zo->y);
            double fa  = za->y * (za->y - zo->y) - za->x * (zo->x - za->x);
            double fb  = zb->y * (zb->y - zo->y) - zb->x * (zo->x - zb->x);

            zc->x = ((za->y - zo->y) * fb - (zb->y - zo->y) * fa) / det;
            zc->y = ((zo->x - za->x) * fb - (zo->x - zb->x) * fa) / det; 

            if (! quadMode) {
                //CURVE
                r = sqrt(zo->d2() - 1);
                double p = ((za->x - zo->x) * (zb->x - zo->x)) + 
                           ((za->y - zo->y) * (zb->y - zo->y)); 
                alpha = acos(p / (r*r));
                HTCoord cPrim;
                cPrim.sub(zc, zo);
                beta = cPrim.arg();

                tz[0]->x = zo->x + (r * cos(beta));
                tz[0]->y = zo->y + (r * sin(beta));

            
                for (int j = 1; j <= n; ++j) {
                    double dj = (double) j;
                    double dn = (double) (2 * n);

                    double pair = beta - (alpha * (dj / dn));
                    double impair  = beta + (alpha * (dj / dn));
                    float indPair = 2 * j;
                    float indImpair = (2 * j) - 1;
                
                    tz[(int)indPair]->x = zo->x + (r * cos(pair));
                    tz[(int)indPair]->y = zo->y + (r * sin(pair));

                    tz[(int)indImpair]->x = zo->x + (r * cos(impair));
                    tz[(int)indImpair]->y = zo->y + (r * sin(impair));
                }
                //CURVE
            }
        }
      }
    }


  /* --- Draw --- */

    /**
     * Draws this geodesic.
     *
     * @param g    the graphic context
     */
    void draw() {
      if (intensity < 0.001) return;
         /*float d = ((ts[0]->x - ts[2*n]->x) * (ts[0]->x - ts[2*n]->x)) +
                 ((ts[0]->y - ts[2*n]->y) * (ts[0]->y - ts[2*n]->y));
         if (d < 0.0001) {
             return;
         }*/
         
      glColor4f(1,1,1,intensity);

      glBegin(GL_LINES);
        glVertex3f(ts[0]->x,ts[0]->y,0);
        glVertex3f(ts[1]->x,ts[1]->y,0);
        glVertex3f(ts[0]->x,ts[0]->y,0);
        glVertex3f(ts[2]->x,ts[2]->y,0);
  
        int p = 0;
        for (int j = 1; j < n; ++j) {
          p = 2 * j;
          glVertex3f(ts[p]->x,ts[p]->y,0);
          glVertex3f(ts[p+2]->x,ts[p+2]->y,0);
          p = (2 * j) - 1;
          glVertex3f(ts[p]->x,ts[p]->y,0);
          glVertex3f(ts[p+2]->x,ts[p+2]->y,0);
        }
      glEnd();
    } 


//CURVE
  /* --- Draw curve --- */

    /**
     * Draws the curve specified by the given points.
     *
     * @param g    the graphic context
     */
    void drawCurve()
    {
    }
//CURVE

  /* --- Klein --- */

    /**
     * Sets the klein mode.
     *
     * @param mode    setting on or off
     */
    void setkleinMode(bool mode) {
        if (mode != kleinMode) {
            kleinMode = mode;
        }
    }

    /**
     * Sets the projection type.
     *
     * @param z    the value of the projection
     */
    /*void changeProjType(double z) {
        if (z != projType) {
            projType = z;
        }
    }*/

    void setQuadMode(bool mode) {
        if (quadMode != mode) {
            quadMode = mode;
            rebuild();
        }
    }

    bool getQuadMode() {
        return quadMode;
    }
};

/**
 * The HTSector class implements the sector division.
 *
 * @author Christophe Bouthier [bouthier@loria.fr]
 * @version 1.0
 */
class HTSector {
public:
    HTCoord* A;
    HTCoord* B;

  /* --- Angle --- */

    /**
     * Returns the angle of the sector.
     *
     * @return    the angle of the sector
     */
    double getAngle() {
        double angle = B->arg() - A->arg();
        if (angle < 0) {
            angle += 2*pi;
        }
        return angle;
    }

    /**
     * Returns the angle of the bissectrice of the sector.
     *
     * @return    the bisector angle
     */
    double getBisectAngle() {
        return A->arg() + (getAngle() / 2);
    }

  /* --- Transformations --- */
    /**
     * Translates.
     *
     * @param t    the translation vector
     */
    void translate(HTCoord t) {
        A->translate(t);
        B->translate(t);
    }

};


/**
 * The HTNode interface should be implemented by 
 * object that are node of the tree that want to be 
 * displayed in the TreeMap.
 * <P>
 * If you have already a tree structure, just implements
 * this interface in node of the tree.
 *
 * @author Christophe Bouthier [bouthier@loria.fr]
 * @version 1.0
 */
class HTNode {

public:
  vsx_module_info* module_info;
  int value;
  vsx_string name;
  vsx_color color;
  vsx_vector size;
    std::vector<HTNode*> children;
    HTNode();
    virtual ~HTNode();
    
    HTNode* add(vsx_string name,vsx_module_info* m_info);
    
    /*HTNode(int t) {
      color.r = 0.5;
      color.g = 0.5;
      color.b = 0.5;
      color.a = 0.9;
      size.x = 1;
      size.y = 0.2;

      HTNode* a = new HTNode(1,1); 
      children.push_back(a);
      int n = rand()%5;
      for (int b = 0; b < n; ++b) {
      a = new HTNode(1,1); 
      children.push_back(a);
      }
    }

    HTNode(int t,int t2) {
      color.r = 0.5;
      color.g = 0.5;
      color.b = 0.5;
      color.a = 0.9;
      size.x = 0.5;
      size.y = 0.1;
      HTNode* a = new HTNode(1,1,1); 
      children.push_back(a);
      a = new HTNode(1,1,1); 
      children.push_back(a);
      a = new HTNode(1,1,1); 
      children.push_back(a);
    }
    
    HTNode(int t,int t2,int t3) {
      color.r = 0.5;
      color.g = 0.5;
      color.b = 0.5;
      color.a = 0.9;
      size.x = 0.5;
      size.y = 0.1;
      HTNode* a = new HTNode(1,1,1,1); 
      children.push_back(a);
      int n = rand()%4;
      for (int b = 0; b < n; ++b) {
        a = new HTNode(1,1,1,1); 
        children.push_back(a);
      }
    }

    HTNode(int t,int t2,int t3,int t4) {
      color.r = 0.5;
      color.g = 0.5;
      color.b = 0.5;
      color.a = 0.9;
      size.x = 0.4;
      size.y = 0.08;
    }*/

    /**
     * Checks if this node is a leaf or not.
     * A node could have no children and still not
     * be a leaf.
     *
     * @return    <CODE>true</CODE> if this node is a leaf;
     *            <CODE>false</CODE> otherwise
     */
    virtual bool isLeaf();
    
    /**
     * Returns the name of this node.
     * Used to display a label in the hyperbolic tree.
     *
     * @return    the name of this node
     */
    vsx_string getName() {
      return name;
    }
};

class HTModel {
private:
    HTModel* parent; // our parent, if we're root this is NULL
    
    double      length;  // distance between node and children
    int         nodes;    // number of nodes


    int ntype; // if 0, single, if 1, composite
    HTCoord*             z     ; // Euclidian coordinates
    double globalWeight;  // sum of children weight
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
    HTModel(HTNode* noderoot);
    
    ~HTModel() {
      for (std::vector<HTModel*>::iterator it = children.begin(); it != children.end(); ++it) {
        (*it)->delete_();
        delete *it;
      }
    }
    void delete_() {
      for (std::vector<HTModel*>::iterator it = children.begin(); it != children.end(); ++it) {
        (*it)->delete_();
        delete *it;
      }  
      delete z;
    }  
    
    
    HTModel(HTModel* n_mroot, HTModel* s_root, HTNode *node, HTModel *parent,int type) {
      //printf("HTModel constructor\n");
      mroot = n_mroot;
      root = s_root;
      weight = 1.0;
      if (root == 0) root = this;
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
        //for (Enumeration e = node.children(); e.hasMoreElements(); ) {
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

  /* --- Accessor --- */

    /**
     * Returns the root of the tree model.
     *
     * @return    the root of the tree model
     */
    //HTModelNode getRoot() {
//        return root;
//    }


  /* --- Length --- */

    /**
     * Returns the distance between a node and its children
     * in the hyperbolic space.
     *
     * @return    the distance
     */
     
     bool isLeaf() {
      //if (ntype) return false; else return true;
      return node->isLeaf();
      //return !ntype;
        //return true;
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

/*    void layoutHyperbolicTree() {
        root->layoutHyperbolicTree_root();
    }
  */  
    vsx_string getName() {
        return node->getName();
    }

    double getWeight() {
        return weight;
    }

    HTCoord* getCoordinates() {
        return z;
    }
    void layoutHyperbolicTree();

    
    
    void layout(HTSector* sector, double length);

    void computeWeight() {
        for (unsigned long i = 0; i < children.size(); i++) {
          globalWeight += children[i]->getWeight();
        } 
        //printf("pre_globalWeight : %f\n",weight);
        if (globalWeight != 0.0) {
          weight += log(globalWeight);
        }
        //printf("globalWeight : %f\n",weight);
    }


    //void addChild(HTModel* child) {
//      children.push_back(child);
//    }


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


class vsx_widget_hyperbolic_tree : public vsx_widget {
private:
    HTModel*    model;  // the tree model
    //HTView*     view;  // the view using this drawing model
    
    HTCoord   sOrigin;  // origin of the screen plane
    HTCoord   sMax;  // max point in the screen plane 

    int       border;
    
    int ntype; // type - 0 = leaf, 1 = composite
      
    double ray[4];
    
    bool    fastMode; // fast mode
    bool    longNameMode; // long name mode
    bool    transNotCorrected;
    bool    kleinMode; // klein mode
// NEU
    double  projType; // projection type 
// FIN NEU
//------------------------------------------------------------------------------
 

    vsx_widget_hyperbolic_tree* father;  // father of this node
    vsx_widget_hyperbolic_tree* brother;  // brother of this node
    
    std::vector<vsx_widget_hyperbolic_tree*> children;
    std::map<vsx_widget_hyperbolic_tree*,HTGeodesic*> geodesics;

    //HTNodeLabel         label    = null;  // label of the node

public:

  HTCoord*            ze;  // current euclidian coords
  HTCoord*            oldZe;  // old euclidian coords
  HTCoord*            zs;  // current screen coords
  HTGeodesic* mygeodesic;
#ifndef VSXU_PLAYER
  vsx_texture* mtex;
#endif
  //static vsx_font myf;
  vsx_widget_hyperbolic_tree* selected;  // selected node
  HTModel*         node;  // encapsulated HTModelNode
  vsx_color color;
  vsx_vector pos, size;
  // previously static
  vsx_widget_hyperbolic_tree* root;  // drawing model
  vsx_widget_hyperbolic_tree* draw_root;  // the root of the drawing tree
    
  int NBR_FRAMES; // number of intermediates 
    
  void on_delete()
  {
    if (ze) delete ze;
    if (zs) delete zs;
    if (oldZe) delete oldZe;
  }
     
    // create the base maintainer class     
  vsx_widget_hyperbolic_tree(vsx_widget_hyperbolic_tree* s_root, vsx_widget_hyperbolic_tree* s_drawroot,HTModel* model) { 
      //HTDraw(HTModel model, HTView view) {
      if (s_root == 0) root = this;
      selected = 0;
      root = this;
      mygeodesic = 0;

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
//        this.view = view;
        this->model = model;
        HTModel* model_root = model->root;
        //sOrigin = new HTCoord();
        //sMax = new HTCoord();

        //ray = double[4];
        /*ray[0] = model->getLength();
        printf("ray[0] = %f",ray[0]);

        for (int i = 1; i < 4; i++) {
            ray[i] = (ray[0] + ray[i - 1]) / (1 + (ray[0] * ray[i - 1]));
        }*/

        if (model_root->isLeaf()) {
            // single
            draw_root = new vsx_widget_hyperbolic_tree(root, 0, 0, model_root, 0);
        } else {
//          printf("composite drawroot\n");
          // composite
            draw_root = new vsx_widget_hyperbolic_tree(root, 0, 0, model_root, 1);
        }
    }
    
    vsx_widget_hyperbolic_tree(vsx_widget_hyperbolic_tree* s_root, vsx_widget_hyperbolic_tree* s_drawroot, vsx_widget_hyperbolic_tree* father, HTModel* node, int type) {
      selected = 0;
      mygeodesic = 0;
      root = s_root;
      if (s_drawroot == 0) draw_root = this;
      else draw_root = s_drawroot;
			projType = 0;
      this->father = father;
      this->node = node;
      kleinMode = false;
//      this->model = model;

      //label = new HTNodeLabel(this);

      ze = new HTCoord(node->getCoordinates());
      oldZe = new HTCoord(ze);
      zs = new HTCoord();
      
      ntype = type;

      if (type) {
// already done        this->node = node;

        HTModel* childNode = 0;
        vsx_widget_hyperbolic_tree* child = 0;
        vsx_widget_hyperbolic_tree* brother = 0;
        bool first = true;
        bool second = false;
        for (unsigned long i = 0; i < node->children.size(); i++)
        {
//          printf("vsx_widget_hyperbolic_tree construct %d\n",i);

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
    
    int getBorder() {
      return border;
    }
    
    void refreshScreenCoordinates() {
        //Insets insets = view.getInsets();
        //PDA
        sMax.x = (size.x)/2;
        sMax.y = (size.y)/2;
        //sMax.x = (view.getSize().width - insets.left - insets.right) / 2;
        //sMax.y = (view.getSize().height - insets.top - insets.bottom) / 2;
        //PDA
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
    
    void translateToOrigin(vsx_widget_hyperbolic_tree* node);

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

                // alpha = 1 + conj(zo)*t
                HTCoord alpha;
                alpha.x = 1 + (zo->x * t.x) + (zo->y * t.y);
                alpha.y = (zo->x * t.y) - (zo->y * t.x);
                // beta = zo + t
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

            // alpha = 1 + conj(zo)*t
            HTCoord* alpha = new HTCoord();
            alpha->x = 1 + (zo->x * t->x) + (zo->y * t->y);
            alpha->y = (zo->x * t->y) - (zo->y * t->x);
            // beta = zo + t
            HTCoord* beta = new HTCoord();
            beta->x = zo->x + t->x;
            beta->y = zo->y + t->y;
           
            draw_root->specialTrans(alpha, beta);
            //view.repaint();
        }
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
            /*if (mode == false) {
                view.epaint();
            }*/
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
            //view.repaint();
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
    void set_kleinMode(bool mode) {
        /*if (mode != kleinMode) {
            HTCoord* zo = new HTCoord(draw_root->getCoordinates());
            
            kleinMode = mode;
            if (kleinMode) {
                model->setLengthKlein();
            } else {
                model->setLengthPoincare();
            }
            model->layoutHyperbolicTree();
            draw_root->set_kleinMode_(mode);
            
            restore();
            if (! kleinMode) { // poincare
                translateCorrected(new HTCoord(), zo->pToK());
            } else {           // klein
                translateCorrected(new HTCoord(), zo->kToP());
            }
            endTranslation();
        }*/
    }

// NEU
    /**
     * Change projection type.
     *
     * @param z    the new value of proj type
     */
    void changeProjType(double z);

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
    vsx_string getName() {
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
// NEU
        } else if (projType != 0.0) {
          HTCoord a;
          ze->pToZ(&a,projType);
            zs->projectionEtoS(a, sOrigin, sMax);
// FIN NEU
        } else {
            zs->projectionEtoS(ze, sOrigin, sMax);
        }

        if (ntype) {
          //vsx_widget_hyperbolic_tree child = null;
          for (unsigned long i = 0; i < children.size(); i++) {
//          printf("refreshing geodesic on child %d",i);
          //for (Enumeration e = children(); e.hasMoreElements(); ) {
            //child = (HTDrawNode) e.nextElement();
            children[i]->refreshScreenCoordinates(sOrigin, sMax);
            
            if (geodesics.find(children[i]) != geodesics.end())
            geodesics[children[i]]->refreshScreenCoordinates(sOrigin, sMax);
            
          }
        }
    } 
    

    void drawNodes_() {
        if (fastMode == false) {
          // add text drawing HERE
            for (unsigned long i = 0; i < children.size(); i++) {
                children[i]->drawNodes_();
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
        //for (Enumeration e = children(); e.hasMoreElements(); ) {
            children[i]->specialTrans(alpha, beta);

            if (geodesics.find(children[i]) != geodesics.end())
            geodesics[children[i]]->rebuild();
        }
    }

    void translate(HTCoord dest) {
        ze->copy(oldZe);
        ze->translate(dest);
        

        if (!ntype) return;
        for (unsigned long i = 0; i < children.size(); i++) {
            //child = (HTDrawNode) e.nextElement();
            children[i]->translate(dest);
            if (geodesics.find(children[i]) != geodesics.end())
            geodesics[children[i]]->rebuild();
        }
        
        
    }
    void endTranslation_() {
      oldZe->copy(ze);
      if (!ntype) return;

      for (unsigned long i = 0; i < children.size(); i++) {
        children[i]->endTranslation_();
      }
    }

    void restore_() {
        HTCoord* orig = node->getCoordinates();
        ze->x = orig->x;
        ze->y = orig->y;
        oldZe->copy(ze);
      if (!ntype) return;

      for (unsigned long i = 0; i < children.size(); i++) {
            children[i]->restore_();
            if (geodesics.find(children[i]) != geodesics.end())
            geodesics[children[i]]->rebuild();
      }
    }


    void set_longNameMode_(bool mode) {
        if (mode != longNameMode) {
            longNameMode = mode;
        }
    }

    /*void setQuadMode(bool mode) {
    }*/
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
    
    vsx_widget_hyperbolic_tree* findNode(HTCoord* zs) {
        //if (label->contains(zs)) {
//            return this;
//        } else {
            return 0;
//        }
    }
    
//----------------------------------------------------------------------------------------------------------------------    
  // 3rd, composite stuff CRAP
    void addChild(vsx_widget_hyperbolic_tree* child) {
        children.push_back(child);
        geodesics[child] = new HTGeodesic(getCoordinates(), child->getCoordinates());
        geodesics[child]->zpos = pos.z;
        //printf("creating geodesic\n");
    }
  /* --- Drawing --- */

    /**
     * Draws the branches from this node to 
     * its children.
     *
     * @param g    the graphic context
     */
    void drawBranches_();
    /*void changeProjType__(double z) {
        changeProjType_(z);

        HTDrawNode child = null;
        for (Enumeration e = children(); e.hasMoreElements(); ) {
            child = (HTDrawNode) e.nextElement();
            child.changeProjType(z);
            HTGeodesic geod = (HTGeodesic) geodesics.get(child);
            if (geod != null) {
                geod->changeProjType(z);
                geod->rebuild();
            }
        }
    }*/
// FIN NEU

    vsx_widget_hyperbolic_tree* findNode__(HTCoord zs);
    vsx_widget_hyperbolic_tree* findNode(vsx_string designator);
};

class vsx_widget_ultra_chooser : public vsx_widget {
 HTModel* mymodel;
 vsx_widget_hyperbolic_tree* treedraw;
 
 HTCoord startpoint, endpoint, clickpoint;
 bool show_, hide_;
 bool first;
 bool moved;
 
 HTCoord anim_startpoint, anim_endpoint, anim_curpoint;
 vsx_vector v_anim_startpoint, v_anim_endpoint;
 double anim_x;
 
 bool animating;
 //vsx_font myf;
 bool drag_module;
 bool drag_dropped;
 bool allow_move;
 vsx_vector drag_pos;
 vsx_vector drop_pos;
 vsx_string macro_name;
 vsx_vector tooltip_pos;
 int draw_tooltip;
 vsx_string tooltip_text;
 
 vsx_widget* name_dialog;
 float interpolation_speed;
 
 vsx_widget* object_inspector;
 void cancel_drop();

public:
  HTNode* module_tree;
  
  int mode; // 0 = ordinary, 1 = double-click-returns-string
  vsx_widget* command_receiver;
  vsx_string command;
  vsx_string message; // friendly message showing the user what to do
  
  vsx_widget* server;
  
  double sx,sy,ex,ey; // internal coordinates
  double smx, smy; // coordinate where mouse has been clicked
  double mdx, mdy; // delta mouse move
  int inside_xy_l(vsx_vector &test, vsx_vector &global);
  void vsx_command_process_b(vsx_command_s *t);
  
  void show();
  void hide();
  void build_tree();
  void center_on_item(vsx_string target);
  
  void init();
#ifndef VSXU_PLAYER  
  void reinit();
#endif
  void event_mouse_down(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_double_click(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_up(vsx_widget_distance distance,vsx_widget_coords coords,int button);
  void event_mouse_move_passive(vsx_widget_distance distance,vsx_widget_coords coords);
  void event_mouse_move(vsx_widget_distance distance,vsx_widget_coords coords);
  bool event_key_down(signed long key, bool alt, bool ctrl, bool shift);          
  void draw_2d();
  vsx_widget_ultra_chooser();
  ~vsx_widget_ultra_chooser();
  void on_delete();
};

#endif
#endif
