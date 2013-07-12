//****************************************************************************//
// springsystem.cpp                                                           //
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
#include "cal3d/springsystem.h"
#include "cal3d/coremodel.h"
#include "cal3d/model.h"
#include "cal3d/mesh.h"
#include "cal3d/submesh.h"
#include "cal3d/skeleton.h"
#include "cal3d/bone.h"
#include "cal3d/coresubmesh.h"
#include "cal3d/vector.h"

 /*****************************************************************************/
/** Constructs the spring system instance.
  *
  * This function is the default constructor of the spring system instance.
  *****************************************************************************/

CalSpringSystem::CalSpringSystem(CalModel* pModel)
{
  assert(pModel);
  m_pModel = pModel;
  
  m_vGravity = CalVector(0.0f, 0.0f, -98.1f);
  // We add this force to simulate some movement
  m_vForce = CalVector(0.0f, 0.5f, 0.0f);
  m_collision=false;
}


 /*****************************************************************************/
/** Calculates the forces on each unbound vertex.
  *
  * This function calculates the forces on each unbound vertex of a specific
  * submesh.
  *
  * @param pSubmesh A pointer to the submesh from which the forces should be
  *                 calculated.
  * @param deltaTime The elapsed time in seconds since the last calculation.
  *****************************************************************************/

void CalSpringSystem::calculateForces(CalSubmesh *pSubmesh, float deltaTime)
{
  // get the vertex vector of the submesh
  std::vector<CalVector>& vectorVertex = pSubmesh->getVectorVertex();

  // get the vertex vector of the submesh
  std::vector<CalSubmesh::PhysicalProperty>& vectorPhysicalProperty = pSubmesh->getVectorPhysicalProperty();

  // get the physical property vector of the core submesh
  std::vector<CalCoreSubmesh::PhysicalProperty>& vectorCorePhysicalProperty = pSubmesh->getCoreSubmesh()->getVectorPhysicalProperty();

  // loop through all the vertices
  int vertexId;
  for(vertexId = 0; vertexId < (int)vectorVertex.size(); ++vertexId)
  {
    // get the physical property of the vertex
    CalSubmesh::PhysicalProperty& physicalProperty = vectorPhysicalProperty[vertexId];

    // get the physical property of the core vertex
    CalCoreSubmesh::PhysicalProperty& corePhysicalProperty = vectorCorePhysicalProperty[vertexId];

    // only take vertices with a weight > 0 into account
    if(corePhysicalProperty.weight > 0.0f)
    {
		physicalProperty.force = m_vForce + m_vGravity * corePhysicalProperty.weight;		
    }
  }
}

 /*****************************************************************************/
/** Calculates the vertices influenced by the spring system instance.
  *
  * This function calculates the vertices influenced by the spring system
  * instance.
  *
  * @param pSubmesh A pointer to the submesh from which the vertices should be
  *                 calculated.
  * @param deltaTime The elapsed time in seconds since the last calculation.
  *****************************************************************************/

void CalSpringSystem::calculateVertices(CalSubmesh *pSubmesh, float deltaTime)
{
  // get the vertex vector of the submesh
  std::vector<CalVector>& vectorVertex = pSubmesh->getVectorVertex();

  // get the physical property vector of the submesh
  std::vector<CalSubmesh::PhysicalProperty>& vectorPhysicalProperty = pSubmesh->getVectorPhysicalProperty();

  // get the physical property vector of the core submesh
  std::vector<CalCoreSubmesh::PhysicalProperty>& vectorCorePhysicalProperty = pSubmesh->getCoreSubmesh()->getVectorPhysicalProperty();

  // loop through all the vertices
  int vertexId;
  for(vertexId = 0; vertexId < (int)vectorVertex.size(); ++vertexId)
  {
    // get the vertex
    CalVector& vertex = vectorVertex[vertexId];

    // get the physical property of the vertex
    CalSubmesh::PhysicalProperty& physicalProperty = vectorPhysicalProperty[vertexId];

    // get the physical property of the core vertex
    CalCoreSubmesh::PhysicalProperty& corePhysicalProperty = vectorCorePhysicalProperty[vertexId];

    // store current position for later use
    CalVector position;
    position = physicalProperty.position;

    // only take vertices with a weight > 0 into account
    if(corePhysicalProperty.weight > 0.0f)
    {
      // do the Verlet step
      physicalProperty.position += (position - physicalProperty.positionOld) * 0.99f + physicalProperty.force / corePhysicalProperty.weight * deltaTime * deltaTime;

		CalSkeleton *pSkeleton = m_pModel->getSkeleton();
		
		if(m_collision)
		{
			std::vector<CalBone *> &m_vectorbone =  pSkeleton->getVectorBone();
			
			unsigned long boneId;
			for(boneId=0; boneId < m_vectorbone.size(); boneId++)
			{
				CalBoundingBox p = m_vectorbone[boneId]->getBoundingBox();
				bool in=true;
				float min=1e10;
				int index=-1;
				
				int faceId;
				for(faceId=0; faceId < 6 ; faceId++)
				{				
					if(p.plane[faceId].eval(physicalProperty.position)<=0)
					{
						in=false;
					}
					else
					{
						float dist=p.plane[faceId].dist(physicalProperty.position);
						if(dist<min)
						{
							index=faceId;
							min=dist;
						}
					}
				}
				
				if(in && index!=-1)
				{
					CalVector normal = CalVector(p.plane[index].a,p.plane[index].b,normal.z = p.plane[index].c);
					normal.normalize();
					physicalProperty.position = physicalProperty.position - min*normal;
				}
				
				in=true;
				
				for(faceId=0; faceId < 6 ; faceId++)
				{				
					if(p.plane[faceId].eval(physicalProperty.position) < 0 )
					{
						in=false;				
					}
				}
				if(in)
				{
					physicalProperty.position = vectorVertex[vertexId];
				}
			}
		}

    }
    else
    {
      physicalProperty.position = vectorVertex[vertexId];
    }

    // make the current position the old one
    physicalProperty.positionOld = position;

    // set the new position of the vertex
    vertex = physicalProperty.position;

    // clear the accumulated force on the vertex
    physicalProperty.force.clear();
  }

  // get the spring vector of the core submesh
  std::vector<CalCoreSubmesh::Spring>& vectorSpring = pSubmesh->getCoreSubmesh()->getVectorSpring();

  // iterate a few times to relax the constraints
  int iterationCount;
#define ITERATION_COUNT 2
  for(iterationCount = 0; iterationCount < ITERATION_COUNT; ++iterationCount)
  {
    // loop through all the springs
    std::vector<CalCoreSubmesh::Spring>::iterator iteratorSpring;
    for(iteratorSpring = vectorSpring.begin(); iteratorSpring != vectorSpring.end(); ++iteratorSpring)
    {
      // get the spring
      CalCoreSubmesh::Spring& spring = *iteratorSpring;

      // compute the difference between the two spring vertices
      CalVector distance;
      distance = vectorVertex[spring.vertexId[1]] - vectorVertex[spring.vertexId[0]];

      // get the current length of the spring
      float length;
      length = distance.length();

      if(length > 0.0f)
      {
      	/*if (spring.springCoefficient == 0)
      	{ 
      	 	vectorVertex[spring.vertexId[1]] = vectorVertex[spring.vertexId[0]];  
      	 	vectorPhysicalProperty[spring.vertexId[1]].position = vectorVertex[spring.vertexId[0]]; 
      	} 
      	else
	{*/
	   float factor[2];
	   factor[0] = (length - spring.idleLength) / length;
	   factor[1] = factor[0];
	   
	   if(vectorCorePhysicalProperty[spring.vertexId[0]].weight > 0.0f)
	   {
              factor[0] /= 2.0f;
              factor[1] /= 2.0f;
           }
           else
           {
             factor[0] = 0.0f;
           }
           
           if(vectorCorePhysicalProperty[spring.vertexId[1]].weight <= 0.0f)
           {
              factor[0] *= 2.0f;
              factor[1] = 0.0f;
           }

           vectorVertex[spring.vertexId[0]] += distance * factor[0];
           vectorPhysicalProperty[spring.vertexId[0]].position = vectorVertex[spring.vertexId[0]];

           vectorVertex[spring.vertexId[1]] -= distance * factor[1];
           vectorPhysicalProperty[spring.vertexId[1]].position = vectorVertex[spring.vertexId[1]];
        //}
      }
    }
  }
/* DEBUG-CODE ********************
  CalVector spherePosition(Sphere.x, Sphere.y, Sphere.z);
  float sphereRadius = Sphere.radius;

  // loop through all the vertices
  for(vertexId = 0; vertexId < (int)vectorVertex.size(); ++vertexId)
  {
    // get the vertex
    CalVector& vertex = vectorVertex[vertexId];

    // get the physical property of the vertex
    CalSubmesh::PhysicalProperty& physicalProperty = vectorPhysicalProperty[vertexId];

    // get the physical property of the core vertex
    CalCoreSubmesh::PhysicalProperty& corePhysicalProperty = vectorCorePhysicalProperty[vertexId];

    // only take vertices with a weight > 0 into account
    if(corePhysicalProperty.weight > 0.0f)
    {
      CalVector position;
      position = physicalProperty.position;
      position -= spherePosition;

      float length;
      length = position.normalize();

      if(length < sphereRadius)
      {
        position *= sphereRadius;
        position += spherePosition;

        physicalProperty.position = position;
        physicalProperty.positionOld = position;
        vertex = physicalProperty.position;
      }
    }
  }
*********************************/
}


 /*****************************************************************************/
/** Updates all the spring systems in the attached meshes.
  *
  * This functon updates all the spring systems in the attached meshes.
  *****************************************************************************/

void CalSpringSystem::update(float deltaTime)
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // loop through all the attached meshes
  std::vector<CalMesh *>::iterator iteratorMesh;
  for(iteratorMesh = vectorMesh.begin(); iteratorMesh != vectorMesh.end(); ++iteratorMesh)
  {
    // get the ssubmesh vector of the mesh
    std::vector<CalSubmesh *>& vectorSubmesh = (*iteratorMesh)->getVectorSubmesh();

    // loop through all the submeshes of the mesh
    std::vector<CalSubmesh *>::iterator iteratorSubmesh;
    for(iteratorSubmesh = vectorSubmesh.begin(); iteratorSubmesh != vectorSubmesh.end(); ++iteratorSubmesh)
    {
      // check if the submesh contains a spring system
      if((*iteratorSubmesh)->getCoreSubmesh()->getSpringCount() > 0 && (*iteratorSubmesh)->hasInternalData())
      {
        // calculate the new forces on each unbound vertex
        calculateForces(*iteratorSubmesh, deltaTime);

        // calculate the vertices influenced by the spring system
        calculateVertices(*iteratorSubmesh, deltaTime);
      }
    }
  }
}


 /*****************************************************************************/
/** Returns the gravity vector.
  *
  * This function returns the gravity vector of the spring system instance.
  *
  * @return the gravity vector as vector.
  *****************************************************************************/

CalVector & CalSpringSystem::getGravityVector()
{
	return m_vGravity;
}

 /*****************************************************************************/
/** Returns the force vector.
  *
  * This function returns the force vector of the spring system instance.
  *
  * @return the force vector as vector.
  *****************************************************************************/


CalVector & CalSpringSystem::getForceVector()
{
	return m_vForce;
}

 /*****************************************************************************/
/** Sets the gravity vector.
  *
  * This function sets the gravity vector of the spring system instance.
  *
  * @param vGravity the gravity vector as vector.
  *****************************************************************************/

void CalSpringSystem::setGravityVector(const CalVector & vGravity)
{
	m_vGravity = vGravity;
}

 /*****************************************************************************/
/** Sets the force vector.
  *
  * This function sets the force vector of the spring system instance.
  *
  * @param vForce the force vector as vector.
  *****************************************************************************/

void CalSpringSystem::setForceVector(const CalVector & vForce)
{
	m_vForce = vForce;
}

 /*****************************************************************************/
/** Enable or disable the collision system
  *
  * @param collision true to enable the collision system else false
  *****************************************************************************/

void CalSpringSystem::setCollisionDetection(bool collision)
{
	m_collision=collision;
}


//****************************************************************************//
