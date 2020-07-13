#include "Trising.h"
#include <dirent.h> // for scandir
#include <malloc.h> // for scandir

#include <sstream>
#include "TIFJAnalysis.h"
#include "Tnamed_pointer.h"
TIFJCalibratedEvent* Trising::tmp_event_calibrated;
TIFJEvent* Trising::tmp_event_unpacked;


//####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
//####################################################################

// static
// For g-factor experiments (P means particle)
int Trising::xia_P_time_minus_Ge_time_OR_raw;
double Trising::xia_P_time_minus_Ge_time_OR_cal;

int Trising::vxi_P_time_minus_Ge_time_OR_raw;
double Trising::vxi_P_time_minus_Ge_time_OR_cal;


double Trising::vxi_LR_time_calibrated_by_the_first_who_fired;

#endif

//extern int local_mult_tmp ;
const string Trising::cluster_characters =
    (HOW_MANY_GE_CLUSTERS == 8) ? ("ABCDGJKL")
    : ("ABCDEFGJKLMNPQR") ;
//*******************************************************
//*******************************************************
//         RISING class
//*******************************************************
//*******************************************************
Trising::Trising()
{
    //cout << "\n\n\n\nConstructor of the Rising Class =========================== " << endl ;
    //Tcrystal::rising_pointer = this ; // for crystals

    Tcrystal_xia::rising_pointer = this ; // for crystals
    Tcrystal::rising_pointer = this ; // for crystals

    Tcluster::rising_pointer = this ; // for cluster
    Tcluster_xia::rising_pointer = this ; // for cluster

    TIFJAnalysis* ptr_anal = TIFJAnalysis;
//     dynamic_cast<TIFJAnalysis*> (TGo4Analysis::Instance());

    frs_pointer = ptr_anal->give_frs_ptr()  ;  // we will need if to ask traget for theta angles
#ifdef HECTOR_PRESENT

    hector_pointer = ptr_anal->give_hector_ptr()  ;  // we will need if to ask traget for theta angles
#endif

    //---------------------------------------------
    // constructing the current setup of Rising
    //---------------------------------------------

#if CURRENT_EXPERIMENT_TYPE == RISING_FAST_BEAM_CAMPAIGN
    // normal old Rising setup
    detector_vxi.push_back(new Tcluster("cluster_crys_A", 0));
    detector_vxi.push_back(new Tcluster("cluster_crys_B", 1));
    detector_vxi.push_back(new Tcluster("cluster_crys_C", 2));
    detector_vxi.push_back(new Tcluster("cluster_crys_D", 3));
    detector_vxi.push_back(new Tcluster("cluster_crys_E", 4));
    detector_vxi.push_back(new Tcluster("cluster_crys_F", 5));
    detector_vxi.push_back(new Tcluster("cluster_crys_G", 6));
    detector_vxi.push_back(new Tcluster("cluster_crys_J", 7));
    detector_vxi.push_back(new Tcluster("cluster_crys_K", 8));
    detector_vxi.push_back(new Tcluster("cluster_crys_L", 9));
    detector_vxi.push_back(new Tcluster("cluster_crys_M", 10));
    detector_vxi.push_back(new Tcluster("cluster_crys_N", 11));
    detector_vxi.push_back(new Tcluster("cluster_crys_P", 12));
    detector_vxi.push_back(new Tcluster("cluster_crys_Q", 13));
    detector_vxi.push_back(new Tcluster("cluster_crys_R", 14));
#elif CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

    detector_vxi.push_back(new Tcluster("cluster_crys_A", 0));
    detector_vxi.push_back(new Tcluster("cluster_crys_B", 1));
    detector_vxi.push_back(new Tcluster("cluster_crys_C", 2));
    detector_vxi.push_back(new Tcluster("cluster_crys_D", 3));

    detector_vxi.push_back(new Tcluster("cluster_crys_G", 4));
    detector_vxi.push_back(new Tcluster("cluster_crys_J", 5));
    detector_vxi.push_back(new Tcluster("cluster_crys_K", 6));
    detector_vxi.push_back(new Tcluster("cluster_crys_L", 7));


    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_A", 0));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_B", 1));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_C", 2));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_D", 3));

    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_G", 4));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_J", 5));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_K", 6));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_L", 7));

#elif ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||         (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008)\
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008)  \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))


    // only xia (dgf) no VXI electronics
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_A", 0));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_B", 1));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_C", 2));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_D", 3));

    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_E", 4));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_F", 5));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_G", 6));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_J", 7));

    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_K", 8));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_L", 9));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_M", 10));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_N", 11));

    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_P", 12));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_Q", 13));
    detector_xia.push_back(new Tcluster_xia("cluster_xia_crys_R", 14));

#endif

    create_my_spectra() ;

    // conditional spectra are created/recreated during preloop
#ifdef VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT
    named_pointer["Rising_crystal_multiplicity"]
    = Tnamed_pointer(&Tcrystal::multiplicity, 0, this) ;
    named_pointer["Rising_all_clusters_multiplicity"]
    = Tnamed_pointer(&Tcluster::all_clusters_multiplicity, 0, this) ;
    named_pointer["Rising_crystal_sum_energy4MeV_cal"]
    = Tnamed_pointer(&Tcrystal::sum_en4_cal, 0, this) ;
    named_pointer["Rising_crystal_sum_energy4MeV_doppler"]
    = Tnamed_pointer(&Tcrystal::sum_en4_dop, 0, this) ;
#endif

#ifdef XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT

    named_pointer["Rising_crystal_xia_multiplicity"]
    = Tnamed_pointer(&Tcrystal_xia::multiplicity, 0, this) ;
    named_pointer["Rising_all_clusters_xia_multiplicity"]
    = Tnamed_pointer(&Tcluster_xia::all_clusters_multiplicity, 0, this) ;
    named_pointer["Rising_crystal_xia_sum_energy4MeV_cal"]
    = Tnamed_pointer(&Tcrystal_xia::sum_en4_cal, 0, this) ;
#endif

#ifdef DOPPLER_CORRECTION_NEEDED
    named_pointer["Rising_crystal_xia_sum_energy4MeV_doppler"]
    = Tnamed_pointer(&Tcrystal_xia::sum_en4_dop, 0, this) ;
#endif


    named_pointer["ger_synchro_data"]
    = Tnamed_pointer(&ger_synchro_data, 0, this) ;

#ifdef MINIBALL_PRESENT
    for(int i = 0 ; i < 10 ; i++)
    {
        string nam("cluster_delivered_by_miniball[");
        nam += (char('0' + i));
        nam += "]" ;

        //      cout << "adding name " << nam << endl;
        named_pointer[nam]
        = Tnamed_pointer(&cluster_delivered_by_miniball[i], 0, this) ;
    }
#endif  // MINIBALL_PRESENT

    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################
    named_pointer["vxi_P_time_minus_Ge_time_OR_raw"]
    = Tnamed_pointer(&vxi_P_time_minus_Ge_time_OR_raw, 0, this) ;

    named_pointer["vxi_P_time_minus_Ge_time_OR_cal"]
    = Tnamed_pointer(&vxi_P_time_minus_Ge_time_OR_cal, 0, this) ;

    named_pointer["xia_P_time_minus_Ge_time_OR_raw"]
    = Tnamed_pointer(&xia_P_time_minus_Ge_time_OR_raw, 0, this) ;

    named_pointer["xia_P_time_minus_Ge_time_OR_cal"]
    = Tnamed_pointer(&xia_P_time_minus_Ge_time_OR_cal, 0, this) ;



    named_pointer["vxi_LR_time_calibrated_by_the_first_who_fired"]
    = Tnamed_pointer(&vxi_LR_time_calibrated_by_the_first_who_fired, 0, this) ;
    //####################################################################
#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################


    // for Adam
    named_pointer["timestamp_GER_minus_timestamp_FRS__WHEN_both_exists"]
    = Tnamed_pointer(&timestamp_GER_minus_timestamp_FRS,
                     &timestamp_GER_minus_timestamp_FRS_possible, this) ;

    named_pointer["timestamp_DGF_minus_timestamp_FRS__WHEN_both_exists"]
    = Tnamed_pointer(&timestamp_DGF_minus_timestamp_FRS,
                     &timestamp_DGF_minus_timestamp_FRS_possible, this) ;

#if CURRENT_EXPERIMENT_TYPE==RISING_STOPPED_BEAM_CAMPAIGN

    named_pointer["plastic_time_LR_raw"] =
        Tnamed_pointer(&plastic_time_LR_raw, 0 , this) ;

    named_pointer["rising_trigger"] =
        Tnamed_pointer(&trigger, 0 , this) ;

#endif


#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE

    // array of 3 elements - three possible hits
    named_pointer[
        "cluster__Silicon_LR_trigger_time_minus_Plastic_LR_time[0]"] =
            Tnamed_pointer(&Silicon_LR_trigger_time_minus_Plastic_LR_time[0],
                           0, this) ;
    //-----------
    named_pointer[
        "cluster__Silicon_LR_trigger_time_minus_Plastic_LR_time[1]"] =
            Tnamed_pointer(&Silicon_LR_trigger_time_minus_Plastic_LR_time[1],
                           0, this) ;
    //-----------
    named_pointer[
        "cluster__Silicon_LR_trigger_time_minus_Plastic_LR_time[2]"] =
            Tnamed_pointer(&Silicon_LR_trigger_time_minus_Plastic_LR_time[2],
                           0, this) ;

#endif



}
//*************************************************************************
void Trising::create_my_spectra()
{

    string folder = "clusters/" ;

    //=============== calibrated spectra (for Gold lines) =====
    // energy cal ------------------------------------
    string name_of_this_element = "Rising totals";
    string name;

#ifdef VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT
    name = "cluster_total_energy4MeV_cal" ;
    spec_total_energy4_cal = new spectrum_1D(
        name,
        name,
        4096, 0, 4096,
        folder);
    spectra_rising.push_back(spec_total_energy4_cal) ;

    name = "cluster_total_energy20MeV_cal" ;
    spec_total_energy20_cal = new spectrum_1D(
        name,
        name,
        5000, 0, 25000,
        folder);
    spectra_rising.push_back(spec_total_energy20_cal) ;

    // time ------------------------------

    name = "cluster_total_time_cal" ;
    spec_total_time_cal = new spectrum_1D(name,
                                          name,
                                          4096, 0, 8192,
                                          folder);
    spectra_rising.push_back(spec_total_time_cal) ;




    name =  "cluster_total_time_cal_minus_GeOR_cal";
    spec_cluster_total_time_cal_minus_GeOR_cal =
        new spectrum_1D(name,
                        name,
                        8192, -8192, 8192,
                        folder);
    spectra_rising.push_back(spec_cluster_total_time_cal_minus_GeOR_cal) ;

#endif


    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################


    // PARTICLE

    // XIA
    name = "cluster_xia_P_time_minus_Ge_time_OR_cal" ;
    spec_xia_P_time_minus_Ge_time_OR_cal =
        new spectrum_1D(name,
                        name,
                        8192, 0, 8192,
                        folder,   "Common (cal) OR signal from  XIA_DGF electronics");
    spectra_rising.push_back(spec_xia_P_time_minus_Ge_time_OR_cal) ;

    name = "cluster_xia_P_time_minus_Ge_time_OR_raw" ;
    spec_xia_P_time_minus_Ge_time_OR_raw =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,  "Common (raw) OR signal from  XIA_DGF electronics");
    spectra_rising.push_back(spec_xia_P_time_minus_Ge_time_OR_raw) ;
    // VXI

    name = "cluster_vxi_P_time_minus_Ge_time_OR_raw" ;
    spec_vxi_P_time_minus_Ge_time_OR_raw =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder,  "Common (raw) OR signal from  VXI electronics");
    spectra_rising.push_back(spec_vxi_P_time_minus_Ge_time_OR_raw) ;


    name = "cluster_vxi_P_time_minus_Ge_time_OR_cal" ;
    spec_vxi_P_time_minus_Ge_time_OR_cal =
        new spectrum_1D(name,
                        name,
                        8192, 0, 8192,
                        folder, "Common (cal) OR signal from  VXI electronics");
    spectra_rising.push_back(spec_vxi_P_time_minus_Ge_time_OR_cal) ;


    name = "cluster_vxi_LR_time_calibrated_by_the_first_who_fired" ;
    spec_vxi_LR_time_calibrated_by_the_first_who_fired =
        new spectrum_1D(name,
                        name,
                        8192, 0, 8192,
                        folder, "Common LR TAC signal from  VXI electronics");
    spectra_rising.push_back(spec_vxi_LR_time_calibrated_by_the_first_who_fired) ;


    //####################################################################
#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################

    //===============================================

    name = "cluster_total_theta_mrad"  ;
    spec_total_theta_miliradians = new spectrum_1D(name,
            name,
            1000, 0, 100,
            folder);
    spectra_rising.push_back(spec_total_theta_miliradians) ;

#ifdef DOPPLER_CORRECTION_NEEDED

    name = "cluster_total_energy4MeV_doppler"  ;
    spec_total_energy4_doppler = new spectrum_1D(name,
            name,
            4096, 0, 4096,
            folder);
    spectra_rising.push_back(spec_total_energy4_doppler) ;

    name = "cluster_total_energy20MeV_doppler"  ;
    spec_total_energy20_doppler = new spectrum_1D(name,
            name,
            5000, 0, 25000,
            folder);
    spectra_rising.push_back(spec_total_energy20_doppler) ;
#endif  // def DOPPLER_CORRECTION_NEEDED

    // any other ?

    name = "cluster_total_energy4MeV_cal_addbacked"  ;
    spec_total_energy4_cal_addbacked = new spectrum_1D(name,
            name,
            4096, 0, 4096,
            folder,
            "Normal + addback");
    spectra_rising.push_back(spec_total_energy4_cal_addbacked) ;


    name = "cluster_total_energy20MeV_cal_addbacked"  ;
    spec_total_energy20_cal_addbacked = new spectrum_1D(name,
            name,
            5000, 0, 25000,
            folder);
    spectra_rising.push_back(spec_total_energy20_cal_addbacked) ;


#ifdef DOPPLER_CORRECTION_NEEDED

    name = "cluster_total_energy4MeV_doppler_addbacked"  ;
    spec_total_energy4_dop_addbacked = new spectrum_1D(name,
            name,
            4096, 0, 4096,
            folder);
    spectra_rising.push_back(spec_total_energy4_dop_addbacked) ;


    name = "cluster_total_energy20MeV_doppler_addbacked"  ;
    spec_total_energy20_dop_addbacked = new spectrum_1D(name,
            name,
            5000, 0, 25000,
            folder);
    spectra_rising.push_back(spec_total_energy20_dop_addbacked) ;
#endif  // def DOPPLER_CORRECTION_NEEDED


    // FAN spectrum of all crystals----------------------------------------
    name = "cluster_fan"  ;
    spec_fan = new spectrum_1D(
        name,
        name,
        2000, 0, 2000,
        folder,
#if CURRENT_EXPERIMENT_TYPE == RISING_FAST_BEAM_CAMPAIGN
        "A B C D E F G J K L M N P Q R  (4MeV, 20MeV, FT, all together)"
#elif CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
        "A B C D   G J K L   (4MeV, 20MeV, FT, all together, 0, 0, EnergyXia, TimeXia, together)"

#elif ( (CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN)  ||         (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN)\
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))

        "A B C D E F G J K L M N P Q R  (peaks mean a presence of: Energy, LR time, SR time, E+LR together, Plastic)"
#else
        "Unknown experiment???" or any nonsense definition ?
#endif
    );
    spectra_rising.push_back(spec_fan) ;

#ifdef VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT
    name = "cluster_multiplicity"  ;
    spec_clusters_vxi_multiplicity = new spectrum_1D(
        name,
        name,
        HOW_MANY_GE_CLUSTERS, 0, HOW_MANY_GE_CLUSTERS,
        folder);
    spectra_rising.push_back(spec_clusters_vxi_multiplicity) ;
#endif

#ifdef XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
    name = "cluster_xia_multiplicity";
    spec_clusters_xia_multiplicity = new spectrum_1D(
        name,
        name,
        HOW_MANY_GE_CLUSTERS, 0, HOW_MANY_GE_CLUSTERS,
        folder);
    spectra_rising.push_back(spec_clusters_xia_multiplicity) ;
#endif

    name = "ge_data_for_synchronisation"  ;
    spec_ger_synchro = new spectrum_1D(
        name,
        name,
        512, 1, 8192,
        folder);
    spectra_rising.push_back(spec_ger_synchro) ;

    name = "ge_crate2_data_for_synchronisation"  ;
    spec_ger2_synchro = new spectrum_1D(
        name,
        name,
        512, 1, 8192,
        folder);
    spectra_rising.push_back(spec_ger2_synchro) ;


    name = "ge_crate2_vs_crate1_synchronisation"  ;
    spec_synch_crate2_vs_crate1 = new spectrum_2D(
        name,
        name,
        512, 1, 8192,
        512, 1, 8192,
        folder);
    spectra_rising.push_back(spec_synch_crate2_vs_crate1) ;

#if CURRENT_EXPERIMENT_TYPE==RISING_STOPPED_BEAM_CAMPAIGN
    name = "plastic_time_LR_raw"  ;
    spec_plastic_time_LR_raw = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);
    spectra_rising.push_back(spec_plastic_time_LR_raw) ;
#endif


}
//*************************************************************************
void Trising::make_preloop_action(TIFJEvent* event_unpacked ,
                                  TIFJCalibratedEvent* event_calibrated)
{
    tmp_event_unpacked =    event_unpacked ;
    tmp_event_calibrated =    event_calibrated ;
    //   cout << "Trising::make_preloop_action "  << endl;

    // first we inform where are the data coresponding to these devices
    Tcrystal::target_ptr = frs_pointer->give_target_address() ;
    Tcrystal_xia::target_ptr = frs_pointer->give_target_address() ;

    cate_ptr = NULL; //Tcrystal::target_ptr->give_cate_ptr() ;

    //simulate_event();

    // loop over the enumeration type
    //    for(unsigned int i = 0 ; i < detector.size() ; i++)
    //    {
    //        // cout << "cluster nr  =  " << i << endl;
    //        // remember the source of data
    //        detector[i]->set_event_data_pointers(&event_unpacked->Cluster_data[i]);
    //    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // very important:
    // calibrations and gates are read everytime we start the analysis


    read_calibration_and_gates_from_disk();
    read_good_crystal_specification();
    read_addback_options_for_clusters();

    // zeroing monitoring statistics for the watchdog
    memset(&fire_statistics[0][0], 0,  sizeof(fire_statistics));
}
//**********************************************************************
void Trising::read_calibration_and_gates_from_disk()
{

    time_t start = time(NULL) ;

    // (by calling cluster preloop functions)
    cout << "\n\n\n\nNEW Reading Rising calibration from the disk" << endl ;

    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    string cal_filename = "calibration/rising_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;    // ascii file
        if(!plik)
        {
            string mess = "I can't open rising calibration file: "
                          + cal_filename  ;
            throw mess ;
        }

        // reading into the string

        string all;
        string one_line;
        while(plik)
        {
            getline(plik, one_line);  // delimiter (\n) is not added to the string
            // so we add it
            all += one_line;
            all += '\n';
        }

        istringstream plik_fast(all);

        // real reading the calibration -----------------
        for(unsigned int i = 0 ; i < detector_vxi.size() ; i++)
        {
            //      cout << "reading calibration loop for detector cluster VXI" << i << endl ;
            detector_vxi[i]->make_preloop_action(plik_fast) ; // reference
        }

        for(unsigned int i = 0 ; i < detector_xia.size() ; i++)
        {
            //      cout << "reading calibration loop for detector cluster XIA" << i << endl ;
            detector_xia[i]->make_preloop_action(plik_fast) ; // reference
        }

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
        // reading the calibration for g-factor OR signal

        // for VXI --------------
        vxi_GeOR_calibr_factors.clear() ;
        string slowo = "vxi_P_time_minus_Ge_time_OR_calibration_factors" ;
        Tfile_helper::spot_in_file(plik_fast, slowo);
        for(int i = 0 ; i < 3 ; i++)
        {
            double value ;
            plik_fast >> value ;
            if(!plik_fast)
            {
                Treading_value_exception capsule ;
                capsule.message =
                    "I found keyword '" + slowo
                    + "' but error was in reading its value." ;
                throw capsule ;
            }
            // if ok
            vxi_GeOR_calibr_factors.push_back(value) ;
        }
        // for XIA --------------
        xia_GeOR_calibr_factors.clear() ;
        slowo = "xia_P_time_minus_Ge_time_OR_calibration_factors" ;
        Tfile_helper::spot_in_file(plik_fast, slowo);
        for(int i = 0 ; i < 3 ; i++)
        {
            double value ;
            plik_fast >> value ;
            if(!plik_fast)
            {
                Treading_value_exception capsule ;
                capsule.message =
                    "I found keyword '" + slowo
                    + "' but error was in reading its value." ;
                throw capsule ;
            }
            // if ok
            xia_GeOR_calibr_factors.push_back(value) ;
        }
#endif  // g-factor

    }
    catch(Tfile_helper_exception & k)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message << endl  ;
        exit(-1) ; // is it healthy ?
    }


    cout << "RISING calibration successfully read"
         << endl;


    cout << "It took " << (time(NULL) - start) << "seconds" << endl;
}
//********************************************************
void Trising::analyse_current_event()
{
#ifdef VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT
    analyse_current_event_vxi();
#endif

#ifdef XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
    analyse_current_event_xia();
#endif
}
//********************************************************
void Trising::analyse_current_event_vxi()
{
    //crystal_which_fired.clear() ; // new event,
    Tcluster::all_clusters_multiplicity = 0 ;
    Tcrystal::multiplicity = 0 ;
    Tcrystal::sum_en4_cal = 0  ;
    Tcrystal::sum_en4_dop = 0 ;

    //    cout << "ANALIZING GE Trising::analyse_current_event_vxi() GE"<<endl;

    // here, for testing purposes, we want pack artificially to the event
    //simulate_event();
#ifdef MINIBALL_PRESENT

    for(int i = 0 ; i < 10 ; i++)
    {
        cluster_delivered_by_miniball[i] = tmp_event_unpacked->miniball_deliveres_cluster_energy[i];
    }
#endif // MINIBALL_PRESENT


#ifdef NIGDY
    int piotr_multiplicity = 0 ;
    for(unsigned clu = 0; clu < detector_vxi.size() ; clu++)
    {
        for(unsigned cry = 0; cry < 7; cry++)
        {
            if(tmp_event_unpacked->ge_fired[clu][cry])    piotr_multiplicity++;
        }
    }

    if(piotr_multiplicity < 2)
        return ;

#endif

    //=================================================
    // at first time callibrtor spectra. There are separtely in two vxi crates
    if((ger_synchro_data = tmp_event_unpacked->ger_synchro_data))   // <--- assigment
    {
        spec_ger_synchro->manually_increment(ger_synchro_data);
        //cout << "in Rising synchro = " << ger_synchro_data << endl;
    }

    if((ger2_synchro_data = tmp_event_unpacked->ger2_synchro_data))   // <--- assigment
    {
        spec_ger2_synchro->manually_increment(ger2_synchro_data);
        //cout << "in Rising synchro2 = " << ger2_synchro_data << endl;
    }

    if((ger_synchro_data   &&  ger2_synchro_data))
    {
        spec_synch_crate2_vs_crate1->manually_increment(ger_synchro_data, ger2_synchro_data);
    }

    //  cout << "============ Next event ================" << endl;


    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################
    // for g-factor experiment we have these new type of signals
    vxi_P_time_minus_Ge_time_OR_raw = tmp_event_unpacked->vxi_P_time_minus_Ge_time_OR;
    Trising::vxi_LR_time_calibrated_by_the_first_who_fired  = 0;

    // calibration of the signal

    double time_tmp = vxi_P_time_minus_Ge_time_OR_raw + randomizer::randomek() ;
    if(vxi_GeOR_calibr_factors[2] == 0) // simple linear calibration
    {
        vxi_P_time_minus_Ge_time_OR_cal = vxi_GeOR_calibr_factors[0]
                                          + (time_tmp * vxi_GeOR_calibr_factors[1]) ;
    }
    else
        for(unsigned int i = 0 ; i < vxi_GeOR_calibr_factors.size() ; i++)
        {
            vxi_P_time_minus_Ge_time_OR_cal += vxi_GeOR_calibr_factors[i] * pow(time_tmp, (int)i) ;
        }

    spec_vxi_P_time_minus_Ge_time_OR_raw->manually_increment(vxi_P_time_minus_Ge_time_OR_raw);
    spec_vxi_P_time_minus_Ge_time_OR_cal->manually_increment(vxi_P_time_minus_Ge_time_OR_cal);


    if(RisingAnalysis_ptr->is_verbose_on())
    {
        cout << "vxi_P_time_minus_Ge_time_OR_raw = "
             << vxi_P_time_minus_Ge_time_OR_raw
             << ", vxi_P_time_minus_Ge_time_OR_cal = "
             << vxi_P_time_minus_Ge_time_OR_cal
             << endl;
    }

    Tcrystal::pointer_to_the_crystal_which_first_fired = 0; // NULL
    Tcrystal::time_of_the_crystal_which_first_fired = 0;      // needs zeroing

    //####################################################################
#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################

    // Specially for Adam Maj, we make the incrementer  which is the difference between
    // two timestamps

    if(tmp_event_unpacked->timestamp_GER  &&  tmp_event_unpacked->timestamp_FRS)
    {
        timestamp_GER_minus_timestamp_FRS =
            tmp_event_unpacked->timestamp_GER -
            tmp_event_unpacked->timestamp_FRS;
        timestamp_GER_minus_timestamp_FRS_possible = true ;
    }
    else
    {
        timestamp_GER_minus_timestamp_FRS = 0;
        timestamp_GER_minus_timestamp_FRS_possible = false;
    }

    //======================

    for(unsigned clu = 0; clu < detector_vxi.size() ; clu++)
    {
        //cout << "Dane for clu=" << clu << " crystal=" << cry << endl ;

        detector_vxi[clu]->  set_this_cluster_multiplicity(0);
        detector_vxi[clu]->  reset_variables();

        int mult_before_cluster = Tcrystal::multiplicity  ;
        int first_crystal = -1;
        int secondary_crystal = -1;
        int localmultiplicity = 0;
        //    local_mult_tmp = 0 ;

        double E20max = -1e10;
        double E20second_max = -1e10;
        //double E20min= 9999999;

#ifdef BGO_PRESENT
        detector_vxi[clu]-> analyse_BGO();  // just take BGO data from the input event
#endif // def BGO_PRESENT

        for(unsigned cry = 0; cry < 7; cry++)
        {
            if(tmp_event_unpacked->ge_fired[clu][cry])
            {
                //  int krotnosc =
                //  (tmp_event_unpacked->ge_fired[clu][0] != 0)
                //  + (tmp_event_unpacked->ge_fired[clu][1] != 0)
                //          + (tmp_event_unpacked->ge_fired[clu][2] != 0)
                //          + (tmp_event_unpacked->ge_fired[clu][3]!= 0)
                //          + (tmp_event_unpacked->ge_fired[clu][4]!= 0)
                //          + (tmp_event_unpacked->ge_fired[clu][5]!= 0)
                //          + (tmp_event_unpacked->ge_fired[clu][6] != 0) ;
                //
                //         if(krotnosc == 2)
                //          {
                //    int breakpoint = 9 ;
                //    cout << "Krotnosc w tym klastrze " << krotnosc << endl ;
                //
                //      }

                fire_statistics[clu][cry]++;  // for graphic monitoring
                detector_vxi[clu]->GetCrystal(cry)->analyse_current_event();
                detector_vxi[clu]->GetCrystal(cry)->increment_crystal_spectra_raw_and_fan();

                // incrementing the total spectrum (always!)
                detector_vxi[clu]->GetCrystal(cry)->increment_crystal_spectra_cal_and_doppler();


                //
                if(detector_vxi[clu]->GetCrystal(cry)->flag_good_data)
                {
                    if(flag_make_addback)   //  addback was asked, !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    {
                        double energy = detector_vxi[clu]->GetCrystal(cry)->give_energy20_calibrated();
                        if(energy >= E20max)
                        {
                            E20max = detector_vxi[clu]->GetCrystal(cry)->give_energy20_calibrated();
                            if(first_crystal != -1)
                            {
                                secondary_crystal = first_crystal;
                                E20second_max = E20max;
                            }
                            first_crystal = (int)cry;

                        } // endif  E20max
                        else if(energy >= E20second_max) // this energy is smaller
                        {
                            secondary_crystal = cry;
                            E20second_max = energy;
                        }

                        //             cout << " Fired good:  det cluster nr " << clu << " cry " << cry  << "E20 = "
                        //             << detector_vxi[clu]->GetCrystal(cry)->give_energy20_calibrated()
                        //             << ", E4 = "
                        //             << detector_vxi[clu]->GetCrystal(cry)->give_energy4_calibrated()
                        //             << endl;

                        localmultiplicity++;  // fired, ok, but not the maximum energy
                        //                local_mult_tmp ++;
                    }
                }  // end if good data


            } // endif crystal fired fired
            else
            {
                detector_vxi[clu]->GetCrystal(cry)->zeroing_good_flags();
            }
            /*      if(detector_vxi[clu]->GetCrystal(cry)->check_flag_good_fired())
                  {
                    cout << "scandal" << endl;
                  }
            */

        } // end for (cry)

        // after workinig with the all crystal of this one cluster =====================

        if(flag_make_addback)
        {
            if(localmultiplicity == 1)
            {
                //        cout<<"SINGLE SPECTRUM "<< clu <<" "<< first_crystal<<endl;
                detector_vxi[clu]->GetCrystal(first_crystal)->increment_spect_total_addbacked_cal_and_doppler();
            }
            else if(localmultiplicity > 1 && localmultiplicity <= Tcluster::max_fold)
            {
                //         cout<<"ADDBACK SPECTRUM clu= "<< clu <<", FIRST crys= "<< first_crystal
                //         << ", secondary crystal was " << secondary_crystal
                //         <<" mult ="<<localmultiplicity         << endl;

                if(E20max > Tcluster::Eprimary_threshold)
                {
                    detector_vxi[clu]->first_crystal_nb = first_crystal;
                    detector_vxi[clu]->calculate_addback();

                    // remember the secondary crystal inside of this cluster
                    detector_vxi[clu]->secondary_crystal_nb = secondary_crystal;
                }

            }
            else
            {
                // otherwise, such event is discarded - -------------
            }

        }
        else
            {}   // no addback, no work here



        // some statistics
        //-------------------
        // at the end of cluster, we may check the multiplicity
        if(Tcrystal::multiplicity - mult_before_cluster)
        {
            Tcluster::all_clusters_multiplicity++ ;
        }
        detector_vxi[clu]->  set_this_cluster_multiplicity(Tcrystal::multiplicity - mult_before_cluster) ;

    } // end of for clusters ###################

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    // Now we can check which SR time was the prompt (it was the shortest) , we can return an make the proper
    // calibration of the LR time,dependint of the promt
    if(Tcrystal::pointer_to_the_crystal_which_first_fired) // something fired and found
    {
        //       if(vxi_P_time_minus_Ge_time_OR_raw)
        //       {
        //          bool stopper = true;
        //          }

        Tcrystal::pointer_to_the_crystal_which_first_fired->you_are_chosen_to_calibrate_LRtime();
        //cout << "I am here, value =  " << vxi_LR_time_calibrated_by_the_first_who_fired << endl;
        spec_vxi_LR_time_calibrated_by_the_first_who_fired->
        manually_increment(77/*vxi_LR_time_calibrated_by_the_first_who_fired*/);

        // after this, we have the calibrated version LR, so we can finished the work by calling the funcion below
        for(unsigned clu = 0; clu < detector_vxi.size() ; clu++)
        {
            for(unsigned cry = 0; cry < 7; cry++)
            {
                if(tmp_event_unpacked->ge_fired[clu][cry])
                {
                    detector_vxi[clu]->GetCrystal(cry)->analyse_LR_common_time();
                }
            }
            // here we can return to the cluster to continue addback things which are related to the j
            // recently (12 lines above) calibrated LR time
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
            if(flag_make_addback)
            {
                // inside there is checking if addback was done and succesfull
                detector_vxi[clu]->analyse_LR_common_time_in_cluster_for_addback_purposes();
            }
#endif

        } // end for clusters

    }
    else
    {
        if(Tcrystal::multiplicity)
        {
            //      cout << "Some crystal fired, but the triggering crystal was not found (among \"good\" crystals) for this event "
            //      << endl;

        }
    }
#endif


    spec_clusters_vxi_multiplicity->manually_increment(Tcluster::all_clusters_multiplicity) ;
    user_spectra_loop();
    monitoring_statistics();



    // saving into ntuple

    //   tmp_event_calibrated->Rising_crystal_multiplicity =  Tcrystal::multiplicity;
    //
    //     tmp_event_calibrated->xia_P_time_minus_Ge_time_OR_raw =
    //             Trising::xia_P_time_minus_Ge_time_OR_raw;
    //
    //
    //    tmp_event_calibrated->vxi_P_time_minus_Ge_time_OR_raw =
    //            Trising::vxi_P_time_minus_Ge_time_OR_raw;


    if(RisingAnalysis_ptr->is_verbose_on())
    {
        cout << "Tcrystal::multiplicity = " << Tcrystal::multiplicity << endl;
    }

    //    if(Tcluster::all_clusters_multiplicity)
    //    {}//cout<<"TOTOAL MULT "<<Tcluster::all_clusters_multiplicity<<endl;
    //cout << "endf. Trising::analyse_current_event()" << endl;
}
//**************************************************************************************
void Trising::analyse_current_event_xia()
{
    //crystal_which_fired.clear() ; // new event,
    Tcluster_xia::all_clusters_multiplicity = 0 ;
    Tcrystal_xia::multiplicity = 0 ;
    Tcrystal_xia::sum_en4_cal = 0  ;

#ifdef DOPPLER_CORRECTION_NEEDED
    Tcrystal_xia::sum_en4_dop = 0 ;
#endif // def DOPPLER_CORRECTION_NEEDED

    //cout << "ANALIZING GE Trising::analyse_current_event() GE"<<endl;

    // here, for testing purposes, we want pack artificially to the event
    //simulate_event();

    //=================================================

    //  cout << "============ Next event ================" << endl;

#if CURRENT_EXPERIMENT_TYPE==RISING_STOPPED_BEAM_CAMPAIGN
    spec_plastic_time_LR_raw->manually_increment(
        (plastic_time_LR_raw = Trising::tmp_event_unpacked->Plastic_LR_time));
    trigger = tmp_event_unpacked->trigger;
#endif
    // Similary for the XIA branch

    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################
    // for g-factor experiment we have these new type of signals
    xia_P_time_minus_Ge_time_OR_raw = tmp_event_unpacked->xia_P_time_minus_Ge_time_OR;

    // calibration of the signal

    double time_tmp = xia_P_time_minus_Ge_time_OR_raw + randomizer::randomek() ;
    if(xia_GeOR_calibr_factors[2] == 0) // simple linear calibration
    {
        xia_P_time_minus_Ge_time_OR_cal = xia_GeOR_calibr_factors[0]
                                          + (time_tmp * xia_GeOR_calibr_factors[1]) ;
    }
    else
        for(unsigned int i = 0 ; i < xia_GeOR_calibr_factors.size() ; i++)
        {
            xia_P_time_minus_Ge_time_OR_cal += xia_GeOR_calibr_factors[i] * pow(time_tmp, (int)i) ;
        }

    spec_xia_P_time_minus_Ge_time_OR_raw->manually_increment(xia_P_time_minus_Ge_time_OR_raw);
    spec_xia_P_time_minus_Ge_time_OR_cal->manually_increment(xia_P_time_minus_Ge_time_OR_cal);


    //####################################################################
#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################

#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE

    for(int i = 0 ; i < 3 ; i++)
    {
        Silicon_LR_trigger_time_minus_Plastic_LR_time[i] = 0 ;
    }

    // at first we have to know how many of them fired
    int how_many =
        (tmp_event_unpacked->Silicon_LR_trigger_time[0] != 0)
        +
        (tmp_event_unpacked->Silicon_LR_trigger_time[1] != 0)
        +
        (tmp_event_unpacked->Silicon_LR_trigger_time[2] != 0);

    switch(how_many)
    {
    default:
        break;
    case 1:
        Silicon_LR_trigger_time_minus_Plastic_LR_time[0] =
            tmp_event_unpacked->Plastic_LR_time -
            tmp_event_unpacked->Silicon_LR_trigger_time[0];
        break;
    case 2: // note below : twisted indexes are on purpose!
        Silicon_LR_trigger_time_minus_Plastic_LR_time[0] =
            tmp_event_unpacked->Plastic_LR_time
            - tmp_event_unpacked->Silicon_LR_trigger_time[1];

        Silicon_LR_trigger_time_minus_Plastic_LR_time[1] =
            tmp_event_unpacked->Plastic_LR_time
            - tmp_event_unpacked->Silicon_LR_trigger_time[0];
        break;
    case 3:
        Silicon_LR_trigger_time_minus_Plastic_LR_time[0] =
            tmp_event_unpacked->Plastic_LR_time -
            tmp_event_unpacked->Silicon_LR_trigger_time[2];

        Silicon_LR_trigger_time_minus_Plastic_LR_time[1] =
            tmp_event_unpacked->Plastic_LR_time -
            tmp_event_unpacked->Silicon_LR_trigger_time[1];

        Silicon_LR_trigger_time_minus_Plastic_LR_time[2] =
            tmp_event_unpacked->Plastic_LR_time -
            tmp_event_unpacked->Silicon_LR_trigger_time[0];
        break;
    } // endof switch

    /*
      if(how_many > 1)
      {

        for(int i = 0 ; i < 3 ; i++)
        {
          cout << "Silicon_LR_trigger_time_minus_Plastic_LR_time i= "
          << i << " is " <<
          Silicon_LR_trigger_time_minus_Plastic_LR_time[i]
          << endl;
        }
        cout << "Drugie" << endl;
      }
    */

#endif



    // Specially for Adam Maj, we make the incrementer  which is the difference between
    // two timestamps

    if(tmp_event_unpacked->timestamp_DGF  &&  tmp_event_unpacked->timestamp_FRS)
    {
        timestamp_DGF_minus_timestamp_FRS =
            tmp_event_unpacked->timestamp_DGF -
            tmp_event_unpacked->timestamp_FRS;
        timestamp_DGF_minus_timestamp_FRS_possible = true ;
    }
    else
    {
        timestamp_DGF_minus_timestamp_FRS = 0;
        timestamp_DGF_minus_timestamp_FRS_possible = false;
    }




    //======================
    for(unsigned clu = 0; clu < detector_xia.size() ; clu++)
    {
        //cout << "Dane for clu=" << clu << " crystal=" << cry << endl ;

        detector_xia[clu]->  set_this_cluster_multiplicity(0);
        detector_xia[clu]->  reset_variables();

        int mult_before_cluster = Tcrystal_xia::multiplicity  ;
        int first_crystal = 0;
        int localmultiplicity = 0;
        //    local_mult_tmp = 0 ;

        double E20max = -1e10;
        //double E20min= 9999999;

#ifdef BGO_PRESENT
        detector_xia[clu]-> analyse_BGO();  // just take BGO data from the input event
#endif // BGO_PRESENT
        for(unsigned cry = 0; cry < 7; cry++)
        {
            if(tmp_event_unpacked->ger_xia_dgf_fired[clu][cry])
            {
                //          int krotnosc =
                //          (tmp_event_unpacked->ge_fired[clu][0] != 0)
                //          + (tmp_event_unpacked->ge_fired[clu][1] != 0)
                //          + (tmp_event_unpacked->ge_fired[clu][2] != 0)
                //          + (tmp_event_unpacked->ge_fired[clu][3]!= 0)
                //          + (tmp_event_unpacked->ge_fired[clu][4]!= 0)
                //          + (tmp_event_unpacked->ge_fired[clu][5]!= 0)
                //          + (tmp_event_unpacked->ge_fired[clu][6] != 0) ;
                //
                //         if(krotnosc == 2)
                //          {
                //            int breakpoint = 9 ;
                //            cout << "Krotnosc w tym klastrze " << krotnosc << endl ;
                //
                //        }

                //                fire_statistics[clu][cry]++;  // for graphic monitoring
                detector_xia[clu]->GetCrystal(cry)->analyse_current_event();
                detector_xia[clu]->GetCrystal(cry)->increment_crystal_spectra_raw_and_fan();

                // incrementing the total spectrum (always!)
                detector_xia[clu]->GetCrystal(cry)->increment_crystal_spectra_cal_and_doppler();


                //
                if(detector_xia[clu]->GetCrystal(cry)->flag_good_data)
                {
                    if(flag_make_addback)   //  addback was asked, !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    {
                        if(detector_xia[clu]->GetCrystal(cry)->give_energy4_calibrated() >= E20max)
                        {
                            // for xia we do not have energy20, so we use energy4
                            E20max = detector_xia[clu]->GetCrystal(cry)->give_energy4_calibrated();
                            first_crystal = (int)cry;

                        } // endif  E20max

                        //                cout << " Fired good:  det cluster nr " << clu << " cry " << cry  << "E20 = "
                        //                  << detector[clu]->GetCrystal(cry)->give_energy20_calibrated()
                        //                  << ", E4 = "
                        //                  << detector[clu]->GetCrystal(cry)->give_energy4_calibrated()
                        //                  << endl;

                        localmultiplicity++;  // fired, ok, but not the maximum energy
                        //                local_mult_tmp ++;
                    }
                }  // end if good data


            } // endif crystal fired fired
            else
            {
                detector_xia[clu]->GetCrystal(cry)->zeroing_good_flags();
            }
        } // end for (cry)

        // after workinig with the all crystal of this cluster


        if(flag_make_addback)
        {
            if(localmultiplicity == 1)
            {
                //        cout<<"SINGLE SPECTRUM "<< clu <<" "<< first_crystal<<endl;
                detector_xia[clu]->GetCrystal(first_crystal)->increment_spect_total_addbacked_cal_and_doppler();
            }
            else if(localmultiplicity > 1)
            {
                //  cout<<"ADDBACK SPECTRUM clu= "<< clu <<" crys= "<< first_crystal<<" mult ="<<localmultiplicity
                //    << endl;

                if(E20max > Tcluster_xia::Eprimary_threshold  && localmultiplicity <= Tcluster_xia::max_fold)  // common to both types)
                {
                    detector_xia[clu]->set_first_crystal_nb(first_crystal);
                    detector_xia[clu]->calculate_addback();
                }
                detector_xia[clu]->calculate_multi_addback();
            }
            else
            {
                // otherwise, such event is discarded
            }
        }
        else
            {}   // no addback, no work here



        // some statistics
        //-------------------
        // at the end of cluster, we may check the multiplicity
        if(Tcrystal_xia::multiplicity - mult_before_cluster)
        {
            Tcluster_xia::all_clusters_multiplicity++ ;
        }
        detector_xia[clu]->  set_this_cluster_multiplicity(Tcrystal_xia::multiplicity - mult_before_cluster) ;

    } // end of for clusters

    spec_clusters_xia_multiplicity->manually_increment(Tcluster_xia::all_clusters_multiplicity) ;
    //    user_spectra_loop();
    //    monitoring_statistics();
    //    if(Tcluster::all_clusters_multiplicity)
    //    {}//cout<<"TOTOAL MULT "<<Tcluster::all_clusters_multiplicity<<endl;
    //cout << "endf. Trising::analyse_current_event()" << endl;
    if(RisingAnalysis_ptr->is_verbose_on())
    {
        cout << "Tcrystal_xia::multiplicity = " << Tcrystal_xia::multiplicity << endl;
    }
}
//***********************************************************************
void  Trising::make_postloop_action()
{
    // cout << "F.Trising::make_postloop_action() " << endl ;

    // save all spectra made in frs objects and in frs elements
    // (mw, sci, etc.)
    cout <<  "Saving Rising " << spectra_rising.size() << " spectra... " << endl;
    for(unsigned int i = 0 ; i < spectra_rising.size() ; i++)
    {
        spectra_rising[i]->save_to_disk() ;
    }
    cout <<  "Saving Rising spectra - done " << endl;
}
//***********************************************************************
void Trising::simulate_event()
{
    //    for(unsigned int i = 0 ; i < detector.size() ; i++)
    //    {
    //        //cout << "Loop i =  " << i << endl;
    //        for(int cri = 0 ; cri < 7 ; cri++)
    //        {
    //            tmp_event_unpacked->Cluster_data[i][cri][0] =
    //                (100*i) +(10*cri) +0 ; // energy
    //            tmp_event_unpacked->Cluster_data[i][cri][1] =
    //                (100*i) +(10*cri) +1;  // time
    //        }
    //        // remember the source of data
    //    }
}
//**********************************************************************
/** No descriptions */
bool Trising::save_selected_spectrum(string name)
{
    for(unsigned int i = 0 ; i < spectra_rising.size() ; i++)
    {
        if(name == spectra_rising[i]->give_name())
        {
            spectra_rising[i]->save_to_disk() ;
            return true ;
        }
    }
    return false ;  // was not here, perhaps in frs or hector?
}
//***********************************************************************
/** Depending on the status of hardware we can use different algorithms */
void Trising::read_doppler_corr_options()
{}
//*******************************************************************
/** Rereading the definiton of the condiotinal (gated) spectra */
void Trising::read_def_conditional_spectra()   // called form preloop
{
    // here should be a function for retrieving the names.
    vector<string> names =
        Tfile_helper::find_files_in_directory("./definitions_user_spectra",
                ".user_definitiom");

    // loop which is looking into the directory for a specified definions *.
    for(unsigned nr = 0 ; nr < names.size() ; nr++)
    {
        cout << "User defined conditional file :" << names[nr] << endl;
        create_cond_spectrum(names[nr]);
    }
}
//*******************************************************************************
/** Create one conditional spectrum */
void Trising::create_cond_spectrum(string name)
{
#ifdef NIGDY
#endif

}
//********************************************************************************
/** where the user spectra can be incremented */
void Trising::user_spectra_loop()
{
#ifdef NIGDY
#endif

}
//**************************************************************
/******************************************** No descriptions */
int Trising::give_synchro_data()
{
    // now it is cluster D 2
    return  tmp_event_unpacked->ger_synchro_data;
}
//**************************************************************
void Trising::read_good_crystal_specification()
{
    // reading from the file
    string pname =  "./options/good_germanium.txt" ;
    ifstream plik(pname.c_str());
    if(!plik)
    {
        cout << "Can't open file " << pname << endl;
        // so  we set the default values
        //------- En 4
        Tcrystal::flag_good_4MeV_requires_threshold = true ;
        Tcrystal::en4MeV_threshold = 100 ;
        Tcrystal::en4MeV_threshold_upper = 8192 ;

        Tcrystal_xia::flag_good_4MeV_requires_threshold = true ;
        Tcrystal_xia::en4MeV_threshold = 100 ;
        Tcrystal_xia::en4MeV_threshold_upper = 8192 ;
        // everything else is zero (because  it is  static)
        return ;
    }
    try
    {
        // reading
        // we assign the same values to  VXI and to XIA electronics

        Tcrystal::flag_increment_20MeV_cal_with_zero =
            //         Tcrystal_xia::flag_increment_20MeV_cal_with_zero =
            (bool) Tfile_helper::find_in_file(plik, "increment_20MeV_cal_with_zero") ;

        Tcrystal::flag_increment_4MeV_cal_with_zero   =
            Tcrystal_xia::flag_increment_4MeV_cal_with_zero   =
                (bool) Tfile_helper::find_in_file(plik, "increment_4MeV_cal_with_zero") ;

        Tcrystal::flag_increment_time_cal_with_zero =
            Tcrystal_xia::flag_increment_time_cal_with_zero =
                (bool) Tfile_helper::find_in_file(plik, "increment_time_cal_with_zero") ;

        //---------- En 20 ----------------
        Tcrystal::flag_good_20MeV_requires_threshold  =
            //         Tcrystal_xia::flag_good_20MeV_requires_threshold  =
            (bool) Tfile_helper::find_in_file(plik, "good_20MeV_requires_threshold") ;

        Tcrystal::en20MeV_threshold =  Tfile_helper::find_in_file(plik, "en20MeV_threshold") ;

        //------- En 4 ---------------------
        Tcrystal::flag_good_4MeV_requires_threshold =
            Tcrystal_xia::flag_good_4MeV_requires_threshold =
                (bool) Tfile_helper::find_in_file(plik, "good_4MeV_requires_threshold") ;

        Tcrystal::en4MeV_threshold =
            Tcrystal_xia::en4MeV_threshold =
                Tfile_helper::find_in_file(plik, "en4MeV_threshold") ;

        Tcrystal::en4MeV_threshold_upper =
            Tcrystal_xia::en4MeV_threshold_upper =
                Tfile_helper::find_in_file(plik, "en4MeV_threshold_upper") ;
        // time -------
        Tcrystal::flag_good_time_requires_threshold =
            Tcrystal_xia::flag_good_time_requires_threshold =
                (bool) Tfile_helper::find_in_file(plik, "good_time_requires_threshold") ;


        // The new version has time threshold for LOW and High, so actually this is not a threshold
        // but just a time gate
        try    // the new style
        {
            Tcrystal::good_time_threshold_lower =
                Tcrystal_xia::good_time_threshold_lower =
                    Tfile_helper::find_in_file(plik, "good_time_threshold_lower") ;

            Tcrystal::good_time_threshold_upper =
                Tcrystal_xia::good_time_threshold_upper =
                    Tfile_helper::find_in_file(plik, "good_time_threshold_upper") ;

            Tcrystal::en4MeV_threshold_upper =
                Tcrystal_xia::en4MeV_threshold_upper =
                    Tfile_helper::find_in_file(plik, "en4MeV_threshold_upper") ;


        }
        catch(...) // perhaps it is an old style
        {
            // at first repair failed stream
            cout << "You are using the old style definiotion of 'good germanium'" << endl;
            // repair the stream
            plik.clear(plik.rdstate() & ~(ios::eofbit | ios::failbit));

            // now try to read
            Tcrystal::good_time_threshold_lower =
                Tcrystal_xia::good_time_threshold_lower =
                    Tfile_helper::find_in_file(plik, "good_time_threshold") ;
            Tcrystal::good_time_threshold_upper =
                Tcrystal_xia::good_time_threshold_upper = 99999;
        }



    }
    catch(Tfile_helper_exception k)
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
        exit(1);
    }
}
//**********************************************************************
/** No descriptions */
void Trising::monitoring_statistics()
{

    // for fire_statistics[clu][cry]++;  // for graphic monitoring
    static int events ;
    static time_t last = time(0) ;
    time_t now = time(0) ;
    events++ ;
    if(now - last > 60)
    {
        struct
        {
            int  moment;
            int real_seconds_of_measurement;
            int predicted_seconds;
            int nr_events;
        }
        opis ;

        opis.moment  = now ;
        opis.real_seconds_of_measurement = now - last ;
        opis.predicted_seconds = 60;
        opis.nr_events = events ;

        ofstream plik("ge_statistics.mon", ios::binary);
        plik.write((char*) &opis, sizeof(opis));
        plik.write((char*) &fire_statistics[0][0], sizeof(fire_statistics));
        struct tm * timeinfo;
        timeinfo = localtime(&now);
        ostringstream   robot ;

        robot << "Report at " <<  asctime(timeinfo) ;  //  << ends ; <--- error for strings
        events = 0 ;
        string  some_comment = robot.str() ;
        plik.write(some_comment.c_str() , some_comment.size() + 1);
        plik.close();
        memset(fire_statistics, 0 , sizeof(fire_statistics));
        last = now ;
    }
}
//*********************************************************************************************
/** This function reads the options defined in the dialog window of the
    cracow viewer. The result is given to the static members of
    the Tcluster */
void Trising::read_addback_options_for_clusters()
{
    // reading from the file
    string pname =  "./options/addback.option" ;
    ifstream plik(pname.c_str());
    if(!plik)
    {
        cout << "Can't open file " << pname << endl;
        //------- En 4
        Tcluster::max_fold = Tcluster_xia::max_fold = 2 ;
        Tcluster::adjacent_mode = Tcluster_xia::adjacent_mode = 2 ;
        Tcluster::primary_seg_algorithm = Tcluster_xia::primary_seg_algorithm = 1;  // others do not exist yet
        Tcluster::Eprimary_threshold =         Tcluster_xia::Eprimary_threshold = 0;
        Tcluster::addback_option_max_acceptable_time_diff_to_the_fastest = 999999;
        return ;
    }
    try
    {
        // reading
        flag_make_addback =
            (bool) Tfile_helper::find_in_file(plik, "addback_on") ;

        Tcluster::max_fold =         Tcluster_xia::max_fold =
                                         (int) Tfile_helper::find_in_file(plik, "max_fold") ;

        Tcluster::adjacent_mode   =        Tcluster_xia::adjacent_mode   =
                                               (bool) Tfile_helper::find_in_file(plik, "adjacent_mode") ;

        Tcluster::nonadjacent_mode  =        Tcluster_xia::nonadjacent_mode =
                (bool) Tfile_helper::find_in_file(plik, "nonadjacent_mode") ;

        Tcluster::sum_all_mode   =        Tcluster_xia::sum_all_mode   =
                                              (bool) Tfile_helper::find_in_file(plik, "sum_all_mode") ;

        Tcluster::primary_seg_algorithm =        Tcluster_xia::primary_seg_algorithm =
                    (int) Tfile_helper::find_in_file(plik, "primary_seg_algorithm") ;

        try
        {

            Tcluster::Eprimary_threshold =            Tcluster_xia::Eprimary_threshold =
                        (int) Tfile_helper::find_in_file(plik, "Eprimary_threshold") ;

            Tcluster_xia::addback_option_max_acceptable_time_diff_to_the_fastest = Tcluster::addback_option_max_acceptable_time_diff_to_the_fastest =
                        Tfile_helper::find_in_file(plik, "addback_option_max_acceptable_time_diff_to_the_fastest") ;

            Tcluster_xia::addback_option_max_acceptable_time_diff_is_enabled = Tcluster::addback_option_max_acceptable_time_diff_is_enabled =
                        Tfile_helper::find_in_file(plik, "addback_option_max_acceptable_time_diff_is_enabled") ;

        }
        catch(...)
        {
            // it is quite new  option, so ignore when it is not present
            Tcluster::Eprimary_threshold = 0 ;
            Tcluster::addback_option_max_acceptable_time_diff_to_the_fastest =
                Tcluster_xia::addback_option_max_acceptable_time_diff_to_the_fastest = 999999;
        }
    }
    catch(Tfile_helper_exception k)
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
        exit(1);
    }
}
//***************************************************************************************
