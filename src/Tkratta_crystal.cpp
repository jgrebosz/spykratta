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
#include "TIFJAnalysis.h"   // for verbose mode



//************************************************************************
Tkratta_crystal:: Tkratta_crystal(Tkratta *ptr_owner, string name, int id_nr)
    : owner(ptr_owner),
      name_of_this_element(name)
{
    segment_nr = id_nr;
    data = nullptr;

    // reading some data for geo file


    pos_x = KRATTA_NR_OF_CRYSTALS % KRATTA_NCOL;
    pos_y = KRATTA_NR_OF_CRYSTALS / KRATTA_NROW;

    ostringstream s  ;

//     s << name
//       << setfill('0')
//       << setw(2) << id_nr;
//     name_of_this_element = s.str();


    // cout << "CREATING" << name_of_this_element << endl;

    create_my_spectra() ;


    named_pointer[name_of_this_element+"_pd0_raw_when_fired"]
        = Tnamed_pointer(&pd0_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd1_raw_when_fired"]
        = Tnamed_pointer(&pd1_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd2_raw_when_fired"]
        = Tnamed_pointer(&pd2_raw, &flag_fired, this) ;


    named_pointer[name_of_this_element+"_pd0_time_raw_when_fired"]
        = Tnamed_pointer(&pd0_time_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd1_time_raw_when_fired"]
        = Tnamed_pointer(&pd1_time_raw, &flag_fired, this) ;

    named_pointer[name_of_this_element+"_pd2_time_raw_when_fired"]
        = Tnamed_pointer(&pd2_time_raw, &flag_fired, this) ;



}
//*******************************************  *****************************
//************************************************************************
Tkratta_crystal::~Tkratta_crystal()
{
}
//************************************************************************
/** This function is creating the basic spectra dedicatet to the one
    segment (kratta_crystal) of the Cate detector */
void Tkratta_crystal::create_my_spectra()
{

    // to give the proper name to the spectra, at first
    // we shoud read the geo file.


    string folder = "kratta_folder/" + name_of_this_element ;
    string name ;

    // ---------------------------------
    name = name_of_this_element + "_pd0_raw"  ;
    spec_pd0_raw = new spectrum_1D( name, name,
                                    4095, 1, 4096,
                                    folder );
    owner->spectra_ptr()->push_back(spec_pd0_raw) ;
// ---------------------------------
    name = name_of_this_element + "_pd1_raw"  ;
    spec_pd1_raw = new spectrum_1D( name, name,
                                    4095, 1, 4096,
                                    folder );
    owner->spectra_ptr()->push_back(spec_pd1_raw) ;
// ---------------------------------
    name = name_of_this_element + "_pd2_raw"  ;
    spec_pd2_raw = new spectrum_1D( name, name,
                                    4095, 1, 4096,
                                    folder );
    owner->spectra_ptr()->push_back(spec_pd2_raw) ;


    // ---------------------------------
    name = name_of_this_element + "_pd0_time_raw"  ;
    spec_pd0_time_raw = new spectrum_1D( name, name,
                                         4095, 1, 4096,
                                         folder );
    owner->spectra_ptr()->push_back(spec_pd0_time_raw) ;
// ---------------------------------
    name = name_of_this_element + "_pd1_time_raw"  ;
    spec_pd1_time_raw = new spectrum_1D( name, name,
                                         4095, 1, 4096,
                                         folder );
    owner->spectra_ptr()->push_back(spec_pd1_time_raw) ;
// ---------------------------------
    name = name_of_this_element + "_pd2_time_raw"  ;
    spec_pd2_time_raw = new spectrum_1D( name, name,
                                         4095, 1, 4096,
                                         folder );
    owner->spectra_ptr()->push_back(spec_pd2_time_raw) ;


    // ----------------------------------
    // CALIBRATED -------------------------------
    // ------- --------------------------
#if 0
    // ---------------------------------
    name = name_of_this_element + "_pd0_cal"  ;
    spec_pd0_cal = new spectrum_1D( name, name,
                                    4095, 1, 4096,
                                    folder );
    owner->spectra_ptr()->push_back(spec_pd0_cal) ;
// ---------------------------------
    name = name_of_this_element + "_pd1_cal"  ;
    spec_pd1_cal = new spectrum_1D( name, name,
                                    4095, 1, 4096,
                                    folder );
    owner->spectra_ptr()->push_back(spec_pd1_cal) ;
// ---------------------------------
    name = name_of_this_element + "_pd2_cal"  ;
    spec_pd2_cal = new spectrum_1D( name, name,
                                    4095, 1, 4096,
                                    folder );
    owner->spectra_ptr()->push_back(spec_pd2_cal) ;

#endif

    // ============ FAN spectra ===============================

    // mutiplicity of pins ------------------------------------

    // PATTERN -------------------combintaion of raw pds
    name = name_of_this_element + "_fan_pattern_pds_raw"  ;
    spec_fan_pattern_pds_raw = new spectrum_1D( name, name,
            5, 0, 5,
            folder );
    owner->spectra_ptr()->push_back(spec_fan_pattern_pds_raw);


    // Pedestals ==============================================

    name = name_of_this_element + "_pd0_pedestal_raw"  ;
    spec_pd0_pedestal_raw = new spectrum_1D( name, name,
            4095, 1, 4096,
            folder );
    owner->spectra_ptr()->push_back(spec_pd0_pedestal_raw) ;




    name = name_of_this_element + "_pd1_pedestal_raw"  ;
    spec_pd1_pedestal_raw = new spectrum_1D( name, name,
            4095, 1, 4096,
            folder );
    owner->spectra_ptr()->push_back(spec_pd1_pedestal_raw) ;

    name = name_of_this_element + "_pd2_pedestal_raw"  ;
    spec_pd2_pedestal_raw = new spectrum_1D( name, name,
            4095, 1, 4096,
            folder );
    owner->spectra_ptr()->push_back(spec_pd2_pedestal_raw) ;


    // Matrices ==============================================

    name = name_of_this_element + "_pd0_vs_pd1_raw"  ;
    spec_pd0_vs_pd1 = new spectrum_2D( name, name,
                                       1024, 1, 4096,
                                       1024, 1, 2048,
                                       folder );
    owner->spectra_ptr()->push_back(spec_pd0_vs_pd1) ;
    // ---
    /*
        name = name_of_this_element + "_pd1_vs_pd2_raw"  ;
        spec_pd1_vs_pd2 = new spectrum_2D( name, name,
                                           1024, 1, 2048,
                                           1024, 1, 2048,
                                           folder );
        owner->spectra_ptr()->push_back(spec_pd1_vs_pd2) ;
    */


}
//**************************************************************
//**************************************************************
/** the most important fuction,  executed for every event */
void Tkratta_crystal::analyse_current_event()
{

    //  cout << "   analyse_current_event()          for "
    //        << name_of_this_element
    //        << endl ;



    flag_fired = false ;

    pd0_raw = pd1_raw = pd2_raw =
                            pd0_time_raw = pd1_time_raw = pd2_time_raw = 0;

    pd0_cal = pd1_cal = pd2_cal = 0.0;



    // take it out from input event
    if((pd0_raw = data[0]))
    {
        flag_fired = true;
        spec_pd0_raw->manually_increment(pd0_raw);
        spec_fan_pattern_pds_raw->manually_increment(0);
        spec_pd0_pedestal_raw->manually_increment(data[3+0]);
    }


    if((pd1_raw = data[1]))
    {
        flag_fired = true;
        spec_pd1_raw->manually_increment(pd1_raw);
        spec_fan_pattern_pds_raw->manually_increment(1);
        spec_pd1_pedestal_raw->manually_increment(data[3+1]);
    }



    if((pd2_raw = data[2]))
    {
        flag_fired = true;
        spec_pd2_raw->manually_increment(pd2_raw);
        spec_fan_pattern_pds_raw->manually_increment(2);
        spec_pd2_pedestal_raw->manually_increment(data[3+2]);
    }
// times ---------------------------------------------------------

    if((pd0_time_raw = data[6+0]))
    {
        flag_fired = true;
        spec_pd0_time_raw->manually_increment(pd0_time_raw);
        spec_fan_pattern_pds_raw->manually_increment(6+0);

    }


    if((pd1_time_raw = data[6+1]))
    {
        flag_fired = true;
        spec_pd1_time_raw->manually_increment(pd1_time_raw);
        spec_fan_pattern_pds_raw->manually_increment(6+1);

        if(name_of_this_element == "krattaZ7")
        {
            cout << name_of_this_element << " pd1_time_raw= " << pd1_time_raw << endl;
        }
    }

    if((pd2_time_raw = data[6+2]))
    {
        flag_fired = true;
        spec_pd2_time_raw->manually_increment(pd2_time_raw);
        spec_fan_pattern_pds_raw->manually_increment(6+2);
    }


    if(flag_fired)
    {
        spec_pd0_vs_pd1->manually_increment(pd1_raw, pd0_raw);
        //spec_pd1_vs_pd2->manually_increment(pd2_raw, pd1_raw);
    }



    //-------------------------------
//     pd0_cal = (pd0_calibr_factors[1]
//                    *
//                    (pd0_raw
//                     + randomizer::randomek()) )
//                   + pd0_calibr_factors[0]  ;
//



//     if(IFJAnalysis_ptr->is_verbose_on() && ( flag_fired )
    {
//         cout
//                 << name_of_this_element
//                 << " --->  CsI_energy_raw = " << CsI_energy_raw
//                 << ",  CsI_time_raw = " << CsI_time_raw
//                 << ",  Si_time_raw = " << Si_time_raw
//                 << ", Si_energy_raw = " << Si_energy_raw
//                 << endl;
    }




}
//**************************************************************
/** reading calibration, gates, setting pointers */
void Tkratta_crystal::make_preloop_action(ifstream & s)
{
    //
    //  cout << "preloop action for kratta_crystal "
    //       << name_of_this_element
    //       << endl ;



    // cout << "Reading the calibration for the kratta_crystal "
    //        << name_of_this_element
    //        << endl ;
#if 0

    if(!event_kratta_crystal_data)
    {
        cout << "Alarm, in " << name_of_this_element
             << " the pointer to event was not set" << endl;
        exit (1) ;
    }



    // in this file we look for a string
    int how_many_factors = 2 ;   // max linear calibration

    string slowo = name_of_this_element
                   + "_CsI_energy_cal_factors" ;

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo );

    CsI_energy_calibr_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s) {
            string mess = "I found keyword:" + slowo
                          + ", but error was in reading its value." ;
            throw mess ;
        }
        // if ok
        CsI_energy_calibr_factors.push_back(value ) ;

    }

    if(CsI_energy_calibr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for energy calibration CsI of " + name_of_this_element ;
        throw mess ;
    }


    // reading the time calibration factors -----------

    slowo = name_of_this_element + "_CsI_time_cal_factors" ;

    Tfile_helper::spot_in_file(s, slowo );

    CsI_time_calibr_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s) {
            string mess ;
            mess += "I found keyword: " + slowo
                    + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok

        CsI_time_calibr_factors.push_back(value ) ;

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
    Tfile_helper::spot_in_file(s, slowo );

    Si_energy_calibr_factors.clear();
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s) {
            string mess = "I found keyword: " + slowo
                          + ", but error was in reading its value." ;
            throw mess ;
        }
        // if ok
        Si_energy_calibr_factors.push_back(value ) ;

    }

    if(Si_energy_calibr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for energy calibration Si of " + name_of_this_element;
        throw mess ;
    }

    // reading the time calibration factors --------------

    slowo = name_of_this_element + "_Si_time_cal_factors" ;

    Tfile_helper::spot_in_file(s, slowo );

    Si_time_calibr_factors.clear();
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s) {
            string mess =
                "I found keyword: " + slowo
                + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok

        Si_time_calibr_factors.push_back(value ) ;

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
             << char('a' + symb )
             << "_cal_factors" ;
        // << ends ;   //<------------------ !!!!!! error for strings

        slowo = name_of_pin = plik.str();

        Tfile_helper::spot_in_file(s, slowo );


        for(int i = 0 ; i < how_many_factors ; i++)
        {
            // otherwise we read the data
            double value ;
            s >> value ;

            if(!s) {
                string mess =
                    "I found keyword: " + slowo
                    + " but error was in reading its value." ;
                throw mess ;

            }
            // if ok

            pin_calibr_factors[symb].push_back(value ) ;
            break ;

        }
    }

    // reading the position correction calibration factors -----------
    // X
    slowo = name_of_this_element + "_x_position_corr_factors" ;

    Tfile_helper::spot_in_file(s, slowo );

    x_position_corr_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s) {
            string mess ;
            mess += "I found keyword: " + slowo
                    + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok

        x_position_corr_factors.push_back(value ) ;

    }

    if(x_position_corr_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for x_position_correction  of " + name_of_this_element ;
        throw mess ;
    }


// Y ------------------------------------------
    slowo = name_of_this_element + "_y_position_corr_factors" ;

    Tfile_helper::spot_in_file(s, slowo );

    y_position_corr_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        s >> value ;

        if(!s) {
            string mess ;
            mess += "I found keyword: " + slowo
                    + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok

        y_position_corr_factors.push_back(value ) ;

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

        par_force_segment_always_fired =  Tfile_helper::find_in_file(s, "par_force_segment_always_fired" );
        par_force_segment_always_fired_good =  Tfile_helper::find_in_file(s, "par_force_segment_always_fired_good" );

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

    // cout << " reading the geometry angles  for kratta_crystal "
    //        << name_of_this_element
    //        << endl ;


    //-----------------

    // here we read the Geometry
    // the geometry is actually the information
    // 1. how far away from the target is the Cate detector (Silicon part)
    //                                 (this is given by the constructor)

    // 2. how big x,y is one kratta_crystal (50 mm)
    // 3. how big is the gap between them (now 4 mm)
    // all this we need to know the position of the centre of the telscope


    string geometry_file = "calibration/kratta_geometry.txt" ;

    try {

        ifstream plik_geometry(geometry_file.c_str() );
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

#endif
    read_gates();

}
//***************************************************************
/** Reading the gates for all pins pds (raw) */
void Tkratta_crystal::read_gates()
{

    //  gates are created by my viewer, or manually
    // so that in one file there is only one gate, for this particullar object
#if 0

    for(int symb = 0 ; symb < 4 ; symb++)
    {
        string core_of_the_name = "gates/" + name_of_this_element ;
        string nam_gate = core_of_the_name + "_Si_pin_" ;
        nam_gate += (char)('a' +symb);
        nam_gate += "_raw_gate_GATE.gate" ;

        ifstream plik_gate(nam_gate.c_str() );

        if(! plik_gate)
        {
            string mess = "I can't open  file: " + nam_gate
                          + "\nThis gate you can produce interactively using a 'cracow' viewer program" ;

            cout << mess
                 << "\nI create a gate and assmue that the gate is wide open ! \a\n"
                 << endl ;

            ofstream pliknew(nam_gate.c_str() );
            pliknew   << low_g[symb] << "   "    // lower limit
                      << high_g[symb] << endl ;
        } else {


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
#endif
}
//************************************************************
/** where in the event starts data for this particular kratta_crystal */
void Tkratta_crystal::set_event_data_pointers(int16_t ptr[3+3])   // 3 signals, 3 pedestals
{
    data = ptr;
//  cout << " Tkratta_crystal::   Accepted adres trojki (szostki) " << hex
    // << ptr  << dec << endl;

}
//************************************************************


//*************************************************************
#endif // KRATTA_PRESENT

