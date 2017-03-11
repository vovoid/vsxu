/**
* Project: VSXu: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/





class module_mesh_render : public vsx_module
{
  // in
  vsx_module_param_texture* tex_a;
  vsx_module_param_mesh* mesh_in;
  vsx_module_param_mesh* particle_cloud;
  vsx_module_param_int* vertex_colors;
  vsx_module_param_int* use_vertex_colors;
  vsx_module_param_int* use_display_list;
  vsx_module_param_int* particles_size_center;
  vsx_module_param_int* particles_size_from_color;
  vsx_module_param_int* ignore_uvs_in_vbo_updates;
  vsx_module_param_particlesystem* particles_in;


  vsx_nw_vector< vsx_string<> > gl_errors;

  // out
  vsx_module_param_render* render_result;

  // internal
  vsx_mesh<>** particle_mesh;
  vsx_mesh<>** mesh;
  vsx_texture<>** ta;
  bool m_normals, m_tex_coords, m_colors;
  vsx_matrix<float> mod_mat, proj_mat;
  vsx_particlesystem<>* particles;
  vsx_matrix<float> ma;

  unsigned long prev_mesh_timestamp;

  // vbo index offsets
  #if PLATFORM_BITS == 32
    int offset_normals;
    int offset_vertices;
    int offset_texcoords;
    int offset_vertex_colors;
  #else
    int64_t offset_normals;
    int64_t offset_vertices;
    int64_t offset_texcoords;
    int64_t offset_vertex_colors;
  #endif

  // vbo handles
  GLuint vbo_id_vertex_normals_texcoords;
  GLuint vbo_id_draw_indices;


  // current - state - used to see if anything has changed
  GLuint current_vbo_draw_type;
  size_t current_num_vertices;
  size_t current_num_faces;



  ///////////////////////////////////////////////////////////////////////////////
  // generate vertex buffer object and bind it with its data
  // You must give 2 hints about data usage; target and mode, so that OpenGL can
  // decide which data should be stored and its location.
  // VBO works with 2 different targets; GL_ARRAY_BUFFER_ARB for vertex arrays
  // and GL_ELEMENT_ARRAY_BUFFER_ARB for index array in glDrawElements().
  // The default target is GL_ARRAY_BUFFER_ARB.
  // By default, usage mode is set as GL_STATIC_DRAW_ARB.
  // Other usages are GL_STREAM_DRAW_ARB, GL_STREAM_READ_ARB, GL_STREAM_COPY_ARB,
  // GL_STATIC_DRAW_ARB, GL_STATIC_READ_ARB, GL_STATIC_COPY_ARB,
  // GL_DYNAMIC_DRAW_ARB, GL_DYNAMIC_READ_ARB, GL_DYNAMIC_COPY_ARB.
  void create_vbo(GLuint &id, const void* data, int dataSize, GLenum target, GLenum usage)
  {
    if (id == 0)
    {
      glGenBuffersARB(1, &id);                        // create a vbo
    }
    glBindBufferARB(target, id);                    // activate vbo id to use
    glBufferDataARB(target, dataSize, data, usage); // upload data to video card

    // check data size in VBO is same as input array, if not return 0 and delete VBO
    int bufferSize = 0;
    glGetBufferParameterivARB(target, GL_BUFFER_SIZE_ARB, &bufferSize);
    if(dataSize != bufferSize)
    {
      //printf("datasize not equal to buffersize\n");
      glDeleteBuffersARB(1, &id);
      id = 0;
    }
  }

  bool init_vbo(GLuint draw_type = GL_DYNAMIC_DRAW_ARB)
  {
    if (vbo_id_vertex_normals_texcoords) {
      return true;
    }
    current_vbo_draw_type = draw_type;
    offset_normals = 0;
    offset_vertices = 0;
    offset_texcoords = 0;
    offset_vertex_colors = 0;
    GLintptr offset = 0;

    //-----------------------------------------------------------------------
    // generate the buffers
    if (vbo_id_vertex_normals_texcoords == 0)
    {
      glGenBuffersARB
      (
        1,
        &vbo_id_vertex_normals_texcoords
      );
    }

    // bind the vertex, normals buffer for use
    glBindBufferARB
    (
      GL_ARRAY_BUFFER_ARB,
      vbo_id_vertex_normals_texcoords
    );
    if (!glIsBufferARB(vbo_id_vertex_normals_texcoords))
    {
      return false;
    }
    //-----------------------------------------------------------------------
    // allocate the buffer
    glBufferDataARB(
      GL_ARRAY_BUFFER_ARB,
      (*mesh)->data->vertex_normals.get_sizeof()
      +
      (*mesh)->data->vertices.get_sizeof()
      +
      (*mesh)->data->vertex_tex_coords.get_sizeof()
      +
      (*mesh)->data->vertex_colors.get_sizeof()+10
      ,
      0,
      draw_type//GL_STATIC_DRAW_ARB // only static draw
    );

    //-----------------------------------------------------------------------
    // inject the different arrays
    // 1: vertex normals ----------------------------------------------------
    if ( (*mesh)->data->vertex_normals.size() )
    {
      offset_normals = offset;
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset,
        (*mesh)->data->vertex_normals.get_sizeof(),
        (*mesh)->data->vertex_normals.get_pointer()
      );
      offset += (*mesh)->data->vertex_normals.get_sizeof();
      //printf("offset after vertex normals: %d\n", offset);
    }

    // 2: texture coordinates -----------------------------------------------
    if ( (*mesh)->data->vertex_tex_coords.size() )
    {
      offset_texcoords = offset;
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset,
        (*mesh)->data->vertex_tex_coords.get_sizeof(),
        (*mesh)->data->vertex_tex_coords.get_pointer()
      );
      offset += (*mesh)->data->vertex_tex_coords.get_sizeof();
      //printf("offset after texcoords: %d\n", offset);
    }

    // 3: optional: vertex color coordinates -----------------------------------------------
    if ( (*mesh)->data->vertex_colors.size() )
    {
      offset_vertex_colors = offset;
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset,
        (*mesh)->data->vertex_colors.get_sizeof(),
        (*mesh)->data->vertex_colors.get_pointer()
      );
      offset += (*mesh)->data->vertex_colors.get_sizeof();
      //printf("offset after vertex colors: %d\n", offset);
    }

    // 4: vertices ----------------------------------------------------------
    offset_vertices = offset;
    glBufferSubDataARB
    (
      GL_ARRAY_BUFFER_ARB,
      offset,
      (*mesh)->data->vertices.get_sizeof(),
      (*mesh)->data->vertices.get_pointer()
    );
    offset += (*mesh)->data->vertices.get_sizeof();
    current_num_vertices = (*mesh)->data->vertices.size();
    //printf("offset after vertices: %d\n", offset);

    //-----------------------------------------------------------------------

    int bufferSize;
    glGetBufferParameterivARB(GL_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
    //printf("vertex and normal array in vbo: %d bytes\n", bufferSize);

    // unbind the array buffer
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    // create VBO for index array
    // Target of this VBO is GL_ELEMENT_ARRAY_BUFFER_ARB and usage is GL_STATIC_DRAW_ARB
    /*for (size_t fi = 0; fi < (*mesh)->data->faces.size(); fi++ )
    {
      (*mesh)->data->faces[fi].a = 0;
      (*mesh)->data->faces[fi].b = 1;
      (*mesh)->data->faces[fi].c = 2;
    }*/
    create_vbo
    (
      vbo_id_draw_indices,
      (*mesh)->data->faces.get_pointer(),
      (*mesh)->data->faces.get_sizeof(),
      GL_ELEMENT_ARRAY_BUFFER_ARB,
      GL_STATIC_DRAW_ARB
    );

    glGetBufferParameterivARB(GL_ELEMENT_ARRAY_BUFFER_ARB, GL_BUFFER_SIZE_ARB, &bufferSize);
    //printf("index array in vbo: %d bytes\n", bufferSize);
    //used_memory += bufferSize;
    //printf("total VBO memory used: %d bytes\n", used_memory);
    current_num_faces = (*mesh)->data->faces.size();
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);


    return true;
  }

  void destroy_vbo()
  {
    if (!vbo_id_vertex_normals_texcoords)
      return;

    glDeleteBuffersARB(1, &vbo_id_draw_indices);
    glDeleteBuffersARB(1, &vbo_id_vertex_normals_texcoords);


    vbo_id_vertex_normals_texcoords = 0;
    vbo_id_draw_indices = 0;
  }


  bool check_if_need_to_reinit_vbo(GLuint draw_type)
  {
    if (!vbo_id_vertex_normals_texcoords)
      return true;

    if (current_num_vertices != (*mesh)->data->vertices.size() ) return true;
    if (current_num_faces != (*mesh)->data->faces.size() ) return true;
    if (current_vbo_draw_type != draw_type) return true;
    return false;
  }

  bool maintain_vbo_type(GLuint draw_type = GL_DYNAMIC_DRAW_ARB)
  {
    if (!check_if_need_to_reinit_vbo(draw_type)) return true;
    //printf("re-initializing the VBO!\n");
    destroy_vbo();
    return init_vbo(draw_type);
  }


public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "renderers;mesh;mesh_basic_render";

    info->in_param_spec =
      "tex_a:texture,mesh_in:mesh,"
      "particles:particlesystem,"
      "particle_cloud:mesh,"
      "options:complex"
      "{"
        "vertex_colors:enum?no|yes,"
        "use_display_list:enum?no|yes,"
        "use_vertex_colors:enum?no|yes,"
        "particles_size_center:enum?no|yes,"
        "particles_size_from_color:enum?no|yes,"
        "ignore_uvs_in_vbo_updates:enum?no|yes"
      "}"
    ;

    info->out_param_spec = "render_out:render";

    info->component_class = "render";
  }

  int num_uploads;
  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    tex_a = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"tex_a");
    mesh_in = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"mesh_in");
    particle_cloud = (vsx_module_param_mesh*)in_parameters.create(VSX_MODULE_PARAM_ID_MESH,"particle_cloud");

    vertex_colors = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"vertex_colors");
    vertex_colors->set(0);

    use_vertex_colors = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_vertex_colors");
    use_vertex_colors->set(1);

    use_display_list = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"use_display_list");
    use_display_list->set(0);    


    particles_size_center = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"particles_size_center");
    particles_size_center->set(0);

    particles_size_from_color = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"particles_size_from_color");
    particles_size_from_color->set(0);

    ignore_uvs_in_vbo_updates = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"ignore_uvs_in_vbo_updates");
    ignore_uvs_in_vbo_updates->set(0);

    particles_in = (vsx_module_param_particlesystem*)in_parameters.create(VSX_MODULE_PARAM_ID_PARTICLESYSTEM,"particles");

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);

    particle_mesh = 0x0;
    mesh = 0x0;
    ta = 0x0;

    m_normals = false;
    m_tex_coords = false;
    m_colors = false;

    particles = 0x0;

    prev_mesh_timestamp = 0xFFFFFF;

    // init variables
    // vbo index offsets

    offset_normals = 0;
    offset_vertices = 0;
    offset_texcoords = 0;
    offset_vertex_colors = 0;
    // vbo handles
    vbo_id_vertex_normals_texcoords = 0;
    vbo_id_draw_indices = 0;
    // current - state - used to see if anything has changed
    current_vbo_draw_type = 0;
    current_num_vertices = 0;
    current_num_faces = 0;

    current_vbo_draw_type = 0;

    current_num_vertices = 0;
    current_num_faces = 0;

    num_uploads = 0;
  }

  void  enable_texture()
  {
    ta = tex_a->get_addr();
    if (ta)
    {
      glMatrixMode(GL_TEXTURE);
      glPushMatrix();

      if ((*ta)->get_transform())
        (*ta)->get_transform()->transform();

      (*ta)->bind();
    }
  }

  void  disable_texture()
  {
    if (ta)
    {
      (*ta)->_bind();
      glMatrixMode(GL_TEXTURE);
      glPopMatrix();
    }
  }

  void  enable_client_arrays_no_vbo()
  {
    // reset presence values
    m_colors = false;
    m_normals = false;
    m_tex_coords = false;

    // enable vertex colors
    if (use_vertex_colors->get())
    {
      if ((*mesh)->data->vertex_colors.get_used()) {
        glColorPointer(4,GL_FLOAT,0,(*mesh)->data->vertex_colors.get_pointer());
        m_colors = true;
      }
    }
    // enable vertex normals
    if ((*mesh)->data->vertex_normals.get_used()) {
      glNormalPointer(GL_FLOAT,0,(*mesh)->data->vertex_normals.get_pointer());
      m_normals = true;
    }
    // enable tex coords
    if ((*mesh)->data->vertex_tex_coords.get_used()) {
      if ( (*mesh)->data->vertex_tex_coords.size() == (*mesh)->data->vertices.size() * 2 )
      {

        glTexCoordPointer(4,GL_FLOAT,0,(*mesh)->data->vertex_tex_coords.get_pointer());
      }
      else
        glTexCoordPointer(2,GL_FLOAT,0,(*mesh)->data->vertex_tex_coords.get_pointer());
      m_tex_coords = true;
    }
    // enable vertices
    glVertexPointer(3,GL_FLOAT,0,(*mesh)->data->vertices.get_pointer());

    glEnableClientState(GL_VERTEX_ARRAY);
    if (m_colors) glEnableClientState(GL_COLOR_ARRAY);
    if (m_normals) glEnableClientState(GL_NORMAL_ARRAY);
    if (m_tex_coords) glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }

  void disable_client_arrays_no_vbo()
  {
    glDisableClientState(GL_VERTEX_ARRAY);

    if (m_normals) {
      glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (m_tex_coords) {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (m_colors) {
      glDisableClientState(GL_COLOR_ARRAY);
    }
  }

  bool enable_client_arrays_vbo()
  {
    // reset presence values
    m_colors = false;
    m_normals = false;
    m_tex_coords = false;


    // bind the vertex, normals buffer for use
    glBindBufferARB
    (
      GL_ARRAY_BUFFER_ARB,
      vbo_id_vertex_normals_texcoords
    );

    // enable vertex colors
    if (use_vertex_colors->get())
    {
      if ((*mesh)->data->vertex_colors.get_used())
      {
        glColorPointer(4,GL_FLOAT,0,(GLvoid*)offset_vertex_colors);
        m_colors = true;
      }
    }
    // enable vertex normals
    if (
      //0 &&
      (*mesh)->data->vertex_normals.get_used()
    )
    {
      glNormalPointer(GL_FLOAT,0,(GLvoid*)offset_normals);
      m_normals = true;
    }

    // enable tex coords
    if (
      //0 &&
      (*mesh)->data->vertex_tex_coords.get_used()
    )
    {
      if ( (*mesh)->data->vertex_tex_coords.size() == (*mesh)->data->vertices.size() * 2 )
      {
        glTexCoordPointer(4,GL_FLOAT,0,(GLvoid*)offset_texcoords);
      }
      else
        glTexCoordPointer(2,GL_FLOAT,0,(GLvoid*)offset_texcoords);

      m_tex_coords = true;
    }

    // enable vertices
    glVertexPointer(3,GL_FLOAT,0,(GLvoid*)offset_vertices);

    // bind the index array buffer buffer for use
    glBindBufferARB
    (
      GL_ELEMENT_ARRAY_BUFFER_ARB,
      vbo_id_draw_indices
    );

    glEnableClientState(GL_VERTEX_ARRAY);

    if (m_colors)
      glEnableClientState(GL_COLOR_ARRAY);

    if (m_normals)
      glEnableClientState(GL_NORMAL_ARRAY);

    if (m_tex_coords)
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // all went well
    return true;
  }


  void disable_client_arrays_vbo()
  {
    glDisableClientState(GL_VERTEX_ARRAY);

    if (m_normals) {
      glDisableClientState(GL_NORMAL_ARRAY);
    }

    if (m_tex_coords) {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (m_colors)
    {
      glDisableClientState(GL_COLOR_ARRAY);
    }

    // unbind the VBO buffers
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
    glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB, 0);
  }

  void perform_draw()
  {
    glDrawElements(GL_TRIANGLES,(*mesh)->data->faces.get_used()*3,GL_UNSIGNED_INT,0);
  }

  void cleanup_successful_rendering()
  {
    disable_client_arrays_vbo();
    disable_texture();
    if (vertex_colors->get()) {
      glDisable(GL_COLOR_MATERIAL);
    }
    render_result->set(1);
  }

  void run()
  {
    mesh = mesh_in->get_addr();
    // sanity checks
    if (!mesh)
    {
      user_message="module||Can not render: mesh is not set"; render_result->set(0);
      return;
    }
    if (!(*mesh)->data)
    {
      user_message="module||Can not render: Mesh data is not set";
      render_result->set(0);
      return;
    }
    if (!(*mesh)->data->faces.get_used())
    {
      user_message="module||Can not render: Mesh has no faces";
      render_result->set(0);
      return;
    }
    user_message="module||ok";

    // don't upload unless changed
    req(prev_mesh_timestamp != (*mesh)->timestamp);
    req(!check_if_need_to_reinit_vbo(current_vbo_draw_type));


    // bind the vertex, normals buffer for use
    glBindBufferARB
    (
      GL_ARRAY_BUFFER_ARB,
      vbo_id_vertex_normals_texcoords
    );

    // if buffer type is "DYNAMIC_DRAW", upload new data
    if (current_vbo_draw_type == GL_DYNAMIC_DRAW_ARB)
    {
      //vsx_printf(L"uploading %d vertices to VBO\n", (*mesh)->data->vertices.size());
      //printf("vertices ofset: %d\n", offset_vertices);
      if ((*mesh)->data->vertex_normals.get_used())
      {
        glBufferSubDataARB
        (
          GL_ARRAY_BUFFER_ARB,
          0,
          (*mesh)->data->vertex_normals.get_sizeof(),
          (*mesh)->data->vertex_normals.get_pointer()
        );
      }

      if ((*mesh)->data->vertex_tex_coords.get_used())
      {
        // optimize away the UV uploads
        if
        (
          false == (ignore_uvs_in_vbo_updates->get() == 1 && num_uploads > 100)
        )
        {
          glBufferSubDataARB
          (
            GL_ARRAY_BUFFER_ARB,
            offset_texcoords,
            (*mesh)->data->vertex_tex_coords.get_sizeof(),
            (*mesh)->data->vertex_tex_coords.get_pointer()
          );
        }
      }

      if (use_vertex_colors->get())
      {
        if ((*mesh)->data->vertex_colors.get_used())
        {
          glBufferSubDataARB
          (
            GL_ARRAY_BUFFER_ARB,
            offset_vertex_colors,
            (*mesh)->data->vertex_colors.get_sizeof(),
            (*mesh)->data->vertex_colors.get_pointer()
          );
        }
      }
      glBufferSubDataARB
      (
        GL_ARRAY_BUFFER_ARB,
        offset_vertices,
        (*mesh)->data->vertices.get_sizeof(),
        (*mesh)->data->vertices.get_pointer()
      );
      num_uploads++;

    }
    // unbind the VBO buffers
    glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);

    prev_mesh_timestamp = (*mesh)->timestamp;
  }

  // if no display list: re-init vbo with stream rendering
  // if display list: re-init vbo with static rendering
  // display_list being set overrides new functionality
  // since indices will be static always, need to check for changes in number of vertices; rebuild if changed.

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);
    mesh = mesh_in->get_addr();
    // sanity checks
    if (!mesh)
    {
      user_message="module||Can not render: mesh is not set";
      render_result->set(0);
      return;
    }
    if (!(*mesh)->data) { user_message="module||Can not render: Mesh data is not set"; render_result->set(0); return; }
    if (!(*mesh)->data->faces.get_used()) { user_message="module||Can not render: Mesh has no faces"; render_result->set(0); return; }
    user_message="module||ok";
    if (use_display_list->get())
    {
      // make sure vbo is set to static draw
      if (!maintain_vbo_type(GL_STATIC_DRAW_ARB)) return;
    } else
    {
      // make sure vbo is set to stream draw
      if (!maintain_vbo_type(GL_DYNAMIC_DRAW_ARB)) return;
    }
    if (gl_errors.size()) {
      return;
    }

    enable_texture();

    if (vertex_colors->get()) glEnable(GL_COLOR_MATERIAL);


    particle_mesh = particle_cloud->get_addr();
    if (particle_mesh)
    {
      // make sure vbo is set to static draw
      //maintain_vbo_type(GL_STATIC_DRAW_ARB);
      enable_client_arrays_vbo();

      glMatrixMode(GL_MODELVIEW);
      for (unsigned long i = 0; i < (*particle_mesh)->data->vertices.size(); ++i) {
        glPushMatrix();

        glTranslatef(
          (*particle_mesh)->data->vertices[i].x,
          (*particle_mesh)->data->vertices[i].y,
          (*particle_mesh)->data->vertices[i].z
        );
        if (particles_size_from_color->get() && (*particle_mesh)->data->vertex_colors.size())
        {
          glScalef(
            (*particle_mesh)->data->vertex_colors[i].r,
            (*particle_mesh)->data->vertex_colors[i].g,
            (*particle_mesh)->data->vertex_colors[i].b
          );
        }
        perform_draw();
        glPopMatrix();
      }
      cleanup_successful_rendering();
      return;
    }


    particles = particles_in->get_addr();
    if (particles)
    {
      // sanity checks
      if (!particles->particles) { render_result->set(0); return; }

      // make sure vbo is set to static draw
      //maintain_vbo_type(GL_STATIC_DRAW_ARB);
      enable_client_arrays_vbo();
      float ss;
      glMatrixMode(GL_MODELVIEW);

      for (unsigned long i = 0; i < particles->particles->size(); ++i)
      {
        (*particles->particles)[i].color.a = (1-((*particles->particles)[i].time/(*particles->particles)[i].lifetime));

        glColor4f(
          (*particles->particles)[i].color.r,
          (*particles->particles)[i].color.g,
          (*particles->particles)[i].color.b,
          (*particles->particles)[i].color.a
        );

        glPushMatrix();

        ma = (*particles->particles)[i].rotation.matrix();
        if (particles_size_center->get())
        {
          glTranslatef(
            (*particles->particles)[i].creation_pos.x,
            (*particles->particles)[i].creation_pos.y,
            (*particles->particles)[i].creation_pos.z
          );
          glMultMatrixf(ma.m);
          vsx_vector3<> ipos = (*particles->particles)[i].pos;
          ipos.x -= (*particles->particles)[i].creation_pos.x;
          ipos.y -= (*particles->particles)[i].creation_pos.y;
          ipos.z -= (*particles->particles)[i].creation_pos.z;
          ss = ipos.norm();
          glScalef(
            ss,
            ss,
            ss
          );
        }
        else
        {
          glTranslatef(
            (*particles->particles)[i].pos.x,
            (*particles->particles)[i].pos.y,
            (*particles->particles)[i].pos.z
          );
          ss = (*particles->particles)[i].size*(1-((*particles->particles)[i].time/(*particles->particles)[i].lifetime));
          glScalef(
            ss,
            ss,
            ss
          );
          glMultMatrixf(ma.m);
        }

        perform_draw();

        glPopMatrix();
      }

      cleanup_successful_rendering();
      return;
    }

    if (!enable_client_arrays_vbo())
      return;

    if (gl_errors.size() > 0)
      return;

    perform_draw();

    cleanup_successful_rendering();
  }

  void stop()
  {
    destroy_vbo();
  }
};
