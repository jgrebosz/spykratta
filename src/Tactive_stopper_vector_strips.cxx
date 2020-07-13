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
#include "Tactive_stopper_vector_strips.h"

//#ifdef ACTIVE_STOPPER_PRESENT

#include <sstream>
#include "Tnamed_pointer.h"
using namespace std;
using namespace active_stopper_namespace;

//#include "Tactive_stopper_full.h"
#include "TIFJAnalysis.h" // for verbose
#include <algorithm>
// ******************************************************************
Tactive_stopper_vector_strips::Tactive_stopper_vector_strips(
    std::string name,
    unsigned int nr,
    const int const_nr_of_strips_, bool flag_we_want_also_decay_):
    Tfrs_element(name), module_id_nr(nr),
    const_nr_of_strips(const_nr_of_strips_),
    flag_algorithm_for_implantation_position_calculation_is_mean(true),
    flag_we_want_also_decay(flag_we_want_also_decay_)
{
    string name_of_stripe ;
    //     symbol = string(1, symbol_);
    array_of_data = 0;

    energy_cal_implantation_threshold = 0;
    energy_cal_implantation_threshold_upper = 8192;

    energy_cal_decay_threshold  = 0;
    energy_cal_decay_threshold_upper = 8192;

    flag_algorithm_for_implantation_position_calculation_is_mean  = true;

    //-----------------  ------------------------
    int how_many_digits = const_nr_of_strips > 100 ? 3 : 2 ;
    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
        // stopper_m0_01
    {
        ostringstream out ;
        // what about leading zeros
        out << name << "_"
            // << symbol << "_"
            <<  setfill('0') << setw(how_many_digits) << i ;
        name_of_stripe = out.str() ;
        //     cout << " name is : " << name_of_stripe << endl;
        stripe.push_back(new Tactive_stopper_one_stripe(
                             name_of_stripe, module_id_nr, 0, i, flag_we_want_also_decay));
    }

    // INCREMENTERS

    named_pointer[name_of_this_element + "_nr_1st_energy_implantation_cal"]
    = Tnamed_pointer(&nr_1st_energy_implantation_cal, 0, this) ;

    named_pointer[name_of_this_element + "_2nd_energy_implantation_cal"] =
        Tnamed_pointer(&nr_2nd_energy_implantation_cal, 0, this) ;

    named_pointer[name_of_this_element + "_nr_3rd_energy_implantation_cal"] =
        Tnamed_pointer(&nr_3rd_energy_implantation_cal, 0, this) ;

    if(flag_we_want_also_decay)
    {
        named_pointer[name_of_this_element + "_nr_1st_energy_decay_cal"]
        = Tnamed_pointer(&nr_1st_energy_decay_cal, 0, this) ;

        named_pointer[name_of_this_element + "_2nd_energy_decay_cal"] =
            Tnamed_pointer(&nr_2nd_energy_decay_cal, 0, this) ;

        named_pointer[name_of_this_element + "_nr_3rd_energy_decay_cal"] =
            Tnamed_pointer(&nr_3rd_energy_decay_cal, 0, this) ;
    }

    named_pointer[name_of_this_element + "_rms_implantation"] =
        Tnamed_pointer(&rms_implantation,
                       &position_implantation_ok, this) ;

    if(flag_we_want_also_decay)
    {

        named_pointer[name_of_this_element + "_rms_decay"] =
            Tnamed_pointer(&rms_decay, &position_decay_ok, this) ;

    }
    // thrshlds -----------
    named_pointer[name_of_this_element + "_rms_implantation_thrsh"] =
        Tnamed_pointer(&rms_implantation_thrsh,
                       &position_implantation_thrsh_ok, this) ;

    if(flag_we_want_also_decay)
    {
        named_pointer[name_of_this_element + "_rms_decay_thrsh"] =
            Tnamed_pointer(&rms_decay_thrsh,
                           &position_decay_thrsh_ok, this) ;
    }

    // mult

    named_pointer[name_of_this_element +
                  "_multiplicity_for_energy_cal_implantation_above_threshold"] =
                      Tnamed_pointer(
                          &impl_multiplicity_above_thrsh,
                          0, this) ;



    if(flag_we_want_also_decay)
    {
        named_pointer[name_of_this_element +
                      "_multiplicity_for_energy_cal_decay_above_threshold"] =
                          Tnamed_pointer(
                              &decay_multiplicity_above_thrsh,
                              0, this) ;
    }


    //     named_pointer[name_of_this_element+"_multiplicity_for_energy_cal"] =
    //         Tnamed_pointer(&multiplicity_for_energy_cal, 0, this) ;



    //====================== positions

//   named_pointer[name_of_this_element+"_position_implantation"] =
//     Tnamed_pointer(
//       &position_implantation, &position_implantation_ok, this) ;
//
//   if(flag_we_want_also_decay)
//   {
//     named_pointer[name_of_this_element+"_position_decay"] =
//       Tnamed_pointer(&position_decay, &position_decay_ok, this) ;
//   }

    // ----- thrsh

    named_pointer[name_of_this_element +
                  "_position_implantation_above_threshold"] =
                      Tnamed_pointer(
                          &position_implantation_thrsh,
                          &position_implantation_thrsh_ok, this) ;

    if(flag_we_want_also_decay)
    {
        named_pointer[name_of_this_element +
                      "_position_decay_above_threshold"] =
                          Tnamed_pointer(
                              &position_decay_thrsh,
                              &position_decay_thrsh_ok, this) ;
    }

    named_pointer[name_of_this_element +
                  "_implantation_sum_energy_stripes"] =
                      Tnamed_pointer(&denominator_implantation, 0, this) ;


    if(flag_we_want_also_decay)
    {
        named_pointer[name_of_this_element +
                      "_decay_sum_energy_stripes"] =
                          Tnamed_pointer(&denominator_decay, 0, this) ;
    }

    //---------------



    // INCREMENTERS----------------

    string entry_name;
    Tmap_of_named_pointers::iterator pos;

    named_pointer[name_of_this_element
                  + "_position_implantation_above_threshold"] =
                      Tnamed_pointer(
                          &position_implantation_thrsh,
                          &position_implantation_thrsh_ok, this) ;

    if(flag_we_want_also_decay)
    {
        named_pointer[name_of_this_element
                      + "_position_decay_above_threshold"] =
                          Tnamed_pointer(&position_decay_thrsh,
                                         &position_decay_thrsh_ok, this) ;
    }

    create_my_spectra();
}
// *********************************************************************
Tactive_stopper_vector_strips::~Tactive_stopper_vector_strips()
{
    //    save_spectra();
    for(unsigned int i = 0 ; i < stripe.size() ; i++)
    {
        delete stripe[i];
    }
}
// ***********************************************************************
void Tactive_stopper_vector_strips::create_my_spectra()
{
    string name = name_of_this_element + "_fan"  ;
    string folder("stopper/");
    spec_fan =
        new spectrum_1D(name,
                        name,
                        (const_nr_of_strips * 10) + 10, 0, (const_nr_of_strips * 10) + 10,
                        folder,
                        "stripes, (each stripe marks the triggers:"
                        "   *1- implantation, *2 - decay, "
                        "*3 -undefined");

    give_spectra_pointer()->push_back(spec_fan) ;


    name = name_of_this_element + "_fan_above_thresholds"  ;

    spec_fan_above_thresholds =
        new spectrum_1D(name,
                        name,
                        (const_nr_of_strips * 10) + 10, 0, (const_nr_of_strips * 10) + 10,
                        folder,
                        "when above implantion and decay "
                        "thresholds (triggers:   *1- "
                        "implantation, *2 - decay, *3 "
                        "-undefined]");

    give_spectra_pointer()->push_back(spec_fan_above_thresholds) ;


    //-----------------  -----------------------------------------------
    name = name_of_this_element + "_nr_1st_max_energy_implantation_cal"  ;
    spec_nr_1st_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Maximal value of energy");

    give_spectra_pointer()->push_back(spec_nr_1st_energy_implantation_cal) ;

    //-----------------

    name = name_of_this_element + "_2nd_max_energy_implantation_cal"  ;
    spec_nr_2nd_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Second maximal value of energy");

    give_spectra_pointer()->push_back(spec_nr_2nd_energy_implantation_cal) ;
    //-----------------

    name = name_of_this_element + "_nr_3rd_max_energy_implantation_cal"  ;
    spec_nr_3rd_energy_implantation_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Third maximal value of energy");

    give_spectra_pointer()->push_back(spec_nr_3rd_energy_implantation_cal) ;


    if(flag_we_want_also_decay)
    {
        // --------------- LINLOG --------------------
        name = name_of_this_element + "_nr_1st_max_energy_decay_cal"  ;
        spec_nr_1st_energy_decay_cal =
            new spectrum_1D(name,
                            name,
                            4096, 0, 4096,
                            folder,
                            "Maximal value of energy");

        give_spectra_pointer()->push_back(spec_nr_1st_energy_decay_cal) ;

        //-----------------

        name = name_of_this_element + "_nr_2nd_max_energy_decay_cal"  ;
        spec_nr_2nd_energy_decay_cal =
            new spectrum_1D(name,
                            name,
                            4096, 0, 4096,
                            folder,
                            "Second maximal value of energy");

        give_spectra_pointer()->push_back(spec_nr_2nd_energy_decay_cal) ;
        //-----------------

        name = name_of_this_element + "_nr_3rd_max_energy_decay_cal"  ;
        spec_nr_3rd_energy_decay_cal =
            new spectrum_1D(name,
                            name,
                            4096, 0, 4096,
                            folder,
                            "Third maximal value of energy");

        give_spectra_pointer()->push_back(spec_nr_3rd_energy_decay_cal) ;
    }

    //-----------------
    // multiplicity spectra ===================================
    // spectra multiplicity with threshold

    //--------------------------------- IMPLANT
    name = name_of_this_element +
           "_multiplicity_for_energy_cal_implantation_above_threshold";
    spec_multiplicity_for_energy_cal_implantation_above_threshold =
        new spectrum_1D(name,  name,
                        const_nr_of_strips + 1, 0,
                        const_nr_of_strips + 1,
                        folder,
                        "multiplicity_for_energy_cal_implan"
                        "tation_above_threshold",
                        name_of_this_element +
                        "_multiplicity_for_energy_cal_implan"
                        "tation_above_threshold");
    give_spectra_pointer()->push_back(
        spec_multiplicity_for_energy_cal_implantation_above_threshold);

    //-----------------------DECAY
    if(flag_we_want_also_decay)
    {

        name = name_of_this_element +
               "_multiplicity_for_energy_cal_decay_above_threshold"  ;
        spec_multiplicity_for_energy_cal_decay_above_threshold =
            new spectrum_1D(name,  name,
                            const_nr_of_strips + 1, 0,
                            const_nr_of_strips + 1,
                            folder,
                            "multiplicity_for_energy_cal_decay_abo"
                            "ve_threshold",
                            name_of_this_element +
                            "_multiplicity_for_energy_cal_decay_abo"
                            "ve_threshold"
                           );
        give_spectra_pointer()->push_back(
            spec_multiplicity_for_energy_cal_decay_above_threshold);
    }

    // spectra multiplicity without threshold
    //---------------------------------
    name = name_of_this_element + "_multiplicity_for_energy_cal"  ;
    spec_multiplicity_for_energy_cal =
        new spectrum_1D(name,  name,
                        const_nr_of_strips + 1, 0,
                        const_nr_of_strips + 1,
                        folder,
                        "without threshold, (implantation, decay or"
                        " undefined trigger)",
                        name_of_this_element +
                        "_multiplicity_for_energy_cal");
    give_spectra_pointer()->push_back(
        spec_multiplicity_for_energy_cal);

    //-------------- POSITIONS --------------------

    //   name = name_of_this_element + "_position_implantation"  ;
    //   spec_position_implantation =
    //     new spectrum_1D( name,  name,
    //                      const_nr_of_strips*10, 0,
    //                      const_nr_of_strips ,
    //                      folder,
    //                      "position from lin calibration (without threshold)",
    //                      name_of_this_element+
    //                      "_position_implantation");
    //   give_spectra_pointer()->push_back(spec_position_implantation);

    //-----------------------------
    //   if(flag_we_want_also_decay)
    //   {
    //     name = name_of_this_element + "_position_decay"  ;
    //     spec_position_decay =
    //       new spectrum_1D( name,  name,
    //                        const_nr_of_strips*10, 0,
    //                        const_nr_of_strips ,
    //                        folder,
    //                        "position from decay calibration (wi"
    //                        "thout threshold)",
    //                        name_of_this_element+"_position_decay");
    //     give_spectra_pointer()->push_back(spec_position_decay);
    //   }

    //================== with Threshold ===================
    //-------------- POSITIONS --------------------

    name = name_of_this_element + "_position_implantation_thrsh"  ;
    spec_position_implantation_thrsh =
        new spectrum_1D(name,  name,
                        const_nr_of_strips * 10, 0,
                        const_nr_of_strips ,
                        folder,
                        "position from lin calibration (wit"
                        "h threshold)",
                        name_of_this_element +
                        "_position_implantation_above_threshold");
    give_spectra_pointer()->push_back(spec_position_implantation_thrsh);

    //-----------------------------------

    if(flag_we_want_also_decay)
    {
        name = name_of_this_element + "_position_decay_thrsh"  ;
        spec_position_decay_thrsh =
            new spectrum_1D(name,  name,
                            const_nr_of_strips * 10, 0,
                            const_nr_of_strips ,
                            folder,
                            "position from decay calibration (with threshold)",
                            name_of_this_element +
                            "_position_decay_above_threshold");
        give_spectra_pointer()->push_back(spec_position_decay_thrsh);
    }
}
//*************************************************************************************************
void Tactive_stopper_vector_strips::analyse_current_event()
{
    //     if(RisingAnalysis_ptr->is_verbose_on() )
    //         cout <<
    //         "Tactive_stopper_vector_strips::analyse_current_event()  -------------"
    //         << name_of_this_element
    //         << endl;

    // incrementers
    strip_energies_implantation_cal.clear();
    strip_energies_decay_cal.clear();

    sum_energies_above_threshold_when_decay = 0;

    nr_1st_energy_implantation_cal =
        nr_2nd_energy_implantation_cal =
            nr_3rd_energy_implantation_cal =

                impl_multiplicity_above_thrsh =
                    impl_multiplicity =
                        decay_multiplicity =
                            decay_multiplicity_above_thrsh  =  0 ;

    position_implantation =
        position_implantation_thrsh =

            position_decay =
                position_decay_thrsh =     -9999;

    int channel = 3 ;

    if(active_stopper_event_type ==   implantation_event)
        channel = 1 ;
    else if(active_stopper_event_type ==   decay_event)
        channel = 2;

    //   if(channel == 3)
    //   {
    //     cout << "undefined type of event (active_stopper_event_type) " << endl;
    //   }

    double en = 0; // for tmp results

    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
    {
        /*  cout << i << ")  decay_multiplicity_above_thrsh = "
          << decay_multiplicity_above_thrsh
          << endl; */

        stripe[i]->analyse_current_event();

        if(stripe[i]->give_fired())
        {
            spec_fan->manually_increment((int) i * 10  + 0 + channel);

            switch(active_stopper_event_type)
            {
            case implantation_event:
                impl_multiplicity++;
                en =   stripe[i]->give_energy_implantation_cal();

                if(is_energy_cal_implantation_in_gate(en))
                {
                    strip_energies_implantation_cal.push_back(en); // to avoid putting the overflow value, we register only these
                    // which are in the gate

                    impl_multiplicity_above_thrsh++;
                    spec_fan_above_thresholds->manually_increment(
                        (int) i   * 10  + 0 + channel);
                }
                break;

            case decay_event:
                if(flag_we_want_also_decay)
                {
                    decay_multiplicity++;
                    en =   stripe[i]->give_energy_decay_cal();

                    if(is_energy_cal_decay_in_gate(en))
                    {
                        strip_energies_decay_cal.push_back(en);
                        decay_multiplicity_above_thrsh++;

                        sum_energies_above_threshold_when_decay += en;
                        spec_fan_above_thresholds->manually_increment(
                            (int) i * 10 + 0 + channel);
                    }
                }
                break;

            case undefined_event:
                break;
            } // end switch

        } // if fired
        //-------------------------------------------------------------
    } // end for analysis of all the stripes=============


    // Now more general --------------
    if(active_stopper_event_type ==
            implantation_event)
    {

        // ====================== Highest Energies spectra ===============
        // sorting the energy results
        sort(strip_energies_implantation_cal.begin(),
             strip_energies_implantation_cal.end());

        // incrementing two spectra with the highest energies
        if(strip_energies_implantation_cal.size())
        {
            nr_1st_energy_implantation_cal =
                strip_energies_implantation_cal[
                    strip_energies_implantation_cal.size() - 1];
            spec_nr_1st_energy_implantation_cal->
            manually_increment(nr_1st_energy_implantation_cal);
            if(RisingAnalysis_ptr->is_verbose_on())
                cout << "nr_1st_energy_implantation_cal = "
                     << nr_1st_energy_implantation_cal << endl;
        }

        if(strip_energies_implantation_cal.size() >= 2)
        {
            nr_2nd_energy_implantation_cal =
                strip_energies_implantation_cal[
                    strip_energies_implantation_cal.size() - 2];
            spec_nr_2nd_energy_implantation_cal->
            manually_increment(nr_2nd_energy_implantation_cal);
        }

        if(strip_energies_implantation_cal.size() >= 3)
        {
            nr_3rd_energy_implantation_cal =
                strip_energies_implantation_cal[
                    strip_energies_implantation_cal.size() - 3];
            spec_nr_3rd_energy_implantation_cal->
            manually_increment(nr_3rd_energy_implantation_cal);
        }
        // incrementing multiplicity spectra
        // spectra multiplicity with threshold
        spec_multiplicity_for_energy_cal_implantation_above_threshold->
        manually_increment(
            impl_multiplicity_above_thrsh);
    } // end of if implantation


    //##########################################
    // ---- same for decay
    if(active_stopper_event_type ==
            decay_event
            &&
            flag_we_want_also_decay)
    {
        sort(strip_energies_decay_cal.begin(),
             strip_energies_decay_cal.end());

        // incrementing two spectra with the highest energies
        if(strip_energies_decay_cal.size())
        {
            nr_1st_energy_decay_cal =
                strip_energies_decay_cal[strip_energies_decay_cal.size() - 1];
            spec_nr_1st_energy_decay_cal->
            manually_increment(nr_1st_energy_decay_cal);
        }

        if(strip_energies_decay_cal.size() >= 2)
        {
            nr_2nd_energy_decay_cal =
                strip_energies_decay_cal[strip_energies_decay_cal.size() - 2];
            spec_nr_2nd_energy_decay_cal->
            manually_increment(nr_2nd_energy_decay_cal);
        }

        if(strip_energies_decay_cal.size() >= 3)
        {
            nr_3rd_energy_decay_cal =
                strip_energies_decay_cal[strip_energies_decay_cal.size() - 3];
            spec_nr_3rd_energy_decay_cal->
            manually_increment(nr_3rd_energy_decay_cal);
        }
        // cout << name_of_this_element
        //<< "decay_multiplicity_above_thrsh "
        // << decay_multiplicity_above_thrsh
        // << endl;

        spec_multiplicity_for_energy_cal_decay_above_threshold->
        manually_increment(
            decay_multiplicity_above_thrsh);

        //    spec_decay_multiplicity_above_thrsh->
        //     manually_increment(5);
    } // end of decay


    // spectra multiplicity without threshold
    //     spec_multiplicity_for_energy_cal->
    //     manually_increment(multiplicity_for_energy_cal);

    //   cout << "in this event the y mult was = "
    // << y_multiplicity_for_energy_cal << endl;


    switch(active_stopper_event_type)
    {
    case implantation_event:
        calculate_position_and_RMS_for_implantation();
        if(position_implantation_thrsh_ok)
            impl_multiplicity_above_thrsh++;
        //implantation_action();
        break;

    case decay_event:
        if(flag_we_want_also_decay)
        {
            calculate_position_and_RMS_for_decay();
            if(position_decay_thrsh_ok)
                decay_multiplicity_above_thrsh++;
        }

        break;

    case undefined_event:
    default:
        break; // nothing to do
    }

    calculations_already_done = true ;
}
// **********************************************************************
// read the calibration factors, gates
void Tactive_stopper_vector_strips::make_preloop_action(ifstream & plik)
{
    //cout
    //<< "Tactive_stopper_vector_strips::make_preloop_action(ifstream & plik)"
    //<< endl;

    /*
      string opt_filename = "options/active_stopper_settings.txt";
      try
      {
        ifstream plik(opt_filename.c_str() ) ; // ascii file
        if(!plik)
        {
          string mess = "I can't open file: "
                        + opt_filename  ;
          throw mess ;
        }

        //char znak[] = { 'L', 'M', 'R' };

    //       std::ostringstream s;
    //         s << znak[module_id_nr] ;
    //        string symbol_of_this_plate = s.str();

        string symbol_of_this_plate = symbol;
        // radio buttons for Y information to be used
        //    cout << "symbol = >" << symbol << "<, symbol_of_plate = >"
    // << symbol_of_this_plate
    // << "<" << endl;
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

    */
    zero_flags_fired_vectors();

    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
    {
        //cout << "reading calibration loop for active stopper segment"
        //<< i << endl ;
        stripe[i]->make_preloop_action(plik) ;
        //        // int * ptr = array_of_data;
        //         stripe[i]->your_data_will_be_here( array_of_data);
    }
    //  cout << "Active stopper  calibration successfully read"  << endl;

}
//*********************************************************************
/*!
    \fn Tactive_stopper_vector_strips::
    zero_flags_fired_vectors()
 */
void Tactive_stopper_vector_strips::zero_flags_fired_vectors()
{
    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
        stripe[i]->mark_not_fired();

    //     decay_sum_energy_both_xy_stripes =
    denominator_decay = 0.0;
    denominator_implantation = 0;
}
//********************************************************************
/*!
    \fn Tactive_stopper_vector_strips::
    calculate_position_and_RMS_for_implantation()
 */
void Tactive_stopper_vector_strips::calculate_position_and_RMS_for_implantation()
{
    // mianownik = denominator
    // licznik = numerator

    //double
    denominator_implantation = 0;  // made a memeber of the class
    double numerator_implantation  = 0;
    double numerator2_implantation  = 0;

    double denominator_implantation_thrsh = 0;
    double numerator_implantation_thrsh  = 0;
    double numerator2_implantation_thrsh  = 0;

    rms_implantation =
        rms_decay =
            rms_implantation_thrsh =
                rms_decay_thrsh = 0;


    double en = 0;


    int nr_max = 0;
    double max_en = 0;


    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
    {

        en =  stripe[i]->give_energy_implantation_cal();
        numerator_implantation += i  * en;
        denominator_implantation += en;


        // threshold
        en = stripe[i]->give_energy_implantation_cal();
        if(RisingAnalysis_ptr->is_verbose_on())
            cout << "stripe  " << i << " had implantation  energy  " << en
                 << (

                     is_energy_cal_implantation_in_gate(en)
                     ?  " <--- in gate " :
                     "")
                 << endl;

        if(is_energy_cal_implantation_in_gate(en))
        {
            numerator_implantation_thrsh += i  * en;
            denominator_implantation_thrsh += en;
            if(max_en  < en)
            {
                nr_max = i ;
                max_en = en;
            }
        }
    }

    if(RisingAnalysis_ptr->is_verbose_on())
    {
        if(max_en)
            cout << "Max en  " << max_en
                 << " was form  strip nr " <<  nr_max << endl;

        cout << " sum energy of  stripes =" << denominator_implantation
             << endl;
    }


    //-----------------------
    if(denominator_implantation)
    {
        position_implantation =
            numerator_implantation / denominator_implantation;
        //     spec_position_implantation->
        //     manually_increment(position_implantation);

        position_implantation_ok  = true;
    }
    else
        position_implantation_ok  = false;



    // threshold==========================================
    if(denominator_implantation_thrsh)
    {
        position_implantation_thrsh =
            numerator_implantation_thrsh / denominator_implantation_thrsh;
        //     spec_position_implantation_thrsh->
        //     manually_increment(position_implantation_thrsh);
        if(RisingAnalysis_ptr->is_verbose_on())
            cout <<
                 "So calcultated  position = " << position_implantation_thrsh
                 << endl;
        position_implantation_thrsh_ok  = true;
    }
    else
        position_implantation_thrsh_ok  = false;


    // ------- RMS ------------------------------------------
    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
    {

        if(stripe[i]->give_fired())
        {
            numerator2_implantation +=
                pow((position_implantation  - i), 2)  *
                stripe[i]->give_energy_implantation_cal();

            if(
                is_energy_cal_implantation_in_gate(
                    stripe[i]->give_energy_implantation_cal()))
            {
                numerator2_implantation_thrsh +=
                    pow((position_implantation_thrsh  - i), 2)  *
                    stripe[i]->give_energy_implantation_cal();
            }
        }
    }


    if(denominator_implantation)
        rms_implantation =
            sqrt(numerator2_implantation / denominator_implantation);


    if(denominator_implantation_thrsh)
        rms_implantation_thrsh =
            sqrt(numerator2_implantation_thrsh /
                 denominator_implantation_thrsh);

    // Par force - another way of calculating the position
    if(flag_algorithm_for_implantation_position_calculation_is_mean  == false)
    {
        if(max_en)
        {
            position_implantation_thrsh = nr_max;
            position_implantation_thrsh_ok  = true;
        }
        else
            position_implantation_thrsh_ok  = false;
    }

    //   if(position_implantation_ok)
    //     spec_position_implantation->
    //     manually_increment(position_implantation);

    if(position_implantation_thrsh_ok)
        spec_position_implantation_thrsh->
        manually_increment(position_implantation_thrsh);
}

//**********************************************************************
/*!
    \fn Tactive_stopper_vector_strips::
    calculate_position_and_RMS_for_decay()
 */
void Tactive_stopper_vector_strips::calculate_position_and_RMS_for_decay()
{
    // mianownik = denominator
    // licznik = numerator
    if(flag_we_want_also_decay)
    {

        denominator_decay = 0;  // sum energy
        double numerator_decay  = 0;
        double numerator2_decay  = 0;


        double denominator_decay_thrsh = 0;
        double numerator_decay_thrsh  = 0;
        double numerator2_decay_thrsh  = 0;


        rms_implantation =
            rms_decay =
                rms_implantation_thrsh =
                    rms_decay_thrsh = 0;

        double en = 0;

        for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
        {
            en = stripe[i]->give_energy_decay_cal();
            numerator_decay += i * en;
            denominator_decay += en ;

            // threshold
            //en = stripe[i]->give_energy_decay_cal();
            if(is_energy_cal_decay_in_gate(en))
            {
                numerator_decay_thrsh += i  * en;
                denominator_decay_thrsh += en;
                if(RisingAnalysis_ptr->is_verbose_on())
                    cout << name_of_this_element <<
                         " stripe " << i << " had decay energy  " << en << endl;
            }

        }

        //======================================
        //     decay_sum_energy_both_xy_stripes = denominator_x_decay ;

        //======================================
        if(denominator_decay)
        {
            position_decay = numerator_decay / denominator_decay;
            //       spec_position_decay->manually_increment(position_decay);
            position_decay_ok = true;
        }
        else
        {
            position_decay_ok = false;
            position_decay = -9999;
        }

        // same with threshold
        if(denominator_decay_thrsh)
        {
            position_decay_thrsh =
                numerator_decay_thrsh / denominator_decay_thrsh;
            spec_position_decay_thrsh->manually_increment
            (position_decay_thrsh);
            if(RisingAnalysis_ptr->is_verbose_on())
                cout <<
                     "Decay position thrsh = " << position_decay_thrsh << endl;
            position_decay_thrsh_ok = true;
            //     position_decay_thrsh = 9 + module_id_nr;
        }
        else
        {
            position_decay_thrsh_ok = false;
            position_decay_thrsh = -9999;
        }

        if(position_decay_thrsh_ok)
            spec_position_decay_thrsh->manually_increment
            (position_decay_thrsh);


        // ------- rms --------------------------------------------

        for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
        {
            if(stripe[i]->give_fired())
            {
                numerator2_decay += pow((position_decay  - i), 2)  *
                                    stripe[i]->give_energy_decay_cal();

                if(is_energy_cal_decay_in_gate(
                            stripe[i]->give_energy_decay_cal()))
                {
                    numerator2_decay_thrsh +=
                        pow((position_decay_thrsh  - i), 2)  *
                        stripe[i]->give_energy_decay_cal();
                }
            }
        }


        if(denominator_decay)
            rms_decay = sqrt(numerator2_decay / denominator_decay);


        if(denominator_decay_thrsh)
            rms_decay_thrsh =
                sqrt(numerator2_decay_thrsh / denominator_decay_thrsh);





    } // if also decay
}
//**********************************************************************
void Tactive_stopper_vector_strips::implantation_action()
{
    // if the y positioning is broken - we may want to match only with x
    // in such a case we ptu we put yy = 0 and match to pixel  (x, 0)

    if(RisingAnalysis_ptr->is_verbose_on())
        cout << name_of_this_element
             << "--------- Tactive_stopper_vector_strips::implantation_action()"
             "for " << name_of_this_element
             << " - in "
             <<  position_implantation_thrsh
             //     << ", "
             //     <<  y_position_implantation_thrsh
             << endl;


    // storing in the pixel the infomation about
    // - energy - for fun - or recognizing the patter by funny energy value
    //  - timestamp
    // - Zet, Aoq and perhaps something more


    // selecting the pixel
    // position is in range 0-15


    // what to do, when position not calculated???


    //-------------
    // matrix of encrementrs - zeroed every event
    int x = (int) position_implantation_thrsh;
    //   int y =  (int) y_position_implantation_thrsh;

    if((x < 0 || x >= (int) const_nr_of_strips))
    {
        cout << "Illegal coordinate of the implantation pixel (= " << x
             //     << ", Y=" << y
             << ")"
             << endl;
        return;
    }




    //--------------
    cout << "Comented code in the function:  Tactive_stopper_vector_strips::implantation_action() "
         << __LINE__ << endl;
    //   owner->set_position_implantation_thrsh(
    //     position_implantation_thrsh+ (const_nr_of_strips * module_id_nr) );


}
//**********************************************************************
bool Tactive_stopper_vector_strips::decay_action()
{
    //     if(RisingAnalysis_ptr->is_verbose_on() )
    //         cout << name_of_this_element <<
    //         " --> Tactive_stopper_vector_strips::decay_action()"
    //         << endl;

    if(flag_we_want_also_decay)
    {

        // if the y positioning is broken - we may want to match only with x
        // in such a case we ptu we put yy = 0 and match to pixel  (x, 0)

        // as this is the decay situation at first we need to
        // know if in this pixel there is some implantation
        //At first we will look in this pixel
        int xx = (int) position_decay_thrsh;

        if((xx < 0 || xx >= (int) const_nr_of_strips))

        {
            cout << "Illegal coordinate of the decay pixel (= " << xx
                 << endl;
            return false;
        }
    }
    return true; // ??????????????????
}
//***********************************************************************
//***********************************************************************
//***********************************************************************
void Tactive_stopper_vector_strips::row_not_fired()
{
    zero_flags_fired_vectors();
    strip_energies_implantation_cal.clear();
    strip_energies_decay_cal.clear();

    impl_multiplicity_above_thrsh =
        decay_multiplicity_above_thrsh =
            impl_multiplicity =
                decay_multiplicity = 0;

    nr_1st_energy_implantation_cal =
        nr_2nd_energy_implantation_cal =
            nr_3rd_energy_implantation_cal =

                nr_1st_energy_decay_cal =
                    nr_2nd_energy_decay_cal =
                        nr_3rd_energy_decay_cal =

                            position_implantation =
                                position_decay   =

                                    // ----- thrsh
                                    position_implantation_thrsh =
                                        position_decay_thrsh   =

                                            //==========
                                            rms_implantation  =
                                                    rms_decay  =
                                                            rms_implantation_thrsh  =
                                                                    rms_decay_thrsh = 0.0;

    position_implantation_thrsh_ok  = false;
    position_implantation_ok = false;

    position_decay_thrsh_ok  = false;
    position_decay_ok = false;

    //     decay_sum_energy_both_xy_stripes =
    denominator_decay = 0.0;

    // #$%
    denominator_implantation = 0;


    position_implantation =
        position_implantation_thrsh =

            position_decay =
                position_decay_thrsh  = -9999;

}
//**********************************************************************
//**********************************************************************
//***********************************************************************


//#endif   // def ACTIVE_STOPPER_PRESENT
