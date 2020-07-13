#ifndef _MULTIWIRE_H_
#define _MULTIWIRE_H_


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
class Tmultiwire : public Tfrs_element, public Tincrementer_donnor
{

    // friend class focus_point ; // will read this private data, but using
    // constant pointer
    double distance ;
    double left_raw, right_raw, up_raw, down_raw, anode_raw ;

    // for rates spectra
    bool left_raw_fired, right_raw_fired, up_raw_fired,
         down_raw_fired, anode_raw_fired ;

    // calibration factors
    double x_slope, x_offset ;
    double y_slope, y_offset ;

    // did we make good calculations for this event
    bool
    x_ok,
    y_ok ;

    double
    x,
    y ;  // poisition


    double
    x_sum ,
    y_sum ;

    double x_sum_gate[2] ;  // gates have to be copied here in preloop action
    double y_sum_gate[2] ;

    bool use_tdc_algorithm ;

    double
    // electrodes
    anode_factors_offset,
    anode_factors_slope,


    cathode_x_right_factors_offset,
    cathode_x_right_factors_slope,

    cathode_x_left_factors_offset,
    cathode_x_left_factors_slope,


    cathode_y_up_factors_offset,
    cathode_y_up_factors_slope,

    cathode_y_down_factors_offset,
    cathode_y_down_factors_slope,

    // pulser delays
    pulser_delay_left_anode,
    pulser_delay_right_anode,

    pulser_delay_down_anode,
    pulser_delay_up_anode,

    pulser_delay_left_right,
    pulser_delay_down_up,

    // nanoseconds to milimeters
    x_factor_ns_to_mm,
    x_offset_ns_to_mm,

    y_factor_ns_to_mm,
    y_offset_ns_to_mm;

    double
    x_sum_ns, y_sum_ns,


              anode_cal,
              left_cal,
              right_cal,
              up_cal, down_cal,
              left_cal_right_cal_diff_ns ,
              down_cal_up_cal_diff_ns,
              x_mm, y_mm ;



    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*x_left;
    int TIFJEvent::*x_right;
    int TIFJEvent::*y_up;
    int TIFJEvent::*y_down;
    int TIFJEvent::*anode;


    spectrum_1D * spec_left ;
    spectrum_1D * spec_right ;
    spectrum_1D * spec_up;
    spectrum_1D * spec_down ;
    spectrum_1D * spec_anode ;

    spectrum_1D * spec_x ;
    spectrum_1D * spec_x_sum ;
    spectrum_1D * spec_y ;
    spectrum_1D * spec_y_sum ;

    spectrum_2D * spec_xy ;


public:
    //  constructor
    Tmultiwire(string _name,
               int TIFJEvent::*x_left_ptr,
               int TIFJEvent::*x_right_ptr,
               int TIFJEvent::*y_up_ptr,
               int TIFJEvent::*y_down_ptr,
               int TIFJEvent::*anode_ptr
              ) ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read thye calibration factors, gates

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
        return use_tdc_algorithm ? x_mm : x ;
    }

    double give_y()
    {
        if(calculations_already_done == false) analyse_current_event();
        return use_tdc_algorithm ? y_mm : y ;
    }

//*----------------------
#ifdef NIGDY
    double give_x_mm()
    {
        if(calculations_already_done == false) analyse_current_event();
        return x_mm ;
    }

    double give_y_mm()
    {
        if(calculations_already_done == false) analyse_current_event();
        return y_mm ;
    }
#endif
    double give_distance()
    {
        return distance ;
    }
    /** No descriptions */

protected:
    void create_my_spectra();
    //void check_legality_of_data_pointers();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _MULTIWIRE_H_
