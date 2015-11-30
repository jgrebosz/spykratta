#ifndef _MUSIC_H_
#define _MUSIC_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "Tfrs_element.h"
#include "spectrum.h"
#include "Tfocal_plane.h"

#include <string>

#include "TIFJEvent.h"

// const int  HOW_MANY_ANODES = 4 ;
// what is it ?
// const double multiwire_constant[HOW_MANY_ANODES] = { 21, 20, 25, 25 } ;

//////////////////////////////////////////////////////////////////////////////
class Tmusic : public Tfrs_element
{
protected:
    double distance;

    double deposit_energy ;  // no initialization needed
    bool  music_ok  ;
    bool  drift_time_ok ;
    double  deposit_energy_corr ;  // initialized ok

    enum type_of_correction { self_correction = 1, mwpc_correction = 2 } how_to_correct ;

    // calibration factors for correction "self_correction:"
    double amusic_pos [6] ;
    // calibration factors for x correction "mwpc_correction:"
    double bmusic_pos [6] ;

    // calibration factors for x correction "mwpc_correction:"
    double b_y_music_pos [6] ;

#ifdef TPC42_PRESENT
    // calibration factors for correction "TPC_correction:"
    double cmusic_pos [6] ;
#endif

    double music_ang ;   // ok, but result never used

    // Spectra ------------
    // related objects
    Tfocal_plane  * related_focus ;
    double music_x ;  // x calculated earlier, in scintillator, extrapolated here

#ifdef TPC42_PRESENT
    Tfocal_plane  * related_tpc_focus;  // used for position correction not from MW but from TPC
#endif

public:
    //  constructor
    Tmusic(string _name, string  name_of_related_focus_point
#ifdef TPC42_PRESENT
           ,   string  name_of_nearest_tpc_focal_plane
#endif
          );

    bool was_ok()
    {
        if(!calculations_already_done) analyse_current_event();
        //cout << "music was_ok = " << music_ok << endl;
        return music_ok ;
    }

    double give_music_deposit_energy_corr()
    {
        if(!calculations_already_done) analyse_current_event();
        return deposit_energy_corr ;
    }

    double give_deposit_energy_non_corr()
    {
        if(!calculations_already_done) analyse_current_event();
        return deposit_energy ;
    }

    virtual
    double give_music_deposit_energy_sci_corr()
    {
        return -999;
    }

#ifdef TPC42_PRESENT
    virtual  double give_music_deposit_energy_tpc_corr()
    {
        return -999;
    }
#endif

protected:
    // void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////
#endif // __CINT__

#endif // _MUSIC_H_
