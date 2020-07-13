#ifndef _CLUSTER_H_
#define _CLUSTER_H_
#include "spectrum.h"
#include <vector>
#include "Tcrystal.h"
#include "Tincrementer_donnor.h"
#include <string>
//istream & zjedz(istream & plik);
//////////////////////////////////////////////////////////////////////////////
class Tcluster : public Tincrementer_donnor
{
public:
    static Trising *rising_pointer ;
    static int all_clusters_multiplicity ;  // of clusters (0-15)
    static int max_fold;

    // 3 modes
    static bool adjacent_mode;
    static bool nonadjacent_mode;
    static bool sum_all_mode;
    static double addback_option_max_acceptable_time_diff_to_the_fastest;
    static bool addback_option_max_acceptable_time_diff_is_enabled;

    static int primary_seg_algorithm ;  // others do not exist yet
    /** for addback algorithm */
    static int Eprimary_threshold;

protected:
    int this_cluster_multiplicity ;

    // this below is not a vector. This is only a pointer to a vector
    // which is in the parent class frs. There, we "register" all
    // spectra created in various rising_detectors. By this - it is
    // easier to make automatic save

    // but of course the real owners of the spectra are particullar
    // rising_detectors
    vector<spectrum_abstr*> *rising_spectra_ptr ;

    string name_of_this_element ;

    vector<Tcrystal*> crys ;    // seven crystals of the cluster

    // the pointer to this place in the unpacked event,
    // where is the data for this cluster

    int (* event_data_ptr)[7][3];

public:
    Tcluster(string name, int nr) ;
    //-----------------------------

    // read the calibration factors, gates
//  void make_preloop_action(ifstream & );
    void make_preloop_action(istream &);

    void analyse_current_event() ;
    string give_detector_name()
    {
        return name_of_this_element ;
    }
    Tcrystal* GetCrystal(int cry) const
    {
        return crys[cry] ;
    }
    double addback4MeV_cal;
    double addback20MeV_cal;
    double addback4MeV_doppler;
    double addback20MeV_doppler;

    double addback_time_cal; // here we will keep not the time of the primary, but the fastest (of those participating in addback) (max value of time cal)

    double time_cal_of_the_first_crystal_minus_addback_time_cal;
    double LR_plus_addbacked_dt_time_cal ;

    int first_crystal_nb;         // first highest energy in the addback
    int secondary_crystal_nb;     // second highest energy in the addback

    bool flag_addback_incrementer_valid;
    vector<int> addback_participants;


    // sets the pointer to this place in the unpacked event,
    // where is the data for this cluster

    void set_event_data_pointers(int (* ptr)[7][3]);
    /** When we set gate on cate dE vs E matrix to gate the germanoum time  */
//  void cluster_do_increm_of_time_gated_cate();

    // Rising will call this function
    void set_this_cluster_multiplicity(int n)
    {
        this_cluster_multiplicity = n ;
    }
    // selfgate will call this function
    int get_this_cluster_multiplicity()
    {
        return this_cluster_multiplicity;
    }

    void calculate_addback();

    /** because we do not call function analyse_current event for non-fired clusters */
    void  reset_variables();
    /** No descriptions */
#ifdef BGO_PRESENT
    void analyse_BGO();
    int BGO_energy;
    int get_this_cluster_BGO_energy()
    {
        return BGO_energy;
    }
#endif
    /** Sometimes somebody may put a selfgate only on BGO signal
    so we make here the limited version, just testing BGO and mult */
    bool check_selfgate(Tself_gate_abstract_descr *desc);
    int give_cluster_id()
    {
        return id_cluster;
    }
protected:
    void create_my_spectra();
    void check_legality_of_data_pointers();
    int id_cluster ;
    spectrum_1D * spec_energy4_addback_cal ;
    spectrum_1D * spec_energy20_addback_cal ;
    spectrum_1D * spec_energy4_addback_doppler ;
    spectrum_1D * spec_energy20_addback_doppler ;
    spectrum_1D * spec_addback_time_cal;

    spectrum_1D * spec_BGO_energy_raw ;

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    spectrum_1D * spec_LR_plus_addbacked_dt_time_cal;
    spectrum_1D * spec_time_cal_of_the_first_crystal_minus_addback_time_cal;
    double offset_of_the_local_fastest; // for LILI
    double time_LR_plus_addbacked_SR_plus_offsets_difference;
    double addback_time_cal_minus_GeOR_cal;
public:
    void analyse_LR_common_time_in_cluster_for_addback_purposes();
#endif
};

/////////////////////////////////////////////////////////////////////////

#endif // _CLUSTER_H_
