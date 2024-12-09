#ifndef PulseAnalyser_h
#define PulseAnalyser_h

//typedef double DataType;
template <typename DataType>
class PulseAnalyser{
private:
    const DataType * buf = 0;
    int Nbuf = 0;
    int pedestal_bins = 50;
    float fraction = 0.2;
    float amplitude = 0;
    float time = 0;
    float pedestal = 0;
    int polarity;

public:
    PulseAnalyser(const DataType * buffer = 0, int size = 0, int pol = 1);
    void SetBuffer(const DataType * buffer, int size, int po = 1);
    void SetPedestalBins(int n){pedestal_bins = n;}
    void SetFraction(DataType f){fraction = f;}
    float LeadingEdgeTime(float thresh);
    float ConstantFractionTime(float thres, float delay, float fract);
    float GetAmplitude(int start = 0, int stop = 0);
    float GetRiseTime();
    float GetTime();
    float GetPedestal();
};

template <typename DataType>
PulseAnalyser<DataType>::PulseAnalyser(const DataType * buffer, int size, int pol){
    SetBuffer(buffer, size, pol);
}

template <typename DataType>
void PulseAnalyser<DataType>::SetBuffer(const DataType * buffer, int size, int pol){
    buf = buffer;
    Nbuf = size;
    polarity =  pol > 0 ? 1 : -1;
    pedestal = amplitude = time = 0;
}

template <typename DataType>
float PulseAnalyser<DataType>::GetPedestal(){
    if(pedestal) return pedestal;
    if(!buf) return 0;
    float sum = 0;
    if(pedestal_bins > Nbuf)
        printf("Warning! pedestal_bins: %d greater than buffer size %d\n",pedestal_bins, Nbuf);
    for(int j=0; j<pedestal_bins; j++)
        sum += buf[j];
    return pedestal = sum/pedestal_bins;
}
using namespace std;
template <typename DataType>
float PulseAnalyser<DataType>::LeadingEdgeTime(float thresh){
    if(!pedestal) GetPedestal();
    float value1 = 0, value2 = polarity*(buf[0] - pedestal);
    for(int j=1; j<Nbuf; j++){
        value1 = value2;
        value2 = polarity*(buf[j] - pedestal);

        // cout << "v1 = " << value1 << " val2 = " << value2 << endl;
        if(value2 > thresh)
            return (value2 == value1) ? 0 : j - 1 + (thresh - value1)/(value2 - value1);
    }
    return 0;
}

template <typename DataType>
float PulseAnalyser<DataType>::ConstantFractionTime(float thresh, float delay, float fract){
    float start = LeadingEdgeTime(thresh);
    float stop = start + delay;
    float ampl = GetAmplitude(start, stop);
    return LeadingEdgeTime(fract*ampl);
}

template <typename DataType>
float PulseAnalyser<DataType>::GetRiseTime(){
    if(!amplitude) GetAmplitude();
    float t_low = LeadingEdgeTime(0.1*amplitude);
    float t_high = LeadingEdgeTime(0.9*amplitude);
    return t_high - t_low;
}

template <typename DataType>
float PulseAnalyser<DataType>::GetAmplitude(int start, int stop){
    if(amplitude) return amplitude;
    if(!pedestal) GetPedestal();
    if(!buf) return 0;
    if(!stop) stop = Nbuf;
    amplitude = buf[start];
    if(polarity == 1){
        for(int j=start; j<stop; j++)
            if(buf[j] > amplitude)
                amplitude = buf[j];
        amplitude -= pedestal;
    }
    else{
        for(int j=start; j<stop; j++)
            if(buf[j] < amplitude)
                amplitude = buf[j];
        amplitude = pedestal - amplitude;
    }
    return amplitude;
}

template <typename DataType>
float PulseAnalyser<DataType>::GetTime(){
    if(time) return time;
    if(!amplitude) GetAmplitude();
    if(!buf) return 0;
    float thresh =  fraction*amplitude;
    return time = LeadingEdgeTime(thresh);
    //     if(polarity == 1){
    //         float value1 = 0, value2 = buf[0] - pedestal;
    //         for(int j=1; j<Nbuf; j++){
    //             value1 = value2;
    //             value2 = buf[j] - pedestal;
    //             if(value2 > thresh)
    //             return time = j - 1 + (thresh - value1)/(value2 - value1);
    //         }
    //     }
    //     else{
    //         float value1 = 0, value2 = pedestal - buf[0];
    //         for(int j=1; j<Nbuf; j++){
    //             value1 = value2;
    //             value2 = pedestal - buf[j];
    //             if(value2 > thresh)
    //             return time = j - 1 + (thresh - value1)/(value2 - value1);
    //         }
    //     }
    return 0;
}





#endif
