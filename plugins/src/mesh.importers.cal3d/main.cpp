#include "_configuration.h"
#include "main.h"
#include "vsx_math_3d.h"
//#include "vsx_string_lib.h"
#include <pthread.h>
#include "cal3d.h"

typedef struct {
  CalBone* bone;
  vsx_string name;
  vsx_module_param_quaternion* param;
  vsx_module_param_float3* translation;
  vsx_module_param_quaternion* result_rotation;
  vsx_module_param_float3* result_translation;
  CalVector o_t;
} bone_info;

/*class vsx_module_cal3d_loader : public vsx_module {
  // in
  vsx_module_param_resource* filename;
  vsx_module_param_quaternion* quat_p;
  // out
  vsx_module_param_mesh* result;
  // internal
  vsx_mesh mesh;
  bool first_run;
  int n_rays;
  vsx_string current_filename;
  CalCoreModel* c_model;
  CalModel* m_model;
  vsx_avector<bone_info> bones;
public:
  vsx_module_cal3d_loader() {
    m_model = 0;
    c_model = 0;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;importers;cal3d_importer";
    info->description = "";
    info->in_param_spec = "filename:resource";
    info->out_param_spec = "mesh:mesh";
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
    quat_p = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"bone_1");
    quat_p->set(0.0f,0);
    quat_p->set(0.0f,1);
    quat_p->set(0.0f,2);
    quat_p->set(1.0f,3);
    if (bones.size()) {
      for (unsigned long i = 0; i < bones.size(); ++i) {
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
    result->set_p(mesh);
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
    loading_done = false;
    current_filename = "";
    redeclare_in_params(in_parameters);
    redeclare_out_params(out_parameters);
    first_run = true;
    c_model = 0;
  }

  void param_set_notify(const vsx_string& name) {
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
#if defined(VSXU_DEBUG)
        printf("file path: %s\n",file_path.c_str());
#endif
        //-------------------------------------------------
        vsx_avector<int> mesh_parts;
        vsx_avector<int> material_parts;
        vsxf_handle *fp;
        fp = engine->filesystem->f_open(current_filename.c_str(), "r");
        if (!fp) return;
    
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
                TiXmlDocument doc;
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
                TiXmlDocument doc;
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
            //if (parts[0] == "material") {
              //vsxf_handle* h = engine->filesystem->f_open((file_path+parts[1]).c_str(),"r");
              //if (h) {
                //resources.push_back(file_path+parts[1]);
                //char* a = engine->filesystem->f_gets_entire(h);
                //TiXmlDocument doc;
                //doc.Parse(a);
                //delete a;
                //int material_id = c_model->loadCoreMaterial(doc);
                //if (material_id == -1) {
                  //printf("failed loading material.. %s \n",(file_path+parts[1]).c_str(),"r");
                //} else
                //{
                  //printf("loaded material: %s\n",parts[1].c_str());
                  //material_parts.push_back(material_id);
                //}
                //engine->filesystem->f_close(h);
              //}
            //}
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
        #ifdef VSXU_DEBUG
          printf("num bones: %d\n",(int)bones.size());
        #endif
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
        std::vector<CalSubmesh *>mesh_list = m_model->getMesh(mesh_id)->getVectorSubmesh();
        for (std::vector<CalSubmesh *>::iterator it = mesh_list.begin(); it != mesh_list.end(); it++)
        {
          (*it)->enableTangents(0, true);
        }
    
    
        loading_done = true;
        return;
      }
      
    }
  }


  void run() {
    //printf("cal3d run\n");
    if (!m_model) return;
    CalSkeleton* m_skeleton = m_model->getSkeleton();
    //CalBone* m_bone_0 = m_skeleton->getBone(2);
      //printf("cal3d run2\n");
    if (bones.size()) {
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
    }
      
  //    CalQuaternion q1;
  //    q1.x = quat_p->get(0);
  //    q1.y = quat_p->get(1);
  //    q1.z = quat_p->get(2);
  //    q1.w = quat_p->get(3);
  //    
  //    m_bone_0->setRotation(q1);
  //    CalQuaternion q2;
  //    q2 = m_bone_0->getRotation();
      //printf("q2.x: %f\n",q2.x);
      m_skeleton->calculateState();
      //m_model->update(0.0f);


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
        //printf("submesh count: %d\n",submeshCount);
  
        // loop through all submeshes of the mesh
        int submeshId;
        for(submeshId = 0; submeshId < submeshCount; submeshId++)
        {
          // select mesh and submesh for further data access
          if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
          {
            //printf("a");
            // get the material colors
            //unsigned char ambientColor[4], diffuseColor[4], specularColor[4];
            //pCalRenderer->getAmbientColor(&ambientColor[0]);
            //pCalRenderer->getDiffuseColor(&diffuseColor[0]);
            //pCalRenderer->getSpecularColor(&specularColor[0]);
  
            // get the material shininess factor
            //float shininess;
            //shininess = pCalRenderer->getShininess();
  
            // get the transformed vertices of the submesh
            //static float meshVertices[30000][3];
            
            mesh.data->vertices[pCalRenderer->getVertexCount()+1] = vsx_vector(0,0,0);
            pCalRenderer->getVertices(&mesh.data->vertices[0].x);
            //pCalRenderer->getVertices(&meshVertices[0][0]);
            //printf("number of vertices: %d\n",pCalRenderer->getVertexCount());
            //printf("vertex0.x: %f\n",meshVertices[0][0]);
            
  //printf("b");
          //// get the transformed normals of the submesh
          //static float meshNormals[30000][3];
          mesh.data->vertex_normals[pCalRenderer->getVertexCount()+1] = vsx_vector(0,0,0);
          pCalRenderer->getNormals(&mesh.data->vertex_normals[0].x);

          //printf("caltangent size: %d\n",sizeof(CalCoreSubmesh::TangentSpace));
          //printf("vsx_vector size: %d\n",sizeof(vsx_vector));

          if (pCalRenderer->isTangentsEnabled(0)) {
            //printf("Tangents are enabled\n");
            mesh.data->vertex_tangents[pCalRenderer->getVertexCount()+1].x = 0;// = vsx_vector(0,0,0);
            int num_tagentspaces = pCalRenderer->getTangentSpaces(0,&mesh.data->vertex_tangents[0].x);
            //printf("fetched %d tangents\n", num_tagentspaces);
          }
          //else printf("Tangents are NOT enabled\n");
          

          mesh.data->vertex_tex_coords[pCalRenderer->getVertexCount()+1].s = 0;
          pCalRenderer->getTextureCoordinates(0,&mesh.data->vertex_tex_coords[0].s);
  
          // get the texture coordinates of the submesh
          // (only for the first map as example, others can be accessed in the same way though)
          //static float meshTextureCoordinates[30000][2];
          //int textureCoordinateCount;
          //textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, &meshTextureCoordinates[0][0]);
  
          // get the stored texture identifier
          // (only for the first map as example, others can be accessed in the same way though)
          //Cal::UserData textureId;
          //textureId = pCalRenderer->getMapUserData(0)
  
          //[ set the material, vertex, normal and texture states in the graphic-API here ]
  //printf("c");
          // get the faces of the submesh
          //static int meshFaces[50000][3];
          int faceCount = pCalRenderer->getFaceCount();
          //printf("facecount: %d\n",faceCount);
//          vsx_face a;

          //printf("allocated: %d\n",mesh.data->faces.get_allocated());
          //mesh.data->faces[faceCount*3] = a;
          mesh.data->faces.allocate(faceCount*3);
          //printf("allocated: %d\n",mesh.data->faces.get_allocated());
          pCalRenderer->getFaces((int*)&mesh.data->faces[0].a);
          mesh.data->faces.reset_used(faceCount);
          //printf("face id 0: %d\n",mesh.data->faces[10].a);
          //faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);
  //printf("d");
          //[ render the faces with the graphic-API here ]
        }
      }
    }
    // end the rendering of the model
    pCalRenderer->endRendering();

    if (bones.size())
    {
      CalQuaternion q2;
      CalVector t1;
      if (!redeclare_out)
      {
        for (unsigned long j = 0; j < bones.size(); ++j)
        {
          if (bones[j].bone != 0)
          {
            CalVector t1 = bones[j].bone->getTranslationAbsolute();
            CalQuaternion q2 = bones[j].bone->getRotationAbsolute();

            bones[j].result_rotation   ->set( -q2.x, 0 );
            bones[j].result_rotation   ->set( -q2.y, 1 );
            bones[j].result_rotation   ->set( -q2.z, 2 );
            bones[j].result_rotation   ->set( q2.w, 3 );
  
            bones[j].result_translation->set( t1.x, 0 );
            bones[j].result_translation->set( t1.y, 1 );
            bones[j].result_translation->set( t1.z, 2 );
          }
        }
      }
    }

    mesh.timestamp++;
    result->set_p(mesh);

    //printf("cal3d run3\n");
  }
  
  void on_delete() {
    if (c_model) {
      delete c_model;
    }
    //mesh.clear();
  }
};
*/
//******************************************************************************************
//******************************************************************************************
//******************************************************************************************


class vsx_module_cal3d_loader_threaded : public vsx_module {
public:
    // in
    vsx_module_param_resource* filename;
    vsx_module_param_quaternion* quat_p;
    vsx_module_param_int* use_thread;
    // out
    vsx_module_param_mesh* result;
    // internal
    vsx_mesh* mesh;
    vsx_mesh mesh_a;
    vsx_mesh mesh_b;
    bool first_run;
    int n_rays;
    vsx_string current_filename;
    CalCoreModel* c_model;
    CalModel* m_model;
    vsx_avector<bone_info> bones;

    // threading stuff
    pthread_t         worker_t;
    pthread_attr_t    worker_t_attr;
    int p_updates;
    bool              worker_running;
    int               thread_state;
    int               thread_exit;
    int               prev_use_thread;

  vsx_module_cal3d_loader_threaded() {
    m_model = 0;
    c_model = 0;
    mesh = &mesh_a;
    thread_state = 0;
    thread_exit = 0;
    worker_running = false;
    p_updates = -1;
    prev_use_thread = 0;
  }

  void module_info(vsx_module_info* info)
  {
    info->identifier = "mesh;importers;cal3d_importer";
    info->description = "";
    info->in_param_spec = "filename:resource,use_thread:enum?no|yes";
    info->out_param_spec = "mesh:mesh";
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
    quat_p = (vsx_module_param_quaternion*)in_parameters.create(VSX_MODULE_PARAM_ID_QUATERNION,"bone_1");
    quat_p->set(0.0f,0);
    quat_p->set(0.0f,1);
    quat_p->set(0.0f,2);
    quat_p->set(1.0f,3);
    use_thread = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_thread");
    use_thread->set(0);
    prev_use_thread = 0;
    if (bones.size()) {
      for (unsigned long i = 0; i < bones.size(); ++i) {
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
    result->set_p(mesh_a);
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
    loading_done = false;
    current_filename = "";
    redeclare_in_params(in_parameters);
    redeclare_out_params(out_parameters);
    first_run = true;
    c_model = 0;
  }

  void param_set_notify(const vsx_string& name) {
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
                TiXmlDocument doc;
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
                TiXmlDocument doc;
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
        std::vector<CalSubmesh *>mesh_list = m_model->getMesh(mesh_id)->getVectorSubmesh();
        for (std::vector<CalSubmesh *>::iterator it = mesh_list.begin(); it != mesh_list.end(); it++)
        {
          (*it)->enableTangents(0, true);
        }
        loading_done = true;
        return;
      }
    }
  }


  static void* worker(void *ptr) {
    while (1)
    {
      vsx_module_cal3d_loader_threaded* my = ((vsx_module_cal3d_loader_threaded*)ptr);
      if (1 == my->prev_use_thread)
      {
        while (my->p_updates == my->param_updates) {
          usleep(1);
        }
      }
      my->p_updates = my->param_updates;
      my->thread_state = 1;

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

            if (pCalRenderer->isTangentsEnabled(0)) {
              my->mesh->data->vertex_tangents[pCalRenderer->getVertexCount()+1].x = 0;// = vsx_vector(0,0,0);
              int num_tagentspaces = pCalRenderer->getTangentSpaces(0,&my->mesh->data->vertex_tangents[0].x);
              //printf("fetched %d tangents\n", num_tagentspaces);
            }
            //else printf("Tangents are NOT enabled\n");


            my->mesh->data->vertex_tex_coords[pCalRenderer->getVertexCount()+1].s = 0;
            pCalRenderer->getTextureCoordinates(0,&my->mesh->data->vertex_tex_coords[0].s);

            int faceCount = pCalRenderer->getFaceCount();
            (my->mesh)->data->faces.allocate(faceCount*3);
            pCalRenderer->getFaces((int*)&(my->mesh)->data->faces[0].a);
            (my->mesh)->data->faces.reset_used(faceCount);
          }
        }
      }
      // end the rendering of the model
      pCalRenderer->endRendering();
      my->thread_state = 2;

      if (0 == my->prev_use_thread) {
        return 0;
      }
      while (my->thread_state == 2)
      {
        if (my->thread_exit) {my->thread_state = 10; return 0; }
        usleep(100);
      }
      
    }
    return 0;
    //pthread_exit((void*) ptr);

  }

  void run() {
    if (!m_model)
    {
      return;
    }
    if (!bones.size()) return;

    // deal with changes in threading use
    if (prev_use_thread != use_thread->get())
    {
      if (use_thread->get()) {
        thread_state = 0;
      }
      else
      {
        // thread is running, kill it off
        thread_exit = 1;
        while (thread_state != 10)
        {
          usleep(100);
        }
        thread_state = 0;
        thread_exit = 0;
      }
      prev_use_thread = use_thread->get();
    }
    unsigned long waits=0;
    // if running, stall and wait for thread
    if (thread_state == 1)
    {
      while (thread_state == 1) { usleep(1); }
      //while (thread_state == 1 && waits < 1000000) { waits++;}
    }

    // this concept assumes that the run takes shorter than the framerate to do
    if (thread_state == 2) { // thread is done
      // no thread running
      vsx_module_cal3d_loader_threaded* my = this;
      if (!my->redeclare_out)
      {
        for (unsigned long j = 0; j < my->bones.size(); ++j)
        {
          if (my->bones[j].bone != 0)
          {
            CalVector t1 = my->bones[j].bone->getTranslationAbsolute();
            CalQuaternion q2 = my->bones[j].bone->getRotationAbsolute();
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

      CalQuaternion q2;
      CalVector t1;
      for (unsigned long j = 0; j < my->bones.size(); ++j)
      {
        t1.x = my->bones[j].translation->get(0);
        t1.y = my->bones[j].translation->get(1);
        t1.z = my->bones[j].translation->get(2);
        q2.x = my->bones[j].param->get(0);
        q2.y = my->bones[j].param->get(1);
        q2.z = my->bones[j].param->get(2);
        q2.w = my->bones[j].param->get(3);
        if (my->bones[j].bone != 0) {
          my->bones[j].bone->setRotation(q2);
          my->bones[j].bone->setTranslation(my->bones[j].o_t + t1);
        }
      }

      mesh->timestamp++;
      result->set_p(*mesh);

      // toggle to the other mesh
      if (mesh == &mesh_a) mesh = &mesh_b;
      else mesh = &mesh_a;
      // the one we point to now is the one that is going to be worked on
      thread_state = 3;
    }
    //-------------------------------------
    // no-thread execution
    if (0 == use_thread->get())
    {
      if (p_updates != param_updates)
      {
        p_updates = param_updates;
        worker((void*)this);
      }
    } else
    {
      // threaded execution
      if ( (thread_state == 0))
      {
        //printf("creating thread\n");
        pthread_attr_init(&worker_t_attr);
        //pthread_attr_setdetachstate(&worker_t_attr, PTHREAD_CREATE_JOINABLE);

        pthread_create(&worker_t, &worker_t_attr, &worker, (void*)this);
        pthread_detach(worker_t);
      }

    }
    /*if (!redeclare_out)
    {
      for (unsigned long j = 0; j < bones.size(); ++j)
      {
        if (bones[j].bone != 0)
        {
          CalVector t1 = bones[j].bone->getTranslationAbsolute();
          CalQuaternion q2 = bones[j].bone->getRotationAbsolute();
          bones[j].result_rotation   ->set( -q2.x, 0 );
          bones[j].result_rotation   ->set( -q2.y, 1 );
          bones[j].result_rotation   ->set( -q2.z, 2 );
          bones[j].result_rotation   ->set( q2.w, 3 );
  
          bones[j].result_translation->set( t1.x, 0 );
          bones[j].result_translation->set( t1.y, 1 );
          bones[j].result_translation->set( t1.z, 2 );
        }
      }
    }*/
  }

  void on_delete() {
    thread_exit = 1;
    while (thread_state != 10)
    {
      usleep(100);
    }
    if (c_model) {
      delete (CalCoreModel*)c_model;
    }
  }
};





vsx_module* create_new_module(unsigned long module) {
  switch(module) {
    case 0: return (vsx_module*)(new vsx_module_cal3d_loader_threaded);
    //case 0: return (vsx_module*)(new vsx_module_cal3d_loader);
  }
  return 0;
}

void destroy_module(vsx_module* m,unsigned long module) {
  switch(module) {
    //case 0: delete (vsx_module_cal3d_loader*)m; break;
    case 0: delete (vsx_module_cal3d_loader_threaded*)m; break;
  }
}


unsigned long get_num_modules() {
  // we have only one module. it's id is 0
  return 1;
}



