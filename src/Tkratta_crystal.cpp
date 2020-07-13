/***************************************************************************
Tkratta_crystal.cpp  -  description
-------------------
begin                : Oct 12 2014
copyright            : (C) 2003 by dr Jerzy Grebosz (IFJ Krakow, Poland)
  email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "experiment_def.h"
#ifdef KRATTA_PRESENT

#include "Tkratta_crystal.h"
#include "Tkratta.h"
#include "Tfile_helper.h"
#include "randomizer.h"
#include <sstream>
#include <iomanip>

#include "Tnamed_pointer.h"
#include "Tself_gate_kratta_descr.h"
#include "TIFJAnalysis.h"   // for verbose mode

// STATIC members
bool  Tkratta_crystal::good_pd0_time_cal_requires_threshold;
bool  Tkratta_crystal::good_pd1_time_cal_requires_threshold;
bool  Tkratta_crystal::good_pd2_time_cal_requires_threshold;

double Tkratta_crystal::pd0_time_cal_lower_threshold;
double Tkratta_crystal::pd0_time_cal_upper_threshold;

double Tkratta_crystal::pd1_time_cal_lower_threshold;
double Tkratta_crystal::pd1_time_cal_upper_threshold;

double Tkratta_crystal::pd2_time_cal_lower_threshold;
double Tkratta_crystal::pd2_time_cal_upper_threshold;

//static
spectrum_2D* Tkratta_crystal::spec_geometry;

//************************************************************************
Tkratta_crystal:: Tkratta_crystal(Tkratta *ptr_owner, string name, int id_nr)
    : owner(ptr_owner), name_of_this_element(name), id(id_nr)
{
    my_selfgate_type =  Tselfgate_type::kratta;

   // segment_nr = id_nr;
    data = nullptr;

    // reading some data for geo file

    // this below is a nonsense, as position can change from
    // one experiment to the other. It is better tu use Phi and Theta
    //    pos_x = KRATTA_NR_OF_CRYSTALS % KRATTA_NCOL;
    //    pos_y = KRATTA_NR_OF_CRYSTALS / KRATTA_NROW;

//      cout << "CREATING" << name_of_this_element << endl;

    create_my_spectra() ;

    // Incrementers
    named_pointer[name_of_this_element+"_pd0_amplitude_raw_when_fired"]
            = Tnamed_pointer(&pd0_amplitude_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd1_amplitude_raw_when_fired"]
            = Tnamed_pointer(&pd1_amplitude_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd2_amplitude_raw_when_fired"]
            = Tnamed_pointer(&pd2_amplitude_raw, &flag_fired, this) ;


    named_pointer[name_of_this_element+"_pd0_time_raw_when_fired"]
            = Tnamed_pointer(&pd0_time_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd1_time_raw_when_fired"]
            = Tnamed_pointer(&pd1_time_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd2_time_raw_when_fired"]
            = Tnamed_pointer(&pd2_time_raw, &flag_fired, this) ;



    named_pointer[name_of_this_element+"_pd0_pedestal_raw_when_fired"]
            = Tnamed_pointer(&pd0_pedestal_raw, &flag_fired, this) ;
    named_pointer[name_of_this_element+"_pd1_pedestal_raw_when_fired"]
            = Tnamed_pointer(&pd1_pedestal_raw, &flag_fired, this) ;
    named_pointer[name_of_this_element+"_pd2_pedestal_raw_when_fired"]
            = Tnamed_pointer(&pd2_pedestal_raw, &flag_fired, this) ;


    named_pointer[name_of_this_element+"_pd0_pedestal_cal_when_fired"]
            = Tnamed_pointer(&pd0_pedestal_cal, &flag_fired, this) ;
    named_pointer[name_of_this_element+"_pd1_pedestal_cal_when_fired"]
            = Tnamed_pointer(&pd1_pedestal_cal, &flag_fired, this) ;
    named_pointer[name_of_this_element+"_pd2_pedestal_cal_when_fired"]
            = Tnamed_pointer(&pd2_pedestal_cal, &flag_fired, this) ;



    //---
    named_pointer[name_of_this_element+"_pd0_amplitude_cal_when_fired"]
            = Tnamed_pointer(&pd0_amplitude_cal, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd1_amplitude_cal_when_fired"]
            = Tnamed_pointer(&pd1_amplitude_cal, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd2_amplitude_cal_when_fired"]
            = Tnamed_pointer(&pd2_amplitude_cal, &flag_fired, this) ;


    named_pointer[name_of_this_element+"_pd0_time_cal_when_fired"]
            = Tnamed_pointer(&pd0_time_cal, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd1_time_cal_when_fired"]
            = Tnamed_pointer(&pd1_time_cal, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd2_time_cal_when_fired"]
            = Tnamed_pointer(&pd2_time_cal, &flag_fired, this) ;

    // when GOOD flag

    named_pointer[name_of_this_element+"_pd0_amplitude_cal_when_good"]
            = Tnamed_pointer(&pd0_amplitude_cal, &flag_good, this) ;

    named_pointer[name_of_this_element+"_pd1_amplitude_cal_when_good"]
            = Tnamed_pointer(&pd1_amplitude_cal, &flag_good, this) ;

    named_pointer[name_of_this_element+"_pd2_amplitude_cal_when_good"]
            = Tnamed_pointer(&pd2_amplitude_cal, &flag_good, this) ;


    named_pointer[name_of_this_element+"_pd0_time_cal_when_good"]
            = Tnamed_pointer(&pd0_time_cal, &flag_good, this) ;

    named_pointer[name_of_this_element+"_pd1_time_cal_when_good"]
            = Tnamed_pointer(&pd1_time_cal, &flag_good, this) ;

    named_pointer[name_of_this_element+"_pd2_time_cal_when_good"]
            = Tnamed_pointer(&pd2_time_cal, &flag_good, this) ;





    Tnamed_pointer::add_as_ALL("ALL_kratta_pd0_amplitude_cal_when_fired", &pd0_amplitude_cal, &flag_fired, this);
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd1_amplitude_cal_when_fired", &pd1_amplitude_cal, &flag_fired, this);
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd2_amplitude_cal_when_fired", &pd2_amplitude_cal, &flag_fired, this);

    Tnamed_pointer::add_as_ALL("ALL_kratta_pd0_time_cal_when_fired", &pd0_time_cal, &flag_fired, this);
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd1_time_cal_when_fired", &pd1_time_cal, &flag_fired, this);
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd2_time_cal_when_fired", &pd2_time_cal, &flag_fired, this);

    // when good
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd0_amplitude_cal_when_good", &pd0_amplitude_cal, &flag_good, this);
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd1_amplitude_cal_when_good", &pd1_amplitude_cal, &flag_good, this);
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd2_amplitude_cal_when_good", &pd2_amplitude_cal, &flag_good, this);

    Tnamed_pointer::add_as_ALL("ALL_kratta_pd0_time_cal_when_good", &pd0_time_cal, &flag_good, this);
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd1_time_cal_when_good", &pd1_time_cal, &flag_good, this);
    Tnamed_pointer::add_as_ALL("ALL_kratta_pd2_time_cal_when_good", &pd2_time_cal, &flag_good, this);


    named_pointer[name_of_this_element+"_phi_geom_deg"]
            = Tnamed_pointer(&phi_geom, nullptr, this) ;
    named_pointer[name_of_this_element+"_theta_geom_deg"]
            = Tnamed_pointer(&theta_geom, nullptr, this) ;

    named_pointer[name_of_this_element+"_distance_from_target"]
            = Tnamed_pointer(&distance_from_target, nullptr, this) ;


    // calculated from theta phi, distance
    named_pointer[name_of_this_element+"_crystal_position_x"]
            = Tnamed_pointer(&crystal_position_x, nullptr, this) ;
    named_pointer[name_of_this_element+"_crystal_position_y"]
            = Tnamed_pointer(&crystal_position_y, nullptr, this) ;
    named_pointer[name_of_this_element+"_crystal_position_z"]
            = Tnamed_pointer(&crystal_position_z, nullptr, this) ;

}
//************************************************************************
Tkratta_crystal::~Tkratta_crystal()
{
}
//************************************************************************
/** This function is creating the basic spectra of one segment (kratta_crystal) of the Krata detector */
void Tkratta_crystal::create_my_spectra()
{
    // to give the proper name to the spectra, at first
    // we shoud read the geo file.

    string folder = "kratta_folder/" + name_of_this_element ;
    string name ;

    // -------------------ENERGY--------------
    name = name_of_this_element + "_pd0_amplitude_raw"  ;
    spec_pd0_amplitude_raw = new spectrum_1D( name,
                                    4095, 1, 4096,
                                    folder,
                                    "",
                                    name_of_this_element+"_pd0_amplitude_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_pd0_amplitude_raw) ;
    // ---------------------------------
    name = name_of_this_element + "_pd1_amplitude_raw"  ;
    spec_pd1_amplitude_raw = new spectrum_1D( name,
                                    4095, 1, 4096,
                                    folder, "",
                                    name_of_this_element+"_pd1_amplitude_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_pd1_amplitude_raw) ;
    // ---------------------------------
    name = name_of_this_element + "_pd2_amplitude_raw"  ;
    spec_pd2_amplitude_raw = new spectrum_1D( name,
                                    4095, 1, 4096,
                                    folder,"",
                                    name_of_this_element+"_pd2_amplitude_raw_when_fired" );
    owner->spectra_ptr()->push_back(spec_pd2_amplitude_raw) ;


    // --------------------TIME-------------
    name = name_of_this_element + "_pd0_time_raw"  ;
    spec_pd0_time_raw = new spectrum_1D( name,
                                         4095, 1, 4096,
                                         folder , "",
                                         name_of_this_element + "_pd0_time_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_pd0_time_raw) ;
    // ---------------------------------
    name = name_of_this_element + "_pd1_time_raw"  ;
    spec_pd1_time_raw = new spectrum_1D( name,
                                         4095, 1, 4096,
                                         folder , "",
                                         name_of_this_element + "_pd1_time_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_pd1_time_raw) ;
    // ---------------------------------
    name = name_of_this_element + "_pd2_time_raw"  ;
    spec_pd2_time_raw = new spectrum_1D( name,
                                         4095, 1, 4096,
                                         folder , "",
                                         name_of_this_element + "_pd2_time_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_pd2_time_raw) ;


    // ---------------------------------
    // CALIBRATED Energy----------------
    // ---------------------------------

    name = name_of_this_element + "_pd0_amplitude_cal"  ;
    spec_pd0_amplitude_cal = new spectrum_1D( name,
                                    4095, 1, 4096,
                                    folder, "",
                                    name_of_this_element+"_pd0_amplitude_cal_when_fired");
    owner->spectra_ptr()->push_back(spec_pd0_amplitude_cal) ;
    // ---------------------------------
    name = name_of_this_element + "_pd1_amplitude_cal"  ;
    spec_pd1_amplitude_cal = new spectrum_1D( name,
                                    4095, 1, 4096,
                                    folder, "",
                                    name_of_this_element+"_pd1_amplitude_cal_when_fired");
    owner->spectra_ptr()->push_back(spec_pd1_amplitude_cal) ;
    // ---------------------------------
    name = name_of_this_element + "_pd2_amplitude_cal"  ;
    spec_pd2_amplitude_cal = new spectrum_1D( name,
                                    4095, 1, 4096,
                                    folder, "",
                                    name_of_this_element+"_pd2_amplitude_cal_when_fired");
    owner->spectra_ptr()->push_back(spec_pd2_amplitude_cal) ;

    // Calibrated time
    name = name_of_this_element + "_pd0_time_cal"  ;
    spec_pd0_time_cal = new spectrum_1D( name,
                                         4095, 1, 4096,
                                         folder, "",
                                         name_of_this_element+"_pd0_time_cal_when_fired");
    owner->spectra_ptr()->push_back(spec_pd0_time_cal) ;
    // ---------------------------------
    name = name_of_this_element + "_pd1_time_cal"  ;
    spec_pd1_time_cal = new spectrum_1D( name,
                                         4095, 1, 4096,
                                         folder, "",
                                         name_of_this_element+"_pd1_time_cal_when_fired");
    owner->spectra_ptr()->push_back(spec_pd1_time_cal) ;
    // ---------------------------------
    name = name_of_this_element + "_pd2_time_cal"  ;
    spec_pd2_time_cal = new spectrum_1D( name,
                                         4095, 1, 4096,
                                         folder, "",
                                         name_of_this_element+"_pd2_time_cal_when_fired");
    owner->spectra_ptr()->push_back(spec_pd2_time_cal) ;



    // = FAN spectrum ===============================

    // PATTERN -------------------combination of raw pds
    name = name_of_this_element + "_fan_pattern_pds_raw"  ;
    spec_fan_pattern_pds_raw = new spectrum_1D( name,
                                                12, 0, 12,
                                                folder,
                                                "presence of raw signals: pd0_amplitude, pd1_amplitude, pd2_amplitude, - - -, pd0_time, pd1_time, pd2_time" ,
                                                "No incrementer existing, but ask Jurek for 'pseudo fan'"
                                                );
    owner->spectra_ptr()->push_back(spec_fan_pattern_pds_raw);


    // Pedestals ==============================================

    name = name_of_this_element + "_pd0_pedestal_raw"  ;
    spec_pd0_pedestal_raw = new spectrum_1D( name,
                                             4095, 1, 4096,
                                             folder, "",
                                             name_of_this_element+"_pd0_pedestal_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_pd0_pedestal_raw) ;


    name = name_of_this_element + "_pd1_pedestal_raw"  ;
    spec_pd1_pedestal_raw = new spectrum_1D( name,
                                             4095, 1, 4096,
                                             folder, "",
                                             name_of_this_element+"_pd1_pedestal_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_pd1_pedestal_raw) ;

    name = name_of_this_element + "_pd2_pedestal_raw"  ;
    spec_pd2_pedestal_raw = new spectrum_1D( name,
                                             4095, 1, 4096,
                                             folder, "",
                                             name_of_this_element+"_pd2_pedestal_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_pd2_pedestal_raw) ;


    // Matrices ==============================================

    name = name_of_this_element + "_pd0_vs_pd1_amplitude_raw"  ;
   spec_pd0_vs_pd1 = new spectrum_2D( name,
                                       1024, 1, 4096,
                                       1024, 1, 4096,
                                       folder, "",
                                       "X: " + name_of_this_element
                                       + "_pd1_amplitude_raw_when_fired,   "
                                         "Y:" + name_of_this_element
                                       + "_pd0_amplitude_raw_when_fired\n"
                                       );

    owner->spectra_ptr()->push_back(spec_pd0_vs_pd1) ;

    name = name_of_this_element + "_pd1_vs_pd2_amplitude_raw"  ;
    spec_pd1_vs_pd2 = new spectrum_2D( name,
                                       1024, 1, 4096,
                                       1024, 1, 4096,
                                       folder, "",
                                       "X: " + name_of_this_element
                                       + "_pd2_amplitude_raw_when_fired, "
                                         "   Y:" + name_of_this_element
                                       + "_pd1_amplitude_raw_when_fired"
                                       );

    owner->spectra_ptr()->push_back(spec_pd1_vs_pd2) ;

    // static!
    if(!spec_geometry){
        name = "kratta_geometry"  ;
        spec_geometry = new spectrum_2D( name,
                                           16*4,  0, 160,
                                           12,    -120, 0,
                                         folder, "",
                                         "X: No_such_incrementer_defined\n"
                                           "Y: No_such_incrementer_defined"
                                           );
        owner->spectra_ptr()->push_back(spec_geometry) ;
    }
}
//********************************************************************************************************
void Tkratta_crystal::analyse_current_event()
{
//      cout << "   analyse_current_event()          for "
//            << name_of_this_element
//            << endl ;

    flag_fired = false;
    flag_good = false;

    pd0_amplitude_raw = pd1_amplitude_raw = pd2_amplitude_raw =
            pd0_time_raw = pd1_time_raw = pd2_time_raw = 0;

    pd0_amplitude_cal = pd1_amplitude_cal = pd2_amplitude_cal =
            pd0_time_cal = pd1_time_cal = pd2_time_cal =  0.0;

    pd0_pedestal_raw = pd1_pedestal_raw = pd2_pedestal_raw = 0;
    pd0_pedestal_cal = pd1_pedestal_cal = pd2_pedestal_cal = 0.0;



    // take it out from input event
    // as it is marked in lookup table:y
    constexpr int time_position = 0;
    constexpr int amplitude_position = 1;
    constexpr int pedestal_position = 2;

    constexpr int pd0 = 0;
    constexpr int pd1 = 3;
    constexpr int pd2 = 6;



    if((pd0_amplitude_raw = data[pd0+amplitude_position]))    // Energy
    {

        spec_pd0_amplitude_raw->manually_increment(pd0_amplitude_raw);
        spec_fan_pattern_pds_raw->manually_increment(0);               
        pd0_amplitude_cal = (pd0_amplitude_calibr_factors[1]
                   *
                   (pd0_amplitude_raw
                    + randomizer::randomek()) )
                + pd0_amplitude_calibr_factors[0]  ;
        spec_pd0_amplitude_cal->manually_increment(pd0_amplitude_cal);
    }

    if((pd1_amplitude_raw = data[pd1+amplitude_position]))
    {

        spec_pd1_amplitude_raw->manually_increment(pd1_amplitude_raw);
        spec_fan_pattern_pds_raw->manually_increment(1);


        pd1_amplitude_cal = (pd1_amplitude_calibr_factors[1]
                   *
                   (pd1_amplitude_raw
                    + randomizer::randomek()) )
                + pd1_amplitude_calibr_factors[0]  ;
        spec_pd1_amplitude_cal->manually_increment(pd1_amplitude_cal);
    }

    if((pd2_amplitude_raw = data[pd2+amplitude_position]))
    {

        spec_pd2_amplitude_raw->manually_increment(pd2_amplitude_raw);
        spec_fan_pattern_pds_raw->manually_increment(2);

        pd2_amplitude_cal = (pd2_amplitude_calibr_factors[1]
                   *
                   (pd2_amplitude_raw
                    + randomizer::randomek()) )
                + pd2_amplitude_calibr_factors[0]  ;
        spec_pd2_amplitude_cal->manually_increment(pd2_amplitude_cal);
    }

    // pedestals
    pd0_pedestal_raw = data[pd0 + pedestal_position];
    pd1_pedestal_raw = data[pd1 + pedestal_position];
    pd2_pedestal_raw = data[pd2 + pedestal_position];

    if(pd0_pedestal_raw) spec_pd0_pedestal_raw->manually_increment(pd0_pedestal_raw);
    if(pd1_pedestal_raw) spec_pd1_pedestal_raw->manually_increment(pd1_pedestal_raw);
    if(pd2_pedestal_raw) spec_pd2_pedestal_raw->manually_increment(pd2_pedestal_raw);


    // times ---------------------------------------------------------

    if((pd0_time_raw = data[pd0 + time_position]))
    {
        //flag_fired = true;
        spec_pd0_time_raw->manually_increment(pd0_time_raw);
        spec_fan_pattern_pds_raw->manually_increment(6+0);
        pd0_time_cal = (pd0_time_calibr_factors[1]
                        *
                        (pd0_time_raw
                         + randomizer::randomek()) )
                + pd0_time_calibr_factors[0]  ;
        spec_pd0_time_cal->manually_increment(pd0_time_cal);
    }


    if((pd1_time_raw = data[pd1 + time_position]))
    {

        spec_pd1_time_raw->manually_increment(pd1_time_raw);
        spec_fan_pattern_pds_raw->manually_increment(6+1);
        //        if(name_of_this_element == "krattaZ7")
        //        {
        //            cout << name_of_this_element << " pd1_time_raw= " << pd1_time_raw << endl;
        //        }
        pd1_time_cal = (pd1_time_calibr_factors[1]
                        *
                        (pd1_time_raw
                         + randomizer::randomek()) )
                + pd1_time_calibr_factors[0]  ;
        spec_pd1_time_cal->manually_increment(pd1_time_cal);
    }

    if((pd2_time_raw = data[pd2 + time_position]))
    {

        spec_pd2_time_raw->manually_increment(pd2_time_raw);
        spec_fan_pattern_pds_raw->manually_increment(6+2);
        pd2_time_cal = (pd2_time_calibr_factors[1]
                        *
                        (pd2_time_raw
                         + randomizer::randomek()) )
                + pd2_time_calibr_factors[0]  ;

        spec_pd2_time_cal->manually_increment(pd2_time_cal);
    }
    // ---------------------

    //      spec_geometry->manually_increment(crystal_position_x, crystal_position_y);

    //flag_fired = pd0_amplitude_raw || pd1_amplitude_raw || pd2_amplitude_raw;
    flag_fired = pd0_time_raw || pd1_time_raw || pd2_time_raw;

    if(pd1_amplitude_raw)
    {

        if(pd0_amplitude_raw)
        {
            spec_pd0_vs_pd1->manually_increment(pd1_amplitude_raw, pd0_amplitude_raw);
        }
        if(pd2_amplitude_raw)
        {
            spec_pd1_vs_pd2->manually_increment(pd2_amplitude_raw, pd1_amplitude_raw);
            // cout << "JEST INKREMENTACJA" << endl;
        }
    }
    if(flag_fired)
    {        
//        spec_geometry->manually_increment(crystal_position_x, crystal_position_y);

        // checking flag good
        flag_good = true;
        if(good_pd0_time_cal_requires_threshold &&
                ((pd0_time_cal < pd0_time_cal_lower_threshold) ||
                 (pd0_time_cal > pd0_time_cal_upper_threshold))
                ) flag_good = false;

        if(good_pd1_time_cal_requires_threshold &&
                ((pd1_time_cal < pd1_time_cal_lower_threshold) ||
                 (pd1_time_cal > pd1_time_cal_upper_threshold))
                ) flag_good = false;

        if(good_pd2_time_cal_requires_threshold &&
                ((pd2_time_cal < pd2_time_cal_lower_threshold) ||
                 (pd2_time_cal > pd2_time_cal_upper_threshold))
                ) flag_good = false;
    }

    //if(flag_good)
    if(flag_fired)
    {
        if(pd0_amplitude_raw>2)
            spec_geometry->manually_increment(crystal_position_x, crystal_position_y);
        if(pd1_amplitude_raw>2)
            spec_geometry->manually_increment(crystal_position_x + 3, crystal_position_y);
        if(pd2_amplitude_raw>2)
            spec_geometry->manually_increment(crystal_position_x + 6, crystal_position_y);
    }

    if(RisingAnalysis_ptr->is_verbose_on() && flag_fired )
//    if(id == 0 && flag_fired )
    {
        cout
                << name_of_this_element
                << " --->  pd0_amplitude_raw = " << pd0_amplitude_raw
                << ",  pd1_amplitude_raw = " << pd1_amplitude_raw
                << ",  pd2_amplitude_raw = " << pd2_amplitude_raw

                << ",  pedestals = " << pd0_amplitude_raw
                << ",  " << pd1_amplitude_raw
                << ",  " << pd2_amplitude_raw << "]"


                << ",  pd0_time_raw = " << pd0_time_raw
                << ",  pd1_time_raw = " << pd1_time_raw
                << ",  pd2_time_raw = " << pd2_time_raw
                << endl;
    }
}
//**************************************************************
/** reading calibration, gates, setting pointers */
void Tkratta_crystal::make_preloop_action(ifstream & s)
{
//    cout << "Reading the calibration for the kratta_crystal "
//         << name_of_this_element  << endl ;

    read_cal_factors_into(pd0_amplitude_calibr_factors, name_of_this_element + "_pd0_amplitude_calibr_factors", s );
    read_cal_factors_into(pd1_amplitude_calibr_factors, name_of_this_element + "_pd1_amplitude_calibr_factors", s );
    read_cal_factors_into(pd2_amplitude_calibr_factors, name_of_this_element + "_pd2_amplitude_calibr_factors", s );

    read_cal_factors_into(pd0_time_calibr_factors, name_of_this_element + "_pd0_time_calibr_factors", s );
    read_cal_factors_into(pd1_time_calibr_factors, name_of_this_element + "_pd1_time_calibr_factors", s );
    read_cal_factors_into(pd2_time_calibr_factors, name_of_this_element + "_pd2_time_calibr_factors", s );
    //========================================

    // ---------------------
    // Geometry: phi, theta, distance
    //----------------------
    // here we read the Geometry
    string geometry_file = "calibration/kratta_geometry.txt" ;

    string slowo;
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
        plik_geometry   >> zjedz >> phi_geom
                        >> zjedz >> theta_geom
                        >> zjedz >> distance_from_target ;
        if(!plik_geometry) throw 1;

#if 0
        //
        distance_from_target *= 10 ; // it was in [cm] and we need in milimeters !
        phi_geom_radians   =  M_PI * phi_geom / 180.0 ;
        theta_geom_radians =  M_PI * theta_geom / 180.0;

        // ============== recalculate phi and theta into x,y,z of the crystal
        crystal_position_x = distance_from_target * sin(theta_geom_radians) * cos(phi_geom_radians+ (M_PI/2)) ;
        crystal_position_y = distance_from_target * sin(theta_geom_radians) * sin(phi_geom_radians+ (M_PI/2) ) ;
        double ttt = cos(theta_geom_radians);
        crystal_position_z = distance_from_target * ttt;
#endif


        // instead of this above we put into the file kratta_gemometry
        // kind of coordinates on the map
        // phi_geom has X coordinate
        // theta_geom has Y coordindate


        crystal_position_x = phi_geom * 2;   // (20 * (id % 6)) -60 + some_space;
        crystal_position_y = -theta_geom * 2;   // -20 * (id / 6);
        crystal_position_z = 0;

//        int some_space = 20;
//        crystal_position_x = (20 * (id % 8)) -60 + some_space;
//        crystal_position_y = -20 * (id / 8);
//        crystal_position_z = 0;


        ofstream plikG("my_binnings/kratta_geometry.mat.pinuptxt", ios::app);
        if(!plikG){
         cout << "Wrong opening of the pinup text file";
         exit(1);
        }
        plikG   << crystal_position_x - 10 << "   "
                << crystal_position_y - 2
                << " "
                //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
                << (name_of_this_element.substr(name_of_this_element.size() -2, 2)) // + "_ phi=" +to_string(phi_geom))
                << endl;

        plikG.close();

//        cout     << crystal_position_x << "   "
//                << crystal_position_y
//                << " " << name_of_this_element
//                << endl;

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
             << "./calibration/kratta_geometry.txt\n"
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

    read_gates();

}
//***************************************************************
/** Reading the gates for all pins pds (raw) */
void Tkratta_crystal::read_gates()
{
}
//************************************************************
/** where in the event starts data for this particular kratta_crystal */
void Tkratta_crystal::set_event_data_pointers(int16_t ptr[3+3+3])   // 3 signals, 3 pedestals, 3 times
{
    data = ptr;
    //  cout << " Tkratta_crystal::   Accepted adres trojki (szostki) " << hex
    // << ptr  << dec << endl;

}
//************************************************************
//****************************************************************************
bool  Tkratta_crystal::check_selfgate(Tself_gate_abstract_descr *desc)
{

    // here we assume that this is really germ
    Tself_gate_kratta_descr *d = (Tself_gate_kratta_descr*)desc ;
    if(d->enable_pd0_amplitude_cal_gate)
    {
        if(pd0_amplitude_cal < d->pd0_amplitude_cal_gate[0]
                ||
                pd0_amplitude_cal > d->pd0_amplitude_cal_gate[1])
            return false ;
    }

    if(d->enable_pd1_amplitude_cal_gate)
    {
        if(pd1_amplitude_cal < d->pd1_amplitude_cal_gate[0]
                ||
                pd1_amplitude_cal > d->pd1_amplitude_cal_gate[1])
            return false ;
    }

    if(d->enable_pd2_amplitude_cal_gate)
    {
        if(pd2_amplitude_cal < d->pd2_amplitude_cal_gate[0]
                ||
                pd2_amplitude_cal > d->pd2_amplitude_cal_gate[1])
            return false ;
    }

    // 2 polygons here

    if(d->enable_pd0_vs_pd1_polygon_gate)
    {
        if(d->pd0_vs_pd1_polygon->Test(pd1_amplitude_cal, pd0_amplitude_cal) == false)
        {
            //cout << " is outside " << endl;;
            return false;
        }
    }

    if(d->enable_pd1_vs_pd2_polygon_gate)
    {
        if(d->pd1_vs_pd2_polygon->Test(pd2_amplitude_cal, pd1_amplitude_cal) == false)
        {
            //cout << " is outside " << endl;;
            return false;
        }
    }
    // ------------------

    if(d->enable_pd0_time_cal_gate)
    {
        if(pd0_time_cal < d->pd0_time_cal_gate[0]
                ||
                pd0_time_cal > d->pd0_time_cal_gate[1])
            return false ;
    }

    if(d->enable_pd1_time_cal_gate)
    {
        if(pd1_time_cal < d->pd1_time_cal_gate[0]
                ||
                pd1_time_cal > d->pd1_time_cal_gate[1])
            return false ;
    }

    if(d->enable_pd2_time_cal_gate)
    {
        if(pd2_time_cal < d->pd2_time_cal_gate[0]
                ||
                pd2_time_cal > d->pd2_time_cal_gate[1])
            return false ;
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

#if 0
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


#endif

    return true ;// when EVETHING was successful
}
//****************************************************************************


//*************************************************************
#endif // KRATTA_PRESENT

