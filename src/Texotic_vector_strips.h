//
// C++ Interface: Texotic_vector_strips
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TEXOTIC_VECTOR_STRIPS_H
#define TEXOTIC_VECTOR_STRIPS_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"

#if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT

#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"
#include "Texotic_one_stripe.h"

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
class Texotic_strip_detector;
/////////////////////////////////////////////////////////////////////////
class Texotic_vector_strips : public Tfrs_element, public Tincrementer_donnor
{
    Texotic_strip_detector *owner_det;
    unsigned int module_id_nr; // which module it is
    //string symbol;
    bool this_is_x_coordinate;   // for X it is true, for Y it is false
    //   const
    unsigned int const_nr_of_strips;

    vector <Texotic_one_stripe* > stripe;

    vector <double>strip_energies_cal;

    double nr_1st_energy_cal;
    double nr_2nd_energy_cal;
    double nr_3rd_energy_cal;


    int multiplicity_above_thrsh;
    int multiplicity;

    int *array_of_data;

//         int twelve_test;

public:
    Texotic_vector_strips(Texotic_strip_detector *owner,
                          std::string name,
                          unsigned int nr,
                          bool   x_otherwise_y,
                          //         char symbol_,
                          const int const_nr_of_strips_);
    ~Texotic_vector_strips();

    double give_energy_cal(bool it_is_x, int  strip_nr)
    {
        return stripe[strip_nr]->give_energy_cal();
    }

    void analyse_current_event() ; //  pure virtual  ;
    void make_preloop_action(ifstream &) ;   // read the calibration
    // factors, gates

    void zero_flags_fired_vectors();
    void action();

    void row_not_fired() ;

    vector< spectrum_abstr*>*  give_spectra_pointer()
    {
        return frs_spectra_ptr  ;
    }

    double give_position()
    {
        return position;
    }

    double give_thrsh_position()
    {
        return position_thrsh;
    }

    bool is_position_ok()
    {
        return position_ok;
    }
    bool is_position_thrsh_ok()
    {
        return position_thrsh_ok;
    }

    //---------------------
    void set_flag_algorithm_for_position_calculation_is_mean(bool f)
    {
        flag_algorithm_for_position_calculation_is_mean = f;
    }
    //-------------
    void your_array_of_data_will_be_here(int *ptr)
    {
        array_of_data = ptr;
        for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
        {
//       cout << "reading calibration loop for exotic strip detector  segment"
//       << i << endl ;
//
            // int * ptr = array_of_data;
            stripe[i]->your_data_will_be_here(array_of_data + (i * NR_OF_EXOTIC_SAMPLES));
        }
    }


    void set_options(
        double energy_cal_threshold_,
        double energy_cal_threshold_upper_,
        bool flag_algorithm_for_position_calculation_is_mean_)
    {
        energy_cal_threshold = energy_cal_threshold_;
        energy_cal_threshold_upper = energy_cal_threshold_upper_;
        flag_algorithm_for_position_calculation_is_mean =
            flag_algorithm_for_position_calculation_is_mean_;
    }

    void increment_oscilloscope_matrix(int x, int y);

protected:

    double energy_cal_threshold;
    double energy_cal_threshold_upper;
    bool flag_algorithm_for_position_calculation_is_mean;

    bool is_energy_cal_in_gate(double en)
    {
        return
            (en >= energy_cal_threshold
             &&
             en <= energy_cal_threshold_upper);
    }

    void create_my_spectra();
    void calculate_position_and_RMS_for();

    spectrum_1D * spec_fan , * spec_fan_above_thresholds;
    spectrum_1D * spec_nr_1st_energy_cal;
    spectrum_1D * spec_nr_2nd_energy_cal;
    spectrum_1D * spec_nr_3rd_energy_cal;

    // spectra multiplicity with threshold
    spectrum_1D
    *spec_multiplicity_for_energy_cal_above_threshold;

    // spectra multiplicity without threshold
    spectrum_1D  *spec_multiplicity_for_energy_cal;

//         spectrum_2D  *spec_tst;  // for debugging purposes
    double position;
    double position_decay  ;

    // in case if mianownik is zero, it is impossible to
    // calculate the positions
    bool position_ok;
    bool position_thrsh_ok;

    // ----- thrsh
    double position_thrsh;

    spectrum_1D  *spec_position_thrsh ;

    //==========
    double  rms ;
    double rms_thrsh ;

    // sum energies
    double denominator;

    //double denominator_decay;

    int  previous_0_strip_value;
    int  jitter_correction ;
    static int algorithm_for_jittering_correction;
};

#endif // CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
#endif // __CINT__

#endif
