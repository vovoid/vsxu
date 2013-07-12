//****************************************************************************//
// loader.h                                                                   //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_LOADER_H
#define CAL_LOADER_H

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//


#include <string>
#include <istream>
#include "cal3d/global.h"
#include "cal3d/datasource.h"
#include "cal3d/tinyxml.h"

//****************************************************************************//
// Forward declarations                                                       //
//****************************************************************************//

class CalCoreModel;
class CalCoreSkeleton;
class CalCoreBone;
class CalCoreAnimation;
class CalCoreTrack;
class CalCoreKeyframe;
class CalCoreMesh;
class CalCoreSubmesh;
class CalCoreMaterial;

enum
{
    LOADER_ROTATE_X_AXIS = 1,
    LOADER_INVERT_V_COORD = 2
};

//****************************************************************************//
// Class declaration                                                          //
//****************************************************************************//

 /*****************************************************************************/
/** The loader class.
  *****************************************************************************/

class CAL3D_API CalLoader
{
// member functions
public:
  static CalCoreAnimation *loadCoreAnimation(const std::string& strFilename, CalCoreSkeleton *skel=NULL);
  static CalCoreMaterial *loadCoreMaterial(const std::string& strFilename);
  static CalCoreMesh *loadCoreMesh(const std::string& strFilename);
  static CalCoreSkeleton *loadCoreSkeleton(const std::string& strFilename);

  static CalCoreAnimation *loadCoreAnimation(std::istream& inputStream, CalCoreSkeleton *skel=NULL);
  static CalCoreMaterial *loadCoreMaterial(std::istream& inputStream);
  static CalCoreMesh *loadCoreMesh(std::istream& inputStream);
  static CalCoreSkeleton *loadCoreSkeleton(std::istream& inputStream);

  static CalCoreAnimation *loadCoreAnimation(void* inputBuffer, CalCoreSkeleton *skel=NULL);
  static CalCoreMaterial *loadCoreMaterial(void* inputBuffer);
  static CalCoreMesh *loadCoreMesh(void* inputBuffer);
  static CalCoreSkeleton *loadCoreSkeleton(void* inputBuffer);

  static CalCoreAnimation *loadCoreAnimation(CalDataSource& inputSrc, CalCoreSkeleton *skel=NULL);
  static CalCoreMaterial *loadCoreMaterial(CalDataSource& inputSrc);
  static CalCoreMesh *loadCoreMesh(CalDataSource& inputSrc);
  static CalCoreSkeleton *loadCoreSkeleton(CalDataSource& inputSrc);

  static CalCoreAnimation *loadXmlCoreAnimation_xml(vsxTiXmlDocument &doc, CalCoreSkeleton *skel=NULL);
  static CalCoreSkeleton *loadXmlCoreSkeleton_xml(vsxTiXmlDocument &doc);
  static CalCoreMesh *loadXmlCoreMesh_xml(vsxTiXmlDocument &doc);
  static CalCoreMaterial *loadXmlCoreMaterial_xml(vsxTiXmlDocument &doc);


  static void setLoadingMode(int flags);

private:
  static CalCoreBone *loadCoreBones(CalDataSource& dataSrc);
  static CalCoreKeyframe *loadCoreKeyframe(CalDataSource& dataSrc);
  static CalCoreSubmesh *loadCoreSubmesh(CalDataSource& dataSrc);
  static CalCoreTrack *loadCoreTrack(CalDataSource& dataSrc, CalCoreSkeleton *skel);

  static CalCoreAnimation *loadXmlCoreAnimation(const std::string& strFilename, CalCoreSkeleton *skel=NULL);
  static CalCoreSkeleton *loadXmlCoreSkeleton(const std::string& strFilename);
  static CalCoreMesh *loadXmlCoreMesh(const std::string& strFilename);
  static CalCoreMaterial *loadXmlCoreMaterial(const std::string& strFilename);

  static int loadingMode;
};

#endif

//****************************************************************************//
