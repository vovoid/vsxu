#include <time.h>
#include <vsx_data_path.h>
#include <texture/buffer/vsx_texture_buffer_color_depth.h>

class module_texture_screenshot_hires : public vsx_module
{
  // in
  vsx_module_param_render* render_in;

  vsx_module_param_float* fov;

  vsx_module_param_float* image_width;
  vsx_module_param_float* image_height;

  vsx_module_param_float* image_sections_x;
  vsx_module_param_float* image_sections_y;


  vsx_module_param_int* start_capture;

  // out
  vsx_module_param_render* render_out;

  // internal
  vsx_texture<>* texture = 0x0;
  vsx_texture_buffer_color_depth buffer;

  vsx_gl_state* gl_state;
  GLfloat tmpMat[16];

  size_t tex_width;
  size_t tex_height;
  double frustum_segment_x;
  double frustum_segment_y;
  size_t n;
  size_t m;
  size_t N;
  size_t M;
  size_t capture_in_progress;

  char* pixeldata;
  char* pixeldata_target;

public:

  void module_info(vsx_module_specification* info)
  {
    info->identifier =
      "texture;capture;screenshot_hires";

    info->in_param_spec =
      "render_in:render,"
      "start_capture:enum?NO|YES,"
      "parameters:complex"
      "{"
        "image_width:float?nc=1,"
        "image_height:float?nc=1"
      "},"
      "fov:float"
    ;

    info->out_param_spec =
      "render_out:render";

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {
    loading_done = true;

    start_capture = (vsx_module_param_int*)in_parameters.create(VSX_MODULE_PARAM_ID_INT,"start_capture");
    start_capture->set(0);

    fov = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"fov");
    fov->set(90.0);

    image_width = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"image_width");
    image_width->set(512.0);

    image_height = (vsx_module_param_float*)in_parameters.create(VSX_MODULE_PARAM_ID_FLOAT,"image_height");
    image_height->set(512.0);

    render_in = (vsx_module_param_render*)in_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_in");
    render_in->run_activate_offscreen = true;

    render_out = (vsx_module_param_render*)out_parameters.create(VSX_MODULE_PARAM_ID_RENDER,"render_out");

    gl_state = vsx_gl_state::get_instance();
    capture_in_progress = 0;
    M = 32;
    N = 32;


    pixeldata = (char*)malloc( 512 * 512 * 4 );
    pixeldata_target = (char*)malloc( 512 * 512 * 4 * M * N );
  }

  void on_delete()
  {
    if (!texture)
      return;

    delete texture;
    free(pixeldata);
    free(pixeldata_target);
  }

  bool activate_offscreen()
  {
    if (!texture)
    {
      texture = new vsx_texture<>;
      buffer.init(texture, 512,512, false,true,true,false,0);
    }


    double z_near = 0.01;
    double z_far = 2000.0;
    double aspect = 1.0 / gl_state->viewport_width_div_height_get();
    if (capture_in_progress)
      aspect = 1.0;
    double fovy = clamp(fov->get(),1.0,180.0);
    double right = -0.5 * tan( PI - (fovy * (PI / 180.0f)) * 0.5 ) * z_near;
    double left = -right;
    double top = aspect * right;
    double bottom = -top;

    if (start_capture->get())
    {
      start_capture->set(0);
      capture_in_progress = 1;
      n = m = 0;
      frustum_segment_x = (right - left) / (double)M;
      frustum_segment_y = (top - bottom) / (double)N;
    }

    if (capture_in_progress)
      buffer.begin_capture_to_buffer();



    // save current matrix
    gl_state->matrix_get_v( VSX_GL_PROJECTION_MATRIX, tmpMat );

    gl_state->matrix_mode( VSX_GL_MODELVIEW_MATRIX );
    gl_state->matrix_load_identity();

    gl_state->matrix_mode( VSX_GL_PROJECTION_MATRIX );
    gl_state->matrix_load_identity();



    if (!capture_in_progress)
    {
      gl_state->matrix_frustum(left, right, bottom, top, z_near, z_far);
      return true;
    }

    gl_state->matrix_frustum(
      left + frustum_segment_x * (double)(m),
      left + frustum_segment_x * (double)(m + 1),
      bottom + frustum_segment_y * (double)(n),
      bottom + frustum_segment_y * (double)(n + 1),
      z_near,
      z_far
    );


    int old_depth_mask;
    int old_depth_test;

    old_depth_mask = gl_state->depth_mask_get();
    old_depth_test = gl_state->depth_test_get();

    gl_state->depth_mask_set( 1, true );
    gl_state->depth_test_set( 1, true );

    glClearColor(0,0,0,0);
    glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    gl_state->depth_mask_set( old_depth_mask, true );
    gl_state->depth_test_set( old_depth_test, true );

    return true;

  }

  void deactivate_offscreen() {
    // reset the matrix to previous value
    gl_state->matrix_mode( VSX_GL_PROJECTION_MATRIX );
    gl_state->matrix_load_identity();
    gl_state->matrix_mult_f( tmpMat );

    if (!capture_in_progress)
      return;

    char filename[512];

    glReadPixels(0,0,512,512,GL_BGRA,GL_UNSIGNED_BYTE, pixeldata);

    size_t row_size = 512 * 4 * M;
    size_t subrow_size = 512*4;
    for (size_t row = 0; row < 512; row++) {
      size_t target_pos = row_size * 512 * n + subrow_size * m + row_size * row;
      size_t source_pos = subrow_size * row;
      memcpy(&pixeldata_target[ target_pos ], &pixeldata[ source_pos ], subrow_size);
    }

    buffer.end_capture_to_buffer();

    m++;

    if (m == M)
    {
      m = 0;
      n++;
    }

    if (n == N)
    {
      capture_in_progress = 0;

      sprintf( filename, "%sscreenshots/multi_%ld_entire_rgba.tga",vsx_data_path::get_instance()->data_path_get().c_str(),time(0));
      FILE* fp = fopen(filename,"wb");

      short width = 512*M;
      short height = 512*N;

      putc(0,fp);
      putc(0,fp);
      putc(2,fp);                         /* uncompressed RGB */
      putc(0,fp); putc(0,fp);
      putc(0,fp); putc(0,fp);
      putc(0,fp);
      putc(0,fp); putc(0,fp);           /* X origin */
      putc(0,fp); putc(0,fp);           /* y origin */
      putc((width & 0x00FF),fp);
      putc((width & 0xFF00) / 256,fp);
      putc((height & 0x00FF),fp);
      putc((height & 0xFF00) / 256,fp);
      putc(32,fp);                        /* 32 bit bitmap */
      putc(0,fp);

      fwrite(pixeldata_target, 1, 512*512*4*M*N, fp);
      fclose(fp);
    }
  }

  void output(vsx_module_param_abs* param) { VSX_UNUSED(param);
    render_out->set(render_in->get());
  }

};
