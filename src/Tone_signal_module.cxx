#include "Tone_signal_module.h"
#include "Tnamed_pointer.h"

//***********************************************************************
//  constructor
template <class T>
Tone_signal_module<T>::Tone_signal_module(string _name,
                                       T *first_ptr,
                                       string description
                                      ) : Tfrs_element(_name),
    signal_ptr(first_ptr),
    signal_description(description)
{
    create_my_spectra();


//    named_pointer[name_of_this_element + "_" +  description + "_raw_(signed)"]
//    = Tnamed_pointer(&signed_signal_data, 0, this) ;
    named_pointer[name_of_this_element + "_" +  description + "_raw"]
    = Tnamed_pointer(&double_signal_data, 0, this) ;

}
//************************************************************
template <class T>
void Tone_signal_module<T>::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "tests/" + name_of_this_element ;

    //-----------
    string name = name_of_this_element + "_" + signal_description + "_raw"  ;
    spec_first = new spectrum_1D(name,
                                 5000, -1000, 4000,
                                 folder, "", name);
    frs_spectra_ptr->push_back(spec_first) ;

    //-----------

}
//*************************************************************
template <class T>
void Tone_signal_module<T>::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;

//    unsigned_signal_data = event_unpacked->*signal_ptr;
    T data = *signal_ptr;
    double_signal_data = (double)  *signal_ptr;
   // signed_signal_data = (T)unsigned_signal_data;

    if(data)
        spec_first ->manually_increment(data) ;

    calculations_already_done = true ;
}
//**************************************************************
// read the calibration factors, gates
template <class T>
void  Tone_signal_module<T>::make_preloop_action(ifstream & /*s*/)
{

}
//***************************************************************
