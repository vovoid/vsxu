#include <stdio.h>
#include <stdlib.h>

#include <vsx_string.h>


template < class V>
class vsx_map_str {

private:
  unsigned long I;

  unsigned long get_prime(unsigned long i1)
  {
    for (int k = 0, int r = 1; k < sizeof(unsigned long)<<4&&r;++k)
    {
      
    }
    unsigned long primes =
    {
      0,
      1,
      2,
      3,
      7,
      17,
      31,
      61,
      127,
      251,
      509,
      1021,
      2039,
      4093,
      8191,
      16381,
      32749,
      65521,
      131071,
      262139,
      524287,
      1048573,
      2097143,
      4194301,
      8388593,
      16777213,
      33554393,
      67108859,
      134217689,
      268435399,
      536870909,
      1073741789,
      2147483647
    };
  }
  
public:
  V& operator[](vsx_string index) {
    //
  }
  vsx_map_str()
  {
    allocated = 256;
    I = 251;
  }
};

int main(int argc, char **argv) {
  return 0;
};