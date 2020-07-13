#ifndef _PISOLO_SI_FINAL_H_
#define _PISOLO_SI_FINAL_H_

#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"
//////////////////////////////////////////////////////////////////////////////
class Tpisolo_si_final : public Tfrs_element, public Tincrementer_donnor
{
    int  energy_raw;
    // calibration factors
    double slope, offset ;
    double energy_cal;

    // did we make good calculations for this event
    bool  energy_ok;

    int TIFJEvent::*pisolo_si_energy;

    spectrum_1D * spec_e_raw ;
    spectrum_1D * spec_e_cal ;

public:
    //  constructor
    Tpisolo_si_final(string _name,        int TIFJEvent::*energy_ptr) ;

    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates

    int give_e_raw()
    {
        if(! calculations_already_done) analyse_current_event();
        return energy_raw ;
    }
    double give_e_cal()
    {
        if(! calculations_already_done) analyse_current_event();
        return energy_cal ;
    }

protected:
    void create_my_spectra();
};
/////////////////////////////////////////////////////////////////////////
#endif // _PISOLO_SI_FINAL_H_

