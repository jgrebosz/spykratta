#ifndef v2740_h
#define v2740_h
#include "DataDecoder.h"
#include "PulseAnalyser.h"

struct V2740Header{
        uint32_t event_size;
        uint32_t timestamp1;
        uint32_t timestamp2;
        uint32_t trigger_id;
        uint16_t n_channels;
        uint16_t n_samples;
        uint32_t data;
};

class v2740: public DataDecoder{
    friend class Silicon;
private:
    static const int Ndata = 4;
    static const int Nchan = 63;
    V2740Header * header;
//     uint8_t marker;
//     uint32_t size;
//     uint8_t channel_mask;
//     uint16_t pattern;
//     uint8_t  board_id;
// //    uint8_t  compression;
//     uint32_t counter;
//     uint32_t time;
    uint16_t * data;
//     int buf_size;
    uint16_t * channel[Nchan];
    PulseAnalyser<uint16_t> anal;
    double aux[Nchan][Ndata];

    void SetUpChannels();
    void Reset();
    void SetOutputData();
public:
    v2740(const char * type = "V2740");
    ~v2740();
    DataDecoder * Clone(){return new v2740(*this);}
    virtual int Load(void * buf); // Main decoding function
    virtual int GetNchan(){return Nchan;} // returns number of channels in the board
    virtual int GetDataSize(int i);  // returns number of data per channel
    virtual DataType GetData(int channel, int ndata); // Returns data
    virtual int GetXmin(){return 0;} // returns minimal value for data
    virtual int GetXmax(){return 0xFFFF;} // returns max value for data
    virtual int GetNbin(){return header->n_samples;} // returns number of bins
    virtual void Dump();
    virtual void DumpRaw();
    void ShowHeader();
    V2740Header GetHeader(){return *header;}
    int GetSize(){return header->event_size;}
//     int GetBoardID(){return board_id;}
    bool Empty(int i){ return !channel[i];}

};

#endif
