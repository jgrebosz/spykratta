#include "v1730B.h"
#include <stdio.h>
#include <string.h>
#include <iomanip>

static v1730B deco;

static inline int GetBit(uint32_t  word, int bit){
    return (word >> bit) & 0x1;
}

enum ERRORCODE{
        SubeventBadHeader = -1,
        AggregateBadHeader = -2,
        BadBlockSize = -3,
	BoardFailFlag = -4
    };
    
static int Error(enum ERRORCODE ecode){
    switch(ecode){
        case SubeventBadHeader:
            printf("V1730B error: Subevent Bad header\n");
            break;
        case AggregateBadHeader:
            printf("V1730B error: Aggregate Bad header\n");
            break;
        case BadBlockSize:
            printf("V1730B error: Bad block size\n");
            break;
        case BoardFailFlag:
            printf("V1730B error: Board Fail Flag\n");
            break;
        default: printf("Unknown error");
    }
    return ecode;
    return 0;
}

    int Aggregate::SetBuffer(void * ptr){
        header = (AggregateHeader*)ptr;
        if(!header->check)
            return Error(AggregateBadHeader);
        footer = (AggregateFooter*)((uint32_t*)ptr + header->size - 2);
        sample = header->samples ? (AggregateSample*)((uint8_t*)ptr + sizeof(AggregateHeader)) : 0;
        return header->size;
    }

void Aggregate::Dump(){
    std::cout << "Channel size: " << header->size << std::endl;
    std::cout << "Samples: " << header->samples << std::endl;
    std::cout << "Trigger time tag: " << header->time_tag << std::endl;
    std::cout << "Flags:" 
              << " DP1:"<<header->DP1
              << " DP2:"<<header->DP2
              << " EX:"<< header->EX
              << " ES:"<< header->ES
              << " EE:"<< header->EE
              << " ET:"<< header->ET
              << " EQ:"<< header->EQ
              << " DT:"<< header->DT
              << " CH:"<< header->CH
              <<std::endl;
//                   for(int i=0; i<GetNsamp(); i++)
//                       std::cout << i << " " << sample[i].value << std::endl;
}



V1730_Subevent::V1730_Subevent(){
    Reset();
}

int V1730_Subevent::Load(uint32_t * buf){
    buffer = buf;
    header = (SubeventHeader*)buffer;
    if(header->tag != 0xA)
        return Error(SubeventBadHeader);
//     size = buffer[0]& 0x0FFFFFFF;
//     geo = buffer[1]>>27;
//     pattern = buffer[1] & 0x7FFF >> 8;
//     channel_mask = buffer[1] & 0xFF;
//     counter = buffer[2] & 0x7FFFFF;
//     time_tag = buffer[3];
//     data = &buffer[4];
    size = header->size;
    geo = header->board_id;
    pattern = header->pattern;
    channel_mask = header->channel_mask;
    counter = header->counter;
    time_tag = header->time_tag;
    data = &header->data;
    SetUpChannels(data);
    return size;
}

template <typename Type>
char * binary(Type word){
    static const int size = sizeof(word)*8;
    static char buf[size+1];
    buf[size] = 0;
    for(int i=0; i<size; i++){
        buf[size-i-1] = 48 + GetBit(word, i);
    }
    return buf;
}

void V1730_Subevent::ShowHeader(){
printf("#GEO: %u\n", geo);
printf("#Size: %u\n", size);
printf("#Channel mask: %s\n", binary(channel_mask));
printf("#Pattern: %s\n",binary(pattern));
printf("#counter: %u\n", counter);
printf("#board aggregate time tag: %u\n", time_tag);
}

void V1730_Subevent::Dump(){
    ShowHeader();
    for(int ich=0; ich<8; ich++)
        if(channel[ich]){
            channel[ich].Dump();
        }

  }
void V1730_Subevent::DumpRaw(){
    for(unsigned int i=0; i<size; i++){
        std::cout << std::hex << std::setw(8) << std::setfill('0')<< buffer[i]<< " (" << std::dec << buffer[i] << ")" << std::endl;
    }
}

void  V1730_Subevent::SetUpChannels(uint32_t * ptr){
      for(int ich=0; ich<8; ich++){
	if(GetBit(channel_mask,ich))
	    ptr += channel[ich].SetBuffer(ptr); 
//            ptr += *ptr & 0x3FFFFF;
	else channel[ich].Reset();
      }
}


void V1730_Subevent::Reset(){
    buffer = 0;
    size = 0;
    geo = 0;
    pattern = 0;
    channel_mask = 0;
    counter = 0;
    time_tag = 0;
    data = 0;
    memset(channel, 0, sizeof(channel));
}

v1730B::v1730B():DataDecoder("V1730B"){
    Reset();
}

int v1730B::GetDataSize(int i){
   return Ndata + channel[i].GetNsamp(); 
}

DataType v1730B::GetData(int chan, int bin){
    int sampl = channel[chan].GetNsamp();
    if(bin < Ndata)
        return data[chan][bin];
    else 
        if(sampl && bin - Ndata < sampl)
            return channel[chan].sample[bin - Ndata].value;
    else
        return 0;
}

void v1730B::Reset(){
    subevt.Reset();
    memset(data,0, sizeof(data));
    memset(channel, 0, sizeof(channel));
}

int v1730B::Load(void * ptr){
    Reset();
    uint32_t * buffer = (uint32_t*)ptr;
    Nsubevt = buffer[0];    
    ++buffer;
    for(int i=0; i < Nsubevt; ++i){
            subevt.Reset();
            buffer += subevt.Load(buffer);
            for(int i=0; i<8; ++i)
                if(subevt[i]){
                    int chan = 2*i + subevt[i].GetParity();
                    channel[chan] = subevt[i];
            }
    }
    
     SetOutputData();
    return buffer - (uint32_t*)ptr;
}

void v1730B::SetOutputData(){
    double Tref = 0;
    for(int i=0; i<Nchan; i++){
        if(Empty(i)) continue;
        int j=0;
        data[i][j++] = channel[i].GetTime() - Tref;
        // Qshort
        data[i][j++] = channel[i].GetQshort();
        // Qlong
        data[i][j++] = channel[i].GetQlong();
        // Extras
        data[i][j++] = channel[i].GetExtendedTimeStamp();
        data[i][j++] = channel[i].GetFineTimeStamp();
    }
}
  
void v1730B::Dump(){
    
for(int i=0; i<Nchan; i++){
    if(Empty(i)) continue;
    channel[i].Dump();
    std::cout << "ch " << std::setw(2) << i << ":";
    for(int j=0; j<Ndata; j++ )
        std::cout << std::setw(16)<< data[i][j];
    std::cout << std::endl;
}
}
