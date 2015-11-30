#ifndef TACTIVE_STOPPER_MUNICH_H
#define TACTIVE_STOPPER_MUNICH_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"
#ifdef MUN_ACTIVE_STOPPER_PRESENT

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

#include "Taoq_calculator.h"
#include "Tzet_calculator.h"
#include "Tfocal_plane.h"
#include "Tscintillator.h"
#include "Tdegrader.h"

/**
System of

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
------------------------------------------------------------
This class is using the file
         Tactive_stopper_vector_strips
which later is using
    Tactive_stopper_one_stripe
-----------------------------------------------------------

Tactive_stopper_munich----->  Tactive_stopper_vector_strips  ---->  Tactive_stopper_one_stripe
-----------------------------------------------------------
*/
class Tactive_stopper_matrix_xy;
/////////////////////////////////////////////////////////////////////////
class Tactive_stopper_munich : public Tfrs_element, public Tincrementer_donnor
{
protected:
    const int my_number;
    int incrementer_with_beamline_position_of_this_device;

    Tactive_stopper_vector_strips*   x_plate;
    Tactive_stopper_vector_strips*   y_plate;

    int (TIFJEvent::*data)[NR_OF_MUN_STOPPER_MODULES]
    [NR_OF_MUN_STOPPER_STRIPES_X + NR_OF_MUN_STOPPER_STRIPES_Y];
    int (TIFJEvent::*data_plate_fired)[NR_OF_MUN_STOPPER_MODULES][2];

    spectrum_2D * spec_map_for_implantations_thrsh;
    spectrum_2D * spec_map_for_decays_thrsh;
    spectrum_2D * spec_map_of_matching;

    /*  spectrum_2D * spec_trajectories_x_of_implantation;
      spectrum_2D * spec_trajectories_y_of_implantation;
      spectrum_2D * spec_trajectories_x_of_decay;
      spectrum_2D * spec_trajectories_y_of_decay;*/

    void create_my_spectra();

    // options------------------------------
    double        x_energy_cal_implantation_gate_dn;
    double        x_energy_cal_implantation_gate_up ;
    double        y_energy_cal_implantation_gate_dn;
    double        y_energy_cal_implantation_gate_up ;

    double        x_energy_cal_decay_gate_dn ;
    double        x_energy_cal_decay_gate_up ;
    double        y_energy_cal_decay_gate_dn ;
    double        y_energy_cal_decay_gate_up ;

    bool        flag_algorithm_for_implantation_position_calculation_is_mean;
    // otherwise MAXimum

    int compression_factor_for_x_position; // the X resolution can be too big. We may treat 2 pixels as one
    // this is important only for x position because of the features of its electronics.



    bool flag_decay_situation_is_needed;  // if we want to work with decays as well

    double decay_sum_energy;

    bool  flag_trigger_of_implantation;  // for incrementers
    bool flag_trigger_of_decay; // for incrementers

    // if we make a successful match between implantation and decay,
    // here we have retrieved values
    int found_impl_energy; // <--- for testing purposes

    unsigned long long found_impl_timestamp;;
    double found_impl_zet;
    double found_impl_zet2;

    // before magnets - was sci
    double found_impl_aoq_sci_mw_corrected;
    double found_impl_aoq_sci_sci_corrected;
    double found_impl_aoq_sci_tpc_corrected;

    // before magnets was tic
    double found_impl_aoq_tic_mw_corrected;
    double found_impl_aoq_tic_sci_corrected;
    double found_impl_aoq_tic_tpc_corrected;
    //---------

    double found_impl_sci42_energy ;
    double found_impl_sci43_energy;
    double found_impl_sci21_position;
    double found_sci41horiz_position;
    double found_degrader_dE_gamma;

    int found_nr_of_times_retrieved;

    bool  flag_matching_is_valid;
    bool flag_ok_and_2nd_decay;

    bool flag_implantation_successful ; // when it was really put into pixels (conditions on Veto, zet vs aoq, etc.)


    // for result of successul cross matching
    unsigned long long found_cross_impl_timestamp;;
    double found_cross_impl_zet;
    double found_cross_impl_zet2;
    double found_cross_impl_mus_dep_energy_non_corr;
    double found_cross_impl_mus2_dep_energy_non_corr;
    double found_cross_impl_aoq;
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
    double found_cross_aoq_sci_corrected;
    int found_cross_nr_of_times_retrieved;

    bool flag_cross_matching_is_valid;


    long long implantation_decay_time_difference_in_timestamp_ticks;
    int implantation_decay_time_difference_in_seconds;
    int implantation_decay_time_difference_in_one_tenth_of_seconds;
    int implantation_decay_time_difference_in_miliseconds;

    static long long implantation_decay_time_gate_low;
    static long long implantation_decay_time_gate_high;


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


    // the positions are always when threshold
    double  universal_x_position_implantation;
    double  universal_y_position_implantation ;

    bool universal_x_position_implantation_ok;
    bool universal_y_position_implantation_ok;




    //   double x_position_decay_thrsh  ;
    //   double y_position_decay_thrsh  ;
    //
    //   bool x_position_decay_thrsh_ok;
    //   bool y_position_decay_thrsh_ok;


    // %%%%%%%%%%%%%%%   nested class   %%%%%%%%%%%%%%%%%%%%
    class active_stopper_pixel:  public Tincrementer_donnor
    {
        //public:
        bool flag_valid_impl; // sometime we want to discard too old implantation

        int nr_of_times_retrieved;// how many decays were retrieving this pixel
        // #$% place to add wishes to pixels
        unsigned long long timestamp_of_implantation;
        double zet;
        double zet2;

        // before magnets - was sci
        double aoq_sci_mw_corrected;
        double aoq_sci_sci_corrected;
        double aoq_sci_tpc_corrected;

        // before magnets was tic
        double aoq_tic_mw_corrected;
        double aoq_tic_sci_corrected;
        double aoq_tic_tpc_corrected;
        //---------

        double sci42_energy;
        double sci43_energy;
        double sci21_position;

        double sci41horiz_position;
        double degrader_dE_gamma;
        // -----------

        int nr_x, nr_y;
        int module_id_nr;
    public:
        void store_implantation(

            unsigned long long timest,     // 1
            double z,     // 2
            double z2,      //3
            double aoq_sci_mw_corrected_,       // 4
            double aoq_sci_sci_corrected_,
            double aoq_sci_tpc_corrected_,
            double aoq_tic_mw_corrected_,
            double aoq_tic_sci_corrected_,
            double aoq_tic_tpc_corrected_,      // 9

            double sci42e, double sci43e,   // 10, 11
            double sci21_p_a,       // 12
            double sci41horiz_position_arg,     // 13
            double degrader_dE_gamma_arg    // 14
        )
        {
            //      energy = ener;
            // at first calculate when was the previous
            /*      time_since_last_implantation[nr_x][nr_y] =
            (timest - timestamp_of_implantation) / 50000;

                  if(    nr_x == 7 &&  nr_y == 5 &&  module_id_nr == 1)
                  cout << "time_since_last_implantation="
                  << time_since_last_implantation[nr_x][nr_y]  << endl;
                  */

            timestamp_of_implantation = timest;
            zet = z;
            zet2 = z2;

            aoq_sci_mw_corrected = aoq_sci_mw_corrected_;
            aoq_sci_sci_corrected =  aoq_sci_sci_corrected_;
            aoq_sci_tpc_corrected = aoq_sci_tpc_corrected_;
            aoq_tic_mw_corrected =  aoq_tic_mw_corrected_;
            aoq_tic_sci_corrected =  aoq_tic_sci_corrected_;
            aoq_tic_tpc_corrected =  aoq_tic_tpc_corrected_;

            sci42_energy = sci42e ;
            sci43_energy = sci43e;
            sci21_position = sci21_p_a ;

            sci41horiz_position = sci41horiz_position_arg;
            degrader_dE_gamma = degrader_dE_gamma_arg;

            nr_of_times_retrieved = 0;
            flag_valid_impl = true;
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
            double *z,
            double *z2,
            double *aoq_sci_mw_corrected_,
            double *aoq_sci_sci_corrected_,
            double *aoq_sci_tpc_corrected_,
            double *aoq_tic_mw_corrected_,
            double *aoq_tic_sci_corrected_,
            double *aoq_tic_tpc_corrected_,
            double *sci42e,
            double *sci43e,
            double *sci21_position_a,
            double * sci41horiz_position_arg,
            double * degrader_dE_gamma_arg,
            int *nr_of_times_retrieved_arg
        )
        {
            *timest = timestamp_of_implantation;
            *z = zet;
            *z2 = zet2;
            *aoq_sci_mw_corrected_ = aoq_sci_mw_corrected;
            *aoq_sci_sci_corrected_ =  aoq_sci_sci_corrected;
            *aoq_sci_tpc_corrected_ = aoq_sci_tpc_corrected;
            *aoq_tic_mw_corrected_ =  aoq_tic_mw_corrected;
            *aoq_tic_sci_corrected_ =  aoq_tic_sci_corrected;
            *aoq_tic_tpc_corrected_ =  aoq_tic_tpc_corrected;

            *sci42e = sci42_energy ;
            *sci43e = sci43_energy;
            *sci21_position_a = sci21_position ;

            // #$% place to add wishes to pixels
            *sci41horiz_position_arg = sci41horiz_position;
            *degrader_dE_gamma_arg = degrader_dE_gamma;

            *nr_of_times_retrieved_arg = ++nr_of_times_retrieved;

            return flag_valid_impl; // stephan wants this always, even if it was
            // already taken once by some other decay - even the "noise" decay
        }

        long long give_stored_timestamp()
        {
            return timestamp_of_implantation;
        }

    };
    // %%%%%%%%%%%%%%%% end of nested class %%%%%%%%%%%%%

protected:
    active_stopper_pixel
    pixel[NR_OF_MUN_STOPPER_STRIPES_X][NR_OF_MUN_STOPPER_STRIPES_Y];
    int time_since_last_implantation[
        NR_OF_MUN_STOPPER_STRIPES_X][NR_OF_MUN_STOPPER_STRIPES_Y];
    bool time_since_last_implantation_valid[
        NR_OF_MUN_STOPPER_STRIPES_X][NR_OF_MUN_STOPPER_STRIPES_Y];

    void successful_match_impl_with_decay(int x, int y);

public:
    Tactive_stopper_munich(string name,
                           int my_number,
                           bool flag_decay_situation_is_needed_,
                           int (TIFJEvent::*data_ptr)[NR_OF_MUN_STOPPER_MODULES]
                           [NR_OF_MUN_STOPPER_STRIPES_X + NR_OF_MUN_STOPPER_STRIPES_Y],
                           int (TIFJEvent::*data_fired_ptr)[NR_OF_MUN_STOPPER_MODULES][2],
                           std::string name_aoq_value,       // <-- to take calculated aoq
                           std::string name_zet_object, std::string name_zet2_object,
                           std::string name_of_focal_plane,
                           std::string name_of_first_good_scintillator,
                           std::string name_of_second_veto_scintillator,
                           std::string name_of_sci21_scintillator,
                           std::string name_of_sci41_scintillator,
                           std::string name_of_degrader_object,
                           std::string name_of_xy_matrix_object
                          );

    ~Tactive_stopper_munich();

    void analyse_current_event();
    void make_preloop_action(ifstream &); // read the calibration factors, gates

    static long long give_implantation_decay_time_gate_low()
    {
        return implantation_decay_time_gate_low;
    }
    static long long give_implantation_decay_time_gate_high()
    {
        return implantation_decay_time_gate_high;
    }

    double  give_universal_x_position_implantation(bool *flag_valid)
    {
        *flag_valid = universal_x_position_implantation_ok;
        return universal_x_position_implantation;
    }
    double  give_universal_y_position_implantation(bool *flag_valid)
    {
        *flag_valid = universal_y_position_implantation_ok;
        return universal_y_position_implantation;
    }


    double  give_universal_x_position_decay(bool *flag_valid)
    {
        *flag_valid = x_plate->is_position_decay_thrsh_ok();
        return  x_plate->give_decay_thrsh_position() ;
    }
    //----------
    double  give_universal_y_position_decay(bool *flag_valid)
    {
        *flag_valid = y_plate->is_position_decay_thrsh_ok();
        return  y_plate->give_decay_thrsh_position() ;;
    }

    double give_decay_sum_energy()
    {
        return decay_sum_energy;
    }

protected:

    void implantation_action();
    bool decay_action();
    void zero_flags_fired_vectors();


    double give_aoq()
    {
        return aoq_object->give_aoq();
    }
    double give_aoq_sci_corrected()
    {
        return aoq_object->give_aoq_sci_corrected();
    }
    double give_zet()
    {
        return zet_object->give_zet();
    };
    double give_zet2()
    {
        return zet2_object->give_zet();
    };

    double give_aoq_sci_sci_corrected()
    {
        return aoq_object->give_aoq_sci_corrected();
    }
    double give_aoq_sci_mw_corrected()
    {
        return aoq_object->give_aoq_mw_corrected();
    }
    double give_aoq_sci_tpc_corrected()
    {
        return aoq_object->give_aoq_sci_tpc_corrected();
    }
    double give_aoq_tic_sci_corrected()
    {
        return aoq_object->give_aoq_tic_sci_corrected();
    }
    double give_aoq_tic_mw_corrected()
    {
        return aoq_object->give_aoq_tic_mw_corrected();
    }
    double give_aoq_tic_tpc_corrected()
    {
        return aoq_object->give_aoq_tic_tpc_corrected();
    }


    // Steve ask to have this - good for situation when the music is not calibrated yet
    double give_music_deposit_energy_non_corr()
    {
        return zet_object->give_music_deposit_energy_non_corr();
    }

    double give_music2_deposit_energy_non_corr()
    {
        return zet2_object->give_music_deposit_energy_non_corr();
    }

    /*  double give_x_extrapolated_here()
      { return focal_plane_object->extrapolate_x_to_distance(distance); }
      double give_y_extrapolated_here()
      { return focal_plane_object->extrapolate_y_to_distance(distance); }*/
    double give_first_good_sci_energy()
    {
        return first_good_scintillator->give_e();
    }
    double give_second_veto_sci_energy()
    {
        return second_veto_scintillator->give_e();
    }
    double give_tof_ps()
    {
        return aoq_object->give_tof_ps();
    }
    double give_sci21_position()
    {
        return sci21_object->give_position();
    }
    //double give_sci21_left_fired() { return sci21_object->give_sci21_left_fired(); }
    //double give_sci21_right_fired() { return sci21_object->give_sci21_right_fired(); }

    // #$% place to add wishes to pixels
    double give_sci41horiz_position()
    {
        return sci41horiz_object->give_position();
    }
    double give_degrader_dE_gamma()
    {
        return degrader_object->give_dE_gamma();
    }

    // pointers to objects which will supply the information
    Taoq_calculator  *aoq_object;
    Tzet_calculator  *zet_object;
    Tzet_calculator  *zet2_object;
    // the other music has sometimes better resolution

    Tfocal_plane * focal_plane_object ;
    Tscintillator * first_good_scintillator;
    Tscintillator * second_veto_scintillator;
    Tscintillator * sci21_object;
    Tscintillator * sci41horiz_object;
    Tdegrader *degrader_object;
    Tactive_stopper_matrix_xy * xy_matrix_object;

    bool flag_match_only_same_pixel;

};
#endif  // MUN_ACTIVE_STOPPER_PRESENT


#endif // __CINT__
#endif  // GUARDIAN
