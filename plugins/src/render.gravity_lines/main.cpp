#include "_configuration.h"
#include "vsx_param.h"
#include "vsx_module.h"
#include "vsx_math_3d.h"
#include "gravity_lines/gravity_lines.h"
#include "gravity_lines/gravity_strip.h"

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */

extern "C" {
_declspec(dllexport) vsx_module* create_new_module(unsigned long module);
_declspec(dllexport) void destroy_module(vsx_module* m,unsigned long module);
_declspec(dllexport) unsigned long get_num_modules();
}


class vsx_module_gravlines : public vsx_module {
  // in

	vsx_module_param_float3* pos;
	vsx_module_param_float4* color0;
	vsx_module_param_float4* color1;

	vsx_module_param_float* friction;
	vsx_module_param_float* step_length;

	// out
	vsx_module_param_render* render_result;
	// internal
	gravity_lines gr;
	float last_updated;

public:

	void module_info(vsx_module_info* info)
	{
	  info->identifier = "renderers;vovoid;gravity_lines";

	  info->in_param_spec = "pos:float3,"
													"params:complex"
													"{"
														"friction:float,"
													  "step_length:float,"
													  "color0:float4,"
													  "color1:float4"
													"}";

  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
	}


	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
		last_updated = -1.0f;
	  loading_done = true;
	  pos = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "pos");
	  pos->set(0,0);
	  pos->set(0,1);
	  pos->set(0,2);

	  color0 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color0");
	  color0->set(1.0f,0);
	  color0->set(1.0f,1);
	  color0->set(1.0f,2);
	  color0->set(0.3f,3);

	  color1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color1");
	  color1->set(1.0f,0);
	  color1->set(1.0f,1);
	  color1->set(1.0f,2);
	  color1->set(1.0f,3);

	  // parameters for the effect
	  friction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "friction");
	  friction->set(1);

	  step_length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_length");
	  step_length->set(10);

	  render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
	  render_result->set(0);
	  gr.init();
	}

	void output(vsx_module_param_abs* param) {
		gr.friction = friction->get();
	  gr.color0[0] = color0->get(0);
	  gr.color0[1] = color0->get(1);
	  gr.color0[2] = color0->get(2);
	  gr.color0[3] = color0->get(3);

	  gr.color1[0] = color1->get(0);
	  gr.color1[1] = color1->get(1);
	  gr.color1[2] = color1->get(2);
	  gr.step_freq = 10.0f * step_length->get();
	  if (last_updated != engine->vtime)
	  {
			gr.update(engine->dtime, pos->get(0), pos->get(1), pos->get(2));
			last_updated = engine->vtime;
	  }
		gr.render();
	  render_result->set(1);
	}
};

class vsx_module_gravity_ribbon : public vsx_module {
  // in
	vsx_module_param_float3* pos;
	vsx_module_param_float4* color0;
	vsx_module_param_float4* color1;

	vsx_module_param_float* friction;
	vsx_module_param_float* step_length;
	vsx_module_param_float* ribbon_width;
	vsx_module_param_float* length;

	// out
	vsx_module_param_render* render_result;
	// internal
	gravity_strip gr;

	float last_updated;

public:

	bool init() {
	  last_updated = -1;
	  return true;
	}

	void module_info(vsx_module_info* info)
	{
	  info->identifier = "renderers;vovoid;gravity_ribbon";

	  info->in_param_spec = "pos:float3,"
													"params:complex"
													"{"
														"ribbon_width:float,"
														"length:float,"
														"friction:float,"
													  "step_length:float,"
													  "color0:float4,"
													  "color1:float4"
													"}";

  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
	}


	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
	  pos = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "pos");

	  color0 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color0");
	  color0->set(1.0f,0);
	  color0->set(1.0f,1);
	  color0->set(1.0f,2);
	  color0->set(0.3f,3);

	  color1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color1");
	  color1->set(1.0f,0);
	  color1->set(1.0f,1);
	  color1->set(1.0f,2);
	  color1->set(1.0f,3);

	  // parameters for the effect
	  friction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "friction");
	  friction->set(1);

	  step_length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_length");
	  step_length->set(10);

		ribbon_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "ribbon_width");
		ribbon_width->set(0.2f);

		length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "length");
		length->set(1.0f);


		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
		render_result->set(0);

	}

	void output(vsx_module_param_abs* param) {
		gr.masses[1].mass = gr.masses[0].mass + ribbon_width->get();
		gr.length = length->get();
		gr.friction = friction->get();
	  gr.color0[0] = color0->get(0);
	  gr.color0[1] = color0->get(1);
	  gr.color0[2] = color0->get(2);
	  gr.color0[3] = color0->get(3);

	  gr.color1[0] = color1->get(0);
	  gr.color1[1] = color1->get(1);
	  gr.color1[2] = color1->get(2);
	  gr.step_freq = 10.0f * step_length->get();
	  if (last_updated != engine->vtime) {
			gr.update(engine->dtime, pos->get(0), pos->get(1), pos->get(2));
			last_updated = engine->vtime;
	  }
		gr.render();
	  render_result->set(1);
	}
};

class vsx_module_gravity_ribbon_particles : public vsx_module {
  // in
	vsx_module_param_float4* color0;
	vsx_module_param_float4* color1;

	vsx_module_param_float* friction;
	vsx_module_param_float* step_length;
	vsx_module_param_float* ribbon_width;
	vsx_module_param_float* length;
	vsx_particlesystem* particles;
	vsx_module_param_particlesystem* in_particlesystem;
	// out
	vsx_module_param_render* render_result;
	// internal
	vsx_avector<gravity_strip*> gr;
	gravity_strip* grp;

	float last_update;
	unsigned long prev_num_particles;

public:

	bool init() {
	  last_update = 0;
	  return true;
	}

  void on_delete()
  {
    for (unsigned long i = 0; i < gr.size(); i++)
    {
      delete gr[i];
    }
  }
  
	void module_info(vsx_module_info* info)
	{
	  info->identifier = "renderers;particlesystems;render_particle_ribbon";
	  info->in_param_spec = "in_particlesystem:particlesystem,"
													"params:complex"
													"{"
														"ribbon_width:float,"
														"length:float,"
														"friction:float,"
													  "step_length:float,"
													  "color0:float4,"
													  "color1:float4"
													"}";

  	info->out_param_spec = "render_out:render";
  	info->component_class = "render";
	}


	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
	{
	  loading_done = true;
	  prev_num_particles = 0;
	  in_particlesystem = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"in_particlesystem");

	  color0 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color0");
	  color0->set(1.0f,0);
	  color0->set(1.0f,1);
	  color0->set(1.0f,2);
	  color0->set(0.3f,3);

	  color1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color1");
	  color1->set(1.0f,0);
	  color1->set(1.0f,1);
	  color1->set(1.0f,2);
	  color1->set(1.0f,3);

	  // parameters for the effect
	  friction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "friction");
	  friction->set(1);

	  step_length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_length");
	  step_length->set(10);

	  ribbon_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "ribbon_width");
	  ribbon_width->set(0.2f);

		length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "length");
		length->set(1.0f);

		render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
		render_result->set(0);
	}

	void output(vsx_module_param_abs* param) {
    particles = in_particlesystem->get_addr();
    if (particles) {
      if (prev_num_particles != particles->particles->size())
      {
    	// remove all the old ones
    	/*for (unsigned long i = 0; i < gr.size(); i++)
    	{
    		delete gr[i];
    	}
        gr.reset_used(0);*/
        //printf("num particles: %d\n",particles->particles->size());
    	//printf("prev_num: %d\n",prev_num_particles);
        for (unsigned long i = prev_num_particles; i < particles->particles->size(); ++i) {
        	if (i == prev_num_particles) printf("allocating again\n");
        	gr[i] = new gravity_strip;
          //printf("i: %d\n",i);
          gr[i]->init();
          gr[i]->init_strip();
        }
        prev_num_particles = particles->particles->size();
      }
      //printf("done alloc %d\n",particles->particles->size());

      for (unsigned long i = 0; i < particles->particles->size(); ++i) {
      	//gr[i].length = 0.0f;
      	gr[i]->width = ribbon_width->get();
				//gr[i].masses[1].mass = gr[i].masses[0].mass + ribbon_width->get();
				gr[i]->length = length->get();
				gr[i]->friction = friction->get();
        float tt = ((*particles->particles)[i].time/(*particles->particles)[i].lifetime);
        if (tt < 0.0f) tt = 0.0f;
        if (tt > 1.0f) tt = 1.0f;
			  gr[i]->color0[0] = color0->get(0)*tt;
			  gr[i]->color0[1] = color0->get(1)*tt;
			  gr[i]->color0[2] = color0->get(2)*tt;
			  gr[i]->color0[3] = color0->get(3)*tt;

			  gr[i]->color1[0] = color1->get(0);
			  gr[i]->color1[1] = color1->get(1);
			  gr[i]->color1[2] = color1->get(2);
			  gr[i]->step_freq = 10.0f * step_length->get();
		  	//if (last_update != engine->vtime) {
			  gr[i]->update(engine->dtime, (*(particles->particles))[i].pos.x, (*(particles->particles))[i].pos.y, (*(particles->particles))[i].pos.z);
					//last_upd ate = engine->vtime;
	  		//}
				//printf("%f, %f, %f\n", (*particles->particles)[i].pos.x, (*particles->particles)[i].pos.y, (*particles->particles)[i].pos.z);
			  gr[i]->render();
		//		printf("%d %d;;; %d\n",__LINE__,i, particles->particles->size());
        // add the delta-time to the time of the particle
        /*(*particles->particles)[i].pos.x += px*engine->dtime;
        (*particles->particles)[i].pos.y += py*engine->dtime;
        (*particles->particles)[i].pos.z += pz*engine->dtime;*/
      }
      //printf("done drawing\n");
    }
	render_result->set(1);
  }
};

class vsx_module_gravity_ribbon_mesh : public vsx_module {
  // in
  vsx_module_param_float4* color0;
  vsx_module_param_float4* color1;

  vsx_module_param_float* mesh_id_start;
  vsx_module_param_float* mesh_id_count;

  vsx_module_param_float* friction;
  vsx_module_param_float* step_length;
  vsx_module_param_float* ribbon_width;
  vsx_module_param_float* length;
  vsx_module_param_float* reset_pos;
  vsx_module_param_mesh* in_mesh;
  vsx_module_param_matrix* modelview_matrix;
  vsx_module_param_float3* upvector;
  // out
  vsx_module_param_render* render_result;
  vsx_module_param_mesh* mesh_result;
  // internal
  vsx_avector<gravity_strip*> gr;
  gravity_strip* grp;
  vsx_mesh** mesh;
  vsx_mesh* mesh_out;

  vsx_matrix modelview_matrix_no_connection;
  float last_update;
  long prev_num_vertices;

public:
  
  bool init() {
    mesh_out = new vsx_mesh;
    last_update = 0;
    return true;
  }

  void on_delete()
  {
    delete mesh_out;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "renderers;mesh;render_mesh_ribbon";
    info->in_param_spec = "in_mesh:mesh,"
        "mesh_settings:complex"
        "{"
          "mesh_id_start:float,"
          "mesh_id_count:float,"
        "},"
        "params:complex"
        "{"
          "ribbon_width:float,"
          "length:float,"
          "friction:float,"
          "step_length:float,"
          "color0:float4,"
          "color1:float4,"
          "reset_pos:float,"
          "modelview_matrix:matrix,"
          "upvector:float3"
        "}";

    info->out_param_spec = "render_out:render,mesh_out:mesh";
    info->component_class = "render";
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    prev_num_vertices = 0;
    mesh_id_start = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "mesh_id_start");
    mesh_id_start->set(0.0f);
    mesh_id_count = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "mesh_id_count");
    mesh_id_count->set(0.0f);
    in_mesh = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"in_mesh");

    upvector = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3, "upvector");
    upvector->set(0.0f);
    upvector->set(0.0f);
    upvector->set(1.0f);

    color0 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color0");
    color0->set(1.0f,0);
    color0->set(1.0f,1);
    color0->set(1.0f,2);
    color0->set(0.3f,3);

    color1 = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4, "color1");
    color1->set(1.0f,0);
    color1->set(1.0f,1);
    color1->set(1.0f,2);
    color1->set(1.0f,3);

    modelview_matrix = (vsx_module_param_matrix*)in_parameters.create(VSX_MODULE_PARAM_ID_MATRIX,"modelview_matrix");



    // parameters for the effect
    friction = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "friction");
    friction->set(1);

    step_length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "step_length");
    step_length->set(10);

    ribbon_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "ribbon_width");
    ribbon_width->set(0.2f);

    length = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "length");
    length->set(1.0f);

    reset_pos = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "reset_pos");
    reset_pos->set(-1.0f);

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);

    mesh_result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_out");
    mesh_result->set_p(mesh_out);
  }

  void output(vsx_module_param_abs* param) {
    mesh = in_mesh->get_addr();

    if (mesh && (*mesh)->data->vertices.size())
    {
      if (prev_num_vertices != (int)mesh_id_count->get())
      {
        // remove all the old ones
        for (unsigned long i = prev_num_vertices; i < (int)mesh_id_count->get(); ++i)
        {
          //printf("allocating %d\n", i);
          //if (i == prev_num_vertices)
            //printf("allocating again\n");
          gr[i] = new gravity_strip;
          gr[i]->init();
          gr[i]->init_strip();
        }
        prev_num_vertices = (int)mesh_id_count->get();
      }
      //printf("mesh_id_start: %d\n", (int)mesh_id_start->get());
      size_t mesh_index = (size_t)mesh_id_start->get() % (*mesh)->data->vertices.size();

      vsx_matrix* matrix_result = modelview_matrix->get_addr();
      if (!matrix_result)
      {
        matrix_result = &modelview_matrix_no_connection;
        glGetFloatv(GL_MODELVIEW_MATRIX, modelview_matrix_no_connection.m);
      }


      //printf("in-mesh vertex count: %d\n", mesh->data->vertices.size());
      if (param == render_result)
      {
        for (unsigned long i = 0; i < prev_num_vertices; ++i)
        {
          //gr[i].length = 0.0f;
          gr[i]->width = ribbon_width->get();
          //gr[i].masses[1].mass = gr[i].masses[0].mass + ribbon_width->get();
          gr[i]->length = length->get();
          gr[i]->friction = friction->get();
          //float tt = ((*particles->particles)[i].time/(*particles->particles)[i].lifetime);
          //if (tt < 0.0f) tt = 0.0f;
          //if (tt > 1.0f) tt = 1.0f;
          gr[i]->color0[0] = color0->get(0);
          gr[i]->color0[1] = color0->get(1);
          gr[i]->color0[2] = color0->get(2);
          gr[i]->color0[3] = color0->get(3);

          gr[i]->color1[0] = color1->get(0);
          gr[i]->color1[1] = color1->get(1);
          gr[i]->color1[2] = color1->get(2);
          gr[i]->step_freq = 10.0f * step_length->get();
          //if (last_update != engine->vtime) {
          if (reset_pos->get() > 0.0f)
          {
            gr[i]->reset_pos(
              (*mesh)->data->vertices[mesh_index].x,
              (*mesh)->data->vertices[mesh_index].y,
              (*mesh)->data->vertices[mesh_index].z
            );
          } else
          {
            gr[i]->update(
              engine->dtime,
              (*mesh)->data->vertices[mesh_index].x,
              (*mesh)->data->vertices[mesh_index].y,
              (*mesh)->data->vertices[mesh_index].z
            );
          }
          gr[i]->render();
              //(*(particles->particles))[i].pos.x, (*(particles->particles))[i].pos.y, (*(particles->particles))[i].pos.z);
            //last_upd ate = engine->vtime;
          //}
          //printf("%f, %f, %f\n", (*particles->particles)[i].pos.x, (*particles->particles)[i].pos.y, (*particles->particles)[i].pos.z);
      //    printf("%d %d;;; %d\n",__LINE__,i, particles->particles->size());
          // add the delta-time to the time of the particle
          /*(*particles->particles)[i].pos.x += px*engine->dtime;
          (*particles->particles)[i].pos.y += py*engine->dtime;
          (*particles->particles)[i].pos.z += pz*engine->dtime;*/
          mesh_index++;
          mesh_index = mesh_index % (*mesh)->data->vertices.size();
        }
      }
      else
      {
        float ilength = length->get();
        if (ilength > 1.0f) ilength = 1.0f;
        if (ilength < 0.01f) ilength = 0.01f;

        int num2 = BUFF_LEN * (int)(ilength * 8.0f * (float)prev_num_vertices);

        //printf("num2: %d\n", num2);
        // allocate mesh memory for all parts
        mesh_out->data->faces.allocate(num2);
        mesh_out->data->vertices.allocate(num2);
        mesh_out->data->vertex_normals.allocate(num2);
        mesh_out->data->vertex_tex_coords.allocate(num2);
        //printf("mesh: %d\n", __LINE__);
        mesh_out->data->faces.reset_used(num2);
        mesh_out->data->vertices.reset_used(num2);
        mesh_out->data->vertex_normals.reset_used(num2);
        mesh_out->data->vertex_tex_coords.reset_used(num2);
        //printf("mesh: %d\n", __LINE__);

        vsx_face*      fs_d = mesh_out->data->faces.get_pointer();
        vsx_vector*    vs_d = mesh_out->data->vertices.get_pointer();
        vsx_vector*    ns_d = mesh_out->data->vertex_normals.get_pointer();
        vsx_tex_coord* ts_d = mesh_out->data->vertex_tex_coords.get_pointer();
        int generated_vertices = 0;
        int generated_faces = 0;
        //printf("mesh: %d\n", __LINE__);

        generated_faces = 0;
        generated_vertices = 0;
        generated_vertices = 0;
        generated_vertices = 0;
        vsx_vector upv;
        upv.x = upvector->get(0);
        upv.y = upvector->get(1);
        upv.z = upvector->get(2);
        for (int i = 0; i < prev_num_vertices; ++i)
        {
          gr[i]->width = ribbon_width->get();
          gr[i]->length = length->get();
          gr[i]->friction = friction->get();
          gr[i]->color0[0] = color0->get(0);
          gr[i]->color0[1] = color0->get(1);
          gr[i]->color0[2] = color0->get(2);
          gr[i]->color0[3] = color0->get(3);

          gr[i]->color1[0] = color1->get(0);
          gr[i]->color1[1] = color1->get(1);
          gr[i]->color1[2] = color1->get(2);
          gr[i]->step_freq = 10.0f * step_length->get();

          //printf("mesh: %d %d\n", __LINE__, mesh_index);
          if (reset_pos->get() > 0.0f)
          {
            gr[i]->reset_pos(
              (*mesh)->data->vertices[mesh_index].x,
              (*mesh)->data->vertices[mesh_index].y,
              (*mesh)->data->vertices[mesh_index].z
            );
          } else
          {
            gr[i]->update(
              engine->dtime,
              (*mesh)->data->vertices[mesh_index].x,
              (*mesh)->data->vertices[mesh_index].y,
              (*mesh)->data->vertices[mesh_index].z
            );
          }
          //printf("%d\n", (int)i);

          gr[i]->generate_mesh(*mesh_out,fs_d, vs_d, ns_d, ts_d, matrix_result, &upv, generated_vertices, generated_faces);
          mesh_index++;
          mesh_index = mesh_index % (*mesh)->data->vertices.size();
        }

//        printf("generated faces: %d\n", generated_faces);
        //printf("generated vertices: %d\n", generated_vertices);
        mesh_out->data->faces.reset_used(generated_faces);
        mesh_out->data->vertices.reset_used(generated_vertices);
        mesh_out->data->vertex_normals.reset_used(generated_vertices);
        mesh_out->data->vertex_tex_coords.reset_used(generated_vertices);
        //printf("mesh: %d\n", __LINE__);

        mesh_result->set_p(mesh_out);
      }
      //printf("done drawing\n");
    }
  render_result->set(1);
  }
};

#if BUILDING_DLL
unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 4;
}

vsx_module* create_new_module(unsigned long module) {
  // as we have only one module available, don't look at the module variable, just return - FOR SPEED (says jaw)
  // otherwise you'd have something like,, switch(module) { case 0: break; }
  switch (module) {
  	case 0: return (vsx_module*)new vsx_module_gravlines;
  	case 1: return (vsx_module*)new vsx_module_gravity_ribbon;
  	case 2: return (vsx_module*)new vsx_module_gravity_ribbon_particles;
    case 3: return (vsx_module*)new vsx_module_gravity_ribbon_mesh;
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch (module) {
    case 0: delete (vsx_module_gravlines*)m; break;
    case 1: delete (vsx_module_gravity_ribbon*)m; break;
    case 2: delete (vsx_module_gravity_ribbon_particles*)m; break;
    case 3: delete (vsx_module_gravity_ribbon_mesh*)m; break;
  }
}
#endif
