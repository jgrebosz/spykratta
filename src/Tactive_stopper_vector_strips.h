//
// C++ Interface: Tactive_stopper_vector_strips
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TACTIVE_STOPPER_VECTOR_STRIPS_H
#define TACTIVE_STOPPER_VECTOR_STRIPS_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"
#include "Tactive_stopper_one_stripe.h"
#include "spectrum.h"
#include "TIFJEvent.h"
#include "Tnamed_pointer.h"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
/**
System of many stripes creating one active stopper

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/
/////////////////////////////////////////////////////////////////////////
class Tactive_stopper_vector_strips : public Tfrs_element, public Tincrementer_donnor
{
    unsigned int module_id_nr; // which module it is
    //string symbol;

    //   const
    unsigned int const_nr_of_strips;

    vector <Tactive_stopper_one_stripe* > stripe;

    vector <double>strip_energies_implantation_cal;
    vector <double>strip_energies_decay_cal;

    double nr_1st_energy_implantation_cal;
    double nr_2nd_energy_implantation_cal;
    double nr_3rd_energy_implantation_cal;

    double nr_1st_energy_decay_cal;
    double nr_2nd_energy_decay_cal;
    double nr_3rd_energy_decay_cal;

    int impl_multiplicity_above_thrsh;
    int impl_multiplicity;

    int decay_multiplicity;
    int decay_multiplicity_above_thrsh;

    int *array_of_data;


public:
    Tactive_stopper_vector_strips(
        std::string name,
        unsigned int nr,
        //         char symbol_,
        const int const_nr_of_strips_   , bool flag_we_want_also_decay_);
    ~Tactive_stopper_vector_strips();

    double give_implantation_energy_cal(bool it_is_x, int  strip_nr)
    {
        return stripe[strip_nr]->give_energy_implantation_cal();
    }

    void analyse_current_event() ; //  pure virtual  ;
    void make_preloop_action(ifstream &) ;// read the calibration
    // factors, gates

    void zero_flags_fired_vectors();
    void implantation_action();
    bool decay_action();

    void row_not_fired() ;

    vector< spectrum_abstr*>*  give_spectra_pointer()
    {
        return frs_spectra_ptr  ;
    }

    double give_implantation_position()
    {
        return position_implantation;
    }
    double give_decay_position()
    {
        return position_decay;
    }

    double give_implantation_thrsh_position()
    {
        return position_implantation_thrsh;
    }
    double give_decay_thrsh_position()
    {
        return position_decay_thrsh;
    }

    bool is_position_implantation_ok()
    {
        return position_implantation_ok;
    }
    bool is_position_implantation_thrsh_ok()
    {
        return position_implantation_thrsh_ok;
    }

    bool is_position_decay_ok()
    {
        return position_decay_ok;
    }
    bool is_position_decay_thrsh_ok()
    {
        return position_decay_thrsh_ok;
    }


    //---------------------
    void set_flag_algorithm_for_position_calculation_is_mean(bool f)
    {
        flag_algorithm_for_implantation_position_calculation_is_mean = f;
    }
    //-------------
    void your_array_of_data_will_be_here(int *ptr)
    {
        array_of_data = ptr;
        for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
        {
            //cout << "reading calibration loop for active stopper segment"
            //<< i << endl ;
            // int * ptr = array_of_data;
            stripe[i]->your_data_will_be_here(array_of_data + i);
        }
    }


    void set_options(
        double energy_cal_implantation_threshold_,
        double energy_cal_implantation_threshold_upper_,
        double energy_cal_decay_threshold_,
        double energy_cal_decay_threshold_upper_,
        bool flag_algorithm_for_implantation_position_calculation_is_mean_)
    {
        energy_cal_implantation_threshold = energy_cal_implantation_threshold_;
        energy_cal_implantation_threshold_upper = energy_cal_implantation_threshold_upper_;
        energy_cal_decay_threshold = energy_cal_decay_threshold_;
        energy_cal_decay_threshold_upper = energy_cal_decay_threshold_upper_;
        flag_algorithm_for_implantation_position_calculation_is_mean =
            flag_algorithm_for_implantation_position_calculation_is_mean_;
    }

    double give_sum_energies_above_threshold_when_decay()
    {
        return sum_energies_above_threshold_when_decay;
    }
protected:

    double energy_cal_implantation_threshold;
    double energy_cal_implantation_threshold_upper;
    double energy_cal_decay_threshold;
    double energy_cal_decay_threshold_upper;
    bool flag_algorithm_for_implantation_position_calculation_is_mean;
// when the dectetor works at S2 - the decay situation may not be needed, so we do not need the calibrations
    bool flag_we_want_also_decay;

    double sum_energies_above_threshold_when_decay;



    bool is_energy_cal_implantation_in_gate(double en)
    {
        return
            (en >= energy_cal_implantation_threshold
             &&
             en <= energy_cal_implantation_threshold_upper);
    }

    bool is_energy_cal_decay_in_gate(double en)
    {
        return
            (en >= energy_cal_decay_threshold
             &&
             en <= energy_cal_decay_threshold_upper);
    }



    //void increment_your_spectra()    {}

    //void save_spectra()    {}

    void create_my_spectra();
    void calculate_position_and_RMS_for_implantation();
    void calculate_position_and_RMS_for_decay();

    //   void successful_match_impl_with_decay(int x, int y);

    spectrum_1D * spec_fan , * spec_fan_above_thresholds;
    spectrum_1D * spec_nr_1st_energy_implantation_cal;
    spectrum_1D * spec_nr_2nd_energy_implantation_cal;
    spectrum_1D * spec_nr_3rd_energy_implantation_cal;

    spectrum_1D * spec_nr_1st_energy_decay_cal;
    spectrum_1D * spec_nr_2nd_energy_decay_cal;
    spectrum_1D * spec_nr_3rd_energy_decay_cal;

    // spectra multiplicity with threshold
    spectrum_1D
    *spec_multiplicity_for_energy_cal_implantation_above_threshold;

    spectrum_1D
    *spec_multiplicity_for_energy_cal_decay_above_threshold;

    // spectra multiplicity without threshold
    spectrum_1D  *spec_multiplicity_for_energy_cal;


    double position_implantation;
    double position_decay  ;

//   spectrum_1D  *spec_position_implantation ;
//   spectrum_1D  *spec_position_decay ;

    // in case if mianownik is zero, it is impossible to
    // calculate the positions
    bool position_implantation_ok;
    bool position_implantation_thrsh_ok;

    bool position_decay_ok;
    bool position_decay_thrsh_ok;

    // ----- thrsh
    double position_implantation_thrsh;
    double position_decay_thrsh  ;

    spectrum_1D  *spec_position_implantation_thrsh ;
    spectrum_1D  *spec_position_decay_thrsh ;

    //==========
    double  rms_implantation ;
    double  rms_decay ;
    double rms_implantation_thrsh ;
    double rms_decay_thrsh;

    // sum energies
    double denominator_implantation;

    double denominator_decay;
};

#endif // __CINT__

#endif
