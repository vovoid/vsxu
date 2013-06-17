//****************************************************************************//
// coresubmorphtarget.cpp                                                     //
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


#include "cal3d/coresubmorphtarget.h"


/*****************************************************************************/
/** Returns the blend vertex vector.
  *
  * This function returns the vector that contains all blend vertices of the core
  * sub morph target instance.
  *
  * @return A reference to the blend vertex vector.
  *****************************************************************************/

std::vector<CalCoreSubMorphTarget::BlendVertex>& CalCoreSubMorphTarget::getVectorBlendVertex()
{
  return m_vectorBlendVertex;
}

 /*****************************************************************************/
/** Returns the number of blend vertices.
  *
  * This function returns the number of blend vertices in the 
  * core sub morph target instance.
  *
  * @return The number of blend vertices.
  *****************************************************************************/

int CalCoreSubMorphTarget::getBlendVertexCount()
{
  return m_vectorBlendVertex.size();
}


 /*****************************************************************************/
/** Reserves memory for the blend vertices.
 *
 * This function reserves memory for the blend vertices
 * of the core sub morph target instance.
 *
 * @param blendVertexCount The number of blend vertices that
 *                    this core sub morph target instance should be able to hold.
 *
 * @return One of the following values:
 *         \li \b true if successful
 *         \li \b false if an error happend
 *****************************************************************************/

bool CalCoreSubMorphTarget::reserve(int blendVertexCount)
{
  // reserve the space needed in all the vectors
  m_vectorBlendVertex.reserve(blendVertexCount);
  m_vectorBlendVertex.resize(blendVertexCount);

  return true;
}

 /*****************************************************************************/
/** Sets a specified blend vertex.
  *
  * This function sets a specified blend vertex in the core sub morph target instance.
  *
  * @param vertexId  The ID of the vertex.
  * @param vertex The vertex that should be set.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreSubMorphTarget::setBlendVertex(int blendVertexId, const BlendVertex& blendVertex)
{
  if((blendVertexId < 0) || (blendVertexId >= (int)m_vectorBlendVertex.size())) return false;

  m_vectorBlendVertex[blendVertexId] = blendVertex;

  return true;
}

//****************************************************************************//
