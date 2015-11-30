//
// C++ Interface: tstopper_stripe
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TACTIVE_STOPPER_ONE_STRIPE_H
#define TACTIVE_STOPPER_ONE_STRIPE_H

#include "experiment_def.h"

// #ifdef ACTIVE_STOPPER_PRESENT
// #define PLEASE_COMPILE_THIS_FILE
// #endif
//
//
//
// #ifdef PLEASE_COMPILE_THIS_FILE

#include "Tfrs_element.h"
class Tfrs_element;

#include <Tincrementer_donnor.h>
#include <vector>
#include "spectrum.h"
#include "TIFJEvent.h"

/**
Just one stripe of the active stopper

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/
////////////////////////////////////////////////////////////////////////
class Tactive_stopper_one_stripe : public Tfrs_element, public Tincrementer_donnor
{
    string name_of_this_element ;
    int id_module ; // parent parent module number 0,1,2
    int id_x_or_y; //  x or y
    int id_stripe ;//

    int *data_ptr;  // where is the data in the event
    bool flag_fired;
    int energy_raw;

    vector<double> energy_implantation_cal_factors;
    vector<double> energy_decay_cal_factors;

    double energy_implantation_calibrated;
    double energy_decay_calibrated;
    //double time_calibrated;

    bool flag_also_decay;  // someone may not need the decay situation

    void create_my_spectra();

    spectrum_1D * spec_energy_raw;
    spectrum_1D * spec_energy_implantation_cal;
    spectrum_1D * spec_energy_decay_cal;

    int pseudo_fan;

public:
    Tactive_stopper_one_stripe(
        string name, int id_module, int id_xy,
        int id_stripe_nr, bool flag_also_decay_);

    ~Tactive_stopper_one_stripe();
    void analyse_current_event();
    void make_preloop_action(ifstream & plik);
    void mark_not_fired()
    {
        flag_fired = false;
        pseudo_fan = 0;
        energy_implantation_calibrated = 0 ;
        energy_decay_calibrated = 0;
    }

    double give_energy_implantation_cal()
    {
        return energy_implantation_calibrated;
    }
    double give_energy_decay_cal()
    {
        return energy_decay_calibrated;
    }
    bool give_fired()
    {
        return flag_fired;
    }

    void your_data_will_be_here(int * ptr)
    {
        data_ptr = ptr;
    }

};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace active_stopper_namespace
{
enum type_of_active_stopper_event
{
    undefined_event,
    implantation_event,
    decay_event
};
extern  type_of_active_stopper_event   active_stopper_event_type;
}

#undef PLEASE_COMPILE_THIS_FILE
// #endif // #ifdef PLEASE_COMPILE_THIS_FILE

#endif  // TACTIVE_STOPPER_ONE_STRIPE_H
