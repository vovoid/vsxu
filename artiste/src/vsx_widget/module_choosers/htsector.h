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
            angle += 2*PI;
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
