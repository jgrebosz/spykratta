/***************************************************************************
                          Tcate_telescope.h  -  description
                             -------------------
    begin                : Fri May 16 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TTELESCOPE_H
#define TTELESCOPE_H


/**CATE detector consists of 9 segments. Each of them is a system of
 * Si and CsI detectors, which is just a telescope detector
 *
 */
#include <string>

#include "Tfrs_element.h"
#include "spectrum.h"
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

#ifdef CATE_PRESENT

class Tcate;

/////////////////////////////////////////////////////////////////////////////
class Tcate_telescope : public Tincrementer_donnor
{
protected:
    Tcate * owner; //!
    string name_of_this_element ;

    bool flag_segment_Si_fired ; // when all 6 Si signals were OK
    bool flag_segment_CsI_fired ; // when  2 CsI signals were OK

    bool flag_xy_ok;

    // perhaps not important.  This can be calculated by the target
    //double phi, theta ; // geometry position related  to the target

    double central_x ; // geometrical position x of the central point of this telescope
    double central_y ; // geometrical position y of the central point of this telescope

    int Si_energy_raw;
    int Si_time_raw;
    int CsI_energy_raw;
    int CsI_time_raw;


    // data for the CsI (thick) part
    double CsI_energy_calibrated ;
    double CsI_time_calibrated;

    // data for the Si (thin) part
    double Si_energy_calibrated;     // silicon energy loss
    double mean_delta_E ; // geometric mean of this (above) and the
    // corresponding Si energy from Si_detector_at_target

    double Si_time_calibrated;
    int                   //------------------ position signals
    pin_raw[4];         // left down corner

    //------------ for rates spectrum/ raport ----------
    bool  flag_pin_raw_fired[4];         // left down corner
    bool   flag_Si_energy_raw_fired,
           flag_Si_time_raw_fired,
           flag_CsI_energy_raw_fired,
           flag_CsI_time_raw_fired;

    double                //------------------ position signals
    pin_cal[4];

    // calibration factors
    vector<double> CsI_energy_calibr_factors;
    vector<double> CsI_time_calibr_factors;
    vector<double> Si_energy_calibr_factors;
    vector<double> Si_time_calibr_factors;

    vector< vector<double> > pin_calibr_factors;  // [4][3]

    vector<double>  x_position_corr_factors;
    vector<double>  y_position_corr_factors;

    // Carl Wheldon has the Si part (only) in the place of target
    // using this flag below, we can remove CsI calculations and Cate
    // dependencies
    bool flag_CsI_part_exists  ;


    // gates for every pin raw
    double low_g[4] ;   // have to be double, bec cracow stores value of
    // gates as double
    double high_g[4] ;

    // Cate data structure, defined on the top of the TIFJEvnet.h file
    /*-----------------------
      struct cate_segment
      {
      int CsI_energy,
      CsI_time,

      Si_time,
      pin_1, pin_2,
      Si_energy,           //     <--- same what pin_3
      pin_4, pin_5 ;
      };
      ------------------------  */

    cate_segment  *event_cate_telescope_data ;




    // sopectra -----------------
    spectrum_1D
    * spec_CsI_energy_raw,
    * spec_CsI_time_raw,
    * spec_CsI_energy_fired,
    * spec_CsI_time_fired,
    * spec_CsI_energy_cal,
    * spec_CsI_time_cal;


    spectrum_1D * spec_Si_energy_raw;
    spectrum_1D * spec_Si_time_raw;

    spectrum_1D * spec_Si_energy_fired;
    spectrum_1D * spec_Si_time_fired;

    spectrum_1D * spec_Si_energy_cal;
    spectrum_1D * spec_Si_time_cal;

    // pins have to be raw and calibrated
    spectrum_1D *spec_pin_raw[4];
    spectrum_1D *spec_pin_cal[4];


    spectrum_1D
    *spec_fan_pin_mult_raw,
    *spec_fan_pin_mult_gated,

    *spec_fan_signals_raw,
    *spec_fan_signals_gated,

    *spec_fan_pattern_signals_raw ;


    spectrum_2D *matr_local_position_xy ;
    spectrum_2D *matr_local_position_xy_corr ;

    spectrum_2D *matr_dE_E_fired, *matr_dE_E_good, *matr_dE_E ;

public:

    Tcate_telescope(Tcate *ptr_owner, string name, int id_nr);
    virtual ~Tcate_telescope();

    // Cate register of all spectra
    vector< spectrum_abstr*>*  cate_spectra_ptr();

    /**  the most important fuction,  executed for every event */
    /** readinng calibration, gates, setting pointers */
    void make_preloop_action(ifstream &);

    /** the most important fuction,  executed for every event */
    void analyse_current_event() ; // return value: was Si hit or not ?

    // calibration i calculation
    //  void analyse_current_event() ;
    //  void make_preloop_action(ifstream & );  // read the calibration factors, gates

    /** where in the event starts data for this particular cate_telescope */
    void set_event_data_pointers(cate_segment * ptr);

    double give_central_x()
    {
        return central_x ;
    }
    double give_central_y()
    {
        return central_y ;
    }
    double give_x()
    {
        return central_x + x;
    }
    double give_y()
    {
        return central_y + y;
    }
    double give_local_x()
    {
        return x;
    }
    double give_local_y()
    {
        return y;
    }

    //// for corrected position
    double give_x_corr()
    {
        return central_x + x_corr;
    }
    double give_y_corr()
    {
        return central_y + y_corr;
    }
    double give_local_x_corr()
    {
        return x_corr;
    }
    double give_local_y_corr()
    {
        return y_corr;
    }


    /** If position was calculated precisely */
    bool was_xy_ok()
    {
        return flag_xy_ok ;
    }

    /** if this CsI segment had signals */
    inline bool was_segment_hit();


    bool give_Si_and_CsI_fired_good()
    {
        return flag_Si_and_CsI_fired_good ;
    }

    bool give_Si_segment_fired_good()
    {
        return flag_Si_segment_fired_good ;
    }
    bool give_CsI_segment_fired_good()
    {
        return flag_CsI_segment_fired_good ;
    }

    double give_dE()
    {
        return Si_energy_calibrated  ;
    }
    double give_E()
    {
        return CsI_energy_calibrated ;
    }
    double give_dE_raw()
    {
        return Si_energy_raw  ;
    }
    double give_E_raw()
    {
        return CsI_energy_raw ;
    }


protected: // Protected methods
    /** This function is creating the basic spectra dedicated to the one
        segment (cate_telescope) of the Cate detector */
    void create_my_spectra();
    /** Reading the gates for all pins signals (raw) */

protected: // Protected attributes
    /**  */


    /** positon of the hit (local) */
    double x;
    /** positon of the hit (local) */
    double y;

    /** corrected positon of the hit (local) */
    double x_corr;
    /** corrected positon of the hit (local) */
    double y_corr;

    double x_abs, y_abs ; // absolute to the whole CATE
    /** Whis segment (numbering 0-8) */
    int segment_nr;

    void read_gates();

public: // Public attributes
    /**  */

    bool
    flag_Si_segment_fired_good,
    flag_CsI_segment_fired_good,
    flag_Si_and_CsI_fired_good ;

private:

    static    bool par_force_segment_always_fired;
    static    bool par_force_segment_always_fired_good;


};
///////////////////////////////////////////////////////////////////////////////

#endif   // #ifdef CATE_PRESENT

#endif
