#include "v830.h"
#include <stdio.h>
#include <string.h>

static v830 deco;

v830::v830():DataDecoder("V830"){
    Reset();
}

int v830::Error(enum ERRORCODE ecode){
    switch(ecode){
        case BadHeader:
            printf("%s error: Bad header\n", type);
            break;
        case BadBlockSize:
            printf("%s error: Bad block size\n", type);
            break;
        default: printf("Unknown error");
    }
    return ecode;
}

void v830::Reset(){
    for(int i=0; i<32; i++) data[i] = 0;
}

int v830::Load(void * ptr){
    Reset();
    uint32_t * buffer = (uint32_t*)ptr;
//    offset = (size_t) buffer;
    if(!CheckStartBlock(buffer[0])) 
         return Error(BadHeader);
    
    geo = (buffer[0]>>27);
    nchan = (buffer[0]>>18) & 0x3F;
    evcount = buffer[0] & 0xFFFF;    
//     if(!CheckEndBlock(buffer[nchan+1])) 
//          return Error(BadBlockSize);

    for(int i=1; i<=nchan;i++){
        data[buffer[i]>>27] = buffer[i] & 0x3FFFFFF;
    }
    
//      Dump();
    return nchan+2;
}

void v830::Dump(){
//    printf("offset %08x\n", offset);
    printf("#Event: %d\n #Geo: %d\n #Nchan: %d\n",evcount, geo, nchan);
    for(int i=0; i<nchan; i++)
        if(data[i] > 0) printf("%d -> %d\n", i, data[i]);
}
