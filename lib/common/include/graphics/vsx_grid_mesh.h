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


#pragma once

#include <container/vsx_nw_vector_nd.h>
#include <container/vsx_nw_vector.h>
#include <container/vsx_ma_vector.h>
#include <color/vsx_color.h>
#include <math/vector/vsx_vector3.h>


// this is an attempt to make a mesh class that can help with producing meshes
// for use in the lowlevel mesh format where the mesh itself can be described as a 2d wrap
// or some sort, a tube, plane or similar - to speed up calculations

// vertices are arranged in a grid so that you can check if a vertex is already calculated
// 
class vsx_grid_vertex {
public:
  bool valid;
  GLuint id; // used when dumping faces into vsx_mesh
  vsx_vector3<> coord;  // xyz
  vsx_color<> color;  // color
  vsx_vector3<> normal; // vertex normal
  vsx_vector3<> tex_coord;  // texture coordinate
  vsx_nw_vector<unsigned long> faces;
  vsx_ma_vector<unsigned long> lines;
  vsx_grid_vertex()
  {
    valid = false;
  }
};

class vsx_2dgrid_face {
public:
  unsigned long ax,ay,
                bx,by,
                cx,cy;
                
  vsx_vector3<> normal;
  vsx_vector3<> center;
  vsx_ma_vector<unsigned long> lines;
};

class vsx_grid_line {
public:
  vsx_grid_vertex* a,b; // from vertex a to vertex b
  vsx_2dgrid_face* fa, fb; // the 2 faces touching this line
};


/* the order in which to use this class:

 1a. define vertices
 1b. define faces of added vertices
 2. calculate face normals
 3. calculate vertex normals (if you need to)
 4. dump to vsx_mesh
 5. send to renderer (not to be done in code, use the renderers supplied in vsxu)

Notes:
  If you want to know the id of an added vertex, just go:
    unsigned long id = gmesh.vertices[segment].push_back(vsx_vector__(0,0,0)) - 1;
  push_back returns how many elements are in the array.
*/  


class vsx_2dgrid_mesh {
public:
  // vertices array, access it like this:
  // grid_mesh.vertices[segment_id][vertex_id].coord = vsx_vector__(0,0,0);
  vsx_nw_vector_nd< vsx_nw_vector_nd<vsx_grid_vertex> > vertices;
  // don't touch these arrays unless you really know what you're doing!
  // it has a direct relation to the [vertices] array above, if this is broken
  // you'll not get correct normals.
  vsx_nw_vector_nd< vsx_2dgrid_face > faces;
  vsx_nw_vector_nd< vsx_grid_line > lines;
    
  // Defines a face from 3 points given as id-numbers in the 2dimensional array (x,y)
  // So that per point you need one x and y value.
  // Example: 0,0,  1,0,  0,1
  // 
  void add_face(
    unsigned long ax, unsigned long ay, 
    unsigned long bx, unsigned long by, 
    unsigned long cx, unsigned long cy) {
    vsx_2dgrid_face f;
    f.ax = ax;
    f.ay = ay;
    f.bx = bx;
    f.by = by;
    f.cx = cx;
    f.cy = cy;
    faces.push_back(f);
    vertices[ax][ay].faces.push_back((unsigned long)faces.size()-1);
    vertices[bx][by].faces.push_back((unsigned long)faces.size()-1);
    vertices[cx][cy].faces.push_back((unsigned long)faces.size()-1);
  }
  
  // run this when you have defined all vertices and faces  
  void calculate_face_normals() {
    for (unsigned long a = 0; a < faces.size(); ++a) {
      faces[a].normal.assign_face_normal(
        &vertices[faces[a].ax][faces[a].ay].coord,
        &vertices[faces[a].bx][faces[a].by].coord,
        &vertices[faces[a].cx][faces[a].cy].coord
      );
      faces[a].normal.normalize();
    }
  }
  
  // resets the vertex array
  void reset_vertices() {
    for (unsigned long a = 0; a < vertices.size(); ++a) {
      for (unsigned long b = 0; b < vertices[a].size(); ++b) {
        vertices[a][b].faces.reset_used();
        vertices[a][b].lines.reset_used();
      }
    }
  }
  
  void reset_faces() {
    for (unsigned long a = 0; a < faces.size(); ++a) {
      faces[a].lines.reset_used();
    }
    faces.reset_used();
  }
  

  // run this after you've run the calculate_face_normals.
  void calculate_vertex_normals() {
    for (unsigned long a = 0; a < vertices.size(); ++a) {
      for (unsigned long b = 0; b < vertices[a].size(); ++b) {
        vertices[a][b].normal.x = 0.0f;
        vertices[a][b].normal.y = 0.0f;
        vertices[a][b].normal.z = 0.0f;
        for (unsigned long c = 0; c < vertices[a][b].faces.size(); ++c) {
          vertices[a][b].normal += faces[vertices[a][b].faces[c]].normal;
        }
        vertices[a][b].normal.normalize();
      }
    }
  }
  // when you're ready to send it to rendering, give it a pointer to a vsx_mesh
  void dump_vsx_mesh(vsx_mesh<>* mesh) {
    // 1. vertices
    mesh->data->vertices.reset_used();
    mesh->data->vertex_normals.reset_used();
    mesh->data->vertex_tex_coords.reset_used();
    GLuint mvid = 0;
    for (unsigned long a = 0; a < vertices.size(); ++a) {
      for (unsigned long b = 0; b < vertices[a].size(); ++b) {
        mesh->data->vertices[mvid] = vertices[a][b].coord;
        vertices[a][b].id = mvid;
        mesh->data->vertex_normals[mvid] = vertices[a][b].normal;
        mesh->data->vertex_colors[mvid] = vertices[a][b].color;
        mesh->data->vertex_tex_coords[mvid].s = vertices[a][b].tex_coord.x;
        mesh->data->vertex_tex_coords[mvid].t = vertices[a][b].tex_coord.y;
        ++mvid;
      }
    }
    if (mesh->data->faces.size() != faces.size()) {
      mesh->data->faces.reset_used();
      vsx_face3 f;
      for (unsigned long a = 0; a < faces.size(); ++a) {
        f.a = vertices[faces[a].ax][faces[a].ay].id;
        f.b = vertices[faces[a].bx][faces[a].by].id;
        f.c = vertices[faces[a].cx][faces[a].cy].id;
        mesh->data->faces.push_back(f);
      }
    }
  }
  
  void cleanup() {
    for (unsigned long a = 0; a < vertices.size(); ++a) {
      vertices[a].clear();
    }
    vertices.clear();
    faces.clear();
    lines.clear();
  }
  
};  
