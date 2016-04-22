#pragma once

#include <string/vsx_string.h>
#include <string/vsx_string_helper.h>

size_t num_errors = 0;

#define test_assert(a) if (!(a)) { num_errors++; vsx_printf(L"test failed in %s on line %d\n", vsx_string_helper::char_to_wchar(__FILE__).c_str(), __LINE__); }
#define test_complete if (num_errors) { vsx_printf(L"** FAIL\n"); } else { vsx_printf(L"** OK\n"); }
