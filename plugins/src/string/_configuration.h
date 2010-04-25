/*
 * _configuration.h
 */
#ifdef BUILDING_DLL
#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

// size optimization togglers
#define VSX_NO_TEXTURE
//#define VSX_TEXTURE_NO_R_UPLOAD
//#define VSX_TEXTURE_NO_RT
#define VSX_TEXTURE_NO_GLPNG
#define VSX_NO_SEQUENCE
#define VSX_NO_MESH
//#define VSX_STRINGLIB_NOSTL
#define VSX_NO_GL

// parameter defines
//#define VSX_P_INT
#define VSX_P_FLOAT
//#define VSX_P_FLOAT3
//#define VSX_P_FLOAT4
//#define VSX_P_DOUBLE
//#define VSX_P_MATRIX
//#define VSX_P_FLOAT_ARRAY
//#define VSX_P_FLOAT3_ARRAY
#define VSX_P_STRING
//#define VSX_P_SEQUENCE
//#define VSX_P_ABSTRACT
//#define VSX_P_QUATERNION
//#define VSX_P_QUATERNION_ARRAY
//#define VSX_P_TEXTURE
//#define VSX_P_BITMAP
//#define VSX_P_PARTICLESYSTEM
//#define VSX_P_SEGMENT_MESH
//#define VSX_P_MESH
//#define VSX_P_RENDER
//#define VSX_P_RESOURCE

#endif /* _CONFIGURATION_H_ */
#endif
