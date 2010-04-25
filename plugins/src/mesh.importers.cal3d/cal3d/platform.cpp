//****************************************************************************//
// platform.cpp                                                               //
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

#include "cal3d/platform.h"

 /*****************************************************************************/
/** Constructs the platform instance.
  *
  * This function is the default constructor of the platform instance.
  *****************************************************************************/

CalPlatform::CalPlatform()
{
}

 /*****************************************************************************/
/** Destructs the platform instance.
  *
  * This function is the destructor of the platform instance.
  *****************************************************************************/

CalPlatform::~CalPlatform()
{
}

 /*****************************************************************************/
/** Reads a number of bytes.
  *
  * This function reads a given number of bytes from an input stream.
  *
  * @param input The stream to read the bytes from.
  * @param pBuffer A pointer to the buffer where the bytes are stored into.
  * @param length The number of bytes that should be read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPlatform::readBytes(std::istream& input, void *pBuffer, int length)
{
  input.read((char *)pBuffer, length);

  return !input ? false : true;
}

 /*****************************************************************************/
/** Reads a float.
  *
  * This function reads a float from an input stream.
  *
  * @param input The input stream to read the float from.
  * @param value A reference to the float into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPlatform::readFloat(std::istream& input, float& value)
{
  input.read((char *)&value, 4);

#ifdef CAL3D_BIG_ENDIAN
  float x = value ;
  ((char*)&value)[0] = ((char*)&x)[3] ;
  ((char*)&value)[1] = ((char*)&x)[2] ;
  ((char*)&value)[2] = ((char*)&x)[1] ;
  ((char*)&value)[3] = ((char*)&x)[0] ;  
#endif

  return !input ? false : true;
}

 /*****************************************************************************/
/** Reads an integer.
  *
  * This function reads an integer from an input stream.
  *
  * @param input The input stream to read the integer from.
  * @param value A reference to the integer into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPlatform::readInteger(std::istream& input, int& value)
{
  input.read((char *)&value, 4);

#ifdef CAL3D_BIG_ENDIAN
  int x = value ;
  ((char*)&value)[0] = ((char*)&x)[3] ;
  ((char*)&value)[1] = ((char*)&x)[2] ;
  ((char*)&value)[2] = ((char*)&x)[1] ;
  ((char*)&value)[3] = ((char*)&x)[0] ;
#endif

  return !input ? false : true;
}

 /*****************************************************************************/
/** Reads a string.
  *
  * This function reads a string from an input stream.
  *
  * @param input The input stream to read the string from.
  * @param value A reference to the string into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPlatform::readString(std::istream& input, std::string& strValue)
{
  // get the string length
  int length;
  input.read((char *)&length, 4);

#ifdef CAL3D_BIG_ENDIAN
  int x = length ;
  ((char*)&length)[0] = ((char*)&x)[3] ;
  ((char*)&length)[1] = ((char*)&x)[2] ;
  ((char*)&length)[2] = ((char*)&x)[1] ;
  ((char*)&length)[3] = ((char*)&x)[0] ;
#endif

  if(length < 0) return false;

  // read the string
  char *strBuffer;
  strBuffer = new char[length];
  input.read(strBuffer, length);
  strValue = strBuffer;
  delete [] strBuffer;

  return true;
}

 /*****************************************************************************/
/** Reads a number of bytes.
  *
  * This function reads a given number of bytes from a memory buffer.
  *
  * @param input The buffer to read the bytes from.
  * @param pBuffer A pointer to the buffer where the bytes are stored into.
  * @param length The number of bytes that should be read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if the input or destination buffer is NULL
  *****************************************************************************/

bool CalPlatform::readBytes(char* input, void *pBuffer, int length)
{
  if ((input == NULL) || (pBuffer == NULL)) return false;

  memcpy( pBuffer, input, length );

  return true;
}

 /*****************************************************************************/
/** Reads a float.
  *
  * This function reads a float from a memory buffer.
  *
  * @param input The buffer to read the float from.
  * @param value A reference to the float into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if the input buffer is NULL
  *****************************************************************************/

bool CalPlatform::readFloat(char* input, float& value)
{
  if (input == NULL) return false;

  memcpy( (void*)&value, (void*)input, 4 );

#ifdef CAL3D_BIG_ENDIAN
  float x = value ;
  ((char*)&value)[0] = ((char*)&x)[3] ;
  ((char*)&value)[1] = ((char*)&x)[2] ;
  ((char*)&value)[2] = ((char*)&x)[1] ;
  ((char*)&value)[3] = ((char*)&x)[0] ;  
#endif

  return true;
}

 /*****************************************************************************/
/** Reads an integer.
  *
  * This function reads an integer from a memory buffer.
  *
  * @param input The buffer to read the integer from.
  * @param value A reference to the integer into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if the input buffer is NULL
  *****************************************************************************/

bool CalPlatform::readInteger(char* input, int& value)
{
  if (input == NULL) return false;

  memcpy( (void*)&value, (void*)input, 4 );

#ifdef CAL3D_BIG_ENDIAN
  int x = value ;
  ((char*)&value)[0] = ((char*)&x)[3] ;
  ((char*)&value)[1] = ((char*)&x)[2] ;
  ((char*)&value)[2] = ((char*)&x)[1] ;
  ((char*)&value)[3] = ((char*)&x)[0] ;
#endif

  return true;
}

 /*****************************************************************************/
/** Reads a string.
  *
  * This function reads a string from a memory buffer.
  *
  * @param input The buffer to read the string from.
  * @param value A reference to the string into which the data is read.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if the input buffer is NULL
  *****************************************************************************/

bool CalPlatform::readString(char* input, std::string& strValue)
{
  if (input == NULL) return false;

  // get the string length
  int length;
  memcpy( (void*)&length, (void*)input, 4 );

#ifdef CAL3D_BIG_ENDIAN
  int x = length ;
  ((char*)&length)[0] = ((char*)&x)[3] ;
  ((char*)&length)[1] = ((char*)&x)[2] ;
  ((char*)&length)[2] = ((char*)&x)[1] ;
  ((char*)&length)[3] = ((char*)&x)[0] ;
#endif

  if(length < 0) return false;

  // read the string
  char *strBuffer;
  strBuffer = new char[length];

  //offset the read by 4 bytes (skip over the length integer)
  memcpy( (void*)strBuffer, (char*)input+4, length );

  //skip over the first 4 bytes
  //KWF char* strTemp = &strBuffer[4];

  strValue = strBuffer;
  delete [] strBuffer;

  return true;
}


 /*****************************************************************************/
/** Writes a number of bytes.
  *
  * This function writes a given number of bytes to an output stream.
  *
  * @param output The output stream to write the bytes to.
  * @param pBuffer A pointer to the byte buffer that should be written.
  * @param length The number of bytes that should be written.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPlatform::writeBytes(std::ostream& output, const void *pBuffer, int length)
{
  output.write((char *)pBuffer, length);
  return !output ? false : true;
}

 /*****************************************************************************/
/** Writes a float.
  *
  * This function writes a float to an output stream.
  *
  * @param output The output stream to write the float to.
  * @param value The float that should be written..
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPlatform::writeFloat(std::ostream& output, float value)
{

#ifdef CAL3D_BIG_ENDIAN
  float x = value ;
  ((char*)&value)[0] = ((char*)&x)[3] ;
  ((char*)&value)[1] = ((char*)&x)[2] ;
  ((char*)&value)[2] = ((char*)&x)[1] ;
  ((char*)&value)[3] = ((char*)&x)[0] ;  
#endif

  output.write((char *)&value, 4);
  return !output ? false : true;
}

 /*****************************************************************************/
/** Writes an integer.
  *
  * This function writes an integer to an output stream.
  *
  * @param file The output stream to write the integer to.
  * @param value The integer that should be written.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPlatform::writeInteger(std::ostream& output, int value)
{

#ifdef CAL3D_BIG_ENDIAN
  int x = value ;
  ((char*)&value)[0] = ((char*)&x)[3] ;
  ((char*)&value)[1] = ((char*)&x)[2] ;
  ((char*)&value)[2] = ((char*)&x)[1] ;
  ((char*)&value)[3] = ((char*)&x)[0] ;
#endif

  output.write((char *)&value, 4);
  return !output ? false : true;
}

 /*****************************************************************************/
/** Writes a string.
  *
  * This function writes a string to an output stream.
  *
  * @param file The output stream to write the string to.
  * @param value A reference to the string that should be written.
  *
  * @return One of the following values:
  *         \li \b true if successful
  *         \li \b false if an error happend
  *****************************************************************************/

bool CalPlatform::writeString(std::ostream& output, const std::string& strValue)
{
  // get the string length
  int length;
  length = strValue.size() + 1;

#ifdef CAL3D_BIG_ENDIAN
  int x = length ;
  ((char*)&length)[0] = ((char*)&x)[3] ;
  ((char*)&length)[1] = ((char*)&x)[2] ;
  ((char*)&length)[2] = ((char*)&x)[1] ;
  ((char*)&length)[3] = ((char*)&x)[0] ;
#endif

  output.write((char *)&length, 4);
  output.write(strValue.c_str(), strValue.size()+1);

  return !output ? false : true;
}

//****************************************************************************//
