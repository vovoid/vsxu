class HTCoord {
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
            a += PI;
        } else if (y < 0) {
            a += 2 * PI;
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
        double d = 2.0 / (1.0 + d2());
        k->x = (d * x);
        k->y = (d * y);
    }

    void kToP(HTCoord* k) {
        double d = 1.0 / (1.0 + sqrt(1 - d2()));
        k->x = (d * x);
        k->y = (d * y);
    }

    void pToZ(HTCoord* k, double z) {
        double d = 2.0 / (2.0 - z * (1.0 - d2()));
        k->x = (d * x);
        k->y = (d * y);
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
