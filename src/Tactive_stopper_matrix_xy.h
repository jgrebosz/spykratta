//
// C++ Interface: Tactive stopper_combined X and Y
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef MUNICH_MATRIX_H
#define MUNICH_MATRIX_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"
#ifdef MUNICH_MATRIX_STRIPES_X


#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"

#include "spectrum.h"
#include "TIFJEvent.h"
#include "Tnamed_pointer.h"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>

#include "Tactive_stopper_vector_strips.h"
/**
System of many tracking xy - readout is similar to Multiwire chamber

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

/////////////////////////////////////////////////////////////////////////
class Tactive_stopper_matrix_xy : public Tfrs_element, public Tincrementer_donnor
{
    double distance ;
    double left_raw, right_raw, up_raw, down_raw;

    // calibration factors
    double left_slope, left_offset ;
    double right_slope, right_offset ;
    double up_slope, up_offset ;
    double down_slope, down_offset ;

    // did we make good calculations for this event
    bool     x_ok,  y_ok ;
    double  x,  y ;  // poisition

    double x_sum ,  y_sum ;

    double x_sum_gate[2] ;  // gates have to be copied here in preloop action
    double y_sum_gate[2] ;

    double left_cal, right_cal, up_cal, down_cal;


    vector<double>  x_final_cal;
    vector<double>  y_final_cal;

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int TIFJEvent::*x_left;
    int TIFJEvent::*x_right;
    int TIFJEvent::*y_up;
    int TIFJEvent::*y_down;


    spectrum_1D * spec_left ;
    spectrum_1D * spec_right ;
    spectrum_1D * spec_up;
    spectrum_1D * spec_down ;

    spectrum_1D * spec_left_cal ;
    spectrum_1D * spec_right_cal ;
    spectrum_1D * spec_up_cal;
    spectrum_1D * spec_down_cal ;

    spectrum_1D * spec_x ;
    spectrum_1D * spec_x_sum ;
    spectrum_1D * spec_y ;
    spectrum_1D * spec_y_sum ;

    spectrum_2D * spec_xy ;

public:
    //  constructor
    Tactive_stopper_matrix_xy(string _name,
                              int TIFJEvent::*x_left_ptr,
                              int TIFJEvent::*x_right_ptr,
                              int TIFJEvent::*y_up_ptr,
                              int TIFJEvent::*y_down_ptr
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
        return  x ;
    }

    double give_y()
    {
        if(calculations_already_done == false) analyse_current_event();
        return y ;
    }

    double give_distance()
    {
        return distance ;
    }
    /** No descriptions */

protected:
    void create_my_spectra();
};

#endif // __CINT__

#endif   // def MUNICH_MATRIX

#endif
