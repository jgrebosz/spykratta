#ifndef _SCINTILLATOR_H_
#define _SCINTILLATOR_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__



#include "Tfrs_element.h"
#include "spectrum.h"
#include "Tfocal_plane.h"

#include <string>

#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

//////////////////////////////////////////////////////////////////////////////
class Tscintillator : public Tfrs_element, public Tincrementer_donnor
{

    double distance ;
    // did we make good calculations for this event
    bool   position_ok ;
    bool   e_ok;

    double e;          // calculted energy
    double position  ; // calculated position


    double cal_factor_offset,
           cal_factor_slope,
           cal_factor_quadratic,
           cal_factor_third_degree,
           cal_factor_4th_degree;

    double sci_dt_gate_low ;
    double sci_dt_gate_high;


    Tfocal_plane *related_focus_point ;
    bool horizontal_orientation ;


    int TIFJEvent::*left_down_ptr ;
    int TIFJEvent::*right_up_ptr ;
    int TIFJEvent::*dt_ptr;

    int left_down ;
    int right_up ;
    int dt ;

    bool left_down_fired, right_up_fired ;

    double x_or_y_extrapolated_here;
    bool x_or_y_extrapolation_ok;


    spectrum_1D * spec_e ;
    spectrum_1D * spec_dt ;
    spectrum_2D * matr_mw_sci ;  // matrix to fit the calibration line
    spectrum_1D * spec_position ;
    spectrum_1D * spec_position_extr;
    spectrum_1D * spec_left_down ;
    spectrum_1D * spec_right_up ;


public:
    //  constructor
    Tscintillator(string _name,
                  string name_of_focus_point,
                  bool horizontal,   // horizontal or vertical orientation
                  int TIFJEvent::*left_ptr,
                  int TIFJEvent::*right_ptr,
                  int TIFJEvent::*dt_ptr);


    //--------------------------------------------------
    // this is made in the moment of the preloop


    // calibration i calculation
    void analyse_current_event() ;
    void  make_preloop_action(ifstream &);  // read the calibration factors, gates

    double give_position()
    {
        if(!calculations_already_done) analyse_current_event();
        return position_ok ? position : 99999 ;
    }

    int give_left_raw()
    {
        return left_down;
    }
    int give_right_raw()
    {
        return right_up;
    }


    double give_distance()
    {
        return distance ;
    }

    double give_position_extrapolated_by_focal_plane(bool *valid)
    {
        if(!calculations_already_done) analyse_current_event();
        *valid = x_or_y_extrapolation_ok ;
        return x_or_y_extrapolated_here ;
    }


    double give_rel_focus_x()    // to be deleted !!!
    {
        if(calculations_already_done == false) analyse_current_event();
        return related_focus_point->give_x() ;
    }

    string give_rel_focus_name()
    {
        return related_focus_point->give_name() ;
    }

    /** No descriptions */
    bool is_position_ok()
    {
        if(calculations_already_done == false) analyse_current_event();
        return position_ok;
    }


    /** To incremet spectra gated by the identified isotope */
    //  void isotope_was_identified();
    /** No descriptions */
    double give_e();


protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // __CINT__

#endif // _SCINTILLATOR_H_
