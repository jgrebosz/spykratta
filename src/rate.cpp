#include "rate.h"
#include <stdio.h>
#include <string.h>

static rate deco;

rate::rate():DataDecoder("RATE"){
    Reset();
}


void rate::Reset(){
    memset(data, 0, sizeof(data));
}

int rate::Load(void * ptr){
    Reset();
    trig_rate = (struct trig*)ptr;
    for(int i=0; i<14; i++){
        data[i][0] = trig_rate->count_rate[i];
        data[i][1] = trig_rate->byte_rate[i];
        data[i][2] = trig_rate->dead_time[i];
    }
    data[14][0] = trig_rate->currentTime[0];
    data[14][1] = trig_rate->currentTime[1];
    data[14][2] = 0;
    
//        Dump();
    return Nchan;
}

ssize_t format_timeval(struct timeval *tv, char *buf, size_t sz)
{
  ssize_t written = -1;
  struct tm *gm = gmtime(&tv->tv_sec);

  if (gm)
  {
    written = (ssize_t)strftime(buf, sz, "%Y-%m-%d %H:%M:%S", gm);
    if ((written > 0) && ((size_t)written < sz))
    {
      int w = snprintf(buf+written, sz-(size_t)written, " + %d usec", tv->tv_usec);
      written = (w > 0) ? written + w : -1;
    }
  }
  return written;
}

void rate::Dump(){
//    printf("offset %08x\n", offset);
//      printf("timestamp: %d %d\n", data[14][0], data[14][1]);
  char buf[40];    
  struct timeval tv = {data[14][0], data[14][1]};
   if (format_timeval(&tv, buf, sizeof(buf)) > 0) 
     printf("%s\n", buf);
  printf("         | %17s | %17s | %9s\n", "count rate", "byte rate", "dead time");
  for(int i=0; i<Nchan-1; i++)
    if(i)
      printf(" Trg %2d: | %12d Ev/s | %12.2f kB/s | %7.3f %%\n", 
	      i, data[i][0], 0.001*data[i][1], 1e-4*data[i][2]);
    else
      printf(" Total : | %12d Ev/s | %12.2f kB/s | %7.3f %%\n", 
	      data[i][0], 0.001*data[i][1], 1e-4*data[i][2]);

}
