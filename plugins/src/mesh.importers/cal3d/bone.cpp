//****************************************************************************//
// bone.cpp                                                                   //
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


#include "cal3d/error.h"
#include "cal3d/bone.h"
#include "cal3d/coremodel.h"
#include "cal3d/coremesh.h"
#include "cal3d/coresubmesh.h"
#include "cal3d/corebone.h"
#include "cal3d/skeleton.h"
#include "cal3d/coreskeleton.h"


CalBone::CalBone(CalCoreBone* coreBone)
  : m_pSkeleton(0)
{
  assert(coreBone);
  m_pCoreBone = coreBone;
  clearState();
}


 /*****************************************************************************/
/** Interpolates the current state to another state.
  *
  * This function interpolates the current state (relative translation and
  * rotation) of the bone instance to another state of a given weight.
  *
  * @param weight The blending weight.
  * @param translation The relative translation to be interpolated to.
  * @param rotation The relative rotation to be interpolated to.
  *****************************************************************************/

void CalBone::blendState(float weight, const CalVector& translation, const CalQuaternion& rotation)
{
  if(m_accumulatedWeightAbsolute == 0.0f)
  {
    // it is the first state, so we can just copy it into the bone state
    m_translationAbsolute = translation;
    m_rotationAbsolute = rotation;

    m_accumulatedWeightAbsolute = weight;
  }
  else
  {
    // it is not the first state, so blend all attributes
    float factor;
    factor = weight / (m_accumulatedWeightAbsolute + weight);

    m_translationAbsolute.blend(factor, translation);
    m_rotationAbsolute.blend(factor, rotation);

    m_accumulatedWeightAbsolute += weight;
  }
}

 /*****************************************************************************/
/** Calculates the current state.
  *
  * This function calculates the current state (absolute translation and
  * rotation, as well as the bone space transformation) of the bone instance
  * and all its children.
  *****************************************************************************/

void CalBone::calculateState()
{
  // check if the bone was not touched by any active animation
  if(m_accumulatedWeight == 0.0f)
  {
    // set the bone to the initial skeleton state
    m_translation = m_pCoreBone->getTranslation();
    m_rotation = m_pCoreBone->getRotation();
  }

  // get parent bone id
  int parentId;
  parentId = m_pCoreBone->getParentId();

  if(parentId == -1)
  {
    // no parent, this means absolute state == relative state
    m_translationAbsolute = m_translation;
    m_rotationAbsolute = m_rotation;
  }
  else
  {
    // get the parent bone
    CalBone *pParent;
    pParent = m_pSkeleton->getBone(parentId);

    // transform relative state with the absolute state of the parent
    m_translationAbsolute = m_translation;
    m_translationAbsolute *= pParent->getRotationAbsolute();
    m_translationAbsolute += pParent->getTranslationAbsolute();

    m_rotationAbsolute = m_rotation;
    m_rotationAbsolute *= pParent->getRotationAbsolute();
  }

  // calculate the bone space transformation
  m_translationBoneSpace = m_pCoreBone->getTranslationBoneSpace();
  m_translationBoneSpace *= m_rotationAbsolute;
  m_translationBoneSpace += m_translationAbsolute;

  m_rotationBoneSpace = m_pCoreBone->getRotationBoneSpace();
  m_rotationBoneSpace *= m_rotationAbsolute;

  // Generate the vertex transform.  If I ever add support for bone-scaling
  // to Cal3D, this step will become significantly more complex.
  m_transformMatrix = m_rotationBoneSpace;

  // calculate all child bones
  std::list<int>::iterator iteratorChildId;
  for(iteratorChildId = m_pCoreBone->getListChildId().begin(); iteratorChildId != m_pCoreBone->getListChildId().end(); ++iteratorChildId)
  {
    m_pSkeleton->getBone(*iteratorChildId)->calculateState();
  }
}

 /*****************************************************************************/
/** Clears the current state.
  *
  * This function clears the current state (absolute translation and rotation)
  * of the bone instance and all its children.
  *****************************************************************************/

void CalBone::clearState()
{
  m_accumulatedWeight = 0.0f;
  m_accumulatedWeightAbsolute = 0.0f;
}


 /*****************************************************************************/
/** Provides access to the core bone.
  *
  * This function returns the core bone on which this bone instance is based on.
  *
  * @return One of the following values:
  *         \li a pointer to the core bone
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreBone *CalBone::getCoreBone()
{
  return m_pCoreBone;
}

 /*****************************************************************************/
/** Resets the bone to its core state
  *
  * This function changes the state of the bone to its default non-animated
  * position and orientation. Child bones are unaffected and may be animated
  * independently. 
  *****************************************************************************/

void CalBone::setCoreState()
{
   // set the bone to the initial skeleton state
   m_translation = m_pCoreBone->getTranslation();
   m_rotation = m_pCoreBone->getRotation();

   // set the appropriate weights
   m_accumulatedWeightAbsolute = 1.0f;
   m_accumulatedWeight = 1.0f ;

   calculateState() ;
}


 /*****************************************************************************/
/** Resets the bone and children to core states
  *
  * This function changes the state of the bone to its default non-animated
  * position and orientation. All child bones are also set in this manner.
  *****************************************************************************/

void CalBone::setCoreStateRecursive()
{
  // set the bone to the initial skeleton state
  m_translation = m_pCoreBone->getTranslation();
  m_rotation = m_pCoreBone->getRotation();

  // set the appropriate weights
  m_accumulatedWeightAbsolute = 1.0f;
  m_accumulatedWeight = 1.0f ;

  // set core state for all child bones
  std::list<int>::iterator iteratorChildId;
  for(iteratorChildId = m_pCoreBone->getListChildId().begin(); iteratorChildId != m_pCoreBone->getListChildId().end(); ++iteratorChildId)
  {
    m_pSkeleton->getBone(*iteratorChildId)->setCoreStateRecursive();
  }

  calculateState() ;
}

 /*****************************************************************************/
/** Sets the current rotation.
  *
  * This function sets the current relative rotation of the bone instance.
  * Caveat: For this change to appear, calculateState() must be called 
  * afterwards.
  *****************************************************************************/

void CalBone::setRotation(const CalQuaternion& rotation)
{
  m_rotation = rotation;
  m_accumulatedWeightAbsolute = 1.0f;
  m_accumulatedWeight = 1.0f ;
}

 /*****************************************************************************/
/** Returns the current rotation.
  *
  * This function returns the current relative rotation of the bone instance.
  *
  * @return The relative rotation to the parent as quaternion.
  *****************************************************************************/

const CalQuaternion& CalBone::getRotation()
{
  return m_rotation;
}

 /*****************************************************************************/
/** Returns the current absolute rotation.
  *
  * This function returns the current absolute rotation of the bone instance.
  *
  * @return The absolute rotation to the parent as quaternion.
  *****************************************************************************/

const CalQuaternion& CalBone::getRotationAbsolute()
{
  return m_rotationAbsolute;
}

 /*****************************************************************************/
/** Returns the current bone space rotation.
  *
  * This function returns the current rotation to bring a point into the bone
  * instance space.
  *
  * @return The rotation to bring a point into bone space.
  *****************************************************************************/

const CalQuaternion& CalBone::getRotationBoneSpace()
{
  return m_rotationBoneSpace;
}

 /*****************************************************************************/
/** Sets the current translation.
  *
  * This function sets the current relative translation of the bone instance.
  * Caveat: For this change to appear, calculateState() must be called 
  * afterwards.
  *****************************************************************************/

void CalBone::setTranslation(const CalVector& translation)
{
  m_translation = translation;
  m_accumulatedWeightAbsolute = 1.0f;
  m_accumulatedWeight = 1.0f ;
}

 /*****************************************************************************/
/** Returns the current translation.
  *
  * This function returns the current relative translation of the bone instance.
  *
  * @return The relative translation to the parent as quaternion.
  *****************************************************************************/

const CalVector& CalBone::getTranslation()
{
  return m_translation;
}

 /*****************************************************************************/
/** Returns the current absolute translation.
  *
  * This function returns the current absolute translation of the bone instance.
  *
  * @return The absolute translation to the parent as quaternion.
  *****************************************************************************/

const CalVector& CalBone::getTranslationAbsolute()
{
  return m_translationAbsolute;
}

 /*****************************************************************************/
/** Returns the current bone space translation.
  *
  * This function returns the current translation to bring a point into the
  *bone instance space.
  *
  * @return The translation to bring a point into bone space.
  *****************************************************************************/

const CalVector& CalBone::getTranslationBoneSpace()
{
  return m_translationBoneSpace;
}

 /*****************************************************************************/
/** Returns the current bone space translation.
  *
  * This function returns the current translation to bring a point into the
  *bone instance space.
  *
  * @return The translation to bring a point into bone space.
  *****************************************************************************/

const CalMatrix& CalBone::getTransformMatrix()
{
  return m_transformMatrix;
}


 /*****************************************************************************/
/** Locks the current state.
  *
  * This function locks the current state (absolute translation and rotation)
  * of the bone instance and all its children.
  *****************************************************************************/

void CalBone::lockState()
{
  // clamp accumulated weight
  if(m_accumulatedWeightAbsolute > 1.0f - m_accumulatedWeight)
  {
    m_accumulatedWeightAbsolute = 1.0f - m_accumulatedWeight;
  }

  if(m_accumulatedWeightAbsolute > 0.0f)
  {
    if(m_accumulatedWeight == 0.0f)
    {
      // it is the first state, so we can just copy it into the bone state
      m_translation = m_translationAbsolute;
      m_rotation = m_rotationAbsolute;

      m_accumulatedWeight = m_accumulatedWeightAbsolute;
    }
    else
    {
      // it is not the first state, so blend all attributes
      float factor;
      factor = m_accumulatedWeightAbsolute / (m_accumulatedWeight + m_accumulatedWeightAbsolute);

      m_translation.blend(factor, m_translationAbsolute);
      m_rotation.blend(factor, m_rotationAbsolute);

      m_accumulatedWeight += m_accumulatedWeightAbsolute;
    }

    m_accumulatedWeightAbsolute = 0.0f;
  }
}

 /*****************************************************************************/
/** Sets the skeleton.
  *
  * This function sets the skeleton to which the bone instance is attached to.
  *
  * @param pSkeleton The skeleton to which the bone instance should be
  *                  attached to.
  *****************************************************************************/

void CalBone::setSkeleton(CalSkeleton *pSkeleton)
{
  m_pSkeleton = pSkeleton;
}

 /*****************************************************************************/
/** Calculates the bounding box.
  *
  * This function Calculates the bounding box of the bone instance.
  *
  *****************************************************************************/


void CalBone::calculateBoundingBox()
{
   if(!getCoreBone()->isBoundingBoxPrecomputed())
	   return;

   CalVector dir = CalVector(1.0f,0.0f,0.0f);
   dir*=getTransformMatrix();
   m_boundingBox.plane[0].setNormal(dir);

   dir = CalVector(-1.0f,0.0f,0.0f);
   dir*=getTransformMatrix();
   m_boundingBox.plane[1].setNormal(dir);

   dir = CalVector(0.0f,1.0f,0.0f);
   dir*=getTransformMatrix();
   m_boundingBox.plane[2].setNormal(dir);

   dir = CalVector(0.0f,-1.0f,0.0f);
   dir*=getTransformMatrix();
   m_boundingBox.plane[3].setNormal(dir);

   dir = CalVector(0.0f,0.0f,1.0f);
   dir*=getTransformMatrix();
   m_boundingBox.plane[4].setNormal(dir);

   dir = CalVector(0.0f,0.0f,-1.0f);
   dir*=getTransformMatrix();
   m_boundingBox.plane[5].setNormal(dir);
   
   int i;
   
   for(i=0;i< 6; i++)
   {
       CalVector position;
       getCoreBone()->getBoundingData(i,position);
      
       position*=getTransformMatrix();
       position+=getTranslationBoneSpace();

       int planeId;
       for(planeId = 0; planeId < 6; ++planeId)
       {
          if(m_boundingBox.plane[planeId].eval(position) < 0.0f)
          {
             m_boundingBox.plane[planeId].setPosition(position);
          }
       }
       
   }
}

 /*****************************************************************************/
/** Returns the current bounding box.
  *
  * This function returns the current bounding box of the bone instance.
  *
  * @return bounding box.
  *****************************************************************************/


CalBoundingBox & CalBone::getBoundingBox()
{
   return m_boundingBox;
}


//****************************************************************************//
