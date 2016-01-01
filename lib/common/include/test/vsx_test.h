#pragma once

#define test_assert(a) if (!a) vsx_printf(L"test failed in %s on line %d", __FILE__, __LINE__);