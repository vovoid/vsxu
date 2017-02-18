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


// mesh inflation by CoR
// optimized 2010-01 by jaw

#define printVector(a,b,c)
#define printFloat(a,b,c)

class module_mesh_inflate_thread : public vsx_module
{
public:
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

  // out
  vsx_module_param_mesh* mesh_out;
  vsx_module_param_float* volume_out;

  // internal
  vsx_mesh<>* mesh;
  vsx_ma_vector< vsx_vector3<> > faceLengths;
  vsx_ma_vector<float> faceAreas;
  vsx_ma_vector< vsx_vector3<> > verticesSpeed;

  bool debug;
  bool init()
  {
    mesh = new vsx_mesh<>;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

  void _printVector(const char* name, int faceInd, vsx_vector3<> vec) {
    if(debug) printf("%d %s: %f %f %f \n", faceInd, name, vec.x, vec.y, vec.z);
  }

  void _printFloat(const char* name, int faceInd, float f) {
    if(debug) printf("%d %s: %f \n", faceInd, name, f);
  }


  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;modifiers;deformers;mesh_inflate";

    info->description =
      "Inflates a mesh";

    info->in_param_spec =
      "mesh_in:mesh,"
      "steps_per_second:float,"
      "step_size:float,"
      "gas_amount:float,"
      "gas_expansion_factor:float,"
      "grid_stiffness_factor:float,"
      "damping_factor:float,"
      "material_weight:float,"
      "lower_boundary:float"
    ;

    info->out_param_spec =
      "mesh_out:mesh,volume_out:float";

    info->component_class =
      "mesh";
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
    volume_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"volume_out");
    volume_out->set(0.0f);
    prev_timestamp = 0xFFFFFFFF;
  }

  unsigned long prev_timestamp;
  vsx_vector3<> v;
  float dtimeRest;

  void run()
  {
    vsx_mesh<>** p = mesh_in->get_addr();
    if (!p)
    {
      return;
    }

    debug = false;
    bool newMeshLoaded = false;


    //after a mesh change clone the mesh
    if (p && (prev_timestamp != (*p)->timestamp)) {
      prev_timestamp = (*p)->timestamp;
      mesh->data->vertices.reset_used(0);
      mesh->data->vertex_normals.reset_used(0);
      mesh->data->vertex_tex_coords.reset_used(0);
      mesh->data->vertex_colors.reset_used(0);
      mesh->data->faces.reset_used(0);

      for (unsigned int i = 0; i < (*p)->data->vertices.size(); i++)
      {
        mesh->data->vertices[i] = (*p)->data->vertices[i] + v;
        verticesSpeed[i] = vsx_vector3<>(0, 0, 0);
      }

      for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_tangents.size(); i++) mesh->data->vertex_tangents[i] = (*p)->data->vertex_tangents[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_tex_coords.size(); i++) mesh->data->vertex_tex_coords[i] = (*p)->data->vertex_tex_coords[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_normals.size(); i++) mesh->data->vertex_normals[i] = (*p)->data->vertex_normals[i];
      for (unsigned int i = 0; i < (*p)->data->vertex_colors.size(); i++) mesh->data->vertex_colors[i] = (*p)->data->vertex_colors[i];
      for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) mesh->data->faces[i] = (*p)->data->faces[i];

      //calc and store original face lengths
      faceLengths.reset_used();
      vsx_vector3<> normal;
      vsx_vector3<> len;
      for (unsigned int i = 0; i < (*p)->data->faces.size(); i++) {
        vsx_face3& f = mesh->data->faces[i];
        vsx_vector3<>& v0 = mesh->data->vertices[f.a];
        vsx_vector3<>& v1 = mesh->data->vertices[f.b];
        vsx_vector3<>& v2 = mesh->data->vertices[f.c];
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
    }

    float stepSize = step_size->get();
    //float stepsPerSecond = steps_per_second->get();
    float gasExpansionFactor = gas_expansion_factor->get();
    float gridStiffnessFactor = grid_stiffness_factor->get();
    float dampingFactor =  damping_factor->get();
    float materialWeight = material_weight->get();
    float lowerBoundary = lower_boundary->get();

    //use engine_state->dtime; and dtimeRest
    //to repeat the calculation several times ((dtime + rest) * stepsPerSecond)


    //calculate volume
    float volume = 0.0f;
    vsx_face3* face_p = mesh->data->faces.get_pointer();
    vsx_vector3<>* vertex_p = mesh->data->vertices.get_pointer();
    vsx_vector3<>* faces_length_p = faceLengths.get_pointer();

    verticesSpeed.allocate(mesh->data->vertices.size());
    vsx_vector3<>* vertices_speed_p = verticesSpeed.get_pointer();

    float onedivsix = (1.0f / 6.0f);
    for(unsigned int i = 0; i < mesh->data->faces.size(); i++) {
      vsx_face3& f = face_p[i];
      vsx_vector3<>& v0 = vertex_p[f.a];
      vsx_vector3<>& v2 = vertex_p[f.b];
      vsx_vector3<>& v1 = vertex_p[f.c];

      volume += (v0.x * (v1.y - v2.y) +
           v1.x * (v2.y - v0.y) +
           v2.x * (v0.y - v1.y)) * (v0.z + v1.z + v2.z) * onedivsix;

    }

    //default gas_amount to volume of a new mesh i.e. no pressure
    if(newMeshLoaded) {
      gas_amount->set(volume);
    }
    float pressure = (gas_amount->get() - volume) / volume;


    //calculate face areas, normals, forces and add to speed
    for(unsigned int i = 0; i < mesh->data->faces.size(); i++) {
      vsx_face3& f = face_p[i];
      vsx_vector3<>& v0 = vertex_p[f.a];
      vsx_vector3<>& v1 = vertex_p[f.b];
      vsx_vector3<>& v2 = vertex_p[f.c];

              printVector("v0", i, v0);
              printVector("v1", i, v1);
              printVector("v2", i, v2);

      vsx_vector3<> a = vertex_p[face_p[i].b] - vertex_p[face_p[i].a];
      vsx_vector3<> b = vertex_p[face_p[i].c] - vertex_p[face_p[i].a];
      vsx_vector3<> normal;
      normal.cross(a,b);



              printVector("normal", i, normal);

              printFloat("length", i, len);
              printFloat("area", i, len);

      vsx_vector3<> edgeA = (v1 - v0);
      vsx_vector3<> edgeB = (v2 - v1);
      vsx_vector3<> edgeC = (v0 - v2);

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

      vsx_vector3<> accA = edgeA * edgeAccA;
      vsx_vector3<> accB = edgeB * edgeAccB;
      vsx_vector3<> accC = edgeC * edgeAccC;

              printVector("accA", i, accA);
              printVector("accB", i, accB);
              printVector("accC", i, accC);

      vertices_speed_p[f.a] -= (accA - accC) * gridStiffnessFactor;
      vertices_speed_p[f.b] -= (accB - accA) * gridStiffnessFactor;
      vertices_speed_p[f.c] -= (accC - accB) * gridStiffnessFactor;

      //applying pressure to areas of faces
      vsx_vector3<> pressureAcc = normal * pressure * gasExpansionFactor;
      vertices_speed_p[f.a] -= pressureAcc;
      vertices_speed_p[f.b] -= pressureAcc;
      vertices_speed_p[f.c] -= pressureAcc;

      //apply material weight
      vertices_speed_p[f.a].y -= materialWeight;
      vertices_speed_p[f.b].y -= materialWeight;
      vertices_speed_p[f.c].y -= materialWeight;

    }

    //apply speeds to vertices
    for(unsigned int i = 0; i < mesh->data->vertices.size(); i++)
    {
      vertex_p[i] += vertices_speed_p[i] * stepSize;
      if(vertex_p[i].y < lowerBoundary) {
        vertex_p[i].y = lowerBoundary;
      }
      vertices_speed_p[i] = vertices_speed_p[i] * dampingFactor;
    }


    mesh->data->vertex_normals.allocate(mesh->data->vertices.size());
    mesh->data->vertex_normals.memory_clear();
    vsx_vector3<>* vertex_normals_p = mesh->data->vertex_normals.get_pointer();

    //TODO: create vertex normals, for rendering... should be a separate module...
    for(unsigned int i = 0; i < mesh->data->faces.size(); i++)
    {
      vsx_vector3<> a = vertex_p[face_p[i].b] - vertex_p[face_p[i].a];
      vsx_vector3<> b = vertex_p[face_p[i].c] - vertex_p[face_p[i].a];
      vsx_vector3<> normal;
      normal.cross(a,b);

      normal = -normal;
      normal.normalize();
      vertex_normals_p[face_p[i].a] += normal;
      vertex_normals_p[face_p[i].b] += normal;
      vertex_normals_p[face_p[i].c] += normal;
    }

    volume_out->set(volume);

    //printf("***************Pressure %f     ", pressure);
    //printf(" Volume %f\n", volume);

    mesh_out->set_p(mesh);
  }
};

#undef printVector
#undef printFloat
