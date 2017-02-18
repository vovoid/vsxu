// mtex.cc: Calculates the sample mean of 1,000,000 draws from U(0,1).

#include <iostream>
#include "mt.h"

using namespace std;

int main (int argc, char** argv) {
    int N = 1000000;
    double ran[N];
    double mean = 0.0;

    // Initialize a Mersenne Twister
    MersenneTwister mt;
    mt.print();

    // Take N draws from Uniform(0,1)
    for (int i = 0; i < N; i++) {
        ran[i] = mt.random();
    }

    // Calculate the mean
    for (int i = 0; i < N; i++) {
        mean += ran[i];
    }
    mean /= N;

    cout << "Sample mean of " << N << "draws:" << mean << endl;

    return EXIT_SUCCESS;
}
