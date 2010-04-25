//****************************************************************************//
// coretrack.h                                                                //
// Copyright (C) 2001, 2002 Bruno 'Beosil' Heidelberger                       //
//****************************************************************************//
// This library is free software; you can redistribute it and/or modify it    //
// under the terms of the GNU Lesser General Public License as published by   //
// the Free Software Foundation; either version 2.1 of the License, or (at    //
// your option) any later version.                                            //
//****************************************************************************//

#ifndef CAL_CORETRACK_H
#define CAL_CORETRACK_H


#include "cal3d/global.h"
#include "cal3d/matrix.h"
#include "cal3d/vector.h"
#include "cal3d/quaternion.h"


class CalCoreBone;
class CalCoreKeyframe;


class CAL3D_API CalCoreTrack
{
public:
  CalCoreTrack();
  ~CalCoreTrack();

  bool getState(float time, CalVector& translation, CalQuaternion& rotation);

  int getCoreBoneId();
  bool setCoreBoneId(int coreBoneId);
  
  int getCoreKeyframeCount();
  CalCoreKeyframe* getCoreKeyframe(int idx);

  bool addCoreKeyframe(CalCoreKeyframe *pCoreKeyframe);

  void scale(float factor);

private:
  std::vector<CalCoreKeyframe*>::iterator getUpperBound(float time);

  /// The index of the associated CoreBone in the CoreSkeleton.
  int m_coreBoneId;

  /// List of keyframes, always sorted by time.
  std::vector<CalCoreKeyframe*> m_keyframes;
};

#endif

//****************************************************************************//
