#ifndef v1724_h
#define v1724_h
#include "DataDecoder.h"
#include "PulseAnalyser.h"

struct V1724Header{
    uint32_t size:28, tag:4;
    uint32_t channel_mask:8, pattern:15, ZLE:1, RES:3, board_id:5;
    uint32_t counter:23, :9;
    uint32_t time_tag;
    uint32_t data;
};

class v1724: public DataDecoder{
    friend class Kratta;
private:
    static const int Ndata = 3;
    static const int Nchan = 8;
    V1724Header * header;
    uint8_t marker;
    uint32_t size;
    uint8_t channel_mask;
    uint16_t pattern;
    uint8_t  board_id;
//    uint8_t  compression;
    uint32_t counter;
    uint32_t time;
    uint16_t * data;
    int buf_size;
    uint16_t * channel[Nchan];
    PulseAnalyser<uint16_t> anal;
    double aux[Nchan][Ndata];

    void SetUpChannels();
    void Reset();
    void SetOutputData();
public:
    v1724();
    ~v1724();
    DataDecoder * Clone(){return new v1724(*this);}
    virtual int Load(void * buf); // Main decoding function
    virtual int GetNchan(){return Nchan;} // returns number of channels in the board
    virtual int GetDataSize(int i);  // returns number of data per channel
    virtual DataType GetData(int channel, int ndata); // Returns data
    virtual int GetXmin(){return 0;} // returns minimal value for data
    virtual int GetXmax(){return 16368;} // returns max value for data
    virtual int GetNbin(){return 1024;} // returns number of bins
    virtual void Dump();
    void ShowHeader();
    int GetSize(){return size;}
    int GetBoardID(){return board_id;}
    bool Empty(int i){ return !channel[i];}

};

#endif
