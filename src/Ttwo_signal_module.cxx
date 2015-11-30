#include "Ttwo_signal_module.h"
#include "Tnamed_pointer.h"

//***********************************************************************
//  constructor
Ttwo_signal_module::Ttwo_signal_module(string _name,
                                       int TIFJEvent::*first_ptr,
                                       int TIFJEvent::*second_ptr,
                                       string name_first,
                                       string name_second
                                      ) : Tfrs_element(_name),
    first_signal_ptr(first_ptr),
    second_signal_ptr(second_ptr), first_txt(name_first), second_txt(name_second)
{
    create_my_spectra();


    named_pointer[name_of_this_element + "_" + first_txt]
    = Tnamed_pointer(&first_signal_data, 0, this) ;
    named_pointer[name_of_this_element + "_" + second_txt]
    = Tnamed_pointer(&second_signal_data, 0, this) ;

}
//************************************************************
void Ttwo_signal_module::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "tests/" + name_of_this_element ;


    //-----------
    string name = name_of_this_element + "_" + first_txt + "_raw"  ;
    spec_first = new spectrum_1D(name,
                                 name,
                                 8191, 1, 8192,
                                 folder);
    frs_spectra_ptr->push_back(spec_first) ;

    //-----------
    name = name_of_this_element + "_" + second_txt  + "_raw";
    spec_second = new spectrum_1D(name,
                                  name,
                                  8191, 1, 8192,
                                  folder);
    frs_spectra_ptr->push_back(spec_second) ;


}
//*************************************************************
void Ttwo_signal_module::analyse_current_event()
{


    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;


    first_signal_data = event_unpacked->*first_signal_ptr;
    second_signal_data = event_unpacked->*second_signal_ptr;

    spec_first ->manually_increment(first_signal_data) ;
    spec_second ->manually_increment(second_signal_data) ;



    calculations_already_done = true ;

}
//**************************************************************
// read the calibration factors, gates
void  Ttwo_signal_module::make_preloop_action(ifstream & s)
{


}
//***************************************************************
