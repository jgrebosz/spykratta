#ifndef v1190_h
#define v1190_h
#include <stdint.h>
#include "DataDecoder.h"

struct v1190Header{
//   uint32_t flag:5, ev_count:22, geo:5;
  uint32_t geo:5, ev_count:22, flag:5;
};

struct  v1190Trailer{
//  uint32_t flag:5, status:3, :3, wd_count:16, geo:5;
  uint32_t geo:5, wd_count:16, :3, status:3, flag:5;
};

struct v1190Channel{
//   uint32_t flag:5, tl:1, channel:7, measurement:19;
  uint32_t measurement:19 , channel:7, tl:1, flag:5;
};


class v1190: public DataDecoder{
private:
    
enum ERRORCODE{
        BadHeader = -1,
        BadBlockSize = -2
    };

enum v1190_flags{
    GlobalHeader     = 0x08,
    GlobalTrailer    = 0x10,
    TriggerTimeTag   = 0x11,
    TDCHeader        = 0x01,
    TDCTrailer       = 0x03,
    TDCMeasurement   = 0x00,
    TDCError   	 = 0x04,  
};

static const int Nchan = 160;
v1190Header * header;
v1190Channel * channel;
v1190Trailer * trailer;

public:
    v1190(const char type[6] = "V1190");
    DataDecoder * Clone(){return new v1190(*this);}
    int geo;
    int crate;
    int nchan;
    int evcount;
    int data[Nchan];
    void Reset();
    int Error(enum ERRORCODE);
//     virtual methods
    int Load(void * ptr);
    int GetNchan(){return Nchan;}
    DataType GetData(int channel, int bin){return data[channel];}
    int GetDataSize(int){return 1;}
    void Dump();
    bool Empty(int i){ return !data[i];}
};
#endif
