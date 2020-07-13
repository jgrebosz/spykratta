#include "calCoef.h"

calCoef::calCoef(int detnum, int runnum)
{
    ndet = detnum;
    nrun = runnum;

    theCoef.clear();

}

calCoef::~calCoef()
{
    theCoef.clear();
}

int calCoef::addCoeff(double coef)
{
    theCoef.push_back(coef);
    return (int)(theCoef.size());
}

double calCoef::getCoeff(int index)
{
    return theCoef.at(index);
}


double calCoef::getCalValue(double raw)
{
    int size = (int)(theCoef.size()) - 1;

    // a_0 + a_1 x + ... + a_N x^N = (( ..( a_N x + a_{N-1}) x + a_{N-2}) x +...) + a_0
    double calValue = theCoef[size] * raw + theCoef[size - 1];

    for(int ii = size - 1; ii > 0; ii--)
    {
        calValue = calValue * raw + theCoef[ii - 1];
    }

    return calValue;
}
