//****************************************************************************//
// loader.cpp                                                                 //
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

#include "cal3d/loader.h"
#include "cal3d/error.h"
#include "cal3d/matrix.h"
#include "cal3d/vector.h"
#include "cal3d/quaternion.h"
#include "cal3d/coremodel.h"
#include "cal3d/coreskeleton.h"
#include "cal3d/corebone.h"
#include "cal3d/coreanimation.h"
#include "cal3d/coretrack.h"
#include "cal3d/corekeyframe.h"
#include "cal3d/coremesh.h"
#include "cal3d/coresubmesh.h"
#include "cal3d/corematerial.h"
#include "cal3d/tinyxml.h"
#include "cal3d/streamsource.h"
#include "cal3d/buffersource.h"

int CalLoader::loadingMode;

 /*****************************************************************************/
/** Sets optional flags which affect how the model is loaded into memory.
  *
  * This function sets the loading mode for all future loader calls.
  *
  * @param flags A boolean OR of any of the following flags
  *         \li LOADER_ROTATE_X_AXIS will rotate the mesh 90 degrees about the X axis,
  *             which has the effect of swapping Y/Z coordinates.
  *         \li LOADER_INVERT_V_COORD will substitute (1-v) for any v texture coordinate
  *             to eliminate the need for texture inversion after export.
  *
  *****************************************************************************/
void CalLoader::setLoadingMode(int flags)
{
    loadingMode = flags;
}

 /*****************************************************************************/
/** Loads a core animation instance.
  *
  * This function loads a core animation instance from a file.
  *
  * @param strFilename The file to load the core animation instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreAnimation *CalLoader::loadCoreAnimation(const std::string& strFilename, CalCoreSkeleton *skel)
{
  if(strFilename.size()>= 3 && stricmp(strFilename.substr(strFilename.size()-3,3).c_str(),Cal::ANIMATION_XMLFILE_MAGIC)==0)
    return loadXmlCoreAnimation(strFilename, skel);

/*  // open the file
  std::ifstream file(strFilename.c_str(), std::ios::in | std::ios::binary);

  //make sure it was opened properly
  if(!file)
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }

  //make a new stream data source and use it to load the animation
  CalStreamSource streamSrc( file );
  
  CalCoreAnimation* coreanim = loadCoreAnimation( streamSrc,skel );
  if(coreanim) coreanim->setFilename( strFilename );

  //close the file
  file.close();

  return coreanim;*/
  return 0;
}

 /*****************************************************************************/
/** Loads a core material instance.
  *
  * This function loads a core material instance from a file.
  *
  * @param strFilename The file to load the core material instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core material
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreMaterial *CalLoader::loadCoreMaterial(const std::string& strFilename)
{

  if(strFilename.size()>= 3 && stricmp(strFilename.substr(strFilename.size()-3,3).c_str(),Cal::MATERIAL_XMLFILE_MAGIC)==0)
    return loadXmlCoreMaterial(strFilename);

/*  // open the file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);

  // make sure it opened properly
  if(!file)
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }

  //make a new stream data source and use it to load the material
  CalStreamSource streamSrc( file );
  
  CalCoreMaterial* coremat = loadCoreMaterial( streamSrc );

  if(coremat) coremat->setFilename( strFilename );

  //close the file
  file.close();

  return coremat;
*/
  return 0;
}

 /*****************************************************************************/
/** Loads a core mesh instance.
  *
  * This function loads a core mesh instance from a file.
  *
  * @param strFilename The file to load the core mesh instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core mesh
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreMesh *CalLoader::loadCoreMesh(const std::string& strFilename)
{

  if(strFilename.size()>= 3 && stricmp(strFilename.substr(strFilename.size()-3,3).c_str(),Cal::MESH_XMLFILE_MAGIC)==0)
    return loadXmlCoreMesh(strFilename);
/*
  // open the file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);

  // make sure it opened properly
  if(!file)
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }

  //make a new stream data source and use it to load the mesh
  CalStreamSource streamSrc( file );
  
  CalCoreMesh* coremesh = loadCoreMesh( streamSrc );

  if(coremesh) coremesh->setFilename( strFilename );


  //close the file
  file.close();

  return coremesh;
*/
  return 0;
}

 /*****************************************************************************/
/** Loads a core skeleton instance.
  *
  * This function loads a core skeleton instance from a file.
  *
  * @param strFilename The file to load the core skeleton instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreSkeleton *CalLoader::loadCoreSkeleton(const std::string& strFilename)
{

  if(strFilename.size()>= 3 && stricmp(strFilename.substr(strFilename.size()-3,3).c_str(),Cal::SKELETON_XMLFILE_MAGIC)==0)
    return loadXmlCoreSkeleton(strFilename);
/*
  // open the file
  std::ifstream file;
  file.open(strFilename.c_str(), std::ios::in | std::ios::binary);

  //make sure it opened properly
  if(!file)
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }

  //make a new stream data source and use it to load the skeleton
  CalStreamSource streamSrc( file );
  
  CalCoreSkeleton* coreskeleton = loadCoreSkeleton( streamSrc );

  //close the file
  file.close();

  return coreskeleton;
*/
  return 0;
}


 /*****************************************************************************/
/** Loads a core animation instance.
  *
  * This function loads a core animation instance from an input stream.
  *
  * @param inputStream The stream to load the core animation instance from. This
  *                    stream should be initialized and ready to be read from.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreAnimation *CalLoader::loadCoreAnimation(std::istream& inputStream, CalCoreSkeleton *skel )
{
   //Create a new istream data source and pass it on
   CalStreamSource streamSrc(inputStream);
   return loadCoreAnimation(streamSrc, skel);
}

 /*****************************************************************************/
/** Loads a core material instance.
  *
  * This function loads a core material instance from an input stream.
  *
  * @param inputStream The stream to load the core material instance from. This
  *                    stream should be initialized and ready to be read from.
  *
  * @return One of the following values:
  *         \li a pointer to the core material
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreMaterial *CalLoader::loadCoreMaterial(std::istream& inputStream)
{
   //Create a new istream data source and pass it on
   CalStreamSource streamSrc(inputStream);
   return loadCoreMaterial(streamSrc);
}

 /*****************************************************************************/
/** Loads a core mesh instance.
  *
  * This function loads a core mesh instance from an input stream.
  *
  * @param inputStream The stream to load the core mesh instance from. This
  *                    stream should be initialized and ready to be read from.
  *
  * @return One of the following values:
  *         \li a pointer to the core mesh
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreMesh *CalLoader::loadCoreMesh(std::istream& inputStream)
{
   //Create a new istream data source and pass it on
   CalStreamSource streamSrc(inputStream);
   return loadCoreMesh(streamSrc);
}

 /*****************************************************************************/
/** Loads a core skeleton instance.
  *
  * This function loads a core skeleton instance from an input stream.
  *
  * @param inputStream The stream to load the core skeleton instance from. This
  *                    stream should be initialized and ready to be read from.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreSkeleton *CalLoader::loadCoreSkeleton(std::istream& inputStream)
{
   //Create a new istream data source and pass it on
   CalStreamSource streamSrc(inputStream);
   return loadCoreSkeleton(streamSrc);
}




 /*****************************************************************************/
/** Loads a core animation instance.
  *
  * This function loads a core animation instance from a memory buffer.
  *
  * @param inputBuffer The memory buffer to load the core animation instance 
  *                    from. This buffer should be initialized and ready to 
  *                    be read from.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreAnimation *CalLoader::loadCoreAnimation(void* inputBuffer, CalCoreSkeleton *skel)
{
   //Create a new buffer data source and pass it on
   CalBufferSource bufferSrc(inputBuffer);
   return loadCoreAnimation(bufferSrc,skel);
}

 /*****************************************************************************/
/** Loads a core material instance.
  *
  * This function loads a core material instance from a memory buffer.
  *
  * @param inputBuffer The memory buffer to load the core material instance 
  *                    from. This buffer should be initialized and ready to 
  *                    be read from.
  *
  * @return One of the following values:
  *         \li a pointer to the core material
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreMaterial *CalLoader::loadCoreMaterial(void* inputBuffer)
{
   //Create a new buffer data source and pass it on
   CalBufferSource bufferSrc(inputBuffer);
   return loadCoreMaterial(bufferSrc);
}

 /*****************************************************************************/
/** Loads a core mesh instance.
  *
  * This function loads a core mesh instance from a memory buffer.
  *
  * @param inputBuffer The memory buffer to load the core mesh instance from. 
  *                    This buffer should be initialized and ready to be 
  *                    read from.
  *
  * @return One of the following values:
  *         \li a pointer to the core mesh
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreMesh *CalLoader::loadCoreMesh(void* inputBuffer)
{
   //Create a new buffer data source and pass it on
   CalBufferSource bufferSrc(inputBuffer);
   return loadCoreMesh(bufferSrc);
}

 /*****************************************************************************/
/** Loads a core skeleton instance.
  *
  * This function loads a core skeleton instance from a memory buffer.
  *
  * @param inputBuffer The memory buffer to load the core skeleton instance 
  *                    from. This buffer should be initialized and ready to 
  *                    be read from.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreSkeleton *CalLoader::loadCoreSkeleton(void* inputBuffer)
{
   //Create a new buffer data source and pass it on
   CalBufferSource bufferSrc(inputBuffer);
   return loadCoreSkeleton(bufferSrc);
}

 /*****************************************************************************/
/** Loads a core animation instance.
  *
  * This function loads a core animation instance from a data source.
  *
  * @param dataSrc The data source to load the core animation instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreAnimation *CalLoader::loadCoreAnimation(CalDataSource& dataSrc, CalCoreSkeleton *skel)
{
  (void)dataSrc;
  (void)skel;
/*
  // check if this is a valid file
  char magic[4];
  if(!dataSrc.readBytes(&magic[0], 4) || (memcmp(&magic[0], Cal::ANIMATION_FILE_MAGIC, 4) != 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // check if the version is compatible with the library
  int version;
  if(!dataSrc.readInteger(version) || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__);
    return 0;
  }

  // allocate a new core animation instance
  CalCoreAnimation* pCoreAnimation = new CalCoreAnimation();
  if(pCoreAnimation == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // get the duration of the core animation
  float duration;
  if(!dataSrc.readFloat(duration))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    delete pCoreAnimation;
    return 0;
  }

  // check for a valid duration
  if(duration <= 0.0f)
  {
    CalError::setLastError(CalError::INVALID_ANIMATION_DURATION, __FILE__, __LINE__);
    delete pCoreAnimation;
    return 0;
  }

  // set the duration in the core animation instance
  pCoreAnimation->setDuration(duration);

  // read the number of tracks
  int trackCount;
  if(!dataSrc.readInteger(trackCount) || (trackCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // load all core bones
  int trackId;
  for(trackId = 0; trackId < trackCount; ++trackId)
  {
    // load the core track
    CalCoreTrack *pCoreTrack = loadCoreTrack(dataSrc,skel);
    if(pCoreTrack == 0)
    {
      delete pCoreAnimation;
      return 0;
    }

    // add the core track to the core animation instance
    pCoreAnimation->addCoreTrack(pCoreTrack);
  }

  return pCoreAnimation;*/
	return 0;
}


 /*****************************************************************************/
/** Loads a core material instance.
  *
  * This function loads a core material instance from a data source.
  *
  * @param dataSrc The data source to load the core material instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core material
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreMaterial *CalLoader::loadCoreMaterial(CalDataSource& dataSrc)
{
  (void)dataSrc;
/*
  // check if this is a valid file
  char magic[4];
  if(!dataSrc.readBytes(&magic[0], 4) || (memcmp(&magic[0], Cal::MATERIAL_FILE_MAGIC, 4) != 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // check if the version is compatible with the library
  int version;
  if(!dataSrc.readInteger(version) || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__);
    return 0;
  }

  // allocate a new core material instance
  CalCoreMaterial *pCoreMaterial = new CalCoreMaterial();
  if(pCoreMaterial == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // get the ambient color of the core material
  CalCoreMaterial::Color ambientColor;
  dataSrc.readBytes(&ambientColor, sizeof(ambientColor));

  // get the diffuse color of the core material
  CalCoreMaterial::Color diffuseColor;
  dataSrc.readBytes(&diffuseColor, sizeof(diffuseColor));

  // get the specular color of the core material
  CalCoreMaterial::Color specularColor;
  dataSrc.readBytes(&specularColor, sizeof(specularColor));

  // get the shininess factor of the core material
  float shininess;
  dataSrc.readFloat(shininess);

  // check if an error happened
  if(!dataSrc.ok())
  {
    dataSrc.setError();
    delete pCoreMaterial;
    return 0;
  }

  // set the colors and the shininess
  pCoreMaterial->setAmbientColor(ambientColor);
  pCoreMaterial->setDiffuseColor(diffuseColor);
  pCoreMaterial->setSpecularColor(specularColor);
  pCoreMaterial->setShininess(shininess);

  // read the number of maps
  int mapCount;
  if(!dataSrc.readInteger(mapCount) || (mapCount < 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // reserve memory for all the material data
  if(!pCoreMaterial->reserve(mapCount))
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    delete pCoreMaterial;
    return 0;
  }

  // load all maps
  int mapId;
  for(mapId = 0; mapId < mapCount; ++mapId)
  {
    CalCoreMaterial::Map map;

    // read the filename of the map
    std::string strName;
    dataSrc.readString(map.strFilename);

    // initialize the user data
    map.userData = 0;

    // check if an error happened
    if(!dataSrc.ok())
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
      delete pCoreMaterial;
      return 0;
    }

    // set map in the core material instance
    pCoreMaterial->setMap(mapId, map);
  }

  return pCoreMaterial;*/
	return 0;
}

 /*****************************************************************************/
/** Loads a core mesh instance.
  *
  * This function loads a core mesh instance from a data source.
  *
  * @param dataSrc The data source to load the core mesh instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core mesh
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreMesh *CalLoader::loadCoreMesh(CalDataSource& dataSrc)
{
  (void)dataSrc;
/*
  // check if this is a valid file
  char magic[4];
  if(!dataSrc.readBytes(&magic[0], 4) || (memcmp(&magic[0], Cal::MESH_FILE_MAGIC, 4) != 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // check if the version is compatible with the library
  int version;
  if(!dataSrc.readInteger(version) || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__);
    return 0;
  }

  // get the number of submeshes
  int submeshCount;
  if(!dataSrc.readInteger(submeshCount))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // allocate a new core mesh instance
  CalCoreMesh *pCoreMesh = new CalCoreMesh();
  if(pCoreMesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // load all core submeshes
  for(int submeshId = 0; submeshId < submeshCount; ++submeshId)
  {
    // load the core submesh
    CalCoreSubmesh *pCoreSubmesh;
    pCoreSubmesh = loadCoreSubmesh(dataSrc);
    if(pCoreSubmesh == 0)
    {
      delete pCoreMesh;
      return 0;
    }

    // add the core submesh to the core mesh instance
    pCoreMesh->addCoreSubmesh(pCoreSubmesh);
  }

  return pCoreMesh;*/
	return 0;
}

 /*****************************************************************************/
/** Loads a core skeleton instance.
  *
  * This function loads a core skeleton instance from a data source.
  *
  * @param dataSrc The data source to load the core skeleton instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreSkeleton *CalLoader::loadCoreSkeleton(CalDataSource& dataSrc)
{
  (void)dataSrc;
/*
  // check if this is a valid file
  char magic[4];
  if(!dataSrc.readBytes(&magic[0], 4) || (memcmp(&magic[0], Cal::SKELETON_FILE_MAGIC, 4) != 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // check if the version is compatible with the library
  int version;
  if(!dataSrc.readInteger(version) || (version < Cal::EARLIEST_COMPATIBLE_FILE_VERSION) || (version > Cal::CURRENT_FILE_VERSION))
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__);
    return 0;
  }

  // read the number of bones
  int boneCount;
  if(!dataSrc.readInteger(boneCount) || (boneCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // allocate a new core skeleton instance
  CalCoreSkeleton *pCoreSkeleton = new CalCoreSkeleton();
  if(pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // load all core bones
  for(int boneId = 0; boneId < boneCount; ++boneId)
  {
    // load the core bone
    CalCoreBone *pCoreBone = loadCoreBones(dataSrc);
    if(pCoreBone == 0)
    {
      delete pCoreSkeleton;
      return 0;
    }

    // set the core skeleton of the core bone instance
    pCoreBone->setCoreSkeleton(pCoreSkeleton);

    // add the core bone to the core skeleton instance
    pCoreSkeleton->addCoreBone(pCoreBone);

    // add a core skeleton mapping of the bone's name for quick reference later
    pCoreSkeleton->mapCoreBoneName(boneId, pCoreBone->getName());

  }

  // calculate state of the core skeleton
  pCoreSkeleton->calculateState();

  return pCoreSkeleton;*/
	return 0;
}



/*****************************************************************************/
/** Loads a core bone instance.
  *
  * This function loads a core bone instance from a data source.
  *
  * @param dataSrc The data source to load the core bone instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core bone
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreBone *CalLoader::loadCoreBones(CalDataSource& dataSrc)
{
  (void)dataSrc;
  /*if(!dataSrc.ok())
  {
    dataSrc.setError();
    return 0;
  }

  // read the name of the bone
  std::string strName;
  dataSrc.readString(strName);

  // get the translation of the bone
  float tx, ty, tz;
  dataSrc.readFloat(tx);
  dataSrc.readFloat(ty);
  dataSrc.readFloat(tz);

  // get the rotation of the bone
  float rx, ry, rz, rw;
  dataSrc.readFloat(rx);
  dataSrc.readFloat(ry);
  dataSrc.readFloat(rz);
  dataSrc.readFloat(rw);

  // get the bone space translation of the bone
  float txBoneSpace, tyBoneSpace, tzBoneSpace;
  dataSrc.readFloat(txBoneSpace);
  dataSrc.readFloat(tyBoneSpace);
  dataSrc.readFloat(tzBoneSpace);

  // get the bone space rotation of the bone
  float rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace;
  dataSrc.readFloat(rxBoneSpace);
  dataSrc.readFloat(ryBoneSpace);
  dataSrc.readFloat(rzBoneSpace);
  dataSrc.readFloat(rwBoneSpace);

  // get the parent bone id
  int parentId;
  dataSrc.readInteger(parentId);

  CalQuaternion rot(rx,ry,rz,rw);
  CalQuaternion rotbs(rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace);
  CalVector trans(tx,ty,tz);

  if (loadingMode & LOADER_ROTATE_X_AXIS)
  {
    if (parentId == -1) // only root bone necessary
    {
      // Root bone must have quaternion rotated
      CalQuaternion x_axis_90(0.7071067811f,0.0f,0.0f,0.7071067811f);
      rot *= x_axis_90;
      // Root bone must have translation rotated also
      trans *= x_axis_90;
    }
  }
  

  // check if an error happened
  if(!dataSrc.ok())
  {
    dataSrc.setError();
    return 0;
  }

  // allocate a new core bone instance
  CalCoreBone *pCoreBone;
  pCoreBone = new CalCoreBone(strName);
  if(pCoreBone == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // set the parent of the bone
  pCoreBone->setParentId(parentId);

  // set all attributes of the bone
  pCoreBone->setTranslation(trans);
  pCoreBone->setRotation(rot);
  pCoreBone->setTranslationBoneSpace(CalVector(txBoneSpace, tyBoneSpace, tzBoneSpace));
  pCoreBone->setRotationBoneSpace(rotbs);

  // read the number of children
  int childCount;
  if(!dataSrc.readInteger(childCount) || (childCount < 0))
  {
    delete pCoreBone;
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // load all children ids
  for(; childCount > 0; childCount--)
  {
    int childId;
    if(!dataSrc.readInteger(childId) || (childId < 0))
    {
      delete pCoreBone;
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
      return 0;
    }

    pCoreBone->addChildId(childId);
  }

  return pCoreBone;*/
	return 0;
}

 /*****************************************************************************/
/** Loads a core keyframe instance.
  *
  * This function loads a core keyframe instance from a data source.
  *
  * @param dataSrc The data source to load the core keyframe instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core keyframe
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreKeyframe *CalLoader::loadCoreKeyframe(CalDataSource& dataSrc)
{
  (void)dataSrc;
  /*if(!dataSrc.ok())
  {
    dataSrc.setError();
    return 0;
  }

  // get the time of the keyframe
  float time;
  dataSrc.readFloat(time);

  // get the translation of the bone
  float tx, ty, tz;
  dataSrc.readFloat(tx);
  dataSrc.readFloat(ty);
  dataSrc.readFloat(tz);

  // get the rotation of the bone
  float rx, ry, rz, rw;
  dataSrc.readFloat(rx);
  dataSrc.readFloat(ry);
  dataSrc.readFloat(rz);
  dataSrc.readFloat(rw);

  // check if an error happened
  if(!dataSrc.ok())
  {
    dataSrc.setError();
    return 0;
  }

  // allocate a new core keyframe instance
  CalCoreKeyframe *pCoreKeyframe;
  pCoreKeyframe = new CalCoreKeyframe();
  if(pCoreKeyframe == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // set all attributes of the keyframe
  pCoreKeyframe->setTime(time);
  pCoreKeyframe->setTranslation(CalVector(tx, ty, tz));
  pCoreKeyframe->setRotation(CalQuaternion(rx, ry, rz, rw));

  return pCoreKeyframe;*/
	return 0;
}

 /*****************************************************************************/
/** Loads a core submesh instance.
  *
  * This function loads a core submesh instance from a data source.
  *
  * @param dataSrc The data source to load the core submesh instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core submesh
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreSubmesh *CalLoader::loadCoreSubmesh(CalDataSource& dataSrc)
{
  (void)dataSrc;
  /*if(!dataSrc.ok())
  {
    dataSrc.setError();
    return 0;
  }

  // get the material thread id of the submesh
  int coreMaterialThreadId;
  dataSrc.readInteger(coreMaterialThreadId);

  // get the number of vertices, faces, level-of-details and springs
  int vertexCount;
  dataSrc.readInteger(vertexCount);

  int faceCount;
  dataSrc.readInteger(faceCount);

  int lodCount;
  dataSrc.readInteger(lodCount);

  int springCount;
  dataSrc.readInteger(springCount);

  // get the number of texture coordinates per vertex
  int textureCoordinateCount;
  dataSrc.readInteger(textureCoordinateCount);

  // check if an error happened
  if(!dataSrc.ok())
  {
    dataSrc.setError();
    return 0;
  }

  // allocate a new core submesh instance
  CalCoreSubmesh *pCoreSubmesh;
  pCoreSubmesh = new CalCoreSubmesh();
  if(pCoreSubmesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // set the LOD step count
  pCoreSubmesh->setLodCount(lodCount);

  // set the core material id
  pCoreSubmesh->setCoreMaterialThreadId(coreMaterialThreadId);

  // reserve memory for all the submesh data
  if(!pCoreSubmesh->reserve(vertexCount, textureCoordinateCount, faceCount, springCount))
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    delete pCoreSubmesh;
    return 0;
  }

  // load the tangent space enable flags.
  int textureCoordinateId;
  for (textureCoordinateId = 0; textureCoordinateId < textureCoordinateCount; textureCoordinateId++)
  {
    pCoreSubmesh->enableTangents(textureCoordinateId, false);
  }

  // load all vertices and their influences
  int vertexId;
  for(vertexId = 0; vertexId < vertexCount; ++vertexId)
  {
    CalCoreSubmesh::Vertex vertex;

    // load data of the vertex
    dataSrc.readFloat(vertex.position.x);
    dataSrc.readFloat(vertex.position.y);
    dataSrc.readFloat(vertex.position.z);
    dataSrc.readFloat(vertex.normal.x);
    dataSrc.readFloat(vertex.normal.y);
    dataSrc.readFloat(vertex.normal.z);
    dataSrc.readInteger(vertex.collapseId);
    dataSrc.readInteger(vertex.faceCollapseCount);

    // check if an error happened
    if(!dataSrc.ok())
    {
      dataSrc.setError();
      delete pCoreSubmesh;
      return 0;
    }

    // load all texture coordinates of the vertex
    int textureCoordinateId;
    for(textureCoordinateId = 0; textureCoordinateId < textureCoordinateCount; ++textureCoordinateId)
    {
      CalCoreSubmesh::TextureCoordinate textureCoordinate;

      // load data of the influence
      dataSrc.readFloat(textureCoordinate.u);
      dataSrc.readFloat(textureCoordinate.v);

      if (loadingMode & LOADER_INVERT_V_COORD)
      {
          textureCoordinate.v = 1.0f - textureCoordinate.v;
      }

      // check if an error happened
      if(!dataSrc.ok())
      {
        dataSrc.setError();
        delete pCoreSubmesh;
        return 0;
      }

      // set texture coordinate in the core submesh instance
      pCoreSubmesh->setTextureCoordinate(vertexId, textureCoordinateId, textureCoordinate);
    }

    // get the number of influences
    int influenceCount;
    if(!dataSrc.readInteger(influenceCount) || (influenceCount < 0))
    {
      dataSrc.setError();
      delete pCoreSubmesh;
      return 0;
    }

    // reserve memory for the influences in the vertex
    vertex.vectorInfluence.reserve(influenceCount);
    vertex.vectorInfluence.resize(influenceCount);

    // load all influences of the vertex
    int influenceId;
    for(influenceId = 0; influenceId < influenceCount; ++influenceId)
    {
      // load data of the influence
      dataSrc.readInteger(vertex.vectorInfluence[influenceId].boneId),
      dataSrc.readFloat(vertex.vectorInfluence[influenceId].weight);

      // check if an error happened
      if(!dataSrc.ok())
      {
        dataSrc.setError();
        delete pCoreSubmesh;
        return 0;
      }
    }

    // set vertex in the core submesh instance
    pCoreSubmesh->setVertex(vertexId, vertex);

    // load the physical property of the vertex if there are springs in the core submesh
    if(springCount > 0)
    {
      CalCoreSubmesh::PhysicalProperty physicalProperty;

      // load data of the physical property
      dataSrc.readFloat(physicalProperty.weight);

      // check if an error happened
      if(!dataSrc.ok())
      {
        dataSrc.setError();
        delete pCoreSubmesh;
        return 0;
      }

      // set the physical property in the core submesh instance
      pCoreSubmesh->setPhysicalProperty(vertexId, physicalProperty);
    }
  }

  // load all springs
  int springId;
  for(springId = 0; springId < springCount; ++springId)
  {
    CalCoreSubmesh::Spring spring;

    // load data of the spring
    dataSrc.readInteger(spring.vertexId[0]);
    dataSrc.readInteger(spring.vertexId[1]);
    dataSrc.readFloat(spring.springCoefficient);
    dataSrc.readFloat(spring.idleLength);

    // check if an error happened
    if(!dataSrc.ok())
    {
      dataSrc.setError();
      delete pCoreSubmesh;
      return 0;
    }

    // set spring in the core submesh instance
    pCoreSubmesh->setSpring(springId, spring);
  }


  // load all faces
  int faceId;
  int justOnce = 0;
  bool flipModel = false;
  for(faceId = 0; faceId < faceCount; ++faceId)
  {
    CalCoreSubmesh::Face face;

    // load data of the face

  int tmp[4];
  dataSrc.readInteger(tmp[0]);
  dataSrc.readInteger(tmp[1]);
  dataSrc.readInteger(tmp[2]);

  if(sizeof(CalIndex)==2)
  {
    if(tmp[0]>65535 || tmp[1]>65535 || tmp[2]>65535)
    {      
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
      delete pCoreSubmesh;
      return 0;
    }
  }
  face.vertexId[0]=tmp[0];
  face.vertexId[1]=tmp[1];
  face.vertexId[2]=tmp[2];
  
    // check if an error happened
    if(!dataSrc.ok())
    {
      dataSrc.setError();
      delete pCoreSubmesh;
      return 0;
    }

    // check if left-handed coord system is used by the object
    // can be done only once since the object has one system for all faces
    if (justOnce==0)
    {
      // get vertexes of first face
      std::vector<CalCoreSubmesh::Vertex>& vectorVertex = pCoreSubmesh->getVectorVertex();
      CalCoreSubmesh::Vertex& v1 = vectorVertex[tmp[0]];
      CalCoreSubmesh::Vertex& v2 = vectorVertex[tmp[1]];
      CalCoreSubmesh::Vertex& v3 = vectorVertex[tmp[2]];

      CalVector point1 = CalVector(v1.position.x, v1.position.y, v1.position.z);
      CalVector point2 = CalVector(v2.position.x, v2.position.y, v2.position.z);
      CalVector point3 = CalVector(v3.position.x, v3.position.y, v3.position.z);

      // gets vectors (v1-v2) and (v3-v2)
      CalVector vect1 = point1 - point2;
      CalVector vect2 = point3 - point2;

      // calculates normal of face
      CalVector cross = vect1 % vect2;
      CalVector faceNormal = cross / cross.length();

      // compare the calculated normal with the normal of a vertex
      CalVector maxNorm = v1.normal;

      // if the two vectors point to the same direction then the poly needs flipping
      // so if the dot product > 0 it needs flipping
      if (faceNormal*maxNorm>0)
      flipModel = true;

      justOnce = 1;
    }

    // flip if needed
    if (flipModel) {
      tmp[3] = face.vertexId[1];
      face.vertexId[1]=face.vertexId[2];
    face.vertexId[2]=tmp[3];
    }

    // set face in the core submesh instance
    pCoreSubmesh->setFace(faceId, face);
  }

  return pCoreSubmesh;*/
	return 0;
}

 /*****************************************************************************/
/** Loads a core track instance.
  *
  * This function loads a core track instance from a data source.
  *
  * @param dataSrc The data source to load the core track instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core track
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreTrack *CalLoader::loadCoreTrack(CalDataSource& dataSrc, CalCoreSkeleton *skel)
{
  (void)dataSrc;
  (void)skel;
  /*if(!dataSrc.ok())
  {
    dataSrc.setError();
    return 0;
  }

  // read the bone id
  int coreBoneId;
  if(!dataSrc.readInteger(coreBoneId) || (coreBoneId < 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // allocate a new core track instance
  CalCoreTrack *pCoreTrack;
  pCoreTrack = new CalCoreTrack();
  if(pCoreTrack == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // link the core track to the appropriate core bone instance
  pCoreTrack->setCoreBoneId(coreBoneId);

  // read the number of keyframes
  int keyframeCount;
  if(!dataSrc.readInteger(keyframeCount) || (keyframeCount <= 0))
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__);
    return 0;
  }

  // load all core keyframes
  int keyframeId;
  for(keyframeId = 0; keyframeId < keyframeCount; ++keyframeId)
  {
    // load the core keyframe
    CalCoreKeyframe *pCoreKeyframe = loadCoreKeyframe(dataSrc);
    if(pCoreKeyframe == 0)
    {
      delete pCoreTrack;
      return 0;
    }
    if (loadingMode & LOADER_ROTATE_X_AXIS)
    {
      // Check for anim rotation
      if (skel && skel->getCoreBone(coreBoneId)->getParentId() == -1)  // root bone
      {
        // rotate root bone quaternion
        CalQuaternion rot = pCoreKeyframe->getRotation();
        CalQuaternion x_axis_90(0.7071067811f,0.0f,0.0f,0.7071067811f);
        rot *= x_axis_90;
        pCoreKeyframe->setRotation(rot);
        // rotate root bone displacement
        CalVector vec = pCoreKeyframe->getTranslation();
      vec *= x_axis_90;
        pCoreKeyframe->setTranslation(vec);
      }
    }    

    // add the core keyframe to the core track instance
    pCoreTrack->addCoreKeyframe(pCoreKeyframe);
  }

  return pCoreTrack;*/
	return 0;
}



 /*****************************************************************************/
/** Loads a core skeleton instance from a XML file.
  *
  * This function loads a core skeleton instance from a XML file.
  *
  * @param strFilename The name of the file to load the core skeleton instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core skeleton
  *         \li \b 0 if an error happened
  *****************************************************************************/


CalCoreSkeleton *CalLoader::loadXmlCoreSkeleton(const std::string& strFilename)
{

  vsxTiXmlDocument doc(strFilename);
  if(!doc.LoadFile())
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }
  return loadXmlCoreSkeleton_xml(doc);
}

CalCoreSkeleton *CalLoader::loadXmlCoreSkeleton_xml(vsxTiXmlDocument &doc) {
  std::stringstream str;
  vsxTiXmlNode* node;
  vsxTiXmlElement*skeleton = doc.FirstChildElement();
  if(!skeleton)
  {
    #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
      return 0;
  }

  if(stricmp(skeleton->Value(),"HEADER")==0)
  {
    if(stricmp(skeleton->Attribute("MAGIC"),Cal::SKELETON_XMLFILE_MAGIC)!=0)
  {
    #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
    return 0;
  }

  if(atoi(skeleton->Attribute("VERSION")) < Cal::EARLIEST_COMPATIBLE_FILE_VERSION )
  {
    #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "xml");
    #endif
          return 0;
  }

  skeleton = skeleton->NextSiblingElement();
  }

  if(!skeleton || stricmp(skeleton->Value(),"SKELETON")!=0)
  {
    #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
        return 0;
  }

  if(skeleton->Attribute("MAGIC")!=NULL && stricmp(skeleton->Attribute("MAGIC"),Cal::SKELETON_XMLFILE_MAGIC)!=0)
  {
    #ifdef CAL_DEBUG
  CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
  #endif
  return 0;
  }

  if(skeleton->Attribute("VERSION")!=NULL && atoi(skeleton->Attribute("VERSION")) < Cal::EARLIEST_COMPATIBLE_FILE_VERSION )
  {
    #ifdef CAL_DEBUG
  CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "xml");
  #endif
         return 0;
  }


  // allocate a new core skeleton instance
  CalCoreSkeleton *pCoreSkeleton;
  pCoreSkeleton = new CalCoreSkeleton();
  if(pCoreSkeleton == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  vsxTiXmlElement* bone;
  for( bone = skeleton->FirstChildElement();bone;bone = bone->NextSiblingElement() )
  {
    if(stricmp(bone->Value(),"BONE")!=0)
    {
      #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
        delete pCoreSkeleton;    
        return 0;
    }    
    
    std::string strName=bone->Attribute("NAME");
      

    // get the translation of the bone

    vsxTiXmlElement* translation = bone->FirstChildElement();
    if(!translation || stricmp( translation->Value(),"TRANSLATION")!=0)
    {
      #ifdef CAL_DEBUG
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      #endif
          delete pCoreSkeleton;    
      return 0;
    }

    float tx, ty, tz;
    
    node = translation->FirstChild();
    if(!node)
    {
      #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
                delete pCoreSkeleton;    
                return 0;
    }    
    vsxTiXmlText* translationdata = node->ToText();
    if(!translationdata)
    {
      #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
        delete pCoreSkeleton;    
        return 0;
    }    
    str.clear();
    str << translationdata->Value();
    str >> tx >> ty >> tz;

      // get the rotation of the bone

    vsxTiXmlElement* rotation = translation->NextSiblingElement();
    if(!rotation || stricmp(rotation->Value(),"ROTATION")!=0)
    {
      #ifdef CAL_DEBUG
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      #endif
          delete pCoreSkeleton;    
      return 0;
    }

    float rx, ry, rz, rw;

    node = rotation->FirstChild();
    if(!node)
    {
      #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
                delete pCoreSkeleton;    
                return 0;
    }
    vsxTiXmlText* rotationdata = node->ToText();
    if(!rotationdata)
    {
      #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
                delete pCoreSkeleton;    
                return 0;
    }
    str.clear();
    str << rotationdata->Value();
    str >> rx >> ry >> rz >> rw;    

    // get the bone space translation of the bone
      
    
    vsxTiXmlElement* translationBoneSpace = rotation->NextSiblingElement();
    if(!rotation || stricmp(translationBoneSpace->Value(),"LOCALTRANSLATION")!=0)
    {
      #ifdef CAL_DEBUG
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      #endif
                  delete pCoreSkeleton;     
      return 0;
    }

    float txBoneSpace, tyBoneSpace, tzBoneSpace;
    
    node = translationBoneSpace->FirstChild();
    if(!node)
    {
      #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
        delete pCoreSkeleton;    
        return 0;
    }
    vsxTiXmlText* translationBoneSpacedata = node->ToText();
    if(!translationBoneSpacedata)
    {
      #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
                delete pCoreSkeleton;    
                return 0;
    }
    str.clear();
    str << translationBoneSpacedata->Value();
    str >> txBoneSpace >> tyBoneSpace >> tzBoneSpace;

      // get the bone space rotation of the bone

    vsxTiXmlElement* rotationBoneSpace = translationBoneSpace->NextSiblingElement();
    if(!rotationBoneSpace || stricmp(rotationBoneSpace->Value(),"LOCALROTATION")!=0)
    { 
      #ifdef CAL_DEBUG
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      #endif
                  delete pCoreSkeleton;      
      return 0;
    }

    float rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace;
    
    node = rotationBoneSpace->FirstChild();
    if(!node)
    {
      #ifdef CAL_DEBUG
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
    #endif
        delete pCoreSkeleton;    
        return 0;
    }
    vsxTiXmlText* rotationBoneSpacedata = node->ToText();
    if(!rotationBoneSpacedata)
    {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreSkeleton;    
        return 0;
    }
    str.clear();
    str << rotationBoneSpacedata->Value();
    str >> rxBoneSpace >> ryBoneSpace >> rzBoneSpace >> rwBoneSpace;

    // get the parent bone id

    vsxTiXmlElement* parent = rotationBoneSpace->NextSiblingElement();
    if(!parent ||stricmp(parent->Value(),"PARENTID")!=0)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreSkeleton;       
      return 0;
    }
    

    int parentId;
    
    node = parent->FirstChild();
    if(!node)
    {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreSkeleton;    
        return 0;
    }
    vsxTiXmlText* parentid = node->ToText();
      if(!parentid)
    {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreSkeleton;    
        return 0;
    }
    parentId = atoi(parentid->Value());

    // allocate a new core bone instance
    CalCoreBone *pCoreBone = new CalCoreBone(strName);
    if(pCoreBone == 0)
    {
            delete pCoreSkeleton;    
            CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
            return 0;
    }

    // set the parent of the bone
    pCoreBone->setParentId(parentId);

    // set all attributes of the bone

    CalVector trans = CalVector(tx, ty, tz);
    CalQuaternion rot = CalQuaternion(rx, ry, rz, rw);

    if (loadingMode & LOADER_ROTATE_X_AXIS)
    {
      if (parentId == -1) // only root bone necessary
      {
        // Root bone must have quaternion rotated
        CalQuaternion x_axis_90(0.7071067811f,0.0f,0.0f,0.7071067811f);
        rot *= x_axis_90;
        // Root bone must have translation rotated also
        trans *= x_axis_90;
      }
    }    
    

    pCoreBone->setTranslation(trans);
    pCoreBone->setRotation(rot);
    pCoreBone->setTranslationBoneSpace(CalVector(txBoneSpace, tyBoneSpace, tzBoneSpace));
    pCoreBone->setRotationBoneSpace(CalQuaternion(rxBoneSpace, ryBoneSpace, rzBoneSpace, rwBoneSpace));


    vsxTiXmlElement* child;
    for( child = parent->NextSiblingElement();child;child = child->NextSiblingElement() )
    {
      if(stricmp(child->Value(),"CHILDID")!=0)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreSkeleton;
        delete pCoreBone;
        return 0;
      }

      vsxTiXmlNode *node= child->FirstChild();
      if(!node)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreSkeleton;
        delete pCoreBone;
        return 0;
      }
      vsxTiXmlText* childid = node->ToText();
      if(!childid)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreSkeleton;
        delete pCoreBone;
        return 0;
      }
      
      int childId = atoi(childid->Value());

      pCoreBone->addChildId(childId);
    }

    // set the core skeleton of the core bone instance
    pCoreBone->setCoreSkeleton(pCoreSkeleton);

    // add the core bone to the core skeleton instance
    pCoreSkeleton->addCoreBone(pCoreBone);

  }

  doc.Clear();

  pCoreSkeleton->calculateState();

    return pCoreSkeleton;
}

 /*****************************************************************************/
/** Loads a core animation instance from a XML file.
  *
  * This function loads a core animation instance from a XML file.
  *
  * @param strFilename The name of the file to load the core animation instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happened
  *****************************************************************************/

CalCoreAnimation *CalLoader::loadXmlCoreAnimation(const std::string& strFilename, CalCoreSkeleton *skel) {
  vsxTiXmlDocument doc(strFilename);
  if(!doc.LoadFile())
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }
  return loadXmlCoreAnimation_xml(doc,skel);
}

CalCoreAnimation *CalLoader::loadXmlCoreAnimation_xml(vsxTiXmlDocument &doc, CalCoreSkeleton *skel)
{
  std::stringstream str;
  vsxTiXmlNode* node;

  vsxTiXmlElement*animation = doc.FirstChildElement();
  if(!animation)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
  }

  if(stricmp(animation->Value(),"HEADER")==0)
  {
    if(stricmp(animation->Attribute("MAGIC"),Cal::ANIMATION_XMLFILE_MAGIC)!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
  }

  if(atoi(animation->Attribute("VERSION")) < Cal::EARLIEST_COMPATIBLE_FILE_VERSION )
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "xml");
    return 0;
  }

  animation = animation->NextSiblingElement();
  }

  if(!animation || stricmp(animation->Value(),"ANIMATION")!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      return 0;
  }

  if(animation->Attribute("MAGIC") !=NULL && stricmp(animation->Attribute("MAGIC"),Cal::ANIMATION_XMLFILE_MAGIC)!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
  }

  if(animation->Attribute("VERSION")!=NULL && atoi(animation->Attribute("VERSION")) < Cal::EARLIEST_COMPATIBLE_FILE_VERSION )
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "xml");
        return 0;
  }

  int trackCount= atoi(animation->Attribute("NUMTRACKS"));
  float duration= (float) atof(animation->Attribute("DURATION"));

  // allocate a new core animation instance
  CalCoreAnimation *pCoreAnimation;
  pCoreAnimation = new CalCoreAnimation();
  if(pCoreAnimation == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  // check for a valid duration
  if(duration <= 0.0f)
  {
    CalError::setLastError(CalError::INVALID_ANIMATION_DURATION, __FILE__, __LINE__, "xml");
    delete pCoreAnimation;
    return 0;
  }

  // set the duration in the core animation instance
  pCoreAnimation->setDuration(duration);
  vsxTiXmlElement* track=animation->FirstChildElement();

  // load all core bones
  int trackId;
  for(trackId = 0; trackId < trackCount; ++trackId)
  {
    if(!track || stricmp(track->Value(),"TRACK")!=0)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      delete pCoreAnimation;
      return 0;
    }

    CalCoreTrack *pCoreTrack;

    pCoreTrack = new CalCoreTrack();
    if(pCoreTrack == 0)
    {
          delete pCoreAnimation;
      CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
      return 0;
    }

    int coreBoneId = atoi(track->Attribute("BONEID"));

    // link the core track to the appropriate core bone instance
    pCoreTrack->setCoreBoneId(coreBoneId);

    // read the number of keyframes
    int keyframeCount= atoi(track->Attribute("NUMKEYFRAMES"));

    if(keyframeCount <= 0)
    {
      delete pCoreAnimation;
      delete pCoreTrack;
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      return 0;
    }

    vsxTiXmlElement* keyframe= track->FirstChildElement();

    // load all core keyframes
    int keyframeId;
    for(keyframeId = 0; keyframeId < keyframeCount; ++keyframeId)
    {
      // load the core keyframe
      if(!keyframe|| stricmp(keyframe->Value(),"KEYFRAME")!=0)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreAnimation;
          delete pCoreTrack;
        return 0;
      }

      float time= (float) atof(keyframe->Attribute("TIME"));

      vsxTiXmlElement* translation = keyframe->FirstChildElement();
      if(!translation || stricmp(translation->Value(),"TRANSLATION")!=0)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreAnimation;
          delete pCoreTrack;
        return 0;
      }

      float tx, ty, tz;

      node = translation->FirstChild();
      if(!node)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreAnimation;
          delete pCoreTrack;
        return 0;
      }

      vsxTiXmlText* translationdata = node->ToText();
      if(!translationdata)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreAnimation;
          delete pCoreTrack;
        return 0;
      }
      str.clear();
      str << translationdata->Value();
      str >> tx >> ty >> tz;

      vsxTiXmlElement* rotation = translation->NextSiblingElement();
      if(!rotation || stricmp(rotation->Value(),"ROTATION")!=0)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreAnimation;
          delete pCoreTrack;
        return 0;
      }

      float rx, ry, rz, rw;

      node = rotation->FirstChild();
      if(!node)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreAnimation;
          delete pCoreTrack;
        return 0;
      }
      vsxTiXmlText* rotationdata = node->ToText();
      if(!rotationdata)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreAnimation;
          delete pCoreTrack;
        return 0;
      }
      str.clear();
      str << rotationdata->Value();
      str >> rx >> ry >> rz >> rw;  
      
      // allocate a new core keyframe instance

      CalCoreKeyframe *pCoreKeyframe;
      pCoreKeyframe = new CalCoreKeyframe();
      if(pCoreKeyframe == 0)
      {
          delete pCoreAnimation;
          delete pCoreTrack;        
        CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
        return 0;
      }

      // set all attributes of the keyframe
      pCoreKeyframe->setTime(time);
      pCoreKeyframe->setTranslation(CalVector(tx, ty, tz));
      pCoreKeyframe->setRotation(CalQuaternion(rx, ry, rz, rw));

      
      if (loadingMode & LOADER_ROTATE_X_AXIS)
      {
        // Check for anim rotation
        if (skel && skel->getCoreBone(coreBoneId)->getParentId() == -1)  // root bone
        {
          // rotate root bone quaternion
          CalQuaternion rot = pCoreKeyframe->getRotation();
          CalQuaternion x_axis_90(0.7071067811f,0.0f,0.0f,0.7071067811f);
          rot *= x_axis_90;
          pCoreKeyframe->setRotation(rot);
          // rotate root bone displacement
          CalVector trans = pCoreKeyframe->getTranslation();
          trans *= x_axis_90;
          pCoreKeyframe->setTranslation(trans);
        }
      }
      
      
      // add the core keyframe to the core track instance
         pCoreTrack->addCoreKeyframe(pCoreKeyframe);

     keyframe = keyframe->NextSiblingElement();

    }

    pCoreAnimation->addCoreTrack(pCoreTrack);
    track=track->NextSiblingElement();
  }

  // explicitly close the file
  doc.Clear();

  return pCoreAnimation;
}

 /*****************************************************************************/
/** Loads a core mesh instance from a Xml file.
  *
  * This function loads a core mesh instance from a Xml file.
  *
  * @param strFilename The name of the file to load the core mesh instance from.
  *
  * @return One of the following values:
  *         \li a pointer to the core mesh
  *         \li \b 0 if an error happened
  *****************************************************************************/
CalCoreMesh *CalLoader::loadXmlCoreMesh(const std::string& strFilename)
{
  vsxTiXmlDocument doc(strFilename);
  if(!doc.LoadFile())
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  }
  return 0;
}


CalCoreMesh *CalLoader::loadXmlCoreMesh_xml(vsxTiXmlDocument &doc) {
  vsxTiXmlNode* node;
  std::stringstream str;

  vsxTiXmlElement*mesh = doc.FirstChildElement();
  if(!mesh)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
  }

  if(stricmp(mesh->Value(),"HEADER")==0)
  {
    if(stricmp(mesh->Attribute("MAGIC"),Cal::MESH_XMLFILE_MAGIC)!=0)
    {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
  }

  if(atoi(mesh->Attribute("VERSION")) < Cal::EARLIEST_COMPATIBLE_FILE_VERSION )
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "xml");
          return 0;
  }

  mesh = mesh->NextSiblingElement();
  }
  if(!mesh || stricmp(mesh->Value(),"MESH")!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      return 0;
  }

  if(mesh->Attribute("MAGIC")!=NULL && stricmp(mesh->Attribute("MAGIC"),Cal::MESH_XMLFILE_MAGIC)!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
         return 0;
  }

  if(mesh->Attribute("VERSION")!=NULL && atoi(mesh->Attribute("VERSION")) < Cal::EARLIEST_COMPATIBLE_FILE_VERSION )
  {
  CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "xml");
         return 0;
  }

  // get the number of submeshes
  int submeshCount = atoi(mesh->Attribute("NUMSUBMESH"));

  // allocate a new core mesh instance
  CalCoreMesh *pCoreMesh = new CalCoreMesh();
  if(pCoreMesh == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  vsxTiXmlElement*submesh = mesh->FirstChildElement();

  // load all core submeshes
  int submeshId;
  for(submeshId = 0; submeshId < submeshCount; ++submeshId)
  {
    if(!submesh || stricmp(submesh->Value(),"SUBMESH")!=0)
    {
      delete pCoreMesh;
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      return 0;
    }

    // get the material thread id of the submesh
    int coreMaterialThreadId = atoi(submesh->Attribute("MATERIAL"));

    // get the number of vertices, faces, level-of-details and springs
    int vertexCount = atoi(submesh->Attribute("NUMVERTICES"));

    int faceCount = atoi(submesh->Attribute("NUMFACES"));

    int lodCount = atoi(submesh->Attribute("NUMLODSTEPS"));

    int springCount = atoi(submesh->Attribute("NUMSPRINGS"));

    int textureCoordinateCount = atoi(submesh->Attribute("NUMTEXCOORDS"));

    // allocate a new core submesh instance
    CalCoreSubmesh *pCoreSubmesh = new CalCoreSubmesh();
    if(pCoreSubmesh == 0)
    {
      CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
      delete pCoreMesh;
      return 0;
    }

    // set the LOD step count
    pCoreSubmesh->setLodCount(lodCount);

    // set the core material id
    pCoreSubmesh->setCoreMaterialThreadId(coreMaterialThreadId);

    // reserve memory for all the submesh data
    if(!pCoreSubmesh->reserve(vertexCount, textureCoordinateCount, faceCount, springCount))
    {
      CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__, "xml");
      delete pCoreMesh;
      delete pCoreSubmesh;
      return 0;
    }

    vsxTiXmlElement *vertex = submesh->FirstChildElement();
    
    // load all vertices and their influences
    int vertexId;
    for(vertexId = 0; vertexId < vertexCount; ++vertexId)
    {
      if(!vertex || stricmp(vertex->Value(),"VERTEX")!=0)
      {
        delete pCoreMesh;
          delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }      

      CalCoreSubmesh::Vertex Vertex;

      vsxTiXmlElement *pos= vertex->FirstChildElement();
      if(!pos || stricmp(pos->Value(),"POS")!=0)
      {
        delete pCoreMesh;
          delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }
      
      node = pos->FirstChild();
      if(!node)
      {
        delete pCoreMesh;
          delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }      
      vsxTiXmlText* posdata = node->ToText();
      if(!posdata)
      {
        delete pCoreMesh;
          delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }
      str.clear();
      str << posdata->Value();
      str >> Vertex.position.x >> Vertex.position.y >> Vertex.position.z;  

      vsxTiXmlElement *norm= pos->NextSiblingElement();
      if(!norm||stricmp(norm->Value(),"NORM")!=0)
      {
        delete pCoreMesh;
          delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }
      
      node = norm->FirstChild();
      if(!norm)
      {
        delete pCoreMesh;
          delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }
      vsxTiXmlText* normdata = node->ToText();
      if(!normdata)
      {
        delete pCoreMesh;
          delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }
      str.clear();
      str << normdata->Value();
      str >> Vertex.normal.x >> Vertex.normal.y >> Vertex.normal.z;

      vsxTiXmlElement *collapse= norm->NextSiblingElement();
      if(collapse && stricmp(collapse->Value(),"COLLAPSEID")==0)
      {
        node = collapse->FirstChild();
        if(!node)
        {
            delete pCoreMesh;
              delete pCoreSubmesh;
            CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
            return 0;
        }
        vsxTiXmlText* collapseid = node->ToText();
        if(!collapseid)
        {
            delete pCoreMesh;
              delete pCoreSubmesh;
            CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
            return 0;
        }
        Vertex.collapseId = atoi(collapseid->Value());

        vsxTiXmlElement *collapseCount= collapse->NextSiblingElement();
        if(!collapseCount|| stricmp(collapseCount->Value(),"COLLAPSECOUNT")!=0)
        {
            delete pCoreMesh;
            delete pCoreSubmesh;
            CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
            return 0;
        }

        node = collapseCount->FirstChild();
        if(!node)
        {
            delete pCoreMesh;
              delete pCoreSubmesh;
            CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
            return 0;
        }
        vsxTiXmlText* collapseCountdata = node->ToText();
        if(!collapseCountdata)
        {
            delete pCoreMesh;
              delete pCoreSubmesh;
            CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
            return 0;
        }
        Vertex.faceCollapseCount= atoi(collapseCountdata->Value());
        collapse = collapseCount->NextSiblingElement();        
      }
      else
      {
        Vertex.collapseId=-1;
        Vertex.faceCollapseCount=0;
      }


      vsxTiXmlElement *texcoord = collapse;

      // load all texture coordinates of the vertex
      int textureCoordinateId;
      for(textureCoordinateId = 0; textureCoordinateId < textureCoordinateCount; ++textureCoordinateId)
      {
        CalCoreSubmesh::TextureCoordinate textureCoordinate;
        // load data of the influence
        if(!texcoord || stricmp(texcoord->Value(),"TEXCOORD")!=0)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }

        node = texcoord->FirstChild();
        if(!node)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }
        vsxTiXmlText* texcoorddata = node->ToText();
        if(!texcoorddata)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }
        str.clear();
        str << texcoorddata->Value();
        str >> textureCoordinate.u >> textureCoordinate.v;

        if (loadingMode & LOADER_INVERT_V_COORD)
        {
          textureCoordinate.v = 1.0f - textureCoordinate.v;
        }


        // set texture coordinate in the core submesh instance
        pCoreSubmesh->setTextureCoordinate(vertexId, textureCoordinateId, textureCoordinate);
        texcoord = texcoord->NextSiblingElement();
      }

      // get the number of influences
      int influenceCount= atoi(vertex->Attribute("NUMINFLUENCES"));

      if(influenceCount < 0)
      {
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMesh;
        delete pCoreSubmesh;
        return 0;
      }

      // reserve memory for the influences in the vertex
      Vertex.vectorInfluence.reserve(influenceCount);
      Vertex.vectorInfluence.resize(influenceCount);

      vsxTiXmlElement *influence = texcoord;
      
      // load all influences of the vertex
      int influenceId;
      for(influenceId = 0; influenceId < influenceCount; ++influenceId)
      {
        if(!influence ||stricmp(influence->Value(),"INFLUENCE")!=0)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }

        node = influence->FirstChild();
        if(!node)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }
        vsxTiXmlText* influencedata = node->ToText();
        if(!influencedata)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }

        Vertex.vectorInfluence[influenceId].boneId = atoi(influence->Attribute("ID"));

        Vertex.vectorInfluence[influenceId].weight = (float) atof(influencedata->Value());

        influence=influence->NextSiblingElement();    
      }

      // set vertex in the core submesh instance
      pCoreSubmesh->setVertex(vertexId, Vertex);

      vsxTiXmlElement *physique = influence;

        

      // load the physical property of the vertex if there are springs in the core submesh
      if(springCount > 0)
      {
        CalCoreSubmesh::PhysicalProperty physicalProperty;
        
        if(!physique || stricmp(physique->Value(),"PHYSIQUE")!=0)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }
        node = physique->FirstChild();
        if(!node)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }
        vsxTiXmlText* physiquedata = node->ToText();
        if(!physiquedata)
        {
          delete pCoreMesh;
          delete pCoreSubmesh;
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
        }

        physicalProperty.weight = (float) atof(physiquedata->Value());

        // set the physical property in the core submesh instance
        pCoreSubmesh->setPhysicalProperty(vertexId, physicalProperty);          
        
      }


      vertex = vertex->NextSiblingElement();
   }

   vsxTiXmlElement *spring= vertex;

   // load all springs
   int springId;
   for(springId = 0; springId < springCount; ++springId)
   {
      CalCoreSubmesh::Spring Spring;
      if(!spring ||stricmp(spring->Value(),"SPRING")!=0)
      {
        delete pCoreMesh;
        delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }
      str.clear();
      str << spring->Attribute("VERTEXID");
      str >> Spring.vertexId[0] >> Spring.vertexId[1];
      Spring.springCoefficient = (float) atof(spring->Attribute("COEF"));
      Spring.idleLength = (float) atof(spring->Attribute("LENGTH"));

      // set spring in the core submesh instance
      pCoreSubmesh->setSpring(springId, Spring);
      spring = spring->NextSiblingElement();
   }

    vsxTiXmlElement *face = spring;

   // load all faces
   int faceId;
   for(faceId = 0; faceId < faceCount; ++faceId)
   {
      CalCoreSubmesh::Face Face;

      if(!face || stricmp(face->Value(),"FACE")!=0)
      {
        delete pCoreMesh;
        delete pCoreSubmesh;
        CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
      }

      int tmp[3];

      // load data of the face

      str.clear();
      str << face->Attribute("VERTEXID");
      str >> tmp[0] >> tmp [1] >> tmp[2];

      if(sizeof(CalIndex)==2)
      {
        if(tmp[0]>65535 || tmp[1]>65535 || tmp[2]>65535)
        {
          CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          delete pCoreSubmesh;
          return 0;
        }
      }
      Face.vertexId[0]=tmp[0];
      Face.vertexId[1]=tmp[1];
      Face.vertexId[2]=tmp[2];

      pCoreSubmesh->setFace(faceId, Face);

      face=face->NextSiblingElement();
    }
    submesh=submesh->NextSiblingElement();
  
    // add the core submesh to the core mesh instance
      pCoreMesh->addCoreSubmesh(pCoreSubmesh);

  }


// explicitly close the file
doc.Clear();
return pCoreMesh;
}



 /*****************************************************************************/
/** Loads a core material instance from a XML file.
  *
  * This function loads a core material instance from a XML file.
  *
  * @param strFilename The name of the file to load the core material instance
  *                    from.
  *
  * @return One of the following values:
  *         \li a pointer to the core material
  *         \li \b 0 if an error happened
  *****************************************************************************/


CalCoreMaterial *CalLoader::loadXmlCoreMaterial(const std::string& strFilename)
{
  vsxTiXmlDocument doc(strFilename);
  if(!doc.LoadFile())
  {
    CalError::setLastError(CalError::FILE_NOT_FOUND, __FILE__, __LINE__, strFilename);
    return 0;
  } 
  return loadXmlCoreMaterial_xml(doc);
}

CalCoreMaterial *CalLoader::loadXmlCoreMaterial_xml(vsxTiXmlDocument &doc)
{
  int r,g,b,a;
  std::stringstream str;
  vsxTiXmlNode* node;

  vsxTiXmlElement*material = doc.FirstChildElement();
  if(!material)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
  }

  if(stricmp(material->Value(),"HEADER")==0)
  {
    if(stricmp(material->Attribute("MAGIC"),Cal::MATERIAL_XMLFILE_MAGIC)!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
          return 0;
    }

    if(atoi(material->Attribute("VERSION")) < Cal::EARLIEST_COMPATIBLE_FILE_VERSION )
    {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "xml");
          return 0;
    }

    material = material->NextSiblingElement();
  }

  if(!material||stricmp(material->Value(),"MATERIAL")!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      return 0;
  }

  if(material->Attribute("MAGIC")!=NULL && stricmp(material->Attribute("MAGIC"),Cal::MATERIAL_XMLFILE_MAGIC)!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        return 0;
  }

  if(material->Attribute("VERSION") != NULL && atoi(material->Attribute("VERSION")) < Cal::EARLIEST_COMPATIBLE_FILE_VERSION )
  {
    CalError::setLastError(CalError::INCOMPATIBLE_FILE_VERSION, __FILE__, __LINE__, "xml");
        return 0;
  }

  CalCoreMaterial *pCoreMaterial;
  pCoreMaterial = new CalCoreMaterial();
  if(pCoreMaterial == 0)
  {
    CalError::setLastError(CalError::MEMORY_ALLOCATION_FAILED, __FILE__, __LINE__);
    return 0;
  }

  vsxTiXmlElement* ambient = material->FirstChildElement();
  if(!ambient ||stricmp(ambient->Value(),"AMBIENT")!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }

  CalCoreMaterial::Color ambientColor; 
  node = ambient->FirstChild();
  if(!node)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }
  vsxTiXmlText* ambientdata = node->ToText();
  if(!ambientdata)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }
  str << ambientdata->Value();
  str >> r >> g >> b >> a;
  ambientColor.red = (unsigned char)r;
  ambientColor.green = (unsigned char)g;
  ambientColor.blue = (unsigned char)b;
  ambientColor.alpha = (unsigned char)a; 

  vsxTiXmlElement* diffuse = ambient->NextSiblingElement();
  if(!diffuse || stricmp(diffuse->Value(),"DIFFUSE")!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }

  CalCoreMaterial::Color diffuseColor; 
  node = diffuse->FirstChild();
  if(!node)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }
  vsxTiXmlText* diffusedata = node->ToText();
  if(!diffusedata)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }
  str.clear();
  str << diffusedata->Value();
  str >> r >> g >> b >> a;
  diffuseColor.red = (unsigned char)r;
  diffuseColor.green = (unsigned char)g;
  diffuseColor.blue = (unsigned char)b;
  diffuseColor.alpha = (unsigned char)a;
  

  vsxTiXmlElement* specular = diffuse->NextSiblingElement();
  if(!specular||stricmp(specular->Value(),"SPECULAR")!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }

  CalCoreMaterial::Color specularColor; 
  node = specular->FirstChild();
  if(!node)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }
  vsxTiXmlText* speculardata = node->ToText();
  if(!speculardata)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }
  str.clear();
  str << speculardata->Value();
  str >> r >> g >> b >> a;
  specularColor.red = (unsigned char)r;
  specularColor.green = (unsigned char)g;
  specularColor.blue = (unsigned char)b;
  specularColor.alpha = (unsigned char)a;


  vsxTiXmlElement* shininess = specular->NextSiblingElement();
  if(!shininess||stricmp(shininess->Value(),"SHININESS")!=0)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }

  float fshininess;
  node = shininess->FirstChild();
  if(!node)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }
  vsxTiXmlText* shininessdata = node->ToText();
  if(!shininessdata)
  {
    CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
        delete pCoreMaterial;    
        return 0;
  }
  fshininess = (float)atof(shininessdata->Value());

  // set the colors and the shininess
  pCoreMaterial->setAmbientColor(ambientColor);
  pCoreMaterial->setDiffuseColor(diffuseColor);
  pCoreMaterial->setSpecularColor(specularColor);
  pCoreMaterial->setShininess(fshininess);
  
  std::vector<std::string> MatFileName;

  vsxTiXmlElement* map;

  for( map = shininess->NextSiblingElement();map;map = map->NextSiblingElement() )
  {
    if(!map||stricmp(map->Value(),"MAP")!=0)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      delete pCoreMaterial;    
      return 0;
    }
    

    node= map->FirstChild();
    if(!node)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      delete pCoreMaterial;    
      return 0;
    }

    vsxTiXmlText* mapfile = node->ToText();
    if(!mapfile)
    {
      CalError::setLastError(CalError::INVALID_FILE_FORMAT, __FILE__, __LINE__, "xml");
      delete pCoreMaterial;    
      return 0;
    }

    MatFileName.push_back(mapfile->Value());
  }

  pCoreMaterial->reserve(MatFileName.size());



  for (unsigned int mapId=0; mapId < MatFileName.size(); ++mapId)
  {
    CalCoreMaterial::Map Map;
    // initialize the user data
    Map.userData = 0;

    Map.strFilename= MatFileName[mapId];    

    // set map in the core material instance
    pCoreMaterial->setMap(mapId, Map);
  }

  doc.Clear();
  
  return pCoreMaterial;
}




//****************************************************************************//

