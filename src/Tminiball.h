#ifndef _MINIBALL_CLASS_H_
#define _MINIBALL_CLASS_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__


#include "spectrum.h"

#include "Tcate.h"
#include "Tminiball_cryostat.h"

#include <vector>

#include "TIFJEvent.h"
#include "TIFJCalibratedEvent.h"

#include "TjurekPolyCond.h"
#include "TIFJAnalysis.h"  // for registering conditions

#include "Tincrementer_donnor.h"
// class Tspectrum_1D_conditional;

class Thector ;

///////////////////////////////////////////////////////////////////////////////
class Tminiball : public Tincrementer_donnor
{
    friend class Tminiball_elementary_det;

    Tfrs    *frs_pointer ;  // pointer to the frs object
//     Thector *hector_pointer ;  // pointer to the HECTOR object

    // all pointers to spectra for MINIBALL elementary_dets and clusters
    // are registered also here - to make easier saving them
    // from time to time
    vector<spectrum_abstr*> spectra_miniball ;

//      vector<Tspectrum_1D_conditional*>  user_spectra_list;
    //-------------------------------
    // elements of the miniball
    vector< Tminiball_cryostat* > cryostat ;



    TIFJEvent* tmp_event_unpacked;

    spectrum_1D *spec_total_core_energy_cal ;
    spectrum_1D *spec_total_core_energy_doppler ;

    spectrum_1D
    * spec_total_core_energy_cal_addbacked,
    * spec_total_core_energy_dop_addbacked;
    spectrum_1D *spec_total_time_cal ;
    spectrum_1D * spec_total_theta_miliradians;
    spectrum_1D *spec_fan ; // fan spectrum for all elementary_dets
    spectrum_1D *spec_miniball_cryostats_multiplicity ;
    spectrum_1D *spec_mib_synchro ;



public:

    Tminiball();
    virtual ~Tminiball() {} ;

    static TIFJCalibratedEvent *tmp_event_calibrated;


    // function to get the pointer to list of the spectra
    vector<spectrum_abstr*> *  address_spectra_miniball()
    {
        return &spectra_miniball ;
    }
    Tminiball_cryostat* give_cryostat(int i)
    {
        return cryostat[i] ;
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

    void increm_spec_total_core_energy_calibrated(double ene_cal)
    {
        spec_total_core_energy_cal->manually_increment(ene_cal)  ;
    }

    void increm_spec_total_time_calibrated(double tim_cal)
    {
        spec_total_time_cal->manually_increment(tim_cal) ;
    }

    // addbacked verisons

    void increm_spec_total_core_energy_calibrated_addbacked(double ene_cal)
    {
        spec_total_core_energy_cal_addbacked->manually_increment(ene_cal)  ;
    }




    // addb, dopl
    void increm_spec_total_core_energy_doppler_addbacked(double ene_cal)
    {
        spec_total_core_energy_dop_addbacked->manually_increment(ene_cal)  ;
    }




    Tfrs* frs_ptr()
    {
        return frs_pointer ;   // for doppler corr. we need contact with target
    }

    double  give_target_x()
    {
        return target_x ;
    }
    double  give_target_y()
    {
        return target_y ;
    }
    /** No descriptions */
    bool save_selected_spectrum(string name);;
    /** Depending on the status of hardware we can use different algorithms */
    void read_doppler_corr_options();
    /** No descriptions */
    int give_synchro_data() ;;
    /** No descriptions */
    void read_good_elementary_det_specification();
    /** No descriptions */
    void monitoring_statistics();

    //------------------------------
protected:
    double target_x ;   // we keep it here for doppler correction
    /**  */
    Tcate * cate_ptr;


    double target_y ;
    /**  */
    int fire_statistics[15][7];  // for graphic monitoring

    int mib_synchro_data;

    //for testing time

    void simulate_event();
    void create_my_spectra() ;
    /** Rerading the definiton of the conditinal (gated) spectra */
    void read_def_conditional_spectra();
    /** Create one conditional spectrum */
    void create_cond_spectrum(string name);
    /** where the user spectra can be incremented */
    void user_spectra_loop();
    /** This function reads the options defined in the dialog window of the
    cracow viewer. The result is given to the static members of
    the Tminiball_cryostat */
    void read_addback_options_for_cryostats();

    /** Do we wand addback, or no - just classical simple analysis */
    bool flag_make_addback ;

};
/////////////////////////////////////////////////////////////////////
#endif // __CINT__


#endif // _MINIBALL_CLASS_H_
