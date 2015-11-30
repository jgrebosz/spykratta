#ifndef _MUSIC_4ANODES_H_
#define _MUSIC_4ANODES_H_

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

const int  FOUR_ANODES = 4 ;


//////////////////////////////////////////////////////////////////////////////
class Tmusic_4anodes : public Tmusic, public Tincrementer_donnor
{
    //double distance;

    int time[ FOUR_ANODES] ;
    int energy[ FOUR_ANODES] ;



    //double music_deposit_energy ;  // no initialization needed
    //bool  music_ok  ;
    //bool  music_drift_time_ok ;
    bool  time_ok[FOUR_ANODES];

    double position[ FOUR_ANODES] ; // no initialization needed
    double z_coordinate[FOUR_ANODES] ;                   // music 'z' coordinate of particular anode


    double dist_music_anode[FOUR_ANODES];
    double pedestal[FOUR_ANODES]  ;

    // calibration factors for all anodes
    double
    time_offset[FOUR_ANODES],
                time_slope[FOUR_ANODES];

    //double  deposit_energy_corr ;  // initialized ok

    //enum type_of_correction { self_correction = 1, mwpc_correction =2 } how_to_correct ;




    // calibration factors for correction "self_correction:"
    //double amusic_pos [6] ;
    // calibration factors for correction "mwpc_correction:"
    //double bmusic_pos [6] ;


    //double music_ang ;   // ok, but result never used

    double  x_musica[FOUR_ANODES]; // calculated by scintillators, but we
    // will ask for that using the 'extrapolate' function from focusing point (s4)
    bool    x_musica_ok[FOUR_ANODES];
    double music_x_mean ;



    /*   int *times_array;  // ptr to the energy data in musics */
    /*   int *energies_array ;   // as above */


    int (TIFJEvent::*time_array)[FOUR_ANODES] ;
    int (TIFJEvent::*energy_array)[FOUR_ANODES] ;


    // Spectra ------------
    spectrum_1D
    *spec_energy[FOUR_ANODES],
    *spec_time[FOUR_ANODES],
    *spec_music_deposit_energy,
    *spec_music_deposit_energy_corr_self,
    *spec_music_deposit_energy_corr_mwpc,
    *spec_music_rms,
    *spec_music_x_mw,
    *spec_x_musica_mw[FOUR_ANODES];


    spectrum_2D
    * matr_music_de_vs_x_mean__self ,
    * matr_music_x_mean_mwpc_vs_de;

#ifdef TPC42_PRESENT
    spectrum_2D *  matr_music_x_mean_tpc_vs_de;
    spectrum_1D *  spec_music_deposit_energy_corr_tpc;
#endif

    // related objects
    //Tfocus_point  * related_focus ;


    //double music_x ;  // x calculated earlier, in scintillator, extrapolated here


    bool anode_works [FOUR_ANODES];
    int how_many_works ;

public:
    //  constructor
    Tmusic_4anodes(string _name,
                   int (TIFJEvent::*time_array_ptr)[FOUR_ANODES],
                   int (TIFJEvent::*energy_array_ptr)[FOUR_ANODES],
                   string  name_of_related_focus_point
#ifdef TPC42_PRESENT
                   ,   string  name_of_nearest_tpc_focal_plane
#endif
                  );


    void analyse_current_event() ;  // calculations
    void make_preloop_action(ifstream &);  // read the calibration factors, gates
    /** To incremet spectra gated by the identified isotope */
    //  void isotope_was_identified();

    //  bool was_ok()
    //    {
    //      if(!calculations_already_done) analyse_current_event();
    //      //cout << "music was_ok = " << music_ok << endl;
    //      return music_ok ;
    //    }

    //  double give_music_deposit_energy_corr()
    //    {
    //      if(!calculations_already_done) analyse_current_event();
    //      return music_deposit_energy_corr ;
    //    }


protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////
#endif // __CINT__

#endif // _MUSIC_4ANODES_H_
