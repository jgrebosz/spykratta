#ifndef _FOCAL_PLANE_H_
#define _FOCAL_PLANE_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__


#include "Tfrs_element.h"

#include "Tmultiwire.h"
#include "spectrum.h"
#include <string>
#include "Tincrementer_donnor.h"


class Tmultiwire ;
class Tscintillator;
class Ttime_projection_chamber;
class Ttracking_ionisation_chamber;  // munich invention

//////////////////////////////////////////////////////////////////////////////
class Tfocal_plane : public Tfrs_element, public Tincrementer_donnor
{
protected:
    double distance ;

    // did we make good calculations for this event
    bool
    x_ok,
    y_ok ;

    double
    x,    // position on the scintillator
    y ;   // position

    double x_previous ;
    double y_previous ;

    double angle_x_previous ;
    double angle_y_previous ;

    double
    angle_x ,
    angle_y ;



    double
    z_x, z_y ;   // real position of the focus point (how far away from this
    // point along the z axis

    double user_wished_extrapolated_x;
    bool user_wished_extrapolated_x_ok;

    double   user_wished_extrapolated_y;
    bool   user_wished_extrapolated_y_ok;


    Tmultiwire * mw_before ; // multiwire from the left of scintillator
    Tmultiwire * mw_after ; // multiwire from the right of scintillator
    Tscintillator *   sci_after ; // sci a from the right of focal plane

    Ttime_projection_chamber * tpc_before;
    Ttime_projection_chamber * tpc_after;

#ifdef TRACKING_IONISATION_CHAMBER_X
    Ttracking_ionisation_chamber* tic_before;
    Ttracking_ionisation_chamber* tic_after;
#endif

    double user_x_extrapolation_point;

public:
    enum what_type { mw = 555, sci
#if defined TPC41_PRESENT || defined TPC42_PRESENT
                     , tpc
#endif
#ifdef TRACKING_IONISATION_CHAMBER_X
                     , tic
#endif
                   } ;
protected:
    what_type      device_type_before;
    what_type      device_type_after;


    spectrum_1D * spec_x ;
    spectrum_1D * spec_angle_x ;
    spectrum_1D * spec_z_x ;

    spectrum_1D * spec_y ;
    spectrum_1D * spec_angle_y ;
    spectrum_1D * spec_z_y ;

    double first_x, first_y, first_distance;
    bool flag_first_x_ok;
    bool flag_first_y_ok;

    double second_x, second_y, second_distance;
    bool flag_second_x_ok;
    bool flag_second_y_ok;


public:

    //  constructor
    Tfocal_plane(string _name, string name_device_before,
                 what_type device_before_,
                 string name_mw_after,
                 what_type device_after_
                );


    // calibration i calculation
    void analyse_current_event() ;
    void  make_preloop_action(ifstream &);   // read the calibration factors, gates

    // some other devices would like to know the tracking of the ion
    double extrapolate_x_to_distance(double dist) ;
    double extrapolate_y_to_distance(double dist) ;

    bool was_x_ok()
    {
        if(!calculations_already_done) analyse_current_event();
        //cout << "Focus X ok = " << foc_x_ok << endl ;
        return x_ok;
    }

    bool was_y_ok()
    {
        if(!calculations_already_done) analyse_current_event();
        //cout << "Focus Y ok = " << foc_y_ok << endl ;
        return y_ok;
    }

    double give_distance_of_second_mw()
    {
        if(!calculations_already_done) analyse_current_event();
        return mw_after->give_distance() ;
    }

    double give_x()
    {
        if(!calculations_already_done) analyse_current_event();
        return x ;
    } // need for aoq calucl


    double give_angle_x()
    {
        if(!calculations_already_done) analyse_current_event();
        return angle_x ;
    } // need for aoq correction

    // target will ask for this to calculate cosinus theta
    Tmultiwire * ptr_multiwire_before()
    {
        return mw_before ;    // multiwire from the left of scintillator
    }
    Tmultiwire * ptr_multiwire_after()
    {
        return mw_after ;    // multiwire from the right of scintillator
    }
    /** to obtain the vector - usefull for scattering angle on target */
    bool give_projectile_vector(double *vx, double * vy, double *vz, double *len);


protected:
    void create_my_spectra();
    bool flag_hybrid ;

};

/////////////////////////////////////////////////////////////////////////
#endif // __CINT__

#endif // _FOCAL_PLANE_H_
