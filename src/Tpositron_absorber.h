//
// C++ Interface: Tpositron_absorber
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TPOSITRON_ABSORBER_H
#define TPOSITRON_ABSORBER_H
// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"

#ifdef POSITRON_ABSORBER_PRESENT
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"

#include "Tactive_stopper_vector_strips.h"
#include "TIFJEvent.h"
#include <vector>
#include "spectrum.h"

//*************************************************************************************************
/**
Seven strip in X for detecting the positrons

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/
class Tpositron_absorber : public Tfrs_element, public Tincrementer_donnor
{
    vector<Tactive_stopper_vector_strips*>   plate;

    int (TIFJEvent::*positron_absorber) [POSITRON_ABSORBER_NR][POSITRON_ABSORBER_STRIPS_NR + 1];
    int (TIFJEvent::*positron_absorber_fired) [POSITRON_ABSORBER_NR];


    int back_energy_raw[POSITRON_ABSORBER_NR];  // for having an incrementer

    //   spectrum_2D * spec_implant_trajectory;
    spectrum_2D * spec_implant_trajectory_thrsh;
    //   spectrum_2D * spec_decay_trajectory;
    spectrum_2D * spec_decay_trajectory_thrsh;

    spectrum_1D * spec_back_side_energy_raw[POSITRON_ABSORBER_NR];
    void create_my_spectra();

    // options------------------------------
    double        energy_cal_implantation_gate_dn;
    double        energy_cal_implantation_gate_up ;

    double        energy_cal_decay_gate_dn ;
    double        energy_cal_decay_gate_up ;
    bool        flag_algorithm_for_implantation_position_calculation_is_mean;
    // otherwise MAX

    int beamline_position_of_this_device;
    bool flag_it_is_front;

    // for making panoramic trajectory drawing of all munich devices we need
    // incrementers
    int position_in_plate_in_mm[POSITRON_ABSORBER_NR];
    bool position_in_plate_valid[POSITRON_ABSORBER_NR];

public:
    static double decay_sum_energy_all_absorbers;
    double decay_sum_energy_this_absorber;


    // + 1 below is to transmit the back side of the absorber
    Tpositron_absorber(string nam,
                       int (TIFJEvent::*positron_absorber_front_ptr)
                       [POSITRON_ABSORBER_NR]
                       [POSITRON_ABSORBER_STRIPS_NR + 1],

                       int (TIFJEvent::*positron_absorber_front_fired_ptr)
                       [POSITRON_ABSORBER_NR],
                       int start_nr_for_id_position, // To make the global trajectory matrix we must know where
                       // on the beamline stays this device (related to the active stopper)
                       bool it_is_front  // front absorber is conted from 10 to 0, the rear  0-10
                      );

    ~Tpositron_absorber();

    void analyse_current_event();
    void make_preloop_action(ifstream &); // read the calibration factors, gates
    void mark_not_fired()
    {
        memset(position_in_plate_in_mm, sizeof(position_in_plate_in_mm), 0);
        memset(position_in_plate_valid, sizeof(position_in_plate_valid), 0);
    }

    int give_x_pos_plate(int i, bool *flag_valid);
    double  give_decay_sum_energy()
    {
        return decay_sum_energy_this_absorber;
    }


};

#endif   // POSITRON_ABSORBER_PRESENT
#endif // __CINT__
#endif
