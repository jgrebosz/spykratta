/***************************************************************************
                          tcate.h  -  description
                             -------------------
    begin                : Fri May 16 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/

#ifndef TCATE_H
#define TCATE_H


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__



const int how_many_cate_segments = 9 ;


#include "Tcate_telescope.h"




class TIFJEvent;
class TIFJCalibratedEvent;

#include <vector>
#include "spectrum.h"
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"

#if CATE_PRESENT

class Tsi_detector;
//////////////////////////////////////////////////////////////////////
/**Class representing the CATE detector of the Rising
 *@author Jerzy Grebosz
 */
//////////////////////////////////////////////////////////////////////
class Tcate : public Tfrs_element, public Tincrementer_donnor
{
    friend class Tcate_telescope ;
protected:
    double distance ;
    Tsi_detector * related_silicon_at_target;

    // deltaE registered by the separate Si detector installed at the target position
    double Si_energy_cal_from_si_det_at_target ;

    double give_Si_energy_cal_from_si_det_at_target()  // telescope will ask for this
    {
        return Si_energy_cal_from_si_det_at_target;
    }

    vector<spectrum_abstr*> cate_spectra_list ;

    vector<Tcate_telescope *>  telescope ;     // nine segments of the Cate detector

public:
    Tcate(string name, string name_of_silicon_at_target);   // distance is refering to the silicon !
    ~Tcate();

    //-------------------------------
    vector< spectrum_abstr*>*  give_spectra_pointer()
    {
        return frs_spectra_ptr  ;
    }

    Tcate_telescope *  give_telescope(unsigned nr)
    {
        if(nr > 8) return 0;
        return telescope[nr] ;
    }
    //------------------------------
    void make_preloop_action(ifstream &) ;  // read the calibration factors, gates

    // taking the calibration factors
    void read_calibration_and_gates_from_disk();  // called form preloop function

    void analyse_current_event();
    void make_user_event_action(); // shop of events ?
    void make_postloop_action();  // saving the spectra ?


    /** gives the geometrical position of the hit */
    double give_x();
    /** returns the y geometrical position of the hit on the Cate detector */
    double give_y();


    /**  Checking if we were able to determine X Y correctly */
    inline bool was_xy_ok()
    {
        if(! calculations_already_done) analyse_current_event();
        return flag_exact_xy_ok ;
    }

    // I see it only user wishi is using this function below
    inline bool was_dE_E_ok()
    {
        return dE_E_ok;
    }

    /** if only Si telescope fired */
    inline bool was_only_one_hit()
    {
        return flag_only_one_hit ;
    }


    inline double give_distance()
    {
        return distance ;
    }
    /** made in post loop, but also when the FRS ask for it (i.e. every 5 min) */
    void save_spectra();

    /** cate will ask the Si detector for this information */
    bool give_xy_of_si_at_target(double *xx, double *yy);


//  double give_central_x_pos_of_element(int nr) { return telescope[nr]->give_central_x() ; }
//  double give_central_y_pos_of_element(int nr) { return telescope[nr]->give_central_y() ; }
//
    double give_central_x_pos_of_segment_which_fired();
    //===========================
    double give_central_y_pos_of_segment_which_fired();

    /** if we had data from one and only one segment. (even if the data is outside of some gates) */
//  bool segment_known();
    /** was zero, one, or more cate segment hit by the residue */
    int how_many_Si_hits();

    /** No descriptions */
    double give_E()
    {
        return E;
    }
    /** No descriptions */
    double give_dE()
    {
        return dE;
    }
    /** No descriptions */
    //void incr_matr_dE_vs_E_gated_by_multiplicity(int nr);


//  bool check_CsI_time()
//  {
//
//    for(unsigned i = 0 ; i < time_CsI_cal.size() ; i++)
//    {
//      if(total_CsI_time_gate_low <= time_CsI_cal[i]
//      && time_CsI_cal[i] <= total_CsI_time_gate_high) return true ;
//    }
//    return false ;
//  }
    //*******************************
    /** No descriptions */
//  bool check_Si_time()
//  {
//    for(unsigned i = 0 ; i < time_Si_cal.size() ; i++)
//    {
//      if(total_Si_time_gate_low <= time_Si_cal[i]
//      && time_Si_cal[i] <= total_Si_time_gate_high) return true ;
//    }
//    return false ;
//  }

    //------------------------------
protected:

    //for testing time

    void simulate_event();
    /** No descriptions */
    void create_my_spectra();

protected: // Protected attributes
public:
    /**  */
    bool flag_only_one_hit;
    /**  */
    bool flag_exact_xy_ok ;
    /** When it was only one hit and possible to calculate xy */
    bool flag_cate_xy_ok_and_only_one_hit;

    bool   flag_cate_dE_E_ok_and_only_one_hit;

    /** only one telescope can have a hit. If more, forget this event !*/
    int id_of_fired ;

    // for last experiments with Cate there was no correspondence beteween Si and Csi segments
    // ( we had 9 Si segments but only 4 CsI
    int id_of_CsI_fired_when_multiplicity_1 ;
    int id_of_Si_fired_when_multiplicity_1 ;


    int give_id_of_fired()
    {
        return  flag_only_one_hit ? id_of_fired : -1 ;
    }

    /** x position of the hit */
    double x_position;
    /** y position of the hit */
    double y_position;
    /**  */
    /** corrected x position of the hit */
    double x_position_corr;
    /** corrected y position of the hit */
    double y_position_corr;
    /**  */
    int multiplicity_of_Si_hits;
    int multiplicity_of_CsI_hits;

    spectrum_2D
    *matr_position_xy,
    *matr_position_xy_corr,

    *matr_total_dE_E,
    *matr_Si_E_vs_cluster_multiplicyty,

    *matr_total_dE_E_gated_by_crystal_multiplicityA,
    *matr_total_dE_E_gated_by_crystal_multiplicityB,
    *matr_total_dE_E_gated_by_crystal_multiplicityC ;


    spectrum_1D
    *spec_total_CsI_time_cal,
    *spec_total_Si_time_cal,

    *spec_Si_multiplicity,
    *spec_CsI_multiplicity,
    *spec_fan ;


    /** if it was element 1, 3, 7, 9 (starting from 1) */
    bool corners();

protected: // Protected attributes
    bool flag_one_of_corner_segments_1_3_7_9_fired;
    bool flag_one_of_cross_segments_2_4_6_8_fired;


    double dE ;
    double E;
    double CsI_time_calibrated;

    /**  */
    bool dE_E_ok;

public: // Public attributes
    /**  */

};
//////////////////////////////////////////////////////////////////////////////

#endif // __CINT__

#endif // #if CATE_PRESENT


#endif
