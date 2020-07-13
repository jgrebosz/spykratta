#ifndef _TOF_MEASUREMENT_H_
#define _TOF_MEASUREMENT_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__


#include "Tfrs_element.h"

#include "spectrum.h"
#include <string>

#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"
class Tscintillator;

//////////////////////////////////////////////////////////////////////////////
class Ttof_measurement : public Tfrs_element, public Tincrementer_donnor
{

    // did we make good calculations for this event
    //  bool
    //  foc_x_ok,
    //  foc_y_ok ;

    double
    tof_LL,    // time of flight LeftLeft
    tof_RR,

    tof_LL_cal,    // calibrated
    tof_RR_cal;

    bool  flag_tof_ok;

    // it seams that those two values below - mean the same
    double   tof_ps ; // in picoseconds


    double tof_L_slope, tof_L_offset ;
    double tof_R_slope, tof_R_offset ;


    // to calculate beta = v/c  from time of flight
    double beta_calibr_factor0, beta_calibr_factor1 ;

    double beta, gamma, bega ;  // be-ga  = is beta*gamma


    // source data in the unpacked event
    /*---
    int *left_left_data_ptr  ;
    int *right_right_data_ptr ;
    --------*/

    int TIFJEvent::*left_left;
    int TIFJEvent::*right_right;



    spectrum_1D * spec_tof_RR_raw ;
    spectrum_1D * spec_tof_LL_raw ;
    spectrum_1D * spec_tof_RR_cal ;
    spectrum_1D * spec_tof_LL_cal ;
    spectrum_1D * spec_tof_ps ;
    spectrum_1D * spec_beta ;


    Tscintillator *start_sci, *stop_sci; // for Zsolt
    bool zsolt_algorithm_active ;

public:
    //  constructor
    Ttof_measurement(string _name,
                     int TIFJEvent::*left_left_ptr,
                     int TIFJEvent::*right_right_ptr,
                     Tscintillator * start = 0,   // for Zsolt algorithm
                     Tscintillator * stop = 0);   // for Zsolt algorithm


    // calibration i calculation
    void analyse_current_event() ;

    // read the calibration factors, gates
    void  make_preloop_action(ifstream &);


    // some other devices would like to know the tracking of the ion
    bool is_tof_ok()
    {
        return flag_tof_ok ;
    }

    double give_beta()
    {
        if(calculations_already_done == false) analyse_current_event();
        return beta ;
    }

    double  give_beta_gamma()
    {
        if(calculations_already_done == false) analyse_current_event();
        return bega ;
    }

    double  give_gamma()
    {
        if(calculations_already_done == false) analyse_current_event();
        return gamma ;
    }


    double  give_tof_ps()
    {
        if(calculations_already_done == false) analyse_current_event();
        return tof_ps ;
    }

private:
    void create_my_spectra();
    //void check_legality_of_data_pointers();


};

/////////////////////////////////////////////////////////////////////////
#endif // __CINT__
#endif // _TOF_MEASUREMENT_H_
