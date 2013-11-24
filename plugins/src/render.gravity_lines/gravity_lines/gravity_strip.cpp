#include "vsx_avector.h"
#include "vsx_array.h"
#include "vsx_mesh.h"
#include "gravity_strip.h"
#include "vsx_timer.h"

void gravity_strip::init_strip()
{
	length = 1.0f;
  	thin_edges = true;
	curr_time = 0.0f;
	last_step_time = 0.0f;
	step_freq = 100.0f;
	offs = 0;


	num_lines = 1;
	vsx_vector v;

	Mass m;
	m.init(v,v,7 / (rand() / (float)RAND_MAX * 2.5f + 0.35f));
	masses.push_back(m);
  masses.push_back(m);
	first = true;
}

vsx_timer timer;

void gravity_strip::render()
{
  if (!oldPos.size()) return;
  glBegin(GL_QUAD_STRIP);

    float p = 1.0f/num_lines;
    float p_ = 1-p;

    if (length > 1.0f) length = 1.0f;
    if (length < 0.01f) length = 0.01f;
    int num = (int)((float)BUFF_LEN * length);
    float fnum = num;
    int i_ofs = offs - num;
    if (i_ofs < 0) i_ofs += BUFF_LEN;
    glColor4f(color0[0]*p_ + p * color1[0],
              color0[1]*p_ + p * color1[1],
              color0[2]*p_ + p * color1[2],
              1.0f);
    for(int j = 0; j < num; j++)
    {
      int k = (i_ofs + j) % (BUFF_LEN-1);

      vsx_vector d;
      vsx_vector kk = oldPos[0][k+1] - oldPos[0][k];
      if (kk.norm() > 0.000001f)
      {
        d.cross( oldPos[0][k], oldPos[0][k+1] - oldPos[0][k] );
        d.normalize();
        d = d * width*0.1f;
        float w = 1.0f;

        vsx_vector res = oldPos[0][k] + d * 0.5f;
        vsx_vector res_a = res + d * w;
        vsx_vector res_b = res - d * w;

        glColor4f(color0[0], color0[1], color0[2], color0[3]);
        glTexCoord2f(0.0f, (float)j / fnum);
        glVertex3f(res_a.x, res_a.y, res_a.z);

        vsx_vector res_n = res_a;
        res_n.normalize();
        glNormal3f(res_n.x, res_n.y, res_n.z);

        glTexCoord2f(1.0f, (float)j / fnum);
        glVertex3f(res_b.x, res_b.y, res_b.z);
        res_n = res_b;
        res_n.normalize();
        glNormal3f(res_n.x, res_n.y, res_n.z);
      }
    }
  glEnd();
}

void gravity_strip::generate_mesh(
       vsx_mesh       &mesh,
       vsx_face*      &fs_d,
       vsx_vector*    &vs_d,
       vsx_vector*    &ns_d,
       vsx_tex_coord* &ts_d,
       vsx_matrix*    modelview_matrix,
       vsx_vector*    upvector,
       int &generated_vertices,
       int &generated_faces
     )
{
  VSX_UNUSED(mesh);
  if (!oldPos.size()) return;
  if (length > 1.0f) length = 1.0f;
  if (length < 0.01f) length = 0.01f;

  // compute eyvec
  // 1. get the modelview matrix
  // 2. invert it
  // 3. multiply by vector [0,0,0,1]
  vsx_matrix model_matrix = *modelview_matrix;
  model_matrix.transpose();
  vsx_vector eye_pos = model_matrix.multiply_vector(vsx_vector(upvector->x,upvector->y,upvector->z));


  int num = (int)((float)BUFF_LEN * length);
  float fnum = (float)num;
  int i_ofs = offs - num;
  if (i_ofs < 0) i_ofs += BUFF_LEN;

  float m_width = width*0.1f;
  float d_inc = 1.0f / fnum;
  float d_j = 0.0f;
  int loop_counter = 0;
  for(int j = 0; j < num; j++)
  {
    int k = (i_ofs + j) % (BUFF_LEN-1);
    vsx_vector kk = oldPos[0][k+1] - oldPos[0][k];
    if (kk.norm() > 0.0001f)
    {
      vsx_vector a;
      a.cross( -kk, eye_pos );

      a.normalize();
      vsx_vector d;

      d = a * m_width;

      vsx_vector res = oldPos[0][k] + d * 0.5f;
      //#
      *vs_d = res + d;
      *ns_d = *vs_d;
      (*ns_d).normalize();
      vs_d++;
      ns_d++;
      //#
      *vs_d = res - d;
      *ns_d = *vs_d;
      (*ns_d).normalize();
      vs_d++;
      ns_d++;

      (*ts_d).s = 0.0f;
      (*ts_d).t = d_j;
      ts_d++;
      (*ts_d).s = 1.0f;
      (*ts_d).t = d_j;
      ts_d++;
      generated_vertices++;
      generated_vertices++;

      // -1   -2
      // -3   -4
      if (loop_counter > 2)
      {
        (*fs_d).a = generated_vertices -2;
        (*fs_d).b = generated_vertices -3;
        (*fs_d).c = generated_vertices -1;
        fs_d++;
        generated_faces++;
        (*fs_d).a = generated_vertices -4;
        (*fs_d).b = generated_vertices -3;
        (*fs_d).c = generated_vertices -2;
        fs_d++;
        generated_faces++;
      }
      loop_counter++;
    }
    d_j += d_inc;
  }
}
