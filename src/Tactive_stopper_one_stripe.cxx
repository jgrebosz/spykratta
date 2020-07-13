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


#include "Tactive_stopper_one_stripe.h"

#include "Tfile_helper.h"
#include "randomizer.h"

#include "Tactive_stopper_vector_strips.h"



#include "Tactive_stopper_full.h"
#include "Tnamed_pointer.h"
#include <sstream>

#include "TIFJEvent.h"

namespace active_stopper_namespace
{
// to distinguish two modes of operation implantation/decay
type_of_active_stopper_event   active_stopper_event_type;
}
using namespace active_stopper_namespace;
//********************************************************************
Tactive_stopper_one_stripe::Tactive_stopper_one_stripe(
    //   Tactive_stopper_vector_strips *ptr_owner,
    string name,
    int module_nr, int id_xy, int id_stripe_nr,
    bool flag_also_decay_)
    : Tfrs_element(name), Tincrementer_donnor(),
      // owner(ptr_owner),
      name_of_this_element(name),
      id_module(module_nr),
      id_x_or_y(id_xy),
      id_stripe(id_stripe_nr), flag_also_decay(flag_also_decay_)
{

    data_ptr = 0; // NULL;

    named_pointer[name_of_this_element +
                  "_energy_implantation_cal_when_fired"]
    = Tnamed_pointer(&energy_implantation_calibrated, &flag_fired, this) ;

    if(flag_also_decay)
        named_pointer[name_of_this_element +
                      "_energy_decay_cal_when_fired"]
        = Tnamed_pointer(&energy_decay_calibrated, &flag_fired, this) ;

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

Tactive_stopper_one_stripe::~Tactive_stopper_one_stripe()
{}
//********************************************************************

void Tactive_stopper_one_stripe::create_my_spectra()
{

    string name = name_of_this_element + "_energy_raw"  ;
    string folder("stopper/");
    spec_energy_raw = new spectrum_1D(name,
                                      name,
                                      (id_x_or_y ? 256 : 8192),  // x can be short (256 channels)
                                      0,
                                      (id_x_or_y ? 256 : 8192),  // x can be short (256 channels)
                                      folder, noraw);
    frs_spectra_ptr->push_back(spec_energy_raw) ;


    // calibrated spectra ++++++++++++++++++++++++++++++++++++++++++

    name = name_of_this_element + "_energy_implantation_cal"  ;
    spec_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        (id_x_or_y ? 256 : 8192),  // x can be short (256 channels)
                        0,
                        (id_x_or_y ? 256 : 8192),  // x can be short (256 channels)
                        folder, "", name_of_this_element +
                        "_energy_implantation_cal_when_fired");
    frs_spectra_ptr->push_back(spec_energy_implantation_cal) ;

    //---------------
    if(flag_also_decay)
    {
        name = name_of_this_element + "_energy_decay_cal"  ;
        spec_energy_decay_cal =
            new spectrum_1D(name,
                            name,
                            (id_x_or_y ? 256 : 8192),  // x can be short (256 channels)
                            0,
                            (id_x_or_y ? 256 : 8192),  // x can be short (256 channels)
                            folder, "", name_of_this_element +
                            "_energy_decay_cal_when_fired");
        frs_spectra_ptr->push_back(spec_energy_decay_cal) ;
    }

}
//*********************************************************************

void Tactive_stopper_one_stripe::analyse_current_event()
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

    energy_implantation_calibrated = energy_decay_calibrated = 0 ;


    energy_raw  =  *data_ptr ;
    if(!energy_raw)
    {
        mark_not_fired();
        return;
    }

    // cout << "Absorber  one stripe event data "
    //   << "  id_module= " << id_module
    //   << " id_x_or_y= " <<  id_x_or_y
    //   << " id_stripe= " << id_stripe
    //   << ", value = " << dec << energy_raw << endl;

    flag_fired = true;
    spec_energy_raw->manually_increment(energy_raw);
    // Piotr wants to have the incrementer similar to this what
    // increments fan spectrum
    pseudo_fan = // (400*id_module) +(20*id_x_or_y) +
        id_stripe + 100 ;   // we avoid 0 ?


    //   if(energy_raw)   cout << name_of_this_element
    //     << "  energy_raw = " << energy_raw;


    // Two kinds of calibration.
    // One is for implantation event, another for decay event

    // ################## at first implantation ---------------------------
    if(active_stopper_event_type == implantation_event)
    {

        // calibration of the energy -------------------------------
        energy_implantation_calibrated =
            (energy_implantation_cal_factors[1]  *
             (energy_raw + randomizer::randomek()))
            + energy_implantation_cal_factors[0]  ;

        spec_energy_implantation_cal->
        manually_increment(energy_implantation_calibrated);

    } // end of implantation

    //######################### DECAY ##########################
    if(flag_also_decay)
    {
        if(active_stopper_event_type == decay_event)
        {
            // calibration of the energy ---------------------------------
            energy_decay_calibrated  =
                (energy_decay_cal_factors[1] * (energy_raw +
                                                randomizer::randomek()))
                + energy_decay_cal_factors[0]  ;

            spec_energy_decay_cal->manually_increment(energy_decay_calibrated);

            //     cout
            //     << name_of_this_element << " energy_decay_calibrated = "
            //     << energy_decay_calibrated << endl;

        } // end of if decay
    }
}
//********************************************************************
//********************************************************************
void  Tactive_stopper_one_stripe::make_preloop_action(ifstream & plik)
{
    your_data_will_be_here(NULL);  // later must be set by a separate function

    // reading the calibration factors

    // in this file we look for a string
    int how_many_factors = 2 ;   // max linear calibration

    // implantation calibration factors ==============================
    string  slowo = name_of_this_element +
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
    if(flag_also_decay)
    {
        slowo = name_of_this_element + "_energy_decay_cal_factors" ;
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
    } // also decay
}

