/*****************************************************************************
*                                                                            *
*       Small test program for FFTReal class.                                *
*                                                                            *
******************************************************************************/



#include	"FFTReal.h"

#include	<new>

#include	<cassert>
#include	<cmath>
#include	<cstdio>
#include	<cstdlib>
#include	<ctime>



/* Another MS VC++ trick: "new" doesn't throw exception by default */
#if defined (_MSC_VER)

#include	<new.h>
int	my_new_handler (size_t dummy)
{
	throw std::bad_alloc ();
	return (0);
}

#endif	// _MSC_VER



int	main (void)
{
#if defined (_MSC_VER)
	_set_new_handler (my_new_handler);
#endif	// _MSC_VER

	try
	{

/*______________________________________________
 *
 * Exactness test
 *______________________________________________
 */

		{
			printf ("Accuracy test:\n\n");

			const long		nbr_points = 16;			// Power of 2
			flt_t	* const	x = new flt_t [nbr_points];
			flt_t	* const	f = new flt_t [nbr_points];
			FFTReal	fft (nbr_points);					// FFT object initialized here
			long		i;

			/* Test signal */
			const double	PI = atan (1) * 4;
			for (i = 0; i < nbr_points; ++ i)
			{
				x [i] = (flt_t) (-1
			                    + sin (3*2*PI*i/nbr_points)
			                    + cos (5*2*PI*i/nbr_points) * 2
			                    - sin (7*2*PI*i/nbr_points) * 3
			                    + cos (8*2*PI*i/nbr_points) * 5);
			}

			/* Compute FFT and IFFT */
			fft.do_fft (f, x);
			fft.do_ifft (f, x);
			fft.rescale (x);

			/* Display the result */
			printf ("FFT:\n");
			for (i = 0; i <= nbr_points/2; i ++)
			{
				double			img;

				const double	real = f [i];
				if (i > 0 && i < nbr_points/2)
				{
					img = f [i + nbr_points/2];
				}
				else
				{
					img = 0;
				}

				const double	f_abs = sqrt (real * real + img * img);
				//printf ("%5d: %+12.6f %+12.6f (%12.6f)\n", i, real, img, f_abs);
			}
			printf ("\n");

			printf ("IFFT:\n");
			for (i = 0; i < nbr_points; i ++)
			{
				//printf ("%5d: %+f\n", i, double (x [i]));
			}
			printf ("\n");

			delete [] x;
			delete [] f;
		}

/*______________________________________________
 *
 * Speed test
 *______________________________________________
 */

		{
			printf ("Speed test:\nPlease wait...\n\n");

			const long		nbr_points = 1024;			// Power of 2
			const long		buffer_size = 256*1024L;	// Number of flt_t (float or double)
			const long		nbr_tests = 10000;

			assert (nbr_points <= buffer_size);
			flt_t	* const	x = new flt_t [buffer_size];
			flt_t	* const	f = new flt_t [buffer_size];
			FFTReal	fft (nbr_points);					// FFT object initialized here
			long				i;

			/* Test signal: noise */
			for (i = 0; i < nbr_points; ++ i)
			{
				x [i] = (flt_t) rand () - (RAND_MAX >> 1);
			}

			clock_t	tt0 = clock ();
			for (i = 0; i < nbr_tests; ++ i)
			{
				const long		offset = (i * nbr_points) & (buffer_size - 1);
				fft.do_fft (f + offset, x + offset);
			}
			clock_t	tt1 = clock ();
			for (i = 0; i < nbr_tests; ++ i)
			{
				const long		offset = (i * nbr_points) & (buffer_size - 1);
				fft.do_ifft (f + offset, x + offset);
				fft.rescale (x);
			}
			clock_t	tt2 = clock ();

			const double	t1 = double (tt1 - tt0) / nbr_tests / CLOCKS_PER_SEC;
			const double	t2 = double (tt2 - tt1) / nbr_tests / CLOCKS_PER_SEC;
			printf ("%ld-points FFT           : %.0f us.\n", nbr_points, t1 * 1000000);
			printf ("%ld-points IFFT + scaling: %.0f us.\n", nbr_points, t2 * 1000000);

			const long	nbr_s_chn = long (floor (nbr_points / ((t1 + t2) * 44100 * 2)));
			printf ("Peak performance: FFT+IFFT on %ld mono channels at 44.1 KHz (with overlapping).\n", nbr_s_chn);
			printf ("\n");

			delete [] x;
			delete [] f;
		}

		printf ("Press [Return] key to terminate...\n");
		getchar ();
	}

	catch (std::exception e)
	{
		printf ("Exception: %s\n", e.what ());
	}

	catch (...)
	{
		printf ("Unknown execption captured. Program termination.\n");
		exit (-1);
	}

	return (0);
}
