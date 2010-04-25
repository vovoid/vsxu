#ifdef _WIN32
#include <windows.h>
#endif
#include <vector>
#include "vsx_array.h"
#include "vsx_math_3d.h"
#include "vsx_avector.h"
#include "vsx_string.h"
using namespace std;
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

	getcwd(cur_path,4096);
	printf("current path is: %s\n", cur_path);

	printf("sizeof vsx_vector: %d\n",sizeof(vsx_vector));

	//srand ( time(NULL)+rand() );

	if (argc == 1)
		printf(" * add \"-help\" for command syntax\n");
	//printf("\n\n");
  if (argc > 1)
  {
	  if (vsx_string(argv[1]) == "-help") {
			printf("obj2vsxmesh command line syntax:\n"
			 			 "-x [filename] (extract)\n");
			return 0;
	  }

	  if (argc > 2)
	  {
	  	// step 1, build up mesh in ram
	    FILE* fp = fopen(argv[1],"r");
	    if (fp)
	    {
  	    char buf[65535];
		    vsx_string line;
		    vsx_array<vsx_vector> vertices; //vertices.set_allocation_increment(15000);
		    vsx_array<vsx_vector> normals; //normals.set_allocation_increment(15000);
		    vsx_array<vsx_tex_coord> texcoords; //texcoords.set_allocation_increment(15000);
		    mesh.data->vertex_tex_coords.reset_used();
		    int face_cur = 0;
		    while (fgets(buf,65535,fp)) {
		      line = buf;
		      //printf("read line: %s\n",line.c_str());
		      //if (line.size())
		      if (line.size()) {
			      if (line[line.size()-1] == 0x0A) line.pop_back();
		      	if (line[line.size()-1] == 0x0D) line.pop_back();
		        vsx_avector<vsx_string> parts;
		        vsx_string deli = " ";
		        explode(line, deli, parts);
		        //printf("%d\n",__LINE__);
		        if (parts[0] == "v") {
		        	//printf("%d\n",__LINE__);
		          vertices.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
		        	//printf("%d\n",__LINE__);
		        } else
		        if (parts[0] == "vt") {
		        //printf("%d\n",__LINE__);
		          vsx_tex_coord a;
		          a.s = (s2f(parts[1]));
		          a.t = (s2f(parts[2]));
		          texcoords.push_back(a);

		        } else
		        if (parts[0] == "vn") {
		        //printf("%d\n",__LINE__);
		          normals.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
		        } else
		        if (parts[0] == "f") {
		        //printf("%d\n",__LINE__);
		            vsx_face ff;
		            vsx_string deli2 = "/";

		            vsx_avector<vsx_string> parts2;
		            explode(parts[1], deli2, parts2);
		            vsx_avector<vsx_string> parts3;
		            explode(parts[2], deli2, parts3);
		            vsx_avector<vsx_string> parts4;
		            explode(parts[3], deli2, parts4);

		            ff.c = face_cur;//s2i(parts2[0])-1;
		            ff.b = face_cur+1;//s2i(parts3[0])-1;
		            ff.a = face_cur+2;//s2i(parts4[0])-1;

		            mesh.data->vertices[ff.a] = vertices[s2i(parts2[0])-1];
		            mesh.data->vertices[ff.b] = vertices[s2i(parts3[0])-1];
		            mesh.data->vertices[ff.c] = vertices[s2i(parts4[0])-1];

								if (parts2[2] != "") {
		            	mesh.data->vertex_normals[ff.a] = normals[s2i(parts2[2])-1];
		            	mesh.data->vertex_normals[ff.b] = normals[s2i(parts3[2])-1];
		            	mesh.data->vertex_normals[ff.c] = normals[s2i(parts4[2])-1];
								}
								if (parts2[1] != "") {
		            	mesh.data->vertex_tex_coords[ff.a] = texcoords[s2i(parts2[1])-1];
		            	mesh.data->vertex_tex_coords[ff.b] = texcoords[s2i(parts3[1])-1];
		            	mesh.data->vertex_tex_coords[ff.c] = texcoords[s2i(parts4[1])-1];
								}

		            face_cur += 3;
		            mesh.data->faces.push_back(ff);
		        //printf("%d\n",__LINE__);
		          //}
		        } // face
		      } // line.size()
		    } // while fgets
		    fclose(fp);
		    // we should be done reading now
		    // time to write
		    fp = fopen(argv[2],"wb");
		    char tag[] = "vxm";
		    fwrite((void*)&tag,sizeof(char),4,fp);
		    size_t vert_size = mesh.data->vertices.size() * sizeof(vsx_vector);
		    printf("writing %d vertex bytes\n",vert_size);
		    fwrite((void*)&vert_size,sizeof(size_t),1,fp);
		    fwrite((void*)mesh.data->vertices.get_pointer(),sizeof(vsx_vector),mesh.data->vertices.size(),fp);

		    size_t normals_size = mesh.data->vertex_normals.size() * sizeof(vsx_vector);
		    printf("writing %d normals bytes\n",normals_size);
		    fwrite((void*)&normals_size,sizeof(size_t),1,fp);
		    fwrite((void*)mesh.data->vertex_normals.get_pointer(),sizeof(vsx_vector),mesh.data->vertex_normals.size(),fp);

		    size_t tex_coords_size = mesh.data->vertex_tex_coords.size() * sizeof(vsx_vector);
		    printf("writing %d texcoord bytes\n",tex_coords_size);
		    fwrite((void*)&tex_coords_size,sizeof(size_t),1,fp);
		    fwrite((void*)mesh.data->vertex_tex_coords.get_pointer(),sizeof(vsx_vector),mesh.data->vertex_tex_coords.size(),fp);

		    size_t faces_size = mesh.data->faces.size() * sizeof(vsx_face);
		    printf("writing %d face bytes\n",faces_size);
		    fwrite((void*)&faces_size,sizeof(size_t),1,fp);
		    fwrite((void*)mesh.data->faces.get_pointer(),sizeof(vsx_face),mesh.data->faces.size(),fp);

		    fclose(fp);
//  vsx_array<vsx_vector> vertices;
//  vsx_array<vsx_vector> vertex_normals;
//  vsx_array<vsx_color> vertex_colors;
//  vsx_array<vsx_tex_coord> vertex_tex_coords;
//  vsx_array<vsx_face> faces;
//  vsx_array<vsx_vector> face_normals;
//  vsx_array<vsx_vector> face_centers; // centers of the faces - the average of each face's v1+v2+v3/3


	    } // if (fp)
		}
  }
	return 0;
}
