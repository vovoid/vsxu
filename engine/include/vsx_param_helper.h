#ifndef VSX_PARAM_HELPER_H
#define VSX_PARAM_HELPER_H

#include <string/vsx_string.h>

class vsx_param_helper
{
public:
  static int param_id_from_string(vsx_string<> param_name)
  {
    if (vsx_string<>::s_equals(param_name, "int"))
      return VSX_MODULE_PARAM_ID_INT;

    if (vsx_string<>::s_equals(param_name, "render"))
      return VSX_MODULE_PARAM_ID_RENDER;

    if (vsx_string<>::s_equals(param_name, "float3"))
      return VSX_MODULE_PARAM_ID_FLOAT3;

    if (vsx_string<>::s_equals(param_name, "double"))
      return VSX_MODULE_PARAM_ID_DOUBLE;

    if (vsx_string<>::s_equals(param_name, "string"))
      return VSX_MODULE_PARAM_ID_STRING;

    if (vsx_string<>::s_equals(param_name, "texture"))
      return VSX_MODULE_PARAM_ID_TEXTURE;

    if (vsx_string<>::s_equals(param_name, "float"))
      return VSX_MODULE_PARAM_ID_FLOAT;

    if (vsx_string<>::s_equals(param_name, "float4"))
      return VSX_MODULE_PARAM_ID_FLOAT4;

    if (vsx_string<>::s_equals(param_name, "matrix"))
      return VSX_MODULE_PARAM_ID_MATRIX;

    if (vsx_string<>::s_equals(param_name, "mesh"))
      return VSX_MODULE_PARAM_ID_MESH;

    if (vsx_string<>::s_equals(param_name, "bitmap"))
      return VSX_MODULE_PARAM_ID_BITMAP;

    if (vsx_string<>::s_equals(param_name, "particlesystem"))
      return VSX_MODULE_PARAM_ID_PARTICLESYSTEM;

    if (vsx_string<>::s_equals(param_name, "float_array"))
      return VSX_MODULE_PARAM_ID_FLOAT_ARRAY;

    if (vsx_string<>::s_equals(param_name, "float_sequence"))
      return VSX_MODULE_PARAM_ID_FLOAT_SEQUENCE;

    if (vsx_string<>::s_equals(param_name, "string_sequence"))
      return VSX_MODULE_PARAM_ID_STRING_SEQUENCE;

    if (vsx_string<>::s_equals(param_name, "segment_mesh"))
      return VSX_MODULE_PARAM_ID_SEGMENT_MESH;

    if (vsx_string<>::s_equals(param_name, "abstract"))
      return VSX_MODULE_PARAM_ID_ABSTRACT;

    if (vsx_string<>::s_equals(param_name, "quaternion"))
      return VSX_MODULE_PARAM_ID_QUATERNION;

    if (vsx_string<>::s_equals(param_name, "resource"))
      return VSX_MODULE_PARAM_ID_RESOURCE;

    if (vsx_string<>::s_equals(param_name, "float3_array"))
      return VSX_MODULE_PARAM_ID_FLOAT3_ARRAY;

    if (vsx_string<>::s_equals(param_name, "quaternion_array"))
      return VSX_MODULE_PARAM_ID_QUATERNION_ARRAY;

    return -1;
  }
};

#endif
