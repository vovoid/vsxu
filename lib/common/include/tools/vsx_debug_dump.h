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

inline void debug_dump_value(vsx_vector3f& v, vsx_string<> name)
{
  vsx_printf(L"%hs: %f, %f, %f\n", name.c_str(), v.x, v.y, v.z);
}

#ifdef VSX_COLOR_H
inline void debug_dump_value(vsx_colorf& v, vsx_string<> name)
{
  vsx_printf(L"%hs: %f, %f, %f, %f\n", name.c_str(), v.r, v.g, v.b, v.a);
}
#endif

#ifdef VSX_MATRIX
inline void debug_dump_value( vsx_matrixf& m, vsx_string<> name)
{
  vsx_printf(
    L"%hs. 0: %f 1: %f 2: %f 3: %f\n"
    "4: %f 5: %f 6: %f 7: %f\n"
    "8: %f 9: %f 10: %f 11: %f\n"
    "12: %f 13: %f 14: %f 15: %f\n",
    name.c_str(),
    m.m[0],m.m[1],m.m[2],m.m[3],
    m.m[4],m.m[5],m.m[6],m.m[7],
    m.m[8],m.m[9],m.m[10],m.m[11],
    m.m[12],m.m[13],m.m[14],m.m[15]
  );
}
#endif

#define VSX_DUMP(v) debug_dump_value(v, #v );

