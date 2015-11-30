#ifndef _FINGER_H_
#define _FINGER_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__



#include "Tfrs_element.h"
#include "spectrum.h"
#include "Tfocal_plane.h"

#include <string>

#include "TIFJEvent.h"

//////////////////////////////////////////////////////////////////////////////
class Tfinger : public Tfrs_element
{

    double distance ;


    int finger_data[15];
    double finger_low_gate[15];
    double finger_high_gate[15];


    // did we make good calculations for this event
    bool   position_ok ;
    bool   e_ok;

    double e;          // calculted energy
    double position  ; // calculated position


    double cal_factor_offset, cal_factor_slope, cal_factor_quadratic ;



    Tfocal_plane *related_focal_plane ;

    // source of data
    int (TIFJEvent::*finger)[15] ; // elem array



    spectrum_1D  *spec_finger[15],
                 *spec_finger_posX[15] ;
    spectrum_2D  *matr_mw_sci ;  // matrix to fit the calibration line



public:
    //  constructor
    Tfinger(string _name,
            string name_of_focal_plane,
            int (TIFJEvent::*finger_ptr)[15]);

    //--------------------------------------------------
    // this is made in the moment of the preloop


    // calibration i calculation
    void analyse_current_event() ;
    void  make_preloop_action(ifstream &);  // read the calibration factors, gates

//  double give_position()
//    {
//      if(!calculations_already_done) analyse_current_event();
//      return position ;
//    }
//
//  double give_rel_focus_x()
//    {
//      if(calculations_already_done == false ) analyse_current_event();
//      return related_focal_plane->give_x() ;
//    }

//  string give_rel_focus_name() { return related_focal_plane->give_name() ;}

//  /** No descriptions */
//  bool is_position_ok()
//  {
//    if(calculations_already_done == false ) analyse_current_event();
//      return position_ok;
//  }


protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////

#endif // __CINT__

#endif // _FINGER_H_
