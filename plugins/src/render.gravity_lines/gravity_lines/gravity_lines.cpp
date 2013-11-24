#include "vsx_avector.h"
#include "vsx_array.h"
#include "gravity_lines.h"

void Mass::setCenter(const float &x, const float &y, const float &z) {
  center.x = x;
  center.y = y;
  center.z = z;
}


//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

void gravity_lines::init()
{
	curr_time = 0.0f;
	last_step_time = 0.0f;
	step_freq = 100.0f;
	offs = 0;
	width = 0.1f;

	first = true;
	num_lines = 40;
	vsx_vector v;
	
  for(int i = 0; i < num_lines / 3; i++)
  {
		Mass m; 
		m.init(v, v, 7 / (rand() / (float)RAND_MAX * 2.5f + 0.35f));
	  masses.push_back(m);
	}

  for(int i = num_lines / 3; i < num_lines+1; i++)
  {
		Mass m; 
		m.init(v, v, 7 / (rand() / (float)RAND_MAX * 1.1f + 1.31f));
	  masses.push_back(m);
	}
}


void gravity_lines::reset_pos(float x, float y, float z)
{
  if (!oldPos.size()) return;
  for(int i = 0; i < num_lines; i++) {
    for (size_t j = 0; j < BUFF_LEN; j++)
    {
      oldPos[i][j].x = x;
      oldPos[i][j].y = y;
      oldPos[i][j].z = z;
    }
  }
}

void gravity_lines::update(float delta_time, float x, float y, float z) {
	if (delta_time > 0.16667f) delta_time = 0.16667f;
	while (oldPos.size() != (unsigned long)num_lines)
	{
		oldPos.push_back(new vsx_vector[BUFF_LEN]);
	}
	
  curr_time += (float)fabs(delta_time);
  
  int num_steps = (int)((curr_time - last_step_time) * step_freq);

	if (first) {
		num_steps = BUFF_LEN;
	}

  last_step_time += num_steps / step_freq;
  
  vsx_vector pos;

  for(int j = 0; j < num_steps; j++) {
    for(int i = 0; i < num_lines; i++) {
  		masses[i].friction = friction*0.07f;
  		masses[i].setCenter(x, y, z);
      masses[i].update(0.08f);
      pos = masses[i].getPos();
      oldPos[i][offs].x = pos.x;
      oldPos[i][offs].y = pos.y;
      oldPos[i][offs].z = pos.z;
    }
    offs = (offs + 1) % BUFF_LEN;
  }

	if (first) {
  	offs = 0; 
		first = false;
	}

}

void gravity_lines::render() {
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.0f);
  for(int i = 0; i < num_lines; i++) {
    glBegin(GL_LINE_STRIP);
    float p = (float)(i+1)/num_lines;
    float p_ = 1-p;
    
    for (int j = 0; j < BUFF_LEN; j++) {
      glColor4f(color0[0]*p_ + p * color1[0],
                color0[1]*p_ + p * color1[1],
                color0[2]*p_ + p * color1[2],
                (j+1) / (float)BUFF_LEN * color0[3]);
      
      int k = (offs + j) % BUFF_LEN;
      glVertex3f(
        oldPos[i][k].x,
        oldPos[i][k].y,
        oldPos[i][k].z);
    }
    glEnd();
  }
}

gravity_lines::~gravity_lines()
{
  for (unsigned long i = 0; i < oldPos.size(); ++i) {
    delete[] oldPos[i];
  } 
}

