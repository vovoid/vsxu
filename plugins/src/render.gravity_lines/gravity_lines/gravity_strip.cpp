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
/*	color0[0] = 1;
	color0[1] = 1;
	color0[2] = 1;
	color0[3] = 1;
	color1[0] = 1;
	color1[1] = 1;
	color1[2] = 1;
	color1[3] = 1;*/

	Mass m;
	m.init(v,v,7 / (rand() / (float)RAND_MAX * 2.5f + 0.35f));
	masses.push_back(m);
	first = true;
	//masses[1].init(v,v, masses[0].mass+0.2f);
}

vsx_timer timer;

void gravity_strip::render() {
  if (!oldPos.size()) return;
  //glEnable(GL_LINE_SMOOTH);
  //glLineWidth(1.0f);
  //for(int i = 0; i < num_lines; i++) {
    glBegin(GL_QUAD_STRIP);
    //glBegin(GL_LINES);
    //glBegin(GL_POINTS);

    float p = 1.0f/num_lines;
    float p_ = 1-p;

    if (length > 1.0f) length = 1.0f;
    if (length < 0.01f) length = 0.01f;
    int num = (int)((float)BUFF_LEN * length);
    float fnum = num;
    int i_ofs = offs - num;
    if (i_ofs < 0) i_ofs += BUFF_LEN;
    //int num2 = num / 2;
    glColor4f(color0[0]*p_ + p * color1[0],
              color0[1]*p_ + p * color1[1],
              color0[2]*p_ + p * color1[2],
              1.0f);//(j+1) / (float)BUFF_LEN * color0[3]);
    //timer.start();
    //float t_acc = 0.0f;
    for(int j = 0; j < num; j++) {
      //printf("a %f\n",color0[0]*p_ + p * color1[0]);
    	//printf("%d\n",__LINE__);
      int k = (i_ofs + j) % (BUFF_LEN-1);
      //if (k >= 0)
      //if (k < BUFF_LEN)
      //vsx_vector d = oldPos[1][k] - oldPos[0][k];

      //timer.start();
      vsx_vector d;
      vsx_vector kk = oldPos[0][k+1] - oldPos[0][k];
      //kk.normalize();
      //oldPos[0][k].normalize();
      if (kk.norm() > 0.000001f)
      {
        d.cross( oldPos[0][k], oldPos[0][k+1] - oldPos[0][k] );
        //float n =
        d.normalize();
        //if (n > 0.01f)
        //{
        //n = 1.0f / n;
        /*d.x *= n;
        d.y *= n;
        d.z *= n;
        }*/
        d = d * width*0.1f;
        float w = 1.0f;
        //t_acc += timer.dtime();

        /*if (j < num2) {
      	w = sin( ((float)j / (float)num2) * 3.14159 / 2.0f);
        } else
        {
      	w = (1.0f-((float)(j-num2) / (float)num2));
      	w = sin ( w * 3.14159 / 2.0f);
        }*/

        vsx_vector res = oldPos[0][k] + d * 0.5f;
        vsx_vector res_a = res + d * w;
        vsx_vector res_b = res - d * w;

/*      glVertex3f(
        oldPos[0][k].x,
        oldPos[0][k].y,
        oldPos[0][k].z);*/
        glColor4f(color0[0], color0[1], color0[2], color0[3]);
        glTexCoord2f(0.0f, (float)j / fnum);
        glVertex3f(res_a.x, res_a.y, res_a.z);
        //vsx_vector res_an = res_a;
        //vsx_vector res_bn = res_b;
        //res_an.normalize();
        //res_bn.normalize();
        vsx_vector res_n = res_a;
        //res_n.x = -res_n.x;
        //res_n.y = -res_n.y;
        //res_n.z = -res_n.z;
        //res_n.cross(res_an, res_bn);
        res_n.normalize();
        glNormal3f(res_n.x, res_n.y, res_n.z);

        glTexCoord2f(1.0f, (float)j / fnum);
        glVertex3f(res_b.x, res_b.y, res_b.z);
        res_n = res_b;
        res_n.normalize();
        glNormal3f(res_n.x, res_n.y, res_n.z);
      }
    }
    //printf("timer: %f\n", t_acc );
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
  if (!oldPos.size()) return;
  //glEnable(GL_LINE_SMOOTH);
  //glLineWidth(1.0f);
  //for(int i = 0; i < num_lines; i++) {
    //glBegin(GL_LINES);
    //glBegin(GL_POINTS);

    //float p = 1.0f/num_lines;

    if (length > 1.0f) length = 1.0f;
    if (length < 0.01f) length = 0.01f;

    // compute eyvec
    // 1. get the modelview matrix
    // 2. invert it
    // 3. multiply by vector [0,0,0,1]
    // 4. profit
    vsx_matrix model_matrix = *modelview_matrix;
    //vsx_matrix inverted_model_matrix;
    //glGetFloatv(GL_MODELVIEW_MATRIX, model_matrix.m);
    //modelview_matrix->dump();
    //model_matrix = *modelview_matrix;
    model_matrix.transpose();//assign_inverse(modelview_matrix);
    vsx_vector eye_pos = model_matrix.multiply_vector(vsx_vector(upvector->x,upvector->y,upvector->z));

    //printf("eye pos: %f, %f, %f\n", eye_pos.x, eye_pos.y, eye_pos.z);

    int num = (int)((float)BUFF_LEN * length);
    float fnum = (float)num;
    int i_ofs = offs - num;
    if (i_ofs < 0) i_ofs += BUFF_LEN;

    //int generated_vertices = 0;
    /*int num2 = num << 1;
    mesh.data->faces.allocate(num2);
    mesh.data->vertices.allocate(num2);
    mesh.data->vertex_normals.allocate(num2);
    mesh.data->vertex_tex_coords.allocate(num2);

    mesh.data->faces.reset_used(num2);
    mesh.data->vertices.reset_used(num2);
    mesh.data->vertex_normals.reset_used(num2);
    mesh.data->vertex_tex_coords.reset_used(num2);
*/
    //vsx_face*      fs_d = &mesh.data->faces[0];
    //vsx_vector*    vs_d = &mesh.data->vertices[0];
    //vsx_vector*    ns_d = &mesh.data->vertex_normals[0];
    //vsx_tex_coord* ts_d = &mesh.data->vertex_tex_coords[0];

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
        //kk.normalize();
        vsx_vector a;
        a.cross( -kk, eye_pos );

        a.normalize();
        //vsx_vector b;
        //b.cross(kk,a);
        //b.normalize();
        vsx_vector d;
        //d.cross(b,kk);
        //d.normalize();

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
// TODO: add faces
        if (loop_counter > 2)
        {
          //printf("generated_vertices_id: %d\n", generated_vertices);
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
    //num2 = generated_vertices << 1;
    //mesh.data->faces.reset_used(num2);
    //mesh.data->vertices.reset_used(num2);
    //mesh.data->vertex_normals.reset_used(num2);
    //mesh.data->vertex_tex_coords.reset_used(num2);

    //printf("timer: %f\n", t_acc );

}
