#ifndef _PISOLO_DELTA_E_H_
#define _PISOLO_DELTA_E_H_

#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

class Tpisolo_tof;
class Tpisolo_si_final;

#define HOW_MANY_DELTA_E  2
//////////////////////////////////////////////////////////////////////////////
class Tpisolo_delta_e : public Tfrs_element, public Tincrementer_donnor
{

    int  deltaE_raw[HOW_MANY_DELTA_E];      //  delta E1, delta E2
    double deltaE_cal[HOW_MANY_DELTA_E];      //  delta E1, delta E2
    int  dE_raw;          // residual energy
    double dE_cal;     // residual energy

    // calibration factors
    double slope[HOW_MANY_DELTA_E], offset[HOW_MANY_DELTA_E] ;

    double dE_slope, dE_offset;

    // did we make good calculations for this event
    bool  dE_ok;

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int (TIFJEvent::*deltaE_event)[HOW_MANY_DELTA_E];
    int TIFJEvent::*dE_res_event;

    spectrum_1D * spec_dE_raw[HOW_MANY_DELTA_E] ;
    spectrum_1D * spec_dE_cal[HOW_MANY_DELTA_E] ;

    // to make matrices we need pointer to the Tof object and Sililcon obj
    Tpisolo_tof *     tof_ptr;
    Tpisolo_si_final  * e_final_ptr;

    double Esum_cal;
    int Esum_raw;

    double mass;      // mass = a * Esum * b(-ToF1) ^2
    double  mass_a_factor;
    double mass_b_factor;

    spectrum_1D * spec_Esum_raw;
    spectrum_1D * spec_Esum_cal;
    spectrum_1D * spec_dE_res_raw;
    spectrum_1D * spec_dE_res_cal;
    spectrum_2D *   spec_dE_vs_Esum;
    spectrum_2D *   spec_tof1_vs_Esum;
    spectrum_2D *   spec_tof1_vs_dE;
    spectrum_1D * spec_mass;


public:
    //  constructor
    Tpisolo_delta_e(string _name,  int (TIFJEvent::*delta_E)[HOW_MANY_DELTA_E] ,
                    int  TIFJEvent::*dE_res,
                    string name_of_tof,
                    string name_of_final_si) ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates

    // the scintillators may ask, to perform position calculation
    bool was_dE_ok()
    {
        if(calculations_already_done == false) analyse_current_event();
        return  dE_ok  ;
    }

    //*----------------------

protected:
    void create_my_spectra();
};
/////////////////////////////////////////////////////////////////////////
#endif // _MICRO_CHANNEL_PLATE_H_

