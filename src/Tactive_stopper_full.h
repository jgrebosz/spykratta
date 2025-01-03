//
// C++ Interface: Tactive stopper_combined X and Y
//
// Description:
//
/*


TActive_stopper_full
         -----> Tactive_stopper_combined_xy_universal (to samo co: Tactive_stopper_combined_xy)
                   ------->Tactive_stopper_stripe



*/
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TACTIVE_STOPPER_FULL_H
#define TACTIVE_STOPPER_FULL_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"

#ifdef ACTIVE_STOPPER_PRESENT
#include "Tactive_stopper_combined_xy.h"
#define PLEASE_COMPILE_THIS_FILET_ACTIVE_STOPPER_FULL_H
#endif




#ifdef PLEASE_COMPILE_THIS_FILET_ACTIVE_STOPPER_FULL_H

#include "spectrum.h"
#include "TIFJEvent.h"


#include <string>
#include <vector>

/**
System of many stripes creating one active stopper

  @author dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>
*/

#include "Taoq_calculator.h"
#include "Tzet_calculator.h"
#include "Tfocal_plane.h"
#include "Tscintillator.h"
#include "Tdegrader.h"


////////////////////////////////////////////////////////////////////////
class Tactive_stopper_full : public Tfrs_element, public Tincrementer_donnor
{
    double distance ;

    const int id_nr;

    static int how_many_implantations_in_this_event;

    vector <Tactive_stopper_combined_xy_universal *> module;

    static double energy_cal_implantation_threshold;
    static double energy_cal_implantation_threshold_upper;

    static double energy_cal_decay_threshold;
    static double energy_cal_decay_threshold_upper;

    static long long implantation_decay_time_gate_low;
    static long long implantation_decay_time_gate_high;

    static bool flag_match_only_same_pixel;

    int impl_multiplicity_above_thrsh;
    // how many implantation (in case of energies above threshold)
    int  decay_multiplicity_above_thrsh;
    // how many implantation (in case of energies above threshold)

    bool flag_accept_more_than_one_implantation;

    bool flag_algorithm_for_implantation_position_calculation_is_mean;
    // otherwise it is Max value

    int (TIFJEvent::*active_stopper)
    [HOW_MANY_STOPPER_MODULES]
    [NR_OF_STOPPER_STRIPES_X + NR_OF_STOPPER_STRIPES_Y];

#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

    int (TIFJEvent::*active_stopper_time)
    [HOW_MANY_STOPPER_MODULES]
    [NR_OF_STOPPER_STRIPES_X + NR_OF_STOPPER_STRIPES_Y];
#endif

    int (TIFJEvent::*active_stopper_fired)[
        HOW_MANY_STOPPER_MODULES] [2];



    bool flag_matching_was_ok;
    // if everything was ok, we can use Zet, Aoq,  - for
    // validation of incrmenters

    bool  flag_trigger_of_implantation;  // for incrementers
    bool flag_trigger_of_decay; // for incrementers

    // GOOD and VETO scintillators
    double sci42_acceptable_energy_low;
    double sci42_acceptable_energy_high;
    double sci43_veto_energy_low;
    double sci43_veto_energy_high;

    // values of zet and AoQ which will allow making "implantation" action
    int type_of_zet;
    double somezet_low;
    double somezet_high;

    int type_of_aoq;
    double someaoq_low;
    double someaoq_high;

    bool are_zet_aoq_legal()
    {
        double some_zet = 0, some_aoq = 0;

        switch(type_of_zet)
        {
        case 0:
            some_zet = give_zet();
            break;
        case 1:
            some_zet = give_zet2();
            break;
        case 2:
            some_zet = give_zet_sci();
            break;
        case 3:
            some_zet = give_zet2_sci();
            break;
        default:
            cout << "Error in definition of zet used for active stopper implantation\n";
            exit(2);
        }

        switch(type_of_aoq)
        {
        case 0:
            some_aoq = give_aoq();
            break;
        case 1:
            some_aoq = give_aoq_sci_corrected();
            break;
        default:
            cout << "Error in definition of aoq used for active stopper implantation\n";
            exit(2);
        }
        if(some_zet < somezet_low || some_zet > somezet_high)
            return false;
        if(some_aoq < someaoq_low || some_aoq > someaoq_high)
            return false;
        return true;
    }



    //   long long implantation_decay_time_difference_in_timestamp_ticks;
    //
    //   int implantation_decay_time_difference_in_seconds;
    //   int implantation_decay_time_difference_in_one_tenth_of_seconds;

    //  long long recent_implantation_event_timestamp;
    //  int decay_to_any_recent_implantation_time_difference_in_ms;
    // recent in the spill - we hope...

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

    bool module_fired[HOW_MANY_STOPPER_MODULES];
    // to know that more than one implementation or decay


    //================ for incremeters
    double x_position_implantation_thrsh;
    bool x_position_implantation_thrsh_ok;

    double x_position_decay_thrsh;
    bool x_position_decay_thrsh_ok;

    double y_position_implantation_thrsh;
    bool y_position_implantation_thrsh_ok;

    double y_position_decay_thrsh;
    bool  y_position_decay_thrsh_ok;

#ifdef PLAMEN

    double plamen__module_0_stripe_n_en_impl_cal_MINUS_module_1_stripe_n_en_impl_cal[16];
#endif
    ////////////////////////////////////
public:

    void incr_how_many_implantations_in_this_event()
    {
        how_many_implantations_in_this_event++;
    }

    void mark_me_as_succesfully_calclulated_the_position(int nr_of_module)
    {
        module_fired[nr_of_module] = true;
    }

    void incr_impl_multiplicity_above_thrsh()
    {
        impl_multiplicity_above_thrsh++;
    }
    void incr_decay_multiplicity_above_thrsh()
    {
        decay_multiplicity_above_thrsh++;
    }

    static bool flag_this_is_implantation_event;  // to distinguish two
    // modes of operation implantation/decay

    enum type_of_active_stopper_event { undefined, implantation, decay };
    // to distinguish two modes of operation implantation/decay
    static type_of_active_stopper_event   active_stopper_event_type;


    static bool give_flag_match_only_same_pixel()
    {
        return flag_match_only_same_pixel;
    }
    static long long give_implantation_decay_time_gate_low()
    {
        return implantation_decay_time_gate_low;
    }
    static long long give_implantation_decay_time_gate_high()
    {
        return implantation_decay_time_gate_high;
    }

    bool give_flag_algorithm_for_implantation_position_calculation_is_mean()
    {
        return flag_algorithm_for_implantation_position_calculation_is_mean;
    }

    Tactive_stopper_full(
        std::string name, int id,
        int (TIFJEvent::*active_stopper_ptr)[HOW_MANY_STOPPER_MODULES]
        [NR_OF_STOPPER_STRIPES_X + NR_OF_STOPPER_STRIPES_Y],
#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
        int (TIFJEvent::*active_stopper_time_ptr)[HOW_MANY_STOPPER_MODULES]
        [NR_OF_STOPPER_STRIPES_X + NR_OF_STOPPER_STRIPES_Y],
#endif


        int (TIFJEvent::*active_stopper_fired_ptr)
        [HOW_MANY_STOPPER_MODULES][2],
        std::string name_aoq_value,       // <-- to take calculated aoq
        std::string name_zet_object, std::string name_zet2_object,
        std::string name_of_focal_plane,
        std::string name_of_first_good_scintillator,
        std::string name_of_second_veto_scintillator,
        std::string name_of_sci21_scintillator,
        std::string name_of_sci41_scintillator,
        std::string name_of_degrader_object
    );

    ~Tactive_stopper_full();

    //   static double give_energy_cal_implantation_threshold()
    //   { return energy_cal_implantation_threshold;}
    //   static double give_energy_cal_decay_threshold()
    //   { return energy_cal_decay_threshold;}

    static double is_energy_cal_implantation_in_gate(double en)
    {
        return
            (en >= energy_cal_implantation_threshold
             &&
             en <= energy_cal_implantation_threshold_upper);
    }

    static double is_energy_cal_decay_in_gate(double en)
    {
        return
            (en >= energy_cal_decay_threshold
             &&
             en <= energy_cal_decay_threshold_upper);
    }


    void analyse_current_event() ; //  pure virtual  ;
    void make_preloop_action(ifstream &) ;
    // read the calibration factors, gates

    int give_event_data(int i, int j, int k)
    {
        return (event_unpacked->*active_stopper)[i]
               [(j * NR_OF_STOPPER_STRIPES_X) + k];
    }

#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
    int give_event_time_data(int i, int j, int k)
    {
        return (event_unpacked->*active_stopper_time)[i]
               [(j * NR_OF_STOPPER_STRIPES_X) + k];
    }
#endif   // ACTIVE_STOPPERS_HAVE_TIME_INFORMATION


    bool give_event_fired(int i, int j)
    {
        return (event_unpacked->*active_stopper_fired)[i][j];
    }


    vector< spectrum_abstr*>*  give_spectra_pointer()
    {
        return frs_spectra_ptr  ;
    }

    double give_aoq()
    {
        return aoq_object->give_aoq();
    }
    double give_aoq_sci_corrected()
    {
        return aoq_object->give_aoq_sci_corrected();
    }

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

    double give_aoq_tpc_tpc_and_angle_at_s2tpc_corrected()
    {
#ifdef  TPC22_PRESENT
        return aoq_object->give_aoq_tpc_tpc_and_angle_at_s2tpc_corrected();
#else
        return -9999;
#endif

    }

    double give_zet()
    {
        return zet_object->give_zet();
    };
    double give_zet2()
    {
        return zet2_object->give_zet();
    };
    double give_zet_sci()
    {
        return zet_object->give_zet_sci();
    };
    double give_zet2_sci()
    {
        return zet2_object->give_zet_sci();
    };

    // Steve ask to have this - good for situation when the music is not calibrated yet
    double give_music_deposit_energy_non_corr()
    {
        return zet_object->give_music_deposit_energy_non_corr();
    }

    double give_music2_deposit_energy_non_corr()
    {
        return zet2_object->give_music_deposit_energy_non_corr();
    }

    double give_x_extrapolated_here()
    {
        return focal_plane_object->extrapolate_x_to_distance(distance);
    }
    double give_y_extrapolated_here()
    {
        return focal_plane_object->extrapolate_y_to_distance(distance);
    }
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
    int give_sci21_left()
    {
        return sci21_object->give_left_raw();
    }
    int give_sci21_right()
    {
        return sci21_object->give_right_raw();
    }

    // #$% place to add wishes to pixels
    double give_sci41horiz_position()
    {
        return sci41horiz_object->give_position();
    }
    double give_degrader_dE_gamma()
    {
        return degrader_object->give_dE_gamma();
    }

    void incr_impl_map(double x, double y)
    {
        spec_map_of_implantations->manually_increment(x, y);
    }

    void incr_decay_map(double x, double y)
    {
        spec_map_of_decays->manually_increment(x, y);
    }

    void incr_matching_map(double x, double y)
    {
        spec_map_of_matching->manually_increment(x, y);
    }

    // for setting incrementers
    //================ for incremeters

    void set_x_position_implantation_thrsh(double x)
    {
        x_position_implantation_thrsh = x;
        x_position_implantation_thrsh_ok = true;
    }

    void set_x_position_decay_thrsh(double x)
    {
        x_position_decay_thrsh = x;
        x_position_decay_thrsh_ok = true;
    }
    //------
    void set_y_position_implantation_thrsh(double y)
    {
        y_position_implantation_thrsh = y;
        y_position_implantation_thrsh_ok = true;
    }
    //----------
    void set_y_position_decay_thrsh(double y)
    {
        y_position_decay_thrsh = y;
        y_position_decay_thrsh_ok = true;
    }


    int get_id_nr()
    {
        return  id_nr;
    }

    static string give_symbol_nr(uint i)
    {
        if(i < characters.size())
            return string(1, characters[i]);
        else
            return "not_defined_symbol_of_stopper_module" ;
    }

    static
    string remove_substring_hitN_from_the_name(string input);
protected:
    void save_spectra()
    {}
    void create_my_spectra();

    spectrum_1D * spec_multiplicity_of_modules_implantation;
    spectrum_1D * spec_multiplicity_of_modules_decay;

    spectrum_2D  *spec_map_of_implantations;
    spectrum_2D  *spec_map_of_decays;
    spectrum_2D  *spec_map_of_matching;

    //   spectrum_2D  *matr_zet_vs_aoq;
    //   spectrum_2D  *matr__vs_aoq;
    static string characters;  // ("LMR");  // symbolic names

};


#endif // __CINT__
#undef PLEASE_COMPILE_THIS_FILET_ACTIVE_STOPPER_FULL_H
#endif   // def PLEASE_COMPILE_THIS_FILET_ACTIVE_STOPPER_FULL_H

#endif
