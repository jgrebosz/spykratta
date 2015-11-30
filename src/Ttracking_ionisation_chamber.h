//
// C++ Interface: tactive stopper_combined X and Y
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TRACKING_IONISATION_CHAMBER_H
#define TRACKING_IONISATION_CHAMBER_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"


#ifdef TRACKING_IONISATION_CHAMBER_X


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
System of

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

/////////////////////////////////////////////////////////////////////////
class Ttracking_ionisation_chamber : public Tfrs_element, public Tincrementer_donnor
{
    double distance;

    Tactive_stopper_vector_strips*   x_plate;
    Tactive_stopper_vector_strips*   y_plate;

    int (TIFJEvent::*xy_matrix_data)
    [TRACKING_IONISATION_CHAMBER_X + TRACKING_IONISATION_CHAMBER_Y];
    int (TIFJEvent::*xy_matrix_plate_fired);

    spectrum_1D * spec_decay_x_calibrated;
    spectrum_1D * spec_decay_y_calibrated;
    spectrum_1D * spec_implantation_x_calibrated;
    spectrum_1D * spec_implantation_y_calibrated;

    spectrum_2D * spec_map_for_implantations_thrsh;
    spectrum_2D * spec_map_for_decays_thrsh;

    void create_my_spectra();

    // options------------------------------
    double        energy_cal_implantation_gate_dn;
    double        energy_cal_implantation_gate_up ;

    double        energy_cal_decay_gate_dn ;
    double        energy_cal_decay_gate_up ;
    bool        flag_algorithm_for_implantation_position_calculation_is_mean;
    // otherwise MAXimum

    bool flag_decay_situation_is_needed;  // if we want to work with decays as well

public:


    Ttracking_ionisation_chamber(string name, bool flag_decay_situation_is_needed_,
                                 int (TIFJEvent::*data_ptr)
                                 [TRACKING_IONISATION_CHAMBER_X + TRACKING_IONISATION_CHAMBER_Y],
                                 int (TIFJEvent::*data_fired_ptr)
                                );

    ~Ttracking_ionisation_chamber();

    void analyse_current_event();
    void make_preloop_action(ifstream &); // read the calibration factors, gates

    void implantation_action();
    bool decay_action();

#define SPR if(!calculations_already_done)analyse_current_event();
    // by default we will ask for this in case of implantation
    bool was_y_ok()
    {
        SPR;
        return
            active_stopper_namespace::active_stopper_event_type == active_stopper_namespace::implantation_event ?
            impl_position_y_calibrated_validator
            :
            decay_position_y_calibrated_validator;
    }

    bool was_x_ok()
    {
        SPR;
        return
            active_stopper_namespace::active_stopper_event_type == active_stopper_namespace::implantation_event ?
            impl_position_x_calibrated_validator
            :
            decay_position_x_calibrated_validator;
    }

    double give_x()
    {
        SPR;
        return
            active_stopper_namespace::active_stopper_event_type == active_stopper_namespace::implantation_event ?
            impl_position_x_calibrated
            :
            decay_position_x_calibrated;

    }
    double give_y()
    {
        SPR
        return
            active_stopper_namespace::active_stopper_event_type == active_stopper_namespace::implantation_event ?
            impl_position_y_calibrated
            :
            decay_position_y_calibrated;
    }

    double give_distance()
    {
        return distance ;
    }

    // they are the same in case of implantation and decay
    vector<double> position_y_calibr_factors;
    vector<double> position_x_calibr_factors;

    // more important set - for implantation case
    double  impl_position_x_calibrated;
    double  impl_position_y_calibrated;
    bool impl_position_x_calibrated_validator;
    bool impl_position_y_calibrated_validator;

    //---------------------
    // less important set - for decay case
    double  decay_position_x_calibrated;
    double  decay_position_y_calibrated;
    bool decay_position_x_calibrated_validator;
    bool decay_position_y_calibrated_validator;

};
#endif // TRACKING_IONISATION_CHAMBER_X
#endif // __CINT__
#endif  // GUARDIAN
