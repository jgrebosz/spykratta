#include "Ttof_measurement.h"
#include "Tnamed_pointer.h"
#include "Tscintillator.h"
//***********************************************************************
//  constructor
Ttof_measurement::Ttof_measurement(
    string name_of,
    int TIFJEvent::*left_left_ptr,
    int TIFJEvent::*right_right_ptr,
    Tscintillator * start,   // for Zsolt algorithm
    Tscintillator * stop   // for Zsolt algorithm
) :
    Tfrs_element(name_of),
    left_left(left_left_ptr),
    right_right(right_right_ptr),
    start_sci(start),
    stop_sci(stop)
{
    // pointers
    spec_tof_RR_raw = 0 ; // null
    spec_tof_LL_raw = 0 ; // null
    spec_tof_RR_cal = 0 ; // null
    spec_tof_LL_cal = 0 ; // null
    spec_tof_ps     = 0 ; // null

    create_my_spectra();


    named_pointer[name_of_this_element + "_tof_LL"]
    = Tnamed_pointer(&tof_LL , 0, this) ;
    named_pointer[name_of_this_element + "_tof_RR"]
    = Tnamed_pointer(&tof_RR , 0, this) ;
    named_pointer[name_of_this_element + "_tof_LL_cal"]
    = Tnamed_pointer(&tof_LL_cal , 0, this) ;
    named_pointer[name_of_this_element + "_tof_RR_cal"]
    = Tnamed_pointer(&tof_RR_cal , 0, this) ;

    named_pointer[name_of_this_element + "_tof_in_picoseconds_when_tof_ok"]
    = Tnamed_pointer(&tof_ps  , &flag_tof_ok, this) ;

    named_pointer[name_of_this_element + "_beta_when_tof_ok"]
    = Tnamed_pointer(&beta , &flag_tof_ok, this) ;

}
//************************************************************************
void Ttof_measurement::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "tof_folder/" + name_of_this_element ;
    //  Note: for
    //          first tof the length of calibrated spectra could be
    //                                 10000, 0, 1000000
    //          second tof the length of calibrated spectra could be
    //                                        1000, 0,  100000

    // --------------  RAW, the   ----------------------
    string name =  name_of_this_element + "_RR_raw"  ;
    spec_tof_RR_raw = new spectrum_1D(name,
                                      name,
                                      4096, 0, 4096,
                                      folder, "",  name_of_this_element + "_tof_RR");  // always
    frs_spectra_ptr->push_back(spec_tof_RR_raw) ;

    // ------------------------------------
    name =  name_of_this_element + "_LL_raw"  ;
    spec_tof_LL_raw = new spectrum_1D(name,
                                      name,
                                      4096, 0, 4096,
                                      folder, "", name_of_this_element + "_tof_LL");  // always
    frs_spectra_ptr->push_back(spec_tof_LL_raw) ;

    // -------------- CALIBRATED ----------------------
    name =  name_of_this_element + "_RR_cal"  ;
    spec_tof_RR_cal = new spectrum_1D(name,
                                      name,
                                      10000, 0, 1000000,
                                      folder, "", name_of_this_element + "_tof_RR_cal");  // see note above

    frs_spectra_ptr->push_back(spec_tof_RR_cal) ;

    // ------------------------------------
    name =  name_of_this_element + "_LL_cal"  ;
    spec_tof_LL_cal = new spectrum_1D(name,
                                      name,
                                      10000, 0, 1000000,
                                      folder, "", name_of_this_element + "_tof_LL_cal");  // see note above

    frs_spectra_ptr->push_back(spec_tof_LL_cal) ;


    // ---------------- picoseconds --------------------
    name =  name_of_this_element + "_picoseconds"  ;
    spec_tof_ps = new spectrum_1D(name,
                                  name,
                                  10000, 0, 1000000,
                                  folder, "",
                                  name_of_this_element + "_tof_in_picoseconds_when_tof_ok");   // see note above

    frs_spectra_ptr->push_back(spec_tof_ps) ;

    //-----------------------------------------------
    name =  name_of_this_element + "_beta"  ;
    spec_beta = new spectrum_1D(name,
                                name,
                                1000, 0, 1,
                                folder, "",
                                name_of_this_element + "_beta_when_tof_ok");  // see note above

    frs_spectra_ptr->push_back(spec_beta) ;

}
//**********************************************************************
void Ttof_measurement::analyse_current_event()
{


    //  cout << "analyse_current_event()  for "
    //  <<  name_of_this_element << "" << endl ;
    flag_tof_ok = false ;

    int tof_LL_raw = event_unpacked->*left_left;
    int tof_RR_raw = event_unpacked->*right_right;

    // we do not want to increment with zeros
    if(tof_LL_raw) spec_tof_LL_raw->manually_increment(tof_LL_raw)  ;
    if(tof_RR_raw) spec_tof_RR_raw->manually_increment(tof_RR_raw)  ;


    // calculations

    tof_LL = (event_unpacked->*left_left)   + randomizer::randomek() ;
    tof_RR = (event_unpacked->*right_right) + randomizer::randomek() ;


    tof_LL_cal = (tof_LL * tof_L_slope) + tof_L_offset;
    tof_RR_cal = (tof_RR * tof_R_slope) + tof_R_offset;

    if(tof_LL_raw)spec_tof_LL_cal->manually_increment(tof_LL_cal)  ;
    if(tof_RR_raw)spec_tof_RR_cal->manually_increment(tof_RR_cal)  ;

    if(
        (tof_LL_cal > 1500)
        &&
        (tof_RR_cal > 1500)
    )
    {
        flag_tof_ok = true ;
        // arithmetic average
        tof_ps = (tof_LL_cal + tof_RR_cal) / 2 ;
        // fake: tof_ps = tof_RR_cal ;

        if(zsolt_algorithm_active && start_sci && stop_sci)
        {
            if(start_sci->is_position_ok() && stop_sci->is_position_ok())
            {
                double correction =
                    (start_sci->give_position() + stop_sci->give_position()) / 0.3;
                tof_ps -= correction;
            }

        }

        spec_tof_ps->manually_increment(tof_ps) ;

        //=============================================
        // to calcultate something what was directly at the beginning of identification
        // algorithm


        // calibration factor for beta_tof, are from the file
        beta =   beta_calibr_factor1 / (beta_calibr_factor0  - tof_ps)  ;

        if(beta > 0 && beta < 1.0)
        {
            gamma = 1.0 / (sqrt(1.0 - (beta * beta)));

            bega = beta * gamma;

            //id = 1400 + i;
            //call_hf1(id, beta_tof_sc21_41, 1.);
            spec_beta->manually_increment(beta);
        }

    } // if both tofs were

    calculations_already_done = true ;


}
//**************************************************************************
// read the calibration factors, gates
//**************************************************************************
void  Ttof_measurement::make_preloop_action(ifstream & s)
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;

    // in this file we look for a string


    tof_L_offset = Tfile_helper::find_in_file(s, name_of_this_element  + "_L_offset");
    tof_L_slope  = Tfile_helper::find_in_file(s, name_of_this_element  + "_L_slope");

    tof_R_offset = Tfile_helper::find_in_file(s, name_of_this_element  + "_R_offset");
    tof_R_slope  = Tfile_helper::find_in_file(s, name_of_this_element  + "_R_slope");


    beta_calibr_factor0 = Tfile_helper::find_in_file(s, name_of_this_element  + "_beta_calibr_factor0");
    beta_calibr_factor1 = Tfile_helper::find_in_file(s, name_of_this_element  + "_beta_calibr_factor1");

    try
    {
        // whe have to try if  the Zsolt algorithm is  ON
        zsolt_algorithm_active = Tfile_helper::find_in_file(s, name_of_this_element  + "_Zsolt_algorithm_active");
    }
    catch(Tno_keyword_exception mess)
    {
        zsolt_algorithm_active = false ;
        // stream 's' may be in the error state
        s.clear() ;
    }

    // there are no gates for ToFs

}

//************************************************************************


