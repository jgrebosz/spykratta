#ifndef _RISING_CLASS_H_
#define _RISING_CLASS_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"
#include "spectrum.h"

#include "Tcate.h"
#include "Tcluster.h"
#include "Tcluster_xia.h"
#include "Tfrs.h"
#include <vector>

#include "TIFJEvent.h"
#include "TIFJCalibratedEvent.h"

//#include "Go4ConditionsBase/TGo4PolyCond.h"
//#include "Go4Analysis/TGo4Analysis.h"  // for registering conditions

#include "Tincrementer_donnor.h"
// class Tspectrum_1D_conditional;
// class Thector ;

///////////////////////////////////////////////////////////////////////////////
class Trising : public Tincrementer_donnor
{
    friend class Tcrystal ;
    friend class Tcrystal_xia ;

    Tfrs    *frs_pointer ;  // pointer to the frs object
#ifdef HECTOR_PRESENT
    //Thector *hector_pointer ;  // pointer to the HECTOR object
#endif

    // all pointers to spectra for rising crystals and clusters
    // are registered also here - to make easier saving them
    // from time to time
    vector<spectrum_abstr*> spectra_rising ;

//      vector<Tspectrum_1D_conditional*>  user_spectra_list;
    //-------------------------------
    // elements of the rising
    vector< Tcluster* > detector_vxi ;
    vector< Tcluster_xia* > detector_xia ;

    int cluster_delivered_by_miniball[10];


//#ifdef NIGDY

#ifdef DOPPLER_CORRECTION_NEEDED
    spectrum_1D *spec_total_energy4_doppler;
    spectrum_1D *spec_total_energy20_doppler;
    spectrum_1D *spec_total_energy4_dop_addbacked;
    spectrum_1D * spec_total_energy20_dop_addbacked;
#endif

    spectrum_1D *spec_total_energy4_cal;
    spectrum_1D *spec_total_energy20_cal;



    spectrum_1D
    * spec_total_energy4_cal_addbacked,
    * spec_total_energy20_cal_addbacked;

    spectrum_1D *  spec_cluster_total_time_cal_minus_GeOR_cal;
    spectrum_1D *spec_total_time_cal ;
    spectrum_1D * spec_total_theta_miliradians;



    spectrum_1D *spec_clusters_vxi_multiplicity ;
    spectrum_1D *spec_clusters_xia_multiplicity ;

    spectrum_1D *spec_ger_synchro ; // crate1 time calibrator spectrum
    spectrum_1D *spec_ger2_synchro ;  // crate2 time calibrator
    spectrum_2D *spec_synch_crate2_vs_crate1 ;
#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE
    int Silicon_LR_trigger_time_minus_Plastic_LR_time[3];
#endif


public:
    spectrum_1D *spec_fan ; // fan spectrum for all crystals

    Trising();
    virtual ~Trising() {} ;

    static TIFJCalibratedEvent *tmp_event_calibrated;
    static TIFJEvent* tmp_event_unpacked;

//####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
//####################################################################

    // For g-factor experiments (P means particle)
    static int xia_P_time_minus_Ge_time_OR_raw;
    static double xia_P_time_minus_Ge_time_OR_cal;

    static int vxi_P_time_minus_Ge_time_OR_raw;
    static double vxi_P_time_minus_Ge_time_OR_cal;

    // this below is a new idea, when we realized, that the calibration of this above
    // must be done by 56 ways - depending on the crystal which fired
    static double vxi_LR_time_calibrated_by_the_first_who_fired;

    spectrum_1D *spec_vxi_P_time_minus_Ge_time_OR_raw;
    spectrum_1D *spec_vxi_P_time_minus_Ge_time_OR_cal;
    spectrum_1D *spec_xia_P_time_minus_Ge_time_OR_raw;
    spectrum_1D *spec_xia_P_time_minus_Ge_time_OR_cal;

    spectrum_1D *spec_vxi_LR_time_calibrated_by_the_first_who_fired;
    // for g-factor experiments
    void  increm_spec_cluster_total_time_cal_minus_GeOR_cal(double t)
    {
        spec_cluster_total_time_cal_minus_GeOR_cal->manually_increment(t) ;
    }

#endif
    static const string cluster_characters; // Letters assigned to particular clusters

    // function to get the pointer to list of the spectra
    vector<spectrum_abstr*> *  address_spectra_rising()
    {
        return &spectra_rising ;
    }
    Tcluster* give_detector_vxi(int i)
    {
        return detector_vxi[i];
    }

    //------------------------------
    void make_preloop_action(TIFJEvent* event_unpacked ,
                             TIFJCalibratedEvent* event_calibrated);

    // taking the calibration factors
    void read_calibration_and_gates_from_disk();
    // above function is called form preloop function

    //void make_step1_action();
    void analyse_current_event();
    void analyse_current_event_vxi();
    void analyse_current_event_xia();

    void make_user_event_action(); // shop of events ?
    void make_postloop_action();  // saving the spectra ?

    void increm_spec_total_energy4_calibrated(double ene_cal)
    {
        spec_total_energy4_cal->manually_increment(ene_cal)  ;
    }
    void increm_spec_total_energy20_calibrated(double ene_cal)
    {
        spec_total_energy20_cal->manually_increment(ene_cal)  ;
    }

    void increm_spec_total_time_calibrated(double tim_cal)
    {
        spec_total_time_cal->manually_increment(tim_cal) ;
    }


    // addbacked verisons

    void increm_spec_total_energy4_calibrated_addbacked(double ene_cal)
    {
        spec_total_energy4_cal_addbacked->manually_increment(ene_cal)  ;
    }


    void increm_spec_total_energy20_calibrated_addbacked(double ene_cal)
    {
        spec_total_energy20_cal_addbacked->manually_increment(ene_cal)  ;
    }


#ifdef DOPPLER_CORRECTION_NEEDED
    // addb, dopl
    void increm_spec_total_energy4_doppler_addbacked(double ene_cal)
    {
        spec_total_energy4_dop_addbacked->manually_increment(ene_cal)  ;
    }

    void increm_spec_total_energy20_doppler_addbacked(double ene_cal)
    {
        spec_total_energy20_dop_addbacked->manually_increment(ene_cal)  ;
    }
#endif


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
    bool save_selected_spectrum(string name);
    /** Depending on the status of hardware we can use different algorithms */
    void read_doppler_corr_options();
    /** Before analyzing anything in Hector and Rising */
//  bool first_check_cluster_multiplicity();
    /** No descriptions */
    int give_synchro_data();
    /** No descriptions */
    void read_good_crystal_specification();
    /** No descriptions */
    void monitoring_statistics();

    //------------------------------
protected:
    double target_x ;   // we keep it here for doppler correction
    /**  */
#ifdef CATE_PRESENT

    Tcate * cate_ptr;
#endif


    double target_y ;
    /**  */

    int ger_synchro_data;  // time calibrator from the vxi crate nr 1
    int ger2_synchro_data; // time calibrator from the vxi crate nr 2

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
    the Tcluster */
    void read_addback_options_for_clusters();

    /** Do we want addback, or no - just classical simple analysis */
    bool flag_make_addback ;
    vector<double> vxi_GeOR_calibr_factors ;
    vector<double> xia_GeOR_calibr_factors ;

    // Adam would like to have such incrementers,
    int timestamp_GER_minus_timestamp_FRS;
    bool timestamp_GER_minus_timestamp_FRS_possible;
    int timestamp_DGF_minus_timestamp_FRS;
    bool timestamp_DGF_minus_timestamp_FRS_possible;

#if CURRENT_EXPERIMENT_TYPE==RISING_STOPPED_BEAM_CAMPAIGN
    spectrum_1D * spec_plastic_time_LR_raw;
    int plastic_time_LR_raw;
    int trigger;  // determines what "subevents" will be comming
#endif

};
/////////////////////////////////////////////////////////////////////
#endif // __CINT__


#endif // _RISING_CLASS_H_
