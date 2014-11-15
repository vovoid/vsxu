#include <OpenEXR/OpenEXRConfig.h>
#include <OpenEXR/ImfArray.h>
#include <OpenEXR/ImfRgba.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfRgbaFile.h>
#include <OpenEXR/ImathBox.h>


using namespace Imf;
using namespace Imath;

class module_texture_load_openexr : public vsx_module
{
  // in
  float time;
  vsx_module_param_resource* filename;

  // out
  vsx_module_param_texture* result_texture;

  // internal
  vsx_bitmap bitm;
  int bitm_timestamp;

  vsx_texture* texture;

public:

  void module_info(vsx_module_info* info)
  {
    info->identifier =
      "texture;loaders;exr_tex_load";

    info->description =
      "";

    info->in_param_spec =
        "filename:resource"
    ;

    info->out_param_spec =
      "texture:texture";

    info->component_class =
      "texture";
  }

  void declare_params(vsx_module_param_list& in_parameters, vsx_module_param_list& out_parameters)
  {

    filename = (vsx_module_param_resource*)in_parameters.create(VSX_MODULE_PARAM_ID_RESOURCE,"filename");
    filename->set("");
    bitm_timestamp = 0;

    texture = new vsx_texture;
    texture->init_opengl_texture_2d();

    result_texture = (vsx_module_param_texture*)out_parameters.create(VSX_MODULE_PARAM_ID_TEXTURE,"texture");
    loading_done = true;
  }



  void readGZ1
  (
    const char fileName[],
    Array2D<float> &rPixels,
    Array2D<float> &gPixels,
    Array2D<float> &bPixels,
    Array2D<float> &aPixels,
    int &width,
    int &height
  )
  {
    InputFile file (fileName);
    Box2i dw = file.header().dataWindow();


    width = dw.max.x - dw.min.x + 1;
    height = dw.max.y - dw.min.y + 1;

    vsx_printf(L"width %d\n", width);
    vsx_printf(L"height %d\n", height);

    rPixels.resizeErase (height, width);
    gPixels.resizeErase (height, width);
    bPixels.resizeErase (height, width);
    aPixels.resizeErase (height, width);

    FrameBuffer frameBuffer;

    frameBuffer.insert
    (
      "R", // name
      Slice
      (
        FLOAT, // type
        (char *)
        (
          &rPixels[0][0] - // base
          dw.min.x -
          dw.min.y * width
        ),
        sizeof (rPixels[0][0]) * 1, // xStride
        sizeof (rPixels[0][0]) * width, // yStride
        1,
        1, // x/y sampling
        0.0
      )
    ); // fillValue

    frameBuffer.insert
    (
      "G", // name
      Slice
      (
        FLOAT, // type
        (char *)
        (
          &gPixels[0][0] - // base
          dw.min.x -
          dw.min.y * width
        ),
        sizeof (gPixels[0][0]) * 1, // xStride
        sizeof (gPixels[0][0]) * width, // yStride
        1,
        1, // x/y sampling
        0.0
      )
    ); // fillValue

    frameBuffer.insert
    (
      "B", // name
      Slice
      (
        FLOAT, // type
        (char *)
        (
          &bPixels[0][0] - // base
          dw.min.x -
          dw.min.y * width
        ),
        sizeof (bPixels[0][0]) * 1, // xStride
        sizeof (bPixels[0][0]) * width, // yStride
        1,
        1, // x/y sampling
        0.0
      )
    ); // fillValue

    frameBuffer.insert
    (
      "A", // name
      Slice
      (
        FLOAT, // type
        (char *)
        (
          &aPixels[0][0] - // base
          dw.min.x -
          dw.min.y * width
        ),
        sizeof (aPixels[0][0]) * 1, // xStride
        sizeof (aPixels[0][0]) * width, // yStride
        1,
        1, // x/y sampling
        0.0
      )
    ); // fillValue

    file.setFrameBuffer (frameBuffer);
    file.readPixels (dw.min.y, dw.max.y);
  }


  /*
  void readRgba1(
      const char fileName[],
      Array2D<Rgba> &pixels,
      int &width,
      int &height
  )
  {
    RgbaInputFile file (fileName);
    Box2i dw = file.dataWindow();
    width = dw.max.x - dw.min.x + 1;
    height = dw.max.y - dw.min.y + 1;
    pixels.resizeErase (height, width);
    file.setFrameBuffer (&pixels[0][0] - dw.min.x - dw.min.y * width, 1, width);
    file.readPixels (dw.min.y, dw.max.y);
  }
  */

  void param_set_notify(const vsx_string<>& name)
  {
    VSX_UNUSED(name);
    Array2D<float> rPixels;
    Array2D<float> gPixels;
    Array2D<float> bPixels;
    Array2D<float> aPixels;
    int width, height;

    vsx_string<>nf = engine->filesystem->get_base_path() + filename->get();

    readGZ1
    (
      nf.c_str(),
      rPixels,
      gPixels,
      bPixels,
      aPixels,
      width,
      height
    );

    bitm.bpp = GL_RGBA32F_ARB;
    bitm.bformat = GL_RGBA;

    size_t count_bytes = sizeof(float) * width * height * 4;
    float* data = (float*)malloc( count_bytes );
    bitm.data = (void*)data;

    for (size_t y = 0; y < height; y++)
    {
      for (size_t x = 0; x < width; x++)
      {
        data[ y * width * 4 + x * 4     ] = rPixels[ y ][ x ];
        data[ y * width * 4 + x * 4 + 1 ] = gPixels[ y ][ x ];
        data[ y * width * 4 + x * 4 + 2 ] = bPixels[ y ][ x ];
        data[ y * width * 4 + x * 4 + 3 ] = aPixels[ y ][ x ];
      }
    }

    bitm.size_x = width;
    bitm.size_y = height;

    bitm_timestamp++;
    bitm.timestamp = bitm_timestamp;

    texture->upload_ram_bitmap_2d(&bitm,false);

    free(data);

    result_texture->set(texture);
  }

  void stop()
  {
    texture->unload();
  }

  void start()
  {
    texture->init_opengl_texture_2d();
    texture->upload_ram_bitmap_2d(&bitm,false);
    result_texture->set(texture);
  }

  void on_delete()
  {
    texture->unload();
    delete texture;
  }

};


