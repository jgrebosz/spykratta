#include "Tfocal_plane.h"
#include "Tnamed_pointer.h"

#include "Tfrs.h"

#include "Ttime_projection_chamber.h"
#ifdef TRACKING_IONISATION_CHAMBER_X
#include "Ttracking_ionisation_chamber.h"
#endif
//***********************************************************************
//  constructor
Tfocal_plane::Tfocal_plane(string name_of_focus,
                           string name_dev_before,
                           what_type     device_before_,
                           string name_dev_after,
                           what_type     device_after_
                          ) : Tfrs_element(name_of_focus),
    device_type_before(device_before_),
    device_type_after(device_after_)
{
    sci_after = 0 ;
    flag_hybrid = false;


#ifdef TRACKING_IONISATION_CHAMBER_X
    tic_before = NULL;
    tic_after = NULL;
#endif



    //================== BEFORE ===========================
    switch(device_type_before)
    {
    case mw:
        mw_before = dynamic_cast<Tmultiwire*>(owner->address_of_device(name_dev_before)) ;
        if(! mw_before)
        {
            cout <<  "Error in def " << name_of_this_element << ". The name " << name_dev_before
                 << " is  not found among Multiwires names " << endl;
            exit(1);
        }
        break;

#if defined TPC41_PRESENT || defined TPC42_PRESENT
    case tpc:
        tpc_before = dynamic_cast<Ttime_projection_chamber*>(owner->address_of_device(name_dev_before)) ;
        if(! tpc_before)
        {
            cout <<  "Error in def " << name_of_this_element << ". The name " << name_dev_before
                 << " is  not found among TPC names " << endl;
            exit(1);
        }

        break;
#endif // defined TPC41_PRESENT || defined TPC42_PRESENT

#ifdef TRACKING_IONISATION_CHAMBER_X
    case tic:
        tic_before = dynamic_cast<Ttracking_ionisation_chamber*>(owner->address_of_device(name_dev_before)) ;
        if(! tic_before)
        {
            cout <<  "Error in def " << name_of_this_element << ". The name " << name_dev_before
                 << " is  not found among TIC names " << endl;
            exit(1);
        }

        break;
#endif // defined TIC




    default:
        // before must  be MW o TPC
        cout << "Error in definiton of " << name_of_this_element
             << " Before this focal plane can be only "
             "Multiwire chamber or TPC, while you specified " << name_dev_before
             << endl;
        exit(2);
    }

    //============ AFTER =======================

    switch(device_type_after)
    {
    case mw:
        mw_after  = dynamic_cast<Tmultiwire*>(owner->address_of_device(name_dev_after));
        if(! mw_after)
        {
            cout <<  "Error in def " << name_of_this_element << ". The name " << name_dev_after
                 << " is  not found among Multiwires names " << endl;
            exit(1);
        }
        break;

#if defined TPC41_PRESENT || defined TPC42_PRESENT
    case tpc:
        tpc_after = dynamic_cast<Ttime_projection_chamber*>(owner->address_of_device(name_dev_after)) ;
        if(! tpc_after)
        {
            cout <<  "Error in def " << name_of_this_element << ". The name " << name_dev_after
                 << " is  not found among TPC names " << endl;
            exit(1);
        }
        break;
#endif // defined TPC41_PRESENT || defined TPC42_PRESENT

    case sci:
        sci_after  = dynamic_cast<Tscintillator*>(owner->address_of_device(name_dev_after));
        flag_hybrid = true;
        if(! sci_after)
        {
            cout <<  "Error in def " << name_of_this_element << ". The name " << name_dev_after
                 << " is  not found among Scintillator names " << endl;
            exit(1);
        }
        break;

#ifdef TRACKING_IONISATION_CHAMBER_X
    case tic:
        tic_after = dynamic_cast<Ttracking_ionisation_chamber*>(owner->address_of_device(name_dev_after)) ;
        if(! tic_after)
        {
            cout <<  "Error in def " << name_of_this_element << ". The name " << name_dev_after
                 << " is  not found among TIC names " << endl;
            exit(1);
        }

        break;
#endif // defined TIC


    default:

        cout << "Error in definiton of " << name_of_this_element
             << " Before this focal plane can be only "
             "Multiwire chamber or Scintillator or TPC, while you specified " << name_dev_after
             << endl;
        exit(2);
    }




    //=================
    /*
      if(mw_before == 0
          ||
          //     mw_after == 0  ||
          (mw_before == mw_after)
        )
      {
        cout << "Error. in the constructor "
        << name_of_this_element
        << " definition: \nThe multiwire chamber from "
        << "left or right side were not correctly specified.\n"
        << "Please change it and recompile the program" ;

        exit(-1) ;
      }*/

    // if all was ok.

    x_previous = 0 ;
    y_previous = 0 ;
    angle_x_previous = 0;
    angle_y_previous = 0 ;


    create_my_spectra();

    named_pointer[name_of_this_element + "_x_when_ok"]
    = Tnamed_pointer(&x, &x_ok, this) ;
    named_pointer[name_of_this_element + "_y_when_ok"]
    = Tnamed_pointer(&y, &y_ok, this) ;

    named_pointer[name_of_this_element + "_angle_x_when_ok"]
    = Tnamed_pointer(&angle_x, &x_ok, this) ;
    named_pointer[name_of_this_element + "_angle_y_when_ok"]
    = Tnamed_pointer(&angle_y, &y_ok, this) ;

    named_pointer[name_of_this_element + "_z_x_when_ok"]
    = Tnamed_pointer(&z_x, &x_ok, this) ;
    named_pointer[name_of_this_element + "_z_y_when_ok"]
    = Tnamed_pointer(&z_y, &y_ok, this) ;


    named_pointer[name_of_this_element + "_user_x_extrapolation_point"]
    = Tnamed_pointer(&user_wished_extrapolated_x, &user_wished_extrapolated_x_ok, this) ;
    named_pointer[name_of_this_element + "_user_y_extrapolation_point"]
    = Tnamed_pointer(&user_wished_extrapolated_y, &user_wished_extrapolated_y_ok, this) ;

}
//************************************************************************
void Tfocal_plane::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor
    string folder = "focal_planes/" + name_of_this_element ;

    // X focus ------------------------------------
    string name = name_of_this_element + "_x"  ;
    spec_x = new spectrum_1D(name,
                             name,
                             200, -100, 100,
                             folder, "", name_of_this_element + "_x_when_ok");
    frs_spectra_ptr->push_back(spec_x) ;


    // X_ angle ------------------------------
    name = name_of_this_element + "_angle_x"  ;

    spec_angle_x = new spectrum_1D(name,
                                   name,
                                   200, -100, 100,
                                   folder, "(in miliradians)",
                                   name_of_this_element + "_angle_x_when_ok");
    frs_spectra_ptr->push_back(spec_angle_x) ;

    // Z -------------------------------------
    name = name_of_this_element + "_z_shape_of_x_focus"  ;

    spec_z_x = new spectrum_1D(name,
                               name,
                               2100, -100, 2000,
                               folder,
                               "here we can see the real Z-postion of X focus",
                               name_of_this_element + "_z_x_when_ok");
    frs_spectra_ptr->push_back(spec_z_x) ;

    //-------------------------------------
    // Y------------------------------------
    name = name_of_this_element + "_y"  ;
    spec_y = new spectrum_1D(name,
                             name,
                             200, -200, 200,
                             folder, "", name_of_this_element + "_y_when_ok");
    frs_spectra_ptr->push_back(spec_y) ;




    // Y angle ------------------------------
    name = name_of_this_element + "_angle_y"  ;

    spec_angle_y = new spectrum_1D(name,
                                   name,
                                   200, -100, 100,
                                   folder, "(in miliradians)",
                                   name_of_this_element + "_angle_x_when_ok");
    frs_spectra_ptr->push_back(spec_angle_y) ;

    // Z_Y -------------------------------------
    name = name_of_this_element + "_z_shape_of_y_focus"  ;

    spec_z_y = new spectrum_1D(name,
                               name,
                               2100, -100, 2000,
                               folder, "here we can see the real Z-postion of Y focus",
                               name_of_this_element + "_z_y_when_ok");
    frs_spectra_ptr->push_back(spec_z_y) ;


}
//**********************************************************************
void Tfocal_plane::analyse_current_event()
{
    //   cout << "make_calculation for " <<  name_of_this_element << endl ;

    // initially
    x_ok = false ;
    y_ok = false ;

    x = -888 ;
    y = -888;

    angle_x = x = angle_y = y = -888;


    // to write shorter
    //   Tmultiwire &mw_first = *mw_before ;
    //   Tmultiwire &mw_Asecond = *mw_after ;


    first_x = 0, first_y = 0, first_distance = 0;
    flag_first_x_ok  = false;
    flag_first_y_ok  = false;

    switch(device_type_before)
    {
    case mw:
        flag_first_x_ok  =  mw_before->was_x_ok();
        flag_first_y_ok  =  mw_before->was_y_ok();
        first_x  =  mw_before->give_x();
        first_y  =  mw_before->give_y();
        first_distance  =  mw_before->give_distance();
        break;

#if defined TPC41_PRESENT || defined TPC42_PRESENT
    case tpc:
        flag_first_x_ok  =  tpc_before->was_x_ok();
        flag_first_y_ok  =  tpc_before->was_y_ok();
        first_x  =  tpc_before->give_x();
        first_y  =  tpc_before->give_y();
        first_distance  =  tpc_before->give_distance();
        break;
#endif // defined TPC41_PRESENT || defined TPC42_PRESENT

    case sci:
        cout << name_of_this_element <<  " analysis: Sci "
             << " is not expected to be on this position" << endl;
        return;
        break;


#if defined TRACKING_IONISATION_CHAMBER_X
    case tic:
        flag_first_x_ok  =  tic_before->was_x_ok();
        flag_first_y_ok  =  tic_before->was_y_ok();
        first_x  =  tic_before->give_x();
        first_y  =  tic_before->give_y();
        first_distance  =  tic_before->give_distance();
        break;
#endif // defined TIC


    }

    // Second device, this on which is "after"
    second_x = 0, second_y = 0, second_distance = 0;
    flag_second_x_ok  = false;
    flag_second_y_ok  = false;

    switch(device_type_after)
    {
    case mw:
        flag_second_x_ok  =  mw_after->was_x_ok();
        flag_second_y_ok  =  mw_after->was_y_ok();
        second_x  =  mw_after->give_x();
        second_y  =  mw_after->give_y();
        second_distance  =  mw_after->give_distance();
        break;

#if defined TPC41_PRESENT || defined TPC42_PRESENT
    case tpc:
        flag_second_x_ok  =  tpc_after->was_x_ok();
        flag_second_y_ok  =  tpc_after->was_y_ok();
        second_x  =  tpc_after->give_x();
        second_y  =  tpc_after->give_y();
        second_distance  =  tpc_after->give_distance();
        break;
#endif //defined TPC41_PRESENT || defined TPC42_PRESENT

    case sci:
        flag_second_x_ok  =  sci_after->is_position_ok();
        second_x  =  sci_after->give_position();
        second_distance  =  sci_after->give_distance();
        // sci does not give y information
        break;


#if defined TRACKING_IONISATION_CHAMBER_X
    case tic:
        flag_second_x_ok  =  tic_after->was_x_ok();
        flag_second_y_ok  =  tic_after->was_y_ok();
        second_x  =  tic_after->give_x();
        second_y  =  tic_after->give_y();
        second_distance  =  tic_after->give_distance();

        //     cout << "For " << name_of_this_element
        //     << "first x ok = " <<   flag_first_x_ok
        //     << ", first_x = " << first_x
        //     << ",     second x ok = " <<   flag_second_x_ok
        //     << ", second_x = " << second_x << endl;

        break;
#endif //defined TIC


    }



    //-----------------------------------------------
    // calculating  X-focus  ------------------------
    //-----------------------------------------------

    user_wished_extrapolated_x = -999;
    user_wished_extrapolated_x_ok = false;
    user_wished_extrapolated_y = -999;
    user_wished_extrapolated_y_ok  = false;

    // idea:
    // Focus in point S2 we can calculate knowing results of MW 21 and 22 which are
    // before and after S2 point
    //   The algorythm is just a TALES calculation to interpolate the x and y
    // coordinate in the point S2


    // warning: the mw21 and mw22 are IN only during calibrating period
    // later they are removed


    if(flag_first_x_ok
            &&
            flag_second_x_ok  // <- if both multiwires were inside
            &&
            // they are 200 mm, but only 160 mm are active
            (first_x >= -100) && (first_x <= 100)
            &&
            // they are 200 mm, but only 160 mm are active
            (second_x  >= -100) && (second_x  <= 100))
    {
        // extrapolating to this focusing point----------------
        //  X-angle at S2 in mrad  (mili rad => 1000)

        angle_x = (second_x - first_x)
                  /
                  (second_distance - first_distance) * 1000.0  ;

        //       cout << "  second_x " <<  second_x
        //     << "  first_x  " <<  first_x
        //     << "  second_distance " <<  second_distance
        //     << " first_distance " << first_distance
        //     << " SO angle = " <<  angle_x
        //     << endl ;


        // distance below - it is the distance of THIS focusing point


        x = second_x + ((distance - second_distance) * angle_x / 1000.0) ;
        //     cout << " second_x=" << second_x
        //     << ", distance= " << distance
        //     << ", second_distance= " << second_distance
        //     << ", angle_x= " << angle_x
        //     << "x = " << x << endl;

        user_wished_extrapolated_x_ok = true;
        user_wished_extrapolated_x =
            second_x +
            ((user_x_extrapolation_point - second_distance) * angle_x / 1000.0) ;
        /*    cout << " second_x=" << second_x
            << ", distance= " << distance
            << ", second_distance= " << second_distance
            << ", angle_x= " << angle_x
            << "x = " << x
            << ", user_wished_extrapolated_x=" << user_wished_extrapolated_x
            << endl;*/

        //       cout <<" second_x "  << second_x
        //     << " distance = " << distance
        //     << "  mw_second.give_distance()" <<  mw_second.give_distance()
        //     << endl ;
        //
        //      cout << "============\nSpectrum for " << name_of_this_element << " incremented "
        //     << ", angle_x = " << angle_x
        //     << ", x " << x
        //     << endl ;

        //     if(name_of_this_element == "s2tic")
        //     {
        //       cout << "For " << name_of_this_element << " x = " << x << endl;
        //     }
        spec_angle_x ->manually_increment(angle_x);
        spec_x->manually_increment(x);

        x_ok = true ;   // valid

        z_x = 0 ;

        // -------real z-postion of S2 x focus in cm
        // that means: if in the S2 point we are out of focus (we see some elypsis
        // instead of the beautifull point, we would like to know where is the real
        // focus, - how many cm after (or before) of S2 point. [This is along the z-axis.
        // This axis is along the whole FRS

        long double rh = angle_x - angle_x_previous ;

        if(fabs(rh) > 1e-4)  // if it is remarkably different from the previously saved
        {
            // if yes, calculate the new one
            z_x = ((x_previous - x) / rh) * 100. + distance / 10. ;
            x_previous = x ;
            angle_x_previous = angle_x ;

            // it was only for s4
            if(z_x > 10000 || z_x < -100)
                z_x = 0 ;
            //else
            {
                //      cout << "Spectrum z_x " << name_of_this_element << " incremented "
                //     << ", z_x = " << z_x
                //     << endl ;

                spec_z_x->manually_increment(z_x);
            }


            //extrapolate_x_to_distance( );

        } // endif
    } // endif



    //===============================================
    // Y-focus  ----------------------------------
    //===============================================

    if(! flag_hybrid)
    {
        // warning: the mw21 and mw22 are IN only during calibrating period
        // later they are removed

        //     double second_y = mw_Asecond.give_x() ;
        //     bool flag_second_y_ok  = mw_Asecond.was_x_ok() ;
        //     second_distance  = mw_Asecond.give_distance();


        if(flag_first_y_ok   // testing if they are IN
                &&
                flag_second_y_ok // testing if they are IN
                &&
                (first_y >= -80) && (first_y <= 80) &&
                (second_y >= -80) && (second_y <= 80))
        {
            // Y-angle at  in mrad
            angle_y = (second_y - first_y)
                      /
                      (second_distance - first_distance) * 1000.0  ;


            //    cout << "  second_y " <<  second_y
            //     << "  first_y  " <<  first_y
            //     << " second_distance " <<  second_distance
            //     << " first_distance " << first_distance
            //     << " SO angle = " <<  angle_y
            //     << endl ;


            y = second_y + ((distance - second_distance) * angle_y / 1000.0) ;
            y_ok = true ;   // valid

            user_wished_extrapolated_y_ok = true;
            user_wished_extrapolated_y =
                second_y +
                ((user_x_extrapolation_point - second_distance) * angle_y / 1000.0) ;

            //      cout << "------- Spectrum for " << name_of_this_element << " incremented "
            //     << ", angle_y = " << angle_y
            //     << ", y " << y
            //     << endl ;

            spec_angle_y ->manually_increment(angle_y);
            spec_y->manually_increment(y);


            //--------------------------------------------------------------
            //   NOTE:

            // for focusing point we can also extrapolate till the scintillator sc21
            // sc21_y  = second_y + (dist_sc21 - mw_second->distance) * angle_y / 1000.0 ;
            // but this will be done if somebody (scintillator)ask for this by calling
            // the function  extrapolate....

            // for focusing point S4 we can make
            //         y_music  NO ?
            //         sc41_y
            //         sc42_y
            //         y_s4tgt
            //         all music anodes positions
            //----------------------------------------------------------------

            z_y = 0 ;

            long double rh = angle_y - angle_y_previous ;
            // -------real z-postion of S2 y   focus in cm
            // (see remark for X focus)
            if(fabs(rh) > 1e-4)
            {
                z_y = ((y_previous - y) / rh) * 100. + distance / 10. ;
                y_previous = y;
                angle_y_previous = angle_y;

                spec_z_y->manually_increment(z_y);

                //    cout << "Spectrum z_y " << name_of_this_element << " incremented "
                //         << ", z_y = " << z_y
                //         << endl ;

            } // endif
        } // endif

    }// if not hybrid, but real, with  two multiwires

    calculations_already_done = true ;
}
//**************************************************************************
// read the calibration factors, gates
//**************************************************************************
void  Tfocal_plane::make_preloop_action(ifstream & s)
{
    //no calibration, no gates, but...
    distance = Tfile_helper::find_in_file(s,  name_of_this_element  + "_distance");

    try
    {
        user_x_extrapolation_point =
            Tfile_helper::find_in_file(s,  name_of_this_element  + "_user_x_extrapolation_point");
        cout << "Succesfully read value = " << user_x_extrapolation_point
             << endl;
    }
    catch(...)   // because it is optional
    {
        cout << name_of_this_element
             << "Info: no specification of the user extrapolation point" << endl;
        user_x_extrapolation_point = 0 ;
        s.clear() ;

    }



}
//**************************************************************************
// some other devices would like to know the tracking of the ion
//**************************************************************************
double  Tfocal_plane::extrapolate_x_to_distance(double some_dist)
{
    // what to do, if calculations are not ok ?
    if(!calculations_already_done)
        analyse_current_event();

    if(x_ok)
    {
        //  cout << "mw_x[5] = " <<  second_x
        //     << " s4_angle_x=" <<  angle_x
        //     << " dist_mw42=" << second_distance
        //     << endl ;
        return  second_x + ((some_dist - second_distance) * angle_x / 1000.0) ;
    }
    else
        return -888 ;  // funny value , perhaps the exception should be thrown here ???

}
//**************************************************************************
double  Tfocal_plane::extrapolate_y_to_distance(double some_dist)
{
    if(!calculations_already_done)
        analyse_current_event();

    // what to do, if calculations are not ok ?

    if(y_ok)
        return  second_y + ((some_dist - second_distance) * angle_y / 1000.0) ;
    else
        return -888 ;  // funny value  , perhaps the exception should be thrown here ???

}
//***********************************************************************
/** to obtain the vector - usefull for scattering angle on target */
bool Tfocal_plane::give_projectile_vector(double *vx, double * vy, double *vz, double *len)
{
    if(!calculations_already_done)
        analyse_current_event();
    *vx = second_x  - first_x ;
    *vy = second_y  - first_y ;
    *vz = second_distance  - first_distance ;

    *len = sqrt((*vx) * (*vx) + (*vy) * (*vy) + (*vz) * (*vz)) ;

    return (flag_first_y_ok  &&  flag_second_y_ok
            &&
            flag_first_x_ok  &&  flag_second_y_ok) ;

}
