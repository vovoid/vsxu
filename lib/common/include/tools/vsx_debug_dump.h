#pragma once

inline void debug_dump_value(double& v, vsx_string<> name)
{
  vsx_printf(L"%hs: %f\n", name.c_str(), v);
}

inline void debug_dump_value(float& v, vsx_string<> name)
{
  vsx_printf(L"%hs: %f\n", name.c_str(), v);
}

inline void debug_dump_value(vsx_vector2f& v, vsx_string<> name)
{
  vsx_printf(L"%hs: %f, %f\n", name.c_str(), v.x, v.y);
}



inline void debug_dump_value(vsx_vector2d& v, vsx_string<> name)
{
  vsx_printf(L"%hs: %f, %f\n", name.c_str(), v.x, v.y);
}

inline void debug_dump_value(vsx_vector3d& v, vsx_string<> name)
{
  vsx_printf(L"%hs: %f, %f, %f\n", name.c_str(), v.x, v.y, v.z);
}


#define VSX_DUMP(v) debug_dump_value(v, #v );

