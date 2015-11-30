#include "Ttime_projection_chamber.h"

#if defined TPC41_PRESENT || defined TPC42_PRESENT

#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h" // for verbose mode
//static
int Ttime_projection_chamber::tpc_det_counter;
//***********************************************************************
//  constructor
Ttime_projection_chamber::Ttime_projection_chamber(string _name,
        int (TIFJEvent::*drift_time_array_ptr)[TPC_NR_OF_DRIFT_TIMES],
        int (TIFJEvent::*left_time_array_ptr)[TPC_NR_OF_X_TIMES],
        int (TIFJEvent::*right_time_array_ptr)[TPC_NR_OF_X_TIMES],
        int (TIFJEvent::*drift_energy_array_ptr)[TPC_NR_OF_DRIFT_TIMES],
        int (TIFJEvent::*left_energy_array_ptr)[TPC_NR_OF_X_TIMES],
        int (TIFJEvent::*right_energy_array_ptr)[TPC_NR_OF_X_TIMES]
                                                  ) :
    Tfrs_element(_name),
    drift_time_array(drift_time_array_ptr),
    left_time_array(left_time_array_ptr),
    right_time_array(right_time_array_ptr),
    drift_energy_array(drift_energy_array_ptr),
    left_energy_array(left_energy_array_ptr),
    right_energy_array(right_energy_array_ptr)
{
    create_my_spectra();

    my_tpc_det_nr = tpc_det_counter++;

	 
	 named_pointer[name_of_this_element + "_x_when_ok"]
    = Tnamed_pointer(&x, &x_ok, this) ;
    named_pointer[name_of_this_element + "_y_when_ok"]
    = Tnamed_pointer(&y, &y_ok, this) ;


    named_pointer[name_of_this_element + "_x0_minus_x1_when_ok"]
    = Tnamed_pointer(&x0_minus_x1, &x_ok, this) ;


    named_pointer[name_of_this_element + "_nr_of_checksums_true__when_times_fired"]
    = Tnamed_pointer(&nr_checksums_true, &flag_fired_times, this) ;


    named_pointer[name_of_this_element + "_left_time_raw[0]"]
    = Tnamed_pointer(&left_time_raw[0], 0, this) ;

    named_pointer[name_of_this_element + "_left_time_raw[1]"]
    = Tnamed_pointer(&left_time_raw[1], 0, this) ;

    named_pointer[name_of_this_element + "_right_time_raw[0]"]
    = Tnamed_pointer(&right_time_raw[0], 0, this) ;

    named_pointer[name_of_this_element + "_right_time_raw[1]"]
    = Tnamed_pointer(&right_time_raw[1], 0, this) ;


    named_pointer[name_of_this_element + "_energy_deposit"]
    = Tnamed_pointer(&deposit_energy, 0, this) ;

    // for rates - purposes
    //   named_pointer[name_of_this_element+"_left_raw_fired"]
    //   = Tnamed_pointer(&left_raw, &left_raw_fired, this) ;
    //
    //   named_pointer[name_of_this_element+"_right_raw_fired"]
    //   = Tnamed_pointer(&right_raw, &right_raw_fired, this) ;
    //
    //   named_pointer[name_of_this_element+"_up_raw_fired"]
    //   = Tnamed_pointer(&up_raw, &up_raw_fired, this) ;
    //
    //   named_pointer[name_of_this_element+"_down_raw_fired"]
    //   = Tnamed_pointer(&down_raw, &down_raw_fired, this) ;
    //
    //   named_pointer[name_of_this_element+"_anode_raw_fired"]
    //   = Tnamed_pointer(&anode_raw, &anode_raw_fired, this) ;



}
//************************************************************************
//************************************************************************
void Ttime_projection_chamber::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "time_projection_chamber/" + name_of_this_element ;
    string name;

    for(int i = 0 ; i < TPC_NR_OF_DRIFT_TIMES ; i++)
    {
        name = name_of_this_element + "_drift_time_" + char('0' + i) + "_raw" ;
        spec_drift_time_raw[i] = new spectrum_1D(name,
                name,
                4096, 0, 4096,
                folder, "", name);
        frs_spectra_ptr->push_back(spec_drift_time_raw[i]) ;


        name = name_of_this_element + "_drift_energy_" + char('0' + i) + "_raw" ;
        spec_drift_energy_raw[i] = new spectrum_1D(name,
                name,
                4096, 0, 4096,
                folder, "", name);
        frs_spectra_ptr->push_back(spec_drift_energy_raw[i]) ;


        // X_ SUM------------------------------
        name =  name_of_this_element + "_check_sum_" + char('0' + i) ;

        string explan = "GATE is for calculation positiions x";
        explan += ((i / 2) ? "0" : "1");
        spec_check_sum[i] = new spectrum_1D(name,
                                            name,
                                            16000, -8000, 8000,
                                            folder,
                                            explan, noinc);
        frs_spectra_ptr->push_back(spec_check_sum[i]) ;
        // in case is the gate is not set yet
        check_sum_gate[i][0] = 0 ;    // lower limit
        check_sum_gate[i][1] = +8191;

    }


    for(int i = 0 ; i < TPC_NR_OF_X_TIMES ; i++)
    {
        // times
        name = name_of_this_element + "_left_time_" + char('0' + i) + "_raw" ;
        spec_left_time_raw[i] = new spectrum_1D(name,
                                                name,
                                                4096, 0, 4096,
                                                folder, "", name);
        frs_spectra_ptr->push_back(spec_left_time_raw[i]) ;


        name = name_of_this_element + "_right_time_" + char('0' + i) + "_raw" ;
        spec_right_time_raw[i] = new spectrum_1D(name,
                name,
                4096, 0, 4096,
                folder, "", name);
        frs_spectra_ptr->push_back(spec_right_time_raw[i]) ;

        // energies
        name = name_of_this_element + "_left_energy_" + char('0' + i) + "_raw" ;
        spec_left_energy_raw[i] = new spectrum_1D(name,
                name,
                4096, 0, 4096,
                folder, "", name);
        frs_spectra_ptr->push_back(spec_left_energy_raw[i]) ;


        name = name_of_this_element + "_right_energy_" + char('0' + i) + "_raw" ;
        spec_right_energy_raw[i] = new spectrum_1D(name,
                name,
                4096, 0, 4096,
                folder, "", name);
        frs_spectra_ptr->push_back(spec_right_energy_raw[i]) ;
    }



    // Note: mutliwires are 200 mm, but only 160 mm are active

    // X------------------------------------
    string explan = "arithmetic mean of positions: x0, x1 (if empty, check the GATEs on spectra: check_sums0-4) ";

    name = name_of_this_element + "_x"  ;
    spec_x = new spectrum_1D(name,
                             name,
                             400, -150, 150,
                             folder, explan, name_of_this_element + "_x_when_ok");
    frs_spectra_ptr->push_back(spec_x) ;

    //----------------
    explan = "(if empty, check the GATEs on spectra: check_sums 0, 1) ";
    name = name_of_this_element + "_x0"  ;
    spec_x0 = new spectrum_1D(name,
                              name,
                              400, -150, 150,
                              folder, explan, name_of_this_element + "_x_when_ok");
    frs_spectra_ptr->push_back(spec_x0) ;
    //----------------
    explan = "(if empty, check the GATEs on spectra: check_sums 2,3) ";
    name = name_of_this_element + "_x1"  ;
    spec_x1 = new spectrum_1D(name,
                              name,
                              400, -150, 150,
                              folder, explan, name_of_this_element + "_x_when_ok");
    frs_spectra_ptr->push_back(spec_x1) ;

    // Y------------------------------------
    name =  name_of_this_element + "_y"  ;
    explan = "(if empty,  check the GATEs on spectra: check_sums0-4) ";
    spec_y = new spectrum_1D(name,
                             name,
                             400, -150, 150,
                             folder,
                             "when the gate on sumY is passed",
                             name_of_this_element + "_y_when_ok");
    frs_spectra_ptr->push_back(spec_y) ;



    //--------------------------------------
    name =  name_of_this_element + "_xy"  ;

    spec_xy = new spectrum_2D(name,
                              name,
                              400, -200, 200,
                              400, -200, 200,
                              folder, "",
                              string("as X:") + name_of_this_element + "_y_when_ok" +
                              ", as Y: " + name_of_this_element + "_y_when_ok");

    frs_spectra_ptr->push_back(spec_xy) ;


}
//**********************************************************************
void Ttime_projection_chamber::analyse_current_event()
{
#define DBGT if(false)
    DBGT cout << "analyse_current_event()  for "
              <<  name_of_this_element
              << endl ;

    x0_minus_x1 = 0;
    nr_checksums_true = 0;

    deposit_energy = 1 ;
    deposit_energy_counter = 0 ;

    flag_fired_times = false;

    // raw spectra
    for(int i = 0 ; i < TPC_NR_OF_DRIFT_TIMES ; i++)
    {
        drift_time_raw[i] = (event_unpacked->*drift_time_array) [i] ;
        if(drift_time_raw[i])
        {
            spec_drift_time_raw[i]->manually_increment(drift_time_raw[i]) ;
            flag_fired_times = true;
        }
        drift_energy_raw[i] = (event_unpacked->*drift_energy_array) [i] ;
        if(drift_energy_raw[i])
        {
            spec_drift_energy_raw[i]->manually_increment(drift_energy_raw[i]) ;
            // calibration of the energies
            drift_energy_cal[i] =
                (drift_energy_slope[i] * (drift_energy_raw[i] + randomizer::randomek())) +
                drift_energy_offset[i];

            deposit_energy_counter++;
            deposit_energy *= drift_energy_cal[i];
        }
        else drift_energy_cal[i] = 0;
    }

    // kind of mean value of energy deposit
    if(deposit_energy_counter)
        deposit_energy = pow(deposit_energy, 1.0 / deposit_energy_counter);

    // left and right
    for(int i = 0 ; i < TPC_NR_OF_X_TIMES ; i++)
    {
        left_time_raw[i] = (event_unpacked->*left_time_array) [i] ;
        if(left_time_raw[i])
        {
            spec_left_time_raw[i]->manually_increment(left_time_raw[i]) ;
            flag_fired_times = true;
        }
        right_time_raw[i] = (event_unpacked->*right_time_array) [i] ;
        if(right_time_raw[i])
        {
            spec_right_time_raw[i]->manually_increment(right_time_raw[i]) ;
            flag_fired_times = true;
        }
        //same for energy

        left_energy_raw[i] = (event_unpacked->*left_energy_array) [i] ;
        if(left_energy_raw[i])spec_left_energy_raw[i]->manually_increment(left_energy_raw[i]) ;

        right_energy_raw[i] = (event_unpacked->*right_energy_array) [i] ;
        if(right_energy_raw[i])spec_right_energy_raw[i]->manually_increment(right_energy_raw[i]) ;

    }



    // initially
    x_ok = false ;
    y_ok = false ;
    x = -9999 ;
    y = -9999;


    //  double raw_y = 0;  // raw, not calibrated value of y
    int count = 0 ;
    int countx = 0;
    bool  flag_check_sum_ok[TPC_NR_OF_DRIFT_TIMES] = { false };

    // calculations for position ----------------------------------
    if(flag_fired_times)
    {

        y = 0;
        for(int an = 0 ; an < TPC_NR_OF_DRIFT_TIMES ; an++)
        {
            flag_check_sum_ok[an] = false;

            int which = (an < 2) ?  0 : 1;
            //       check_sum[an] = (left_time_raw[which]- drift_time_raw[an])
            //                       +
            //                       (right_time_raw[which] - drift_time_raw[an]) ;

            if(an < 2)
            {
                check_sum[an] = left_time_raw[0] + right_time_raw[0] - 2 * drift_time_raw[an] ;
            }
            else
            {
                check_sum[an] = left_time_raw[1] + right_time_raw[1] - 2 * drift_time_raw[an] ;
            }
            //   check_sum
            if(spec_check_sum[an])
            {
                spec_check_sum[an] ->manually_increment(check_sum[an]) ;
                // note, the max nr of TPC is strictly defined in the "calibated root tree" (now: 4)
                // so if in fact it is more of them, you will have the segmentation violation
                TIFJAnalysis::fxCaliEvent-> tpc_check_sum[my_tpc_det_nr][an] = check_sum[an];
            }

            DBGT cout << an << ")-------------- DT = " << drift_time_raw[an]
                      << " Checksum = " << check_sum[an]
                      << ",  left_time_raw[which] " << left_time_raw[which]
                      << ", right_time_raw[which] " << right_time_raw[which]
                      << ", which = "
                      << which << endl;

            if(check_sum[an] >= check_sum_gate[an][0]  && check_sum[an] <= check_sum_gate[an][1])
            {
                flag_check_sum_ok[an] = true;
                nr_checksums_true++;
            }

            if(! left_time_raw[which] &&  ! right_time_raw[which])
                flag_check_sum_ok[an] = false;

            DBGT cout << "       chks " << flag_check_sum_ok[an]  << endl ;
            if(flag_check_sum_ok[an]  && drift_time_raw[an])
            {
                double tmp = drift_time_raw[an]  + randomizer::randomek() ;
                y += (drift_time_slope[an] * tmp) + drift_time_offset[an];
                count++;

                DBGT  cout << " chks OK, so currently y = " << y << ", count = " << count << endl;
            }
        } // end of for drif times

        double x0 = 0, x1 = 0 ;
        x = 0;
        // calculation of the postion x0, x1

        if(flag_check_sum_ok[0] || flag_check_sum_ok[1])
        {
            double r_x0 =  left_time_raw[0]  - right_time_raw[0]  + randomizer::randomek() ;
            x0 = x = (leftRight_time_slope[0] * r_x0) + leftRight_time_offset[0];
            if(x0) spec_x0 ->manually_increment(x0) ;
            countx++;
        }

        if(flag_check_sum_ok[2] || flag_check_sum_ok[3])
        {
            double r_x1 =  left_time_raw[1] -  right_time_raw[1]  + randomizer::randomek() ;
            x += (x1 = (leftRight_time_slope[1] * r_x1) + leftRight_time_offset[1]);
            if(x1) spec_x1 ->manually_increment(x1) ;
            countx++;
        }

        // arithmetic mean

        if(countx && count)
        {
            y /= count;
            x /= countx;

            x_ok = true ;
            spec_x ->manually_increment(x) ;
            y_ok = true ;
            spec_y ->manually_increment(y) ;
            spec_xy->manually_increment(x, y);

            x0_minus_x1 = x0 - x1;

            /*
                  DBGT cout << name_of_this_element
                  << ", x = " << x
                  << ", y = " << y << endl ;

                  DBGT    if(x < -200)
                    cout << "NOnsense with x" << endl;

                  DBGT    if(y < -200)
                    cout << "NOnsense with y" << endl;
            */

            TIFJAnalysis::fxCaliEvent->tpc_x[my_tpc_det_nr] = x;
            TIFJAnalysis::fxCaliEvent->tpc_y[my_tpc_det_nr] = y;

        }
    } // if fired
    calculations_already_done = true ;
}
//**************************************************************************
void  Ttime_projection_chamber::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;

    // in this file we look for a string
    distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_distance");

    for(int i = 0 ; i < TPC_NR_OF_DRIFT_TIMES ; i++)
    {
        drift_time_offset[i] = Tfile_helper::find_in_file(s,  name_of_this_element
                               + "_drift_time_" + char('0' + i) + "_offset");
        drift_time_slope[i]  = Tfile_helper::find_in_file(s,  name_of_this_element
                               + "_drift_time_" + char('0' + i) + "_slope");

        drift_energy_offset[i] = Tfile_helper::find_in_file(s,  name_of_this_element
                                 + "_drift_energy_" + char('0' + i) + "_offset");
        drift_energy_slope[i]  = Tfile_helper::find_in_file(s,  name_of_this_element
                                 + "_drift_energy_" + char('0' + i) + "_slope");

    }

    for(int i = 0 ; i < TPC_NR_OF_X_TIMES ; i++)
    {
        leftRight_time_offset[i] = Tfile_helper::find_in_file(s,  name_of_this_element
                                   + "_leftRight_time_" + char('0' + i) + "_offset");
        leftRight_time_slope[i]  = Tfile_helper::find_in_file(s,  name_of_this_element
                                   + "_leftRight_time_" + char('0' + i) + "_slope");
    }



    // but this is in a different file....

    //     cout << " reading the gates for " << name_of_this_element << " sum's " << endl ;
    //-----------------
    // here we
    //string gates_file = "gates/time_projection_chamber_sum_gates.txt" ;

    // the new style, gates are created by my viewer, or manually
    // so that in one file there is only one gate, for this particullar object



    for(int i = 0 ; i < TPC_NR_OF_DRIFT_TIMES ; i++)
    {

        string core_of_the_name = "gates/" + name_of_this_element ;
        string nam_gate = core_of_the_name + "_check_sum_" + char('0' + i) + "_gate_GATE.gate" ;

        ifstream plik_gatesX(nam_gate.c_str());

        if(! plik_gatesX)
        {
            string mess = "I can't open  file: " + nam_gate
                          + "\nThis gate you can produce interactively using a 'cracow' viewer program" ;
            cout << mess
                 << "\nI create a gate and assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
                 << endl ;

            ofstream pliknew(nam_gate.c_str());
            pliknew   << check_sum_gate[i][0] << "   "    // lower limit
                      << check_sum_gate[i][1] << endl ;
        }
        else
        {
            plik_gatesX >> zjedz >> check_sum_gate[i][0]     // lower limit
                        >> zjedz >> check_sum_gate[i][1] ;   // upper limit

            if(! plik_gatesX)
            {
                string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
                exit(-1) ;
            }
            plik_gatesX.close() ;
        }
    } // end of for over 4 drift times

}

#endif // defined TPC41_PRESENT || defined TPC42_PRESENT

//***************************************************************

