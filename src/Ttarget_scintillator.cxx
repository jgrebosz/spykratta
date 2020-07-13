#include "Ttarget_scintillator.h"

#include "Tnamed_pointer.h"

//***********************************************************************
//  constructor
Ttarget_scintillator::Ttarget_scintillator(string _name,
        int TIFJEvent::*x_p,
        int TIFJEvent::*y_p)
    : Tfrs_element(_name),
      x(x_p),
      y(y_p)
{

    x_p = y_p  = 0 ; // null
    create_my_spectra();


    named_pointer[name_of_this_element + "_x_raw"]
    = Tnamed_pointer(&x_value, 0, this) ;

    named_pointer[name_of_this_element + "_y_raw"]
    = Tnamed_pointer(&y_value, 0, this) ;


    // for rates spectra/raport
    named_pointer[name_of_this_element + "_x_raw"]
    = Tnamed_pointer(&x_value, &x_fired, this) ;

    named_pointer[name_of_this_element + "_y_raw"]
    = Tnamed_pointer(&y_value, &y_fired, this) ;

}
//************************************************************************
void Ttarget_scintillator::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "scintillators/"  ;


    //-----------
    string name = name_of_this_element + "_xy_fan"  ;
    spec_xy_fan = new spectrum_1D(name,
                                  name,
                                  4, 0, 3,
                                  folder);
    frs_spectra_ptr->push_back(spec_xy_fan) ;

}
//**********************************************************************
void Ttarget_scintillator::analyse_current_event()
{

    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;

    x_fired = false;
    y_fired = false ;

    x_value = event_unpacked->*x ;
    y_value = event_unpacked->*y ;

    if(x_value)
    {
        spec_xy_fan ->manually_increment(0) ;
        x_fired = true ;
    }
    if(y_value)
    {
        spec_xy_fan ->manually_increment(1) ;
        y_fired = true ;
    }

    if(x_value && y_value)
        spec_xy_fan ->manually_increment(3) ;


    calculations_already_done = true ;

}
//**************************************************************************
void  Ttarget_scintillator::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;


    // in this file we look for a string

    //distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_distance" );

}
//***************************************************************
