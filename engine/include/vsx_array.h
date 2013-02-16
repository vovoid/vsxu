/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Jonatan Wallmander, Robert Wenzel, Vovoid Media Technologies Copyright (C) 2003-2011
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#ifndef VSX_ARRAY_H
#define VSX_ARRAY_H

#include <stdlib.h>
#include <string.h>
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
class vsx_array {
  size_t allocated;
  size_t used;
  T* A;
  size_t allocation_increment;
  size_t data_volatile;

public:
  size_t timestamp;

  void set_allocation_increment(unsigned long new_increment) {
  	allocation_increment = new_increment;
  }

  void set_data(T* nA, int nsize)
  {
  	A = nA;
  	used = allocated = nsize;
  }

  // clones another array of same type into this one
  void clone(vsx_array<T>* F)
  {
    allocate(F->size());
    memcpy((void*)A, (void*)(F->get_pointer()), sizeof(T) * used);
  }

  void set_volatile() {
    if (0 == data_volatile && A && allocated)
    {
      clear();
    }
    data_volatile = 1;
  }

  void unset_volatile()
  {
    if (data_volatile) {
      A = 0;
      allocated = 0;
      data_volatile = 0;
    }
  }

  T* get_pointer() {
    return A;
  }
  T* get_end_pointer() {
    return &A[used-1];
  }
  size_t get_allocated() {
    return allocated;
  }
  size_t get_used() {
    return used;
  }
  // std::vector compatibility
  size_t push_back(T val) {
    (*this)[used] = val;
    return used;
  }
  size_t size() {
    return used;
  }

  size_t get_sizeof()
  {
    return used * sizeof(T);
  }

  void clear() {
    if (data_volatile) { return; }
  	if (A)
    free(A);
    A = 0;
    used = allocated = 0;
    allocation_increment = 1;
  }

  void memory_clear()
  {
    memset(A, 0, sizeof(T) * allocated);
  }

  void reset_used(size_t val = 0) {
  	// TODO: if value larger than count of allocated items in memory handle this some way
    used = val;
  }

  void allocate(size_t index) {
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
    }
    if (index >= used) {
      used = index+1;
    }
  }
  T& operator[](unsigned long index) {
    allocate(index);
    return A[index];
  }

  vsx_array() : allocated(0),used(0),A(0),allocation_increment(1),data_volatile(0),timestamp(0) {};
  ~vsx_array() {
    if (data_volatile) return;
  	if (A) free(A);
  }
};



#endif
