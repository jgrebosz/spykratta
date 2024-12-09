#ifndef v2495s_h
#define v2495s_h
#include <stdint.h>
#include "DataDecoder.h"

class v2495s: public DataDecoder{
private:
public:
    v2495s();
    DataDecoder * Clone(){return new v2495s(*this);}
    int nchan;
    int evcount;
    int evsize;
    int data[161];
    void Reset();
//     virtual methods
    int Load(void * ptr);
    int GetNchan(){return nchan;}
    DataType GetData(int channel, int bin){return data[channel];}
    int GetDataSize(int){return 1;}
    void Dump();
    int GetXmin() {return 0;} // returns minimal value for data
    int GetXmax() {return 67108864;} // returns max value for data
    int GetNbin() {return 1;}  // returns number of bins
    bool Empty(int i){ return !data[i];}

};
#endif
