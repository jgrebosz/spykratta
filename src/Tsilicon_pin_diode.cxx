#include "Tsilicon_pin_diode.h"
#include "Tnamed_pointer.h"
#include "Tfile_helper.h"

//***********************************************************************
//  constructor
Tsilicon_pin_diode::Tsilicon_pin_diode(string _name,
                                       int TIFJEvent::*first_ptr
                                      ) : Tfrs_element(_name),
    signal_ptr(first_ptr)
{
    create_my_spectra();


    named_pointer[name_of_this_element + "_dE_raw"]
    = Tnamed_pointer(&dE_raw, &fired, this) ;

    named_pointer[name_of_this_element + "_dE_cal"]
    = Tnamed_pointer(&dE_cal,  &fired, this) ;

}
//************************************************************
void Tsilicon_pin_diode::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "tests/" + name_of_this_element ;


    //-----------
    string name = name_of_this_element + "_dE_raw"  ;
    spec_dE_raw = new spectrum_1D(name,
                                  name,
                                  8191, 1, 8192,
                                  folder);
    frs_spectra_ptr->push_back(spec_dE_raw) ;

    //-----------
    name = name_of_this_element + "_dE"  ;
    spec_dE_cal = new spectrum_1D(name,
                                  name,
                                  8191, 1, 8192,
                                  folder);
    frs_spectra_ptr->push_back(spec_dE_cal) ;

}
//*************************************************************
void Tsilicon_pin_diode::analyse_current_event()
{


    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;


    dE_raw = event_unpacked->*signal_ptr;

    if(dE_raw)
        fired = true ;
    else
        fired = false ;

    spec_dE_raw ->manually_increment(dE_raw) ;

    dE_cal = (slope * dE_raw) + offset ;
    spec_dE_cal ->manually_increment(dE_cal) ;

    calculations_already_done = true ;
}
//**************************************************************
// read the calibration factors, gates
void  Tsilicon_pin_diode::make_preloop_action(ifstream & s)
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;

    offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_offset");
    slope  = Tfile_helper::find_in_file(s,  name_of_this_element  + "_slope");

}
//***************************************************************
