//****************************************************************************//
// coremorphanimation.cpp                                                     //
// Copyright (C) 2003 Steven Geens                                            //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "cal3d/coremorphanimation.h"

 /*****************************************************************************/
/** Adds a core mesh ID and a morph target ID of that core mesh.
  *
  * @param coreMeshID A core mesh ID that should be added.
  * @param morphTargetID A morph target ID that should be added.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreMorphAnimation::addMorphTarget(int coreMeshID,int morphTargetID)
{
  m_vectorCoreMeshID.push_back(coreMeshID);
  m_vectorMorphTargetID.push_back(morphTargetID);

  return true;
}


 /*****************************************************************************/
/** Returns the core mesh ID list.
  *
  * This function returns the list that contains all core mesh IDs of the core
  * morph animation instance.
  *
  * @return A reference to the core mesh ID list.
  *****************************************************************************/

std::vector<int>& CalCoreMorphAnimation::getVectorCoreMeshID()
{
  return m_vectorCoreMeshID;
}

 /*****************************************************************************/
/** Returns the morph target ID list.
  *
  * This function returns the list that contains all morph target  IDs of the core
  * morph animation instance.
  *
  * @return A reference to the morph target ID list.
  *****************************************************************************/

std::vector<int>& CalCoreMorphAnimation::getVectorMorphTargetID()
{
  return m_vectorMorphTargetID;
}

//****************************************************************************//
