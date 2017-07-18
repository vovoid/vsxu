#include <math/vsx_rand.h>
#include <lib/common/third-party/mt/mt.h>


vsx_rand::vsx_rand()
{
  MersenneTwister* mt = new MersenneTwister();
  mt->init_genrand(1);
  state = (void*)mt;
}


vsx_rand::~vsx_rand()
{
  delete (MersenneTwister*)state;
}

void vsx_rand::srand(uint32_t seed)
{
  ((MersenneTwister*)state)->init_genrand(seed);
}

uint32_t vsx_rand::rand()
{
  return ((MersenneTwister*)state)->genrand_int32();
}

double vsx_rand::drand()
{
  return ((MersenneTwister*)state)->genrand_real1();
}

double vsx_rand::drand_c()
{
  return ((MersenneTwister*)state)->genrand_real1() - 0.5;
}


float vsx_rand::frand()
{
  return (float)((MersenneTwister*)state)->genrand_real1();
}

float vsx_rand::frand_c()
{
  return (float)((MersenneTwister*)state)->genrand_real1() - 0.5f;
}
