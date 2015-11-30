#ifndef TJUREKMBSEVENT_H
#define TJUREKMBSEVENT_H

/**
    @author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

class  TjurekMbsSubEvent;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TjurekMbsEvent
{
public:
    TjurekMbsEvent();

    ~TjurekMbsEvent();
    int GetTrigger()
    {
        return 77;
    }
    TjurekMbsSubEvent*  NextSubEvent()
    {
        return 0;
    }
};

#endif
