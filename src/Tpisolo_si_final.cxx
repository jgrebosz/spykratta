#include "Tpisolo_si_final.h"
#include "Tnamed_pointer.h"
//***********************************************************************
//  constructor
Tpisolo_si_final::Tpisolo_si_final(
    string _name,        int TIFJEvent::*energy_ptr
) : Tfrs_element(_name),
    pisolo_si_energy(energy_ptr)

{
    create_my_spectra();

    named_pointer[name_of_this_element + "_energy_raw"]
    = Tnamed_pointer(&energy_raw, 0, this) ;

    named_pointer[name_of_this_element + "_energy_cal"]
    = Tnamed_pointer(&energy_cal, 0, this) ;

//   named_pointer[name_of_this_element+"_y_when_ok"]
//   = Tnamed_pointer(&y, &y_ok, this) ;
//
}
//************************************************************************
//************************************************************************
void Tpisolo_si_final::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "micro_channel_plate/" + name_of_this_element ;

    //-----------
    string name = name_of_this_element + "_energy_raw"  ;
    spec_e_raw = new spectrum_1D(name, name,
                                 4095, 1, 4096,
                                 folder, "", noraw);
    frs_spectra_ptr->push_back(spec_e_raw) ;


    name = name_of_this_element + "_energy_cal"  ;
    spec_e_cal = new spectrum_1D(name,
                                 name,
                                 4095, 1, 4096,
                                 folder,
                                 "calibrated",
                                 name_of_this_element + "_energy_cal");
    frs_spectra_ptr->push_back(spec_e_cal) ;

}
//**********************************************************************
void Tpisolo_si_final::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;

    energy_raw = event_unpacked->*pisolo_si_energy ;
    energy_cal  = 0 ;
    energy_ok = false;

    if(energy_raw)
    {
        spec_e_raw ->manually_increment(energy_raw) ;

        energy_cal = (slope * ((energy_raw)  + randomizer::randomek())) + offset ;
        energy_ok = true ;
        spec_e_cal ->manually_increment(energy_cal) ;
        //             cout << name_of_this_element  << ",  x = " << x
        //                 << ",       x_raw =" << x_raw
        //                 << endl;
    } // endif sensible x data

    calculations_already_done = true ;
}
//**************************************************************************
void  Tpisolo_si_final::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;
    // in this file we look for a string

    offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_offset");
    slope  = Tfile_helper::find_in_file(s,  name_of_this_element  + "_slope");

}
//***************************************************************

