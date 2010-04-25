//****************************************************************************//
// animation.cpp                                                              //
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


#include "cal3d/animation.h"
#include "cal3d/error.h"
#include "cal3d/coreanimation.h"
#include "cal3d/animcallback.h"

CalAnimation::CalAnimation(CalCoreAnimation* pCoreAnimation)
  : m_pCoreAnimation(pCoreAnimation)
  , m_type(TYPE_NONE)
  , m_state(STATE_NONE)
  , m_time(0.0f)
  , m_timeFactor(1.0f)
  , m_weight(0.0f)
{
  assert(pCoreAnimation);

  std::vector<CalCoreAnimation::CallbackRecord>& list = m_pCoreAnimation->getCallbackList();
  for (size_t i=0; i<list.size(); i++)
    m_lastCallbackTimes.push_back(0.0F);  // build up the last called list
}



 /*****************************************************************************/
/** Provides access to the core animation.
  *
  * This function returns the core animation on which this animation instance
  * is based on.
  *
  * @return One of the following values:
  *         \li a pointer to the core animation
  *         \li \b 0 if an error happend
  *****************************************************************************/

CalCoreAnimation *CalAnimation::getCoreAnimation()
{
  return m_pCoreAnimation;
}

 /*****************************************************************************/
/** Returns the state.
  *
  * This function returns the state of the animation instance.
  *
  * @return One of the following states:
  *         \li \b STATE_NONE
  *         \li \b STATE_SYNC
  *         \li \b STATE_ASYNC
  *         \li \b STATE_IN
  *         \li \b STATE_STEADY
  *         \li \b STATE_OUT
  *****************************************************************************/

CalAnimation::State CalAnimation::getState()
{
  return m_state;
}

 /*****************************************************************************/
/** Returns the time.
  *
  * This function returns the time of the animation instance.
  *
  * @return The time in seconds.
  *****************************************************************************/

float CalAnimation::getTime()
{
  return m_time;
}

 /*****************************************************************************/
/** Returns the type.
  *
  * This function returns the type of the animation instance.
  *
  * @return One of the following types:
  *         \li \b TYPE_NONE
  *         \li \b TYPE_CYCLE
  *         \li \b TYPE_POSE
  *         \li \b TYPE_ACTION
  *****************************************************************************/

CalAnimation::Type CalAnimation::getType()
{
  return m_type;
}

 /*****************************************************************************/
/** Returns the weight.
  *
  * This function returns the weight of the animation instance.
  *
  * @return The weight.
  *****************************************************************************/

float CalAnimation::getWeight()
{
  return m_weight;
}

 /*****************************************************************************/
/** Set the time.
  *
  * This function set the time of the animation instance.
  *
  *****************************************************************************/

void CalAnimation::setTime(float time)
{
    m_time = time;
}

/*****************************************************************************/
/** Set the time factor.
  * 
  * This function sets the time factor of the animation instance.
  * this time factor affect only sync animation
  *
  *****************************************************************************/

void CalAnimation::setTimeFactor(float timeFactor)
{
    m_timeFactor = timeFactor;
}

/*****************************************************************************/
/** Get the time factor.
  * 
  * This function return the time factor of the animation instance.
  *
  *****************************************************************************/

float CalAnimation::getTimeFactor()
{
    return m_timeFactor;
}

void CalAnimation::checkCallbacks(float animationTime,CalModel *model)
{
  std::vector<CalCoreAnimation::CallbackRecord>& list = m_pCoreAnimation->getCallbackList();
  
  for (size_t i=0; i<list.size(); i++)
  {
    if (animationTime > 0 && animationTime < m_lastCallbackTimes[i])  // looped
        m_lastCallbackTimes[i] -= m_pCoreAnimation->getDuration();
    else if (animationTime < 0 && animationTime > m_lastCallbackTimes[i])     // reverse-looped  
        m_lastCallbackTimes[i] += m_pCoreAnimation->getDuration();
  
    if ((animationTime >= 0 && animationTime >= m_lastCallbackTimes[i] + list[i].min_interval) ||
        (animationTime <  0 && animationTime <= m_lastCallbackTimes[i] - list[i].min_interval))
    {
      list[i].callback->AnimationUpdate(animationTime,model);
      m_lastCallbackTimes[i] = animationTime;
    }
  }
}

void CalAnimation::completeCallbacks(CalModel *model)
{
  std::vector<CalCoreAnimation::CallbackRecord>& list = m_pCoreAnimation->getCallbackList();
  for (size_t i=0; i<list.size(); i++)
    list[i].callback->AnimationComplete(model);
}

//****************************************************************************//
