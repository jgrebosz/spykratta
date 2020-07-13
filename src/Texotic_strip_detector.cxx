//
// C++ Implementation: tstopper_combined
//
// Description:
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution

#include "Texotic_strip_detector.h"

#include "experiment_def.h"

#ifdef NR_OF_EXOTIC_MODULES
#include <sstream>
#include <iomanip>
#include "Tnamed_pointer.h"
using namespace std;

#include "TIFJAnalysis.h" // for verbose
//#include "Texotic_matrix_xy.h"
#include "Texotic_one_stripe.h"   // for namespace
//using namespace exotic_namespace;
// ******************************************************************
Texotic_strip_detector::Texotic_strip_detector(
    string name,
    int nr,
    int (TIFJEvent::*data_ptr) [NR_OF_EXOTIC_MODULES]
    [NR_OF_EXOTIC_STRIPES_X][NR_OF_EXOTIC_SAMPLES],
    int (TIFJEvent::*data_fired_ptr) [NR_OF_EXOTIC_MODULES]) :
    Tfrs_element(name),
    Tincrementer_donnor(),
    number(nr),
    data(data_ptr),
    data_plate_fired(data_fired_ptr)
{

    //    to make possible the general trajectory plot
    //    incrementer_with_beamline_position_of_this_device = my_number;
    //     compression_factor_for_x_position = 1; // no compression

    //----------------- X ------------------------
    x_plate =  new Texotic_vector_strips(this,
                                         name_of_this_element  + "_x",
                                         number,
                                         true,    // I am X coordinate
                                         NR_OF_EXOTIC_STRIPES_X
                                        );

    //----------------- Y------------------------
    y_plate =  new Texotic_vector_strips(this,
                                         name_of_this_element + "_y" ,
                                         number,
                                         false ,   // I am NOT X coordinate
                                         NR_OF_EXOTIC_STRIPES_X
                                        );


    my_x_number = -1;    // will come from lookup table
    my_y_number = -1;    // will come from lookup table

    create_my_spectra();
    x_energy_cal_gate_dn = 0;
    x_energy_cal_gate_up = 4096;
    y_energy_cal_gate_dn = 0;
    y_energy_cal_gate_up = 4096;

    flag_algorithm_for_position_calculation_is_mean  = true;
    // otherwise MAX

    last  = 0;

    //===============
    //     named_pointer[name_of_this_element+"_flag_trigger_of"]=
    //         Tnamed_pointer ( &flag_trigger_of, 0, this ) ;

    //     named_pointer[name_of_this_element+"_beamline_position_of_this_device"]=
    //         Tnamed_pointer ( &, 0, this ) ;


    //--------- Incremeners of "found" implantation data
    // made for a sake of multi decay - when one of modules is in a different
    // geometric position so having more than one decay in the 3 modules stopper
    // has some sense

    // INCREMENTERS
    //---------

    string entry_name;
    Tmap_of_named_pointers::iterator pos;

    // corresponding ALLs - local to this stopper and global to all stopperS
    //   entry_name =  owner->give_name() +
    //                 "_AllModules_"
    //                 "_zet_matched_with_decay_when_ok" ;
    //
    //   Tnamed_pointer::add_as_ALL(entry_name, &found_impl_zet, &flag_matching_is_valid, this);

    //-----------

}
//***********************************************************************************************
void Texotic_strip_detector::create_my_spectra()
{
    string folder("exotic_strip_detector/");

    string name = name_of_this_element + "_x_cumullative_oscilloscope_matrix" ;
    spec_oscilloscope_matrix_x = new spectrum_2D(name,
            name,
            350, 0, 350,
            255, 0, 4096,
            folder, "text1",
            name_of_this_element + "_text2");
    frs_spectra_ptr->push_back(spec_oscilloscope_matrix_x) ;

    name = name_of_this_element + "_y_cumullative_oscilloscope_matrix" ;
    spec_oscilloscope_matrix_y = new spectrum_2D(name,
            name,
            350, 0, 350,
            255, 0, 4096,
            folder, "text1",
            name_of_this_element + "_text2");
    frs_spectra_ptr->push_back(spec_oscilloscope_matrix_y) ;

    //-----------------

    name = name_of_this_element + "_x_oscilloscope_flash"  ;
    spec_oscilloscope_flash_x = new spectrum_1D(name,
            name,
            350, 0, 350,

            folder, "   flash is made every 5 seconds",
            name_of_this_element + "   incrementers impossible");
    frs_spectra_ptr->push_back(spec_oscilloscope_flash_x) ;


    name = name_of_this_element + "_y_oscilloscope_flash"  ;
    spec_oscilloscope_flash_y = new spectrum_1D(name,
            name,
            350, 0, 350,

            folder, "   flash is made every 5 seconds",
            name_of_this_element + "   incrementers impossible");
    frs_spectra_ptr->push_back(spec_oscilloscope_flash_y) ;




    name = name_of_this_element + "_xy_position_map" ;
    spec_position_xy_map = new spectrum_2D(name,
                                           name,
                                           32, 0, 32,
                                           32, 0, 32,
                                           folder, "text1",
                                           name_of_this_element + "_text2");
    frs_spectra_ptr->push_back(spec_position_xy_map) ;

}
//*********************************************************************************************
Texotic_strip_detector::~Texotic_strip_detector()
{
    delete y_plate;
    delete x_plate;
}
//*********************************************************************************************
void Texotic_strip_detector::analyse_current_event()
{
//       cout << "Funct. Texotic_strip_detector::analyse_current_event "
//       << name_of_this_element << endl;

    zero_flags_fired_vectors();
    // every 15 seconds we will make the flash oscilloscope view
    time_t now = time(0);
    if(difftime(now, last) > 5)
    {
        flag_make_flash = true;
        last = now;
        spec_oscilloscope_flash_x->zeroing();
        spec_oscilloscope_flash_y->zeroing();
    }


    // ---------- X ----------
//   cout << "my_x_number = " << my_x_number << endl;

    if((event_unpacked->*data_plate_fired) [my_x_number])
    {
//   cout << "goin to x_plate " << endl;
        x_plate->analyse_current_event();
        // here we may take the xy info from the other detector

        universal_x_position =
            x_plate->give_thrsh_position(); //  / compression_factor_for_x_position;

        universal_x_position_ok =
            x_plate->is_position_thrsh_ok();

        //         cout
        //         << name_of_this_element
        //         << "   universal_x_position_ok = " << universal_x_position_ok
        //         << ", universal_x_position = " << universal_x_position << endl;

        if(universal_x_position_ok)
        {
            //             action();
            // matrix of encrementrs - zeroed every event
            int x = (int) universal_x_position;

            if((x < 0 || x >= (int) NR_OF_EXOTIC_STRIPES_X))
            {
//         cout << "###Illegal coordinate of the implantation pixel (X= " << x
//         << ")"<< endl;
            }
        }
    }
    else
    {
        x_plate->row_not_fired();
    }

    // ---------- Y ----------
    if((event_unpacked->*data_plate_fired) [my_y_number])
    {
        y_plate->analyse_current_event();
        // here we may take the xy info from the other detector

        universal_y_position =
            y_plate->give_thrsh_position(); //  / compression_factor_for_x_position;

        universal_y_position_ok =
            y_plate->is_position_thrsh_ok();

        //         cout
        //         << name_of_this_element
        //         << "   universal_y_position_ok = " << universal_y_position_ok
        //         << ", universal_y_position = " << universal_y_position << endl;

        if(universal_y_position_ok)
        {
            //action();
            int y = (int) universal_x_position;

            if((y < 0 || y >= (int) NR_OF_EXOTIC_STRIPES_X))
            {
//         cout << "###Illegal coordinate of the implantation pixel (Y= " << y
//         << ")"<< endl;
                return;
            }

            if(universal_x_position_ok)   // y is ok by default here
            {
                spec_position_xy_map->manually_increment(universal_x_position, universal_y_position);
            }

        }
    }
    else
    {
        y_plate->row_not_fired();
    }

    flag_make_flash = false;
}
//**********************************************************************************************
void Texotic_strip_detector::make_preloop_action(ifstream & s)    // read the calibration factors, gates
{
    //cout << "Empty funct. make_preloop_action " << endl;
    string cal_filename = "calibration/exotic_detector_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;    // ascii file
        if(!plik)
        {
            string mess = "I can't open calibration file: "
                          + cal_filename  ;
            throw mess ;
        }

        int block  = (int)
                     Tfile_helper::find_in_file(plik,
                                                name_of_this_element + "_X_comes_from_block_nr__module_nr");
        int mod = 0 ;
        plik >> zjedz >> mod;

        my_x_number = (8 * block) + mod ;
        block  = (int)
                 Tfile_helper::find_in_file(plik,
                                            name_of_this_element + "_Y_comes_from_block_nr__module_nr");
        plik >> zjedz >> mod;

        my_y_number = (8 * block) + mod ;

        //====================
        //         cout << "Preloop for X " <<    name_of_this_element  << endl;
        x_plate->make_preloop_action(plik);
        //         cout << "Preloop for X " <<    name_of_this_element  << endl;
        y_plate->make_preloop_action(plik);

        // Now reading the lookup table data - placed in the calibration file

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
        exit(-1)
        ; // is it healthy ?
    }

    // reading options ====================================
    // ==============================================
    cout << "Reading the options for " << name_of_this_element << endl;
    // --------------- options -------------------------
    // real reading the calibration -----------------
    string opt_filename = "options/exotic_detector_settings.txt";
    try
    {
        ifstream plik(opt_filename.c_str()) ;    // ascii file
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

        x_energy_cal_gate_dn  =
            Tfile_helper::find_in_file(plik,
                                       name_of_this_element + "_x_energy_gate");
        plik >> zjedz >> x_energy_cal_gate_up  ;

        y_energy_cal_gate_dn  =
            Tfile_helper::find_in_file(plik,
                                       name_of_this_element + "_y_energy_gate");
        plik >> zjedz >> y_energy_cal_gate_up  ;

        flag_algorithm_for_position_calculation_is_mean  =
            (bool)    Tfile_helper::find_in_file(plik,
                    name_of_this_element
                    + "_algorithm_for_position_calculation_is");
        // otherwise MAX

        // not all samples need to participate in the mean value. Here below, the user may decide which does.
        //        bool   sample_participates_in_mean[[NR_OF_EXOTIC_SAMPLES];
        for(int i = 0 ; i < NR_OF_EXOTIC_SAMPLES ; i++)
        {
            Texotic_one_stripe::sample_participates_in_mean[i] = true; //
        }

        try
        {

            for(int i = 0 ; i < NR_OF_EXOTIC_SAMPLES ; i++)
            {
                ostringstream s;
                s << "sample_nr_" << i << "_should_paricipate_in_calculation_of_mean_value" ;
                Texotic_one_stripe::sample_participates_in_mean[i]  =
                    (bool)    Tfile_helper::find_in_file(plik, s.str().c_str());
            }
        }
        catch(Tno_keyword_exception &k)
        {
            cerr << "Error while reading the  file "
                 << opt_filename
                 << ":\n"
                 << k.message
                 << endl  ;
            throw;
        }
        catch(Treading_value_exception &k)
        {
            cerr << "Error while reading the  file  "
                 << opt_filename
                 << ":\n"
                 << k.message
                 << endl  ;
            throw;
        }
        zero_flags_fired_vectors();

        //============================================================================


        // Here comes LOOKUP TABLE -- read from the 'calibration' file (above)   -----------------

        int *ptr =    & ((event_unpacked->*data) [my_x_number][0][0]);
        x_plate->your_array_of_data_will_be_here(ptr);

        // setting the options

        x_plate->set_options(
            x_energy_cal_gate_dn,
            x_energy_cal_gate_up,
            flag_algorithm_for_position_calculation_is_mean);

        ptr =    & ((event_unpacked->*data) [my_y_number][0][0]);
        y_plate->your_array_of_data_will_be_here(ptr);

        // setting the options

        y_plate->set_options(
            y_energy_cal_gate_dn,
            y_energy_cal_gate_up,
            flag_algorithm_for_position_calculation_is_mean);

    }

    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)    ;    // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "Error while reading options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
        ; // is it healthy ?
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
        exit(-1)
        ; // is it healthy ?
    }
    //
    //cout << name_of_this_element
    // << " options are successfully read " <<endl;
    //==============================
}
//*********************************************************************
/*!
    \fn Texotic_combined_xy::
    zero_flags_fired_vectors()
 */
void Texotic_strip_detector::zero_flags_fired_vectors()
{
    //    flag_successful = false;
}
//********************************************************************
//**********************************************************************
void Texotic_strip_detector::action()
{
    cout << "Empty function " << endl;

    // PROBABLY this is now an empty function --------------------

    if(1)
    {
        // if the y positioning is broken - we may want to match only with x
        // in such a case we ptu we put yy = 0 and match to pixel  (x, 0)

        //  if ( RisingAnalysis_ptr->is_verbose_on() )
        //         cout << name_of_this_element
        //         << "--------- Texotic_strip_detector::action()"
        //         "for " << name_of_this_element
        //         << " - in "
        //         <<  universal_x_position
        //         << endl;

        //-------------
        // matrix of encrementrs - zeroed every event
        int x = (int) universal_x_position;

        if((x < 0 || x >= (int) NR_OF_EXOTIC_STRIPES_X))
        {
            cout << "###Illegal coordinate of the implantation pixel (X= " << x
                 << ")" << endl;
            return;
        }




        int y = (int) universal_x_position;

        if((y < 0 || y >= (int) NR_OF_EXOTIC_STRIPES_X))
        {
            cout << "###Illegal coordinate of the implantation pixel (Y= " << y
                 << ")" << endl;
            return;
        }

        // here we can increment the XY matrix

        //        flag_successful = true;
    }// endif false/ture
}
//**********************************************************************
void Texotic_strip_detector::increment_oscilloscope_matrix(int x, int y, bool xORy)
{
    if(xORy)
    {
        spec_oscilloscope_matrix_x->manually_increment(x, y);
    }
    else
    {
        spec_oscilloscope_matrix_y->manually_increment(x, y);
    }

    if(flag_make_flash)
    {
        //         cout << "OSC matrix, and flash " << name_of_this_element
        //         << " incr with " << x << ", " << y
        //         << endl;
        //         static int gdzie = 150;
        if(xORy)
        {
            //spec_oscilloscope_matrix_x->manually_increment ( x, y );
            spec_oscilloscope_flash_x->manually_inc_by(x, y);
        }
        else
        {
            //spec_oscilloscope_matrix_y->manually_increment ( x, y );
            spec_oscilloscope_flash_y->manually_inc_by(x, y);
            //         spec_oscilloscope_flash->manually_increment (100);
        }
        //gdzie--;
    }  // end of make flash
}

//***********************************************************************
//***********************************************************************
#endif // #ifdef NR_OF_EXOTIC_MODULES

