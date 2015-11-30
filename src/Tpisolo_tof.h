#ifndef _PISOLO_tof_H_
#define _PISOLO_tof_H_

#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"
//////////////////////////////////////////////////////////////////////////////
class Tpisolo_tof : public Tfrs_element, public Tincrementer_donnor
{

    int  tof1_raw, tof2_raw, tof3_raw;      // delta E, delta E1, delta E2
    double  tof1_cal, tof2_cal, tof3_cal;      // delta E, delta E1, delta E2

    // calibration factors
    double slope1, offset1 ;
    double slope2, offset2 ;
    double slope3, offset3 ;

    // did we make good calculations for this event
    bool  tof1_ok;
    bool  tof2_ok;
    bool  tof3_ok;

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*tof1_event;
    int TIFJEvent::*tof2_event;
    int TIFJEvent::*tof3_event;

    spectrum_1D * spec_tof1_raw ;
    spectrum_1D * spec_tof2_raw ;
    spectrum_1D * spec_tof3_raw ;

    spectrum_1D * spec_tof1_cal ;
    spectrum_1D * spec_tof2_cal ;
    spectrum_1D * spec_tof3_cal ;

public:
    //  constructor
    Tpisolo_tof(string _name,  int TIFJEvent::*tof1, int TIFJEvent::*tof2, int TIFJEvent::*tof3) ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates


    bool was_tof_ok()
    {
        if(calculations_already_done == false) analyse_current_event();
        return  tof1_ok  ;
    }

    int give_tof1_raw()
    {
        if(calculations_already_done == false) analyse_current_event();
        if(! tof1_ok) return -1;
        return  tof1_raw  ;
    }


    double give_tof1_cal()
    {
        if(calculations_already_done == false) analyse_current_event();
        if(! tof1_ok) return -1;
        return  tof1_cal  ;
    }

    //*----------------------

protected:
    void create_my_spectra();
};
/////////////////////////////////////////////////////////////////////////
#endif // _MICRO_CHANNEL_PLATE_H_

