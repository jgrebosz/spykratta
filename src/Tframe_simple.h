#ifndef _FRAME_SIMPLE_H_
#define _FRAME_SIMPLE_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__



#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>

#include "TIFJEvent.h"




//////////////////////////////////////////////////////////////////////////////
class Tframe_simple : public Tfrs_element
{

    double distance ;


    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*signal;

    spectrum_1D * spec_signal_raw ;


public:
    //  constructor
    Tframe_simple(string _name, int TIFJEvent::*signal_ptr) ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates


    double give_distance()
    {
        return distance ;
    }
protected:
    void create_my_spectra();
    //void check_legality_of_data_pointers();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _FRAME_SIMPLE_H_
