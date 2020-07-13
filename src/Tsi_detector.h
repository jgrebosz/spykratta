/***************************************************************************
                          Tsi_detector.h  -  description
                             -------------------
    begin                : Fri May 16 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/



#ifndef TSI_DETECTOR_H
#define TSI_DETECTOR_H

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#ifdef NIGDY


/**CATE detector consists of 9 segments. Each of them is a system of
 * Si and CsI detectors, which is just a SI_DETECTOR detector
 *
 *@author Jurek Grebosz (IFJ Krakow, Poland)
 */
#include <string>

#include "Tfrs_element.h"
#include "spectrum.h"
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

class Tfrs ;

/////////////////////////////////////////////////////////////////////////////
class Tsi_detector : public Tincrementer_donnor, public Tfrs_element
{
protected:
    Tfrs * owner; //!
    string name_of_this_element ;

    bool flag_segment_Si_fired ; // when all 6 Si signals were OK
    bool flag_xy_ok;

    int Si_energy_raw;
    int Si_time_raw;


    // data for the Si (thin) part
    double Si_energy_calibrated;     // silicon energy loss
    double Si_time_calibrated;
    int // double                       //------------------ position signals
    pin_raw[4];         // left down corner

    double                       //------------------ position signals
    pin_cal[4];

    vector<double> Si_energy_calibr_factors;
    vector<double> Si_time_calibr_factors;

    vector< vector<double> > pin_calibr_factors;  // [4][3]
    vector<double>  x_position_corr_factors;
    vector<double>  y_position_corr_factors;

    // gates for every pin raw
    double low_g[4] ;   // have to be double, bec cracow stores value of gates as double
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

    cate_segment(TIFJEvent::*ptr_to_si);

    cate_segment  *event_si_detector_data ;




    // spectra -----------------

    spectrum_1D * spec_Si_energy;
    spectrum_1D * spec_Si_time;
    spectrum_1D * spec_Si_energy_cal;
    spectrum_1D * spec_Si_time_cal;

    // pins have to be raw and calibrated
    spectrum_1D *spec_pin_raw[4];
    spectrum_1D *spec_pin_cal[4];


    spectrum_1D *spec_fan_pin_mult_raw,
                *spec_fan_pin_mult_gated,

                *spec_fan_signals_raw,
                *spec_fan_signals_gated,

                *spec_fan_pattern_signals_raw ;


    spectrum_2D *matr_position_xy ;
    spectrum_2D *matr_position_xy_corr ;


public:

    Tsi_detector(string name, cate_segment(TIFJEvent::*ptr));
    virtual ~Tsi_detector();

    /** readinng calibration, gates, setting pointers */
    void make_preloop_action(ifstream &);

    /** the most important fuction,  executed for every event */
    void analyse_current_event() ; // return value: was Si hit or not ?

    /** where in the event starts data for this particular si_detector */
    void set_event_data_pointers(cate_segment * ptr);
    /** No descriptions */
    double give_Si_energy_cal()
    {
        if(calculations_already_done == false) analyse_current_event();
        return Si_energy_calibrated;     // silicon energy loss
    }

    double give_x()
    {
        if(! calculations_already_done) analyse_current_event();
        return x_corr ;
    }

    double give_y()
    {
        if(! calculations_already_done) analyse_current_event();
        return y_corr;
    }
    bool was_xy_ok()
    {
        if(! calculations_already_done) analyse_current_event();
        return flag_xy_ok;
    }

protected: // Protected methods
    /** This function is creating the basic spectra dedicated to the one
        segment (si_detector) of the Cate detector */
    void create_my_spectra();
    /** Reading the gates for all pins signals (raw) */

protected: // Protected attributes
    /**  */


    /** positon of the hit (local) */
    double x;
    /** positon of the hit (local) */
    double y;
    /** corrected x positon of the hit (local) */
    double x_corr;
    /** corrected x positon of the hit (local) */
    double y_corr;


    void read_gates();

public: // Public attributes
    /**  */

    bool  Si_segment_fired_good;
};
///////////////////////////////////////////////////////////////////////////////

#endif   //  #ifdef NIGDY

#endif
