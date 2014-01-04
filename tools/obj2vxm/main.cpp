/**
* Project: VSXu: Realtime modular visual programming language, music/audio visualizer.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
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

#ifdef _WIN32
#include <windows.h>
#endif
#include <vector>
#include "vsx_array.h"
#include "vsx_vector.h"
#include "vsx_avector.h"
#include "vsx_string.h"
#include "vsx_string_aux.h"
using namespace std;
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "vsxfst.h"
#include "vsx_mesh.h"
#ifdef _WIN32
#include <io.h>
#endif

vsx_mesh mesh;

char cur_path[4096];
vsx_string current_path = cur_path;

int main(int argc, char* argv[])
{
  vsx_string base_path = get_path_from_filename(vsx_string(argv[0]));

  printf("Vovoid VSX OBJ 2 VSXMESH\n");

  char* c = getcwd(cur_path,4096);
  (void)c;

  printf("current path is: %s\n", cur_path);

  printf("sizeof vsx_vector: %ld\n",sizeof(vsx_vector));
  printf("sizeof vsx_tex_coord: %ld\n",sizeof(vsx_tex_coord));
  printf("sizeof vsx_face: %ld\n",sizeof(vsx_face));

  if
  (
    argc < 3
    ||
    vsx_string(argv[1]) == "-help"
  )
  {
    printf("obj2vsxmesh command line syntax:\n"
           "-tc compute tangents and store in color array\n");
    return 0;
  }

  FILE* fp = fopen(argv[1],"r");
  if (!fp)
  {
    vsx_printf("error opening file!\n");
    return 1;
  }


  char buf[65535];
  vsx_string line;
  vsx_array<vsx_vector> vertices;
  vsx_array<vsx_vector> normals;
  vsx_array<vsx_tex_coord> texcoords;
  vsx_array<vsx_face> faces;
  mesh.data->vertex_tex_coords.reset_used();
  int face_cur = 0;
  while (fgets(buf,65535,fp))
  {
    line = buf;

    if (!line.size())
      continue;

    line.trim_lf();

    vsx_avector<vsx_string> parts;
    vsx_string deli = " ";
    explode(line, deli, parts);
    if (parts[0] == "v")
    {
      vertices.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
      continue;
    }


    if (parts[0] == "vt")
    {
      vsx_tex_coord a;
      a.s = (s2f(parts[1]));
      a.t = (s2f(parts[2]));
      texcoords.push_back(a);
      continue;
    }


    if (parts[0] == "vn")
    {
      normals.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
      continue;
    }




    if (parts[0] == "f")
    {
      vsx_face ff;
      vsx_string deli2 = "/";

      vsx_avector<vsx_string> parts2;
      explode(parts[1], deli2, parts2);
      vsx_avector<vsx_string> parts3;
      explode(parts[2], deli2, parts3);
      vsx_avector<vsx_string> parts4;
      explode(parts[3], deli2, parts4);

      ff.c = face_cur;
      ff.b = face_cur+1;
      ff.a = face_cur+2;

      mesh.data->vertices[ff.a] = vertices[vsx_string_aux::s2i(parts2[0])-1];
      mesh.data->vertices[ff.b] = vertices[vsx_string_aux::s2i(parts3[0])-1];
      mesh.data->vertices[ff.c] = vertices[vsx_string_aux::s2i(parts4[0])-1];

      if (parts2[2] != "")
      {
        mesh.data->vertex_normals[ff.a] = normals[vsx_string_aux::s2i(parts2[2])-1];
        mesh.data->vertex_normals[ff.b] = normals[vsx_string_aux::s2i(parts3[2])-1];
        mesh.data->vertex_normals[ff.c] = normals[vsx_string_aux::s2i(parts4[2])-1];
      }
      if (parts2[1] != "")
      {
        mesh.data->vertex_tex_coords[ff.a] = texcoords[vsx_string_aux::s2i(parts2[1])-1];
        mesh.data->vertex_tex_coords[ff.b] = texcoords[vsx_string_aux::s2i(parts3[1])-1];
        mesh.data->vertex_tex_coords[ff.c] = texcoords[vsx_string_aux::s2i(parts4[1])-1];
      }

      face_cur += 3;
      mesh.data->faces.push_back(ff);
    } // face


  } // while fgets
  fclose(fp);


  //***************************************************************************

  // time to write
  fp = fopen(argv[2],"wb");

  char tag[] = "vxm";
  fwrite((void*)&tag,sizeof(char),4,fp);


  size_t vert_size = mesh.data->vertices.size() * sizeof(vsx_vector);
  printf("writing %ld vertex bytes\n",vert_size);
  fwrite((void*)&vert_size,sizeof(size_t),1,fp);

  fwrite((void*)mesh.data->vertices.get_pointer(),sizeof(vsx_vector),mesh.data->vertices.size(),fp);



  size_t normals_size = mesh.data->vertex_normals.size() * sizeof(vsx_vector);
  printf("writing %ld normals bytes\n",normals_size);
  fwrite((void*)&normals_size,sizeof(size_t),1,fp);

  fwrite((void*)mesh.data->vertex_normals.get_pointer(),sizeof(vsx_vector),mesh.data->vertex_normals.size(),fp);



  size_t tex_coords_size = mesh.data->vertex_tex_coords.size() * sizeof(vsx_tex_coord);
  printf("writing %ld texcoord bytes\n",tex_coords_size);
  fwrite((void*)&tex_coords_size,sizeof(size_t),1,fp);

  fwrite((void*)mesh.data->vertex_tex_coords.get_pointer(),sizeof(vsx_tex_coord), mesh.data->vertex_tex_coords.size(),fp);




  size_t faces_size = mesh.data->faces.size() * sizeof(vsx_face);
  printf("writing %ld face bytes\n",faces_size);
  fwrite((void*)&faces_size,sizeof(size_t),1,fp);

  fwrite((void*)mesh.data->faces.get_pointer(),sizeof(vsx_face),mesh.data->faces.size(),fp);

  fclose(fp);

  vsx_printf("Exported:\n");
  vsx_printf("  %ld vertices\n", mesh.data->vertices.size());
  vsx_printf("  %ld normals\n", mesh.data->vertex_normals.size());
  vsx_printf("  %ld texture/uv coordinates\n", mesh.data->vertex_tex_coords.size());
  vsx_printf("Making up a total of:\n");
  vsx_printf("  %ld faces\n", mesh.data->faces.size());


  return 0;
}
