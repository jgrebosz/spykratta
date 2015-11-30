#ifndef _BONN_MAGNET_H_
#define _BONN_MAGNET_H_


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
class Tbonn_magnet : public Tfrs_element, public Tincrementer_donnor
{

    // friend class focus_point ; // will read this private data, but using
    // constant pointer

//double distance ;


    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*magnet_up;
    int TIFJEvent::*magnet_dn;

    int polarity_up ; // ADC delivering the Magnet polarity signal
    int polarity_dn ; // ADC delivering the Magnet polarity signal

    spectrum_1D * spec_magnet_up ;
    spectrum_1D * spec_magnet_dn ;

    bool flag_polarity_up_is_in_default_gate ;
    bool flag_polarity_dn_is_in_default_gate ;

    int gate_on_up_polarity[2];
    int gate_on_dn_polarity[2];

public:
    //  constructor
    Tbonn_magnet(string _name, int TIFJEvent::*magnet_up_ptr, int TIFJEvent::*magnet_dn_ptr);
    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates

// bool polarity_up() { return flag_polarity_up ;}

protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _BONN_MAGNET_H_
