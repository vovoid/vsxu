#include <filesystem/mmap/vsx_filesystem_mmap.h>
#include <string/vsx_string_helper.h>
#include <time/vsx_timer.h>
#include <vsx_argvector.h>

size_t times = 2000;
size_t block = 1024*1024;

void reset_linux_cache(vsx_string<> name, int value)
{
  FILE* fp = fopen(name.c_str(), "w");
  vsx_string<> value_str = vsx_string_helper::i2s(value);
  fputs(value_str.c_str(), fp);
  fclose(fp);
}

vsx_string<> create_random_data_file()
{
  vsx_string<> filename = "test_fread_" + vsx_string_helper::i2s(rand());
  FILE* fp = fopen(filename.c_str(), "wb");
  for (uint64_t i = 0; i < times; i++)
  {
    char* buf = (char*)malloc(block);
    for (size_t j = 0; j < block; j++ )
      buf[j] = rand();

    fwrite(buf,1, block, fp);
  }
  fclose(fp);
  return filename;
}

int main(int argc, char *argv[])
{
  srand(time(0x0));
  vsx_argvector::get_instance()->init_from_argc_argv(argc, argv);
  if (vsx_argvector::get_instance()->has_param_with_value("times"))
    times = vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_value("times") );

  if (vsx_argvector::get_instance()->has_param_with_value("block"))
    block = vsx_string_helper::s2i( vsx_argvector::get_instance()->get_param_value("block") );
  vsx_timer timer;

  vsx_string<> name = create_random_data_file();

//  reset_linux_cache("/proc/sys/vm/drop_caches", 3);

  timer.start();
  vsx::file* fp = vsx::filesystem::get_instance()->f_open(name.c_str());
  unsigned char* data = (unsigned char*)vsx::filesystem::get_instance()->f_gets_entire(fp);
  float map_time = timer.dtime();
  uint64_t sum = 0;
//  vsx_printf(L"file handle size: %d\n", times * block);

  for (size_t i = 0; i < block*times; i++)
  {
//    vsx_printf(L"data: %d\n", data[i]);
    sum += data[i];
  }
  float read_time = timer.dtime();

//  vsx_printf(L"fread time: %f\n", map_time);
//  vsx_printf(L"read time: %f\n", read_time);
  vsx_printf(L"sum: %f\n", map_time + read_time);

  unlink(name.c_str());

//  char b = getchar();

  return 0;
}

