#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"
#include "vsx_grid_mesh.h"
#include "plane.h"

void vsx_cloud_plane::module_info(vsx_module_info* info)
{
  // make sure this code doesn't load any DLL's or build anything, that is, load the DLL in the run method

  // set the identifier. this should be dynamic, so in the real world
  info->identifier = "mesh;vovoid;cloud_plane";

// i left this development renderer (in wich we test controller initializations)
// untouched so you can see how you can build this dynamically.
  info->in_param_spec = "";
  info->out_param_spec = "mesh_result:mesh";
//  info->out_param_spec = "render_out:render";
  info->component_class = "mesh";
}


void vsx_cloud_plane::declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
{
//  quadratic=gluNewQuadric();										      // Create A Pointer To The Quadric Object
//	gluQuadricNormals(quadratic, GLU_SMOOTH);						// Create Smooth Normals
//	gluQuadricTexture(quadratic, GL_TRUE);							// Create Texture Coords
	mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_result");
	mesh_result->set(mesh);

	//render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
//	render_result->set(0);
//	GLfloat light_diffuse[] = {1, 1, 1, 1};
/*	GLfloat light_diffuse[] = {0.1, 0.1, 0.2, 1};
	GLfloat light_specular[] = {0.3, 0.6, 1, 1};
  GLfloat light_position[] = {0.0, 0.5, 1.0, 0};

  glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse);
  glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular);
  glLightfv(GL_LIGHT2, GL_POSITION, light_position);

	GLfloat light_diffuse1[] = {0.1, 0.1, 0.2, 1};
	GLfloat light_specular1[] = {0.3, 0.6, 1, 1};
	GLfloat light_position1[] = {0.0, 0.5, -1.0, 0};

  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
*/
  //mesh.data->add_vertex(0,0,0);
//  printf("mesh count %d %d\n",mesh.data->num_allocated_vertices, mesh.data->num_vertices);
}

void vsx_cloud_plane::simple_box() {
  vsx_2dgrid_mesh gmesh;
  for (int x = 0; x < 50; ++x) {
    for (int y = 0; y < 50; ++y) {
      gmesh.vertices[x][y].coord = vsx_vector(((float)x-25)*0.8f,(float(rand()%1000))*0.0002f,(float(y)-25)*0.8f);
      gmesh.vertices[x][y].tex_coord = vsx_vector(((float)x)/50.0f,((float)y)/50.0f,0);
      gmesh.vertices[x][y].color = vsx_color((float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.001f,(float)(rand()%1000)*0.0005f);
    }
  }
  for (int x = 0; x < 49; ++x) {
    for (int y = 0; y < 49; ++y) {
      gmesh.add_face(x,y+1,  x+1,y+1,  x,y);
      gmesh.add_face(x,y,  x+1,y+1,  x+1,y);
    }
  }


/*  gmesh.vertices[0][1].coord = vsx_vector__(-1, 1,1);  gmesh.vertices[0][2].coord = vsx_vector__(1, 1,1);
  gmesh.vertices[0][0].coord = vsx_vector__(-1,-1,1);  gmesh.vertices[0][3].coord = vsx_vector__(1,-1,1);

  gmesh.vertices[0][5].coord = vsx_vector__(-1, 1,-1);  gmesh.vertices[0][6].coord = vsx_vector__(1, 1,-1);
  gmesh.vertices[0][4].coord = vsx_vector__(-1,-1,-1);  gmesh.vertices[0][7].coord = vsx_vector__(1,-1,-1);
*/
  //gmesh.vertices[1][0].coord = vsx_vector__(-1,-1,1);
  /*printf("vert size: %d\n",gmesh.vertices.size());
  gmesh.vertices[0][0].coord = vsx_vector__(-1,-1,1);
  printf("vert size: %d\n",gmesh.vertices.size());
  gmesh.vertices[0][1].coord = vsx_vector__(-1, 1,1);
  gmesh.vertices[0][2].coord = vsx_vector__(1, 1,1);
  gmesh.vertices[0][3].coord = vsx_vector__(1,-1,1);

  gmesh.vertices[0][4].coord = vsx_vector__(-1,-1,-1);
  gmesh.vertices[0][5].coord = vsx_vector__(-1, 1,-1);
  gmesh.vertices[0][6].coord = vsx_vector__(1, 1,-1);
  gmesh.vertices[0][7].coord = vsx_vector__(1,-1,-1);

  gmesh.vertices[0][0].color = vsx_color__(1,0,0,0.5);
  gmesh.vertices[0][1].color = vsx_color__(1,0,0,0.5);
  printf("vert size: %d\n",gmesh.vertices.size());

  gmesh.vertices[0][2].color = vsx_color__(0,1,0,0.5);
  gmesh.vertices[0][3].color = vsx_color__(0,1,0,0.5);

  gmesh.vertices[0][4].color = vsx_color__(0,0,1,0.5);
  gmesh.vertices[0][5].color = vsx_color__(0,0,1,0.5);

  gmesh.vertices[0][6].color = vsx_color__(0,1,1,0.5);
  gmesh.vertices[0][7].color = vsx_color__(0,1,1,0.5);
printf("vert size: %d\n",gmesh.vertices.size());

  gmesh.add_face(0,3, 0,1, 0,2 );
  //gmesh.add_face(0,3, 0,1, 0,0 );

  gmesh.add_face(0,2, 0,7, 0,6 );
  //gmesh.add_face(0,2, 0,3, 0,7 );

  gmesh.add_face(0,6, 0,4, 0,5 );
  //gmesh.add_face(0,6, 0,7, 0,4 );

  gmesh.add_face(0,5, 0,0, 0,1 );
  //gmesh.add_face(0,5, 0,4, 0,0 );

printf("vert sizef: %d\n",gmesh.vertices.size());
  */

//  gmesh.vertices[0][0].coord = vsx_vector__(-1,-1,0);    gmesh.vertices[1][0].coord = vsx_vector__(0,-1,0);   gmesh.vertices[2][0].coord = vsx_vector__(1, -1,0);
//  gmesh.vertices[0][1].coord = vsx_vector__(-1,0,0);    gmesh.vertices[1][1].coord = vsx_vector__(0,0,1);   gmesh.vertices[2][1].coord = vsx_vector__(1, 0,0);
//  gmesh.vertices[0][2].coord = vsx_vector__(-1,1,0);    gmesh.vertices[1][2].coord = vsx_vector__(0,1,0);   gmesh.vertices[2][2].coord = vsx_vector__(1, 1,0);

  //gmesh.vertices[1][1].color = vsx_color__(1,0.4,0.4,0.5);


  gmesh.calculate_vertex_normals();
  //printf("vert size: %d\n",gmesh.vertices.size());

  gmesh.dump_vsx_mesh(&mesh);
  //printf("apa2000\n");
  mesh.data->calculate_face_centers();
  mesh.timestamp++;
  loading_done = true;
}

void vsx_cloud_plane::run() {
  if (mesh.data->faces.get_used()) return;
  simple_box();
}



