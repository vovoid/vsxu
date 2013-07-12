/**
* Project: VSXu: Realtime visual programming language, music/audio visualizer, animation tool and much much more.
*
* @author Paul Bourke
* @see The GNU Public License (GPL)
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

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
