#include "Tcrystal_xia.h"
#include "Tcluster_xia.h"



#include <sstream>
#include <iomanip>
using namespace std;

#include <algorithm>
#include "Trising.h"
#include "Ttarget.h"
#include "Tfrs.h"
#include "Thector.h"
#include "Tnamed_pointer.h"
#include "Tself_gate_ger_descr.h"
#include "TIFJAnalysis.h" // for verbose mode
Trising * Tcrystal_xia::rising_pointer; // static pointer
Ttarget * Tcrystal_xia::target_ptr;      // for faster acces of the doppler correction



// enums declared inside the Tcrystal_xia class
type_of_tracking_requirements   Tcrystal_xia::tracking_position_on_target;
type_of_cate_requirements       Tcrystal_xia::tracking_position_on_cate;
type_of_doppler_algorithm       Tcrystal_xia::which_doppler_algorithm;

bool   Tcrystal_xia::flag_beta_from_frs; // which beta to use, frs or fixed
double Tcrystal_xia::energy_deposit_in_target; // bec. beta on the other side
// of the target will be slower


int Tcrystal_xia::multiplicity;
double Tcrystal_xia::sum_en4_cal;

bool Tcrystal_xia::flag_increment_4MeV_cal_with_zero;
bool Tcrystal_xia::flag_increment_time_cal_with_zero;


bool Tcrystal_xia::flag_good_4MeV_requires_threshold;
double Tcrystal_xia::en4MeV_threshold;
double Tcrystal_xia::en4MeV_threshold_upper;

bool Tcrystal_xia::flag_good_time_requires_threshold;
double Tcrystal_xia::good_time_threshold_lower; // this is the new style
double Tcrystal_xia::good_time_threshold_upper; // this is the new style



#ifdef DOPPLER_CORRECTION_NEEDED
double Tcrystal_xia::sum_en4_dop;
spectrum_1D * Tcrystal_xia::spec_total_energy4_doppler;
#endif

spectrum_1D * Tcrystal_xia::spec_total_time_cal;
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
spectrum_1D * Tcrystal_xia::spec_total_time_cal_minus_GeOR_cal;
#endif
spectrum_1D * Tcrystal_xia::spec_total_energy4_cal;

#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
spectrum_1D * Tcrystal_xia::spec_total_time_LR_cal;
spectrum_1D * Tcrystal_xia::spec_total_time_SR_cal;
#endif

#define dcout   if(0)cout

//**************************************************************************
Tcrystal_xia::Tcrystal_xia(Tcluster_xia *parent_ptr, int nr): parent_cluster(parent_ptr)
{
    //  cout << "constructor of crystal"
    //        << ", addres of parent is "
    //        << parent_cluster
    //        << ", my address is " << this
    //        << endl;

    id_number = nr;  // note: this starts form 1,and goes: 2,3,4,5,6,7
    ostringstream sss;
    sss << parent_cluster->give_detector_name()
        << "_"  << id_number;

    name_of_this_element = sss.str();

    // to be able to increment total fan spectrum we must know which
    // cluster it is

    cluster_nr = parent_cluster->give_cluster_id();

    //  cout << "This crystal has numbers " << cluster_nr << ", "
    //      << id_number << endl;

    //  energy4_data = 0;
    //  time_data = 0;
    //  threshold_to_multiplicity = 100; // 100 keV

#ifdef DOPPLER_CORRECTION_NEEDED
    doppler_factor = 1;
#endif // def DOPPLER_CORRECTION_NEEDED

    create_my_spectra();

    named_pointer[name_of_this_element + "_energy4MeV_raw_when_fired"]
    = Tnamed_pointer(&energy4_raw, &flag_fired, this);

    named_pointer[name_of_this_element + "_time_raw_when_fired"]
    = Tnamed_pointer(&time_raw, &flag_fired, this);

    named_pointer[name_of_this_element + "_energy4MeV_cal_when_fired"]
    = Tnamed_pointer(&energy4_calibrated, &flag_fired, this);


#ifdef  LUCIA_TWO_STEPS_ENERGY_CALIBRATION
    named_pointer[name_of_this_element + "_eenergy4_first_step_cal_when_fired"]
    = Tnamed_pointer(&energy4_first_step_calibrated, &flag_fired, this);
#endif


#ifndef LONG_RANGE_SHORT_RANGE_TDC_USED
    named_pointer[name_of_this_element + "_time_cal_when_fired"]
    = Tnamed_pointer(&time_calibrated, &flag_fired, this);
#endif

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    named_pointer[name_of_this_element + "_time_cal_minus_GeOR_cal_when_fired"]
    = Tnamed_pointer(&time_cal_minus_GeOR_cal, &flag_fired, this);
#endif

    named_pointer[name_of_this_element + "_energy4MeV_cal_when_good"]
    = Tnamed_pointer(&energy4_calibrated, &flag_good_data, this);

    named_pointer[name_of_this_element + "_energy4MeV_cal_when_good_and_NotUsedForAddback"]
    = Tnamed_pointer(&energy4_calibrated, &flag_good_nonaddbacked_crystal, this);

    //#ifndef LONG_RANGE_SHORT_RANGE_TDC_USED
    named_pointer[name_of_this_element + "_time_cal_when_good"]
    = Tnamed_pointer(&time_calibrated, &flag_good_data, this);
    //#endif

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    named_pointer[name_of_this_element + "_time_cal_minus_GeOR_cal_when_good"]
    = Tnamed_pointer(&time_cal_minus_GeOR_cal, &flag_good_data, this);
#endif


#if ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||         (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))

#ifdef TIME_DIFF_USED
    named_pointer[name_of_this_element + "_time_diff_calibrated_when_good"]
    = Tnamed_pointer(&time_diff_calibrated, &flag_good_data, this);
#endif

    named_pointer[name_of_this_element + "_time_LR_calibrated_when_good"]
    = Tnamed_pointer(&time_LR_calibrated, &flag_good_data, this);

    named_pointer[name_of_this_element + "_time_SR_calibrated_when_good"]
    = Tnamed_pointer(&time_SR_calibrated, &flag_good_data, this);


    // ALL
    {
        // local scope
        string entry_name;
        Tmap_of_named_pointers::iterator pos;

#ifdef TIME_DIFF_USED
        entry_name =  "ALL_cluster_xia_crys_time_diff_calibrated_when_good";
        pos = named_pointer.find(entry_name);
        if(pos != named_pointer.end())
            pos->second.add_item(&time_diff_calibrated, &flag_good_data, this);
        else
            named_pointer[entry_name] = Tnamed_pointer(&time_diff_calibrated,
                                        &flag_good_data, this);
#endif

        entry_name =  "ALL_cluster_xia_crys_time_LR_calibrated_when_good";
        pos = named_pointer.find(entry_name);
        if(pos != named_pointer.end())
            pos->second.add_item(&time_LR_calibrated, &flag_good_data, this);
        else
            named_pointer[entry_name] = Tnamed_pointer(&time_LR_calibrated,
                                        &flag_good_data, this);

        entry_name =  "ALL_cluster_xia_crys_time_SR_calibrated_when_good";
        pos = named_pointer.find(entry_name);
        if(pos != named_pointer.end())
            pos->second.add_item(&time_SR_calibrated, &flag_good_data, this);
        else
            named_pointer[entry_name] = Tnamed_pointer(&time_SR_calibrated,
                                        &flag_good_data, this);
    } // end of local
#endif



#ifdef DOPPLER_CORRECTION_NEEDED
    named_pointer[name_of_this_element +
                  "_energy4MeV_doppler_when_successful"]
    = Tnamed_pointer(&energy4_doppler_corrected,
                     &flag_doppler_successful, this);


    for(int i = 0; i < 9; i++)
    {
        string bet("beta");
        bet += char('1' + i);
        named_pointer[name_of_this_element +
                      "_energy4MeV_doppler_" + bet + "_when_successful"]
        = Tnamed_pointer(&energy4_doppler_corrected_betaN[i],
                         &flag_doppler_successful, this);

    }
#endif

    // TOTALS


    // find in the map the entry with such a name -
    // if there is not - create it
    // if there is already - open it and add to it

    string entry_name =  "ALL_cluster_xia_crys_energy4MeV_cal_when_good";
    Tmap_of_named_pointers::iterator pos =
        named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&energy4_calibrated, &flag_good_data, this);
    else
        named_pointer[entry_name] = Tnamed_pointer(&energy4_calibrated,
                                    &flag_good_data, this);


    // multiaddbackflag_matching_is_valid
    entry_name =  "ALL_cluster_xia_crys_energy4MeV_cal_when_good_and_NotUsedForAddback";
    pos =  named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&energy4_calibrated, &flag_good_nonaddbacked_crystal, this);
    else
        named_pointer[entry_name] = Tnamed_pointer(&energy4_calibrated,
                                    &flag_good_nonaddbacked_crystal, this);

    /*
    entry_name =  "ALL_cluster_xia_crys_energy4MeV_cal_when_UsedForAdback";
      pos =  named_pointer.find(entry_name);
      if (pos != named_pointer.end ())
        pos->second.add_item(&energy4_calibrated, &flag_participant_of_addback, this );
      else
        named_pointer[entry_name] = Tnamed_pointer(&energy4_calibrated,
                                    &flag_participant_of_addback, this );
    */

    //---------------------------
#ifdef  LUCIA_TWO_STEPS_ENERGY_CALIBRATION
    entry_name =  "ALL_cluster_xia_crys_energy4MeV_first_step_cal_when_good";
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&energy4_first_step_calibrated, &flag_good_data, this);
    else
        named_pointer[entry_name] = Tnamed_pointer(&energy4_first_step_calibrated,
                                    &flag_good_data, this);
#endif


    //----------------------------
    entry_name =  "ALL_cluster_xia_crys_time_cal_when_good";
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&time_calibrated, &flag_good_data, this);
    else
        named_pointer[entry_name] = Tnamed_pointer(&time_calibrated,
                                    &flag_good_data, this);

    //----------------------------
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    entry_name =  "ALL_cluster_xia_crys_time_cal_minus_GeOR_cal_when_good";
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&time_cal_minus_GeOR_cal, &flag_good_data, this);
    else
        named_pointer[entry_name] = Tnamed_pointer(&time_cal_minus_GeOR_cal,
                                    &flag_good_data, this);
#endif

    //------------------------------

#ifdef DOPPLER_CORRECTION_NEEDED

    entry_name =  "ALL_cluster_xia_crys_energy4MeV_doppler_when_successful";
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&energy4_doppler_corrected,
                             &flag_doppler_successful, this);
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&energy4_doppler_corrected,
                           &flag_doppler_successful, this);


    // Beta playfield
    for(int i = 0; i < 9; i++)
    {

        string bet("beta");
        bet += char('1' + i);
        entry_name =  "ALL_cluster_xia_crys_energy4MeV_doppler_" + bet +
                      "_when_successful";

        pos = named_pointer.find(entry_name);
        if(pos != named_pointer.end())
            pos->second.add_item(&energy4_doppler_corrected_betaN[i],
                                 &flag_doppler_successful, this);
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&energy4_doppler_corrected_betaN[i],
                               &flag_doppler_successful, this);

    }

#endif // Doppler

    //  pseudo FAN for Piotrek
    entry_name =  "ALL_cluster_xia_crys_pseudo_fan_when_good";
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&pseudo_fan, &flag_good_data, this);
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&pseudo_fan, &flag_good_data, this);


#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE

    // array of 3 elements - three possible hits
    named_pointer[name_of_this_element +
                  "_Silicon_LR_trigger_time_minus_LR_time[0]_when_fired"] =
                      Tnamed_pointer(&Silicon_LR_trigger_time_minus_LR_time[0],
                                     &flag_fired, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    entry_name =
        "ALL_stoppers_"
        "__Silicon_LR_trigger_time_minus_LR_time[0]_when_fired" ;
    Tnamed_pointer::add_as_ALL(entry_name,
                               &Silicon_LR_trigger_time_minus_LR_time[0],
                               &flag_fired, this);
    //-----------
    named_pointer[name_of_this_element +
                  "_Silicon_LR_trigger_time_minus_LR_time[1]_when_fired"] =
                      Tnamed_pointer(&Silicon_LR_trigger_time_minus_LR_time[1],
                                     &flag_fired, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    entry_name =
        "ALL_stoppers_"
        "__Silicon_LR_trigger_time_minus_LR_time[1]_when_fired" ;
    Tnamed_pointer::add_as_ALL(entry_name,
                               &Silicon_LR_trigger_time_minus_LR_time[1],
                               &flag_fired, this);
    //-----------
    named_pointer[name_of_this_element +
                  "_Silicon_LR_trigger_time_minus_LR_time[2]_when_fired"] =
                      Tnamed_pointer(&Silicon_LR_trigger_time_minus_LR_time[2],
                                     &flag_fired, this) ;

    // corresponding ALLs - local to this stopper and global to all stopperS
    entry_name =
        "ALL_stoppers_"
        "__Silicon_LR_trigger_time_minus_LR_time[2]_when_fired" ;
    Tnamed_pointer::add_as_ALL(entry_name,
                               &Silicon_LR_trigger_time_minus_LR_time[2],
                               &flag_fired, this);

#endif

}
//************************************************************************
void Tcrystal_xia::create_my_spectra()
{
    //cout << "before creating the spectra " << endl;

    // the spectra have to live all the time, so here we can create
    // them during  the constructor

    vector<spectrum_abstr*> *rising_spectra_ptr =
        rising_pointer -> address_spectra_rising();

    string folder = "clusters/" + parent_cluster->give_detector_name();

    // energy -------------------------------------
    string name = name_of_this_element;
    name += "_energy4MeV_raw";
    spec_energy4_raw = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,  // here the binning must start from 0,
        //because Carl Wheldon automatic routine expect this
        folder);
    rising_spectra_ptr->push_back(spec_energy4_raw);

    //-----------------------

    // time ------------------------------------------

    name =   name = name_of_this_element + "_time_raw";
    spec_time_raw = new spectrum_1D(name,
                                    name,
                                    // 4096, 0, 4096,
#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
                                    8196, 0, 1 << 15,
#else
                                    8196, 0, 8192,
#endif
                                    folder);
    rising_spectra_ptr->push_back(spec_time_raw);


#ifdef  WE_WANT_ORIGINAL_DGF_TIME
    //  DGF time ------------------------------------------

    name  = name_of_this_element + "_time_dgf_raw";
    spec_time_dgf_raw = new spectrum_1D(name,
                                        name,
                                        // 4096, 0, 4096,
                                        8196, 0, 1 << 16,
                                        folder , "Original time taken from the DGF electronics (25 ns/unit)l");
    rising_spectra_ptr->push_back(spec_time_dgf_raw);

    name = name_of_this_element + "_time_dgf_cal";
    spec_time_dgf_cal = new spectrum_1D(name,
                                        name,
                                        8196, 0, 1 << 16,
                                        folder , "Original time taken from the DGF electronics - no need to calibrate (25 ns/unit)l");
    rising_spectra_ptr->push_back(spec_time_dgf_cal);

#endif


    //=============== calibrated spectra  =====
    // energy cal ------------------------------------
    name = name_of_this_element;

    name += "_energy4MeV_cal";
    spec_energy4_cal = new spectrum_1D(
        name,
        name,
        6000, 0, 6000,
        folder);
    rising_spectra_ptr->push_back(spec_energy4_cal);
    name = name_of_this_element;

    //---------------
    name = name_of_this_element;
    name += "_time_cal";
    spec_time_cal = new spectrum_1D(name,
                                    name,
                                    8192, 0, 8192,
                                    folder);
    rising_spectra_ptr->push_back(spec_time_cal);


#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    name =  name_of_this_element + "time_cal_minus_GeOR_cal";
    spec_time_cal_minus_GeOR_cal = new spectrum_1D(name,
            name,
            8192, -8192, 8192,
            folder,
            "after aritmetics:   time_cal   - "
            "cluster_xia_P_time_minus_Ge_time_OR_cal"
                                                  );
    rising_spectra_ptr->push_back(spec_time_cal_minus_GeOR_cal);
#endif


#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
    // time ------------------------------------------

    name =   name = name_of_this_element + "_time_LR_raw";
    spec_time_LR_raw = new spectrum_1D(name,
                                       name,
                                       // 4096, 0, 4096,
                                       8196, 0, 1048576,
                                       folder);
    rising_spectra_ptr->push_back(spec_time_LR_raw);

    //--------
    name = name_of_this_element;
    name += "_time_LR_cal";
    spec_time_LR_cal = new spectrum_1D(name,
                                       name,
                                       8192, 0, 8192,
                                       folder);
    rising_spectra_ptr->push_back(spec_time_LR_cal);


    name =   name = name_of_this_element + "_time_SR_raw";
    spec_time_SR_raw = new spectrum_1D(name,
                                       name,
                                       //4096, 0, 4096,
                                       8196, 0, 8196,
                                       folder);
    rising_spectra_ptr->push_back(spec_time_SR_raw);

    //--------
    name = name_of_this_element;
    name += "_time_SR_cal";
    spec_time_SR_cal = new spectrum_1D(name,
                                       name,
                                       8192, 0, 8192,
                                       folder);
    rising_spectra_ptr->push_back(spec_time_SR_cal);

#ifdef TIME_DIFF_USED
    name = name_of_this_element;
    name += "_time_diff_cal";
    spec_time_diff_cal = new spectrum_1D(name,
                                         name,
                                         8192, 0, 8192,
                                         folder);
    rising_spectra_ptr->push_back(spec_time_diff_cal);
#endif  // TIME_DIFF_USED


    //--------
#ifdef TIME_TIME_MATRICES_FOR_STEPHANE_PETRI
    // Special wish of Paddy Regan and Stephane Pietri
    //--------
    name = name_of_this_element;
    name += "_energy_vs_LR_time_raw";
    spec_energy_vs_LR_time = new spectrum_2D(name,
            name,
            256, 1, 8192,
            256, 1, 8192,
            folder);
    rising_spectra_ptr->push_back(spec_energy_vs_LR_time);

    //--------
    name = name_of_this_element;
    name += "_LR_time_vs_SR_time_raw";
    spec_LR_time_vs_SR_time = new spectrum_2D(name,
            name,
            256, 1, 8192,
            256, 1, 8192,
            folder);
    rising_spectra_ptr->push_back(spec_LR_time_vs_SR_time);

    //--------
    name = name_of_this_element;
    name += "_LR_time_vs_dgf_time_diff_cal";
    spec_LR_time_vs_dgf_time = new spectrum_2D(name,
            name,
            256, 1, 8192,
            256, 1, 8192,
            folder);
    rising_spectra_ptr->push_back(spec_LR_time_vs_dgf_time);
#endif

#endif


    // static total spectra
    if(!spec_total_energy4_cal)
    {
        name = "cluster_xia_total_energy4MeV_cal";
        spec_total_energy4_cal = new spectrum_1D(
            name,
            name,
            6000, 0, 6000,
            folder);
        rising_spectra_ptr->push_back(spec_total_energy4_cal);
    }

    //------------------
    if(!spec_total_time_cal)
    {

        name = "cluster_xia_total_time_cal";
        spec_total_time_cal = new spectrum_1D(name,
                                              name,
                                              8192, 0, 8192,
                                              folder);
        rising_spectra_ptr->push_back(spec_total_time_cal);
    }




#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
    if(!spec_total_time_LR_cal)
    {
        name = "cluster_xia_total_time_LR_cal";
        spec_total_time_LR_cal = new spectrum_1D(name,
                name,
                8192, 0, 8192,
                folder);
        rising_spectra_ptr->push_back(spec_total_time_LR_cal);
    }
    // SHORT RANGE
    if(!spec_total_time_SR_cal)
    {

        name = "cluster_xia_total_time_SR_cal";
        spec_total_time_SR_cal = new spectrum_1D(name,
                name,
                8192, 0, 8192,
                folder);
        rising_spectra_ptr->push_back(spec_total_time_SR_cal);
    }

#endif



#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    if(!spec_total_time_cal_minus_GeOR_cal)
    {
        name = "cluster_xia_total_time_cal_minus_GeOR_cal";
        spec_total_time_cal_minus_GeOR_cal = new spectrum_1D(name,
                name,
                8192, -8192, 8192,
                folder);
        rising_spectra_ptr->push_back(spec_total_time_cal_minus_GeOR_cal);
    }
#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005


    // ========== Total doppler ==================
#ifdef DOPPLER_CORRECTION_NEEDED

    if(!spec_total_energy4_doppler)
    {
        name = "cluster_xia_total_energy4MeV_doppler";
        spec_total_energy4_doppler = new spectrum_1D(
            name,
            name,
            6000, 0, 6000,
            folder);
        rising_spectra_ptr->push_back(spec_total_energy4_doppler);
    }

#endif // doppler

    //========================================

    name = name_of_this_element  + "_theta_mrad";
    spec_theta_miliradians = new spectrum_1D(name,
            name,
            1000, 0, 100,
            folder);
    rising_spectra_ptr->push_back(spec_theta_miliradians);

#ifdef DOPPLER_CORRECTION_NEEDED

    name = name_of_this_element;
    name += "_energy4MeV_dop";
    spec_energy4_doppler = new spectrum_1D(
        name,
        name,
        6000, 0, 6000,
        folder);
    rising_spectra_ptr->push_back(spec_energy4_doppler);
#endif
}
//**********************************************************************
void Tcrystal_xia::analyse_current_event()
{
    //  cout << " analyse_current_event()  for "
    //        << name_of_this_element
    //        << endl;

    // checking if it is something (not zero) -- is made during unpacking

    zeroing_good_flags();

#ifdef DOPPLER_CORRECTION_NEEDED
    energy4_doppler_corrected = 0;

    for(int i = 0; i < 9; i++)
    {
        energy4_doppler_corrected_betaN[i] = 0;
        betaN_doppler_factor[i] = 0;
    }
#endif

    //----------------
    // so it fired !

    flag_fired =  energy4_raw || time_raw;

    // the time information delivered by XIA electronics
    // this is already the correct time,
    time_raw =
        Trising::tmp_event_unpacked->ger_xia_dgf[cluster_nr][id_number - 1][0];
    energy4_raw =
        Trising::tmp_event_unpacked->ger_xia_dgf[cluster_nr][id_number - 1][1];

#ifdef  WE_WANT_ORIGINAL_DGF_TIME
    time_dgf_raw =
        Trising::tmp_event_unpacked->ger_xia_dgf[cluster_nr][id_number - 1][2];
    time_dgf_cal =  time_dgf_raw; // so far we do not calibrate (25 ns/channel)
#endif

    if(RisingAnalysis_ptr->is_verbose_on())
    {
        cout  << name_of_this_element
              << ", E4raw =  " //<< setw(9)
              << energy4_raw
              << ", T = " //<< setw(9)
              << time_raw ;
        // <<endl; // New line will be after the LP, SR printout
    }

    /* Note: incrementing the raw spectra is done only when multiplicity in this cluster is 1.
       all this is done from Trising class, by calling
       Tcluster_xia::calculate_single_spectrum()
       The "missing" instructions which were here - are moved just there
    */

    //===================================================
    // energy calibration -------------------------------
    energy4_calibrated = 0;
    double energy4_tmp = (energy4_raw) + randomizer::randomek();
    if(energy4_calibr_factors[2] == 0)
    {
        energy4_calibrated = energy4_calibr_factors[0]
                             + (energy4_tmp * energy4_calibr_factors[1]);
    }
    else for(unsigned int i = 0; i < energy4_calibr_factors.size(); i++)
        {
            energy4_calibrated += energy4_calibr_factors[i] *
                                  pow(energy4_tmp, (int)i);
        }


#ifdef  LUCIA_TWO_STEPS_ENERGY_CALIBRATION
    // now we can have the second calibration
    energy4_first_step_calibrated = energy4_calibrated; // to remember

    // we do not need the randomisation second time
    if(energy4_second_calibr_factors[2] == 0)
    {
        energy4_calibrated = energy4_second_calibr_factors[0]
                             + (energy4_first_step_calibrated * energy4_second_calibr_factors[1]);
    }
    else for(unsigned int i = 0; i < energy4_second_calibr_factors.size(); i++)
        {
            energy4_calibrated += energy4_second_calibr_factors[i] *
                                  pow(energy4_first_step_calibrated, (int)i);
        }
#endif //   LUCIA_TWO_STEPS_ENERGY_CALIBRATION


    //=================================================
    // time calibration --------------------------------
    time_calibrated = 0;
    double time_tmp = time_raw + randomizer::randomek();
    if(time_calibr_factors[2] == 0) // simple linear calibration
    {
        time_calibrated = time_calibr_factors[0]
                          + (time_tmp * time_calibr_factors[1]);
    }
    else for(unsigned int i = 0; i < time_calibr_factors.size(); i++)
        {
            time_calibrated += time_calibr_factors[i] * pow(time_tmp, (int)i);
        }




#if (CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005)

    // for g-factor this is a aritmetic formula to know the time difference
    // between registering the
    // particle (scintillator) and registering the gamma.
    time_cal_minus_GeOR_cal =  Trising::xia_P_time_minus_Ge_time_OR_cal -  time_calibrated;
#endif // Gfactor


    //###########################################################
#if ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||   \
      (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN)\
      || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) \
      || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))


    // 1. Take raw time
    // 2. subtract the plastic, actually one of three "trays" and make diff_raw
    // !!!!!!!!! this is done already during unpacking !!!!!!!!!!!!!!!
    // {

    //     int  time_diff_raw = time_raw;
    //- Trising::tmp_event_unpacked->ge_plastic_time_tray[proper_tray_nr];
    //
    //  // 3. calibrate this diff, so we will have diff_cal
    //
    //     // time calibration --------------------------------
    //     time_diff_calibrated = 0;
    //     double time_tmp = time_diff_raw + randomizer::randomek();
    //     if(time_calibr_factors[2]== 0) // simple linear calibration
    //     {
    //       time_diff_calibrated = time_calibr_factors[0]
    //                         + (time_tmp * time_calibr_factors[1]);
    //     }
    //     else for(unsigned int i = 0; i < time_calibr_factors.size(); i++)
    //       {
    //         time_diff_calibrated += time_calibr_factors[i] *
    //           pow(time_tmp, (int)i);
    //       }
    //
    // }
#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED  // mans stopped beam campaign


    //  unpacking and calibration of the time short range, long range

    time_LR_raw =
        Trising::tmp_event_unpacked->ge_time_LR[cluster_nr][id_number - 1];
    //  time_LR_raw = 777; // FAKE

    time_SR_raw =
        Trising::tmp_event_unpacked->ge_time_SR[cluster_nr][id_number - 1];
    //  time_SR_raw =  888;    // Fake


    if(RisingAnalysis_ptr->is_verbose_on())
    {
        cout  << "                "  // name_of_this_element
              << ", time_LR_raw =  " << setw(4) << time_LR_raw
              << ", time_SR_raw = " << setw(4) << time_SR_raw ;
        //<<endl;
    }


    // time LR
    // We use the variable called time_LR_diff, but this diff is
    // not important in case of stopped beam

    // 1. subtract plastic, so we will have LR_diff_raw

    //  time_LR_diff_raw = time_LR_raw -
    //        Trising::tmp_event_unpacked->Plastic_LR_time;
    // 2. calibrate this  LR_diff_call

    // time LR calibration --------------------------------
    time_LR_calibrated = 0;
    time_tmp = time_LR_raw + randomizer::randomek();
    if(time_LR_calibr_factors[2] == 0) // simple linear calibration
    {
        time_LR_calibrated = time_LR_calibr_factors[0]
                             + (time_tmp * time_LR_calibr_factors[1]);
    }
    else for(unsigned int i = 0; i < time_LR_calibr_factors.size(); i++)
        {
            time_LR_calibrated += time_LR_calibr_factors[i] * pow(time_tmp, (int)i);
        }



    // This below is not anymore, because we take the  time for selfgates
    //time_calibrated =  time_LR_diff_calibrated; // because this need
    // for  "good" dialog window, selfgates, etc.


#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE

    for(int i = 0 ; i < 3 ; i++)
    {
        Silicon_LR_trigger_time_minus_LR_time[i] = 0 ;
    }

    // at first we have to know how many of them fired
    int how_many =
        (Trising::tmp_event_unpacked->Silicon_LR_trigger_time[0] != 0)
        +
        (Trising::tmp_event_unpacked->Silicon_LR_trigger_time[1] != 0)
        +
        (Trising::tmp_event_unpacked->Silicon_LR_trigger_time[2] != 0);

    switch(how_many)
    {
    default:
        break;
    case 1:
        Silicon_LR_trigger_time_minus_LR_time[0] =
            Trising::tmp_event_unpacked->Silicon_LR_trigger_time[0] -
            time_LR_calibrated;
        break;
    case 2: // note below : twisted indexes are on purpose!
        Silicon_LR_trigger_time_minus_LR_time[0] =
            Trising::tmp_event_unpacked->Silicon_LR_trigger_time[1] -
            time_LR_calibrated ;

        Silicon_LR_trigger_time_minus_LR_time[1] =
            Trising::tmp_event_unpacked->Silicon_LR_trigger_time[0] -
            time_LR_calibrated ;
        break;
    case 3:
        Silicon_LR_trigger_time_minus_LR_time[0] =
            Trising::tmp_event_unpacked->Silicon_LR_trigger_time[2] -
            time_LR_calibrated;

        Silicon_LR_trigger_time_minus_LR_time[1] =
            Trising::tmp_event_unpacked->Silicon_LR_trigger_time[1] -
            time_LR_calibrated;

        Silicon_LR_trigger_time_minus_LR_time[2] =
            Trising::tmp_event_unpacked->Silicon_LR_trigger_time[0] -
            time_LR_calibrated;
        break;
    } // endof switch

    /*
      if(how_many > 1)    // trap for a breakpoint
      {

        for(int i = 0 ; i < 3 ; i++)
        {
          cout << "Silicon_LR_trigger_time_minus_LR_time i= "
          << i << " is " <<
          Silicon_LR_trigger_time_minus_LR_time[i]
          << endl;
        }
        cout << "Drugie" << endl;
      }
    */

#endif





    // time SR calibration ----------------------------------------------
    // no special tricks with differences -------------------------------

    time_SR_calibrated = 0;
    time_tmp = time_SR_raw + randomizer::randomek();
    if(time_SR_calibr_factors[2] == 0) // simple linear calibration
    {
        time_SR_calibrated = time_SR_calibr_factors[0]
                             + (time_tmp * time_SR_calibr_factors[1]);
    }
    else for(unsigned int i = 0; i < time_SR_calibr_factors.size(); i++)
        {
            time_SR_calibrated += time_SR_calibr_factors[i] * pow(time_tmp, (int)i);
        }

#endif  // LONG_RANGE_SHORT_RANGE_TDC_USED
    if(RisingAnalysis_ptr->is_verbose_on())
    {
        cout << endl;
    }

#endif // FAST BEAM

    //=======================
    // ---incr spectra
    // ---> fragment moved into the function
    //        Tcluster_xia::calculate_single_spectrum


    //if(cluster_nr == 1) energy4_calibrated = 1000; // cheeting


    //================================
    // GOOD
    //================================
    if(
        //     (flag_good_20MeV_requires_threshold  ?
        //      (energy20_calibrated >  en20MeV_threshold)
        //      :
        //      true
        //     )
        //     &&
        (flag_good_4MeV_requires_threshold  ?
         (energy4_calibrated >=  en4MeV_threshold && energy4_calibrated <=  en4MeV_threshold_upper)
         :
         true)
        &&
        (flag_good_time_requires_threshold  ?   // acutally now this is not
         // a threshold, but the gate
         (time_calibrated >=  good_time_threshold_lower)
         :
         true)
        &&
        (flag_good_time_requires_threshold  ?
         (time_calibrated <=  good_time_threshold_upper)
         :
         true)
    )
    {
        flag_good_data = true;
        flag_good_nonaddbacked_crystal = true; // later can made false by the multiaddback routine
        Tcrystal_xia::multiplicity++;
    }




    // this value does not have to be zeroed,because anyway it is validated
    // by the flag
    // good_data, so the value is always there, but the validator sometimes in
    // false

    /*cout
      <<"Cluster " << cluster_nr <<" "<<id_number<<endl
      << energy4_calibrated<<" "<<time_calibrated
      <<" "<<energy20_calibrated<<endl;
    */
#ifdef XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT

    // NOTE id_number starts from 1,   so: 1,2,3,4,5,6,7
    Trising::tmp_event_calibrated->energy_xia_cal[cluster_nr][id_number - 1] =
        energy4_calibrated;

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    Trising::tmp_event_calibrated->time_xia_cal[cluster_nr][id_number - 1] =
        time_cal_minus_GeOR_cal;
#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

#endif // def XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT

    //  cout << " end of analyse_current_event()  for "
    //        << name_of_this_element
    //        << endl;

}

#ifdef DOPPLER_CORRECTION_NEEDED

//**************************************************************************
// This name of the fuction is not good. Should be:
//  chose_and_calculate_doppler()
//**************************************************************************
void  Tcrystal_xia::calculate_chosen_doppler_algorithm()
{
    switch(which_doppler_algorithm)
    {
    case tracked_algorithm:
        if(target_ptr->is_doppler_possible()) doppler_corrector_TVector3();
        break;
    case basic_algorithm:
        doppler_corrector_basic();
        break;
    default:
        cout
                << "Error, the type of doppler correction algorithm was not selected "
                "in the file: option/doppler.options\n (the value is now = "
                << which_doppler_algorithm << ")" << endl;
        break;
    }
}
#endif
//**************************************************************************
void  Tcrystal_xia::make_preloop_action(istream & s)
{
    //check_legality_of_data_pointers();

    // total spectra are in the rising object
    spec_total_theta_miliradians = rising_pointer->
                                   spec_total_theta_miliradians;  // FAKE !!!!!!!!!!!!!!!!!!!!!!!!!!!

    //  spec_total_energy4_doppler= rising_pointer->spec_total_energy4_doppler;
    //  spec_total_energy20_doppler= rising_pointer->spec_total_energy20_doppler;
    spec_fan = rising_pointer->spec_fan;

    // cout << "Reading the calibration for the crystal "
    //        << name_of_this_element
    //        << endl;

    // in this file we look for a string
    int how_many_factors = 3;

    // in this file we look for a string
    //-------------------------------------------------------------------------------
    energy4_calibr_factors.clear();
    string slowo = name_of_this_element + "_energy4MeV_factors";

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo);
    for(int i = 0; i < how_many_factors; i++)
    {
        // otherwise we read the data
        double value;
        s >> value;
        if(!s)
        {
            Treading_value_exception capsule;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value.";
            throw capsule;
        }
        // if ok
        energy4_calibr_factors.push_back(value);
    }

    //-------------------------------------------------------------------------------
#ifdef  LUCIA_TWO_STEPS_ENERGY_CALIBRATION
    energy4_second_calibr_factors.clear();
    slowo = name_of_this_element + "_energy4MeV_second_calibr_factors";

    // first reading the energy calibration
    Tfile_helper::spot_in_file(s, slowo);
    for(int i = 0; i < how_many_factors; i++)
    {
        // otherwise we read the data
        double value;
        s >> value;
        if(!s)
        {
            Treading_value_exception capsule;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value.";
            throw capsule;
        }
        // if ok
        energy4_second_calibr_factors.push_back(value);
    }
#endif //  LUCIA_TWO_STEPS_ENERGY_CALIBRATION
    //-----------------------------------------------------------------------------------

    // reading the time calibration factors --------------
    time_calibr_factors.clear();
    slowo = name_of_this_element + "_time_factors";
    Tfile_helper::spot_in_file(s, slowo);
    for(int i = 0; i < how_many_factors; i++)
    {
        double value;
        s >> value;
        if(!s)
        {
            Treading_value_exception capsule;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value.";
            throw capsule;
        }
        // if ok
        time_calibr_factors.push_back(value);
    }


#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED

    // reading the Short Range and Long Range time calibration factor

    // reading the LONG RANGE time calibration factors --------------

    time_LR_calibr_factors.clear();
    slowo = name_of_this_element + "_time_LR_factors";
    Tfile_helper::spot_in_file(s, slowo);
    for(int i = 0; i < how_many_factors; i++)
    {
        double value;
        s >> value;
        if(!s)
        {
            Treading_value_exception capsule;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value.";
            throw capsule;
        }
        // if ok
        time_LR_calibr_factors.push_back(value);
    }

    // reading the SHORT RANGE time calibration factors --------------

    time_SR_calibr_factors.clear();
    slowo = name_of_this_element + "_time_SR_factors";
    Tfile_helper::spot_in_file(s, slowo);
    for(int i = 0; i < how_many_factors; i++)
    {
        double value;
        s >> value;
        if(!s)
        {
            Treading_value_exception capsule;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value.";
            throw capsule;
        }
        // if ok
        time_SR_calibr_factors.push_back(value);
    }
#endif

    // ---------------------------------------------------
    // Geometry: phi, theta
    // but this is in a different file....
    //----------------------------------------------------

    // cout << " reading the geometry angles  for crystal "
    //        << name_of_this_element
    //        << endl;


    string geometry_file = "calibration/rising_geometry.txt";
    try
    {
        ifstream plik_geometry(geometry_file.c_str());
        if(! plik_geometry)
        {
            cout << "I can't open  file: " << geometry_file;
            exit(-1);
        }
        string keyword = name_of_this_element + "_phi_theta_distance";
        Tfile_helper::spot_in_file(plik_geometry, keyword);
        plik_geometry >> zjedz >> phi_geom
                      >> zjedz >> theta_geom
                      >> zjedz >> distance_from_target;
        if(!plik_geometry)
        {
            cout << "Error while reading (inside geometry file "
                 << geometry_file
                 << ") phi and theta and distance of"
                 << name_of_this_element
                 << "\nI found keyword '"
                 << keyword
                 << "' but error was while reading its (three) values"
                 << " of phi and theta."
                 << endl;
            exit(-1);
        }
    }
    catch(Tfile_helper_exception &k)
    {
        cerr << "Error while reading geometry file "
             << geometry_file
             << ":\n"
             << k.message << endl;
        exit(-1);
    }

    distance_from_target *= 10; // it was in [cm] and we need in milimeters !
    phi_geom_radians   =  M_PI * phi_geom / 180.0;
    theta_geom_radians =  M_PI * theta_geom / 180.0;

    // ============== recalculate phi and theta into x,y,z of the crystal
    crystal_position_x = distance_from_target * sin(theta_geom_radians) *
                         cos(phi_geom_radians);
    crystal_position_y = distance_from_target * sin(theta_geom_radians) *
                         sin(phi_geom_radians);
    crystal_position_z = distance_from_target * cos(theta_geom_radians);

    // doppler correction options are read by target and stored
    // in the static members of this clas (Tcrystal_xia)

#ifdef DOPPLER_CORRECTION_NEEDED
    betaN_doppler_factor.resize(9);
#endif  // def DOPPLER_CORRECTION_NEEDED

}
//**********************************************************************
void Tcrystal_xia::set_event_ptr_for_crystal_data(int ptr[3])
{}
//**********************************************************************
void Tcrystal_xia::check_legality_of_data_pointers()
{}


#ifdef DOPPLER_CORRECTION_NEEDED
//*******************************************************************
void Tcrystal_xia::set_doppler_corr_options
(int typ_track, int typ_cate_pos, int typ_doppl_algor)
{
    tracking_position_on_target = (type_of_tracking_requirements) typ_track;
    tracking_position_on_cate   = (type_of_cate_requirements) typ_cate_pos;
    which_doppler_algorithm = (type_of_doppler_algorithm)  typ_doppl_algor;
}
#endif   // DOPPLER_CORRECTION_NEEDED



#ifdef DOPPLER_CORRECTION_NEEDED
//***************************************************************
/** Based on the simplest algorithm (from Hans-Juergen) */
void Tcrystal_xia::doppler_corrector_basic()
{
    // this algorithm below is on the web page ===================
    double beta = target_ptr->give_beta_after_target();

    //energy4_doppler_corrected = energy4_calibrated
    doppler_factor = (1.0 - beta * cos(theta_geom_radians))
                     / (sqrt(1.0 - beta * beta));


    if(target_ptr->is_beta_playfield_enabled())
    {
        for(int n = 0; n < 9; n++)
        {
            beta =  target_ptr->give_betaN_after_target(n);

            betaN_doppler_factor[n] = (1.0 - beta * cos(theta_geom_radians))
                                      / (sqrt(1.0 - beta * beta));
        }
    }


    flag_doppler_successful = true;
}
#endif  // DOPPLER_CORRECTION_NEEDED

//*******************************************************************************
/** No descriptions */
void Tcrystal_xia::zeroing_good_flags()
{
    flag_fired =
        flag_good_data =   false;
    flag_participant_of_addback = false;
    flag_good_nonaddbacked_crystal = false;

#ifdef DOPPLER_CORRECTION_NEEDED
    flag_doppler_successful =   false;
#endif // def DOPPLER_CORRECTION_NEEDED


#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE
    for(int i = 0 ; i < 3 ; i++)
    {
        Silicon_LR_trigger_time_minus_LR_time[i] = 0 ;
    }
#endif
}
/******************************************************************************/
bool Tcrystal_xia::check_selfgate(Tself_gate_abstract_descr *desc)
{
    // here we assume that this is really germ
    Tself_gate_ger_descr *d = (Tself_gate_ger_descr*)desc;
    if(d->enable_en4gate)
    {
        if(energy4_calibrated < d->en4_gate[0]
                ||
                energy4_calibrated > d->en4_gate[1])
            return false;
    }
    // //   if(d->enable_en20gate)
    // //   {
    // //     if(energy20_calibrated < d->en20_gate[0]
    // //         ||
    // //         energy20_calibrated > d->en20_gate[1])
    // //       return false;
    // //   }

    //---------------------------------------
    //   Two versions of time selfate. One is the classical one. The other
    //   is about the time difference between two
    //       gammas - o X axis and Y axis

    if(d->enable_time_gate)
    {
        // CLASSICAL
        if(d->enable_xy_gamma_time_difference == false)
        {
            if(time_calibrated < d->time_gate[0] ||
                    time_calibrated > d->time_gate[1])
                return false;
        }
        else   // DIFFERENCE time gate
        {
            //       here we check if this is for X axis - we just store
            // the time value

            if(d->flag_this_is_for_X_incrementer)
            {
                d->safe_for_x_time = time_calibrated;
            }
            else       // if it is an Y axis - we make the difference
            {
                double diff = fabs(time_calibrated -  d->safe_for_x_time);
                if(diff < d->time_gate[0] || diff > d->time_gate[1])
                    return false;  // out of the time range
            }
        }
    }

    // banana gate according to Marek Pfutzner idea
    if(d->enable_energy_time_polygon_gate)
    {
        double selected_time = 0 ;
        switch(d->which_time)
        {
        default:
        case Tself_gate_ger_descr::time_cal:
            selected_time = time_calibrated;
            break;
#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
        case Tself_gate_ger_descr::LR_time_cal:
            selected_time = time_LR_calibrated;
            break;
        case Tself_gate_ger_descr::SR_time_cal:
            selected_time = time_SR_calibrated;
            break;
#endif // LONG_RANGE_SHORT_RANGE_TDC_USED

        }

        if(d->polygon->Test(selected_time, energy4_calibrated) == false)
        {
            //cout << " is outside " << endl;;
            return false;
        }
    }



#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
    if(d->enable_LR_time_gate)
    {
        if(time_LR_calibrated < d->LR_time_gate[0] ||
                time_LR_calibrated > d->LR_time_gate[1])
            return false;
    }

    if(d->enable_SR_time_gate)
    {
        if(time_SR_calibrated < d->SR_time_gate[0] ||
                time_SR_calibrated > d->SR_time_gate[1])
            return false;
    }
#endif // LONG_RANGE_SHORT_RANGE_TDC_USED

    if(d->enable_geom_theta_gate)
    {
        if(theta_geom < d->geom_theta_gate[0] ||
                theta_geom > d->geom_theta_gate[1])
            return false;
    }
    if(d->enable_geom_phi_gate)
    {
        if(phi_geom < d->geom_phi_gate[0] ||
                phi_geom > d->geom_phi_gate[1])
            return false;
    }
    if(d->enable_gp_theta_gate)
    {
        if(theta_radians_between_gamma_vs_scattered_particle <  d->gp_theta_gate[0]
                ||
                theta_radians_between_gamma_vs_scattered_particle >
                d->gp_theta_gate[1])
            return false;
    }
    if(d->enable_gp_phi_gate)
    {
        if(phi_radians_between_gamma_vs_scattered_particle  < d->gp_phi_gate[0]
                ||
                phi_radians_between_gamma_vs_scattered_particle > d->gp_phi_gate[1])
            return false;
    }
    if(d->enable_mult_in_cluster_gate)
    {
        int mult =  parent_cluster->get_this_cluster_multiplicity();
        if(mult < d->mult_in_cluster_gate[0]
                ||
                mult > d->mult_in_cluster_gate[1]
          )
            return false;
    }
#ifdef BGO_PRESENT
    if(d->enable_BGO_in_cluster_gate)
    {
        int BGO_energy =  parent_cluster->get_this_cluster_BGO_energy();
        if(BGO_energy < d->BGO_in_cluster_gate[0]
                ||
                BGO_energy > d->BGO_in_cluster_gate[1]
          )
            return false;
    }
#endif

    return true; // when EVETHING was successful
}

//************************************************************************
/** No descriptions */
void Tcrystal_xia::increment_crystal_spectra_raw_and_fan()
{
    /*cout
    <<"Cluster " << cluster_nr <<" "<<id_number<<endl
    << energy4_calibrated<<" "<<time_calibrated
    <<" "<<energy20_calibrated<<endl;
    */

#ifdef  LONG_RANGE_SHORT_RANGE_TDC_USED

    int fan_offset = (100 * cluster_nr) + (10 * id_number) + 6;

    if(energy4_raw) spec_fan->manually_increment(fan_offset + 0);
    if(time_LR_raw) spec_fan->manually_increment(fan_offset + 1);
    if(time_SR_raw) spec_fan->manually_increment(fan_offset + 2);

    if(energy4_raw  && time_LR_raw) // This is what Stephane asked for.
        spec_fan->manually_increment(fan_offset + 3);

    if(Trising::tmp_event_unpacked->Plastic_LR_time)
        spec_fan->manually_increment(fan_offset + 4); // Stephane asked for
    // common plastic as well


#else   // old style fan spectrum
    int fan_offset = (100 * cluster_nr) + (10 * id_number) + 6;
    if(energy4_raw) spec_fan->manually_increment(fan_offset + 0);
    //  if(*energy20_data) spec_fan->manually_increment(fan_offset + 1);
    if(time_raw) spec_fan->manually_increment(fan_offset + 1);
    if(energy4_raw
            //  &&
            //  *energy20_data
            &&
            time_raw)
        spec_fan->manually_increment(fan_offset + 2);
#endif


    // ----------

    if(time_raw)spec_time_raw->manually_increment(time_raw);
    if(energy4_raw)spec_energy4_raw->manually_increment(energy4_raw);

#ifdef  WE_WANT_ORIGINAL_DGF_TIME
    if(time_dgf_raw)
    {
        spec_time_dgf_raw->manually_increment(time_dgf_raw);
        spec_time_dgf_cal->manually_increment(time_dgf_cal);  // calibration is 25 ns/channel
    }
#endif

    if(time_calibrated)spec_time_cal->manually_increment(time_calibrated);

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    if(time_cal_minus_GeOR_cal)spec_time_cal_minus_GeOR_cal->
        manually_increment(time_cal_minus_GeOR_cal);
#endif

#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
    if(time_SR_raw)spec_time_SR_raw->manually_increment(time_SR_raw);
    if(time_LR_raw)spec_time_LR_raw->manually_increment(time_LR_raw);

    if(time_SR_calibrated)
        spec_time_SR_cal->manually_increment(time_SR_calibrated);
    if(time_LR_calibrated)
        spec_time_LR_cal->manually_increment(time_LR_calibrated);

    if(time_LR_calibrated)
        spec_total_time_LR_cal->manually_increment(time_LR_calibrated);
    if(time_SR_calibrated)
        spec_total_time_SR_cal->manually_increment(time_SR_calibrated);

#ifdef TIME_TIME_MATRICES_FOR_STEPHANE_PETRI
    if(time_LR_raw && energy4_raw)
        spec_energy_vs_LR_time->manually_increment(time_LR_raw, energy4_raw);
    if(time_LR_raw && time_SR_raw)
        spec_LR_time_vs_SR_time->manually_increment(time_SR_raw, time_LR_raw);
    if(time_LR_raw && time_raw)
        spec_LR_time_vs_dgf_time->manually_increment(time_LR_raw, time_raw);
#endif


#endif

}
//**************************************************************************
/** No descriptions */
void Tcrystal_xia::increment_crystal_spectra_cal_and_doppler()
{
    /*cout
    <<"Cluster " << cluster_nr <<" "<<id_number<<endl
    << energy4_calibrated<<" "<<time_calibrated
    <<" "<<energy20_calibrated<<endl;
    */

    sum_en4_cal += energy4_calibrated;


    // incrementing calibrated versions of the spectrum

    if(energy4_raw != 0  || flag_increment_4MeV_cal_with_zero)
    {
        spec_energy4_cal->manually_increment(energy4_calibrated);
        spec_total_energy4_cal->manually_increment(energy4_calibrated);

    }



    if(time_raw != 0  || flag_increment_time_cal_with_zero)
    {
        spec_time_cal->manually_increment(time_calibrated);
        spec_total_time_cal->manually_increment(time_calibrated);
    }


#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    if(time_cal_minus_GeOR_cal != 0  ||  flag_increment_time_cal_with_zero)
    {
        spec_time_cal_minus_GeOR_cal->manually_increment(time_cal_minus_GeOR_cal);
        spec_total_time_cal_minus_GeOR_cal->
        manually_increment(time_cal_minus_GeOR_cal);
    }
#endif

#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
#ifdef TIME_DIFF_USED
    spec_time_diff_cal->manually_increment(time_diff_calibrated);
    //      if(local_mult_tmp > 1)
    //      {
    //        cout << "local_mult_tmp = " << local_mult_tmp << endl;
    //        }
#endif
#endif


#ifdef DOPPLER_CORRECTION_NEEDED

    // Doppler corrected spectra we do only when flag  GOOD is set

    if(flag_good_data == true)
    {

        calculate_chosen_doppler_algorithm();
        if(flag_doppler_successful)
        {
            energy4_doppler_corrected = energy4_calibrated * doppler_factor;

            // for beta playfield

            if(target_ptr->is_beta_playfield_enabled())
            {
                for(int n = 0; n < 9; n++)
                {
                    energy4_doppler_corrected_betaN[n] =
                        energy4_calibrated * betaN_doppler_factor[n];

                }
            }


            spec_energy4_doppler->manually_increment(energy4_doppler_corrected);
            spec_total_energy4_doppler->
            manually_increment(energy4_doppler_corrected);

            sum_en4_dop += energy4_doppler_corrected;

            spec_theta_miliradians->
            manually_increment(theta_radians_between_gamma_vs_scattered_particle);
            spec_total_theta_miliradians->
            manually_increment(theta_radians_between_gamma_vs_scattered_particle);
        }
    }
#endif  // DOPPLER_CORRECTION_NEEDED


}
//************************************************************************
/** No descriptions */
void Tcrystal_xia::increment_spect_total_addbacked_cal_and_doppler()
{
    // incrementing calibrated versions of the spectrum

    if(energy4_raw != 0  || flag_increment_4MeV_cal_with_zero)
    {
        //spec_energy4_cal->manually_increment(energy4_calibrated );
        rising_pointer->
        increm_spec_total_energy4_calibrated_addbacked(energy4_calibrated);

    }
    //   if(energy20_raw !=0  || flag_increment_20MeV_cal_with_zero)
    //   {
    //     //   spec_energy20_cal->manually_increment(energy20_calibrated );
    //     rising_pointer->
    //  increm_spec_total_energy20_calibrated_addbacked(energy20_calibrated);
    //   }


#ifdef DOPPLER_CORRECTION_NEEDED

    // Doppler corrected spectra we do only when flag  GOOD is set

    if(flag_good_data == true)
    {

        calculate_chosen_doppler_algorithm();
        if(flag_doppler_successful)
        {
            energy4_doppler_corrected = energy4_calibrated * doppler_factor;

            rising_pointer->
            increm_spec_total_energy4_doppler_addbacked(
                energy4_doppler_corrected);
        }
    }
#endif   // DOPPLER_CORRECTION_NEEDED

}
//************************************************************************
/** this function contains the algorithm of the doppler correction
using the class TVector3 from ROOT library */

#ifdef DOPPLER_CORRECTION_NEEDED
void Tcrystal_xia::doppler_corrector_TVector3()
{

#ifndef CATE_PRESENT
    cout << "The doppler correction algorithm other that so called 'basic' "
         "- is not possible when the CATE  detector is not present"
         "Please go to cracow: spy_options->doppler correction\n"
         "and change the algorithm to 'basic'"
         << endl;
#endif
    //detector.SetMagThetaPhi(1, theta_geom_radians, phi_geom_radians);

    detector.SetXYZ(crystal_position_x - target_ptr->give_x_tracked() ,
                    crystal_position_y - target_ptr->give_y_tracked() ,
                    crystal_position_z);


    dcout << "==== Gamma registered during this event ===================\n"
          << name_of_this_element
          << " (Geometrical phi " << phi_geom << "[deg],  rad= " << phi_geom_radians
          << " theta " << theta_geom << " [deg],  rad= " << theta_geom_radians << ")\n"
          << "\n: Gamma vector   \n\tdetector.x = " << detector.X()
          << ", (because crystal_position_x = " << crystal_position_x
          << " -  target x = " << target_ptr->give_x_tracked()
          << ")\n\tyv1 = " <<  detector.Y()
          << ", (because crystal_position_y = " << crystal_position_y
          << " -  target y = " << target_ptr->give_y_tracked()
          << ")\n\tzv1 = " <<  detector.Z()
          << ", (because  crystal_position_z = " << crystal_position_z
          //<< " -  target z = " << target_ptr->give_z()
          << ")" << endl;


    theta_radians_between_gamma_vs_scattered_particle =
        detector.Angle(target_ptr->outgoing);
    double cos_theta_gamma_scattered_particle =
        cos(theta_radians_between_gamma_vs_scattered_particle);

    double beta = target_ptr->give_beta_after_target();

    dcout
            << "TVector3  cos_theta_gamma_scattered_particle "
            << cos_theta_gamma_scattered_particle
            << ", \ntheta_radians_between_gamma_vs_scattered_particle "
            << theta_radians_between_gamma_vs_scattered_particle
            << "\n, Beta = " << beta << endl;


    // using Alex way -------------------
    if(beta > 0.05  &&   beta < 1.0)
    {
        //energy4_doppler_corrected := energy4_calibrated * doppler_factor
        doppler_factor = (1.0 - (beta * cos_theta_gamma_scattered_particle))
                         / (sqrt(1.0 - (beta * beta)));

        flag_doppler_successful = true;
    }
    else
    {
        flag_doppler_successful = false;
    }
    dcout << "Doppler factor = " << doppler_factor << endl;

    // ------------- for beta playfield--------
    if(target_ptr->is_beta_playfield_enabled())
    {
        for(int n = 0; n < 9; n++)
        {
            beta =  target_ptr->give_betaN_after_target(n);
            betaN_doppler_factor[n]  =
                (1.0 - (beta *
                        cos_theta_gamma_scattered_particle
                       ))
                / (sqrt(1.0 - (beta * beta)));
        }
    }




    /*
    if(name_of_this_element == "cluster_crys_L_5")
    {
    cout
    << "TVector3  cos_theta_gamma_scattered_particle " << cos_theta_gamma_scattered_particle
    << ", \ntheta_radians_between_gamma_vs_scattered_particle "
    << theta_radians_between_gamma_vs_scattered_particle
    << ", in degree: "
    <<  (theta_radians_between_gamma_vs_scattered_particle * 180 / M_PI)
    << ", Det geom " << theta_geom << " degree "
    << endl;
    }
    */

}
#endif  // def DOPPLER_CORRECTION_NEEDED
