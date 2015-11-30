#include "Tmicro_channel_plate.h"
#include "Tnamed_pointer.h"
//***********************************************************************
//  constructor
Tmicro_channel_plate::Tmicro_channel_plate(string _name,
        int TIFJEvent::*x_ptr,
        int TIFJEvent::*y_ptr
                                          ) : Tfrs_element(_name),
    x_event(x_ptr),
    y_event(y_ptr)
{
    create_my_spectra();

    named_pointer[name_of_this_element + "_x_when_ok"]
    = Tnamed_pointer(&x, &x_ok, this) ;
    named_pointer[name_of_this_element + "_y_when_ok"]
    = Tnamed_pointer(&y, &y_ok, this) ;
}
//************************************************************************
//************************************************************************
void Tmicro_channel_plate::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "micro_channel_plate/" + name_of_this_element ;

    //-----------
    string name = name_of_this_element + "_x_raw"  ;
    spec_x_raw = new spectrum_1D(name, name,
                                 4095, 1, 4096,
                                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_x_raw) ;

    //-----------
    name = name_of_this_element + "_y_raw"  ;
    spec_y_raw = new spectrum_1D(name, name,
                                 4095, 1, 4096,
                                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_y_raw) ;

    //-----------

    // Note: mutliwires are 200 mm, but only 160 mm are active


    // X------------------------------------
    name = name_of_this_element + "_x_cal"  ;
    spec_x_cal = new spectrum_1D(name,
                                 name,
                                 400, -150, 150,
                                 folder,
                                 "in milimeters",
                                 name_of_this_element + "_x_when_ok");
    frs_spectra_ptr->push_back(spec_x_cal) ;

    // Y------------------------------------
    name =  name_of_this_element + "_y_cal"  ;
    spec_y_cal = new spectrum_1D(name,
                                 name,
                                 400, -150, 150,
                                 folder,
                                 "in milimeters",
                                 name_of_this_element + "_y_when_ok");
    frs_spectra_ptr->push_back(spec_y_cal) ;

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

}
//**********************************************************************
void Tmicro_channel_plate::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;

    x_raw = event_unpacked->*x_event ;
    y_raw = event_unpacked->*y_event ;

    if(x_raw)    spec_x_raw ->manually_increment(x_raw) ;
    if(y_raw)    spec_y_raw ->manually_increment(y_raw) ;

    // initially
    x_ok = false ;
    y_ok = false ;
    x = -9999 ;
    y = -9999;

    // calculations for X position ----------------------------------
    if(x_raw > 0)
    {
        x = (x_slope * ((x_raw)  + randomizer::randomek())) + x_offset ;
        x_ok = true ;
        spec_x_cal ->manually_increment(x) ;
        //             cout << name_of_this_element  << ",  x = " << x
        //                 << ",       x_raw =" << x_raw
        //                 << endl;
    } // endif sensible x data

    //========================================
    // calculations for Y position ----------------------------------
    //========================================
    if(y_raw > 0)
    {
        y = (y_slope * ((y_raw)  + randomizer::randomek())) + y_offset ;
        y_ok = true ;
        spec_y_cal ->manually_increment(y) ;
        //             cout << name_of_this_element  << ",  y = " << y
        //                 << ",       y_raw =" << y_raw
        //                 << endl;
    } // endif sensible x data

    if(y_ok && x_ok)
    {
        spec_xy->manually_increment(x, y);
    }
    calculations_already_done = true ;
}
//**************************************************************************
void  Tmicro_channel_plate::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;
    // in this file we look for a string
    distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_distance");

    x_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_x_offset");
    x_slope  = Tfile_helper::find_in_file(s,  name_of_this_element  + "_x_slope");
    y_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_y_offset");
    y_slope  = Tfile_helper::find_in_file(s,  name_of_this_element  + "_y_slope");
}
//***************************************************************

