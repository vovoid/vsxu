#pragma once

#include <math/vsx_rand.h>
#include <tools/vsx_singleton.h>


class vsx_rand_singleton
    : public vsx::singleton<vsx_rand_singleton>
{
public:

  vsx_rand rand;

};
