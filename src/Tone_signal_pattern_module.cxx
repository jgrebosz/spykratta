#include "Tone_signal_pattern_module.h"
#include "Tnamed_pointer.h"
#include <sstream>
#include <iomanip>


//***********************************************************************
//  constructor
Tone_signal_pattern_module::Tone_signal_pattern_module(string _name,
        int TIFJEvent::*first_ptr
                                                      ) : Tfrs_element(_name),
    signal_ptr(first_ptr)
{
    create_my_spectra();


    named_pointer[name_of_this_element + "_full_pattern"]
    = Tnamed_pointer(&signal_data, 0, this) ;

    for(int i = 0 ; i < 32 ; i++)
    {
        ostringstream s;
        s << name_of_this_element << "_bit_" << setw(2) << setfill('0') << i ;

        named_pointer[s.str() ] = Tnamed_pointer(&our_incrementer[i], 0, this) ;
    }
}
//************************************************************
void Tone_signal_pattern_module::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "tests/" + name_of_this_element ;


    //-----------
    string name = name_of_this_element + "_signal"  ;
    spec_first = new spectrum_1D(name,
                                 name,
                                 8192, 0, 8192,
                                 folder);
    frs_spectra_ptr->push_back(spec_first) ;

    //-----------

}
//*************************************************************
void Tone_signal_pattern_module::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element  << endl ;

    signal_data = event_unpacked->*signal_ptr;
    spec_first ->manually_increment(signal_data) ;

//cout << "signal for pattern = 0x "<< hex << signal_data << dec << endl;

    // now unpacking for incrementers
    for(int i = 0 ; i < 32 ; i++)
    {
        our_incrementer[i] = (signal_data & (1 << i)) ? true : false;

//     if(i == 15 && our_incrementer[i])
//     {
//       cout << "jedynka" << endl;
//     }
    }

    calculations_already_done = true ;

}
//**************************************************************
// read the calibration factors, gates
void  Tone_signal_pattern_module::make_preloop_action(ifstream & s)
{

}
//***************************************************************
