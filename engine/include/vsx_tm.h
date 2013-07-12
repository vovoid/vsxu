#ifndef VSX_TM_H
#define VSX_TM_H

#ifdef VSXU_TM
#include <vsx_tmi.h>
#else

class vsx_tm
{
public:
void  e(...) {};
void  l(...) {};
void  z(...) {};
void  t(...) {};
};

#endif

#endif
