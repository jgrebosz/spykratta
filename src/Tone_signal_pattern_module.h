#ifndef _one_signal_pattern_module_H_
#define _one_signal_pattern_module_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__



#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>

#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

//class focus_point ;

//////////////////////////////////////////////////////////////////////////////
class Tone_signal_pattern_module : public Tfrs_element , public Tincrementer_donnor
{

    int signal_data;
    int our_incrementer[32]; // Roman wants a separated incrementer for each bit in the signal word

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*signal_ptr;

    spectrum_1D * spec_first ;

public:
    //  constructor
    Tone_signal_pattern_module(string _name, int TIFJEvent::*first_ptr) ;

    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates
    unsigned int give_value()
    {
        return signal_data;
    }

protected:
    void create_my_spectra();
};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _one_signal_pattern_module_H_
