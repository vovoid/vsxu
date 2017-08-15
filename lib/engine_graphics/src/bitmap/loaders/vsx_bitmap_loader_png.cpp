#include <bitmap/loaders/vsx_bitmap_loader_png.h>
#include <lodepng.h>

void vsx_bitmap_loader_png::worker(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename)
{
  vsx::file* file = filesystem->f_open(filename.c_str());
  if (!file)
    vsx_printf(L"vsx_bitmap_loader_png::worker file is null: %hs\n", filename.c_str());
  req(file);

  unsigned char* data = filesystem->f_data_get(file);

  unsigned char* result;
  lodepng_decode32(&result, &bitmap->width, &bitmap->height, data, filesystem->f_get_size(file));
  filesystem->f_close(file);

  bitmap->filename = filename;
  bitmap->channels = 4;
  bitmap->data_set( result, 0, 0, bitmap->width * bitmap->height * bitmap->channels );

  handle_transformations(bitmap);

  bitmap->timestamp = vsx_singleton_counter::get();
  bitmap->data_ready.fetch_add(1);
}
