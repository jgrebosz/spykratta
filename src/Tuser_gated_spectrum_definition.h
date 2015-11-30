/***************************************************************************
                          Tuser_gated_spectrum_definition.h  -  description
                             -------------------
    begin                : Fri Jul 25 2003
    copyright            : (C) 2003 by Dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/

#ifndef USER_GATED_SPECTRUM_H
#define USER_GATED_SPECTRUM_H

#include <string>
using  namespace std;

/**This class contains the information about user defined conditional spectra
  *@author dr. Jerzy Grebosz, IFJ Krakow, Poland
  */
/////////////////////////////////////////////////////////////////////////
class Tuser_gated_spectrum_definition
{
    friend class cond_spec_wizard ;
    friend class  user_defined_spectra;
    //friend class Tspectrum_1D_conditional ;

public:
    Tuser_gated_spectrum_definition();
    /** read the definitons from the disk */
    void read_definition_from(string name);
//  void read_from(string path);
    ~Tuser_gated_spectrum_definition();
    /** save the definition on the disk */
    void write_definitions(string path);
//protected: // Protected attributes

    bool enabled ;

    /** the name, without the extension (*.spc) */
    string name_of_spectrum;
    enum type_of_data { energy4_cal = 1, energy20_cal, time_cal,
                        energy4_doppl, energy20_doppl
                      } ;
    type_of_data what_spectrum ;


    /** nr of bins in the spectrum */
    int bins;

    /** left edge of the first bin */
    double beg;
    /** right edge of the last bin */
    double end;

    //------------------- PHI  & THETA of cristal geometry position-----------
    /** gate on the theta angle of cristal geometry position*/
    bool enable_gate_on_phi_cristal;
    /** left edge of the theta of cristal geometry position */
    double phi_cristal_low;
    /** right edge of the theta of cristal geometry position */
    double phi_cristal_high ;

    /** gate on the phi angle of cristal geometry position*/
    bool enable_gate_on_theta_cristal;
    /** left edge of the phi of cristal geometry position */
    double theta_cristal_low;
    /** right edge of the phi of cristal geometry position */
    double theta_cristal_high ;


    /** gate on the time information comming from the germanium cristals */
    bool enable_use_gate_on_total_time_cal;

    //----------------------------- PHI  & THETA of scatterig ------------------

    /** gate on the theta angle between projectile and scattered particle*/
    bool enable_gate_on_phi_scattering_angle;

    /** left edge of the gate on the scattered angle */
    double phi_scattered_low;
    /** right edge of the gate on the scattered angle */
    double phi_scattered_high ;

    /** gate on the theta angle between projectile and scattered particle*/
    bool enable_gate_on_theta_scattering_angle;
    /** left edge of the gate on the scattered angle */
    double theta_scattered_low;
    /** right edge of the gate on the scattered angle */
    double theta_scattered_high ;



    //--------------------- PHI & THETA between photon and particle --------
    /** gate on the phi angle between gamma and scattered particle*/
    bool enable_gate_on_phi_gamma_particle_angle;

    /** left edge of the gate on the scattered angle */
    double phi_gamma_particle_low;
    /** right edge of the gate on the scattered angle */
    double phi_gamma_particle_high ;



    /** gate on the theta angle between gamma and scattered particle*/
    bool enable_gate_on_theta_gamma_particle_angle;

    /** left edge of the gate on the scattered angle */
    double theta_gamma_particle_low;
    /** right edge of the gate on the scattered angle */
    double theta_gamma_particle_high ;



    /** gate on polarisation of the magnet (our Bonn friends) */
    bool enable_gate_on_polarisation_of_magnet;
    bool polarisation_is_up ;


    /** gate on the CATE Si time*/
    bool enable_default_gate_on_cate_Si_total_time_calibrated;

    /** gate on the CATE CsI time*/
    bool enable_default_gate_on_cate_CsI_total_time_calibrated;

    /** gate on the CATE Si positon matrix*/
    bool enable_cate_position_xy_polygon_gate;
    /** name of the polygon gate on the CATE Si positon */
    string name_of_cate_xy_polygon_gate ;

    /** gate on the CATE  dE vs E matrix  */
    bool enable_cate_de_vs_e_polygon_gate;
    /** name of the polygon gate on the CATE  dE vs E */
    string name_of_cate_de_vs_e_polygon_gate ;

    /** gate on the isotope identified by the FRS */
    bool enable_frs_isotope_identifier_true;


};
/////////////////////////////////////////////////////////////////////////
#endif
