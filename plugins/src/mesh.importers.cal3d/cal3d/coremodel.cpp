//****************************************************************************//
// coremodel.cpp                                                              //
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

#include "cal3d/coremodel.h"
#include "cal3d/error.h"
#include "cal3d/coreskeleton.h"
#include "cal3d/coreanimation.h"
#include "cal3d/coremorphanimation.h"
#include "cal3d/coremesh.h"
#include "cal3d/corematerial.h"
#include "cal3d/loader.h"
#include "cal3d/saver.h"


 /*****************************************************************************/
/** Constructs the core model instance.
  *
  * This function is the default constructor of the core model instance.
  *****************************************************************************/

CalCoreModel::CalCoreModel(const std::string& name)
: m_strName(name)
, m_pCoreSkeleton(0)
, m_userData(0)
{
}

 /*****************************************************************************/
/** Destructs the core model instance.
  *
  * This function is the destructor of the core model instance.
  *****************************************************************************/

CalCoreModel::~CalCoreModel()
{
  // destroy all core materials
  std::vector<CalCoreMaterial *>::iterator iteratorCoreMaterial;
  for(iteratorCoreMaterial = m_vectorCoreMaterial.begin(); iteratorCoreMaterial != m_vectorCoreMaterial.end(); ++iteratorCoreMaterial)
  {
	  if(*iteratorCoreMaterial) 
	  {
		  if((*iteratorCoreMaterial)->decRef())
		  {
			  delete (*iteratorCoreMaterial);
		  }
	  }
  }
  m_vectorCoreMaterial.clear();

  // destroy all core meshes
  std::vector<CalCoreMesh *>::iterator iteratorCoreMesh;
  for(iteratorCoreMesh = m_vectorCoreMesh.begin(); iteratorCoreMesh != m_vectorCoreMesh.end(); ++iteratorCoreMesh)
  {
	  if(*iteratorCoreMesh) 
	  {
		  if((*iteratorCoreMesh)->decRef())
		  {
			  delete (*iteratorCoreMesh);
		  }
	  }
  }
  m_vectorCoreMesh.clear();

  // destroy all core animations
  std::vector<CalCoreAnimation *>::iterator iteratorCoreAnimation;
  for(iteratorCoreAnimation = m_vectorCoreAnimation.begin(); iteratorCoreAnimation != m_vectorCoreAnimation.end(); ++iteratorCoreAnimation)
  {
	  if(*iteratorCoreAnimation) 
	  {
		  if((*iteratorCoreAnimation)->decRef())
		  {
			  delete (*iteratorCoreAnimation);
		  }
	  }
  }
  m_vectorCoreAnimation.clear();

  // destroy all core morph animations
  std::vector<CalCoreMorphAnimation *>::iterator iteratorCoreMorphAnimation;
  for(iteratorCoreMorphAnimation = m_vectorCoreMorphAnimation.begin(); iteratorCoreMorphAnimation != 
      m_vectorCoreMorphAnimation.end(); ++iteratorCoreMorphAnimation)
  {
    delete (*iteratorCoreMorphAnimation);
  }
  m_vectorCoreMorphAnimation.clear();

  if(m_pCoreSkeleton != 0)
  {
    if(m_pCoreSkeleton->decRef())
	{
		delete m_pCoreSkeleton;
	}
	m_pCoreSkeleton = 0;	
  }

  m_strName.erase();
}

 /*****************************************************************************/
/** Adds a core animation.
  *
  * This function adds a core animation to the core model instance.
  *
  * @param pCoreAnimation A pointer to the core animation that should be added.
  *
  * @return One of the following values:
  *         \li the assigned animation \b ID of the added core animation
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::addCoreAnimation(CalCoreAnimation *pCoreAnimation)
{
  // get the id of the core animation
  int animationId;
  animationId = m_vectorCoreAnimation.size();

  m_vectorCoreAnimation.push_back(pCoreAnimation);
  pCoreAnimation->incRef();

  return animationId;
}

 /*****************************************************************************/
/** Adds a core morph animation.
  *
  * This function adds a core morph animation to the core model instance.
  *
  * @param pCoreMorphAnimation A pointer to the core morph animation that 
  *                            should be added.
  *
  * @return One of the following values:
  *         \li the assigned morph animation \b ID of the added core morph animation
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::addCoreMorphAnimation(CalCoreMorphAnimation *pCoreMorphAnimation)
{
  // get the id of the core morph animation
  int morphAnimationId;
  morphAnimationId = m_vectorCoreMorphAnimation.size();

  m_vectorCoreMorphAnimation.push_back(pCoreMorphAnimation);

  return morphAnimationId;
}

 /*****************************************************************************/
/** Adds a core material.
  *
  * This function adds a core material to the core model instance.
  *
  * @param pCoreMaterial A pointer to the core material that should be added.
  *
  * @return One of the following values:
  *         \li the assigned material \b ID of the added core material
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::addCoreMaterial(CalCoreMaterial *pCoreMaterial)
{
  // get the id of the core material
  int materialId;
  materialId = m_vectorCoreMaterial.size();

  m_vectorCoreMaterial.push_back(pCoreMaterial);
  pCoreMaterial->incRef();

  return materialId;
}

 /*****************************************************************************/
/** Adds a core mesh.
  *
  * This function adds a core mesh to the core model instance.
  *
  * @param pCoreMesh A pointer to the core mesh that should be added.
  *
  * @return One of the following values:
  *         \li the assigned mesh \b ID of the added core material
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::addCoreMesh(CalCoreMesh *pCoreMesh)
{
  // get the id of the core mesh
  int meshId;
  meshId = m_vectorCoreMesh.size();

  m_vectorCoreMesh.push_back(pCoreMesh);
  pCoreMesh->incRef();

  return meshId;
}

 /*****************************************************************************/
/** Creates a core material thread.
  *
  * This function creates a new core material thread with the given ID.
  *
  * @param coreMaterialThreadId The ID of the core material thread that should
  *                             be created.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreModel::createCoreMaterialThread(int coreMaterialThreadId)
{
  // insert an empty core material thread with a given id
  std::map<int, int> mapCoreMaterialThreadId;
  m_mapmapCoreMaterialThread.insert(std::make_pair(coreMaterialThreadId, mapCoreMaterialThreadId));

  return true;
}

 /*****************************************************************************/
/** Provides access to a core animation.
  *
  * This function returns the core animation with the given ID.
  *
  * @param coreAnimationId The ID of the core animation that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreAnimation *CalCoreModel::getCoreAnimation(int coreAnimationId)
{
  if((coreAnimationId < 0) || (coreAnimationId >= (int)m_vectorCoreAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return m_vectorCoreAnimation[coreAnimationId];
}

 /*****************************************************************************/
/** Provides access to a core morph animation.
  *
  * This function returns the core morph animation with the given ID.
  *
  * @param coreMorphAnimationId The ID of the core morph animation that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the core morph animation
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreMorphAnimation *CalCoreModel::getCoreMorphAnimation(int coreMorphAnimationId)
{
  if((coreMorphAnimationId < 0) || (coreMorphAnimationId >= (int)m_vectorCoreMorphAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return m_vectorCoreMorphAnimation[coreMorphAnimationId];
}

 /*****************************************************************************/
/** Returns the number of core animations.
  *
  * This function returns the number of core animations in the core model
  * instance.
  *
  * @return The number of core animations.
  *****************************************************************************/

int CalCoreModel::getCoreAnimationCount()
{
  return m_vectorCoreAnimation.size();
}

 /*****************************************************************************/
/** Returns the number of core morph animations.
  *
  * This function returns the number of core morph animations in the core model
  * instance.
  *
  * @return The number of core morph animations.
  *****************************************************************************/

int CalCoreModel::getCoreMorphAnimationCount()
{
  return m_vectorCoreMorphAnimation.size();
}


 /*****************************************************************************/
/** Provides access to a core material.
  *
  * This function returns the core material with the given ID.
  *
  * @param coreMaterialId The ID of the core material that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the core material
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreMaterial *CalCoreModel::getCoreMaterial(int coreMaterialId)
{
  if((coreMaterialId < 0) || (coreMaterialId >= (int)m_vectorCoreMaterial.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return m_vectorCoreMaterial[coreMaterialId];
}

 /*****************************************************************************/
/** Returns the number of core materials.
  *
  * This function returns the number of core materials in the core model
  * instance.
  *
  * @return The number of core materials.
  *****************************************************************************/

int CalCoreModel::getCoreMaterialCount()
{
  return m_vectorCoreMaterial.size();
}

 /*****************************************************************************/
/** Returns a specified core material ID.
  *
  * This function returns the core material ID for a specified core material
  * thread / core material set pair.
  *
  * @param coreMaterialThreadId The ID of the core material thread.
  * @param coreMaterialSetId The ID of the core material set.
  *
  * @return One of the following values:
  *         \li the \b ID of the core material
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::getCoreMaterialId(int coreMaterialThreadId, int coreMaterialSetId)
{
  // find the core material thread
  std::map<int, std::map<int, int> >::iterator iteratorCoreMaterialThread;
  iteratorCoreMaterialThread = m_mapmapCoreMaterialThread.find(coreMaterialThreadId);
  if(iteratorCoreMaterialThread == m_mapmapCoreMaterialThread.end())
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // get the core material thread
  std::map<int, int>& coreMaterialThread = (*iteratorCoreMaterialThread).second;

  // find the material id for the given set
  std::map<int, int>::iterator iteratorSet;
  iteratorSet = coreMaterialThread.find(coreMaterialSetId);
  if(iteratorSet == coreMaterialThread.end())
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  return (*iteratorSet).second;
}

 /*****************************************************************************/
/** Provides access to a core mesh.
  *
  * This function returns the core mesh with the given ID.
  *
  * @param coreMeshId The ID of the core mesh that should be returned.
  *
  * @return One of the following values:
  *         \li a pointer to the core mesh
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreMesh *CalCoreModel::getCoreMesh(int coreMeshId)
{
  if((coreMeshId < 0) || (coreMeshId >= (int)m_vectorCoreMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return 0;
  }

  return m_vectorCoreMesh[coreMeshId];
}

 /*****************************************************************************/
/** Returns the number of core meshes.
  *
  * This function returns the number of core meshes in the core model instance.
  *
  * @return The number of core meshes.
  *****************************************************************************/

int CalCoreModel::getCoreMeshCount()
{
  return m_vectorCoreMesh.size();
}

 /*****************************************************************************/
/** Provides access to the core skeleton.
  *
  * This function returns the core skeleton.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreSkeleton *CalCoreModel::getCoreSkeleton()
{
  return m_pCoreSkeleton;
}

 /*****************************************************************************/
/** Provides access to the user data.
  *
  * This function returns the user data stored in the core model instance.
  *
  * @return The user data stored in the core model instance.
  *****************************************************************************/

Cal::UserData CalCoreModel::getUserData()
{
  return m_userData;
}



int CalCoreModel::loadCoreAnimation(TiXmlDocument &doc)
{
  // the core skeleton has to be loaded already
  if(m_pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // load a new core animation
  CalCoreAnimation *pCoreAnimation = CalLoader::loadXmlCoreAnimation_xml(doc);
  if(pCoreAnimation == 0) return -1;

  // add core animation to this core model
  int animationId = addCoreAnimation(pCoreAnimation);
  if(animationId == -1)
  {
    delete pCoreAnimation;
    return -1;
  }

  return animationId;
}


 /*****************************************************************************/
/** Loads a core animation.
  *
  * This function loads a core animation from a file.
  *
  * @param strFilename The file from which the core animation should be loaded
  *                    from.
  *
  * @return One of the following values:
  *         \li the assigned \b ID of the loaded core animation
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::loadCoreAnimation(const std::string& strFilename)
{
  // the core skeleton has to be loaded already
  if(m_pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // load a new core animation
  CalCoreAnimation *pCoreAnimation = CalLoader::loadCoreAnimation(strFilename);
  if(pCoreAnimation == 0) return -1;

  // add core animation to this core model
  int animationId = addCoreAnimation(pCoreAnimation);
  if(animationId == -1)
  {
    delete pCoreAnimation;
    return -1;
  }

  return animationId;
}

 /*****************************************************************************/
/** Loads a core animation and bind it to a name.
  *
  * This function loads a core animation from a file. It is equivalent
  * to calling addAnimName(strAnimationName, loadCoreAnimation(strFilename)).
  * If strAnimationName is already associated to a coreAnimationId because
  * of a previous call to addAnimName, the same coreAnimationId will
  * be used. 
  *
  * @param strFilename The file from which the core animation should be loaded
  *                    from.
  * @param strAnimationName A string that is associated with an anim ID number.
  *
  * @return One of the following values:
  *         \li the assigned \b ID of the loaded core animation
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::loadCoreAnimation(const std::string& strFilename, const std::string& strAnimationName)
{
  int id = -1;
  std::map<std::string, int>::iterator it = m_animationName.find(strAnimationName);
  if (it != m_animationName.end())
  {
    id=(*it).second;

    // the core skeleton has to be loaded already
    if(m_pCoreSkeleton == NULL)
    {
      CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
      return -1;
    }
    if(m_vectorCoreAnimation[id] != NULL)
    {
      CalError::setLastError(CalError::INDEX_BUILD_FAILED, __FILE__, __LINE__);
      return -1;
    }
    CalCoreAnimation *pCoreAnimation = CalLoader::loadCoreAnimation(strFilename);
    if(pCoreAnimation == NULL) return -1;
    pCoreAnimation->setName(strAnimationName);
    m_vectorCoreAnimation[id] = pCoreAnimation;
    pCoreAnimation->incRef();
  }
  else
  {
    id = loadCoreAnimation(strFilename);
    if(id >= 0)
      addAnimationName(strAnimationName, id);
  }

  return id;
}

 /*****************************************************************************/
/** Delete the resources used by the named core animation. The name must 
  * be associated with a valid core animation Id with the function
  * getAnimationId. The caller must ensure that the corresponding is not
  * referenced anywhere otherwise unpredictable results will occur.
  *
  * @param name The symbolic name of the core animation to unload.
  *
  * @return One of the following values:
  *         \li the core \b ID of the unloaded core animation
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::unloadCoreAnimation(const std::string& name)
{
  int id = getCoreAnimationId(name);
  if(id >= 0)
    return unloadCoreAnimation(id);
  else
    return -1;
}

 /*****************************************************************************/
/** Delete the resources used by a core animation. The caller must
  * ensure that the corresponding is not referenced anywhere otherwise
  * unpredictable results will occur.
  *
  * @param coreAnimationId The ID of the core animation that should be unloaded.
  *
  * @return One of the following values:
  *         \li the core \b ID of the unloaded core animation
  *         \li \b -1 if an error happend
  *****************************************************************************/


int CalCoreModel::unloadCoreAnimation(int coreAnimationId)
{
  if((coreAnimationId < 0) || (coreAnimationId >= (int)m_vectorCoreAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  if(m_vectorCoreAnimation[coreAnimationId]->decRef())
  {
	  delete m_vectorCoreAnimation[coreAnimationId];	  
  }
  m_vectorCoreAnimation[coreAnimationId] = 0;

  return coreAnimationId;
}



int CalCoreModel::loadCoreMaterial(TiXmlDocument &doc)
{
  // the core skeleton has to be loaded already
  if(m_pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // load a new core material
  CalCoreMaterial *pCoreMaterial = CalLoader::loadXmlCoreMaterial_xml(doc);
  if(pCoreMaterial == 0) return -1;

  // add core material to this core model
  int materialId = addCoreMaterial(pCoreMaterial);
  if(materialId == -1)
  {
    delete pCoreMaterial;
    return -1;
  }

  return materialId;
}

 /*****************************************************************************/
/** Loads a core material.
  *
  * This function loads a core material from a file.
  *
  * @param strFilename The file from which the core material should be loaded
  *                    from.
  *
  * @return One of the following values:
  *         \li the assigned \b ID of the loaded core material
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::loadCoreMaterial(const std::string& strFilename)
{
  // the core skeleton has to be loaded already
  if(m_pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // load a new core material
  CalCoreMaterial *pCoreMaterial = CalLoader::loadCoreMaterial(strFilename);
  if(pCoreMaterial == 0) return -1;

  // add core material to this core model
  int materialId = addCoreMaterial(pCoreMaterial);
  if(materialId == -1)
  {
    delete pCoreMaterial;
    return -1;
  }

  return materialId;
}

 /*****************************************************************************/
/** Loads a core material and bind it to a name.
  *
  * This function loads a core material from a file. It is equivalent
  * to calling addMaterialName(strMaterialName, loadCoreMaterial(strFilename)).
  * If strMaterialName is already associated to a coreMaterialId because
  * of a previous call to addMaterialName, the same coreMaterialId will
  * be used. 
  *
  * @param strFilename The file from which the core material should be loaded
  *                    from.
  * @param strMaterialName A string that is associated with an anim ID number.
  *
  * @return One of the following values:
  *         \li the assigned \b ID of the loaded core material
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::loadCoreMaterial(const std::string& strFilename, const std::string& strMaterialName)
{
  int id = -1;
  std::map<std::string, int>::iterator it = m_materialName.find(strMaterialName);
  if (it != m_materialName.end())
  {
    id=(*it).second;

    // the core skeleton has to be loaded already
    if(m_pCoreSkeleton == NULL)
    {
      CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
      return -1;
    }
    if(m_vectorCoreMaterial[id] != NULL)
    {
      CalError::setLastError(CalError::INDEX_BUILD_FAILED, __FILE__, __LINE__);
      return -1;
    }
    CalCoreMaterial *pCoreMaterial = CalLoader::loadCoreMaterial(strFilename);
    if(pCoreMaterial == NULL) return -1;
    pCoreMaterial->setName(strMaterialName);
    m_vectorCoreMaterial[id] = pCoreMaterial;
    pCoreMaterial->incRef();
  }
  else
  {
    id = loadCoreMaterial(strFilename);
    if(id >= 0)
      addMaterialName(strMaterialName, id);
  }

  return id;
}

 /*****************************************************************************/
/** Delete the resources used by the named core material. The name must 
  * be associated with a valid core material Id with the function
  * getMaterialId. The caller must ensure that the corresponding is not
  * referenced anywhere otherwise unpredictable results will occur.
  *
  * @param name The symbolic name of the core material to unload.
  *
  * @return One of the following values:
  *         \li the core \b ID of the unloaded core material
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::unloadCoreMaterial(const std::string& name)
{
  int id = getCoreMaterialId(name);
  if(id >= 0)
    return unloadCoreMaterial(id);
  else
    return -1;
}

 /*****************************************************************************/
/** Delete the resources used by a core material. The caller must
  * ensure that the corresponding is not referenced anywhere otherwise
  * unpredictable results will occur.
  *
  * @param coreMaterialId The ID of the core material that should be unloaded.
  *
  * @return One of the following values:
  *         \li the core \b ID of the unloaded core material
  *         \li \b -1 if an error happend
  *****************************************************************************/


int CalCoreModel::unloadCoreMaterial(int coreMaterialId)
{
  if((coreMaterialId < 0) || (coreMaterialId >= (int)m_vectorCoreMaterial.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  if(m_vectorCoreMaterial[coreMaterialId]->decRef())
  {
	  delete m_vectorCoreMaterial[coreMaterialId];	  
  }
  m_vectorCoreMaterial[coreMaterialId] = 0;

  return coreMaterialId;
}

 


int CalCoreModel::loadCoreMesh(TiXmlDocument &doc) {
  // the core skeleton has to be loaded already
  if(m_pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // load a new core mesh
  CalCoreMesh *pCoreMesh = CalLoader::loadXmlCoreMesh_xml(doc);
  if(pCoreMesh == 0) return -1;

  // add core mesh to this core model
  int meshId;
  meshId = addCoreMesh(pCoreMesh);
  if(meshId == -1)
  {
    delete pCoreMesh;
    return -1;
  }

  return meshId;

}

/*****************************************************************************/
/** Loads a core mesh.
  *
  * This function loads a core mesh from a file.
  *
  * @param strFilename The file from which the core mesh should be loaded from.
  *
  * @return One of the following values:
  *         \li the assigned \b ID of the loaded core mesh
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::loadCoreMesh(const std::string& strFilename)
{
  // the core skeleton has to be loaded already
  if(m_pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  // load a new core mesh
  CalCoreMesh *pCoreMesh = CalLoader::loadCoreMesh(strFilename);
  if(pCoreMesh == 0) return -1;

  // add core mesh to this core model
  int meshId;
  meshId = addCoreMesh(pCoreMesh);
  if(meshId == -1)
  {
    delete pCoreMesh;
    return -1;
  }

  return meshId;
}

 /*****************************************************************************/
/** Loads a core mesh and bind it to a name.
  *
  * This function loads a core mesh from a file. It is equivalent
  * to calling addMeshName(strMeshName, loadCoreMesh(strFilename)).
  * If strMeshName is already associated to a coreMeshId because
  * of a previous call to addMeshName, the same coreMeshId will
  * be used. 
  *
  * @param strFilename The file from which the core mesh should be loaded
  *                    from.
  * @param strMeshName A string that is associated with an anim ID number.
  *
  * @return One of the following values:
  *         \li the assigned \b ID of the loaded core mesh
  *         \li \b -1 if an error happend
  *****************************************************************************/



int CalCoreModel::loadCoreMesh(const std::string& strFilename, const std::string& strMeshName)
{
  int id = -1;
  std::map<std::string, int>::iterator it = m_meshName.find(strMeshName);
  if (it != m_meshName.end())
  {
    id=(*it).second;

    // the core skeleton has to be loaded already
    if(m_pCoreSkeleton == NULL)
    {
      CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
      return -1;
    }
    if(m_vectorCoreMesh[id] != NULL)
    {
      CalError::setLastError(CalError::INDEX_BUILD_FAILED, __FILE__, __LINE__);
      return -1;
    }
    CalCoreMesh *pCoreMesh = CalLoader::loadCoreMesh(strFilename);
    if(pCoreMesh == NULL) return -1;
    pCoreMesh->setName(strMeshName);
    m_vectorCoreMesh[id] = pCoreMesh;
    pCoreMesh->incRef();
  }
  else
  {
    id = loadCoreMesh(strFilename);
    if(id >= 0)
      addMeshName(strMeshName, id);
  }

  return id;
}


 /*****************************************************************************/
/** Delete the resources used by the named core mesh. The name must 
  * be associated with a valid core mesh Id with the function
  * getMeshId. The caller must ensure that the corresponding is not
  * referenced anywhere otherwise unpredictable results will occur.
  *
  * @param name The symbolic name of the core mesh to unload.
  *
  * @return One of the following values:
  *         \li the core \b ID of the unloaded core mesh
  *         \li \b -1 if an error happend
  *****************************************************************************/

int CalCoreModel::unloadCoreMesh(const std::string& name)
{
  int id = getCoreMeshId(name);
  if(id >= 0)
    return unloadCoreMesh(id);
  else
    return -1;
}

 /*****************************************************************************/
/** Delete the resources used by a core mesh. The caller must
  * ensure that the corresponding is not referenced anywhere otherwise
  * unpredictable results will occur.
  *
  * @param coreMeshId The ID of the core mesh that should be unloaded.
  *
  * @return One of the following values:
  *         \li the core \b ID of the unloaded core mesh
  *         \li \b -1 if an error happend
  *****************************************************************************/


int CalCoreModel::unloadCoreMesh(int coreMeshId)
{
  if((coreMeshId < 0) || (coreMeshId >= (int)m_vectorCoreMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return -1;
  }

  if(m_vectorCoreMesh[coreMeshId]->decRef())
  {
	  delete m_vectorCoreMesh[coreMeshId];	  
  }
  m_vectorCoreMesh[coreMeshId] = 0;

  return coreMeshId;
}


 /*****************************************************************************/
/** Loads the core skeleton.
  *
  * This function loads the core skeleton from a file.
  *
  * @param strFilename The file from which the core skeleton should be loaded
  *                    from.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreModel::loadCoreSkeleton(TiXmlDocument &doc) {
  // destroy the current core skeleton
  if(m_pCoreSkeleton != 0)
  {
    if(m_pCoreSkeleton->decRef())
    {
        delete m_pCoreSkeleton;
    }        
  }
  m_pCoreSkeleton = CalLoader::loadXmlCoreSkeleton_xml(doc);
  if(m_pCoreSkeleton == 0) return false;
  m_pCoreSkeleton->incRef();
  return true;
}

bool CalCoreModel::loadCoreSkeleton(const std::string& strFilename)
{
  // destroy the current core skeleton
  if(m_pCoreSkeleton != 0)
  {
    if(m_pCoreSkeleton->decRef())
    {
        delete m_pCoreSkeleton;
    }        
  }

  // load a new core skeleton
  m_pCoreSkeleton = CalLoader::loadCoreSkeleton(strFilename);
  if(m_pCoreSkeleton == 0) return false;
  m_pCoreSkeleton->incRef();

  return true;
}

 /*****************************************************************************/
/** Saves a core animation.
  *
  * This function saves a core animation to a file.
  *
  * @param strFilename The file to which the core animation should be saved to.
  * @param coreAnimationId The ID of the core animation that should be saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreModel::saveCoreAnimation(const std::string& strFilename, int coreAnimationId)
{
  // check if the core animation id is valid
  /*if((coreAnimationId < 0) || (coreAnimationId >= (int)m_vectorCoreAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // save the core animation
  if(!CalSaver::saveCoreAnimation(strFilename, m_vectorCoreAnimation[coreAnimationId]))
  {
    return false;
  }
  */
  return true;
}

 /*****************************************************************************/
/** Saves a core material.
  *
  * This function saves a core material to a file.
  *
  * @param strFilename The file to which the core material should be saved to.
  * @param coreMaterialId The ID of the core material that should be saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreModel::saveCoreMaterial(const std::string& strFilename, int coreMaterialId)
{
  // check if the core material id is valid
  /*if((coreMaterialId < 0) || (coreMaterialId >= (int)m_vectorCoreMaterial.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // save the core animation
  if(!CalSaver::saveCoreMaterial(strFilename, m_vectorCoreMaterial[coreMaterialId]))
  {
    return false;
  }
*/
  return true;
}

 /*****************************************************************************/
/** Saves a core mesh.
  *
  * This function saves a core mesh to a file.
  *
  * @param strFilename The file to which the core mesh should be saved to.
  * @param coreMeshId The ID of the core mesh that should be saved.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreModel::saveCoreMesh(const std::string& strFilename, int coreMeshId)
{
/*
  // check if the core mesh id is valid
  if((coreMeshId < 0) || (coreMeshId >= (int)m_vectorCoreMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // save the core animation
  if(!CalSaver::saveCoreMesh(strFilename, m_vectorCoreMesh[coreMeshId]))
  {
    return false;
  }
*/
  return true;
}

 /*****************************************************************************/
/** Saves the core skeleton.
  *
  * This function saves the core skeleton to a file.
  *
  * @param strFilename The file to which the core skeleton should be saved to.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreModel::saveCoreSkeleton(const std::string& strFilename)
{
  /*
  // check if we have a core skeleton in this code model
  if(m_pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // save the core skeleton
  if(!CalSaver::saveCoreSkeleton(strFilename, m_pCoreSkeleton))
  {
    return false;
  }
*/
  return true;
}

 /*****************************************************************************/
/** Sets a core material ID.
  *
  * This function sets a core material ID for a core material thread / core
  * material set pair.
  *
  * @param coreMaterialThreadId The ID of the core material thread.
  * @param coreMaterialSetId The ID of the core maetrial set.
  * @param coreMaterialId The ID of the core maetrial.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalCoreModel::setCoreMaterialId(int coreMaterialThreadId, int coreMaterialSetId, int coreMaterialId)
{
  // find the core material thread
  std::map<int, std::map<int, int> >::iterator iteratorCoreMaterialThread;
  iteratorCoreMaterialThread = m_mapmapCoreMaterialThread.find(coreMaterialThreadId);
  if(iteratorCoreMaterialThread == m_mapmapCoreMaterialThread.end())
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  // get the core material thread
  std::map<int, int>& coreMaterialThread = (*iteratorCoreMaterialThread).second;

  // remove a possible entry in the core material thread
  coreMaterialThread.erase(coreMaterialSetId);

  // set the given set id in the core material thread to the given core material id
  coreMaterialThread.insert(std::make_pair(coreMaterialSetId, coreMaterialId));

  return true;
}

 /*****************************************************************************/
/** Sets the core skeleton.
  *
  * This function sets the core skeleton of the core model instance..
  *
  * @param pCoreSkeleton The core skeleton that should be set.
  *****************************************************************************/

void CalCoreModel::setCoreSkeleton(CalCoreSkeleton *pCoreSkeleton)
{
  if(pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return;
  }

  // destroy a possible existing core skeleton
  if(m_pCoreSkeleton != 0)
  {
	if(m_pCoreSkeleton->decRef())
	{
		delete m_pCoreSkeleton;
	}
  }  
  m_pCoreSkeleton = pCoreSkeleton;  
  m_pCoreSkeleton->incRef();
}

 /*****************************************************************************/
/** Stores user data.
  *
  * This function stores user data in the core model instance.
  *
  * @param userData The user data that should be stored.
  *****************************************************************************/

void CalCoreModel::setUserData(Cal::UserData userData)
{
  m_userData = userData;
}

 /*****************************************************************************/
/** Creates or overwrites a string-to-boneId mapping
  *
  * This function makes a bone ID reference-able by a string name.
  *
  * @param strBoneName The string that will be associated with the ID.
  * @param boneId The ID number of the bone that will be referenced by the string.
  *****************************************************************************/

void CalCoreModel::addBoneName(const std::string& strBoneName, int boneId)
{
  //Make sure the skeleton has been loaded first
  if (m_pCoreSkeleton != NULL)
  {
    //Map the bone ID to the name
    m_pCoreSkeleton->mapCoreBoneName(boneId, strBoneName);
  }
}

 /*****************************************************************************/
/** Retrieves the ID of the bone referenced by a string
  *
  * This function returns a bone ID
  *
  * @param strBoneName A string that is associated with a bone ID number.
  * @return Returns:
  *         \li \b -1 if there is no bone ID associated with the input string
  *         \li \b the ID number of the bone asssociated with the input string
  *****************************************************************************/

int CalCoreModel::getBoneId(const std::string& strBoneName)
{
  if (m_pCoreSkeleton != NULL)
  {
    return m_pCoreSkeleton->getCoreBoneId(strBoneName);
  }
  return -1;
}

 /*****************************************************************************/
/** Creates or overwrites a string-to-animation ID mapping
  *
  * This function makes an animation ID reference-able by a string name.
  * Note that we don't verify that the ID is valid because the animation
  * may be added later.
  * Also, if there is already a helper with this name, it will be overwritten
  * without warning.
  *
  * @param strAnimationName The string that will be associated with the ID.
  * @param coreAnimationId The ID number of the animation to be referenced by the string.
  *****************************************************************************/

bool CalCoreModel::addAnimationName(const std::string& strAnimationName, int coreAnimationId)
{
  // check if the core animation id is valid
  if((coreAnimationId < 0) || (coreAnimationId >= (int)m_vectorCoreAnimation.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_vectorCoreAnimation[ coreAnimationId ]->setName(strAnimationName);
  m_animationName[ strAnimationName ] = coreAnimationId;
  return true;
}

 /*****************************************************************************/
/** Retrieves the ID of the animation referenced by a string
  *
  * This function returns an animation ID
  *
  * @param strAnimationName A string that is associated with an anim ID number.
  * @return Returns:
  *         \li \b -1 if there is no anim ID associated with the input string
  *         \li \b the ID number of the anim asssociated with the input string
  *****************************************************************************/

int CalCoreModel::getCoreAnimationId(const std::string& strAnimationName)
{
  if (m_animationName.count( strAnimationName ) < 1)
  {
    return -1;
  }
   
  if (getCoreAnimation(m_animationName[strAnimationName]) == NULL)
  {
    return -1;
  }

  return m_animationName[strAnimationName];
}

 /*****************************************************************************/
/** Creates or overwrites a string-to-core-material ID mapping
  *
  * This function makes a core material ID reference-able by a string name.
  * Note that we don't verify that the ID is valid because the material
  * may be added later.
  * Also, if there is already a helper with this name, it will be overwritten
  * without warning.
  *
  * @param strMaterialName The string that will be associated with the ID.
  * @param coreMaterialId The core ID number of the material to be referenced by the string.
  *****************************************************************************/

bool CalCoreModel::addMaterialName(const std::string& strMaterialName, int coreMaterialId)
{
  
  // check if the core material id is valid
  if((coreMaterialId < 0) || (coreMaterialId >= (int)m_vectorCoreMaterial.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_vectorCoreMaterial[ coreMaterialId ]->setName(strMaterialName);
  m_materialName[ strMaterialName ] = coreMaterialId;
  return true;
}

 /*****************************************************************************/
/** Retrieves the ID of the core material referenced by a string
  *
  * This function returns a core material ID
  *
  * @param strMaterialName A string that is associated with a core material ID number.
  * @return Returns:
  *         \li \b -1 if there is no core material ID associated with the input string
  *         \li \b the core ID number of the material asssociated with the input string
  *****************************************************************************/

int CalCoreModel::getCoreMaterialId(const std::string& strMaterialName)
{
  if (m_materialName.count( strMaterialName ) < 1)
  {
    return -1;
  }

  if (getCoreMaterial(m_materialName[strMaterialName]) == NULL) 
  {
    return -1;
  }

  return m_materialName[strMaterialName];
}


 /*****************************************************************************/
/** Creates or overwrites a string-to-core-mesh ID mapping
  *
  * This function makes a core mesh ID reference-able by a string name.
  * Note that we don't verify that the ID is valid because the mesh
  * may be added later.
  * Also, if there is already a helper with this name, it will be overwritten
  * without warning.
  *
  * @param strMeshName The string that will be associated with the ID.
  * @param coreMeshId The core ID number of the mesh to be referenced by the string.
  *****************************************************************************/

bool CalCoreModel::addMeshName(const std::string& strMeshName, int coreMeshId)
{
  // check if the core mesh id is valid
  if((coreMeshId < 0) || (coreMeshId >= (int)m_vectorCoreMesh.size()))
  {
    CalError::setLastError(CalError::INVALID_HANDLE, __FILE__, __LINE__);
    return false;
  }

  m_vectorCoreMesh[ coreMeshId ]->setName(strMeshName);
  m_meshName[ strMeshName ] = coreMeshId;
  return true;
}

 /*****************************************************************************/
/** Retrieves the ID of the core mesh referenced by a string
  *
  * This function returns a core mesh ID
  *
  * @param strMeshName A string that is associated with a core mesh ID number.
  * @return Returns:
  *         \li \b -1 if there is no core mesh ID associated with the input string
  *         \li \b the core ID number of the mesh asssociated with the input string
  *****************************************************************************/

int CalCoreModel::getCoreMeshId(const std::string& strMeshName)
{
  if (m_meshName.count( strMeshName ) < 1)
  {
    return -1;
  }

  if (getCoreMesh(m_meshName[strMeshName]) == NULL)
  {
    return -1;
  }

  return m_meshName[strMeshName];
}

 /*****************************************************************************/
/** Scale the core model.
  *
  * This function rescale all data that are in the core model instance
  *
  * @param factor A float with the scale factor
  *
  *****************************************************************************/


void CalCoreModel::scale(float factor)
{
  m_pCoreSkeleton->scale(factor);

  for(size_t animationId = 0; animationId < m_vectorCoreAnimation.size(); animationId++)
  {
    m_vectorCoreAnimation[animationId]->scale(factor);
  }

  for(size_t meshId = 0; meshId < m_vectorCoreMesh.size(); meshId++)
  {
    m_vectorCoreMesh[meshId]->scale(factor);
  }

}

//****************************************************************************//
