/***************************************************************************
                          tkratta.cpp  -  description
                             -------------------
    begin                : Fri Oct 12 2014
    copyright            : (C) 2014 by dr Jerzy Grebosz
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/
#include "spectrum.h"

#include "Tkratta.h"
#include "Tkratta_crystal.h"
#include "Tkratta_crystal_plastic.h"
#ifdef KRATTA_PRESENT

#include "Tfrs.h"
#include "Tcluster.h"
#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h"

#include "Tsi_detector.h"
//#include "Tpaths.h"
#include <iomanip>

#include <fstream>
#include <set>
//*************************************************************************
Tkratta::Tkratta(string name): Tfrs_element(name)
{
    // cout << "constructor of " << name_of_this_element << endl;
    // To know how many kratta modules we have - we must read the lookup table
    //    string geo_name = "calibration/geo_map.geo";
    //    ifstream plik(geo_name);
    //    if(!plik)
    //    {
    //        cout << "!!! Fatal error: I can't open file " << geo_name
    //             << "\nMost probably you do not have such a file in the ./calibration subdirectory of "
    //             << endl;
    //        system ("pwd");
    //        system ("ls *");
    //        system ("ls ./calibration/*");
    //        exit(1);
    //    }
    // -----------------
    try{

        for(int m = 0, p_nr = 0; m < KRATTA_NR_OF_CRYSTALS ; ++m)
        {
            int p = 0 ; // just one for creating
            ostringstream n;
            n.fill('0');
            n << setw(2) << m ;
            string kratta_module_string = n.str();
            string keyword = "kratta_" + kratta_module_string + "_pd" + to_string(p);
            // FH::spot_in_file(plik, keyword);
            //string label;
            // int fadc, chan;
            // plik >> zjedz >> fadc >> zjedz >> chan  ;
            crystal.push_back(  new Tkratta_crystal(this,
                                                    "kratta_"+kratta_module_string,
                                                    m)) ;


            for(int k = 0 ; k < 4 ; ++k)
            {
                //                int p = 0;
                //                ostringstream n;
                //                n.fill('0');
                //                n << setw(2) << k ;
                //                string m_string = n.str();
                string keyword = "plastic_module_" + kratta_module_string +
                        char('a' + k) ;

                plastic.push_back(  new Tkratta_crystal_plastic(this,
                                                                keyword,
                                                                p_nr++)) ;
            } // small for (0-3)
        } // big for

    }
    catch(Tfile_helper_exception & e)
    {
        cout << "While reading lookuptable file:"
                // << geo_name
             << "\n"<< e.message << endl;
        exit(2);
    }
    //---------------------
    named_pointer[name_of_this_element+"_multiplicity"] =
            Tnamed_pointer(&multiplicity_of_hits, 0, this) ;
    named_pointer[name_of_this_element+"_multiplicity_of_good_elements"] =
            Tnamed_pointer(&multiplicity_of_good_hits, 0, this) ;

    named_pointer[name_of_this_element+"_general_pd1_time_cal__when_only_one_good"] =
            Tnamed_pointer(&general_pd1_time_cal__when_only_one_good, &flag_only_one_good, this) ;

    // plastic
    named_pointer[name_of_this_element+"_multiplicity_in_plastic"] =
            Tnamed_pointer(&multiplicity_of_hits_in_plastic, 0, this) ;
    named_pointer[name_of_this_element+"_multiplicity_of_good_elements_in_plastic"] =
            Tnamed_pointer(&multiplicity_of_good_hits_in_plastic, 0, this) ;

    named_pointer[name_of_this_element+"_general_plastic_time_cal__when_only_one_good"] =
            Tnamed_pointer(&general_plastic_time_cal__when_only_one_good, &flag_only_one_good_in_plastic, this) ;




    // Note: this creating of spectra will be executed already
    // AFTER constructing
    // Tkratta_crystal_plastic and Tkratta_crystal objects
    // so it can use their geometry position
    learn_geometry_of_all_elements_of_kratta();
    create_my_spectra();


    //    cout << "End of Tkratta constructor" << endl;
}
//**************************************************************************
Tkratta::~Tkratta()
{
    save_spectra();
}
//*****************************************************************
/** No descriptions */
void Tkratta::create_my_spectra()
{
    // Matrices ==============================================
    // map of the positions // does not have a sense, as this can change
    // from expereiment, to experiment. Better to do it using Phi, Theta
    string folder = "kratta_folder/"  ;
    string name =   name_of_this_element + "_position_xy" ;

    //  /*   matr_position_xy = new spectrum_2D(
    //         name,
    //         name,
    //
    //         10, 0, 10, // KRATTA_NCOL,
    //         10, 0, 10, // KRATTA_NROW+1, 0, KRATTA_NROW,
    //         folder );
    //
    //     frs_spectra_ptr->push_back(matr_position_xy);*/

    //------------------------------------
    name = name_of_this_element + "_multiplicity"  ;
    spec_multiplicity = new spectrum_1D( name,
                                         KRATTA_NR_OF_CRYSTALS+4, 0, KRATTA_NR_OF_CRYSTALS+4,
                                         folder, "",
                                         name_of_this_element+"_multiplicity");
    frs_spectra_ptr->push_back(spec_multiplicity) ;

    name = name_of_this_element + "_multiplicity_of_good_elements"  ;
    spec_multiplicity_of_good = new spectrum_1D( name,
                                                 KRATTA_NR_OF_CRYSTALS+4, 0, KRATTA_NR_OF_CRYSTALS+4,
                                                 folder, "",
                                                 name_of_this_element+"_multiplicity_of_good_elements"
                                                 );
    frs_spectra_ptr->push_back(spec_multiplicity_of_good) ;


    name = name_of_this_element + "_multiplicity_in_plastic"  ;
    spec_plastic_multiplicity = new spectrum_1D( name,
                                                 KRATTA_NR_OF_CRYSTALS+4, 0, KRATTA_NR_OF_CRYSTALS+4,
                                                 folder , "",
                                                 name_of_this_element+"_multiplicity_in_plastic");
    frs_spectra_ptr->push_back(spec_plastic_multiplicity) ;

    name = name_of_this_element + "_multiplicity_of_good_elements_in_plastic"  ;
    spec_plastic_multiplicity_of_good = new spectrum_1D( name,
                                                         KRATTA_NR_OF_CRYSTALS+4, 0, KRATTA_NR_OF_CRYSTALS+4,
                                                         folder , "",
                                                         name_of_this_element + "_multiplicity_of_good_elements_in_plastic");
    frs_spectra_ptr->push_back(spec_plastic_multiplicity_of_good) ;


    name = name_of_this_element + "_fan"  ;
    spec_fan = new spectrum_1D( name,
                                KRATTA_NR_OF_CRYSTALS, 0, KRATTA_NR_OF_CRYSTALS,
                                folder,
                                "if kratta fired (namely: any pd0,1,2 time - was present. (More details see in individual kratta fan spectra) ",
                                "ask Jurek for a pseudo fan - if you really need this" );
    frs_spectra_ptr->push_back(spec_fan) ;

    name = name_of_this_element + "_fan_of_good"  ;
    spec_fan_of_good = new spectrum_1D( name,
                                        KRATTA_NR_OF_CRYSTALS, 0, KRATTA_NR_OF_CRYSTALS,
                                        folder, "elements which fullfill a condition: kratta 'GOOD'" ,
                                        "ask Jurek for a pseudo fan - if you really need this" );
    frs_spectra_ptr->push_back(spec_fan_of_good) ;

    // PLASTIC
    name = name_of_this_element + "_plastic_fan"  ;
    spec_plastic_fan = new spectrum_1D( name,
                                        KRATTA_NR_OF_PLASTICS*10/4, 0, KRATTA_NR_OF_PLASTICS*10/4,
                                        folder, "channel nr means: plastic_detetector_nr*10 + segment nr (0,1,2 or 3)",
                                        "ask Jurek for a pseudo fan - if you really need this");
    frs_spectra_ptr->push_back(spec_plastic_fan) ;

    name = name_of_this_element + "_plastic_fan_of_good"  ;
    spec_plastic_fan_of_good = new spectrum_1D( name,
                                                KRATTA_NR_OF_PLASTICS*10/4, 0, KRATTA_NR_OF_PLASTICS*10/4,
                                                folder, "plastic elements which are fullfill a condition: kratta 'GOOD'",
                                                "ask Jurek for a pseudo fan - if you really need this");
    frs_spectra_ptr->push_back(spec_plastic_fan_of_good) ;

    //-------------

    name = name_of_this_element + "_general_pd1_time_cal__when_only_one_good"  ;
    spec_general_pd1_time_cal__when_only_one_good = new spectrum_1D( name,
                                                                     4096, 0, 4096,
                                                                     folder,
                                                                     "when multiplicity of `good` == 1" ,
                                                                     name_of_this_element+"_general_pd1_time_cal__when_only_one_good"
                                                                     );
    frs_spectra_ptr->push_back(spec_general_pd1_time_cal__when_only_one_good) ;


    name = name_of_this_element + "_general_plastic_time_cal__when_only_one_good"  ;
    spec_general_plastic_time_cal__when_only_one_good = new spectrum_1D( name,
                                                                         4096, 0, 4096,
                                                                         folder, "when multiplicity of `good` plastic == 1",
                                                                         name_of_this_element+
                                                                         "_general_plastic_time_cal__when_only_one_good");
    frs_spectra_ptr->push_back(spec_general_plastic_time_cal__when_only_one_good) ;

    // scalers
    name = "plastic_scalers"  ;
    spec_plastic_scalers = new spectrum_1D( name,
                                            KRATTA_NR_OF_PLASTICS, 0, KRATTA_NR_OF_PLASTICS,
                                            folder, "",
                                            "No_such_incrementer_defined");
    frs_spectra_ptr->push_back(spec_plastic_scalers) ;





    //    // scalers
    //    name = "plastic_scalers"  ;
    //    spec_plastic_scalers = new spectrum_1D( name,
    //                                            KRATTA_NR_OF_PLASTICS, 0, KRATTA_NR_OF_PLASTICS,
    //                                            folder, "", "" );
    //    frs_spectra_ptr->push_back(spec_plastic_scalers) ;



    name = "fan_ratios_plastic_over_kratta"  ;
    spec_fan_ratios_plastic_over_kratta = new spectrum_1D( name,
                                                           KRATTA_NR_OF_PLASTICS, 0, KRATTA_NR_OF_PLASTICS,
                                                           folder, "No_such_incrementer_defined", "No_such_incrementer_defined" );
    frs_spectra_ptr->push_back(spec_fan_ratios_plastic_over_kratta) ;


    if(!matr_kratta_vs_plastic){
        name = "kratta_vs_plastic_coincidences"  ;
        matr_kratta_vs_plastic = new spectrum_2D( name,
                                                  KRATTA_NR_OF_PLASTICS+1 + 5, -1, KRATTA_NR_OF_PLASTICS + 5,
                                                  KRATTA_NR_OF_CRYSTALS+1 + 5, -1, KRATTA_NR_OF_CRYSTALS + 5,
                                                  folder, "nie ma inkrementorow dla tego widma",
                                                  "");
        frs_spectra_ptr->push_back(matr_kratta_vs_plastic) ;
    }

    // ++++++++++++++++++++

    name = "kratta_geometry_map"  ;
    int ile_segmentow_x_ma_detektor = 3;
    [[maybe_unused]] int ile_segmentow_y_ma_detektor = 1;

    int x0 = graph_min_x - graph_margines;
    int x1 = graph_max_x + graph_margines;

    int x_bins = (ile_segmentow_x_ma_detektor * 2) * (graph_columns + 2) * 2;
    int roznica_x = x1 - x0;
    x_bins = roznica_x / 2; // bo 2 to skok z kryształu na kryształ


    // if(graph_columns == 0) graph_columns = 1;
    // int ile_pix_na_detektor = (graph_max_x - graph_min_x) / graph_columns;

    int y0 = graph_min_y - graph_margines;
    int y1 =graph_max_y + graph_margines;
    int roznica_y = y1 - y0;
    int y_bins = 2* roznica_y / 10;

    spec_geometry = new spectrum_2D( name,
                                     x_bins,
                                     x0, x1,
                                     y_bins,
                                     y0, y1,
                                     folder, "",
                                     "X: No_such_incrementer_defined\n"
                                     "Y: No_such_incrementer_defined"
                                     );
    frs_spectra_ptr->push_back(spec_geometry) ;

    //-----------------
    //
    ile_segmentow_x_ma_detektor = 2;
    ile_segmentow_y_ma_detektor = 2;

    //    int x0 = graph_min_x - graph_margines;
    //    int x1 = graph_max_x + graph_margines;

    x_bins = (ile_segmentow_x_ma_detektor * 2) * (graph_columns + 2) * 2;
    //    int roznica_x = x1 - x0;
    x_bins = roznica_x / 2; // bo 2 to skok z kryształu na kryształ


    y_bins = 2* roznica_y / 10;

    name = "plastic_geometry_map"  ;
    spec_plastic_geometry = new spectrum_2D( name,
                                             x_bins,
                                             x0, x1,
                                             y_bins*4,
                                             y0, y1,  folder, "",
                                             "X: No_such_incrementer_defined\n"
                                             "y: No_such_incrementer_defined");
    frs_spectra_ptr->push_back(spec_plastic_geometry) ;



    name = "plastic_geometry_map_ratios"  ;
    spec_geometry_ratios = new spectrum_2D( name,
                                            x_bins,
                                            x0, x1,
                                            y_bins*4,
                                            y0, y1,  folder, "",

                                            "X: No_such_incrementer_defined\n"
                                            "y: No_such_incrementer_defined");
    frs_spectra_ptr->push_back(spec_geometry_ratios) ;
    //============

    name = "plastic_geometry_map_from_scalers"  ;
    spec_geometry_from_scalers = new spectrum_2D(  name,
                                                   x_bins,
                                                   x0, x1,
                                                   y_bins*4,
                                                   y0, y1,  folder, "",

                                                   "X: No_such_incrementer_defined\n"
                                                   "y: No_such_incrementer_defined");
    frs_spectra_ptr->push_back(spec_geometry_from_scalers) ;



}
//*************************************************************************
void Tkratta::make_preloop_action(ifstream &)
{
    cout << "Tkratta::make_preloop_action "  << endl;

    //
    // initially we zero a file with pinup txt, because later it will be
    // appended by particular kratta crystals
    //    ofstream plikG ("my_binnings/plastic_geometry_map_ratios.mat.pinuptxt", ios::trunc);
    //    plikG.close();

    //     cout  << "Cala tablica kratta " << hex << &(event_unpacked->kratta) << dec << endl;
    for(unsigned int i = 0 ; i < crystal.size() ; i++)
    {
        crystal[i]->set_event_data_pointers((event_unpacked->kratta[i]));

        //         cout  << "Ta trojka tablica " << hex << event_unpacked->kratta[i] << dec << endl;
        //        crystal[i]->set_event_data_pointers((event_unpacked->kratta[i]));
    }

    for(unsigned int i = 0 ; i < plastic.size() ; i++)
    {

        plastic[i]->set_event_data_pointers((&event_unpacked->plastic[i]));
        //plastic[i]->set_event_data_pointers((event_unpacked->kratta[i]));
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    // very important:
    // calibrations and gates are read everytime we start the analysis

    read_calibration_and_gates_from_disk();

    //    cout << "End of Tkratta preloop " << endl;

}
//**********************************************************************
void Tkratta::read_calibration_and_gates_from_disk()
{
    // (by calling cluster preloop functions)

    cout << "\n\n\n\nReading Kratta calibration from the disk" << endl ;

    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------


    ofstream plikG("my_binnings/kratta_geometry_map.mat.pinuptxt"); //  ios::app);
    if(!plikG){
        cout << "Wrong opening of the pinup text file";
        exit(1);
    }

    plikG << "30.0  5.0  KRATTA detectors seen from the Farady Cup\n" ;
    for(auto &c : crystal){
        string name = c->give_name();
        name = name.substr(name.size()- 2, 2);
        plikG   << c->give_x()  // -10
                << "   "
                << c->give_y()          // - 2
                << " "
                   //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
                << name  // + "_ phi=" +to_string(phi_geom))
                << endl;

    }
    plikG.close();




    //---------------------

    plikG.open("my_binnings/kratta_vs_plastic_coincidences.mat.pinuptxt", ios::trunc);
    if(!plikG)
    {
        cout << "error while opening pinup coincindences" << endl;
        exit(8);
    }

    plikG   << "5  33  kratta - when crystal fired (any pdf0,1,2 time present)\n"
            << "5  31  plastic - when time present\n"
            << endl;

    plikG.close();

    //-------------------------------------------------------------------------------
    string cal_filename = "calibration/kratta_calibration.txt";
    try {
        ifstream plik(cal_filename.c_str() ) ; // ascii file
        if(!plik)
        {
            string mess = "I can't open kratta calibration file: "
                    + cal_filename  ;
            throw mess ;
        }

        // real reading the calibration -----------------
        for(unsigned int i = 0 ; i < crystal.size() ; i++)
        {
            //cout << "reading calibration loop for kratta segment" << i << endl ;
            crystal[i]->make_preloop_action(plik) ;   // reference
        }


        // real reading the calibration -----------------
        for(unsigned int i = 0 ; i < plastic.size() ; i++)
        {
            //cout << "reading calibration loop for kratta segment" << i << endl ;
            plastic[i]->make_preloop_action(plik) ;   // reference
        }




        //====Having geometry we can calculate positions ====================

        plikG.open("my_binnings/plastic_geometry_map.mat.pinuptxt");
        plikG
                << "30.0  3.0  Kratta Plastic detectors seen from the Farady Cup"
                   // "0.0  -47.0  (Plastic detectors)\n"
                   // << "0.0  0.0  O\n"
                << endl;

        ofstream plikRAT("my_binnings/plastic_geometry_map_ratios.mat.pinuptxt"); //  ios::app);
        if(!plikRAT){
            cout << "Wrong opening of the pinup text file";
            exit(1);
        }

        plikRAT << "30.0  5.0  KRATTA detectors seen from the Farady Cup\n" ;


        ofstream plikSCA("my_binnings/plastic_geometry_map_from_scalers.mat.pinuptxt");
        if(!plikSCA){
            cout << "Wrong opening of the pinup text file";
            exit(1);
        }

        plikSCA << "30.0  5.0  detectors seen from the Farady Cup\n" ;
        plikSCA << "10.0   2.0  (data for this plot is delivered by scalers)\n";



        for(auto &p : plastic){
            string name = p->give_name();

            string short_name = p->give_name();
            short_name = short_name.substr(short_name.size() -3);
            //string short_name = name_of_this_element.substr(name_of_this_element.size() -1);

            plikG    << p->give_xpos()  << "   "
                     << p->give_ypos() + 1
                     << " " << char(p->give_quarter()+ 'a')
                        //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
                     << endl;

            plikSCA    << p->give_xpos()  << "   "
                       << p->give_ypos() + 1
                       << " " << char(p->give_quarter()+ 'a')
                          //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
                       << endl;
            plikRAT    << p->give_xpos()  << "   "
                       << p->give_ypos() + 1
                       << " " << char(p->give_quarter()+ 'a')
                          //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
                       << endl;

            cout    << p->give_xpos() -2  << "   "
                    << p->give_ypos()
                    << " " << char(p->give_quarter()+ 'a')
                       //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
                    << endl;



            if(p->give_quarter() == 0){
                plikG    << p->give_xpos() -2 << "   "
                         << p->give_ypos()
                         << " "
                         << short_name.substr(0,2)
                         << endl;

                plikSCA    << p->give_xpos() -2 << "   "
                           << p->give_ypos()
                           << " "
                           << short_name.substr(0,2)
                           << endl;

                plikRAT    << p->give_xpos() -2 << "   "
                           << p->give_ypos()
                           << " "
                           << short_name.substr(0,2)
                           << endl;

            }

            //        name = name.substr(name.size()- 2, 2);
            //        plikG   << p->give_x()  // -10
            //            << "   "
            //            << p->give_y()          // - 2
            //            << " "
            //            //<< (name_of_this_element) // + "_ phi=" +to_string(phi_geom))
            //            << name  // + "_ phi=" +to_string(phi_geom))
            //            << endl;

        }


        plikG.close();
        plikSCA.close();
        plikRAT.close();



    }  // end of try
    catch(Tfile_helper_exception & k)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message << endl  ;
        exit(-1);
    }
    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ; // is it healthy ?
    }

    //    cout << "kratta calibration successfully read"          << endl;

    //------------------------------------
    // we need to read the 1D gates for KRATTA itself

    // this is:
    // -  the gate on the total calibrated time
    // -  the gate on the total calibrated energy
    //#ifdef NIGDY
    // reading gates
    // but this is in a different file....

    //    cout << " reading the GOOD gates for " << name_of_this_element << "  " << endl ;

    // reading from the file
    string pname =
            //path.options +
            "./options/good_kratta.txt" ;
    ifstream plik(pname.c_str());
    if(!plik)
    {
        cout << "Cant open file " << pname << endl;
        return ;
    }

    try{
        //----------
        Tkratta_crystal::good_pd0_time_cal_requires_threshold =
                (bool) Nfile_helper::find_in_file(plik, "good_pd0_time_cal_requires_threshold") ;
        Tkratta_crystal::pd0_time_cal_lower_threshold =
                Nfile_helper::find_in_file(plik, "pd0_time_cal_lower_threshold") ;
        Tkratta_crystal::pd0_time_cal_upper_threshold =
                Nfile_helper::find_in_file(plik, "pd0_time_cal_upper_threshold") ;

        Tkratta_crystal::good_pd1_time_cal_requires_threshold =
                (bool) Nfile_helper::find_in_file(plik, "good_pd1_time_cal_requires_threshold") ;
        Tkratta_crystal::pd1_time_cal_lower_threshold =
                Nfile_helper::find_in_file(plik, "pd1_time_cal_lower_threshold") ;
        Tkratta_crystal::pd1_time_cal_upper_threshold =
                Nfile_helper::find_in_file(plik, "pd1_time_cal_upper_threshold") ;

        Tkratta_crystal::good_pd2_time_cal_requires_threshold =
                (bool) Nfile_helper::find_in_file(plik, "good_pd2_time_cal_requires_threshold") ;
        Tkratta_crystal::pd2_time_cal_lower_threshold =
                Nfile_helper::find_in_file(plik, "pd2_time_cal_lower_threshold") ;
        Tkratta_crystal::pd2_time_cal_upper_threshold =
                Nfile_helper::find_in_file(plik, "pd2_time_cal_upper_threshold") ;

        //-------
    }
    catch(Tfile_helper_exception k)
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
    }
}
//****************************************************************************************************************
void Tkratta::analyse_current_event()
{
    if(calculations_already_done) return ;

    multiplicity_of_hits = 0;
    multiplicity_of_good_hits = 0;

    multiplicity_of_hits_in_plastic = 0;
    multiplicity_of_good_hits_in_plastic = 0;

    flag_only_one_good = false;
    flag_only_one_good_in_plastic = false;

    // cout << "Kratta is analysing the current event" << endl;
    for(unsigned int i = 0 ; i < crystal.size() ; i++)
    {
        crystal[i]->analyse_current_event();

        if(crystal[i]->fired() )
        {
//            cout << "Kratta segment " << i+1 << " " << crystal[i] -> give_name() << " fired " << endl;
            ++multiplicity_of_hits;

            spec_fan->manually_increment( (int)i);  // they like (1-9)
            kratta_statistics[i]++;

            // int x_position = crystal[i]->give_x();
            // int y_position = crystal[i]->give_y();
            // matr_position_xy->manually_increment(x_position, y_position);
        }  // if this crystal fired

        if(crystal[i]->fired_good() )
        {
            //cout << "Kratta segment " << i+1 << " good " << endl;
            ++multiplicity_of_good_hits;
            spec_fan_of_good->manually_increment( (int)i);

            general_pd1_time_cal__when_only_one_good = crystal[i]->give_pd1_time_cal();
        }  // if this crystal fired in a GOOD region

    }
    spec_multiplicity->manually_increment(multiplicity_of_hits);
    spec_multiplicity_of_good->manually_increment(multiplicity_of_good_hits);

    if(multiplicity_of_good_hits == 1){
        flag_only_one_good = true;
        spec_general_pd1_time_cal__when_only_one_good->manually_increment
                (general_pd1_time_cal__when_only_one_good);
    }


    // PLASTIC ==================================
    for(unsigned int i = 0 ; i < plastic.size() ; i++)
    {
        plastic[i]->analyse_current_event();

        int which_channel = ((i / 4) * 10) + i%4;

        if(plastic[i]->fired() )
        {
            //            cout << "Plastic segment " << i+1 << " fired " << endl;
            ++multiplicity_of_hits_in_plastic;

            spec_plastic_fan->manually_increment( which_channel);

            plastic_statistics[i]++;

            // they like (1-9)
            // int x_position = crystal[i]->give_x();
            // int y_position = crystal[i]->give_y();
            // matr_position_xy->manually_increment(x_position, y_position);
        }  // if this crystal fired

        if(plastic[i]->fired_good() )
        {
            //cout << "Kratta segment " << i+1 << " good " << endl;
            ++multiplicity_of_good_hits_in_plastic;
            spec_plastic_fan_of_good->manually_increment( which_channel);

            general_plastic_time_cal__when_only_one_good = plastic[i]->give_time_cal(); // NOTE:
        }  // if this crystal fired in a GOOD region


    } // for

    // plastic -------------
    spec_plastic_multiplicity->manually_increment(multiplicity_of_hits_in_plastic);
    spec_plastic_multiplicity_of_good->manually_increment(multiplicity_of_good_hits_in_plastic);

    if(multiplicity_of_good_hits_in_plastic == 1){
        flag_only_one_good_in_plastic = true;
        spec_general_plastic_time_cal__when_only_one_good->manually_increment
                (general_plastic_time_cal__when_only_one_good);
    }

    if(event_unpacked->plastic_scalers[0] != 999999)
    {
        //spec_plastic_scalers->zeroing();
        for(int n = 0 ; n < KRATTA_NR_OF_PLASTICS ; ++n)
        {
            if( event_unpacked->plastic_scalers[n] > 0){
                spec_plastic_scalers->manually_inc_by(n, event_unpacked->plastic_scalers[n]);
                //                cout << "scaler chan " << n << " incremented by "
                //                     << event_unpacked->plastic_scalers[n] << endl;

                // TO MA BYĆ KIEDYŚ INACZEJ
                //                // need recable-ing
                //                int rec = 0;
                //                if(n >= 0 && n < 32) rec = n+64;
                //                if(n >= 32 && n < 64) rec = n;
                //                if(n >= 64 && n < 96) rec = n-64;
                // plastic[rec]->incr_geometry_from_scalers_matrix(event_unpacked->plastic_scalers[n]);
            }
            event_unpacked->plastic_scalers[0] = 999999;
        }
    }

    // loop over all plastics
    // take current valuue of fan (or scaler)
    // take corresponding fan of kratta det
    // make ratio
    // incement spectrum


    // Coincidences of plastic vs kratta -----------------------------------------------
    // matrices kratta_0N_PD0_TIME vs  plastic_0Na,b,c,d
    for(unsigned int k = 0 ; k < crystal.size() ; k++)
    {
        // get pd0_time

        if(crystal[k]->fired() )
        {
            double kratta_time = crystal[k]->give_pd0_time_cal();
#if 0
            bool was_plastic = false;
            for(unsigned int p = 0 ; p < 4 ; ++p)
            {
                if(plastic[k*4 + p]->fired())
                {
                    was_plastic = true;
                    double plastic_time = plastic[k*4 + p]->give_time_cal();
                    if(kratta_time > 1 && plastic_time >1 )
                        matr_kratta_vs_plastic->manually_increment((int)(k*4 + p), (int)k);
                }
            }
            if(was_plastic == false && (kratta_time > 1))
            {
                matr_kratta_vs_plastic->manually_increment(-1, k);
            }
#else
            bool was_plastic = false;
            for(unsigned int p = 0 ; p < plastic.size() ; ++p)
            {
                if(plastic[p]->fired())   // and kratta fired
                {
                    //was_plastic = true;
                    double plastic_time = plastic[p]->give_time_cal();
                    if(kratta_time > 1 && plastic_time >1 )
                        matr_kratta_vs_plastic->manually_increment((int)(p), (int)k);
                }
                else if(was_plastic == false && (kratta_time > 1))
                {
                    matr_kratta_vs_plastic->manually_increment(-1, k);
                }
            }
#endif
        }
        else // no crystal
        {
#if 0
            for(unsigned int p = 0 ; p < 4 ; ++p)
            {
                if(plastic[k*4 + p]->fired())
                {
                    double plastic_time = plastic[k*4 + p]->give_time_cal();
                    if(plastic_time >1 )
                        matr_kratta_vs_plastic->manually_increment((int)(k*4 + p), -1);
                }
            }
#else
            for(unsigned int p = 0 ; p < plastic.size() ; ++p)
            {
                if(plastic[p]->fired())
                {
                    double plastic_time = plastic[p]->give_time_cal();
                    if(plastic_time >1 )
                        matr_kratta_vs_plastic->manually_increment((int)(p), -1);
                }
            }
#endif
        }
    }
    //--------------------------------------------------------------------------------
    // Making statistics every 30 seconds
    static time_t last_time = time(0);
    if(time(0) - last_time > 10)
    {
        spec_fan_ratios_plastic_over_kratta->zeroing();
        zero_ratios_matrix();

        for(int i = 0 ; i < KRATTA_NR_OF_PLASTICS ; ++i)
        {
            if(kratta_statistics[i/4] == 0) continue;
            ratio_statistics[i] = 100* plastic_statistics[i] / kratta_statistics[i/4];
            // inkrement spectrum
            spec_fan_ratios_plastic_over_kratta->manually_inc_by(i,
                                                                 (int)ratio_statistics[i]);
            plastic[i]->incr_ratios_matrix( (int) ratio_statistics[i]);

            //            cout << "plastic nr " << i
            //                 << ", ratio = " << ratio_statistics[i]
            //                    << "%, plastic = " << plastic_statistics[i]
            //                       << ", kratta = " << kratta_statistics[i/4]
            //                    << endl;
        }
        last_time = time(0);
        // memset(ratio_statistics, 0, sizeof(ratio_statistics));
        memset(plastic_statistics, 0, sizeof(plastic_statistics));
        memset(kratta_statistics, 0, sizeof(kratta_statistics));
    }

    // finally
    calculations_already_done = true ;
}
//***********************************************************************
void  Tkratta::make_postloop_action()
{
    // cout << "F.Tkratta::make_postloop_action() " << endl ;
    save_spectra();
    // cout << "end of F.Tkratta::make_postloop_action() " << endl ;
}
//***********************************************************************
// packing with some artificial data
//
void Tkratta::simulate_event()
{
#ifdef NIGDY
    for(unsigned int i = 0 ; i < crystal.size() ; i++)
    {
        event_unpacked->kratta_detector[i].CsI_energy = (10*i);
        event_unpacked->kratta_detector[i].CsI_time  = (10*i) +1 ;
        event_unpacked->kratta_detector[i].Si_energy = 100*i ;
        event_unpacked->kratta_detector[i].Si_time = 100*i +1;

        event_unpacked->kratta_detector[i].pin_a = 210 + i ;
        event_unpacked->kratta_detector[i].pin_b = 220 + i ;
        event_unpacked->kratta_detector[i].pin_c = 240 + i ;
        event_unpacked->kratta_detector[i].pin_d = 250 + i ;
    }
#endif
}
//**********************************************************************
/** made in post loop, but also when the FRS ask for it (i.e. every 5 min) */
void Tkratta::save_spectra()
{

}
//***************************************************************
//***************************************************************
/** was zero, one, or more kratta segment hit by the residue how_many_hits */
// int Tkratta::how_many_hits()
// {
//     return multiplicity_of_hits ;
// }
//***************************************************************
void Tkratta::learn_geometry_of_all_elements_of_kratta()
{

    set<int> rows_set;
    set<int> columns_set;

    for( auto *c : crystal){
        c->read_geometry();
        int x = c->give_x();

        // some do not give signals so me move them out
        if(x < -700) continue;

        if(graph_max_x < x) graph_max_x = x;
        if(graph_min_x > x) graph_min_x = x;


        int y = c->give_y();
        if(graph_max_y < y) graph_max_y = y;
        if(graph_min_y > y) graph_min_y = y;

        rows_set.insert(y);
        columns_set.insert(x);

    }
    cout << " range of X = " << graph_min_x << " : " << graph_max_x << endl;
    cout << " range of Y = " << graph_min_y << " : " << graph_max_y << endl;

    // dodatki--------------------

    // This will be a matrix where we want to see boxes for every unit of crystal
    // 7 columns
    // 5 rows
    // empty places are in a middle of row 2,3,4

    graph_columns = columns_set.size();
    graph_rows = rows_set.size();

    /*
     int sections_x = 3; // dla geometry
     int sections_y = 1; // dla geometry

     constexpr int unit_size = 4;    // size of one particular 1/4 box



     int box_size = 2 * crystal_size;
     int col_gap = box_size;
     int row_gap = box_size;
     // so sizes of matrix are
     int width = col_gap + (columns * (box_size + col_gap));
     int height = row_gap + (rows * (box_size + row_gap));

*/
}
#endif   // #ifdef KRATTA_PRESENT
