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

#include "vsx_array.h"
#include "vsx_quaternion.h"

#ifndef __gl_h_
typedef unsigned int GLuint;
#endif

typedef struct {
#ifdef VSXU_OPENGL_ES
  GLuint a,b,c;
#else
	GLuint a,b,c;
#endif
} vsx_face;

typedef struct {
  GLfloat s,t;
} vsx_tex_coord;

inline vsx_tex_coord vsx_tex_coord__(float x, float y) {
  vsx_tex_coord a;
  a.s = x;
  a.t = y;
  return a;
}

// the mesh contains vertices stored in a local coordinate system.
#ifndef VSX_NO_MESH
class vsx_mesh_data {
public:
  // vertices
  vsx_array<vsx_vector> vertices;
  vsx_array<vsx_vector> vertex_normals;
  vsx_array<vsx_color> vertex_colors;
  vsx_array<vsx_tex_coord> vertex_tex_coords;
  vsx_array<vsx_face> faces;
  vsx_array<vsx_vector> face_normals;
  vsx_array<vsx_quaternion> vertex_tangents; // tangent space, for normal mapping
  vsx_array<vsx_vector> face_centers; // centers of the faces - the average of each face's v1+v2+v3/3
  // selected vertices, whom wich should be modified when run through a mesh deformer that modifies the
  // vertex coordinates
  vsx_array<unsigned long>* selected_vertices;

  void calculate_face_centers() {
    if (!faces.size()) return;
    for (unsigned long i = 0; i < faces.size(); ++i) {
      //printf("i: %d %d\n",i,faces[i].a);
      face_centers[i].x = (vertices[faces[i].a].x+vertices[faces[i].b].x+vertices[faces[i].c].x);
      face_centers[i].y = (vertices[faces[i].a].y+vertices[faces[i].b].y+vertices[faces[i].c].y);
      face_centers[i].z = (vertices[faces[i].a].z+vertices[faces[i].b].z+vertices[faces[i].c].z);
    }
  }
  
  inline vsx_vector get_face_normal(unsigned long index)
  {
    // counter-clockwise faces
    //  c
    //  ^
    //  |
    //  b<----a
    vsx_vector a = vertices[faces[index].b] - vertices[faces[index].a];  
    vsx_vector b = vertices[faces[index].c] - vertices[faces[index].a];
    vsx_vector res;
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
  	//printf("vc\n");
    vertices.clear();
    //printf("vnc\n");
    vertex_normals.clear(); 
    //printf("vcc\n");
    vertex_colors.clear(); 
    //printf("vtc\n");
    vertex_tex_coords.clear();
    //printf("fc\n");
    faces.clear(); 
    //printf("fn\n");
    face_normals.clear(); 
    //printf("fc\n");
    face_centers.clear();
    //printf("--end\n");
  }

  ~vsx_mesh_data() {
    clear();
  }
};
#endif

class vsx_mesh {
public:
  unsigned long timestamp;
#ifndef VSX_NO_MESH
  vsx_mesh_data* data;
#else
  void* data;
#endif
  vsx_mesh() {
#ifndef VSX_NO_MESH
    data = new vsx_mesh_data;
    timestamp = rand();
#endif
  }
  ~vsx_mesh() {
#ifndef VSX_NO_MESH
    delete (vsx_mesh_data*)data;
#endif
  }
};

//	
//	vsx_mesh value_given_to_us;
//----------------------------------
//	vsx_mesh our_own_mesh;
//	if (value_given_to_us->is_vertices_modified) {
//	  our_own_mesh->copy_vertices_from_other_mesh(value_given_to_us);
//	}
#endif
