#ifndef VSX_BSPLINE_H
#define VSX_BSPLINE_H

class vsx_bspline {
public:
  float		current_pos;
  float   real_pos;
  vsx_vector	p0, p1, p2, p3;
  float		minDistNext;
  float		maxDistNext;
  vsx_vector center;
  float		radius;
  long old_pos;
  vsx_array<vsx_vector> points;
  
  vsx_bspline() : real_pos(0.0f),old_pos(0) {
 		current_pos = 0.0f;
  };
  
  void init_random_points() {
    for (int i = 0; i < 250; ++i) {
      points.push_back(
        vsx_vector(
          ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f),
          ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f),
          ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f)
        )
      );
    }
    p0 = points[0];
    p1 = points[1];
    p2 = points[2];
    p3 = points[3];

  }

  ~vsx_bspline() {
    printf("bspline destructor\n");
  }
	vsx_bspline(vsx_vector _center, float _radius, float _minDistNext, float _maxDistNext) {
		current_pos = 0.0f;
		center = _center;
		radius = _radius;
		minDistNext = _minDistNext;
		maxDistNext = _maxDistNext;
	};

	void init(vsx_vector _center, float _radius, float _minDistNext, float _maxDistNext) {
		center = _center;
		radius = _radius;
		minDistNext = _minDistNext;
		maxDistNext = _maxDistNext;
	};
	
	void set_pos(float t) {
    int tt = (int)t;
    //float tt = fmod(t,(float)points.size()-4);
    if (tt != old_pos) {
      old_pos = tt;
      p0 = points[(old_pos)%points.size()];
      p1 = points[(old_pos+1)%points.size()];
      p2 = points[(old_pos+2)%points.size()];
      p3 = points[(old_pos+3)%points.size()];
    }
    real_pos = current_pos = t;
    if (current_pos > 1.0f) current_pos -= (int)current_pos;
  }
  
  void step(float stepn) {
    set_pos(real_pos+stepn);
  }

	/*void step(float step) {
		int numNew = (int)(currentPos + step) - (int)(currentPos);

		if (numNew > 4)	numNew = 4; // to avoid creating of too many new points...

		for (int i = 0; i < numNew; i++) {
      //printf("adding new point %d\n",i);

			vsx_vector next;

			p0 = p1;
			p1 = p2;
			p2 = p3;

			float len;

			do {
				next.x = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * maxDistNext;
				next.y = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * maxDistNext;
				next.z = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * maxDistNext;

				len = next.norm();

				if(len > maxDistNext) next.normalize(maxDistNext);
				if(len < minDistNext) next.normalize(minDistNext);

				p3 = p2 + next;
				
				len = (p3 - center).length();
			} while(len > radius);
		}

		currentPos += step;
		if(currentPos > 1.0f) currentPos -= int(currentPos);
	};*/

	vsx_vector calc_coord() {
    vsx_vector v;
		float t = current_pos;
		float t2 = t * t;
		float t3 = t2 * t;

		float k1 = 1.0f - 3.0f * t + 3.0f * t2 - t3;
		float k2 = 4.0f - 6.0f * t2 + 3.0f * t3;
		float k3 = 1.0f + 3.0f * t + 3.0f * t2 - 3.0f * t3;
		/*printf("k1: %f\nk2: %f\nk3: %f\nt3: %f\n",k1,k2,k3,t3);
		p0.dump("p0");
		p1.dump("p1");
		p2.dump("p2");
		p3.dump("p3");*/

		v = (p0 * k1 + 
				p1 * k2 + 
				p2 * k3 + 
				p3 * t3) * (1.0f / 6.0f);
		return v;
	};
};



/*class vsx_bspline_rand {
  float		currentPos;
  vsx_vector	p0, p1, p2, p3;
  float		minDistNext;
  float		maxDistNext;
  vsx_vector center;
  float		radius;

public:
  
  vsx_bspline_rand() {};

	vsx_bspline_rand(vsx_vector _center, float _radius, float _minDistNext, float _maxDistNext) {
		currentPos = 0.0f;
		center = _center;
		radius = _radius;
		minDistNext = _minDistNext;
		maxDistNext = _maxDistNext;
//		step(4.0f);
	};

	void init(vsx_vector _center, float _radius, float _minDistNext, float _maxDistNext) {
		currentPos = 0.0f;
		center = _center;
		radius = _radius;
		minDistNext = _minDistNext;
		maxDistNext = _maxDistNext;
//		step(4.0f);
	};

	void step(float step) {
		int numNew = (int)(currentPos + step) - (int)(currentPos);

		if (numNew > 4)	numNew = 4; // to avoid creating of too many new points...

		for (int i = 0; i < numNew; i++) {
      //printf("adding new point %d\n",i);

			vsx_vector next;

			p0 = p1;
			p1 = p2;
			p2 = p3;

			float len;

			do {
				next.x = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * maxDistNext;
				next.y = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * maxDistNext;
				next.z = ((float)rand() / (float)RAND_MAX * 2.0f - 1.0f) * maxDistNext;

				len = next.norm();

				if(len > maxDistNext) next.normalize(maxDistNext);
				if(len < minDistNext) next.normalize(minDistNext);

				p3 = p2 + next;
				
				len = (p3 - center).length();
			} while(len > radius);
		}

		currentPos += step;
		if(currentPos > 1.0f) currentPos -= int(currentPos);
	};

	vsx_vector calc_coord() {
    vsx_vector v;
		float t = currentPos;
		float t2 = t * t;
		float t3 = t2 * t;

		float k1 = 1.0f - 3.0f * t + 3.0f * t2 - t3;
		float k2 = 4.0f - 6.0f * t2 + 3.0f * t3;
		float k3 = 1.0f + 3.0f * t + 3.0f * t2 - 3.0f * t3;
		printf("k1: %f\nk2: %f\nk3: %f\nt3: %f\n",k1,k2,k3,t3);
		p0.dump("p0");
		p1.dump("p1");
		p2.dump("p2");
		p3.dump("p3");*/

/*		v = (p0 * k1 + 
				p1 * k2 + 
				p2 * k3 + 
				p3 * t3) * (1.0f / 6.0f);
		return v;
	};
};
*/
#endif
