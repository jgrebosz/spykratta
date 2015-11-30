#ifndef _CLOVER_H_
#define _CLOVER_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__


#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>

#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"


//////////////////////////////////////////////////////////////////////////////
class Tclover : public Tfrs_element, public Tincrementer_donnor
{


    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

//  int TIFJEvent::*energy_data;
//  int TIFJEvent::*time_data;

    int (TIFJEvent::*energy_data)[14];
    int (TIFJEvent::*time_data)[4];


    double cc_energy_raw[4];
    double cc_energy_cal[4];

    double cc_time_raw[4];
    double cc_time_cal[4];

    int multiplicity_cc ;


    struct one_segment
    {
        vector<double> energy_calibr_factors ;
        vector<double> time_calibr_factors ;
    } cc_segment[4];

    spectrum_1D * spec_e_cc_raw[4] ;
    spectrum_1D * spec_e_cc_cal[4] ;


    spectrum_1D * spec_e_pos_1 ;
    spectrum_1D * spec_e_pos_2 ;
    spectrum_1D * spec_e_pos_3 ;
    spectrum_1D * spec_e_pos_4 ;
    spectrum_1D * spec_e_pos_5 ;
    spectrum_1D * spec_e_pos_6 ;
    spectrum_1D * spec_e_pos_7 ;
    spectrum_1D * spec_e_pos_8 ;
    spectrum_1D * spec_e_pos_9 ;

    spectrum_1D * spec_e_k7 ;

    spectrum_1D * spec_t_cc_raw[4] ;
    spectrum_1D * spec_t_cc_cal[4] ;

    spectrum_1D * spec_e_cc_total ;
    spectrum_1D * spec_t_cc_total ;

public:
    //  constructor
    Tclover(string _name, int (TIFJEvent::*en_data_ptr)[14], int (TIFJEvent::*ti_data_ptr)[4]) ;


    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates

    // the scintillators may ask, to perform position calculation
protected:

    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // CINT

#endif // _CLOVER_H_
