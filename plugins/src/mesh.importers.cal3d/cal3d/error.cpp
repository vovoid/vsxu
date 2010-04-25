//****************************************************************************//
// error.cpp                                                                  //
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

namespace
{
    CalError::Code m_lastErrorCode = CalError::OK;
    std::string m_strLastErrorFile;
    int m_lastErrorLine = -1;
    std::string m_strLastErrorText;
}

 /*****************************************************************************/
/** Returns the code of the last error.
  *
  * This function returns the code of the last error that occured inside the
  * library.
  *
  * @return The code of the last error.
  *****************************************************************************/

CalError::Code CalError::getLastErrorCode()
{
  return m_lastErrorCode;
}

 /*****************************************************************************/
/** Returns a description of the last error.
  *
  * This function returns a short description of the last error that occured
  * inside the library.
  *
  * @return The description of the last error.
  *****************************************************************************/

std::string CalError::getErrorDescription(Code code)
{
  switch(code)
  {
    case OK:                         return "No error found";
    case INTERNAL:                   return "Internal error";
    case INVALID_HANDLE:             return "Invalid handle as argument";
    case MEMORY_ALLOCATION_FAILED:   return "Memory allocation failed";
    case FILE_NOT_FOUND:             return "File not found";
    case INVALID_FILE_FORMAT:        return "Invalid file format";
    case FILE_PARSER_FAILED:         return "Parser failed to process file";
    case INDEX_BUILD_FAILED:         return "Building of the index failed";
    case NO_PARSER_DOCUMENT:         return "There is no document to parse";
    case INVALID_ANIMATION_DURATION: return "The duration of the animation is invalid";
    case BONE_NOT_FOUND:             return "Bone not found";
    case INVALID_ATTRIBUTE_VALUE:    return "Invalid attribute value";
    case INVALID_KEYFRAME_COUNT:     return "Invalid number of keyframes";
    case INVALID_ANIMATION_TYPE:     return "Invalid animation type";
    case FILE_CREATION_FAILED:       return "Failed to create file";
    case FILE_WRITING_FAILED:        return "Failed to write to file";
    case INCOMPATIBLE_FILE_VERSION:  return "Incompatible file version";
    case NO_MESH_IN_MODEL:           return "No mesh attached to the model";
    case BAD_DATA_SOURCE:            return "Cannot read from data source";
    case NULL_BUFFER:                return "Memory buffer is null";
    case INVALID_MIXER_TYPE:         return "The CalModel mixer is not a CalMixer instance";
    default:                         return "Unknown error";
  }
}

 /*****************************************************************************/
/** Returns the name of the file where the last error occured.
  *
  * This function returns the name of the file where the last error occured.
  *
  * @return The name of the file where the last error occured.
  *****************************************************************************/

const std::string& CalError::getLastErrorFile()
{
  return m_strLastErrorFile;
}

 /*****************************************************************************/
/** Returns the line number where the last error occured.
  *
  * This function returns the line number where the last error occured.
  *
  * @return The line number where the last error occured.
  *****************************************************************************/

int CalError::getLastErrorLine()
{
  return m_lastErrorLine;
}

 /*****************************************************************************/
/** Returns the supplementary text of the last error.
  *
  * This function returns the suppementary text of the last error occured
  * inside the library.
  *
  * @return The supplementary text of the last error.
  *****************************************************************************/

const std::string& CalError::getLastErrorText()
{
  return m_strLastErrorText;
}

 /*****************************************************************************/
/** Dumps all information about the last error to the standard output.
  *
  * This function dumps all the information about the last error that occured
  * inside the library to the standard output.
  *****************************************************************************/

void CalError::printLastError()
{
  std::cout << "cal3d : " << getLastErrorDescription();

  // only print supplementary information if there is some
  if(m_strLastErrorText.size() > 0)
  {
    std::cout << " '" << m_strLastErrorText << "'";
  }

  std::cout << " in " << m_strLastErrorFile << "(" << m_lastErrorLine << ")" << std::endl;
}

 /*****************************************************************************/
/** Sets all the information about the last error.
  *
  * This function sets all the information about the last error that occured
  * inside the library.
  *
  * @param code The code of the last error.
  * @param strFile The file where the last error occured.
  * @param line The line number where the last error occured.
  * @param strText The supplementary text of the last error.
  *****************************************************************************/

void CalError::setLastError(Code code, const std::string& strFile, int line, const std::string& strText)
{
  if(code >= MAX_ERROR_CODE) code = INTERNAL;

  m_lastErrorCode = code;
  m_strLastErrorFile = strFile;
  m_lastErrorLine = line;
  m_strLastErrorText = strText;
}

//****************************************************************************//
