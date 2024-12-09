#include "v1190.h"
#include <stdio.h>
#include <string.h>

static v1190 deco;

v1190::v1190(const char type[6]):DataDecoder(type){
    Reset();
}

int v1190::Error(enum ERRORCODE ecode){
    switch(ecode){
    case BadHeader:
        printf("Error: Bad header\n");
        break;
    case BadBlockSize:
        printf("Error: Bad block size\n");
        break;
    default: printf("Unknown error");
    }
    return ecode;
}

void v1190::Reset(){
    geo = 0;
    //     crate = 0;
    //     nchan = 0;
    evcount = 0;
    header = 0;
    channel = 0;
    trailer = 0;
    for(int i=0; i<Nchan; i++) data[i] = 0;
}

int v1190::Load(void * ptr){
    Reset();
    uint32_t * buffer = (uint32_t*)ptr;
    header = (v1190Header *) ptr;
    //     printf("header->flag: %08x\n", header->flag);
    if(header->flag != GlobalHeader)
        return Error(BadHeader);
    geo = header->geo;
    evcount = header->ev_count;
    buffer++;
    do{
        trailer = (v1190Trailer*)buffer++;
        if(trailer->flag == TDCMeasurement){
            channel = (v1190Channel *) trailer;
            data[channel->channel] = channel->measurement;
        }
    }
    while(trailer->flag != GlobalTrailer);

    int event_size = buffer - (uint32_t*)ptr;

    if(trailer->wd_count != event_size)
        return Error(BadBlockSize);
    //       Dump();
    return event_size;
}

void v1190::Dump(){
    //    printf("offset %08x\n", offset);
    printf("#Event: %d\n #Geo: %d\n",evcount, geo);
    for(int i=0; i<Nchan; i++)
        if(data[i] > 0) printf("%d -> %d\n", i, data[i]);
}
