#ifndef TJUREKMBSSUBEVENT_H
#define TJUREKMBSSUBEVENT_H

/**
    @author Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/
class TjurekMbsSubEvent
{
public:
    TjurekMbsSubEvent();

    ~TjurekMbsSubEvent();
    void *  GetDataField()
    {
        return 0;
    }
    int GetDlen()
    {
        return 77;
    }

    int GetControl()
    {
        return 77;
    }
    int GetAllocatedLength()
    {
        return 77;
    }
    int GetProcid()
    {
        return 77;
    }

};

#endif
