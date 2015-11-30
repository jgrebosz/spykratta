#ifndef _MUSIC_8ANODES_H_
#define _MUSIC_8ANODES_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__


#include "Tmusic.h"
#include "spectrum.h"
#include "Tfocal_plane.h"
#include <string>


#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

const int  EIGHT_ANODES = 8 ;

class Tscintillator;
//////////////////////////////////////////////////////////////////////////////
class Tmusic_8anodes : public Tmusic, public Tincrementer_donnor
{

    int time[ EIGHT_ANODES] ;
    int energy[ EIGHT_ANODES] ;

    bool  time_ok[EIGHT_ANODES];

    double position[ EIGHT_ANODES] ; // no initialization needed
    double z_coordinate[EIGHT_ANODES] ;                   // music 'z' coordinate of particular anode

    double dist_music_anode[EIGHT_ANODES];
    double pedestal[EIGHT_ANODES]  ;

    // calibration factors for all anodes
    double
    time_offset[EIGHT_ANODES],
                time_slope[EIGHT_ANODES];

    double  x_musica[EIGHT_ANODES]; // calculated by scintillators, but we
    // will ask for that using the 'extrapolate' function from focusing point (s4)
    bool    x_musica_ok[EIGHT_ANODES];
    double music_x_mean ;


    Tscintillator *nearest_scintillator;
    double x_from_scintillator;
    spectrum_2D * matr_music_x_mean_scipc_vs_de;
    double deposit_energy_corr_sci;
    bool music_sci_corr_ok;
    spectrum_1D *spec_music_deposit_energy_corr_sci;

#ifdef TPC42_PRESENT
    double deposit_energy_corr_tpc;
    bool music_tpc_corr_ok;
    spectrum_1D *spec_music_deposit_energy_corr_tpc;
#endif

    int (TIFJEvent::*time_array)[EIGHT_ANODES] ;
    int (TIFJEvent::*energy_array)[EIGHT_ANODES] ;

    // Spectra ------------
    spectrum_1D
    *spec_energy[EIGHT_ANODES],
    *spec_time[EIGHT_ANODES],
    *spec_music_deposit_energy,
    *spec_music_deposit_energy_corr_self,
    *spec_music_deposit_energy_corr_mwpc,
    *spec_music_rms,
    *spec_music_x_mw,
    *spec_x_musica_mw[EIGHT_ANODES];


#ifdef MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED
    bool flag_make_y_mw_correction ;
    spectrum_1D   *spec_music_y_mw;
    spectrum_1D   *spec_y_musica_mw[EIGHT_ANODES];
    spectrum_2D   * matr_music_y_mean_mwpc_vs_de;
#endif

    spectrum_2D
    * matr_music_de_vs_x_mean__self ,
    * matr_music_x_mean_mwpc_vs_de;

#ifdef TPC42_PRESENT
    spectrum_2D
    *matr_music_x_mean_tpc_vs_de;
    double deposit_energy_corr_tpc_without_atmospheric_pressure;
    spectrum_1D *spec_deposit_energy_corr_tpc_without_atmospheric_pressure_corr;


#endif

    bool anode_works [EIGHT_ANODES];
    int how_many_works ;

#ifdef MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE
    int *atmospheric_pressure_in_event ;

    bool flag_make_atmospheric_pressure_correction_of_dE;
    double reference_pressure;
    int current_pressure_raw;                                        // taken from the event
    double of_pressure, slo_pressure, qua_pressure; // calibration factors
    double current_pressure_cal;

    double deposit_energy_corr_without_atmospheric_pressure;
    double deposit_energy_corr_sci_without_atmospheric_pressure;

    spectrum_1D *spec_pressure_raw;
    spectrum_1D *spec_pressure_cal;
    spectrum_1D *spec_deposit_energy_corr_without_atmospheric_pressure_corr;
    spectrum_1D *spec_deposit_energy_corr_sci_without_atmospheric_pressure_corr;

#endif


public:
    //  constructor
    Tmusic_8anodes(string _name,
                   int (TIFJEvent::*time_array_ptr)[EIGHT_ANODES],
                   int (TIFJEvent::*energy_array_ptr)[EIGHT_ANODES],
                   string  name_of_related_focus_point,
                   string  name_of_nearest_scintillator
#ifdef TPC42_PRESENT
                   ,   string  name_of_nearest_tpc_focal_plane
#endif
                  );


    void analyse_current_event() ;  // calculations
    void make_preloop_action(ifstream &);  // read the calibration factors, gates
    /** To incremet spectra gated by the identified isotope */

    double give_music_deposit_energy_sci_corr()
    {
        if(calculations_already_done == false) analyse_current_event();
        return  deposit_energy_corr_sci;
    }
#ifdef TPC42_PRESENT
    double give_music_deposit_energy_tpc_corr()
    {
        if(calculations_already_done == false) analyse_current_event();
        return  deposit_energy_corr_tpc;
    }
#endif
protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////
#endif // __CINT__

#endif // _MUSIC_8ANODES_H_
