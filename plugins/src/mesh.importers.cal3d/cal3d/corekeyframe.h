//****************************************************************************//
// corekeyframe.h                                                             //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_COREKEYFRAME_H
#define CAL_COREKEYFRAME_H


#include "cal3d/global.h"
#include "cal3d/matrix.h"
#include "cal3d/vector.h"
#include "cal3d/quaternion.h"


class CAL3D_API CalCoreKeyframe
{
public:
  CalCoreKeyframe();
  ~CalCoreKeyframe() { }

  const CalQuaternion& getRotation();
  float getTime();
  const CalVector& getTranslation();
  void setRotation(const CalQuaternion& rotation);
  void setTime(float time);
  void setTranslation(const CalVector& translation);

private:
  float m_time;
  CalVector m_translation;
  CalQuaternion m_rotation;
};

#endif

//****************************************************************************//
