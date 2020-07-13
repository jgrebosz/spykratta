#ifndef _TIME_PROJECTION_CHAMBER_H_
#define _TIME_PROJECTION_CHAMBER_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>

//#include "Tmultiwire.h"

#if defined TPC41_PRESENT || defined TPC42_PRESENT

#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

//#define NR_OF_DRIFT_TIMES   4
//#define TPC_NR_OF_X_TIMES  2

//////////////////////////////////////////////////////////////////////////////
class Ttime_projection_chamber : public Tfrs_element, public Tincrementer_donnor
{
    double distance ;
    static int tpc_det_counter;

    int my_tpc_det_nr; // important to put into the calibrated root tree

    int drift_time_raw[TPC_NR_OF_DRIFT_TIMES];
    int left_time_raw[TPC_NR_OF_X_TIMES];
    int right_time_raw[TPC_NR_OF_X_TIMES];

    int drift_energy_raw[TPC_NR_OF_DRIFT_TIMES];
    int left_energy_raw[TPC_NR_OF_X_TIMES];
    int right_energy_raw[TPC_NR_OF_X_TIMES];

    double drift_energy_cal[TPC_NR_OF_DRIFT_TIMES] ;
    double deposit_energy;
    int deposit_energy_counter;
    // calibration factors


    double
    drift_energy_offset[TPC_NR_OF_DRIFT_TIMES],
                        drift_energy_slope[TPC_NR_OF_DRIFT_TIMES];

    double
    drift_time_offset[TPC_NR_OF_DRIFT_TIMES],
                      drift_time_slope[TPC_NR_OF_DRIFT_TIMES];

    double
    leftRight_time_offset[TPC_NR_OF_X_TIMES],
                          leftRight_time_slope[TPC_NR_OF_X_TIMES];


    // did we make good calculations for this event
    bool
    x_ok,
    y_ok ;

    double
    x,
    y ;  // poisition

    int   check_sum[TPC_NR_OF_DRIFT_TIMES] ;

    // extra wishes
    double x0_minus_x1 ;
    int nr_checksums_true;
    bool flag_fired_times;

    double check_sum_gate[TPC_NR_OF_DRIFT_TIMES][2] ;  // gates have to be copied here in preloop action

    int (TIFJEvent::*drift_time_array)[TPC_NR_OF_DRIFT_TIMES] ;
    int (TIFJEvent::*left_time_array)[TPC_NR_OF_X_TIMES] ;
    int (TIFJEvent::*right_time_array)[TPC_NR_OF_X_TIMES] ;

    int (TIFJEvent::*drift_energy_array)[TPC_NR_OF_DRIFT_TIMES] ;
    int (TIFJEvent::*left_energy_array)[TPC_NR_OF_X_TIMES] ;
    int (TIFJEvent::*right_energy_array)[TPC_NR_OF_X_TIMES] ;




    spectrum_1D * spec_drift_time_raw[TPC_NR_OF_DRIFT_TIMES]  ;
    spectrum_1D * spec_left_time_raw[TPC_NR_OF_X_TIMES] ;
    spectrum_1D * spec_right_time_raw[TPC_NR_OF_X_TIMES] ;

    // not important in the algorithm, just for testing purposes
    spectrum_1D * spec_drift_energy_raw[TPC_NR_OF_DRIFT_TIMES]  ;
    spectrum_1D * spec_left_energy_raw[TPC_NR_OF_X_TIMES] ;
    spectrum_1D * spec_right_energy_raw[TPC_NR_OF_X_TIMES] ;

    spectrum_1D * spec_check_sum[TPC_NR_OF_DRIFT_TIMES];

    spectrum_1D * spec_x ;
    spectrum_1D * spec_x0 ;
    spectrum_1D * spec_x1 ;
    spectrum_1D * spec_y ;

    spectrum_2D * spec_xy ;


public:
    //  constructor
    Ttime_projection_chamber(string _name,
                             int (TIFJEvent::*drift_time_array_ptr)[TPC_NR_OF_DRIFT_TIMES],
                             int (TIFJEvent::*left_time_array_ptr)[TPC_NR_OF_X_TIMES],
                             int (TIFJEvent::*right_time_array_ptr)[TPC_NR_OF_X_TIMES],
                             int (TIFJEvent::*drift_energy_array_ptr)[TPC_NR_OF_DRIFT_TIMES],
                             int (TIFJEvent::*left_energy_array_ptr)[TPC_NR_OF_X_TIMES],
                             int (TIFJEvent::*right_energy_array_ptr)[TPC_NR_OF_X_TIMES]

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
        return y ;
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
#endif // defined TPC41_PRESENT || defined TPC42_PRESENT

#endif // CINT

#endif // _TIME_PROJECTION_CHAMBER_H_
