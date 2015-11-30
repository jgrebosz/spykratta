//
// C++ Interface: tstopper_stripe
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TEXOTIC_ONE_STRIPE_H
#define TEXOTIC_ONE_STRIPE_H
#include "experiment_def.h"
#if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
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

class Texotic_vector_strips;
////////////////////////////////////////////////////////////////////////
class Texotic_one_stripe : public Tfrs_element, public Tincrementer_donnor
{
    Texotic_vector_strips *owner_det;
    string name_of_this_element ;
    int id_module ; // parent parent module number 0,1,2
    int id_x_or_y; //  x or y
    int id_stripe ;//

    int *data_ptr;  // where is the data in the event
    bool flag_fired;
    int energy_raw;
    int energy_adjusted;

    vector<double> energy_cal_factors;
    double energy_calibrated;
    void create_my_spectra();

    spectrum_1D * spec_energy_raw;
    spectrum_1D * spec_energy_cal;

    int  jitter_correction;
    spectrum_1D * spec_energy_adjusted;   // corrected for jittering

    int pseudo_fan;

    int my_id_for_osciloscope_matrix; //

public:
    Texotic_one_stripe(Texotic_vector_strips *owner_ptr,
                       string name,
                       int id_module,
                       int id_xy,
                       int id_stripe_nr);

    ~Texotic_one_stripe();
    void analyse_current_event()
    {
        cout << "Do not call this function " << endl;
        void analyse_current_event_pass_1();
        void analyse_current_event_pass_2();
    }
    void analyse_current_event_pass_1();
    void analyse_current_event_pass_2();

    void make_preloop_action(ifstream & plik);

    void set_jitter_correction(int jit)
    {
        jitter_correction = jit ;
    }


    static
    // not all samples need to participate in the mean value. Here below, the user may decide which does.
    bool   sample_participates_in_mean[NR_OF_EXOTIC_SAMPLES];

    void mark_not_fired()
    {
        flag_fired = false;
        pseudo_fan = 0;
        energy_calibrated = 0 ;
    }

    double give_energy_cal()
    {
        return energy_calibrated;
    }

    int give_energy_raw_mean()
    {
        return energy_raw ;
    }

    bool give_fired()
    {
        return flag_fired;
    }

    void your_data_will_be_here(int * ptr)
    {
        data_ptr = ptr;
    }

    void increment_oscilloscope_matrix(int x, int y);


    int give_previous_energy_mean()
    {
        return previous_energy_mean ;
    }
protected:
    int previous_energy_mean;
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef NIGDY
namespace exotic_namespace
{
enum type_of_exotic_event
{
    undefined_event,
    event,
    decay_event
};
extern  type_of_exotic_event   exotic_event_type;
}
#endif

#undef PLEASE_COMPILE_THIS_FILE
// #endif // #ifdef PLEASE_COMPILE_THIS_FILE

#endif // CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT

#endif  // Texotic_ONE_STRIPE_H
