#include <atomic>
#include <thread>
#include <tools/vsx_lock.h>

#include "ocean/fftrefraction.h"
#include "ocean/matrix.h"
#include "ocean/paulslib.h"

class module_mesh_ocean_threaded : public vsx_module
{
public:
  // in
  vsx_module_param_float* wave_speed;
  vsx_module_param_float* wind_speed;
  vsx_module_param_float* wind_speed_x;
  vsx_module_param_float* wind_speed_y;
  vsx_module_param_float* time_speed;
  vsx_module_param_int* normals_only;

  // out
  vsx_module_param_mesh* result;

  // internal
  vsx_mesh<>* mesh;
  vsx_mesh<>* mesh_a = 0x0;
  vsx_mesh<>* mesh_b = 0x0;

  //bool first_run;
  Alaska ocean;

  // threading stuff
  std::thread worker_thread;

  bool thread_created = false;
  std::atomic_int_fast8_t thread_has_something_to_deliver;
  std::atomic_int_fast8_t thread_exit;
  std::atomic_int_fast8_t worker_todo; // indicates wether the worker should do anything.

  int p_updates;
  bool              worker_running;
  int               thread_state;

  module_mesh_ocean_threaded()
  {
    thread_has_something_to_deliver = 0;
    thread_exit = 0;
    worker_todo = 0;
  }

  ~module_mesh_ocean_threaded()
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

  bool init() {
    return true;
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;generators;ocean";

    info->description = "";

    info->in_param_spec =
      "time_speed:float,"
      "wave_speed:float,"
      "wind_speed_x:float,"
      "wind_speed_y:float,"
      "wind_speed:float,"
      "normals_only:enum?no|yes"
  ;

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
    time_speed->set(1.0);

    wave_speed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"wave_speed");
    wave_speed->set(1.0);
    wind_speed = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"wind_speed");
    wind_speed->set(1.0);
    wind_speed_x = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"wind_speed_x");
    wind_speed_x->set(20.0);
    wind_speed_y = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"wind_speed_y");
    wind_speed_y->set(30.0);
    normals_only = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"normals_only");
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    ocean.calculate_ho();
  }

  void worker()
  {
    for(;;)
    {
      if (worker_todo.load())
      {
        worker_todo.fetch_sub(1);

        if (param_updates)
        {
          ocean.factor = wave_speed->get() * 10.0;
          ocean.wind = wind_speed->get() * 0.1f;
          ocean.wind_global[0] = wind_speed_x->get();
          ocean.wind_global[1] = wind_speed_y->get();
          ocean.calculate_ho();
          param_updates = 0;
        }

        ocean.dtime = engine_state->vtime * time_speed->get() * 0.1f;
        ocean.normals_only = normals_only->get() != 0;

        ocean.display();
        mesh->data->vertices.reset_used(0);
        mesh->data->vertex_normals.reset_used(0);
        mesh->data->vertex_tex_coords.reset_used(0);
        mesh->data->faces.reset_used(0);
        vsx_face3 face;
        for (int L=-1;L<2;L++)
        {
          for (int i=0;i<(BIG_NX-1);i++)
          {
            size_t b = 0;
            for (int k=-1;k<2;k++)
            {
              unsigned long a = 0;
              for (int j=0;j<(BIG_NY);j++)
              {
                mesh->data->vertex_normals.push_back(vsx_vector3<>((float)ocean.big_normals[i][j][0],(float)ocean.big_normals[i][j][1],(float)ocean.big_normals[i][j][2]));
                b = mesh->data->vertices.push_back(vsx_vector3<>((float)(ocean.sea[i][j][0]+L*MAX_WORLD_X),(float)(ocean.sea[i][j][1]+k*MAX_WORLD_Y),(float)(ocean.sea[i][j][2]*ocean.scale_height)));
                ++a;
                if (a >= 3) {
                  face.a = (GLuint)(b-3);
                  face.b = (GLuint)(b-2);
                  face.c = (GLuint)(b-1);
                  mesh->data->faces.push_back(face);
                }

                mesh->data->vertex_normals.push_back(vsx_vector3<>((float)ocean.big_normals[i+1][j][0], (float)ocean.big_normals[i+1][j][1], (float)ocean.big_normals[i+1][j][2]));
                b = mesh->data->vertices.push_back(vsx_vector3<>((float)(ocean.sea[i+1][j][0]+L*MAX_WORLD_X), (float)(ocean.sea[i+1][j][1]+k*MAX_WORLD_Y), (float)(ocean.sea[i+1][j][2]*ocean.scale_height)));
                ++a;
                if (a >= 4) {
                  face.a = (GLuint)(b-3);
                  face.b = (GLuint)(b-2);
                  face.c = (GLuint)(b-1);
                  mesh->data->faces.push_back(face);
                }
              }
            }
          }
        }
        thread_has_something_to_deliver.fetch_add(1);
      }

      if (thread_exit.load())
        return;
    }
  }

  void run()
  {
    loading_done = true;
    if (!thread_created)
    {
      worker_thread = std::thread( [this](){ worker(); } );
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
