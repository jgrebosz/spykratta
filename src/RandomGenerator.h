#ifndef RANDOM_HH
#define RANDOM_HH

#include <sys/types.h>

#define GAUSSDIM (32*1024)

namespace Random
{

void seed(long new_seed);
long seed();

long randomize();

double normal(double mean, double sigma);
double normal(double sigma);
double normal();

double uniform(double _min, double _max);
double uniform(double _max);
double uniform();

double exponential(double mean);
double exponential();

double gaussian(double mean, double fwhm);
double gaussian(double fwhm);
double gaussian();

size_t arbitrary(double *pd, size_t pd_size);

// Generators

class Uniform
{

    double a, b;

public:

    Uniform(double _a, double _b) : a(_a), b(_b) {}
    Uniform(double _b) : a(0), b(_b) {}
    Uniform() : a(0), b(1) {}

    double operator()() const
    {
        return uniform(a, b);
    }
};

class Normal
{

    double mean, sigma;

public:

    Normal(double _mean, double _sigma) : mean(_mean), sigma(_sigma) {}
    Normal(double _sigma) : mean(0), sigma(_sigma) {}
    Normal() : mean(0), sigma(1) {}

    double operator()() const
    {
        return normal(mean, sigma);
    }
};

class Exponential
{

    double mean;

public:

    Exponential(double _mean) : mean(_mean) {}
    Exponential() : mean(0) {}

    double operator()() const
    {
        return exponential(mean);
    }
};


class Gaussian
{

    double mean;
    double sigma;

public:

    Gaussian(double _mean, double _sigma) : mean(_mean), sigma(_sigma) {}
    Gaussian(double _sigma) : mean(0), sigma(_sigma) {}
    Gaussian() : mean(0), sigma(1) {}

    double operator()() const
    {
        return mean + gaussian() * sigma;
    }
};

}

#endif /* RANDOM_HH */
