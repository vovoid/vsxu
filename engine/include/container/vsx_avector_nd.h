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

#ifndef VSX_AVECTOR_ND_H
#define VSX_AVECTOR_ND_H

template<class T>
class vsx_avector_nd
{
  size_t allocated;
  size_t used;
  __attribute__((aligned(64))) T* A;
  size_t allocation_increment;
public:
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

  T& operator[](unsigned long index) {
    if (index >= allocated || allocated == 0)
    {
      // need to allocate stuff here
      if (A) {

        //if (allocated == 0)
        if (allocation_increment == 0) allocation_increment = 1;
        allocated = index+allocation_increment;
        T* B = new T[allocated];
        for (unsigned long i = 0; i < used; ++i) {
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

  vsx_avector_nd()
    :
      allocated(0),
      used(0),
      A(0),
      allocation_increment(1),
      timestamp(0)
  {
  }
};

#endif
