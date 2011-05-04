#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"
#include "vsx_math_3d.h"

class vsx_module_obj_loader : public vsx_module {
  // in
	vsx_module_param_resource* filename;
	vsx_module_param_int* preserve_uv_coords;
	// out
	vsx_module_param_mesh* result;
	// internal
	vsx_mesh* mesh;
  vsx_mesh* mesh_empty;
	bool first_run;
	int n_rays;
	vsx_string current_filename;
public:
  
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }

void module_info(vsx_module_info* info)
{
  info->identifier = "mesh;importers;obj_importer";
  info->description = "";
  info->in_param_spec = "filename:resource,preserve_uv_coords:enum?NO|YES";
  info->out_param_spec = "mesh:mesh";
  info->component_class = "mesh";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = false;
  filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  filename->set("");
  current_filename = "";

  preserve_uv_coords = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"preserve_uv_coords");
  preserve_uv_coords->set(1);

  result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
  first_run = true;
}

void run() {
  if (filename->get() != current_filename) {
   	if (!verify_filesuffix(filename->get(),"obj")) {
   		filename->set(current_filename);
   		message = "module||ERROR! This is not a OBJ mesh file!";
      result->set_p(mesh_empty);
   		return;
   	} else message = "module||ok";

    current_filename = filename->get();
    vsxf_handle *fp;
    //printf("a\n");
    if ((fp = engine->filesystem->f_open(current_filename.c_str(), "r")) == NULL)
    {
      result->set_p(mesh_empty);
      return;
    }
    
    char buf[65535];
    vsx_string line;
    vsx_array<vsx_vector> vertices; //vertices.set_allocation_increment(15000);
    vsx_array<vsx_vector> normals; //normals.set_allocation_increment(15000);
    vsx_array<vsx_tex_coord> texcoords; //texcoords.set_allocation_increment(15000);
    //mesh->data->vertex_tex_coords.reset_used();
    mesh->data->clear();
    //mesh->data->vertices.set_allocation_increment(15000);
    //mesh->data->vertex_normals.set_allocation_increment(15000);
    //mesh->data->vertex_tex_coords.set_allocation_increment(15000);
    //mesh->data->faces.set_allocation_increment(15000);

    int face_cur = 0;
    //printf("b\n");
    bool found_normals = false;
    bool found_texcoords = false;
    if (preserve_uv_coords->get()) {
      mesh->data->vertices.reset_used();
      mesh->data->vertex_tex_coords.reset_used();
      mesh->data->vertex_normals.reset_used();
      mesh->data->faces.reset_used();

	    while (engine->filesystem->f_gets(buf,65535,fp)) {
	      line = buf;
	      if (line[line.size()-1] == 0x0A) line.pop_back();
	      if (line[line.size()-1] == 0x0D) line.pop_back();
	      //printf("reading line: %s\n",line.c_str());
	      //printf("c\n");
	      if (line.size()) {
	        vsx_avector<vsx_string> parts;
	        vsx_string deli = " ";
	        explode(line, deli, parts);
	        if (parts[0] == "v") {
	        	//printf("v\n");
	          //mesh->data->vertices.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
	          vertices.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
	        } else
	        if (parts[0] == "vt") {
	        	//printf("vt\n");
	          vsx_tex_coord a;
	          a.s = (s2f(parts[1]));
	          a.t = (s2f(parts[2]));
	          //printf("%f  ::   %f\n",a.s,a.t);
	          texcoords.push_back(a);
	          //vsx_vector__(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
	          found_texcoords = true;

	        } else
	        if (parts[0] == "vn") {
	        	//printf("vn\n");
	          //printf("normal\n");
	          normals.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
	          found_normals = true;
	          //mesh->data->vertex_normals.push_back(vsx_vector__(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
	        } else
	        if (parts[0] == "f") {
	        	//printf("f1\n");
	          //printf("face\n");
	          //if (parts.size() == 4) {
	            //printf("num texcoords %d\n",texcoords.size());
	            vsx_face ff;
	//            vsx_avector<vsx_string> parts2;
	            vsx_string deli2 = "/";

	/*            explode(parts[1], deli2, parts2);
	            ff.c = s2i(parts2[0])-1;
	            mesh->data->vertex_normals[ff.c] = normals[s2i(parts2[1])-1];
	            mesh->data->vertex_tex_coords[ff.c] = texcoords[s2i(parts2[1])-1];

	            explode(parts[2], deli2, parts2);
	            ff.b = s2i(parts2[0])-1;
	            mesh->data->vertex_normals[ff.b] = normals[s2i(parts2[1])-1];
	            mesh->data->vertex_tex_coords[ff.b] = texcoords[s2i(parts2[1])-1];

	            explode(parts[3], deli2, parts2);
	            ff.a = s2i(parts2[0])-1;
	            mesh->data->vertex_normals[ff.a] = normals[s2i(parts2[1])-1];
	            mesh->data->vertex_tex_coords[ff.a] = texcoords[s2i(parts2[1])-1];*/


	            vsx_avector<vsx_string> parts2;
	            explode(parts[1], deli2, parts2);
	            vsx_avector<vsx_string> parts3;
	            explode(parts[2], deli2, parts3);
	            vsx_avector<vsx_string> parts4;
	            explode(parts[3], deli2, parts4);

	            ff.c = face_cur;   //s2i(parts2[0])-1;
	            ff.b = face_cur+1; //s2i(parts3[0])-1;
	            ff.a = face_cur+2; //s2i(parts4[0])-1;

              
	            //printf("f2\n");
              //printf("reading line: %s\n",line.c_str());
              int id;
              id = s2i(parts2[0])-1; if (id < 0) id=0;
	            mesh->data->vertices[ff.a] = vertices[id];
              id = s2i(parts3[0])-1; if (id < 0) id=0;
              mesh->data->vertices[ff.b] = vertices[id];
              id = s2i(parts4[0])-1; if (id < 0) id=0;
	            mesh->data->vertices[ff.c] = vertices[id];

	            if (found_texcoords && found_normals) {
                if (parts2[1] != "") {
                  mesh->data->vertex_tex_coords[ff.a] = texcoords[s2i(parts2[1])-1];
                  mesh->data->vertex_tex_coords[ff.b] = texcoords[s2i(parts3[1])-1];
                  mesh->data->vertex_tex_coords[ff.c] = texcoords[s2i(parts4[1])-1];
                }
	              if (parts2[2] != "") {
                  mesh->data->vertex_normals[ff.a] = normals[s2i(parts2[2])-1];
                  mesh->data->vertex_normals[ff.b] = normals[s2i(parts3[2])-1];
                  mesh->data->vertex_normals[ff.c] = normals[s2i(parts4[2])-1];
                }
	            } else
	            if (found_normals) {
                if (parts2[2] != "") {
                  mesh->data->vertex_normals[ff.a] = normals[s2i(parts2[2])-1];
                  mesh->data->vertex_normals[ff.b] = normals[s2i(parts3[2])-1];
                  mesh->data->vertex_normals[ff.c] = normals[s2i(parts4[2])-1];
                }
	            } else
	            if (found_texcoords) {
                if (parts2[1] != "") {
                  mesh->data->vertex_tex_coords[ff.a] = texcoords[s2i(parts2[1])-1];
                  mesh->data->vertex_tex_coords[ff.b] = texcoords[s2i(parts3[1])-1];
                  mesh->data->vertex_tex_coords[ff.c] = texcoords[s2i(parts4[1])-1];
                }
	            }


						  //printf("%d  ",s2i(parts2[1]));
						  //printf("%d  ",s2i(parts3[1]));
						  //printf("%d\n",s2i(parts4[1]));
						  //printf("f3\n");

	            /*printf("ida: %d\n",s2i(parts2[1]));
	            printf("orig coords: %f %f\n",texcoords[s2i(parts2[1])-1].s,texcoords[s2i(parts2[1])-1].t);
	            printf("texcoord s: %f   %f\n",mesh->data->vertex_tex_coords[ff.a].s,mesh->data->vertex_tex_coords[ff.a].t);

	            printf("idb: %d\n",s2i(parts3[1]));
	            printf("orig coords: %f %f\n",texcoords[s2i(parts3[1])-1].s,texcoords[s2i(parts3[1])-1].t);
	            printf("texcoord s: %f   %f\n",mesh->data->vertex_tex_coords[ff.a].s,mesh->data->vertex_tex_coords[ff.a].t);

	            printf("idc: %d\n",s2i(parts4[1]));
	            printf("orig coords: %f %f\n",texcoords[s2i(parts4[1])-1].s,texcoords[s2i(parts4[1])-1].t);
	            printf("texcoord s: %f   %f\n",mesh->data->vertex_tex_coords[ff.a].s,mesh->data->vertex_tex_coords[ff.a].t);
	*/
	            face_cur += 3;
	            mesh->data->faces.push_back(ff);
	            //printf("f4\n");
	          //}
	        }

	      }
	    }
    } else {

	    while (engine->filesystem->f_gets(buf,65535,fp)) {
	      line = buf;
	      if (line[line.size()-1] == 0x0A) line.pop_back();
	      if (line[line.size()-1] == 0x0D) line.pop_back();
	      if (line.size()) {
	        vsx_avector<vsx_string> parts;
	        vsx_string deli = " ";
	        explode(line, deli, parts);
	        if (parts[0] == "v") {
	        	mesh->data->vertices.push_back(vsx_vector(s2f(parts[1]),s2f(parts[2]),s2f(parts[3])));
	        } else
	        if (parts[0] == "f") {
	            vsx_face ff;
	            vsx_string deli2 = "/";

	            vsx_avector<vsx_string> parts2;
	            explode(parts[1], deli2, parts2);
	            vsx_avector<vsx_string> parts3;
	            explode(parts[2], deli2, parts3);
	            vsx_avector<vsx_string> parts4;
	            explode(parts[3], deli2, parts4);

	            ff.c = s2i(parts2[0])-1;
	            ff.b = s2i(parts3[0])-1;
	            ff.a = s2i(parts4[0])-1;

	            //printf("face %d %d %d %d\n", mesh->data->faces.size(), ff.a, ff.b, ff.c);

	            mesh->data->faces.push_back(ff);
	        }

	      }
	    }
    }

    //for (int i = 0; i < texcoords.size(); ++i) {
//      printf("%f %f\n",texcoords[i].s, texcoords[i].t);
//    }
    //for (int i = 0; i < mesh->data->vertex_tex_coords.size(); ++i) {
//      printf("%f %f\n",mesh->data->vertex_tex_coords[i].s,mesh->data->vertex_tex_coords[i].t);
    //}
    //printf("num texcoords in face: %d\n",mesh->data->vertex_tex_coords.size());
    engine->filesystem->f_close(fp);
    loading_done = true;
    mesh->timestamp = (int)(engine->real_vtime*1000.0f);
    #ifdef VSXU_DEBUG
      printf("mesh timestamp: %d\n", (int)mesh->timestamp);
    #endif
  }
  //printf("result_set\n");
  result->set_p(mesh);
//  mesh->data->vertices[0] = vsx_vector__(0,0,0);
//  mesh->data->vertex_colors[0] = vsx_color__(center_color->get(0),center_color->get(1),center_color->get(2),center_color->get(3));
  //balls.Update(engine->real_dtime);
//  a += 0.02;
  //balls.Render();
//  if (first_run || n_rays != (int)num_rays->get()) {
//    mesh->data->vertex_tex_coords[0] = vsx_vector__(0,0,0);
//    mesh->data->vertices.reset_used();
//    mesh->data->faces.reset_used();
    //printf("generating random points\n");
/*    for (int i = 1; i < (int)num_rays->get(); ++i) {
      mesh->data->vertices[i*2].x = (rand()%10000)*0.0001-0.5;
      mesh->data->vertices[i*2].y = (rand()%10000)*0.0001-0.5;
      mesh->data->vertices[i*2].z = (rand()%10000)*0.0001-0.5;
      mesh->data->vertex_colors[i*2] = vsx_color__(0,0,0,0);
      mesh->data->vertex_tex_coords[i*2] = vsx_vector__(0,1,0);
      mesh->data->vertices[i*2+1].x = (rand()%10000)*0.0001-0.5;
      mesh->data->vertices[i*2+1].y = (rand()%10000)*0.0001-0.5;
      mesh->data->vertices[i*2+1].z = (rand()%10000)*0.0001-0.5;
      mesh->data->vertex_colors[i*2+1] = vsx_color__(0,0,0,0);
      mesh->data->vertex_tex_coords[i*2+1] = vsx_vector__(1,0,0);
      mesh->data->faces[i-1].a = 0;
      mesh->data->faces[i-1].b = i*2;
      mesh->data->faces[i-1].c = i*2+1;
      n_rays = (int)num_rays->get();
    }
    first_run = false;

  } */
  /*mesh->data->vertices = balls.vertices;
  mesh->data->vertex_normals = balls.vertex_normals;
  mesh->data->vertex_tex_coords = balls.vertex_tex_coords;
  mesh->data->faces = balls.faces;
  result->set_p(mesh);*/
  /*else {
    if (num_points->get() < mesh->data->vertices.size()) {
      mesh->data->vertices.reset_used((int)num_points->get());
    } else
    if (num_points->get() > mesh->data->vertices.size()) {
      for (int i = mesh->data->vertices.size(); i < (int)num_points->get(); ++i) {
        mesh->data->vertices[i].x = (rand()%10000)*0.0001*scaling->get(0);
        mesh->data->vertices[i].y = (rand()%10000)*0.0001*scaling->get(1);
        mesh->data->vertices[i].z = (rand()%10000)*0.0001*scaling->get(2);
      }
    }
  }
    printf("randMesh done %d\n",mesh->data->vertices.size());*/

  //  }
}
};

//******************************************************************************************
//******************************************************************************************
//******************************************************************************************

class vsx_module_vxm_loader : public vsx_module {
  // in
  vsx_module_param_resource* filename;
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh* mesh;
  bool first_run;
  int n_rays;
  vsx_string current_filename;
public:
  bool init() {
    mesh = new vsx_mesh;
    return true;
  }

  void on_delete()
  {
    delete mesh;
  }
void module_info(vsx_module_info* info)
{
  info->identifier = "mesh;importers;vxm_importer";
  info->description = "VXM loader";
  info->in_param_spec = "filename:resource";
  info->out_param_spec = "mesh:mesh";
  info->component_class = "mesh";
}

void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
  loading_done = false;
  filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
  filename->set("");
  current_filename = "";

  result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
  result->set_p(mesh);
  first_run = true;
}

void run() {
  if (filename->get() != current_filename) {
    if (!verify_filesuffix(filename->get(),"vxm")) {
      filename->set(current_filename);
      message = "module||ERROR! This is not a OBJ mesh file!";
      return;
    } else message = "module||ok";

    current_filename = filename->get();
    vsxf_handle *fp;
    //printf("a\n");
    if ((fp = engine->filesystem->f_open(current_filename.c_str(), "r")) == NULL)
      return;
    char tag[4];
    engine->filesystem->f_read((void*)&tag,sizeof(char) * 4,fp);
    vsx_string line;
    line = tag;
    //printf("vxm line read: %s\n",line.c_str());
    if (line == "vxm")
    {
      //printf("found vxm file\n");
      size_t vert_size;
      engine->filesystem->f_read((void*)&vert_size,sizeof(size_t) * 1,fp);
      if (vert_size)
      {
        //printf("vertex bytes: %d\n",vert_size);
        void* vert_p = malloc(vert_size);
        engine->filesystem->f_read(vert_p,vert_size,fp);
        mesh->data->vertices.set_data((vsx_vector*)vert_p,vert_size / sizeof(vsx_vector));
      }

      size_t normals_size;
      engine->filesystem->f_read((void*)&normals_size,sizeof(size_t) * 1,fp);
      if (normals_size)
      {
        //printf("normals bytes: %d\n",normals_size);
        void* norm_p = malloc( normals_size);
        engine->filesystem->f_read(norm_p,normals_size,fp);
        mesh->data->vertex_normals.set_data((vsx_vector*)norm_p,normals_size / sizeof(vsx_vector));
      }

      size_t tex_coords_size;
      engine->filesystem->f_read((void*)&tex_coords_size,sizeof(size_t) * 1,fp);
      if (tex_coords_size)
      {
        //printf("texcoord count: %d\n",tex_coords_size);
        void* texcoords_p = malloc(tex_coords_size);
        engine->filesystem->f_read(texcoords_p,tex_coords_size,fp);
        mesh->data->vertex_tex_coords.set_data((vsx_tex_coord*)texcoords_p,tex_coords_size / sizeof(vsx_tex_coord));
      }

      size_t faces_size;
      engine->filesystem->f_read((void*)&faces_size,sizeof(size_t) * 1,fp);
      if (faces_size)
      {
        //printf("face count: %d\n",faces_size);
        void* faces_p = malloc(faces_size);
        engine->filesystem->f_read(faces_p,faces_size,fp);
        mesh->data->faces.set_data((vsx_face*)faces_p,faces_size / sizeof(vsx_face));
      }
    }
    engine->filesystem->f_close(fp);
    loading_done = true;
    mesh->timestamp++;
  }
  //result->set_p(mesh);
//  mesh->data->vertices[0] = vsx_vector__(0,0,0);
//  mesh->data->vertex_colors[0] = vsx_color__(center_color->get(0),center_color->get(1),center_color->get(2),center_color->get(3));
  //balls.Update(engine->real_dtime);
//  a += 0.02;
  //balls.Render();
//  if (first_run || n_rays != (int)num_rays->get()) {
//    mesh->data->vertex_tex_coords[0] = vsx_vector__(0,0,0);
//    mesh->data->vertices.reset_used();
//    mesh->data->faces.reset_used();
    //printf("generating random points\n");
/*    for (int i = 1; i < (int)num_rays->get(); ++i) {
      mesh->data->vertices[i*2].x = (rand()%10000)*0.0001-0.5;
      mesh->data->vertices[i*2].y = (rand()%10000)*0.0001-0.5;
      mesh->data->vertices[i*2].z = (rand()%10000)*0.0001-0.5;
      mesh->data->vertex_colors[i*2] = vsx_color__(0,0,0,0);
      mesh->data->vertex_tex_coords[i*2] = vsx_vector__(0,1,0);
      mesh->data->vertices[i*2+1].x = (rand()%10000)*0.0001-0.5;
      mesh->data->vertices[i*2+1].y = (rand()%10000)*0.0001-0.5;
      mesh->data->vertices[i*2+1].z = (rand()%10000)*0.0001-0.5;
      mesh->data->vertex_colors[i*2+1] = vsx_color__(0,0,0,0);
      mesh->data->vertex_tex_coords[i*2+1] = vsx_vector__(1,0,0);
      mesh->data->faces[i-1].a = 0;
      mesh->data->faces[i-1].b = i*2;
      mesh->data->faces[i-1].c = i*2+1;
      n_rays = (int)num_rays->get();
    }
    first_run = false;

  } */
  /*mesh->data->vertices = balls.vertices;
  mesh->data->vertex_normals = balls.vertex_normals;
  mesh->data->vertex_tex_coords = balls.vertex_tex_coords;
  mesh->data->faces = balls.faces;
  result->set_p(mesh);*/
  /*else {
    if (num_points->get() < mesh->data->vertices.size()) {
      mesh->data->vertices.reset_used((int)num_points->get());
    } else
    if (num_points->get() > mesh->data->vertices.size()) {
      for (int i = mesh->data->vertices.size(); i < (int)num_points->get(); ++i) {
        mesh->data->vertices[i].x = (rand()%10000)*0.0001*scaling->get(0);
        mesh->data->vertices[i].y = (rand()%10000)*0.0001*scaling->get(1);
        mesh->data->vertices[i].z = (rand()%10000)*0.0001*scaling->get(2);
      }
    }
  }
    printf("randMesh done %d\n",mesh->data->vertices.size());*/

  //  }
}
};


vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_obj_loader);
    case 1: return (vsx_module*)(new vsx_module_vxm_loader);
  }
  return 0;
}


void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_obj_loader*)m; break;
    case 1: delete (vsx_module_vxm_loader*)m; break;
  }
}


unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 2;
}



