#ifndef _MINIBALL_ELEMENTARY_DET_H_
#define _MINIBALL_ELEMENTARY_DET_H_
#include "randomizer.h"
#include "spectrum.h"
#include <string>
#include "Tfile_helper.h"
#include "Tincrementer_donnor.h"

//#include <TVector3.h>
#include "doppler_enums.h"



class Tminiball_cryostat ;
class Tminiball  ;
class Ttarget ;

///////////////////////////////////////////////////////////////////////////
class Tminiball_elementary_det : public Tincrementer_donnor
{
public:
    static Tminiball  *miniball_pointer ;
    static Ttarget * target_ptr ;    // for faster access of the doppler correction
    static int multiplicity ;
    static double sum_en4_cal ;
    static double sum_en4_dop ;

    //  static int threshold_to_multiplicity; // from this energy keV we will count is as "fired"

    static bool flag_increment_segment_energy_cal_with_zero  ;
    static bool flag_increment_time_cal_with_zero  ;

    static bool flag_good_core_energy_requires_threshold  ;
    static double core_energy_threshold  ;

    static bool flag_good_time_requires_threshold  ;
    static double good_time_threshold ;

    bool flag_fired ;
    bool flag_good_data ;
    double doppler_factor;

    /** If it was possible to make good doppler correction (Cate & MW was ok) */
    bool flag_doppler_successful;

protected:
    Tminiball_cryostat *parent_cryostat ;
    string name_of_this_element ;
    int id_number ; // one of 7 numbers
    int cryostat_nr_from_0 ;  // we need for fan spectra incrementation

    vector< vector< double> > energy_calibr_factors;
    vector< double > time_calibr_factors;



    //===== position of the core and 6 segments of the elementary_det ===============
    vector< double >  phi_geom, theta_geom ;
    vector< double >   phi_geom_radians, theta_geom_radians ;
    vector< double >  distance_from_target ;          // from the target

    // After recaluculation of the above data, we have x,y,z
    vector< double >
    elementary_det_position_x,
    elementary_det_position_y,
    elementary_det_position_z ;


    //=============================================
    // pointers to the places in the unpacked event
    int (*energy_data)[8]  ; // pointers to raw data inside the event

    int *time_data ;
    int *fired_data ;

    int  energy_raw[7];
    double energy_calibrated[7] ;
    double energy_doppler_corrected ;

    int  time_raw ;
    double time_calibrated ;

    int primary_segment; // which of segments 1-6 gave the highest calibrated energy

    double xv1, yv1, zv1, length_v1 ; // vector of product (scattered_particle)
    double xv2, yv2, zv2, length_v2 ; // vector of gamma

    double theta_radians_between_gamma_vs_scattered_particle;
    double phi_radians_between_gamma_vs_scattered_particle;

    spectrum_1D * spec_energy_raw[7] ;
    spectrum_1D * spec_energy_cal[7] ;
    spectrum_1D * spec_energy_doppler ;


    spectrum_1D * spec_time_raw;
    spectrum_1D * spec_time_cal ;
    spectrum_1D * spec_theta_miliradians;
    spectrum_1D * spec_total_theta_miliradians; // is in miniball object

    // Total spectra below are really in an miniball object, here only pointers
    spectrum_1D
    * spec_total_core_energy_doppler,
    * spec_fan;
public:
    Tminiball_elementary_det(Tminiball_cryostat *parent_ptr, int nr) ;
    virtual ~Tminiball_elementary_det() {}  ;
    void set_event_ptr_for_elementary_det_data(int (*ptr)[8])  ;
    void analyse_current_event() ;

    void calculate_chosen_doppler_algorithm();
    void make_preloop_action(ifstream &);     // read the calibr. factors, gates
    string give_name() const
    {
        return name_of_this_element ;
    }
    double give_phi_geom()   const
    {
        return phi_geom[primary_segment] ;
    }
    double give_theta_geom() const
    {
        return theta_geom[primary_segment] ;
    }
    double give_x() const
    {
        return elementary_det_position_x[primary_segment] ;
    }
    double give_y() const
    {
        return elementary_det_position_y[primary_segment] ;
    }
    double give_z() const
    {
        return elementary_det_position_z[primary_segment] ;
    }

    // for coincidence matrices
    double give_energy_raw(int i) const
    {
        return energy_raw[i] ;
    }
    double give_energy_calibrated(int i) const
    {
        return energy_calibrated[i] ;
    }
    double give_energy_doppler_corrected(int i) const
    {
        return energy_doppler_corrected ;
    }
    double give_time_raw() const
    {
        return time_raw ;
    }
    double give_time_calibrated() const
    {
        return time_calibrated ;
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
        and then are stored in Tminiball_elementary_det static values */
    static void set_doppler_corr_options
    (int typ_track, int typ_cate_pos, int typ_doppl_algor);

    /** No descriptions */
    void zeroing_good_flags();
    /** No descriptions */
    bool check_selfgate(Tself_gate_abstract_descr *desc);
    /** No descriptions */
    void increment_elementary_det_spectra_cal_and_doppler();
    /** No descriptions */
    void increment_elementary_det_spectra_raw_and_fan();
    /** No descriptions */
    void increment_elementary_det_spectra_cal_and_doppler_when_mult_1();
    void increment_spect_total_addbacked_cal_and_doppler();
    /** this function contains the algorithm of the doppler correction
          using the class TVector3 from ROOT library */
    void doppler_corrector_TVector3();

//  TVector3 detector ;

    /** No descriptions */
protected:
    void create_my_spectra();
    void check_legality_of_data_pointers();

    // option for doppler correction
    static type_of_tracking_requirements    tracking_position_on_target ;
    static type_of_cate_requirements        tracking_position_on_cate ;
    static type_of_doppler_algorithm        which_doppler_algorithm ;
    static bool flag_beta_from_frs ;     // which beta to use, frs or fixed
    static double beta_fixed_value ;     // if fixed, here it is
    static double energy_deposit_in_target ;// bec. beta on the other side of the target
    // will be slower

private: // Private methods
    /** No descriptions */
//  int give_simulated_Eu_152(int range);
//  static double europ_spectrum[4096];
//  static bool flag_europ_read ;
};
#endif
