/***************************************************************************
                          Tdgf_general_detector.cpp  -  description
                             -------------------
    begin                : Apr 28 2004
    copyright            : (C) 2004 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "Tdgf_general_detector.h"
#include "Tfile_helper.h"
#include "randomizer.h"

#include "Tnamed_pointer.h"
//#include "Tself_gate_dgf_descr.h"

#include "Tcrystal.h"  // for doppler corr
#include "Ttarget.h"  // for doppler corr
#include "TIFJAnalysis.h" // for ntuple



//*************************************************************************
// these arguments nr_slow, nr_fast tell us  in which channel of ADC this data
// was send.
// dgftor_adc[32], dgftor_tdc[32]
// for example  BaF nr 6 can get data from adc[2], adc[14], tdc[8]


Tdgf_general_detector::Tdgf_general_detector(Tdgf*ptr_owner, int nr_det,
        int nr_trigtime, int nr_energy, int nr_xiapsa, int nr_userpsa)
    : owner(ptr_owner), det_nr(nr_det),
      channel_nr_of_trig_time(nr_trigtime),
      channel_nr_of_energy(nr_energy),
      channel_nr_of_xia_psa(nr_xiapsa),
      channel_nr_of_user_psa(nr_userpsa)
{
    name_of_this_element = "dgf_general_detector_" ;
    name_of_this_element +=  char('0' + det_nr) ;

    //....................................................
    // warning: The above pointers should point to the TOTAL spectra
    // which belong to  the dgf object. They do NOT exist now, because the
    // dgf constructor runs later than this constructor
    // So these pointers had to be set in the preloop



    //....................................................

    create_my_spectra();



    named_pointer[name_of_this_element + "_trig_time"]
    = Tnamed_pointer(&trig_time_for_named_pointer, 0, this) ;


    named_pointer[name_of_this_element + "_energy"]
    = Tnamed_pointer(&energy, 0, this) ;

    named_pointer[name_of_this_element + "_xia_psa_value"]
    = Tnamed_pointer(&xia_psa_value, 0, this) ;

    named_pointer[name_of_this_element + "_user_psa_value"]
    = Tnamed_pointer(&user_psa_value, 0, this) ;



}
//*************************************************************************
Tdgf_general_detector::~Tdgf_general_detector()
{
}
//*************************************************************************
void Tdgf_general_detector::create_my_spectra()
{
    string folder = "dgf/" + name_of_this_element ;



    string name = name_of_this_element + "_trig_time_raw"  ;
    spec_trig_time_raw = new spectrum_1D(name,
                                         name,
                                         8192, 0, 8192,   // was 4096
                                         folder);
    dgf_spectra_ptr()->push_back(spec_trig_time_raw) ;


    name = name_of_this_element + "_trig_time_corrected"  ;
    spec_trig_time_corrected = new spectrum_1D(name,
            name,
            2560, 0, 100,   // was 4096
            folder);
    dgf_spectra_ptr()->push_back(spec_trig_time_corrected) ;



    name = name_of_this_element + "_energy_raw"  ;
    spec_energy_raw = new spectrum_1D(name,
                                      name,
                                      8192, 0, 8192,   // was 4096
                                      folder);
    dgf_spectra_ptr()->push_back(spec_energy_raw) ;




    name = name_of_this_element + "_xia_psa_value_raw"  ;
    spec_xia_psa_value_raw = new spectrum_1D(name,
            name,
            8192, 0, 8192,   // was 4096
            folder);
    dgf_spectra_ptr()->push_back(spec_xia_psa_value_raw) ;


    name = name_of_this_element + "_user_psa_raw"  ;
    spec_user_psa_value_raw = new spectrum_1D(name,
            name,
            8192, 0, 8192,   // was 4096
            folder);
    dgf_spectra_ptr()->push_back(spec_user_psa_value_raw) ;


}
//*************************************************************************
//************************************************************************
/** readinng calibration, gates, setting pointers */
void Tdgf_general_detector::make_preloop_action(ifstream & s)
{
    //
//  cout << "preloop action for "
//       << name_of_this_element
//       << endl ;
//


}
//***************************************************************************
//***************************************************************************
/** // the most important fuction,  executed for every event */
void Tdgf_general_detector::analyse_current_event()
{

    //  cout << "   analyse_current_event()          for "
    //      << name_of_this_element
    //      << endl ;

    trig_time = *ptr_to_trig_time ;   // taking from the event
    energy = (*ptr_to_energy) >> 3;
    xia_psa_value = (* ptr_to_xia_psa_value) >> 3;
    user_psa_value = (*ptr_to_user_psa_value) >> 3;

//    unsigned int ref = owner -> give_ref_timestamp() ;

//     if( (ref - trig_time) < 0)
//    {
//    cout << name_of_this_element
//    << " trig_time = 0x"  << hex << trig_time << ", dec= " << dec << trig_time
//    <<" ref = 0x" << hex << ref << ", dec = " << dec << ref
//    << ", roznica = 0x" << hex << (ref-trig_time) << ", dec= " << dec << (ref-trig_time)
//    << endl;
//
//
//          trig_time = trig_time ;
//      int m ;
//      m = 6 ;
//  }

    int difference = owner -> give_ref_timestamp() - trig_time  ;
    if(difference < 0)
    {
//              cout << name_of_this_element
//                      << " trig_time = 0x"  << hex << trig_time << ", dec= " << dec << trig_time
//                      <<" ref = 0x" << hex << ref << ", dec = " << dec << ref
//                      << ", roznica = 0x" << hex << (ref-trig_time) << ", dec= " << dec << (ref-trig_time)
//                      << endl;

        trig_time = owner -> give_ref_timestamp() - trig_time + 0x10000 ;
    }
    else
        trig_time =  owner -> give_ref_timestamp() - trig_time;


    trig_time_for_named_pointer = trig_time ; // bec named pointer can not be unsigned short

    // checking if it is something (not zero)
    if(trig_time || energy)     // if any of them had something
    {
        flag_fired = true ;
    }


    if(trig_time)
    {
        spec_trig_time_raw->manually_increment(trig_time);
    }

    if(energy)
        spec_energy_raw->manually_increment(energy);

    if(xia_psa_value)
        spec_xia_psa_value_raw->manually_increment(xia_psa_value);

    if(user_psa_value)
        spec_user_psa_value_raw->manually_increment(user_psa_value);


    if(trig_time &&  xia_psa_value)
    {

        double corrected_time = (trig_time  /* + (xia_psa_value >> 8)*/)  +
                                ((xia_psa_value & 0xff) / 256.0) ;

        spec_trig_time_corrected->manually_increment(corrected_time);

    }



}
