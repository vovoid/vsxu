//****************************************************************************//
// calskeleton.cpp                                                            //
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

#include "cal3d/skeleton.h"
#include "cal3d/error.h"
#include "cal3d/bone.h"
#include "cal3d/coreskeleton.h"
#include "cal3d/coremodel.h"
#include "cal3d/corebone.h" // DEBUG

 /*****************************************************************************/
/** Constructs the skeleton instance.
  *
  * This function is the default constructor of the skeleton instance.
  *****************************************************************************/

CalSkeleton::CalSkeleton(CalCoreSkeleton* pCoreSkeleton)
  : m_pCoreSkeleton(0)
  , m_isBoundingBoxesComputed(false)
{
  assert(pCoreSkeleton);
  m_pCoreSkeleton = pCoreSkeleton;

  // clone the skeleton structure of the core skeleton
  std::vector<CalCoreBone *>& vectorCoreBone = pCoreSkeleton->getVectorCoreBone();

  // get the number of bones
  int boneCount = vectorCoreBone.size();

  // reserve space in the bone vector
  m_vectorBone.reserve(boneCount);

  // clone every core bone
  for(int boneId = 0; boneId < boneCount; ++boneId)
  {
    CalBone *pBone = new CalBone(vectorCoreBone[boneId]);

    // set skeleton in the bone instance
    pBone->setSkeleton(this);

    // insert bone into bone vector
    m_vectorBone.push_back(pBone);
  }
}

 /*****************************************************************************/
/** Destructs the skeleton instance.
  *
  * This function is the destructor of the skeleton instance.
  *****************************************************************************/

CalSkeleton::~CalSkeleton()
{
  // destroy all bones
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    delete (*iteratorBone);
  }
}

 /*****************************************************************************/
/** Calculates the state of the skeleton instance.
  *
  * This function calculates the state of the skeleton instance by recursively
  * calculating the states of its bones.
  *****************************************************************************/

void CalSkeleton::calculateState()
{
  // calculate all bone states of the skeleton
  std::list<int>& listRootCoreBoneId = m_pCoreSkeleton->getListRootCoreBoneId();

  std::list<int>::iterator iteratorRootBoneId;
  for(iteratorRootBoneId = listRootCoreBoneId.begin(); iteratorRootBoneId != listRootCoreBoneId.end(); ++iteratorRootBoneId)
  {
    m_vectorBone[*iteratorRootBoneId]->calculateState();
  }
  m_isBoundingBoxesComputed=false;
}

 /*****************************************************************************/
/** Clears the state of the skeleton instance.
  *
  * This function clears the state of the skeleton instance by recursively
  * clearing the states of its bones.
  *****************************************************************************/

void CalSkeleton::clearState()
{
  // clear all bone states of the skeleton
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    (*iteratorBone)->clearState();
  }
  m_isBoundingBoxesComputed=false;
}


 /*****************************************************************************/
/** Provides access to a bone.
  *
  * This function returns the bone with the given ID.
  *
  * @param boneId The ID of the bone that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the bone
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalBone *CalSkeleton::getBone(int boneId) const
{
  return m_vectorBone[boneId];
}

 /*****************************************************************************/
/** Provides access to the core skeleton.
  *
  * This function returns the core skeleton on which this skeleton instance is
  * based on.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreSkeleton *CalSkeleton::getCoreSkeleton() const
{
  return m_pCoreSkeleton;
}

 /*****************************************************************************/
/** Returns the bone vector.
  *
  * This function returns the vector that contains all bones of the skeleton
  * instance.
  *
  * @return A reference to the bone vector.
  *****************************************************************************/

std::vector<CalBone *>& CalSkeleton::getVectorBone()
{
  return m_vectorBone;
}

 /*****************************************************************************/
/** Locks the state of the skeleton instance.
  *
  * This function locks the state of the skeleton instance by recursively
  * locking the states of its bones.
  *****************************************************************************/

void CalSkeleton::lockState()
{
  // lock all bone states of the skeleton
  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    (*iteratorBone)->lockState();
  }
}

/*****************************************************************************/
/** Calculates axis aligned bounding box of skeleton bones
  *
  * @param min The vector where the min values of bb are stored.
  * @param man The vector where the max values of bb are stored.
  *
  *****************************************************************************/

void CalSkeleton::getBoneBoundingBox(float *min, float *max)
{
  if(!m_isBoundingBoxesComputed)
  {
	  calculateBoundingBoxes();
  }


  std::vector<CalBone *>::iterator iteratorBone;

  if ((iteratorBone = m_vectorBone.begin()) != m_vectorBone.end()) {
    const CalVector& translation = (*iteratorBone)->getTranslationAbsolute();

    min[0] = max[0] = translation[0];
    min[1] = max[1] = translation[1];
    min[2] = max[2] = translation[2];

    ++iteratorBone;
  }

  for(; iteratorBone != m_vectorBone.end(); ++iteratorBone) {
    const CalVector& translation = (*iteratorBone)->getTranslationAbsolute();

    if (translation[0] > max[0])
      max[0] = translation[0];
    else if (translation[0] < min[0])
      min[0] = translation[0];

    if (translation[1] > max[1])
      max[1] = translation[1];
    else if (translation[1] < min[1])
      min[1] = translation[1];

    if (translation[2] > max[2])
      max[2] = translation[2];
    else if (translation[2] < min[2])
      min[2] = translation[2];

  }

}
 /*****************************************************************************/
/** Calculates bounding boxes.
  *
  * This function Calculates the bounding box of every bone in the Skeleton.
  *
  *****************************************************************************/


void CalSkeleton::calculateBoundingBoxes()
{
   if(m_isBoundingBoxesComputed) 
	   return;

   for(size_t boneId=0;boneId<m_vectorBone.size();++boneId)
   {
      m_vectorBone[boneId]->calculateBoundingBox();
   }
   m_isBoundingBoxesComputed=true;

}


//****************************************************************************//



//****************************************************************************//
//****************************************************************************//
//****************************************************************************//
// DEBUG-/TEST-CODE                                                           //
//****************************************************************************//
//****************************************************************************//
//****************************************************************************//

int CalSkeleton::getBonePoints(float *pPoints)
{
  int nrPoints;
  nrPoints = 0;

  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    const CalVector& translation = (*iteratorBone)->getTranslationAbsolute();

    *pPoints++ = translation[0];
    *pPoints++ = translation[1];
    *pPoints++ = translation[2];

    nrPoints++;
  }

  return nrPoints;
}

int CalSkeleton::getBonePointsStatic(float *pPoints)
{
  int nrPoints;
  nrPoints = 0;

  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    const CalVector& translation = (*iteratorBone)->getCoreBone()->getTranslationAbsolute();

    *pPoints++ = translation[0];
    *pPoints++ = translation[1];
    *pPoints++ = translation[2];

    nrPoints++;
  }

  return nrPoints;
}

int CalSkeleton::getBoneLines(float *pLines)
{
  int nrLines;
  nrLines = 0;

  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    int parentId;
    parentId = (*iteratorBone)->getCoreBone()->getParentId();

    if(parentId != -1)
    {
      CalBone *pParent;
      pParent = m_vectorBone[parentId];

      const CalVector& translation = (*iteratorBone)->getTranslationAbsolute();
      const CalVector& translationParent = pParent->getTranslationAbsolute();

      *pLines++ = translationParent[0];
      *pLines++ = translationParent[1];
      *pLines++ = translationParent[2];

      *pLines++ = translation[0];
      *pLines++ = translation[1];
      *pLines++ = translation[2];

      nrLines++;
    }
  }

  return nrLines;
}

int CalSkeleton::getBoneLinesStatic(float *pLines)
{
  int nrLines;
  nrLines = 0;

  std::vector<CalBone *>::iterator iteratorBone;
  for(iteratorBone = m_vectorBone.begin(); iteratorBone != m_vectorBone.end(); ++iteratorBone)
  {
    int parentId;
    parentId = (*iteratorBone)->getCoreBone()->getParentId();

    if(parentId != -1)
    {
      CalBone *pParent;
      pParent = m_vectorBone[parentId];

      const CalVector& translation = (*iteratorBone)->getCoreBone()->getTranslationAbsolute();
      const CalVector& translationParent = pParent->getCoreBone()->getTranslationAbsolute();

      *pLines++ = translationParent[0];
      *pLines++ = translationParent[1];
      *pLines++ = translationParent[2];

      *pLines++ = translation[0];
      *pLines++ = translation[1];
      *pLines++ = translation[2];

      nrLines++;
    }
  }

  return nrLines;
}

//****************************************************************************//
//****************************************************************************//
//****************************************************************************//
// END DEBUG-/TEST-CODE                                                       //
//****************************************************************************//
//****************************************************************************//
//****************************************************************************//
