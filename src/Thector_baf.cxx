/***************************************************************************
                          thector_BaF.cpp  -  description
                             -------------------
    begin                : Wed May 21 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/
#include "Thector_baf.h"
#include <cmath>



#include "Tfile_helper.h"
#include "randomizer.h"

#include "Tnamed_pointer.h"
#include "Tself_gate_hec_descr.h"

#include "Tcrystal_xia.h"  // for doppler corr
#include "Ttarget.h"  // for doppler corr
#include "TIFJAnalysis.h" // for ntuple
#include "Tkratta.h"


// STATIC values ---------------------------

/** does the user require some thereshold value? */
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::flag_good_fast_requires_threshold;
/** does the user require some thereshold value? */
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::flag_good_slow_requires_threshold;
/** does the user require some thereshold value? */
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::flag_good_time_requires_threshold;

//------- En fast
template  <typename TOwnerClass>
int Thector_BaF<TOwnerClass>::fast_threshold  ;
//------- En slow
template  <typename TOwnerClass>
int Thector_BaF<TOwnerClass>::slow_threshold  ;
//------- time
template  <typename TOwnerClass>
int Thector_BaF<TOwnerClass>::good_time_threshold ;

// static
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::flag_increment_fast_cal_with_zero;
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::flag_increment_slow_cal_with_zero;
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::flag_increment_time_cal_with_zero;


// enums declared inside the Tcrystal class
template  <typename TOwnerClass>
type_of_tracking_requirements   Thector_BaF<TOwnerClass>::tracking_position_on_target ;

template  <typename TOwnerClass>
type_of_lycca_requirements       Thector_BaF<TOwnerClass>::tracking_position_on_lycca ;

template  <typename TOwnerClass>
type_of_doppler_algorithm       Thector_BaF<TOwnerClass>::which_doppler_algorithm ;

template  <typename TOwnerClass>
bool   Thector_BaF<TOwnerClass>::flag_beta_from_frs ; // which beta to use, frs or fixed

template  <typename TOwnerClass>
double Thector_BaF<TOwnerClass>::beta_fixed_value ;  // if fixed, here it is

template  <typename TOwnerClass>
double Thector_BaF<TOwnerClass>::energy_deposit_in_target ; // bec. beta on the other side

// of the target will be slower

template  <typename TOwnerClass>
Ttarget * Thector_BaF<TOwnerClass>::target_ptr ;      // for faster acces of the doppler correction


#define dcout  if(0)cout

//*************************************************************************
// these arguments nr_slow, nr_fast tell us  in which channel of ADC this data
// was send.
// hector_adc[32], hector_tdc[32]
// for example  BaF nr 6 can get data from adc[2], adc[14], tdc[8]

template  <typename TOwnerClass>
Thector_BaF<TOwnerClass>::Thector_BaF(TOwnerClass *ptr_owner, string core_name, int nr_det,
                                      int nr_slow,
                                      int nr_fast,
                                      int nr_time,
                                      int statist_offset)
    :owner(ptr_owner), det_nr(nr_det),
      channel_nr_of_slow(nr_slow),
      channel_nr_of_fast(nr_fast),
      channel_nr_of_time(nr_time), stat_off(statist_offset)
{
    my_selfgate_type =  Tselfgate_type::hector;

    name_of_this_element = core_name ;
    if(det_nr < 10) name_of_this_element += "0";
    name_of_this_element +=  to_string(det_nr) ;

    //....................................................
    // warning: The above pointers should point to the TOTAL spectra
    // which belong to  the Hector object. They do NOT exist now, because the
    // Hector constructor runs later than this constructor
    // So these pointers had to be set in the preloop

    spec_total_slow_cal = 0  ;
    spec_total_fast_cal= 0  ;

    //   /*spec_total_slow_doppler= 0  ;
    //   spec_total_fast_doppler= 0  ;*/

    matr_total_fast_vs_slow_cal= 0  ;
    //   matr_total_fast_vs_slow_doppler = 0  ;

    spec_total_time_cal = 0;
    matr_total_time_vs_slow_cal = 0;

    //....................................................

    named_pointer[name_of_this_element+"_fast_vs_slow_rotated_x_when_ok"]
            = Tnamed_pointer(&fast_vs_slow_rotated_x,
                             &flag_fast_vs_slow_rotated_ok, this) ;

    named_pointer[name_of_this_element+"_fast_vs_slow_rotated_y_when_ok"]
            = Tnamed_pointer(&fast_vs_slow_rotated_y,
                             &flag_fast_vs_slow_rotated_ok, this) ;

    create_my_spectra();

    named_pointer[name_of_this_element+"_detector_number_when_fired"]
            = Tnamed_pointer(&det_nr, &flag_fired, this) ;


    named_pointer[name_of_this_element+"_slow_raw"]
            = Tnamed_pointer(&slow_raw, 0, this) ;


    named_pointer[name_of_this_element+"_fast_raw"]
            = Tnamed_pointer(&fast_raw, 0, this) ;

    named_pointer[name_of_this_element+"_time_raw"]
            = Tnamed_pointer(&time_raw, 0, this) ;




    named_pointer[name_of_this_element+"_slow_cal"]
            = Tnamed_pointer(&slow_cal, &flag_fired, this) ;


    named_pointer[name_of_this_element+"_fast_cal"]
            = Tnamed_pointer(&fast_cal, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_time_cal"]
            = Tnamed_pointer(&time_cal, &flag_fired, this) ;


    named_pointer[name_of_this_element+"_slow_cal_when_good"]
            = Tnamed_pointer(&slow_cal, &flag_good_data, this) ;


    named_pointer[name_of_this_element+"_fast_cal_when_good"]
            = Tnamed_pointer(&fast_cal, &flag_good_data, this) ;

    named_pointer[name_of_this_element+"_time_cal_when_good"]
            = Tnamed_pointer(&time_cal, &flag_good_data, this) ;


    //-------------------

    /*
      named_pointer[name_of_this_element+"_fast_doppler_when_good"]
      = Tnamed_pointer(&fast_doppler, &flag_doppler_successful, this) ;

      named_pointer[name_of_this_element+"_slow_doppler_when_good"]
      = Tnamed_pointer(&slow_doppler,  &flag_doppler_successful, this) ;*/

    //  named_pointer[name_of_this_element+"_fast_doppler_when_ok"]
    //    = Tnamed_pointer(&fast_doppler, &flag_doppler_successful, this) ;
    //
    //  named_pointer[name_of_this_element+"_slow_doppler_when_ok"]
    //    = Tnamed_pointer(&slow_doppler,  &flag_doppler_successful, this) ;

    //-------------

    string owner_name = owner->name_of_this_element;

    string entry_name =  "ALL_" + owner_name + "_BaF_slow_energy_cal_when_good" ;
    Tnamed_pointer::add_as_ALL(entry_name, &slow_cal, &flag_good_data, this);


    entry_name =  "ALL_" + owner_name + "_BaF_fast_energy_cal_when_good" ;
    Tnamed_pointer::add_as_ALL(entry_name, &fast_cal, &flag_good_data, this);

    entry_name =  "ALL_" + owner_name + "_BaF_time_cal_when_good" ;
    Tnamed_pointer::add_as_ALL(entry_name, &time_cal, &flag_good_data, this);



    named_pointer[name_of_this_element+"_time_cal_minus_kratta_pd1_time_cal__when_good"]
            = Tnamed_pointer(&time_cal_diff_to_kratta_pd1_time_cal, &flag_time_cal_diff_to_kratta, this) ;


    //========= doppler ================

    //   entry_name =  "ALL_" + owner_name + "_BaF_slow_doppler_when_good" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &slow_doppler, &flag_doppler_successful, this);
    //
    //
    //   entry_name =  "ALL_" + owner_name + "_BaF_fast_doppler_when_good" ;
    //   Tnamed_pointer::add_as_ALL(entry_name, &fast_doppler, &flag_doppler_successful, this);

    // for algorithmic

    named_pointer[name_of_this_element+"_alg_LaBr_rotated_raw_when_in_gate"]
            = Tnamed_pointer(&LaBr_raw, nullptr, this) ;

    named_pointer[name_of_this_element+"_alg_LaBr_rotated_cal_when_in_gate"]
            = Tnamed_pointer(&LaBr_cal, nullptr, this) ;

    named_pointer[name_of_this_element+"_alg_NaI_rotated_raw_when_in_gate"]
            = Tnamed_pointer(&NaI_raw, &flag_rotation_possible , this) ;

    named_pointer[name_of_this_element+"_alg_NaI_rotated_cal_when_in_gate"]
            = Tnamed_pointer(&NaI_cal, &flag_rotation_possible, this) ;

    named_pointer[name_of_this_element+"_alg_Compton_rotated_raw_when_in_gate"]
            = Tnamed_pointer(&Compton_raw, &flag_rotation_possible , this) ;

    named_pointer[name_of_this_element+"_alg_Compton_rotated_cal_when_in_gate"]
            = Tnamed_pointer(&Compton_cal, &flag_rotation_possible , this) ;


    named_pointer[name_of_this_element+"_alg_NaI_plus_Compton_rotated_raw_when_in_gates"]
            = Tnamed_pointer(&NaI_plus_Compt_raw, &flag_rotation_possible , this) ;

    named_pointer[name_of_this_element+"_alg_NaI_plus_Compton_rotated_cal_when_in_gates"]
            = Tnamed_pointer(&NaI_plus_Compt_cal, &flag_rotation_possible , this) ;

}
//*************************************************************************
template  <typename TOwnerClass>
Thector_BaF<TOwnerClass>::~Thector_BaF()
{}
//*************************************************************************
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::create_my_spectra()
{
//    constexpr int up_limit_cal = 30001;
     constexpr int up_limit_cal = 20001;

    constexpr int up_limit_raw = 4096;
    string folder = "hector/" + name_of_this_element ;
    // ------SLOW  ----------------------------
    string name = name_of_this_element + "_slow_raw"  ;
    spec_slow_raw = new spectrum_1D( name,
                                     up_limit_raw, 1, up_limit_raw,   // was 4096
                                     folder , "",
                                     name_of_this_element+"_slow_raw");
    owner->frs_spectra_ptr->push_back(spec_slow_raw) ;



    // ----------------------------------
    //   name = name_of_this_element + "_slow_dopplered"  ;
    //   spec_slow_doppler = new spectrum_1D( name,
    //                                        4095, 1, 4096,
    //                                        folder );
    //   owner->frs_spectra_ptr->push_back(spec_slow_doppler) ;
    //



    //------------- FAST -----------------------------------------
    name = name_of_this_element + "_fast_raw"  ;
    spec_fast_raw = new spectrum_1D( name,
                                     up_limit_raw, 1, up_limit_raw,
                                     folder , "",
                                     name_of_this_element+"_fast_raw");
    owner->frs_spectra_ptr->push_back(spec_fast_raw) ;


#if 0
    // ----------------------------------
    name = name_of_this_element + "_fast_dopplered"  ;
    spec_fast_doppler = new spectrum_1D( name,
                                         4095, 1, 4096,
                                         folder );
    owner->frs_spectra_ptr->push_back(spec_fast_doppler) ;
#endif




    //------------- time -----------------------------------------
    string comment { "Note: times of Paris are *100" };
    if(det_nr >=28) comment = "Note: times of LaBr are *1";

    name = name_of_this_element + "_time_raw"  ;
    spec_time_raw =
            new spectrum_1D( name,
                             14000, -5000, 9000,
                             folder , comment,
                             name_of_this_element+"_time_raw");
    owner->frs_spectra_ptr->push_back(spec_time_raw) ;


    //  time_gate_low = 0 ;
    //  time_gate_high = 4095 ;

    // ----------------------------------

    //#ifdef HECTOR_CALIBRATE_TIME


    name = name_of_this_element + "_time_cal"  ;
    spec_time_cal = new spectrum_1D( name,
                                     4095, 1, 4096,
                                     folder , "",
                                     name_of_this_element+"_time_cal");
    owner->frs_spectra_ptr->push_back(spec_time_cal) ;
    //#endif



    //#ifdef HECTOR_CALIBRATE_TIME
    name = name_of_this_element + "_time_cal_diff_to_kratta_pd1_time_cal"  ;
    spec_time_cal_diff_to_kratta = new spectrum_1D( name,
                                                    4095, 1, 4096,
                                                    folder, "",
                                                    name_of_this_element+"_time_cal_minus_kratta_pd1_time_cal__when_good");
    owner->frs_spectra_ptr->push_back(spec_time_cal_diff_to_kratta) ;
    //#endif



    // Using Michal Ciemala algoritm for rotation and projection

    name = name_of_this_element + "_alg_LaBr_raw"  ;
    spec_LaBr_raw = new spectrum_1D( name,
                                     up_limit_raw, 1, up_limit_raw,   // was 4096
                                     folder , "from the fast_vs_slow (raw) polygon gate: LaBr",
                                     "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back(spec_LaBr_raw) ;


    name = name_of_this_element + "_alg_LaBr_cal"  ;
    spec_LaBr_cal = new spectrum_1D( name,
                                     up_limit_raw, 1, up_limit_raw,   // was 4096
                                     folder , "from the fast_vs_slow (raw) polygon gate: LaBr",
                                     "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back(spec_LaBr_cal) ;

    // NaI -----

    name = name_of_this_element + "_alg_NaI_rotated_raw"  ;
    spec_NaI_raw = new spectrum_1D( name,
                                    up_limit_raw, 1, up_limit_raw,   // was 4096
                                    folder , "after rotation, but from the fast_vs_slow (raw) polygon gate: NaI",
                                    "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back(spec_NaI_raw) ;



    name = name_of_this_element + "_alg_NaI_rotated_cal"  ;
    spec_NaI_cal = new spectrum_1D( name,
                                    up_limit_raw, 1, up_limit_raw,   // was 4096
                                    folder , "after rotation, but from the fast_vs_slow (raw) polygon gate: NaI",
                                    "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back(spec_NaI_cal) ;

    // Compton ---

    name = name_of_this_element + "_alg_Compton_rotated_raw"  ;
    spec_Compton_rotated_raw = new spectrum_1D( name,
                                        up_limit_raw, 1, up_limit_raw,   // was 4096
                                        folder , "after rotation, but from the fast_vs_slow (raw) polygon gate: Compton",
                                        "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back(spec_Compton_rotated_raw) ;


    name = name_of_this_element + "_alg_Compton_rotated_cal"  ;
    spec_Compton_ratated_cal = new spectrum_1D( name,
                                        up_limit_raw, 1, up_limit_raw,   // was 4096
                                        folder , "after rotation, but from the fast_vs_slow (raw) polygon gate: Compton",
                                        "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back(spec_Compton_ratated_cal) ;




    // NaI PLUS Compton ---

    name = name_of_this_element + "_alg_NaI_plus_Compton_raw"  ;
    spec_NaI_plus_Compt_raw = new spectrum_1D( name,
                                               up_limit_raw, 1, up_limit_raw,   // was 4096
                                               folder , "after rotation, but from the fast_vs_slow (raw) polygon gates: NaI or Compton",
                                               "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back(spec_NaI_plus_Compt_raw) ;


    name = name_of_this_element + "_alg_NaI_plus_Compton_cal"  ;
    spec_NaI_plus_Compt_cal = new spectrum_1D( name,
                                               up_limit_raw, 1, up_limit_raw,   // was 4096
                                               folder , "after rotation, but from the fast_vs_slow (raw) polygon gates: NaI or Compton",
                                               "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back(spec_NaI_plus_Compt_cal) ;


    // ----------------------------------
    // matrices
    //-----------------------------------

    name =   name_of_this_element + "_fast_vs_slow" ;

    matr_fast_vs_slow = new spectrum_2D( name,
                                         512, 1, up_limit_raw,
                                         512, 1, up_limit_raw,
                                         folder, "",
                                         "X: " + name_of_this_element+"_slow_raw" +
                                         "\nY: " + name_of_this_element+"_fast_raw"

                                         );

    owner->frs_spectra_ptr->push_back(matr_fast_vs_slow) ;
    //----------

    name =   name_of_this_element + "_fast_vs_slow_rotated" ;
    matr_fast_vs_slow_rotated = new spectrum_2D( name,
                                                 512, -4096, 4096,
                                                 512, -4096, 4096,
                                                 folder, "",
                                                 "X: " + name_of_this_element + "_fast_vs_slow_rotated_x_when_ok" +
                                                 "\nY: " + name_of_this_element + "_fast_vs_slow_rotated_y_when_ok");
    owner->frs_spectra_ptr->push_back(matr_fast_vs_slow_rotated) ;

    //--------------
    name =   name_of_this_element + "_fast_vs_slow_cal" ;
    matr_fast_vs_slow_cal = new spectrum_2D( name,
                                             500, 1, up_limit_cal,
                                             500, 1, up_limit_cal,
                                             folder, "",
                                             "X: " + name_of_this_element+"_slow_cal" +
                                             "\nY: " + name_of_this_element+"_fast_cal");

    owner->frs_spectra_ptr->push_back(matr_fast_vs_slow_cal) ;

    //--------------
    name =   name_of_this_element + "_time_vs_slow" ;
    matr_time_vs_slow = new spectrum_2D( name,
                                         1024, 1, 8192,
                                         2000, -2000, 2000,
                                         folder, "",
                                         "X: " + name_of_this_element+"_slow_raw" +
                                         "\nY: " + name_of_this_element+"_time_raw" );

    owner->frs_spectra_ptr->push_back(matr_time_vs_slow) ;

    //--------------

    name =   name_of_this_element + "_time_vs_slow_cal" ;
    matr_time_vs_slow_cal = new spectrum_2D( name,
                                             512, 1, 4096,
                                             1000, -2000, 4000,
                                             folder, "",
                                             "X: " + name_of_this_element+"_slow_cal" +
                                             "\nY: " + name_of_this_element+"_time_cal");

    owner->frs_spectra_ptr->push_back(matr_time_vs_slow_cal) ;



    // Adam invented this
    //--------------
    name =   name_of_this_element + "_fast_o_slow_const_vs_time" ;
    matr_fast_over_slow_vs_time = new spectrum_2D( name,
                                                   512, 1, up_limit_raw,
                                                   512, 1, up_limit_raw,
                                                   folder, "",
                                                   "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back( matr_fast_over_slow_vs_time) ;




    // Basia invented this
    //--------------
    name =   name_of_this_element + "_slow_minus_fast_over_slow_cal" ;
    matr_slow_minus_fast_over_slow_cal = new spectrum_2D( name,
                                                          999, 1, 4096,
                                                          999, 0, 1,
                                                          folder, "",
                                                          "No_such_incrementer_defined");
    owner->frs_spectra_ptr->push_back( matr_slow_minus_fast_over_slow_cal) ;

}
//*************************************************************************
//************************************************************************
/** readinng calibration, gates, setting pointers */
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::make_preloop_action(ifstream & s)
{
    //
    //  cout << "preloop action for "
    //       << name_of_this_element
    //       << endl ;
    //

#if 1
    // why it is done here, look at the comment in the constructor
    spec_total_slow_cal = owner->spec_total_slow_cal ;
    spec_total_fast_cal= owner->spec_total_fast_cal;

    //   spec_total_slow_doppler= owner->spec_total_slow_doppler;
    //   spec_total_fast_doppler= owner->spec_total_fast_doppler;
    matr_total_fast_vs_slow_cal= owner->matr_total_fast_vs_slow_cal;
    //   matr_total_fast_vs_slow_doppler = owner->matr_total_fast_vs_slow_doppler  ;

    spec_total_time_cal= owner->spec_total_time_cal;
    matr_total_time_vs_slow_cal=owner->matr_total_time_vs_slow_cal;
#else
    spec_total_slow_cal =
            spec_total_fast_cal=
            spec_total_time_cal= nullptr;
    matr_total_fast_vs_slow_cal=
            matr_total_time_vs_slow_cal= nullptr;
#endif

    //  check_legality_of_data_pointers();



    // cout << "Reading the calibration for the "
    //        << name_of_this_element
    //        << endl ;

    // in this file we look for a string
    int how_many_factors = 2 ;   // max linear calibration

    string slowo = name_of_this_element + "_slow_cal_factors" ;

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo );

    slow_calibr_factors.clear() ;

    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess =
                    "I found keyword '" + slowo +
                    "' but error was in reading its value." ;
            throw mess ;
        }
        // if ok
        slow_calibr_factors.push_back(value ) ;
    }


    // reading the FAST calibration factors --------------
    slowo = name_of_this_element + "_fast_cal_factors" ;

    Tfile_helper::spot_in_file(s, slowo );
    fast_calibr_factors.clear();
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess =
                    "I found keyword '" + slowo
                    + "' but error was in reading its value." ;
            throw mess ;
        }
        // if ok

        fast_calibr_factors.push_back(value ) ;
        //cout << name_of_this_element << ", fast factor = " << value << endl;
    }

    // reading the time calibration factors --------------
    slowo = name_of_this_element + "_time_cal_factors" ;

    Tfile_helper::spot_in_file(s, slowo );
    time_calibr_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess =
                    "I found keyword '" + slowo
                    + "' but error was in reading its value." ;
            throw mess ;
        }
        // if ok
        time_calibr_factors.push_back(value ) ;
    }

    //-----------------------------------------------------------
    // calibration of x after rotation of fast_vs_slow matrix

    slowo = name_of_this_element + "_slow_rotated_cal_factors" ;

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo );

    fast_vs_slow_rotated_x_calibr_factors.clear() ;

    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s)
        {
            string mess =
                    "I found keyword '" + slowo +
                    "' but error was in reading its value." ;
            throw mess ;
        }
        // if ok
        fast_vs_slow_rotated_x_calibr_factors.push_back(value ) ;
    }



    // ------------------------------------------------
    // it is one for all (so far)
    slowo = "constant_factor_to_multiply_fast";
    constant_factor_to_multiply_fast
            = Tfile_helper::find_in_file(s, slowo);
    if(!s)
    {
        string mess =
                "I found keyword '" + slowo
                + "' but error was in reading its value." ;
        throw mess ;
    }

    // ---------------------
    // Geometry: phi, theta
    //----------------------

    // but this is in a different file....

    // cout << " reading the geometry angles  for telescope "
    //        << name_of_this_element
    //        << endl ;


    //-----------------

    // here we read the Geometry
    //


    string geometry_file = "calibration/paris_geometry.txt" ;

    try
    {
        ifstream plik_geometry(geometry_file.c_str() );
        if(! plik_geometry)
        {
            string mess = "I can't open  file: " + geometry_file  ;
            throw mess ;
        }
        slowo = name_of_this_element + "_phi_theta";

        Tfile_helper::spot_in_file(plik_geometry, slowo);
        plik_geometry >> zjedz >> phi_geom >> zjedz >> theta_geom
                >> zjedz >> distance_from_target ;
        if(!plik_geometry) throw 1;

        distance_from_target *= 10 ; // it was in [cm] and we need in milimeters !
        phi_geom_radians   =  M_PI * phi_geom / 180.0 ;
        theta_geom_radians =  M_PI * theta_geom / 180.0;

        // ============== recalculate phi and theta into x,y,z of the crystal
        crystal_position_x = distance_from_target * sin(theta_geom_radians) * cos(phi_geom_radians) ;
        crystal_position_y = distance_from_target * sin(theta_geom_radians) * sin(phi_geom_radians) ;
        double ttt = cos(theta_geom_radians);
        crystal_position_z = distance_from_target * ttt;

    }
    catch(string sss)
    {
        cerr << "Error while reading geometry file "
             << geometry_file
             << ":\n"
             << sss << endl  ;

        exit(-1) ; // is it healthy ?
    }

    catch(int  p[[maybe_unused]])
    {
        cerr << "\nError while reading (inside geometry file "
             << geometry_file
             << ") phi, theta and distance  for "
             << name_of_this_element
             << "\nI found keyword '"
             << slowo
             << "' but error was while reading its (double) values of phi "
             << "and theta."
             << endl  ;
        cerr << "NOTE: The old version of spy didn't want the 'distance' value\n"
                "Solution: Please take a copy the file "
             << "cracow_online/calibration/hector_geometry.txt\n"
                "and put in into your ./calibration   directory\n" << endl;

        exit(-1) ; // is it healthy ?

    }
    catch(Tno_keyword_exception &capsule)
    {
        cerr << "Error while reading geometry file "
             << geometry_file
             << ":\n"
             << capsule.message << endl ;
        exit(-1) ;
    }

    //-------------------------------------------------------
    // read the default gate on time ------------------------
    //-------------------------------------------------------

    //  string core_of_the_name = "gates/" + name_of_this_element ;
    //  string nam_gate = core_of_the_name + "_time_raw_gate_GATE.gate" ;
    //
    //  ifstream plik_gatesX(nam_gate.c_str() );

    //  if(! plik_gatesX)
    //  {
    //    string mess = "I can't open  file: " + nam_gate
    //      + "\nThis gate you can produce interactively using a 'cracow' viewer "
    //      "program" ;
    //    cout << mess
    //         << "\nI create a gate and assmue that the "
    //      "gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
    //         << endl ;
    //
    //    ofstream pliknew(nam_gate.c_str() );
    //    pliknew   << time_gate_low << "   "    // lower limit
    //              << time_gate_high << endl ;
    //  }else{
    //
    //    double tmp1, tmp2 ;
    //    plik_gatesX >> zjedz >> tmp1     // lower limit
    //                >> zjedz >> tmp2 ;   // upper limit
    //
    //    time_gate_low = (int)tmp1 ;   // lower limit
    //    time_gate_high = (int)tmp2 ;
    //
    //    if(! plik_gatesX)
    //    {
    //      string mess = "Error while reading 2 numbers from the gate file: "
    //        + nam_gate  ;
    //      exit(-1) ;
    //    }
    //    plik_gatesX.close() ;
    //  }

    prepare_rotation();
    read_important_polygons();
}
//***************************************************************************
//***************************************************************************
/** // the most important function,  executed for every event */
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::analyse_current_event()
{

    //  cout << "   Thector_BaF::analyse_current_event()          for "
    //              << name_of_this_element
    //                << endl ;
    time_raw = *ptr_to_time ;
    slow_raw = *ptr_to_slow ;
    fast_raw = *ptr_to_fast;   // divided by 4, beacause it was far after the range of spectrum (32000)

    flag_fired = flag_good_data  = flag_fast_vs_slow_rotated_ok = false ;
    slow_cal = fast_cal = time_cal =   0 ;
    slow_doppler = fast_doppler  = 0 ;   // doppler corrected values

    // remember to zero the algorithm varibles !!!!!!!!!!!!!!!!
    fast_vs_slow_rotated_x = 0;
    fast_vs_slow_rotated_x_cal = 0;
    fast_vs_slow_rotated_y = 0;
    // for algorithmic
    LaBr_raw =
            LaBr_cal =
            NaI_raw =
            NaI_cal =
            Compton_raw =
            Compton_cal =
            NaI_plus_Compt_raw =
            NaI_plus_Compt_cal = 0.0;


    flag_time_cal_diff_to_kratta = false;
    flag_doppler_successful = false ;
    //==================================


    // checking if it is something (not zero)
    if(fast_raw ||  slow_raw  || time_raw)     // if any of them had something
    {
        flag_fired = true ;

        if(RisingAnalysis_ptr->is_verbose_on() )
        {
            cout  << name_of_this_element
                  << ", E slow=  " << slow_raw
                  << ", E fast = " << fast_raw
                  << ", T = " << time_raw
                  <<endl;
        }

        // simple signals statistics
        if(fast_raw)owner->spec_statistics->manually_increment(stat_off+(10*det_nr));   // there was fast
        if(slow_raw)owner->spec_statistics->manually_increment(stat_off+(10*det_nr) + 1);   // there was slow
        if(time_raw)owner->spec_statistics->manually_increment(stat_off+(10*det_nr) + 2);   // there was time
        if(fast_raw && time_raw)owner->spec_statistics->manually_increment(stat_off+(10*det_nr) + 3);   // there was time& fast
        if(slow_raw && time_raw)owner->spec_statistics->manually_increment(stat_off+(10*det_nr) + 4);   // there was time &slow
        if(fast_raw && slow_raw)owner->spec_statistics->manually_increment(stat_off+(10*det_nr) + 5);   // there was time& fast
        if(fast_raw && slow_raw && time_raw)owner->spec_statistics->manually_increment(stat_off+(10*det_nr) + 6);   // there was time& fast
        // where the statistics of markers ? (in Hector object!)

        //===================================
        if(slow_raw)
            spec_slow_raw->manually_increment( slow_raw);

        if(fast_raw)
            spec_fast_raw->manually_increment( fast_raw);

        if(time_raw )
            spec_time_raw->manually_increment( time_raw);




        // if not, (or anyway) make a rotation

        if(fast_raw &&  slow_raw){

            matr_fast_vs_slow->manually_increment( slow_raw, fast_raw);

            if(flag_rotation_possible)
            {
                rotate(slow_raw, fast_raw, fast_vs_slow_rotated_x, fast_vs_slow_rotated_y);
                matr_fast_vs_slow_rotated->manually_increment(fast_vs_slow_rotated_x, fast_vs_slow_rotated_y);
                // here on this matrix shoud be 2 banana
                // if (in compton gate banana) -> put the rotated data x on spectrum: Compton_slow_not_cal and cal
                // if (in NaI gate banana) -> put the rotated data x on spectra NaI_slow_not_cal and cal

            }
        }

        // Time
        if( time_raw &&  slow_raw )   matr_time_vs_slow->manually_increment( slow_raw, time_raw);

        // Adam invented this, but does he want it calibrated ?

        if( time_raw &&  slow_raw && fast_raw)
            matr_fast_over_slow_vs_time->manually_increment(
                        double(time_raw),
                        (constant_factor_to_multiply_fast *  (fast_raw)) / (slow_raw)
                        );

        // CALIBRATION -----------------------------------------


        // NOTE: so far the option of non incrementing calibrated
        // spectra when raw data is zero - is:
        //   a) disabled in the dialog window (cracow)
        //   b) not used here
        // it seams thea the condition that both of time and slow
        // must be non zero (as above if(...) )
        // is stronger

        if(slow_raw && !slow_calibr_factors.empty())
        {
            slow_cal =
                    (slow_calibr_factors[1]  *  (slow_raw + randomizer::randomek()))
                    + slow_calibr_factors[0]  ;

            spec_total_slow_cal->manually_increment(slow_cal) ;
        }


        // fast---------------------------
        if(fast_raw && !fast_calibr_factors.empty())
        {
            fast_cal =
                    (fast_calibr_factors[1] * (fast_raw  + randomizer::randomek()))
                    + fast_calibr_factors[0];

            //            if(owner->not_led())
            //                spec_fast_cal_not_led->manually_increment(fast_cal) ;

            if(slow_raw) matr_fast_vs_slow_cal->manually_increment( slow_cal,fast_cal);

            //                   cout << name_of_this_element << ", fast_raw = " << fast_raw
            //                   << ", fast_cal " << fast_cal
            // << " fast_calibr_factors[0] = " << fast_calibr_factors[0]
            // << " fast_calibr_factors[1] = " << fast_calibr_factors[1]
            //<< endl;

            // totals
            spec_total_fast_cal->manually_increment(fast_cal);
            matr_total_fast_vs_slow_cal->manually_increment(slow_cal, fast_cal);

            if(slow_cal > 1)
            {
                matr_slow_minus_fast_over_slow_cal->manually_increment(
                            slow_cal, (slow_cal - fast_cal) /slow_cal
                            );
                //                cout << "inkrem x [slow_cal] = " << slow_cal << ", y = "
                //                     << (slow_cal - fast_cal)/slow_cal  << endl;

            }
        }


        //===========================================================
        // Sofisticated analysis invented by Michal Ciemala
        //===========================================================

        if(! fast_vs_slow_rotated_x_calibr_factors.empty() )
        {
            fast_vs_slow_rotated_x_cal =
                    (fast_vs_slow_rotated_x_calibr_factors[1]  *  (fast_vs_slow_rotated_x + randomizer::randomek()))
                    + fast_vs_slow_rotated_x_calibr_factors[0]  ;
        }
        // Checking if it is in the LaBr banana gate
        // if yes, incremnt the 1D spectru LaBr_fast_raw and cal
        if(polyg_LaBr->Test(slow_raw, fast_raw))
        {
            LaBr_raw = fast_raw;
            LaBr_cal = fast_cal;

            spec_LaBr_raw->manually_increment(fast_raw);
            spec_LaBr_cal->manually_increment(fast_cal);
        }
        else if(polyg_Compton->Test(slow_raw, fast_raw))
        {
            if(flag_rotation_possible){

                spec_Compton_rotated_raw->manually_increment(fast_vs_slow_rotated_x);
                spec_Compton_ratated_cal->manually_increment(fast_vs_slow_rotated_x_cal);

                spec_NaI_plus_Compt_raw->manually_increment(fast_vs_slow_rotated_x);
                spec_NaI_plus_Compt_cal->manually_increment(fast_vs_slow_rotated_x_cal);
            }
        }
        else if(polyg_NaI->Test(slow_raw, fast_raw))
        {
            if(flag_rotation_possible){
                NaI_raw= fast_vs_slow_rotated_x;
                NaI_cal = fast_vs_slow_rotated_x_cal;

                spec_NaI_raw->manually_increment(fast_vs_slow_rotated_x);
                spec_NaI_cal->manually_increment(fast_vs_slow_rotated_x_cal);

                NaI_plus_Compt_raw = fast_vs_slow_rotated_x;
                NaI_plus_Compt_cal = fast_vs_slow_rotated_x_cal;

                spec_NaI_plus_Compt_raw->manually_increment(fast_vs_slow_rotated_x);
                spec_NaI_plus_Compt_cal->manually_increment(fast_vs_slow_rotated_x_cal);
            }
        }
        else  // none of these gates
        {
            // zero is inside of
        }
        //=======================================================================


        // time-----------------------------------------------------------------

        if(time_raw && !time_calibr_factors.empty())
        {
            if(time_calibr_factors.size() >= 2)
            {
                time_cal = (time_calibr_factors[1]  *
                        (time_raw + randomizer::randomek()))
                        + time_calibr_factors[0]  ;

                spec_time_cal->manually_increment(time_cal) ;
                matr_time_vs_slow_cal->manually_increment(slow_cal,time_cal);

                // totals
                spec_total_time_cal->manually_increment(time_cal) ;
                matr_total_time_vs_slow_cal->manually_increment( slow_cal, time_cal);
            }
        }

        // Setting the good data flag
        if(
                (flag_good_fast_requires_threshold == false
                 ||
                 fast_cal > fast_threshold )
                &&
                (flag_good_slow_requires_threshold == false
                 ||
                 slow_cal > slow_threshold )
                &&
                (flag_good_time_requires_threshold == false
                 ||
                 time_cal > good_time_threshold )

                )
        {
            flag_good_data = true ;
            owner-> multiplicity++;

            if( RisingAnalysis_ptr->frs.get_kratta_ptr()->get_flag_only_one_good())
            {
                flag_time_cal_diff_to_kratta = true;
                time_cal_diff_to_kratta_pd1_time_cal = time_cal -
                        RisingAnalysis_ptr->frs.get_kratta_ptr()->get_general_pd1_time_cal__when_only_one_good();

                spec_time_cal_diff_to_kratta->manually_increment(time_cal_diff_to_kratta_pd1_time_cal);
            }

        }


#if 0
        // adding th the ntuple
        TIFJAnalysis::fxCaliEvent-> hect_ener_slow_cal_not_led[det_nr-1] =
                slow_cal_not_led;
        // adding to the ntuple
        TIFJAnalysis::fxCaliEvent-> hect_ener_fast_cal_not_led[det_nr-1] =
                fast_cal_not_led;
        TIFJAnalysis::fxCaliEvent->hect_time_cal_not_led[det_nr-1] =
                time_cal_not_led;
        // above -1 is because numbers are 1-8 but ntuple leaves are 0-7

#endif
        //        }

#if 0
        //############################################################
        // now the doppler correction, according to the same algorithm as in Rising
        //############################################################


        // NOTE !!!
        // at first we have to ask target if the parameters of this event
        // fullfill the requirements which were set in the options
        // (for example we want exact tracking, while in this event
        // we can have only estimated).
        // if these requirements are not fullfilled - we do not make
        // the doppler correction, event is rejected



        switch(which_doppler_algorithm)
        {
        //    case polar_coordinates:
        //      if(Tcrystal::target_ptr->is_doppler_possible()) doppler_corrector_polar();
        //      break ;
        case tracked_algorithm:
            if(Tcrystal_xia::target_ptr->is_doppler_possible()) doppler_corrector_TVector3();
            break;

        case basic_algorithm:
            doppler_corrector_basic();
            break;

        default:

            cout
                    << "Error, the type of doppler correction algorithm was not decided "
                       "in the file: option/doppler.options\n" << endl ;
            break ;
        }


        //    doppler_corrector_basic();

        if(flag_doppler_successful)
        {

            spec_slow_doppler->manually_increment(slow_doppler);
            spec_fast_doppler->manually_increment(fast_doppler);

            spec_total_slow_doppler->manually_increment(slow_doppler);
            spec_total_fast_doppler->manually_increment(fast_doppler);


            matr_total_fast_vs_slow_doppler->
                    manually_increment(slow_doppler, fast_doppler);
        }
        // .......
#endif
    } // end of all fast/slow/time  are not zero

}
//***************************************************************************
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::set_event_digitizer_data_pointers(int32_t *digitizer_array_ptr)
{
    ptr_to_slow = &digitizer_array_ptr[channel_nr_of_slow];
    ptr_to_fast = &digitizer_array_ptr[channel_nr_of_fast];
    ptr_to_time = &digitizer_array_ptr[channel_nr_of_time];
}
//****************************************************************************
//****************************************************************************
template  <typename TOwnerClass>
bool  Thector_BaF<TOwnerClass>::check_selfgate(Tself_gate_abstract_descr *desc)
{

    // here we assume that this is really germ
    Tself_gate_hec_descr *d = static_cast<Tself_gate_hec_descr*> (desc) ;
    if(d->enable_energy_fast_gate)
    {
        if(fast_cal < d->en_fast_gate[0]
                ||
                fast_cal > d->en_fast_gate[1])
            return false ;
    }


    if(d->enable_energy_slow_gate)
    {
        if(slow_cal < d->en_slow_gate[0]
                ||
                slow_cal > d->en_slow_gate[1])
            return false ;
    }


    if(d->enable_time_gate)
    {
        if(time_cal < d->time_gate[0]
                ||
                time_cal > d->time_gate[1])
            return false ;
    }

    // polygon here

    if(d->enable_fast_vs_slow_polygon_gate)
    {
        // polygon


        if(d->polygon->Test(slow_cal, fast_cal) == false)
        {
            //cout << " is outside " << endl;;
            return false;
        }
    }

    //---------------
    if(d->enable_geom_theta_gate)
    {
        if(theta_geom < d->geom_theta_gate[0]
                ||
                theta_geom > d->geom_theta_gate[1])
            return false;
    }

    if(d->enable_geom_phi_gate)
    {
        if(phi_geom < d->geom_phi_gate[0]
                ||
                phi_geom > d->geom_phi_gate[1])
            return false;
    }

    //---------------
    if(d->enable_gp_theta_gate)
    {
        if(theta_radians_between_gamma_vs_scattered_particle <  d->gp_theta_gate[0]
                ||
                theta_radians_between_gamma_vs_scattered_particle > d->gp_theta_gate[1])
            return false ;
    }

    if(d->enable_gp_phi_gate)
    {
        if(phi_radians_between_gamma_vs_scattered_particle  < d->gp_phi_gate[0]
                ||
                phi_radians_between_gamma_vs_scattered_particle > d->gp_phi_gate[1])
            return false;
    }

#ifdef NIGDY
    //------------ so far calculation if this multiplicity was not yet implemented
    if(enable_mult_in_cluster_gate)
    {
        if( d->mult_in_cluster_gate[0]
                d->mult_in_cluster_gate[1]
                )
    }
#endif

    return true ;// when EVETHING was successful
}
//****************************************************************************
//****************************************************************************
/** this function contains the algorithm of the doppler correction
    prepared by Samit  */
//****************************************************************************

//*******************************************************************
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::set_doppler_corr_options
(int typ_track, int typ_lycca_pos, int typ_doppl_algor)
{
    tracking_position_on_target = static_cast<type_of_tracking_requirements> (typ_track);
    tracking_position_on_lycca   = static_cast<type_of_lycca_requirements> (typ_lycca_pos);
    which_doppler_algorithm = static_cast<type_of_doppler_algorithm> (typ_doppl_algor);
}
//***************************************************************
/** Based on the simplest algorithm (from Hans-Juergen) */
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::doppler_corrector_basic()
{
#if 0
    // this algorithm below is on the web page ===================
    double beta = target_ptr->give_beta_after_target();

    double doppler_factor =    ( 1.0 - beta * cos(theta_geom_radians))    // <--- theta of this crystal
            / (sqrt(1.0 - beta * beta));


    fast_doppler = fast_cal * doppler_factor ;
    slow_doppler = slow_cal * doppler_factor ;

    dcout << "==== Gamma registered during this event =================================\n"
          << name_of_this_element
          << " (Geometrical phi " << phi_geom << "[deg],  rad= " << phi_geom_radians
          << " theta " << theta_geom << " [deg],  rad= " << theta_geom_radians << ")\n"

          << "\n: Gamma vector   \n\tdetector.x = " << detector.X()
          << ", (because crystal_position_x = " << crystal_position_x
          << " -  target x = " << target_ptr->give_x_tracked()
          << ")\n\tyv1 = " <<  detector.Y()
          << ", (because crystal_position_y = " << crystal_position_y
          << " -  target y = " << target_ptr->give_y_tracked()
          << ")\n\tzv1 = " <<  detector.Z()
          << ", (because  crystal_position_z = " << crystal_position_z
             //<< " -  target z = " << target_ptr->give_z()
          << ")" << endl ;


    dcout << "Doppler_factor = " <<  doppler_factor << endl;
    flag_doppler_successful = true ;
#endif
}
//****************************************************************
/** this function contains the algorithm of the doppler correction
using the class TVector3 from ROOT library */
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::doppler_corrector_TVector3()
{
#if 0
    //detector.SetMagThetaPhi(1, theta_geom_radians, phi_geom_radians);

    detector.SetXYZ(  crystal_position_x - target_ptr->give_x_tracked() ,
                      crystal_position_y - target_ptr->give_y_tracked() ,
                      crystal_position_z) ;

    dcout << "==== Gamma registered during this event =================================\n"
          << name_of_this_element
          << " (Geometric phi " << phi_geom << "[deg],  rad= " << phi_geom_radians
          << " theta " << theta_geom << " [deg],  rad= " << theta_geom_radians << ")\n"
          << "\n: Gamma vector   \n\tdetector.x = " << detector.X()
          << ", (because crystal_position_x = " << crystal_position_x
          << " -  target x = " << target_ptr->give_x_tracked()
          << ")\n\tyv1 = " <<  detector.Y()
          << ", (because crystal_position_y = " << crystal_position_y
          << " -  target y = " << target_ptr->give_y_tracked()
          << ")\n\tzv1 = " <<  detector.Z()
          << ", (because  crystal_position_z = " << crystal_position_z
             //<< " -  target z = " << target_ptr->give_z()
          << ")" << endl ;

    theta_radians_between_gamma_vs_scattered_particle = detector.Angle(target_ptr->outgoing);
    double cos_theta_gamma_scattered_particle = cos(theta_radians_between_gamma_vs_scattered_particle);

    double beta = target_ptr->give_beta_after_target();

    dcout
            << "TVector3  cos_theta_gamma_scattered_particle " << cos_theta_gamma_scattered_particle
            << ", \ntheta_radians_between_gamma_vs_scattered_particle "
            << theta_radians_between_gamma_vs_scattered_particle
            << "\n, Beta = " << beta << endl;

    // using Alex way -------------------
    if(beta > 0.05  &&   beta < 1.0)
    {
        double doppler_factor=  ( 1.0 - (beta * cos_theta_gamma_scattered_particle) )
                / (sqrt(1.0 - (beta * beta) ));

        dcout << "Doppler factor = " << doppler_factor << endl;
        // Now using this factor
        fast_doppler = fast_cal * doppler_factor;
        slow_doppler = slow_cal * doppler_factor ;
        flag_doppler_successful = true ;
    }
    else
    {
        flag_doppler_successful = false ;
    }
#endif
}
//*****************************************************************
template  <typename TOwnerClass>
void Thector_BaF<TOwnerClass>::rotate(double x, double y, double &xr, double &yr)
{
    if(flag_rotation_possible)
    {
        xr = (x * cosinus_omega) - (y * sinus_omega);
        yr = (x * sinus_omega) + ( y * cosinus_omega);
        flag_fast_vs_slow_rotated_ok = true;
    }
    else {
        xr = 0;
        yr = 0;
        //cout << "Rotation impossible" << endl;
    }
}
//*****************************************************************
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::prepare_rotation()
{
    // find polygon for the fast_vs_slow_cal matrix

    TjurekPolyCond   *polygon = nullptr;
    string name =   name_of_this_element + "_fast_vs_slow_polygon_rotation.poly" ;
    flag_rotation_possible = read_banana(
                name,
                &polygon);
    if(!flag_rotation_possible)
    {
        // do it new


        // creating it ----------------------------------------
        ofstream plik(("polygons/" + name).c_str());
        if(!plik)         exit(1);



        double points[8] =
        {
            876.126,		326,		 // x, y of the point nr 0	 polygon: rotation
            1260.15,		1701,		 // x, y of the point nr 1	 polygon: rotation
            1204.76,		396,		 // x, y of the point nr 2	 polygon: rotation
            1042.29,		216		 // x, y of the point nr 3	 polygon: rotation
        };

        for(int n = 0 ; n < 8 ; n += 2)
            plik << points[n]  << "\t" << points[n + 1]  << "\n";

        plik.close();
        // once more we try something what really exit
        if(! read_banana(name, &polygon))
        {
            cout << "Could not create the polygon " << name << " for you " << endl;
            exit(125);
        }

        flag_LaBr_polygon_possible = true;

    } // end if flag rotation


    // find first vertext and calculate tangens omega = y/x
    vector<double> vx;
    vector<double> vy;
    polygon->give_point_vectors(vx, vy); // references
    if(vx.size()<2 || vy.size() < 2) {
        flag_rotation_possible = false;
        return false;
    }

    double x = vx[1] - vx[0];
    double y = vy[1] - vy[0];
    double omega = atan(x / y);

    sinus_omega = sin(omega);
    cosinus_omega = cos(omega);
    return true;
}
//*****************************************************************
template  <typename TOwnerClass>
bool Thector_BaF<TOwnerClass>::read_important_polygons()
{

    string poly_name = name_of_this_element +
            "_fast_vs_slow_polygon_LaBr.poly";
    if(read_banana(poly_name, &polyg_LaBr))
    {

    } else {

        // do it new
        cerr << "During Reading-in the parameters for Paris LaBr "
             << name_of_this_element
             << " it is impossible to read polygon gate: \n"
             << poly_name
                //<< "  - Most probably it does not exist \n"
             << "\n!!! The polygon is going to be automaticaly created for you - set its position properly"
             << endl;

        // creating it ----------------------------------------
        ofstream plik(("polygons/" + poly_name).c_str());
        if(!plik)         exit(1);



        double points[8] =
        {
            3445,	3570,		 // x, y of the point nr 0	 polygon: LaBr
            5,		25,		 // x, y of the point nr 1	 polygon: LaBr
            40,		18,		 // x, y of the point nr 2	 polygon: LaBr
            3608,	3561		 // x, y of the point nr 3	 polygon: LaBr
        };




        for(int n = 0 ; n < 8 ; n += 2)
            plik << points[n]  << "\t" << points[n + 1]  << "\n";

        plik.close();
        // once more we try something what really exit
        if(! read_banana(poly_name, &polyg_LaBr))
        {
            cout << "Could not create the polygon " << poly_name << " for you " << endl;
            exit(125);
        }

    }
    flag_LaBr_polygon_possible = true;


    //=======================================================================

    poly_name = name_of_this_element + "_fast_vs_slow_polygon_Compton.poly";

    if(read_banana(poly_name, &polyg_Compton))
    {

    } else {

        // do it new
        cerr << "During Reading-in the parameters for Paris LaBr "
             << name_of_this_element
             << " it is impossible to read polygon gate: \n"
             << poly_name
                //<< "  - Most probably it does not exist \n"
             << "\n!!! The polygon is going to be automaticaly created for you - set its position properly"
             << endl;

        // creating it ----------------------------------------
        ofstream plik(("polygons/" + poly_name).c_str());
        if(!plik)         exit(1);



        double points[8] =
        {
            3320.57,		3141,		 // x, y of the point nr 0	 polygon: Compton
            3630.74,		2391,		 // x, y of the point nr 1	 polygon: Compton
            259.173,		200.512,		 // x, y of the point nr 2	 polygon: Compton
            202.53,         170.353		 // x, y of the point nr 3	 polygon: Compton
        };

        for(int n = 0 ; n < 8 ; n += 2)
            plik << points[n]  << "\t" << points[n + 1]  << "\n";

        plik.close();
        // once more we try something what really exit
        if(! read_banana(poly_name, &polyg_Compton))
        {
            cout << "Could not create the polygon " << poly_name << " for you " << endl;
            exit(125);
        }

    }
    flag_Compton_polygon_possible = true;

    //=============================================================

    poly_name = name_of_this_element +
            "_fast_vs_slow_polygon_NaI.poly";

    if(read_banana(poly_name, &polyg_NaI))
    {

    } else {

        // do it new
        cerr << "During Reading-in the parameters for Paris LaBr "
             << name_of_this_element
             << " it is impossible to read polygon gate: \n"
             << poly_name
                //<< "  - Most probably it does not exist \n"
             << "\n!!! The polygon is going to be automaticaly created for you - set its position properly"
             << endl;

        // creating it ----------------------------------------
        ofstream plik(("polygons/" + poly_name).c_str());
        if(!plik)         exit(1);



        int points[8] =
        {
            3800,	2271,		 // x, y of the point nr 0	 polygon: NaI
            148,	72,		 // x, y of the point nr 1	 polygon: NaI
            140,	106,		 // x, y of the point nr 2	 polygon: NaI
            3796,	2401		 // x, y of the point nr 3	 polygon: NaI
        };

        for(int n = 0 ; n < 8 ; n += 2)
            plik << points[n]  << "\t" << points[n + 1]  << "\n";

        plik.close();
        // once more we try something what really exit
        if(! read_banana(poly_name, &polyg_NaI))
        {
            cout << "Could not create the polygon " << poly_name << " for you " << endl;
            exit(125);
        }

    }
    flag_NaI_polygon_possible = true;

    if(flag_LaBr_polygon_possible &&  flag_Compton_polygon_possible && flag_NaI_polygon_possible)
        return true;
    else
        return false;
}
//*************************************************************************
// #ifdef HECTOR_PRESENT
// #endif  // if hector present
