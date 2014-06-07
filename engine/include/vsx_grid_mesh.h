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


#ifndef VSX_GRID_MESH_H
#define VSX_GRID_MESH_H

// this is an attempt to make a mesh class that can help with producing meshes
// for use in the lowlevel mesh format where the mesh itself can be described as a 2d wrap
// or some sort, a tube, plane or similar - to speed up calculations

// vertices are arranged in a grid so that you can check if a vertex is already calculated
// 
#ifndef VSX_NO_MESH
class vsx_grid_vertex {
public:
  bool valid;
  unsigned long id; // used when dumping faces into vsx_mesh
  vsx_vector3<> coord;  // xyz
  vsx_color<> color;  // color
  vsx_vector3<> normal; // vertex normal
  vsx_vector3<> tex_coord;  // texture coordinate
  vsx_avector<unsigned long> faces;
  vsx_array<unsigned long> lines;
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
  vsx_array<unsigned long> lines;
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
  /*void allocate_vertices(unsigned long a, unsigned long b) {
    if (a > vertices.size()) vertices.resize(a);
    if (b > vertices[a].size()) vertices[a].resize(b);
  } */ 
public:
  // vertices array, access it like this:
  // grid_mesh.vertices[segment_id][vertex_id].coord = vsx_vector__(0,0,0);
  vsx_avector_nd< vsx_avector_nd<vsx_grid_vertex> > vertices;
  // don't touch these arrays unless you really know what you're doing!
  // it has a direct relation to the [vertices] array above, if this is broken
  // you'll not get correct normals.
  vsx_avector_nd< vsx_2dgrid_face > faces;
  vsx_avector_nd< vsx_grid_line > lines;
    
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
    //f.normal.assign_face_normal(&vertices[ax][ay].coord,&vertices[bx][by].coord,&vertices[cx][cy].coord);
    //f.normal.normalize(); 
    //printf("adding face_2\n");
    faces.push_back(f);
    //printf("adding face_3 %d ax: %d, ay %d\n",faces.size()-1,ax,ay);
    vertices[ax][ay].faces.push_back(faces.size()-1);
    //printf("adding face_4\n");
    vertices[bx][by].faces.push_back(faces.size()-1);
    //printf("adding face_5\n");
    vertices[cx][cy].faces.push_back(faces.size()-1);
    //printf("adding face_6\n");
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
    //printf("cvn vertices size: %d\n",vertices.size());
    for (unsigned long a = 0; a < vertices.size(); ++a) {
      //printf("cvn a: %d, b.size: %d\n",a);
      for (unsigned long b = 0; b < vertices[a].size(); ++b) {
        //printf("cvn b: %d\n",b);
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
#ifndef VSX_NO_MESH
    // 1. vertices
    //printf("vertices size: %d\n",vertices.size());
    mesh->data->vertices.reset_used();
    mesh->data->vertex_normals.reset_used();
    mesh->data->vertex_tex_coords.reset_used();
    unsigned long mvid = 0;
    for (unsigned long a = 0; a < vertices.size(); ++a) {
      //printf("foo a %d\n",a);
      for (unsigned long b = 0; b < vertices[a].size(); ++b) {
        //printf("foo %d\n",b);
        //unsigned long mvid = mesh->data->vertices.push_back(vertices[a][b].coord)-1;
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
        //printf("abc: %d %d %d\n",vertices[faces[a].ax][faces[a].ay].id,vertices[faces[a].bx][faces[a].by].id,vertices[faces[a].cx][faces[a].cy].id);
        f.a = vertices[faces[a].ax][faces[a].ay].id;
        f.b = vertices[faces[a].bx][faces[a].by].id;
        f.c = vertices[faces[a].cx][faces[a].cy].id;
        /*vertices[faces[a].ax][faces[a].ay].coord.dump("a");
        vertices[faces[a].bx][faces[a].by].coord.dump("b");
        vertices[faces[a].cx][faces[a].cy].coord.dump("c");
        mesh->data->vertices[f.a].dump("ra");
        mesh->data->vertices[f.b].dump("rb");
        mesh->data->vertices[f.c].dump("rc");*/
        mesh->data->faces.push_back(f);
      }
    }
#endif    
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
#else
class vsx_2dgrid_mesh {};
#endif


#endif
