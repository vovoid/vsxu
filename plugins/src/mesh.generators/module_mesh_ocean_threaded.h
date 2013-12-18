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
  vsx_mesh* mesh;
  vsx_mesh* mesh_a;
  vsx_mesh* mesh_b;

  //bool first_run;
  Alaska ocean;

  // threading stuff
  pthread_t         worker_t;

  pthread_mutex_t   mesh_mutex;
  int               thread_has_something_to_deliver; // locked by the mesh mutex
  pthread_mutex_t   thread_exit_mutex;
  sem_t sem_worker_todo; // indicates wether the worker should do anything.
  bool              thread_created;

  int p_updates;
  bool              worker_running;
  int               thread_state;
  int               thread_exit;

  module_mesh_ocean_threaded()
  {
    pthread_mutex_init(&mesh_mutex,NULL);
    pthread_mutex_init(&thread_exit_mutex,NULL);
    sem_init(&sem_worker_todo,0,0);
    thread_has_something_to_deliver = 0;
    thread_created = false;
    mesh_a = 0;
    mesh_b = 0;
  }

  ~module_mesh_ocean_threaded()
  {
    if (thread_created)
    {
      pthread_mutex_lock(&thread_exit_mutex);
        thread_exit = 1;
      pthread_mutex_unlock(&thread_exit_mutex);
      void* ret;
      int jret = pthread_join(worker_t, &ret);
      if (jret == 22) printf("ocean_threaded: pthread_join failed: EINVAL\n");
      if (jret == 3) printf("ocean_threaded: pthread_join failed: ESRCH\n");
    }
    if (mesh_a != 0)
    {
      delete mesh_a;
      delete mesh_b;
    }
    pthread_mutex_destroy(&thread_exit_mutex);
    pthread_mutex_destroy(&mesh_mutex);
    sem_destroy(&sem_worker_todo);
  }

  bool init() {
    return true;
  }

  void module_info(vsx_module_info* info)
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
    mesh_a = new vsx_mesh;
    mesh_b = new vsx_mesh;
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

  static void* worker(void *ptr)
  {
    while (1)
    {
      module_mesh_ocean_threaded* my = ((module_mesh_ocean_threaded*)ptr);
      if (0 == sem_trywait(&my->sem_worker_todo))
      {
        pthread_mutex_lock(&my->mesh_mutex);

        if (my->param_updates)
        {
          my->ocean.factor = my->wave_speed->get() * 10.0;
          my->ocean.wind = my->wind_speed->get() * 0.1;
          my->ocean.wind_global[0] = my->wind_speed_x->get();
          my->ocean.wind_global[1] = my->wind_speed_y->get();
          my->ocean.calculate_ho();
          my->param_updates = 0;
        }

        my->ocean.dtime = my->engine->real_vtime*my->time_speed->get() * 0.1f;
        my->ocean.normals_only = my->normals_only->get();

        my->ocean.display();
        my->mesh->data->vertices.reset_used(0);
        my->mesh->data->vertex_normals.reset_used(0);
        my->mesh->data->vertex_tex_coords.reset_used(0);
        my->mesh->data->faces.reset_used(0);
        vsx_face face;
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
                my->mesh->data->vertex_normals.push_back(vsx_vector(my->ocean.big_normals[i][j][0],my->ocean.big_normals[i][j][1],my->ocean.big_normals[i][j][2]));
                b = my->mesh->data->vertices.push_back(vsx_vector(my->ocean.sea[i][j][0]+L*MAX_WORLD_X,my->ocean.sea[i][j][1]+k*MAX_WORLD_Y,my->ocean.sea[i][j][2]*my->ocean.scale_height));
                ++a;
                if (a >= 3) {
                  face.a = b-3;
                  face.b = b-2;
                  face.c = b-1;
                  my->mesh->data->faces.push_back(face);
                }

                my->mesh->data->vertex_normals.push_back(vsx_vector(my->ocean.big_normals[i+1][j][0],my->ocean.big_normals[i+1][j][1],my->ocean.big_normals[i+1][j][2]));
                b = my->mesh->data->vertices.push_back(vsx_vector(my->ocean.sea[i+1][j][0]+L*MAX_WORLD_X,my->ocean.sea[i+1][j][1]+k*MAX_WORLD_Y,my->ocean.sea[i+1][j][2]*my->ocean.scale_height));
                ++a;
                if (a >= 4) {
                  face.a = b-3;
                  face.b = b-2;
                  face.c = b-1;
                  my->mesh->data->faces.push_back(face);
                }
              }
            }
          }
        }
        my->thread_has_something_to_deliver++;
        pthread_mutex_unlock(&my->mesh_mutex);
      } // sem_trywait
      pthread_mutex_lock(&my->thread_exit_mutex);
      int time_to_exit = my->thread_exit;
      pthread_mutex_unlock(&my->thread_exit_mutex);
      if (time_to_exit) {
        int *retval = new int;
        *retval = 0;
        my->thread_exit = 0;
        pthread_exit((void*)retval);
        return 0;
      }
    }
  }

  void run() {
    loading_done = true;
    if (!thread_created)
    {
      pthread_create(&worker_t, NULL, &worker, (void*)this);
      thread_created = true;
      sem_post(&sem_worker_todo);
      return;
    }
    // this concept assumes that the run takes shorter than the framerate to do
    if (0 == pthread_mutex_lock(&mesh_mutex) )
    {
      // lock ackquired. thread is waiting for us to set the semaphore before doing anything again.
      if (thread_has_something_to_deliver)
      {

        mesh->timestamp++;
        result->set(mesh);

        // toggle to the other mesh
        if (mesh == mesh_a) mesh = mesh_b;
        else mesh = mesh_a;
        sem_post(&sem_worker_todo);
      }
      pthread_mutex_unlock(&mesh_mutex);
    }
  }
};
