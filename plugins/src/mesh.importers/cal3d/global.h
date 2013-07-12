//****************************************************************************//
// global.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_GLOBAL_H
#define CAL_GLOBAL_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

// autoconf/automake includes
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// platform dependent includes
#include "cal3d/platform.h"

//****************************************************************************//
// Define options
//****************************************************************************//

//#define CAL_DEBUG

//Uncomment this if you want to use 16bit indices or configure the compiler

//#define CAL_16BIT_INDICES

#ifdef CAL_16BIT_INDICES 
typedef unsigned short CalIndex; 
#else 
typedef int CalIndex; 
#endif


//****************************************************************************//
// Global Cal3D namespace for constants, ...                                  //
//****************************************************************************//

namespace Cal
{  
  // global typedefs
  typedef void *UserData;

  // file magic cookies
  const char SKELETON_FILE_MAGIC[4]  = { 'C', 'S', 'F', '\0' };
  const char ANIMATION_FILE_MAGIC[4] = { 'C', 'A', 'F', '\0' };
  const char MESH_FILE_MAGIC[4]      = { 'C', 'M', 'F', '\0' };
  const char MATERIAL_FILE_MAGIC[4]  = { 'C', 'R', 'F', '\0' };

  const char SKELETON_XMLFILE_MAGIC[4]  = { 'X', 'S', 'F', '\0' };
  const char ANIMATION_XMLFILE_MAGIC[4]  = { 'X', 'A', 'F', '\0' };
  const char MESH_XMLFILE_MAGIC[4]  = { 'X', 'M', 'F', '\0' };
  const char MATERIAL_XMLFILE_MAGIC[4]  = { 'X', 'R', 'F', '\0' };

  // library version
  const int LIBRARY_VERSION = 1000;

  // file versions
  const int CURRENT_FILE_VERSION = LIBRARY_VERSION;
  const int EARLIEST_COMPATIBLE_FILE_VERSION = 699;

  // empty string
  const std::string strNull;
};

#endif

//****************************************************************************//
