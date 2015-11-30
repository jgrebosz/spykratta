#ifndef _CLUSTER_XIA_H_
#define _CLUSTER_XIA_H_
#include "spectrum.h"
#include <vector>
#include "Tcrystal_xia.h"
#include "Tincrementer_donnor.h"
#include <string>
istream & zjedz(istream & plik);

/////////////////////////////////////////////////////////////////////////////////
struct fired_good_crystal   // for multiaddback purposes
{
    int nr;
    double time_calibrated;
    double energy;
    //   bool operator<(const fired_good_crystal other)
    //   {
    //     return time_calibrated < other.time_calibrated;
    //   }
};

//////////////////////////////////////////////////////////////////////////////
class Tcluster_xia : public Tincrementer_donnor
{
public:
    static Trising *rising_pointer ;
    static int all_clusters_multiplicity ;  // of clusters (0-15)
    static int max_fold;

    // ========== ADDBACK OPTIONS =========
    // 3 modes of addback
    static bool adjacent_mode;
    static bool nonadjacent_mode;
    static bool sum_all_mode;
    //  time interval
    static double addback_option_max_acceptable_time_diff_to_the_fastest;
    static bool addback_option_max_acceptable_time_diff_is_enabled;

    static int primary_seg_algorithm ;  // others do not exist yet
    /** for addback algorithm */
    static int Eprimary_threshold;


    //=====================================

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

    vector<Tcrystal_xia*> crys ;    // seven crystals of the cluster

    // the pointer to this place in the unpacked event,
    // where is the data for this cluster

    int (* event_data_ptr)[7][3];

public:
    Tcluster_xia(string name, int nr) ;
    //-----------------------------

    // read the calibration factors, gates
    void make_preloop_action(istream &);

    void analyse_current_event() ;
    string give_detector_name()
    {
        return name_of_this_element ;
    }
    Tcrystal_xia* GetCrystal(int cry) const
    {
        return crys[cry] ;
    }

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
    void calculate_multi_addback();
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
    spectrum_1D * spec_BGO_energy_raw ;

#endif // def BGO_PRESENT

    /** Sometimes somebody may put a selfgate only on BGO signal
    so we make here the limited version, just testing BGO and mult */
    bool check_selfgate(Tself_gate_abstract_descr *desc);
    int give_cluster_id()
    {
        return id_cluster;
    }
    void set_first_crystal_nb(int nr)
    {
        first_crystal_nb = nr;
    }
protected:
    void create_my_spectra();
    //  void check_legality_of_data_pointers();
    int id_cluster ;


    // ADDBACK ============================

    //   Now we can have 3 addbacks in one cluster. white, grey, black
    //   The 'first' 'white' is not called so, for the sake of compatibility with the old algorithm.
    //   Other are called 'grey' and 'black'

#define NR_MULTI_ADDBACKS 3

    double addback4MeV_cal;
    double multi_addback_4MeV_cal[NR_MULTI_ADDBACKS];


    double addback_time_cal; // here we will keep not the time of the primary, but
    // the fastest (of those participating in addback) (max value of time cal)
    double multi_addback_time_cal[NR_MULTI_ADDBACKS];


    double addback_LR_time_cal; // here we will keep not the time of the primary,
    // but the fastest (of those participating in addback) (max value of time cal)
    double multi_addback_LR_time_cal[NR_MULTI_ADDBACKS];

    double addback_SR_time_cal; // here we will keep not the time of the primary,
    // but the fastest (of those participating in addback) (max value of time cal)
    double multi_addback_SR_time_cal[NR_MULTI_ADDBACKS]; // here we will keep not the time of the primary,

#ifdef DOPPLER_CORRECTION_NEEDED
    double addback4MeV_doppler;
    double multi_addback4MeV_doppler[NR_MULTI_ADDBACKS];

#endif // def DOPPLER_CORRECTION_NEEDED

    int first_crystal_nb;
    int multi_addback_primary_crystal_nb[NR_MULTI_ADDBACKS];

    bool flag_addback_incrementer_valid;
    bool flag_multi_addback_incrementer_valid[NR_MULTI_ADDBACKS];  // validator

//  vector<int> addback_participants;   // for old stuff when in the self gate we were testing the time of
    // all the participant, (not the fastest)

    /////////////////////////////////////////////// nested class //////
    // This is a class which serves to makes possible that the selfgate will
    // test one of the 3 possible addbacks.
    ////////////////////////////////////////////////////////////////////////////
    class for_multi_addback_selfgate : public Tincrementer_donnor
    {
        int nr_of_addback;   // 0,1,2
        Tcluster_xia *parent;   // parent is the cluster
    public:
        for_multi_addback_selfgate()
        {
            parent = NULL;
            nr_of_addback  = 0;
        }

        void set_values(int nr, Tcluster_xia *parent_arg)
        {
            nr_of_addback = nr;
            parent = parent_arg;
        }
        bool check_selfgate(Tself_gate_abstract_descr *desc);
    };
/////////////////////////////////////////////////////
    friend class for_multi_addback_selfgate;
    for_multi_addback_selfgate  selfgate_object[NR_MULTI_ADDBACKS];


    // addback spectra --------------------------------------
    spectrum_1D * spec_energy4_addback_cal ;
    spectrum_1D * spec_addback_time_cal;
    spectrum_1D * spec_addback_LR_time_cal;
    spectrum_1D * spec_addback_SR_time_cal;
#ifdef DOPPLER_CORRECTION_NEEDED
    spectrum_1D * spec_energy4_addback_doppler ;
    static spectrum_1D * spec_total_xia_energy4_doppler_addbacked;
#endif // def DOPPLER_CORRECTION_NEEDED


};

/////////////////////////////////////////////////////////////////////////

#endif // _CLUSTER_H_
