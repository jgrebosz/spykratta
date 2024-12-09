#include "v2740.h"
#include <cstring>
#include <cstdio>
#include "PulseAnalyser.h"

static v2740 deco;

static inline int GetBit(uint32_t  word, int bit){
    // return (word & (1<<bit))>>bit;
    return (word >> bit) & 0x1;
}

v2740::v2740(const char * type):DataDecoder(type){
    Reset();
}

void v2740::Reset(){
    //     marker = 0;
    //      size = 0;
    //     channel_mask = 0;
    //     pattern = 0;
    //     board_id = 0;
    //     compression = 0;
    //     counter = 0;
    //     time = 0;
    //     data = 0;
    //     buf_size = 0;
    for(int ich=0; ich<Nchan; ich++)
        channel[ich] = 0; 
}

v2740::~v2740(){}


int v2740::Load(void * buf){

    header = (V2740Header *) buf;
    //     ShowHeader();
    data = (uint16_t*)&header->data;
    //     printf("event size: %d\n", header->event_size);
    SetUpChannels();
    SetOutputData();
    return header->event_size;
}

void v2740::ShowHeader(){
    printf("#event_size: %u\n", header->event_size);
    printf("#timestamp1: %u\n", header->timestamp1);
    printf("#timestamp2: %u\n", header->timestamp2);
    printf("#trigger_id: %u\n", header->trigger_id);
    printf("#n_samples : %u\n", header->n_samples);
}

void  v2740::SetUpChannels(){
    uint32_t * ptr  = &header->data;
    uint32_t * ptr_stop = (uint32_t*)header + header->event_size/4;
    //     printf("ptr, ptr_stop %x, %x\n", ptr, ptr_stop);
    memset(channel, 0, sizeof(channel));

    struct channelData{
        uint16_t id;
        uint16_t size;
    } * chan;

    while(ptr < ptr_stop){
        chan = (struct channelData*)ptr++;
        if(chan->id > 63 ){
            printf("warning: channelData: id = %u, so bigger than 63 (size %u) \n", chan->id, chan->size);
        }
        else {
            if(chan->size)
                channel[chan->id] = (uint16_t*)ptr;
        }
        ptr += chan->size/2;
    }
}

void v2740::DumpRaw(){
    printf("\n");
    uint32_t * p = (uint32_t *)header;
    for( uint32_t * i = p; i< p+4; i++)
        printf("%08x\n", *i);
}

void v2740::Dump(){
    ShowHeader();

    for(int j=0; j<header->n_samples; j++){
        for(int ich=0; ich<Nchan; ich++){
            printf("%hu\t", channel[ich] ? channel[ich] : 0);
        }
        printf("\n");
    }
}

DataType v2740::GetData(int ichannel, int ibin){
    if(Empty(ichannel)) return 0;
    if(ibin < Ndata)
        return aux[ichannel][ibin];
    else
        return channel[ichannel][ibin - Ndata];
}

void v2740::SetOutputData(){
    for(int i=0; i<Nchan; i++){
        if(Empty(i)) continue;
        int j = 0;
        anal.SetBuffer(channel[i], header->n_samples);
        aux[i][j++] = anal.GetTime();
        aux[i][j++] = anal.GetAmplitude();
        aux[i][j++] = anal.GetPedestal();
        aux[i][j++] = anal.GetRiseTime();
    }
}

int v2740::GetDataSize(int i){
    return Ndata + header->n_samples;
}
