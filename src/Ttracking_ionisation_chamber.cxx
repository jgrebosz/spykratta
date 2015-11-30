//
// C++ Implementation: tstopper_combined
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "Ttracking_ionisation_chamber.h"

#ifdef TRACKING_IONISATION_CHAMBER_X

#include <sstream>
#include "Tnamed_pointer.h"
using namespace std;


#include "TIFJAnalysis.h" // for verbose

using namespace active_stopper_namespace;
// ******************************************************************
Ttracking_ionisation_chamber::Ttracking_ionisation_chamber(
    string name, bool flag_decay_situation_is_needed_,
    int (TIFJEvent::*data_ptr)
    [TRACKING_IONISATION_CHAMBER_X + TRACKING_IONISATION_CHAMBER_Y],
    int (TIFJEvent::*data_fired_ptr)
):
    Tfrs_element(name),
    Tincrementer_donnor(),
    xy_matrix_data(data_ptr),
    xy_matrix_plate_fired(data_fired_ptr),
    flag_decay_situation_is_needed(flag_decay_situation_is_needed_)

{

    //----------------- X ------------------------
    string nam = name_of_this_element + "_plate_x"  ;

    x_plate =  new Tactive_stopper_vector_strips(
        nam,
        0,
        TRACKING_IONISATION_CHAMBER_X,
        flag_decay_situation_is_needed// we do not need the decay situation
    );
    // ----------------- Y -------------------------

    nam.clear();
    nam =  name_of_this_element + "_plate_y" ;

    y_plate =  new Tactive_stopper_vector_strips(
        nam,
        1,
        TRACKING_IONISATION_CHAMBER_Y,
        flag_decay_situation_is_needed // we do not need the decay situation
    );

    create_my_spectra();
    energy_cal_implantation_gate_dn = 0;
    energy_cal_implantation_gate_up = 8192;

    energy_cal_decay_gate_dn  = 0;
    energy_cal_decay_gate_up = 8192;

    flag_algorithm_for_implantation_position_calculation_is_mean  = true;
    // otherwise MAX

}
//***********************************************************************************************
void Ttracking_ionisation_chamber::create_my_spectra()
{
    string folder("stopper/");

    //----------------------------------- 2D -----------------;
    string name = name_of_this_element + "__xy_strips_map_for_implantations_thrsh"  ;
    spec_map_for_implantations_thrsh
    = new spectrum_2D(name,
                      name,
                      TRACKING_IONISATION_CHAMBER_X, 0,
                      TRACKING_IONISATION_CHAMBER_X,
                      TRACKING_IONISATION_CHAMBER_Y, 0,
                      TRACKING_IONISATION_CHAMBER_Y,
                      folder, "",
                      string("as X:") + name_of_this_element
                      + "_put_something here" +
                      ", as Y: " + name_of_this_element +
                      " put something here");

    frs_spectra_ptr->push_back(spec_map_for_implantations_thrsh) ;

    //--------------------

    name = name_of_this_element + "__implantation_position_x_calibrated"  ;
    spec_implantation_x_calibrated = new spectrum_1D(name,
            name,
            1000, -500, +500,
            folder, "");
    frs_spectra_ptr->push_back(spec_implantation_x_calibrated) ;
    //---------------
    name = name_of_this_element + "__implantation_position_y_calibrated"  ;
    spec_implantation_y_calibrated = new spectrum_1D(name,
            name,
            1000, -500, +500,
            folder, "");
    frs_spectra_ptr->push_back(spec_implantation_y_calibrated) ;

    /*  name = name_of_this_element + "_map_for_implantations_calibrated"  ;
      spec_map_for_implantations_calibrated
      = new spectrum_2D( name,
                         name,
                         TRACKING_IONISATION_CHAMBER_X, 0,
                         TRACKING_IONISATION_CHAMBER_X,
                         TRACKING_IONISATION_CHAMBER_Y, 0,
                         TRACKING_IONISATION_CHAMBER_Y,
                         folder, "",
                         string("as X:") + name_of_this_element
                         +"_put_something here" +
                         ", as Y: " + name_of_this_element+
                         " put something here");

      frs_spectra_ptr->push_back(spec_map_for_implantations_thrsh) ;
    */

    //-----------------
    if(flag_decay_situation_is_needed)
    {

        name = name_of_this_element + "__decay_position_x_calibrated"  ;
        spec_decay_x_calibrated = new spectrum_1D(name,
                name,
                1000, -500, +500,
                folder, "");
        frs_spectra_ptr->push_back(spec_decay_x_calibrated) ;
        //---------------
        name = name_of_this_element + "__decay_position_y_calibrated"  ;
        spec_decay_y_calibrated = new spectrum_1D(name,
                name,
                1000, -500, +500,
                folder, "");
        frs_spectra_ptr->push_back(spec_decay_y_calibrated) ;


        name = name_of_this_element + "__xy_strips_map_for_decays_thrsh"  ;

        spec_map_for_decays_thrsh
        = new spectrum_2D(name,
                          name,
                          TRACKING_IONISATION_CHAMBER_X, 0,
                          TRACKING_IONISATION_CHAMBER_X,
                          TRACKING_IONISATION_CHAMBER_Y, 0,
                          TRACKING_IONISATION_CHAMBER_Y,
                          folder, "",
                          string("as X:") + name_of_this_element
                          + "_put_something here" +
                          ", as Y: " + name_of_this_element +
                          " put something here");

        frs_spectra_ptr->push_back(spec_map_for_decays_thrsh) ;
    }
    //-----------------
    named_pointer[name_of_this_element +
                  "__impl_position_x_calibrated_when_ok"]
    = Tnamed_pointer(&impl_position_x_calibrated,
                     &impl_position_x_calibrated_validator, this) ;

    named_pointer[name_of_this_element +
                  "__impl_position_y_calibrated_when_ok"]
    = Tnamed_pointer(&impl_position_y_calibrated,
                     &impl_position_y_calibrated_validator, this) ;

    if(flag_decay_situation_is_needed)
    {
        named_pointer[name_of_this_element +
                      "__decay_position_x_calibrated_when_ok"]
        = Tnamed_pointer(&decay_position_x_calibrated,
                         &decay_position_x_calibrated_validator, this) ;

        named_pointer[name_of_this_element +
                      "__decay_position_y_calibrated_when_ok"]
        = Tnamed_pointer(&decay_position_y_calibrated,
                         &decay_position_y_calibrated_validator, this) ;

    }
    //---------------------

}
//***********************************************************************************************
Ttracking_ionisation_chamber::~Ttracking_ionisation_chamber()
{
    delete x_plate;
    delete y_plate;
}
//***********************************************************************************************
void Ttracking_ionisation_chamber::analyse_current_event()
{
    //     cout << "Funct. Ttracking_ionisation_chamber::analyse_current_event  "
    // << name_of_this_element
    //     << endl;

    switch(event_unpacked->trigger)
    {
    case 2:
    case 4:
        active_stopper_event_type = implantation_event;
        break;

    case 3:
    case 5:
        active_stopper_event_type = decay_event;
        break;
    default:
        active_stopper_event_type = undefined_event;
        break;
    }

    impl_position_x_calibrated = 0;
    impl_position_y_calibrated = 0;
    impl_position_x_calibrated_validator = false;
    impl_position_y_calibrated_validator = false;

    decay_position_x_calibrated = 0;
    decay_position_y_calibrated = 0;
    decay_position_x_calibrated_validator = false;
    decay_position_y_calibrated_validator = false;

    /*----
      for(int i = 0 ; i < 256 ;i++)
      {
        string ax = " Y ";
        if(i < 192) ax = " X" ;

        cout <<"    ["<< i << "] " << ax
        <<(( i >=192)? i - 192 : i )
        << " energy raw=   "
        << ((event_unpacked->*xy_matrix_data)[i])
        << endl;

    ******/
    //=============================
    if((event_unpacked->*xy_matrix_plate_fired))
    {
        x_plate->analyse_current_event();
    }
    else
    {
        x_plate->row_not_fired();
    }

    if((event_unpacked->*xy_matrix_plate_fired))   // skipping (offsetting) x stripes
    {
        y_plate->analyse_current_event();
    }
    else
    {
        y_plate->row_not_fired();
    }

    //       spec_fan->manually_increment( (int) i*10 + 400 + channel);


    // drawing map of the matrix
    switch(active_stopper_event_type)
    {
    case implantation_event:
        //         cout << name_of_this_element << " implant, "
        //          << endl;

        impl_position_x_calibrated_validator =
            x_plate->is_position_implantation_thrsh_ok();
        impl_position_y_calibrated_validator =
            y_plate->is_position_implantation_thrsh_ok();

        if(impl_position_x_calibrated_validator
                &&
                impl_position_y_calibrated_validator)
        {

            double x_orig  = x_plate->give_implantation_thrsh_position();
            double y_orig  = y_plate->give_implantation_thrsh_position();

            spec_map_for_implantations_thrsh->
            manually_increment(x_orig,   y_orig);

            // calibration of the X position ----------------------------------
            if(position_x_calibr_factors[2] == 0)
            {
                impl_position_x_calibrated = position_x_calibr_factors[0]
                                             + (x_orig * position_x_calibr_factors[1]);
            }
            else for(unsigned int i = 0; i < position_x_calibr_factors.size(); i++)
                {
                    impl_position_x_calibrated +=
                        position_x_calibr_factors[i] * pow(x_orig, (int)i);
                }
            // calibration of the Y position ----------------------------------
            if(position_y_calibr_factors[2] == 0)
            {
                impl_position_y_calibrated = position_y_calibr_factors[0]
                                             + (y_orig * position_y_calibr_factors[1]);
            }
            else for(unsigned int i = 0; i < position_y_calibr_factors.size(); i++)
                {
                    impl_position_y_calibrated +=
                        position_y_calibr_factors[i] * pow(y_orig, (int)i);
                }

            // now we can increment the spectra.
            // not map matrices, because I do not know the ranges
            spec_implantation_x_calibrated->manually_increment(impl_position_x_calibrated);
            spec_implantation_y_calibrated->manually_increment(impl_position_y_calibrated);

            //       cout << " impl_position_x_calibrated = " << impl_position_x_calibrated
            //       << endl;

        }
        break;

    case  decay_event:
        //     cout << name_of_this_element << "decay, "
        //     << endl;

        if(flag_decay_situation_is_needed)
        {
            if((decay_position_x_calibrated_validator = x_plate->is_position_decay_thrsh_ok())// <- really assignment
                    &&
                    (decay_position_y_calibrated_validator = y_plate->is_position_decay_thrsh_ok()) // <- really assignment
              )
            {
                //         cout << name_of_this_element << " map of decay, thrsh x = "
                //         <<  x_plate->give_decay_thrsh_position()
                //         << ", y = " <<  y_plate->give_decay_thrsh_position()
                //         << endl;

                double x_orig  = x_plate->give_decay_thrsh_position();
                double y_orig  = y_plate->give_decay_thrsh_position();

                spec_map_for_decays_thrsh->
                manually_increment(x_orig,   y_orig);

                // calibration of the X position ----------------------------------
                if(position_x_calibr_factors[2] == 0)
                {
                    decay_position_x_calibrated = position_x_calibr_factors[0]
                                                  + (x_orig * position_x_calibr_factors[1]);
                }
                else for(unsigned int i = 0; i < position_x_calibr_factors.size(); i++)
                    {
                        decay_position_x_calibrated +=
                            position_x_calibr_factors[i] * pow(x_orig, (int)i);
                    }
                // calibration of the Y position ----------------------------------
                if(position_y_calibr_factors[2] == 0)
                {
                    decay_position_y_calibrated = position_y_calibr_factors[0]
                                                  + (y_orig * position_y_calibr_factors[1]);
                }
                else for(unsigned int i = 0; i < position_y_calibr_factors.size(); i++)
                    {
                        decay_position_y_calibrated +=
                            position_y_calibr_factors[i] * pow(y_orig, (int)i);
                    }

                // now we can increment the spectra.
                // not map matrices, because I do not know the ranges
                spec_decay_x_calibrated->manually_increment(decay_position_x_calibrated);
                spec_decay_y_calibrated->manually_increment(decay_position_y_calibrated);

                //       cout << " decay_position_x_calibrated = " << decay_position_x_calibrated
                //       << endl;

            }

        }
        break;
    case undefined_event:
        break;
    } //end switch
    //---------------------------------------------------------------


    calculations_already_done = true ;
}
//**********************************************************************************************
void Ttracking_ionisation_chamber::make_preloop_action(ifstream & /* s */) // read the calibration factors, gates
{
    //cout << "Empty funct. make_preloop_action " << endl;
    string cal_filename = "calibration/tin100_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            cout << "I can't open calibration file: "
                 << cal_filename  ;
            exit(1) ;
        }


        distance = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_distance");

        // in this file we look for a string
        int how_many_factors = 3;

        //-- X -----------------------------------------------------------------------------
        position_x_calibr_factors.clear();
        string slowo = name_of_this_element + "_position_x_calibr_factors";

        // first reading the energy calibration
        Tfile_helper::spot_in_file(plik, slowo);
        for(int i = 0; i < how_many_factors; i++)
        {
            // otherwise we read the data
            double value;
            plik >> value;
            if(!plik)
            {
                Treading_value_exception capsule;
                capsule.message =
                    "I found keyword '" + slowo
                    + "' but error was in reading its value.";
                throw capsule;
            }
            // if ok
            position_x_calibr_factors.push_back(value);
        }
        //========= Y ===============================
        position_y_calibr_factors.clear();
        slowo = name_of_this_element + "_position_y_calibr_factors";

        // first reading the energy calibration
        Tfile_helper::spot_in_file(plik, slowo);
        for(int i = 0; i < how_many_factors; i++)
        {
            // otherwise we read the data
            double value;
            plik >> value;
            if(!plik)
            {
                Treading_value_exception capsule;
                capsule.message =
                    "I found keyword '" + slowo
                    + "' but error was in reading its value.";
                throw capsule;
            }
            // if ok
            position_y_calibr_factors.push_back(value);

        }

        //====================
        x_plate->make_preloop_action(plik);
        y_plate->make_preloop_action(plik);

    }  // end of try
    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ;
    }

    //=============================
    // reading options =================

    cout << "Reading the options for 100Sn,  TIC. "  << name_of_this_element << endl;
    // --------------- options -------------------------
    // real reading the calibration -----------------

    string opt_filename = "options/tin100_settings.txt";

    try
    {
        ifstream plik(opt_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            /*      string mess = "I can't open file: "
            + opt_filename  ;
            throw mess ;*/
            cout << "I can't open file: "  << opt_filename << endl ;
            //       cout << "To create this file you need to run 'Cracow' and choose"
            //       " the option: Spy_options->Active stopper settings\n"
            //       "Then the dialog box will appear. It is enough to press 'OK' and"
            //       " this file will be created." << endl;
            exit(1);
        }

        energy_cal_implantation_gate_dn  =
            Tfile_helper::find_in_file(plik,
                                       name_of_this_element + "_implantation_energy_gate");
        plik >> zjedz >> energy_cal_implantation_gate_up ;


        if(flag_decay_situation_is_needed)
        {
            energy_cal_decay_gate_dn  =
                Tfile_helper::find_in_file(plik,
                                           name_of_this_element + "_decay_energy_gate");
            plik >> zjedz >>     energy_cal_decay_gate_up ;

        }


        flag_algorithm_for_implantation_position_calculation_is_mean  =
            (bool)    Tfile_helper::find_in_file(plik,
                    name_of_this_element
                    + "_algorithm_for_position_calculation_is");
        // otherwise MAX

        //====================
        int *ptr =    &((event_unpacked->*xy_matrix_data)[0]);
        x_plate->your_array_of_data_will_be_here(ptr);

        // setting the options
        x_plate->set_options(
            energy_cal_implantation_gate_dn,
            energy_cal_implantation_gate_up,
            energy_cal_decay_gate_dn,
            energy_cal_decay_gate_up,
            flag_algorithm_for_implantation_position_calculation_is_mean);

        ptr =    &((event_unpacked->*xy_matrix_data)[TRACKING_IONISATION_CHAMBER_X]);
        y_plate->your_array_of_data_will_be_here(ptr);

        // setting the options
        y_plate->set_options(
            energy_cal_implantation_gate_dn,
            energy_cal_implantation_gate_up,
            energy_cal_decay_gate_dn,
            energy_cal_decay_gate_up,
            flag_algorithm_for_implantation_position_calculation_is_mean);
    }

    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "Error while reading options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1);
    }
    catch(Tfile_helper_exception &k)
    {
        cerr << "Error while reading options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
    }
    catch(string sss)
    {
        cerr << " Error while reading the file "
             << opt_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ;
    }

    //cout << name_of_this_element
    // << " options are successfully read " <<endl;

    //==============================

}

#endif // #ifdef TRACKING_IONISATION_CHAMBER_X
