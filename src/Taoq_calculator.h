#ifndef _AOQ_CALCULATOR_H_
#define _AOQ_CALCULATOR_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include <string>
#include <vector>


#include "Tfrs_element.h"
#include "Tscintillator.h"
#include "Ttof_measurement.h"
#include "Tincrementer_donnor.h"
#include "spectrum.h"

#include "Tfocal_plane.h"


/*
NOTE: In case of the 100Sn experiment we were using so called TIC (time ionisation chamber)
to obtain position at s2. Only during this experiment.
Later, for Zsolt experiment in April 2008 we introduced the TPC (Time projection chamber) doing
the same job.
There is no need to change the code, because aoq_calculator does not work directly with them
but with the focal plane defined by these devices ("s2tic" or "s2tpc").
Only change are the names of the spectra and names of the incrementers
 To make this more flexible the following string has been defined

std::string TICorTPC  = sometimes "tic", sometimes "tpc"
(the definition is in the *.cxx file)
*/


extern   std::string TICorTPC;

//////////////////////////////////////////////////////////////////////////////
class Taoq_calculator : public Tfrs_element, public Tincrementer_donnor
{

    double brho_sci_sci ;    // effective radius * average field
    double brho_sci_mw ;    // effective radius * average field
    double brho_sci_tpc; // sci and tpc
    double brho_tic_tpc;   // tic and tpc
    double brho_tic_mw;   // tic and mw
    double brho_tic_sci;   // tic and tpc


    double aoq_sci_sci_corrected ;
    bool aoq_sci_sci_corrected_ok ;

    bool aoq_sci_mw_corrected_ok;
    double aoq_sci_mw_corrected;

    bool  aoq_sci_tpc_corrected_ok;
    double  aoq_sci_tpc_corrected;


    // using linearization at s2 (the factors can be optionally given in frs_calibration.txt)
    double aoq_sci_sci_corrected_linearized ;
    double aoq_sci_mw_corrected_linearized;
    double  aoq_sci_tpc_corrected_linearized;
    //------------

    //   bool aoq_sci_corrected_ok;
    //   double aoq_sci_corrected;

    bool  aoq_tic_tpc_corrected_ok;
    double  aoq_tic_tpc_corrected;

    bool  aoq_tic_mw_corrected_ok;
    double  aoq_tic_mw_corrected;
    bool  aoq_tic_sci_corrected_ok;
    double  aoq_tic_sci_corrected;


#ifdef  TPC22_PRESENT

    double aoq_tic_tpc_and_angle_at_s2tpc_corrected;
    bool aoq_tic_tpc_and_angle_at_s2tpc_corrected_ok ;
#endif // ifdef  TPC22_PRESENT



    Tscintillator *sci_before;
    Tscintillator *sci_after;
    vector<string> magnet_names;

    Tfocal_plane  *foc_tic_before; // s2tic  // alternative for multiwires


    Ttof_measurement *tof ;

    //  Tfinger_tom *sci_finger_before;  // we may use special finger detector which gives position and tof
    Tfocal_plane * foc_after; // for MW correction
    Tfocal_plane  *foc_tpc_after; // s4tpc  // alternative for multiwires

    vector<double> magn_radius ;
    double effective_radius ; // often: average of the
    // double effective_radius_one ; // often: average of the
    //double effective_radius_two ; // often: average of the

    vector<double> magn_field ;
    double mean_field ;  // arithmetic mean

    double brho_zero ;
    // double mean_field_one ;  // arithmetic mean
    // double mean_field_two ;  // arithmetic mean

    // calibration
    double
    corr_offset_s2 ,   // we can make some corrections
    corr_slope_s2  ;

    double
    corr_offset_s4 ,   // we can make some corrections
    corr_slope_s4  ;

    // we need also magnetic fields

    // magnets
    double magnification;
    double dispersion ;

    spectrum_1D * spec_aoq_sci_sci_corr;
    spectrum_1D * spec_aoq_sci_mw_corr;
    spectrum_1D * spec_aoq_sci_tpc_corr;

    spectrum_1D * spec_aoq_tic_tpc_corr;
    spectrum_1D * spec_aoq_tic_sci_corr;
    spectrum_1D * spec_aoq_tic_mw_corr;


    spectrum_2D *  matr_sc_before_vs_aoq_sci_corr;
    spectrum_2D *  matr_sc_after_vs_aoq_sci_corr;

    spectrum_2D *  matr_sc_before_vs_aoq_mw_corr;
    spectrum_2D *  matr_sc_after_vs_aoq_mw_corr;

    double my_tof, my_beta, my_bega, my_gamma; // taken from ToF
    bool flag_tof_ok;


    // for the final linearization (Lucia asked for this)
    vector<double> linearization_factors ;



    // Plamen wants correct AoQ  = AoQ_TPC_TPC * by the angle
    vector<double> ang_x_corr_factor;

public:
    //  constructor
    Taoq_calculator(string _name,
                    string name_of_sci_before,
                    string name_of_sci_after,
                    string name_of_tof,
                    vector<string> magnet_names,
                    string name_focal_plane_after,
                    string name_focal_plane_tpc_after,
                    string name_focal_plane_tic_before);

    // calibration i calculation
    void analyse_current_event() ;

    // read the calibration factors, gates
    void  make_preloop_action(ifstream &);

    // the degrader will ask for this
    double give_aoq()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        if(aoq_sci_mw_corrected_ok)
            return aoq_sci_mw_corrected ;
        else if(aoq_sci_sci_corrected_ok)
            return aoq_sci_sci_corrected;
        else
            return 0;
    }

    double give_aoq_sci_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_sci_sci_corrected ;
    }

    double give_aoq_sci_sci_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_sci_sci_corrected ;
    }

    double give_aoq_mw_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_sci_mw_corrected ;
    }
    double give_aoq_sci_mw_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_sci_mw_corrected ;
    }

    //----------------------------
    bool give_aoq_sci_sci_corrected_ok()
    {
        return aoq_sci_sci_corrected_ok;
    }
    bool give_aoq_sci_mw_corrected_ok()
    {
        return aoq_sci_mw_corrected_ok;
    }
    bool give_aoq_sci_tpc_corrected_ok()
    {
        return aoq_sci_tpc_corrected_ok;
    }

    bool give_aoq_tic_sci_corrected_ok()
    {
        return aoq_tic_sci_corrected_ok;
    }
    bool give_aoq_tic_mw_corrected_ok()
    {
        return aoq_tic_mw_corrected_ok;
    }
    bool give_aoq_tic_tpc_corrected_ok()
    {
        return aoq_tic_tpc_corrected_ok;
    }




    double give_aoq_sci_tpc_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_sci_tpc_corrected ;
    }

    double give_aoq_tic_tpc_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_tic_tpc_corrected ;
    }

    double give_aoq_tic_sci_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_tic_sci_corrected ;
    }

    double give_aoq_tic_mw_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_tic_mw_corrected ;
    }

#ifdef  TPC22_PRESENT

    bool give_aoq_tpc_tpc_and_angle_at_s2tpc_corrected_ok()
    {
        return aoq_tic_tpc_and_angle_at_s2tpc_corrected_ok;
    }


    double give_aoq_tpc_tpc_and_angle_at_s2tpc_corrected()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_tic_tpc_and_angle_at_s2tpc_corrected ;
    }

#endif


    double give_brho_sci()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return brho_sci_sci ;
    }
    double give_brho_mw()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return brho_sci_mw ;
    }

    double give_after_position()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return  sci_after->give_position() ;
    }

    double give_gamma()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return my_gamma ;
    }

    double is_tof_ok()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return flag_tof_ok;
    }
    double give_tof_ps()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return is_tof_ok() ? tof->give_tof_ps() : 0.0 ;
    }

    double give_beta()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return my_beta;
    }

    /** No descriptions */
    //  bool is_aoq_ok()
    //    {
    //      if(calculations_already_done == false ) analyse_current_event();
    //      return aoq_ok ;
    //    }

    //  bool is_aoq_mw_corr_ok()
    //    {
    //      if(calculations_already_done == false ) analyse_current_event();
    //      return aoq_ok_prim ;
    //    }

    bool is_aoq_sci_corrected_ok()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_sci_sci_corrected_ok ;
    }
    bool is_aoq_mw_corrected_ok()
    {
        if(calculations_already_done == false)
            analyse_current_event();
        return aoq_sci_mw_corrected_ok ;
    }

private:
    void create_my_spectra();
    bool flag_this_is_real_aoq; // or dummy
};

/////////////////////////////////////////////////////////////////////////

#endif // __CINT__
#endif // _AOQ_CALCULATOR_H_
