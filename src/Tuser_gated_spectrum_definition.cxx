/***************************************************************************
                          Tuser_gated_spectrum_definition.cpp  -  description
                             -------------------
    begin                : Fri Jul 25 2003
    copyright            : (C) 2003 by Dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "Tuser_gated_spectrum_definition.h"
#include "paths.h"
#include <fstream>
#include "Tfile_helper.h"
using namespace std;

#include <iostream>
//*************************************************************************
Tuser_gated_spectrum_definition::Tuser_gated_spectrum_definition()
{

    name_of_spectrum = "user_" ;
    enabled = true ;

    what_spectrum = energy4_doppl;
    bins = 8192 ;
    beg = 0;
    end = 8192;

    enable_gate_on_phi_cristal = false;
    phi_cristal_low = 0 ;
    phi_cristal_high = 359.9;

    enable_gate_on_theta_cristal = false;
    theta_cristal_low = 0 ;
    theta_cristal_high = 359.9;

    enable_use_gate_on_total_time_cal = false;

    enable_gate_on_phi_scattering_angle = false;
    phi_scattered_low = 0;
    phi_scattered_high = 359.9;

    enable_gate_on_theta_scattering_angle = false;
    theta_scattered_low = 0;
    theta_scattered_high = 359.9;

    enable_gate_on_phi_gamma_particle_angle = false;
    phi_gamma_particle_low = 0 ;
    phi_gamma_particle_high = 359.9;

    enable_gate_on_theta_gamma_particle_angle = 0 ;
    theta_gamma_particle_low = 0;
    theta_gamma_particle_high = 359.9;

    enable_gate_on_polarisation_of_magnet = false;
    polarisation_is_up = true;

    // --------- cate ---------
    enable_default_gate_on_cate_Si_total_time_calibrated = false;
    enable_default_gate_on_cate_CsI_total_time_calibrated = false;

    enable_cate_position_xy_polygon_gate = false;
    name_of_cate_xy_polygon_gate = "NONE";

    enable_cate_de_vs_e_polygon_gate = false;
    name_of_cate_de_vs_e_polygon_gate = "NONE";

    enable_frs_isotope_identifier_true = false ;

}
//**************************************************************************
Tuser_gated_spectrum_definition::~Tuser_gated_spectrum_definition()
{

}
//**************************************************************************
/** read the definitons from the disk */
void Tuser_gated_spectrum_definition::read_definition_from(string file_name)
{
    ifstream plik(file_name.c_str());
    if(!plik)
    {
        cout << "Can't open the file " << file_name
             << " for reading " << endl;
        enabled = false ;
        return  ;
    }

    try
    {
        Tfile_helper::spot_in_file(plik, "spectrum_name");
        plik >> name_of_spectrum;

        enabled = (int)Tfile_helper::find_in_file(plik, "enabled");

        what_spectrum = (type_of_data) Tfile_helper::find_in_file(plik, "what_spectrum");

        bins = (int) Tfile_helper::find_in_file(plik, "bins");
        beg = Tfile_helper::find_in_file(plik, "beg");
        end = Tfile_helper::find_in_file(plik, "end");

        enable_gate_on_phi_cristal = Tfile_helper::find_in_file(plik, "enable_gate_on_phi_cristal");
        phi_cristal_low = Tfile_helper::find_in_file(plik, "phi_cristal_low");
        phi_cristal_high = Tfile_helper::find_in_file(plik, "phi_cristal_high");

        enable_gate_on_theta_cristal
        = Tfile_helper::find_in_file(plik,
                                     "enable_gate_on_theta_cristal");
        theta_cristal_low
        = Tfile_helper::find_in_file(plik, "theta_cristal_low");
        theta_cristal_high
        = Tfile_helper::find_in_file(plik, "theta_cristal_high");

        enable_use_gate_on_total_time_cal
        = Tfile_helper::find_in_file(plik, "enable_use_gate_on_total_time_cal");
        enable_gate_on_phi_scattering_angle
        = Tfile_helper::find_in_file(plik, "enable_gate_on_phi_scattering_angle");
        phi_scattered_low
        = Tfile_helper::find_in_file(plik, "phi_scattered_low");
        phi_scattered_high
        = Tfile_helper::find_in_file(plik, "phi_scattered_high");

        enable_gate_on_theta_scattering_angle
        = Tfile_helper::find_in_file(plik,
                                     "enable_gate_on_theta_scattering_angle");
        theta_scattered_low = Tfile_helper::find_in_file(plik, "theta_scattered_low");
        theta_scattered_high  = Tfile_helper::find_in_file(plik, "theta_scattered_high");

        enable_gate_on_phi_gamma_particle_angle
        = Tfile_helper::find_in_file(plik, "enable_gate_on_phi_gamma_particle_angle");

        phi_gamma_particle_low = Tfile_helper::find_in_file(plik, "phi_gamma_particle_low");
        phi_gamma_particle_high = Tfile_helper::find_in_file(plik, "phi_gamma_particle_high");

        enable_gate_on_theta_gamma_particle_angle
        = Tfile_helper::find_in_file(plik,
                                     "enable_gate_on_theta_gamma_particle_angle");
        theta_gamma_particle_low
        = Tfile_helper::find_in_file(plik, "theta_gamma_particle_low");
        theta_gamma_particle_high
        = Tfile_helper::find_in_file(plik, "theta_gamma_particle_high");

        enable_gate_on_polarisation_of_magnet
        = Tfile_helper::find_in_file(plik, "enable_gate_on_polarisation_of_magnet");
        polarisation_is_up = Tfile_helper::find_in_file(plik, "polarisation_is_up");

        // --------- cate ---------
        enable_default_gate_on_cate_Si_total_time_calibrated
        = Tfile_helper::find_in_file(plik,
                                     "enable_default_gate_on_cate_Si_total_time_calibrated");

        enable_default_gate_on_cate_CsI_total_time_calibrated
        = Tfile_helper::find_in_file(plik,
                                     "enable_default_gate_on_cate_CsI_total_time_calibrated");
        enable_cate_position_xy_polygon_gate
        = Tfile_helper::find_in_file(plik,
                                     "enable_cate_position_xy_polygon_gate");

        Tfile_helper::spot_in_file(plik, "name_of_cate_xy_polygon_gate");
        plik >> zjedz >> name_of_cate_xy_polygon_gate;

        enable_cate_de_vs_e_polygon_gate
        = Tfile_helper::find_in_file(plik, "enable_cate_de_vs_e_polygon_gate");

        Tfile_helper::spot_in_file(plik, "name_of_cate_de_vs_e_polygon_gate");
        plik >> zjedz >> name_of_cate_de_vs_e_polygon_gate;

        enable_frs_isotope_identifier_true
        = Tfile_helper::find_in_file(plik, "enable_frs_isotope_identifier_true");


    }
    catch(Tfile_helper_exception &m)
    {
        cout << "Error during reading the file "
             << file_name << " : "
             << m.message << endl;
    }


    // -------- if gates are not specified -------------
    if(name_of_cate_xy_polygon_gate == "NONE")
    {
        enable_cate_position_xy_polygon_gate = false ;
    }

    if(name_of_cate_de_vs_e_polygon_gate == "NONE")
    {
        enable_cate_de_vs_e_polygon_gate = false ;
    }

}
//**************************************************************************
/** save the definition on the disk */
void Tuser_gated_spectrum_definition::write_definitions(string /*name*/)
{

#ifdef NIGDY  // spy never modifies this

    string file_name =
        path + name_of_spectrum + ".user_definition" ;
    ofstream plik(file_name.c_str());
    if(!plik)
    {
        cout << "Can't open the file " << file_name
             << " for writing " << endl;
        return ;
    }

    plik
            << "// This is a definiton of the contitional spectrum\n"
            << "// enables are 0 or 1 which means false or true\n"
            << "// comments are marked using two slashes: // comment \n\n"
            << "spectrum_name\t\t " << name_of_spectrum << "\n"
            << "enabled\t\t" << enabled << "\n\n"


            << "what_spectrum\t\t" << what_spectrum
            << "\n// NOTE: ----------------------------------------------"
            << "\n//\tenergy 4MeV calibrated         ==> " << energy4_cal
            << "\n//\tenergy 20MeV calibrated        ==> " << energy20_cal
            << "\n//\ttime calibrated                ==> " << time_cal
            << "\n//\tenergy 4MeV doppler corrected  ==> " << energy4_doppl
            << "\n//\tenergy 20MeV doppler corrected ==> " << energy20_doppl

            << "\n//-----------------------------------------------------\n\n"

            << "bins\t\t " << bins <<  "\n"
            << "beg\t\t " << beg <<  "\t\t// left edge of the first bin\n"
            << "end\t\t " << end <<  "\t\t// right edge of the last bin\n\n"

            << "enable_gate_on_phi_cristal\t\t "
            <<  enable_gate_on_phi_cristal << "\n"

            << "phi_cristal_low\t\t"  << phi_cristal_low << "\n"
            << "phi_cristal_high\t\t" << phi_cristal_high << "\n\n"

            << "enable_gate_on_theta_cristal\t\t"
            << enable_gate_on_theta_cristal << "\n"
            << "theta_cristal_low\t\t" << theta_cristal_low << "\n"
            << "theta_cristal_high\t\t" << theta_cristal_high << "\n\n"

            << "enable_use_gate_on_total_time_cal\t\t"
            << enable_use_gate_on_total_time_cal << "\n\n"
            << "enable_gate_on_phi_scattering_angle\t\t"
            << enable_gate_on_phi_scattering_angle << "\n"
            << "phi_scattered_low\t\t" << phi_scattered_low << "\n"
            << "phi_scattered_high\t\t" << phi_scattered_high << "\n\n"

            << "enable_gate_on_theta_scattering_angle\t\t"
            << enable_gate_on_theta_scattering_angle << "\n"
            << "theta_scattered_low\t\t" <<  theta_scattered_low << "\n"
            << "theta_scattered_high\t\t" << theta_scattered_high << "\n\n"

            << "enable_gate_on_phi_gamma_particle_angle\t\t"
            << enable_gate_on_phi_gamma_particle_angle
            << "\n"
            << "phi_gamma_particle_low\t\t"
            << phi_gamma_particle_low << "\n"
            << "phi_gamma_particle_high\t\t"
            << phi_gamma_particle_high << "\n\n"

            << "enable_gate_on_theta_gamma_particle_angle\t\t"
            << enable_gate_on_theta_gamma_particle_angle << "\n"
            << "theta_gamma_particle_low\t\t"
            << theta_gamma_particle_low << "\n"
            << "theta_gamma_particle_high\t\t"
            << theta_gamma_particle_high << "\n\n"

            << "enable_gate_on_polarisation_of_magnet\t\t"
            << enable_gate_on_polarisation_of_magnet << "\n"
            << "polarisation_is_up\t\t"
            << polarisation_is_up << "\t// up = 1, down = 0 \n\n"

            // --------- cate ---------
            << "enable_default_gate_on_cate_Si_total_time_calibrated\t\t"
            << enable_default_gate_on_cate_Si_total_time_calibrated
            << "\n"

            << "enable_default_gate_on_cate_CsI_total_time_calibrated\t\t"
            << enable_default_gate_on_cate_CsI_total_time_calibrated
            << "\n\n"

            << "enable_cate_position_xy_polygon_gate\t\t"
            << enable_cate_position_xy_polygon_gate
            << "\n"

            << "name_of_cate_xy_polygon_gate\t\t"
            << name_of_cate_xy_polygon_gate
            << "\n"

            << "enable_cate_de_vs_e_polygon_gate\t\t"
            << enable_cate_de_vs_e_polygon_gate
            << "\n\n"

            << "name_of_cate_de_vs_e_polygon_gate\t\t"
            << name_of_cate_de_vs_e_polygon_gate
            << "\n"
            << "enable_frs_isotope_identifier_true\t\t"
            << enable_frs_isotope_identifier_true
            << endl;
#endif
}
