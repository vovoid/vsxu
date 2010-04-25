//****************************************************************************//
// renderer.cpp                                                               //
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
#include "cal3d/renderer.h"
#include "cal3d/coremodel.h"
#include "cal3d/model.h"
#include "cal3d/coremesh.h"
#include "cal3d/mesh.h"
#include "cal3d/submesh.h"
#include "cal3d/skeleton.h"
#include "cal3d/bone.h"
#include "cal3d/corematerial.h"
#include "cal3d/coresubmesh.h"
#include "cal3d/physique.h"

 /*****************************************************************************/
/** Constructs the renderer instance.
  *
  * This function is the default constructor of the renderer instance.
  *****************************************************************************/

CalRenderer::CalRenderer(CalModel* pModel)
  : m_pSelectedSubmesh(0)
{
  assert(pModel);

  m_pModel = pModel;
}

 /*****************************************************************************/
/** Copy-constructor for the renderer instance.
  *
  * This function is the copy constructor of the renderer instance.
  * This is useful for multi-pipe parallel rendering.
  *****************************************************************************/

CalRenderer::CalRenderer(CalRenderer* pRenderer)
{
  m_pModel = pRenderer->m_pModel ;
  m_pSelectedSubmesh = pRenderer->m_pSelectedSubmesh ;
}

 /*****************************************************************************/
/** Initializes the rendering query phase.
  *
  * This function initializes the rendering query phase. It must be called
  * before any rendering queries are executed.
  *****************************************************************************/

bool CalRenderer::beginRendering()
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // check if there are any meshes attached to the model
  if(vectorMesh.size() == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // select the default submesh
  m_pSelectedSubmesh = vectorMesh[0]->getSubmesh(0);
  if(m_pSelectedSubmesh == 0) return false;

  return true;
}

 /*****************************************************************************/
/** Finishes the rendering query phase.
  *
  * This function finishes the rendering query phase. It must be called
  * after all rendering queries have been executed.
  *****************************************************************************/

void CalRenderer::endRendering()
{
  // clear selected submesh
  m_pSelectedSubmesh = 0;
}

 /*****************************************************************************/
/** Provides access to the ambient color.
  *
  * This function returns the ambient color of the material of the selected
  * mesh/submesh.
  *
  * @param pColorBuffer A pointer to the user-provided buffer where the color
  *                     data is written to.
  *****************************************************************************/

void CalRenderer::getAmbientColor(unsigned char *pColorBuffer)
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0)
  {
    // write default values to the color buffer
    pColorBuffer[0] = 0;
    pColorBuffer[1] = 0;
    pColorBuffer[2] = 0;
    pColorBuffer[3] = 0;

    return;
  }

  // get the ambient color of the material
  CalCoreMaterial::Color& color = pCoreMaterial->getAmbientColor();

  // write it to the color buffer
  pColorBuffer[0] = color.red;
  pColorBuffer[1] = color.green;
  pColorBuffer[2] = color.blue;
  pColorBuffer[3] = color.alpha;
}

 /*****************************************************************************/
/** Provides access to the diffuse color.
  *
  * This function returns the diffuse color of the material of the selected
  * mesh/submesh.
  *
  * @param pColorBuffer A pointer to the user-provided buffer where the color
  *                     data is written to.
  *****************************************************************************/

void CalRenderer::getDiffuseColor(unsigned char *pColorBuffer)
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0)
  {
    // write default values to the color buffer
    pColorBuffer[0] = 192;
    pColorBuffer[1] = 192;
    pColorBuffer[2] = 192;
    pColorBuffer[3] = 192;

    return;
  }

  // get the diffuse color of the material
  CalCoreMaterial::Color& color = pCoreMaterial->getDiffuseColor();

  // write it to the color buffer
  pColorBuffer[0] = color.red;
  pColorBuffer[1] = color.green;
  pColorBuffer[2] = color.blue;
  pColorBuffer[3] = color.alpha;
}

 /*****************************************************************************/
/** Returns the number of faces.
  *
  * This function returns the number of faces in the selected mesh/submesh.
  *
  * @return The number of faces.
  *****************************************************************************/

int CalRenderer::getFaceCount()
{
  return m_pSelectedSubmesh->getFaceCount();
}

 /*****************************************************************************/
/** Provides access to the face data.
  *
  * This function returns the face data (vertex indices) of the selected
  * mesh/submesh. The LOD setting is taken into account.
  *
  * @param pFaceBuffer A pointer to the user-provided buffer where the face
  *                    data is written to.
  *
  * @return The number of faces written to the buffer.
  *****************************************************************************/
int CalRenderer::getFaces(CalIndex *pFaceBuffer)
{
  return m_pSelectedSubmesh->getFaces(pFaceBuffer);
}

 /*****************************************************************************/
/** Returns the number of maps.
  *
  * This function returns the number of maps in the selected mesh/submesh.
  *
  * @return The number of maps.
  *****************************************************************************/

int CalRenderer::getMapCount()
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0) return 0;

  return pCoreMaterial->getMapCount();
}

 /*****************************************************************************/
/** Provides access to a specified map user data.
  *
  * This function returns the user data stored in the specified map of the
  * material of the selected mesh/submesh.
  *
  * @param mapId The ID of the map.
  *
  * @return One of the following values:
  *         \li the user data stored in the specified map
  *         \li \b 0 if an error happend
  *****************************************************************************/

Cal::UserData CalRenderer::getMapUserData(int mapId)
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0) return 0;

  // get the map vector
  std::vector<CalCoreMaterial::Map>& vectorMap = pCoreMaterial->getVectorMap();

  // check if the map id is valid
  if((mapId < 0) || (mapId >= (int)vectorMap.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return vectorMap[mapId].userData;
}

 /*****************************************************************************/
/** Returns the number of attached meshes.
  *
  * This function returns the number of meshes attached to the renderer
  * instance.
  *
  * @return The number of attached meshes.
  *****************************************************************************/

int CalRenderer::getMeshCount()
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  return vectorMesh.size();
}



 /*****************************************************************************/
/** Provides access to the tangent space data.
  *
  * This function returns the tangent space data of the selected mesh/submesh.
  *
  * @param mapID
  *
  * @param pTangentSpaceBuffer A pointer to the user-provided buffer where the normal
  *                      data is written to.
  *
  * @return The number of tangent space written to the buffer.
  *****************************************************************************/
int CalRenderer::getTangentSpaces(int mapId, float *pTangentSpaceBuffer, int stride)
{
  // get the texture coordinate vector vector
  std::vector<std::vector<CalCoreSubmesh::TangentSpace> >& vectorvectorTangentSpace = m_pSelectedSubmesh->getCoreSubmesh()->getVectorVectorTangentSpace();
  
  // check if the map id is valid
  if((mapId < 0) || (mapId >= (int)vectorvectorTangentSpace.size()) || !m_pSelectedSubmesh->isTangentsEnabled(mapId))
  {    
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // check if the submesh handles vertex data internally
  if(m_pSelectedSubmesh->hasInternalData())
  {
    // get the normal vector of the submesh
    std::vector<CalSubmesh::TangentSpace>& vectorTangentSpace = m_pSelectedSubmesh->getVectorVectorTangentSpace()[mapId];

    // get the number of normals (= number of vertices) in the submesh
    int tangentSpaceCount;
    tangentSpaceCount = m_pSelectedSubmesh->getVertexCount();

    // copy the internal normal data to the provided normal buffer
    
	if(stride == sizeof(CalCoreSubmesh::TangentSpace) || stride <= 0)
	{
		memcpy(pTangentSpaceBuffer, &vectorTangentSpace[0], tangentSpaceCount * sizeof(CalCoreSubmesh::TangentSpace));	
	}
	else
	{
		char * pBuffer = (char*) pTangentSpaceBuffer;
		for(int i=0; i < tangentSpaceCount; ++i)
		{
			memcpy(&pBuffer[0], &vectorTangentSpace[i], sizeof(CalCoreSubmesh::TangentSpace));
			pBuffer+=stride;
		}
	}

    return tangentSpaceCount;
  }

  // submesh does not handle the vertex data internally, so let the physique calculate it now
  return m_pModel->getPhysique()->calculateTangentSpaces(m_pSelectedSubmesh, mapId, pTangentSpaceBuffer, stride);
}


 /*****************************************************************************/
/** Provides access to the normal data.
  *
  * This function returns the normal data of the selected mesh/submesh.
  *
  * @param pNormalBuffer A pointer to the user-provided buffer where the normal
  *                      data is written to.
  *
  * @return The number of normals written to the buffer.
  *****************************************************************************/

int CalRenderer::getNormals(float *pNormalBuffer, int stride)
{
  // check if the submesh handles vertex data internally
  if(m_pSelectedSubmesh->hasInternalData())
  {
    // get the normal vector of the submesh
    std::vector<CalVector>& vectorNormal = m_pSelectedSubmesh->getVectorNormal();

    // get the number of normals (= number of vertices) in the submesh
    int normalCount;
    normalCount = m_pSelectedSubmesh->getVertexCount();

    // copy the internal normal data to the provided normal buffer

	if(stride == sizeof(CalVector) || stride <= 0)
	{
		memcpy(pNormalBuffer, &vectorNormal[0], normalCount * sizeof(CalVector));
	}
	else
	{
		char * pBuffer = (char*) pNormalBuffer;
		for(int i=0; i < normalCount; ++i)
		{
			memcpy(&pBuffer[0], &vectorNormal[i], sizeof(CalVector));
			pBuffer+=stride;
		}

	}

    return normalCount;
  }

  // submesh does not handle the vertex data internally, so let the physique calculate it now
  return m_pModel->getPhysique()->calculateNormals(m_pSelectedSubmesh, pNormalBuffer, stride);
}

 /*****************************************************************************/
/** Returns the shininess factor.
  *
  * This function returns the shininess factor of the material of the selected
  * mesh/submesh..
  *
  * @return The shininess factor.
  *****************************************************************************/

float CalRenderer::getShininess()
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0) return 50.0f;

  return pCoreMaterial->getShininess();
}

 /*****************************************************************************/
/** Provides access to the specular color.
  *
  * This function returns the specular color of the material of the selected
  * mesh/submesh.
  *
  * @param pColorBuffer A pointer to the user-provided buffer where the color
  *                     data is written to.
  *****************************************************************************/

void CalRenderer::getSpecularColor(unsigned char *pColorBuffer)
{
  // get the core material
  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = m_pModel->getCoreModel()->getCoreMaterial(m_pSelectedSubmesh->getCoreMaterialId());
  if(pCoreMaterial == 0)
  {
    // write default values to the color buffer
    pColorBuffer[0] = 255;
    pColorBuffer[1] = 255;
    pColorBuffer[2] = 255;
    pColorBuffer[3] = 0;

    return;
  }

  // get the specular color of the material
  CalCoreMaterial::Color& color = pCoreMaterial->getSpecularColor();

  // write it to the color buffer
  pColorBuffer[0] = color.red;
  pColorBuffer[1] = color.green;
  pColorBuffer[2] = color.blue;
  pColorBuffer[3] = color.alpha;
}

 /*****************************************************************************/
/** Returns the number of submeshes.
  *
  * This function returns the number of submeshes in a given mesh.
  *
  * @param meshId The ID of the mesh for which the number of submeshes should
  *               be returned..
  *
  * @return The number of submeshes.
  *****************************************************************************/

int CalRenderer::getSubmeshCount(int meshId)
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // check if the mesh id is valid
  if((meshId < 0) || (meshId >= (int)vectorMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return vectorMesh[meshId]->getSubmeshCount();
}

 /*****************************************************************************/
/** Provides access to the texture coordinate data.
  *
  * This function returns the texture coordinate data for a given map of the
  * selected mesh/submesh.
  *
  * @param mapId The ID of the map to get the texture coordinate data from.
  * @param pTextureCoordinateBuffer A pointer to the user-provided buffer where
  *                    the texture coordinate data is written to.
  *
  * @return The number of texture coordinates written to the buffer.
  *****************************************************************************/

int CalRenderer::getTextureCoordinates(int mapId, float *pTextureCoordinateBuffer, int stride)
{
  // get the texture coordinate vector vector
  std::vector<std::vector<CalCoreSubmesh::TextureCoordinate> >& vectorvectorTextureCoordinate = m_pSelectedSubmesh->getCoreSubmesh()->getVectorVectorTextureCoordinate();

  // check if the map id is valid
  if((mapId < 0) || (mapId >= (int)vectorvectorTextureCoordinate.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // get the number of texture coordinates to return
  int textureCoordinateCount;
  textureCoordinateCount = m_pSelectedSubmesh->getVertexCount();

  // copy the texture coordinate vector to the face buffer

  if(stride == sizeof(CalCoreSubmesh::TextureCoordinate) || stride <= 0)
  {
	  memcpy(pTextureCoordinateBuffer, &vectorvectorTextureCoordinate[mapId][0], textureCoordinateCount * sizeof(CalCoreSubmesh::TextureCoordinate));
  }
  else
  {
	  char * pBuffer = (char*) pTextureCoordinateBuffer;
	  for(int i=0; i < textureCoordinateCount; ++i)
	  {
		  memcpy(&pBuffer[0], &vectorvectorTextureCoordinate[mapId][i], sizeof(CalCoreSubmesh::TextureCoordinate));
		  pBuffer+=stride;
	  }
	  
  }

  return textureCoordinateCount;
}

 /*****************************************************************************/
/** Returns the number of vertices.
  *
  * This function returns the number of vertices in the selected mesh/submesh.
  *
  * @return The number of vertices.
  *****************************************************************************/

int CalRenderer::getVertexCount()
{
  return m_pSelectedSubmesh->getVertexCount();
}

 /*****************************************************************************/
/** Returns if tangent are enabled.
  *
  * This function returns if tangent of the current submesh are enabled
  *
  * @return True is tangent is enabled.
  *****************************************************************************/

bool CalRenderer::isTangentsEnabled(int mapId)
{
	return m_pSelectedSubmesh->isTangentsEnabled(mapId);
}

 /*****************************************************************************/
/** Provides access to the vertex data.
  *
  * This function returns the vertex data of the selected mesh/submesh.
  *
  * @param pVertexBuffer A pointer to the user-provided buffer where the vertex
  *                      data is written to.
  *
  * @return The number of vertices written to the buffer.
  *****************************************************************************/

int CalRenderer::getVertices(float *pVertexBuffer, int stride)
{
  // check if the submesh handles vertex data internally
  if(m_pSelectedSubmesh->hasInternalData())
  {
    // get the vertex vector of the submesh
    std::vector<CalVector>& vectorVertex = m_pSelectedSubmesh->getVectorVertex();

    // get the number of vertices in the submesh
    int vertexCount;
    vertexCount = m_pSelectedSubmesh->getVertexCount();

    // copy the internal vertex data to the provided vertex buffer

	if(stride == sizeof(CalVector) || stride <= 0)
	{
		memcpy(pVertexBuffer, &vectorVertex[0], vertexCount * sizeof(CalVector));
	}
	else
	{
		char * pBuffer = (char*) pVertexBuffer;
		for(int i=0; i < vertexCount; ++i)
		{
			memcpy(&pBuffer[0], &vectorVertex[i], sizeof(CalVector));
			pBuffer+=stride;
		}

	}

    return vertexCount;
  }

  // submesh does not handle the vertex data internally, so let the physique calculate it now
  return m_pModel->getPhysique()->calculateVertices(m_pSelectedSubmesh, pVertexBuffer, stride);
}

 /*****************************************************************************/
/** Provides access to the submesh data.
  *
  * This function returns the vertex and normal data of the selected mesh/submesh.
  *
  * @param pVertexBuffer A pointer to the user-provided buffer where the vertex
  *                      and normal data is written to.
  *
  * @return The number of vertex written to the buffer.
  *****************************************************************************/

int CalRenderer::getVerticesAndNormals(float *pVertexBuffer, int stride)
{
  // check if the submesh handles vertex data internally
  if(m_pSelectedSubmesh->hasInternalData())
  {
    // get the vertex vector of the submesh
    std::vector<CalVector>& vectorVertex = m_pSelectedSubmesh->getVectorVertex();
	// get the normal vector of the submesh
    std::vector<CalVector>& vectorNormal = m_pSelectedSubmesh->getVectorNormal();


    // get the number of vertices in the submesh
    int vertexCount;
    vertexCount = m_pSelectedSubmesh->getVertexCount();

	if(stride <= 0)
	{
		stride = 6*sizeof(float);
	}	

    // copy the internal vertex data to the provided vertex buffer
	char * pBuffer = (char*) pVertexBuffer;
	for(int i=0; i < vertexCount; ++i)
	{
		memcpy(&pBuffer[0], &vectorVertex[i], sizeof(CalVector));		
		memcpy(&pBuffer[sizeof(CalVector)], &vectorNormal[i], sizeof(CalVector));
		pBuffer+=stride;
	}

    return vertexCount;
  }

  // submesh does not handle the vertex data internally, so let the physique calculate it now
  return m_pModel->getPhysique()->calculateVerticesAndNormals(m_pSelectedSubmesh, pVertexBuffer, stride);
}

 /*****************************************************************************/
/** Provides access to the submesh data.
  *
  * This function returns the vertex and normal data of the selected mesh/submesh.
  *
  * @param pVertexBuffer A pointer to the user-provided buffer where the vertex
  *                      and normal data is written to.
  *
  * @return The number of vertex written to the buffer.
  *****************************************************************************/


int CalRenderer::getVerticesNormalsAndTexCoords(float *pVertexBuffer,int NumTexCoords)
{  

  // check if the submesh handles vertex data internally
  if(m_pSelectedSubmesh->hasInternalData())
  {
    // get the vertex vector of the submesh
    std::vector<CalVector>& vectorVertex = m_pSelectedSubmesh->getVectorVertex();
	// get the normal vector of the submesh
    std::vector<CalVector>& vectorNormal = m_pSelectedSubmesh->getVectorNormal();	
	// get the texture coordinate vector vector
    std::vector<std::vector<CalCoreSubmesh::TextureCoordinate> >& vectorvectorTextureCoordinate = m_pSelectedSubmesh->getCoreSubmesh()->getVectorVectorTextureCoordinate();

	int TextureCoordinateCount =(int)vectorvectorTextureCoordinate.size();
	
	// check if the map id is valid
    if((NumTexCoords < 0) || (NumTexCoords > TextureCoordinateCount))
	{
	   if(TextureCoordinateCount!=0)
	   {
		   CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
		   return -1;
	   }
	}

    // get the number of vertices in the submesh
    int vertexCount;
    vertexCount = m_pSelectedSubmesh->getVertexCount();

    // copy the internal vertex data to the provided vertex buffer
	
	if(TextureCoordinateCount==0)
	{
		for(int vertexId=0; vertexId < vertexCount; ++vertexId)
		{
			memcpy(&pVertexBuffer[0], &vectorVertex[vertexId], sizeof(CalVector));
			memcpy(&pVertexBuffer[3], &vectorNormal[vertexId], sizeof(CalVector));
			pVertexBuffer+=6+2*NumTexCoords;			
		}
	}
	else
	{
		if(NumTexCoords==1)
		{
			for(int vertexId=0; vertexId < vertexCount; ++vertexId)
			{
				memcpy(&pVertexBuffer[0], &vectorVertex[vertexId], sizeof(CalVector));
				memcpy(&pVertexBuffer[3], &vectorNormal[vertexId], sizeof(CalVector));
				memcpy(&pVertexBuffer[6], &vectorvectorTextureCoordinate[0][vertexId], sizeof(CalCoreSubmesh::TextureCoordinate));
				pVertexBuffer+=8;
			}
		}
	    else
		{
			for(int vertexId=0; vertexId < vertexCount; ++vertexId)
			{
				memcpy(&pVertexBuffer[0], &vectorVertex[vertexId], sizeof(CalVector));			
				memcpy(&pVertexBuffer[3], &vectorNormal[vertexId], sizeof(CalVector));
			    pVertexBuffer+=6;
				for(int mapId=0; mapId < NumTexCoords; ++mapId)
				{
					memcpy(&pVertexBuffer[0], &vectorvectorTextureCoordinate[mapId][vertexId], sizeof(CalCoreSubmesh::TextureCoordinate));
					pVertexBuffer+=2;
				}
			}
		}		
	}

    return vertexCount;
  }
  // submesh does not handle the vertex data internally, so let the physique calculate it now
  return m_pModel->getPhysique()->calculateVerticesNormalsAndTexCoords(m_pSelectedSubmesh, pVertexBuffer, NumTexCoords);
}

 /*****************************************************************************/
/** Selects a mesh/submesh for rendering data queries.
  *
  * This function selects a mesh/submesh for further rendering data queries.
  *
  * @param meshId The ID of the mesh that should be used for further rendering
  *               data queries.
  * @param submeshId The ID of the submesh that should be used for further
  *                  rendering data queries.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalRenderer::selectMeshSubmesh(int meshId, int submeshId)
{
  // get the attached meshes vector
  std::vector<CalMesh *>& vectorMesh = m_pModel->getVectorMesh();

  // check if the mesh id is valid
  if((meshId < 0) || (meshId >= (int)vectorMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the core submesh
  m_pSelectedSubmesh = vectorMesh[meshId]->getSubmesh(submeshId);
  if(m_pSelectedSubmesh == 0) return false;

  return true;
}

 /*****************************************************************************/
/** Sets the normalization flag to true or false.
  *
  * This function sets the normalization flag on or off. If off, the normals
  * calculated by Cal3D will not be normalized. Instead, this transform is left
  * up to the user.
  *****************************************************************************/

void CalRenderer::setNormalization(bool normalize)
{ 
	m_pModel->getPhysique()->setNormalization(normalize); 
}


//****************************************************************************//

