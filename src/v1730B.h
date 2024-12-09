#ifndef v1730B_h
#define v1730B_h
#include <stdint.h>
#include "DataDecoder.h"

struct AggregateHeader{
    uint32_t size:22, :9, check:1;
    uint32_t samples:16, DP1:3, DP2:3, AP:2, EX:3, ES:1, EE:1, ET:1, EQ:1, DT:1;
    uint32_t time_tag:31, CH:1;
};

struct AggregateFooter{
    uint16_t extras[2];
    uint32_t q_short:15, PUR:1, q_long:16; 
};

struct AggregateSample{
    uint16_t value:14, DP1:1, DP2:1;
};


class Aggregate{
    friend class v1730B;
private:
   AggregateHeader * header;
   AggregateFooter * footer;
   AggregateSample * sample;
public:
    Aggregate(): header(0), footer(0), sample(0){}
    
    int SetBuffer(void * ptr);
    AggregateHeader * GetHeader(){return header;}
    AggregateFooter * GetFooter(){return footer;}
    AggregateSample * GetSample(int i){
        return sample && header->samples ? sample + i : 0;
    }
    int GetSize(){return header->size;}
    int GetNsamp(){return header->samples*8;}
    int GetTimeTag(){return header->time_tag;}
    int GetExtras(int i){return footer->extras[i];}
    int GetQshort(){return footer->q_short;}
    int GetQlong(){return footer->q_long;}
    int GetParity(){return header->CH;}
    double GetFineTimeStamp(){
        return header->EX == 2 ? (float)(GetExtras(0) & 0x3FF) / 0x400 : 0;
    }
    
    int GetExtendedTimeStamp(){
        return header->EX <3 ? GetExtras(1): 0;
    }
    
    double GetTime(){
        return  GetExtendedTimeStamp() + GetTimeTag() + GetFineTimeStamp();
    }
    void Reset(){header = 0; footer = 0; sample = 0;}
    void Dump();
    operator bool(){
        return header;
    }
};

struct SubeventHeader{
    uint32_t size:28, tag:4;
    uint32_t channel_mask:8, pattern:15, :3, BF:1, board_id:5;
    uint32_t counter:23, :9;
    uint32_t time_tag;
    uint32_t data;
};

class V1730_Subevent{
private:
    uint32_t * buffer;
    SubeventHeader * header;
    uint32_t size;
    uint8_t  geo;
    uint16_t pattern;
    uint8_t channel_mask;
    uint32_t counter;
    uint32_t time_tag;
    bool board_fail;
    
    Aggregate channel[8];
    uint32_t * data;
    
public:
    V1730_Subevent();
    int Load(uint32_t * buf);
    void SetUpChannels(uint32_t * ptr);
    void ShowHeader();
    void Dump();
    void DumpRaw();
    int GetSize(){return size;}
    
    void Reset();
    Aggregate & operator[](int i){return channel[i];}
};

class v1730B: public DataDecoder{
private:
    static const int Nchan = 16;
    static const int Ndata = 5;
    
    int Nsubevt; // Number of subevents
    V1730_Subevent subevt;
    Aggregate channel[Nchan];
    void SetOutputData();

public:
    v1730B(const char * type = "V1730B");
    DataDecoder * Clone(){return new v1730B(*this);}
    void Reset();
    double data[Nchan][Ndata];
    
//     virtual methods
    int Load(void * ptr);
    int GetNchan(){return Nchan;}
    DataType GetData(int chan, int bin);/*{
        return data[chan][bin];
    }*/
    
    int GetDataSize(int i);//{return Ndata;}
    
    void Dump();
/*    int GetXmin() {return 0;} // returns minimal value for data
    int GetXmax() {return 0;} // returns max value for data
    int GetNbin() {return 0;}  // returns number of bins
*/   
bool Empty(int i){return !channel[i];}

};
#endif
