#include "Tclover.h"


#include <fstream>
using namespace std;

#include "Tnamed_pointer.h"

//***********************************************************************
//  constructor
Tclover::Tclover(string _name,
                 int (TIFJEvent::*en_data_ptr)[14],
                 int (TIFJEvent::*ti_data_ptr)[4]
                ) : Tfrs_element(_name),
    energy_data(en_data_ptr),
    time_data(ti_data_ptr)
{
    energy_data = 0 ;
    time_data = 0 ;

    create_my_spectra();



    named_pointer[name_of_this_element + "_cc1_energy_raw"]
    = Tnamed_pointer(&cc_energy_raw[0], 0, this) ;
    named_pointer[name_of_this_element + "_cc2_energy_raw"]
    = Tnamed_pointer(&cc_energy_raw[1], 0, this) ;
    named_pointer[name_of_this_element + "_cc3_energy_raw"]
    = Tnamed_pointer(&cc_energy_raw[2], 0, this) ;
    named_pointer[name_of_this_element + "_cc4_energy_raw"]
    = Tnamed_pointer(&cc_energy_raw[3], 0, this) ;


    named_pointer[name_of_this_element + "_cc1_energy_cal"]
    = Tnamed_pointer(&cc_energy_cal[0], 0, this) ;
    named_pointer[name_of_this_element + "_cc2_energy_cal"]
    = Tnamed_pointer(&cc_energy_cal[1], 0, this) ;
    named_pointer[name_of_this_element + "_cc3_energy_cal"]
    = Tnamed_pointer(&cc_energy_cal[2], 0, this) ;
    named_pointer[name_of_this_element + "_cc4_energy_cal"]
    = Tnamed_pointer(&cc_energy_cal[3], 0, this) ;


    named_pointer[name_of_this_element + "_cc1_time_raw"]
    = Tnamed_pointer(&cc_time_raw[0], 0, this) ;
    named_pointer[name_of_this_element + "_cc2_time_raw"]
    = Tnamed_pointer(&cc_time_raw[1], 0, this) ;
    named_pointer[name_of_this_element + "_cc3_time_raw"]
    = Tnamed_pointer(&cc_time_raw[2], 0, this) ;
    named_pointer[name_of_this_element + "_cc4_time_raw"]
    = Tnamed_pointer(&cc_time_raw[3], 0, this) ;


    named_pointer[name_of_this_element + "_cc1_time_cal"]
    = Tnamed_pointer(&cc_time_cal[0], 0, this) ;
    named_pointer[name_of_this_element + "_cc2_time_cal"]
    = Tnamed_pointer(&cc_time_cal[1], 0, this) ;
    named_pointer[name_of_this_element + "_cc3_time_cal"]
    = Tnamed_pointer(&cc_time_cal[2], 0, this) ;
    named_pointer[name_of_this_element + "_cc4_time_cal"]
    = Tnamed_pointer(&cc_time_cal[3], 0, this) ;



    named_pointer[name_of_this_element + "_multiplicity_cc"]
    = Tnamed_pointer(&multiplicity_cc, 0, this) ;
//-------------


    for(int i = 0 ; i < 4 ; i++)
    {

        string entry_name =  "ALL_clover_cc_energy_cal" ;
        Tmap_of_named_pointers::iterator pos = named_pointer.find(entry_name);

        if(pos != named_pointer.end())
            pos->second.add_item(&cc_energy_cal[i], 0, this);
        else
            named_pointer[entry_name] = Tnamed_pointer(&cc_energy_cal[i], 0, this);


        //--------------
        entry_name =  "ALL_clover_cc_time_cal" ;
        pos = named_pointer.find(entry_name);
        if(pos != named_pointer.end())
            pos->second.add_item(&cc_time_cal[i], 0, this);
        else
            named_pointer[entry_name] = Tnamed_pointer(&cc_time_cal[i], 0, this);
    }

}
//************************************************************************
void Tclover::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "clover/" + name_of_this_element ;
    string core ;
    string name ;

    for(int s = 0 ; s < 4 ; s++)
    {
        core = name_of_this_element + "_cc" ;
        core += char('0' + (s + 1)) ;


        //-----------
        name = core + "_energy_raw"  ;
        spec_e_cc_raw[s] = new spectrum_1D(name,
                                           name,
                                           4096, 0, 4096,
                                           folder);
        frs_spectra_ptr->push_back(spec_e_cc_raw[s]) ;



        //-----------
        name = core + "_time_raw"  ;

        spec_t_cc_raw[s] = new spectrum_1D(name,
                                           name,
                                           4096, 0, 4096,
                                           folder);
        frs_spectra_ptr->push_back(spec_t_cc_raw[s]) ;



        //-----------
        name = core + "_energy_cal"  ;
        spec_e_cc_cal[s] = new spectrum_1D(name,
                                           name,
                                           4096, 0, 4096,
                                           folder);
        frs_spectra_ptr->push_back(spec_e_cc_cal[s]) ;


        //-----------
        name = core + "_time_cal"  ;
        spec_t_cc_cal[s] = new spectrum_1D(name,
                                           name,
                                           4096, 0, 4096,
                                           folder);
        frs_spectra_ptr->push_back(spec_t_cc_cal[s]) ;


    }


    name = name_of_this_element + "_total_cc_energy_cal"  ;
    spec_e_cc_total = new spectrum_1D(name,
                                      name,
                                      4096, 0, 4096,
                                      folder);
    frs_spectra_ptr->push_back(spec_e_cc_total) ;


    name = name_of_this_element + "_total_cc_time_cal"  ;
    spec_t_cc_total = new spectrum_1D(name,
                                      name,
                                      4096, 0, 4096,
                                      folder);
    frs_spectra_ptr->push_back(spec_t_cc_total) ;




    //============== Position spectra ==============
    //-----------
    name = name_of_this_element + "_pos_1_raw"  ;
    spec_e_pos_1 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_1) ;


    name = name_of_this_element + "_pos_2_raw"  ;
    spec_e_pos_2 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_2) ;

    name = name_of_this_element + "_pos_3_raw"  ;
    spec_e_pos_3 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_3) ;


    name = name_of_this_element + "_pos_4_raw"  ;
    spec_e_pos_4 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_4) ;


    name = name_of_this_element + "_pos_5_raw"  ;
    spec_e_pos_5 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_5) ;


    name = name_of_this_element + "_pos_6_raw"  ;
    spec_e_pos_6 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_6) ;


    name = name_of_this_element + "_pos_7_raw"  ;
    spec_e_pos_7 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_7) ;


    name = name_of_this_element + "_pos_8_raw"  ;
    spec_e_pos_8 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_8) ;


    name = name_of_this_element + "_pos_9_raw"  ;
    spec_e_pos_9 = new spectrum_1D(name,
                                   name,
                                   4096, 0, 4096,
                                   folder);
    frs_spectra_ptr->push_back(spec_e_pos_9) ;


    name = name_of_this_element + "_ene_k7_raw"  ;
    spec_e_k7 = new spectrum_1D(name,
                                name,
                                4096, 0, 4096,
                                folder);
    frs_spectra_ptr->push_back(spec_e_k7) ;

}
//**********************************************************************
void Tclover::analyse_current_event()
{


    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;
    multiplicity_cc = 0 ;

    int *dana = &(event_unpacked->clover_energy[0]);
    for(int s = 0 ; s < 4 ; s++)
    {
        spec_e_cc_raw[s]->manually_increment(cc_energy_raw[s] = dana[s]) ;
    }


    spec_e_pos_1 ->manually_increment(dana[4]) ;
    spec_e_pos_2 ->manually_increment(dana[5]) ;
    spec_e_pos_3 ->manually_increment(dana[6]) ;
    spec_e_pos_4 ->manually_increment(dana[7]) ;
    spec_e_pos_5 ->manually_increment(dana[8]) ;
    spec_e_pos_6 ->manually_increment(dana[9]) ;
    spec_e_pos_7 ->manually_increment(dana[10]) ;
    spec_e_pos_8 ->manually_increment(dana[11]) ;
    spec_e_pos_9 ->manually_increment(dana[12]) ;

    spec_e_k7 ->manually_increment(dana[13]) ;


    dana = &(event_unpacked->clover_time[0]);

    for(int s = 0 ; s < 4 ; s++)
    {
        spec_t_cc_raw[s]->manually_increment(cc_time_raw[s]   = dana[s]) ;
    }


    // calibration -------------------------


    for(int s = 0 ; s < 4 ; s++)
    {
        cc_energy_cal[s] = 0 ;

        double energy_tmp = (cc_energy_raw[s]) + randomizer::randomek() ;
        for(unsigned int i = 0 ; i < cc_segment[s].energy_calibr_factors.size() ; i++)
        {
            cc_energy_cal[s]  += cc_segment[s].energy_calibr_factors[i] * pow(energy_tmp, (int) i) ;
        }
        spec_e_cc_cal[s]->manually_increment(cc_energy_cal[s]) ;
        spec_e_cc_total->manually_increment(cc_energy_cal[s]) ;

        //--------------
        cc_time_cal[s] = 0 ;

        double time_tmp = (cc_time_raw[s]) + randomizer::randomek() ;
        for(unsigned int i = 0 ; i < cc_segment[s].time_calibr_factors.size() ; i++)
        {
            cc_time_cal[s]  += cc_segment[s].time_calibr_factors[i] * pow(time_tmp, (int) i) ;
        }
        spec_t_cc_cal[s]->manually_increment(cc_time_cal[s]) ;
        spec_t_cc_total ->manually_increment(cc_time_cal[s]) ;

        if(cc_energy_cal[s] > 100 && cc_time_cal[s] != 0)multiplicity_cc++ ;

    }



    calculations_already_done = true ;

}
//**************************************************************************
void  Tclover::make_preloop_action(ifstream & /*s*/)  // read the calibration factors, gates
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;

    // Note: Clover belongs to the FRS branch, but anyway
    // the calibration factors are stored in the rising calibraion file
    // together with the clusters calibration factors

    string cal_filename = "./calibration/rising_calibration.txt" ;
    ifstream plik(cal_filename.c_str());
    if(!plik)
    {
        cout << "I can't open the file " << cal_filename
             << " to read the calibration for " << name_of_this_element
             << endl;
        exit(1);
    }

    // in this file we look for a string

    try
    {
        for(int s = 0 ; s < 4 ; s++)
        {
            cc_segment[s].energy_calibr_factors.clear() ;
            int how_many_factors = 3 ;

            string slowo = name_of_this_element + "_cc";
            slowo += char('0' + (s + 1));
            slowo += "_energy_factors" ;

            // first reading the energy calibration
            Tfile_helper::spot_in_file(plik, slowo);

            for(int i = 0 ; i < how_many_factors ; i++)
            {
                // otherwise we read the data
                double value ;
                plik >> value ;

                if(!plik)
                {
                    Treading_value_exception capsule ;
                    capsule.message =
                        "I found keyword '" + slowo
                        + "' but error was in reading its value." ;
                    throw capsule ;

                }
                // if ok

                cc_segment[s].energy_calibr_factors.push_back(value) ;

            }  // for

            // TIME calibration =======================

            // reading the time calibration factors --------------
            cc_segment[s].time_calibr_factors.clear() ;
            how_many_factors = 3 ;

            slowo = name_of_this_element + "_cc";
            slowo += char('0' + (s + 1));
            slowo += "_time_factors" ;

            // first reading the time calibration
            Tfile_helper::spot_in_file(plik, slowo);

            for(int i = 0 ; i < how_many_factors ; i++)
            {
                // otherwise we read the data
                double value ;
                plik >> value ;

                if(!plik)
                {
                    Treading_value_exception capsule ;
                    capsule.message =
                        "I found keyword '" + slowo
                        + "' but error was in reading its value." ;
                    throw capsule ;

                }
                // if ok

                cc_segment[s].time_calibr_factors.push_back(value) ;

            }  // for

        } // for segment
    } // endof try

    catch(Tfile_helper_exception &k)
    {

        cerr << "Error while reading geometry file "
             << cal_filename
             << ":\n"
             << k.message << endl  ;

        exit(-1) ; // is it healthy ?
    }



#ifdef NIGDY
    // ---------------------------------------------------
    // Geometry: phi, theta
    //----------------------------------------------------

    // but this is in a different file....

    // cout << " reading the geometry angles  for crystal "
    //        << name_of_this_element
    //        << endl ;


    //-----------------

    // here we read the Geometry
    //


    string geometry_file = "calibration/rising_geometry.txt" ;

    try
    {

        ifstream plik_geometry(geometry_file.c_str());
        if(! plik_geometry)
        {
            cout << "I can't open  file: " << geometry_file  ;
            exit(-1) ; // is it healthy ?
        }

        string keyword = name_of_this_element + "_phi_theta_distance" ;
        Tfile_helper::spot_in_file(plik_geometry, keyword);
        plik_geometry >> zjedz >> phi_geom
                      >> zjedz >> theta_geom
                      >> zjedz >> distance_from_target ;
        if(!plik_geometry)
        {
            cout << "Error while reading (inside geometry file "
                 << geometry_file
                 << ") phi and theta and distance of"
                 << name_of_this_element
                 << "\nI found keyword '"
                 << keyword
                 << "' but error was while reading its (three) values"
                 << " of phi and theta."
                 << endl  ;
            exit(-1);
        }
    }

    catch(Tfile_helper_exception &k)
    {

        cerr << "Error while reading geometry file "
             << geometry_file
             << ":\n"
             << k.message << endl  ;

        exit(-1) ; // is it healthy ?
    }


    phi_geom_radians   =  M_PI * phi_geom / 180.0 ;
    theta_geom_radians =  M_PI * theta_geom / 180.0;

    // ============== recalculate phi and theta into x,y,z of the crystal
    crystal_position_x = distance_from_target * sin(theta_geom) * cos(phi_geom) ;
    crystal_position_y = distance_from_target * sin(theta_geom) * sin(phi_geom) ;
    crystal_position_z = distance_from_target * cos(theta_geom);


    // doppler correction options are read by target and stored
    // in the static members of this clas (Tcrystal)

#endif



}
//***************************************************************
