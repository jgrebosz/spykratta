#include "v1724.h"
#include <cstring>
#include <cstdio>
#include "PulseAnalyser.h"

static v1724 deco;

static inline int GetBit(uint32_t  word, int bit){
// return (word & (1<<bit))>>bit;
    return (word >> bit) & 0x1;
}

v1724::v1724(const char * type):DataDecoder(type){
  Reset();
}

void v1724::Reset(){
    marker = 0;
    size = 0;
    channel_mask = 0;
    pattern = 0;
    board_id = 0;
//     compression = 0;
    counter = 0;
    time = 0;
    data = 0;
    buf_size = 0;
    for(int ich=0; ich<Nchan; ich++)
        channel[ich] = 0; 
}

v1724::~v1724(){}

int v1724::Load(void * buf){
    header = (V1724Header *) buf;
// uint32_t * p_data = (uint32_t*)buf;
// int i;
// uint32_t next;
// marker = *p_data&0xF0000000;
// size = *p_data&0x0FFFFFFF;
// p_data++; 
// channel_mask =  *p_data & 0x000000FF;
// pattern      = (*p_data & 0x00FFFF00)>>8;
// next         = (*p_data & 0xFF000000)>>24;
// compression  = next & 0x1;
// board_id     = (next & 0xF8) >> 3;
// p_data++;
// counter = *p_data & 0x00FFFFFF;
// p_data++;
// time = *p_data;
// p_data++;
// data = (uint16_t*) p_data;
    marker = header->tag;
    size = header->size;
    channel_mask = header->channel_mask;
    pattern = header->pattern;
//    compression = header->comp;
    board_id = header->board_id;
    counter = header->counter;
    time = header->time_tag;
    data = (uint16_t*)&header->data;
    
    SetUpChannels();
    SetOutputData();
    if(strcmp(type, "V1724B") == 0) DumpRaw();
    return size;
}

void v1724::ShowHeader(){
printf("#board id: %d\n", board_id);
printf("#size: %d\n", size);
printf("#Channel mask: %d\n", channel_mask);
printf("#Pattern: %d\n", pattern);
printf("#counter: %d\n", counter);
printf("#time: %d\n", time);
// printf("#compression: %d\n", compression);
}

void  v1724::SetUpChannels(){
      int sum = 0;
      for(int ich=0; ich<8; ich++) 
          sum += GetBit(channel_mask,ich);
      buf_size = sum?2*(size-4)/sum:0;
      int i = 0;
      for(int ich=0; ich<8; ich++){
	if(GetBit(channel_mask,ich)){
	    channel[ich] = data+i*buf_size; 
	    i++;
	}
	else channel[ich] = 0;
      }
}

void v1724::DumpRaw(){
    printf("\n");
    uint32_t * p = (uint32_t *)header;
    for( uint32_t * i = p; i< p+4; i++)
        printf("%08x\n", *i);
    
}
void v1724::Dump(){
      ShowHeader();
      printf("#Module %d\n",board_id);
      printf("#ch0\tch1\tch2\tch3\tch4\tch5\tch6\tch7\n");
      uint16_t value;
      int flip = 0;

      for(int j=0; j<buf_size; j++){
	        for(int ich=0; ich<8; ich++){
		  value = channel[ich]? channel[ich][j+flip]: 0;
		  printf("%d\t",value);
		}
		printf("\n");
		flip = -flip;
      }
  }
  
  
DataType v1724::GetData(int ichannel, int ibin){
    if(Empty(ichannel)) return 0;
    if(ibin < Ndata)
        return aux[ichannel][ibin];
    else
    return channel[ichannel][ibin - Ndata];
}

void v1724::SetOutputData(){
    for(int i=0; i<Nchan; i++){
        if(Empty(i)) continue;
        int j = 0;
        anal.SetBuffer(channel[i], buf_size);
        aux[i][j++] = anal.GetTime();
        aux[i][j++] = anal.GetAmplitude();
        aux[i][j++] = anal.GetPedestal();
    }
}

int v1724::GetDataSize(int i){
    return Ndata + buf_size;
}
