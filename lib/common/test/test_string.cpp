#include <string/vsx_string_helper.h>
#include <time/vsx_timer.h>
#include <vsx_argvector.h>
#include <test/vsx_test.h>

void test_string_replace()
{
  vsx_string<> test1("something $$name something");
  test1.replace("$$name", "abc");

  test_assert((test1 == "something abc something"));
}

void test_string_replace_helper()
{
  vsx_string<> test1("something $$name something");
  test1 = vsx_string_helper::str_replace(vsx_string<>("$$name"), vsx_string<>("abc"), test1);

  test_assert((test1 == "something abc something"));
}

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  test_string_replace();
  test_string_replace_helper();
  test_complete
  return 0;
}

