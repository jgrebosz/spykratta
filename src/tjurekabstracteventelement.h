#ifndef TJUREKABSTRACTEVENTELEMENT_H
#define TJUREKABSTRACTEVENTELEMENT_H

/**
    @author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

#include <string>
using namespace std;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TjurekAbstractEventElement
{
    string name;
public:
    TjurekAbstractEventElement(string name_) : name(name_) {}

    ~TjurekAbstractEventElement();

};

#endif
