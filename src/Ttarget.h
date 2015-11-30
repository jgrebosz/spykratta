/***************************************************************************
                          ttarget.h  -  description
                             -------------------
    begin                : Fri May 23 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#ifndef TTARGET_H
#define TTARGET_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__


#include <Tfrs_element.h>
#include <Tcate.h>
#include <Tcrystal.h>   // to have an enum about doppler correction
#include <spectrum.h>
#include "Tincrementer_donnor.h"

//#include <TVector3.h>


#if CATE_PRESENT


#include <vector>

class Tfocal_plane ;
//class Tcate ;

/**this class is to perform operation of tracing the projectile hitting
   the target and tracing the scattered_particle leaving the target
   *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
   */
////////////////////////////////////////////////////////////////////////////
class Ttarget : public Tfrs_element, public Tincrementer_donnor
{
public:
    Ttarget(string name_of_target,
            string name_focus_before,
            string name_cate_after
           );

    ~Ttarget();

    // for the incrementer called:
    // Projectile_energy_calcul_from_frs_beta ;
    double E_zero ;
    double E_one ; //fragement_energy_after_target

    void make_preloop_action(ifstream & /* s */);

    /** doppler correction algorithm will ask for this */
    inline void give_cos_scattered_particle(double * cx, double *cy, double *cz)
    {
        *cx = cos_scattered_particle_x;
        *cy = cos_scattered_particle_y;
        *cz = cos_scattered_particle_z ;
    }
    /** for doppler correction algorithms */
    inline double give_y_tracked()
    {
        return y_tracked ;
    }
    /** for doppler correction algorithms */
    inline double give_x_tracked()
    {
        return x_tracked ;
    }
    /** No descriptions */
    inline double give_z()
    {
        return distance ;
    }
    /** No descriptions */

    //bool projectile_calculations_ok() { return flag_projectile_ok ;}
    //bool scattered_particle_calculations_ok() { return flag_scattered_particle_ok ;}
    bool is_doppler_possible()
    {
        return flag_doppler_possible ;
    }

    double give_cate_hit_pos_x()
    {
        return cate_after->give_x() ;
    }
    double give_cate_hit_pos_y()
    {
        return cate_after->give_y() ;
    }
    double give_cate_hit_pos_z()
    {
        return cate_after->give_distance() - distance;
    }

    /** Crystal will ask for a vector of resiude (target----->cate) */
    void give_scattered_particle_vector(double *x, double *y,
                                        double *z, double *length);

    /** calculates modified beta for each event, or gives modified 'fixed' value */
    double give_beta_after_target();
    double give_betaN_after_target(int n)
    {
        return betaN_after_target[n];
    }
    bool is_beta_playfield_enabled()
    {
        return beta_playfield_enabled;
    }

    /** For basic doppler correction algorithm we do not need to calculate */
    void analysis_for_basic_algorithm();


    //-----------------------------------
    double give_theta_scattering2()
    {
        cout << "give_theta_scattering() Not implemented yet" << endl ;
        return 0 ; // put here the result of calculations
    }
    //-----------------------------------
    double give_phi_scattering2()
    {
        cout << "give_phi_scattering() Not implemented yet" << endl ;
        return 0 ; // put here the result of calculations
    }
    /** To incremet spectra gated by the identified isotope */
//  void isotope_was_identified();
    /** No descriptions */
    Tcate * give_cate_ptr();
    /** No descriptions */
    void make_xy_on_cate_distance();
    /** position where the projectile hits the cate */

    double give_x_on_cate()
    {
        return x_on_cate;
    }
    double give_y_on_cate()
    {
        return y_on_cate;
    }
    /** to test the extrapolation algorithm */
    void make_xy_on_any_distance(double dist);
    /** No descriptions */
    void read_beta_playfield();
    /** No descriptions */
    // bool calculate_scattered_particle_vector();
    /** we need this in many places */
    // bool make_projectile_vector();

// TVector3 expol , outgoing ;


protected: // Protected methods

    /** position where the projectile hits the cate */
    double x_on_cate;
    double y_on_cate;

    //double x_on_any;
    //double y_on_any;


    /** spectra of angles - theta, phi projectile/residum */
    void create_my_spectra();
    void analyse_current_event();

    /** For Samit algorithm  */
//  void analysis_in_polar_coordinates();
    /** For Take algorithm */
//  void analysis_in_cartesian_coordinates();
    /** For Alex algorithm using TVector3 class */
    void analysis_with_TVector3();


protected: // Protected attributes


    /** to trace the projectile coming to the target */
    Tfocal_plane *focus_before;
    /** to trace the scattered_particle coming out of target */
    Tcate *cate_after;
    /**  geometrical positiona of the target */
    double distance;

    double cos_projectile_x ;
    double cos_projectile_y ;
    double cos_projectile_z ;

    double v_projectile_length  ;

    //  normalized vector
    double dnorm_projectile ;
    double dnorm_scattered_particle ;


    // angles
    double cos_theta_scattered_particle  ;
    double cos_theta_projectile  ;

    double angle_theta_radians_scattered_particle ;
    double angle_phi_radians_scattered_particle ;

    double angle_theta_degree_scattered_particle ;
    double angle_phi_degree_scattered_particle ;

    // for Cartesian coordinates algorithm

    // catesian vector from target hit ----> to Cate hit
    double
    v_scattered_particle_x,
    v_scattered_particle_y,
    v_scattered_particle_z,
    v_scattered_particle_length;

    // for vector of the scattered_particle   target----->cate
    double cos_scattered_particle_x ;
    double cos_scattered_particle_y ;
    double cos_scattered_particle_z ;


    /** position where the projectile hits the target (x coordinate) */
    double x_extrapolated;
    /** position where the projectile hits the target (y coordinate) */
    double y_extrapolated;

    bool flag_xy_extrapolated_ok;

    // this below is different form this above (x,y) only by options set in the cracow
    // viewer. If the user wants no tracking, the x,y contains extrapolated value
    // but x_tracked, and y_tracked contains zero (0,0)

    /** position where the projectile is tracked on the target (x coordinate) */
    double x_tracked;
    /** position where the projectile hits the target (y coordinate) */
    double y_tracked;

    bool flag_xy_tracked_ok;



    type_of_tracking_requirements    tracking_position_on_target ;
    type_of_cate_requirements        tracking_position_on_cate ;
    type_of_doppler_algorithm        which_doppler_algorithm ;

    enum type_of_beta { beta_type_frs = 1, beta_type_fixed, beta_type_atima };
    type_of_beta   beta_from_where;

    /** beta modified by the energy loss in the target */
    double beta_after_target ;
    double beta_fixed_value ;
    double energy_deposit_in_target ;

    double atima_const, atima_factor, atima_mean ;

    vector<double> betaN_fixed_value ;  // for the Beta Playfield purposes
    vector<double> betaN_after_target ;
    bool beta_playfield_enabled;

    // ---- spectra ----------------------
//  spectrum_1D * spec_theta_projectile ;
    spectrum_1D * spec_theta_scattered_particle ;

    spectrum_2D * matr_position_xy ;
    spectrum_2D * matr_xy_projected_on_cate;
//  spectrum_2D * matr_xy_projected_on_any_distance;

    /** if the information in the event is sufficient to perform such doppler
     correction as we demand (accurate, approximate) */
    bool flag_doppler_possible;


};
/////////////////////////////////////////////////////////////////////////////
#endif // #if CATE_PRESENT

#endif  //  __CINT__

#endif

