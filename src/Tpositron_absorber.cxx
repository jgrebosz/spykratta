#include "Tpositron_absorber.h"

#ifdef POSITRON_ABSORBER_PRESENT

//static
double Tpositron_absorber::decay_sum_energy_all_absorbers;

using namespace active_stopper_namespace;
//*****************************************************************************************************************
Tpositron_absorber::Tpositron_absorber(string name_,
                                       int (TIFJEvent::*positron_absorber_ptr)
                                       [POSITRON_ABSORBER_NR]
                                       [POSITRON_ABSORBER_STRIPS_NR + 1], // + 1 below to transmit the back side
                                       int (TIFJEvent::*positron_absorber_fired_ptr)
                                       [POSITRON_ABSORBER_NR],
                                       int start_nr_for_id_position_arg, // To make the global trajectory matrix we must know where
                                       // on the beamline stays this device (related to the active stopper)
                                       bool it_is_front_arg)  // front absorber is conted from 10 to 0, the rear  0-10)
    :
    Tfrs_element(name_), Tincrementer_donnor(),
    positron_absorber(positron_absorber_ptr),
    positron_absorber_fired(positron_absorber_fired_ptr)
{
    // To make the global trajectory matrix we must know where
    // on the beamline stays this device (related to the active stopper)
    beamline_position_of_this_device
    = start_nr_for_id_position_arg ;

    flag_it_is_front = it_is_front_arg;  // front absorber is counted from 10 to 0, the rear  0-10

    for(int p = 0 ; p < POSITRON_ABSORBER_NR ; p++)
    {
        ostringstream s;
        s << name_of_this_element << "_plate_" << p  << "_x";

        plate.push_back(
            new Tactive_stopper_vector_strips(
                s.str(),
                p,
                POSITRON_ABSORBER_STRIPS_NR,
                true // yes, we want the decay
            )
        );
    }
    create_my_spectra();
    energy_cal_implantation_gate_dn = 0;
    energy_cal_implantation_gate_up = 8192;

    energy_cal_decay_gate_dn  = 0;
    energy_cal_decay_gate_up = 8192;

    flag_algorithm_for_implantation_position_calculation_is_mean  = true;
    // otherwise MAX

}
//***********************************************************************************************
void Tpositron_absorber::create_my_spectra()
{
    string folder("stopper/");

    //----------------------------------- 2D -----------------;

    int beg_spec = 0 ; // flag_it_is_front?  -POSITRON_ABSORBER_NR : 0;
    int end_spec = 10; // flag_it_is_front?  0 : POSITRON_ABSORBER_NR;
    int how_many_y_bins = 60 ;// bacause it covers 60 milimeters

    //   beg_spec += beamline_position_of_this_device;
    //   end_spec += beamline_position_of_this_device;

    string name = name_of_this_element + "_implantation_trajectory_thrsh"  ;
    spec_implant_trajectory_thrsh
    = new spectrum_2D(name,
                      name,
                      POSITRON_ABSORBER_NR,
                      beg_spec,
                      end_spec,
                      how_many_y_bins, 0,
                      POSITRON_ABSORBER_STRIPS_NR,
                      folder, "",
                      string("as X:") + name_of_this_element
                      + "_put_something here" +
                      ", as Y: " + name_of_this_element +
                      " put something here");

    frs_spectra_ptr->push_back(spec_implant_trajectory_thrsh) ;
    //-----------------
    name = name_of_this_element + "_decay_trajectory_thrsh"  ;
    spec_decay_trajectory_thrsh
    = new spectrum_2D(name,
                      name,
                      POSITRON_ABSORBER_NR,
                      beg_spec,
                      end_spec,
                      how_many_y_bins, 0,
                      POSITRON_ABSORBER_STRIPS_NR,
                      folder, "",
                      string("as X:") + name_of_this_element
                      + "_put_something here" +
                      ", as Y: " + name_of_this_element +
                      " put something here");

    frs_spectra_ptr->push_back(spec_decay_trajectory_thrsh) ;
    //-----------------

    for(int i = 0 ; i < POSITRON_ABSORBER_NR; i++)
    {
        name = name_of_this_element + "_plate_" + char('0' + i) + "_back_side_energy_raw"  ;
        spec_back_side_energy_raw[i] =
            new spectrum_1D(name,
                            name,
                            8192, 0, 8192,
                            folder,
                            "");

        frs_spectra_ptr->push_back(spec_back_side_energy_raw[i]) ;


        named_pointer[name] =
            Tnamed_pointer(&back_energy_raw[i], 0, this) ;
    }

    named_pointer["absorbers__decay_sum_energy_all_absorbers"] =
        Tnamed_pointer(&decay_sum_energy_all_absorbers, 0, this) ;


    named_pointer[name_of_this_element + "__decay_sum_energy"] =
        Tnamed_pointer(&decay_sum_energy_this_absorber, 0, this) ;
}
//***********************************************************************************************
Tpositron_absorber::~Tpositron_absorber()
{
    for(uint p = 0 ; p < plate.size() ; p++)
    {
        delete plate[p];
    }

}
//***********************************************************************************************
void Tpositron_absorber::analyse_current_event()
{
    //   cout << "Funct. Tpositron_absorber::analyse_current_event  for "
    //   << name_of_this_element
    //   << endl;

    // if this is the first absorber, please zero the static sum_energies
    if(name_of_this_element == "absorber_front")
    {
        decay_sum_energy_all_absorbers = 0;
    }
    decay_sum_energy_this_absorber = 0;

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

    //=============================
    for(int i = 0 ; i < POSITRON_ABSORBER_NR ; i++)
    {
        position_in_plate_in_mm[i] = -999 ;
        position_in_plate_valid[i] = false;
        back_energy_raw[i] = 0 ;

        if((event_unpacked->*positron_absorber_fired) [i])
        {
            plate[i]->analyse_current_event();

            // the last strip+1 has this energy (we do not to have +1 here!)
            back_energy_raw[i] =
                (event_unpacked->*positron_absorber)[i][POSITRON_ABSORBER_STRIPS_NR];
            if(back_energy_raw)
            {
                spec_back_side_energy_raw[i]->manually_increment(back_energy_raw[i]);
            }


            //=========================================
            // drawing trajectory on the matrix
            switch(active_stopper_event_type)
            {
            case implantation_event:
                if(plate[i]->is_position_implantation_thrsh_ok())
                {
                    spec_implant_trajectory_thrsh->manually_increment(
                        double(i),
                        plate[i]->give_implantation_thrsh_position());

                    position_in_plate_in_mm[i] = (int)((60.0 / 7) * plate[i]->give_implantation_thrsh_position());
                    position_in_plate_valid[i] = true;
                }
                break;

            case  decay_event:
                if(plate[i]->is_position_decay_thrsh_ok())
                {
                    spec_decay_trajectory_thrsh->manually_increment(
                        double(i),
                        plate[i]->give_decay_thrsh_position());

                    position_in_plate_in_mm[i] = (int)((60.0 / 7) * plate[i]->give_decay_thrsh_position());
                    position_in_plate_valid[i] = true;
                    decay_sum_energy_all_absorbers +=
                        plate[i]->give_sum_energies_above_threshold_when_decay();

                    decay_sum_energy_this_absorber += plate[i]->give_sum_energies_above_threshold_when_decay();
                }
                break;

            case undefined_event:
                break;
            } //end switch
            //---------------------------------------------------------------
        } // if fired
        else // if not fired
        {
            plate[i]->row_not_fired();
        }
    } // end for analysis of all the stripes

    calculations_already_done = true;
}
//**********************************************************************************************
void Tpositron_absorber::make_preloop_action(ifstream & s) // read the calibration factors, gates
{
    //cout << "Empty funct. make_preloop_action " << endl;
    string cal_filename = "calibration/tin100_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            string mess = "I can't open calibration file: "
                          + cal_filename  ;
            throw mess ;
        }

        //====================
        for(int i = 0 ; i < POSITRON_ABSORBER_NR ; i++)
        {
            plate[i]->make_preloop_action(plik);
        }
    }  // end of try
    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1) ; // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1) ; // is it healthy ?
    }
    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ; // is it healthy ?
    }




    // reading options =================

    cout << "Reading the options for 100Sn  positron_absorber" << endl;
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
        plik >> zjedz >> energy_cal_implantation_gate_up;


        energy_cal_decay_gate_dn  =
            Tfile_helper::find_in_file(plik,
                                       name_of_this_element + "_decay_energy_gate");

        plik >> zjedz >>  energy_cal_decay_gate_up ;


        flag_algorithm_for_implantation_position_calculation_is_mean  =
            (bool)    Tfile_helper::find_in_file(plik,
                    name_of_this_element
                    + "_algorithm_for_position_calculation_is");
        // otherwise MAX

        //====================
        for(int i = 0 ; i < POSITRON_ABSORBER_NR ; i++)
        {
            int *ptr =    &((event_unpacked->*positron_absorber)[i][0]);
            plate[i]->your_array_of_data_will_be_here(ptr);

            // setting the options
            plate[i]->set_options(
                energy_cal_implantation_gate_dn,
                energy_cal_implantation_gate_up,
                energy_cal_decay_gate_dn,
                energy_cal_decay_gate_up,
                flag_algorithm_for_implantation_position_calculation_is_mean);
        }
    }

    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1) ; // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "Error while reading options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1) ; // is it healthy ?
    }
    catch(Tfile_helper_exception &k)
    {
        cerr << "Error while reading options file "
             << opt_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1) ; // is it healthy ?
    }
    catch(string sss)
    {
        cerr << " Error while reading the file "
             << opt_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ; // is it healthy ?
    }
    //cout << name_of_this_element
    // << " options are successfully read " <<endl;

    //==============================
}
//**********************************************************************************************
int Tpositron_absorber::give_x_pos_plate(int i, bool *flag_valid)
{
    if(calculations_already_done == false)
        analyse_current_event();
    *flag_valid = position_in_plate_valid[i];
    return position_in_plate_in_mm[i];
}
//**********************************************************************************************


#endif // #ifdef POSITRON_ABSORBER_PRESENT
