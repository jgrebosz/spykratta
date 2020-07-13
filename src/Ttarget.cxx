/***************************************************************************
                          ttarget.cpp  -  description
                             -------------------
    begin                : Fri May 23 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "TIFJAnalysis.h"
#include "Ttarget.h"
#include "Tfrs.h"
#include "Tcate.h"
#include "Tcrystal.h"
#include "Thector_baf.h"   // for sending the doppler options
#include "Tminiball_elementary_det.h"   // for sending the doppler options

#include "Tnamed_pointer.h"
#include "doppler_enums.h"

#define dcout   if(0)cout

//************************************************************************
Ttarget::Ttarget(string name_of_target,
                 string name_focus_before,
                 string name_cate_after
                ) : Tfrs_element(name_of_target)
{

    focus_before =
        dynamic_cast<Tfocal_plane*>(owner->address_of_device(name_focus_before)) ;
    cate_after  =
        dynamic_cast<Tcate*>(owner->address_of_device(name_cate_after));

    if(focus_before == 0 ||  cate_after == 0)
    {
        cout << "Error. in the constructor "
             << name_of_this_element
             << " definition: \nThe focus point from "
             << "before or the cate detector after were not correctly "
             << "specified.\n"
             << "Please change it and recompile the program" ;

        exit(-1) ;
    }

    // for the incrementer called:
    // Projectile_energy_calcul_from_frs_beta ;
    E_zero  = 0 ;
    E_one  = 0 ;


    // if all was ok.

    create_my_spectra();


    named_pointer[name_of_this_element + "_x_extrapolated_when_ok"]
    = Tnamed_pointer(& x_extrapolated , &flag_xy_extrapolated_ok, this) ;

    named_pointer[name_of_this_element + "_y_extrapolated_when_ok"]
    = Tnamed_pointer(&y_extrapolated, &flag_xy_extrapolated_ok, this) ;


    // difference from above - cracow options (how to track mw or not)
    named_pointer[name_of_this_element + "_x_tracked_when_ok"]
    = Tnamed_pointer(& x_tracked, &flag_xy_tracked_ok, this) ;

    named_pointer[name_of_this_element + "_y_tracked_when_ok"]
    = Tnamed_pointer(&y_tracked, &flag_xy_tracked_ok, this) ;

    //-----------

    named_pointer[name_of_this_element + "_x_extrapolated_to_cate_when_ok"]
    = Tnamed_pointer(&x_on_cate, &flag_xy_extrapolated_ok, this) ;

    named_pointer[name_of_this_element + "_y_extrapolated_to_cate_when_ok"]
    = Tnamed_pointer(&y_on_cate, &flag_xy_extrapolated_ok, this) ;


    //  named_pointer[name_of_this_element + "_projectile_energy_calcul_from_frs_beta"]
    //    = Tnamed_pointer(&E_zero, 0, this) ;

    named_pointer[name_of_this_element + "__projectile_energy_calculated_from_beta"]
    = Tnamed_pointer(&E_zero, 0, this) ;

    named_pointer[name_of_this_element + "__fragment_energy_after_target"]
    = Tnamed_pointer(&E_one, 0, this) ;

    named_pointer[name_of_this_element + "__beta_after_target"]
    = Tnamed_pointer(&beta_after_target, 0, this) ;



    named_pointer[name_of_this_element + "_angle_theta_radians_scattered_particle"]
    = Tnamed_pointer(&angle_theta_radians_scattered_particle, &flag_doppler_possible, this) ;

    named_pointer[name_of_this_element + "_angle_theta_degree_scattered_particle"]
    = Tnamed_pointer(&angle_theta_degree_scattered_particle, &flag_doppler_possible, this) ;

    named_pointer[name_of_this_element + "_angle_phi_(0_to_2PI)_radians_scattered_particle"]
    = Tnamed_pointer(&angle_phi_radians_scattered_particle, &flag_doppler_possible, this) ;

    named_pointer[name_of_this_element + "_angle_phi_(0_to_360)_degree_scattered_particle"]
    = Tnamed_pointer(&angle_phi_degree_scattered_particle, &flag_doppler_possible, this) ;


    //  named_pointer[name_of_this_element + "_angle_theta_radians_scattered_particle"]
    //    = Tnamed_pointer(&angle_theta_radians_scattered_particle, &flag_doppler_possible, this) ;
    //
    //   named_pointer[name_of_this_element + "_angle_phi_radians_scattered_particle"]
    //    = Tnamed_pointer(&angle_phi_radians_scattered_particle, &flag_doppler_possible, this) ;
    //

    //  named_pointer[name_of_this_element + "_x_extrapolated_to_any_when_ok"]
    //    = Tnamed_pointer(&x_on_any, &flag_xy_extrapolated_ok, this) ;
    //
    //  named_pointer[name_of_this_element + "_y_extrapolated_to_any_when_ok"]
    //    = Tnamed_pointer(&y_on_any, &flag_xy_extrapolated_ok, this) ;

}
//***************************************************************************
Ttarget::~Ttarget()
{}
//***************************************************************************
/** spectra of angles - theta, phi projectile/scattered_particle */
void Ttarget::create_my_spectra()
{
    string folder =  string(name_of_this_element) + "_folder/" ;

    string name ;
    //  = name_of_this_element + "_cos_theta_projectile"  ;
    //  spec_theta_projectile = new spectrum_1D( name,
    //                                 name,
    //                                 201, -1.1, 1.1,
    //                                 folder );
    //  frs_spectra_ptr->push_back(spec_theta_projectile) ;

    //-------------------
    name = name_of_this_element + "_angle_theta_radians_scattered_particle"  ;
    spec_theta_scattered_particle = new spectrum_1D(name,
            name,
            2000, 0, 2,
            folder);
    frs_spectra_ptr->push_back(spec_theta_scattered_particle) ;


    //  name = name_of_this_element + "_angle_phi_radians_scattered_particle"  ;
    //  spec_phi_scattered_particle = new spectrum_1D( name,
    //                                                                                                   name,
    //                                                                                                   2000, 0, 0.2,
    //                                                                                                   folder );
    //  frs_spectra_ptr->push_back(spec_phi_scattered_particle) ;

    // map of the positions ===========================
    name =   name_of_this_element + "_xy" ;
    matr_position_xy = new spectrum_2D(name,
                                       name,
                                       200, -100, 100,
                                       200, -100, 100,
                                       folder);
    frs_spectra_ptr->push_back(matr_position_xy) ;


    name =   name_of_this_element + "_xy_projected_on_cate_distance" ;
    matr_xy_projected_on_cate =
        new spectrum_2D(
        name,
        name,
        158, -79, 79,     // (3*50) + (2*4mm) (odstepu) => 158/2 => 79
        158, -79, 79,     // -0.5, 0.5,
        folder);

    frs_spectra_ptr->push_back(matr_xy_projected_on_cate) ;





    //  name =   "matr_" + name_of_this_element + "_xy_projected_on_any_distance" ;
    //  matr_xy_projected_on_any_distance =
    //      new spectrum_2D(
    //          name,
    //          name,
    //          400, -150, 150,
    //          400, -150, 150,
    //          folder );
    //
    //  frs_spectra_ptr->push_back(matr_xy_projected_on_any_distance) ;


}
//**************************************************************************
// read the calibration factors, gates
//**************************************************************************
void  Ttarget::make_preloop_action(ifstream & s)
{

    distance =
        Tfile_helper::find_in_file(s,
                                   string(name_of_this_element + "_distance")
                                  );



    // reading the options of the doppler correction algorithm
    //  type_of_tracking_requirements    tracking_position_on_target ;
    //  type_of_cate_requirements        tracking_position_on_cate ;
    //  type_of_doppler_algorithm        which_doppler_algorithm ;



    string fname = "./options/doppler.options" ;

    try
    {
        ifstream plik(fname.c_str()) ;
        if(!plik)
        {
            Tfile_helper_exception capsule;
            capsule.message =  "Can't open the file";
            throw capsule ;
        }

        tracking_position_on_target =
            (type_of_tracking_requirements)
            Tfile_helper::find_in_file(plik, "projectile_tracking");

        tracking_position_on_cate =
            (type_of_cate_requirements)
            Tfile_helper::find_in_file(plik, "cate_tracking");

        which_doppler_algorithm =
            (type_of_doppler_algorithm)
            Tfile_helper::find_in_file(plik, "doppler_corr_algorithm");


        beta_from_where = (type_of_beta) Tfile_helper::find_in_file(plik, "beta_from_where");
        beta_fixed_value =  Tfile_helper::find_in_file(plik, "beta_fixed_value");
        energy_deposit_in_target
        =  Tfile_helper::find_in_file(plik, "energy_deposit_in_target");

        atima_const = Tfile_helper::find_in_file(plik, "atima_constant");
        atima_factor = Tfile_helper::find_in_file(plik, "atima_factor");
        atima_mean = Tfile_helper::find_in_file(plik, "atima_mean_beta_tof");

    }
    catch(Tfile_helper_exception &m)
    {
        cout << "\n\nTtarget:: Error while reading the file " << fname
             << m.message << endl;
        cout << ">>>> Most probably you have too old version of parameters "
             "for Doppler correction\n"
             " Please open the dialog window in the Cracow: \n"
             "   spy_option->Doppler\n"
             "choose any combination of parameteres and press OK" << endl;
        exit(123);
        //throw ;
    }

#ifdef DOPPLER_CORRECTION_NEEDED

    // inform them about the current doppler settings
    Tcrystal::set_doppler_corr_options(
        tracking_position_on_target,
        tracking_position_on_cate,
        which_doppler_algorithm);
    Tcrystal_xia::set_doppler_corr_options(
        tracking_position_on_target,
        tracking_position_on_cate,
        which_doppler_algorithm);

#endif // def DOPPLER_CORRECTION_NEEDED


#ifdef HECTOR_PRESENT
    // inform them about the current doppler settings
    Thector_BaF::set_doppler_corr_options(
        tracking_position_on_target,
        tracking_position_on_cate,
        which_doppler_algorithm);
#endif

#ifdef MINIBALL_PRESENT
    // inform them about the current doppler settings
    Tminiball_elementary_det::set_doppler_corr_options(
        tracking_position_on_target,
        tracking_position_on_cate,
        which_doppler_algorithm);
#endif

    if(beta_from_where == beta_type_fixed)
    {

        double gamma_zero = 1 / sqrt(1 - (beta_fixed_value * beta_fixed_value));
        E_zero = (gamma_zero - 1) * 931.5 ;
        E_one = E_zero - energy_deposit_in_target ;
        double gamma_one = (E_one + 931.5) / 931.5 ;

        beta_after_target = sqrt(1 - (1 / (gamma_one * gamma_one)));
    }


    read_beta_playfield(); // for different versions of beta

}

//**********************************************************************
void Ttarget::analyse_current_event()
{

    //cout << "make_calculation for " <<  name_of_this_element << endl ;



    flag_doppler_possible = true ;
    flag_xy_extrapolated_ok = false ;
    flag_xy_tracked_ok = false ;

    switch(which_doppler_algorithm)
    {

    case tracked_algorithm:
        // this can be done, when the Cate detector already worked
        cate_after->analyse_current_event() ;
        // analysis_in_cartesian_coordinates();
        analysis_with_TVector3() ;
        break;

    case basic_algorithm:
        analysis_for_basic_algorithm();
        break;

    } // switch type of doppler algorithm #################

    make_xy_on_cate_distance();
    //  make_xy_on_any_distance(1853.0);   <---------- mw42
    // make_xy_on_any_distance(  4281.9);

    // putting into the n-tuple
    TIFJAnalysis::fxCaliEvent->target_x_pos = x_tracked;
    TIFJAnalysis::fxCaliEvent->target_y_pos = y_tracked;

    // here as the result the new value of: flag_doppler_possible
    // should be available

    calculations_already_done = true ;
}
//*************************************************************************
//*************************************************************************
/** calculates modified beta for each event, or gives modified 'fixed' value */
double Ttarget::give_beta_after_target()
{
    if(beta_from_where == beta_type_frs)
    {
        double beta = owner->give_beta_tof();
        double gamma_zero = 1 / sqrt(1 - (beta * beta));
        E_zero = (gamma_zero - 1) * 931.5 ;
        E_one = E_zero - energy_deposit_in_target ;
        double gamma_one = (E_one + 931.5) / 931.5 ;
        beta_after_target = sqrt(1 - (1 / (gamma_one * gamma_one)));
    }
    else      if(beta_from_where == beta_type_atima)
    {
        double beta = owner->give_beta_tof();

        // Pieter Dornenball wish for atima corrected beta
        beta = atima_const + (atima_factor * (beta - atima_mean)) ;

        double gamma_zero = 1 / sqrt(1 - (beta * beta));
        E_zero = (gamma_zero - 1) * 931.5 ;
        E_one = E_zero - energy_deposit_in_target ;
        double gamma_one = (E_one + 931.5) / 931.5 ;
        beta_after_target = sqrt(1 - (1 / (gamma_one * gamma_one)));
    }


    // else -- it was prepared in preloop

    return beta_after_target ;

}
//************************************************************************
/** For basic doppler correction algorithm we do not need to calculate */
void Ttarget::analysis_for_basic_algorithm()
{
    flag_doppler_possible = true ;  // (only by theta of the crystal)

    if(focus_before->was_x_ok() && focus_before->was_y_ok())
    {
        x_extrapolated = focus_before->extrapolate_x_to_distance(distance);
        y_extrapolated = focus_before->extrapolate_y_to_distance(distance);
        matr_position_xy->manually_increment(x_extrapolated, y_extrapolated);
        flag_xy_extrapolated_ok = true ;
    }

    return ;
}
//*************************************************************************
//***********************************************************************
/** No descriptions */
Tcate * Ttarget::give_cate_ptr()
{
    return cate_after ;
}
/************************************************************************/
void Ttarget::make_xy_on_cate_distance()
{

    if(focus_before->was_x_ok() && focus_before->was_y_ok())
    {
        x_on_cate =
            focus_before->extrapolate_x_to_distance(cate_after->give_distance());
        y_on_cate =
            focus_before->extrapolate_y_to_distance(cate_after->give_distance());
        matr_xy_projected_on_cate->manually_increment(x_on_cate, y_on_cate);
    }
}
//***************************************************************************
/** to test the extrapolation algorithm */
//void Ttarget::make_xy_on_any_distance(double dist)
//{
//
//  if(focus_before->was_x_ok() && focus_before->was_y_ok())
//  {
//      x_on_any =
//          focus_before->extrapolate_x_to_distance(dist);
//      y_on_any =
//          focus_before->extrapolate_y_to_distance(dist);
//
//      matr_xy_projected_on_any_distance->manually_increment(x_on_any, y_on_any);
//  }
//}
//*******************************************************************************

//*************************************************************************************************
void Ttarget::give_scattered_particle_vector(double *x, double *y,
        double *z, double *length)
{
    // cartesian vector from target hit ----> to Cate hit
    *x = v_scattered_particle_x;
    *y = v_scattered_particle_y;
    *z = v_scattered_particle_z;
    double len =  sqrt((v_scattered_particle_x * v_scattered_particle_x) +
                       (v_scattered_particle_y * v_scattered_particle_y) +
                       (v_scattered_particle_z * v_scattered_particle_z)
                      )   ; // v_scattered_particle_length;
    *length = len ;
}
//************************************************************************************************
/** For Alex algorithm using TVector3 class */
void Ttarget::analysis_with_TVector3()
{

    return ;
}
//****************************************************************************
/** No descriptions */
void Ttarget::read_beta_playfield()
{


    string fname = "./options/beta_playfield.options" ;

    try
    {
        ifstream plik(fname.c_str()) ;
        if(!plik)
        {
            //          Tfile_helper_exception capsule;
            //          capsule.message =  "Can't open the file";
            //          throw capsule ;
            cout << "Can't open the file " << fname << " so I assume 'beta playfield' disabled " << endl;
            beta_playfield_enabled = false ;
            return;
        }

        beta_playfield_enabled =
            (bool) Tfile_helper::find_in_file(plik, "beta_playfield_enabled");

        if(!beta_playfield_enabled)
            return ;


        betaN_fixed_value.resize(9);
        betaN_after_target.resize(9);

        for(int i = 0 ; i < 9 ; i++)
        {
            string keyw = "beta_" ;
            keyw += char('1' + i) ;

            betaN_fixed_value[i] = (double) Tfile_helper::find_in_file(plik, keyw);

            double gamma_zero = 1 / sqrt(1 - (betaN_fixed_value[i] * betaN_fixed_value[i]));
            double E_zero = (gamma_zero - 1) * 931.5 ;
            double E_one = E_zero - energy_deposit_in_target ;
            double gamma_one = (E_one + 931.5) / 931.5 ;
            betaN_after_target[i] = sqrt(1 - (1 / (gamma_one * gamma_one)));


        }

    }
    catch(Tfile_helper_exception &m)
    {
        cout << "Ttarget:: Error while reading the file " << fname
             << m.message << endl;
        throw ;
    }
}
//*************************************************************************
