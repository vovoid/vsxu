/*#ifndef VSX_ARRAY_H
#define VSX_ARRAY_H

#include <stdlib.h>
// this is a special case array aimed at speed
// DON'T USE IT WITH CLASSES WITH VIRTUAL FUNCTIONS
// DON'T POINT AT DATA STORED IN IT (data is moved around rendering those pointers invalid)


template<class T>
class vsx_array {
  unsigned long allocated;
  unsigned long used;
  T* A;
  //unsigned long allocation_increment;
public:
  unsigned long timestamp;

  //void set_allocation_increment(unsigned long new_increment) {
//  	allocation_increment = new_increment;
//  }

  T* get_pointer() {
    return A;
  }
  T* get_end_pointer() {
    return A+used*sizeof(T);
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
  	if (A)
    free(A);
    A = 0;
    used = allocated = 0;
  }

  void reset_used(unsigned long val = 0) {
    used = val;
  }

  void allocate(unsigned long index) {
    if (index >= allocated || allocated == 0)
    {
    	if (allocated == 0) allocated = 1;
    	while (index >= allocated) allocated = allocated << 1;
    	printf("vsx_array::allocated %d  index %d\n",allocated, index);
      if (A) {
        //allocated = index+allocation_increment;
        A = (T*)realloc(A,sizeof(T)*allocated);
      } else {
        A = (T*)malloc(sizeof(T)*(allocated));
        //allocated = index+allocation_increment;
      }
    }
    if (index >= used) {
      used = index+1;
    }
  }
  T& operator[](unsigned long index) {
  	//printf("vsx_array []\n");
    allocate(index);
    return A[index];
  }

  vsx_array() : allocated(0),used(0),A(0),timestamp(0) {
  	//A = (T*)malloc(sizeof(T));
  };
  ~vsx_array() {
  	if (A)
    free(A);
  }
};


template<class T>
class vsx_avector_nd {
  unsigned long allocated;
  unsigned long used;
  T* A;
public:
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
  	if (A)
    delete[] A;
    A = 0;
    allocated = used = 0;
  }

  void reset_used(int val = 0) {
    used = val;
  }

  T& operator[](unsigned long index) {
    printf("nd[] allocated %d  index %d used %d basepointer %d\n",allocated, index,used,A);
    if (index >= allocated || allocated == 0)
    {
    	if (allocated == 0) allocated = 2;
    	while (index >= allocated) allocated = allocated << 1;
    	printf("allocated %d  index %d used %d\n",allocated, index,used);
      // need to allocate stuff here
      if (A) {

      	//if (allocated == 0)
        //allocated = index+allocation_increment;
        T* B = new T[allocated];
        for (unsigned long i = 0; i < used; ++i) {
        	printf("copying used %d\n",i);
          B[i] = A[i];
        }
        printf("before delete\n");
        //if (A)
        //{
        	delete[] A;
        	printf("after delete\n");
        //}
        A = B;
      } else {
        A = new T[allocated];//(T*)malloc(sizeof(T)*(index+allocation_increment));
        //allocated = index+allocation_increment;
      }
    }
    if (index >= used) {
      used = index+1;
      //printf("used: %d\n",used);
    }
    printf("before return\n");
    return A[index];
  }
  vsx_avector_nd() : A(0),allocated(0),used(0),timestamp(0) {};
};


#endif*/
#ifndef VSX_ARRAY_H
#define VSX_ARRAY_H

#include <stdlib.h>
// this is a special case array aimed at speed
// DON'T USE IT WITH CLASSES WITH VIRTUAL FUNCTIONS
// DON'T POINT AT DATA STORED IN IT (data is moved around rendering those pointers invalid)


template<class T>
class vsx_array {
  unsigned long allocated;
  unsigned long used;
  T* A;
  unsigned long allocation_increment;
  unsigned long data_volatile;

public:
  unsigned long timestamp;

  void set_allocation_increment(unsigned long new_increment) {
  	allocation_increment = new_increment;
  }

  void set_data(T* nA, int nsize)
  {
  	A = nA;
  	used = allocated = nsize;
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
    return A+used*sizeof(T);
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

  void reset_used(unsigned long val = 0) {
  	// TODO: if value larger than count of allocated items in memory handle this some way
    used = val;
  }

  void allocate(unsigned long index) {
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

template<class T>
class vsx_avector_nd {
  unsigned long allocated;
  unsigned long used;
  T* A;
  unsigned long allocation_increment;
public:
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
  vsx_avector_nd() : allocated(0),used(0),A(0),allocation_increment(1),timestamp(0) {};
};


#endif
