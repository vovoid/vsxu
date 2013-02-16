#ifndef VSX_AVECTOR_ND_H
#define VSX_AVECTOR_ND_H

template<class T>
class vsx_avector_nd {
  size_t allocated;
  size_t used;
  T* A;
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
  vsx_avector_nd() : allocated(0),used(0),A(0),allocation_increment(1),timestamp(0) {};
};

#endif
