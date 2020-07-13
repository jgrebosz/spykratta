//
// C++ Implementation: tstopper_stripe
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "Tactive_stopper_stripe.h"

#ifdef ACTIVE_STOPPER_PRESENT

#include "Tfile_helper.h"
#include "randomizer.h"

#define PLEASE_COMPILE_THIS_FILE_Tactive_stopper_stripe
#include "Tactive_stopper_combined_xy.h"
#endif

#ifdef PLEASE_COMPILE_THIS_FILE_Tactive_stopper_stripe


#include "Tactive_stopper_full.h"
#include "Tnamed_pointer.h"
#include <sstream>

#include "TIFJEvent.h"


//********************************************************************
Tactive_stopper_stripe::Tactive_stopper_stripe(
    Tactive_stopper_combined_xy_universal *ptr_owner,
    string name,
    int module_nr, int id_xy, int id_stripe_nr)
    : Tincrementer_donnor(), owner(ptr_owner),
      name_of_this_element(name),
      id_module(module_nr),
      id_x_or_y(id_xy),
      id_stripe(id_stripe_nr)
{

    named_pointer[name_of_this_element +
                  "_energy_implantation_cal_when_fired"]
    = Tnamed_pointer(&energy_implantation_calibrated, &flag_fired, this) ;

    named_pointer[name_of_this_element +
                  "_energy_raw_when_fired"]
    = Tnamed_pointer(&energy_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element +
                  "_energy_decay_cal_when_fired"]
    = Tnamed_pointer(&energy_decay_calibrated, &flag_fired, this) ;

#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
    named_pointer[name_of_this_element +
                  "_time_raw_when_fired"]
    = Tnamed_pointer(&time_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element +
                  "_time_cal_when_fired"]
    = Tnamed_pointer(&time_calibrated, &flag_fired, this) ;
#endif

    //  pseudo FAN for H-J
    string entry_name =  "ALL_stopper_det_stripe_pseudo_fan" ;
    Tmap_of_named_pointers::iterator  pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&pseudo_fan, &flag_fired, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&pseudo_fan, &flag_fired, this) ;

    create_my_spectra();
}
//********************************************************************
Tactive_stopper_stripe::~Tactive_stopper_stripe()
{}
//********************************************************************
void Tactive_stopper_stripe::create_my_spectra()
{

    string name = name_of_this_element + "_energy_raw"  ;
    string folder("stopper/");
    spec_energy_raw = new spectrum_1D(name,
                                      name,
                                      8192, 0, 8192,
                                      folder, noraw);
    owner->give_spectra_pointer()->push_back(spec_energy_raw) ;

    // calibrated spectra ++++++++++++++++++++++++++++++++++++++++++

    name = name_of_this_element + "_energy_implantation_cal"  ;
    spec_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        8192, 0, 8192,
                        folder, "", name_of_this_element +
                        "_energy_implantation_cal_when_fired");
    owner->give_spectra_pointer()->push_back(spec_energy_implantation_cal) ;

    //---------------
    name = name_of_this_element + "_energy_decay_cal"  ;
    spec_energy_decay_cal =
        new spectrum_1D(name,
                        name,
                        8192, 0, 8192,
                        folder, "", name_of_this_element +
                        "_energy_decay_cal_when_fired");
    owner->give_spectra_pointer()->push_back(spec_energy_decay_cal) ;

#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
    name = name_of_this_element + "_time_raw"  ;
    spec_time_raw = new spectrum_1D(name,
                                    name,
                                    8192, 0, 40000,
                                    folder, noraw);
    owner->give_spectra_pointer()->push_back(spec_time_raw) ;

    name = name_of_this_element + "_time_cal"  ;
    spec_time_cal =
        new spectrum_1D(name,
                        name,
                        8192, 0, 40000,
                        folder, "", name_of_this_element +
                        "_time_cal_when_fired");
    owner->give_spectra_pointer()->push_back(spec_time_cal) ;

#endif

}
//*********************************************************************
void Tactive_stopper_stripe::analyse_current_event()
{
    //      cout << "analyse_current_event() for "
    //    << name_of_this_element << endl;

    //
    //   if(energy_raw)
    //     cout << "   energy_raw = " << energy_raw;
    //   if(time_one)
    //     cout << "  time_one = " << time_one;
    //   if(time_two)
    //     cout << "  time_two = " << time_two;
    //   cout << endl;
    //

    flag_fired = true;
    energy_implantation_calibrated = energy_decay_calibrated = 0 ;
    //   cout << "asking for a active stopper event data "
    //   << "  id_module= " << id_module
    //   << " id_x_or_y= " <<  id_x_or_y
    //   << " id_stripe= " << id_stripe << endl;


#ifdef    ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
    time_calibrated = 0 ;

    if((time_raw  = (owner->give_owner_ptr())->
                    give_event_time_data(id_module, id_x_or_y, id_stripe)))
    {
//       cout << name_of_this_element  << ", time_raw = "
//       << time_raw << endl;

        spec_time_raw->manually_increment(time_raw);

        // calibration of time -------------------------------
        time_calibrated =
            (time_cal_factors[1]  *
             (time_raw + randomizer::randomek()))
            + time_cal_factors[0]  ;

        spec_time_cal-> manually_increment(time_calibrated);

    }

#endif

    if((energy_raw  = (owner->give_owner_ptr())->
                      give_event_data(id_module, id_x_or_y, id_stripe)))
    {
        /*    cout << "spec_energy_raw = " << hex << spec_energy_raw
            << ", value = " << dec << energy_raw << endl;
        */
        spec_energy_raw->manually_increment(energy_raw);
        // Piotr wants to have the incrementer similar to this what
        // increments fan spectrum
        pseudo_fan = (400 * id_module) + (20 * id_x_or_y) +
                     id_stripe + 100 ;   // we avoid 0 ?
    }

//         if(energy_raw)   cout << name_of_this_element
//    << "  energy_raw = " << energy_raw;


    // Two kinds of calibration.
    // One is for implantation event, another for decay event

    // ################## at first implantation ---------------------------
    if(Tactive_stopper_full::active_stopper_event_type ==
            Tactive_stopper_full::implantation)
    {

        // calibration of the energy -------------------------------
        energy_implantation_calibrated =
            (energy_implantation_cal_factors[1]  *
             (energy_raw + randomizer::randomek()))
            + energy_implantation_cal_factors[0]  ;

        spec_energy_implantation_cal->
        manually_increment(energy_implantation_calibrated);

//         if(energy_raw)   cout << name_of_this_element
//    << "  energy_implantation_calibrated = " << energy_implantation_calibrated
//    << endl;

        // multiplicity statistics
        if(id_x_or_y == 0)  // if this is X
        {
//       owner->strip_x_energies_implantation_cal.push_back(
//         energy_implantation_calibrated);
            if(
                Tactive_stopper_full::is_energy_cal_implantation_in_gate(energy_implantation_calibrated))
            {
                owner->
                x_multiplicity_for_energy_cal_implantation_above_threshold++;

                owner->strip_x_energies_implantation_cal.push_back(
                    energy_implantation_calibrated);


            }
            owner->  x_multiplicity_for_energy_cal++;
        }
        else   // it is Y
        {
//       owner->strip_y_energies_implantation_cal.push_back(
//         energy_implantation_calibrated);
            if(
                Tactive_stopper_full::is_energy_cal_implantation_in_gate(
                    energy_implantation_calibrated))
            {
                owner->
                y_multiplicity_for_energy_cal_implantation_above_threshold++;

                owner->strip_y_energies_implantation_cal.push_back(
                    energy_implantation_calibrated);
            }
            owner->y_multiplicity_for_energy_cal++;
        }

    } // end of implantation

    //######################### DECAY ##########################

    if(Tactive_stopper_full::active_stopper_event_type ==
            Tactive_stopper_full::decay)
    {
        // calibration of the energy ---------------------------------
        energy_decay_calibrated  =
            (energy_decay_cal_factors[1] * (energy_raw +
                                            randomizer::randomek()))
            + energy_decay_cal_factors[0]  ;

        spec_energy_decay_cal->manually_increment(energy_decay_calibrated);

//           if(energy_raw)   cout << name_of_this_element
//    << "  energy_decay_calibrated = " << energy_decay_calibrated << endl;


        if(id_x_or_y == 0)  // if this is X
        {
            owner->strip_x_energies_decay_cal.push_back(
                energy_decay_calibrated);

            if(Tactive_stopper_full::is_energy_cal_decay_in_gate(energy_decay_calibrated))
                owner->x_multiplicity_for_energy_cal_decay_above_threshold++;

            owner->x_multiplicity_for_energy_cal++;
        }
        else   // it is Y
        {
            owner->strip_y_energies_decay_cal.push_back(
                energy_decay_calibrated);
            if(
                Tactive_stopper_full::is_energy_cal_decay_in_gate(energy_decay_calibrated))
                owner->y_multiplicity_for_energy_cal_decay_above_threshold++;

            owner->y_multiplicity_for_energy_cal++;
        }
    } // end of if decay
}
//********************************************************************
void  Tactive_stopper_stripe::make_preloop_action(ifstream & plik)
{
    // reading the calibration factors

    // in this file we look for a string
    int how_many_factors = 2 ;   // max linear calibration

    // implantation calibration factors ==============================

    string name_of_this_element_without_hit =
        Tactive_stopper_full::remove_substring_hitN_from_the_name(name_of_this_element);

    string  slowo = name_of_this_element_without_hit +
                    "_energy_implantation_cal_factors" ;

    Tfile_helper::spot_in_file(plik, slowo);

    energy_implantation_cal_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        plik >> zjedz >> value ;

        if(!plik)
        {
            string mess ;
            mess += "I found keyword: " + slowo
                    + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok
        energy_implantation_cal_factors.push_back(value) ;
    }

    if(energy_implantation_cal_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for (implantation) energy calibration of " +
                      name_of_this_element ;
        throw mess ;
    }

    // Decay calibration factors ========================================

    slowo = name_of_this_element_without_hit + "_energy_decay_cal_factors" ;
    Tfile_helper::spot_in_file(plik, slowo);
    energy_decay_cal_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        plik >> zjedz >>  value ;
        if(!plik)
        {
            ostringstream mess ;
            mess << "I found keyword: " << slowo
                 << ", but error was in reading its factor nr." << i  ;
            throw mess.str() ;
        }
        // if ok
        energy_decay_cal_factors.push_back(value) ;
    }

    if(energy_decay_cal_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for (decay) energy calibration of " +
                      name_of_this_element ;
        throw mess ;
    }

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
    // time calibration factors ========================================

    slowo = name_of_this_element_without_hit + "_time_cal_factors" ;
    Tfile_helper::spot_in_file(plik, slowo);
    time_cal_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        plik >> zjedz >>  value ;
        if(!plik)
        {
            ostringstream mess ;
            mess << "I found keyword: " << slowo
                 << ", but error was in reading its factor nr." << i  ;
            throw mess.str() ;
        }
        // if ok
        time_cal_factors.push_back(value) ;
    }

    if(time_cal_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for time calibration of " +
                      name_of_this_element ;
        throw mess ;
    }
#endif // define ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

}
#endif // ifdef PLEASE_COMPILE_THIS_FILE

