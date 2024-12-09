#ifndef TSIGNAL_PROCESSING_H
#define TSIGNAL_PROCESSING_H



namespace TSignal_processing
{

using Sample = int; // int32_t; // type of data buffer

class Signal{
public:
    double  Amplitude; // signal amplitude
    double  Integral;  // signal integral
    double  Tstart;    // start time
    double  Tmax;      // maximum position
    double  pedestal;
};
//***************************************************************************************
bool signal_processing(
    Sample * buf, 		// pointer to the data buffer
    int Nbuf, 		// Number of samples to read,
    Signal * signal 	// output
    );

} // koniec namespace does not need a semicolon


#endif // TSIGNAL_PROCESSING_H
