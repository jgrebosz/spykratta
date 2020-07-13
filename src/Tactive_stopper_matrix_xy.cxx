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
#include "Tactive_stopper_matrix_xy.h"

#ifdef MUNICH_MATRIX_STRIPES_X

#include <sstream>
#include "Tnamed_pointer.h"
using namespace std;


#include "TIFJAnalysis.h" // for verbose

using namespace active_stopper_namespace;
// ******************************************************************
Tactive_stopper_matrix_xy::Tactive_stopper_matrix_xy(string _name,
        int TIFJEvent::*x_left_ptr,
        int TIFJEvent::*x_right_ptr,
        int TIFJEvent::*y_up_ptr,
        int TIFJEvent::*y_down_ptr
                                                    ) : Tfrs_element(_name),
    x_left(x_left_ptr),
    x_right(x_right_ptr),
    y_up(y_up_ptr),
    y_down(y_down_ptr)
{

    x_left_ptr = x_right_ptr = y_down_ptr = y_up_ptr = 0 ; // null

    create_my_spectra();


    named_pointer[name_of_this_element + "_x_when_ok"]
    = Tnamed_pointer(&x, &x_ok, this) ;
    named_pointer[name_of_this_element + "_y_when_ok"]
    = Tnamed_pointer(&y, &y_ok, this) ;

}
//************************************************************************
//************************************************************************
void Tactive_stopper_matrix_xy::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "active_stopper/" + name_of_this_element ;


    //-----------
    string name = name_of_this_element + "_left_raw"  ;
    spec_left = new spectrum_1D(name, name,
                                4095, 1, 4096,
                                folder, "", noraw);
    frs_spectra_ptr->push_back(spec_left) ;

    //-----------
    name = name_of_this_element + "_right_raw"  ;
    spec_right = new spectrum_1D(name, name,
                                 4095, 1, 4096,
                                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_right) ;

    //-----------

    name = name_of_this_element + "_up_raw"  ;
    spec_up = new spectrum_1D(name, name,
                              4095, 1, 4096,
                              folder, "", noraw);
    frs_spectra_ptr->push_back(spec_up) ;

    //-----------
    name = name_of_this_element + "_down_raw"  ;
    spec_down = new spectrum_1D(name, name,
                                4095, 1, 4096,
                                folder, "", noraw);
    frs_spectra_ptr->push_back(spec_down) ;




    // X------------------------------------
    name = name_of_this_element + "_x"  ;
    spec_x = new spectrum_1D(name,
                             name,
                             80, -10, 70,
                             folder, "when the gate on sumX is passed", name_of_this_element + "_x_when_ok");
    frs_spectra_ptr->push_back(spec_x) ;


    // X_ SUM------------------------------
    name =  name_of_this_element + "_sumX"  ;

    spec_x_sum = new spectrum_1D(name,
                                 name,
                                 16000, -8000, 8000,
                                 folder,
                                 "GATE is for position x", noinc);
    frs_spectra_ptr->push_back(spec_x_sum) ;
    // in case is the gate is not set yet
    x_sum_gate[0] = 0 ;    // lower limit
    x_sum_gate[1] = +8191;

    // Y------------------------------------
    name =  name_of_this_element + "_y"  ;
    spec_y = new spectrum_1D(name,
                             name,
                             80, -70, 10,
                             folder,
                             "when the gate on sumY is passed",
                             name_of_this_element + "_y_when_ok");
    frs_spectra_ptr->push_back(spec_y) ;


    // Y_SUM------------------------------
    name =  name_of_this_element + "_sumY"  ;

    spec_y_sum = new spectrum_1D(name,
                                 name,
                                 16000, -8000, 8000, //  8192, 0, 8192,
                                 folder,
                                 "GATE is for position y", noinc);

    frs_spectra_ptr->push_back(spec_y_sum) ;

    y_sum_gate[0] = 0 ;    // lower limit
    y_sum_gate[1] = +8191;

    //--------------------------------------
    name =  name_of_this_element + "_xy"  ;

    spec_xy = new spectrum_2D(name,
                              name,
                              400, -150, 150,
                              400, -150, 150,
                              folder, "",
                              string("as X:") + name_of_this_element + "_y_when_ok" +
                              ", as Y: " + name_of_this_element + "_y_when_ok");

    frs_spectra_ptr->push_back(spec_xy) ;


    //---------

    name = name_of_this_element + "_left_cal"  ;
    spec_left_cal = new spectrum_1D(name,
                                    name,
                                    1200, 0, 1200,
                                    folder, "", noinc);
    frs_spectra_ptr->push_back(spec_left_cal) ;

    //---------

    name = name_of_this_element + "_right_cal"  ;
    spec_right_cal = new spectrum_1D(name,
                                     name,
                                     1200, 0, 1200,
                                     folder, "", noinc);
    frs_spectra_ptr->push_back(spec_right_cal) ;

    //---------


    name = name_of_this_element + "_up_cal"  ;
    spec_up_cal = new spectrum_1D(name,
                                  name,
                                  1200, 0, 1200,
                                  folder, "", noinc);
    frs_spectra_ptr->push_back(spec_up_cal) ;

    //---------
    name = name_of_this_element + "_down_cal"  ;
    spec_down_cal = new spectrum_1D(name,
                                    name,
                                    1200, 0, 1200,
                                    folder, "", noinc);
    frs_spectra_ptr->push_back(spec_down_cal) ;
}
//**********************************************************************
void Tactive_stopper_matrix_xy::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;
    left_raw = event_unpacked->*x_left ;
    right_raw = event_unpacked->*x_right ;
    up_raw = event_unpacked->*y_up ;
    down_raw = event_unpacked->*y_down ;


    spec_left ->manually_increment(left_raw) ;
    spec_right ->manually_increment(right_raw) ;
    spec_up ->manually_increment(up_raw) ;
    spec_down ->manually_increment(down_raw) ;


    // initially
    x_ok = false ;
    y_ok = false ;
    x = -9999 ;
    y = -9999;
    x_sum = 0;
    y_sum = 0 ;


    // calculations for X position ----------------------------------
    if(left_raw > 0  &&  right_raw > 0)
    {
        // always or perhaps when bigger than zero ?
        left_cal  = (left_slope * (left_raw + randomizer::randomek())) + left_offset ;
        spec_left_cal ->manually_increment(left_cal) ;

        right_cal  = (right_slope * (right_raw + randomizer::randomek())) + right_offset ;
        spec_right_cal ->manually_increment(right_cal) ;

        //   x_sum
        x_sum = (left_raw)  + (right_raw) ;
        spec_x_sum ->manually_increment(x_sum) ;

        if(x_sum >= x_sum_gate[0]  && x_sum <= x_sum_gate[1])
        {
            x = (left_cal - right_cal) / (left_cal + right_cal) ;
            x_ok = true ;

            if(x_final_cal.size() >= 3) // different than zero
            {
                x = (x_final_cal[2] * x * x) + (x_final_cal[1] * x) + x_final_cal[0]  ;
            }

            spec_x ->manually_increment(x) ;

            //             cout << name_of_this_element  << ",  x = " << x
            //                 << ",       left_raw =" << left_raw
            //                 << ",  right_raw =" << right_raw
            //                 << ",  x_slope =" << x_slope
            //                 << ",  x_offset =" << x_offset
            //                 << endl;
        }
    } // endif sensible x data

    //========================================
    // calculations for Y position ----------------------------------
    //========================================
    if(up_raw > 0  &&  down_raw > 0)
    {
        // always or perhaps when bigger than zero ?
        up_cal  = (up_slope * (up_raw + randomizer::randomek())) + up_offset ;
        spec_up_cal ->manually_increment(up_cal) ;

        down_cal  = (down_slope * (down_raw + randomizer::randomek())) + down_offset ;
        spec_down_cal ->manually_increment(down_cal) ;

        //   x_sum
        y_sum = (up_raw)  + (down_raw) ;
        spec_y_sum ->manually_increment(y_sum) ;

        if(y_sum >= y_sum_gate[0]  && y_sum <= y_sum_gate[1])
        {
            y = (up_cal - down_cal) / (up_cal + down_cal) ;
            y_ok = true ;

            if(y_final_cal.size() >= 3) // different than zero
            {
                y = (y_final_cal[2] * y * y) + (y_final_cal[1] * y) + y_final_cal[0]  ;
            }
            spec_y ->manually_increment(y) ;

            //             cout << name_of_this_element  << ",  y = " << y
            //                 << ",       up_raw =" << up_raw
            //                 << ",  down_raw =" << down_raw
            //                 << ",  y_slope =" << y_slope
            //                 << ",  y_offset =" << y_offset
            //                 << endl;
        }
    } // endif sensible y data



    if(y_ok && x_ok)
    {
        spec_xy->manually_increment(x, y);

    }

    calculations_already_done = true ;

}
//**************************************************************************
void  Tactive_stopper_matrix_xy::make_preloop_action(ifstream & ss)  // read the calibration factors, gates
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;
    x_final_cal.clear();
    y_final_cal.clear();

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
        // in this file we look for a string

        //   distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_distance" );

        left_offset = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_left_offset");
        left_slope  = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_left_slope");

        right_offset = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_right_offset");
        right_slope  = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_right_slope");

        up_offset = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_up_offset");
        up_slope  = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_up_slope");

        down_offset = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_down_offset");
        down_slope  = Tfile_helper::find_in_file(plik,  name_of_this_element  + "_down_slope");


        double tmp =
            Tfile_helper::find_in_file(
                plik,
                name_of_this_element + "_final_x_calibration_factors");
        x_final_cal.push_back(tmp);  // ofset

        plik >> zjedz >> tmp;
        x_final_cal.push_back(tmp);  // slope

        plik >> zjedz >> tmp;
        x_final_cal.push_back(tmp);  // quadratic

        tmp =
            Tfile_helper::find_in_file(
                plik,
                name_of_this_element + "_final_y_calibration_factors");
        y_final_cal.push_back(tmp);  // ofset

        plik >> zjedz >> tmp;
        y_final_cal.push_back(tmp);  // slope

        plik >> zjedz >> tmp;
        y_final_cal.push_back(tmp);  // quadratic

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



    // but this is in a different file....

    //     cout << " reading the gates for " << name_of_this_element << " sum's " << endl ;

    //-----------------

    // here we
    //string gates_file = "gates/multiwire_sum_gates.txt" ;

    // the new style, gates are created by my viewer, or manually
    // so that in one file there is only one gate, for this particullar object

    string core_of_the_name = "gates/" + name_of_this_element ;
    string nam_gate = core_of_the_name + "_sumX_gate_GATE.gate" ;

    ifstream plik_gatesX(nam_gate.c_str());

    if(! plik_gatesX)
    {
        string mess = "I can't open  file: " + nam_gate
                      + "\nThis gate you can produce interactively using a 'cracow' viewer program" ;
        cout << mess
             << "\nI create a gate and assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
             << endl ;

        ofstream pliknew(nam_gate.c_str());
        pliknew   << x_sum_gate[0] << "   "    // lower limit
                  << x_sum_gate[1] << endl ;
    }
    else
    {

        plik_gatesX >> zjedz >> x_sum_gate[0]     // lower limit
                    >> zjedz >> x_sum_gate[1] ;   // upper limit

        if(! plik_gatesX)
        {
            string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
            exit(-1) ;
        }
        plik_gatesX.close() ;
    }
    // y ----------


    nam_gate = core_of_the_name + "_sumY_gate_GATE.gate" ;

    ifstream plik_gatesY(nam_gate.c_str());
    if(! plik_gatesY)
    {
        string mess = "I can't open  file: " + nam_gate
                      + "\nThis gate you can produce interactively using a 'cracow' viewer program" ;

        cout << mess
             << "\nI assmue that the gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
             << endl ;

        ofstream pliknew(nam_gate.c_str());
        pliknew   << y_sum_gate[0] << "   "    // lower limit
                  << y_sum_gate[1] << endl ;

    }
    else
    {

        plik_gatesY >> zjedz >> y_sum_gate[0]     // lower limit
                    >> zjedz >> y_sum_gate[1] ;   // upper limit

        if(! plik_gatesY)
        {
            string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
            exit(-1) ;
        }
        plik_gatesY.close() ;

    }


}
//***************************************************************


#endif // #ifdef POSITRON_ABSORBER_PRESENT
