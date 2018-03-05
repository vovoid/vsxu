/**
* Project: VSXu Engine: Realtime modular visual programming engine.
*
* This file is part of Vovoid VSXu Engine.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies AB Copyright (C) 2003-2013
* @see The GNU Lesser General Public License (LGPL)
*
* VSXu Engine is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU Lesser General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#pragma once

#include <stdint.h>

class vsx_rand
{
private:

  void* state = 0x0;

public:

  vsx_rand();
  ~vsx_rand();

  void srand(uint32_t seed);
  uint32_t rand();

  /**
   * @brief drand
   * @return a value between 0 and 1
   */
  double drand();

  /**
   * @brief drand_c
   * @return a value between -0.5 and 0.5
   */
  double drand_c();

  /**
   * @brief frand
   * @return a value between 0 and 1
   */
  float frand();

  /**
   * @brief frand_c
   * @return a value between -0.5f and 0.5f
   */
  float frand_c();
};
