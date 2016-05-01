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

#ifndef vsx_nw_vector_ND_H
#define vsx_nw_vector_ND_H

template<class T>
class vsx_nw_vector_nd
{
  size_t allocated = 0;
  size_t used = 0;
  size_t allocation_increment = 1;
  size_t timestamp = 0;
  T* A = 0;
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

  size_t size() {
    return used;
  }

  inline bool has(T o)
  {
    for (size_t i = 0; i < used; i++)
      if (A[i] == o)
        return true;
    return false;
  }

  void clear() {
    if (A)
    delete[] A;
    A = 0;
    allocated = used = 0;
    allocation_increment = 1;
  }

  void reset_used(int val = 0) {
    used = val;
  }

  void set_allocation_increment(unsigned long new_increment) {
    allocation_increment = new_increment;
  }

  inline void remove_value(T value) VSX_ALWAYS_INLINE
  {
    // allocate new
    T* n = new T[allocated];
    // iteration pointer
    T* p = n;
    for(unsigned long i = 0; i < used; i++)
    {
      if (A[i] != value)
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
      // need to allocate stuff here
      if (A) 
      {
        if (allocation_increment == 0) 
          allocation_increment = 1;

        allocated = index+allocation_increment;
        T* B = new T[allocated];
        for (size_t i = 0; i < used; ++i) {
          B[i] = A[i];
        }
        delete[] A;
        A = B;
      } else {
        A = new T[index+allocation_increment];
        allocated = index+allocation_increment;
      }
      if (allocation_increment < 64)
        allocation_increment *= 2;
      else
        allocation_increment = (size_t)((float)allocation_increment * 1.3f);
    }
    if (index >= used)
      used = index+1;
  }

  T& operator[](size_t index) {
    allocate(index);
    return A[index];
  }

};

#endif
