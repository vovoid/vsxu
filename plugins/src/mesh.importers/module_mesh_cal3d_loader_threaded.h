#include "cal3d/cal3d.h"
#define VSXU_DEBUG 1
#if (PLATFORM == PLATFORM_LINUX)
  #include <sys/prctl.h>
#endif


typedef struct {
  CalBone* bone;
  vsx_string name;
  vsx_module_param_quaternion* param;
  vsx_module_param_float3* translation;
  vsx_module_param_quaternion* result_rotation;
  vsx_module_param_float3* result_translation;
  CalVector o_t;
} bone_info;

typedef struct {
  bool is_thread;
  void* class_pointer;
} cal3d_thread_info;

class module_mesh_cal3d_import : public vsx_module {
public:
    // in
    vsx_module_param_resource* filename;
    vsx_module_param_quaternion* quat_p;
    vsx_module_param_int* use_thread;

    vsx_module_param_float3* pre_rotation_center;
    vsx_module_param_quaternion* pre_rotation;

    vsx_module_param_float3* rotation_center;
    vsx_module_param_quaternion* rotation;
    vsx_module_param_float3* post_rot_translate;

    // out
    vsx_module_param_mesh* result;
    vsx_module_param_mesh* bones_bounding_box;
    // internal
    vsx_mesh* mesh_a;
    vsx_mesh* mesh_b;
    vsx_mesh* mesh_bbox;
    bool first_run;
    int n_rays;
    vsx_string current_filename;
    CalCoreModel* c_model;
    CalModel* m_model;
    vsx_avector<bone_info> bones;

    // threading stuff
    pthread_t         worker_t;
    pthread_mutex_t   mesh_mutex;
    pthread_cond_t count_threshold_cv;
    int               thread_has_something_to_deliver; // locked by the mesh mutex
    int               have_sent_work_to_thread;
    vsx_mesh*         mesh; // locked by the mesh mutex

    pthread_mutex_t   thread_exit_mutex;
    sem_t sem_worker_todo; // indicates wether the worker should do anything.

    int p_updates;
    bool              worker_running;
    bool              thread_created;
    int               thread_state;
    int               thread_exit;
    int               prev_use_thread;
    cal3d_thread_info thread_info;

    // transform
    vsx_quaternion pre_rotation_quaternion;
    vsx_matrix pre_rotation_mat;
    vsx_vector pre_rot_center;

    vsx_quaternion rotation_quaternion;
    vsx_matrix rotation_mat;
    vsx_vector rot_center;
    vsx_vector post_rot_translate_vec;


  module_mesh_cal3d_import() {
    m_model = 0;
    c_model = 0;
    thread_state = 0;
    thread_exit = 0;
    worker_running = false;
    thread_created = false;
    p_updates = -1;
    prev_use_thread = 0;
    pthread_mutex_init(&mesh_mutex,NULL);
    pthread_mutex_init(&thread_exit_mutex,NULL);
    pthread_cond_init (&count_threshold_cv, NULL);
    sem_init(&sem_worker_todo,0,0);
    thread_has_something_to_deliver = 0;
    have_sent_work_to_thread = 0;
  }
  bool init() {


    return true;
  }

  void on_delete()
  {
    if (thread_created)
    {
      // thread is probably waiting for data now, so this mutex is free
      pthread_mutex_lock(&thread_exit_mutex);
        thread_exit = 1;
      pthread_mutex_unlock(&thread_exit_mutex);

      // if thread is waiting for data, push it out of it
      pthread_mutex_lock(&mesh_mutex);
      pthread_cond_signal(&count_threshold_cv);
      pthread_mutex_unlock(&mesh_mutex);

      // meanwhile, thread will find the thread_exit we set

      pthread_join(worker_t, NULL);
    }

    pthread_mutex_lock(&mesh_mutex);
    if (c_model) {
      delete (CalCoreModel*)c_model;
    }
    delete mesh_a;
    delete mesh_b;
    pthread_mutex_unlock(&mesh_mutex);
    pthread_mutex_destroy(&thread_exit_mutex);
    pthread_mutex_destroy(&mesh_mutex);
    sem_destroy(&sem_worker_todo);
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;importers;cal3d_importer";
    info->description = "";
    info->in_param_spec =
        "filename:resource,use_thread:enum?no|yes,"
        "transforms:complex{"
          "pre_rotation:quaternion,"
          "pre_rotation_center:float3,"
          "rotation:quaternion,"
          "rotation_center:float3,"
          "post_rot_translate:float3"
        "}"
        ;
    info->out_param_spec =
        "mesh:mesh,"
        "bones_bounding_box:mesh"
    ;
    if (bones.size()) {
      info->in_param_spec += ",bones:complex{";
      info->out_param_spec += ",absolutes:complex{";
      for (unsigned long i = 0; i < bones.size(); ++i)
      {
        if (i != 0) {
          info->in_param_spec += ",";
          info->out_param_spec += ",";
        }
        info->in_param_spec += bones[i].name+":complex{"+bones[i].name+"_rotation:quaternion,"+bones[i].name+"_translation:float3}";
        info->out_param_spec += bones[i].name+":complex{"+bones[i].name+"_rotation:quaternion,"+bones[i].name+"_translation:float3}";
      }
      info->in_param_spec += "}";
      info->out_param_spec += "}";
    }
    //printf("inparamspec: %s\n",info->in_param_spec.c_str());
    info->component_class = "mesh";
  }

  void redeclare_in_params(vsx_module_param_list& in_parameters) {
    filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename->set(current_filename);

    pre_rotation = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"pre_rotation");
    pre_rotation->set(0.0f,0);
    pre_rotation->set(0.0f,1);
    pre_rotation->set(0.0f,2);
    pre_rotation->set(1.0f,3);
    pre_rotation_center = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"pre_rotation_center");



    rotation = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"rotation");
    rotation->set(0.0f,0);
    rotation->set(0.0f,1);
    rotation->set(0.0f,2);
    rotation->set(1.0f,3);
    rotation_center = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"rotation_center");

    post_rot_translate = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"post_rot_translate");


    quat_p = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"bone_1");
    quat_p->set(0.0f,0);
    quat_p->set(0.0f,1);
    quat_p->set(0.0f,2);
    quat_p->set(1.0f,3);
    use_thread = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_thread");
    use_thread->set(0);
    prev_use_thread = 0;
    if (bones.size())
    {
      for (unsigned long i = 0; i < bones.size(); ++i)
      {
        #ifdef VSXU_DEBUG
        printf(" declaring parameter for bone: \"%s\"\n", bones[i].name.c_str());
        fflush(stdout);
        #endif
        bones[i].param = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,(bones[i].name+"_rotation").c_str());
        bones[i].translation = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,(bones[i].name+"_translation").c_str());
        CalQuaternion q2;
        if (bones[i].bone != 0) {
          q2 = bones[i].bone->getRotation();
          bones[i].param->set(q2.x,0);
          bones[i].param->set(q2.y,1);
          bones[i].param->set(q2.z,2);
          bones[i].param->set(q2.w,3);
        } else {
          bones[i].param->set(0,0);
          bones[i].param->set(0,1);
          bones[i].param->set(0,2);
          bones[i].param->set(1,3);
        }
      }
    }
  }

  void redeclare_out_params(vsx_module_param_list& out_parameters)
  {
    // default
    result = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh");
    result->set(mesh);
    // bounding box for all bones
    bones_bounding_box = (vsx_module_param_mesh*)out_parameters.create(VSX_MODULE_PARAM_ID_MESH,"bones_bounding_box");
    // bones
    if (bones.size()) {
      for (unsigned long i = 0; i < bones.size(); ++i) {
        bones[i].result_rotation = (vsx_module_param_quaternion*)out_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,(bones[i].name+"_rotation").c_str());
        bones[i].result_translation = (vsx_module_param_float3*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,(bones[i].name+"_translation").c_str());
      }
    }
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    mesh_a = new vsx_mesh;
    mesh_b = new vsx_mesh;
    mesh = mesh_a;
    mesh_bbox = new vsx_mesh;

    loading_done = false;
    current_filename = "";
    redeclare_in_params(in_parameters);
    redeclare_out_params(out_parameters);
    first_run = true;
    c_model = 0;
    thread_info.class_pointer = (void*)this;
  }

  void param_set_notify(const vsx_string& name)
  {
    pthread_mutex_lock(&mesh_mutex);

    #ifdef VSXU_DEBUG
    printf("cal3d param set notify..\n");
    #endif

    if (name == "filename") {
      if (filename->get() != current_filename) {
        current_filename = filename->get();
        // first find the path of the file
        vsx_avector<vsx_string> fparts;
        vsx_string fdeli = "/";
        vsx_string file_path = "";
        explode(filename->get(), fdeli, fparts);
        if (fparts.size() > 1) {
          fparts.reset_used(fparts.get_used()-1);
          file_path = implode(fparts,fdeli)+"/";
        }
        #ifdef VSXU_DEBUG
        printf("file path: %s\n",file_path.c_str());
        #endif
        //-------------------------------------------------
        vsx_avector<int> mesh_parts;
        vsx_avector<int> material_parts;
        vsxf_handle *fp;
        fp = engine->filesystem->f_open(current_filename.c_str(), "r");
        if (!fp) {
          #ifdef VSXU_DEBUG
          printf("cal3d: could not open filename: %s\n", current_filename.c_str() );
          #endif
          return;
        }

        c_model = new CalCoreModel("core");
        char buf[1024];
        vsx_string line;
        int mesh_id = 0;
        while (engine->filesystem->f_gets(buf,1024,fp)) {
          line = buf;
          if (line[line.size()-1] == 0x0A) line.pop_back();
          if (line[line.size()-1] == 0x0D) line.pop_back();
          //printf("reading line: %s\n",line.c_str());
          if (line.size()) {
            vsx_avector<vsx_string> parts;
            vsx_string deli = "=";
            explode(line, deli, parts);
            if (parts[0] == "skeleton") {
              vsxf_handle* h = engine->filesystem->f_open((file_path+parts[1]).c_str(),"r");
              if (h) {
                resources.push_back(file_path+parts[1]);
                char* a = engine->filesystem->f_gets_entire(h);
                vsxTiXmlDocument doc;
                doc.Parse(a);
                free(a);
                if (c_model->loadCoreSkeleton(doc)) {
                  #ifdef VSXU_DEBUG
                  printf("loaded skeleton: %s\n",parts[1].c_str());
                  #endif
                } else {
                  #ifdef VSXU_DEBUG
                  printf("failed loading skeleton.. %s \n",(file_path+parts[1]).c_str());
                  #endif
                }
                engine->filesystem->f_close(h);
              }
            }
            if (parts[0] == "mesh") {
              vsxf_handle* h = engine->filesystem->f_open((file_path+parts[1]).c_str(),"r");
              if (h) {
                resources.push_back(file_path+parts[1]);
                char* a = engine->filesystem->f_gets_entire(h);
                vsxTiXmlDocument doc;
                doc.Parse(a);
                free(a);
                mesh_id = c_model->loadCoreMesh(doc);
                if (mesh_id == -1) {
                  #ifdef VSXU_DEBUG
                  printf("failed loading mesh.. %s \n",(file_path+parts[1]).c_str());
                  #endif
                } else
                {
                  #ifdef VSXU_DEBUG
                  printf("loaded mesh: %s\n",parts[1].c_str());
                  #endif
                  mesh_parts.push_back(mesh_id);
                }
                engine->filesystem->f_close(h);
              }
            }
          }
        }
        engine->filesystem->f_close(fp);
        #ifdef VSXU_DEBUG
        printf("creating new model\n");
        #endif
        m_model = new CalModel(c_model);
        m_model->attachMesh(mesh_id);

        m_model->update(0.0f);
        CalSkeleton* m_skeleton = m_model->getSkeleton();

        int b_id = 0;
        std::vector<CalCoreBone*> blist = c_model->getCoreSkeleton()->getVectorCoreBone();
        for (std::vector<CalCoreBone*>::iterator it = blist.begin(); it != blist.end(); ++it) {
          bone_info b1;
          b1.name = (*it)->getName().c_str();
          b1.bone = 0;
          b1.bone = m_skeleton->getBone(b_id);
          b1.o_t = b1.bone->getTranslation();
          bones.push_back(b1);
          ++b_id;
        }
        redeclare_in = true;
        redeclare_out = true;
        CalBone* m_bone_0 = m_skeleton->getBone(2);

        CalQuaternion q2;
        q2 = m_bone_0->getRotation();
        quat_p->set(q2.z,0);
        quat_p->set(q2.y,1);
        quat_p->set(q2.x,2);
        quat_p->set(q2.w,3);

        // enable tangent space calculations

        /*std::vector<CalSubmesh *>mesh_list = m_model->getMesh(mesh_id)->getVectorSubmesh();
        for (std::vector<CalSubmesh *>::iterator it = mesh_list.begin(); it != mesh_list.end(); it++)
        {
          (*it)->enableTangents(0, true);
        }*/

        loading_done = true;
      }
    }
    pthread_mutex_unlock(&mesh_mutex);
  }


  static void* worker(void *ptr)
  {
    cal3d_thread_info thread_info= *(cal3d_thread_info*)ptr;
    module_mesh_cal3d_import* my = ((module_mesh_cal3d_import*)  thread_info.class_pointer);
    unsigned long exit_check_counter = 0;
    int first_rendering = 0;

    #if (PLATFORM == PLATFORM_LINUX)
      if (thread_info.is_thread)
      {
        const char* cal = "cal3d::worker";
        prctl(PR_SET_NAME,cal);
      }
    #endif
    while (1)
    {
      if (thread_info.is_thread)
      {
        // lock mutex
        pthread_mutex_lock(&my->mesh_mutex);

        // wait for more work
        pthread_cond_wait(&my->count_threshold_cv, &my->mesh_mutex);
      }

      //printf("cal3d %d\n",__LINE__);
      CalSkeleton* m_skeleton = my->m_model->getSkeleton();
      m_skeleton->calculateState();

      CalRenderer *pCalRenderer;
      pCalRenderer = my->m_model->getRenderer();
      pCalRenderer->beginRendering();
      int meshCount;
      meshCount = pCalRenderer->getMeshCount();

      int meshId;
      for(meshId = 0; meshId < meshCount; meshId++)
      {
        // get the number of submeshes
        int submeshCount;
        submeshCount = pCalRenderer->getSubmeshCount(meshId);
        //printf("submesh count: %d\n",submeshCount);

        // loop through all submeshes of the mesh
        int submeshId;
        for(submeshId = 0; submeshId < submeshCount; submeshId++)
        {
          // select mesh and submesh for further data access
          if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
          {
            my->mesh->data->vertices[pCalRenderer->getVertexCount()+1] = vsx_vector(0,0,0);
            pCalRenderer->getVertices(&my->mesh->data->vertices[0].x);

            my->mesh->data->vertex_normals[pCalRenderer->getVertexCount()+1] = vsx_vector(0,0,0);
            pCalRenderer->getNormals(&my->mesh->data->vertex_normals[0].x);

            if (pCalRenderer->isTangentsEnabled(0))
            {
              //my->mesh->data->vertex_tangents[pCalRenderer->getVertexCount()+1].x = 0;// = vsx_vector(0,0,0);
              //int num_tagentspaces = pCalRenderer->getTangentSpaces(0,&my->mesh->data->vertex_tangents[0].x);
              //printf("fetched %d tangents\n", num_tagentspaces);
            }
            //else printf("Tangents are NOT enabled\n");


            if (first_rendering < 4)
            {
              my->mesh->data->vertex_tex_coords[pCalRenderer->getVertexCount()+1].s = 0;
              pCalRenderer->getTextureCoordinates(0,&my->mesh->data->vertex_tex_coords[0].s);

              int faceCount = pCalRenderer->getFaceCount();
              (my->mesh)->data->faces.allocate(faceCount*3);
              pCalRenderer->getFaces((int*)&(my->mesh)->data->faces[0].a);
              (my->mesh)->data->faces.reset_used(faceCount);
              first_rendering++;
            }
          }
        }
      }

      // end the rendering of the model
      pCalRenderer->endRendering();

      // ********************************************************************
      // perform transforms

      my->post_rot_translate_vec += my->rot_center;

      my->pre_rotation_mat = my->pre_rotation_quaternion.matrix();
      my->rotation_mat = my->rotation_quaternion.matrix();

      unsigned long end = (my->mesh)->data->vertices.size();
      vsx_vector* vs_v = &(my->mesh)->data->vertices[0];
      vsx_vector* vs_n = &(my->mesh)->data->vertex_normals[0];


      for (unsigned long i = 0; i < end; i++)
      {
        // pre rotation
        vs_v->multiply_matrix_other_vec
        (
          &my->pre_rotation_mat.m[0],
          *vs_v - my->pre_rot_center
        );
        (*vs_v) += my->pre_rot_center;
        vsx_vector n = *vs_n;
        vs_n->multiply_matrix_other_vec(
          &my->pre_rotation_mat.m[0],
          n
        );


        // post rotation
        vs_v->multiply_matrix_other_vec
        (
          &my->rotation_mat.m[0],
          *vs_v - my->rot_center
        );
        // vertex offset
        (*vs_v) += my->post_rot_translate_vec;
        // normal rotation
        n = *vs_n;
        vs_n->multiply_matrix_other_vec(
          &my->rotation_mat.m[0],
          n
        );
        vs_v++;
        vs_n++;
      }
      //printf("thread dtime: %f\n", time.dtime());

      // ********************************************************************
      // calculate tangent space coordinates

      vsx_mesh* mesh = my->mesh;

      mesh->data->vertex_colors.allocate( mesh->data->vertices.size() );
      mesh->data->vertex_colors.memory_clear();

      vsx_quaternion* vec_d = (vsx_quaternion*)mesh->data->vertex_colors.get_pointer();

      for (unsigned long a = 0; a < mesh->data->faces.size(); a++)
      {
        long i1 = mesh->data->faces[a].a;
        long i2 = mesh->data->faces[a].b;
        long i3 = mesh->data->faces[a].c;

        const vsx_vector& v1 = mesh->data->vertices[i1];
        const vsx_vector& v2 = mesh->data->vertices[i2];
        const vsx_vector& v3 = mesh->data->vertices[i3];

        const vsx_tex_coord& w1 = mesh->data->vertex_tex_coords[i1];
        const vsx_tex_coord& w2 = mesh->data->vertex_tex_coords[i2];
        const vsx_tex_coord& w3 = mesh->data->vertex_tex_coords[i3];

        float x1 = v2.x - v1.x;
        float x2 = v3.x - v1.x;
        float y1 = v2.y - v1.y;
        float y2 = v3.y - v1.y;
        float z1 = v2.z - v1.z;
        float z2 = v3.z - v1.z;

        float s1 = w2.s - w1.s;
        float s2 = w3.s - w1.s;
        float t1 = w2.t - w1.t;
        float t2 = w3.t - w1.t;

        float r = 1.0f / (s1 * t2 - s2 * t1);
        vsx_quaternion sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        //vsx_vector sdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,(s1 * z2 - s2 * z1) * r);

        vec_d[i1] += sdir;
        vec_d[i2] += sdir;
        vec_d[i3] += sdir;

        //tan2[i1] += tdir;
        //tan2[i2] += tdir;
        //tan2[i3] += tdir;
      }
      for (unsigned long a = 0; a < mesh->data->vertices.size(); a++)
      {
          vsx_vector& n = mesh->data->vertex_normals[a];
          vsx_quaternion& t = vec_d[a];

          // Gram-Schmidt orthogonalize
          //vec_d[a] = (t - n * t.dot_product(&n) );
          vec_d[a] = (t - n * t.dot_product(&n) );
          vec_d[a].normalize();

          // Calculate handedness
          //tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
      }

      my->thread_has_something_to_deliver++;
      //printf("cal3d %d\n",__LINE__);
      if (thread_info.is_thread)
      {

        pthread_mutex_unlock(&my->mesh_mutex);
      }
      //printf("cal3d %d\n",__LINE__);

      // if we're not supposed to run in a thread
      if (false == thread_info.is_thread)
      {
        return 0;
      }

      {
        exit_check_counter = 0;
        // see if the exit flag is set (user changed from threaded to non-threaded mode)
        //printf("cal3d %d\n",__LINE__);
        pthread_mutex_lock(&my->thread_exit_mutex);
        //printf("cal3d %d\n",__LINE__);
        int time_to_exit = my->thread_exit;
        //printf("cal3d %d\n",__LINE__);
        pthread_mutex_unlock(&my->thread_exit_mutex);
        //printf("cal3d %d\n",__LINE__);
        if (time_to_exit) {
          int *retval = new int;
          *retval = 0;
          my->thread_exit = 0;
          pthread_exit((void*)retval);
          return 0;
        }
      }
    }
    return 0;
  }

  void run()
  {
    if (!m_model)
      return;
    if (!bones.size())
      return;

    //printf("cal3d module run!\n");
    // deal with changes in threading use
    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->e( "cal3d_run_1" );
    #endif

    if (thread_created && use_thread->get() == 0)
    {
      // this means the thread is running. kill it off.
      //printf("cal3d %d\n",__LINE__);
      pthread_mutex_lock(&thread_exit_mutex);
      //printf("cal3d %d\n",__LINE__);
        thread_exit = 1;
      //printf("cal3d %d\n",__LINE__);
      pthread_mutex_unlock(&thread_exit_mutex);
      //printf("cal3d %d\n",__LINE__);
      void* ret;
      pthread_join(worker_t,&ret);
      thread_created = false;
      thread_info.is_thread = false;
    }
    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->l();
    #endif

    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->e( "cal3d_run_2" );
    #endif
    if (!thread_created && use_thread->get() == 1)
    {
      //printf("cal3d %d\n",__LINE__);
      pthread_create(&worker_t, NULL, &worker, (void*)&thread_info);
      //printf("cal3d %d\n",__LINE__);
      thread_created = true;
      thread_info.is_thread = true;
    }
    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->l();
    #endif

    if (0 == use_thread->get())
    {
      thread_info.is_thread = false;
      worker((void*)&thread_info);
    }

    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->e( "cal3d_run_consume_outer" );
    #endif

    if (0 == pthread_mutex_lock(&mesh_mutex) )
    {
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->e( "cal3d_run_consume_inner" );
      #endif
      // lock ackquired. thread is waiting for us to set the semaphore before doing anything again.
      /*bool wait = true;
      if (!thread_has_something_to_deliver && have_sent_work_to_thread)
      {
        while (wait)
        {
          pthread_mutex_unlock(&mesh_mutex);
          pthread_mutex_lock(&mesh_mutex);
          if (thread_has_something_to_deliver) wait = false;
        }
      }*/
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->e( "cal3d_thread_has_delivery" );
      #endif

      if (thread_has_something_to_deliver)
      {
        thread_has_something_to_deliver = false;
        module_mesh_cal3d_import* my = this;


#ifdef VSXU_TM
((vsx_tm*)engine->tm)->e( "cal3d_calculateboundingboxes" );
#endif
        m_model->getSkeleton()->calculateBoundingBoxes();
#ifdef VSXU_TM
((vsx_tm*)engine->tm)->l();
#endif

        if (!my->redeclare_out)
        {
          mesh_bbox->data->vertices.allocate(my->bones.size() * 8);

          #ifdef VSXU_TM
          ((vsx_tm*)engine->tm)->e( "cal3d_calcbones" );
          #endif
          for (unsigned long j = 0; j < my->bones.size(); ++j)
          {
            if (my->bones[j].bone != 0)
            {

              CalVector t1 = my->bones[j].bone->getTranslationAbsolute();

              CalQuaternion q2 = my->bones[j].bone->getRotationAbsolute();

              /*
              // slow line!!!
              my->bones[j].bone->getCoreBone()->calculateBoundingBox(m_model->getCoreModel());
              my->bones[j].bone->calculateBoundingBox();

              CalBoundingBox bbox = my->bones[j].bone->getBoundingBox();
              CalVector bboxv[8];
              bbox.computePoints((CalVector*)&bboxv);
              for (unsigned long bbi = 0; bbi < 8; bbi++)
              {
                mesh_bbox->data->vertices[j*8+bbi].x = bboxv[bbi].x;
                mesh_bbox->data->vertices[j*8+bbi].y = bboxv[bbi].y;
                mesh_bbox->data->vertices[j*8+bbi].z = bboxv[bbi].z;
              }
            */


              my->bones[j].result_rotation   ->set( q2.x, 0 );
              my->bones[j].result_rotation   ->set( q2.y, 1 );
              my->bones[j].result_rotation   ->set( q2.z, 2 );
              my->bones[j].result_rotation   ->set( q2.w, 3 );

              my->bones[j].result_translation->set( t1.x, 0 );
              my->bones[j].result_translation->set( t1.y, 1 );
              my->bones[j].result_translation->set( t1.z, 2 );

            }
          }
        }
        #ifdef VSXU_TM
        ((vsx_tm*)engine->tm)->l();
        #endif

        #ifdef VSXU_TM
        ((vsx_tm*)engine->tm)->e( "cal3d_handle_mesh_pointers" );
        #endif
        mesh->timestamp++;
        result->set(mesh);

        // toggle to the other mesh
        if (mesh == mesh_a)
          mesh = mesh_b;
        else mesh = mesh_a;
        #ifdef VSXU_TM
        ((vsx_tm*)engine->tm)->l();
        #endif
      }
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->l();
      #endif

      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->e( "cal3d_handle_p_updates" );
      #endif

      if (p_updates != param_updates)
      {
        CalQuaternion q2;
        CalVector t1;
        for (unsigned long j = 0; j < bones.size(); ++j)
        {
          t1.x = bones[j].translation->get(0);
          t1.y = bones[j].translation->get(1);
          t1.z = bones[j].translation->get(2);
          q2.x = bones[j].param->get(0);
          q2.y = bones[j].param->get(1);
          q2.z = bones[j].param->get(2);
          q2.w = bones[j].param->get(3);
          if (bones[j].bone != 0) {
            bones[j].bone->setRotation(q2);
            bones[j].bone->setTranslation(bones[j].o_t + t1);
          }
        }

        pre_rotation_quaternion.x = pre_rotation->get(0);
        pre_rotation_quaternion.y = pre_rotation->get(1);
        pre_rotation_quaternion.z = pre_rotation->get(2);
        pre_rotation_quaternion.w = pre_rotation->get(3);

        pre_rot_center.x = pre_rotation_center->get(0);
        pre_rot_center.y = pre_rotation_center->get(1);
        pre_rot_center.z = pre_rotation_center->get(2);

        rotation_quaternion.x = rotation->get(0);
        rotation_quaternion.y = rotation->get(1);
        rotation_quaternion.z = rotation->get(2);
        rotation_quaternion.w = rotation->get(3);

        rot_center.x = rotation_center->get(0);
        rot_center.y = rotation_center->get(1);
        rot_center.z = rotation_center->get(2);

        post_rot_translate_vec.x = post_rot_translate->get(0);
        post_rot_translate_vec.y = post_rot_translate->get(1);
        post_rot_translate_vec.z = post_rot_translate->get(2);

        p_updates = param_updates;
        //printf("cal3d %d\n",__LINE__);
        have_sent_work_to_thread = 1;
        //sem_post(&sem_worker_todo);
        pthread_cond_signal(&count_threshold_cv);

        //printf("cal3d %d\n",__LINE__);
      }
      pthread_mutex_unlock(&mesh_mutex);

      // p_updates
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->l();
      #endif


      // inner
      #ifdef VSXU_TM
      ((vsx_tm*)engine->tm)->l();
      #endif
    }
    #ifdef VSXU_TM
    ((vsx_tm*)engine->tm)->l();
    #endif

  }
};
