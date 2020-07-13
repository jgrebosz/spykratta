/***************************************************************************
Tcate_telescope.cpp  -  description
-------------------
begin                : Fri May 16 2003
copyright            : (C) 2003 by dr Jerzy Grebosz (IFJ Krakow, Poland)
  email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "experiment_def.h"
#ifdef CATE_PRESENT

#include "Tcate_telescope.h"
#include "Tcate.h"
#include "Tfile_helper.h"
#include "randomizer.h"
#include <sstream>

#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h"   // for verbose mode


//static
bool Tcate_telescope::par_force_segment_always_fired;
bool Tcate_telescope::par_force_segment_always_fired_good;


//************************************************************************
Tcate_telescope:: Tcate_telescope(Tcate *ptr_owner, string name, int id_nr)
    : owner(ptr_owner),
      name_of_this_element(name)
{
    segment_nr = id_nr;

    event_cate_telescope_data = 0 ;       // 0
    create_my_spectra() ;


    vector<double> vvv(3) ;

    for(int i = 0 ; i < 4 ; i++)  // if there was no gates defined
    {
        high_g[i] = 4095 ;
        low_g[i] = 0 ;

        pin_calibr_factors.push_back(vvv) ;

    }

    named_pointer[name_of_this_element + "_CsI_energy_calibrated_when_fired_good"]
    = Tnamed_pointer(&CsI_energy_calibrated, &flag_CsI_segment_fired_good, this) ;

    named_pointer[name_of_this_element + "_CsI_time_calibrated_when_fired_good"]
    = Tnamed_pointer(&CsI_time_calibrated, &flag_CsI_segment_fired_good, this) ;

    named_pointer[name_of_this_element + "_flag_CsI_has_fired"]
    = Tnamed_pointer(&flag_segment_CsI_fired, 0, this) ;

    named_pointer[name_of_this_element + "_flag_CsI_fired_and_good"]
    = Tnamed_pointer(&flag_CsI_segment_fired_good, 0, this) ;


    //
    //
    named_pointer[name_of_this_element + "_Si_energy_calibrated_when_fired_good"]
    = Tnamed_pointer(&Si_energy_calibrated, &flag_Si_segment_fired_good, this) ;


    named_pointer[name_of_this_element + "_Si_ener_cal_geom_mean_with_si_ener_cal_at_target_when_fired_good"]
    = Tnamed_pointer(&mean_delta_E, &flag_Si_segment_fired_good, this) ;


    named_pointer[name_of_this_element + "_Si_time_calibrated_when_fired_good"]
    = Tnamed_pointer(&Si_time_calibrated, &flag_Si_segment_fired_good, this) ;



    named_pointer[name_of_this_element + "_Si_energy_raw_when_fired"]
    = Tnamed_pointer(&Si_energy_raw, &flag_segment_Si_fired, this) ;

    named_pointer[name_of_this_element + "_CsI_energy_raw_when_fired"]
    = Tnamed_pointer(&CsI_energy_raw, &flag_segment_CsI_fired, this) ;


    named_pointer[name_of_this_element + "_Si_time_raw_when_fired"]
    = Tnamed_pointer(&Si_time_raw, &flag_segment_Si_fired, this) ;

    named_pointer[name_of_this_element + "_CsI_time_raw_when_fired"]
    = Tnamed_pointer(&CsI_time_raw, &flag_segment_CsI_fired, this) ;


    named_pointer[name_of_this_element + "_Si_energy_raw"]
    = Tnamed_pointer(&Si_energy_raw, 0, this) ;

    named_pointer[name_of_this_element + "_CsI_energy_raw"]
    = Tnamed_pointer(&CsI_energy_raw, 0, this) ;


    named_pointer[name_of_this_element + "_Si_time_raw"]
    = Tnamed_pointer(&Si_time_raw, 0, this) ;

    named_pointer[name_of_this_element + "_CsI_time_raw"]
    = Tnamed_pointer(&CsI_time_raw, 0, this) ;



    named_pointer[name_of_this_element + "_flag_Si_has_fired"] =
        Tnamed_pointer(&flag_segment_Si_fired, 0, this) ;

    named_pointer[name_of_this_element + "_flag_Si_fired_and_good"]
    = Tnamed_pointer(&flag_Si_segment_fired_good, 0, this) ;



    named_pointer[name_of_this_element + "_flag_xy_ok"] =
        Tnamed_pointer(&flag_xy_ok, 0, this) ;


    named_pointer[name_of_this_element + "_pin_a_raw"] =
        Tnamed_pointer(&pin_raw[0], 0, this) ;
    named_pointer[name_of_this_element + "_pin_b_raw"] =
        Tnamed_pointer(&pin_raw[1], 0, this) ;
    named_pointer[name_of_this_element + "_pin_c_raw"] =
        Tnamed_pointer(&pin_raw[2], 0, this) ;
    named_pointer[name_of_this_element + "_pin_d_raw"] =
        Tnamed_pointer(&pin_raw[3], 0, this) ;


    //
    named_pointer[name_of_this_element + "_pin_a_cal_when_xy_ok"] =
        Tnamed_pointer(&pin_cal[0], &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_pin_b_cal_when_xy_ok"] =
        Tnamed_pointer(&pin_cal[1], &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_pin_c_cal_when_xy_ok"] =
        Tnamed_pointer(&pin_cal[2], &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_pin_d_cal_when_xy_ok"] =
        Tnamed_pointer(&pin_cal[3], &flag_xy_ok, this) ;
    //
    named_pointer[name_of_this_element + "_position_x_local_when_xy_ok"] =
        Tnamed_pointer(&x, &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_position_y_local_when_xy_ok"] =
        Tnamed_pointer(&y, &flag_xy_ok, this) ;

    named_pointer[name_of_this_element + "_position_x_abs_when_xy_ok"] =
        Tnamed_pointer(&x_abs, &flag_xy_ok, this) ;
    named_pointer[name_of_this_element + "_position_y_abs_when_xy_ok"] =
        Tnamed_pointer(&y_abs, &flag_xy_ok, this) ;


    named_pointer[name_of_this_element + "_flag_Si_and_CsI_has_fired_good"] =
        Tnamed_pointer(&flag_Si_and_CsI_fired_good, 0, this) ;


    // for ratio spectra/report %%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    named_pointer[name_of_this_element + "_pin_a_raw_when_good"] =
        Tnamed_pointer(&pin_raw[0], &flag_pin_raw_fired[0], this) ;
    named_pointer[name_of_this_element + "_pin_b_raw_when_good"] =
        Tnamed_pointer(&pin_raw[1], &flag_pin_raw_fired[1], this) ;
    named_pointer[name_of_this_element + "_pin_c_raw_when_good"] =
        Tnamed_pointer(&pin_raw[2], &flag_pin_raw_fired[2], this) ;
    named_pointer[name_of_this_element + "_pin_d_raw_when_good"] =
        Tnamed_pointer(&pin_raw[3], &flag_pin_raw_fired[3], this) ;


    named_pointer[name_of_this_element + "_Si_energy_raw_when_fired"]
    = Tnamed_pointer(&Si_energy_raw, &flag_Si_energy_raw_fired, this) ;

    named_pointer[name_of_this_element + "_CsI_energy_raw_when_fired"]
    = Tnamed_pointer(&CsI_energy_raw, &flag_CsI_energy_raw_fired, this) ;


    named_pointer[name_of_this_element + "_Si_time_raw_when_fired"]
    = Tnamed_pointer(&Si_time_raw, &flag_Si_time_raw_fired, this) ;

    named_pointer[name_of_this_element + "_CsI_time_raw_when_fired"]
    = Tnamed_pointer(&CsI_time_raw, &flag_CsI_time_raw_fired, this) ;


}
//************************************************************************
//************************************************************************
Tcate_telescope::~Tcate_telescope()
{
}
//************************************************************************
/** This function is creating the basic spectra dedicatet to the one
    segment (cate_telescope) of the Cate detector */
void Tcate_telescope::create_my_spectra()
{

    string folder = "cate_folder/" + name_of_this_element ;
    string name ;

    // ------THICK CsI ----------------------------
    name = name_of_this_element + "_CsI_energy_raw"  ;
    spec_CsI_energy_raw = new spectrum_1D(name,
                                          name,
                                          4095, 1, 4096,
                                          folder);
    cate_spectra_ptr()->push_back(spec_CsI_energy_raw) ;

    // ----------------------------------
    name = name_of_this_element + "_CsI_time_raw"  ;
    spec_CsI_time_raw = new spectrum_1D(name,
                                        name,
                                        4095, 1, 4096,
                                        folder);
    cate_spectra_ptr()->push_back(spec_CsI_time_raw) ;

    // FIRED ------------

    name = name_of_this_element + "_CsI_energy_fired"  ;
    spec_CsI_energy_fired = new spectrum_1D(name,
                                            name,
                                            4095, 1, 4096,
                                            folder);
    cate_spectra_ptr()->push_back(spec_CsI_energy_fired) ;

    // ----------------------------------
    name = name_of_this_element + "_CsI_time_fired"  ;
    spec_CsI_time_fired = new spectrum_1D(name,
                                          name,
                                          4096, 0, 4096,
                                          folder);
    cate_spectra_ptr()->push_back(spec_CsI_time_fired) ;



    // ------ CALIBRATED THICK CsI ----------------------------
    name = name_of_this_element + "_CsI_energy_cal"  ;
    spec_CsI_energy_cal = new spectrum_1D(name,
                                          name,
                                          4095, 1, 4096,
                                          folder);
    cate_spectra_ptr()->push_back(spec_CsI_energy_cal) ;

    // ----------------------------------
    name = name_of_this_element + "_CsI_time_cal"  ;
    spec_CsI_time_cal = new spectrum_1D(name,
                                        name,
                                        4095, 1, 4096,
                                        folder);
    cate_spectra_ptr()->push_back(spec_CsI_time_cal) ;


    //===========================================
    // ------- Thin Si --------------------------
    name = name_of_this_element + "_Si_time_raw"  ;
    spec_Si_time_raw = new spectrum_1D(name,
                                       name,
                                       4095, 1, 4096,
                                       folder);
    cate_spectra_ptr()->push_back(spec_Si_time_raw) ;

    //--------------------------------------------
    name = name_of_this_element + "_Si_energy_raw"  ;
    spec_Si_energy_raw = new spectrum_1D(name,
                                         name,
                                         4095, 1, 4096,
                                         folder);
    cate_spectra_ptr()->push_back(spec_Si_energy_raw) ;
    //-------------------------------------------
    //============ FIRED ===============

    // ------- Si --------------------------
    name = name_of_this_element + "_Si_time_fired"  ;
    spec_Si_time_fired = new spectrum_1D(name,
                                         name,
                                         4096, 0, 4096,
                                         folder);
    cate_spectra_ptr()->push_back(spec_Si_time_fired) ;

    //--------------------------------------------
    name = name_of_this_element + "_Si_energy_fired"  ;
    spec_Si_energy_fired = new spectrum_1D(name,
                                           name,
                                           4095, 1, 4096,
                                           folder);
    cate_spectra_ptr()->push_back(spec_Si_energy_fired) ;
    //-------------------------------------------

    // CALIBRATED -------------------------------
    // ------- Thin Si --------------------------
    name = name_of_this_element + "_Si_time_cal"  ;
    spec_Si_time_cal = new spectrum_1D(name,
                                       name,
                                       4095, 1, 4096,
                                       folder);
    cate_spectra_ptr()->push_back(spec_Si_time_cal) ;

    //--------------------------------------------
    name = name_of_this_element + "_Si_energy_cal"  ;
    spec_Si_energy_cal = new spectrum_1D(name,
                                         name,
                                         4095, 1, 4096,
                                         folder);
    cate_spectra_ptr()->push_back(spec_Si_energy_cal) ;
    //-------------------------------------------


    // PINS  - positions given by Si detector
    //--------------------------------------------
    for(int i = 0 ; i < 4 ; i++)
    {
        name = name_of_this_element + "_Si_pin_" ;
        name += (char)('a' + i);
        name += "_raw"  ;
        spec_pin_raw[i]
        = new spectrum_1D(name,
                          name,
                          4095, 1, 4096,
                          folder,
                          "Now this is really a gate, but additionally left edge of GATE is used as 'offset' for pin calibration");
        cate_spectra_ptr()->push_back(spec_pin_raw[i]) ;
        //-------------------------------------------
        // calibrated pins -----------------
        //--------------------------------------------
        name = name_of_this_element + "_Si_pin_"  ;
        name += (char)('a' + i);
        name += "_cal"  ;
        spec_pin_cal[i] = new spectrum_1D(name,
                                          name,
                                          4095, 1, 4096,
                                          folder, "No gates here");
        cate_spectra_ptr()->push_back(spec_pin_cal[i]) ;
    }
    //-------------------------------------------


    // ============ FAN spectra

    // mutiplicity of pins ------------------------------------
    name = name_of_this_element + "_fan_pin_multiplicity_raw"  ;
    spec_fan_pin_mult_raw = new spectrum_1D(name,
                                            name,
                                            5, 0, 5,
                                            folder);
    cate_spectra_ptr()->push_back(spec_fan_pin_mult_raw);


    // mutiplicity of pins after calibrating and gating them
    name = name_of_this_element + "_fan_pin_multiplicity_gated"  ;
    spec_fan_pin_mult_gated = new spectrum_1D(name,
            name,
            5, 0, 5,
            folder);
    cate_spectra_ptr()->push_back(spec_fan_pin_mult_gated);

    // existence of  pin signals ----------------------------------
    name = name_of_this_element + "_fan_signals_raw"  ;
    spec_fan_signals_raw = new spectrum_1D(name,
                                           name,
                                           6, 0, 6,
                                           folder , "4 pins, Si energy, Si time");
    cate_spectra_ptr()->push_back(spec_fan_signals_raw);

    // existence of  pin signals after gating
    name = name_of_this_element + "_fan_signals_gated"  ;
    spec_fan_signals_gated = new spectrum_1D(name,
            name,
            6, 0, 6,
            folder);
    cate_spectra_ptr()->push_back(spec_fan_signals_gated);


    // PATTERN -------------------combintaion of raw signals
    name = name_of_this_element + "_fan_pattern_signals_raw"  ;
    spec_fan_pattern_signals_raw = new spectrum_1D(name,
            name,
            70, 0, 70,
            folder);
    cate_spectra_ptr()->push_back(spec_fan_pattern_signals_raw);


    // Matrices ==============================================

    // map of the positions
    name =    name_of_this_element + "_position_xy" ;

    matr_local_position_xy = new spectrum_2D(name,
            name,
            250, -25, 25,   // 25 mm (all 50 mm)
            250, -25, 25,     // -0.5, 0.5,
            folder);

    cate_spectra_ptr()->push_back(matr_local_position_xy) ;


    // map of the positions
    name =    name_of_this_element + "_position_xy_corr" ;

    matr_local_position_xy_corr = new spectrum_2D(name,
            name,
            250, -25, 25,   // 25 mm (all 50 mm)
            250, -25, 25,     // -0.5, 0.5,
            folder);

    cate_spectra_ptr()->push_back(matr_local_position_xy_corr) ;



    // the classical matrix for cate_telescope detectors deltaE / E


    name =   name_of_this_element + "_dE_vs_E_fired" ;

    matr_dE_E_fired = new spectrum_2D(name,
                                      name,
                                      200, 1, 4096,
                                      200, 1, 4096,
                                      folder);

    cate_spectra_ptr()->push_back(matr_dE_E_fired) ;


    // note, this is going to be FIRED & GOOD matrix ------------
//!!!!!!! OBSOLETE, for old users still avaliable

    name =    name_of_this_element + "_dE_vs_E" ;

    matr_dE_E = new spectrum_2D(name,
                                name,
                                200, 1, 4096,
                                200, 1, 4096,
                                folder);

    cate_spectra_ptr()->push_back(matr_dE_E) ;

// note, this is going to be FIRED & GOOD matrix ------------
    name =    name_of_this_element + "_dE_vs_E_good" ;

    matr_dE_E_good = new spectrum_2D(name,
                                     name,
                                     200, 1, 4096,
                                     200, 1, 4096,
                                     folder);

    cate_spectra_ptr()->push_back(matr_dE_E_good) ;
}
//**************************************************************
vector< spectrum_abstr*>*  Tcate_telescope::cate_spectra_ptr()
{
    return owner->give_spectra_pointer() ;
}
//**************************************************************
/** the most important fuction,  executed for every event */
void Tcate_telescope::analyse_current_event()
{

    //  cout << "   analyse_current_event()          for "
    //        << name_of_this_element
    //        << endl ;

    static int warning_counter;

    if(!(warning_counter++ % 50000))
    {
        cout  << (par_force_segment_always_fired ?
                  "Warning: You are working with 'par_force_segment_always_fired'  = 1\n" : "")
              << (par_force_segment_always_fired_good ?
                  "Warning: You are working with 'par_force_segment_always_fired_good' = 1\n" : "")
              << ((par_force_segment_always_fired || par_force_segment_always_fired_good) ?
                  "(This is normal only during testing the CATE. You can change it in : calibraton/cate_calibration.txt\n" : "")
              ;

    }

    flag_segment_CsI_fired = false ;
    flag_segment_Si_fired  = false ;

    flag_Si_segment_fired_good = false ;
    flag_CsI_segment_fired_good = false ;

    flag_Si_and_CsI_fired_good = false;


    flag_xy_ok = false;
    x_abs = x = y = y_abs = x_corr = y_corr = 0;

    flag_pin_raw_fired[0] = flag_pin_raw_fired[1] = flag_pin_raw_fired[2] = flag_pin_raw_fired[3] = false ;
    flag_Si_energy_raw_fired =  flag_Si_time_raw_fired =
                                    flag_CsI_energy_raw_fired =  flag_CsI_time_raw_fired = false;
    mean_delta_E = 0 ;


    // take it out from input event to make faster access
    if((Si_energy_raw = event_cate_telescope_data->Si_energy)) flag_Si_energy_raw_fired = true;
    if((Si_time_raw   = event_cate_telescope_data->Si_time))       flag_Si_time_raw_fired = true;

    if((CsI_energy_raw = event_cate_telescope_data->CsI_energy)) flag_CsI_energy_raw_fired = true;
    if((CsI_time_raw   = event_cate_telescope_data->CsI_time))       flag_CsI_time_raw_fired = true;



    //============================================================
    // the CsI   part ============================================
    //============================================================

    spec_CsI_energy_raw->manually_increment(CsI_energy_raw) ;
    spec_CsI_time_raw->manually_increment(CsI_time_raw);



    // checking if CsI  FIRED !!!

    if(par_force_segment_always_fired    // <-- for calibration sometimes Franco needs this
            ||
            (
                CsI_energy_raw > 0  &&  CsI_energy_raw < 4096
                &&
                CsI_time_raw > 0  &&  CsI_time_raw < 4096
            )
      )
    {
        flag_segment_CsI_fired = true ;
        // increment its raw spectra

        // incrementing the raw energy spectrum
        spec_CsI_energy_fired->manually_increment(CsI_energy_raw) ;

        //spec_CsI_energy->manually_increment( 5) ;
        // incrementing the raw time spectrum
        spec_CsI_time_fired->manually_increment(CsI_time_raw);

    }


    //-------------------------------
    CsI_energy_calibrated = (CsI_energy_calibr_factors[1]
                             *
                             (CsI_energy_raw
                              + randomizer::randomek()))
                            + CsI_energy_calibr_factors[0]  ;


    // time
    CsI_time_calibrated = (CsI_time_calibr_factors[1] *
                           (CsI_time_raw
                            + randomizer::randomek()))
                          + CsI_time_calibr_factors[0]  ;


// Franco wants calibrated spectra now matter if time is OK or not, so below is commented

    spec_CsI_energy_cal->manually_increment(CsI_energy_calibrated) ;
    spec_CsI_time_cal->manually_increment(CsI_time_calibrated) ;
    owner->spec_total_CsI_time_cal->manually_increment(CsI_time_calibrated) ;
    owner->CsI_time_calibrated = CsI_time_calibrated;
#ifdef CATE_PRESENT
    TIFJAnalysis::fxCaliEvent->cate_E_seg[segment_nr] = CsI_energy_calibrated;
#endif //CATE_PRESENT

    if(
        par_force_segment_always_fired_good   // <-- Franco needs this
        ||
        (
            CsI_energy_calibrated >= 1 && CsI_energy_calibrated < 4096
            &&
            CsI_time_calibrated >= 10 && CsI_time_calibrated < 3800
        )
    )
    {
        owner->multiplicity_of_CsI_hits++ ;
        flag_CsI_segment_fired_good = true ;
    }


    //============================================================
    // the Si   part ============================================
    //============================================================
    spec_Si_energy_raw->manually_increment(Si_energy_raw) ;
    spec_Si_time_raw->manually_increment(Si_time_raw);

    // checking if Si FIRED !!!

    if(par_force_segment_always_fired    // <--- Franco needs this sometimes for calibration of Cate
            ||
            (
                Si_energy_raw > 0  &&  Si_energy_raw < 4096
                &&
                Si_time_raw > 0  &&  Si_time_raw < 4096
            )
      )
    {
        flag_segment_Si_fired = true ;
        spec_Si_energy_fired->manually_increment(Si_energy_raw) ;
        spec_Si_time_fired->manually_increment(Si_time_raw);
    }

    if(RisingAnalysis_ptr->is_verbose_on() && (flag_segment_CsI_fired || flag_segment_Si_fired))
    {
        cout
                << name_of_this_element
                << " --->  CsI_energy_raw = " << CsI_energy_raw
                << ",  CsI_time_raw = " << CsI_time_raw
                << ",  Si_time_raw = " << Si_time_raw
                << ", Si_energy_raw = " << Si_energy_raw
                << endl;
    }



    Si_energy_calibrated = (Si_energy_calibr_factors[1] *
                            (Si_energy_raw
                             + randomizer::randomek()))
                           + Si_energy_calibr_factors[0]  ;

    spec_Si_energy_cal->manually_increment(Si_energy_calibrated) ;
    TIFJAnalysis::fxCaliEvent->cate_dE_seg[segment_nr] = Si_energy_calibrated;


    mean_delta_E =
        sqrt(
            Si_energy_calibrated *
            owner->give_Si_energy_cal_from_si_det_at_target()
        );

    // time --------------------
    Si_time_calibrated = (Si_time_calibr_factors[1] *
                          (Si_time_raw
                           + randomizer::randomek()))
                         + Si_time_calibr_factors[0]  ;



    if(
        par_force_segment_always_fired_good    // for Franco
        ||
        (
            Si_energy_calibrated > 0  && Si_energy_calibrated < 4096
            && Si_time_raw != 0                                                   // <---- otherwise it was multiplicity 9
//     &&
//     Si_time_calibrated >= 10 && Si_time_calibrated < 3800       // commented also
        )
    )
    {
        owner->multiplicity_of_Si_hits++ ;
        flag_Si_segment_fired_good = true ;

        spec_Si_time_cal->manually_increment(Si_time_calibrated) ;
        owner->spec_total_Si_time_cal->manually_increment(Si_time_calibrated) ;
    }



    //the legal situation is when any of 5 signals if present
    pin_raw[0] = event_cate_telescope_data->pin_a;
    pin_raw[1] = event_cate_telescope_data->pin_b;
    pin_raw[2] = event_cate_telescope_data->pin_c;
    pin_raw[3] = event_cate_telescope_data->pin_d;


    spec_pin_raw[0]->manually_increment(pin_raw[0])  ;
    spec_pin_raw[1]->manually_increment(pin_raw[1])  ;
    spec_pin_raw[2]->manually_increment(pin_raw[2])  ;
    spec_pin_raw[3]->manually_increment(pin_raw[3])  ;

    // mutiplicity of pins ------------------------------------
    int pin_multipl = 0 ;
    for(int i = 0 ; flag_Si_segment_fired_good && (i < 4) ; i++)
    {
        if((pin_raw[i] > 250) && (pin_raw[i] < 4096))
        {
            flag_pin_raw_fired[i] = true ;  // <--- for rates spectrum/report

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
        if(pin_raw[i]  > high_g[i]) continue;
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
        double mianownik = pin_cal[0]  +  pin_cal[1]  +  pin_cal[2]  +  pin_cal[3] ;

        if(mianownik != 0)
        {
            x = (50.0 / 2)
                * ((pin_cal[1] + pin_cal[2]) - (pin_cal[3] + pin_cal[0]))
                /  mianownik ;


            x_corr = (x_position_corr_factors[1] * x) + x_position_corr_factors[0]  ;


            y = (50.0 / 2)
                * ((pin_cal[3] + pin_cal[2]) - (pin_cal[0] + pin_cal[1]))
                / mianownik ;

            y_corr = (y_position_corr_factors[1] * y) + y_position_corr_factors[0]  ;


            flag_xy_ok = true ;

            y_abs = y + central_y ;
            x_abs = x + central_x ;

            matr_local_position_xy->manually_increment(x, y);

            matr_local_position_xy_corr->manually_increment(x_corr, y_corr);
        }
    }


    //--------------------------------------------
    //============== Si and CsI together
    //--------------------------------------------
    // matrix Delta E vs E


    if(flag_segment_CsI_fired  && flag_segment_Si_fired)
    {

        // this sould be only FIRED matrix
        matr_dE_E_fired ->manually_increment(CsI_energy_calibrated,
                                             Si_energy_calibrated);


        //the same, but total matrix is in the Cate object

        if(flag_CsI_segment_fired_good  && flag_Si_segment_fired_good)
        {
            flag_Si_and_CsI_fired_good = true;


            // OBSOLATE -----------
            matr_dE_E ->manually_increment(CsI_energy_calibrated,
                                           Si_energy_calibrated);

            // New way
            matr_dE_E_good ->manually_increment(CsI_energy_calibrated,
                                                Si_energy_calibrated);


            owner->matr_total_dE_E->manually_increment(
                CsI_energy_calibrated,
                Si_energy_calibrated);
            owner->dE_E_ok = true;
            owner->dE = Si_energy_calibrated;
            owner->E = CsI_energy_calibrated;

        }
    }
}
//**************************************************************
/** reading calibration, gates, setting pointers */
void Tcate_telescope::make_preloop_action(ifstream & s)
{
    //
    //  cout << "preloop action for cate_telescope "
    //       << name_of_this_element
    //       << endl ;



    // cout << "Reading the calibration for the cate_telescope "
    //        << name_of_this_element
    //        << endl ;

    if(!event_cate_telescope_data)
    {
        cout << "Alarm, in " << name_of_this_element
             << " the pointer to event was not set" << endl;
        exit(1) ;
    }



    // in this file we look for a string
    int how_many_factors = 2 ;   // max linear calibration

    string slowo = name_of_this_element
                   + "_CsI_energy_cal_factors" ;

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo);

    CsI_energy_calibr_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess = "I found keyword:" + slowo
                          + ", but error was in reading its value." ;
            throw mess ;
        }
        // if ok
        CsI_energy_calibr_factors.push_back(value) ;

    }

    if(CsI_energy_calibr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for energy calibration CsI of " + name_of_this_element ;
        throw mess ;
    }


    // reading the time calibration factors -----------

    slowo = name_of_this_element + "_CsI_time_cal_factors" ;

    Tfile_helper::spot_in_file(s, slowo);

    CsI_time_calibr_factors.clear() ;
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

        CsI_time_calibr_factors.push_back(value) ;

    }

    if(CsI_time_calibr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for time calibration CsI of " + name_of_this_element ;
        throw mess ;
    }

    //==================================
    // The same for Si detectors
    slowo = name_of_this_element + "_Si_energy_cal_factors" ;

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

    slowo = name_of_this_element + "_Si_time_cal_factors" ;

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



    //char name_of_pin[100] = "" ;
    string name_of_pin ;

    for(int symb = 0 ; symb < 4 ; symb++)
    {
        ostringstream plik ; // (name_of_pin, sizeof(name_of_pin));
        plik << name_of_this_element << "_pin_"
             << char('a' + symb)
             << "_cal_factors" ;
        // << ends ;   //<------------------ !!!!!! error for strings

        slowo = name_of_pin = plik.str();

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
        throw mess ;
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
        throw mess ;
    }


    // Reading "par_force" parameters - if they exist. They use to make "fired", or "fired_good"
    // always - this is usefull for Franco for Cate calibration analysis
    try
    {

        par_force_segment_always_fired =  Tfile_helper::find_in_file(s, "par_force_segment_always_fired");
        par_force_segment_always_fired_good =  Tfile_helper::find_in_file(s, "par_force_segment_always_fired_good");

    }
    catch(...)
    {
        par_force_segment_always_fired = false;
        par_force_segment_always_fired_good = false;
        // repair the stream
        s.clear(s.rdstate() & ~(ios::eofbit | ios::failbit));
    }



    //========================================


    // ---------------------
    // Geometry: phi, theta
    //----------------------

    // but this is in a different file....

    // cout << " reading the geometry angles  for cate_telescope "
    //        << name_of_this_element
    //        << endl ;


    //-----------------

    // here we read the Geometry
    // the geometry is actually the information
    // 1. how far away from the target is the Cate detector (Silicon part)
    //                                 (this is given by the constructor)

    // 2. how big x,y is one cate_telescope (50 mm)
    // 3. how big is the gap between them (now 4 mm)
    // all this we need to know the position of the centre of the telscope


    string geometry_file = "calibration/cate_geometry.txt" ;

    try
    {

        ifstream plik_geometry(geometry_file.c_str());
        if(! plik_geometry)
        {
            string mess ;
            mess += "I can't open  file: " + geometry_file  ;
            throw mess ;
        }
        slowo = name_of_this_element + "_center_x_and_y";

        Tfile_helper::spot_in_file(plik_geometry, slowo);
        plik_geometry >> zjedz >> central_x
                      >> zjedz >> central_y ;
        if(!plik_geometry) throw 1;

    }
    catch(string sss)
    {
        cerr << "Error while reading geometry file "
             << geometry_file
             << ":\n"
             << sss << endl  ;
        exit(-1) ; // is it healthy ?
    }
    catch(int p)
    {
        cerr << "Error while reading (inside geometry file "
             << geometry_file
             << ") position of the centre of the "
             << name_of_this_element
             << "\nI found keyword '"
             << slowo
             << "' but error was while reading its (two) values (x and y)."
             << endl  ;

        exit(-1) ; // is it healthy ?

    }


    read_gates();

}
//***************************************************************
/** Reading the gates for all pins signals (raw) */
void Tcate_telescope::read_gates()
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
                 << "\nI create a gate and assmue that the gate is wide open ! \a\n"
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
                cout <<  "Error while reading 2 numbers from the gate file: "
                     + nam_gate  ;
                exit(-1) ;
            }
            plik_gate.close() ;
        }

    } // for
}
//************************************************************
/** where in the event starts data for this particular cate_telescope */
void Tcate_telescope::set_event_data_pointers(cate_segment * ptr)
{
    event_cate_telescope_data = ptr ;
//  cout << " Accepted " << ptr << endl;

}
//************************************************************

//***********************************************************
/** if the CsI segment had signals */
bool Tcate_telescope::was_segment_hit()
{
    return flag_segment_CsI_fired ;
// HJ says CsI is more likely to work
    // than the Si part
}
//*************************************************************
#endif // CATE_PRESENT

