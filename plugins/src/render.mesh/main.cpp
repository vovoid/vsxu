#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "main.h"
#include "vsx_math_3d.h"
#include "vsx_font/vsx_font.h"
#include "vsx_glsl.h"

class vsx_module_mesh_render_line : public vsx_module {
  // in
	vsx_module_param_mesh* mesh_in;
	vsx_module_param_int* center;

	vsx_module_param_int* override_base_color;
	vsx_module_param_float4* base_color;
	vsx_module_param_float4* base_color_add;
	vsx_module_param_float4* center_color;
	vsx_module_param_float4* center_color_add;
	vsx_module_param_float* line_width;
	// out
	vsx_module_param_render* render_out;
	// internal
	vsx_mesh** mesh;
  vsx_matrix ma;
  vsx_vector upv;

  GLuint dlist;
  bool list_built;
  GLfloat prev_width;

public:
  void module_info(vsx_module_info* info) {
    info->identifier = "renderers;mesh;mesh_line_render";
    info->description = "Draws lines between points in a mesh.\n\
Faces data is ignored.\n\
If vertex colors are undefined you\n\
can override the colors - then the\n\
[base_color_add] parameter is ignored.";
    info->in_param_spec = "mesh_in:mesh,"
"base_options:complex{"
"line_width:float,"
"override_base_color:enum?no|yes,"
"base_color:float4,"
"base_color_add:float4"
"},"
"center_options:complex{"
"each_to_center:enum?no|yes,"
"center_color:float4,"
"center_color_add:float4"
"}";
  info->out_param_spec = "render_out:render";
  info->component_class = "render";
loading_done = true;
  }
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    override_base_color = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"override_base_color");
    override_base_color->set(0);
    line_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"line_width");
    line_width->set(1.0f);
    base_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color");
    base_color->set(1.0f,0);
    base_color->set(1.0f,1);
    base_color->set(1.0f,2);
    base_color->set(1.0f,3);
    base_color_add = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color_add");
    base_color_add->set(0.0f,0);
    base_color_add->set(0.0f,1);
    base_color_add->set(0.0f,2);
    base_color_add->set(0.0f,3);
    center_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"center_color");
    center_color->set(1.0f,0);
    center_color->set(1.0f,1);
    center_color->set(1.0f,2);
    center_color->set(1.0f,3);
    center_color_add = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"center_color_add");
    center_color_add->set(0.0f,0);
    center_color_add->set(0.0f,1);
    center_color_add->set(0.0f,2);
    center_color_add->set(0.0f,3);
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    center = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"each_to_center");
    center->set(0);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  }
	void output(vsx_module_param_abs* param) {
    //printf("mesh_out\n");
    glGetFloatv(GL_LINE_WIDTH,&prev_width);

    glLineWidth(line_width->get());
    #ifndef VSXU_OPENGL_ES
    glEnable(GL_LINE_SMOOTH);
    #endif
    mesh = mesh_in->get_addr();
    if (mesh) 
    {
        if (center->get()) {
          vsx_color l_center_color = vsx_color__(center_color->get(0)+center_color_add->get(0),center_color->get(1)+center_color_add->get(1),center_color->get(2)+center_color_add->get(2),center_color->get(3)+center_color_add->get(3));
					vsx_color main_color = vsx_color__(base_color->get(0)+base_color_add->get(0),base_color->get(1)+base_color_add->get(1),base_color->get(2)+base_color_add->get(2),base_color->get(3)+base_color_add->get(3));
					#ifdef VSXU_OPENGL_ES_1_0
					GLfloat fan_vertices[] = {
						0,0,0,
						0,0,0,
					};
					GLfloat fan_colors[] = {
						l_center_color.r,l_center_color.g,l_center_color.b,l_center_color.a,
						main_color.r,main_color.g,main_color.b,main_color.a,
					};
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
					glEnableClientState(GL_VERTEX_ARRAY);
					glEnableClientState(GL_COLOR_ARRAY);
					glVertexPointer(3, GL_FLOAT, 0, fan_vertices);
					glColorPointer(4, GL_FLOAT, 0, fan_colors);
					#endif
					#ifdef VSXU_OPENGL_ES_2_0
					//TODO
					#endif
					
					#ifndef VSXU_OPENGL_ES
					glBegin(GL_LINES);
					#endif
          if (override_base_color->get()) {
            
            for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i) {
              #ifdef VSXU_OPENGL_ES_1_0
                fan_vertices[3] = (*mesh)->data->vertices[i].x;
                fan_vertices[4] = (*mesh)->data->vertices[i].y;
                fan_vertices[5] = (*mesh)->data->vertices[i].z;
								glDrawArrays(GL_LINES, 0, 2);
							#endif
							#ifdef VSXU_OPENGL_ES_2_0
							//TODO
							#endif
							#ifndef VSXU_OPENGL_ES
								l_center_color.gl_color();
								glVertex3f(0.0f,0.0f,0.0f);
								main_color.gl_color();
								glVertex3f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z);
							#endif
            }
          } else {
            for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i) {
              #ifdef VSXU_OPENGL_ES_1_0
                fan_vertices[3] = (*mesh)->data->vertices[i].x;
                fan_vertices[4] = (*mesh)->data->vertices[i].y;
                fan_vertices[5] = (*mesh)->data->vertices[i].z;
                fan_colors[4] = (*mesh)->data->vertex_colors[i].r;
                fan_colors[5] = (*mesh)->data->vertex_colors[i].g;
                fan_colors[6] = (*mesh)->data->vertex_colors[i].b;
                fan_colors[7] = (*mesh)->data->vertex_colors[i].a;
								glDrawArrays(GL_LINES, 0, 2);
							#endif
							#ifdef VSXU_OPENGL_ES_2_0
							#endif
							
							#ifndef VSXU_OPENGL_ES
								l_center_color.gl_color();
								glVertex3f(0.0f,0.0f,0.0f);
								(*mesh)->data->vertex_colors[i].gl_color();
								glVertex3f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z);
							#endif
            }
          }
        } // center->get()
        else
        {
          #ifndef VSXU_OPENGL_ES
          glBegin(GL_LINE_STRIP);
          #endif
          #ifdef VSXU_OPENGL_ES_1_0
            glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            if (override_base_color->get()) {
              glColor4f(base_color->get(0)+base_color_add->get(0),base_color->get(1)+base_color_add->get(1),base_color->get(2)+base_color_add->get(2),base_color->get(3)+base_color_add->get(3));
              glDisableClientState(GL_COLOR_ARRAY);
            } else {
              glEnableClientState(GL_COLOR_ARRAY);
              glColorPointer(4, GL_FLOAT, 0, mesh->data->vertex_colors.get_pointer());
            }
            glEnableClientState(GL_VERTEX_ARRAY);
            glVertexPointer(3, GL_FLOAT, 0, mesh->data->vertices.get_pointer());
            glDrawArrays(GL_LINE_STRIP,0,mesh->data->vertices.size());
          #endif
          #ifdef VSXU_OPENGL_ES_2_0
          //TODO
          #endif
					
          #ifndef VSXU_OPENGL_ES
          if (override_base_color->get()) {
            glColor4f(base_color->get(0)+base_color_add->get(0),base_color->get(1)+base_color_add->get(1),base_color->get(2)+base_color_add->get(2),base_color->get(3)+base_color_add->get(3));
            for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i) {
              glVertex3f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z);
            }
          } else
          {
            for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i) {
              if ((*mesh)->data->vertex_colors.size())
              (*mesh)->data->vertex_colors[i].gl_color();
              glVertex3f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z);
            }
          }
          #endif
        }
      #ifndef VSXU_OPENGL_ES_2_0
      glEnd();
      #endif
      render_out->set(1);
    } else
    render_out->set(0);
  }
	void on_delete() {
  }
};


class vsx_module_render_dots : public vsx_module {
  // in
	vsx_module_param_mesh* mesh_in;
	vsx_module_param_float4* base_color;
	vsx_module_param_float* dot_size;
	// out
	vsx_module_param_render* render_out;
	// internal
	vsx_mesh** mesh;
  vsx_matrix ma;
  vsx_vector upv;

  GLuint dlist;
  bool list_built;
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "renderers;mesh;mesh_dot_render";
    info->description = "Renders a dot at each vertex in the mesh.";
    info->in_param_spec = "mesh_in:mesh,base_color:float4,dot_size:float";
  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
		loading_done = true;
  }
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    //mesh_in->set_p(mesh);
    base_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color");
    base_color->set(1.0f,0);
    base_color->set(1.0f,1);
    base_color->set(1.0f,2);
    base_color->set(1.0f,3);
    dot_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"dot_size");
    dot_size->set(1.0f);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
  }
  GLint prev_psize;
	void output(vsx_module_param_abs* param) {
    //printf("mesh_out\n");
    glGetIntegerv(GL_POINT_SIZE, &prev_psize);
    glPointSize(dot_size->get());
    glEnable(GL_POINT_SMOOTH);
    mesh = mesh_in->get_addr();
    if (mesh) {
      glColor4f(base_color->get(0),base_color->get(1),base_color->get(2),base_color->get(3));

      glEnableClientState(GL_VERTEX_ARRAY);
      glVertexPointer(3, GL_FLOAT, sizeof(vsx_vector), (*mesh)->data->vertices.get_pointer());
      glDrawArrays(GL_POINTS,0,(*mesh)->data->vertices.size());
      glDisableClientState(GL_VERTEX_ARRAY);
      
      /*glBegin(GL_POINTS);
	      for (unsigned long i = 0; i < mesh->data->vertices.size(); ++i) {
	        glVertex3f(mesh->data->vertices[i].x,mesh->data->vertices[i].y,mesh->data->vertices[i].z);
	      }
      glEnd();*/
    }
    glPointSize(prev_psize);

    render_out->set(1);
  }
	void on_delete() {
  }
};


class vsx_module_render_billboards : public vsx_module {
  // in
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_float4* base_color;
  vsx_module_param_float* dot_size;

  vsx_module_param_string* i_vertex_program;
  vsx_module_param_string* i_fragment_program;
  vsx_module_param_int* use_display_list;
  // out
  vsx_module_param_render* render_out;
  // internal
  vsx_mesh** mesh;
  vsx_matrix ma;
  vsx_vector upv;
  

  GLuint dlist;
  bool list_built;
  vsx_glsl shader;

  void param_set_notify(const vsx_string& name) {
    if ((name == "vertex_program" || name == "fragment_program")) {
      //printf("PARAM_SET_NOTIFY %s\n\n",name.c_str());
      shader.vertex_program = i_vertex_program->get();
      shader.fragment_program = i_fragment_program->get();
      message = shader.link();//"module||"+shader.get_log();
      if (message.size() == 0) {
        redeclare_in = true;
        message = "module||ok";
      }
    }
  }

public:
  void module_info(vsx_module_info* info) {
    info->identifier = "renderers;mesh;mesh_dot_billboards";
    info->description = "Renders a texture billboard at each vertex in the mesh.";
    info->in_param_spec = "mesh_in:mesh,base_color:float4,dot_size:float,use_display_list:enum?no|yes,shader_params:complex{vertex_program:string,fragment_program:string"+shader.get_param_spec()+"}";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
    loading_done = true;
  }

  void redeclare_in_params(vsx_module_param_list& in_parameters)
  {
    shader.declare_params(in_parameters);
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    //mesh_in->set_p(mesh);
    base_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color");
    base_color->set(1.0f,0);
    base_color->set(1.0f,1);
    base_color->set(1.0f,2);
    base_color->set(1.0f,3);
    dot_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"dot_size");
    dot_size->set(1.0f);
    use_display_list = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_display_list");
    use_display_list->set(0);

    i_fragment_program = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"fragment_program");
    i_fragment_program->set(shader.fragment_program);
    i_vertex_program = (vsx_module_param_string*)in_parameters.create(VSX_MODULE_PARAM_ID_STRING,"vertex_program");
    i_vertex_program->set(shader.vertex_program.c_str());
    
  }  
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    shader.vertex_program = ""
      "varying float particle_alpha;\n"
      "\n"
      "void main(void)\n"
      "{\n"
      "  gl_Position = ftransform();\n"
      "  float vertDist = distance(vec3(gl_Position.x,gl_Position.y,gl_Position.z), vec3(0.0,0.0,0.0));\n"
      "  float dist_alpha;\n"
      "  dist_alpha = pow(1 / vertDist,1.1);\n"
      "  gl_PointSize = 460.155 * dist_alpha * (1.0+abs((sin(gl_Vertex.x*10.0) * sin (gl_Vertex.z*20.0))));\n"
      "  particle_alpha = pow(dist_alpha,1.2);\n"
      "  if (gl_PointSize < 1.0) particle_alpha = gl_PointSize;\n"
      "}"
    ;
    shader.fragment_program =
      "uniform sampler2D _tex;\n"
      "varying float particle_alpha;\n"
      "void main(void)\n"
      "{\n"
        "vec2 l_uv=gl_PointCoord;\n"
        "const vec2 l_offset = vec2(0.5,0.5);\n"
        "l_uv-=l_offset;\n"
        "l_uv=vec2(vec4(l_uv,0.0,1.0));\n"
        "l_uv+=l_offset;\n"
        "vec4 a = texture2D(_tex, l_uv);\n"
        "gl_FragColor = vec4(a.r, a.g, a.b, a.a * min(particle_alpha,1.0) * 0.1);\n"
      "}\n"
    ;

    loading_done = true;
    list_built = false;

    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    redeclare_in_params(in_parameters);
  }
  void output(vsx_module_param_abs* param) {
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    #if defined(__linux__) || defined (WIN32)
      shader.begin();
      GLint viewport[4];
      glGetIntegerv (GL_VIEWPORT, viewport);
      float t_res_x = (float)abs(viewport[2] - viewport[0]);
      
      
      if (shader.v_map.find("_vx") != shader.v_map.end())
      {
        //printf("found vx\n");
        vsx_module_param_float* p = (vsx_module_param_float*)shader.v_map["_vx"]->module_param;
        if (p) p->set(t_res_x);
      }      
    shader.set_uniforms();
    glEnable( GL_POINT_SPRITE_ARB );
    glEnable(GL_POINT_SMOOTH);
    mesh = mesh_in->get_addr();
    if (mesh) {
      if (!use_display_list->get() && list_built) {
        list_built = false;
        glDeleteLists(dlist,1);
      }
      //glColor4f(base_color->get(0),base_color->get(1),base_color->get(2),base_color->get(3));

      //glEnableClientState(GL_VERTEX_ARRAY);
      //glVertexPointer(3, GL_FLOAT, sizeof(vsx_vector), mesh->data->vertices.get_pointer());

      if (!list_built)
      {
        // init list -------------------------------------
        if (use_display_list->get() && list_built == false) {
          dlist = glGenLists(1);
          glNewList(dlist,GL_COMPILE);
        }
          // draw --------------------
          glBegin(GL_POINTS);
          // ugly hack to support vertex id
          float vid = 0.0f;
          float dvdi = 1.0f / (float)(*mesh)->data->vertices.size();
          for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i) {
            glVertex4f((*mesh)->data->vertices[i].x,(*mesh)->data->vertices[i].y,(*mesh)->data->vertices[i].z,vid);
            vid += dvdi;
          }
          glEnd();
          //glDrawArrays(GL_POINTS,0,mesh->data->vertices.size());
          // -------------------------
        if (use_display_list->get() && list_built == false) {
          glEndList();
          list_built = true;
        }
      } else
      {
        glCallList(dlist);
      }
      // kill list -------------------------------------
      //glDisableClientState(GL_VERTEX_ARRAY);
    }
    glDisable(GL_POINT_SMOOTH);
    glDisable( GL_POINT_SPRITE_ARB );
    shader.end();
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);

    render_out->set(1);
  }
  void on_delete() {
  }
};



class vsx_module_render_face_id : public vsx_module {
  // in
	vsx_module_param_mesh* mesh_in;
	vsx_module_param_float4* base_color;
  vsx_module_param_float3* min_box;
  vsx_module_param_float3* max_box;
	vsx_module_param_float* font_size;
	// out
	vsx_module_param_render* render_out;
	// internal
	vsx_mesh** mesh;
  vsx_matrix ma;
  vsx_vector upv;
  vsx_font* myf;

  GLuint dlist;
  bool list_built;
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "renderers;mesh;mesh_face_id_render";
    info->description = "";
    info->in_param_spec = "mesh_in:mesh,base_color:float4,font_size:float,min_box:float3,max_box:float3";
  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
		loading_done = true;
  }
	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    //mesh_in->set_p(mesh);
    base_color = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"base_color");
    base_color->set(1.0f,0);
    base_color->set(1.0f,1);
    base_color->set(1.0f,2);
    base_color->set(1.0f,3);
    min_box = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"min_box");
    max_box = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"max_box");
    min_box->set(-1.0f,0);
    min_box->set(-1.0f,1);
    min_box->set(-1.0f,2);

    max_box->set( 1.0f,0);
    max_box->set( 1.0f,1);
    max_box->set( 1.0f,2);
    font_size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"font_size");
    font_size->set(1.0f);
    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
		myf = new vsx_font();
		myf->init("/home/jaw/vsxu/vsxu/share/font/font-ascii.png");
		//myf->mode_2d = true;

  }
	void output(vsx_module_param_abs* param) {
    //printf("mesh_out\n");
    mesh = mesh_in->get_addr();
    if (mesh) {

      float minx = min_box->get(0);
      float miny = min_box->get(1);
      float minz = min_box->get(2);

      float maxx = max_box->get(0);
      float maxy = max_box->get(1);
      float maxz = max_box->get(2);
      float fs = font_size->get();

      glColor4f(base_color->get(0),base_color->get(1),base_color->get(2),base_color->get(3));
	    for (unsigned long i = 0; i < (*mesh)->data->vertices.size(); ++i) {
      	//glPushMatrix();
	        //glTranslateff(mesh->data->vertices[i].x,mesh->data->vertices[i].y,mesh->data->vertices[i].z);
	      if ((*mesh)->data->vertices[i].x > minx && (*mesh)->data->vertices[i].x < maxx)
	      {
	        if ((*mesh)->data->vertices[i].y > miny && (*mesh)->data->vertices[i].y < maxy)
	        {
	          if ((*mesh)->data->vertices[i].z > minz && (*mesh)->data->vertices[i].z < maxz)
	          {
              myf->print(
                  (*mesh)->data->vertices[i],
                  i2s(i), 0.007f * fs
              );
	          }
	        }
	      }

      	//glPopMatrix();
	    }

    }
    //glPointSize(prev_psize);

    render_out->set(1);
  }
	void on_delete() {
  }
};
#endif

class face_holder {
public:
  double dist;
  int id;
  bool operator < (const face_holder& a) {
      return dist > a.dist;
  }
  //face_holder() : dist(0),id(0) {};
};

class vsx_module_render_mesh : public vsx_module {
  // in
  vsx_module_param_texture* tex_a;
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_mesh* particle_cloud;
  vsx_module_param_int* vertex_colors;
  vsx_module_param_int* use_vertex_colors;
  vsx_module_param_int* use_display_list;
  vsx_module_param_int* particles_size_center;
  vsx_module_param_particlesystem* particles_in;


  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_mesh** particle_mesh;
  vsx_mesh** mesh;
  vsx_texture** ta;
  bool m_normals, m_tex, m_colors;
  vsx_matrix mod_mat, proj_mat;
  vsx_particlesystem* particles;
  vsx_matrix ma;
  vsx_vector upv;

  GLuint dlist;
  bool list_built;
  unsigned long prev_mesh_timestamp;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;mesh;mesh_basic_render";
    info->in_param_spec = "tex_a:texture,mesh_in:mesh,"
                          "particles:particlesystem,"
                          "particle_cloud:mesh,"
                          "options:complex{"
                          "vertex_colors:enum?no|yes,"
                          "use_display_list:enum?no|yes,"
                          "use_vertex_colors:enum?no|yes,"
                          "particles_size_center:enum?no|yes,"
                          "}";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    tex_a = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"tex_a");
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    vertex_colors = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"vertex_colors");
    vertex_colors->set(0);
    use_display_list = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_display_list");
    use_display_list->set(0);
    use_vertex_colors = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_vertex_colors");
    use_vertex_colors->set(1);
    m_colors = true;
    particles_size_center = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"particles_size_center");
    particles_size_center->set(0);
    particles_in = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particles");
    particle_cloud = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"particle_cloud");

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
    upv = vsx_vector(0,1,0);
    //quadratic=gluNewQuadric();                          // Create A Pointer To The Quadric Object
    //gluQuadricNormals(quadratic, GLU_SMOOTH);           // Create Smooth Normals
    //gluQuadricTexture(quadratic, GL_TRUE);              // Create Texture Coords
    list_built = false;
    prev_mesh_timestamp = 0xFFFFFF;
  }

  void output(vsx_module_param_abs* param) {
    GLint dmask;
    glGetIntegerv(GL_DEPTH_WRITEMASK,&dmask);
    mesh = mesh_in->get_addr();
    if (mesh) {
      if (!(*mesh)->data) return;
      if ((*mesh)->data->faces.get_used()) {
        ta = tex_a->get_addr();
        //printf("renderer output\n");

        //GLfloat center[] = {0.0f, 0.0f, 0.0f, 1.0f};
        //GLfloat deep[] = {0.0f, 0.0f, 1.0f, 1.0f};
        //GLfloat mod_mat_inv[16];
        //vsx_matrix mod_mat_inv = mod_mat;
        //mod_mat_inv.assign_inverse(&mod_mat);

        //inv_mat(mod_mat, mod_mat_inv);
        //mat_vec_mult(mod_mat_inv, center, start);
        //mat_vec_mult(mod_mat_inv, deep, end);

        //vsx_matrix m;

        //glGetFloatv(GL_PROJECTION_MATRIX, m.m);
        // we assume that the camera is 10 000 units away from the object
        //vsx_vector a(0,0,100);
        //vsx_vector b = m.multiply_vector(a);
        // b is now the camera position
        //b.normalize();
        //b.x*=10;
        //b.y*=10;
        //b.z*=10;
        //b.dump("camera_pos");

        // make sure we have centers to sort on
        //if (!mesh.data->face_centers.size()) {
          //mesh.data->calculate_face_centers();
        //}
        // loop through all the faces and calculate the distance from the camera
        //for (int i = 0; i < mesh.data->face_centers.size(); ++i) {
          //f_distances[i].dist = mesh.data->face_centers[i].distance(b);
          //f_distances[i].dist = mesh.data->face_centers[i].dot_product(&sort_vec);
          //f_distances[i].id = i;
          /*glPushMatrix();
          glTranslatef(mesh.data->face_centers[i].x,mesh.data->face_centers[i].y,mesh.data->face_centers[i].z);
          glColor4f(1,1,1,1);
          gluSphere(quadratic,0.1,20,20);
          glPopMatrix();
          printf("id: %d  dist: %f\n",i, f_distances[i].dist);*/
        //}
        //printf("-----\n");
        // we have the distances, now time to sort
        //std::sort(f_distances, f_distances.get_end_pointer());
        //fquicksort(f_distances.get_pointer(),f_distances.size());
        //for (int i = 0; i < mesh.data->face_centers.size(); ++i) {
          //printf("id: %d  dist: %f\n",f_distances[i].id, f_distances[i].dist);
          //f_result[i] = mesh.data->faces[f_distances[i].id];
        //}
        //printf("f_Result.size: %d\n",f_result.size());


        if (ta) {
          vsx_transform_obj& texture_transform = *(*ta)->get_transform();

          glMatrixMode(GL_TEXTURE);
          glPushMatrix();

          if ((*ta)->get_transform())
          texture_transform();
          (*ta)->bind();
        }// else printf("not using texture.\n");


        if (vertex_colors->get()) {
          glEnable(GL_COLOR_MATERIAL);
          //printf("vertex colors %d\n");
        }

#ifndef VSXU_OPENGL_ES

        if (!use_display_list->get() && list_built) {
          list_built = false;
          glDeleteLists(dlist,1);
        }
#endif
        prev_mesh_timestamp = (*mesh)->timestamp;
        //printf("texcoords: %d\n",mesh->data->vertex_tex_coords.get_used());

        //printf("faces: %d\n",mesh->data->faces.get_used());
        //printf("vertices: %d\n",mesh->data->vertices.get_used());
        if (use_vertex_colors->get())
        {
          if ((*mesh)->data->vertex_colors.get_used()) {
            glColorPointer(4,GL_FLOAT,0,(*mesh)->data->vertex_colors.get_pointer());
            glEnableClientState(GL_COLOR_ARRAY);
            m_colors = true;
          } else
          m_colors = false;
        }

        if ((*mesh)->data->vertex_normals.get_used()) {
          glNormalPointer(GL_FLOAT,0,(*mesh)->data->vertex_normals.get_pointer());
          glEnableClientState(GL_NORMAL_ARRAY);
          m_normals = true;
        } else
        m_normals = false;

        //#ifdef FOO
        //printf("used: %d\n",(*mesh)->data->vertex_tex_coords.get_used());
        if ((*mesh)->data->vertex_tex_coords.get_used()) {
          glTexCoordPointer(2,GL_FLOAT,0,(*mesh)->data->vertex_tex_coords.get_pointer());
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          m_tex = true;
        } else
        //#endif
        m_tex = false;


        glVertexPointer(3,GL_FLOAT,0,(*mesh)->data->vertices.get_pointer());
        glEnableClientState(GL_VERTEX_ARRAY);
        if (!list_built) {

#ifndef VSXU_OPENGL_ES
          if (use_display_list->get() && list_built == false) {
            dlist = glGenLists(1);
            glNewList(dlist,GL_COMPILE);
          }
#endif

#ifndef VSXU_OPENGL_ES
            glDrawElements(GL_TRIANGLES,(*mesh)->data->faces.get_used()*3,GL_UNSIGNED_INT,(*mesh)->data->faces.get_pointer());
#else
					glDrawElements(GL_TRIANGLES,mesh->data->faces.get_used()*3,GL_UNSIGNED_SHORT,mesh->data->faces.get_pointer());

#endif
          if (use_display_list->get() && list_built == false) {
            glEndList();
            list_built = true;
          }
#ifndef VSXU_OPENGL_ES
        }
        else {
          //printf("cl");
          particles = particles_in->get_addr();
          particle_mesh = particle_cloud->get_addr();
          if (particle_mesh)
          {
            float ss;
            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();
            for (unsigned long i = 0; i < (*particle_mesh)->data->vertices.size(); ++i) {
              glPushMatrix();
              glTranslatef(
                (*particle_mesh)->data->vertices[i].x,
                (*particle_mesh)->data->vertices[i].y,
                (*particle_mesh)->data->vertices[i].z
              );
              glCallList(dlist);
              glPopMatrix();
            }
          } else
          if (particles) {
            float ss;
            glMatrixMode(GL_MODELVIEW);
            if (particles->particles)
            for (unsigned long i = 0; i < particles->particles->size(); ++i) {
              (*particles->particles)[i].color.a = (1-((*particles->particles)[i].time/(*particles->particles)[i].lifetime));
              (*particles->particles)[i].color.gl_color();
              glPushMatrix();
                //ma.rotation_from_vectors(&(*particles->particles)[i].rotation, &upv);
                ma = (*particles->particles)[i].rotation.matrix();
                if (particles_size_center->get())
                {
                  glMultMatrixf(ma.m);
                  ss = (*particles->particles)[i].pos.norm();
                } else {
                  glTranslatef(
                    (*particles->particles)[i].pos.x,
                    (*particles->particles)[i].pos.y,
                    (*particles->particles)[i].pos.z
                  );
                  ss = (*particles->particles)[i].size*(1-((*particles->particles)[i].time/(*particles->particles)[i].lifetime));
                }
                glScalef(
                  ss,
                  ss,
                  ss
                );
                glMultMatrixf(ma.m);
                glCallList(dlist);
              glPopMatrix();

            }
          } else
          glCallList(dlist);
        }
#endif





















        glDisableClientState(GL_VERTEX_ARRAY);

        if (m_normals) {
          glDisableClientState(GL_NORMAL_ARRAY);
        }

        if (m_tex) {
          glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if (m_colors) {
          glDisableClientState(GL_COLOR_ARRAY);
        }

        if (ta) {
          (*ta)->_bind();
          glMatrixMode(GL_TEXTURE);
          glPopMatrix();
        }
        if (vertex_colors->get()) {
          glDisable(GL_COLOR_MATERIAL);
        }

        render_result->set(1);
      } else
      render_result->set(0);

      //  if (!ma) {
      //    render_result->set(0);
    //      return;
    //    }
        //if (!ta) {
          // no texture attached :/
    //      printf("mesh data %d\n",ma.data);
      //printf("number of faces: %d\n",ma.data->faces.get_used());
    /*        glBegin(GL_TRIANGLES);
            //ma->data->faces.get_used()-10;
            if (ma.data->faces.get_used())
            for (unsigned long i = 0; i < ma.data->faces.get_used()-1; ++i) {
              //printf(" %f",ma.data->vertices[ma.data->faces[i].c].x);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].a].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].a].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].a].z);*/
    /*          glNormal3f(
                ma.data->vertex_normals[ma.data->faces[i].a].x,
                ma.data->vertex_normals[ma.data->faces[i].a].y,
                ma.data->vertex_normals[ma.data->faces[i].a].z);
              glVertex3f(
                ma.data->vertices[ma.data->faces[i].a].x,
                ma.data->vertices[ma.data->faces[i].a].y,
                ma.data->vertices[ma.data->faces[i].a].z);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].b].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].b].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].b].z);*/
    /*          glNormal3f(
                ma.data->vertex_normals[ma.data->faces[i].b].x,
                ma.data->vertex_normals[ma.data->faces[i].b].y,
                ma.data->vertex_normals[ma.data->faces[i].b].z);
              glVertex3f(
                ma.data->vertices[ma.data->faces[i].b].x,
                ma.data->vertices[ma.data->faces[i].b].y,
                ma.data->vertices[ma.data->faces[i].b].z);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].c].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].c].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].c].z);*/
    /*          glNormal3f(
                ma.data->vertex_normals[ma.data->faces[i].c].x,
                ma.data->vertex_normals[ma.data->faces[i].c].y,
                ma.data->vertex_normals[ma.data->faces[i].c].z);
              glVertex3f(
                ma.data->vertices[ma.data->faces[i].c].x,
                ma.data->vertices[ma.data->faces[i].c].y,
                ma.data->vertices[ma.data->faces[i].c].z);
            }
            glEnd();
            //ta->_bind();
            render_result->set(1);
      */

    /*    } else {
          //printf("h");
      glColor3f(1, 1, 1);
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glLoadIdentity();

      vsx_transform_obj& texture_transform = *ta->get_transform();
      if (ta->get_transform()) texture_transform();
      printf("number of vertices: %d\n",ma->data->vertices.get_used());
      float time = engine->vtime*4;


      glTranslatef(
        (float)sin(time * 0.0053f),
        (float)sin(time * 0.003f), 0);

      glRotatef(time * 1, 0, 0, 1);
      glRotatef(sin(time * 0.036f) * 60, 0, 1, 0);

      glScalef(
        1.850f + (float)sin(time * 0.0014f) * 0.07f,
        1.850f + (float)sin(time * 0.0010f) * 0.07f,
        1.850f + (float)sin(time * 0.0007f) * 0.07f);      */
          // we have texture

          /*ta->bind();
          if (ma->data->vertex_tex_coords.get_used()) {
            glBegin(GL_TRIANGLES);
            for (unsigned long i = 0; i < ma->data->faces.get_used(); ++i) {
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].a].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].a].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].a].z);
              glVertex3f(
                ma->data->vertices[ma->data->faces[i].a].x,
                ma->data->vertices[ma->data->faces[i].a].y,
                ma->data->vertices[ma->data->faces[i].a].z);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].b].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].b].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].b].z);
              glVertex3f(
                ma->data->vertices[ma->data->faces[i].b].x,
                ma->data->vertices[ma->data->faces[i].b].y,
                ma->data->vertices[ma->data->faces[i].b].z);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].c].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].c].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].c].z);
              glVertex3f(
                ma->data->vertices[ma->data->faces[i].c].x,
                ma->data->vertices[ma->data->faces[i].c].y,
                ma->data->vertices[ma->data->faces[i].c].z);
            }
            glEnd();
            ta->_bind();
            render_result->set(1);
            glPopMatrix();
          } else {

          }    */
          // no texture coordinates, buuuuu :(
    //    }
      //}
    }
    //printf("e\n");
  }

  void stop() {
#ifndef VSXU_OPENGL_ES
    if (list_built) {
      glDeleteLists(dlist,1);
    }
#endif
    list_built = false;
  }
};




///------------------------------------------------------------------------------------
#ifndef VSXU_OPENGL_ES
class vsx_module_render_mesh_array : public vsx_module {
  //GLUquadricObj *quadratic;
  // in
  vsx_module_param_texture* tex_a;
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_int* vertex_colors;

  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_mesh* mesh;
  vsx_particlesystem* particles;
  vsx_texture** ta;
  bool m_normals, m_tex, m_colors;
  vsx_matrix ma;
  vsx_vector upv;

  GLuint dlist;
  bool list_built;
public:

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;mesh;mesh_render_array";
    info->in_param_spec = "tex_a:texture,mesh_in:mesh,vertex_colors:enum?no|yes";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    tex_a = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"tex_a");
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    //particles_in = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particles_in");
    vertex_colors = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"vertex_colors");
    vertex_colors->set(0);
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
    dlist = glGenLists(1);
    list_built = false;
    upv = vsx_vector(0,1);
  }

  void output(vsx_module_param_abs* param) {
    /*mesh = mesh_in->get_addr();
    if (mesh) {
      if (mesh->data->faces.get_used()) {
        particles = particles_in->get_addr();
        if (particles) {
          ta = tex_a->get_addr();
          if (ta) {
            vsx_transform_obj& texture_transform = *ta->get_transform();

            glMatrixMode(GL_TEXTURE);
            glPushMatrix();

            if (ta->get_transform())
            texture_transform();
            ta->bind();
          }
          if (vertex_colors->get()) {
            glEnable(GL_COLOR_MATERIAL);
          }
          if (!list_built) {
            glPushMatrix();
            glScalef(0,0,0);
            glNewList(dlist,GL_COMPILE);
            if (mesh->data->vertex_colors.get_used()) {
              glColorPointer(4,GL_FLOAT,0,mesh->data->vertex_colors.get_pointer());
              glEnableClientState(GL_COLOR_ARRAY);
              m_colors = true;
            } else m_colors = false;

            if (mesh->data->vertex_normals.get_used()) {
              glNormalPointer(GL_FLOAT,0,mesh->data->vertex_normals.get_pointer());
              glEnableClientState(GL_NORMAL_ARRAY);
              m_normals = true;
            } else m_normals = false;

            if (mesh->data->vertex_tex_coords.get_used()) {
              glTexCoordPointer(3,GL_FLOAT,0,mesh->data->vertex_tex_coords.get_pointer());
              glEnableClientState(GL_TEXTURE_COORD_ARRAY);
              m_tex = true;
            } else m_tex = false;

            glVertexPointer(3,GL_FLOAT,0,mesh->data->vertices.get_pointer());
            glEnableClientState(GL_VERTEX_ARRAY);
              glDrawElements(GL_TRIANGLES,mesh->data->faces.get_used()*3,GL_UNSIGNED_INT,mesh->data->faces.get_pointer());
            glDisableClientState(GL_VERTEX_ARRAY);

            if (m_normals) {
              glDisableClientState(GL_NORMAL_ARRAY);
            }

            if (m_tex) {
              glDisableClientState(GL_TEXTURE_COORD_ARRAY);
            }

            if (m_colors) {
              glDisableClientState(GL_COLOR_ARRAY);
            }
            glEndList();
            glPopMatrix();
            list_built = true;
          }
          glMatrixMode(GL_MODELVIEW);
          glPushMatrix();
          for (int i = 0; i < particles->particles->size(); ++i) {
            glPushMatrix();
            glTranslatef(
              (*particles->particles)[i].pos.x,
              (*particles->particles)[i].pos.y,
              (*particles->particles)[i].pos.z
            );
            ma.rotation_from_vectors(&(*particles->particles)[i].rotation, &upv);
            glMatrixMode(GL_MODELVIEW);
            glMultMatrixf(ma.m);
            glScalef(
              (*particles->particles)[i].size,
              (*particles->particles)[i].size,
              (*particles->particles)[i].size
            );
            glCallList(dlist);
            glPopMatrix();
          }
          glPopMatrix();

          if (ta) ta->_bind();
          if (vertex_colors->get()) {
            glDisable(GL_COLOR_MATERIAL);
          }
          glMatrixMode(GL_TEXTURE);
          glPopMatrix();

          render_result->set(1);
          return;
        }
      }
    }
    render_result->set(0);      */
  }
};

///------------------------------------------------------------------------------------

class vsx_module_render_mesh_zsort : public vsx_module {
  // in
  vsx_module_param_texture* tex_a;
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_int* vertex_colors;

  // out
  vsx_module_param_render* render_result;
  // internal
  vsx_mesh** mesh;
  vsx_texture** ta;
  bool m_normals, m_tex, m_colors;
  vsx_matrix mod_mat, proj_mat;
  vsx_avector_nd<face_holder> f_distances;
  vsx_avector_nd<vsx_face> f_result;
public:
  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;mesh;mesh_transparency_render";
    info->in_param_spec = "tex_a:texture,mesh_in:mesh,vertex_colors:enum?no|yes";
    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    tex_a = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"tex_a");
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    //mesh_in->set(mesh);
    vertex_colors = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"vertex_colors");
    vertex_colors->set(0);
    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);
    //quadratic=gluNewQuadric();                          // Create A Pointer To The Quadric Object
  //  gluQuadricNormals(quadratic, GLU_SMOOTH);           // Create Smooth Normals
  //  gluQuadricTexture(quadratic, GL_TRUE);              // Create Texture Coords

  }


  static int fpartition(face_holder* a, int first, int last) {
    face_holder pivot = a[first];
    int lastS1 = first;
    int firstUnknown = first + 1;
    while (firstUnknown <= last) {
      if (a[firstUnknown] < pivot) {
        lastS1++;
        face_holder b = a[firstUnknown];
        a[firstUnknown] = a[lastS1];
        a[lastS1] = b;
        //printf("a");
        //std::swap(a[firstUnknown], a[lastS1]);
      }
      firstUnknown++;
    }
    face_holder b = a[first];
    a[first] = a[lastS1];
    a[lastS1] = b;
    //std::swap(a[first], a[lastS1]);
    return lastS1;
  }

  static void fquicksort(face_holder* a, int first, int last) {
    if (first < last) {
      int pivotIndex = fpartition(a, first, last);
      fquicksort(a, first, pivotIndex - 1);
      fquicksort(a, pivotIndex + 1, last);
    }
  }

  static void fquicksort(face_holder* a, int aSize) {
    fquicksort(a, 0, aSize - 1);
  }

  void output(vsx_module_param_abs* param) {
    //printf("renderer output\n");
    //if (tex_a && mesh_in) {

    ta = tex_a->get_addr();
    //  vsx_mesh* ma = mesh_in->get_addr();
    mesh = mesh_in->get_addr();
    if (mesh) {
      if ((*mesh)->data->faces.get_used()) {



      glGetFloatv(GL_MODELVIEW_MATRIX, mod_mat.m);
      glGetFloatv(GL_PROJECTION_MATRIX, proj_mat.m);

      glPushMatrix();
      glLoadIdentity();
      glMultMatrixf(proj_mat.m);
      glMultMatrixf(mod_mat.m);
      glGetFloatv(GL_PROJECTION_MATRIX, mod_mat.m);
      glPopMatrix();

      vsx_vector center(0);
      vsx_vector deep(0,0,1);
      vsx_vector istart = mod_mat.multiply_vector(center);
      vsx_vector end = mod_mat.multiply_vector(deep);
      vsx_vector sort_vec = end - istart;
      //GLfloat center[] = {0.0f, 0.0f, 0.0f, 1.0f};
      //GLfloat deep[] = {0.0f, 0.0f, 1.0f, 1.0f};
      //GLfloat mod_mat_inv[16];
      //vsx_matrix mod_mat_inv = mod_mat;
      //mod_mat_inv.assign_inverse(&mod_mat);

      //inv_mat(mod_mat, mod_mat_inv);
      //mat_vec_mult(mod_mat_inv, center, start);
      //mat_vec_mult(mod_mat_inv, deep, end);

        //vsx_matrix m;

        //glGetFloatv(GL_PROJECTION_MATRIX, m.m);
        // we assume that the camera is 10 000 units away from the object
        //vsx_vector a(0,0,100);
        //vsx_vector b = m.multiply_vector(a);
        // b is now the camera position
        //b.normalize();
        //b.x*=10;
        //b.y*=10;
        //b.z*=10;
        //b.dump("camera_pos");

        // make sure we have centers to sort on
        if (!(*mesh)->data->face_centers.size()) {
          (*mesh)->data->calculate_face_centers();
        }
        // loop through all the faces and calculate the distance from the camera
        for (unsigned long i = 0; i < (*mesh)->data->face_centers.size(); ++i) {
          //f_distances[i].dist = (*mesh)->data->face_centers[i].distance(b);
          f_distances[i].dist = (*mesh)->data->face_centers[i].dot_product(&sort_vec);
          f_distances[i].id = i;
          /*glPushMatrix();
          glTranslatef((*mesh)->data->face_centers[i].x,(*mesh)->data->face_centers[i].y,(*mesh)->data->face_centers[i].z);
          glColor4f(1,1,1,1);
          gluSphere(quadratic,0.1,20,20);
          glPopMatrix();
          printf("id: %d  dist: %f\n",i, f_distances[i].dist);*/
        }
        //printf("-----\n");
        // we have the distances, now time to sort
        //std::sort(f_distances, f_distances.get_end_pointer());
        fquicksort(f_distances.get_pointer(),f_distances.size());
        for (unsigned long i = 0; i < (*mesh)->data->face_centers.size(); ++i) {
          //printf("id: %d  dist: %f\n",f_distances[i].id, f_distances[i].dist);
          f_result[i] = (*mesh)->data->faces[f_distances[i].id];
          //f_result[(*mesh)->data->face_centers.size()-1-i] = (*mesh)->data->faces[f_distances[i].id];
        }
        //printf("f_Result.size: %d\n",f_result.size());


        if (vertex_colors->get()) {
          glEnable(GL_COLOR_MATERIAL);
        }

        if (ta) {
          vsx_transform_obj& texture_transform = *(*ta)->get_transform();

          glMatrixMode(GL_TEXTURE);
          glPushMatrix();

          if ((*ta)->get_transform())
          {
                  //printf("texture\n");

            texture_transform();
          }
          (*ta)->bind();
        }

        //printf("faces: %d\n",(*mesh)->data->faces.get_used());

        if ((*mesh)->data->vertex_colors.get_used()) {
          glColorPointer(4,GL_FLOAT,0,(*mesh)->data->vertex_colors.get_pointer());
          glEnableClientState(GL_COLOR_ARRAY);
          m_colors = true;
        } else m_colors = false;

        if ((*mesh)->data->vertex_normals.get_used()) {
          glNormalPointer(GL_FLOAT,0,(*mesh)->data->vertex_normals.get_pointer());
          glEnableClientState(GL_NORMAL_ARRAY);
          m_normals = true;
        } else m_normals = false;

        if ((*mesh)->data->vertex_tex_coords.get_used()) {
          glTexCoordPointer(2,GL_FLOAT,0,(*mesh)->data->vertex_tex_coords.get_pointer());
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          m_tex = true;
        } else m_tex = false;

        glVertexPointer(3,GL_FLOAT,0,(*mesh)->data->vertices.get_pointer());
        glEnableClientState(GL_VERTEX_ARRAY);
          //glDrawElements(GL_TRIANGLES,(*mesh)->data->faces.get_used()*3,GL_UNSIGNED_INT,(*mesh)->data->faces.get_pointer());
          glDrawElements(GL_TRIANGLES,f_result.get_used()*3,GL_UNSIGNED_INT,f_result.get_pointer());
        glDisableClientState(GL_VERTEX_ARRAY);

        if (m_normals) {
          glDisableClientState(GL_NORMAL_ARRAY);
        }

        if (m_tex) {
          glDisableClientState(GL_TEXTURE_COORD_ARRAY);
        }

        if (m_colors) {
          glDisableClientState(GL_COLOR_ARRAY);
        }
        if (ta) (*ta)->_bind();
        if (vertex_colors->get()) {
          glDisable(GL_COLOR_MATERIAL);
        }
        glMatrixMode(GL_TEXTURE);
        glPopMatrix();

        render_result->set(1);
        return;
      }
    }
    render_result->set(0);

      //  if (!ma) {
      //    render_result->set(0);
    //      return;
    //    }
        //if (!ta) {
          // no texture attached :/
    //      printf("mesh data %d\n",ma.data);
      //printf("number of faces: %d\n",ma.data->faces.get_used());
    /*        glBegin(GL_TRIANGLES);
            //ma->data->faces.get_used()-10;
            if (ma.data->faces.get_used())
            for (unsigned long i = 0; i < ma.data->faces.get_used()-1; ++i) {
              //printf(" %f",ma.data->vertices[ma.data->faces[i].c].x);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].a].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].a].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].a].z);*/
    /*          glNormal3f(
                ma.data->vertex_normals[ma.data->faces[i].a].x,
                ma.data->vertex_normals[ma.data->faces[i].a].y,
                ma.data->vertex_normals[ma.data->faces[i].a].z);
              glVertex3f(
                ma.data->vertices[ma.data->faces[i].a].x,
                ma.data->vertices[ma.data->faces[i].a].y,
                ma.data->vertices[ma.data->faces[i].a].z);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].b].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].b].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].b].z);*/
    /*          glNormal3f(
                ma.data->vertex_normals[ma.data->faces[i].b].x,
                ma.data->vertex_normals[ma.data->faces[i].b].y,
                ma.data->vertex_normals[ma.data->faces[i].b].z);
              glVertex3f(
                ma.data->vertices[ma.data->faces[i].b].x,
                ma.data->vertices[ma.data->faces[i].b].y,
                ma.data->vertices[ma.data->faces[i].b].z);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].c].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].c].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].c].z);*/
    /*          glNormal3f(
                ma.data->vertex_normals[ma.data->faces[i].c].x,
                ma.data->vertex_normals[ma.data->faces[i].c].y,
                ma.data->vertex_normals[ma.data->faces[i].c].z);
              glVertex3f(
                ma.data->vertices[ma.data->faces[i].c].x,
                ma.data->vertices[ma.data->faces[i].c].y,
                ma.data->vertices[ma.data->faces[i].c].z);
            }
            glEnd();
            //ta->_bind();
            render_result->set(1);
      */

    /*    } else {
          //printf("h");
      glColor3f(1, 1, 1);
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();
      glLoadIdentity();

      vsx_transform_obj& texture_transform = *ta->get_transform();
      if (ta->get_transform()) texture_transform();
      printf("number of vertices: %d\n",ma->data->vertices.get_used());
      float time = engine->vtime*4;


      glTranslatef(
        (float)sin(time * 0.0053f),
        (float)sin(time * 0.003f), 0);

      glRotatef(time * 1, 0, 0, 1);
      glRotatef(sin(time * 0.036f) * 60, 0, 1, 0);

      glScalef(
        1.850f + (float)sin(time * 0.0014f) * 0.07f,
        1.850f + (float)sin(time * 0.0010f) * 0.07f,
        1.850f + (float)sin(time * 0.0007f) * 0.07f);      */
          // we have texture

          /*ta->bind();
          if (ma->data->vertex_tex_coords.get_used()) {
            glBegin(GL_TRIANGLES);
            for (unsigned long i = 0; i < ma->data->faces.get_used(); ++i) {
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].a].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].a].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].a].z);
              glVertex3f(
                ma->data->vertices[ma->data->faces[i].a].x,
                ma->data->vertices[ma->data->faces[i].a].y,
                ma->data->vertices[ma->data->faces[i].a].z);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].b].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].b].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].b].z);
              glVertex3f(
                ma->data->vertices[ma->data->faces[i].b].x,
                ma->data->vertices[ma->data->faces[i].b].y,
                ma->data->vertices[ma->data->faces[i].b].z);
              glTexCoord3f(
                ma->data->vertex_tex_coords[ma->data->faces[i].c].x,
                ma->data->vertex_tex_coords[ma->data->faces[i].c].y,
                ma->data->vertex_tex_coords[ma->data->faces[i].c].z);
              glVertex3f(
                ma->data->vertices[ma->data->faces[i].c].x,
                ma->data->vertices[ma->data->faces[i].c].y,
                ma->data->vertices[ma->data->faces[i].c].z);
            }
            glEnd();
            ta->_bind();
            render_result->set(1);
            glPopMatrix();
          } else {

          }    */
          // no texture coordinates, buuuuu :(
    //    }
      //}

  }
};
///------------------------------------------------------------------------------------
///------------------------------------------------------------------------------------
///------------------------------------------------------------------------------------
#endif
class vsx_module_segmesh_to_mesh : public vsx_module {
  vsx_module_param_segment_mesh* seg_mesh_in;
  vsx_module_param_mesh* mesh_out;
  vsx_2dgrid_mesh* mesh_in;
  vsx_mesh* mesh;
public:

  void module_info(vsx_module_info* info) {
    info->identifier = "mesh;segmesh;segmesh_to_mesh";
    info->in_param_spec = "seg_mesh_in:segment_mesh";
    info->out_param_spec = "mesh_out:mesh";
    info->component_class = "mesh";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    loading_done = true;
    seg_mesh_in = (vsx_module_param_segment_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_SEGMENT_MESH,"seg_mesh_in",true);
    //vsx_2dgrid_mesh foomesh;
    //seg_mesh_in->set(foomesh);
    mesh_out = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    mesh_out->set(mesh);
  }

  bool init() {
    mesh = new vsx_mesh;
    return true;
  }
  void on_delete()
  {
    delete mesh;
  }

  void output(vsx_module_param_abs* param) {
    mesh_in = seg_mesh_in->get_addr();
    if (mesh_in) {
      mesh_in->calculate_face_normals();
      mesh_in->calculate_vertex_normals();
      mesh_in->dump_vsx_mesh(mesh);
      mesh_out->set(mesh);
    }
  }
};




vsx_module* MOD_CM(unsigned long module) {
  // in here you load your module.
  // i suggest you make a cache on the first run (this is called once when the vsxu engine starts
  // as it's looping through the available modules and will call
  // this method to get one of each. If you cache the stuff you need (all the stuff
  // needed for the module_info method call, it'll be fast once the engine actually needs
  // one of these. You can do the actual loading/initialization in the run() method because
  // that will only be called when actually used.
  //
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_render_mesh);
    case 1: return (vsx_module*)(new vsx_module_render_mesh_zsort);
    case 2: return (vsx_module*)(new vsx_module_render_mesh_array);
    case 3: return (vsx_module*)(new vsx_module_segmesh_to_mesh);
    case 4: return (vsx_module*)(new vsx_module_mesh_render_line);
    case 5: return (vsx_module*)(new vsx_module_render_dots);
    case 6: return (vsx_module*)(new vsx_module_render_face_id);
    case 7: return (vsx_module*)(new vsx_module_render_billboards);
  }
  return 0;
}

void MOD_DM(vsx_module* m,unsigned long module) {
  switch(module) {
    case 0: delete (vsx_module_render_mesh*)m; break;
    case 1: delete (vsx_module_render_mesh_zsort*)m; break;
    case 2: delete (vsx_module_render_mesh_array*)m; break;
    case 3: delete (vsx_module_segmesh_to_mesh*)m; break;
    case 4: delete (vsx_module_mesh_render_line*)m; break;
    case 5: delete (vsx_module_render_dots*)m; break;
    case 6: delete (vsx_module_render_face_id*)m; break;
    case 7: delete (vsx_module_render_billboards*)m; break;
  }
}

unsigned long MOD_NM() {
  // in here you have to find out how many salvation modules are available and return
  // the correct number. The create_new_module will then create one of each and run
  //   vsx_module_plugin::module_info(vsx_module_info* info)
  // on it. The aim here is to make VSXU think that each and one of the available
  // slavation cabinets are a unique module.

  return 8;
}
