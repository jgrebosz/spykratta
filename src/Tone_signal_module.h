#ifndef _one_signal_module_H_
#define _one_signal_module_H_


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
class Tone_signal_module : public Tfrs_element , public Tincrementer_donnor
{

    unsigned int unsigned_signal_data;
    int signed_signal_data;

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*signal_ptr;

    spectrum_1D * spec_first ;
    string signal_description;
public:
    //  constructor
    Tone_signal_module(string _name, int TIFJEvent::*first_ptr, string description = "signal") ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates
    int give_value()
    {
        return signed_signal_data;
    }

protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _one_signal_module_H_
