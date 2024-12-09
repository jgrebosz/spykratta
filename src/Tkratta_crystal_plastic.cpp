/***************************************************************************
Tkratta_crystal_plastic.cpp  -  description
-------------------
begin                :
copyright            : (C)  by dr Jerzy Grebosz (IFJ Krakow, Poland)
  email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "experiment_def.h"
#ifdef KRATTA_PLASTIC_PRESENT

#include "Tkratta_crystal_plastic.h"
#include "Tkratta.h"
#include "Tfile_helper.h"
#include "randomizer.h"
#include <sstream>
#include <iomanip>

#include "Tnamed_pointer.h"
#include "Tself_gate_kratta_descr.h"
#include "TIFJAnalysis.h"   // for verbose mode

// STATIC members
bool  Tkratta_crystal_plastic::good_time_cal_requires_threshold;

double Tkratta_crystal_plastic::time_cal_lower_threshold;
double Tkratta_crystal_plastic::time_cal_upper_threshold;

#if PLASTIC_HAS_ENERGY_DATA
bool  Tkratta_crystal_plastic::good_energy_cal_requires_threshold;
double Tkratta_crystal_plastic::energy_cal_lower_threshold;
double Tkratta_crystal_plastic::energy_cal_upper_threshold;
#endif

//static  NOT ANYMORE (they are in kratta.h)
// spectrum_2D* Tkratta_crystal_plastic::spec_geometry;
// spectrum_2D* Tkratta_crystal_plastic::spec_geometry_ratios;
// spectrum_2D* Tkratta_crystal_plastic::spec_geometry_from_scalers;
//************************************************************************
Tkratta_crystal_plastic:: Tkratta_crystal_plastic(Tkratta *ptr_owner, string name, int id_nr)
    : owner(ptr_owner), name_of_this_element(name), my_id_nr(id_nr)
{
    //cout << "CREATING Plastic" << name_of_this_element << endl;
    my_selfgate_type =  Tselfgate_type::kratta;

    // segment_nr = id_nr;
    data = nullptr;
    create_my_spectra() ;

    // Incrementers
    named_pointer[name_of_this_element+"_time_raw_when_fired"]
            = Tnamed_pointer(&time_raw, &flag_fired, this) ;

    //---

    named_pointer[name_of_this_element+"_time_cal_when_fired"]
            = Tnamed_pointer(&time_cal, &flag_fired, this) ;

    // when GOOD flag
    named_pointer[name_of_this_element+"_time_cal_when_good"]
            = Tnamed_pointer(&time_cal, &flag_good, this) ;


    Tnamed_pointer::add_as_ALL("ALL_plastic_time_cal_when_fired", &time_cal, &flag_fired, this);

    // when good
    Tnamed_pointer::add_as_ALL("ALL_plastic_time_cal_when_good", &time_cal, &flag_good, this);

#if PLASTIC_HAS_ENERGY_DATA
    named_pointer[name_of_this_element+"_energy_raw_when_fired"]
            = Tnamed_pointer(&energy_raw, &flag_fired, this) ;
    named_pointer[name_of_this_element+"_energy_cal_when_fired"]
            = Tnamed_pointer(&energy_cal, &flag_fired, this) ;
    named_pointer[name_of_this_element+"_energy_cal_when_good"]
            = Tnamed_pointer(&energy_cal, &flag_good, this) ;
    Tnamed_pointer::add_as_ALL("ALL_plastic_energy_cal_when_fired", &energy_cal, &flag_fired, this);
    Tnamed_pointer::add_as_ALL("ALL_plastic_energy_cal_when_good", &energy_cal, &flag_good, this);

#endif

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
Tkratta_crystal_plastic::~Tkratta_crystal_plastic()
{
}
//************************************************************************
/** This function is creating the basic spectra of one segment (kratta_crystal) of the Krata detector */
void Tkratta_crystal_plastic::create_my_spectra()
{
    // to give the proper name to the spectra, at first
    // we shoud read the geo file.

    string folder = "kratta_folder/" + name_of_this_element ;
    string name ;


    // --------------------TIME-------------
    name = name_of_this_element + "_time_raw"  ;
    spec_time_raw = new spectrum_1D( name,
                                     4999, 1, 5000,
                                     folder, "",
                                     name_of_this_element+"_time_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_time_raw) ;


#if PLASTIC_HAS_ENERGY_DATA

    // -------------------ENERGY--------------
    name = name_of_this_element + "_energy_raw"  ;
    spec_energy_raw = new spectrum_1D( name,
                                       4095, 1, 4096,
                                       folder, "",
                                       name_of_this_element+"_energy_raw_when_fired");
    owner->spectra_ptr()->push_back(spec_energy_raw) ;
    // ---------------------------------
    // CALIBRATED Energy----------------
    // ---------------------------------

    name = name_of_this_element + "_energy_cal"  ;
    spec_energy_cal = new spectrum_1D( name,
                                       4095, 1, 4096,
                                       folder, "",
                                       name_of_this_element+"_energy_cal_when_fired");
    owner->spectra_ptr()->push_back(spec_energy_cal) ;

    // = FAN spectrum ===============================

    // PATTERN -------------------combination of raw pds
    name = name_of_this_element + "_fan"  ;
    spec_fan = new spectrum_1D( name,
                                12, 0, 12,
                                folder,
                                "presence of raw signals: energy, time",
                                "If you really need this, ask Jurek");
    owner->spectra_ptr()->push_back(spec_fan);
#endif

    // Calibrated time--------------------------
    name = name_of_this_element + "_time_cal"  ;
    spec_time_cal = new spectrum_1D( name,
                                     4095, 1, 4096,
                                     folder, "",
                                     name_of_this_element+"_time_cal_when_fired");
    owner->spectra_ptr()->push_back(spec_time_cal) ;







    // Matrices ==============================================

    //    name = name_of_this_element + "_energy_vs_energy1_raw"  ;
    //    spec_energy_vs_energy1 = new spectrum_2D( name,
    //                                       1024, 1, 4096,
    //                                       1024, 1, 2048,
    //                                       folder );
    //    owner->spectra_ptr()->push_back(spec_energy_vs_energy1) ;




    // This will be a matrix where we want to see boxes for every plastic crystal
    // 7 columns
    // 5 rows
    // empty places are in a middle of row 2,3,4
    int columns = 7;
    int rows = 5;
    constexpr int crystal_size = 4;    // size of one particular 1/4 box
    int box_size = 2 * crystal_size;
    int col_gap = 2;
    int row_gap = 2;
    // so sizes of matrix are
    [[maybe_unused]] int width = col_gap + (columns * (box_size + col_gap));
    [[maybe_unused]] int height = row_gap + (rows * (box_size + row_gap));

//            auto *my_kratta = owner->give_crystal(my_id_nr / 4);

            // obliczenie pozycji danej ćwiartki kryształu
            // w stosunku do środka jego detektora
            [[maybe_unused]] int quarter_x = my_id_nr % 4;
//            int dx = 0; int dy = 0;
//            constexpr int unit = crystal_size; // 2;  // rozmiar ćwiartki
//            switch(quarter_x){
//                case 0: dx = -unit; dy = unit; break;
//                case 1: dx = unit; dy = unit; break;
//                case 2: dx = -unit; dy = -unit; break;
//                case 3: dx = unit; dy = -unit; break;
//            }

//            crystal_position_x = my_kratta->give_x()+dx;
//            crystal_position_y = my_kratta->give_y()+dy;
//            crystal_position_z = my_kratta->give_z();


//    if(!spec_geometry_ratios){
//        name = "plastic_geometry_ratios"  ;
//        spec_geometry_ratios = new spectrum_2D( name,
//                                                width/crystal_size,
//                                                0, width,
//                                                height/crystal_size,
//                                                0, height,
//                                                folder, "",
//                                                "No_such_incrementer_defined");
//        owner->spectra_ptr()->push_back(spec_geometry_ratios) ;
//    }


}
//********************************************************************************************************
void Tkratta_crystal_plastic::analyse_current_event()
{
         // cout << "   analyse_current_event()          for "
         //       << name_of_this_element
         //       << endl ;

    flag_fired = false;
    flag_good = false;

#if PLASTIC_HAS_ENERGY_DATA
    energy_raw = time_raw = 0;
    energy_cal = time_cal =  0.0;
#endif

    // take it out from input event


    // times ---------------------------------------------------------

    if((time_raw = data[0]) )   //
    {
        flag_fired = true;
        spec_time_raw->manually_increment(time_raw);
#if PLASTIC_HAS_ENERGY_DATA
        spec_fan->manually_increment(0);
#endif
        if(time_calibr_factors.size() > 1 )
        {
            time_cal = (time_calibr_factors[1]
                    *
                    (time_raw
                     + randomizer::randomek()) )
                    + time_calibr_factors[0]  ;
            spec_time_cal->manually_increment(time_cal);
        }
    }
#if PLASTIC_HAS_ENERGY_DATA
    if((energy_raw = data[1]))    // Energy
    {
        spec_energy_raw->manually_increment(energy_raw);
        spec_fan->manually_increment(1);
        if( energy_calibr_factors.size() > 1 ){
            energy_cal = (energy_calibr_factors[1]
                    *
                    (energy_raw
                     + randomizer::randomek()) )
                    + energy_calibr_factors[0]  ;
            spec_energy_cal->manually_increment(energy_cal);
        }
    }
#endif

    // ---------------------

    flag_fired = time_raw
        #if PLASTIC_HAS_ENERGY_DATA
            || energy_raw
        #endif
            ;
    if(flag_fired)
    {
        // checking flag good
        flag_good = true;

        if(good_time_cal_requires_threshold &&
                ((time_cal < time_cal_lower_threshold) ||
                 (time_cal > time_cal_upper_threshold))
                ) flag_good = false;

#if PLASTIC_HAS_ENERGY_DATA
        if(good_energy_cal_requires_threshold &&
                ((energy_cal < energy_cal_lower_threshold) ||
                 (energy_cal > energy_cal_upper_threshold))
                ) flag_good = false;
#endif
    }



    if(flag_good){

//        cout
//                //<< "wskaznik = " << (long) owner->spec_plastic_geometry
//             << " crystal_position_x = " << crystal_position_x
//             << " crystal_position_y = " << crystal_position_y
//             << endl;

        owner->spec_plastic_geometry->manually_increment(
                    crystal_position_x,
                    crystal_position_y);

        // Test --------------------
//        owner->spec_plastic_geometry->manually_increment(
//                    10,
//                    10);
//        owner->spec_plastic_geometry->manually_increment(
//                    10,
//                    12);
//        owner->spec_plastic_geometry->manually_increment(
//                    12,
//                    10);
//        owner->spec_plastic_geometry->manually_increment(
//                    12,
//                    12);
//        owner->spec_plastic_geometry->manually_increment(
//                    12,
//                    12);
    }

    if(
        RisingAnalysis_ptr->is_verbose_on() &&
            flag_fired )
    {
        cout
                << name_of_this_element
           #if PLASTIC_HAS_ENERGY_DATA
                << " --->  energy_raw = " << energy_raw
           #endif
                << ",  time_raw = " << time_raw
                << endl;
    }
}
//**************************************************************
/** reading calibration, gates, setting pointers */
void Tkratta_crystal_plastic::make_preloop_action(ifstream & s)
{
//    cout << "Reading the calibration for the kratta_crystal "
//         << name_of_this_element
//         // << " SKIPPED now"
//         << endl ;

    spec_geometry_ratios = owner->spec_geometry_ratios;
    spec_geometry_from_scalers = owner->spec_geometry_from_scalers;

#if PLASTIC_HAS_ENERGY_DATA
    read_cal_factors_into(energy_calibr_factors, name_of_this_element + "_energy_calibr_factors", s );
#endif

    read_cal_factors_into(time_calibr_factors, name_of_this_element + "_time_calibr_factors", s );
    //========================================

    auto *my_kratta = owner->give_crystal(my_id_nr / 4);

    quarter_x = my_id_nr % 4;
    int dx = 0; int dy = 0;
    constexpr int unit = 2; // 2;
    switch(quarter_x){
        case 0: dx = 0; dy = unit; break;
        case 1: dx = unit; dy = unit; break;
        case 2: dx = 0; dy = 0; break;
        case 3: dx = unit; dy = 0; break;
    }

    crystal_position_x = my_kratta->   give_x()+dx;
    crystal_position_y = my_kratta->give_y()+dy;
    crystal_position_z = my_kratta->give_z();

    ofstream plikG;
    // ("my_binnings/plastic_geometry_mat.pinuptxt", ios::app);
//    if(!plikG){
//        cout << "Wrong opening of the pinup text file";
//        exit(1);
//    }
//    string short_name = name_of_this_element.substr(name_of_this_element.size() -3);
//    //string short_name = name_of_this_element.substr(name_of_this_element.size() -1);

//    plikG    << crystal_position_x -10 << "   "
//             << crystal_position_y
//             << " " << char(quarter_x + 'a')
//                //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
//             << endl;

//    if(quarter_x == 0)
//        plikG    << crystal_position_x - 10 << "   "
//                 << crystal_position_y-2
//                 << " "
//                 << short_name.substr(0,2)
//                 << endl;


//    plikG.close();

#if 0
    // Ratios pinup txt ---------------------------------------
    plikG.open("my_binnings/plastic_geometry_map_ratios.mat.pinuptxt", ios::app);
    if(!plikG){
        cout << "Wrong opening of the pinup text file";
        exit(1);
    }
    string short_name = name_of_this_element.substr(name_of_this_element.size() -3);
    //string short_name = name_of_this_element.substr(name_of_this_element.size() -1);

    // plikG << "// wydruk 1   ---" << name_of_this_element << " ---\n" ;

    if(quarter_x == 0)    // id nr  of the detector
        plikG    << crystal_position_x << "   "
                 << crystal_position_y-2
                 << " "
                 << short_name.substr(0,2)
                 << endl;

    // section of the detector
    plikG    << crystal_position_x << "   "
             << crystal_position_y
             << " " << char(quarter_x + 'a')
                //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
             << endl;



    plikG.close();
#endif

    //------------------------ do opisu INNEJ macierzy ("from scalers") -------
    plikG.open("my_binnings/plastic_geometry_map_from_scalers.mat.pinuptxt", ios::app);
    if(!plikG){
        cout << "Wrong opening of the pinup text file";
        exit(1);
    }
    string short_name = name_of_this_element.substr(name_of_this_element.size() -3);
    //string short_name = name_of_this_element.substr(name_of_this_element.size() -1);

    plikG    << crystal_position_x << "   "
             << crystal_position_y
             << " " << char(quarter_x + 'a')
                //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
             << endl;

    if(quarter_x == 0)
        plikG    << crystal_position_x << "   "
                 << crystal_position_y-2
                 << " "
                 << short_name.substr(0,2)
                 << endl;


    // plikG << "// wydruk 2 ------\n" ;
    plikG.close();


    read_gates();

}
//***************************************************************
/** Reading the gates for all pins pds (raw) */
void Tkratta_crystal_plastic::read_gates()
{

    if(my_id_nr >0) return;

    // cout << " reading the GOOD gates for " << name_of_this_element << "  " << endl ;

    // reading from the file
    string pname {"./options/good_plastic.txt"} ;
    ifstream plik(pname);
    if(!plik)
    {
        cout << "Can't open file " << pname << " containing GOOD values of plastic detector " << endl;
        exit(3);
        // return ;
    }

    try{
        //----------
        good_time_cal_requires_threshold =
                (bool) Nfile_helper::find_in_file(plik, "good_time_cal_requires_threshold") ;
        time_cal_lower_threshold =
                Nfile_helper::find_in_file(plik, "time_cal_lower_threshold") ;
        time_cal_upper_threshold =
                Nfile_helper::find_in_file(plik, "time_cal_upper_threshold") ;

#if PLASTIC_HAS_ENERGY_DATA
        good_energy_cal_requires_threshold =
                (bool) Nfile_helper::find_in_file(plik, "good_energy_cal_requires_threshold") ;
        energy_cal_lower_threshold =
                Nfile_helper::find_in_file(plik, "energy_cal_lower_threshold") ;
        energy_cal_upper_threshold =
                Nfile_helper::find_in_file(plik, "energy_cal_upper_threshold") ;
#endif



        //-------
    }
    catch(Tfile_helper_exception k)
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
        exit(4);
    }

}
//************************************************************
/** where in the event starts data for this particular kratta_crystal */
void Tkratta_crystal_plastic::set_event_data_pointers(int16_t ptr[3+3+3])   // 3 signals, 3 pedestals, 3 times
{
    data = ptr;
    //  cout << " Tkratta_crystal_plastic::   Accepted adres trojki (szostki) " << hex
    // << ptr  << dec << endl;

}
//************************************************************
//****************************************************************************
#if 0
bool  Tkratta_crystal_plastic::check_selfgate(Tself_gate_abstract_descr *desc)
{

    // here we assume that this is really germ
    Tself_gate_kratta_descr *d = (Tself_gate_kratta_descr*)desc ;
    if(d->enable_energy_cal_gate)
    {
        if(energy_cal < d->energy_cal_gate[0]
                ||
                energy_cal > d->energy_cal_gate[1])
            return false ;
    }

    if(d->enable_energy1_cal_gate)
    {
        if(energy1_cal < d->energy1_cal_gate[0]
                ||
                energy1_cal > d->energy1_cal_gate[1])
            return false ;
    }



    if(d->enable_time_cal_gate)
    {
        if(time_cal < d->time_cal_gate[0]
                ||
                time_cal > d->time_cal_gate[1])
            return false ;
    }

    if(d->enable_time1_cal_gate)
    {
        if(time1_cal < d->time1_cal_gate[0]
                ||
                time1_cal > d->time1_cal_gate[1])
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
#endif
//****************************************************************************


//*************************************************************
#endif // KRATTA_PLASTIC_PRESENT

