/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
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


#ifndef VSX_API_MESH_H
#define VSX_API_MESH_H

#include <container/vsx_ma_vector.h>
#include <math/quaternion/vsx_quaternion.h>
#include <math/vector/vsx_vector3.h>
#include <color/vsx_color.h>
#include <graphics/face/vsx_face1.h>
#include <graphics/face/vsx_face2.h>
#include <graphics/face/vsx_face3.h>
#include <graphics/face/vsx_face4.h>
#include <graphics/vsx_texcoord.h>

// the mesh contains vertices stored in a local coordinate system.
template<typename T = float>
class vsx_mesh_data {
public:
  // vertices
  vsx_ma_vector< vsx_vector3<T> > vertices;
  vsx_ma_vector< vsx_vector3<T> > vertex_normals;
  vsx_ma_vector< vsx_color<T> > vertex_colors;
  vsx_ma_vector< vsx_tex_coord2f > vertex_tex_coords;
  vsx_ma_vector< vsx_face3 > faces;
  vsx_ma_vector< vsx_vector3<T> > face_normals;
  vsx_ma_vector< vsx_quaternion<T> > vertex_tangents; // tangent space, for normal mapping
  vsx_ma_vector< vsx_vector3<T> > face_centers; // centers of the faces - the average of each face's v1+v2+v3/3
  // selected vertices, whom wich should be modified when run through a mesh deformer that modifies the
  vsx_ma_vector<unsigned long>* selected_vertices;

  void calculate_face_centers() {
    if (!faces.size()) return;
    for (unsigned long i = 0; i < faces.size(); ++i) {
      face_centers[i].x = (vertices[faces[i].a].x+vertices[faces[i].b].x+vertices[faces[i].c].x);
      face_centers[i].y = (vertices[faces[i].a].y+vertices[faces[i].b].y+vertices[faces[i].c].y);
      face_centers[i].z = (vertices[faces[i].a].z+vertices[faces[i].b].z+vertices[faces[i].c].z);
    }
  }
  
  inline vsx_vector3<T> get_face_normal(unsigned long index)
  {
    // counter-clockwise faces
    //  c
    //  ^
    //  |
    //  b<----a
    vsx_vector3<T> a = vertices[faces[index].b] - vertices[faces[index].a];
    vsx_vector3<T> b = vertices[faces[index].c] - vertices[faces[index].a];
    vsx_vector3<T> res;
    res.cross(a,b);
    return res;
  }

  void reset() {
    vertices.reset_used();
    vertex_normals.reset_used();
    vertex_colors.reset_used();
    vertex_tex_coords.reset_used();
    faces.reset_used();
    face_normals.reset_used();
    face_centers.reset_used();
  }
  vsx_mesh_data() {
    selected_vertices = 0;
  }
  
  void clear() {
    vertices.clear();
    vertex_normals.clear(); 
    vertex_colors.clear(); 
    vertex_tex_coords.clear();
    faces.clear(); 
    face_normals.clear(); 
    face_centers.clear();
  }

  ~vsx_mesh_data() {
    clear();
  }
};

template<typename T = float>
class vsx_mesh {
public:
  unsigned long timestamp;
  vsx_mesh_data<T>* data;
  vsx_mesh() {
    data = new vsx_mesh_data<T>;
    timestamp = rand();
  }
  ~vsx_mesh() {
    delete (vsx_mesh_data<T>*)data;
  }
};

#endif
