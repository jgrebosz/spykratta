//
// C++ Implementation: tstopper_combined
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "Tactive_stopper_combined_xy.h"

#ifdef ACTIVE_STOPPER_PRESENT
//#include <cstring>
//#include <cstdlib>
#include <algorithm>
#include <sstream>
#include "Tnamed_pointer.h"
using namespace std;

#include "Tactive_stopper_full.h"
#include "TIFJAnalysis.h" // for verbose

// static
vector<Tactive_stopper_combined_xy::active_stopper_pixel>
Tactive_stopper_combined_xy::module_as_one_pixel;

spectrum_2D * Tactive_stopper_combined_xy::spec_map_of_cross_matching;

// below is a static  variable which remembers in which modules was possible to make the succesfull implantation
// we will store this information in the pixel, after finishing analysing all modules of active stopper for
// current implantation event
unsigned int   Tactive_stopper_combined_xy::pattern_of_impantation_modules;

// by this below, we will know in which pixels we need to store the pattern
vector<unsigned int*> Tactive_stopper_combined_xy::addresses_of_implanted_pixels;


// after retrieving the pattern from the decayed pixel, we know which
// other modules (stopper_combined_xy) were also implanted during the matched implantation
bool Tactive_stopper_combined_xy::pattern_says_that_also_was_implanted_module[32];

// ******************************************************************
Tactive_stopper_combined_xy::Tactive_stopper_combined_xy(
    Tactive_stopper_full * ow,  std::string name, unsigned int nr, char symbol_):
    Tfrs_element(name), module_id_nr(nr), owner(ow)
{
    //string nam(name);
    string name_of_stripe ;
    symbol = string(1, symbol_);

    Tactive_stopper_combined_xy::active_stopper_pixel  p;
    p.make_invalid();
    module_as_one_pixel.push_back(p);

    //----------------- X ------------------------
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_X ; i++)
        // stopper_m0_x_01
    {
        ostringstream out ;
        // what about leading zeros
        out << name << "_x_" <<  setfill('0') << setw(2) << i ;
        name_of_stripe = out.str() ;
        // cout << "X name is : " << name_of_stripe << endl;
        x_stripe.push_back(new Tactive_stopper_stripe(
                               this, name_of_stripe, module_id_nr, 0, i));
    }

    // ----------------- Y -------------------------
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_Y ; i++)
    {
        ostringstream out ;
        // what about leading zeros
        out  << name << "_y_" <<  setfill('0') << setw(2) << i ;
        name_of_stripe = out.str() ;
        // cout << "Y name is : " << name_of_stripe << endl;
        y_stripe.push_back(new Tactive_stopper_stripe(
                               this, name_of_stripe , module_id_nr, 1, i));

    }

    // INCREMENTERS

    named_pointer[name_of_this_element + "_x_1st_energy_implantation_cal"]
    = Tnamed_pointer(&x_1st_energy_implantation_cal, 0, this) ;

    named_pointer[name_of_this_element + "_x_1st_energy_decay_cal"]
    = Tnamed_pointer(&x_1st_energy_decay_cal, 0, this) ;


    named_pointer[name_of_this_element + "_x_2nd_energy_implantation_cal"] =
        Tnamed_pointer(&x_2nd_energy_implantation_cal, 0, this) ;

    named_pointer[name_of_this_element + "_x_3rd_energy_implantation_cal"] =
        Tnamed_pointer(&x_3rd_energy_implantation_cal, 0, this) ;

    named_pointer[name_of_this_element + "_y_1st_energy_implantation_cal"]
    = Tnamed_pointer(&y_1st_energy_implantation_cal, 0, this) ;

    named_pointer[name_of_this_element + "_y_2nd_energy_implantation_cal"] =
        Tnamed_pointer(&y_2nd_energy_implantation_cal, 0, this) ;

    named_pointer[name_of_this_element + "_y_3rd_energy_implantation_cal"] =
        Tnamed_pointer(&y_3rd_energy_implantation_cal, 0, this) ;


    named_pointer[name_of_this_element + "_x_1st_energy_decay_cal"]
    = Tnamed_pointer(&x_1st_energy_decay_cal, 0, this) ;


    named_pointer[name_of_this_element + "_rms_x_implantation"] =
        Tnamed_pointer(&rms_x_implantation,
                       &x_position_implantation_ok, this) ;

    named_pointer[name_of_this_element + "_rms_x_decay"] =
        Tnamed_pointer(&rms_x_decay, &x_position_decay_ok, this) ;

    named_pointer[name_of_this_element + "_rms_y_implantation"] =
        Tnamed_pointer(&rms_y_implantation,
                       &y_position_implantation_ok, this) ;

    named_pointer[name_of_this_element + "_rms_y_decay"] =
        Tnamed_pointer(&rms_y_decay, &y_position_decay_ok, this) ;

    // thrshlds -----------
    named_pointer[name_of_this_element + "_rms_x_implantation_thrsh"] =
        Tnamed_pointer(&rms_x_implantation_thrsh,
                       &x_position_implantation_thrsh_ok, this) ;

    named_pointer[name_of_this_element + "_rms_x_decay_thrsh"] =
        Tnamed_pointer(&rms_x_decay_thrsh,
                       &x_position_decay_thrsh_ok, this) ;

    named_pointer[name_of_this_element + "_rms_y_implantation_thrsh"] =
        Tnamed_pointer(&rms_y_implantation_thrsh,
                       &y_position_implantation_thrsh_ok, this) ;

    named_pointer[name_of_this_element + "_rms_y_decay_thrsh"] =
        Tnamed_pointer(&rms_y_decay_thrsh,
                       &y_position_decay_thrsh_ok, this) ;

    // mult

    named_pointer[name_of_this_element +
                  "_x_multiplicity_for_energy_cal_implantation_above_threshold"] =
                      Tnamed_pointer(
                          &x_multiplicity_for_energy_cal_implantation_above_threshold,
                          0, this) ;


    named_pointer[name_of_this_element +
                  "_y_multiplicity_for_energy_cal_implantation_above_threshold"] =
                      Tnamed_pointer(
                          &y_multiplicity_for_energy_cal_implantation_above_threshold,
                          0, this) ;


    named_pointer[name_of_this_element +
                  "_x_multiplicity_for_energy_cal_decay_above_threshold"] =
                      Tnamed_pointer(
                          &x_multiplicity_for_energy_cal_decay_above_threshold,
                          0, this) ;


    named_pointer[name_of_this_element +
                  "_y_multiplicity_for_energy_cal_decay_above_threshold"] =
                      Tnamed_pointer(
                          &y_multiplicity_for_energy_cal_decay_above_threshold,
                          0, this) ;


    named_pointer[name_of_this_element + "_x_multiplicity_for_energy_cal"] =
        Tnamed_pointer(&x_multiplicity_for_energy_cal, 0, this) ;


    named_pointer[name_of_this_element + "_y_multiplicity_for_energy_cal"] =
        Tnamed_pointer(&y_multiplicity_for_energy_cal, 0, this) ;


    //====================== positions

    named_pointer[name_of_this_element + "_x_position_implantation"] =
        Tnamed_pointer(
            &x_position_implantation, &x_position_implantation_ok, this) ;

    named_pointer[name_of_this_element + "_x_position_decay"] =
        Tnamed_pointer(&x_position_decay, &x_position_decay_ok, this) ;

    named_pointer[name_of_this_element + "_y_position_implantation"] =
        Tnamed_pointer(
            &y_position_implantation, &y_position_implantation_ok, this) ;

    named_pointer[name_of_this_element + "_y_position_decay"] =
        Tnamed_pointer(&y_position_decay, &y_position_decay_ok, this) ;

    // ----- thrsh

    named_pointer[name_of_this_element +
                  "_x_position_implantation_above_threshold"] =
                      Tnamed_pointer(
                          &x_position_implantation_thrsh,
                          &x_position_implantation_thrsh_ok, this) ;

    named_pointer[name_of_this_element +
                  "_x_position_decay_above_threshold"] =
                      Tnamed_pointer(
                          &x_position_decay_thrsh,
                          &x_position_decay_thrsh_ok, this) ;

    named_pointer[name_of_this_element +
                  "_y_position_implantation_above_threshold"] =
                      Tnamed_pointer(
                          &y_position_implantation_thrsh,
                          &y_position_implantation_thrsh_ok, this) ;

    named_pointer[name_of_this_element +
                  "_y_position_decay_above_threshold"] =
                      Tnamed_pointer(&y_position_decay_thrsh,
                                     &y_position_decay_thrsh_ok, this) ;

    named_pointer[name_of_this_element +
                  "_implantation_sum_energy_x_stripes"] =
                      Tnamed_pointer(&denominator_x_implantation, 0, this) ;

    named_pointer[name_of_this_element +
                  "_implantation_sum_energy_y_stripes"] =
                      Tnamed_pointer(&denominator_y_implantation, 0, this) ;


    named_pointer[name_of_this_element +
                  "_decay_sum_energy_x_stripes"] =
                      Tnamed_pointer(&denominator_x_decay, 0, this) ;

    named_pointer[name_of_this_element +
                  "_decay_sum_energy_y_stripes"] =
                      Tnamed_pointer(&denominator_y_decay, 0, this) ;

    named_pointer[name_of_this_element +
                  "_decay_sum_energy_both_xy_stripes"] =
                      Tnamed_pointer(&decay_sum_energy_both_xy_stripes,
                                     0, this) ;


    //---------------



#ifdef IF_SOMEBODY_WANTS

    for(unsigned int x = 0 ; x < NR_OF_STOPPER_STRIPES_X ; x++)
        for(unsigned int y = 0 ; y < NR_OF_STOPPER_STRIPES_Y ; y++)
        {
            std::ostringstream s;
            s
                    << owner->give_name()   // << "stopper" or "stopper2", etc.
                    << "_"
                    << symbol
                    << "_pixel_"
                    << setw(2) << setfill('0') << x << "_"
                    << setw(2) << y << "_time_since_last_implantation";

            //std::cout << "Name of incremeter >" << s.str() << "<" << endl;
            string name = s.str();

            named_pointer[name.c_str()] =
                Tnamed_pointer(&time_since_last_implantation[x][y],
                               &time_since_last_implantation_valid[x][y], this) ;
        }
#endif

    //--------- Incremeners of "found" implantation data
    // made for a sake of multi decay - when one of modules is in a different
    // geometric position so having more than one decay in the 3 modules stopper
    // has some sense

    // INCREMENTERS
    //---------
    named_pointer[name_of_this_element +
                  "_implantation_zet_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet, &flag_matching_is_valid, this) ;

    string entry_name;
    Tmap_of_named_pointers::iterator pos;

    // corresponding ALLs - local to this stopper and global to all stopperS
    entry_name =
        "ALL_stoppers_"
        "_implantation_zet_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet, &flag_matching_is_valid, this);
    //-----------


    named_pointer[name_of_this_element +
                  "_implantation_zet2_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet2, &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet2_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet2, &flag_matching_is_valid, this);


    // same as above but with 2nd decay-----------------------------

    named_pointer[name_of_this_element +
                  "_implantation_zet_matched_with_2nd_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet, &flag_ok_and_2nd_decay, this) ;


    // corresponding ALLs - local to this stopper and global to all stopperS

    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet, &flag_ok_and_2nd_decay, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet_matched_with_2nd_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet, &flag_ok_and_2nd_decay, this);
    //-----------


    named_pointer[name_of_this_element +
                  "_implantation_zet2_matched_with_2nd_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet2, &flag_ok_and_2nd_decay, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet2_matched_with_2nd_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet2, &flag_ok_and_2nd_decay, this);

    //##### same as above but music corrected by sci
    named_pointer[name_of_this_element +
                  "_implantation_zet_sci_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet_sci, &flag_matching_is_valid, this) ;


    // corresponding ALLs - local to this stopper and global to all stopperS
    entry_name =
        "ALL_stoppers_"
        "_implantation_zet_sci_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet_sci, &flag_matching_is_valid, this);
    //-----------


    named_pointer[name_of_this_element +
                  "_implantation_zet2_sci_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet2_sci, &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet2_sci_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet2_sci, &flag_matching_is_valid, this);


    // same as above but with 2nd decay-----------------------------

    named_pointer[name_of_this_element +
                  "_implantation_zet_sci_matched_with_2nd_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet_sci, &flag_ok_and_2nd_decay, this) ;


    // corresponding ALLs - local to this stopper and global to all stopperS
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet_sci, &flag_ok_and_2nd_decay, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet_sci_matched_with_2nd_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet_sci, &flag_ok_and_2nd_decay, this);
    //-----------


    named_pointer[name_of_this_element +
                  "_implantation_zet2_sci_matched_with_2nd_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet2_sci, &flag_ok_and_2nd_decay, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet2_sci_matched_with_2nd_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet2_sci, &flag_ok_and_2nd_decay, this);

    //+++++++++++++++++++++++++++++++++++++++

    named_pointer[name_of_this_element +
                  "_implantation_music_dE_(non_corr)_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_mus_dep_energy_non_corr,
                                     &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_music_dE_(non_corr)_matched_with_decay_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_mus_dep_energy_non_corr,
    //                              &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_music_dE_(non_corr)_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_mus_dep_energy_non_corr,
                               &flag_matching_is_valid, this);

    //-----------------

    named_pointer[name_of_this_element +
                  "_implantation_music2_dE_(non_corr)_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_mus2_dep_energy_non_corr,
                                     &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_music2_dE_(non_corr)_matched_with_decay_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_mus2_dep_energy_non_corr,
    //                              &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_music2_dE_(non_corr)_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_mus2_dep_energy_non_corr,
                               &flag_matching_is_valid, this);


    //-------------
    // for compatibility we need this incrementers

    // OLD double found_impl_aoq;                      // NEW:   found_impl_aoq_sci_mw_corrected

    named_pointer[name_of_this_element +
                  "_implantation_aoq_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_sci_mw_corrected, &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_sci_mw_corrected,
                               &flag_matching_is_valid, this);


    // ------------- 2nd decay -----------------
    named_pointer[name_of_this_element +
                  "_implantation_aoq_matched_with_2nd_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_sci_mw_corrected, &flag_ok_and_2nd_decay, this) ;


    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_matched_with_2nd_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_sci_mw_corrected,
                               &flag_ok_and_2nd_decay, this);


    //------------------
    //OLD  found_aoq_sci_corrected;         // NEW  found_impl_aoq_sci_sci_corrected

    named_pointer[name_of_this_element +
                  "_implantation_aoq_sci_corrected_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_sci_sci_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_sci_corrected_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_sci_sci_corrected,
                               &flag_matching_is_valid, this);




    //=============== New style AOQ

    // SCI MW
    named_pointer[name_of_this_element +
                  "_implantation_aoq_(sci_mw)_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_sci_mw_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_(sci_mw)_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_sci_mw_corrected,
                               &flag_matching_is_valid, this);



    // SCI SCI
    named_pointer[name_of_this_element +
                  "_implantation_aoq_(sci_sci_corrected)_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_sci_sci_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_(sci_sci_corrected)_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_sci_sci_corrected,
                               &flag_matching_is_valid, this);

    // SCI TPC


    named_pointer[name_of_this_element +
                  "_implantation_aoq_(sci_tpc)_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_sci_tpc_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_(sci_tpc)_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_sci_tpc_corrected,
                               &flag_matching_is_valid, this);

    // TIC SCI


    named_pointer[name_of_this_element +
                  "_implantation_aoq_(tic_sci)_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_tic_sci_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_(tic_sci)_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_tic_sci_corrected,
                               &flag_matching_is_valid, this);
    // TIC MW


    named_pointer[name_of_this_element +
                  "_implantation_aoq_(tic_mw)_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_tic_mw_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_(tic_mw)_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_tic_mw_corrected,
                               &flag_matching_is_valid, this);
    // TIC TPC


    named_pointer[name_of_this_element +
                  "_implantation_aoq_(tic_tpc)_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_tic_tpc_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_(tic_tpc)_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_tic_tpc_corrected,
                               &flag_matching_is_valid, this);


#ifdef  TPC22_PRESENT
    //------------------
    named_pointer[name_of_this_element +
                  "_implantation_aoq_(tpc_tpc_and_angle_at_s2tpc)_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_aoq_tpc_tpc_and_angle_at_s2tpc_corrected,
                                     &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_(tpc_tpc_and_angle_at_s2tpc)_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name,
                               &found_aoq_tpc_tpc_and_angle_at_s2tpc_corrected,
                               &flag_matching_is_valid, this);

#endif
    //  End of AOQ incrementers #######################################################



    // -------------

    named_pointer[name_of_this_element +
                  "_implantation_x_position_extrapolated_when_ok"] =
                      Tnamed_pointer(&found_impl_mw_x_position_extrapolated_to_stopper_position,
                                     &flag_matching_is_valid, this) ;

    //-----------------
    named_pointer[name_of_this_element +
                  "_implantation_y_position_extrapolated_when_ok"] =
                      Tnamed_pointer(
                          &found_impl_mw_y_position_extrapolated_to_stopper_position,
                          &flag_matching_is_valid, this) ;
    //----------------

    named_pointer[name_of_this_element +
                  "_implantation_sci42_energy_when_ok"] =
                      Tnamed_pointer(&found_impl_sci42_energy,
                                     &flag_matching_is_valid, this) ;



    named_pointer[name_of_this_element +
                  "_previous_decay_time_(for_same_implantation)_in_miliseconds"] =
                      Tnamed_pointer(&found_previous_decay_time_for_this_implantation_in_miliseconds,
                                     &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_sci42_energy_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_sci42_energy,
    //                              &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_sci42_energy_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_sci42_energy,
                               &flag_matching_is_valid, this);


    //-----------------
    named_pointer[name_of_this_element +
                  "_implantation_sci43_energy_when_ok"] =
                      Tnamed_pointer(&found_impl_sci43_energy,
                                     &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_sci43_energy_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_sci43_energy,
    //                              &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_sci43_energy_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_sci43_energy,
                               &flag_matching_is_valid, this);

    //-----------------
    // tof, sci21 pos
    named_pointer[name_of_this_element +
                  "_implantation_sci21_position_when_ok"] =
                      Tnamed_pointer(&found_impl_sci21_position,
                                     &flag_matching_is_valid, this) ;


    named_pointer[name_of_this_element +
                  "_implantation_sci21_left_when_ok"] =
                      Tnamed_pointer(&found_sci21_left,
                                     &flag_matching_is_valid, this) ;

    named_pointer[name_of_this_element +
                  "_implantation_sci21_right_when_ok"] =
                      Tnamed_pointer(&found_sci21_right,
                                     &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_sci21_position_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_sci21_position,
    //                              &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_sci21_position_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_sci21_position,
                               &flag_matching_is_valid, this);


    Tnamed_pointer::add_as_ALL("ALL_stoppers_"
                               "_implantation_sci21_left_when_ok",
                               &found_sci21_left,
                               &flag_matching_is_valid, this) ;
    Tnamed_pointer::add_as_ALL("ALL_stoppers_"
                               "_implantation_sci21_right_when_ok",
                               &found_sci21_right,
                               &flag_matching_is_valid, this) ;



    //----------------
    named_pointer[name_of_this_element +
                  "_implantation_tof_ps_when_ok"] =
                      Tnamed_pointer(&found_impl_tof_ps, &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_tof_ps_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_tof_ps,
    //                              &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_tof_ps_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_tof_ps,
                               &flag_matching_is_valid, this);

    //-----------------
    // #$%

    named_pointer[name_of_this_element +
                  "_implantation_sci41horiz_position_when_ok"] =
                      Tnamed_pointer(&found_sci41horiz_position, &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_sci41horiz_position_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_sci41horiz_position,
    //                              &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_sci41horiz_position_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_sci41horiz_position,
                               &flag_matching_is_valid, this);

    //---------------------
    named_pointer[name_of_this_element +
                  "_implantation_degrader_dE_gamma_when_ok"] =
                      Tnamed_pointer(&found_degrader_dE_gamma, &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_degrader_dE_gamma_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_degrader_dE_gamma,
    //                              &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_degrader_dE_gamma_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_degrader_dE_gamma,
                               &flag_matching_is_valid, this);

    //--------------------
    //int found_nr_of_times_retrieved;
    named_pointer[name_of_this_element +
                  "_nr_of_decays_when_ok"] =
                      Tnamed_pointer(&found_nr_of_times_retrieved,
                                     &flag_matching_is_valid, this) ;


#ifdef CHWILOWO_NIE

    named_pointer[name_of_this_element +
                  "_implantation_decay_time_difference_in_se"
                  "conds_when_successful_match"] =
                      Tnamed_pointer(&implantation_decay_time_difference_in_seconds,
                                     &flag_matching_is_valid, this) ;

    named_pointer[name_of_this_element +
                  "_implantation_decay_time_difference_in_1/10_of_seco"
                  "nds_when_successful_match"] =
                      Tnamed_pointer(
                          &implantation_decay_time_difference_in_one_tenth_of_seconds,
                          &flag_matching_is_valid, this) ;




    //-------------


    named_pointer[name_of_this_element
                  + "_x_position_implantation_above_threshold"] =
                      Tnamed_pointer(
                          &x_position_implantation_thrsh,
                          &x_position_implantation_thrsh_ok, this) ;

    named_pointer[name_of_this_element
                  + "_x_position_decay_above_threshold"] =
                      Tnamed_pointer(&x_position_decay_thrsh,
                                     &x_position_decay_thrsh_ok, this) ;

    named_pointer[name_of_this_element
                  + "_y_position_implantation_above_threshold"] =
                      Tnamed_pointer(&y_position_implantation_thrsh,
                                     &y_position_implantation_thrsh_ok, this) ;

    named_pointer[name_of_this_element
                  + "_y_position_decay_above_threshold"] =
                      Tnamed_pointer(&y_position_decay_thrsh,
                                     &y_position_decay_thrsh_ok, this) ;


    named_pointer[name_of_this_element
                  + "_decay_to_any_recent_implantation_time_difference_(in_ms)"] =
                      Tnamed_pointer(
                          &decay_to_any_recent_implantation_time_difference_in_ms,
                          &flag_trigger_of_decay, this) ;




    named_pointer[name_of_this_element
                  + "_multiplicity_of_x_strips_above_thrs_during_calculation_of_the_position"]
    =
        Tnamed_pointer(
            &multiplicity_of_x_strips_above_thrs_during_calculation_of_the_position,
            &flag_matching_is_valid, this) ;

    named_pointer[name_of_this_element +
                  "_multiplicity_of_y_strips_above_thrs_during_calculation_of_the_position"] =
                      Tnamed_pointer(&multiplicity_of_y_strips_above_thrs_during_calculation_of_the_position,
                                     &flag_matching_is_valid, this) ;

#endif



    named_pointer[name_of_this_element
                  + "_flag__implantation_was_successful"] =
                      Tnamed_pointer(
                          &flag_implantation_successful,
                          0, this) ;




    named_pointer[name_of_this_element +
                  "_implantation_decay_time_difference_in_se"
                  "conds_when_successful_match"] =
                      Tnamed_pointer(&implantation_decay_time_difference_in_seconds,
                                     &flag_matching_is_valid, this) ;

    named_pointer[name_of_this_element +
                  "_implantation_decay_time_difference_in_1/10_of_seco"
                  "nds_when_successful_match"] =
                      Tnamed_pointer(
                          &implantation_decay_time_difference_in_one_tenth_of_seconds,
                          &flag_matching_is_valid, this) ;

    named_pointer[name_of_this_element +
                  "implantation_decay_time_difference_in_miliseco"
                  "nds_when_successful_match"] =
                      Tnamed_pointer(
                          &implantation_decay_time_difference_in_miliseconds,
                          &flag_matching_is_valid, this) ;

    // cross matching between modules  #################################################
    named_pointer[name_of_this_element +
                  "_implantation_zet2_cross_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_cross_impl_zet2, &flag_cross_matching_is_valid, this) ;

    named_pointer[name_of_this_element +
                  "_implantation_zet_cross_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_cross_impl_zet, &flag_cross_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet2_cross_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_cross_impl_zet2, &flag_cross_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet_cross_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_cross_impl_zet, &flag_cross_matching_is_valid, this);

    //------------------

    named_pointer[name_of_this_element +
                  "_implantation_aoq_sci_corrected_cross_matched_when_ok"] =
                      Tnamed_pointer(&found_cross_impl_aoq_sci_sci_corrected,
                                     &flag_cross_matching_is_valid, this) ;

    named_pointer[name_of_this_element +
                  "_implantation_aoq_cross_matched_when_ok"] =
                      Tnamed_pointer(&found_cross_impl_aoq_sci_mw_corrected,
                                     &flag_cross_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS


    //double found_impl_aoq;                      // <---- found_impl_aoq_sci_mw_corrected
    //double found_aoq_sci_corrected;         // <---- found_impl_aoq_sci_sci_corrected


    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_sci_corrected_cross_matched_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_cross_impl_aoq_sci_sci_corrected,
                               &flag_cross_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_cross_matched_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_cross_impl_aoq_sci_mw_corrected,
                               &flag_cross_matching_is_valid, this);



    named_pointer[name_of_this_element
                  + "_flag__previous_implantation_never_decayed"] =
                      Tnamed_pointer(
                          &found_previous_implantation_never_decayed,
                          &flag_matching_is_valid, this) ;


    /*  named_pointer[name_of_this_element
                    +"__multiplicity_of_successful_matchings_(common_to_all_stoppers)"] =
                      Tnamed_pointer(
                        &multiplicity_of_successful_matchings,
                        0, this) ;
    */

    // check if id does not exist
//     char*  rows[] = {"stopper_", "stopper2_", "stopper3_"};
//     char*  which[] = {"_L", "_M", "_R"};
//
    string  rows[] = {"stopper_", "stopper2_", "stopper3_"};
    string  which[] = {"_L", "_M", "_R"};

    int my_nr = (owner->get_id_nr() * HOW_MANY_STOPPER_MODULES)   + module_id_nr;


    // for(int i = 0 ; i < 9 ; i++)
    {
        entry_name =  string("stoppers__retrieved_implantation_occured_(also)_in_")
                      + rows[owner->get_id_nr() ] + which[module_id_nr]

                      + "__when_decay_matched_ok" ;
        pos = named_pointer.find(entry_name);
        if(pos == named_pointer.end())     // if id does not exist yet, do it now
            named_pointer[entry_name] =
                Tnamed_pointer(&pattern_says_that_also_was_implanted_module[my_nr],
                               0,  // &flag_matching_is_valid,
                               this) ;
    }


    create_my_spectra();
}
// *********************************************************************
Tactive_stopper_combined_xy::~Tactive_stopper_combined_xy()
{
    save_spectra();

    for(unsigned int i = 0 ; i < x_stripe.size() ; i++)
    {
        delete x_stripe[i];
    }
    for(unsigned int i = 0 ; i < y_stripe.size() ; i++)
    {
        delete y_stripe[i];
    }
}
// ***********************************************************************
void Tactive_stopper_combined_xy::create_my_spectra()
{
    string name = name_of_this_element + "_fan"  ;
    string folder("stopper/");
    spec_fan =
        new spectrum_1D(name,
                        name,
                        1000, 0, 1000,
                        folder,
                        "0 - starts X stripes, 400 starts Y "
                        "stripes, (each stripe marks the triggers:"
                        "   *1- implantation, *2 - decay, "
                        "*3 -undefined");

    give_spectra_pointer()->push_back(spec_fan) ;


    name = name_of_this_element + "_fan_above_thresholds"  ;

    spec_fan_above_thresholds =
        new spectrum_1D(name,
                        name,
                        1000, 0, 1000,
                        folder,
                        "when above implantion and decay "
                        "thresholds [0 - starts X stripes, 400 "
                        "starts Y stripes, (triggers:   *1- "
                        "implantation, *2 - decay, *3 "
                        "-undefined]");

    give_spectra_pointer()->push_back(spec_fan_above_thresholds) ;


    //----------------- X -----------------------------------------------
    name = name_of_this_element + "_x_1st_max_energy_implantation_cal"  ;
    spec_x_1st_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Maximal value of energy");

    give_spectra_pointer()->push_back(spec_x_1st_energy_implantation_cal) ;

    //-----------------

    name = name_of_this_element + "_x_2nd_max_energy_implantation_cal"  ;
    spec_x_2nd_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Second maximal value of energy");

    give_spectra_pointer()->push_back(spec_x_2nd_energy_implantation_cal) ;
    //-----------------

    name = name_of_this_element + "_x_3rd_max_energy_implantation_cal"  ;
    spec_x_3rd_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Third maximal value of energy");

    give_spectra_pointer()->push_back(spec_x_3rd_energy_implantation_cal) ;


    //-----------------
    name = name_of_this_element + "_y_1st_max_energy_implantation_cal"  ;
    spec_y_1st_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Maximal value of energy");

    give_spectra_pointer()->push_back(spec_y_1st_energy_implantation_cal) ;

    //-----------------

    name = name_of_this_element + "_y_2nd_max_energy_implantation_cal"  ;
    spec_y_2nd_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Second maximal value of energy");

    give_spectra_pointer()->push_back(spec_y_2nd_energy_implantation_cal) ;
    //-----------------

    name = name_of_this_element + "_y_3rd_max_energy_implantation_cal"  ;
    spec_y_3rd_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Third maximal value of energy");

    give_spectra_pointer()->push_back(spec_y_3rd_energy_implantation_cal) ;

    // --------------- LINLOG --------------------
    name = name_of_this_element + "_x_1st_max_energy_decay_cal"  ;
    spec_x_1st_energy_decay_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Maximal value of energy");

    give_spectra_pointer()->push_back(spec_x_1st_energy_decay_cal) ;

    //-----------------

    name = name_of_this_element + "_x_2nd_max_energy_decay_cal"  ;
    spec_x_2nd_energy_decay_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Second maximal value of energy");

    give_spectra_pointer()->push_back(spec_x_2nd_energy_decay_cal) ;
    //-----------------

    name = name_of_this_element + "_x_3rd_max_energy_decay_cal"  ;
    spec_x_3rd_energy_decay_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Third maximal value of energy");

    give_spectra_pointer()->push_back(spec_x_3rd_energy_decay_cal) ;


    //-----------------
    name = name_of_this_element + "_y_1st_max_energy_decay_cal"  ;
    spec_y_1st_energy_decay_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Maximal value of energy");

    give_spectra_pointer()->push_back(spec_y_1st_energy_decay_cal) ;

    //-----------------

    name = name_of_this_element + "_y_2nd_max_energy_decay_cal"  ;
    spec_y_2nd_energy_decay_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Second maximal value of energy");

    give_spectra_pointer()->push_back(spec_y_2nd_energy_decay_cal) ;
    //-----------------

    name = name_of_this_element + "_y_3rd_max_energy_decay_cal"  ;
    spec_y_3rd_energy_decay_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Third maximal value of energy");

    give_spectra_pointer()->push_back(spec_y_3rd_energy_decay_cal) ;

    // multiplicity spectra ===================================
    // spectra multiplicity with threshold

    //--------------------------------- IMPLANT
    name = name_of_this_element +
           "_x_multiplicity_for_energy_cal_implantation_above_threshold";
    spec_x_multiplicity_for_energy_cal_implantation_above_threshold =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_X + 1, 0,
                        NR_OF_STOPPER_STRIPES_X + 1,
                        folder,
                        "x multiplicity_for_energy_cal_implan"
                        "tation_above_threshold",
                        name_of_this_element +
                        "_x_multiplicity_for_energy_cal_implan"
                        "tation_above_threshold");
    give_spectra_pointer()->push_back(
        spec_x_multiplicity_for_energy_cal_implantation_above_threshold);


    //---------------------------------
    name = name_of_this_element +
           "_y_multiplicity_for_energy_cal_implantation_above_threshold"  ;
    spec_y_multiplicity_for_energy_cal_implantation_above_threshold =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_Y + 1, 0,
                        NR_OF_STOPPER_STRIPES_Y + 1,
                        folder,
                        "y_multiplicity_for_energy_cal_implan"
                        "tation_above_threshold",
                        name_of_this_element +
                        "_y_multiplicity_for_energy_cal_implant"
                        "ation_above_threshold");
    give_spectra_pointer()->push_back(
        spec_y_multiplicity_for_energy_cal_implantation_above_threshold);


    //-----------------------DECAY

    name = name_of_this_element +
           "_x_multiplicity_for_energy_cal_decay_above_threshold"  ;
    spec_x_multiplicity_for_energy_cal_decay_above_threshold =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_X + 1, 0,
                        NR_OF_STOPPER_STRIPES_X + 1,
                        folder,
                        "x_multiplicity_for_energy_cal_decay_abo"
                        "ve_threshold",
                        name_of_this_element +
                        "_x_multiplicity_for_energy_cal_decay_abo"
                        "ve_threshold"
                       );
    give_spectra_pointer()->push_back(
        spec_x_multiplicity_for_energy_cal_decay_above_threshold);


    //---------------------------------
    name = name_of_this_element +
           "_y_multiplicity_for_energy_cal_decay_above_threshold"  ;
    spec_y_multiplicity_for_energy_cal_decay_above_threshold =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_Y + 1, 0,
                        NR_OF_STOPPER_STRIPES_Y + 1,
                        folder,
                        "y_multiplicity_for_energy_cal_decay_abo"
                        "ve_threshold",
                        name_of_this_element +
                        "_y_multiplicity_for_energy_cal_decay_abo"
                        "ve_threshold");

    give_spectra_pointer()->push_back(
        spec_y_multiplicity_for_energy_cal_decay_above_threshold);


    // spectra multiplicity without threshold

    //---------------------------------
    name = name_of_this_element + "_x_multiplicity_for_energy_cal"  ;
    spec_x_multiplicity_for_energy_cal =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_X + 1, 0,
                        NR_OF_STOPPER_STRIPES_X + 1,
                        folder,
                        "without threshold, (implantation, decay or"
                        " undefined trigger)",
                        name_of_this_element +
                        "_x_multiplicity_for_energy_cal");
    give_spectra_pointer()->push_back(
        spec_x_multiplicity_for_energy_cal);

    //---------------------------------
    name = name_of_this_element + "_y_multiplicity_for_energy_cal"  ;
    spec_y_multiplicity_for_energy_cal =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_Y + 1, 0,
                        NR_OF_STOPPER_STRIPES_Y + 1,
                        folder,
                        "without threshold, (implantation, decay or"
                        " undefined trigger)",
                        name_of_this_element +
                        "_y_multiplicity_for_energy_cal");
    give_spectra_pointer()->push_back(spec_y_multiplicity_for_energy_cal);


    //-------------- POSITIONS --------------------


    name = name_of_this_element + "_x_position_implantation"  ;
    spec_x_position_implantation =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_X * 10, 0,
                        NR_OF_STOPPER_STRIPES_X ,
                        folder,
                        "position from lin calibration (without threshold)",
                        name_of_this_element +
                        "_x_position_implantation");
    give_spectra_pointer()->push_back(spec_x_position_implantation);

    //-----------------------------
    name = name_of_this_element + "_x_position_decay"  ;
    spec_x_position_decay =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_X * 10, 0,
                        NR_OF_STOPPER_STRIPES_X ,
                        folder,
                        "position from decay calibration (wi"
                        "thout threshold)",
                        name_of_this_element + "_x_position_decay");
    give_spectra_pointer()->push_back(spec_x_position_decay);

    //--------------------------------
    name = name_of_this_element + "_y_position_implantation"  ;
    spec_y_position_implantation =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_Y * 10, 0,
                        NR_OF_STOPPER_STRIPES_Y ,
                        folder,
                        "position from lin calibration (without threshold)",
                        name_of_this_element + "_y_position_implantation");

    give_spectra_pointer()->push_back(spec_y_position_implantation);

    //--------------------------------

    name = name_of_this_element + "_y_position_decay"  ;
    spec_y_position_decay =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_Y * 10, 0,
                        NR_OF_STOPPER_STRIPES_Y ,
                        folder,
                        "position from decay calibration (wit"
                        "hout threshold)",
                        name_of_this_element + "_y_position_decay");
    give_spectra_pointer()->push_back(spec_y_position_decay);

    //================== with Threshold ===================
    //-------------- POSITIONS --------------------

    name = name_of_this_element + "_x_position_implantation_thrsh"  ;
    spec_x_position_implantation_thrsh =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_X * 10, 0,
                        NR_OF_STOPPER_STRIPES_X ,
                        folder,
                        "position from lin calibration (wit"
                        "h threshold)",
                        name_of_this_element +
                        "_x_position_implantation_above_threshold");
    give_spectra_pointer()->push_back(spec_x_position_implantation_thrsh);

    //-----------------------------------

    name = name_of_this_element + "_x_position_decay_thrsh"  ;
    spec_x_position_decay_thrsh =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_X * 10, 0,
                        NR_OF_STOPPER_STRIPES_X ,
                        folder,
                        "position from decay calibration (with threshold)",
                        name_of_this_element +
                        "_x_position_decay_above_threshold");
    give_spectra_pointer()->push_back(spec_x_position_decay_thrsh);

    //--------------------------------
    name = name_of_this_element + "_y_position_implantation_thrsh"  ;
    spec_y_position_implantation_thrsh =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_Y * 10, 0,
                        NR_OF_STOPPER_STRIPES_Y ,
                        folder,
                        "position from lin calibration (with threshold)",
                        name_of_this_element +
                        "_y_position_implantation_above_threshold");
    give_spectra_pointer()->push_back(spec_y_position_implantation_thrsh);

    //--------------------------------

    name = name_of_this_element + "_y_position_decay_thrsh"  ;
    spec_y_position_decay_thrsh =
        new spectrum_1D(name,  name,
                        NR_OF_STOPPER_STRIPES_Y * 10, 0,
                        NR_OF_STOPPER_STRIPES_Y,
                        folder,
                        "position from decay calibration (with threshold)",
                        name_of_this_element +
                        "_y_position_decay_above_threshold");
    give_spectra_pointer()->push_back(spec_y_position_decay_thrsh);

    //----------------------------------- 2D -----------------;

    name = name_of_this_element + "_map_of_implantations_thrsh"  ;
    spec_map_of_implantations = new spectrum_2D(name,
            name,
            NR_OF_STOPPER_STRIPES_X,
            0, NR_OF_STOPPER_STRIPES_X,
            NR_OF_STOPPER_STRIPES_Y,
            0, NR_OF_STOPPER_STRIPES_Y,
            folder, "",
            string("as X:") +
            name_of_this_element +
            "_put_something here" +
            ", as Y: " + name_of_this_element +
            " put something here");

    give_spectra_pointer()->push_back(spec_map_of_implantations) ;

    //----------------
    name = name_of_this_element + "_map_of_decays_thrsh"  ;
    spec_map_of_decays =
        new spectrum_2D(name,
                        name,
                        NR_OF_STOPPER_STRIPES_X,
                        0, NR_OF_STOPPER_STRIPES_X,
                        NR_OF_STOPPER_STRIPES_Y,
                        0, NR_OF_STOPPER_STRIPES_Y,
                        folder, "",
                        string("as X:") +
                        name_of_this_element + "_put_something here" +
                        ", as Y: " + name_of_this_element +
                        " put something here");

    give_spectra_pointer()->push_back(spec_map_of_decays) ;



    name = name_of_this_element + "_map_of_decays_without_thrsh"  ;
    spec_map_of_decays_without_thrsh =
        new spectrum_2D(name,
                        name,
                        NR_OF_STOPPER_STRIPES_X, 0,
                        NR_OF_STOPPER_STRIPES_X,
                        NR_OF_STOPPER_STRIPES_Y, 0,
                        NR_OF_STOPPER_STRIPES_Y,
                        folder, "",
                        string("as X:") +
                        name_of_this_element + "_put_something here" +
                        ", as Y: " + name_of_this_element +
                        " put something here");

    give_spectra_pointer()->push_back(spec_map_of_decays_without_thrsh) ;


    //-------------------
    name = name_of_this_element +
           "_map_of_successful_matching_implantations_with_decays_thrsh"  ;
    spec_map_of_matching =
        new spectrum_2D(name,
                        name,
                        NR_OF_STOPPER_STRIPES_X, 0,
                        NR_OF_STOPPER_STRIPES_X,
                        NR_OF_STOPPER_STRIPES_Y, 0,
                        NR_OF_STOPPER_STRIPES_Y,
                        folder, "",
                        string("as X:") + name_of_this_element +
                        "_put_something here" +
                        ", as Y: " + name_of_this_element +
                        " put something here");

    give_spectra_pointer()->push_back(spec_map_of_matching) ;


#ifdef NIGDY

    // sum energies of stripes
    name = name_of_this_element + "_x_sum_energy_impl"  ;
    spec_x_sum_energy_impl =
        new spectrum_1D(name,  name,
                        6000, 0, 60000,
                        folder,
                        "",
                        name_of_this_element + "");
    give_spectra_pointer()->push_back(spec_x_sum_energy_impl);

    //-----------------------------
    name = name_of_this_element + "_y_sum_energy_impl"  ;
    spec_y_sum_energy_impl =
        new spectrum_1D(name,  name,
                        6000, 0, 60000,
                        folder,
                        "",
                        name_of_this_element + "");
    give_spectra_pointer()->push_back(spec_y_sum_energy_impl);

    //===================
    name = name_of_this_element + "_xy_sum_energy_impl"  ;
    spec_xy_sum_energy_impl = new spectrum_2D(name,
            name,
            900, 0, 50000,
            900, 0, 50000,
            folder, "",
            string("as X:") + name_of_this_element +
            "_put_something here" +
            ", as Y: " + name_of_this_element +
            " put something here");

    give_spectra_pointer()->push_back(spec_xy_sum_energy_impl) ;

    //------------- when it was possible to match this
    name = name_of_this_element + "_xy_sum_energy_impl_matched"  ;
    spec_xy_sum_energy_impl_matched =
        new spectrum_2D(name,
                        name,
                        900, 0, 50000,
                        900, 0, 50000,
                        folder, "",
                        string("as X:") +
                        name_of_this_element +
                        "_put_something here" +
                        ", as Y: " +
                        name_of_this_element +
                        " put something here");

    give_spectra_pointer()->push_back(spec_xy_sum_energy_impl_matched) ;
#endif  // NIGDY

    if(spec_map_of_cross_matching == NULL)
    {
        int roz = 6 ; //  size of module_as_one_pixel is stil growing during the constructors call
        name = // name_of_this_element +
            "stoppers_all_map_of_successful_cross_matching"  ;
        spec_map_of_cross_matching =
            new spectrum_2D(name,
                            name,
                            roz, 0, roz,
                            roz, 0, roz,
                            folder, "",
                            string("as X:") + name_of_this_element +
                            "_put_something here" +
                            ", as Y: " + name_of_this_element +
                            " put something here");

        give_spectra_pointer()->push_back(spec_map_of_cross_matching) ;
    }

}
//*******************************************************************
void Tactive_stopper_combined_xy::analyse_current_event()
{
    //   if(RisingAnalysis_ptr->is_verbose_on() )
    //   cout <<
    //   "Tactive_stopper_combined_xy::analyse_current_event()  -------------"
    //   << name_of_this_element
    //   << endl;

    // incrementers
    memset(&time_since_last_implantation[0][0], 0,
           sizeof(time_since_last_implantation));
    memset(&time_since_last_implantation_valid[0][0], 0,
           sizeof(time_since_last_implantation_valid));

    strip_x_energies_implantation_cal.clear();
    strip_y_energies_implantation_cal.clear();
    strip_x_energies_decay_cal.clear();
    strip_y_energies_decay_cal.clear();

    x_1st_energy_implantation_cal =
        x_2nd_energy_implantation_cal =
            x_3rd_energy_implantation_cal =
                y_1st_energy_implantation_cal =
                    y_2nd_energy_implantation_cal =
                        y_3rd_energy_implantation_cal =
                            x_multiplicity_for_energy_cal_implantation_above_threshold
                            =
                                y_multiplicity_for_energy_cal_implantation_above_threshold
                                =
                                    x_multiplicity_for_energy_cal_decay_above_threshold  =
                                        y_multiplicity_for_energy_cal_decay_above_threshold =
                                            x_multiplicity_for_energy_cal =
                                                    y_multiplicity_for_energy_cal =   0 ;

    x_position_implantation =
        y_position_implantation =
            x_position_implantation_thrsh =
                y_position_implantation_thrsh =

                    x_position_decay =
                        y_position_decay =
                            x_position_decay_thrsh =
                                y_position_decay_thrsh = -9999;

    flag_implantation_successful = false;
    flag_cross_matching_is_valid = false;
    int channel = 3 ;
    if(Tactive_stopper_full::active_stopper_event_type ==
            Tactive_stopper_full::implantation)
        channel = 1 ;
    else if(Tactive_stopper_full::active_stopper_event_type ==
            Tactive_stopper_full::decay)
        channel = 2;


    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_X ; i++)
    {

        /*  cout << i << ")  x_multiplicity_for_energy_cal_decay_above_threshold = "
            << x_multiplicity_for_energy_cal_decay_above_threshold
            << endl; */
#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
        //     cout << name_of_this_element << i
        //     << " module combined  fired, for  = "
        //     << owner->give_event_fired(module_id_nr, 0)
        //     << ", energy data = " <<     owner->give_event_data(module_id_nr, 0, i)
        //     << ", time data = " <<     owner->give_event_time_data(module_id_nr, 0, i)
        //     << endl;
#endif

        if(owner->give_event_fired(module_id_nr, 0) &&
                (
                    owner->give_event_data(module_id_nr, 0, i)
#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
                    ||    owner->give_event_time_data(module_id_nr, 0, i)
#endif
                ))
        {
            x_stripe[i]->analyse_current_event();
            spec_fan->manually_increment((int) i * 10 + 0 + channel);


            // spectra fun above threshold------------------------------------
            if(
                (channel == 1  // in case of implantation
                 &&
                 Tactive_stopper_full::is_energy_cal_implantation_in_gate(
                     x_stripe[i]->give_energy_implantation_cal()))
                ||
                (channel == 2 // decay
                 &&
                 Tactive_stopper_full::is_energy_cal_decay_in_gate(x_stripe[i]->give_energy_decay_cal()))
            )
            {
                spec_fan_above_thresholds->manually_increment(
                    (int) i * 10 + 0 + channel);
            }
            //-------------------------------------------------------------
        }
        else
            x_stripe[i]->mark_not_fired();
    }

    //============== Y =====================
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_Y ; i++)
    {
        if(owner->give_event_fired(module_id_nr, 1) &&
                (
                    owner->give_event_data(module_id_nr, 1, i)
#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
                    ||    owner->give_event_time_data(module_id_nr, 1, i)
#endif
                ))
        {
            y_stripe[i]->analyse_current_event();
            spec_fan->manually_increment((int) i * 10 + 400 + channel);

            // spectra fun above threshold ----------------------------------
            if(
                (channel == 1  // in case of implantation
                 &&
                 Tactive_stopper_full::
                 is_energy_cal_implantation_in_gate(y_stripe[i]->give_energy_implantation_cal()))
                ||
                (channel == 2 // decay
                 &&
                 Tactive_stopper_full::is_energy_cal_decay_in_gate(y_stripe[i]->give_energy_decay_cal()))
            )
            {
                spec_fan_above_thresholds->manually_increment(
                    (int) i * 10 + 400 + channel);
            }
            //---------------------------------------------------------------
        }
        else
            y_stripe[i]->mark_not_fired();

    } // end for analysis of all the stripes


    if(Tactive_stopper_full::active_stopper_event_type ==
            Tactive_stopper_full::implantation)
    {

        // ====================== Highest Energies spectra ===============
        // sorting the energy results
        sort(strip_x_energies_implantation_cal.begin(),
             strip_x_energies_implantation_cal.end());
        sort(strip_y_energies_implantation_cal.begin(),
             strip_y_energies_implantation_cal.end());

        // incrementing two spectra with the highest energies
        if(strip_x_energies_implantation_cal.size())
        {
            x_1st_energy_implantation_cal =
                strip_x_energies_implantation_cal[
                    strip_x_energies_implantation_cal.size() - 1];

            spec_x_1st_energy_implantation_cal->
            manually_increment(x_1st_energy_implantation_cal);
            if(RisingAnalysis_ptr->is_verbose_on())
                cout << "x_1st_energy_implantation_cal = "
                     << x_1st_energy_implantation_cal << endl;
        }

        if(strip_x_energies_implantation_cal.size() >= 2)
        {
            x_2nd_energy_implantation_cal =
                strip_x_energies_implantation_cal[
                    strip_x_energies_implantation_cal.size() - 2];
            spec_x_2nd_energy_implantation_cal->
            manually_increment(x_2nd_energy_implantation_cal);
        }

        if(strip_x_energies_implantation_cal.size() >= 3)
        {
            x_3rd_energy_implantation_cal =
                strip_x_energies_implantation_cal[
                    strip_x_energies_implantation_cal.size() - 3];
            spec_x_3rd_energy_implantation_cal->
            manually_increment(x_3rd_energy_implantation_cal);
        }
        //============ Y ===============
        // incrementing two spectra with the highest energies
        if(strip_y_energies_implantation_cal.size())
        {
            y_1st_energy_implantation_cal =
                strip_y_energies_implantation_cal[
                    strip_y_energies_implantation_cal.size() - 1];
            spec_y_1st_energy_implantation_cal->
            manually_increment(y_1st_energy_implantation_cal);
            if(RisingAnalysis_ptr->is_verbose_on())
                cout <<
                     "y_1st_energy_implantation_cal = " <<
                     y_1st_energy_implantation_cal << endl;
        }

        if(strip_y_energies_implantation_cal.size() >= 2)
        {
            y_2nd_energy_implantation_cal =
                strip_y_energies_implantation_cal[
                    strip_y_energies_implantation_cal.size() - 2];
            spec_y_2nd_energy_implantation_cal->
            manually_increment(y_2nd_energy_implantation_cal);
        }

        if(strip_y_energies_implantation_cal.size() >= 3)
        {
            y_3rd_energy_implantation_cal =
                strip_y_energies_implantation_cal[
                    strip_y_energies_implantation_cal.size() - 3];
            spec_y_3rd_energy_implantation_cal->
            manually_increment(y_3rd_energy_implantation_cal);
        }
        // incrementing multiplicity spectra
        // spectra multiplicity with threshold
        spec_x_multiplicity_for_energy_cal_implantation_above_threshold->
        manually_increment(
            x_multiplicity_for_energy_cal_implantation_above_threshold);
        spec_y_multiplicity_for_energy_cal_implantation_above_threshold->
        manually_increment(
            y_multiplicity_for_energy_cal_implantation_above_threshold);
    } // end of if implantation


    //##########################################
    // ---- same for decay
    if(Tactive_stopper_full::active_stopper_event_type ==
            Tactive_stopper_full::decay)
    {
        sort(strip_x_energies_decay_cal.begin(),
             strip_x_energies_decay_cal.end());
        sort(strip_y_energies_decay_cal.begin(),
             strip_y_energies_decay_cal.end());

        // incrementing two spectra with the highest energies
        if(strip_x_energies_decay_cal.size())
        {
            x_1st_energy_decay_cal =
                strip_x_energies_decay_cal[strip_x_energies_decay_cal.size() - 1];
            spec_x_1st_energy_decay_cal->
            manually_increment(x_1st_energy_decay_cal);
        }

        if(strip_x_energies_decay_cal.size() >= 2)
        {
            x_2nd_energy_decay_cal =
                strip_x_energies_decay_cal[strip_x_energies_decay_cal.size() - 2];
            spec_x_2nd_energy_decay_cal->
            manually_increment(x_2nd_energy_decay_cal);
        }

        if(strip_x_energies_decay_cal.size() >= 3)
        {
            x_3rd_energy_decay_cal =
                strip_x_energies_decay_cal[strip_x_energies_decay_cal.size() - 3];
            spec_x_3rd_energy_decay_cal->
            manually_increment(x_3rd_energy_decay_cal);
        }
        // ---------------- Y ----------------
        // incrementing two spectra with the highest energies
        if(strip_y_energies_decay_cal.size())
        {
            y_1st_energy_decay_cal =
                strip_y_energies_decay_cal[strip_y_energies_decay_cal.size() - 1];
            spec_y_1st_energy_decay_cal->
            manually_increment(y_1st_energy_decay_cal);
        }

        if(strip_y_energies_decay_cal.size() >= 2)
        {
            y_2nd_energy_decay_cal =
                strip_y_energies_decay_cal[strip_y_energies_decay_cal.size() - 2];
            spec_y_2nd_energy_decay_cal->
            manually_increment(y_2nd_energy_decay_cal);
        }

        if(strip_y_energies_decay_cal.size() >= 3)
        {
            y_3rd_energy_decay_cal =
                strip_y_energies_decay_cal[
                    strip_y_energies_decay_cal.size() - 3];
            spec_y_3rd_energy_decay_cal->
            manually_increment(y_3rd_energy_decay_cal);
        }

        // cout << name_of_this_element
        //<< "x_multiplicity_for_energy_cal_decay_above_threshold "
        // << x_multiplicity_for_energy_cal_decay_above_threshold
        // << endl;

        spec_x_multiplicity_for_energy_cal_decay_above_threshold->
        manually_increment(
            x_multiplicity_for_energy_cal_decay_above_threshold);

        //    spec_x_multiplicity_for_energy_cal_decay_above_threshold->
        //     manually_increment(5);


        spec_y_multiplicity_for_energy_cal_decay_above_threshold->
        manually_increment(
            y_multiplicity_for_energy_cal_decay_above_threshold);

    } // end of decay


    // spectra multiplicity without threshold
    spec_x_multiplicity_for_energy_cal->
    manually_increment(x_multiplicity_for_energy_cal);
    spec_y_multiplicity_for_energy_cal->
    manually_increment(y_multiplicity_for_energy_cal);


    //   cout << "in this event the y mult was = "
    // << y_multiplicity_for_energy_cal << endl;


    switch(Tactive_stopper_full::active_stopper_event_type)
    {
    case Tactive_stopper_full::implantation:
        calculate_position_and_RMS_for_implantation();
        if(x_position_implantation_thrsh_ok ||
                y_position_implantation_thrsh_ok)
            owner->
            incr_impl_multiplicity_above_thrsh();
        //implantation_action();  <-- now this is called form the Tactive_stopper_full::analyse_current_event()
        break;

    case Tactive_stopper_full::decay:
        calculate_position_and_RMS_for_decay();
        if(x_position_decay_thrsh_ok || y_position_decay_thrsh_ok)
            owner->
            incr_decay_multiplicity_above_thrsh();
        // decay_action();      <-- now this is called form the Tactive_stopper_full::analyse_current_event()
        break;

    case Tactive_stopper_full::undefined:
    default:
        break; // nothing to do
    }

    calculations_already_done = true ;
}
// **********************************************************************
void Tactive_stopper_combined_xy::make_preloop_action(ifstream & plik)   // read the calibration factors, gates
{
    //cout
    //<< "Tactive_stopper_combined_xy::make_preloop_action(ifstream & plik)"
    //<< endl;


    string opt_filename = "options/active_stopper_settings.txt";
    try
    {
        ifstream plik(opt_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            string mess = "I can't open file: "
                          + opt_filename  ;
            throw mess ;
        }

        //char znak[] = { 'L', 'M', 'R' };

        /*    std::ostringstream s;
        s << znak[module_id_nr] ;
        string symbol_of_this_plate = s.str();*/
        string symbol_of_this_plate = symbol;
        // radio buttons for XY information to be used
        //    cout << "symbol = >" << symbol << "<, symbol_of_plate = >" << symbol_of_this_plate << "<" << endl;
        bool radio_xy_both  = (bool) Tfile_helper::find_in_file(
                                  plik, symbol_of_this_plate + "_xy_both");
        bool radio_x_only = (bool) Tfile_helper::find_in_file(
                                plik, symbol_of_this_plate + "_x_only");
        bool radio_y_only = (bool) Tfile_helper::find_in_file(
                                plik, symbol_of_this_plate + "_y_only");
        // changing this into the enum type
        if(radio_x_only)
            how_to_match_position = only_x;
        else if(radio_y_only)
            how_to_match_position = only_y;
        else if(radio_xy_both)
            how_to_match_position = xy_both;
        else
            how_to_match_position = xy_both; // default situation
    }
    catch(Tfile_helper_exception &k)
    {
        cerr << "Error while reading options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1) ; // is it healthy ?
    }


    zero_flags_fired_vectors();

    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_X ; i++)
    {
        //cout << "reading calibration loop for active stopper segment"
        //<< i << endl ;
        x_stripe[i]->make_preloop_action(plik) ;
    }
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_Y ; i++)
    {
        y_stripe[i]->make_preloop_action(plik) ;
    }
    //  cout << "Active stopper  calibration successfully read"  << endl;

}
//*********************************************************************
/*!
\fn Tactive_stopper_combined_xy::
zero_flags_fired_vectors()
*/
void Tactive_stopper_combined_xy::zero_flags_fired_vectors()
{
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_X ; i++)
        x_stripe[i]->mark_not_fired();

    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_Y ; i++)
        y_stripe[i]->mark_not_fired();


    memset(&time_since_last_implantation[0][0], 0,
           sizeof(time_since_last_implantation));
    memset(&time_since_last_implantation_valid[0][0], 0,
           sizeof(time_since_last_implantation_valid));
    decay_sum_energy_both_xy_stripes = denominator_x_decay =
                                           denominator_y_decay = 0.0;
    denominator_x_implantation = denominator_y_implantation = 0;

    flag_implantation_successful = false;
}
//********************************************************************
/*!
\fn Tactive_stopper_combined_xy::
calculate_position_and_RMS_for_implantation()
*/
void Tactive_stopper_combined_xy::calculate_position_and_RMS_for_implantation()
{
    // mianownik = denominator
    // licznik = numerator

    //double
    denominator_x_implantation = 0;  // made a memeber of the class
    double numerator_x_implantation  = 0;
    double numerator_x2_implantation  = 0;

    // double
    denominator_y_implantation = 0;  // made a memeber of the class

    double numerator_y_implantation  = 0;
    double numerator_y2_implantation  = 0;

    double denominator_x_implantation_thrsh = 0;
    double numerator_x_implantation_thrsh  = 0;
    double numerator_x2_implantation_thrsh  = 0;

    double denominator_y_implantation_thrsh = 0;
    double numerator_y_implantation_thrsh  = 0;
    double numerator_y2_implantation_thrsh  = 0;



    rms_x_implantation =
        rms_x_decay =
            rms_y_implantation =
                rms_y_decay =
                    rms_x_implantation_thrsh =
                        rms_x_decay_thrsh =
                            rms_y_implantation_thrsh =
                                rms_y_decay_thrsh = 0;


    double en = 0;


    int nr_x_max = 0;
    int nr_y_max = 0;
    double max_en_x = 0, max_en_y = 0 ;

    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_X ; i++)
    {
        if(owner->give_event_fired(module_id_nr, 0))
        {
            en =  x_stripe[i]->give_energy_implantation_cal();
            numerator_x_implantation += i  * en;
            denominator_x_implantation += en;


            // threshold
            en = x_stripe[i]->give_energy_implantation_cal();
            if(RisingAnalysis_ptr->is_verbose_on())
                cout << "stripe X " << i << " had implantation  energy  " << en
                     << (
                         Tactive_stopper_full::
                         is_energy_cal_implantation_in_gate(en)
                         ?  " <--- in gate " :
                         "")
                     << endl;

            if(Tactive_stopper_full::is_energy_cal_implantation_in_gate(en))
            {
                numerator_x_implantation_thrsh += i  * en;
                denominator_x_implantation_thrsh += en;
                if(max_en_x  < en)
                {
                    nr_x_max = i ;
                    max_en_x = en;
                }

            }
        }
    }
    // -------- Y ---------------------------------------------
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_Y ; i++)
    {
        if(owner->give_event_fired(module_id_nr, 1))
        {
            en = y_stripe[i]->give_energy_implantation_cal();
            numerator_y_implantation += i  * en;
            denominator_y_implantation += en;

            en = y_stripe[i]->give_energy_implantation_cal();
            if(RisingAnalysis_ptr->is_verbose_on())
                cout << "stripe Y " << i
                     << " had implantation energy               " << en
                     << (
                         Tactive_stopper_full::
                         is_energy_cal_implantation_in_gate(en)
                         ?
                         " <--- in gate " :
                         "") << endl;


            if(Tactive_stopper_full::
                    is_energy_cal_implantation_in_gate(en))
            {
                numerator_y_implantation_thrsh += i * en ;
                denominator_y_implantation_thrsh += en;
                if(max_en_y  < en)
                {
                    nr_y_max = i ;
                    max_en_y = en;
                }
            }
        }
    } // end for

    if(RisingAnalysis_ptr->is_verbose_on())
    {
        if(max_en_x)
            cout << "Max en X " << max_en_x
                 << " was form X strip nr " <<  nr_x_max << endl;
        if(max_en_y)
            cout << "Max en Y " << max_en_y
                 << " was form Y strip nr " <<  nr_y_max << endl;


        cout << " sum energy of X stripes =" << denominator_x_implantation
             << " sum energy of Y stripes =" << denominator_y_implantation
             << endl;
    }
#ifdef NIDGY
    spec_x_sum_energy_impl->manually_increment(denominator_x_implantation);
    spec_y_sum_energy_impl->manually_increment(denominator_y_implantation);
    spec_xy_sum_energy_impl->manually_increment
    (denominator_x_implantation, denominator_y_implantation);
#endif // NIGDY


    //-----------------------
    if(denominator_x_implantation)
    {
        x_position_implantation =
            numerator_x_implantation / denominator_x_implantation;
        spec_x_position_implantation->
        manually_increment(x_position_implantation);

        x_position_implantation_ok  = true;
    }
    else
        x_position_implantation_ok  = false;

    if(denominator_y_implantation)
    {
        y_position_implantation =
            numerator_y_implantation / denominator_y_implantation;
        spec_y_position_implantation->
        manually_increment(y_position_implantation);

        y_position_implantation_ok  = true;
    }
    else
        y_position_implantation_ok  = false;



    // threshold==========================================
    if(denominator_x_implantation_thrsh)
    {
        x_position_implantation_thrsh =
            numerator_x_implantation_thrsh / denominator_x_implantation_thrsh;
        spec_x_position_implantation_thrsh->
        manually_increment(x_position_implantation_thrsh);
        if(RisingAnalysis_ptr->is_verbose_on())
            cout <<
                 "So calcultated X position = " << x_position_implantation_thrsh
                 << endl;
        x_position_implantation_thrsh_ok  = true;
    }
    else
        x_position_implantation_thrsh_ok  = false;

    if(denominator_y_implantation_thrsh)
    {
        y_position_implantation_thrsh =
            numerator_y_implantation_thrsh / denominator_y_implantation_thrsh;
        spec_y_position_implantation_thrsh->
        manually_increment(y_position_implantation_thrsh);
        if(RisingAnalysis_ptr->is_verbose_on())
            cout <<
                 "So calcultated Y positon = " << y_position_implantation_thrsh
                 << endl;
        y_position_implantation_thrsh_ok  = true;
    }
    else
        y_position_implantation_thrsh_ok  = false;


    // ------- RMS ------------------------------------------
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_X ; i++)
    {

        if(x_stripe[i]->give_fired())
        {
            numerator_x2_implantation +=
                pow((x_position_implantation  - i), 2)  *
                x_stripe[i]->give_energy_implantation_cal();

            if(
                Tactive_stopper_full::is_energy_cal_implantation_in_gate(
                    x_stripe[i]->give_energy_implantation_cal()))
            {
                numerator_x2_implantation_thrsh +=
                    pow((x_position_implantation_thrsh  - i), 2)  *
                    x_stripe[i]->give_energy_implantation_cal();
            }
        }
    }
    // -------- Y
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_Y ; i++)
    {

        if(y_stripe[i]->give_fired())
        {
            numerator_y2_implantation +=
                pow((y_position_implantation  - i), 2)  *
                y_stripe[i]->give_energy_implantation_cal();

            if(
                Tactive_stopper_full::is_energy_cal_implantation_in_gate(
                    y_stripe[i]->give_energy_implantation_cal()))
            {
                numerator_y2_implantation_thrsh +=
                    pow((y_position_implantation_thrsh  - i), 2)  *
                    y_stripe[i]->give_energy_implantation_cal();
            }
        }
    } // end for


    if(denominator_x_implantation)
        rms_x_implantation =
            sqrt(numerator_x2_implantation / denominator_x_implantation);

    if(denominator_y_implantation)
        rms_y_implantation =
            sqrt(numerator_y2_implantation / denominator_y_implantation);

    if(denominator_x_implantation_thrsh)
        rms_x_implantation_thrsh =
            sqrt(numerator_x2_implantation_thrsh /
                 denominator_x_implantation_thrsh);

    if(denominator_y_implantation_thrsh)
        rms_y_implantation_thrsh =
            sqrt(numerator_y2_implantation_thrsh /
                 denominator_y_implantation_thrsh);

    // Par force - another way of calculating the position


    if(owner->
            give_flag_algorithm_for_implantation_position_calculation_is_mean()
            == false)
    {
        if(max_en_x)
        {
            x_position_implantation_thrsh = nr_x_max;
            x_position_implantation_thrsh_ok  = true;
        }
        else
            x_position_implantation_thrsh_ok  = false;

        if(max_en_y)
        {
            y_position_implantation_thrsh = nr_y_max;
            y_position_implantation_thrsh_ok  = true;
        }
        else
            y_position_implantation_thrsh_ok  = false;

    }
}

//**********************************************************************
/*!
\fn Tactive_stopper_combined_xy::
calculate_position_and_RMS_for_decay()
*/
void Tactive_stopper_combined_xy::calculate_position_and_RMS_for_decay()
{
    // mianownik = denominator
    // licznik = numerator


    /*    double decay_sum_energy_x_strips;
    double decay_sum_energy_y_strips;
    double decay_sum_energy_both_xy_strips;
    */

    denominator_x_decay = 0;  // sum energy
    double numerator_x_decay  = 0;
    double numerator_x2_decay  = 0;

    denominator_y_decay = 0;   // sum energy
    double numerator_y_decay  = 0;
    double numerator_y2_decay  = 0;

    double denominator_x_decay_thrsh = 0;
    double numerator_x_decay_thrsh  = 0;
    double numerator_x2_decay_thrsh  = 0;

    double denominator_y_decay_thrsh = 0;
    double numerator_y_decay_thrsh  = 0;
    double numerator_y2_decay_thrsh  = 0;


    rms_x_implantation =
        rms_x_decay =
            rms_y_implantation =
                rms_y_decay =
                    rms_x_implantation_thrsh =
                        rms_x_decay_thrsh =
                            rms_y_implantation_thrsh =
                                rms_y_decay_thrsh = 0;



    double en = 0;

    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_X ; i++)
    {
        if(owner->give_event_fired(module_id_nr, 0))
        {
            en = x_stripe[i]->give_energy_decay_cal();
            numerator_x_decay += i * en;
            denominator_x_decay += en ;

            // threshold
            // en = x_stripe[i]->give_energy_decay_cal();
            if(Tactive_stopper_full::is_energy_cal_decay_in_gate(en))
            {
                numerator_x_decay_thrsh += i  * en;
                denominator_x_decay_thrsh += en;
                if(RisingAnalysis_ptr->is_verbose_on())
                    cout << name_of_this_element
                         << " stripe x " << i << " had decay energy  " << en << endl;
            }
        }
    }
    // -------- Y ---------------------------------------------
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_Y ; i++)
    {
        if(owner->give_event_fired(module_id_nr, 1))
        {
            en = y_stripe[i]->give_energy_decay_cal();
            numerator_y_decay += i * en;
            denominator_y_decay += en;

            en = y_stripe[i]->give_energy_decay_cal();
            if(Tactive_stopper_full::is_energy_cal_decay_in_gate(en))
            {
                numerator_y_decay_thrsh += i * en;
                denominator_y_decay_thrsh += en;
                if(RisingAnalysis_ptr->is_verbose_on())
                    cout << name_of_this_element
                         << "stripe y " << i << " had decay energy  " << en << endl;
            }
        }
    } // end for


    //======================================

    decay_sum_energy_both_xy_stripes = denominator_x_decay +
                                       denominator_y_decay;


    //======================================
    if(denominator_x_decay)
    {
        x_position_decay = numerator_x_decay / denominator_x_decay;
        spec_x_position_decay->manually_increment(x_position_decay);
        x_position_decay_ok = true;
    }
    else
    {
        x_position_decay_ok = false;
        x_position_decay = -9999;
    }

    if(denominator_y_decay)
    {
        y_position_decay = numerator_y_decay / denominator_y_decay;
        spec_y_position_decay->manually_increment(y_position_decay);
        y_position_decay_ok = true;

        //     y_position_decay = 4+module_id_nr;
        //cout << "Position Y = " << y_position_decay << endl;
    }
    else
    {
        y_position_decay_ok = false;
        y_position_decay = -9999;
    }

    if(x_position_decay_ok && y_position_decay_ok)
        spec_map_of_decays_without_thrsh->manually_increment(
            x_position_decay,
            y_position_decay);


    // same with threshold
    if(denominator_x_decay_thrsh)
    {
        x_position_decay_thrsh =
            numerator_x_decay_thrsh / denominator_x_decay_thrsh;
        spec_x_position_decay_thrsh->manually_increment
        (x_position_decay_thrsh);
        if(RisingAnalysis_ptr->is_verbose_on())
            cout <<
                 "Decay x position thrsh = " << x_position_decay_thrsh << endl;
        x_position_decay_thrsh_ok = true;
        //     x_position_decay_thrsh = 9 + module_id_nr;
    }
    else
    {
        x_position_decay_thrsh_ok = false;
        x_position_decay_thrsh = -9999;
    }

    if(denominator_y_decay_thrsh)
    {
        y_position_decay_thrsh =
            numerator_y_decay_thrsh / denominator_y_decay_thrsh;
        spec_y_position_decay_thrsh->
        manually_increment(y_position_decay_thrsh);
        if(RisingAnalysis_ptr->is_verbose_on())
            cout <<
                 "Decay y position thrsh = " << y_position_decay_thrsh
                 << endl;
        y_position_decay_thrsh_ok = true;
        //     y_position_decay_thrsh = 10 + module_id_nr;
    }
    else
    {
        y_position_decay_thrsh_ok = false;
        y_position_decay_thrsh = -9999;
    }

    // ------- rms --------------------------------------------

    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_X ; i++)
    {
        if(x_stripe[i]->give_fired())
        {
            numerator_x2_decay += pow((x_position_decay  - i), 2)  *
                                  x_stripe[i]->give_energy_decay_cal();

            if(Tactive_stopper_full::is_energy_cal_decay_in_gate(
                        x_stripe[i]->give_energy_decay_cal()))
            {
                numerator_x2_decay_thrsh +=
                    pow((x_position_decay_thrsh  - i), 2)  *
                    x_stripe[i]->give_energy_decay_cal();
            }
        }
    }
    // -------- Y ----------------
    for(unsigned int i = 0 ; i < NR_OF_STOPPER_STRIPES_Y ; i++)
    {
        if(y_stripe[i]->give_fired())
        {
            numerator_y2_decay += pow((y_position_decay  - i), 2)  *
                                  y_stripe[i]->give_energy_decay_cal();

            if(Tactive_stopper_full::is_energy_cal_decay_in_gate(
                        y_stripe[i]->give_energy_decay_cal()))
            {
                numerator_y2_decay_thrsh +=
                    pow((y_position_decay_thrsh  - i), 2)  *
                    y_stripe[i]->give_energy_decay_cal();
            }
        }
    } // end for


    if(denominator_x_decay)
        rms_x_decay = sqrt(numerator_x2_decay / denominator_x_decay);

    if(denominator_y_decay)
        rms_y_decay = sqrt(numerator_y2_decay / denominator_y_decay);

    if(denominator_x_decay_thrsh)
        rms_x_decay_thrsh =
            sqrt(numerator_x2_decay_thrsh / denominator_x_decay_thrsh);

    if(denominator_y_decay_thrsh)
        rms_y_decay_thrsh =
            sqrt(numerator_y2_decay_thrsh / denominator_y_decay_thrsh);

}
//**********************************************************************
void Tactive_stopper_combined_xy::implantation_action()
{
    // if the y positioning is broken - we may want to match only with x
    // in such a case we ptu we put yy = 0 and match to pixel  (x, 0)

    flag_matching_is_valid = false;
    flag_cross_matching_is_valid = false;
    flag_ok_and_2nd_decay = false;


    if(matching_without_y())
    {
        y_position_implantation_thrsh = 0;
    }
    else if(!y_position_implantation_thrsh_ok)
    {
        // if we want Y, but it is not valid, skip this implantation
        return;
    }

    // same in case if x is broken
    if(matching_without_x())
    {
        x_position_implantation_thrsh = 0;
    }
    else if(!x_position_implantation_thrsh_ok)
    {
        // if we want X, but it is not valid, skip this implantation
        return;
    }

    if(RisingAnalysis_ptr->is_verbose_on())
        cout << name_of_this_element
             << "--------- Tactive_stopper_combined_xy::implantation_action()"
             "for " << name_of_this_element
             << " - in "
             <<  x_position_implantation_thrsh
             << ", "
             <<  y_position_implantation_thrsh
             << endl;


    // storing in the pixel the infomation about
    // - energy - for fun - or recognizing the patter by funny energy value
    //  - timestamp
    // - Zet, Aoq and perhaps something more


    // selecting the pixel
    // position is in range 0-15


    // what to do, when position not calculated???

    //#ifdef NIGDY

    if(RisingAnalysis_ptr->is_verbose_on())
        cout << "Storing timestamp_FRS " << event_unpacked-> timestamp_FRS
             //<< " and energy " << x_1st_energy_implantation_cal
             << "in " << name_of_this_element  << " pixel "
             << (int) x_position_implantation_thrsh << ", "
             << (int) y_position_implantation_thrsh
             << ", zet = " << owner->give_zet()
             << ", zet2 = " << owner->give_zet2()
             << ", zet_sci = " << owner->give_zet_sci()
             << ", zet2_sci = " << owner->give_zet2_sci()
             <<  ", aoq = " << owner->give_aoq()
             << ", x_mult  = " <<
             x_multiplicity_for_energy_cal_implantation_above_threshold
             << ", y_mult = "   <<
             y_multiplicity_for_energy_cal_implantation_above_threshold
             << endl;
    //#endif

    //-------------
    // matrix of encrementrs - zeroed every event
    int x = (int) x_position_implantation_thrsh;
    int y = (int) y_position_implantation_thrsh;

    if((x < 0 || x >= (int) NR_OF_STOPPER_STRIPES_X) ||
            (y < 0 || y >= (int) NR_OF_STOPPER_STRIPES_Y))
    {
        cout << "Illegal coordinate of the implantation pixel (X= " << x
             << ", Y=" << y  << ")" << endl;
        return;
    }



    time_since_last_implantation[x][y]  =
        (event_unpacked-> timestamp_FRS -  pixel[x]
         [y].give_stored_timestamp()) / 50000;
    time_since_last_implantation_valid[x][y] = true;

    flag_implantation_successful = true;
    // really storing
    pixel[x] [y].store_implantation
    (
        // (int) x_1st_energy_implantation_cal,  //  int ener,
        event_unpacked-> timestamp_FRS,
        owner->give_zet(),    // zet,
        owner->give_zet2(),     // zet
        owner->give_zet_sci(),    // zet,
        owner->give_zet2_sci(),     // zet
        //owner->give_aoq(),    // aoq,
        //owner->give_aoq_sci_corrected(),

        owner->give_aoq_sci_mw_corrected(),  // 4
        owner->give_aoq_sci_sci_corrected(),   // 5
        owner->give_aoq_sci_tpc_corrected(),   // 6
        owner->give_aoq_tic_mw_corrected(),   // 7
        owner->give_aoq_tic_sci_corrected(),    // 8
        owner->give_aoq_tic_tpc_corrected(),   // 9

        owner->give_aoq_tpc_tpc_and_angle_at_s2tpc_corrected(),

        owner->give_music_deposit_energy_non_corr(),
        owner->give_music2_deposit_energy_non_corr(),

        owner->give_x_extrapolated_here(),
        // double mw_x_position_extrapolated,
        owner->give_y_extrapolated_here(),
        // double mw_y_position_extrapolated,
        owner->give_first_good_sci_energy(),        // sci42,
        owner->give_second_veto_sci_energy(),     // sci43
        owner->give_sci21_position(),       // sci21
        owner->give_tof_ps(),       // sci21
        x_multiplicity_for_energy_cal_implantation_above_threshold,
        y_multiplicity_for_energy_cal_implantation_above_threshold,
        denominator_x_implantation,
        denominator_y_implantation,
        owner->give_sci41horiz_position(),
        owner->give_degrader_dE_gamma(),
        owner->give_sci21_left(),
        owner->give_sci21_right()        // #$% place to add wishes to pixels
    );
    // adding in the (static) pattern word - the bit corresponding to the current module
    pattern_of_impantation_modules
    |=
        1 << (owner->get_id_nr() * HOW_MANY_STOPPER_MODULES)   + module_id_nr;

    // putting the address on the list
    addresses_of_implanted_pixels.push_back(
        pixel[x] [y].give_address_of_pattern_word());

    // loop which is storing t he current pattern_word into all registered in this event implantation pixels
    // even those from other  stopper modules
    //cout << name_of_this_element << endl;
    for(unsigned int i = 0 ; i < addresses_of_implanted_pixels.size() ; i++)
    {
        *addresses_of_implanted_pixels[i] = Tactive_stopper_combined_xy::pattern_of_impantation_modules;  // zeroing the vector of addresses

        //    cout << " storing the pattern " << hex  << Tactive_stopper_combined_xy::pattern_of_impantation_modules
        //    << " in the pixel from the list " << dec << i << endl;
    }


    owner->incr_how_many_implantations_in_this_event();

    // when we treat whole module as one pixel
    module_as_one_pixel[
        (owner->get_id_nr() * HOW_MANY_STOPPER_MODULES)   + module_id_nr].store_implantation
    (
        // (int) x_1st_energy_implantation_cal,  //  int ener,
        event_unpacked-> timestamp_FRS,
        owner->give_zet(),    // zet,
        owner->give_zet2(),     // zet
        owner->give_zet_sci(),    // zet,
        owner->give_zet2_sci(),     // zet

        /*owner->give_aoq(),    // aoq,
             owner->give_aoq_sci_corrected(),
             */
        owner->give_aoq_sci_mw_corrected(),  // 4
        owner->give_aoq_sci_sci_corrected(),   // 5
        owner->give_aoq_sci_tpc_corrected(),   // 6
        owner->give_aoq_tic_mw_corrected(),   // 7
        owner->give_aoq_tic_sci_corrected(),    // 8
        owner->give_aoq_tic_tpc_corrected(),   // 9

        owner->give_aoq_tpc_tpc_and_angle_at_s2tpc_corrected(),
        owner->give_music_deposit_energy_non_corr(),
        owner->give_music2_deposit_energy_non_corr(),

        owner->give_x_extrapolated_here(),
        // double mw_x_position_extrapolated,
        owner->give_y_extrapolated_here(),
        // double mw_y_position_extrapolated,
        owner->give_first_good_sci_energy(),        // sci42,
        owner->give_second_veto_sci_energy(),     // sci43
        owner->give_sci21_position(),       // sci21
        owner->give_tof_ps(),       // sci21
        x_multiplicity_for_energy_cal_implantation_above_threshold,
        y_multiplicity_for_energy_cal_implantation_above_threshold,
        denominator_x_implantation,
        denominator_y_implantation,
        owner->give_sci41horiz_position(),
        owner->give_degrader_dE_gamma(),

        owner->give_sci21_left(),
        owner->give_sci21_right()       // #$% place to add wishes to pixels

    );

    //--------------
    spec_map_of_implantations->manually_increment(
        x_position_implantation_thrsh, y_position_implantation_thrsh);
    owner->incr_impl_map(x_position_implantation_thrsh +
                         (NR_OF_STOPPER_STRIPES_X * module_id_nr),
                         y_position_implantation_thrsh);

    owner->set_x_position_implantation_thrsh(
        x_position_implantation_thrsh + (NR_OF_STOPPER_STRIPES_X * module_id_nr));
    owner->set_y_position_implantation_thrsh(
        y_position_implantation_thrsh);

}
//**********************************************************************
bool Tactive_stopper_combined_xy::decay_action()
{
    //   if(RisingAnalysis_ptr->is_verbose_on() )
    //     cout << name_of_this_element <<
    //     " --> Tactive_stopper_combined_xy::decay_action()"
    //     << endl;

    flag_cross_matching_is_valid = false;

    flag_matching_is_valid = false;
    flag_ok_and_2nd_decay = false;

    // if the y positioning is broken - we may want to match only with x
    // in such a case we ptu we put yy = 0 and match to pixel  (x, 0)
    if(matching_without_y())
    {
        y_position_decay_thrsh = 0;
    }
    else if(!y_position_decay_thrsh_ok)
    {
        // if we want Y, but it is not valid, skip this implantation
        // cout << "Rejected decay, bec. we want Y position ok" << endl;
        return false;
    }

    // same in case if x is broken
    if(matching_without_x())
    {
        x_position_decay_thrsh = 0;
    }
    else if(!x_position_decay_thrsh_ok)
    {
        // if we want X, but it is not valid, skip this implantation
        // cout << "Rejected decay, bec. we want Y position ok" << endl;
        return false;
    }

    // as this is the decay situation at first we need to
    // know if in this pixel there is some implantation
    //At first we will look in this pixel
    int xx = (int) x_position_decay_thrsh;
    int yy = (int) y_position_decay_thrsh;

    if((xx < 0 || xx >= (int) NR_OF_STOPPER_STRIPES_X) ||
            (yy < 0 || yy >= (int) NR_OF_STOPPER_STRIPES_Y))
    {
        cout << "Illegal coordinate of the decay pixel (X= " << xx
             << ", Y=" << yy  << ")" << endl;
        return false;
    }


    //   if(name_of_this_element == "stopper_M")
    //   {
    //   cout
    //   << name_of_this_element
    //   << " -------------------- map of decays thrsh, incr poiint x="
    //   <<xx << ", y= "   << yy << endl;
    //   }
    spec_map_of_decays->manually_increment(xx, yy);
    owner->incr_decay_map((NR_OF_STOPPER_STRIPES_X * module_id_nr) + xx, yy);

    owner->set_x_position_decay_thrsh(
        (NR_OF_STOPPER_STRIPES_X * module_id_nr) +  xx);
    owner->set_y_position_decay_thrsh(yy);

    if(
        pixel[xx] [yy].are_you_in_time_gate(event_unpacked-> timestamp_FRS))
    {
        //cout << "Found exact match in the current pixel  (" << xx
        // << ", " << yy << ")" << endl;
        // success
        successful_match_impl_with_decay(xx, yy);
        return true;
    }
    else if(
        Tactive_stopper_full::give_flag_match_only_same_pixel() == false)
    {
        //cout << "Exact match not found, lets look at "
        // "the neighbours..." << endl;
        // check 8 neighbours around

        // we do not want to address elements < 0, or > 15
        int left_margin = -1;
        if(xx + left_margin < 0)
            left_margin = 0 ;
        int right_margin = 1;
        if(xx + right_margin >= (int) NR_OF_STOPPER_STRIPES_X)
            right_margin = 0 ;

        // same for y
        int down_margin = -1;
        if(yy + down_margin < 0)
            down_margin = 0 ;
        int up_margin = 1;
        if(yy + up_margin > (int) NR_OF_STOPPER_STRIPES_Y)
            up_margin = 0 ;


        for(int xm = left_margin ; xm <= right_margin ; xm++)
            for(int ym = down_margin ; ym <= up_margin  ; ym++)
            {
                if(xm == 0 && ym == 0)
                    continue ;
                // bec. precise center case was made already above

#define ONLY_4_NEIGHBOURS
#ifdef ONLY_4_NEIGHBOURS
                // if we want not 8 neigbours, but just 4, the closest ones, skipping the corners
                if(xm == -1 && ym == -1 // left upper corner
                        ||
                        xm == +1 && ym == -1 // right upper corner
                        ||
                        xm == -1 && ym == 1 // left down corner
                        ||
                        xm == +1 && ym == +1 // right down corner
                  )
                    continue;
#endif


                //cout << "Checking neighbour: " << xx + xm << ", "
                // << yy + ym << endl;
                if(pixel[xx + xm] [yy + ym].are_you_in_time_gate(
                            event_unpacked-> timestamp_FRS))
                {
                    //cout << "Found match in one of the neighbours "
                    // << xx + xm << ", " << yy + ym << endl;
                    // success
                    successful_match_impl_with_decay(xx + xm, yy + ym);
                    return true;
                }
            } // end of both for loops
    } // end else the neighbours

    // only if it was impossible to find here, in this module
    if(y_position_decay_thrsh_ok && x_position_decay_thrsh_ok)
    {
        try_to_match_with_other_modules();
    }
    return false;
}
//***********************************************************************
bool Tactive_stopper_combined_xy::
active_stopper_pixel::
are_you_in_time_gate(long long current_time)
{
    if(!flag_valid_impl)  // no sensible implantation data
        return false;

    //     cout << "Current time = " << current_time
    //     << ", time of impl= " << timestamp_of_implantation << endl;

    long long time_diff = (current_time - timestamp_of_implantation);
    if(time_diff >
            Tactive_stopper_full::give_implantation_decay_time_gate_high())
    {
        // too old implantation
        make_invalid();
        return 0;
    }
    if(time_diff <
            Tactive_stopper_full::give_implantation_decay_time_gate_low())
    {
        // too young implantation -
        // should we make it invalid in this case? Ask Stephane: he sys NO
        return 0;
    }
    return true; // we are in the time gate, success
}
//***********************************************************************
void Tactive_stopper_combined_xy::successful_match_impl_with_decay(int x, int y)
{

    /*  cout << "Found successful matching the implantation: "
    << x_position_implantation
    << ", " << y_position_implantation
    << "  with  decay: " << x << ",  " << y << endl;

    */
#define PIX   pixel[x] [y]
    if(PIX.retrieve_implantation(
                //&found_impl_energy,
                &found_impl_timestamp,
                &found_impl_zet, &found_impl_zet2,
                &found_impl_zet_sci, &found_impl_zet2_sci,

                // &found_impl_aoq,
                //&found_aoq_sci_corrected,

                & found_impl_aoq_sci_mw_corrected,
                & found_impl_aoq_sci_sci_corrected,
                & found_impl_aoq_sci_tpc_corrected,
                & found_impl_aoq_tic_mw_corrected,
                & found_impl_aoq_tic_sci_corrected,
                & found_impl_aoq_tic_tpc_corrected,

                & found_aoq_tpc_tpc_and_angle_at_s2tpc_corrected ,

                &found_impl_mus_dep_energy_non_corr,
                &found_impl_mus2_dep_energy_non_corr,

                &found_impl_mw_x_position_extrapolated_to_stopper_position,
                &found_impl_mw_y_position_extrapolated_to_stopper_position,
                &found_impl_sci42_energy,
                &found_impl_sci43_energy,
                &found_impl_sci21_position,
                &found_impl_tof_ps,
                &found_multiplicity_of_x_strips,
                &found_multiplicity_of_y_strips,
                & found_sum_en_x,
                &found_sum_en_y,
                &found_sci41horiz_position,
                &found_degrader_dE_gamma,

                &found_nr_of_times_retrieved,
                &found_previous_implantation_never_decayed,  // #$%
                &found_sci21_left,
                &found_sci21_right,
                &found_previous_decay_time_for_this_implantation_in_miliseconds

            ))
    {
        flag_matching_is_valid = true;
        if(found_nr_of_times_retrieved == 2)
            flag_ok_and_2nd_decay = true;
        if(found_previous_implantation_never_decayed)
        {
            //   cout << "To jest ten zly przypadek " << endl;
        }

        unsigned int pattern_of_implanted_detectors = PIX.retrieve_implantations_pattern_word();
        //cout << name_of_this_element << endl;
        unsigned int pattern_shifted = pattern_of_implanted_detectors;


        for(unsigned int i = 0 ; i < 32 ; i++, pattern_shifted >>= 1)
        {
            pattern_says_that_also_was_implanted_module[i] = false;

            //if(!pattern_shifted) break;
            if(pattern_shifted & 1)
            {
                //cout << "Active stopper module nr " << i << " had implantation in the same time " << endl;
                pattern_says_that_also_was_implanted_module[i] = true;
            }
        }


        if(RisingAnalysis_ptr->is_verbose_on())
            cout << "I found following implantation data in this pixel\n"
                 << name_of_this_element
                 << ", pixel " << x
                 << ", " << y
                 << ", flag_valid = " <<  flag_matching_is_valid
                 // sometime we want to discard too old implantation
                 //<< ", ene = " <<  found_impl_energy
                 << ", timest = " <<  found_impl_timestamp
                 << ", zet = " <<  found_impl_zet
                 << ", zet2 = " <<  found_impl_zet2
                 << ", zet_sci = " <<  found_impl_zet_sci
                 << ", zet2_sci = " <<  found_impl_zet2_sci
                 //<< ", aoq = " <<  found_impl_aoq
                 << ", x_mw = " <<
                 found_impl_mw_x_position_extrapolated_to_stopper_position
                 << ", y_mw = " <<
                 found_impl_mw_y_position_extrapolated_to_stopper_position
                 << ", sci42 = " <<  found_impl_sci42_energy
                 << ", sci43 = " <<  found_impl_sci43_energy
                 << ", sci21_pos = " <<  found_impl_sci21_position
                 << ", tof_ps = " <<  found_impl_tof_ps
                 << ", found_multiplicity_of_x_strips = " <<
                 found_multiplicity_of_x_strips
                 << ", found_multiplicity_of_y_strips = " <<
                 found_multiplicity_of_y_strips
                 << endl;


        implantation_decay_time_difference_in_timestamp_ticks =
            event_unpacked-> timestamp_FRS -  found_impl_timestamp;

        implantation_decay_time_difference_in_seconds =
            implantation_decay_time_difference_in_timestamp_ticks / 50000000;

        implantation_decay_time_difference_in_one_tenth_of_seconds =
            implantation_decay_time_difference_in_timestamp_ticks / 5000000;

        implantation_decay_time_difference_in_miliseconds =
            implantation_decay_time_difference_in_timestamp_ticks / 50000;

        spec_map_of_matching->manually_increment(x, y);
        owner->incr_matching_map(x + (NR_OF_STOPPER_STRIPES_X * module_id_nr),
                                 y);

        // now we store in  this pixel the current time since
        PIX.remember_the_previous_decay_time(implantation_decay_time_difference_in_miliseconds);


#ifdef NIGDY

        if(found_multiplicity_of_x_strips == 1 &&
                found_multiplicity_of_y_strips == 1)
            spec_xy_sum_energy_impl_matched->manually_increment(
                found_sum_en_x, found_sum_en_y);
#endif // NIGDY

    }
    else
    {
        // not valid
        flag_matching_is_valid = false;
        flag_ok_and_2nd_decay = false;
    }
#undef PIX

}
//***********************************************************************
void Tactive_stopper_combined_xy::not_fired()
{
    zero_flags_fired_vectors();
    flag_cross_matching_is_valid = false;

    strip_x_energies_implantation_cal.clear();
    strip_y_energies_implantation_cal.clear();

    strip_x_energies_decay_cal.clear();
    strip_y_energies_decay_cal.clear();

    x_multiplicity_for_energy_cal_implantation_above_threshold =
        y_multiplicity_for_energy_cal_implantation_above_threshold =

            x_multiplicity_for_energy_cal_decay_above_threshold =
                y_multiplicity_for_energy_cal_decay_above_threshold =
                    x_multiplicity_for_energy_cal =
                        y_multiplicity_for_energy_cal = 0;


    x_1st_energy_implantation_cal =
        x_2nd_energy_implantation_cal =
            x_3rd_energy_implantation_cal =

                y_1st_energy_implantation_cal =
                    y_2nd_energy_implantation_cal =
                        y_3rd_energy_implantation_cal =

                            x_1st_energy_decay_cal =
                                x_2nd_energy_decay_cal =
                                    x_3rd_energy_decay_cal =

                                        y_1st_energy_decay_cal =
                                            y_2nd_energy_decay_cal =
                                                    y_3rd_energy_decay_cal =

                                                            x_position_implantation =
                                                                    x_position_decay   =

                                                                            y_position_implantation   =
                                                                                    y_position_decay   =

                                                                                            // ----- thrsh
                                                                                            x_position_implantation_thrsh =
                                                                                                    x_position_decay_thrsh   =

                                                                                                            y_position_implantation_thrsh   =
                                                                                                                    y_position_decay_thrsh   =

                                                                                                                            //==========
                                                                                                                            rms_x_implantation  =
                                                                                                                                    rms_x_decay  =
                                                                                                                                            rms_y_implantation =
                                                                                                                                                    rms_y_decay
                                                                                                                                                    =
                                                                                                                                                            rms_x_implantation_thrsh  =
                                                                                                                                                                    rms_x_decay_thrsh =
                                                                                                                                                                            rms_y_implantation_thrsh  =
                                                                                                                                                                                    rms_y_decay_thrsh = 0.0;

    x_position_implantation_thrsh_ok  = false;
    y_position_implantation_thrsh_ok  = false;
    x_position_implantation_ok = false;
    y_position_implantation_ok = false;

    x_position_decay_thrsh_ok  = false;
    y_position_decay_thrsh_ok  = false;
    x_position_decay_ok = false;
    y_position_decay_ok = false;


    decay_sum_energy_both_xy_stripes =
        denominator_x_decay =  denominator_y_decay = 0.0;


    denominator_x_implantation =
        denominator_y_implantation =
            found_sci41horiz_position =
                found_degrader_dE_gamma = 0;
    // found_aoq_sci_corrected =  0;


    x_position_implantation =
        y_position_implantation =
            x_position_implantation_thrsh =
                y_position_implantation_thrsh =

                    x_position_decay =
                        y_position_decay =
                            x_position_decay_thrsh =
                                y_position_decay_thrsh = -9999;

}
//**********************************************************************
bool  Tactive_stopper_combined_xy::try_to_match_with_other_modules()
{

    //   if(RisingAnalysis_ptr->is_verbose_on() )
    //     cout << name_of_this_element <<
    //     " --> Tactive_stopper_combined_xy::try_to_match_with_other_modules()"
    //     << endl;

    flag_cross_matching_is_valid = false;

    // if the y positioning is broken
    if(!y_position_decay_thrsh_ok)
    {
        return false;
    }
    // same in case if x is broken
    if(!x_position_decay_thrsh_ok)
    {
        return false;
    }


    uint my_nr = HOW_MANY_STOPPER_MODULES * owner->get_id_nr() + module_id_nr ;

    //   cout << "Searching cross match for decay in stopper  " << name_of_this_element << " "
    //   << owner->get_id_nr() << ", module " << module_id_nr << endl;


#ifdef ANDRES_OLD_CROSS_MATCHING
    //========================================
    // Andres says that we chceck only 1M, 2L modules if      the implantation was from the module 2M
    //========================================
    // nr 1 is stopper_M
    // nr 3 is stopper2_L

    if(!(my_nr == 1 || my_nr == 3))
        return false;

    // 4 is nr of stopper2_M

    if(module_as_one_pixel[4] .are_you_in_time_gate(
                event_unpacked-> timestamp_FRS))
    {
        if(retrieve_this_cross(4))
        {
            // success
            // break,
            return true;
        }
    }
#endif  // ANDRES_OLD_CROSS_MATCHING


    // Anther idea of Andres. He wants to check the following combination
    // of cross matching
    //================================
    // Decay    |    Implantation
    //  M2      |    M
    //  M       |    M2 (when L2 was not decaying also)
    //  L2      |    M2 (when M was not decaying also)
    //  M2      |    L2
    enum { L, M, R, L2, M2, R2 };

    switch(my_nr)
    {

        //-------------------
    case M2 : // if decay was in M2
        // try to find implantation in M
        if(module_as_one_pixel[M] .are_you_in_time_gate(
                    event_unpacked-> timestamp_FRS))
        {
            if(retrieve_this_cross(M))
            {
                // success
                // break,
                return true;
            }
        }

        // if not, try another crossmatch - with the L2
        if(module_as_one_pixel[L2] .are_you_in_time_gate(
                    event_unpacked-> timestamp_FRS))
        {
            if(retrieve_this_cross(L2))
            {
                // success
                // break,
                return true;
            }
        }

        break;


        //-------------------
    case M : // if decay was in M
        // try to find implantation in 2

        // but at first we try if L2 also had implatntaion

        if(! module_as_one_pixel[L2] .are_you_in_time_gate(
                    event_unpacked-> timestamp_FRS))
        {
            if(module_as_one_pixel[M2] .are_you_in_time_gate(
                        event_unpacked-> timestamp_FRS))
            {
                if(retrieve_this_cross(M2))
                {
                    // success
                    // break,
                    return true;
                }
            }
        }
        break;



        //-------------------
    case L2 : // if decay was in L2
        // try to find implantation in M2

        // but at first we try if M also had implatntaion

        if(! module_as_one_pixel[M] .are_you_in_time_gate(
                    event_unpacked-> timestamp_FRS))
        {
            if(module_as_one_pixel[M2] .are_you_in_time_gate(
                        event_unpacked-> timestamp_FRS))
            {
                if(retrieve_this_cross(M2))
                {
                    // success
                    // break,
                    return true;
                }
            }
        }
        break;


    }



#ifdef NIGDY
    bool already_checked[6] = { false};

    for(uint i = module_id_nr ; i < module_as_one_pixel.size() ; i += 3)
    {
        already_checked[i] = true;
        if(i == my_nr)
        {
            //       cout << " Skipping combination for " << my_nr << endl;
            continue;
        }
        //     cout << "checking the vector module i = " << i << endl;
        if(module_as_one_pixel[i] .are_you_in_time_gate(
                    event_unpacked-> timestamp_FRS))
        {
            if(retrieve_this_cross(i))
            {
                // success
                // break,
                return true;
            }
        }
    }
    //=================
    // if this was not successful, search all of them (even those above, but it is faster)
    //   cout << "second scan... " << endl;
    for(uint i = 0 ; i < module_as_one_pixel.size() ; i++)
    {
        //     cout << "checking the vector module i = " << i << endl;
        if(already_checked[i])
        {
            //       cout << "    Skipping already chcecked det "<< i << endl;
            continue;
        }
        // bec. precise center case was made already above
        // cout << "i = " << i ;
        if(module_as_one_pixel[i] .are_you_in_time_gate(
                    event_unpacked-> timestamp_FRS))
        {
            if(retrieve_this_cross(i))
            {
                // success
                // break,
                return true;
            }
        }
    } // end of both for loops
#endif
    //   cout << "End of cross check " << endl;

    return false;
}
//***********************************************************************
bool  Tactive_stopper_combined_xy::retrieve_this_cross(int i)
{
    //       ile_razy++;
    //       cout << "Found cross match in the module nr  " << i + module_id_nr << endl;

    //#ifdef JESZCZE_NIE
    if(module_as_one_pixel[i].retrieve_implantation(
                &found_cross_impl_timestamp,
                &found_cross_impl_zet,
                &found_cross_impl_zet2,
                &found_cross_impl_zet_sci,
                &found_cross_impl_zet2_sci,
                //&found_cross_aoq_sci_corrected,
                //     &found_cross_impl_aoq,
                &found_cross_impl_aoq_sci_mw_corrected,
                &found_cross_impl_aoq_sci_sci_corrected,
                &found_cross_impl_aoq_sci_tpc_corrected,

                // before magnets was tic
                &found_cross_impl_aoq_tic_mw_corrected,
                &found_cross_impl_aoq_tic_sci_corrected,
                &found_cross_impl_aoq_tic_tpc_corrected,

                &found_cross_aoq_tpc_tpc_and_angle_at_s2tpc_corrected,

                &found_cross_impl_mus_dep_energy_non_corr,
                &found_cross_impl_mus2_dep_energy_non_corr,

                &found_cross_impl_mw_x_position_extrapolated_to_stopper_position,
                &found_cross_impl_mw_y_position_extrapolated_to_stopper_position,
                &found_cross_impl_sci42_energy,
                &found_cross_impl_sci43_energy,
                &found_cross_impl_sci21_position,
                &found_cross_impl_tof_ps,
                &found_cross_multiplicity_of_x_strips,
                &found_cross_multiplicity_of_y_strips,
                &found_cross_sum_en_x,
                &found_cross_sum_en_y,
                &found_cross_sci41horiz_position,// #$%
                &found_cross_degrader_dE_gamma,
                &found_cross_nr_of_times_retrieved,
                &found_previous_implantation_never_decayed,
                &found_cross_sci21_left,
                &found_cross_sci21_right,
                &found_previous_decay_time_for_this_implantation_in_miliseconds
            ))
    {
        flag_cross_matching_is_valid = true;

        spec_map_of_cross_matching->manually_increment(
            int((owner->get_id_nr() * 3) + module_id_nr) ,
            int(i));

        return true;  // success
    }
    else
    {
        // not valid
        flag_cross_matching_is_valid = false;
        return false;
    }
}
//**************************************************************************************************
void Tactive_stopper_combined_xy::active_stopper_pixel::store_implantation(
    //int ener,
    unsigned long long timest,
    double z, double z2,
    double z_sci, double z2_sci,

    //   double a,
    //    double aoq_sci_corrected_arg,
    double aoq_sci_mw_corrected_,       // 4
    double aoq_sci_sci_corrected_,
    double aoq_sci_tpc_corrected_,
    double aoq_tic_mw_corrected_,
    double aoq_tic_sci_corrected_,
    double aoq_tic_tpc_corrected_,      // 9

    double  aoq_tpc_tpc_and_angle_at_s2tpc_corrected_,

    double mus_dE,
    double mus2_dE,

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

    int sci21_left_arg,
    int sci21_right_arg       // #$% place to add wishes to pixels
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
    zet_sci = z_sci;
    zet2_sci = z2_sci;

    mus_dep_energy_non_corr =  mus_dE;
    mus2_dep_energy_non_corr = mus2_dE;
    //aoq = a;
    //aoq_sci_corrected = aoq_sci_corrected_arg;

    aoq_sci_mw_corrected = aoq_sci_mw_corrected_;
    aoq_sci_sci_corrected =  aoq_sci_sci_corrected_;
    aoq_sci_tpc_corrected = aoq_sci_tpc_corrected_;
    aoq_tic_mw_corrected =  aoq_tic_mw_corrected_;
    aoq_tic_sci_corrected =  aoq_tic_sci_corrected_;
    aoq_tic_tpc_corrected =  aoq_tic_tpc_corrected_;

    aoq_tpc_tpc_and_angle_at_s2tpc_corrected =
        aoq_tpc_tpc_and_angle_at_s2tpc_corrected_;


    mw_x_position_extrapolated_to_stopper_position =
        mw_x_position_extrapolated;
    mw_y_position_extrapolated_to_stopper_position =
        mw_y_position_extrapolated;
    sci42_energy = sci42e ;
    sci43_energy = sci43e;
    sci21_position = sci21_p_a ;
    tof_ps = tof_ps_a;
    multiplicity_of_x_strips_above_thrs_during_calculation_of_the_position =
        multiplicity_of_x_strips_arg;
    multiplicity_of_y_strips_above_thrs_during_calculation_of_the_position =
        multiplicity_of_y_strips_arg;
    sum_en_x = sum_en_x_arg;
    sum_en_y = sum_en_y_arg;   // #$% place to add wishes to pixels
    sci41horiz_position = sci41horiz_position_arg;
    degrader_dE_gamma = degrader_dE_gamma_arg;

    sci21_left = sci21_left_arg;
    sci21_right =  sci21_right_arg ;


    long long time_diff = (timest - timestamp_of_implantation);
    if(nr_of_times_retrieved == 0
            &&
            flag_valid_impl
            &&   // if the previous implantation is not too old
            (time_diff < Tactive_stopper_full::give_implantation_decay_time_gate_high())
      )

    {
        previous_implantation_never_decayed = true;
        // cout << "While storing the implantation I see that the previous_implantation_never_decayed " << endl;
    }
    else
    {
        previous_implantation_never_decayed = false;
    } // how nice!

    nr_of_times_retrieved = 0;
    timestamp_of_implantation = timest; // now we can put the current time of implantation
    flag_valid_impl = true;
    implantations_pattern_word = 0 ; // it will be filled with the special function after all other modules has been anaysed for this event

    time_of_previous_decay_in_miliseconds = 0; // for implantations which will have  two decays
}
//**************************************************************************************************
/*
void Tactive_stopper_combined_xy::active_stopper_pixel::
store_pattern_word_of_all_implantations_during_current_event()
{
implantations_pattern_word = Tactive_stopper_combined_xy::pattern_of_impantation_modules;
}
*/


#endif   // def ACTIVE_STOPPER_PRESENT
