//****************************************************************************//
// morphtargetmixer.cpp                                                       //
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

//****************************************************************************//
// Includes                                                                   //
//****************************************************************************//

#include "cal3d/error.h"
#include "cal3d/morphtargetmixer.h"
#include "cal3d/model.h"
#include "cal3d/mesh.h"
#include "cal3d/submesh.h"
#include "cal3d/coremorphanimation.h"
#include "cal3d/coremodel.h"
#include "cal3d/coremesh.h"
#include "cal3d/coresubmesh.h"

 /*****************************************************************************/
/** Constructs the morph target mixer instance.
  *
  * This function is the default constructor of the morph target mixer instance.
  *****************************************************************************/

CalMorphTargetMixer::CalMorphTargetMixer(CalModel* pModel)
{
  assert(pModel);
  m_pModel = pModel;

  if(pModel->getCoreModel()->getCoreMorphAnimationCount() != 0)
  {
    int morphAnimationCount = pModel->getCoreModel()->getCoreMorphAnimationCount();
    // reserve the space needed in all the vectors
    m_vectorCurrentWeight.resize(morphAnimationCount);
    m_vectorEndWeight.resize(morphAnimationCount);
    m_vectorDuration.resize(morphAnimationCount);
    std::vector<float>::iterator iteratorCurrentWeight = m_vectorCurrentWeight.begin();
    std::vector<float>::iterator iteratorEndWeight = m_vectorEndWeight.begin();
    std::vector<float>::iterator iteratorDuration = m_vectorDuration.begin();
    while(iteratorCurrentWeight!=m_vectorCurrentWeight.end())
    {
      (*iteratorCurrentWeight) = 0.0f;
      (*iteratorEndWeight) = 0.0f;
      (*iteratorDuration) = 0.0f;
      ++iteratorCurrentWeight;
      ++iteratorEndWeight;
      ++iteratorDuration;
    }
  }
}


/*****************************************************************************/
/** Interpolates the weight of a morph target.
  *
  * This function interpolates the weight of a morph target a new value
  * in a given amount of time.
  *
  * @param id The ID of the morph target that should be blended.
  * @param weight The weight to interpolate the morph target to.
  * @param delay The time in seconds until the new weight should be reached.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/
bool CalMorphTargetMixer::blend(int id, float weight, float delay)
{
  if((id < 0) || (id >= (int)m_vectorCurrentWeight.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }
  m_vectorEndWeight[id] = weight;
  m_vectorDuration[id] = delay;
  return true;
}

 /*****************************************************************************/
/** Fades a morph target out.
  *
  * This function fades a morph target out in a given amount of time.
  *
  * @param id The ID of the morph target that should be faded out.
  * @param delay The time in seconds until the the morph target is
  *              completely removed.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalMorphTargetMixer::clear(int id, float delay)
{
  if((id < 0) || (id >= (int)m_vectorCurrentWeight.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }
  m_vectorEndWeight[id] = 0.0f;
  m_vectorDuration[id] = delay;
  return true;
}

 /*****************************************************************************/
/** Get the weight of a morph target.
  *
  * @param id The id of the morph target which weight you want.
  *
  * @return The weight of the morph target with the given id.
  *****************************************************************************/
float CalMorphTargetMixer::getCurrentWeight(int id)
{
  if((id < 0) || (id >= (int)m_vectorCurrentWeight.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }
  return m_vectorCurrentWeight[id];
}

 /*****************************************************************************/
/** Get the weight of the base vertices.
  *
  * @return The weight of the base vertices.
  *****************************************************************************/
float CalMorphTargetMixer::getCurrentWeightBase()
{
  float currentWeight = 1.0f;
  std::vector<float>::iterator iteratorCurrentWeight = m_vectorCurrentWeight.begin();
  while(iteratorCurrentWeight!=m_vectorCurrentWeight.end())
  {
    currentWeight -=(*iteratorCurrentWeight);
    ++iteratorCurrentWeight;
  }
  return currentWeight;
}

 /*****************************************************************************/
/** Updates all morph targets.
  *
  * This function updates all morph targets of the mixer instance for a
  * given amount of time.
  *
  * @param deltaTime The elapsed time in seconds since the last update.
  *****************************************************************************/

void CalMorphTargetMixer::update(float deltaTime)
{
  std::vector<float>::iterator iteratorCurrentWeight = m_vectorCurrentWeight.begin();
  std::vector<float>::iterator iteratorEndWeight = m_vectorEndWeight.begin();
  std::vector<float>::iterator iteratorDuration = m_vectorDuration.begin();
  while(iteratorCurrentWeight!=m_vectorCurrentWeight.end())
  {
    if(deltaTime >= (*iteratorDuration))
    {
      (*iteratorCurrentWeight) = (*iteratorEndWeight);
      (*iteratorDuration) = 0.0f;
    }
    else
    {
      (*iteratorCurrentWeight) += ((*iteratorEndWeight)-(*iteratorCurrentWeight)) *
                                  deltaTime/(*iteratorDuration);
      (*iteratorDuration) -= deltaTime;
    }
    ++iteratorCurrentWeight;
    ++iteratorEndWeight;
    ++iteratorDuration;
  }
  int morphAnimationID = 0;
  while(morphAnimationID<getMorphTargetCount())
  {
    CalCoreMorphAnimation* pCoreMorphAnimation = 
                   m_pModel->getCoreModel()->getCoreMorphAnimation(morphAnimationID);
    std::vector<int>& vectorCoreMeshID = pCoreMorphAnimation->getVectorCoreMeshID();
    std::vector<int>& vectorMorphTargetID = pCoreMorphAnimation->getVectorMorphTargetID();
    size_t meshIterator = 0;
    while(meshIterator<vectorCoreMeshID.size())
    {
       std::vector<CalSubmesh *> &vectorSubmesh = 
             m_pModel->getMesh(vectorCoreMeshID[meshIterator])->getVectorSubmesh();
       int submeshCount = vectorSubmesh.size();
       int submeshId;
       for(submeshId=0;submeshId<submeshCount;++submeshId)
       {
         vectorSubmesh[submeshId]->setMorphTargetWeight 
                  (vectorMorphTargetID[meshIterator],m_vectorCurrentWeight[morphAnimationID]);
       }
       ++meshIterator;
    }
    ++morphAnimationID;
  }
}

 /*****************************************************************************/
/** Returns the number of morph targets this morph target mixer mixes.
  *
  * @return The number of morph targets this morph target mixer mixes.
  *****************************************************************************/

int CalMorphTargetMixer::getMorphTargetCount()
{
  return m_vectorCurrentWeight.size();
}

//****************************************************************************//
