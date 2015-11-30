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
#include "Tactive_stopper_munich.h"

#ifdef MUN_ACTIVE_STOPPER_PRESENT

#include <sstream>
#include "Tnamed_pointer.h"
using namespace std;


#include "TIFJAnalysis.h" // for verbose
#include "Tactive_stopper_matrix_xy.h"


long long Tactive_stopper_munich::implantation_decay_time_gate_low;
long long Tactive_stopper_munich::implantation_decay_time_gate_high;


#include "Tactive_stopper_one_stripe.h"   // for namespace
using namespace active_stopper_namespace;
// ******************************************************************
Tactive_stopper_munich::Tactive_stopper_munich(
    string name,
    int my_number_,
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
):
    Tfrs_element(name),
    Tincrementer_donnor(),
    my_number(my_number_),
    data(data_ptr),
    data_plate_fired(data_fired_ptr),
    flag_decay_situation_is_needed(flag_decay_situation_is_needed_)
{

    // to make possible the general trajectory plot
    incrementer_with_beamline_position_of_this_device = my_number;
    compression_factor_for_x_position = 1; // no compression

    aoq_object =
        dynamic_cast<Taoq_calculator*>(owner->
                                       address_of_device(name_aoq_value));
    zet_object = dynamic_cast<Tzet_calculator*>(owner->
                 address_of_device(name_zet_object));
    zet2_object = dynamic_cast<Tzet_calculator*>(owner->
                  address_of_device(name_zet2_object));
    focal_plane_object  =
        dynamic_cast<Tfocal_plane*>(owner->
                                    address_of_device(name_of_focal_plane)) ;

    first_good_scintillator =
        dynamic_cast<Tscintillator*>(owner->
                                     address_of_device(
                                         name_of_first_good_scintillator));

    second_veto_scintillator =
        dynamic_cast<Tscintillator*>(owner->
                                     address_of_device(
                                         name_of_second_veto_scintillator));

    sci21_object = dynamic_cast<Tscintillator*>(owner->
                   address_of_device(name_of_sci21_scintillator));


    sci41horiz_object = dynamic_cast<Tscintillator*>(owner->
                        address_of_device(name_of_sci41_scintillator));

    degrader_object = dynamic_cast<Tdegrader*>(owner->
                      address_of_device(name_of_degrader_object));

    xy_matrix_object = dynamic_cast<Tactive_stopper_matrix_xy*>(owner->
                       address_of_device(name_of_xy_matrix_object));

    if(!aoq_object || ! zet_object || ! zet2_object ||
            !focal_plane_object || !first_good_scintillator ||
            !second_veto_scintillator
            || !sci21_object || !sci41horiz_object || !degrader_object
            || !xy_matrix_object)
    {
        cout << "Cant find one of the collaborators of Active stopper. "
             "Look at the constructor... " << endl;
        exit(1);
    }


    //----------------- X ------------------------
    string nam = name_of_this_element + "_plate_x"  ;

    x_plate =  new Tactive_stopper_vector_strips(
        nam,
        0,
        NR_OF_MUN_STOPPER_STRIPES_X,
        flag_decay_situation_is_needed// we do not need the decay situation
    );
    // ----------------- Y -------------------------

    nam.clear();
    nam =  name_of_this_element + "_plate_y" ;

    y_plate =  new Tactive_stopper_vector_strips(
        nam,
        1,
        NR_OF_MUN_STOPPER_STRIPES_Y,
        flag_decay_situation_is_needed // we do not need the decay situation
    );

    create_my_spectra();
    x_energy_cal_implantation_gate_dn = 0;
    x_energy_cal_implantation_gate_up = 8192;
    y_energy_cal_implantation_gate_dn = 0;
    y_energy_cal_implantation_gate_up = 8192;

    x_energy_cal_decay_gate_dn  = 0;
    x_energy_cal_decay_gate_up = 8192;
    y_energy_cal_decay_gate_dn  = 0;
    y_energy_cal_decay_gate_up = 8192;

    flag_algorithm_for_implantation_position_calculation_is_mean  = true;
    // otherwise MAX

    flag_match_only_same_pixel = false;  // also 8 neighbours around

    //===============
    named_pointer[name_of_this_element + "_flag_trigger_of_implantation"] =
        Tnamed_pointer(&flag_trigger_of_implantation, 0, this) ;

    named_pointer[name_of_this_element + "_flag_trigger_of_decay"] =
        Tnamed_pointer(&flag_trigger_of_decay, 0, this) ;



    named_pointer[name_of_this_element + "_beamline_position_of_this_device"] =
        Tnamed_pointer(&incrementer_with_beamline_position_of_this_device, 0, this) ;




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
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_zet_matched_with_decay_when_ok" ;
    //
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet, &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet, &flag_matching_is_valid, this);
    //-----------


    named_pointer[name_of_this_element +
                  "_implantation_zet2_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_zet2, &flag_matching_is_valid, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_implantation_zet2_matched_with_decay_when_ok" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet2, &flag_matching_is_valid, this);

    entry_name =
        "ALL_stoppers_"
        "_implantation_zet2_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet2, &flag_matching_is_valid, this);





    //------------- AOQ
    // Old style for compatibility

    named_pointer[name_of_this_element +
                  "_implantation_aoq_matched_with_decay_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_sci_mw_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_matched_with_decay_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_sci_mw_corrected,
                               &flag_matching_is_valid, this);


    named_pointer[name_of_this_element +
                  "_implantation_aoq_sci_corrected_when_ok"] =
                      Tnamed_pointer(&found_impl_aoq_sci_sci_corrected, &flag_matching_is_valid, this) ;

    entry_name =
        "ALL_stoppers_"
        "_implantation_aoq_sci_corrected_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_aoq_sci_sci_corrected,
                               &flag_matching_is_valid, this);



    // ---- now new

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


    //  End of AOQ incrementers #######################################################


    named_pointer[name_of_this_element +
                  "_implantation_sci42_energy_when_ok"] =
                      Tnamed_pointer(&found_impl_sci42_energy,
                                     &flag_matching_is_valid, this) ;
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

    entry_name =
        "ALL_stoppers_"
        "_implantation_sci21_position_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_impl_sci21_position,
                               &flag_matching_is_valid, this);

    //-----------------
    // #$%

    named_pointer[name_of_this_element +
                  "_implantation_sci41horiz_position_when_ok"] =
                      Tnamed_pointer(&found_sci41horiz_position, &flag_matching_is_valid, this) ;


    entry_name =
        "ALL_stoppers_"
        "_implantation_sci41horiz_position_when_ok" ;
    Tnamed_pointer::add_as_ALL(entry_name, &found_sci41horiz_position,
                               &flag_matching_is_valid, this);

    //---------------------
    named_pointer[name_of_this_element +
                  "_implantation_degrader_dE_gamma_when_ok"] =
                      Tnamed_pointer(&found_degrader_dE_gamma, &flag_matching_is_valid, this) ;

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


    named_pointer[name_of_this_element + "__decay_sum_energy"] =
        Tnamed_pointer(
            &decay_sum_energy,
            0, this) ;


#if 0
    named_pointer[name_of_this_element
                  + "_flag__previous_implantation_never_decayed"] =
                      Tnamed_pointer(
                          &found_previous_implantation_never_decayed,
                          &flag_matching_is_valid, this) ;
#endif

}
//***********************************************************************************************
void Tactive_stopper_munich::create_my_spectra()
{
    string folder("stopper/");

    //----------------------------------- 2D -----------------;
    string name = name_of_this_element + "_map_for_implantations_thrsh"  ;
    spec_map_for_implantations_thrsh
    = new spectrum_2D(name,
                      name,
                      NR_OF_MUN_STOPPER_STRIPES_X, 0,
                      NR_OF_MUN_STOPPER_STRIPES_X,
                      NR_OF_MUN_STOPPER_STRIPES_Y, 0,
                      NR_OF_MUN_STOPPER_STRIPES_Y,
                      folder, "",
                      string("as X:") + name_of_this_element
                      + "_put_something here" +
                      ", as Y: " + name_of_this_element +
                      " put something here");

    frs_spectra_ptr->push_back(spec_map_for_implantations_thrsh) ;

    //-----------------
    if(flag_decay_situation_is_needed)
    {
        name = name_of_this_element + "_map_for_decays_thrsh"  ;

        spec_map_for_decays_thrsh
        = new spectrum_2D(name,
                          name,
                          NR_OF_MUN_STOPPER_STRIPES_X, 0,
                          NR_OF_MUN_STOPPER_STRIPES_X,
                          NR_OF_MUN_STOPPER_STRIPES_Y, 0,
                          NR_OF_MUN_STOPPER_STRIPES_Y,
                          folder, "",
                          string("as X:") + name_of_this_element
                          + "_put_something here" +
                          ", as Y: " + name_of_this_element +
                          " put something here");

        frs_spectra_ptr->push_back(spec_map_for_decays_thrsh) ;
    }

    //=================
    name = name_of_this_element +
           "_map_of_successful_matching_implantations_with_decays_thrsh"  ;
    spec_map_of_matching =
        new spectrum_2D(name,
                        name,
                        NR_OF_MUN_STOPPER_STRIPES_X, 0,
                        NR_OF_MUN_STOPPER_STRIPES_X,
                        NR_OF_MUN_STOPPER_STRIPES_Y, 0,
                        NR_OF_MUN_STOPPER_STRIPES_Y,
                        folder, "",
                        string("as X:") + name_of_this_element +
                        "_put_something here" +
                        ", as Y: " + name_of_this_element +
                        " put something here");

    frs_spectra_ptr->push_back(spec_map_of_matching) ;


}
//*********************************************************************************************
Tactive_stopper_munich::~Tactive_stopper_munich()
{
    delete x_plate;
    delete y_plate;
}
//*********************************************************************************************
void Tactive_stopper_munich::analyse_current_event()
{
    //      cout << "Funct. Tactive_stopper_munich::analyse_current_event " << endl;

    flag_matching_is_valid = false;
    flag_trigger_of_decay =
        flag_trigger_of_implantation =  false;
    decay_sum_energy = 0 ;

    /*    implantation_decay_time_difference_in_timestamp_ticks =  0;
        implantation_decay_time_difference_in_seconds = 0;
        implantation_decay_time_difference_in_one_tenth_of_seconds = 0;
        implantation_decay_time_difference_in_miliseconds = 0;*/
    zero_flags_fired_vectors();

    switch(event_unpacked->trigger)
    {
    case 2:
    case 4:
        active_stopper_event_type = implantation_event;
        flag_trigger_of_implantation = true;  // for incrementers
        break;

    case 3:
    case 5:
        active_stopper_event_type = decay_event;
        flag_trigger_of_decay = true; // for incrementers
        break;

    default:
        active_stopper_event_type = undefined_event;
        break;
    }

    //=============================

    // ---------- X ----------
    if((event_unpacked->*data_plate_fired)[my_number][0])
    {
        x_plate->analyse_current_event();
    }
    else
    {
        x_plate->row_not_fired();
    }

    // -------- Y --------
    if((event_unpacked->*data_plate_fired))   // skipping (offsetting) x stripes
    {
        y_plate->analyse_current_event();
    }
    else
    {
        y_plate->row_not_fired();
    }


    // drawing map of the matrix
    switch(active_stopper_event_type)
    {
    case implantation_event:
        //     cout << name_of_this_element << "implant, "
        //     << endl;

        // here we may take the xy info from the other detector
        if(0)
        {
            universal_x_position_implantation = xy_matrix_object->give_x() / compression_factor_for_x_position ;
            universal_y_position_implantation = xy_matrix_object->give_y() ;

            universal_x_position_implantation_ok =
                xy_matrix_object->was_x_ok();
            universal_y_position_implantation_ok =
                xy_matrix_object->was_y_ok();
        }
        else
        {
            universal_x_position_implantation =
                x_plate->give_implantation_thrsh_position() / compression_factor_for_x_position;
            universal_y_position_implantation = y_plate->give_implantation_thrsh_position() ;

            universal_x_position_implantation_ok =
                x_plate->is_position_implantation_thrsh_ok();
            universal_y_position_implantation_ok =
                y_plate->is_position_implantation_thrsh_ok();
        }


#if 0

        // FAKE !!!!!!!!!!!!!
        universal_x_position_implantation = 5;
        universal_y_position_implantation = 30;

        universal_x_position_implantation_ok = true;
        universal_y_position_implantation_ok = true;

#endif



        if(universal_x_position_implantation_ok
                &&
                universal_y_position_implantation_ok
          )
        {
            implantation_action();
        }
        break;

    case  decay_event:
        //     cout << name_of_this_element << "decay, "
        //     << endl;


        if(flag_decay_situation_is_needed)
        {
            if(x_plate->is_position_decay_thrsh_ok()
                    &&
                    y_plate->is_position_decay_thrsh_ok()
              )
            {
                //                 cout << name_of_this_element << " map of decay, thrsh x = "
                //                 <<  x_plate->give_decay_thrsh_position()
                //                 << ", y = " <<  y_plate->give_decay_thrsh_position()
                //                 << endl;

                decay_action();
                decay_sum_energy = y_plate->give_sum_energies_above_threshold_when_decay();
            }

        }
        break;
    case undefined_event:
        break;
    } //end switch
}
//**********************************************************************************************
void Tactive_stopper_munich::make_preloop_action(ifstream & s) // read the calibration factors, gates
{
    //cout << "Empty funct. make_preloop_action " << endl;
    string cal_filename = "calibration/tin100_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            string mess = "I can't open calibration file: "
                          + cal_filename  ;
            throw mess ;
        }

        //====================
        x_plate->make_preloop_action(plik);
        y_plate->make_preloop_action(plik);

    }  // end of try
    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    // reading options =================
    cout << "Reading the options for 100Sn..." << endl;
    // --------------- options -------------------------
    // real reading the calibration -----------------
    string opt_filename = "options/tin100_settings.txt";
    try
    {
        ifstream plik(opt_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            /*      string mess = "I can't open file: "
            + opt_filename  ;
            throw mess ;*/
            cout << "I can't open file: "  << opt_filename << endl ;
            //       cout << "To create this file you need to run 'Cracow' and choose"
            //       " the option: Spy_options->Active stopper settings\n"
            //       "Then the dialog box will appear. It is enough to press 'OK' and"
            //       " this file will be created." << endl;
            exit(1);
        }

        x_energy_cal_implantation_gate_dn  =
            Tfile_helper::find_in_file(plik,
                                       name_of_this_element + "_implantation_x_energy_gate");
        plik >> zjedz >> x_energy_cal_implantation_gate_up  ;

        y_energy_cal_implantation_gate_dn  =
            Tfile_helper::find_in_file(plik,
                                       name_of_this_element + "_implantation_y_energy_gate");
        plik >> zjedz >> y_energy_cal_implantation_gate_up  ;

        if(flag_decay_situation_is_needed)
        {
            x_energy_cal_decay_gate_dn  =
                Tfile_helper::find_in_file(plik,
                                           name_of_this_element + "_decay_x_energy_gate");
            plik >> zjedz >> x_energy_cal_decay_gate_up ;

            y_energy_cal_decay_gate_dn  =
                Tfile_helper::find_in_file(plik,
                                           name_of_this_element + "_decay_y_energy_gate");
            plik >> zjedz >> y_energy_cal_decay_gate_up ;
        }



        flag_algorithm_for_implantation_position_calculation_is_mean  =
            (bool)    Tfile_helper::find_in_file(plik,
                    name_of_this_element
                    + "_algorithm_for_position_calculation_is");
        // otherwise MAX


        implantation_decay_time_gate_low = (long long)
                                           Tfile_helper::find_in_file(plik,
                                                   "implantation_decay_time_gate_low");
        implantation_decay_time_gate_high  = (long long)
                                             Tfile_helper::find_in_file(plik,
                                                     "implantation_decay_time_gate_high");

        // in Cracow we put these data in miliseconds, so we have
        // to recalculate it into timestam units
        // one unit is 20 nano seconds
        implantation_decay_time_gate_low *= 50000;
        implantation_decay_time_gate_high *= 50000;

        zero_flags_fired_vectors();

        //====================
        int *ptr =    &((event_unpacked->*data)[my_number][0]);
        x_plate->your_array_of_data_will_be_here(ptr);

        // setting the options
        x_plate->set_options(

            x_energy_cal_implantation_gate_dn,
            x_energy_cal_implantation_gate_up,
            x_energy_cal_decay_gate_dn,
            x_energy_cal_decay_gate_up,
            flag_algorithm_for_implantation_position_calculation_is_mean);

        ptr =    &((event_unpacked->*data)[my_number][NR_OF_MUN_STOPPER_STRIPES_X]);
        y_plate->your_array_of_data_will_be_here(ptr);

        // setting the options
        y_plate->set_options(
            y_energy_cal_implantation_gate_dn,
            y_energy_cal_implantation_gate_up,
            y_energy_cal_decay_gate_dn,
            y_energy_cal_decay_gate_up,
            flag_algorithm_for_implantation_position_calculation_is_mean);

        // newest paramete
        try
        {
            compression_factor_for_x_position  = (int)
                                                 Tfile_helper::find_in_file(plik,
                                                         "munich_stoppers__compression_factor_for_x_position");
        }
        catch(...)
        {
            compression_factor_for_x_position = 1 ;
        }

    }

    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)    ; // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "Error while reading options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    catch(Tfile_helper_exception &k)
    {
        cerr << "Error while reading options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    catch(string sss)
    {
        cerr << " Error while reading the file "
             << opt_filename
             << ":\n"
             << sss << endl  ;
        exit(-1)
        ; // is it healthy ?
    }


    //


    //cout << name_of_this_element
    // << " options are successfully read " <<endl;

    //==============================

}
//*************************************************************************
//*********************************************************************
/*!
    \fn Tactive_stopper_combined_xy::
    zero_flags_fired_vectors()
 */
void Tactive_stopper_munich::zero_flags_fired_vectors()
{

    memset(&time_since_last_implantation[0][0], 0,
           sizeof(time_since_last_implantation));
    memset(&time_since_last_implantation_valid[0][0], 0,
           sizeof(time_since_last_implantation_valid));



    implantation_decay_time_difference_in_timestamp_ticks =  0;
    implantation_decay_time_difference_in_seconds = 0;
    implantation_decay_time_difference_in_one_tenth_of_seconds = 0;
    implantation_decay_time_difference_in_miliseconds = 0;


    flag_implantation_successful = false;
}
//********************************************************************
//**********************************************************************
void Tactive_stopper_munich::implantation_action()
{

    if(1)
    {
        // if the y positioning is broken - we may want to match only with x
        // in such a case we ptu we put yy = 0 and match to pixel  (x, 0)

        flag_matching_is_valid = false;
#ifdef NIGDY
        if(matching_without_y())
        {
            universal_y_position_implantation = 0;
        }
        else if(!y_position_implantation_thrsh_ok)
        {
            // if we want Y, but it is not valid, skip this implantation
            return;
        }

        // same in case if x is broken
        if(matching_without_x())
        {
            universal_x_position_implantation = 0;
        }
        else if(!x_position_implantation_thrsh_ok)
        {
            // if we want X, but it is not valid, skip this implantation
            return;
        }
#endif // NIGDY

        if(RisingAnalysis_ptr->is_verbose_on())
            cout << name_of_this_element
                 << "--------- Tactive_stopper_combined_xy::implantation_action()"
                 "for " << name_of_this_element
                 << " - in "
                 <<  universal_x_position_implantation
                 << ", "
                 <<  universal_y_position_implantation
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
                 << (int) universal_x_position_implantation << ", "
                 << (int) universal_y_position_implantation
                 << ", zet = " << give_zet()
                 << ", zet2 = " << give_zet2()
                 <<  ", aoq = " << give_aoq()
                 /*    << ", x_mult  = " <<
                     x_multiplicity_for_energy_cal_implantation_above_threshold
                     << ", y_mult = "   <<
                     y_multiplicity_for_energy_cal_implantation_above_threshold*/
                 << endl;
        //#endif

        //-------------
        // matrix of encrementrs - zeroed every event
        int x = (int) universal_x_position_implantation;
        int y = (int) universal_y_position_implantation;

        if((x < 0 || x >= (int) NR_OF_MUN_STOPPER_STRIPES_X) ||
                (y < 0 || y >= (int) NR_OF_MUN_STOPPER_STRIPES_Y))
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
            event_unpacked-> timestamp_FRS,   // 1
            give_zet(),    // zet, //2
            give_zet2(),     // zet //3
            give_aoq_sci_mw_corrected(),  // 4
            give_aoq_sci_sci_corrected(),   // 5
            give_aoq_sci_tpc_corrected(),   // 6
            give_aoq_tic_mw_corrected(),   // 7
            give_aoq_tic_sci_corrected(),    // 8
            give_aoq_tic_tpc_corrected(),   // 9

            give_first_good_sci_energy(),        // sci42, //10
            give_second_veto_sci_energy(),     // sci43  // 11
            give_sci21_position(),       // sci21   // 12
            give_sci41horiz_position(),  // 13
            give_degrader_dE_gamma()   //14
        );




    }// endif false/ture

    spec_map_for_implantations_thrsh->manually_increment(
        universal_x_position_implantation,
        universal_y_position_implantation);
}
//**********************************************************************
bool Tactive_stopper_munich::decay_action()
{
    //   if(RisingAnalysis_ptr->is_verbose_on() )
    //     cout << name_of_this_element <<
    //     " --> Tactive_stopper_munich::decay_action()"
    //     << endl;

    flag_matching_is_valid = false;

#ifdef NEVER
    if(!y_plate->is_position_decay_thrsh_ok())
    {
        // if we want Y, but it is not valid, skip this implantation
        // cout << "Rejected decay, bec. we want Y position ok" << endl;
        return false;
    }

    // same in case if x is broken
    if(!x_plate->is_position_decay_thrsh_ok())
    {
        // if we want X, but it is not valid, skip this implantation
        // cout << "Rejected decay, bec. we want Y position ok" << endl;
        return false;
    }
#endif

    //         cout << name_of_this_element << " map of decay, thrsh x = "
    //         <<  x_plate->give_decay_thrsh_position()
    //         << ", y = " <<  y_plate->give_decay_thrsh_position()
    //         << endl;


    // as this is the decay situation at first we need to
    // know if in this pixel there is some implantation
    //At first we will look in this pixel
    int xx = (int)(x_plate->give_decay_thrsh_position() / compression_factor_for_x_position) ;
    int yy = (int) y_plate->give_decay_thrsh_position();

    if((xx < 0 || xx >= (int) NR_OF_MUN_STOPPER_STRIPES_X) ||
            (yy < 0 || yy >= (int) NR_OF_MUN_STOPPER_STRIPES_Y))
    {
        cout << "Illegal coordinate of the decay pixel (X= " << xx
             << ", Y=" << yy  << ")" << endl;
        return false;
    }

    //   cout
    //   << name_of_this_element
    //   << " -------------------- map of decays thrsh, incr poiint x="
    //   <<xx << ", y= "   << yy << endl;

    spec_map_for_decays_thrsh->manually_increment(xx, yy);


    if(event_unpacked-> timestamp_FRS == 0)
    {
        static int counter;
        if(!((counter++) % 1000))
        {
            cout << "\n\n!!! WARNING: No TITRIS information in the decay event, (timestamp = 0) !!! \n" << endl;
        }

    }
    if(
        pixel[xx] [yy].are_you_in_time_gate(event_unpacked-> timestamp_FRS))
    {
        //cout << "Found exact match in the current pixel  (" << xx
        // << ", " << yy << ")" << endl;
        // success
        successful_match_impl_with_decay(xx, yy);
        return true;
    }
    else if(flag_match_only_same_pixel == false)
    {
        //cout << "Exact match not found, lets look at "
        // "the neighbours..." << endl;
        // check 8 neighbours around

        // we do not want to address elements < 0, or > 15
        int left_margin = -1;
        if(xx + left_margin < 0) left_margin = 0 ;
        int right_margin = 1;
        if(xx + right_margin >= (int) NR_OF_MUN_STOPPER_STRIPES_X) right_margin = 0 ;

        // same for y
        int down_margin = -1;
        if(yy + down_margin < 0) down_margin = 0 ;
        int up_margin = 1;
        if(yy + up_margin > (int) NR_OF_MUN_STOPPER_STRIPES_Y) up_margin = 0 ;


        for(int xm = left_margin ; xm <= right_margin ; xm++)
            for(int ym = down_margin ; ym <= up_margin  ; ym++)
            {
                if(xm == 0 && ym == 0) continue ;
                // bec. precise center case was made already above

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

    return false;
}
//***********************************************************************
//***********************************************************************
bool Tactive_stopper_munich::
active_stopper_pixel::
are_you_in_time_gate(long long current_time)
{
    if(!flag_valid_impl)  // no sensible implantation data
        return false;

    //     cout << "Current time = " << current_time
    //     << ", time of impl= " << timestamp_of_implantation << endl;

    long long time_diff = (current_time - timestamp_of_implantation);
    if(time_diff >
            give_implantation_decay_time_gate_high())
    {
        // too old implantation
        make_invalid();
        return 0;
    }
    if(time_diff <
            give_implantation_decay_time_gate_low())
    {
        // too young implantation -
        // should we make it invalid in this case? Ask Stephane: he sys NO
        return 0;
    }
    return true; // we are in the time gate, success
}
//***********************************************************************
void Tactive_stopper_munich::successful_match_impl_with_decay(int x, int y)
{
    /*
      cout << "Found successful matching the implantation: "
      << x_position_implantation
      << ", " << y_position_implantation
      << "  with  decay: " << x << ",  " << y << endl;
    */

#define PIX   pixel[x] [y]
    if(PIX.retrieve_implantation(
                &found_impl_timestamp,
                &found_impl_zet,
                &found_impl_zet2,

                & found_impl_aoq_sci_mw_corrected,
                & found_impl_aoq_sci_sci_corrected,
                & found_impl_aoq_sci_tpc_corrected,
                & found_impl_aoq_tic_mw_corrected,
                & found_impl_aoq_tic_sci_corrected,
                & found_impl_aoq_tic_tpc_corrected,

                &found_impl_sci42_energy,
                &found_impl_sci43_energy,
                &found_impl_sci21_position,
                &found_sci41horiz_position,// #$%
                &found_degrader_dE_gamma,
                &found_nr_of_times_retrieved



            ))
    {
        flag_matching_is_valid = true;
        if(found_nr_of_times_retrieved == 2)flag_ok_and_2nd_decay = true;

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
                 << ", aoq mw_corr= " <<  found_impl_aoq_sci_mw_corrected
                 << ", sci42 = " <<  found_impl_sci42_energy
                 << ", sci43 = " <<  found_impl_sci43_energy
                 << ", sci21_pos = " <<  found_impl_sci21_position
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


    }
    else
    {
        // not valid
        flag_matching_is_valid = false;
    }
#undef PIX

}
#endif // #ifdef MUN_ACTIVE_STOPPER_PRESENT
