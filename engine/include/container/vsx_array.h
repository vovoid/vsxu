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

#ifndef VSX_ARRAY_H
#define VSX_ARRAY_H

#include <stdlib.h>
#include <string.h>
#include <vsx_platform.h>
// VSX Array class
//
// This is a special case array aimed at speed - for mesh data etc.
// It uses a nasty trick - allocating class pointers with malloc.
// Rules if you want to avoid segfault:
// * DON'T STORE POINTERS TO CLASSES WITH VIRTUAL FUNCTIONS
// * DON'T POINT TO ANY ELEMENT/DATA STORED IN THE ARRAY
//   (data is realloc'd, such pointers would be invalid)
// Now you've been warned, use it for speed!

template<class T>
class vsx_array
{
  size_t allocated;
  size_t used;
  size_t allocation_increment;
  size_t data_volatile;
  size_t timestamp;
  __attribute__((aligned(64))) T* A;

public:

  void set_allocation_increment(unsigned long new_increment) VSX_ALWAYS_INLINE
  {
  	allocation_increment = new_increment;
  }

  void set_data(T* nA, int nsize) VSX_ALWAYS_INLINE
  {
    if (A && !data_volatile)
      free(A);
  	A = nA;
  	used = allocated = nsize;
  }

  // clones another array of same type into this one
  void clone(vsx_array<T>* F) VSX_ALWAYS_INLINE
  {
    allocate(F->size());
    memcpy((void*)A, (void*)(F->get_pointer()), sizeof(T) * used);
  }

  void set_volatile() VSX_ALWAYS_INLINE
  {
    if (0 == data_volatile && A && allocated)
    {
      clear();
    }
    data_volatile = 1;
  }

  void unset_volatile() VSX_ALWAYS_INLINE
  {
    if (!data_volatile)
      return;
    A = 0;
    allocated = 0;
    data_volatile = 0;
  }

  inline T* get_pointer() VSX_ALWAYS_INLINE
  {
    return A;
  }

  inline T* get_end_pointer() VSX_ALWAYS_INLINE
  {
    return &A[used-1];
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

  inline size_t size() VSX_ALWAYS_INLINE
  {
    return used;
  }

  inline size_t get_sizeof() VSX_ALWAYS_INLINE
  {
    return used * sizeof(T);
  }

  inline void clear() VSX_ALWAYS_INLINE
  {
    if (data_volatile) { return; }
  	if (A)
    free(A);
    A = 0;
    used = allocated = 0;
    allocation_increment = 1;
  }

  inline void memory_clear(int c = 0) VSX_ALWAYS_INLINE
  {
    memset(A, c, sizeof(T) * allocated);
  }

  inline void reset_used(size_t val = 0) VSX_ALWAYS_INLINE
  {
  	// TODO: if value larger than count of allocated items in memory handle this some way
    used = val;
  }


  inline void allocate_bytes(size_t b) VSX_ALWAYS_INLINE
  {
    if (A)
    {
      free(A);
    }
    A = (T*)malloc( b );
    used = b / sizeof(T);
    allocated = used;
  }

  inline void allocate(size_t index) VSX_ALWAYS_INLINE
  {
    if (index >= allocated || allocated == 0)
    {
    	if (allocation_increment == 0) allocation_increment = 1;
      if (A)
      {
        allocated = index + allocation_increment;
        A = (T*)realloc(A,sizeof(T)*allocated);
      } else
      {
        A = (T*)malloc(sizeof(T)*(index+allocation_increment));
        allocated = index+allocation_increment;
      }
      allocation_increment = allocation_increment << 1;
      #ifdef VSX_ARRAY_ALLOCATE_CONSERVATIVE
        if (allocation_increment > 2)
          allocation_increment = 2;
      #endif
    }
    if (index >= used) {
      used = index+1;
    }
  }

  T& operator[](unsigned long index)
  {
    allocate(index);
    return A[index];
  }

  vsx_array() :
    allocated(0),
    used(0),
    allocation_increment(1),
    data_volatile(0),
    timestamp(0),
    A(0)
  {
  }

  ~vsx_array()
  {
    if (data_volatile) return;
  	if (A) free(A);
  }
};



#endif
