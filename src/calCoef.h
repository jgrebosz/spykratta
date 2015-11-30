#ifndef CALCOEF
#define CALCOEF

#include <vector>

using namespace std;

class calCoef
{
public:
    calCoef(int = 0, int = 0);
    ~calCoef();

private:
    std::vector<double> theCoef;  // coefficients
    int                 ndet;     // detector number
    int                 nrun;     // run number

public:
    inline int getNDet()
    {
        return ndet;
    };
    inline int getNRun()
    {
        return nrun;
    };

public:
    inline int getOrder()
    {
        return (int)(theCoef.size());
    };

public:
    int        addCoeff(double);
    double     getCoeff(int);
    double     getCalValue(double);
};


#endif
