#ifndef TJUREKABSTRACTEVENTPROCESSOR_H
#define TJUREKABSTRACTEVENTPROCESSOR_H

/**
    @author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

// abstract class which represents the step of analysis

#include <string>
using namespace std;
////////////////////////////////////////////////////////////////////////////////////////////////////
class TjurekAbstractEventProcessor
{
    string proc_name;
public:
//    TjurekAbstractEventProcessor();
    TjurekAbstractEventProcessor(string name_) : proc_name(name_) {} ;
    ~TjurekAbstractEventProcessor();

    virtual void preLoop() = 0;
    virtual void postLoop() = 0;
    void *   GetInputEvent()
    {
        return NULL;
    }
    virtual void Process() = 0;
private:

};

#endif
