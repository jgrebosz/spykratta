#ifndef TEXOTIC_STRIP_DETECTOR_H
#define TEXOTIC_STRIP_DETECTOR_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"
//#ifdef MUN_ACTIVE_STOPPER_PRESENT

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

#include "Texotic_vector_strips.h"


#if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT

/**
System of

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
------------------------------------------------------------
This class is using the file
         Texotic_vector_strips
which later is using
    Texotic_one_stripe
-----------------------------------------------------------

Texotic_strip_detector----->  Texotic_vector_strips  ---->  Texotic_one_stripe
-----------------------------------------------------------
*/
// class Texotic_matrix_xy;

/////////////////////////////////////////////////////////////////////////
class Texotic_strip_detector : public Tfrs_element, public Tincrementer_donnor
{
protected:
    Texotic_vector_strips*   x_plate;
    Texotic_vector_strips*   y_plate;

    int number ;   // temporarily?
    int my_x_number;    // will come from lookup table
    int my_y_number;    // will come from lookup table


    int (TIFJEvent::*data)
    [NR_OF_EXOTIC_MODULES]
    [NR_OF_EXOTIC_STRIPES_X]
    [NR_OF_EXOTIC_SAMPLES];

    int (TIFJEvent::*data_plate_fired) [NR_OF_EXOTIC_MODULES];

    void create_my_spectra();

    // options------------------------------
    double        x_energy_cal_gate_dn;
    double        x_energy_cal_gate_up ;
    double        y_energy_cal_gate_dn;
    double        y_energy_cal_gate_up ;


    bool        flag_algorithm_for_position_calculation_is_mean;
    // otherwise MAXimum
    //  int flip ;
    //         int compression_factor_for_x_position; // the X resolution can be too big. We may treat 2 pixels as one
    // this is important only for x position because of the features of its electronics.

    // bool  flag_trigger_of;  // for incrementers
    bool flag_make_flash; // when every 5 seconds we want to make a flash

    // if we make a successful match between implantation and decay,
    // here we have retrieved values
    // int found_impl_energy; // <--- for testing purposes
    //bool flag_successful ; // when it was really put into pixels (conditions on Veto, zet vs aoq, etc.)

    // in case if some geometry is broken
    //         enum enum_for_how_to_match_position {  xy_both,  only_x,  only_y };
    //         enum_for_how_to_match_position  how_to_match_position ;

    //         bool matching_without_x()
    //         { return ( how_to_match_position ==  only_y ) ; }

    // the positions are always when threshold
    double  universal_x_position;
    bool universal_x_position_ok;

    double  universal_y_position;
    bool universal_y_position_ok;

    spectrum_2D * spec_oscilloscope_matrix_x;
    spectrum_1D * spec_oscilloscope_flash_x;  // every 15 seconds - new view

    spectrum_2D * spec_oscilloscope_matrix_y;
    spectrum_1D * spec_oscilloscope_flash_y;  // every 15 seconds - new view

    spectrum_2D * spec_position_xy_map;

    time_t last ;   // for making flash oscilloscope

public:
    Texotic_strip_detector(string name, int nr,
                           int (TIFJEvent::*data_ptr) [NR_OF_EXOTIC_MODULES]
                           [NR_OF_EXOTIC_STRIPES_X][NR_OF_EXOTIC_SAMPLES],

                           int (TIFJEvent::*data_fired_ptr) [NR_OF_EXOTIC_MODULES]
                          );

    ~Texotic_strip_detector();

    void analyse_current_event();
    void make_preloop_action(ifstream &);    // read the calibration factors, gates

    double  give_universal_x_position(bool *flag_valid)
    {
        *flag_valid = universal_x_position_ok;
        return universal_x_position;
    }
    void increment_oscilloscope_matrix(int x, int y, bool xORy);

protected:

    void action();
    void zero_flags_fired_vectors();

};
//#endif  //

#endif  // CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
#endif // __CINT__
#endif  // GUARDIAN --- TEXOTIC_STRIP_DETECTOR_H
