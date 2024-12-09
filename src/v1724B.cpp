#include "v1724B.h"
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <iostream>
#include "PulseAnalyser.h"

static V1724B deco;

static inline int GetBit(uint32_t  word, int bit){
    return (word >> bit) & 0x1;
}

enum ERRORCODE{
        V1724B_BadHeader = -1,
        V1724B_ChannelBadHeader = -2,
        BadBlockSize = -3,
	BoardFailFlag = -4
    };
    
static int Error(enum ERRORCODE ecode){
    switch(ecode){
        case V1724B_BadHeader:
            printf("V1724B error: Bad event header\n");
            break;
        case V1724B_ChannelBadHeader:
            printf("V1724B error: Bad channel header\n");
            break;
        case BadBlockSize:
            printf("V1724B error: Bad block size\n");
            break;
        case BoardFailFlag:
            printf("V1724B error: Board Fail Flag\n");
            break;
        default: printf("Unknown error");
    }
    return ecode;
}

    int V1724B_Channel::SetBuffer(uint32_t * ptr){
        uint32_t * word = ptr;
        header = (V1724B_ChannelHeader*)word++;
// 	printf("channel_header: %08x\n", *header);
	if(header->FI)
	  format = (V1724B_ChannelFormat*)word++;
// 	if(format && format->ET)
	  timetag = (V1724B_ChannelTimeTag*)word++;
// 	if(format && format->ES)
	  sample = (V1724B_ChannelSample*)word++;
// 	if(format && format->EE)
	  amplitude = (V1724B_ChannelAmplitude*)((uint32_t*)ptr + header->size - 1);
        return header->size;
    }

void V1724B_Channel::Dump(){
  
    std::cout << "#Channel size: " 	<< GetSize() 	  << std::endl;
    std::cout << "#Samples: " 		<< GetNsamp() 	  << std::endl;
    std::cout << "#Trigger time tag: "	<< GetTimeTag()   << std::endl;
    std::cout << "#Amplitude: " 	<< GetAmplitude() << std::endl;
    std::cout << "#Extras: " 		<<  GetExtras()   << std::endl;  
    std::cout << "#Analog probe 1: " 	<<  GetAP1()      << std::endl;  
    std::cout << "#Analog probe 2: " 	<<  GetAP2() 	  << std::endl;  
    std::cout << "#Digital probe: " 	<<  GetDP() 	  << std::endl;  
    for(int i=0; i< GetNsamp(); i++)
      std::cout << i << "\t" << GetSample(i)->value << "\t" << GetSample(i)->D  << "\t" << GetSample(i)->T<< std::endl;
  
}



V1724B_Event::V1724B_Event(){
    Reset();
}

int V1724B_Event::Load(uint32_t * buf){
    buffer = buf;
    header = (V1724B_EventHeader*)buffer;
    if(header->tag != 0xA)
        return Error(V1724B_BadHeader);
    else{
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

void V1724B_Event::ShowHeader(){
printf("#GEO: %u\n", header->board_id);
printf("#Size: %u\n", header->size);
printf("#Channel mask: %s\n", binary(header->channel_mask));
printf("#Pattern: %s\n",binary(header->pattern));
printf("#counter: %u\n", header->counter);
printf("#time tag: %u\n", header->time_tag);
}

void V1724B_Event::Dump(){
    ShowHeader();
    
    for(int ich=0; ich<8; ich++)
        if(channel[ich]){
	    printf("########### Channel %d\n", ich);
            channel[ich].Dump();
        }
 }

  void V1724B_Event::DumpRaw(){
        printf("size: %d\n", size);
    for(unsigned int i=0; i<size; i++){
        printf("0x%08x (%d)\n", buffer[i], buffer[i]);
//         std::cout << std::hex << std::setw(8) << std::setfill('0')<< buffer[i]<< " (" << std::dec << buffer[i] << ")" << std::endl;
    }
}

void  V1724B_Event::SetUpChannels(uint32_t * ptr){
      for(int ich=0; ich<8; ich++){
	if(GetBit(channel_mask,ich))
	    ptr += channel[ich].SetBuffer(ptr); 
//            ptr += *ptr & 0x3FFFFF;
	else channel[ich].Reset();
      }
}


void V1724B_Event::Reset(){
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

V1724B::V1724B(const char * type):DataDecoder(type), channel(event.channel){
    Reset();
}

int V1724B::GetDataSize(int i){
   return Ndata + channel[i].GetNsamp(); 
}

DataType V1724B::GetData(int chan, int bin){
    int sampl = channel[chan].GetNsamp();
    if(bin < Ndata)
        return data[chan][bin];
    else 
        if(sampl && bin - Ndata < sampl)
            return channel[chan].sample[bin - Ndata].value;
    else
        return 0;
}

void V1724B::Reset(){
     event.Reset();
     memset(data,0, sizeof(data));
     memset(channel, 0, sizeof(channel));
}

int V1724B::Load(void * ptr){
    Reset();
    uint32_t * buffer = (uint32_t*)ptr;
//    event = (V1724B_Event*)buffer;
    buffer += event.Load(buffer);
     SetOutputData();
    return buffer - (uint32_t*)ptr;
}

void V1724B::SetOutputData(){
    static uint16_t trace[2000];
    static PulseAnalyser<uint16_t> anal;
    for(int i=0; i<Nchan; i++){
        if(Empty(i)) continue;
        int j=0;
        if(event.channel[i].sample){
            int Nsamp = event.channel[i].GetNsamp();
            for(int s=0; s<Nsamp; s++)
                trace[s] = event.channel[i].GetSample(s)->value;  
            anal.SetBuffer(trace, Nsamp);
            data[i][j++] = anal.GetTime();
            data[i][j++] = anal.GetAmplitude();
            data[i][j++] = event.channel[i].GetAmplitude();
        }
        else{
            data[i][j++] = event.channel[i].GetTimeTag()/* - event.time_tag*/;
            data[i][j++] = event.channel[i].GetAmplitude();
            data[i][j++] = 0;
        }
    }
}
  
void V1724B::Dump(){
event.Dump();
// printf("Raw\n");
event.DumpRaw();

for(int i=0; i<Nchan; i++){
//    std::cout << "ch " << std::setw(2) << i << ":";
//     if(Empty(i)) continue;
//    channel[i].Dump();
    for(int j=0; j<Ndata; j++ )
        std::cout << std::setw(16)<< data[i][j];
    std::cout << std::endl;
}
}
    
