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

#include <stdlib.h>
#include <string.h>
#include <vsx_platform.h>
#include <tools/vsx_req.h>
#include <stdio.h>
#include <iomanip>

#include <wchar.h>

template<class T>
class vsx_nw_vector
{
  size_t allocated = 0;
  size_t used = 0;
  size_t allocation_increment = 1;
  size_t data_volatile = 0;
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

  inline size_t move_back(T&& val) VSX_ALWAYS_INLINE
  {
    (*this)[used] = std::move(val);
    return used;
  }

  inline void push_front(T& val)
  {
    insert(0, val);
  }

  inline void pop_back()
  {
    req(used);
    used--;
  }

  inline T pop_front()
  {
    if (!used)
      return std::move(T());
    T v = std::move(A[0]);
    remove_index(0);
    return v;
  }

  inline T* front()
  {
    if (!used)
      return 0x0;

    return &A[0];
  }

  inline T* back()
  {
    if (!used)
      return 0x0;

    return &A[used-1];
  }

  inline size_t size() VSX_ALWAYS_INLINE
  {
    return used;
  }

  inline size_t get_sizeof() VSX_ALWAYS_INLINE
  {
    return used * sizeof(T);
  }

  inline bool has(T o)
  {
    for (size_t i = 0; i < used; i++)
      if (A[i] == o)
        return true;
    return false;
  }

  inline void clear() VSX_ALWAYS_INLINE
  {
    req(!data_volatile);

    if (!A)
      return;
    delete[] A;
    A = 0;
    allocated = used = 0;
    allocation_increment = 1;
  }


  inline void reset_used(size_t val = 0) VSX_ALWAYS_INLINE
  {
     used = val;
  }

  inline void set_allocation_increment(unsigned long new_increment)  VSX_ALWAYS_INLINE
  {
  	allocation_increment = new_increment;
  }

  void set_data(T* nA, size_t nsize) VSX_ALWAYS_INLINE
  {
    if (A && !data_volatile)
      delete[] A;

    A = nA;
    used = allocated = nsize;
  }

  void set_volatile() VSX_ALWAYS_INLINE
  {
    if (0 == data_volatile && A && allocated)
      clear();
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

  inline void remove_value(T value) VSX_ALWAYS_INLINE
  {
    req(!data_volatile);
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

  inline void remove_index(size_t index)
  {
    req(!data_volatile);
    req(index < used);
    for (size_t i = index; i < used-1; i++)
      A[i] = std::move(A[i+1]);
    used--;
  }

  inline void insert(size_t index, T value)
  {
    req(!data_volatile);
    req(index < used);
    allocate(used);
    for (size_t i = used; i > index; i--)
      A[i] = std::move(A[i-1]);
    A[index] = value;
  }



  inline void allocate( size_t index ) VSX_ALWAYS_INLINE
  {
    req(!data_volatile);
    if (index >= allocated || allocated == 0)
    {
      if (A)
      {
        if (allocation_increment == 0)
          allocation_increment = 1;

        allocated = index+allocation_increment;
        T* B = new T[allocated];

        for (size_t i = 0; i < used; ++i)
          B[i] = std::move(A[i]);

        delete[] A;
        A = B;
      }
      else
      {
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

  inline T& operator[](size_t index) VSX_ALWAYS_INLINE
  {
    #if VSXU_DEBUG
    if (data_volatile && index > (used - 1))
      exit(500);
    #endif
    allocate( index );
    return A[index];
  }

  inline vsx_nw_vector<T>& operator=(vsx_nw_vector<T>&& other) VSX_ALWAYS_INLINE
  {
    reqrv(!data_volatile, *this);

    if (A)
      clear();

    allocated = other.allocated;
    used = other.used;
    allocation_increment = other.allocation_increment;
    timestamp = other.timestamp;
    A = std::move(other.A);

    other.allocated = 0;
    other.used = 0;
    other.allocation_increment = 1;
    other.timestamp = 0;
    other.A = 0x0;
    return *this;
  }

  inline const vsx_nw_vector<T>& operator=(const vsx_nw_vector<T>& other) VSX_ALWAYS_INLINE
  {
    reqrv(!data_volatile, *this);

    allocate(other.allocated);
    used = other.used;
    timestamp = other.timestamp;
    allocation_increment = other.allocation_increment;
    for (size_t i = 0; i < used; i++)
      A[i] = other.A[i];
    return *this;
  }

  vsx_nw_vector(vsx_nw_vector<T>& other)
  {
    req(!other.data_volatile);
    allocate(other.allocated);
    allocated = other.allocated;
    used = other.used;
    allocation_increment = other.allocation_increment;
    timestamp = other.timestamp;
    for (size_t i = 0; i < used; i++)
      A[i] = other.A[i];
  }

  vsx_nw_vector(vsx_nw_vector<T>&& other)
  {
    req(!other.data_volatile);
    allocate(other.allocated);
    allocated = other.allocated;
    used = other.used;
    allocation_increment = other.allocation_increment;
    timestamp = other.timestamp;
    for (size_t i = 0; i < used; i++)
      A[i] = std::move(other.A[i]);

    other.allocated = 0;
    other.used = 0;
    other.allocation_increment = 1;
    other.timestamp = 0;
    other.A = 0x0;
  }

  vsx_nw_vector()
  {
  }

  ~vsx_nw_vector()
  {
    req(!data_volatile);
    req(A);
    delete[] A;
    A = 0x0; // valgrind
  }
};

#endif
