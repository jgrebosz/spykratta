#ifndef _MICRO_CHANNEL_PLATE_H_
#define _MICRO_CHANNEL_PLATE_H_

#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"
//////////////////////////////////////////////////////////////////////////////
class Tmicro_channel_plate : public Tfrs_element, public Tincrementer_donnor
{
    double distance ;   // if some 'focal plane' will ever be used
    double x_raw, y_raw;

    // calibration factors
    double x_slope, x_offset ;
    double y_slope, y_offset ;

    // did we make good calculations for this event
    bool  x_ok;
    bool  y_ok ;

    double  x;      // in milimeters
    double  y ;  // poisition in milimeters

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*x_event;
    int TIFJEvent::*y_event;

    spectrum_1D * spec_x_raw ;
    spectrum_1D * spec_x_cal ;
    spectrum_1D * spec_y_raw ;
    spectrum_1D * spec_y_cal ;

    spectrum_2D * spec_xy ;

public:
    //  constructor
    Tmicro_channel_plate(string _name,
                         int TIFJEvent::*x_ptr,
                         int TIFJEvent::*y_ptr
                        ) ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates

    // the scintillators may ask, to perform position calculation
    bool was_x_ok()
    {
        if(calculations_already_done == false) analyse_current_event();
        return  x_ok  ;
    }

    bool was_y_ok()
    {
        if(calculations_already_done == false) analyse_current_event();
        return  y_ok  ;
    }
    // ------------------------- now universal, for both situations-------
    double give_x()
    {
        if(calculations_already_done == false) analyse_current_event();
        return x ;
    }

    double give_y()
    {
        if(calculations_already_done == false) analyse_current_event();
        return  y ;
    }

//*----------------------
    double give_distance()
    {
        return distance ;
    }
    /** No descriptions */

protected:
    void create_my_spectra();
};
/////////////////////////////////////////////////////////////////////////
#endif // _MICRO_CHANNEL_PLATE_H_

