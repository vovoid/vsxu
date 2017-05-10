#include "cal3d/cal3d.h"
#define VSXU_DEBUG 1
#if (PLATFORM == PLATFORM_LINUX)
  #include <sys/prctl.h>
#endif
#include <time/vsx_timer.h>

#include <profiler/vsx_profiler_manager.h>

using namespace cal3d;

typedef struct {
  CalBone* bone;
  vsx_string<>name;
  vsx_module_param_quaternion* param;
  vsx_module_param_float3* translation;
  vsx_module_param_quaternion* result_rotation;
  vsx_module_param_float3* result_translation;
  CalVector o_t;
} bone_info;

class bone_result
{
public:
  vsx_quaternion<> rotation;
  vsx_vector3f translation;
};

typedef struct {
  vsx_string<> name;
  int id;
  vsx_module_param_float* weight;
} morph_info;

typedef struct {
  std::atomic_uint_fast64_t is_thread;
  void* class_pointer;
} cal3d_thread_info;

class module_mesh_cal3d_import : public vsx_module {
public:
	VSXP_CLASS_DECLARE
    // in
    vsx_module_param_resource* filename;
    vsx_module_param_quaternion* quat_p;
    vsx_module_param_int* use_thread;

    vsx_module_param_float3* pre_rotation_center;
    vsx_module_param_quaternion* pre_rotation;

    vsx_module_param_float3* rotation_center;
    vsx_module_param_quaternion* rotation;
    vsx_module_param_float3* post_rot_translate;

    vsx_module_param_int* wait_for_thread;
    vsx_module_param_int* thread_sync_strategy;

    // out
    vsx_module_param_mesh* result;
    vsx_module_param_mesh* bones_bounding_box;

    // internal
    vsx_mesh<>* mesh_a;
    vsx_mesh<>* mesh_b;
    vsx_mesh<>* mesh_bbox;
    bool first_run;
    int n_rays;
    vsx_string<>current_filename;
    CalCoreModel* c_model;
    CalModel* m_model;

    vsx_nw_vector<bone_info> bones;
    vsx_nw_vector<bone_result> bones_result;
    vsx_nw_vector<morph_info> morphs;

    // threading stuff
    std::thread worker_thread;
    vsx_mesh<>* mesh = 0x0; // locked by the mesh

    int p_updates;
    bool              worker_running;
    bool              thread_created;
    int               thread_state;
    uint64_t               times_run;
    cal3d_thread_info thread_info;

    std::atomic_uint_fast64_t worker_produce;
    std::atomic_uint_fast64_t param_produce;
    std::atomic_uint_fast64_t thread_exit;

    float time_to_animate = 1.0f / 120.0f;

    // transform
    vsx_quaternion<> pre_rotation_quaternion;
    vsx_matrix<float> pre_rotation_mat;
    vsx_vector3<> pre_rot_center;

    vsx_quaternion<> rotation_quaternion;
    vsx_matrix<float> rotation_mat;
    vsx_vector3<> rot_center;
    vsx_vector3<> post_rot_translate_vec;

  module_mesh_cal3d_import()
  {
    VSXP_CLASS_CONSTRUCTOR

    m_model = 0;
    c_model = 0;
    thread_state = 0;
    thread_exit = 0;
    worker_running = false;
    thread_created = false;
    p_updates = -1;

    // signalling
    worker_produce = 0;
    param_produce = 0;
    thread_exit = 0;

    times_run = 0;

    thread_info.class_pointer = 0x0;
    thread_info.is_thread = 0;
  }

  bool init()
  {
    return true;
  }

  void on_delete()
  {
    if (thread_created)
    {
      thread_exit.fetch_add(1);

      // make thread do a dry run if not already running
      if (!param_produce.load())
          param_produce.fetch_add(1);

      if (param_produce.load()) // we have sent some work to the thread
        while ( param_produce.load() && !worker_produce.load())
        {}

      worker_thread.join();
    }

    if (c_model) {
      delete (CalCoreModel*)c_model;
    }

    if (m_model)
      delete m_model;

    delete mesh_a;
    delete mesh_b;
    delete mesh_bbox;
  }

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "mesh;importers;cal3d_importer";

    info->description = "";

    info->in_param_spec =
      "filename:resource,"
      "use_thread:enum?no|yes,"
      "wait_for_thread:enum?no|yes,"
      "thread_sync_strategy:enum?sleep|yield|active,"
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

    info->component_class =
      "mesh";

    if (bones.size()) {
      info->in_param_spec += ",bones:complex{";
      info->out_param_spec += ",absolutes:complex{";
      for (unsigned long i = 0; i < bones.size(); ++i)
      {
        if (i) {
          info->in_param_spec += ",";
          info->out_param_spec += ",";
        }
        info->in_param_spec += bones[i].name+":complex{"+bones[i].name+"_rotation:quaternion,"+bones[i].name+"_translation:float3}";
        info->out_param_spec += bones[i].name+":complex{"+bones[i].name+"_rotation:quaternion,"+bones[i].name+"_translation:float3}";
      }
      info->in_param_spec += "}";
      info->out_param_spec += "}";
    }

    if (morphs.size())
    {
      info->in_param_spec += ",morph_targets:complex{";
      for ( size_t i = 0; i < morphs.size(); i++)
      {
        if (i)
          info->in_param_spec += ",";
        info->in_param_spec += morphs[i].name+"_weight:float";
      }
      info->in_param_spec += "}";
    }

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

    wait_for_thread = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"wait_for_thread");
    wait_for_thread->set(0);

    thread_sync_strategy = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"thread_sync_strategy");
    thread_sync_strategy->set(0);


    use_thread = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_thread");
    use_thread->set(0);


    for (unsigned long i = 0; i < bones.size(); ++i)
    {
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

    for ( size_t i = 0; i < morphs.size(); i++)
    {
      morphs[i].weight = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,(morphs[i].name+"_weight").c_str());
      morphs[i].weight->set(0.0f);
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
    mesh_a = new vsx_mesh<>;
    mesh_b = new vsx_mesh<>;
    mesh = mesh_a;
    mesh_bbox = new vsx_mesh<>;

    loading_done = false;
    current_filename = "";
    redeclare_in_params(in_parameters);
    redeclare_out_params(out_parameters);
    first_run = true;
    c_model = 0;
    thread_info.class_pointer = (void*)this;
  }

  void param_set_notify(const vsx_string<>& name)
  {
    if (param_produce.load())
      while (param_produce.load())
      {}


    if (name == "filename") {
      if (filename->get() != current_filename) {
        current_filename = filename->get();
        // first find the path of the file
        vsx_nw_vector< vsx_string<> > fparts;
        vsx_string<>fdeli = "/";
        vsx_string<>file_path = "";
        vsx_string_helper::explode(filename->get(), fdeli, fparts);
        if (fparts.size() > 1) {
          fparts.reset_used(fparts.get_used()-1);
          file_path = vsx_string_helper::implode(fparts,fdeli)+"/";
        }
        //-------------------------------------------------
        vsx_nw_vector<int> mesh_parts;
        vsx_nw_vector<int> material_parts;
        vsx::file *fp;
        fp = engine_state->filesystem->f_open(current_filename.c_str());
        if (!fp) {
          return;
        }

        c_model = new CalCoreModel("core");
        char buf[1024];
        vsx_string<>line;
        int mesh_id = 0;
        while (engine_state->filesystem->f_gets(buf,1024,fp))
        {
          line = buf;

          req_continue(line.size());
          if (line[line.size()-1] == 0x0A) line.pop_back();
          req_continue(line.size());

          if (line[line.size()-1] == 0x0D) line.pop_back();
          req_continue(line.size());

          if (line.size()) {
            vsx_nw_vector< vsx_string<> > parts;
            vsx_string<>deli = "=";
            vsx_string_helper::explode(line, deli, parts);
            if (parts[0] == "skeleton") {
              vsx::file* h = engine_state->filesystem->f_open((file_path+parts[1]).c_str());
              if (h) {
                resources.push_back(file_path+parts[1]);
                char* a = engine_state->filesystem->f_gets_entire(h);
                TiXmlDocument doc;
                doc.Parse(a);
                free(a);
                CalCoreSkeletonPtr skeleton = CalLoader::loadXmlCoreSkeleton(doc);
                if (skeleton)
                  c_model->setCoreSkeleton( skeleton.get() );
                engine_state->filesystem->f_close(h);
              }
            }
            if (parts[0] == "mesh") {
              vsx::file* h = engine_state->filesystem->f_open((file_path+parts[1]).c_str());
              if (h) {
                resources.push_back(file_path+parts[1]);
                char* a = engine_state->filesystem->f_gets_entire(h);
                TiXmlDocument doc;
                doc.Parse(a);
                free(a);
                CalCoreMeshPtr mesh = CalLoader::loadXmlCoreMesh( doc );
                c_model->addCoreMesh( mesh.get() );
                if (mesh_id > -1)
                  mesh_parts.push_back(mesh_id);
                engine_state->filesystem->f_close(h);
              }
            }
          }
        }
        engine_state->filesystem->f_close(fp);
        m_model = new CalModel(c_model);
        m_model->attachMesh(mesh_id);

        m_model->update(0.0f);

        // Iterate over Bones
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
          bone_result res;
          bones_result.push_back(res);
          ++b_id;
        }

        int morph_target_count = m_model->getMesh(0)->getSubmesh(0)->getMorphTargetWeightCount();
        if (morph_target_count)
        {
          //CalMorphTargetMixer* m_morp_mixer = m_model->getMorphTargetMixer();
          std::vector<CalCoreSubMorphTarget *> morph_targets = m_model->getMesh(0)->getSubmesh(0)->getCoreSubmesh()->getVectorCoreSubMorphTarget();
          for( size_t i = 0; i < morph_targets.size(); i++)
          {
            morph_info mi;
            mi.id = i;
            mi.name = morph_targets[i]->getName().c_str();
            morphs.push_back( mi );
          }
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
  }


  void worker()
  {
    int first_rendering = 0;

    vsx_timer timer;
    #if (PLATFORM == PLATFORM_LINUX)
      if (thread_info.is_thread.load())
      {
        const char* cal = "cal3d::worker";
        prctl(PR_SET_NAME,cal);
      }
    #endif

    forever
    {
      if (thread_info.is_thread.load())
      {
        if (thread_sync_strategy->get() == 0)
          while (!param_produce.load())
            std::this_thread::sleep_for(std::chrono::microseconds(1));

        if (thread_sync_strategy->get() == 1)
          while (!param_produce.load())
            #if (PLATFORM == PLATFORM_LINUX)
              sched_yield();
            #else
              Sleep(0);
            #endif

        if (thread_sync_strategy->get() == 2)
          while (!param_produce.load())
          {}
      }

      timer.start();

      CalSkeleton* m_skeleton = m_model->getSkeleton();
      m_skeleton->calculateState();

      CalRenderer *pCalRenderer;
      pCalRenderer = m_model->getRenderer();
      pCalRenderer->beginRendering();
      int meshCount;
      meshCount = pCalRenderer->getMeshCount();

      int meshId;
      for(meshId = 0; meshId < meshCount; meshId++)
      {
        // get the number of submeshes
        int submeshCount;
        submeshCount = pCalRenderer->getSubmeshCount(meshId);

        // loop through all submeshes of the mesh
        int submeshId;
        for(submeshId = 0; submeshId < submeshCount; submeshId++)
        {
          // select mesh and submesh for further data access
          if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
          {
            mesh->data->vertices[pCalRenderer->getVertexCount()+1] = vsx_vector3<>(0,0,0);
            pCalRenderer->getVertices(&mesh->data->vertices[0].x);

            mesh->data->vertex_normals[pCalRenderer->getVertexCount()+1] = vsx_vector3<>(0,0,0);
            pCalRenderer->getNormals(&mesh->data->vertex_normals[0].x);

            if (pCalRenderer->isTangentsEnabled(0))
            {
              //mesh->data->vertex_tangents[pCalRenderer->getVertexCount()+1].x = 0;// = vsx_vector(0,0,0);
              //int num_tagentspaces = pCalRenderer->getTangentSpaces(0,&mesh->data->vertex_tangents[0].x);
            }


            if (first_rendering < 4)
            {
              mesh->data->vertex_tex_coords[pCalRenderer->getVertexCount()+1].s = 0;
              pCalRenderer->getTextureCoordinates(0,&mesh->data->vertex_tex_coords[0].s);

              int faceCount = pCalRenderer->getFaceCount();
              (mesh)->data->faces.allocate(faceCount*3);
              pCalRenderer->getFaces((int*)&(mesh)->data->faces[0].a);
              (mesh)->data->faces.reset_used(faceCount);
              first_rendering++;
            }
          }
        }
      }

      // end the rendering of the model
      pCalRenderer->endRendering();

      // ********************************************************************
      // perform transforms

      post_rot_translate_vec += rot_center;

      pre_rotation_mat = pre_rotation_quaternion.matrix();
      rotation_mat = rotation_quaternion.matrix();

      unsigned long end = (mesh)->data->vertices.size();
      vsx_vector3<>* vs_v = &(mesh)->data->vertices[0];
      vsx_vector3<>* vs_n = &(mesh)->data->vertex_normals[0];


      for (unsigned long i = 0; i < end; i++)
      {
        // pre rotation
        vs_v->multiply_matrix_other_vec
        (
          &pre_rotation_mat.m[0],
          *vs_v - pre_rot_center
        );
        (*vs_v) += pre_rot_center;
        vsx_vector3<> n = *vs_n;
        vs_n->multiply_matrix_other_vec(
          &pre_rotation_mat.m[0],
          n
        );


        // post rotation
        vs_v->multiply_matrix_other_vec
        (
          &rotation_mat.m[0],
          *vs_v - rot_center
        );
        // vertex offset
        (*vs_v) += post_rot_translate_vec;
        // normal rotation
        n = *vs_n;
        vs_n->multiply_matrix_other_vec(
          &rotation_mat.m[0],
          n
        );
        vs_v++;
        vs_n++;
      }

      // ********************************************************************
      // calculate tangent space coordinates

      mesh->data->vertex_colors.allocate( mesh->data->vertices.size() );
      mesh->data->vertex_colors.memory_clear();

      vsx_quaternion<>* vec_d = (vsx_quaternion<>*)mesh->data->vertex_colors.get_pointer();

      for (unsigned long a = 0; a < mesh->data->faces.size(); a++)
      {
        long i1 = mesh->data->faces[a].a;
        long i2 = mesh->data->faces[a].b;
        long i3 = mesh->data->faces[a].c;

        const vsx_vector3<>& v1 = mesh->data->vertices[i1];
        const vsx_vector3<>& v2 = mesh->data->vertices[i2];
        const vsx_vector3<>& v3 = mesh->data->vertices[i3];

        const vsx_tex_coord2f& w1 = mesh->data->vertex_tex_coords[i1];
        const vsx_tex_coord2f& w2 = mesh->data->vertex_tex_coords[i2];
        const vsx_tex_coord2f& w3 = mesh->data->vertex_tex_coords[i3];

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
        vsx_quaternion<> sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
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
          vsx_vector3<>& n = mesh->data->vertex_normals[a];
          vsx_quaternion<>& t = vec_d[a];

          // Gram-Schmidt orthogonalize
          //vec_d[a] = (t - n * t.dot_product(&n) );
          vec_d[a] = (t - n * t.dot_product(&n) );
          vec_d[a].normalize();
          vec_d[a].w = 1.0f;//(float)a;

          // Calculate handedness
          //tangent[a].w = (Dot(Cross(n, t), tan2[a]) < 0.0F) ? -1.0F : 1.0F;
      }

      update_bones_output();

      worker_produce.fetch_add(1);
      if (param_produce.load())
        param_produce.fetch_sub(1);

      time_to_animate = timer.dtime();

      // if we're not supposed to run in a thread
      if (0 == thread_info.is_thread.load())
        return;

      // see if the exit flag is set (user changed from threaded to non-threaded mode)
      if (thread_exit.load())
      {
        thread_exit = 0;
        return;
      }
    }
  }


  void update_bones_output()
  {
    vsx_matrix<float> pre_rotation_mat_local = pre_rotation_quaternion.matrix();
    vsx_matrix<float> rotation_mat_local = rotation_quaternion.matrix();

    for (unsigned long j = 0; j < bones.size(); ++j)
    {
      req_continue(bones[j].bone != 0);
      CalVector t1 = bones[j].bone->getTranslationAbsolute();
      vsx_vector3f vs_v;
      vs_v.x = t1.x;
      vs_v.y = t1.y;
      vs_v.z = t1.z;

      CalQuaternion q2 = bones[j].bone->getRotationAbsolute();

      // pre rotation
      vs_v.multiply_matrix_other_vec
      (
        &pre_rotation_mat_local.m[0],
        vs_v - pre_rot_center
      );
      vs_v += pre_rot_center;

      // post rotation
      vs_v.multiply_matrix_other_vec
      (
        &rotation_mat_local.m[0],
        vs_v - rot_center
      );
      // vertex offset
      vs_v.x += post_rot_translate->get(0) + rotation_center->get(0);
      vs_v.y += post_rot_translate->get(1) + rotation_center->get(1);
      vs_v.z += post_rot_translate->get(2) + rotation_center->get(2);

      bones_result[j].translation = vs_v;
      bones_result[j].rotation.x = q2.x;
      bones_result[j].rotation.y = q2.y;
      bones_result[j].rotation.z = q2.z;
      bones_result[j].rotation.w = q2.w;

//      bones[j].result_translation->set( vs_v.x, 0 );
//      bones[j].result_translation->set( vs_v.y, 1 );
//      bones[j].result_translation->set( vs_v.z, 2 );

//      bones[j].result_rotation   ->set( q2.x, 0 );
//      bones[j].result_rotation   ->set( q2.y, 1 );
//      bones[j].result_rotation   ->set( q2.z, 2 );
//      bones[j].result_rotation   ->set( q2.w, 3 );
    }
  }

  void run()
  {
    if (!m_model)
      return;
    if (!bones.size())
      return;

VSXP_CLASS_LOCAL_INIT

VSXP_S_BEGIN("cal3d run");

    // deal with changes in threading use

    if (thread_created && use_thread->get() == 0)
    {
      // this means the thread is running. kill it off.
      thread_exit.fetch_add(1);

      // make thread do a dry run if not already running
      if (!param_produce.load())
        param_produce.fetch_add(1);

      if (param_produce.load()) // we have sent some work to the thread
        while (param_produce.load() && !worker_produce.load() )
        {}

      worker_thread.join();
      thread_created = false;
      thread_info.is_thread = 0;
    }

    if (!thread_created && use_thread->get() == 1)
    {
      thread_info.is_thread.fetch_add(1);
      worker_thread = std::thread( [this](){worker();} );
      thread_created = true;
    }

    if (0 == use_thread->get())
    {
      thread_info.is_thread = 0;
      worker();
    }

    if (thread_info.is_thread.load())
      if ( wait_for_thread->get() )
        if (times_run++ > 60 && engine_state->dtime > 0.01)
        {
          if (thread_sync_strategy->get() == 0)
            while (!worker_produce.load())
              std::this_thread::sleep_for(std::chrono::microseconds(1));

          if (thread_sync_strategy->get() == 1)
            while (!worker_produce.load())
              #if (PLATFORM == PLATFORM_LINUX)
                sched_yield();
              #else
                Sleep(0);
              #endif

          if (thread_sync_strategy->get() == 2)
            while (!worker_produce.load() && param_produce.load())
            {}
        }

    if (worker_produce.load() == 1)
    {
      worker_produce.fetch_sub(1);
      // lock ackquired. thread is waiting for us to set the semaphore before doing anything again.
      m_model->getSkeleton()->calculateBoundingBoxes();

      if (!redeclare_out)
      {
        mesh_bbox->data->vertices.allocate(bones.size() * 8);

        foreach (bones, i)
        {
          bones[i].result_translation->set( bones_result[i].translation.x, 0 );
          bones[i].result_translation->set( bones_result[i].translation.y, 1 );
          bones[i].result_translation->set( bones_result[i].translation.z, 2 );
          bones[i].result_rotation   ->set( bones_result[i].rotation.x, 0 );
          bones[i].result_rotation   ->set( bones_result[i].rotation.y, 1 );
          bones[i].result_rotation   ->set( bones_result[i].rotation.z, 2 );
          bones[i].result_rotation   ->set( bones_result[i].rotation.w, 3 );
        }

        for (unsigned long j = 0; j < morphs.size(); ++j)
          m_model->getMesh(0)->getSubmesh(0)->
            setMorphTargetWeight(morphs[j].id, morphs[j].weight->get() );
      }

      mesh->timestamp++;
      result->set(mesh);

      // toggle to the other mesh
      if (mesh == mesh_a)
        mesh = mesh_b;
      else
        mesh = mesh_a;
    }

    if
    (
        p_updates != param_updates
        &&
        !param_produce.load()
        &&
        !worker_produce.load()
    )
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
      param_produce.fetch_add(1);
    }
    VSXP_S_END
  }
};
