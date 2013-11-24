class module_texture_visual_fader : public vsx_module
{
  // in
	vsx_module_param_texture* texture_a_in;
	vsx_module_param_texture* texture_b_in;
	vsx_module_param_float* fade_pos_in;
	vsx_module_param_float* fade_pos_from_engine;
	// out
	vsx_module_param_texture* texture_a_out;
	vsx_module_param_texture* texture_b_out;
	vsx_module_param_float* fade_pos_out;
	// internal
	vsx_texture texture_a;
	vsx_texture texture_b;
	vsx_bitmap ab;
	vsx_bitmap bb;

public:
	void module_info(vsx_module_info* info) {
	  info->identifier = "system;visual_fader";
	  info->in_param_spec = "texture_a_in:texture,texture_b_in:texture,fade_pos_in:float";
	  info->out_param_spec = "texture_a_out:texture,texture_b_out:texture,fade_pos_out:float";
	  info->component_class = "system";
	  loading_done = true;
	}

	void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters) {
		texture_a_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_a_in");
		texture_b_in = (vsx_module_param_texture*)in_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_b_in");

		fade_pos_from_engine = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_from_engine");
		fade_pos_in = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_in");
		fade_pos_out = (vsx_module_param_float*)out_parameters.create(VSX_MODULE_PARAM_ID_FLOAT, "fade_pos_out");
		fade_pos_out->set(0.0f);

		texture_a_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_a_out");
		texture_b_out = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE, "texture_b_out");

		ab.bpp = 4;
		ab.bformat = GL_RGBA;
		ab.size_x = ab.size_y = 2;
    ab.data = new vsx_bitmap_32bt[4];
    ((vsx_bitmap_32bt*)ab.data)[0] = 0xFFFF0000;
    ((vsx_bitmap_32bt*)ab.data)[1] = 0xFF000000;
    ((vsx_bitmap_32bt*)ab.data)[2] = 0xFF000000;
    ((vsx_bitmap_32bt*)ab.data)[3] = 0xFFFF0000;
		ab.valid = true;

		bb.bpp = 4;
		bb.bformat = GL_RGBA;
		bb.size_x = bb.size_y = 2;
    bb.data = new vsx_bitmap_32bt[4];
    ((vsx_bitmap_32bt*)bb.data)[0] = 0xFF000000;
    ((vsx_bitmap_32bt*)bb.data)[1] = 0xFF0000FF;
    ((vsx_bitmap_32bt*)bb.data)[2] = 0xFF0000FF;
    ((vsx_bitmap_32bt*)bb.data)[3] = 0xFF000000;
		bb.valid = true;

    texture_a.init_opengl_texture_2d();

    texture_b.init_opengl_texture_2d();

    texture_a.upload_ram_bitmap_2d(&ab,false);
    texture_b.upload_ram_bitmap_2d(&bb,false);

		texture_a_out->set(&texture_a);
		texture_b_out->set(&texture_b);
	}

	void output(vsx_module_param_abs* param)
	{
    VSX_UNUSED(param);
		vsx_texture** t_a;
			t_a = texture_a_in->get_addr();
		vsx_texture** t_b;
			t_b = texture_b_in->get_addr();
		if (t_a && t_b) {
			((vsx_module_param_texture*)texture_a_out)->set(*t_a);
			((vsx_module_param_texture*)texture_b_out)->set(*t_b);
		}
		if (fade_pos_from_engine->get() != 0.0f)
		{
			fade_pos_out->set(fade_pos_in->get());
		} else
		{
			fade_pos_out->set((float)fmod(engine->real_vtime, 1.0f));
		}
	}
};

