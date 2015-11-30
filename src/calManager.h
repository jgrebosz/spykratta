#ifndef CALMANAGER
#define CALMANAGER

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "calCoef.h"

using namespace std;
#include <fstream>

class calCoef;
////////////////////////////////////////////////////////////////////////////////////////////////////
class calManager
{
public:
    calManager(string, int = 0);
    calManager(ifstream & s, double how_many_detectors, int = 0);
    ~calManager();

private:
    int runDependent;
    int file_read;

private:
    int maxDet;
    int maxRun;

private:
    std::vector<std::vector<int> > lookup;

private:
    std::vector<calCoef> theCoeff;

private:
    double offset;
    double gain;

private:
    int ReadCoefficients(string);
    int ReadCoefficients(ifstream & s, double how_many_detectors);

public:
    int setCoeffFile(string);

public:
    inline void setGain(double value)
    {
        gain = value;
    };
    inline void setOffset(double value)
    {
        offset = value;
    };

public:
    double getCalValue(int, int, double);
    double getCoeff(int, int , int);

};




#endif
