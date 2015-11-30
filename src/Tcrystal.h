#ifndef _CRISTAL_H_
#define _CRISTAL_H_
#include "randomizer.h"
#include "spectrum.h"
#include <string>
#include "Tfile_helper.h"
#include "Tincrementer_donnor.h"

//#include <TVector3.h>
#include "doppler_enums.h"



class Tcluster ;
class Trising  ;
class Ttarget ;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tcrystal : public Tincrementer_donnor
{
public:
    static Trising  *rising_pointer ;
    static Ttarget * target_ptr ;    // for faster acces of the doppler correction
    static int multiplicity ;
    static double sum_en4_cal ;
    static double sum_en4_dop ;

    //  static int threshold_to_multiplicity; // from this energy keV we will count is as "fired"

    static bool flag_increment_20MeV_cal_with_zero;
    static bool flag_increment_4MeV_cal_with_zero  ;
    static bool flag_increment_time_cal_with_zero  ;
    static bool flag_good_20MeV_requires_threshold ;
    static double en20MeV_threshold;
    static bool flag_good_4MeV_requires_threshold  ;
    static double en4MeV_threshold  ;
    static double en4MeV_threshold_upper  ;
    static bool flag_good_time_requires_threshold  ;
    //static double good_time_threshold ; // this was the old style
    static double good_time_threshold_lower ; // this is the new style
    static double good_time_threshold_upper ; // this is the new style

    bool flag_fired ;
    bool flag_good_data ;
    double doppler_factor;

    /** If it was possible to make good doppler correction (Cate & MW was ok) */
    bool flag_doppler_successful;
protected:
    Tcluster *parent_cluster ;
    string name_of_this_element ;
    int id_number ; // one of 7 numbers
    int cluster_nr ;  // we need for fan spectra incrementation

    vector<double> energy4_calibr_factors ;
    vector<double> energy20_calibr_factors ;
    vector<double> time_calibr_factors ;

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    // calibration ffactors for common time singal TAC (Long range)
    vector<double> LRtime_calibr_factors ;
    double LRtime_calibrated;
    double time_cal_of_the_first_crystal_minus_time_cal;  // this one, which fired first
    double LR_plus_dt_time_cal ; // Sum of LRtime + above time
    bool flag_dt_calculated;
    bool flag_this_crystal_triggered_electronics;
    /** After analysing all crystals, we know which of them has a prompt time (smallest SR) so now we can
    return to continue the calibration of the LR time */
    spectrum_1D * spec_LRtime_cal;
    spectrum_1D * spec_time_cal_of_the_first_crystal_minus_time_cal;
    spectrum_1D * spec_LR_plus_dt_time_cal;

    // For Lili
    spectrum_1D *spec_time_LR_plus_SR_plus_offsets_difference;
    double time_LR_plus_SR_plus_offsets_difference;

public:
    /** No descriptions */
    void you_are_chosen_to_calibrate_LRtime();
    void analyse_LR_common_time();

    static Tcrystal *pointer_to_the_crystal_which_first_fired;
    static double time_of_the_crystal_which_first_fired;
    //  bool flag_I_was_the_crystal_which_started_the_trigger;
    double give_calibrating_offset()
    {
        if(LRtime_calibr_factors.size() > 0) return LRtime_calibr_factors[0];
        else
        {
            cout << "Tcrystal.h  -  Error, empty vector of LRtime_calibr_factors" << endl;
            return 0;
        }
    }


#endif // CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
protected:

    //===== position of the crystal ===============
    double phi_geom, theta_geom ;
    double
    phi_geom_radians,
    theta_geom_radians ;
    double distance_from_target ;          // from the target
    double crystal_position_x, crystal_position_y, crystal_position_z ;

    //=============================================
    int *fired_data ;
    int energy4_raw,
        energy20_raw,
        time_raw ;

    double energy4_calibrated ;
    double energy20_calibrated ;
    double time_cal;
    double time_cal_minus_GeOR_cal;

    double energy4_doppler_corrected ;
    double energy20_doppler_corrected ;

    int pseudo_fan; // Piotr wants to create fan spectrum gated by some condition, so he needs
    // an inrementer similar to this what increments Fan specrum. The difference between fan
    // spectrum is, that fan spectrum for current crystal increments three channels np.
    // for ex. 60, 62, 64  - corresponding to presence of  E4, E20, FT
    // This incrementer will contain only one "channel" (60)


    double energy4_doppler_corrected_betaN[9] ;
    double energy20_doppler_corrected_betaN[9] ;

    double xv1, yv1, zv1, length_v1 ; // vector of product (scattered_particle)
    double xv2, yv2, zv2, length_v2 ; // vector of gamma

    double theta_radians_between_gamma_vs_scattered_particle;
    double phi_radians_between_gamma_vs_scattered_particle;

    spectrum_1D * spec_energy4_raw ;
    spectrum_1D * spec_energy20_raw ;
    spectrum_1D * spec_time_raw ;

    spectrum_1D * spec_energy4_cal ;
    spectrum_1D * spec_energy20_cal ;
    spectrum_1D * spec_time_cal ;
    spectrum_1D * spec_time_cal_minus_GeOR_cal;


    spectrum_1D * spec_theta_miliradians;
    spectrum_1D * spec_total_theta_miliradians; // is in rising object
    spectrum_1D * spec_energy4_doppler ;
    spectrum_1D * spec_energy20_doppler ;


    // Total spectra below are really in an rising object, here only pointers
    spectrum_1D
    * spec_total_energy4_doppler,
    * spec_total_energy20_doppler,
    * spec_fan;

    // local
    static spectrum_1D * spec_total_time_cal_minus_GeOR_cal;

public:
    Tcrystal(Tcluster *parent_ptr, int nr) ;
    virtual ~Tcrystal() {}  ;
    void analyse_current_event() ;

    /***********
      bool check_flag_good_fired()
      {
        if(flag_good_data != flag_fired)
        {
          cout << name_of_this_element << ", flag_good_data = " << flag_good_data
                << ", flag_fired = " << flag_fired  << endl;
                return true;
        }
            return false;
      }
    **********/
#ifdef DOPPLER_CORRECTION_NEEDED
    void calculate_chosen_doppler_algorithm();
#endif // def DOPPLER_CORRECTION_NEEDED

    void make_preloop_action(istream &);     // read the calibr. factors, gates
    string give_name() const
    {
        return name_of_this_element ;
    }
    double give_phi_geom()   const
    {
        return phi_geom ;
    }
    double give_theta_geom() const
    {
        return theta_geom ;
    }
    double give_x() const
    {
        return crystal_position_x ;
    }
    double give_y() const
    {
        return crystal_position_y ;
    }
    double give_z() const
    {
        return crystal_position_z ;
    }

    // for coincidence matrices
    double give_energy4_raw() const
    {
        return energy4_raw ;
    }
    double give_energy4_calibrated() const
    {
        return energy4_calibrated ;
    }
    double give_energy4_doppler_corrected() const
    {
        return energy4_doppler_corrected ;
    }
    double give_energy20_raw() const
    {
        return energy20_raw ;
    }
    double give_energy20_calibrated() const
    {
        return energy20_calibrated ;
    }
    double give_energy20_doppler_corrected() const
    {
        return energy20_doppler_corrected ;
    }
    double give_time_raw() const
    {
        return time_raw ;
    }
    double give_time_calibrated() const
    {
        return time_cal ;
    }

    double give_gp_phi() const    // gamma vs particle  angle (phi)
    {
        return phi_radians_between_gamma_vs_scattered_particle ;
    }

    double give_gp_theta() const // gamma vs particle  angle (theta)
    {
        return theta_radians_between_gamma_vs_scattered_particle ;
    }

    /** Based on the simplest algorithm (from Hans-Juergen) */
    void doppler_corrector_basic();
    /** Options are read from the disk by TIFJ class (during preloop)
        and then are stored in Tcrystal static values */
    static void set_doppler_corr_options
    (int typ_track, int typ_cate_pos, int typ_doppl_algor);


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
    /** this function contains the algorithm of the doppler correction
    using the class TVector3 from ROOT library */
    void doppler_corrector_TVector3();



//  TVector3  ;

    /** No descriptions */
protected:
    void create_my_spectra();

    // option for doppler correction
    static type_of_tracking_requirements    tracking_position_on_target ;
    static type_of_lycca_requirements        tracking_position_on_lycca ;
    static type_of_doppler_algorithm        which_doppler_algorithm ;
    static bool flag_beta_from_frs ;     // which beta to use, frs or fixed
    //  static double beta_fixed_value ;     // if fixed, here it is
    static double energy_deposit_in_target ;// bec. beta on the other side of the target
    // will be slower

    // for beta playfield
    //   static vector<double> betaN_after_target ;
    vector<double> betaN_doppler_factor;

};
#endif
