#ifndef rate_h
#define rate_h
#include <stdint.h>
#include "DataDecoder.h"

#define MAX__TRIG_TYPE 16

class rate: public DataDecoder{
private:
static const int Nchan = 15;
static const int Ndata = 3;

struct trig{
uint32_t count_rate[MAX__TRIG_TYPE];
uint32_t byte_rate[MAX__TRIG_TYPE];
uint32_t dead_time[MAX__TRIG_TYPE];
uint32_t currentTime[2];
} * trig_rate;


public:
    rate();
    DataDecoder * Clone(){return new rate(*this);}
    uint32_t data[Nchan][Ndata];
    void Reset();
    int Load(void * ptr);
    int GetNchan(){return Nchan;}
    DataType GetData(int channel, int bin){return data[channel][bin];}
    int GetDataSize(int){return Ndata;}
    void Dump();
    bool Empty(int i){ return !data[i][0];}
};
#endif
