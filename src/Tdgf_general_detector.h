/********** *****************************************************************
                          tdgf_general_detector.h  -  description
                             -------------------
    begin                : Wed May 21 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#ifndef TDGF_general_detector_H
#define TDGF_general_detector_H

#include "Tdgf.h"
#include "Tincrementer_donnor.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
class Tdgf_general_detector : public Tincrementer_donnor
{
    friend class Tdgf ;
protected:
    Tdgf *owner ;

    bool flag_fired ;
    bool flag_good_data ;

    const int det_nr ;

    string name_of_this_element ;


    // raw data comming in the event
    unsigned short trig_time;
    int energy ;
    int xia_psa_value;
    int user_psa_value;

    int trig_time_for_named_pointer;  // bec. it does not accept unsigned short


    double distance_from_target ; // from the target?


    // places in the array in the unpacked event
    int channel_nr_of_trig_time;
    int channel_nr_of_energy;
    int channel_nr_of_xia_psa;
    int channel_nr_of_user_psa;



    // pointers to the data inside the event
    short int *ptr_to_trig_time;
    int *ptr_to_energy ;

    int *ptr_to_xia_psa_value;
    int *ptr_to_user_psa_value;


    // spectra ----------------------------------
    spectrum_1D
    *spec_trig_time_corrected,
    *spec_trig_time_raw,
    *spec_energy_raw,
    *spec_xia_psa_value_raw,
    *spec_user_psa_value_raw;

    // pointers to the Total spectra, defined inside the dgf object, and
    // shared by all general_detector

    // warning, to not set this pointers in the constructor (or in
    // create_my_spectra) that would be too early, because the costructor of
    // Hector runs later than the consructor of general_detector



public:

    // these arguments nr_slow, nr_fast tell in which channel we send this data
    // hector_adc[32], hector_tdc[32]
    // for example  general_detector nr 6 can get data from adc[2], adc[14], tdc[8]

    Tdgf_general_detector(Tdgf*ptr_owner, int nr_det,
                          int nr_trig_time, int nr_energy, int nr_xiapsa, int nr_userpsa);
//   Tdgf_general_detector(Tdgf*ptr_owner, int nr_det,  int nr_trig_time, int nr_energy);

    ~Tdgf_general_detector();


    vector< spectrum_abstr*>*  dgf_spectra_ptr()
    {
        return owner->give_spectra_pointer() ;
    }
    /**  the most important fuction,  executed for every event */

    /** readinng calibration, gates, setting pointers */
    void make_preloop_action(ifstream &);
    void analyse_current_event() ;


    /** where in the event are data for  this particular general_detector*/
    void set_event_data_pointers(int * dgf_adc_ptr, short int *dgf_tdc_ptr
                                 , int * dgf_xia_psa_ptr,
                                 int * dgf_user_psa_ptr
                                )
    {
        ptr_to_energy = &dgf_adc_ptr[channel_nr_of_energy];
        ptr_to_trig_time = &dgf_tdc_ptr[channel_nr_of_trig_time];

        ptr_to_xia_psa_value = &dgf_xia_psa_ptr[channel_nr_of_xia_psa];
        ptr_to_user_psa_value = &dgf_user_psa_ptr[channel_nr_of_user_psa];

    }


protected:
    void create_my_spectra() ;


};
//////////////////////////////////////////////////////////////////////////////
#endif
