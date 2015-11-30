#include "Tfinger_tom.h"



#ifdef NIGDY
////////////////////////// NEVER USED SERIOUSLY //////////////////////////////////////////////////////
#include <sstream>

#include <iomanip>   // for setw
#include "Tfrs.h"
#include "Tnamed_pointer.h"


//***********************************************************************
//  constructor
Tfinger_tom::Tfinger_tom(string _name,
                         string name_of_focal_plane,
                         //                int* (TIFJEvent::*finger_ptr))
                         int (TIFJEvent::*finger_time_ptr)[how_many_PMs],
                         int (TIFJEvent::*finger_charge_ptr)[how_many_PMs]
                        )                   :
    Tfrs_element(_name),
    finger_time(finger_time_ptr),
    finger_charge(finger_time_ptr)
{

    if(name_of_focal_plane == ""
            ||
            name_of_focal_plane == "NONE"
            ||
            name_of_focal_plane == "none"
      )
    {
        related_focal_plane = 0 ;
    }
    else
    {
        related_focal_plane = dynamic_cast<Tfocal_plane*>(owner->address_of_device(name_of_focal_plane)) ;
    }

//  charge_raw_low_gate = 1 ;
//  charge_raw_high_gate = 4090 ;

//  cal_factor_offset = cal_factor_slope = cal_factor_quadratic = 0 ;
    position_cal_factor_offset =  position_cal_factor_slope = 0;

    create_my_spectra();

    for(int i = 0 ; i  < how_many_PMs ; i++)
    {
        //
        ostringstream sss ;

        sss << "_nr_"  << setfill('0') << setw(2) << i ;

        string nam = string(name_of_this_element) +  sss.str() + "_time_raw";
        named_pointer[nam] = Tnamed_pointer(&time_raw[i], 0, this) ;

        nam = string(name_of_this_element) +  sss.str() + "_time_cal";
        named_pointer[nam] = Tnamed_pointer(&time_cal[i], 0, this) ;

        nam = string(name_of_this_element) +  sss.str() + "_charge_raw";
        named_pointer[nam] = Tnamed_pointer(&charge_raw[i], 0, this) ;
    }

    named_pointer[name_of_this_element + "_multiplicity_of_pm"] =
        Tnamed_pointer(&pm_multiplicity, 0, this) ;

    named_pointer[name_of_this_element + "_multiplicity_of_fingers"] =
        Tnamed_pointer(&finger_multiplicity, 0, this) ;

}
//************************************************************************
void Tfinger_tom::create_my_spectra()
{
    // the spectra have to live all the time, so here we can create
    // them during  the constructor
    string folder = "scintillators/" + name_of_this_element ;
    string name ;

    for(int i = 0 ; i  < how_many_PMs ; i++)
    {
        ostringstream sss ;
        sss << "_pm_nr_"  << setfill('0') << setw(2) << i ;  //<< ends;
        //--------
        name = string(name_of_this_element) +  sss.str();
        name += "_time_raw";
        spec_time_raw[i] = new spectrum_1D(name,
                                           name,
                                           4095, 1, 4096,
                                           folder);
        frs_spectra_ptr->push_back(spec_time_raw[i]) ;

        // time calibrated--------
        name = string(name_of_this_element) +  sss.str();
        name += "_time_cal";

        spec_time_cal[i] = new spectrum_1D(name,
                                           name,
                                           4095, 1, 4096,
                                           folder,
                                           "No gate here, (set it on: total time)");
        frs_spectra_ptr->push_back(spec_time_cal[i]) ;

        //---------------------------
        name = string(name_of_this_element) +  sss.str();
        name  += "_charge_raw";

        spec_charge_raw[i] = new spectrum_1D(name,
                                             name,
                                             4095, 1, 4096,
                                             folder, "GATE is used for fired 'good' -> multiplicity, position, etc.");
        frs_spectra_ptr->push_back(spec_charge_raw[i]) ;

        //----------------------------
    }

// loop over fingers (not PMs)
    for(int i = 0 ; i < how_many_fingers ; i++)
    {
        // time tof of finger - mean value of two neighbours -------

        ostringstream ppp ;
        ppp << "_finger_nr_"  << setfill('0') << setw(2) << i ;  //<< ends;
        //--------
        name = string(name_of_this_element) +  ppp.str();
        name += "_tof";

        spec_finger_tof[i] = new spectrum_1D(name,
                                             name,
                                             4095, 1, 4096,
                                             folder,
                                             "This is a mean value of  time_cal from two neighbouring PMs");
        frs_spectra_ptr->push_back(spec_finger_tof[i]) ;
    }


    // TOTAL time calibrated--------
    name = name_of_this_element + "_total_time_cal";

    spec_time_total = new spectrum_1D(name,
                                      name,
                                      4095, 1, 4096,
                                      folder,
                                      "Sum of all individual time_cal, GATE is used for fired 'good' -> multiplicity, position, etc.");
    frs_spectra_ptr->push_back(spec_time_total) ;


    // ------------- BIT PATTERN -------------------
    name = string(name_of_this_element) +   "_hit_pattern_of_pm_which_fired";

    spec_pattern_of_pm = new spectrum_1D(name,
                                         name,
                                         15, 0, 16,
                                         folder,
                                         "PM 'fired good'  when gate on total_time_cal, gate on individual charge_raw are passed");
    frs_spectra_ptr->push_back(spec_pattern_of_pm) ;

    name = string(name_of_this_element) +   "_hit_pattern_of_fingers_which_fired";

    spec_pattern_of_fingers = new spectrum_1D(name,
            name,
            15, 0, 16,
            folder, "Finger considered as 'fired good' when both PM 'fired good' ");
    frs_spectra_ptr->push_back(spec_pattern_of_fingers) ;




    //------------------POSITION -----------------
    // one spectum of the position
    name = string(name_of_this_element) +   "_position_x_fingers";

    spec_position_raw = new spectrum_1D(name,
                                        name,
                                        15, 0, 16,
                                        folder, "when multiplicity of fingers is one");
    frs_spectra_ptr->push_back(spec_position_raw) ;

    // one spectum of the position
    name = string(name_of_this_element) +   "_position_x_milimeters";

    spec_position_cal = new spectrum_1D(name,
                                        name,
                                        200, -100, 100,
                                        folder, "when multiplicity of fingers is one");
    frs_spectra_ptr->push_back(spec_position_cal) ;

    //-------------- MULTIPLICITY
    name = name_of_this_element + "_multiplicity_of_fingers"  ;
    spec_finger_multiplicity = new spectrum_1D(name,
            name,
            16, 0, 16,
            folder,
            "multiplicity of 'fired good' fingers "
            "(When gate (common) time_cal and (individual) charge_raw gates are passed)");

    frs_spectra_ptr->push_back(spec_finger_multiplicity) ;

    //--------------
    name = name_of_this_element + "_multiplicity_of_pm"  ;
    spec_pm_multiplicity = new spectrum_1D(name,
                                           name,
                                           16, 0, 16,
                                           folder,
                                           "multiplicity of 'fired good' pm."
                                           "(When gate (common) time_cal and (individual) charge_raw gates are passed)");

    frs_spectra_ptr->push_back(spec_pm_multiplicity) ;


//  //--------------
//  if(related_focal_plane)   // if not null
//  {
//    name = name_of_this_element + "_nr_vs_tracked_pos_X"  ;
//    matr_mw_sci = new spectrum_2D(  name,
//                                    name,
//                                    200, -100, 100,
//                                    32, 0, 16,
//                                    folder );
//
//    frs_spectra_ptr->push_back(matr_mw_sci) ;
//  }


    // ------ TOF
    name = name_of_this_element + "_tof"  ;
    spec_tof = new spectrum_1D(name,
                               name,
                               10000, 0, 1000000,
                               folder);   // see note above

    frs_spectra_ptr->push_back(spec_tof) ;



    // ------ BETA
    name = name_of_this_element + "_beta"  ;
    spec_beta = new spectrum_1D(name,
                                name,
                                0, 1, 1000,
                                folder);   // see note above

    frs_spectra_ptr->push_back(spec_beta) ;

}
//**********************************************************************
void Tfinger_tom::analyse_current_event()
{
    //cout << "make calculations for " <<  name_of_this_element  << endl ;
    // initially
    //e_ok = false ;
    flag_tof_ok = false ;

    //
    for(int i = 0 ; i  < how_many_PMs ; i++)
    {
        //   cout << "finger " << i << " raw value = "
        //        <<event_unpacked->finger[i] << endl;

        spec_time_raw[i]->manually_increment(time_raw[i] =  event_unpacked->sci_finger_time[i]);
        spec_charge_raw[i]->manually_increment(charge_raw[i] = event_unpacked->sci_finger_charge[i]);

        // here we can calibrate the time
        if(time_raw[i])
        {
            time_cal[i] = (time_calibr_factors[i][1] * (time_raw[i]
                           + randomizer::randomek())) + time_calibr_factors[i][0]  ;

            spec_time_cal[i]->manually_increment(time_cal[i]);
            spec_time_total->manually_increment(time_cal[i]);
        }
        else time_cal[i] = 0;

    } // end for


    // which really fired we can know if we look the neighbourning PMs, because
    // the photomultiplier is always watchig two fingers

    finger_multiplicity = 0 ;
    pm_multiplicity = 0 ;

    for(int i = 0 ; i  < how_many_PMs ; i++)
    {
        pm_fired[i] = false;

        // checking if the PM "fired"
        if(time_cal[i] >= total_time_cal_gate[0]
                &&
                time_cal[i] <= total_time_cal_gate[1]
                &&
                charge_raw_gate[i][0]  <= charge_raw[i]
                &&
                charge_raw_gate[i][1]  >= charge_raw[i]
          )
        {
            pm_multiplicity++;
            // incrementing the pm pattern
            pm_fired[i] = true ;
            spec_pattern_of_pm->manually_increment(i);
//      cout << "Pm " << i << " fired because charge is " << charge_raw[i]
//      << " gate low = " << charge_raw_gate[i][0]
//      << " gate high = " << charge_raw_gate[i][1] << endl;

        }
    }
    spec_pm_multiplicity->manually_increment(pm_multiplicity);


    // checking if the FINGER "fired" - when the next PM also fired
    for(int i = 0 ; i  < how_many_fingers ; i++)
    {
        finger_fired[i] = false;

        if(pm_fired[i]  &&   pm_fired[i + 1])
        {
            finger_fired[i] = true;
            finger_multiplicity++;
            spec_pattern_of_fingers->manually_increment(i);

            spec_finger_tof[i]->manually_increment((time_cal[i] + time_cal[i]) / 2);
        }
    }
    spec_finger_multiplicity->manually_increment(finger_multiplicity);




//  // PRODUCING MATRIX TO MAKE POSTION CALIBRATION FACTORS
//
//  // this were TIME data comming from the scintilator,
//  // Having this we are able to estimate the geometric position
//  // of the scintillation inside the cyistal.
//  // Unfortunately we can not calibrate
//  // it in a classical way. (how to convert this time into milimeters)
//  // But there is a trick...
//
//
//  // In the first phase of the experiment we have also Multiwires
//  // working. They can help us to extrapolate the point where
//  // ion hit the scintillator. So we have two information comming
//  // from different devices. If MW was calibrated - we can use
//  // this calibration to make the calibration of the scintillator
//
//
//
//  // this is an "official comment":
//  //------Relation between the position of MW and  SCI----------
//
//  // this is important in the moment of calibration
//  // when the data of mw are present, and we want to see the
//  // matrices to see invent the calibration factors
//
//
//  // if this scintillator is related to any focusing point which
//  // was measured by two neighbouring MWs
//
//  if(related_focal_plane )
//  {
//    if(related_focal_plane-> was_x_ok()  )
//    {
//      double mw_x_pos = related_focal_plane->extrapolate_x_to_distance(distance) ;
//
//      for(int i = 0 ; i  < 15 ; i++)
//      {
//        // check if it is in the gate?
//        if(time_raw[i])
//        {
//          //cout << "for finger i = " << i << ", x_pos = " << x_pos << endl;
//          matr_mw_sci->manually_increment((int)mw_x_pos, i);  // extrapolated_position_value
//        }
//      } // for
//
//    } // if x was ok
//
//  } // end if rel focal plane


    // POSITION CALCULATION

    // now there is a part assuming that above calibration has already been made
    // and calibration factors are stored in the calibration file.
    // there is no conflict between these two actions. (above: producing matrices
    // to read from them calibration factors
    // and action below: making real calculation of the position on scintilator,



    position = -9999 ;
    position_ok = false;

    // we make the position only if the charge (QDC)of the corresponding PMs
    // is in some range
    if(finger_multiplicity == 1)
    {
        for(int i = 0 ; i  < how_many_fingers ; i++)
        {
            if(finger_fired[i])
            {
                double tmp =  i  + randomizer::randomek() ;

                position =  position_cal_factor_offset + (position_cal_factor_slope * tmp);

                position_ok = true;
                spec_position_raw->manually_increment(i)  ;
                spec_position_cal->manually_increment(position)  ;

                tof = (time_cal[i] + time_cal[i]) / 2;
                spec_tof->manually_increment(tof);
                flag_tof_ok = true ;

                //=============================================
                // to calcultate something what was directly at the beginning of identification
                // algorithm

                // calibration factor for beta_tof, are from the file
                beta =   beta_calibr_factor1 / (beta_calibr_factor0  - tof)  ;
                if(beta > 0 && beta < 1.0)
                {
                    gamma = 1. / (sqrt(1. - beta * beta));
                    bega = beta * gamma;
                    spec_beta->manually_increment(beta);
                }
                break;
            }// end if finger fired
        } // end for

    }// end   if mult
    calculations_already_done = true ;
}
//**************************************************************************
void  Tfinger_tom::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;

    distance = Tfile_helper::find_in_file(s, name_of_this_element  + "_distance");

    // reading the calibration factors

    position_cal_factor_offset = Tfile_helper::find_in_file(s, name_of_this_element  + "_position_calibration_offset");
    position_cal_factor_slope = Tfile_helper::find_in_file(s, name_of_this_element  + "_position_calibration_slope");
//  cal_factor_quadratic = Tfile_helper::find_in_file(s, name_of_this_element  + "_quadratic");

    //charge_raw_low_gate = (int)Tfile_helper::find_in_file(s, name_of_this_element  + "_charge_raw_low_gate");
    //charge_raw_high_gate = (int)Tfile_helper::find_in_file(s, name_of_this_element  + "_charge_raw_high_gate");

    charge_raw_gate.resize(how_many_PMs);

    for(int i = 0 ; i < how_many_PMs ; i++)
    {
        charge_raw_gate[i].resize(2);  // up, down
        charge_raw_gate[i][0] = 0;
        charge_raw_gate[i][1] = 4095;

        ostringstream outs;  // Declare an output string stream.
        outs << "gates/" + name_of_this_element << "_pm_nr_"  << setfill('0') << setw(2) << i
             << "_charge_raw_gate_GATE.gate";
        string nam_gate = outs.str();      // Get the created string from the output stream.


        ifstream plik_gatesX(nam_gate.c_str());
        if(! plik_gatesX)
        {
            string mess = "I can't open  file: " + nam_gate
                          + "\nThis gate you can produce interactively using a 'cracow' viewer prg_ram" ;
            cout << mess
                 << "\nI create a gate and assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
                 << endl ;

            ofstream pliknew(nam_gate.c_str());
            pliknew  << charge_raw_gate[i][0]  << "     " << charge_raw_gate[i][1] << endl;
        }
        else
        {
            plik_gatesX >> zjedz >> charge_raw_gate[i][0]      // lower limit
                        >> zjedz >> charge_raw_gate[i][1]  ;   // upper limit

            if(! plik_gatesX)
            {
                string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
                exit(-1) ;
            }
            plik_gatesX.close() ;
        }

    }
// ============= Reading total cal gate ====================


    total_time_cal_gate.resize(2);
    total_time_cal_gate[0] = 0 ;   // defaults wide open gate
    total_time_cal_gate[1] = 4095 ;


    ostringstream outs;  // Declare an output string stream.
    outs << "gates/" + name_of_this_element
         << "_total_time_cal_gate_GATE.gate";
    string nam_gate = outs.str();      // Get the created string from the output stream.


    ifstream plik_gatesX(nam_gate.c_str());
    if(! plik_gatesX)
    {
        string mess = "I can't open  file: " + nam_gate
                      + "\nThis gate you can produce interactively using a 'cracow' viewer prg_ram" ;
        cout << mess
             << "\nI create a gate and assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
             << endl ;

        ofstream pliknew(nam_gate.c_str());
        pliknew  << total_time_cal_gate[0]  << "     " << total_time_cal_gate[1] << endl;
    }
    else
    {
        plik_gatesX >> zjedz >> total_time_cal_gate[0]      // lower limit
                    >> zjedz >> total_time_cal_gate[1]  ;   // upper limit

        if(! plik_gatesX)
        {
            string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
            exit(-1) ;
        }
        plik_gatesX.close() ;
    }



//===================

    beta_calibr_factor0 = Tfile_helper::find_in_file(s, name_of_this_element  + "_beta_calibr_factor0");
    beta_calibr_factor1 = Tfile_helper::find_in_file(s, name_of_this_element  + "_beta_calibr_factor1");





    // reading calibrations for time (will need this for ToF)?

    time_calibr_factors.resize(how_many_PMs) ;
    string slowo;
    for(int i = 0 ; i < how_many_PMs ; i++)
    {
        ostringstream outs;  // Declare an output string stream.
        outs << name_of_this_element << "_pm_"  << setfill('0') << setw(2) << i << "_time_cal_factors";
        slowo = outs.str();      // Get the created string from the output stream.

        //cout << "Try to spot the word >>" << slowo << "<<" << endl;
        Tfile_helper::spot_in_file(s, slowo);


        for(int f = 0 ; f < 2 ; f++)
        {
            // otherwise we read the data
            double value ;
            s >> value ;

            if(!s)
            {
                string mess =
                    "I found keyword: " + slowo
                    + " but error was in reading its value." ;
                throw mess ;

            }
            // if ok

            time_calibr_factors[i].push_back(value) ;
            // break ; // ?

        }
    }



}
//***********************************************************************
//***********************************************************************
//***********************************************************************

#endif   // NEVER -------------------------------------
