//
// C++ Implementation: Texotic_vector_strips
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "Texotic_vector_strips.h"

#include <sstream>
#include "Tnamed_pointer.h"
using namespace std;
//using namespace exotic_namespace;


#include "TIFJAnalysis.h" // for verbose
#include <algorithm>
#include "Texotic_strip_detector.h"
#if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT

//static
int Texotic_vector_strips::algorithm_for_jittering_correction;

#define COTO   cout << "File = " << __FILE__ << " line = " << __LINE__ << endl;

// ******************************************************************
Texotic_vector_strips::Texotic_vector_strips(Texotic_strip_detector *owner_ptr,
        std::string name,
        unsigned int nr,
        bool x_otherwise_y,
        const int const_nr_of_strips_) :
    owner_det(owner_ptr),
    Tfrs_element(name),
    module_id_nr(nr),
    this_is_x_coordinate(x_otherwise_y),
    const_nr_of_strips(const_nr_of_strips_),
    flag_algorithm_for_position_calculation_is_mean(true)
{
    string name_of_stripe ;
    //     symbol = string(1, symbol_);
    array_of_data = 0;

    energy_cal_threshold = 0;
    energy_cal_threshold_upper = 8192;
    previous_0_strip_value  =  0 ;

    flag_algorithm_for_position_calculation_is_mean  = true;

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

        //cout << " here was giving the module_id_nr " << endl;
        stripe.push_back(new Texotic_one_stripe(this,
                                                name_of_stripe,
                                                module_id_nr,
                                                0,
                                                i));
    }

    // INCREMENTERS

    named_pointer[name_of_this_element + "_nr_1st_energy_cal"]
    = Tnamed_pointer(&nr_1st_energy_cal, 0, this) ;

    named_pointer[name_of_this_element + "_2nd_energy_cal"] =
        Tnamed_pointer(&nr_2nd_energy_cal, 0, this) ;

    named_pointer[name_of_this_element + "_nr_3rd_energy_cal"] =
        Tnamed_pointer(&nr_3rd_energy_cal, 0, this) ;


    named_pointer[name_of_this_element + "_rms"] =
        Tnamed_pointer(&rms,
                       &position_ok, this) ;

    // thrshlds -----------
    named_pointer[name_of_this_element + "_rms_thrsh"] =
        Tnamed_pointer(&rms_thrsh,
                       &position_thrsh_ok, this) ;


    // mult

    named_pointer[name_of_this_element +
                  "_multiplicity_for_energy_cal_above_threshold"] =
                      Tnamed_pointer(
                          &multiplicity_above_thrsh,
                          0, this) ;




    //====================== positions

    //   named_pointer[name_of_this_element+"_position"] =
    //     Tnamed_pointer(
    //       &position, &position_ok, this) ;
    //
    //   if(flag_we_want_also_decay)
    //   {
    //     named_pointer[name_of_this_element+"_position_decay"] =
    //       Tnamed_pointer(&position_decay, &position_decay_ok, this) ;
    //   }

    //     // ----- thrsh
    //
    //     named_pointer[name_of_this_element+
    //                   "_position_above_thresholdxxx"] =
    //                       Tnamed_pointer (
    //                           &position_thrsh,
    //                           &position_thrsh_ok, this ) ;

    named_pointer[name_of_this_element +
                  "_sum_energy_stripes"] =
                      Tnamed_pointer(&denominator, 0, this) ;
    //---------------
    // INCREMENTERS----------------

    string entry_name;
    Tmap_of_named_pointers::iterator pos;

    named_pointer[name_of_this_element
                  + "_position_above_threshold"] =
                      Tnamed_pointer(
                          &position_thrsh,
                          &position_thrsh_ok, this) ;

    // twelve_test = 12;
    //     named_pointer[name_of_this_element
    //                   +"_twelve_tst"] =
    //                       Tnamed_pointer (
    //                           &twelve_test,
    //                           NULL, this ) ;
    //


    create_my_spectra();
}
// *********************************************************************
Texotic_vector_strips::~Texotic_vector_strips()
{
    //    save_spectra();
    for(unsigned int i = 0 ; i < stripe.size() ; i++)
    {
        delete stripe[i];
    }
}
// ***********************************************************************
void Texotic_vector_strips::create_my_spectra()
{
    string name = name_of_this_element + "_fan"  ;
    string folder("stopper/");
    spec_fan =
        new spectrum_1D(name,
                        name,
                        (const_nr_of_strips * 10) + 10, 0, (const_nr_of_strips * 10) + 10,
                        folder,
                        "stripes which fired, (without any threshold)"
                        //                             " ( each stripe marks the triggers:"
                        //                           "   *1- implantation, *2 - decay, "
                        //                           "*3 -undefined"
                       );

    give_spectra_pointer()->push_back(spec_fan) ;


    name = name_of_this_element + "_fan_above_thresholds"  ;

    spec_fan_above_thresholds =
        new spectrum_1D(name,
                        name,
                        (const_nr_of_strips * 10) + 10, 0, (const_nr_of_strips * 10) + 10,
                        folder,
                        "when above thresholds "
                        /*" ( triggers:   *1- "
                                  "implantation, *2 - decay, *3 "
                                  "-undefined]"*/
                       );
    give_spectra_pointer()->push_back(spec_fan_above_thresholds) ;


    //-----------------  -----------------------------------------------
    name = name_of_this_element + "_nr_1st_max_energy_cal"  ;
    spec_nr_1st_energy_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Maximal value of energy");

    give_spectra_pointer()->push_back(spec_nr_1st_energy_cal) ;

    //-----------------

    name = name_of_this_element + "_2nd_max_energy_cal"  ;
    spec_nr_2nd_energy_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Second maximal value of energy");

    give_spectra_pointer()->push_back(spec_nr_2nd_energy_cal) ;
    //-----------------

    name = name_of_this_element + "_nr_3rd_max_energy_cal"  ;
    spec_nr_3rd_energy_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,
                        "Third maximal value of energy");

    give_spectra_pointer()->push_back(spec_nr_3rd_energy_cal) ;



    //-----------------
    // multiplicity spectra ===================================
    // spectra multiplicity with threshold

    //--------------------------------- IMPLANT
    name = name_of_this_element +
           "_multiplicity_for_energy_cal_above_threshold";
    spec_multiplicity_for_energy_cal_above_threshold =
        new spectrum_1D(name,  name,
                        const_nr_of_strips + 1, 0,
                        const_nr_of_strips + 1,
                        folder,
                        "multiplicity_for_energy_cal_above_threshold",
                        name_of_this_element +
                        "_multiplicity_for_energy_cal_above_threshold");
    give_spectra_pointer()->push_back(
        spec_multiplicity_for_energy_cal_above_threshold);


    // spectra multiplicity without threshold
    //---------------------------------
    name = name_of_this_element + "_multiplicity_for_energy_cal"  ;
    spec_multiplicity_for_energy_cal =
        new spectrum_1D(name,  name,
                        const_nr_of_strips + 1, 0,
                        const_nr_of_strips + 1,
                        folder,
                        "without threshold ",
                        name_of_this_element +
                        "_multiplicity_for_energy_cal");
    give_spectra_pointer()->push_back(
        spec_multiplicity_for_energy_cal);

    //-------------- POSITIONS --------------------

    //   name = name_of_this_element + "_position"  ;
    //   spec_position =
    //     new spectrum_1D( name,  name,
    //                      const_nr_of_strips*10, 0,
    //                      const_nr_of_strips ,
    //                      folder,
    //                      "position from lin calibration ( without threshold ) ",
    //                      name_of_this_element+
    //                      "_position");
    //   give_spectra_pointer()->push_back(spec_position);

    //-----------------------------
    //   if(flag_we_want_also_decay)
    //   {
    //     name = name_of_this_element + "_position_decay"  ;
    //     spec_position_decay =
    //       new spectrum_1D( name,  name,
    //                        const_nr_of_strips*10, 0,
    //                        const_nr_of_strips ,
    //                        folder,
    //                        "position from decay calibration ( wi"
    //                        "thout threshold ) ",
    //                        name_of_this_element+"_position_decay");
    //     give_spectra_pointer()->push_back(spec_position_decay);
    //   }

    //================== with Threshold ===================
    //-------------- POSITIONS --------------------

    name = name_of_this_element + "_position_thrsh"  ;
    spec_position_thrsh =
        new spectrum_1D(name,  name,
                        const_nr_of_strips * 10, 0,
                        const_nr_of_strips ,
                        folder,
                        "position from lin calibration ( wit"
                        "h threshold ) ",
                        name_of_this_element +
                        "_position_above_threshold");
    give_spectra_pointer()->push_back(spec_position_thrsh);

    //-----------------------------------
    // flag_talking_histograms = true;
    //     name = name_of_this_element + "_test"  ;
    //     spec_tst =
    //         new spectrum_2D ( name,  name,
    //                           32, 0 , 32,
    //                           32, 0 , 32,
    //                           folder,
    //                           "position from lin calibration ( wit"
    //                           "h threshold ) ",
    //                           name_of_this_element+
    //                           "_position_above_threshold" );
    // // flag_talking_histograms = false;
    //
    //     give_spectra_pointer()->push_back ( spec_tst );
}
//*************************************************************************************************
void Texotic_vector_strips::analyse_current_event()
{
    //     if(RisingAnalysis_ptr->is_verbose_on() )
//   cout << "Texotic_vector_strips::analyse_current_event() for " << name_of_this_element << endl;
    // incrementers
    strip_energies_cal.clear();

    nr_1st_energy_cal =
        nr_2nd_energy_cal =
            nr_3rd_energy_cal =

                multiplicity_above_thrsh =
                    multiplicity = 0 ;

    position =
        position_thrsh =   -9999;

    int channel = 0 ;  // for fan spectrum

    //     if ( exotic_event_type ==   event )
    //         channel = 1 ;


    double en = 0; // for tmp results
    jitter_correction = 0 ;
    int current_0_strip_value = 0 ;


    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
    {
        stripe[i]->analyse_current_event_pass_1();
    }

    // now we calculate the correction factor------------------------
//   cout << "Now calculating the jitter correction " << endl;
    switch(algorithm_for_jittering_correction)
    {
    case 0:       //-----------------------------------------------------------------
        //   COTO;
        current_0_strip_value = stripe[0]->give_energy_raw_mean();
        //       COTO;
        if(previous_0_strip_value)
        {
            jitter_correction =  previous_0_strip_value - current_0_strip_value;
        }
        previous_0_strip_value  =  current_0_strip_value;
        // jitter_correction= 20 ;
        // COTO
        break;

    case 1:       //-----------------------------------------------------------------
    {
        //   COTO;
        int min_nr = 0;
        int min_value = 9999999;
        for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
        {

            //        COTO;
            int v = abs(
                        stripe[i]->give_previous_energy_mean()
                        -
                        stripe[i]->give_energy_raw_mean()
                    );


//         cout << "Strip nr " << i << " )  Difference  = "
//         << v
//  <<" because previous mean = " << stripe[i]->give_previous_energy_mean()
//  <<" current mean  = " << stripe[i]->give_energy_raw_mean()
//         << endl;
            //       COTO;
            if(v < min_value)
            {
                min_value = v;
                min_nr = i;
            }
            //       COTO;
        }
        //     COTO;
        jitter_correction =  stripe[min_nr]->give_previous_energy_mean()  -  stripe[min_nr]->give_energy_raw_mean();
//       cout << "For strip = " << min_nr
//       << "  the difference value was = " << jitter_correction
//       << " and as it was the mininmu -> this is a correction value."
//       << endl;
    }
    break;

    } // end switch //-----------------------------------------------------------------

    // COTO;
    // now we know the jitter correction
    // PASS 2--------------------------------------
    //   cout << " jitter_correction = "  << jitter_correction  << endl;
    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
    {

        stripe[i]->set_jitter_correction(jitter_correction);
        stripe[i]->analyse_current_event_pass_2();

        if(stripe[i]->give_fired())
        {
            spec_fan->manually_increment((int) i * 10  + 0 + channel);
            multiplicity++;
            en =   stripe[i]->give_energy_cal();

            if(is_energy_cal_in_gate(en))
            {
                strip_energies_cal.push_back(en);    // to avoid putting the overflow value, we register only these
                // which are in the gate

                multiplicity_above_thrsh++;
                spec_fan_above_thresholds->manually_increment(
                    (int) i   * 10  + 0 + channel);
            }
        } // if fired
        //-------------------------------------------------------------
    } // end for analysis of all the stripes=============

    // Now more general --------------
    // ====================== Highest Energies spectra ===============
    // sorting the energy results
    sort(strip_energies_cal.begin(),
         strip_energies_cal.end());

    // incrementing two spectra with the highest energies
    if(strip_energies_cal.size())
    {
        nr_1st_energy_cal =
            strip_energies_cal[
                strip_energies_cal.size() - 1];
        spec_nr_1st_energy_cal->
        manually_increment(nr_1st_energy_cal);
        if(RisingAnalysis_ptr->is_verbose_on())
            cout << "nr_1st_energy_cal = "
                 << nr_1st_energy_cal << endl;
    }

    if(strip_energies_cal.size() >= 2)
    {
        nr_2nd_energy_cal =
            strip_energies_cal[
                strip_energies_cal.size() - 2];
        spec_nr_2nd_energy_cal->
        manually_increment(nr_2nd_energy_cal);
    }

    if(strip_energies_cal.size() >= 3)
    {
        nr_3rd_energy_cal =
            strip_energies_cal[
                strip_energies_cal.size() - 3];
        spec_nr_3rd_energy_cal->
        manually_increment(nr_3rd_energy_cal);
    }
    // incrementing multiplicity spectra
    // spectra multiplicity with threshold
    spec_multiplicity_for_energy_cal_above_threshold->
    manually_increment(
        multiplicity_above_thrsh);

    //##########################################

    // spectra multiplicity without threshold
    spec_multiplicity_for_energy_cal->
    manually_increment(multiplicity);

    //   cout << "in this event the y mult was = "
    // << y_multiplicity_for_energy_cal << endl;


    calculate_position_and_RMS_for();
    if(position_thrsh_ok)
        multiplicity_above_thrsh++;

    // for(int x = 0 ; x < 40 ; x+= 2)
    // for(int y = 0 ; y < 40 ; y+= 2)
    //     spec_tst->manually_increment(x, y);

    //     spec_tst->manually_increment(position_thrsh, position_thrsh);
    calculations_already_done = true ;
}
// **********************************************************************
// read the calibration factors, gates
void Texotic_vector_strips::make_preloop_action(ifstream & plik)
{
//   cout
//   << "Texotic_vector_strips::make_preloop_action ( ifstream & plik )  for  "
//   << name_of_this_element
//   << endl;

    string opt_filename = "options/exotic_detector_settings.txt";
    try
    {
        ifstream plik(opt_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            string mess = "I can't open file: "
                          + opt_filename  ;
            throw mess ;
        }


        algorithm_for_jittering_correction = (int) Tfile_helper::find_in_file(plik, "algorithm_for_jittering_correction");
//     cout << "algorithm_for_jittering_correction == >" << algorithm_for_jittering_correction << endl;
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
\fn Texotic_vector_strips::
zero_flags_fired_vectors()
*/
void Texotic_vector_strips::zero_flags_fired_vectors()
{
    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
        stripe[i]->mark_not_fired();

    //     decay_sum_energy_both_xy_stripes =
    //    denominator_decay = 0.0;
    denominator = 0;
}
//********************************************************************
/*!
\fn Texotic_vector_strips::
calculate_position_and_RMS_for()
*/
void Texotic_vector_strips::calculate_position_and_RMS_for()
{
    // mianownik = denominator
    // licznik = numerator

    //double
    denominator = 0;  // made a memeber of the class
    double numerator  = 0;
    double numerator2  = 0;

    double denominator_thrsh = 0;
    double numerator_thrsh  = 0;
    double numerator2_thrsh  = 0;

    rms =
        rms_thrsh = 0;


    double en = 0;


    int nr_max = 0;
    double max_en = 0;


    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
    {
        en =  stripe[i]->give_energy_cal();
        numerator += i  * en;
        denominator += en;

        // threshold
        en = stripe[i]->give_energy_cal();
        if(RisingAnalysis_ptr->is_verbose_on())
            cout << "stripe  " << i << " had implantation  energy  " << en
                 << (
                     is_energy_cal_in_gate(en)
                     ?  " <--- in gate " :
                     "")
                 << endl;

        if(is_energy_cal_in_gate(en))
        {
            numerator_thrsh += i  * en;
            denominator_thrsh += en;
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

        cout << " sum energy of  stripes =" << denominator
             << endl;
    }

    //-----------------------
    if(denominator)
    {
        position =
            numerator / denominator;
        //     spec_position->
        //     manually_increment(position);

        position_ok  = true;
    }
    else
        position_ok  = false;


    // threshold==========================================
    if(denominator_thrsh)
    {
        position_thrsh =
            numerator_thrsh / denominator_thrsh;
        //     spec_position_thrsh->
        //     manually_increment(position_thrsh);
        if(RisingAnalysis_ptr->is_verbose_on())
            cout <<
                 "So calcultated  position = " << position_thrsh
                 << endl;
        position_thrsh_ok  = true;
    }
    else
        position_thrsh_ok  = false;


    // ------- RMS ------------------------------------------
    for(unsigned int i = 0 ; i < const_nr_of_strips ; i++)
    {

        if(stripe[i]->give_fired())
        {
            numerator2 +=
                pow((position  - i), 2)  *
                stripe[i]->give_energy_cal();

            if(
                is_energy_cal_in_gate(
                    stripe[i]->give_energy_cal()))
            {
                numerator2_thrsh +=
                    pow((position_thrsh  - i), 2)  *
                    stripe[i]->give_energy_cal();
            }
        }
    }


    if(denominator)
        rms =
            sqrt(numerator2 / denominator);


    if(denominator_thrsh)
        rms_thrsh =
            sqrt(numerator2_thrsh /
                 denominator_thrsh);

    // Par force - another way of calculating the position
    if(flag_algorithm_for_position_calculation_is_mean  == false)
    {
        if(max_en)
        {
            position_thrsh = nr_max;
            position_thrsh_ok  = true;
        }
        else
            position_thrsh_ok  = false;
    }

    //   if(position_ok)
    //     spec_position->
    //     manually_increment(position);

    if(position_thrsh_ok)
        spec_position_thrsh->
        manually_increment(position_thrsh);
}

//**********************************************************************
void Texotic_vector_strips::action()
{
    // if the y positioning is broken - we may want to match only with x
    // in such a case we ptu we put yy = 0 and match to pixel  (x, 0)

    if(RisingAnalysis_ptr->is_verbose_on())
        cout << name_of_this_element
             << "--------- Texotic_vector_strips::action()"
             "for " << name_of_this_element
             << " - in "
             <<  position_thrsh
             //     << ", "
             //     <<  y_position_thrsh
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
    int x = (int) position_thrsh;
    //   int y =  (int) y_position_thrsh;

    if((x < 0 || x >= (int) const_nr_of_strips))
    {
        cout << "@@@ Illegal coordinate of the implantation pixel (= " << x
             //     << ", Y=" << y
             << ")"
             << endl;
        return;
    }




    //--------------
    cout << "Comented code in the function:  Texotic_vector_strips::action() "
         << __LINE__ << endl;
    //   owner->set_position_thrsh(
    //     position_thrsh+ (const_nr_of_strips * module_id_nr) );


}
//**********************************************************************
//***********************************************************************
//***********************************************************************
//***********************************************************************
void Texotic_vector_strips::row_not_fired()
{
    zero_flags_fired_vectors();
    strip_energies_cal.clear();

    multiplicity_above_thrsh =
        multiplicity  = 0;

    nr_1st_energy_cal =
        nr_2nd_energy_cal =
            nr_3rd_energy_cal =
                position =

                    // ----- thrsh
                    position_thrsh =

                        //==========
                        rms  =
                            rms_thrsh  = 0.0;

    position_thrsh_ok  = false;
    position_ok = false;


    //     decay_sum_energy_both_xy_stripes =

    // #$%
    denominator = 0;


    position =
        position_thrsh = -9999;

}
//**********************************************************************
void Texotic_vector_strips::increment_oscilloscope_matrix(int x, int y)
{
    //   cout << "comented increment_oscilloscope_matrix " << endl;

    owner_det-> increment_oscilloscope_matrix(x, y, this_is_x_coordinate);
}
//**********************************************************************

#endif // #if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
//***********************************************************************
//#endif   // def ACTIVE_STOPPER_PRESENT
