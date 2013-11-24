class module_render_basic_textured_rectangle : public vsx_module
{
  // in
  vsx_module_param_float3* position;
  vsx_module_param_float* opacity;
  vsx_module_param_float* size;
  vsx_module_param_float* x_aspect;
  vsx_module_param_texture* tex_inf;
  vsx_module_param_float* angle;
  vsx_module_param_float4* color_multiplier;
  vsx_module_param_float4* color_center;
  vsx_module_param_float4* color_a;
  vsx_module_param_float4* color_b;
  vsx_module_param_float4* color_c;
  vsx_module_param_float4* color_d;
  vsx_module_param_float3* tex_coord_a;
  vsx_module_param_float3* tex_coord_b;
  vsx_module_param_int* facing_camera;

  // out
  vsx_module_param_render* render_result;

  // internal
  float tax, tay, tbx, tby;
  vsx_color cm;
  GLfloat blobMat[16];
  GLfloat blobVec0[4];
  GLfloat blobVec1[4];

  vsx_gl_state* gl_state;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "renderers;basic;textured_rectangle"
      "||"
      "renderers;examples;simple_with_texture"
    ;

    info->in_param_spec =
      "spatial:complex{"
        "position:float3,"
        "size:float,"
        "angle:float,"
        "x_aspect_ratio:float,"
        "tex_coord_a:float3,"
        "tex_coord_b:float3,"
        "facing_camera:enum?no|yes&nc=1" // don't allow connections to this, pure config only
      "},"
      "color:complex{"
        "global_alpha:float,"
        "color_multiplier:float4?default_controller=controller_col,"
        "color_center:float4?default_controller=controller_col,"
        "color_a:float4?default_controller=controller_col,"
        "color_b:float4?default_controller=controller_col,"
        "color_c:float4?default_controller=controller_col,"
        "color_d:float4?default_controller=controller_col"
      "},"
      "texture_in:texture"
    ;

    info->description =
      "Renders a textured rectangle.\n"
      "It has the following featurs:\n"
      "- facing camera (billboard)\n"
      "- variable texture aspect ratio\n"
      "- colors in center and 4 corners"
    ;

    info->out_param_spec = "render_out:render";
    info->component_class = "render";
  }



  void beginBlobs(vsx_gl_state* gl_state)
  {
    GLfloat tmpMat[16];
    gl_state->matrix_get_v( VSX_GL_MODELVIEW_MATRIX, blobMat );
    gl_state->matrix_mode( VSX_GL_PROJECTION_MATRIX );
    gl_state->matrix_push();
    gl_state->matrix_get_v( VSX_GL_PROJECTION_MATRIX, tmpMat );

    tmpMat[3] = 0;
    tmpMat[7] = 0;
    tmpMat[11] = 0;
    tmpMat[12] = 0;
    tmpMat[13] = 0;
    tmpMat[14] = 0;
    tmpMat[15] = 1;

    v_norm(tmpMat);
    v_norm(tmpMat + 4);
    v_norm(tmpMat + 8);

    gl_state->matrix_load_identity();
    gl_state->matrix_mult_f(tmpMat);

    blobMat[3] = 0;
    blobMat[7] = 0;
    blobMat[11] = 0;
    blobMat[12] = 0;
    blobMat[13] = 0;
    blobMat[14] = 0;
    blobMat[15] = 1;

    v_norm(blobMat);
    v_norm(blobMat + 4);
    v_norm(blobMat + 8);

    gl_state->matrix_mult_f(blobMat);
    gl_state->matrix_get_v( VSX_GL_PROJECTION_MATRIX, blobMat );
    gl_state->matrix_pop();
    gl_state->matrix_mode (VSX_GL_MODELVIEW_MATRIX );

    GLfloat upLeft[] = {-0.5f, 0.5f, 0.0f, 1.0f};
    GLfloat upRight[] = {0.5f, 0.5f, 0.0f, 1.0f};

    mat_vec_mult(blobMat, upLeft, blobVec0);
    mat_vec_mult(blobMat, upRight, blobVec1);
  }


  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;
    opacity = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "global_alpha");
    opacity->set(1.0f);

    facing_camera = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT, "facing_camera");

    x_aspect = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "x_aspect_ratio");
    x_aspect->set(1.0f);

    size = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "size");
    size->set(1.0f);
    // critical parameter, without this it won't run
    tex_inf = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_in",true,true);

    angle = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "angle");

    color_multiplier = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_multiplier");
    color_center = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_center");
    color_a = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_a");
    color_b = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_b");
    color_c = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_c");
    color_d = (vsx_module_param_float4*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT4,"color_d");
    color_multiplier->set(1,0);
    color_multiplier->set(1,1);
    color_multiplier->set(1,2);
    color_multiplier->set(1,3);

    color_center->set(1,0);
    color_center->set(1,1);
    color_center->set(1,2);
    color_center->set(1,3);

    color_a->set(1,0);
    color_a->set(1,1);
    color_a->set(1,2);
    color_a->set(1,3);

    color_b->set(1,0);
    color_b->set(1,1);
    color_b->set(1,2);
    color_b->set(1,3);

    color_c->set(1,0);
    color_c->set(1,1);
    color_c->set(1,2);
    color_c->set(1,3);

    color_d->set(1,0);
    color_d->set(1,1);
    color_d->set(1,2);
    color_d->set(1,3);

    tex_coord_a = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"tex_coord_a");
    tex_coord_b = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"tex_coord_b");
    tex_coord_a->set(0,0);
    tex_coord_a->set(0,1);
    tex_coord_a->set(0,2);
    tex_coord_b->set(1,0);
    tex_coord_b->set(1,1);
    tex_coord_b->set(0,2);

    position = (vsx_module_param_float3*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT3,"position");

    render_result = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");
    render_result->set(0);

    gl_state = vsx_gl_state::get_instance();
  }

  void output(vsx_module_param_abs* param)
  {
    VSX_UNUSED(param);

    vsx_texture** t_inf;
    t_inf = tex_inf->get_addr();

    if (!t_inf)
    {
      render_result->set(0);
      return;
    }

    if (!((*t_inf)->valid))
    {
      render_result->set(0);
      return;
    }

    vsx_transform_obj& texture_transform = *(*t_inf)->get_transform();

    float obj_size = size->get();


    glMatrixMode(GL_TEXTURE);
    glPushMatrix();

    if ((*t_inf)->get_transform()) texture_transform();

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(position->get(0),position->get(1),position->get(2));

    glRotatef((float)angle->get()*360, 0, 0, 1);

    glScalef(obj_size*x_aspect->get(), obj_size, obj_size);

    (*t_inf)->bind();
    float alpha = opacity->get();
    if (alpha < 0) alpha = 0;


    cm.r = color_multiplier->get(0);
    cm.g = color_multiplier->get(1);
    cm.b = color_multiplier->get(2);
    cm.a = color_multiplier->get(3)*opacity->get();
    tax = tex_coord_a->get(0);
    tay = tex_coord_a->get(1);
    tbx = tex_coord_b->get(0);
    tby = tex_coord_b->get(1);
    if (facing_camera->get()) {
      beginBlobs(gl_state);

      GLfloat tmpVec0[] = {blobVec0[0]*2, blobVec0[1]*2, blobVec0[2]*2};
      GLfloat tmpVec1[] = {blobVec1[0]*2, blobVec1[1]*2, blobVec1[2]*2};
      #ifdef VSXU_OPENGL_ES
        GLfloat fan_vertices[] = {
          0,0,0,
          -tmpVec0[0], -tmpVec0[1], -tmpVec0[2],
          tmpVec1[0],  tmpVec1[1], tmpVec1[2],
          tmpVec0[0],  tmpVec0[1], tmpVec0[2],
          -tmpVec1[0], -tmpVec1[1], -tmpVec1[2],
          -tmpVec0[0], -tmpVec0[1], -tmpVec0[2],
        };

        #ifdef VSXU_OPENGL_ES_1_0
          GLfloat fan_colors[] = {
            cm.r*color_center->get(0), cm.g*color_center->get(1) , cm.b*color_center->get(2),cm.a*color_center->get(3),
            cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3),
            cm.r*color_b->get(0), cm.g*color_b->get(1)   , cm.b*color_b->get(2),cm.a*color_b->get(3),
            cm.r*color_c->get(0), cm.g*color_c->get(1)   , cm.b*color_c->get(2),cm.a*color_c->get(3),
            cm.r*color_d->get(0), cm.g*color_d->get(1)   , cm.b*color_d->get(2),cm.a*color_d->get(3),
            cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3),
          };
          GLfloat fan_texcoords[] = {
            (tbx-tax)*0.5+tax,(tby-tay)*0.5+tay,
            tax, tby,
            tax, tay,
            tbx, tay,
            tbx, tby,
            tax, tby,
          };
          glEnableClientState(GL_VERTEX_ARRAY);
          glEnableClientState(GL_COLOR_ARRAY);
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          glVertexPointer(3, GL_FLOAT, 0, fan_vertices);
          glColorPointer(4, GL_FLOAT, 0, fan_colors);
          glTexCoordPointer(2, GL_FLOAT, 0, fan_texcoords);
        #endif

        #ifdef VSXU_OPENGL_ES_2_0
          glEnableVertexAttribArray(0);
          glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, fan_vertices);
        #endif

        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
      #endif
      #ifndef VSXU_OPENGL_ES
        glBegin(GL_TRIANGLE_FAN);
          glColor4f(cm.r*color_center->get(0), cm.g*color_center->get(1) , cm.b*color_center->get(2),cm.a*color_center->get(3));  (*t_inf)->texcoord2f((tbx-tax)*0.5f+tax,(tby-tay)*0.5f+tay);
          glVertex3f(0,0,0);

          glColor4f(cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3));	(*t_inf)->texcoord2f(tax, tby);
          glVertex3f(-tmpVec0[0], -tmpVec0[1], -tmpVec0[2]);

          glColor4f(cm.r*color_b->get(0), cm.g*color_b->get(1)   , cm.b*color_b->get(2),cm.a*color_b->get(3));	(*t_inf)->texcoord2f(tax, tay);
          glVertex3f(tmpVec1[0],  tmpVec1[1], tmpVec1[2]);

          glColor4f(cm.r*color_c->get(0), cm.g*color_c->get(1)   , cm.b*color_c->get(2),cm.a*color_c->get(3));	(*t_inf)->texcoord2f(tbx, tay);
          glVertex3f( tmpVec0[0],  tmpVec0[1], tmpVec0[2]);

          glColor4f(cm.r*color_d->get(0), cm.g*color_d->get(1)   , cm.b*color_d->get(2),cm.a*color_d->get(3));	(*t_inf)->texcoord2f(tbx, tby);
          glVertex3f(-tmpVec1[0], -tmpVec1[1], -tmpVec1[2]);

          glColor4f(cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3));	(*t_inf)->texcoord2f(tax, tby);
          glVertex3f(-tmpVec0[0], -tmpVec0[1], -tmpVec0[2]);
        glEnd();
      #endif
    }
    else
    {
      #ifdef VSXU_OPENGL_ES
        GLfloat fan_vertices[] = {
          0,0,
          -1,-1,
          -1,1,
          1,1,
          1,-1,
          -1,-1,
        };

        #ifdef VSXU_OPENGL_ES_1_0
          GLfloat fan_colors[] = {
            cm.r*color_center->get(0), cm.g*color_center->get(1) , cm.b*color_center->get(2),cm.a*color_center->get(3),
            cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3),
            cm.r*color_b->get(0), cm.g*color_b->get(1)   , cm.b*color_b->get(2),cm.a*color_b->get(3),
            cm.r*color_c->get(0), cm.g*color_c->get(1)   , cm.b*color_c->get(2),cm.a*color_c->get(3),
            cm.r*color_d->get(0), cm.g*color_d->get(1)   , cm.b*color_d->get(2),cm.a*color_d->get(3),
            cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3),
          };
          GLfloat fan_texcoords[] = {
            (tbx-tax)*0.5+tax,(tby-tay)*0.5+tay,
            tax, tay,
            tax, tby,
            tbx, tby,
            tbx, tay,
            tax, tay,
          };
          glEnableClientState(GL_VERTEX_ARRAY);
          glEnableClientState(GL_COLOR_ARRAY);
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          glVertexPointer(3, GL_FLOAT, 0, fan_vertices);
          glColorPointer(4, GL_FLOAT, 0, fan_colors);
          glTexCoordPointer(2, GL_FLOAT, 0, fan_texcoords);
        #endif

        #ifdef VSXU_OPENGL_ES_2_0
          glEnableVertexAttribArray(0);
          glVertexAttribPointer(0,3,GL_FLOAT, GL_FALSE, 0, fan_vertices);
        #endif

        glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
      #endif
      #ifndef VSXU_OPENGL_ES
        glBegin(GL_TRIANGLE_FAN);
          glColor4f(cm.r*color_center->get(0), cm.g*color_center->get(1) , cm.b*color_center->get(2),cm.a*color_center->get(3));  (*t_inf)->texcoord2f((tbx-tax)*0.5f+tax,(tby-tay)*0.5f+tay);
          glVertex3i(0,0,0);
          glColor4f(cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3));	(*t_inf)->texcoord2f(tax, tay);	glVertex3f(-1, -1, 0);
          glColor4f(cm.r*color_b->get(0), cm.g*color_b->get(1)   , cm.b*color_b->get(2),cm.a*color_b->get(3));	(*t_inf)->texcoord2f(tax, tby);	glVertex3f(-1,  1, 0);
          glColor4f(cm.r*color_c->get(0), cm.g*color_c->get(1)   , cm.b*color_c->get(2),cm.a*color_c->get(3));	(*t_inf)->texcoord2f(tbx, tby);	glVertex3f( 1,  1, 0);
          glColor4f(cm.r*color_d->get(0), cm.g*color_d->get(1)   , cm.b*color_d->get(2),cm.a*color_d->get(3));	(*t_inf)->texcoord2f(tbx, tay);	glVertex3f( 1, -1, 0);
          glColor4f(cm.r*color_a->get(0), cm.g*color_a->get(1)   , cm.b*color_a->get(2),cm.a*color_a->get(3));	(*t_inf)->texcoord2f(tax, tay);	glVertex3f(-1, -1, 0);
        glEnd();
      #endif
    }

    (*t_inf)->_bind();

    glPopMatrix();
    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    render_result->set(1);
  }

};
