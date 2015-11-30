#ifndef _CRISTAL_XIA_H_
#define _CRISTAL_XIA_H_
#include "randomizer.h"
#include "spectrum.h"
#include <string>
#include "Tfile_helper.h"
#include "Tincrementer_donnor.h"

//#include <TVector3.h>
#include "doppler_enums.h"

#define  SPECTRUM_20MeV_NEEDED       true
// false

//#define  TIME_TIME_MATRICES_FOR_STEPHANE_PETRI

class Tcluster_xia;
class Trising;
class Ttarget;

/////////////////////////////////////////////////////////////////////////////
class Tcrystal_xia : public Tincrementer_donnor
{
public:
    static Trising  *rising_pointer;
    static Ttarget * target_ptr;  // for faster access of the doppler corr.
    static int multiplicity;
    static double sum_en4_cal;

#ifdef DOPPLER_CORRECTION_NEEDED
    static double sum_en4_dop;
#endif


    static bool flag_increment_4MeV_cal_with_zero;
    static bool flag_increment_time_cal_with_zero;

    static bool flag_good_4MeV_requires_threshold;
    static double en4MeV_threshold;
    static double en4MeV_threshold_upper;

    static bool flag_good_time_requires_threshold;
    static double good_time_threshold_lower; // this is the new style
    static double good_time_threshold_upper; // this is the new style


    bool flag_fired;
    bool flag_good_data;
    bool flag_good_nonaddbacked_crystal; // fired good, and additionally was not used for addback

#ifdef DOPPLER_CORRECTION_NEEDED
    double doppler_factor;

    /** If it was possible to make good doppler correction (Cate & MW was ok) */
    bool flag_doppler_successful;
#endif // def DOPPLER_CORRECTION_NEEDED

protected:
    Tcluster_xia *parent_cluster;
    string name_of_this_element;
    int id_number; // one of 7 numbers
    int cluster_nr;  // we need for fan spectra incrementation

    vector<double> energy4_calibr_factors;
#ifdef  LUCIA_TWO_STEPS_ENERGY_CALIBRATION
    vector<double> energy4_second_calibr_factors;
#endif
    vector<double> time_calibr_factors;

#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED


#ifdef TIME_DIFF_USED
    double time_diff_calibrated;
    spectrum_1D * spec_time_diff_cal;
#endif


    int time_LR_raw;
    int time_SR_raw;

    vector<double> time_LR_calibr_factors; // extra time information
    vector<double> time_SR_calibr_factors;
    double time_LR_calibrated;
    double time_SR_calibrated;

    spectrum_1D * spec_time_SR_raw;
    spectrum_1D * spec_time_LR_raw;
    spectrum_1D * spec_time_SR_cal;
    spectrum_1D * spec_time_LR_cal;


#ifdef TIME_TIME_MATRICES_FOR_STEPHANE_PETRI
    spectrum_2D * spec_energy_vs_LR_time;
    spectrum_2D * spec_LR_time_vs_SR_time;
    spectrum_2D * spec_LR_time_vs_dgf_time;
#endif


#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE
    double  Silicon_LR_trigger_time_minus_LR_time[3];  // multi_hit
#endif


#endif

    //===== position of the crystal ===============
    double phi_geom, theta_geom;
    double
    phi_geom_radians,
    theta_geom_radians;
    double distance_from_target;          // from the target
    double crystal_position_x, crystal_position_y, crystal_position_z;

    //=============================================

    int
    energy4_raw,
    time_raw;

#ifdef  WE_WANT_ORIGINAL_DGF_TIME
    int time_dgf_raw;  // for g-factor campaign
    double time_dgf_cal;

#endif

    double energy4_calibrated;

#ifdef  LUCIA_TWO_STEPS_ENERGY_CALIBRATION
    double energy4_first_step_calibrated;
#endif

    double time_calibrated;  // in case of STOPPED beam it will keep
    // the copy Long Range Time

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    double time_cal_minus_GeOR_cal;
#endif

#ifdef DOPPLER_CORRECTION_NEEDED
    double energy4_doppler_corrected;
#endif // def DOPPLER_CORRECTION_NEEDED

    int pseudo_fan; // Piotr wants to create fan spectrum gated by some
// condition, so he needs an inrementer similar to this what increments
// Fan specrum. The difference between fan
// spectrum is, that fan spectrum for current crystal increments three
// channels np.
// for ex. 60, 62, 64  - corresponding to presence of  E4, E20, FT
// This incrementer will contain only one "channel" (60)
// !!!!!!!!!!!!!!!!!!!!!!

#ifdef DOPPLER_CORRECTION_NEEDED
    double energy4_doppler_corrected_betaN[9];
#endif

    double xv1, yv1, zv1, length_v1; // vector of product (scattered_particle)
    double xv2, yv2, zv2, length_v2; // vector of gamma

    double theta_radians_between_gamma_vs_scattered_particle;
    double phi_radians_between_gamma_vs_scattered_particle;

    spectrum_1D * spec_energy4_raw;

    spectrum_1D * spec_time_raw;
    spectrum_1D * spec_time_cal;
    spectrum_1D * spec_time_cal_minus_GeOR_cal;
    spectrum_1D * spec_energy4_cal;

#ifdef  WE_WANT_ORIGINAL_DGF_TIME
    spectrum_1D * spec_time_dgf_raw;
    spectrum_1D * spec_time_dgf_cal;
#endif
    static  spectrum_1D * spec_total_time_cal;
    static  spectrum_1D * spec_total_time_cal_minus_GeOR_cal;
    static  spectrum_1D * spec_total_energy4_cal;

#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
    static  spectrum_1D * spec_total_time_LR_cal;
    static  spectrum_1D * spec_total_time_SR_cal;
#endif // LONG_RANGE_SHORT_RANGE_TDC_USED

    //------------------
    spectrum_1D * spec_theta_miliradians;
    spectrum_1D * spec_total_theta_miliradians; // is in rising object
    spectrum_1D * spec_energy4_doppler;

    // Total spectra are common to this class
#ifdef DOPPLER_CORRECTION_NEEDED
    static spectrum_1D   * spec_total_energy4_doppler;
#endif

    spectrum_1D * spec_fan;

public:
    Tcrystal_xia(Tcluster_xia *parent_ptr, int nr);
    virtual ~Tcrystal_xia() {};
    void set_event_ptr_for_crystal_data(int ptr[3]);
    void analyse_current_event();

#ifdef DOPPLER_CORRECTION_NEEDED
    void calculate_chosen_doppler_algorithm();
    double give_energy4_doppler_corrected() const
    {
        return energy4_doppler_corrected;
    }

    /** For perfect condition  (Tracing OK, Cate OK) */
    /** Based on the simplest algorithm (from Hans-Juergen) */

    void doppler_corrector_basic();
    /** Options are read from the disk by TIFJ class (during preloop)
        and then are stored in Tcrystal_xia static values */
    static void set_doppler_corr_options
    (int typ_track, int typ_cate_pos, int typ_doppl_algor);

// for beta playfield
    vector<double> betaN_doppler_factor;

#endif

    void make_preloop_action(istream &);     // read the calibr. factors, gates
    string give_name() const
    {
        return name_of_this_element;
    }
    double give_phi_geom()   const
    {
        return phi_geom;
    }
    double give_theta_geom() const
    {
        return theta_geom;
    }
    double give_x() const
    {
        return crystal_position_x;
    }
    double give_y() const
    {
        return crystal_position_y;
    }
    double give_z() const
    {
        return crystal_position_z;
    }

    // for coincidence matrices
    double give_energy4_raw() const
    {
        return energy4_raw;
    }
    double give_energy4_calibrated() const
    {
        return energy4_calibrated;
    }
    double give_time_raw() const
    {
        return time_raw;
    }
    double give_time_calibrated() const
    {
        return time_calibrated;
    }

#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
    double give_LR_time_calibrated() const
    {
        return time_LR_calibrated;
    }
    double give_SR_time_calibrated() const
    {
        return time_SR_calibrated;
    }
#endif

    double give_gp_phi() const    // gamma vs particle  angle (phi)
    {
        return phi_radians_between_gamma_vs_scattered_particle;
    }

    double give_gp_theta() const // gamma vs particle  angle (theta)
    {
        return theta_radians_between_gamma_vs_scattered_particle;
    }

    /** this function contains the algorithm of the doppler correction
        mentioned on the web page of Rising */


    /** No descriptions */
    void zeroing_good_flags();
    /** No descriptions */
    bool check_selfgate(Tself_gate_abstract_descr *desc);
    /** No descriptions */
    void increment_crystal_spectra_cal_and_doppler();
    /** No descriptions */
    void increment_crystal_spectra_raw_and_fan();
    /** No descriptions */
    void increment_crystal_spectra_cal_and_doppler_when_mult_1();
    void increment_spect_total_addbacked_cal_and_doppler();

#ifdef DOPPLER_CORRECTION_NEEDED

    /** this function contains the algorithm of the doppler correction
    using the class TVector3 from ROOT library */
    void doppler_corrector_TVector3();

    TVector3 detector;
#endif // def DOPPLER_CORRECTION_NEEDED

    void set_flag_participant_of_addback()
    {
        flag_participant_of_addback = true;
        flag_good_nonaddbacked_crystal = false;
    }

    /** No descriptions */
protected:
    void create_my_spectra();
    void check_legality_of_data_pointers();

    // option for doppler correction
    static type_of_tracking_requirements    tracking_position_on_target;
    static type_of_lycca_requirements        tracking_position_on_lycca;
    static type_of_doppler_algorithm        which_doppler_algorithm;
    static bool flag_beta_from_frs;     // which beta to use, frs or fixed
//  static double beta_fixed_value;     // if fixed, here it is
    static double energy_deposit_in_target;// bec. beta on the other side
    // of the target will be slower

    bool flag_participant_of_addback;  // if this crystal was used for making addback

};
#endif
