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
#ifndef vsx_nw_vector_H
#define vsx_nw_vector_H

#include <vsx_platform.h>
#include <stdio.h>

template<class T>
class vsx_nw_vector
{
  size_t allocated;
  size_t used;
  size_t allocation_increment;
  size_t timestamp;
  __attribute__((aligned(64))) T* A;

public:

  inline T* get_pointer() VSX_ALWAYS_INLINE
  {
    return A;
  }

  inline size_t get_allocated() VSX_ALWAYS_INLINE
  {
    return allocated;
  }


  inline size_t get_used() VSX_ALWAYS_INLINE
  {
    return used;
  }


  // std::vector compatibility
  inline size_t push_back(T val) VSX_ALWAYS_INLINE
  {
    (*this)[used] = val;
    return used;
  }

  inline size_t push_back_ref(T& val) VSX_ALWAYS_INLINE
  {
    (*this)[used] = val;
    return used;
  }

  inline size_t size() VSX_ALWAYS_INLINE
  {
    return used;
  }


  inline void clear() VSX_ALWAYS_INLINE
  {
    delete[] A;
    A = 0;
    allocated = used = 0;
    allocation_increment = 1;
  }


  inline void reset_used(int val = 0) VSX_ALWAYS_INLINE
  {
    if (val >= 0)
    used = val;
    else used = 0;
  }

  inline void set_allocation_increment(unsigned long new_increment)  VSX_ALWAYS_INLINE
  {
  	allocation_increment = new_increment;
  }

  inline void remove(T val) VSX_ALWAYS_INLINE
  {
    // allocate new
    T* n = new T[allocated];
    // iteration pointer
    T* p = n;
    for(unsigned long i = 0; i < used; i++)
    {
      if (A[i] != val)
      {
        *p = A[i];
        p++;
      }
    }
    used--;
    delete[] A;
    A = n;
  }

  inline void allocate( size_t index ) VSX_ALWAYS_INLINE
  {
    if (index >= allocated || allocated == 0)
    {
      // need to allocate memory
      if (A)
      {
        if (allocation_increment == 0) allocation_increment = 1;
        allocated = index+allocation_increment;
        T* B = new T[allocated];
        for (size_t i = 0; i < used; ++i) {
          B[i] = A[i];
        }
        delete[] A;
        A = B;
      }
      else
      {
        A = new T[index+allocation_increment];
        allocated = index+allocation_increment;
      }
      allocation_increment = allocation_increment << 1;
    }
    if (index >= used)
    {
      used = index+1;
    }
  }

  inline T& operator[](size_t index) VSX_ALWAYS_INLINE
  {
    allocate( index );
    return A[index];
  }

  vsx_nw_vector()
    :
    allocated(0),
    used(0),
    allocation_increment(1),
    timestamp(0),
    A(0)
  {
  }

  ~vsx_nw_vector()
  {
    if (A)
    {
      delete[] A;
      A = 0x0; // valgrind
    }
  }
};

#endif
