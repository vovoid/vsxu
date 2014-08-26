/**
 * The HTGeodesic class implements a geodesic
 * linking to points in the Poincarre model.
 *
 * @author Christophe Bouthier [bouthier@loria.fr]
 * @version 1.0
 */
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

    this->za    = za;
    this->zb    = zb;

    zc = new HTCoord();
    zo = new HTCoord();

    rebuild();
  }

  ~HTGeodesic()
  {
    for(std::vector<HTCoord*>::iterator it = tz.begin(); it != tz.end(); it++)
    {
      delete (*it);
    }
    for(std::vector<HTCoord*>::iterator it = ts.begin(); it != ts.end(); it++)
    {
      delete (*it);
    }
    delete zc;
    delete zo;
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
