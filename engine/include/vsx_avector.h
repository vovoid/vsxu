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

#ifndef VSX_AVECTOR_H
#define VSX_AVECTOR_H

#include <stdio.h>

template<class T>
class vsx_avector
{
public:
  size_t allocated;
  size_t used;
  T* A;
  size_t allocation_increment;

  size_t timestamp;
  T* get_pointer() {
    return A;
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

  void clear() {
    delete[] A;
    A = 0;
    allocated = used = 0;
    allocation_increment = 1;
  }

  void reset_used(int val = 0) {
    if (val >= 0)
    used = val;
    else used = 0;
  }

  void set_allocation_increment(unsigned long new_increment) {
  	allocation_increment = new_increment;
  }

  void remove(T val)
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

  /*vsx_avector(vsx_avector<T>& v) {
    T* f = v.get_pointer();
    unsigned long vs = v.size();
    while (used < vs) {
      push_back(*f);
      ++f;
    }
  }*

  vsx_avector<T>& operator=(vsx_avector<T>& v) {
    clear();
    // T* f = v.get_pointer();
    unsigned long vs = v.size();
    unsigned long i = 0;
    while (i < vs) {
      push_back(v[i]);
      ++i;
    }
    return *this;
  }*/

  T& operator[](size_t index) {
    if (index >= allocated || allocated == 0)
    {
      // need to allocate stuff here
      if (A) {
      	if (allocation_increment == 0) allocation_increment = 1;
        allocated = index+allocation_increment;
        T* B = new T[allocated];
        // printf("used: %d\n",used);
        for (size_t i = 0; i < used; ++i) {
          B[i] = A[i];
        }
        delete[] A;
        A = B;
      } else {
        A = new T[index+allocation_increment];//(T*)malloc(sizeof(T)*(index+allocation_increment));
        allocated = index+allocation_increment;
      }
      allocation_increment = allocation_increment << 1;
    }
    if (index >= used) {
      used = index+1;
      //printf("used: %d\n",used);
    }
    return A[index];
  }
  vsx_avector() : allocated(0),used(0),A(0),allocation_increment(1),timestamp(0) {}
  ~vsx_avector()
  {
    if (A)
    {
      delete[] A;
      A = 0x0;
    }
  }
};

#endif
