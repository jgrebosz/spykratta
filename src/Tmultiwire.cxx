#include "Tmultiwire.h"

#include "Tnamed_pointer.h"

//***********************************************************************
//  constructor
Tmultiwire::Tmultiwire(string _name,
                       int TIFJEvent::*x_left_ptr,
                       int TIFJEvent::*x_right_ptr,
                       int TIFJEvent::*y_up_ptr,
                       int TIFJEvent::*y_down_ptr,
                       int TIFJEvent::*anode_ptr
                      ) : Tfrs_element(_name),
    x_left(x_left_ptr),
    x_right(x_right_ptr),
    y_up(y_up_ptr),
    y_down(y_down_ptr),
    anode(anode_ptr)

{


    x_left_ptr = x_right_ptr = y_down_ptr = y_up_ptr = 0 ; // null

    create_my_spectra();


    named_pointer[name_of_this_element + "_x_when_ok"]
    = Tnamed_pointer(&x, &x_ok, this) ;
    named_pointer[name_of_this_element + "_y_when_ok"]
    = Tnamed_pointer(&y, &y_ok, this) ;


    // for rates - purposes
    named_pointer[name_of_this_element + "_left_raw_fired"]
    = Tnamed_pointer(&left_raw, &left_raw_fired, this) ;

    named_pointer[name_of_this_element + "_right_raw_fired"]
    = Tnamed_pointer(&right_raw, &right_raw_fired, this) ;

    named_pointer[name_of_this_element + "_up_raw_fired"]
    = Tnamed_pointer(&up_raw, &up_raw_fired, this) ;

    named_pointer[name_of_this_element + "_down_raw_fired"]
    = Tnamed_pointer(&down_raw, &down_raw_fired, this) ;

    named_pointer[name_of_this_element + "_anode_raw_fired"]
    = Tnamed_pointer(&anode_raw, &anode_raw_fired, this) ;



}
//************************************************************************
//************************************************************************
void Tmultiwire::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "multiwire/" + name_of_this_element ;


    //-----------
    string name = name_of_this_element + "_left_raw"  ;
    spec_left = new spectrum_1D(name,
                                4095, 1, 4096,
                                folder, "", noraw);
    frs_spectra_ptr->push_back(spec_left) ;

    //-----------
    name = name_of_this_element + "_right_raw"  ;
    spec_right = new spectrum_1D(name,
                                 4095, 1, 4096,
                                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_right) ;

    //-----------

    name = name_of_this_element + "_up_raw"  ;
    spec_up = new spectrum_1D(name,
                              4095, 1, 4096,
                              folder, "", noraw);
    frs_spectra_ptr->push_back(spec_up) ;

    //-----------
    name = name_of_this_element + "_down_raw"  ;
    spec_down = new spectrum_1D(name,
                                4095, 1, 4096,
                                folder, "", noraw);
    frs_spectra_ptr->push_back(spec_down) ;


    //-----------
    name = name_of_this_element + "_anode_raw"  ;
    spec_anode = new spectrum_1D(name,
                                 4095, 1, 4096,
                                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_anode) ;



    // Note: mutliwires are 200 mm, but only 160 mm are active


    // X------------------------------------
    name = name_of_this_element + "_posX"  ;
    spec_x = new spectrum_1D(name,
                             400, -150, 150,
                             folder, "when the gate on sumX is passed", name_of_this_element + "_x_when_ok");
    frs_spectra_ptr->push_back(spec_x) ;


    // X_ SUM------------------------------
    name =  name_of_this_element + "_sumX"  ;

    spec_x_sum = new spectrum_1D(name,
                                 16000, -8000, 8000,
                                 folder,
                                 "GATE is for posX", noinc);
    frs_spectra_ptr->push_back(spec_x_sum) ;
    // in case is the gate is not set yet
    x_sum_gate[0] = 0 ;    // lower limit
    x_sum_gate[1] = +8191;

    // Y------------------------------------
    name =  name_of_this_element + "_posY"  ;
    spec_y = new spectrum_1D(name,
                             400, -150, 150,
                             folder,
                             "when the gate on sumY is passed",
                             name_of_this_element + "_y_when_ok");
    frs_spectra_ptr->push_back(spec_y) ;


    // Y_SUM------------------------------
    name =  name_of_this_element + "_sumY"  ;

    spec_y_sum = new spectrum_1D(name,
                                 16000, -8000, 8000, //    8192, 0, 8192,
                                 folder,
                                 "GATE is for posY", noinc);

    frs_spectra_ptr->push_back(spec_y_sum) ;

    y_sum_gate[0] = 0 ;    // lower limit
    y_sum_gate[1] = +8191;

    //--------------------------------------
    name =  name_of_this_element + "_xy"  ;

    spec_xy = new spectrum_2D(name,
                              400, -150, 150,
                              400, -150, 150,
                              folder, "",
                              string("as X:") + name_of_this_element + "_y_when_ok" +
                              ", as Y: " + name_of_this_element + "_y_when_ok");

    frs_spectra_ptr->push_back(spec_xy) ;


#ifdef ADRIANNA_ALGORITHM_NEEDED
    // for adriana tdc algorithm


    name = name_of_this_element + "_x_sum_ns"  ;
    spec_x_sum_ns = new spectrum_1D(name,
                                    2400, 0, 2400,
                                    folder, "", noinc);
    frs_spectra_ptr->push_back(spec_x_sum_ns) ;

    //---------

    name = name_of_this_element + "_y_sum_ns"  ;
    spec_y_sum_ns = new spectrum_1D(name,
                                    2400, 0, 2400,
                                    folder, "", noinc);
    frs_spectra_ptr->push_back(spec_y_sum_ns) ;

    //---------

    name = name_of_this_element + "_anode_ns"  ;
    spec_anode_cal = new spectrum_1D(name,
                                     1200, 0, 1200,
                                     folder, "", noinc);
    frs_spectra_ptr->push_back(spec_anode_cal) ;

    //---------

    name = name_of_this_element + "_left_ns"  ;
    spec_left_cal = new spectrum_1D(name,
                                    1200, 0, 1200,
                                    folder, "", noinc);
    frs_spectra_ptr->push_back(spec_left_cal) ;

    //---------

    name = name_of_this_element + "_right_ns"  ;
    spec_right_cal = new spectrum_1D(name,
                                     1200, 0, 1200,
                                     folder, "", noinc);
    frs_spectra_ptr->push_back(spec_right_cal) ;

    //---------


    name = name_of_this_element + "_up_ns"  ;
    spec_up_cal = new spectrum_1D(name,
                                  0, 1200,
                                  folder, "", noinc);
    frs_spectra_ptr->push_back(spec_up_cal) ;

    //---------



    name = name_of_this_element + "_down_ns"  ;
    spec_down_cal = new spectrum_1D(name,
                                    1200, 0, 1200,
                                    folder, "", noinc);
    frs_spectra_ptr->push_back(spec_down_cal) ;

    //---------

    name = name_of_this_element + "_x_mm_calibrated"  ;
    spec_x_mm = new spectrum_1D(name,
                                300, -150, 150,
                                folder, "", noinc);
    frs_spectra_ptr->push_back(spec_x_mm) ;

    //---------


    name = name_of_this_element + "_y_mm_calibrated"  ;
    spec_y_mm = new spectrum_1D(name,
                                300, -150, 150,
                                folder, "", noinc);
    frs_spectra_ptr->push_back(spec_y_mm) ;

    //---------
    name =   name_of_this_element + "_xy_mm"  ;

    spec_xy_mm = new spectrum_2D(name,
                                 300, -150, 150,
                                 300, -150, 150,
                                 folder, "", noinc);

    frs_spectra_ptr->push_back(spec_xy_mm) ;

#endif   // ADRIANNA_ALGORITHM
}
//**********************************************************************
void Tmultiwire::analyse_current_event()
{


    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;



    spec_left ->manually_increment(event_unpacked->*x_left) ;
    spec_right ->manually_increment(event_unpacked->*x_right) ;
    spec_anode ->manually_increment(event_unpacked->*anode) ;
    spec_up ->manually_increment(event_unpacked->*y_up) ;
    spec_down ->manually_increment(event_unpacked->*y_down) ;


    // initially
    x_ok = false ;
    y_ok = false ;
    x = -9999 ;
    y = -9999;
    x_sum = 0;
    y_sum = 0 ;

    x_mm = -9999;
    y_mm = -9999;

    left_raw = event_unpacked->*x_left ;
    right_raw = event_unpacked->*x_right ;
    up_raw = event_unpacked->*y_up ;
    down_raw = event_unpacked->*y_down ;
    anode_raw = event_unpacked->*anode ;

    // for rates spectra purposes ------------------------------
    left_raw_fired =  right_raw_fired = up_raw_fired =
                                            down_raw_fired = anode_raw_fired  = false ;

    if(left_raw > 0)
        left_raw_fired = true;
    if(right_raw > 0)
        right_raw_fired = true;
    if(up_raw > 0)
        up_raw_fired = true;
    if(down_raw > 0)
        down_raw_fired = true;
    if(anode_raw > 0)
        anode_raw_fired = true;



    // calculations for X position ----------------------------------
    if(left_raw > 0  &&  right_raw > 0)
    {
        // always or perhaps when bigger than zero ?

        x_sum = (left_raw - anode_raw)
                +
                (right_raw - anode_raw) ;

        //    cout
        //         << name_of_this_element << " .x = " << x
        //             << ", x_sum = " << x_sum << endl ;


        //   x_sum
        spec_x_sum ->manually_increment(x_sum) ;


        if(x_sum >= x_sum_gate[0]  && x_sum <= x_sum_gate[1])
        {
            double rx = (left_raw - right_raw)
                        + randomizer::randomek()  ;

            x = (x_slope * rx) + x_offset ;


            x_ok = true ;
            spec_x ->manually_increment(x) ;

//             cout << name_of_this_element  << ",  x = " << x
//                 << ",       left_raw =" << left_raw
//                 << ",  right_raw =" << right_raw
//                 << ",  x_slope =" << x_slope
//                 << ",  x_offset =" << x_offset
//                 << endl;
        }


        //================= Adriana tdc algorithm =====================
#ifdef ADRIANNA_ALGORITHM_NEEDED
        if(use_tdc_algorithm)
        {

            anode_cal = (anode_raw) * anode_factors_slope + anode_factors_offset ;
            left_cal = (left_raw) * cathode_x_left_factors_slope + cathode_x_left_factors_offset ;
            right_cal = (right_raw) * cathode_x_right_factors_slope + cathode_x_right_factors_offset ;

            spec_anode_cal ->manually_increment(anode_cal) ;
            spec_left_cal ->manually_increment(left_cal) ;
            spec_right_cal ->manually_increment(right_cal) ;


#ifdef NIGDY
            x_sum_ns = (left_cal - anode_cal) + (right_cal - anode_cal)
                       - pulser_delay_left_anode - pulser_delay_right_anode - 4;


            spec_x_sum_ns ->manually_increment(x_sum_ns) ;


            left_cal_right_cal_diff_ns = left_cal - right_cal - pulser_delay_left_right  ;

            x_mm = ((x_factor_ns_to_mm * left_cal_right_cal_diff_ns) / 2.0)  + x_offset_ns_to_mm ;
#else

            double offset = 0;
            if(name_of_this_element == "mw41")  offset = 2.7;
            if(name_of_this_element == "mw42")  offset = -4.8;

            double tmpy = (right_cal - left_cal  + pulser_delay_left_right) / 2.0;
            x_mm = tmpy * x_factor_ns_to_mm + x_offset_ns_to_mm + offset;
#endif
            x_ok = true ; // there is no gate in this algorithm, so always ok?

            spec_x_mm ->manually_increment(x_mm) ;
            cout <<  name_of_this_element  << ", ------->  x_mm = " << x_mm  << endl;
        } // endif use tdc algorithm
#endif // ADRIANNA_ALGORITHM

    } // endif sensible x data

    //========================================
    // calculations for Y position ----------------------------------
    //========================================
    if(up_raw > 0  &&  down_raw > 0)
    {

        y_sum = (up_raw - anode_raw)
                +
                (down_raw - anode_raw);

        spec_y_sum ->manually_increment(y_sum) ;

        if(y_sum >= y_sum_gate[0]  && y_sum <= y_sum_gate[1])
        {
            double ry = (down_raw - up_raw) + randomizer::randomek()  ;
            y = (y_slope * ry) + y_offset ;
            y_ok = true ;

            //    cout << "mw["
            //         << name_of_mw
            //         << "].y = " << y
            //         << ", y_sum = " << y_sum << endl ;


            // now we could pack ther result y to the calibrated event

            // here we could increment the spectra
            spec_y ->manually_increment(y) ;
        }

#ifdef ADRIANNA_ALGORITHM_NEEDED
        //================= Adriana tdc algorithm =====================

        if(use_tdc_algorithm)
        {


            down_cal = (down_raw) * cathode_y_down_factors_slope + cathode_y_down_factors_offset ;
            up_cal = (up_raw) * cathode_y_up_factors_slope + cathode_y_up_factors_offset ;


            spec_down_cal ->manually_increment(down_cal) ;
            spec_up_cal ->manually_increment(up_cal) ;


            y_sum_ns = (down_cal - anode_cal)
                       +
                       (up_cal - anode_cal)
                       - pulser_delay_down_anode - pulser_delay_up_anode - 4;


            spec_y_sum_ns ->manually_increment(y_sum_ns) ;



            down_cal_up_cal_diff_ns = down_cal - up_cal  - pulser_delay_down_up ;

            y_mm = (y_factor_ns_to_mm * down_cal_up_cal_diff_ns / 2.0)  + y_offset_ns_to_mm ;
            spec_y_mm ->manually_increment(y_mm) ;
            y_ok = true ; // there is no gate in this algorithm, so this is always true
            spec_xy_mm ->manually_increment(x_mm, y_mm) ;



        } // endif use tdc algorithm
#endif  // ADRIANNA_ALGORITHM

    } // endif sensible y data



    if(y_ok && x_ok)
    {
        spec_xy->manually_increment(x, y);

    }

    calculations_already_done = true ;

}
//**************************************************************************
void  Tmultiwire::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;


    // in this file we look for a string

    distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_distance");

    x_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "x_offset");
    x_slope  = Tfile_helper::find_in_file(s,  name_of_this_element  + "x_slope");
    y_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "y_offset");
    y_slope  = Tfile_helper::find_in_file(s,  name_of_this_element  + "y_slope");


    // for the new, adriana algoritm
    use_tdc_algorithm = false ;
#ifdef ADRIANNA_ALGORITHM_NEEDED

    try    // this may be not defined for classical multiwire
    {
        use_tdc_algorithm = Tfile_helper::find_in_file(s,  name_of_this_element  + "_use_tdc_algorithm");
    }
    catch(Tno_keyword_exception & m)
    {
        cout << "Reading the calibration for frs...\n" << m.message
             <<   "\n As the keyword "
             << (name_of_this_element  + "_use_tdc_algorithm")
             << " is not defined in the calibration file,  the program assumes that "
             << name_of_this_element << " is the classical multiwire " << endl;

        // here we should repair the stopped stream (eof and failbit)

        // cout <<  "before clearing status word = " << hex << s.rdstate() << dec << endl ;

        s.clear(s.rdstate() & ~(ios::failbit | ios::eofbit));      // turn off failbit & eofbit leaving others intact:
        //cout <<  "Now status word = " << hex << s.rdstate() << dec << endl ;
    }

    if(use_tdc_algorithm)
    {

        // electrodes
        anode_factors_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_anode_factors_offset");
        anode_factors_slope = Tfile_helper::find_in_file(s,  name_of_this_element  + "_anode_factors_slope");


        cathode_x_right_factors_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_x_right_factors_offset");
        cathode_x_right_factors_slope = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_x_right_factors_slope");

        cathode_x_left_factors_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_x_left_factors_offset");
        cathode_x_left_factors_slope = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_x_left_factors_slope");


        cathode_y_up_factors_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_y_up_factors_offset");
        cathode_y_up_factors_slope = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_y_up_factors_slope");

        cathode_y_down_factors_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_y_down_factors_offset");
        cathode_y_down_factors_slope = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_y_down_factors_slope");

        // pulser delays
        pulser_delay_left_anode = Tfile_helper::find_in_file(s,  name_of_this_element  + "_pulser_delay_left_anode");
        pulser_delay_right_anode = Tfile_helper::find_in_file(s,  name_of_this_element  + "_pulser_delay_right_anode");
        pulser_delay_up_anode = Tfile_helper::find_in_file(s,  name_of_this_element  + "_pulser_delay_up_anode");
        pulser_delay_down_anode = Tfile_helper::find_in_file(s,  name_of_this_element  + "_pulser_delay_down_anode");

        pulser_delay_left_right = Tfile_helper::find_in_file(s,  name_of_this_element  + "_pulser_delay_left_right");
        pulser_delay_down_up = Tfile_helper::find_in_file(s,  name_of_this_element  + "_pulser_delay_down_up");

        // nanoseconds to milimeters
        x_factor_ns_to_mm = Tfile_helper::find_in_file(s,  name_of_this_element  + "_x_factor_ns_to_mm");
        x_offset_ns_to_mm = Tfile_helper::find_in_file(s,  name_of_this_element  + "_x_offset_ns_to_mm");

        y_factor_ns_to_mm = Tfile_helper::find_in_file(s,  name_of_this_element  + "_y_factor_ns_to_mm");
        y_offset_ns_to_mm = Tfile_helper::find_in_file(s,  name_of_this_element  + "_y_offset_ns_to_mm");

    }

#endif // ADRIANNA_ALGORITHM_NEEDED



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

