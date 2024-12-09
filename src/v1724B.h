#ifndef v1724B_h
#define v1724B_h
#include <stdint.h>
#include "DataDecoder.h"

struct V1724B_ChannelHeader{
    uint32_t size:21, :10, FI:1;
};

struct V1724B_ChannelFormat{
    uint32_t samples:16, DP:4, AP2:2, AP1:2, :4, ET:1, EE:1, ES:1, DT:1;
};

struct V1724B_ChannelTimeTag{
    uint32_t time_tag:30, :1, TT:1;
};

struct V1724B_ChannelSample{
    uint16_t value:14, D:1, T:1;
};

struct V1724B_ChannelAmplitude{
    uint32_t energy:15, PU:1, extras:4, :12; 
};


class V1724B_Channel{
    friend class V1724B;
private:
   V1724B_ChannelHeader * header;
   V1724B_ChannelFormat * format;
   V1724B_ChannelTimeTag * timetag;
   V1724B_ChannelAmplitude * amplitude;
   V1724B_ChannelSample * sample;
public:
    V1724B_Channel(): header(0), format(0), timetag(0), amplitude(0), sample(0){}
    
    int SetBuffer(uint32_t * ptr);
    V1724B_ChannelSample * GetSample(int i){return sample ? sample + i : 0;}
    int GetSize(){return header->size;}
    int GetNsamp(){return format ? format->samples*2 : 2*(header->size - 3);}
    int GetDP(){return format ? format->DP : -1;}
    int GetAP1(){return format ? format->AP1 : -1;}
    int GetAP2(){return format ? format->AP2 : -1;}
    int GetDT(){return format ? format->DT : -1;}
    int GetES(){return format ? format->ES : -1;}
    int GetEE(){return format ? format->EE : -1;}
    int GetET(){return format ? format->ET : -1;}
    int GetTimeTag(){return timetag ? timetag->time_tag : 0;}
    int GetAmplitude(){return amplitude ? amplitude->energy : 0;}
    int GetExtras(){return amplitude ? amplitude->extras :0;}    
    void Reset(){header = 0; format = 0; timetag = 0; amplitude = 0; sample = 0;}
    void Dump();
    operator bool(){return header;}
};

struct V1724B_EventHeader{
    uint32_t size:28, tag:4;
    uint32_t channel_mask:8, pattern:15, :3, BF:1, board_id:5;
    uint32_t counter:23, :9;
    uint32_t time_tag;
    uint32_t data;
};

class V1724B_Event{
  friend class V1724B;
private:
    uint32_t * buffer;
    V1724B_EventHeader * header;
    uint32_t size;
    uint8_t  geo;
    uint16_t pattern;
    uint8_t channel_mask;
    uint32_t counter;
    uint32_t time_tag;
    bool board_fail;
    
    V1724B_Channel channel[8];
    uint32_t * data;
    
public:
    V1724B_Event();
    int Load(uint32_t * buf);
    void SetUpChannels(uint32_t * ptr);
    void ShowHeader();
    void Dump();
    void DumpRaw();
    int GetSize(){return size;}
    
    void Reset();
    V1724B_Channel & operator[](int i){return channel[i];}
};


class V1724B: public DataDecoder{
private:
    static const int Nchan = 8;
    static const int Ndata = 3;
    V1724B_Event event;
    V1724B_Channel * channel;
    void SetOutputData();

public:
    V1724B(const char * type = "V1724B");
    DataDecoder * Clone(){return new V1724B(*this);}
    void Reset();
    double data[Nchan][Ndata];
    
//     virtual methods
    int Load(void * ptr);
    int GetNchan(){return Nchan;}
    DataType GetData(int chan, int bin);
    V1724B_Event GetRawEvent() {return event;}
    int GetDataSize(int i);
    
    void Dump();
    bool Empty(int i){return !channel[i];}
};
#endif
