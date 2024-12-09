#include "tsignal_processing.h"

#include <cstring>
//*************************************************************************************
bool TSignal_processing::signal_processing(
            Sample * buf, 		// pointer to the data buffer
            int Nbuf, 		// Number of samples to read,
            Signal * signal 	// output
            )
{
  constexpr int adc_scale = 16383; //14-bit ADC
  constexpr float threshold = 20;
  constexpr int pedestal_bins = 50;

  memset(signal, 0, sizeof(Signal));

  float sum = 0;
  float integ = 0;
  float value = 0, value_max = 0, tmax = 0;
  bool positive = buf[0] < adc_scale/2;

// Loop over samples
  for(int j=0; j<Nbuf; j++){

    value = (
        positive ? buf[j] :
        adc_scale - buf[j]
    );

    if(j <= pedestal_bins) sum += value;

    integ += value;
    if(value > value_max && value <= adc_scale) {
        value_max = value;
        tmax = j;
    }
  }

  double pedestal = sum/pedestal_bins;
  signal->Amplitude = value_max - pedestal;
  signal->Tmax = tmax;
  signal->Integral = integ - Nbuf*pedestal;
  signal->pedestal = pedestal;

  float thresh = pedestal + threshold;

  for(int j=tmax; j>0; j--){
  value = (
        positive ? buf[j] :
        adc_scale - buf[j]
    );
      if(value < thresh) {
          signal->Tstart = j;
          break;
      }
  }

// returns true if the signal looks reasonable, otherwise returns false
  return (signal->Tstart > 0 && signal->Integral > 0);
}

