#include "Tone_signal_module.h"
#include "Tnamed_pointer.h"

//***********************************************************************
//  constructor
Tone_signal_module::Tone_signal_module(string _name,
                                       int TIFJEvent::*first_ptr,
                                       string description
                                      ) : Tfrs_element(_name),
    signal_ptr(first_ptr),
    signal_description(description)
{
    create_my_spectra();


    named_pointer[name_of_this_element + "_" +  description + "_raw_(signed)"]
    = Tnamed_pointer(&signed_signal_data, 0, this) ;

}
//************************************************************
void Tone_signal_module::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "tests/" + name_of_this_element ;


    //-----------
    string name = name_of_this_element + "_" + signal_description + "_raw"  ;
    spec_first = new spectrum_1D(name,
                                 name,
                                 8192, 0, 8192,
                                 folder);
    frs_spectra_ptr->push_back(spec_first) ;

    //-----------

}
//*************************************************************
void Tone_signal_module::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;

    unsigned_signal_data = event_unpacked->*signal_ptr;
    signed_signal_data = (int)unsigned_signal_data;

    if(signed_signal_data)
        spec_first ->manually_increment(signed_signal_data) ;

    calculations_already_done = true ;
}
//**************************************************************
// read the calibration factors, gates
void  Tone_signal_module::make_preloop_action(ifstream & s)
{

}
//***************************************************************
