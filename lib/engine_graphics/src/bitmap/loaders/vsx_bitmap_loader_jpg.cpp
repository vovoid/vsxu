#include <tools/vsx_thread_pool.h>
#include <bitmap/loaders/vsx_bitmap_loader_jpg.h>
#include <jpeg_decoder.h>


void vsx_bitmap_loader_jpg::worker(vsx_bitmap* bitmap, vsx::filesystem* filesystem, vsx_string<> filename)
{
  vsx::file* file = filesystem->f_open(filename.c_str());
  req(file);
  njInit();

  unsigned char* data = filesystem->f_data_get(file);
  nj_result_t result = njDecode(data, (int)filesystem->f_get_size(file));
  filesystem->f_close(file);

  req(result == NJ_OK);

  bitmap->filename = filename;
  bitmap->width = njGetWidth();
  bitmap->height = njGetHeight();

  if (njIsColor())
    bitmap->channels = 3;
  else
    bitmap->channels = 1;

  bitmap->data_set( njGetImage(), 0, 0, bitmap->channels * bitmap->height * bitmap->width );

  handle_transformations(bitmap);

  bitmap->timestamp = vsx_singleton_counter::get();
  bitmap->data_ready.fetch_add(1);
}

