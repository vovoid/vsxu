#include <test/vsx_test.h>

#include <string/vsx_json.h>

int main(int argc, char *argv[])
{
  VSX_UNUSED(argc);
  VSX_UNUSED(argv);

  std::string err;
  vsx::json js = vsx::json::parse(
        "{"
        " \"moo\" : \"bar\""
        "}"
        ,
        err
        );
  test_assert( !err.size() );
  test_assert( js["moo"] == "bar");

  test_complete
  return 0;
}

