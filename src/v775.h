#ifndef v775_h
#define v775_h
#include <stdint.h>
#include "DataDecoder.h"

struct V775Header{
    uint32_t :8, cnt:6, :2, crate:8, check:3, geo:5;
};
struct V775Channel{
    uint32_t adc:12, OV:1, UN:1, VD:1, :1, chan:5, :3, check:3, geo:5;
};
struct V775Trailer{
    uint32_t event_counter:24, check:3, geo:5;
};


class v775: public DataDecoder{
private:
    
    enum ERRORCODE{
        BadHeader = -1,
        BadBlockSize = -2
    };
static const int Nchan = 32;
V775Header * header;
V775Channel * channel;
V775Trailer * trailer;

public:
    v775(const char * type);
    DataDecoder * Clone(){return new v775(*this);}
    int geo;
    int crate;
    int nchan;
    int evcount;
    int data[Nchan];
    void Reset();
//     int CheckStartBlock(uint32_t x){return (x >> 24 & 7) == 2;}
//     int CheckEndBlock(uint32_t x){return (x >> 24 & 7) == 4;}
    int Error(enum ERRORCODE);
//     virtual methods
    int Load(void * ptr);
    int GetNchan(){return 32;}
    DataType GetData(int channel, int bin){return data[channel];}
    int GetDataSize(int){return 1;}
    void Dump();
//     int GetXmin() {return 0;} // returns minimal value for data
//     int GetXmax() {return 1024;} // returns max value for data
//     int GetNbin() {return 1024;}  // returns number of bins
    bool Empty(int i){ return !data[i];}
};
#endif
