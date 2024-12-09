#include "v2495s.h"
#include <stdio.h>
#include <string.h>

static v2495s deco;

v2495s::v2495s():DataDecoder("V2495S"){
    Reset();
}


void v2495s::Reset(){
    for(int i=0; i<161; i++) data[i] = 0;
}

int v2495s::Load(void * ptr){
    Reset();
    uint32_t * buffer = (uint32_t*)ptr;
    evsize = buffer[0] & 0xFF;
    evcount = buffer[0] & 0x3FF >> 8;
    bool tst = buffer[0] & 0x80000000;
    if(tst)
        data[0] = buffer[1];
    nchan = 160 + tst;

    for(int i=tst; i<nchan; i++)
        data[i] = buffer[i];

    // Dump();
        printf("0x%x\n", buffer[0]);
    // for(int i=0; i<evsize; i++)
    //     printf("%d -> 0x%x\n", i, buffer[i]);
    return nchan;
}

void v2495s::Dump(){
//    printf("offset %08x\n", offset);
    printf("#Event: %d\n #Nchan: %d\n",evcount, nchan);
    for(int i=0; i<nchan; i++)
        if(data[i] > 0) printf("%d -> %d\n", i, data[i]);
}
