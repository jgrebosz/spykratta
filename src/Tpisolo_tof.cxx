#include "Tpisolo_tof.h"
#include "Tnamed_pointer.h"
//***********************************************************************
//  constructor
Tpisolo_tof::Tpisolo_tof(string _name,
                         int TIFJEvent::*tof1, int TIFJEvent::*tof2, int TIFJEvent::*tof3)
    : Tfrs_element(_name),
      tof1_event(tof1),
      tof2_event(tof2),
      tof3_event(tof3)
{
    create_my_spectra();

    named_pointer[name_of_this_element + "_tof1_raw"]
    = Tnamed_pointer(&tof1_raw, 0, this) ;

    named_pointer[name_of_this_element + "_tof2_raw"]
    = Tnamed_pointer(&tof2_raw, 0, this) ;

    named_pointer[name_of_this_element + "_tof3_raw"]
    = Tnamed_pointer(&tof3_raw, 0, this) ;
}
//************************************************************************
//************************************************************************
void Tpisolo_tof::create_my_spectra()
{
    string nocal = "Please report this missing incrementer" ;
    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "pisolo_tof/" + name_of_this_element ;

    //-----------
    string name = name_of_this_element + "_tof1_raw";
    spec_tof1_raw = new spectrum_1D(name,
                                    8191, 1, 8192,
                                    folder, "", name_of_this_element + "_tof1_raw");
    frs_spectra_ptr->push_back(spec_tof1_raw) ;

    //-----------
    name = name_of_this_element + "_tof2_raw";
    spec_tof2_raw = new spectrum_1D(name,
                                    8191, 1, 8192,
                                    folder, "", name_of_this_element + "_tof2_raw");
    frs_spectra_ptr->push_back(spec_tof2_raw) ;
    //-----------
    name = name_of_this_element + "_tof3_raw";
    spec_tof3_raw = new spectrum_1D(name,
                                    8191, 1, 8192,
                                    folder, "", name_of_this_element + "_tof3_raw");
    frs_spectra_ptr->push_back(spec_tof3_raw) ;

    // ==== cal =====
    //-----------
    name = name_of_this_element + "_tof1_cal";
    spec_tof1_cal = new spectrum_1D(name,
                                    8191, 1, 8192,
                                    folder, "", name_of_this_element + "_tof1_cal");
    frs_spectra_ptr->push_back(spec_tof1_cal) ;

    //-----------
    name = name_of_this_element + "_tof2_cal";
    spec_tof2_cal = new spectrum_1D(name,
                                    8191, 1, 8192,
                                    folder, "", name_of_this_element + "_tof2_cal");
    frs_spectra_ptr->push_back(spec_tof2_cal) ;
    //-----------
    name = name_of_this_element + "_tof3_cal";
    spec_tof3_cal = new spectrum_1D(name,
                                    8191, 1, 8192,
                                    folder, "", name_of_this_element + "_tof3_cal");
    frs_spectra_ptr->push_back(spec_tof3_cal) ;

    //-----------

    //--------------------------------------
    /*
    name =  name_of_this_element + "_xy"  ;

      spec_xy = new spectrum_2D( name,
                                 400, -150, 150,
                                 400, -150, 150,
                                 folder, "",
                                 string("as X:") + name_of_this_element +"_y_when_ok" +
                                 ", as Y: " + name_of_this_element+"_y_when_ok");

      frs_spectra_ptr->push_back(spec_xy) ;
    */

}
//**********************************************************************
void Tpisolo_tof::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;
    tof1_ok =  tof2_ok   =  tof3_ok = false;


    tof1_raw =   event_unpacked->*tof1_event;
    if(tof1_raw)
    {
        spec_tof1_raw -> manually_increment(tof1_raw) ;
        tof1_cal = ((tof1_raw + randomizer::randomek()) * slope1) + offset1 ;
        tof1_ok = true;
        spec_tof1_cal ->manually_increment(tof1_cal) ;
    }

    // ====2 =====
    tof2_raw =   event_unpacked->*tof2_event;
    if(tof2_raw)
    {
        spec_tof2_raw -> manually_increment(tof2_raw) ;
        tof2_cal = ((tof2_raw + randomizer::randomek()) * slope2) + offset2 ;
        tof2_ok = true;
        spec_tof2_cal ->manually_increment(tof2_cal) ;
    }


    // ====3 =====
    tof3_raw =   event_unpacked->*tof3_event;
    if(tof3_raw)
    {
        spec_tof3_raw -> manually_increment(tof3_raw) ;
        tof3_cal = ((tof3_raw + randomizer::randomek()) * slope3) + offset3 ;
        tof3_ok = true;
        spec_tof3_cal ->manually_increment(tof3_cal) ;
    }

    calculations_already_done = true ;
}
//**************************************************************************
void  Tpisolo_tof::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;
    // in this file we look for a string

    slope1  = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof1_slope");
    offset1 = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof1_offset");

    slope2  = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof2_slope");
    offset2 = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof2_offset");

    slope3  = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof3_slope");
    offset3 = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof3_offset");

}
//***************************************************************

