#include <atomic>
#include <thread>
#include "ocean/fftrefraction.h"
#include "ocean/matrix.h"
#include "ocean/paulslib.h"

class module_mesh_ocean_tunnel_threaded : public vsx_module
{
public:
  // in
  vsx_module_param_float* time_speed;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh = 0x0;
  vsx_mesh<>* mesh_a = 0x0;
  vsx_mesh<>* mesh_b = 0x0;
  Alaska ocean;
  float t;

  // threading stuff
  std::thread worker_thread;

  bool thread_created = false;
  std::atomic_int_fast8_t thread_has_something_to_deliver;
  std::atomic_int_fast8_t thread_exit;
  std::atomic_int_fast8_t worker_todo; // indicates wether the worker should do anything.

  int p_updates;
  bool              worker_running;
  int               thread_state;

  module_mesh_ocean_tunnel_threaded()
  {
    thread_has_something_to_deliver = 0;
    thread_exit = 0;
    worker_todo = 0;
  }

  bool init()
  {
    return true;
  }

  ~module_mesh_ocean_tunnel_threaded()
  {
    if (worker_thread.joinable())
    {
      thread_exit.fetch_add(1);
      worker_thread.join();
    }
    if (mesh_a != 0)
    {
      delete mesh_a;
      delete mesh_b;
    }
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;generators;ocean_tunnel";

    info->description = "";

    info->in_param_spec =
      "time_speed:float";

    info->out_param_spec =
      "mesh:mesh";

    info->component_class =
      "mesh";
  }
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_a = new vsx_mesh<>;
    mesh_b = new vsx_mesh<>;
    mesh = mesh_a;
    thread_state = 0;
    thread_exit = 0;
    worker_running = false;

    loading_done = false;
    time_speed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"time_speed");
    time_speed->set(0.2f);
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    //first_run = true;
    ocean.calculate_ho();
    t = 0;
  }

  void worker()
  {
    for(;;)
    {
      if (worker_todo.load())
      {
        t += time_speed->get()*engine_state->real_dtime;
        ocean.dtime = t;
        ocean.display();
        mesh->data->vertices.reset_used(0);
        mesh->data->vertex_normals.reset_used(0);
        mesh->data->vertex_tex_coords.reset_used(0);
        mesh->data->faces.reset_used(0);
        vsx_face3 face;
        vsx_vector3<> g;
        vsx_vector3<> c;
        for (int L=-1;L<2;L++)
        {
          for (int i=0;i<(BIG_NX-1);i++)
          {
            unsigned long b = 0;
            for (int k=-1;k<2;k++)
            {
              unsigned long a = 0;
              for (int j=0;j<(BIG_NY);j++)
              {
                //printf("j: %d\n", j);
                if (j%2 == 1) continue;
    #define TDIV (float)MAX_WORLD_X
    #define TD2  (float)MAX_WORLD_X*0.5f
                g.x = (float)ocean.sea[i][j][0];//+L*MAX_WORLD_X;
                g.y = (float)ocean.sea[i][j][1];//+k*MAX_WORLD_Y;
                g.z = (float)ocean.sea[i][j][2];//*ocean.scale_height;

                float gr = \
                (float)PI*2.0f * g.x/(TDIV);
                float nra = gr + 90.0f / 360.0f * 2*(float)PI;


                vsx_vector3<> nn;
                nn.x = (float)ocean.big_normals[i][j][0];
                nn.y = (float)ocean.big_normals[i][j][1];
                nn.normalize();
                mesh->data->vertex_normals.push_back(vsx_vector3<>(\
                  nn.x* cosf(nra) + nn.y * -sinf(nra),\
                  nn.x* sinf(nra) + nn.y * cosf(nra),\
                  (float)ocean.big_normals[i][j][2]));
                mesh->data->vertex_normals[mesh->data->vertex_normals.size()-1].normalize();


                float gz = 2.0f+fabs(g.z)*1.5f;
                c.x = cos(gr)*gz;
                c.y = sin(gr)*gz;
                c.z = g.y*2.0f;
                b = (unsigned long)mesh->data->vertices.push_back(c);
                mesh->data->vertex_tex_coords.push_back(vsx_tex_coord2f(fabs(g.x-TD2)*2.0f , fabs(g.y-TD2)*2.0f));
                ++a;
                if (a >= 3) {
                  face.a = b-3;
                  face.b = b-2;
                  face.c = b-1;
                  mesh->data->faces.push_back(face);
                }
                g.x = (float)ocean.sea[i+1][j][0];//+L*MAX_WORLD_X;
                g.y = (float)ocean.sea[i+1][j][1];//+k*MAX_WORLD_Y;
                g.z = (float)ocean.sea[i+1][j][2];//*ocean.scale_height;

                gr = \
                (float)PI*2.0f* g.x/(TDIV);
                nra = gr + 90.0f / 360.0f * 2*(float)PI;


                nn.x = (float)ocean.big_normals[i+1][j][0];
                nn.y = (float)ocean.big_normals[i+1][j][1];
                nn.normalize();
                mesh->data->vertex_normals.push_back(
                  vsx_vector3<>(
                    (float)(nn.x * cos(nra) + nn.y * -sin(nra)),
                    (float)(nn.x * (float)sin(nra) + nn.y * cos(nra)),
                    (float)ocean.big_normals[i+1][j][2]
                  )
                );

                mesh->data->vertex_normals[mesh->data->vertex_normals.size()-1].normalize();

                gz = 2.0f+fabs(g.z)*1.5f;
                c.x = cos(gr)*gz;
                c.y = sin(gr)*gz;
                c.z = g.y*2.0f;
                b = (unsigned long)mesh->data->vertices.push_back(c);

                mesh->data->vertex_tex_coords.push_back(vsx_tex_coord2f(fabs(g.x-TD2)*2.0f , fabs(g.y-TD2)*2.0f));

                ++a;

                if (a >= 4) {
                  face.a = b-3;
                  face.b = b-2;
                  face.c = b-1;
                  mesh->data->faces.push_back(face);
                }
              }
            }
          }
        }
        thread_has_something_to_deliver++;
      } // sem_trywait
      if (thread_exit.load())
        return;
    }
  }

  void run()
  {
    loading_done = true;
    if (!thread_created)
    {
      worker_thread = std::thread( [this](){worker();} );
      thread_created = true;
      worker_todo.fetch_add(1);
      return;
    }

    if (thread_has_something_to_deliver.load())
    {
      mesh->timestamp++;
      result->set(mesh);

      // toggle to the other mesh
      if (mesh == mesh_a)
        mesh = mesh_b;
      else
        mesh = mesh_a;

      worker_todo.fetch_add(1);
    }
  }
};
