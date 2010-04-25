// gauss.cpp
// this actually calculates the gauss values.

#include <math.h>
#include <time.h>
#include <stdlib.h>

double ranf()
{
	return (double)rand()/(double)RAND_MAX;
}

void gauss(double work[2])
{
	// Algorithm by Dr. Everett (Skip) Carter, Jr.

	double x1, x2, w;

	do {
		x1 = 2.0 * ranf() - 1.0;
		x2 = 2.0 * ranf() - 1.0;
		w = x1 * x1 + x2 * x2; 
	} while ( w >= 1.0 );

	w = sqrt( (-2.0 * log( w ) ) / w );
	work[0] = x1 * w;	// first gauss random
	work[1] = x2 * w;	// second gauss random
}
