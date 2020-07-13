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
#include "Tactive_stopper_full.h"

/*
TActive_stopper_full
         -----> Tactive_stopper_combined_xy_universal (to samo co: Tactive_stopper_combined_xy)
                   ------->Tactive_stopper_stripe

*/

#ifdef ACTIVE_STOPPER_PRESENT
#define PLEASE_COMPILE_THIS_FILE
#endif


#ifdef PLEASE_COMPILE_THIS_FILE



#include <sstream>
#include "Tnamed_pointer.h"
#include "Tfrs.h"
using namespace std;

#include "TIFJAnalysis.h" // for verbose

// static values
double Tactive_stopper_full::energy_cal_implantation_threshold;
double Tactive_stopper_full::energy_cal_decay_threshold;
double Tactive_stopper_full::energy_cal_implantation_threshold_upper;
double Tactive_stopper_full::energy_cal_decay_threshold_upper;


long long Tactive_stopper_full::implantation_decay_time_gate_low;
long long Tactive_stopper_full::implantation_decay_time_gate_high;
bool Tactive_stopper_full::flag_match_only_same_pixel; //otherwise also 8 neigbours around
// in case if some geometry is broken
//Tactive_stopper_full::enum_for_how_to_match_position  Tactive_stopper_full::how_to_match_position ;

//bool Tactive_stopper_full::flag_this_is_implantation_event ;
Tactive_stopper_full::type_of_active_stopper_event  Tactive_stopper_full::active_stopper_event_type;

// static
string Tactive_stopper_full::characters("LMR");  // symbolic names
int Tactive_stopper_full::how_many_implantations_in_this_event;

// ******************************************************************
Tactive_stopper_full::Tactive_stopper_full(std::string name, int id,
        int (TIFJEvent::*active_stopper_ptr)[HOW_MANY_STOPPER_MODULES]
        [NR_OF_STOPPER_STRIPES_X + NR_OF_STOPPER_STRIPES_Y],
#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
        int (TIFJEvent::*active_stopper_time_ptr)[HOW_MANY_STOPPER_MODULES]
        [NR_OF_STOPPER_STRIPES_X + NR_OF_STOPPER_STRIPES_Y],
#endif

        int (TIFJEvent::*active_stopper_fired_ptr)[
            HOW_MANY_STOPPER_MODULES][2],

        string name_aoq_value,       // <-- to take calculated aoq
        string name_zet_object,
        string name_zet2_object,
        string name_of_focal_plane,
        string name_of_first_good_scintillator,
        string name_of_second_veto_scintillator,
        string name_of_sci21_scintillator,
        string name_of_sci41_scintillator,
        string name_of_degrader_object
                                          ): Tfrs_element(name), id_nr(id),
    active_stopper(active_stopper_ptr),
#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
    active_stopper_time(active_stopper_time_ptr),
#endif

    active_stopper_fired(active_stopper_fired_ptr)
{
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

    if(!aoq_object || ! zet_object || ! zet2_object ||
            !focal_plane_object || !first_good_scintillator ||
            !second_veto_scintillator
            || !sci21_object || !sci41horiz_object || !degrader_object)
    {
        cout << "Cant find one of the collaborators of Active stopper. "
             "Look at the constructor... " << endl;
        exit(1);
    }


    distance = 0;
    //------------
    string name_of_module ;


    // stands for Left, Middle, Right
    for(unsigned int i = 0 ; i < HOW_MANY_STOPPER_MODULES ; i++)
    {
        //   name_of_module = name + '_' +  char('A' + i);
        name_of_module = name + '_' +  char(characters[ i]);

        // Tactive_stopper_combined_xy    *ppp =  new Tactive_stopper_combined_xy( this, name_of_module, i) ;

        module.push_back(new Tactive_stopper_combined_xy_universal(this, name_of_module, i, characters[i]));
        //     cout << "Created module " << name_of_module << " at address 0x"
        //     << hex <<  module[i]
        //     << endl;
    }

    // INCREMENTERS
    named_pointer[name_of_this_element +
                  "_how_many_(globally)_implantations_in_this_event"] =
                      Tnamed_pointer(&how_many_implantations_in_this_event, 0, this) ;

    //---------

    named_pointer[name_of_this_element + "_flag_trigger_of_implantation"] =
        Tnamed_pointer(&flag_trigger_of_implantation, 0, this) ;

    named_pointer[name_of_this_element + "_flag_trigger_of_decay"] =
        Tnamed_pointer(&flag_trigger_of_decay, 0, this) ;

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


    // // //   named_pointer[name_of_this_element
    // // //                 +"_decay_to_any_recent_implantation_time_difference_(in_ms)"] =
    // // //                   Tnamed_pointer(
    // // //                     &decay_to_any_recent_implantation_time_difference_in_ms,
    // // //                     &flag_trigger_of_decay, this) ;


#ifdef NIGDY


    named_pointer[name_of_this_element
                  + "_multiplicity_of_x_strips_above_thrs_during_calculation_of_the_position"]
    =
        Tnamed_pointer(
            &multiplicity_of_x_strips_above_thrs_during_calculation_of_the_position,
            &flag_matching_was_ok, this) ;

    named_pointer[name_of_this_element +
                  "_multiplicity_of_y_strips_above_thrs_during_calculation_of_the_position"] =
                      Tnamed_pointer(&multiplicity_of_y_strips_above_thrs_during_calculation_of_the_position,
                                     &flag_matching_was_ok, this) ;


    named_pointer[name_of_this_element +
                  "_nr_of_decays_when_ok"] =
                      Tnamed_pointer(&found_nr_of_times_retrieved,
                                     &flag_matching_was_ok, this) ;

#endif


#ifdef PLAMEN

    //  double plamen__module_0_stripe_n_en_impl_cal_MINUS_module_1_stripe_n_en_impl_cal[16];


    for(int n = 0 ; n < 16 ; n++)
    {
        ostringstream s(name_of_this_element);
        s << "_plamen__module_0_stripe_" << n
          << "_en_impl_cal_MINUS_module_1_stripe_"
          << n
          << "_en_impl_cal" ;

        named_pointer[s.str()] =
            Tnamed_pointer(
                &plamen__module_0_stripe_n_en_impl_cal_MINUS_module_1_stripe_n_en_impl_cal[n],
                0, this) ;
    }
#endif

    create_my_spectra();
}
// *********************************************************************
Tactive_stopper_full::~Tactive_stopper_full()
{
    save_spectra();
    for(unsigned int i = 0 ; i < module.size() ; i++)
    {
        delete module[i];
    }
}
// ********************************************************************
void Tactive_stopper_full::create_my_spectra()
{
    string folder("stopper/");

    string name = name_of_this_element +
                  "_multiplicity_of_modules_during_implantation"  ;
    spec_multiplicity_of_modules_implantation =
        new spectrum_1D(name,  name,
                        HOW_MANY_STOPPER_MODULES + 1, 0,
                        HOW_MANY_STOPPER_MODULES + 1,
                        folder,
                        "How many modules registered implantation (above"
                        " threshold)",
                        ""  //name_of_this_element+
                        // "_y_position_decay_above_threshold"
                       );
    give_spectra_pointer()->
    push_back(spec_multiplicity_of_modules_implantation);

    //-------------------
    name = name_of_this_element +
           "_multiplicity_of_modules_during_decay"  ;
    spec_multiplicity_of_modules_decay =
        new spectrum_1D(name,  name,
                        HOW_MANY_STOPPER_MODULES + 1, 0,
                        HOW_MANY_STOPPER_MODULES + 1,
                        folder,
                        "How many modules registered decay (above"
                        " threshold)",
                        ""
                       );
    give_spectra_pointer()->push_back(spec_multiplicity_of_modules_decay);

    //----------------------

    /*
    spec_map_of_implantations;
    spec_map_of_decays;
    spec_map_of_matching;
    */

    //----------------------------------- 2D -----------------;

    name = name_of_this_element + "_map_of_implantations_thrsh"  ;
    spec_map_of_implantations
    = new spectrum_2D(name,
                      name,
                      HOW_MANY_STOPPER_MODULES * NR_OF_STOPPER_STRIPES_X, 0,
                      HOW_MANY_STOPPER_MODULES * NR_OF_STOPPER_STRIPES_X,
                      NR_OF_STOPPER_STRIPES_Y, 0,
                      NR_OF_STOPPER_STRIPES_Y,
                      folder, "",
                      string("as X:") + name_of_this_element
                      + "_put_something here" +
                      ", as Y: " + name_of_this_element +
                      " put something here");

    give_spectra_pointer()->push_back(spec_map_of_implantations) ;

    //----------------
    name = name_of_this_element + "_map_of_decays_thrsh"  ;
    spec_map_of_decays =
        new spectrum_2D(name,
                        name,
                        HOW_MANY_STOPPER_MODULES * NR_OF_STOPPER_STRIPES_X, 0,
                        HOW_MANY_STOPPER_MODULES * NR_OF_STOPPER_STRIPES_X,
                        NR_OF_STOPPER_STRIPES_Y, 0,
                        NR_OF_STOPPER_STRIPES_Y,
                        folder, "",
                        string("as X:") +
                        name_of_this_element
                        + "_put_something here" +
                        ", as Y: " +
                        name_of_this_element +
                        " put something here");

    give_spectra_pointer()->push_back(spec_map_of_decays) ;

    //-------------------
    name = name_of_this_element +
           "_map_of_successful_matching_implantations_with_decays_thrsh"  ;
    spec_map_of_matching =
        new spectrum_2D(name,
                        name,
                        HOW_MANY_STOPPER_MODULES * NR_OF_STOPPER_STRIPES_X, 0,
                        HOW_MANY_STOPPER_MODULES * NR_OF_STOPPER_STRIPES_X,
                        NR_OF_STOPPER_STRIPES_Y, 0,
                        NR_OF_STOPPER_STRIPES_Y,
                        folder, "",
                        string("as X:") + name_of_this_element
                        + "_put_something here" +
                        ", as Y: " + name_of_this_element +
                        " put something here");

    give_spectra_pointer()->push_back(spec_map_of_matching) ;

}
// *********************************************************************
void Tactive_stopper_full::analyse_current_event()
{
    //   if(RisingAnalysis_ptr->is_verbose_on() )
    //         cout << "Tactive_stopper_full::analyse_current_event() for "
    //       << name_of_this_element
    //       <<  " #################################################### "
    //      << endl;

    //flag_this_is_implantation_event =
    //  event_unpacked->trigger_implantation;
    flag_matching_was_ok = false;
    impl_multiplicity_above_thrsh = 0;
    // how many implantation (in case of energies above thershol)
    decay_multiplicity_above_thrsh = 0;
    // how many implantation (in case of energies above thershol)

    flag_trigger_of_implantation = 0;
    flag_trigger_of_decay = 0;
    //   found_nr_of_times_retrieved = -1;

    // incrementers - ivalidating

    x_position_implantation_thrsh_ok = false;
    x_position_decay_thrsh_ok = false;
    y_position_implantation_thrsh_ok  = false;
    y_position_decay_thrsh_ok  = false;

    if(id_nr == 0) // only the first module ("stopper") is zeroing this counter
    {
        how_many_implantations_in_this_event = 0 ; // static value
        Tactive_stopper_combined_xy::clear_pattern_of_impantation_modules();
        Tactive_stopper_combined_xy::reset_pattern_modules();
    }
    switch(event_unpacked->trigger)
    {
        CASES_FOR_IMPLANTATION_TRIGGERS      // OLDER: case 2: case 4:
        active_stopper_event_type = implantation;
        flag_trigger_of_implantation = true;
        //     recent_implantation_event_timestamp = event_unpacked-> timestamp_FRS;
        if(RisingAnalysis_ptr->is_verbose_on())cout <<
                    "Implantation trigger -----------" << endl;
        break;

        CASES_FOR_DECAY_TRIGGERS          // OLDER: case 3: case 5:
        active_stopper_event_type = decay;
        flag_trigger_of_decay = 1;
        if(RisingAnalysis_ptr->is_verbose_on())cout <<
                    "Decay trigger -----------" << endl;
        break;
    default:
        active_stopper_event_type = undefined;
        break;
    }


    for(unsigned int i = 0 ; i < HOW_MANY_STOPPER_MODULES ; i++)
    {
        if(
            (event_unpacked->*active_stopper_fired)[i][0]
            ||
            (event_unpacked->*active_stopper_fired)[i][1]
        )
        {
            module[i]->analyse_current_event();
        }
        else module[i]->not_fired();
    } // end for

    // now we already know how many on modules fired


    if(active_stopper_event_type == implantation)
    {
        // checking the GOOD and VETO condition
        double good_en = give_first_good_sci_energy();
        double veto_en = give_second_veto_sci_energy();
        if(
            sci42_acceptable_energy_low <=
            good_en && good_en <= sci42_acceptable_energy_high  // GOOD
            &&
            !(sci43_veto_energy_low  <= veto_en && veto_en <=
              sci43_veto_energy_high)  // VETO
            && are_zet_aoq_legal()
        )
        {
            spec_multiplicity_of_modules_implantation->
            manually_increment(impl_multiplicity_above_thrsh);
            if((flag_accept_more_than_one_implantation &&
                    impl_multiplicity_above_thrsh > 1)
                    // when we accept more hits
                    ||
                    (impl_multiplicity_above_thrsh == 1))
                // when we accept only one
            {
                // L M R
                for(unsigned int i = 0 ; i < HOW_MANY_STOPPER_MODULES ; i++)
                {
                    module[i]->implantation_action();
                }
            }
        }
    }

    if(active_stopper_event_type == decay)
    {
        spec_multiplicity_of_modules_decay->manually_increment(
            decay_multiplicity_above_thrsh);

        //     decay_to_any_recent_implantation_time_difference_in_ms =
        //       (event_unpacked-> timestamp_FRS -
        //        recent_implantation_event_timestamp) / 50000;


        //     cout << "Recent imp TS = "
        //     << recent_implantation_event_timestamp
        //     << ", now TS = "
        //     << event_unpacked-> timestamp_FRS
        //     << ", diff TS = "
        //     << (event_unpacked-> timestamp_FRS -
        //      recent_implantation_event_timestamp)
        //     << " result = "
        //     << decay_to_any_recent_implantation_time_difference_in_ms
        //     << endl;

        //    if(decay_multiplicity_above_thrsh == 1)
        {
            for(unsigned int i = 0 ; i < HOW_MANY_STOPPER_MODULES ; i++)//LMR
            {
                if(module[i]->decay_action())
                {

#ifdef NIGDY
#endif  // NIGDY

                }
            }
        }
    }

#ifdef PLAMEN
    for(int n = 0 ; n < 16; n++)
    {
        plamen__module_0_stripe_n_en_impl_cal_MINUS_module_1_stripe_n_en_impl_cal[n] =
            module[0]->give_implantation_energy_cal(1 /*it_is_x*/, n)
            -
            module[1]->give_implantation_energy_cal(1 /*it_is_x*/, n);
    }
#endif // PLAMEN


    /*
      if(how_many_implantations_in_this_event > 1)
      {
        cout << "how many implantations are " << how_many_implantations_in_this_event
        << endl;

      }
      */

    calculations_already_done = true ;
}
// **********************************************************************
void Tactive_stopper_full::make_preloop_action(ifstream & plik_frs)   // read the calibration factors, gates
{
    // cout
    //   << "Tactive_stopper_full::make_preloop_action(ifstream & plik)"
    //   << endl;

    // read the options and read the calibration

    // we could read the distance hereTactive_stopper_combined_xy::pattern_of_impantation_modules
    // read the distance

    string name_of_this_element_without_hit = remove_substring_hitN_from_the_name(name_of_this_element);
    distance =
        Tfile_helper::find_in_file(plik_frs, name_of_this_element_without_hit  +
                                   "_distance");


    sci42_acceptable_energy_low = 0;
    sci42_acceptable_energy_high = 8192;
    sci43_veto_energy_low = 8191;
    sci43_veto_energy_high = 8191;
    flag_algorithm_for_implantation_position_calculation_is_mean  =  false;


    type_of_zet = 0;
    somezet_low = 1 ;
    somezet_high = 100;

    type_of_aoq = 0;
    someaoq_low = 0;
    someaoq_high = 4;

    energy_cal_implantation_threshold_upper = 8191;
    energy_cal_decay_threshold_upper = 8191;


    // --------------- options -------------------------
    // real reading the calibration -----------------

    string opt_filename = "options/active_stopper_settings.txt";
    try
    {
        ifstream plik(opt_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            /*      string mess = "I can't open file: "
            + opt_filename  ;
            throw mess ;*/
            cout << "I can't open file: "  << opt_filename << endl ;
            cout << "To create this file you need to run 'Cracow' and choose"
                 " the option: Spy_options->Active stopper settings\n"
                 "Then the dialog box will appear. It is enough to press 'OK' and"
                 " this file will be created." << endl;
            exit(1);
        }

        energy_cal_implantation_threshold  =
            Tfile_helper::find_in_file(plik,
                                       "threshold_for_implantation_energy");


        energy_cal_decay_threshold  =
            Tfile_helper::find_in_file(plik, "threshold_for_decay_energy");

        // remove this try after several months (july 2007)
        /*
           cout << "\n\n\n!!!!!!!!!!!!!!!!!!!!!!!! NOTE: !!!!!!!!!!!!!!!!!!!!!!\n"
           "the new option was added, so if you have an error "
           "here below, \ngo to CRACOW, open the menu dialog window: \n"
           "     Spy_options'->'Active stopper settings' \n"
           " and press the button OK\n" << endl;
        */

        energy_cal_implantation_threshold_upper  =
            Tfile_helper::find_in_file(plik,
                                       "threshold_upper_for_implantation_energy");
        energy_cal_decay_threshold_upper  =
            Tfile_helper::find_in_file(plik, "threshold_upper_for_decay_energy");




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

        // radio buttons for pixel search - only one
        // of them can be true (in an time)

        flag_match_only_same_pixel = (bool) Tfile_helper::find_in_file(
                                         plik, "only_same_pixel");
        // otherwise is :  also_8_neighbours  1

        flag_accept_more_than_one_implantation  = (bool)
                Tfile_helper::find_in_file(plik, "accept_more_implantations");

        try
        {
            sci42_acceptable_energy_low =
                Tfile_helper::find_in_file(plik, "sci42_acceptable_energy_low");
            sci42_acceptable_energy_high =
                Tfile_helper::find_in_file(plik, "sci42_acceptable_energy_high");
            sci43_veto_energy_low  =
                Tfile_helper::find_in_file(plik, "sci43_veto_energy_low");
            sci43_veto_energy_high   =
                Tfile_helper::find_in_file(plik, "sci43_veto_energy_high");

            flag_algorithm_for_implantation_position_calculation_is_mean  =
                (bool)    Tfile_helper::find_in_file(plik,
                        "algorithm_for_implantation_position_calculation_is_mean");
            // otherwise MAX


            // Zet vs Aoq condition
            type_of_zet = (int)Tfile_helper::find_in_file(plik, "type_of_zet");
            somezet_low =  Tfile_helper::find_in_file(plik, "some_zet_low");
            somezet_high = Tfile_helper::find_in_file(plik, "some_zet_high");

            type_of_aoq = (int)Tfile_helper::find_in_file(plik, "type_of_aoq");
            someaoq_low =  Tfile_helper::find_in_file(plik, "some_aoq_low");
            someaoq_high =  Tfile_helper::find_in_file(plik, "some_aoq_high");

        }
        catch(...)
        {
            cout << "In cracow open the dialog Spy_options->Active stopper"
                 " settings and press OK" << endl;
        }

        //====================

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
    //cout << name_of_this_element
    // << " options are successfully read " <<endl;


    // ------------- calibration -----------------------

    string cal_filename = "calibration/stopper_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            string mess = "I can't open calibration file: "
                          + cal_filename  ;
            throw mess ;
        }


        // first reading the energy calibration


        for(unsigned int i = 0 ; i < module.size() ; i++)
        {
            // //       cout << "reading calibration loop for active stopper " << name_of_this_element
            // //       << " module nr " << i << ", address = 0x" << hex << module[i] << endl ;
            module[i]->make_preloop_action(plik) ;
        }
    }  // end of try

    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the active stopper calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1) ; // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "2222 Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1) ; // is it healthy ?
    }
    catch(string sss)
    {
        cerr << " 3333 Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ; // is it healthy ?
    }

    // cout << "Active stopper  calibration successfully read"  << endl;


}

// ****** static function ****************************************************************
string Tactive_stopper_full::
remove_substring_hitN_from_the_name(string input)
{
    string::size_type pos  = input.find("_hit");

    if(pos != string::npos)
    {
        input.erase(pos, 5) ;   //  "_hit3"
    }

    return input;
}
#endif   // def ACTIVE_STOPPER_PRESENT
