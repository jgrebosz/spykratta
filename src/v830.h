#ifndef v830_h
#define v830_h
#include <stdint.h>
#include "DataDecoder.h"

class v830: public DataDecoder{
private:
    
    enum ERRORCODE{
        BadHeader = -1,
        BadBlockSize = -2
    };
    
public:
    v830();
    DataDecoder * Clone(){return new v830(*this);}

    int geo;
//    int crate;
    int nchan;
    int evcount;
    int data[32];
    void Reset();
    int CheckStartBlock(uint32_t x){return (x >> 24 & 7) == 4;}
    int Error(enum ERRORCODE);
//     virtual methods
    int Load(void * ptr);
    int GetNchan(){return 32;}
    DataType GetData(int channel, int bin){return data[channel];}
    int GetDataSize(int){return 1;}
    void Dump();
    int GetXmin() {return 0;} // returns minimal value for data
    int GetXmax() {return 67108864;} // returns max value for data
    int GetNbin() {return 16384;}  // returns number of bins
    bool Empty(int i){ return !data[i];}

};
#endif
