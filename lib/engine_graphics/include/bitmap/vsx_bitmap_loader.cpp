#include <bitmap/vsx_bitmap_loader.h>

vsx_bitmap_loader_base *vsx_bitmap_loader::get_data_loader_by_image_type(vsx_string<> &filename)
{
  if (vsx_string_helper::verify_filesuffix(filename, "png"))
    return vsx_bitmap_loader_png::get_instance();

  if (vsx_string_helper::verify_filesuffix(filename, "jpg"))
    return vsx_bitmap_loader_jpg::get_instance();

  if (vsx_string_helper::verify_filesuffix(filename, "tga"))
    return vsx_bitmap_loader_tga::get_instance();

  if (vsx_string_helper::verify_filesuffix(filename, "dds"))
    return vsx_bitmap_loader_dds::get_instance();

  return 0x0;
}

void vsx_bitmap_loader::do_load(vsx_bitmap *bitmap, vsx_string<> filename, vsx::filesystem *filesystem, bool thread)
{
  vsx_bitmap_loader_base* data_loader = vsx_bitmap_loader::get_data_loader_by_image_type( filename );
  req_error(data_loader, "unknown image format");
  data_loader->load( bitmap, filename, filesystem, thread );
}

void vsx_bitmap_loader::reload(vsx_bitmap *bitmap, vsx_string<> filename, vsx::filesystem *filesystem, bool thread, uint64_t hint)
{
  bitmap->hint = hint;
  do_load(bitmap, filename, filesystem, thread);
}

void vsx_bitmap_loader::load(vsx_bitmap *bitmap, vsx_string<> filename, vsx::filesystem *filesystem, bool thread, uint64_t hint)
{
  if (bitmap->references > 1)
    ret(bitmap->timestamp = vsx_singleton_counter::get());
  bitmap->hint = hint;

  vsx_string<> question_mark = "?";
  int question_mark_location = filename.find(question_mark);
  if (question_mark_location > 0)
    filename = filename.substr(0, question_mark_location);

  vsx_printf(L"DEBUG: bitmap loader filename: %hs\n", filename.c_str());

  do_load(bitmap, filename, filesystem, thread);
}
