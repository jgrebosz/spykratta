#include "Tframe_simple.h"


//***********************************************************************
//  constructor
Tframe_simple::Tframe_simple(string _name,
                             int TIFJEvent::*signal_p)
    : Tfrs_element(_name), signal(signal_p)
{

    signal = 0 ; // null
    create_my_spectra();

}
//************************************************************************
void Tframe_simple::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "frame_simple/" + name_of_this_element ;


    //-----------
    string name = name_of_this_element + "_signal_raw"  ;
    spec_signal_raw = new spectrum_1D(name,
                                      1000, 0, 2000,
                                      folder);
    frs_spectra_ptr->push_back(spec_signal_raw) ;

}
//**********************************************************************
void Tframe_simple::analyse_current_event()
{

    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;

    spec_signal_raw ->manually_increment(event_unpacked->*signal) ;

#ifdef NIGDY
// this was for testing purposes
    static bool flag_first_time = true;

    if(flag_first_time)
    {
        for(int i = 0 ; i < 200 ; i++)
            spec_signal_raw ->manually_inc_by(i, i) ;

        flag_first_time = false;
    }

    spec_signal_raw ->manually_increment(100) ;

#endif // NIGDY
    calculations_already_done = true ;

}
//**************************************************************************
void  Tframe_simple::make_preloop_action(ifstream & /*s*/)  // read the calibration factors, gates
{

    cout << "Reading the calibration for " << name_of_this_element << endl ;


    // in this file we look for a string
    //distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_distance" );

}
//***************************************************************
