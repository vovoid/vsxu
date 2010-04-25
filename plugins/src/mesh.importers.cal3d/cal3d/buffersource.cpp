//****************************************************************************//
// buffersource.h                                                            //
// Copyright (C) 2001-2003 Bruno 'Beosil' Heidelberger                       //
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

#include "cal3d/buffersource.h"
#include "cal3d/error.h"

 /*****************************************************************************/
/** Constructs a buffer source instance from an existing memory buffer.
  *
  * This function is the only constructor of the buffer source.
  *
  * @param inputBuffer The input buffer to read from
  *****************************************************************************/

CalBufferSource::CalBufferSource(void* inputBuffer)
  : mInputBuffer(inputBuffer), mOffset(0)
{
}


/**
 * Destruct the CalBufferSource. Note that the memory is not deleted here;
 * this should be handled externally.
 */

CalBufferSource::~CalBufferSource()
{
}


 /*****************************************************************************/
/** Checks whether the data source is in a good state.
  *
  * This function checks if the buffer is NULL or not.
  *
  * @return One of the following values:
  *         \li \b true if data source is in a good state
  *         \li \b false if not
  *****************************************************************************/

bool CalBufferSource::ok() const
{
   if (mInputBuffer == NULL)
      return false;

   return true;
}

 /*****************************************************************************/
/** Sets the error code and message related to a memory buffer source.
  *
  *****************************************************************************/

void CalBufferSource::setError() const
{
   CalError::setLastError(CalError::NULL_BUFFER, __FILE__, __LINE__);
}

 /*****************************************************************************/
/** Reads a number of bytes.
  *
  * This function reads a given number of bytes from this data source.
  *
  * @param pBuffer A pointer to the buffer where the bytes are stored into.
  * @param length The number of bytes that should be read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalBufferSource::readBytes(void* pBuffer, int length)
{
   //Check that the buffer and the target are usable
   if (!ok() || (pBuffer == NULL)) return false;
   
   bool result = CalPlatform::readBytes( ((char*)mInputBuffer+mOffset), pBuffer, length );
   mOffset += length;

   return result;
}

 /*****************************************************************************/
/** Reads a float.
  *
  * This function reads a float from this data source.
  *
  * @param value A reference to the float into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalBufferSource::readFloat(float& value)
{
   //Check that the buffer is usable
   if (!ok()) return false;

   bool result = CalPlatform::readFloat( ((char*)mInputBuffer+mOffset), value );
   mOffset += 4;

   return result;
}

 /*****************************************************************************/
/** Reads an integer.
  *
  * This function reads an integer from this data source.
  *
  * @param value A reference to the integer into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalBufferSource::readInteger(int& value)
{
   //Check that the buffer is usable
   if (!ok()) return false;

   bool result = CalPlatform::readInteger( ((char*)mInputBuffer+mOffset), value );
   mOffset += 4;

   return result;
}

 /*****************************************************************************/
/** Reads a string.
  *
  * This function reads a string from this data source.
  *
  * @param value A reference to the string into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalBufferSource::readString(std::string& strValue)
{
   //Check that the buffer is usable
   if (!ok()) return false;

   bool result = CalPlatform::readString( ((char*)mInputBuffer+mOffset), strValue );

   mOffset += (strValue.length() + 4 + 1); // +1 is for Null-terminator
   
   return result;
}
