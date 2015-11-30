#ifndef _Tdgf_H_
#define _Tdgf_H_

#include "spectrum.h"
#include <vector>

#include "TIFJEvent.h"
#include "TIFJCalibratedEvent.h"
// #include "Go4ConditionsBase/TGo4PolyCond.h"
// #include "Go4Analysis/TGo4Analysis.h"  // for registering conditions

#include "Tincrementer_donnor.h"

#define  dgf_CALIBRATE_TIME    1

class Tdgf_general_detector ;
///////////////////////////////////////////////////////////////////////////////
class Tdgf : public Tincrementer_donnor
{
    // to give access to pointers to the total spectra
    friend class Tdgf_general_detector ;

    vector< Tdgf_general_detector* > general_detector;    // BAF

//  int dgf_ENERGY_channel[8] ;
//  int dgf_FAST_TRIG_channel[8] ;
//  int dgf_XIA_PSA_channel[8] ;
//  int dgf_USER_PSA_channel[8] ;


    int synchro_data ;

    string name_of_this_element ;

    // all pointers to spectra for dgf cristals and clusters
    // are registered also here - to make easier saving them
    // from time to time
    vector<spectrum_abstr*> spectra_dgf ;

    vector<spectrum_abstr*> spectra_markers ;

    //-------------------------------
    // elements of the dgf


    TIFJEvent* tmp_event_unpacked;


    spectrum_1D *spec_synchro ; // which detectors fired


public:

    Tdgf();
    void create_my_spectra() ;

    // function to get the pointer to list of the spectra
    vector<spectrum_abstr*> *  address_spectra_dgf()
    {
        return &spectra_dgf ;
    }

    //------------------------------
    void make_preloop_action(TIFJEvent* event_unpacked ,
                             TIFJCalibratedEvent* event_calibrated);

    // taking the calibration factors
    void read_calibration_and_gates_from_disk();
    // above function is called form preloop function

    //void make_step1_action();
    void analyse_current_event();
    void make_user_event_action(); // shop of events ?
    void make_postloop_action();  // saving the spectra ?

    vector< spectrum_abstr*>*  give_spectra_pointer()
    {
        return &spectra_dgf ;
    }

    /** No descriptions */
    bool save_selected_spectrum(string name);

    /** No descriptions */
    int  give_synchro_data() ;
    //  /** For matrix produced in rising object */
    //  int give_time_BaF(int nr);

    unsigned int give_ref_timestamp()
    {
        return reference_time_stamp ;
    }


    //------------------------------
protected:

    //for testing time
    void simulate_event();

    spectrum_1D *spec_tdc_ch[8] ;
    spectrum_1D *spec_adc_ch[8] ;
    spectrum_1D *spec_xia_psa_ch[8] ;
    spectrum_1D *spec_user_psa_ch[8] ;
    /**  */
    unsigned int reference_time_stamp;

};
////////////////////////////////////////////////////////////////
#endif // _Tdgf_H_
