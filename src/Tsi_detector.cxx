/***************************************************************************
Tsi_detector.cpp  -  description
-------------------
begin                : Fri May 16 2003
copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
  email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/


#ifdef NIGDY

#include "Tsi_detector.h"

#include "Tfile_helper.h"
#include "randomizer.h"
#include <sstream>
#include "Tnamed_pointer.h"
#include "Tfrs.h"


//************************************************************************
Tsi_detector:: Tsi_detector(string name,
                            cate_segment(TIFJEvent::*ptr))
    :
    Tfrs_element(name),
    name_of_this_element(name),
    ptr_to_si(ptr)
{

    create_my_spectra() ;

    vector<double> vvv(3) ;
    for(int i = 0 ; i < 4 ; i++)  // if there was no gates defined
    {
        high_g[i] = 4095 ;
        low_g[i] = 0 ;

        pin_calibr_factors.push_back(vvv) ;

    }

    named_pointer[name_of_this_element + "_energy_calibrated"] =
        Tnamed_pointer(&Si_energy_calibrated, &flag_segment_Si_fired, this) ;

    named_pointer[name_of_this_element + "_time_calibrated"]   =
        Tnamed_pointer(&Si_time_calibrated, &flag_segment_Si_fired, this) ;

    named_pointer[name_of_this_element + "_flag_Si_segment_fired"] =
        Tnamed_pointer(&flag_segment_Si_fired, 0, this) ;


    named_pointer[name_of_this_element + "_flag_xy_ok"] =
        Tnamed_pointer(&flag_xy_ok, 0, this) ;
    //
    named_pointer[name_of_this_element + "_pin_a_cal"] =
        Tnamed_pointer(&pin_cal[0], &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_pin_b_cal"] =
        Tnamed_pointer(&pin_cal[1], &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_pin_c_cal"] =
        Tnamed_pointer(&pin_cal[2], &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_pin_d_cal"] =
        Tnamed_pointer(&pin_cal[3], &flag_xy_ok, this) ;
    //
    named_pointer[name_of_this_element + "_position_x"] =
        Tnamed_pointer(&x, &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_position_y"] =
        Tnamed_pointer(&y, &flag_xy_ok, this) ;


}
//************************************************************************
Tsi_detector::~Tsi_detector()
{}
//************************************************************************
/** This function is creating the basic spectra dedicatet to the one
    segment (si_detector) of the Cate detector */
void Tsi_detector::create_my_spectra()
{

    string folder = "target_folder/" + name_of_this_element ;
    string name ;


    //===========================================
    // ------- Thin Si --------------------------
    name = name_of_this_element + "_Si_time_raw"  ;
    spec_Si_time = new spectrum_1D(name,
                                   name,
                                   1024, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_Si_time) ;

    //--------------------------------------------
    name = name_of_this_element + "_Si_energy_raw"  ;
    spec_Si_energy = new spectrum_1D(name,
                                     name,
                                     1024, 0, 4096,
                                     folder);
    frs_spectra_ptr->push_back(spec_Si_energy) ;
    //-------------------------------------------

    // CALIBRATED -------------------------------
    // ------- Thin Si --------------------------
    name = name_of_this_element + "_Si_time_cal"  ;
    spec_Si_time_cal = new spectrum_1D(name,
                                       name,
                                       1024, 0, 4096,
                                       folder);
    frs_spectra_ptr->push_back(spec_Si_time_cal) ;

    //--------------------------------------------
    name = name_of_this_element + "_Si_energy_cal"  ;
    spec_Si_energy_cal = new spectrum_1D(name,
                                         name,
                                         1024, 0, 4096,
                                         folder);
    frs_spectra_ptr->push_back(spec_Si_energy_cal) ;
    //-------------------------------------------


    // PINS  - positions given by Si detector
    //--------------------------------------------
    for(int i = 0 ; i < 4 ; i++)
    {
        name = name_of_this_element + "_Si_pin_" ;
        name += (char)('a' + i);
        name += "_raw"  ;
        spec_pin_raw[i] = new spectrum_1D(name,
                                          name,
                                          1024, 0, 4096,
                                          folder,
                                          "GATE: left edge defines an offset for the pin calibration");
        frs_spectra_ptr->push_back(spec_pin_raw[i]) ;
        //-------------------------------------------
        // calibrated pins -----------------
        //--------------------------------------------
        name = name_of_this_element + "_Si_pin_"  ;
        name += (char)('a' + i);
        name += "_cal"  ;
        spec_pin_cal[i] = new spectrum_1D(name,
                                          name,
                                          1024, 0, 4096,
                                          folder);
        frs_spectra_ptr->push_back(spec_pin_cal[i]) ;
    }
    //-------------------------------------------


    // ============ FAN spectra

    // mutiplicity of pins ------------------------------------
    name = name_of_this_element + "_fan_pin_multiplicity_raw"  ;
    spec_fan_pin_mult_raw = new spectrum_1D(name,
                                            name,
                                            5, 0, 5,
                                            folder);
    frs_spectra_ptr->push_back(spec_fan_pin_mult_raw);


    /*
    // mutiplicity of pins after calibrating and gating them
    name = name_of_this_element + "_fan_pin_multiplicity_gated"  ;
    spec_fan_pin_mult_gated = new spectrum_1D( name,
                  name,
                  5, 0, 5,
                  folder );
    frs_spectra_ptr->push_back(spec_fan_pin_mult_gated);

    */

    // existence of  pin signals ----------------------------------
    name = name_of_this_element + "_fan_signals_raw"  ;
    spec_fan_signals_raw = new spectrum_1D(name,
                                           name,
                                           6, 0, 6,
                                           folder);
    frs_spectra_ptr->push_back(spec_fan_signals_raw);

    /*---
    // existence of  pin signals after gating
    name = name_of_this_element + "_fan_signals_gated"  ;
    spec_fan_signals_gated = new spectrum_1D( name,
                    name,
                    6, 0, 6,
                    folder );
    frs_spectra_ptr->push_back(spec_fan_signals_gated);
    */

    /*
      // PATTERN -------------------combintaion of raw signals
      name = name_of_this_element + "_fan_pattern_signals_raw"  ;
      spec_fan_pattern_signals_raw = new spectrum_1D( name,
                        name,
                        70, 0, 70,
                        folder );
      frs_spectra_ptr->push_back(spec_fan_pattern_signals_raw);
    */

    // Matrices ==============================================

    // map of the positions
    name =   name_of_this_element + "_position_xy" ;

    matr_position_xy = new spectrum_2D(name,
                                       name,
                                       250, -25, 25,   // 25 mm (all 50 mm)
                                       250, -25, 25,     // -0.5, 0.5,
                                       folder);

    frs_spectra_ptr->push_back(matr_position_xy) ;

    // map of the positions
    name =   name_of_this_element + "_position_xy_corr" ;

    matr_position_xy_corr = new spectrum_2D(name,
                                            name,
                                            250, -25, 25,   // 25 mm (all 50 mm)
                                            250, -25, 25,     // -0.5, 0.5,
                                            folder);

    frs_spectra_ptr->push_back(matr_position_xy_corr) ;


}
//**************************************************************
//vector< spectrum_abstr*>*  Tsi_detector::frs_spectra_ptr
//{
//    return owner->give_spectra_pointer() ;
//}
//**************************************************************
/** the most important fuction,  executed for every event */
void Tsi_detector::analyse_current_event()
{

    //  cout << "   analyse_current_event()          for "
    //        << name_of_this_element
    //        << endl ;

    flag_segment_Si_fired  = false ;
    flag_xy_ok = false ;
    x = y = 0 ;
    x_corr = y_corr = 0;



    // take it out from input event to make faster access
    Si_energy_raw = event_si_detector_data->Si_energy;
    Si_time_raw = event_si_detector_data->Si_time;


    //============================================================
    // the Si   part ============================================
    //============================================================
    // incrementing the raw energy spectrum
    spec_Si_energy->manually_increment(Si_energy_raw) ;
    // incrementing the raw time spectrum
    spec_Si_time->manually_increment(Si_time_raw);



    // checking if Si FIRED !!!

    if(
        Si_energy_raw > 0  &&  Si_energy_raw < 4096
        //  &&
        //  Si_time_raw > 0  &&  Si_time_raw < 4096   <----Commented to CHEAT !!!!!!!!!!!
    )
    {
        flag_segment_Si_fired = true ;
        // increment its raw spectra

        // incrementing the raw energy spectrum
        //    spec_Si_energy->manually_increment( Si_energy_raw ) ;
        // incrementing the raw time spectrum
        //    spec_Si_time->manually_increment(Si_time_raw);
    }



    Si_energy_calibrated = (Si_energy_calibr_factors[1] * (Si_energy_raw
                            + randomizer::randomek())) + Si_energy_calibr_factors[0]  ;




    spec_Si_energy_cal->manually_increment(Si_energy_calibrated) ;
    // rising_pointer->increm_spec_total_energy_calibrated(energy_calibrated) ;

    // time
    Si_time_calibrated = (Si_time_calibr_factors[1] * (Si_time_raw
                          + randomizer::randomek())) + Si_time_calibr_factors[0]  ;


    if(Si_energy_calibrated >= 1 && Si_energy_calibrated < 4096
            &&
            Si_time_calibrated >= 10 && Si_time_calibrated < 3800)
    {
        Si_segment_fired_good = true ;
        spec_Si_time_cal->manually_increment(Si_time_calibrated) ;
    }



    //the legal situation is when any of 5 signals if present
    pin_raw[0] = event_si_detector_data->pin_a;
    pin_raw[1] = event_si_detector_data->pin_b;
    pin_raw[2] = event_si_detector_data->pin_c;
    pin_raw[3] = event_si_detector_data->pin_d;


    for(int i = 0 ;  i < 4 ; i++)
    {
        spec_pin_raw[i]->manually_increment(pin_raw[i])  ;

    }

    // mutiplicity of pins ------------------------------------
    int pin_multipl = 0 ;
    for(int i = 0 ; /*Si_segment_fired_good &&*/ i < 4 ; i++)
    {
        if((pin_raw[i] > 250) && (pin_raw[i] < 4096))  // was 250
        {
            pin_multipl++ ;
            spec_fan_signals_raw->manually_increment(i) ;

        }
    }

    spec_fan_pin_mult_raw->manually_increment(pin_multipl) ;

    // Fans spectra of Si
    if(Si_energy_raw)
        spec_fan_signals_raw->manually_increment(4) ;
    if(Si_time_raw)
        spec_fan_signals_raw->manually_increment(5) ;


    //===================================


    // CALIBRATION -----------------------------------------
    // all pins have to be calibrated

    //  so far Pieter says that we subtract the offset which is the
    //  left edge of the gate


    for(int i = 0 ; i < 4 ; i++)
    {
        pin_cal[i] = pin_raw[i] - low_g[i] ;
        spec_pin_cal[i]->manually_increment(pin_cal[i]) ;  // if below is NIGDY
    }

    // now we can check the gates


    //=========================================================
    // calculation of the real position,
    // where the ion hit the detector
    //=========================================================


    bool flag_all_pins_in_gates = (pin_multipl == 4)  ;


    if(flag_all_pins_in_gates)
    {
        // X position
        double mianownik = pin_cal[0] + pin_cal[1]
                           + pin_cal[2] + pin_cal[3] ;

        if(mianownik != 0)
        {
            x = (50.0 / 2)
                * ((pin_cal[1] + pin_cal[2]) - (pin_cal[3] + pin_cal[0]))
                /  mianownik ;

            y = (50.0 / 2)
                * ((pin_cal[3] + pin_cal[2]) - (pin_cal[0] + pin_cal[1]))
                / mianownik ;

            x_corr = (x_position_corr_factors[1] * x) + x_position_corr_factors[0]  ;
            //       spec_x_corr->manually_increment(x_corr) ;
            y_corr = (y_position_corr_factors[1] * y) + y_position_corr_factors[0]  ;
            //        spec_y_corr->manually_increment(y_corr) ;

            flag_xy_ok = true ;

            //cout << " incrementing the matrix x,y = " << x << ", " << y << endl ;
            matr_position_xy->manually_increment(x, y);
            matr_position_xy_corr->manually_increment(x_corr, y_corr);
        }
    }

    calculations_already_done = true;

}
//**************************************************************
/** reading calibration, gates, setting pointers */
void Tsi_detector::make_preloop_action(ifstream & s)
{
    //
    //  cout << "preloop action for si_detector "
    //       << name_of_this_element
    //       << endl ;



    // cout << "Reading the calibration for the si_detector "
    //        << name_of_this_element
    //        << endl ;

    //  cate_segment (TIFJEvent::*ptr_to_si);
    //
    //  cate_segment  *event_si_detector_data ;


    event_si_detector_data = &(event_unpacked->target_Si);

    if(!event_si_detector_data)
    {
        cout << "Alarm, in " << name_of_this_element
             << " the pointer to event was not set" << endl;
        exit(1) ;
    }



    // in this file we look for a string
    int how_many_factors = 2 ;   // max linear calibration

    string slowo;


    //==================================
    slowo = name_of_this_element + "_energy_cal_factors" ;

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo);

    Si_energy_calibr_factors.clear();
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess = "I found keyword: " + slowo
                          + ", but error was in reading its value." ;
            throw mess ;
        }
        // if ok
        Si_energy_calibr_factors.push_back(value) ;

    }

    if(Si_energy_calibr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for energy calibration Si of " + name_of_this_element;
        throw mess ;
    }

    // reading the time calibration factors --------------

    slowo = name_of_this_element + "_time_cal_factors" ;

    Tfile_helper::spot_in_file(s, slowo);

    Si_time_calibr_factors.clear();
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess =
                "I found keyword: " + slowo
                + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok

        Si_time_calibr_factors.push_back(value) ;

    }
    if(Si_time_calibr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for time calibration Si of " + name_of_this_element ;
        throw mess ;
    }


    // reading the pin calibration factors --------------
    for(int i = 0 ; i < 4 ; i++)
    {
        if(pin_calibr_factors[i].size() > 0)
            pin_calibr_factors[i].clear();
    }



    //  string name_of_pin; // [100] = "" ;
    for(int symb = 0 ; symb < 4 ; symb++)
    {
        ostringstream outs;  // Declare an output string stream.
        outs << name_of_this_element << "_pin_"
             << char('a' + symb)
             << "_cal_factors";
        //<< ends ;   // Convert value into a string.   << this was giving an error !!!!!!!!!!!!!!!!!!!!!!!!!!!!

        slowo = outs.str();      // Get the created string from the output stream.

        //cout << "Try to spot the word >>" << slowo << "<<" << endl;
        Tfile_helper::spot_in_file(s, slowo);


        for(int i = 0 ; i < how_many_factors ; i++)
        {
            // otherwise we read the data
            double value ;
            s >> value ;

            if(!s)
            {
                string mess =
                    "I found keyword: " + slowo
                    + " but error was in reading its value." ;
                throw mess ;

            }
            // if ok

            pin_calibr_factors[symb].push_back(value) ;
            break ;

        }
    }


    // reading the position correction calibration factors -----------
    // X
    slowo = name_of_this_element + "_x_position_corr_factors" ;

    Tfile_helper::spot_in_file(s, slowo);

    x_position_corr_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess ;
            mess += "I found keyword: " + slowo
                    + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok

        x_position_corr_factors.push_back(value) ;

    }

    if(x_position_corr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for x_position_correction  of " + name_of_this_element ;

        // for old programs
        x_position_corr_factors.clear() ;
        x_position_corr_factors.push_back(0) ;
        x_position_corr_factors.push_back(1) ;

        //    throw mess ;
    }


    // Y ------------------------------------------
    slowo = name_of_this_element + "_y_position_corr_factors" ;

    Tfile_helper::spot_in_file(s, slowo);

    y_position_corr_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess ;
            mess += "I found keyword: " + slowo
                    + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok

        y_position_corr_factors.push_back(value) ;

    }

    if(y_position_corr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for x_position_correction  of " + name_of_this_element ;
        // throw mess ;
        y_position_corr_factors.clear() ;
        y_position_corr_factors.push_back(0) ;
        y_position_corr_factors.push_back(1) ;

    }



    //========================================
    read_gates();

}
//***************************************************************
/** Reading the gates for all pins signals (raw) */
void Tsi_detector::read_gates()
{

    //  gates are created by my viewer, or manually
    // so that in one file there is only one gate, for this particullar object

    for(int symb = 0 ; symb < 4 ; symb++)
    {
        string core_of_the_name = "gates/" + name_of_this_element ;
        string nam_gate = core_of_the_name + "_Si_pin_" ;
        nam_gate += (char)('a' + symb);
        nam_gate += "_raw_gate_GATE.gate" ;

        ifstream plik_gate(nam_gate.c_str());

        if(! plik_gate)
        {
            string mess = "I can't open  file: " + nam_gate
                          + "\nThis gate you can produce interactively using a 'cracow' viewer program" ;
            cout << mess
                 << "\nI create a gate and assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
                 << endl ;

            ofstream pliknew(nam_gate.c_str());
            pliknew   << low_g[symb] << "   "    // lower limit
                      << high_g[symb] << endl ;
        }
        else
        {


            plik_gate >> zjedz >> low_g[symb]     // lower limit
                      >> zjedz >>  high_g[symb] ;   // upper limit

            if(! plik_gate)
            {
                cout <<  "Error while reading 2 numbers from the gate file: " + nam_gate  ;
                exit(-1) ;
            }
            plik_gate.close() ;
        }

    } // for
}
//************************************************************
/** where in the event starts data for this particular si_detector */
void Tsi_detector::set_event_data_pointers(cate_segment * ptr)
{
    event_si_detector_data = ptr ;
}
//***********************************************************
//***********************************************************
//***********************************************************
#endif //  #ifdef NIGDY


