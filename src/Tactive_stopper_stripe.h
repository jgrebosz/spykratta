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
#ifndef TACTIVE_STOPPER_STRIPES_H
#define TACTIVE_STOPPER_STRIPES_H

#include "experiment_def.h"

#ifdef ACTIVE_STOPPER_PRESENT

#include <Tincrementer_donnor.h>
#include <vector>
#include "spectrum.h"



#define Tactive_stopper_combined_xy_universal   Tactive_stopper_combined_xy

class Tactive_stopper_combined_xy_universal;
/**
Just one stripe of the active stopper

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/
////////////////////////////////////////////////////////////////////////
class Tactive_stopper_stripe : public Tincrementer_donnor
{
    Tactive_stopper_combined_xy_universal * owner; //!
    string name_of_this_element ;
    int id_module ; // parent parent module number 0,1,2
    int id_x_or_y; //  x or y
    int id_stripe ;//

    bool flag_fired;

    int energy_raw;

    vector<double> energy_implantation_cal_factors;
    vector<double> energy_decay_cal_factors;

    double energy_implantation_calibrated;
    double energy_decay_calibrated;
    //double time_calibrated;

    void create_my_spectra();

    spectrum_1D * spec_energy_raw;
    spectrum_1D * spec_energy_implantation_cal;
    spectrum_1D * spec_energy_decay_cal;

    int pseudo_fan;

public:
    Tactive_stopper_stripe(Tactive_stopper_combined_xy_universal *ptr_owner,
                           string name, int id_module, int id_xy,
                           int id_stripe_nr);

    ~Tactive_stopper_stripe();
    void analyse_current_event();
    void make_preloop_action(ifstream & plik);
    void mark_not_fired()
    {
        flag_fired = false;
        pseudo_fan = 0;
        energy_implantation_calibrated = 0 ;
        energy_decay_calibrated = 0;
#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
        time_calibrated = 0;
#endif

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


#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
private:
    int time_raw;
    vector<double> time_cal_factors;
    double time_calibrated;
    spectrum_1D * spec_time_raw;
    spectrum_1D * spec_time_cal;
#endif


};
#endif  // ACTIVE_STOPPER_PRESENT

#endif  // TACTIVE_STOPPER_STRIPES_H
