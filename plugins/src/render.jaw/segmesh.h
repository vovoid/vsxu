/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


class vsx_module_segmesh_shape_basic : public vsx_module {
  vsx_module_param_int* shape_type;
  vsx_module_param_float* sides;
  vsx_module_param_segment_mesh* seg_mesh;
  vsx_module_param_float* size;
  vsx_2dgrid_mesh gmesh;
  float incr;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;segmesh;shape;segmesh_shape_basic";
    info->in_param_spec = "shape_type:enum?sphere|rectangle,sides:float,size:float";
    info->out_param_spec = "segment_mesh:segment_mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    shape_type = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"shape_type");
    size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"size");
    size->set(1);
    sides = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"sides");
    sides->set(16);
    seg_mesh = (vsx_module_param_segment_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"segment_mesh");
    seg_mesh->set_p(gmesh);
  }

  void run()
  {
    //printf("shape type: %d\n",shape_type->get());
    gmesh.reset_vertices();
    float ss = size->get();
    switch (shape_type->get()) {
      case 0:
        {
          incr = pi_float*2.0f/sides->get();
          for (float a = 0; a < sides->get(); ++a) {
            gmesh.vertices[0][(int)a].coord = vsx_vector((float)cos(a*incr)*ss,(float)sin(a*incr)*ss,0);
            gmesh.vertices[0][(int)a].color = vsx_color((float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,0.8f);
            //vsx_color__(0.5,0.5,0.5,0.5);
          }
        }
      break;
    }; // end switch
    //vsx_2dgrid_mesh b = gmesh;
    //printf("base sides: %d\n",gmesh.vertices[0].size());
    seg_mesh->set_p(gmesh);
    loading_done = true;
  }

  void on_delete() {
    gmesh.cleanup();
  }
};





// this should deliver a rotation matrix that is the rotation in a specific point
// on a spline defined by the vertices in bspline_mesh_in.

class vsx_module_segmesh_bspline_matrix : public vsx_module {
  // in
  vsx_module_param_mesh* bspline_vertices_mesh;
  vsx_module_param_float* b_pos;
  // out
  vsx_module_param_matrix* matrix_result;
  vsx_module_param_float3* position;
  // internal
  vsx_bspline spline0;
  vsx_matrix matrix;
  vsx_vector pos1, pos2, e, upv;
public:

  void module_info(vsx_module_info* info) {
    info->identifier = "maths;bspline;bspline_matrix";
    info->in_param_spec = "\
      bspline_vertices_mesh:mesh,\
      b_pos:float";
    info->out_param_spec = "matrix_result:matrix,position:float3";
    info->component_class = "parameters";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    //vsx_mesh foo;
    bspline_vertices_mesh = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"bspline_vertices_mesh");
    //bspline_vertices_mesh->set(foo);
    b_pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b_pos");
    matrix_result = (vsx_module_param_matrix*)out_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"matrix_result");
    matrix_result->set(matrix);
    position = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");
    position->set(0,0);
    position->set(0,1);
    position->set(0,2);
    upv = vsx_vector(0,1);
    loading_done = true;
  }

  void run() {
    vsx_mesh** spline_mesh = bspline_vertices_mesh->get_addr();
    if (spline_mesh)
    {
      spline0.points.set_volatile();
      spline0.points.set_data((*spline_mesh)->data->vertices.get_pointer(),(*spline_mesh)->data->vertices.size());
      //printf("spline_0: %f\n",spline0.p0.x);

    if (!spline0.points.size()) return;
    //printf("spline0.points.size: %d\n",spline0.points.size());
    spline0.set_pos(b_pos->get()+0.1f);
      pos1 = spline0.calc_coord();
      spline0.step(0.1f);
      pos2 = spline0.calc_coord();
      e = pos2-pos1;
      e.normalize();

      /// TODOOOOOOO!
      vsx_quaternion q;
      q.x = e.x * (float)sin(1/2);
      q.y = e.y * (float)sin(1/2);
      q.z = e.z * (float)sin(1/2);
      q.w = (float)cos(1/2);
      q.normalize();

      matrix = q.matrix();

  //    matrix.rotation_from_vectors(&e, &upv);
      //matrix.transpose();

      matrix_result->set(matrix);
      //pos2.dump("pos2");
      position->set(pos2.x,0);
      position->set(pos2.y,1);
      position->set(pos2.z,2);
    }
    loading_done = true;
  }
};

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------


class vsx_module_segmesh_map_bspline : public vsx_module {
  // in
  vsx_module_param_segment_mesh* seg_mesh_in;
  vsx_module_param_mesh* bspline_vertices_mesh;
  vsx_module_param_float* b_pos;
  vsx_module_param_float* length;
  // out
  vsx_module_param_render* render_result;
  vsx_module_param_mesh* mesh_result;
  // internal
  vsx_bspline spline0;
  // the mesh we're gonna put on the path
  vsx_2dgrid_mesh *base_mesh;
  // our building tool
  vsx_2dgrid_mesh gmesh;

  int i,j;
  float stime;
  float sides;
  int num;

  // the step for each spline segment - defined as number of length/segments
  float spline_step;

  vsx_vector old;
  vsx_vector e;
  vsx_vector f;
  vsx_vector d;
  vsx_matrix ma;
  vsx_vector old_vec;
  vsx_vector spos;
  vsx_vector upv;
  // the resulting mesh
  vsx_mesh* result_mesh;
public:

  bool init()
  {
    result_mesh = new vsx_mesh;
    return true;
  }
  void on_delete()
  {
    delete result_mesh;
    gmesh.cleanup();
  }


  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;segmesh;map_segmesh_bspline";
    info->in_param_spec = ""
      "seg_mesh_in:segment_mesh,"
      "bspline_vertices_mesh:mesh,"
      "b_pos:float,"
      "length:float";
    info->out_param_spec = "mesh_result:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    sides = 8;
    stime = 0.0f;

    seg_mesh_in = (vsx_module_param_segment_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"seg_mesh_in");
    //seg_mesh_in->set(gmesh);

    bspline_vertices_mesh = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"bspline_vertices_mesh");
    //vsx_mesh foomesh;
    //bspline_vertices_mesh->set(foomesh);

    b_pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"b_pos");
    b_pos->set(1);

    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"length");
    length->set(0.1f);

    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_result");
    mesh_result->set_p(result_mesh);

    spline0.init(vsx_vector(0), 0.7f, 0.3f, 0.6f);
    spline0.points[0] = vsx_vector(0);

    upv = vsx_vector(0,1);
  }

  void run() {
    //if (mesh.data->faces.get_used()) return;
    //printf("vsx_module_segmesh_map_bspline::output()\n");

    base_mesh = seg_mesh_in->get_addr();
    if (base_mesh) {
      //printf("base_mesh is ok\n");
      vsx_mesh** spline_mesh = bspline_vertices_mesh->get_addr();
      if (spline_mesh) {
        stime = b_pos->get();
        // a bad pointer hack, but it works :)
        // we're only reading anyway..
        spline0.points.set_volatile();
        spline0.points.set_data((*spline_mesh)->data->vertices.get_pointer(),(*spline_mesh)->data->vertices.size());
        if (!spline0.points.size()) return;
        spline0.set_pos(stime);

        num = base_mesh->vertices.size();
        //printf("segments: %d\n",num);
        sides = base_mesh->vertices[0].size();
        spline_step = length->get()/((float)num+1.0f);
        //printf("running bspline\n");


        //spline0.points[14].x = sin(engine->real_vtime);
        //printf("adjusting spline\n");

        //spline0.step(1);
        //spos = spline0.calc_coord();

        //gmesh.vertices[0][0].coord = vsx_vector__(-2*s,-1*s,0);
        //gmesh.vertices[0][1].coord = vsx_vector__(2*s,-1*s,0);
        //gmesh.vertices[0][2].coord = vsx_vector__(2*s,1*s,0);
        //gmesh.vertices[0][3].coord = vsx_vector__(-2*s,1*s,0);

        //spline0.step(0.05);
        //old_vec = spline0.calc_coord();

        //loft_x_seq.set_time(0);

        //printf("bspline num: %d\n",num);
        //-----


        spline0.step(spline_step);
        spos = spline0.calc_coord();
        e = spos-old;
        //f = e + old_vec;
        e.normalize();
        //ma.rotation_from_vectors(&e, &upv);
        //old_vec = e;
        old = spos;

        //printf("num: %d  stime: %f\n",num,stime);

        for (i = 0; i < num; ++i) {
          //printf("i = %d\n",i);
          spline0.step(spline_step);
          //spline0.set_pos(stime+(float)i*spline_step);
          spos = spline0.calc_coord();
          //spos.x *= f_size->get(0);
          //spos.y *= f_size->get(1);
          //spos.z *= f_size->get(2);

          e = spos-old; //vsx_vector__(0,0.5,1.4);
          //f = e + old_vec;
          e.normalize();
          //d = d + e*0.4;

          ma.rotation_from_vectors(&e, &upv);
          //old_vec = e;
          old = spos;

          if (i == 0) {
            //spos.dump("spos");

            //printf("spline_0: %f\n",spline0.p0.x);
          }

      //    gmesh.vertices[i+1][0].coord = gmesh.vertices[0][0].coord+spos;
      //    gmesh.vertices[i+1][1].coord = gmesh.vertices[0][1].coord+spos;
      //    gmesh.vertices[i+1][2].coord = gmesh.vertices[0][2].coord+spos;
      //    gmesh.vertices[i+1][3].coord = gmesh.vertices[0][3].coord+spos;
          //printf("s: %f\n",s);
          for (j = 0; j < (int)sides; ++j) {
            //size_c.x = gmesh.vertices[0][j].coord.x * sx;
            //size_c.y = gmesh.vertices[0][j].coord.y * sy;
            //size_c.z = 0;
            //gmesh.vertices[i][j].coord = base_mesh->vertices[i][j].coord+spos;//ma.multiply_vector(base_mesh->vertices[i][j].coord)+spos;
            gmesh.vertices[i][j].coord = ma.multiply_vector(base_mesh->vertices[i][j].coord)+spos;
            gmesh.vertices[i][j].color = vsx_color__(0.5f,0.5f,0.5f,1.0f);
            //base_mesh->vertices[i][j].color;
      ///      gmesh.vertices[i+1][j].coord = ma.multiply_vector(base_mesh->vertices[i][j].coord)+spos;
          }
          //gmesh.vertices[i+1][0].coord = ma.multiply_vector(gmesh.vertices[0][0].coord)+spos;
          //gmesh.vertices[i+1][1].coord = ma.multiply_vector(gmesh.vertices[0][1].coord)+spos;
          //gmesh.vertices[i+1][2].coord = ma.multiply_vector(gmesh.vertices[0][2].coord)+spos;
          //gmesh.vertices[i+1][3].coord = ma.multiply_vector(gmesh.vertices[0][3].coord)+spos;

          //gmesh.vertices[i+1][0].color = vsx_color__(1,1,1,0.2);
          //gmesh.vertices[i+1][1].color = vsx_color__(1,1,1,0.2);
          //gmesh.vertices[i+1][2].color = vsx_color__(1,1,1,0.2);
          //gmesh.vertices[i+1][3].color = vsx_color__(1,1,1,0.2);

          //gmesh.vertices[i+1][0].coord = ma.multiply_vector(gmesh.vertices[0][0].coord)+d;
          //gmesh.vertices[i+1][1].coord = ma.multiply_vector(gmesh.vertices[0][1].coord)+d;
          //gmesh.vertices[i+1][2].coord = ma.multiply_vector(gmesh.vertices[0][2].coord)+d;
          //gmesh.vertices[i+1][3].coord = ma.multiply_vector(gmesh.vertices[0][3].coord)+d;

        }
        // colors
      /*  if (first_run) {
          for (i = 0; i < num; ++i) {
            for (j = 0; j < (int)sides; ++j) {
              gmesh.vertices[i+1][j].color = vsx_color__(1,1,1,0.2);
            }
          }
        }*/

        /*if (first_run) {
          for (i = 0; i < num; ++i) {
            for (j = 0; j < (int)sides; ++j) {
              gmesh.vertices[i][j].color = vsx_color__((float)(rand()%1000)*0.001,(float)(rand()%1000)*0.001,(float)(rand()%1000)*0.001,0.8);
            }
          }
        }*/

        //printf("efter vertexarna\n");
        //printf("vert size: %d\n",gmesh.vertices.size());
        if (!gmesh.faces.size()) {
          for (int i = 0;  i < num-2; ++i) {
          //for (int i = 1;  i < num-1; ++i) {
            //printf("i1: %d\n",i);
            for (int j = 0; j < (int)sides; ++j) {
              int j1 = j+1;
              //printf("h1: %d\n",j);
              if (j1 == (int)sides) j1 = 0;
              //gmesh.add_face(i+2,j, i+2,j, i+1,j1);
              //gmesh.add_face(i+1,j, i+1,j1, i+2,j1);
              gmesh.add_face(i+1,j1, i+1,j, i,j);
              gmesh.add_face(i,j, i,j1, i+1,j1);
            }

            //gmesh.add_face(i+1,1, i+2,1, i+2,2);
            //gmesh.add_face(i+1,1, i+1,2, i+2,2);

            //gmesh.add_face(i+1,2, i+2,2, i+2,3);
            //gmesh.add_face(i+1,2, i+1,3, i+2,3);

            //gmesh.add_face(i+1,3, i+2,3, i+2,0);
            //gmesh.add_face(i+1,3, i+1,0, i+2,0);

            //gmesh.add_face(i+2,1, i+1,2, i+1,1);
            //gmesh.add_face(i+2,1, i+2,2, i+1,2);

            //gmesh.add_face(i+2,3, i+1,3, i+1,2);
            //gmesh.add_face(i+2,3, i+1,2, i+2,2);

            //gmesh.add_face(i+1,0, i+1,3, i+2,3);
            //gmesh.add_face(i+1,0, i+2,3, i+2,0);

            //gmesh.add_face(i+1,1, i+1,2, i+1,3);
      //      gmesh.add_face(i+1,1, i+1,3, i+1,0);
      //      gmesh.add_face(i+1,1, i+1,2, i+1,3);
          }
          //gmesh.calculate_face_normals();
        }
        //printf("num faces: %d|",gmesh.faces.size());
        gmesh.calculate_face_normals();

        //

        /*spline0.step(0.4);
        spos = spline0.calc_coord();

        e = spos;//vsx_vector__(0,-0.3,1.2);
        d = e;
        e.normalize();
        ma.rotation_from_vectors(&e, &upv);

        gmesh.vertices[2][0].coord = ma.multiply_vector(gmesh.vertices[0][0].coord)+d;
        gmesh.vertices[2][1].coord = ma.multiply_vector(gmesh.vertices[0][1].coord)+d;
        gmesh.vertices[2][2].coord = ma.multiply_vector(gmesh.vertices[0][2].coord)+d;
        gmesh.vertices[2][3].coord = ma.multiply_vector(gmesh.vertices[0][3].coord)+d;
        */

        //gmesh.add_face(0,1, 0,3, 0,0);
        //gmesh.add_face(0,1, 0,2, 0,3);

        //gmesh.add_face(1,1, 1,3, 1,0);
        //gmesh.add_face(1,1, 1,2, 1,3);

        //gmesh.add_face(2,1, 2,3, 2,0);
        //gmesh.add_face(2,1, 2,2, 2,3);

        gmesh.calculate_vertex_normals();

        gmesh.dump_vsx_mesh(result_mesh);
        mesh_result->set_p(result_mesh);
        loading_done = true;
        return;
      }
      result_mesh->data->clear();
      mesh_result->set_p(result_mesh);
      loading_done = true;
      return;
    }
    result_mesh->data->clear();
    mesh_result->set_p(result_mesh);
    loading_done = true;
    //mesh.data->calculate_face_centers();
  }
};

