#include <test/vsx_test.h>

#include <perf/vsx_perf.h>



void test_cache_misses()
{
  long long count;
  const char* foo = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
  vsx_perf perf_cache;
  perf_cache.cache_misses_start();

  perf_cache.cache_misses_begin();
  printf("Measuring cache miss count for 1111 printf\n");
  count = perf_cache.cache_misses_end();

  printf("Caused %lld cache misses\n", count);

  perf_cache.cache_misses_begin();
  printf("Measuring cache miss count for 2222 printf %d\n", foo[rand() % 64 ]);
  count = perf_cache.cache_misses_end();

  printf("Caused %lld cache misses", count);

  perf_cache.cache_misses_stop();
}

void test_cpu_instructions()
{
  long long count;
  vsx_perf perf_instructions;
  perf_instructions.cpu_instructions_start();

  perf_instructions.cpu_instructions_begin();
  printf("Measuring cpu instruction count for this printf\n");
  count = perf_instructions.cpu_instructions_end();

  printf("Used %lld instructions\n", count);

  perf_instructions.cpu_instructions_begin();
  printf("Measuring cpu instruction count for this printf\n");
  count = perf_instructions.cpu_instructions_end();

  printf("Used %lld instructions\n", count);

  perf_instructions.cpu_instructions_begin();
  printf("Measuring cpu instruction count for this printf\n");
  count = perf_instructions.cpu_instructions_end();

  printf("Used %lld instructions\n", count);

  perf_instructions.cpu_instructions_stop();
}

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  test_cpu_instructions();
  test_cache_misses();

  test_complete
  return 0;
}

