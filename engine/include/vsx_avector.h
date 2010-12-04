/*#ifndef VSX_AVECTOR_H
#define VSX_AVECTOR_H

#include <stdio.h>

template<class T>
class vsx_avector {
public:
  unsigned long allocated;
  unsigned long used;
  T* A;

  unsigned long timestamp;
  T* get_pointer() {
    return A;
  }
  unsigned long get_allocated() {
    return allocated;
  }
  unsigned long get_used() {
    return used;
  }
  // std::vector compatibility
  unsigned long push_back(T val) {
    (*this)[used] = val;
    return used;
  }

  unsigned long size() {
    return used;
  }

  void clear() {
    delete[] A;
    A = 0;
    allocated = used = 0;
  }

  void reset_used(int val = 0) {
    if (val >= 0)
    used = val;
    else used = 0;
  }

  //void set_allocation_increment(unsigned long new_increment) {
//  	allocation_increment = new_increment;
//  }

//vsx_avector(vsx_avector<T>& v) {
//    T* f = v.get_pointer();
//    unsigned long vs = v.size();
//    while (used < vs) {
//      push_back(*f);
//      ++f;
//    }
//  }*

//  vsx_avector<T>& operator=(vsx_avector<T>& v) {
//    clear();
//    // T* f = v.get_pointer();
//    unsigned long vs = v.size();
//    unsigned long i = 0;
//    while (i < vs) {
//      push_back(v[i]);
//      ++i;
    //}
    //return *this;
//  }

  T& operator[](unsigned long index) {
  	printf("vsx_avector [%d]\n",index);
    if (index >= allocated || allocated == 0)
    {
    	if (allocated == 0) allocated = 1;
    	while (index >= allocated) {
    		allocated = allocated << 1;
    		printf("allocated: %d index: %d used:\n",allocated,index,used);
    	}
      // need to allocate stuff here
      //if (A) {
        //allocated = index+allocation_increment;
        T* B = new T[allocated];
        for (unsigned long i = 0; i < used; ++i) {
          B[i] = A[i];
        }
        if (A)
        delete[] A;
        A = B;
      //}
      //else {
//        A = new T[index+allocation_increment];//(T*)malloc(sizeof(T)*(index+allocation_increment));
        //allocated = index+allocation_increment;
      //}
    }
    if (index >= used) {
      used = index+1;
      //printf("used: %d\n",used);
    }
    printf("vsx_avector returning\n");
    return A[index];
  }
  vsx_avector() : allocated(2),used(0),timestamp(0) {
  	A = new T[2];
  };
//#ifndef VSX_AVECTOR_ND
  ~vsx_avector() {
    //printf("avector destructor\n");
    delete[] A;
  }
//#endif
};

#endif*/
#ifndef VSX_AVECTOR_H
#define VSX_AVECTOR_H

#include <stdio.h>

template<class T>
class vsx_avector {
public:
  unsigned long allocated;
  unsigned long used;
  T* A;
  unsigned long allocation_increment;

  unsigned long timestamp;
  T* get_pointer() {
    return A;
  }
  unsigned long get_allocated() {
    return allocated;
  }
  unsigned long get_used() {
    return used;
  }
  // std::vector compatibility
  unsigned long push_back(T val) {
    (*this)[used] = val;
    return used;
  }

  unsigned long size() {
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

  T& operator[](unsigned long index) {
    if (index >= allocated || allocated == 0)
    {
      // need to allocate stuff here
      if (A) {
      	if (allocation_increment == 0) allocation_increment = 1;
        allocated = index+allocation_increment;
        T* B = new T[allocated];
//        printf("used: %d\n",used);
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
  vsx_avector() : allocated(0),used(0),A(0),allocation_increment(1),timestamp(0) {};
//#ifndef VSX_AVECTOR_ND
  ~vsx_avector() {
    //printf("avector destructor\n");
    delete[] A;
  }
//#endif
};

#endif
