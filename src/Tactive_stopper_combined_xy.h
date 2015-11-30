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
#ifndef TACTIVE_STOPPER_COMBINED_XY_MUNICH_H
#define TACTIVE_STOPPER_COMBINED_XY_MUNICH_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"
#ifdef ACTIVE_STOPPER_PRESENT


#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"
#include "Tactive_stopper_stripe.h"
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
class Tactive_stopper_full; // owner

/////////////////////////////////////////////////////////////////////////
class Tactive_stopper_combined_xy : public Tfrs_element, public Tincrementer_donnor
{
    unsigned int module_id_nr; // which module it is
    string symbol;

    // below is a static  variable which remembers in which modules was possible to make the succesfull implantation
    // we will store this information in the pixel, after finishing analysing all modules of active stopper for
    // current implantation event
    static unsigned int pattern_of_impantation_modules;

    // after retrieving the pattern from the decayed pixel, we know which
    // other modules (stopper_combined_xy) were also implanted during the matched implantation
    static bool pattern_says_that_also_was_implanted_module[32];  // usually 3 per "full"


    friend class Tactive_stopper_stripe;
    vector <Tactive_stopper_stripe*> y_stripe;
    vector <Tactive_stopper_stripe*> x_stripe;

    vector <double>strip_x_energies_implantation_cal;
    vector <double>strip_y_energies_implantation_cal;

    vector <double>strip_x_energies_decay_cal;
    vector <double>strip_y_energies_decay_cal;

    double x_1st_energy_implantation_cal;
    double x_2nd_energy_implantation_cal;
    double x_3rd_energy_implantation_cal;

    double y_1st_energy_implantation_cal;
    double y_2nd_energy_implantation_cal;
    double y_3rd_energy_implantation_cal;

    double x_1st_energy_decay_cal;
    double x_2nd_energy_decay_cal;
    double x_3rd_energy_decay_cal;

    double y_1st_energy_decay_cal;
    double y_2nd_energy_decay_cal;
    double y_3rd_energy_decay_cal;


    int x_multiplicity_for_energy_cal_implantation_above_threshold;
    int y_multiplicity_for_energy_cal_implantation_above_threshold;

    int x_multiplicity_for_energy_cal_decay_above_threshold;
    int y_multiplicity_for_energy_cal_decay_above_threshold;


    int  x_multiplicity_for_energy_cal;
    int  y_multiplicity_for_energy_cal;

    Tactive_stopper_full* const owner;

    // if we make a successful match between implantation and decay,
    // here we have retrieved values
    int found_impl_energy; // <--- for testing purposes

    unsigned long long found_impl_timestamp;

    double found_impl_zet;
    double found_impl_zet2;
    double found_impl_zet_sci;
    double found_impl_zet2_sci;
    // before magnets - was sci
    double found_impl_aoq_sci_mw_corrected;
    double found_impl_aoq_sci_sci_corrected;
    double found_impl_aoq_sci_tpc_corrected;

    // before magnets was tic
    double found_impl_aoq_tic_mw_corrected;
    double found_impl_aoq_tic_sci_corrected;
    double found_impl_aoq_tic_tpc_corrected;

    double found_aoq_tpc_tpc_and_angle_at_s2tpc_corrected;

    //double found_impl_aoq;                      // <---- found_impl_aoq_sci_mw_corrected
    //double found_aoq_sci_corrected;         // <---- found_impl_aoq_sci_sci_corrected

    double found_impl_mus_dep_energy_non_corr;
    double found_impl_mus2_dep_energy_non_corr;
    double found_impl_mw_x_position_extrapolated_to_stopper_position;
    double found_impl_mw_y_position_extrapolated_to_stopper_position;
    double found_impl_sci42_energy ;
    double found_impl_sci43_energy;
    double found_impl_sci21_position;
    double found_impl_tof_ps;
    int found_multiplicity_of_x_strips;  // stored in pixel
    int found_multiplicity_of_y_strips;
    double found_sum_en_x;
    double found_sum_en_y;
    double found_sci41horiz_position;
    double found_degrader_dE_gamma;
    int found_nr_of_times_retrieved;

    bool  flag_matching_is_valid;
    bool flag_ok_and_2nd_decay;

    bool flag_implantation_successful ; // when it was really put into pixels (conditions on Veto, zet vs aoq, etc.)
    bool found_previous_implantation_never_decayed;
    int found_sci21_left;
    int  found_sci21_right;
    int    found_previous_decay_time_for_this_implantation_in_miliseconds;

    // for result of successul cross matching
    unsigned long long found_cross_impl_timestamp;

    double found_cross_impl_zet;
    double found_cross_impl_zet2;
    double found_cross_impl_zet_sci;
    double found_cross_impl_zet2_sci;

    /*double found_cross_impl_aoq;
    double found_cross_aoq_sci_corrected;*/
    // before magnets - was sci
    double found_cross_impl_aoq_sci_mw_corrected;
    double found_cross_impl_aoq_sci_sci_corrected;
    double found_cross_impl_aoq_sci_tpc_corrected;

    // before magnets was tic
    double found_cross_impl_aoq_tic_mw_corrected;
    double found_cross_impl_aoq_tic_sci_corrected;
    double found_cross_impl_aoq_tic_tpc_corrected;

    double found_cross_aoq_tpc_tpc_and_angle_at_s2tpc_corrected;

    double found_cross_impl_mus_dep_energy_non_corr;
    double found_cross_impl_mus2_dep_energy_non_corr;
    double found_cross_impl_mw_x_position_extrapolated_to_stopper_position;
    double found_cross_impl_mw_y_position_extrapolated_to_stopper_position;
    double found_cross_impl_sci42_energy ;
    double found_cross_impl_sci43_energy;
    double found_cross_impl_sci21_position;
    double found_cross_impl_tof_ps;
    int found_cross_multiplicity_of_x_strips;  // stored in pixel
    int found_cross_multiplicity_of_y_strips;
    double found_cross_sum_en_x;
    double found_cross_sum_en_y;
    double found_cross_sci41horiz_position;
    double found_cross_degrader_dE_gamma;
    int found_cross_nr_of_times_retrieved;
    int found_cross_sci21_left;
    int  found_cross_sci21_right;
    int    found_cross_previous_decay_time_for_this_implantation_in_miliseconds;
    bool flag_cross_matching_is_valid;


    long long implantation_decay_time_difference_in_timestamp_ticks;
    int implantation_decay_time_difference_in_seconds;
    int implantation_decay_time_difference_in_one_tenth_of_seconds;
    int implantation_decay_time_difference_in_miliseconds;

    // in case if some geometry is broken
    enum enum_for_how_to_match_position {  xy_both,  only_x,  only_y };
    enum_for_how_to_match_position  how_to_match_position ;

    bool matching_without_x()
    {
        return (how_to_match_position ==  only_y) ;
    }
    bool matching_without_y()
    {
        return (how_to_match_position ==  only_x) ;
    }


public:
    Tactive_stopper_combined_xy(Tactive_stopper_full *
                                const owner,  std::string name,
                                unsigned int nr, char symbol_);
    ~Tactive_stopper_combined_xy();

    static
    vector<unsigned int*> addresses_of_implanted_pixels;

    static
    void clear_pattern_of_impantation_modules()
    {
        pattern_of_impantation_modules = 0;

        // by this below, we will know in which pixels we need to store the pattern
        addresses_of_implanted_pixels.clear();  // zeroing the vector of addresses

    }
    static void reset_pattern_modules()
    {
        for(int i = 0 ; i < 32; i++)
            pattern_says_that_also_was_implanted_module[i] = false;
    }

    double give_implantation_energy_cal(bool it_is_x, int  strip_nr)
    {
        if(it_is_x)
            return x_stripe[strip_nr]->give_energy_implantation_cal();
        else
            return y_stripe[strip_nr]->give_energy_implantation_cal();
    }
    // { return energy_cal_noise_threshold; }

    void analyse_current_event() ; //  pure virtual  ;
    void make_preloop_action(ifstream &) ;// read the calibration
    // factors, gates

    void zero_flags_fired_vectors();
    void implantation_action();
    bool decay_action();


    Tactive_stopper_full* const  give_owner_ptr()
    {
        return owner;
    }
    void not_fired() ;

    vector< spectrum_abstr*>*  give_spectra_pointer()
    {
        return frs_spectra_ptr  ;
    }




protected:
    void increment_your_spectra()
    {}

    void save_spectra()
    {}

    void create_my_spectra();
    void calculate_position_and_RMS_for_implantation();
    void calculate_position_and_RMS_for_decay();

public:
    // %%%%%%%%%%%%%%%   nested class   %%%%%%%%%%%%%%%%%%%%%%
    class active_stopper_pixel:  public Tincrementer_donnor
    {
        //public:
        bool flag_valid_impl; // sometime we want to discard too old implantation

        int nr_of_times_retrieved;// how many decays were retrieving this pixel
        // #$% place to add wishes to pixels
        unsigned long long timestamp_of_implantation;
        double zet;
        double zet2;   // <--- with normal, mw correction in music
        double zet_sci;
        double zet2_sci;  // <--- with sci correction in music

        //double aoq;
        // double aoq_sci_corrected;

        // before magnets - was sci
        double aoq_sci_mw_corrected;
        double aoq_sci_sci_corrected;
        double aoq_sci_tpc_corrected;
        // before magnets was tic
        double aoq_tic_mw_corrected;
        double aoq_tic_sci_corrected;
        double aoq_tic_tpc_corrected;

        double aoq_tpc_tpc_and_angle_at_s2tpc_corrected;

        double mus_dep_energy_non_corr;
        double mus2_dep_energy_non_corr;

        double mw_x_position_extrapolated_to_stopper_position;
        double mw_y_position_extrapolated_to_stopper_position;
        double sci42_energy;
        double sci43_energy;
        double sci21_position;
        int sci21_left;
        int sci21_right;

        double tof_ps;
        int
        multiplicity_of_x_strips_above_thrs_during_calculation_of_the_position;
        int
        multiplicity_of_y_strips_above_thrs_during_calculation_of_the_position;
        double sum_en_x;
        double sum_en_y;

        double sci41horiz_position;
        double degrader_dE_gamma;

        // -----------
        //      unsigned long long timestamp_of_previous_decay;
        // Explanation: for situation when we have one implantation and later several decays
        // if we retrieve the implantation information we update the counter: nr_of_times_retrieved
        // but also we store here the time information when we took this information.
        // This is usefull when we have two decays from information


        int time_of_previous_decay_in_miliseconds;
        // If we are measuring the reaction with two (or more) beta decays,
        // Ernest would like to have the information about the previous decay. So if it is a second decay for particular implantation
        // he would like to know the time of the previous implantation


        bool previous_implantation_never_decayed;  // this is for situation when
        // there is the next impantation (in this pixel), but there was no decay from the
        // previous implantation,
        // So there is a risk, taht we can have a decay but we are not sure if it is from
        // the newest implantaion or form the older one.

        int nr_x, nr_y;
        int module_id_nr;

        // to know if any other modules had good implantation in the same event
        unsigned int implantations_pattern_word;  //


    public:
        void store_implantation(
            //int ener,
            unsigned long long timest,
            double z, double z2,
            double z_sci, double z2_sci,
            double aoq_sci_mw_corrected_,       // 4
            double aoq_sci_sci_corrected_,
            double aoq_sci_tpc_corrected_,
            double aoq_tic_mw_corrected_,
            double aoq_tic_sci_corrected_,
            double aoq_tic_tpc_corrected_,      // 9

            double aoq_tpc_tpc_and_angle_at_s2tpc_corrected_,

            double mus_dE,
            double mus2_dE,

            //double a,
            double mw_x_position_extrapolated,
            double mw_y_position_extrapolated,
            double sci42e, double sci43e,
            double sci21_p_a,
            double tof_ps_a,
            int multiplicity_of_x_strips_arg,
            int multiplicity_of_y_strips_arg,
            double sum_en_x_arg,
            double sum_en_y_arg,
            double sci41horiz_position_arg,
            double degrader_dE_gamma_arg,
            //double aoq_sci_corrected_arg,
            int sci21_left_arg,
            int sci21_right_arg      // #$% place to add wishes to pixels
        );

        unsigned int *  give_address_of_pattern_word()
        {
            return &implantations_pattern_word;
        }

        //
        void make_invalid()
        {
            flag_valid_impl = false ;
            nr_of_times_retrieved = 0;
        }
        // constructor
        active_stopper_pixel()
        {
            make_invalid() ;
        }

        //-------------------------------------
        bool are_you_in_time_gate(long long current_time);

        bool retrieve_implantation(
            unsigned long long *timest,
            double *z,  double *z2,
            double *z_sci,  double *z2_sci,
            double *aoq_sci_mw_corrected_,
            double *aoq_sci_sci_corrected_,
            double *aoq_sci_tpc_corrected_,
            double *aoq_tic_mw_corrected_,
            double *aoq_tic_sci_corrected_,
            double *aoq_tic_tpc_corrected_,

            double *aoq_tpc_tpc_and_angle_at_s2tpc_corrected_,
            //double *a,
            // double * aoq_sci_corrected_arg,
            double *mus_dE_non_cor,  double *mus2_dE_non_cor,

            double *mw_x_position_extrapolated,
            double *mw_y_position_extrapolated,
            double *sci42e, double *sci43e,
            double *sci21_position_a,
            double *tof_ps_a,
            int *multiplicity_of_x_strips_arg,
            int *multiplicity_of_y_strips_arg,
            double *sum_en_x_arg,
            double *sum_en_y_arg,
            // #$% place to add wishes to pixels
            double * sci41horiz_position_arg,
            double * degrader_dE_gamma_arg,

            int *nr_of_times_retrieved_arg,
            bool * previous_implantation_never_decayed_arg,
            int *sci21_left_arg,
            int *sci21_right_arg,
            int *previous_decay_time_for_this_implantation_in_miliseconds
        )
        {
            *timest = timestamp_of_implantation;
            *z = zet;
            *z2 = zet2;
            *z_sci = zet_sci;
            *z2_sci = zet2_sci;
            //*a = aoq;
            //*aoq_sci_corrected_arg = aoq_sci_corrected;
            *aoq_sci_mw_corrected_ = aoq_sci_mw_corrected;
            *aoq_sci_sci_corrected_ =  aoq_sci_sci_corrected;
            *aoq_sci_tpc_corrected_ = aoq_sci_tpc_corrected;
            *aoq_tic_mw_corrected_ =  aoq_tic_mw_corrected;
            *aoq_tic_sci_corrected_ =  aoq_tic_sci_corrected;
            *aoq_tic_tpc_corrected_ =  aoq_tic_tpc_corrected;

            *aoq_tpc_tpc_and_angle_at_s2tpc_corrected_ = aoq_tpc_tpc_and_angle_at_s2tpc_corrected;

            *mus_dE_non_cor =  mus_dep_energy_non_corr;
            *mus2_dE_non_cor = mus2_dep_energy_non_corr;
            *mw_x_position_extrapolated =
                mw_x_position_extrapolated_to_stopper_position;
            *mw_y_position_extrapolated =
                mw_y_position_extrapolated_to_stopper_position;
            *sci42e = sci42_energy ;
            *sci43e = sci43_energy;
            *sci21_position_a = sci21_position ;
            *tof_ps_a = tof_ps;
            *multiplicity_of_x_strips_arg =
                multiplicity_of_x_strips_above_thrs_during_calculation_of_the_position;

            *multiplicity_of_y_strips_arg =
                multiplicity_of_y_strips_above_thrs_during_calculation_of_the_position;

            *sum_en_x_arg = sum_en_x;
            *sum_en_y_arg = sum_en_y;
            // #$% place to add wishes to pixels
            *sci41horiz_position_arg = sci41horiz_position;
            *degrader_dE_gamma_arg = degrader_dE_gamma;

            *nr_of_times_retrieved_arg = ++nr_of_times_retrieved;
            *previous_implantation_never_decayed_arg  = previous_implantation_never_decayed;

            *sci21_left_arg = sci21_left;
            *sci21_right_arg = sci21_right;

            // if one implantation has two or more decays.
            *previous_decay_time_for_this_implantation_in_miliseconds = time_of_previous_decay_in_miliseconds;

            return flag_valid_impl; // stephan wants this always, even if it was
            // already taken once by some other decay - even the "noise" decay
        }

        // this function is called after successful match, in case if there will be the next decay
        // from the same implantation (some reactions can have this) we will know
        // when was the previous (Ernest wants this for some efficiency calculation)
        void remember_the_previous_decay_time(int time_in_miliseconds)
        {
            time_of_previous_decay_in_miliseconds = time_in_miliseconds;
        }

        long long give_stored_timestamp()
        {
            return timestamp_of_implantation;
        }

        unsigned int retrieve_implantations_pattern_word()
        {
            return implantations_pattern_word;
        }
    };
    // %%%%%%%%%%%%%%%% end of nested class %%%%%%%%%%%%%

protected:
    active_stopper_pixel
    pixel[NR_OF_STOPPER_STRIPES_X][NR_OF_STOPPER_STRIPES_Y];
    int time_since_last_implantation[
        NR_OF_STOPPER_STRIPES_X][NR_OF_STOPPER_STRIPES_Y];
    bool time_since_last_implantation_valid[
        NR_OF_STOPPER_STRIPES_X][NR_OF_STOPPER_STRIPES_Y];



    void successful_match_impl_with_decay(int x, int y);

    spectrum_1D * spec_fan , * spec_fan_above_thresholds;
    spectrum_1D * spec_x_1st_energy_implantation_cal;
    spectrum_1D * spec_x_2nd_energy_implantation_cal;
    spectrum_1D * spec_x_3rd_energy_implantation_cal;

    spectrum_1D * spec_y_1st_energy_implantation_cal;
    spectrum_1D * spec_y_2nd_energy_implantation_cal;
    spectrum_1D * spec_y_3rd_energy_implantation_cal;

    spectrum_1D * spec_x_1st_energy_decay_cal;
    spectrum_1D * spec_x_2nd_energy_decay_cal;
    spectrum_1D * spec_x_3rd_energy_decay_cal;

    spectrum_1D * spec_y_1st_energy_decay_cal;
    spectrum_1D * spec_y_2nd_energy_decay_cal;
    spectrum_1D * spec_y_3rd_energy_decay_cal;

    // spectra multiplicity with threshold
    spectrum_1D
    *spec_x_multiplicity_for_energy_cal_implantation_above_threshold;
    spectrum_1D
    *spec_y_multiplicity_for_energy_cal_implantation_above_threshold;

    spectrum_1D
    *spec_x_multiplicity_for_energy_cal_decay_above_threshold;
    spectrum_1D
    * spec_y_multiplicity_for_energy_cal_decay_above_threshold;

    // spectra multiplicity without threshold
    spectrum_1D  *spec_x_multiplicity_for_energy_cal;
    spectrum_1D  *spec_y_multiplicity_for_energy_cal;


    double x_position_implantation;
    double x_position_decay  ;

    spectrum_1D  *spec_x_position_implantation ;
    spectrum_1D  *spec_x_position_decay ;

    double  y_position_implantation  ;
    double y_position_decay  ;

    spectrum_1D  *spec_y_position_implantation ;
    spectrum_1D  *spec_y_position_decay;


    // in case if mianownik is zero, it is impossible to
    // calculate the positions
    bool x_position_implantation_ok;
    bool y_position_implantation_ok;
    bool x_position_implantation_thrsh_ok;
    bool y_position_implantation_thrsh_ok;

    bool x_position_decay_ok;
    bool y_position_decay_ok;
    bool x_position_decay_thrsh_ok;
    bool y_position_decay_thrsh_ok;

    //---------
    spectrum_2D  *spec_map_of_implantations;
    spectrum_2D  *spec_map_of_decays;
    spectrum_2D  *spec_map_of_matching;

    spectrum_2D  *spec_map_of_decays_without_thrsh;

    // ----- thrsh
    double x_position_implantation_thrsh;
    double x_position_decay_thrsh  ;
    spectrum_1D  *spec_x_position_implantation_thrsh ;
    spectrum_1D  *spec_x_position_decay_thrsh ;

    double  y_position_implantation_thrsh  ;
    double y_position_decay_thrsh  ;

    spectrum_1D  *spec_y_position_implantation_thrsh ;
    spectrum_1D  *spec_y_position_decay_thrsh;
    //==========
    double  rms_x_implantation ;
    double  rms_x_decay ;
    double  rms_y_implantation;
    double  rms_y_decay ;
    double rms_x_implantation_thrsh ;
    double rms_x_decay_thrsh;
    double rms_y_implantation_thrsh ;
    double rms_y_decay_thrsh;

    // sum energies
    double denominator_x_implantation;
    double denominator_y_implantation;

    double denominator_x_decay;
    double denominator_y_decay;
    double decay_sum_energy_both_xy_stripes;
    //--------------

    static vector<active_stopper_pixel> module_as_one_pixel;
    bool try_to_match_with_other_modules();
    static  spectrum_2D * spec_map_of_cross_matching;
    bool retrieve_this_cross(int nr);
};

#endif // __CINT__

#endif   // def ACTIVE_STOPPER_PRESENT

#endif
