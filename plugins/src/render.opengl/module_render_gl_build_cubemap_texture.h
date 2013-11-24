class module_render_gl_build_cubemap_texture : public vsx_module {
  // in
	vsx_module_param_bitmap* positive_x;
	vsx_module_param_bitmap* negative_x;
	vsx_module_param_bitmap* positive_y;
	vsx_module_param_bitmap* negative_y;
	vsx_module_param_bitmap* positive_z;
	vsx_module_param_bitmap* negative_z;
	// out
	vsx_module_param_texture* texture_out;
	// internal
	vsx_texture my_tex;
	int need_to_run;
	vsx_bitmap* p_x;
	vsx_bitmap* n_x;
	vsx_bitmap* p_y;
	vsx_bitmap* n_y;
	vsx_bitmap* p_z;
	vsx_bitmap* n_z;
public:
  void module_info(vsx_module_info* info) {
    info->identifier = "texture;opengl;6bitm2cubemap";
    info->description = "Takes 6 bitmaps and builds a cubemap\ntexture.";
    info->in_param_spec = "bitmaps:complex{positive_x:bitmap,negative_x:bitmap,positive_y:bitmap,negative_y:bitmap,positive_z:bitmap,negative_z:bitmap}";
    info->out_param_spec = "texture_out:texture";
    info->component_class = "texture";
  }

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
    p_x = n_x = p_y = n_y = p_z = n_z = 0;
    positive_x = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_x");
    negative_x = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_x");
    positive_y = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_y");
    negative_y = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_y");
    positive_z = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"positive_z");
    negative_z = (vsx_module_param_bitmap*)in_parameters.create(VSX_MODULE_PARAM_ID_BITMAP,"negative_z");
    texture_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture_out");
    //texture_out->set(&my_tex);
    need_to_run = 1;
  }

	void run() {
    if (positive_x->get_addr() != p_x) {
      if (negative_x->get_addr() != n_x) {
        if (positive_y->get_addr() != p_y) {
          if (negative_y->get_addr() != n_y) {
            if (positive_z->get_addr() != p_z) {
              if (negative_z->get_addr() != n_z) {
                p_x = positive_x->get_addr();
                n_x = negative_x->get_addr();
                p_y = positive_y->get_addr();
                n_y = negative_y->get_addr();
                p_z = positive_z->get_addr();
                n_z = negative_z->get_addr();
                glEnable(GL_TEXTURE_CUBE_MAP_EXT);
                GLuint texture;
                glGenTextures(1, &texture);
                glBindTexture(GL_TEXTURE_CUBE_MAP_EXT, texture);

                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB, GL_RGB, p_x->size_x, p_x->size_y, p_x->bformat, GL_UNSIGNED_BYTE, p_x->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB, GL_RGB, n_x->size_x, n_x->size_y, n_x->bformat, GL_UNSIGNED_BYTE, n_x->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB, GL_RGB, p_y->size_x, p_y->size_y, p_y->bformat, GL_UNSIGNED_BYTE, p_y->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB, GL_RGB, n_y->size_x, n_y->size_y, n_y->bformat, GL_UNSIGNED_BYTE, n_y->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB, GL_RGB, p_z->size_x, p_z->size_y, p_z->bformat, GL_UNSIGNED_BYTE, p_z->data );
                gluBuild2DMipmaps( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB, GL_RGB, n_z->size_x, n_z->size_y, n_z->bformat, GL_UNSIGNED_BYTE, n_z->data );

                glDisable(GL_TEXTURE_CUBE_MAP_EXT);
                my_tex.texture_info->ogl_id = texture;
                my_tex.texture_info->ogl_type = GL_TEXTURE_CUBE_MAP_EXT;
                my_tex.valid = true;
                texture_out->set(&my_tex);
                need_to_run = 0;
                loading_done = true;
                //printf("vsx_build_cubemap_texture: COMPLETE\n");
                return;
              }
            }
          }
        }
      }
    }
    if (
      (positive_x->get_addr() == 0) ||
      (negative_x->get_addr() == 0) ||
      (positive_y->get_addr() == 0) ||
      (negative_y->get_addr() == 0) ||
      (positive_z->get_addr() == 0) ||
      (negative_z->get_addr() == 0)
    ) {

      p_x = n_x = p_y = n_y = p_z = n_z = 0;
      my_tex.unload();
      texture_out->set(&my_tex);
    }
    need_to_run = 1;
  }

	void on_delete() {
  }
};

