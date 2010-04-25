#ifndef VSX_TIMER_H
#define VSX_TIMER_H

//#ifndef WIN32
#include <time.h>
#include <sys/time.h>
//#endif

#ifdef _WIN32
#include <windows.h>
#endif

class vsx_timer {
  double startt;
  double lastt;
  double dtimet;
#ifdef _WIN32  
  LONGLONG init_time;
#endif
public:
  
  void start() {
    startt = atime();
    lastt = startt;
  }
  
  double dtime() {
    double at = atime();
    dtimet = at-lastt;
    lastt = at;
    return dtimet;
  }
  
// normal time
double ntime() {
  return ((double)clock())/((double)CLOCKS_PER_SEC);
}

// accurate time
double atime() {
#ifdef _WIN32
  	LARGE_INTEGER freq, time;
  	QueryPerformanceFrequency(&freq);
  	QueryPerformanceCounter(&time);  	
    return (double)((double)(time.QuadPart-init_time) / (double)freq.QuadPart);; 
#else
    struct timeval now;
    gettimeofday(&now, 0);
    return (double)now.tv_sec+0.000001*(double)now.tv_usec;
#endif
}

vsx_timer() {
#ifdef _WIN32
 	LARGE_INTEGER time;
 	QueryPerformanceCounter(&time);
  init_time = time.QuadPart;
#endif
}

};


#endif
