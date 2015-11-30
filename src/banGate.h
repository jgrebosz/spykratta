/////////////////////////////////////////////////////
//// This class handles banana gates. It is assumed
//// that these 2D-gates depend on the detector
//// but NOT on the run.
/////////////////////////////////////////////////////
#ifndef BANGATE
#define BANGATE

#include <vector>
#include "Vectors.h"

using namespace std;

class Vector2D;
//----------------------------------------------------------------------
class banGate
{
public:
    banGate(int, int, int = 0, int = 0, double = 0.);
    ~banGate();

private:
    std::vector<Vector2D> thePoints;  // points

private:
    int                   ndet;       // detector number

    // resolution of the 2D-gate
private:
    int                   resX;
    int                   resY;

    // equivalent of "pin" command of gsort:
    // a "particle identification" and a "weight"
    // are associated to the 2D-gate
private:
    int                   p_id;
    double                weight;

    // the "inside" relation is checked storing in
    // memory the minimum and maximum values associated
    // to each x value
private:
//  double* minBan;
//  double* maxBan;

    vector<double> minBan;
    vector<double> maxBan;

public:
    inline int    getNDet()
    {
        return ndet;
    };

public:
    inline int    getP_Id()
    {
        return p_id;
    };
    inline double getWeight()
    {
        return weight;
    };

public:
    inline int NPoints()
    {
        return (int)(thePoints.size());
    };

public:
    void       addPoint(double, double);
    int        inside(double, double);

public:
    int        mapBanana();

public:
    double     getMinBan(int);
    double     getMaxBan(int);

public:
    int        overlapBan(banGate*);

};

#endif
