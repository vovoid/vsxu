//****************************************************************************//
// coretrack.cpp                                                              //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include "cal3d/coretrack.h"
#include "cal3d/error.h"
#include "cal3d/corekeyframe.h"


CalCoreTrack::CalCoreTrack()
  : m_coreBoneId(-1)
{
}


CalCoreTrack::~CalCoreTrack()
{
  // destroy all core keyframes
  for (unsigned i = 0; i < m_keyframes.size(); ++i)
  {
    delete m_keyframes[i];
  }
  m_keyframes.clear();

  m_coreBoneId = -1;
}

 /*****************************************************************************/
/** Adds a core keyframe.
  *
  * This function adds a core keyframe to the core track instance.
  *
  * @param pCoreKeyframe A pointer to the core keyframe that should be added.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreTrack::addCoreKeyframe(CalCoreKeyframe *pCoreKeyframe)
{
  m_keyframes.push_back(pCoreKeyframe);
  int idx = m_keyframes.size() - 1;
  while (idx > 0 && m_keyframes[idx]->getTime() < m_keyframes[idx - 1]->getTime()) {
    std::swap(m_keyframes[idx], m_keyframes[idx - 1]);
    --idx;
  }

  return true;
}

 /*****************************************************************************/
/** Returns the ID of the core bone.
  *
  * This function returns the ID of the core bone to which the core track
  * instance is attached to.
  *
  * @return One of the following values:
  *         \li the \b ID of the core bone
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreTrack::getCoreBoneId()
{
  return m_coreBoneId;
}

 /*****************************************************************************/
/** Returns a specified state.
  *
  * This function returns the state (translation and rotation of the core bone)
  * for the specified time and duration.
  *
  * @param time The time in seconds at which the state should be returned.
  * @param translation A reference to the translation reference that will be
  *                    filled with the specified state.
  * @param rotation A reference to the rotation reference that will be filled
  *                 with the specified state.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreTrack::getState(float time, CalVector& translation, CalQuaternion& rotation)
{
  std::vector<CalCoreKeyframe*>::iterator iteratorCoreKeyframeBefore;
  std::vector<CalCoreKeyframe*>::iterator iteratorCoreKeyframeAfter;

  // get the keyframe after the requested time
  iteratorCoreKeyframeAfter = getUpperBound(time);

  // check if the time is after the last keyframe
  if(iteratorCoreKeyframeAfter == m_keyframes.end())
  {
    // return the last keyframe state
    --iteratorCoreKeyframeAfter;
    rotation = (*iteratorCoreKeyframeAfter)->getRotation();
    translation = (*iteratorCoreKeyframeAfter)->getTranslation();

    return true;
  }

  // check if the time is before the first keyframe
  if(iteratorCoreKeyframeAfter == m_keyframes.begin())
  {
    // return the first keyframe state
    rotation = (*iteratorCoreKeyframeAfter)->getRotation();
    translation = (*iteratorCoreKeyframeAfter)->getTranslation();

    return true;
  }

  // get the keyframe before the requested one
  iteratorCoreKeyframeBefore = iteratorCoreKeyframeAfter;
  --iteratorCoreKeyframeBefore;

  // get the two keyframe pointers
  CalCoreKeyframe *pCoreKeyframeBefore;
  pCoreKeyframeBefore = *iteratorCoreKeyframeBefore;
  CalCoreKeyframe *pCoreKeyframeAfter;
  pCoreKeyframeAfter = *iteratorCoreKeyframeAfter;

  // calculate the blending factor between the two keyframe states
  float blendFactor;
  blendFactor = (time - pCoreKeyframeBefore->getTime()) / (pCoreKeyframeAfter->getTime() - pCoreKeyframeBefore->getTime());

  // blend between the two keyframes
  translation = pCoreKeyframeBefore->getTranslation();
  translation.blend(blendFactor, pCoreKeyframeAfter->getTranslation());

  rotation = pCoreKeyframeBefore->getRotation();
  rotation.blend(blendFactor, pCoreKeyframeAfter->getRotation());

  return true;
}

std::vector<CalCoreKeyframe*>::iterator CalCoreTrack::getUpperBound(float time)
{

  int lowerBound = 0;
  int upperBound = m_keyframes.size()-1;

  while(lowerBound<upperBound-1)
  {
	  int middle = (lowerBound+upperBound)/2;

	  if(time >= m_keyframes[middle]->getTime())
	  {
		  lowerBound=middle;
	  }
	  else
	  {
		  upperBound=middle;
	  }
  }

  return m_keyframes.begin() + upperBound;

}

 /*****************************************************************************/
/** Sets the ID of the core bone.
  *
  * This function sets the ID of the core bone to which the core track instance
  * is attached to.
  *
  * @param coreBoneId The ID of the bone to which the core track instance should
  *                   be attached to.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreTrack::setCoreBoneId(int coreBoneId)
{
  if(coreBoneId < 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_coreBoneId = coreBoneId;

  return true;
}


int CalCoreTrack::getCoreKeyframeCount()
{
  return m_keyframes.size();
}


CalCoreKeyframe* CalCoreTrack::getCoreKeyframe(int idx)
{
  return m_keyframes[idx];
}

 /*****************************************************************************/
/** Scale the core track.
  *
  * This function rescale all the data that are in the core track instance.
  *
  * @param factor A float with the scale factor
  *
  *****************************************************************************/


void CalCoreTrack::scale(float factor)
{
  for(size_t keyframeId = 0; keyframeId < m_keyframes.size(); keyframeId++)
  {
    CalVector translation = m_keyframes[keyframeId]->getTranslation();
    translation*=factor;
    m_keyframes[keyframeId]->setTranslation(translation);
  }

}

//****************************************************************************//
