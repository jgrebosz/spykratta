#ifndef _two_signal_module_H_
#define _two_signal_module_H_


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
class Ttwo_signal_module : public Tfrs_element , public Tincrementer_donnor
{

    // friend class focus_point ; // will read this private data, but using
    // constant pointer

    int first_signal_data;
    int second_signal_data;

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*first_signal_ptr;
    int TIFJEvent::*second_signal_ptr;

    spectrum_1D * spec_first ;
    spectrum_1D * spec_second ;

    string first_txt, second_txt;

public:
    //  constructor
    Ttwo_signal_module(string _name,
                       int TIFJEvent::*first_ptr,
                       int TIFJEvent::*second_ptr,
                       string name_first = "first",
                       string name_second = "second"
                      ) ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates

protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _two_signal_module_H_
