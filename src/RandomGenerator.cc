#ifndef _MSC_VER     // this symbol is defined in the Microsoft compiler
#include <sys/time.h>
#endif
#include <algorithm>
#include <cmath>

#include <RandomGenerator.h>

namespace Random
{

static long _seed = -987654321;

// uniform

static long iv[32];
static long iy = 0;
static long _seed2 = 123456789;

// normal

static double gset;
static bool flag = true;

// functions

void seed(long new_seed)
{
    if(new_seed != 0) _seed = new_seed > 0 ? -new_seed : new_seed;
}
long seed()
{
    return _seed;
}

long randomize()
{
#ifndef _MSC_VER     // this symbol is defined in the Microsoft compiler
    struct timeval tv;
    struct timezone tz;

    gettimeofday(&tv, &tz);

    long new_seed = tv.tv_sec + tv.tv_usec * 2000;

    seed(new_seed);

    return new_seed;
#else
    return 0;
#endif
}

double uniform()
{
    if(_seed <= 0)
    {
        _seed2 = _seed = (_seed > 1) ? 1 : -_seed;

        for(int j = 32 + 7; j >= 0; j--)
        {
            long k = _seed / 53668;

            _seed = 40014 * (_seed - k * 53668) - k * 12211;

            if(_seed < 0) _seed += 2147483563;

            if(j < 32) iv[j] = _seed;
        }

        iy = iv[0];
    }

    long k = _seed / 53668;

    _seed = 40014 * (_seed - k * 53668) - k * 12211;

    if(_seed < 0) _seed += 2147483563;

    k = _seed2 / 52774;

    _seed2 = 40692 * (_seed2 - k * 52774) - k * 3791;

    if(_seed2 < 0) _seed2 += 2147483399;

    long j = iy / 67108862;

    iy = iv[j] - _seed2;

    iv[j] = _seed;

    if(iy < 1) iy += 2147483562;

    return std::min(double(iy) / 2147483563, 1.0);
}

double uniform(double width)
{
    return width * uniform();
}
double uniform(double _min, double _max)
{
    return _min + uniform(_max - _min);
}

double normal()
{
    double result;

    if(flag == true)
    {
        flag = false;

        double radius, v1, v2;

        do
        {

            v1 = uniform(-1, 1);
            v2 = uniform(-1, 1);

            radius = v1 * v1 + v2 * v2;

        }
        while(radius >= 1 || radius == 0);

        double func = sqrt(-2 * log(radius) / radius);

        gset = v1 * func;

        result = v2 * func;

    }
    else
    {

        flag = true;

        result = gset;
    }

    return result;
}

double normal(double sdev)
{
    return sdev * normal();
}
double normal(double mean, double sdev)
{
    return mean + sdev * normal();
}

double exponential()
{
    double result;
    do result = uniform();
    while(result == 0);
    return -log(result);
}
double exponential(double mean)
{
    double result;
    do result = uniform();
    while(result == 0);
    return -log(result) * mean;
}

double gaussian()
{
    double pi, r1, r2;

    pi =  4.*atan(1.);
    r1 = -log(1. - uniform());
    r2 =  2.*pi * uniform();
    r1 =  sqrt(2.*r1);
    return r1 * cos(r2);
}
double gaussian(double sigma)
{
    return sigma * gaussian();
}
double gaussian(double mean, double sigma)
{
    return mean + sigma * gaussian();
}


size_t arbitrary(double *pd, size_t pd_size)
{
    double u = Random::uniform();

    for(size_t i = 0; i < pd_size; i++)
    {
        u -= pd[i];

        if(u < 0)    return i;
    }

    return size_t(-1);
}

}
