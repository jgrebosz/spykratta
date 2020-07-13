//////////////////////////////////////
/// This class handles banana gates:
/// decodes 2D-gates from file and
/// instantiates banGate objects to
/// store them
//////////////////////////////////////

#ifndef BANMANAGER
#define BANMANAGER

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include "banGate.h"

using namespace std;

//class banGate;

class banManager
{
public:
    banManager(string, int, int, int = 0, double = 0.);
    ~banManager();

private:
    int maxDet;
    int detDependent;
    int file_read;

private:
    int resX;
    int resY;

    // equivalent of "pin" command of gsort
private:
    int     p_id;
    double  weight;

private:
    std::vector<int> lookup;

private:
    std::vector<banGate> theGates;

private:
    int ReadBananas(string);

public:
    int setBananasFile(string);

public:
    int  inside(int, double, double);

public:
    int    getP_Id(int = 0);
    double getWeight(int = 0);

public:
    inline int howManyGates()
    {
        return (int)theGates.size();
    };

public:
    banGate* getBanGate(int);


};

#endif
