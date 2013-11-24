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

#ifndef VSX_RAND_H
#define VSX_RAND_H

#include <mtwist.h>

class vsx_rand
{
private:

  mt_state* state;

public:

  vsx_rand()
  {
    state = new mt_state;
    srand(1);
  }

  ~vsx_rand()
  {
    delete state;
  }

  inline void srand(uint32_t seed)
  {
    mts_seed32( state, seed );
  }

  inline uint32_t rand()
  {
    return mts_lrand( state );
  }

  inline float frand()
  {
    return (float)mts_drand( state );
  }

};


#endif
