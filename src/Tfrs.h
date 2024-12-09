#ifndef _FRS_CLASS_H_
#define _FRS_CLASS_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

class Target;

#include <vector>
#include <string>
#include "Tfrs_element.h"
#include "Ttarget.h"
#include "Tframe_simple.h"
#include "Tmultiwire.h"

#include "TIFJEvent.h"
#include "TIFJCalibratedEvent.h"
//#include "Go4ConditionsBase/TGo4PolyCond.h"
//#include "Go4Analysis/TGo4Analysis.h"  // for registering conditions
#include "Tnamed_pointer.h"


//****************************************************************
// this file contains definition of two classes
// 1. frs_class - representing the Fragment Separator

//****************************************************************

//class Tone_signal_module ;

class TIFJAnalysis;
class Tkratta;
class Ttarget;
///////////////////////////////////////////////////////////////////////////////
class Tfrs : public Tincrementer_donnor
{
    // all spectra for frs calculations are here
    vector<spectrum_abstr*> spectra_frs ;
    vector<Tfrs_element *> module;  // only pointers to devices defined below
    //-------------------------------
    int dead_time ;  // will be calculated by the scalers
    TIFJCalibratedEvent* event_calibrated ;

public:
    Tfrs(TIFJAnalysis *ptr, std::string name = "frs");

    ~ Tfrs()
    {
      for(unsigned int i = 0 ; i <  spectra_frs.size() ; i++)
        {
            delete   spectra_frs[i];
        }
        for(unsigned int i = 0 ; i < module.size() ; i++)
        {
            delete module[i] ;
        }
    }

    // filling the vector with the proper configuration
    // or perhaps we can make it in preloop - when we know all the options?

    vector<spectrum_abstr*> *  address_spectra_frs()
    {
        return &spectra_frs ;
    }

    //------------------------------
    void make_frs_preloop_action(TIFJEvent* event_unpacked ,
                                 TIFJCalibratedEvent* event_calibrated_ptr);

    // taking the calibration factors
    void read_calibration_and_gates_from_disk();
    // above function is called form preloop function

    void analyse_current_event();
    void make_user_event_action(); // shop of events ?
    void make_postloop_action();  // saving the spectra ?

    Tfrs_element* address_of_device(string name);
    /**   For doppler correction algorithm */
    double give_beta_tof();

    void perhaps_autosave();
    /** To refresh more often spectra which are currently on the screen of the cracow viewer */
    bool save_selected_spectrum(string name);
    /** CATE will make everything only when some rising multiplicity */
    void remember_rising_cluster_multiplicity(bool ok);
    /** No descriptions */
    /** No descriptions */
    void put_data_to_ntuple_tree();
    /** To have the proper ranges of the spectra with zet or aoq  */
    Tkratta* get_kratta_ptr() { return kratta_ptr;}
    //------------------------------
protected:

    void create_your_spectra();
    /** No descriptions */


protected: // Protected attributes
    /** doppler correcting devices will ask for some data */
    Ttarget * target_ptr;
    std::string official_name; // will be used as prefix for calibration file
    Tkratta * kratta_ptr;

};
#endif // __CINT__

#endif // _FRS_CLASS_H_
