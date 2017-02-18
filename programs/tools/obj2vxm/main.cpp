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
#include <vsx_platform.h>
#include <container/vsx_ma_vector.h>
#include <math/vector/vsx_vector3.h>
#include <container/vsx_nw_vector.h>
#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>

#if PLATFORM == PLATFORM_WINDOWS
  #include <vsx_unistd.h>
#else
  #include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

#include <filesystem/vsx_filesystem.h>
#include <graphics/vsx_mesh.h>
#ifdef _WIN32
#include <io.h>
#endif

vsx_mesh<float> mesh;

char cur_path[4096];
vsx_string<>current_path = cur_path;

int main(int argc, char* argv[])
{
  vsx_printf(L"Vovoid VSX OBJ 2 VSXMESH\n");

  char* c = getcwd(cur_path,4096);
  (void)c;

  vsx_printf(L"current path is: %s\n", cur_path);

  vsx_printf(L"sizeof vsx_vector: %ld\n",sizeof(vsx_vector3<float>));
  vsx_printf(L"sizeof vsx_tex_coord: %ld\n",sizeof(vsx_tex_coord2f));
  vsx_printf(L"sizeof vsx_face: %ld\n",sizeof(vsx_face3));

  if
  (
    argc < 3
    ||
    vsx_string<>(argv[1]) == "-help"
  )
  {
    vsx_printf(L"obj2vsxmesh command line syntax:\n"
           "-tc compute tangents and store in color array\n");
    return 0;
  }

  FILE* fp = fopen(argv[1],"r");
  if (!fp)
  {
    vsx_printf(L"error opening file!\n");
    return 1;
  }


  char buf[65535];
  vsx_string<>line;
  vsx_ma_vector< vsx_vector3<> > vertices;
  vsx_ma_vector< vsx_vector3<> > normals;
  vsx_ma_vector< vsx_tex_coord2f > texcoords;
  vsx_ma_vector< vsx_face3 > faces;
  mesh.data->vertex_tex_coords.reset_used();
  int face_cur = 0;
  while (fgets(buf,65535,fp))
  {
    line = buf;

    if (!line.size())
      continue;

    line.trim_lf();

    vsx_nw_vector< vsx_string<> > parts;
    vsx_string<>deli = " ";
    vsx_string_helper::explode(line, deli, parts);
    if (parts[0] == "v")
    {
      vertices.push_back(vsx_vector3<>(vsx_string_helper::s2f(parts[1]),vsx_string_helper::s2f(parts[2]),vsx_string_helper::s2f(parts[3])));
      continue;
    }


    if (parts[0] == "vt")
    {
      vsx_tex_coord2f a;
      a.s = (vsx_string_helper::s2f(parts[1]));
      a.t = (vsx_string_helper::s2f(parts[2]));
      texcoords.push_back(a);
      continue;
    }


    if (parts[0] == "vn")
    {
      normals.push_back(vsx_vector3<>(vsx_string_helper::s2f(parts[1]),vsx_string_helper::s2f(parts[2]),vsx_string_helper::s2f(parts[3])));
      continue;
    }




    if (parts[0] == "f")
    {
      vsx_face3 ff;
      vsx_string<>deli2 = "/";

      vsx_nw_vector< vsx_string<> > parts2;
      vsx_string_helper::explode(parts[1], deli2, parts2);
      vsx_nw_vector< vsx_string<> > parts3;
      vsx_string_helper::explode(parts[2], deli2, parts3);
      vsx_nw_vector< vsx_string<> > parts4;
      vsx_string_helper::explode(parts[3], deli2, parts4);

      ff.c = face_cur;
      ff.b = face_cur+1;
      ff.a = face_cur+2;

      mesh.data->vertices[ff.a] = vertices[vsx_string_helper::s2i(parts2[0])-1];
      mesh.data->vertices[ff.b] = vertices[vsx_string_helper::s2i(parts3[0])-1];
      mesh.data->vertices[ff.c] = vertices[vsx_string_helper::s2i(parts4[0])-1];

      if (parts2[2] != "")
      {
        mesh.data->vertex_normals[ff.a] = normals[vsx_string_helper::s2i(parts2[2])-1];
        mesh.data->vertex_normals[ff.b] = normals[vsx_string_helper::s2i(parts3[2])-1];
        mesh.data->vertex_normals[ff.c] = normals[vsx_string_helper::s2i(parts4[2])-1];
      }
      if (parts2[1] != "")
      {
        mesh.data->vertex_tex_coords[ff.a] = texcoords[vsx_string_helper::s2i(parts2[1])-1];
        mesh.data->vertex_tex_coords[ff.b] = texcoords[vsx_string_helper::s2i(parts3[1])-1];
        mesh.data->vertex_tex_coords[ff.c] = texcoords[vsx_string_helper::s2i(parts4[1])-1];
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


  size_t vert_size = mesh.data->vertices.size() * sizeof(vsx_vector3<>);
  vsx_printf(L"writing %ld vertex bytes\n",vert_size);
  fwrite((void*)&vert_size,sizeof(size_t),1,fp);

  fwrite((void*)mesh.data->vertices.get_pointer(),sizeof(vsx_vector3<>),mesh.data->vertices.size(),fp);



  size_t normals_size = mesh.data->vertex_normals.size() * sizeof(vsx_vector3<>);
  vsx_printf(L"writing %ld normals bytes\n",normals_size);
  fwrite((void*)&normals_size,sizeof(size_t),1,fp);

  fwrite((void*)mesh.data->vertex_normals.get_pointer(),sizeof(vsx_vector3<>),mesh.data->vertex_normals.size(),fp);



  size_t tex_coords_size = mesh.data->vertex_tex_coords.size() * sizeof(vsx_tex_coord2f);
  vsx_printf(L"writing %ld texcoord bytes\n",tex_coords_size);
  fwrite((void*)&tex_coords_size,sizeof(size_t),1,fp);

  fwrite((void*)mesh.data->vertex_tex_coords.get_pointer(),sizeof(vsx_tex_coord2f), mesh.data->vertex_tex_coords.size(),fp);




  size_t faces_size = mesh.data->faces.size() * sizeof(vsx_face3);
  vsx_printf(L"writing %ld face bytes\n",faces_size);
  fwrite((void*)&faces_size,sizeof(size_t),1,fp);

  fwrite((void*)mesh.data->faces.get_pointer(),sizeof(vsx_face3),mesh.data->faces.size(),fp);

  fclose(fp);

  vsx_printf(L"Exported:\n");
  vsx_printf(L"  %ld vertices\n", mesh.data->vertices.size());
  vsx_printf(L"  %ld normals\n", mesh.data->vertex_normals.size());
  vsx_printf(L"  %ld texture/uv coordinates\n", mesh.data->vertex_tex_coords.size());
  vsx_printf(L"Making up a total of:\n");
  vsx_printf(L"  %ld faces\n", mesh.data->faces.size());


  return 0;
}
