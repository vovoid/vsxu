==============================================================================

        FFTReal
        Version 1.02, 2001/03/27

        Class of Fourier transformation of real data (FFT and IFFT)
        Portable ISO C++

        (c) Laurent de Soras
        ldesoras@club-internet.fr

==============================================================================



1) Legal
========

Source code may be freely used for any purpose, including commercial
applications. Programs must display in their "About" dialog-box (or
documentation) a text telling they use these routines by Laurent de Soras.
Modified source code can be distributed, but modifications must be clearly
indicated.



2) Content
==========

There is no official web site where to get these files. If you have them,
that's all to the good for you.

You should find in this package the following files:
- readme.txt
- FFTReal.cpp
- FFTReal.h
- test.cpp



3) Usage
========

Just instanciate one time a FFTReal object. The constructor precompute a lot
of things, so it can be long. The parameter is the number of points used for
the next FFTs. It must be a power of 2:

   FFTReal  fft_object (1024);   // 1024-point FFT object constructed.

Then you can use this object to compute as many FFTs and IFFTs as you want.
They will be computed very quickly because a lot of work has been done in the
object construction.

   flt_t x [1024];
   flt_t f [1024];

   ...
   fft_object.do_fft (f, x);     // x --FFT---> f
   ...
   fft_object.do_ifft (f, x);    // f --IFFT--> x
	fft_object.rescale (x);       // Post-scaling should be done after FFT+IFFT
   ...

x [] and f [] are floating point number arrays. flt_t is a predefined type
mapped to float. You can change it in FFTReal.h if you want to compute FFTs on
double or long double numbers.

x [] is the real number sequence which we want to compute the FFT. f [] is the
result, in the frequency domain. f has the same number of elements as x [],
but f [] elements are complex numbers. A FFT property is the transformaton of
a real number sequence into a conjugate complex number sequence. This is
useful to save memory and to reduce calculations.

The following table shows how the f [] sequence is mapped:

   f [0]          : Real (coef 0)
   f [...]        : Real (coef ...)
   f [length/2]   : Real (coef length/2)
   f [length/2+1] : Imag (coef 1)
   f [...]        : Imag (coef ...)
   f [length-1]   : Imag (coef length/2-1)

And FFT coefficients are distributed in f [] as above:

   Coef        | Real part      | Imaginary part 
   ------------+----------------+----------------
   0           | f [0]          | 0
   1           | f [1]          | f [length/2+1]
   ...         | f [...],       | f [...]
   length/2-1  | f [length/2-1] | f [length-1]
   length/2    | f [length/2]   | 0
   length/2+1  | f [length/2-1] | - f [length-1]
   ...         | f [...]        | - f [...]
   length-1    | f [1]          | - f [length/2+1]

f [] coefficients have the same layout for FFT and IFFT functions. You may
notice that scaling must be done if you want to retrieve x after FFT and IFFT.
Actually, IFFT (FFT (x)) = x * length(x). This is a not a problem because
most of the applications don't care about absolute values. Thus, the operation
requires less calculation. If you want to use the FFT and IFFT to transform a
signal, you have to apply post- (or pre-) processing yourself. Multiplying
or dividing floating point numbers by a power of 2 doesn't generate extra
computation noise.



4) Implementation notes
=======================

These FFT routines are optimized for Intel Pentium II compatible processors.
However, they should work fine on every other types of processor.

You should also activate the exception handling of your compiler to get the
class memory-leak-safe. Thus, when a memory allocation fails, an exception is
thrown and the entire object is destructed.



---[ EOF ]--------------------------------------------------------------------
