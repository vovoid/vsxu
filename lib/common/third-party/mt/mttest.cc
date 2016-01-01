/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

#include <iostream>
#include <iomanip>
#include <stdio.h>

#include "mt.h"

int main(void)
{
    MersenneTwister* mt;

    // Original test code
    int i;
    unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
    mt = new MersenneTwister();
    mt->init_by_array(init, length);
    printf("1000 outputs of genrand_int32()\n");
    for (i=0; i<1000; i++) {
        printf("%10lu ", mt->genrand_int32());
      if (i%5==4) printf("\n");
    }
    printf("\n1000 outputs of genrand_real2()\n");
    for (i=0; i<1000; i++) {
      printf("%10.8f ", mt->genrand_real2());
      if (i%5==4) printf("\n");
    }

    // Standard test code
    std::cout << std::endl << " * Mersenne Twister tests" << std::endl;
    int R = 1000000;                       // Number of draws
    double ran[R];                         // Array to store draws
    double sum = 0.0;                      // Sum of draws
    double avg = 0.0;                      // Sample mean of draws (Want: 0.5)
    double var = 0.0;                      // Sample variance of draws (Want: 1.0)
    time_t starttime;                      // Time the process began
    time_t endtime;                        // Time the process ended
    double duration;                       // Duration in seconds

    mt->print();                           // Print Mersenne twister info

    // Draw R deviates and time the process
    time(&starttime);                      // Get the start time
    for (int i = 0; i < R; i++) {
        ran[i] = mt->random();             // Draw a uniform random deviate
    }
    time(&endtime);                        // Get the end time
    duration = difftime(endtime, starttime);

    // Calculate sample statistics
    for (int i = 0; i < R; i++) {
        sum += ran[i];                     // Add to cumulative sum
    }
    avg = sum/R;                           // Take sample mean
    for (int i = 0; i < R; i++) {
        var += (ran[i]-avg)*(ran[i]-avg);    // Sum of squared deviations
    }
    var /= (R - 1);                        // Calculate unbiased estimate of var

    // Print results
    std::cout << "sample size: " << R << std::endl;
    std::cout << "sample mean: " << std::scientific << std::setprecision(5)
              << avg << std::endl;
    std::cout << "sample var:  " << std::scientific << std::setprecision(5)
              << var << std::endl;
    std::cout << "sec./draw:   " << std::scientific << std::setprecision(5)
              << duration/R << std::endl;

    delete mt;
    return 0;
}
