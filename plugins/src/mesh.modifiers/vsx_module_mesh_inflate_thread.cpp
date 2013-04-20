/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#define printVector(a,b,c)
#define printFloat(a,b,c)

class vsx_module_mesh_inflate : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float* steps_per_second;
  vsx_module_param_float* step_size;
  vsx_module_param_float* gas_amount;
  vsx_module_param_float* gas_expansion_factor;
  vsx_module_param_float* grid_stiffness_factor;
  vsx_module_param_float* damping_factor;
  vsx_module_param_float* material_weight;
  vsx_module_param_float* lower_boundary;
  vsx_module_param_int* use_thread;
  // out
  vsx_module_param_mesh* mesh_out;
  vsx_module_param_float* volume_out;
  // internal
  vsx_mesh mesh_a;
  vsx_mesh mesh_b;
  vsx_mesh_data* source_data;
  vsx_mesh_data* dest_data;
  vsx_mesh* mesh;

  vsx_array<vsx_vector> faceLengths;
  vsx_array<float> faceAreas;
  vsx_array<vsx_vector> verticesSpeed;


  pthread_t         worker_t;
  pthread_attr_t    worker_t_attr;
  
  int p_updates;

  int thread_state;
  bool debug;
  bool newMeshLoaded;

  
  
  static void* worker(void *ptr) {
    vsx_module_mesh_inflate* my = (vsx_module_mesh_inflate*)ptr;
    my->debug = false;
    


    float stepSize = my->step_size->get();
    //float stepsPerSecond = steps_per_second->get();
    float gasExpansionFactor = my->gas_expansion_factor->get();
    float gridStiffnessFactor = my->grid_stiffness_factor->get();
    float dampingFactor =  my->damping_factor->get();
    float materialWeight = my->material_weight->get();
    float lowerBoundary = my->lower_boundary->get();

    //use engine->dtime; and dtimeRest
    //to repeat the calculation several times ((dtime + rest) * stepsPerSecond)



    //calculate volume
    float volume = 0.0f;
    vsx_face* face_p = my->source_data->faces.get_pointer();
    vsx_vector* vertex_p = my->source_data->vertices.get_pointer();
    vsx_vector* faces_length_p = my->faceLengths.get_pointer();

    my->verticesSpeed.allocate(my->source_data->vertices.size());
    vsx_vector* vertices_speed_p = my->verticesSpeed.get_pointer();

    float onedivsix = 0.16666666666f;
    for(unsigned int i = 0; i < my->source_data->faces.size(); i++) {
      vsx_face& f = face_p[i];//mesh.data->faces[i];
      vsx_vector& v0 = vertex_p[f.a];
      vsx_vector& v2 = vertex_p[f.b];
      vsx_vector& v1 = vertex_p[f.c];

      volume += (v0.x * (v1.y - v2.y) +
           v1.x * (v2.y - v0.y) +
           v2.x * (v0.y - v1.y)) * (v0.z + v1.z + v2.z) * onedivsix;

    }

    //default gas_amount to volume of a new mesh i.e. no pressure
    if(my->newMeshLoaded) {
      my->gas_amount->set(volume);
    }
    float pressure = (my->gas_amount->get() - volume) / volume;

    //mesh.data->face_normals.reset_used(0);
    //mesh.data->vertex_normals.reset_used(0);


    //calculate face areas, normals, forces and add to speed
    for(unsigned int i = 0; i < my->source_data->faces.size(); i++) {
      vsx_face& f = face_p[i];//mesh.data->faces[i];
      vsx_vector& v0 = vertex_p[f.a];//mesh.data->vertices[f.a];
      vsx_vector& v1 = vertex_p[f.b];//mesh.data->vertices[f.b];
      vsx_vector& v2 = vertex_p[f.c];//mesh.data->vertices[f.c];

              printVector("v0", i, v0);
              printVector("v1", i, v1);
              printVector("v2", i, v2);

      //vsx_vector normal = mesh.data->get_face_normal(i);
      vsx_vector a = vertex_p[face_p[i].b] - vertex_p[face_p[i].a];
      vsx_vector b = vertex_p[face_p[i].c] - vertex_p[face_p[i].a];
      vsx_vector normal;
      normal.cross(a,b);



              printVector("normal", i, normal);

      //float len = normal.length();
      //float area = len / 2;

              printFloat("length", i, len);
              printFloat("area", i, len);

      vsx_vector edgeA = (v1 - v0);
      vsx_vector edgeB = (v2 - v1);
      vsx_vector edgeC = (v0 - v2);

              printVector("edgeA", i, edgeA);
              printVector("edgeB", i, edgeB);
              printVector("edgeC", i, edgeC);

      float lenA = edgeA.length();
      float lenB = edgeB.length();
      float lenC = edgeC.length();

              printFloat("lenA", i, lenA);
              printFloat("lenB", i, lenB);
              printFloat("lenC", i, lenC);


      float edgeForceA = (faces_length_p[i].x - lenA) / faces_length_p[i].x;
      float edgeForceB = (faces_length_p[i].y - lenB) / faces_length_p[i].y;
      float edgeForceC = (faces_length_p[i].z - lenC) / faces_length_p[i].z;

              printFloat("edgeForceA", i, edgeForceA);
              printFloat("edgeForceB", i, edgeForceB);
              printFloat("edgeForceC", i, edgeForceC);

      float edgeAccA = edgeForceA / lenA;
      float edgeAccB = edgeForceB / lenB;
      float edgeAccC = edgeForceC / lenC;

              printFloat("edgeAccA", i, edgeAccA);
              printFloat("edgeAccB", i, edgeAccB);
              printFloat("edgeAccC", i, edgeAccC);

      vsx_vector accA = edgeA * edgeAccA;
      vsx_vector accB = edgeB * edgeAccB;
      vsx_vector accC = edgeC * edgeAccC;

              printVector("accA", i, accA);
              printVector("accB", i, accB);
              printVector("accC", i, accC);

      vertices_speed_p[f.a] -= (accA - accC) * gridStiffnessFactor;
      vertices_speed_p[f.b] -= (accB - accA) * gridStiffnessFactor;
      vertices_speed_p[f.c] -= (accC - accB) * gridStiffnessFactor;

      //applying pressure to areas of faces
      vsx_vector pressureAcc = normal * pressure * gasExpansionFactor;
      vertices_speed_p[f.a] -= pressureAcc;
      vertices_speed_p[f.b] -= pressureAcc;
      vertices_speed_p[f.c] -= pressureAcc;

      //apply material weight
      float gravityAcc = materialWeight;
      vertices_speed_p[f.a].y -= gravityAcc;
      vertices_speed_p[f.b].y -= gravityAcc;
      vertices_speed_p[f.c].y -= gravityAcc;

    }

    vsx_vector* vertex_dest_p = my->dest_data->vertices.get_pointer();
    //apply speeds to vertices
    for(unsigned int i = 0; i < my->source_data->vertices.size(); i++) {
      vertex_dest_p[i] = vertex_p[i]+vertices_speed_p[i] * stepSize;
      if(vertex_dest_p[i].y < lowerBoundary) {
        vertex_dest_p[i].y = lowerBoundary;
      }
      vertices_speed_p[i] = vertices_speed_p[i] * dampingFactor;
    }


    my->dest_data->vertex_normals.allocate(my->source_data->vertices.size());
    my->dest_data->vertex_normals.memory_clear();
    vsx_vector* vertex_normals_p = my->dest_data->vertex_normals.get_pointer();

    for(unsigned int i = 0; i < my->source_data->faces.size(); i++)
    {
      vsx_vector a = vertex_dest_p[face_p[i].b] - vertex_dest_p[face_p[i].a];
      vsx_vector b = vertex_dest_p[face_p[i].c] - vertex_dest_p[face_p[i].a];
      vsx_vector normal;
      normal.cross(a,b);

      //vsx_vector normal = mesh.data->get_face_normal(i);
      normal = -normal;
      normal.normalize();
      vertex_normals_p[face_p[i].a] += normal;
      vertex_normals_p[face_p[i].b] += normal;
      vertex_normals_p[face_p[i].c] += normal;
    }
    my->volume_out->set(volume);

    my->thread_state = 2;
    return 0;
  }

  
  
  
public:
  void _printVector(const char* name, int faceInd, vsx_vector vec) {
    if(debug) printf("%d %s: %f %f %f \n", faceInd, name, vec.x, vec.y, vec.z);
  }
  void _printFloat(const char* name, int faceInd, float f) {
    if(debug) printf("%d %s: %f \n", faceInd, name, f);
  }


  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;modifiers;deformers;mesh_inflate";
    info->description = "Inflates a mesh";
    info->in_param_spec = "mesh_in:mesh,"
              "steps_per_second:float,"
              "step_size:float,"
              "gas_amount:float,"
              "gas_expansion_factor:float,"
              "grid_stiffness_factor:float,"
              "damping_factor:float,"
              "material_weight:float,"
              "lower_boundary:float,"
              "use_thread:enum?no|yes";

    info->out_param_spec = "mesh_out:mesh,volume_out:float";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");

    steps_per_second = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "steps_per_second");
    step_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_size");
    gas_amount = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "gas_amount");
    gas_expansion_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "gas_expansion_factor");
    grid_stiffness_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "grid_stiffness_factor");
    damping_factor = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "damping_factor");
    material_weight = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "material_weight");
    lower_boundary = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "lower_boundary");

    use_thread = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_thread");
    use_thread->set(0);


    steps_per_second->set(100.0f);
    step_size->set(0.01f);
    gas_amount->set(1.0f);
    gas_expansion_factor->set(1.0f);
    grid_stiffness_factor->set(1.0f);
    damping_factor->set(0.98f);
    material_weight->set(0.00f);
    lower_boundary->set(-150.00f);


    loading_done = true;
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    mesh_out->set_p(mesh_a);
    volume_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"volume_out");
    volume_out->set(0.0f);
    newMeshLoaded = false;
    p_updates = -1;
  }

  unsigned long prev_timestamp;
  vsx_vector v;
  float dtimeRest;

  void run() {
    vsx_mesh* p = mesh_in->get_addr();
    if (!p)
    {
      mesh_empty.timestamp = (int)(engine->real_vtime*1000.0f);
      mesh_out->set_p(mesh_empty);
      prev_timestamp = 0xFFFFFFFF;
      return;
    }

    if (thread_state == 1)
    while (thread_state != 2) {}

    //after a mesh change clone the mesh
    if (p && (prev_timestamp != p->timestamp)) {
      mesh = &mesh_a;
      prev_timestamp = p->timestamp;
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      for (unsigned int i = 0; i < p->data->vertices.size(); i++)
      {
        mesh->data->vertices[i] = p->data->vertices[i] + v;
        verticesSpeed[i] = vsx_vector(0, 0, 0);
      }

      for (unsigned int i = 0; i < p->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = p->data->vertex_normals[i];
      for (unsigned int i = 0; i < p->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = p->data->vertex_tangents[i];
      for (unsigned int i = 0; i < p->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = p->data->vertex_tex_coords[i];
      for (unsigned int i = 0; i < p->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = p->data->vertex_colors[i];
      for (unsigned int i = 0; i < p->data->faces.size(); i++) mesh->data->faces[i] = p->data->faces[i];

      mesh_b.data->vertices.allocate(p->data->vertices.size());
      mesh_b.data->vertex_normals.allocate(p->data->vertex_normals.size());
      mesh_b.data->vertex_tangents.allocate(p->data->vertex_tangents.size());
      mesh_b.data->vertex_tex_coords.allocate(p->data->vertex_tex_coords.size());
      mesh_b.data->vertex_colors.allocate(p->data->vertex_colors.size());
      mesh_b.data->faces.allocate(p->data->vertex_colors.size());


      //calc and store original face lengths
      faceLengths.reset_used();
      vsx_vector normal;
      vsx_vector len;
      vsx_vector hypVec;
      for (unsigned int i = 0; i < p->data->faces.size(); i++) {
        vsx_face& f = mesh->data->faces[i];
        vsx_vector& v0 = mesh->data->vertices[f.a];
        vsx_vector& v1 = mesh->data->vertices[f.b];
        vsx_vector& v2 = mesh->data->vertices[f.c];
        //calc face area
        normal.assign_face_normal(&v0, &v1, &v2);
        float area = normal.length() / 2.0f;
        faceAreas[i] = area;
        //facelengths a, b, c stored in vector x, y, z
        len.x = (v1 - v0).length();
        len.y = (v2 - v1).length();
        len.z = (v0 - v2).length();
        faceLengths.push_back(len);
      }
      mesh->timestamp++;
      param_updates = 0;

      newMeshLoaded = true;
      dtimeRest = 0.0f;
      thread_state = 0;
    }




    // this concept assumes that the run takes shorter than the framerate to do
    if (thread_state == 2) { // thread is done
      // no thread running


      mesh->timestamp++;
      mesh_out->set_p(*mesh);

      // toggle to the other mesh
      if (mesh == &mesh_a) {
        mesh = &mesh_b;
        source_data = mesh_a.data;
        dest_data = mesh_b.data;
      }
      else {
        mesh = &mesh_a;
        source_data = mesh_b.data;
        dest_data = mesh_a.data;
      }
      // the one we point to now is the one that is going to be worked on
      thread_state = 3;
    }

    if ( (thread_state == 3 || thread_state == 0) && p_updates != param_updates) {
      p_updates = param_updates;

      if (0 == use_thread->get())
      {
        worker((void*)this);
      } else
      {
        //printf("creating thread\n");
        pthread_attr_init(&worker_t_attr);
        //pthread_attr_setdetachstate(&worker_t_attr, PTHREAD_CREATE_JOINABLE);

        thread_state = 1;

        pthread_create(&worker_t, &worker_t_attr, &worker, (void*)this);
        pthread_detach(worker_t);
      }
    }
  }
};

#undef printVector
#undef printFloat

