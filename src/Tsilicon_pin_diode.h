#ifndef _silicon_pin_diode_H_
#define _silicon_pin_diode_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__



#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>

#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"


//////////////////////////////////////////////////////////////////////////////
class Tsilicon_pin_diode : public Tfrs_element , public Tincrementer_donnor
{

    int dE_raw;
    double dE_cal;
    double slope, offset ; // calibration

    bool fired;
    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*signal_ptr;


    spectrum_1D * spec_dE_raw ;
    spectrum_1D * spec_dE_cal ;

public:
    //  constructor
    Tsilicon_pin_diode(string _name, int TIFJEvent::*first_ptr) ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates
    double give_dE_value()
    {
        return dE_cal;
    }

protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _silicon_pin_diode_H_
