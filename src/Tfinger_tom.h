#ifndef _FINGER_TOM_H_
#define _FINGER_TOM_H_

#ifdef NIGDY
////////////////////////// NEVER USED SERIOUSLY //////////////////////////////////////////////////////

Hi Jurek,
   as far as I know it was used only in our experiment, we have checked
   that we get identical data with the standard one and so we have used the
   standard one to have a simpler analysis.
   I do not think it is even mounted now
       (Franco Camera)

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__



#include "Tfrs_element.h"
#include "spectrum.h"
#include "Tfocal_plane.h"

#include <string>

#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

// A class for Tom Aumann finger detector


//////////////////////////////////////////////////////////////////////////////
       class Tfinger_tom : public Tfrs_element, public Tincrementer_donnor
    {

        double distance ;

        // there is 15 strips (fingers) and 16 photo multipliers
        enum { how_many_PMs = 16, how_many_fingers = how_many_PMs - 1 } ;

        int time_raw[how_many_PMs];
        int charge_raw[how_many_PMs];
        bool finger_fired[how_many_fingers];
        bool pm_fired[how_many_PMs];

        double time_cal[how_many_PMs];

        // when coresponding charge is out of range, we do not use this for position
        //int charge_raw_low_gate;
        //int charge_raw_high_gate;
        vector< vector<double> > charge_raw_gate;  // [16][2]
        vector<double> total_time_cal_gate;  //[2]

        //  bool   e_ok;
        //  double e;          // calculted charge


        // did we make good calculations for this event
        bool   position_ok ;
        double position  ; // calculated position

        // for position calibration
        double position_cal_factor_offset, position_cal_factor_slope;

        // for tof calibrtion
        vector< vector<double> > time_calibr_factors;  // [4][3]
        double tof;
        bool flag_tof_ok;

        int finger_multiplicity; // how many fingers fired
        int pm_multiplicity;   // how many PMs gave sensible signal


        Tfocal_plane *related_focal_plane ;

        // source of data
        int (TIFJEvent::*finger_time)[how_many_PMs] ; // elem array for time of PMs
        // source of data
        int (TIFJEvent::*finger_charge)[how_many_PMs] ; // elem array for charge of PMs
        // perhaps there is some pattern also?



        // spectra
        spectrum_1D  *spec_time_raw[how_many_PMs],
                     *spec_time_cal[how_many_PMs],
                     *spec_charge_raw[how_many_PMs],
                     *spec_time_total,

                     *spec_position_raw, // only ne spectrum for the whole
                     *spec_position_cal, // only one spectrum for the whole
                     *spec_finger_multiplicity,
                     *spec_pm_multiplicity ;

        spectrum_1D  *spec_pattern_of_fingers;
        spectrum_1D  *spec_pattern_of_pm;

        spectrum_1D   *spec_finger_tof[how_many_fingers];

        spectrum_1D  *spec_tof;
        spectrum_1D * spec_beta ;


//  spectrum_2D  *matr_mw_sci ;  // matrix to fit the calibration line

        //-----------
        // to calculate beta = v/c  from time of flight
        double beta_calibr_factor0, beta_calibr_factor1 ;
        double beta, gamma, bega ;  // be-ga  = is beta*gamma




    public:
        //  constructor
        Tfinger_tom(string _name, string name_of_focal_plane,
                    int (TIFJEvent::*finger_time_ptr)[how_many_PMs],
                    int (TIFJEvent::*finger_charge_ptr)[how_many_PMs]);

        //--------------------------------------------------
        // this is made in the moment of the preloop


        // calibration i calculation
        void analyse_current_event() ;
        void  make_preloop_action(ifstream &);  // read the calibration factors, gates

        double give_position()
        {
            if(!calculations_already_done) analyse_current_event();
            return position ;
        }

        //
        double give_tof()
        {
            if(calculations_already_done == false) analyse_current_event();
            return tof ;
        }
        bool is_tof_ok()
        {
            return flag_tof_ok ;
        }
        //  string give_rel_focus_name() { return related_focal_plane->give_name() ;}

        //  /** No descriptions */
        bool is_position_ok()
        {
            if(calculations_already_done == false) analyse_current_event();
            return position_ok;
        }

        // some other devices would like to know the tracking of the ion

        double give_beta()
        {
            if(calculations_already_done == false) analyse_current_event();
            return beta ;
        }

        double  give_beta_gamma()
        {
            if(calculations_already_done == false) analyse_current_event();
            return bega ;
        }

        double  give_gamma()
        {
            if(calculations_already_done == false) analyse_current_event();
            return gamma ;
        }

    protected:
        void create_my_spectra();

    };

/////////////////////////////////////////////////////////////////////////

#endif // __CINT__
#endif // NEVER ---------------------------------
#endif // _FINGER_TOM_H_
