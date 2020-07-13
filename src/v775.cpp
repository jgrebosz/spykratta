#include "v775.h"
#include <stdio.h>
#include <string.h>

static v775 deco("V775");

v775::v775(const char * type):DataDecoder(type){
    Reset();
}

int v775::Error(enum ERRORCODE ecode){
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

void v775::Reset(){
    geo = 0;
    crate = 0;
    nchan = 0;
    evcount = 0;
    header = 0;
    channel = 0;
    trailer = 0;
    for(int i=0; i<Nchan; i++) data[i] = 0;
}

int v775::Load(void * ptr){
    Reset();
    uint32_t * buffer = (uint32_t*)ptr;
    header = (V775Header *) ptr;
//    offset = (size_t) buffer;
//     if(!CheckStartBlock(buffer[0])) 
//          return Error(BadHeader);
    if(header->check != 2)
        return Error(BadHeader);
//     geo = (buffer[0]>>24) & 0xF8;
//     crate = (buffer[0]>>16) & 0xFF;
//     nchan = (buffer[0]>>8) & 0xFF;
        geo = header->geo;
        crate = header->crate;
        nchan = header->cnt;
    trailer = (V775Trailer*) &buffer[nchan+1];
    if(trailer->check != 4)
        return Error(BadBlockSize);
//     if(!CheckEndBlock(buffer[nchan+1])) 
//          return Error(BadBlockSize);

    for(int i=1; i<=nchan;i++){
        channel = (V775Channel *) &buffer[i];
        data[channel->chan] = channel->adc;
//        data[(buffer[i]>>16)&0xFF] = buffer[i] & 0xFFF;
    }
//    evcount = buffer[nchan+1] & 0xFFFFFF;
      evcount = trailer->event_counter;
//       Dump();
    return nchan+2;
}

void v775::Dump(){
//    printf("offset %08x\n", offset);
    printf("#Event: %d\n #Geo: %d\n #Crate: %d\n #Nchan: %d\n",evcount, geo, crate, nchan);
    for(int i=0; i<32; i++)
        if(data[i] > -1) printf("%d -> %d\n", i, data[i]);
}
