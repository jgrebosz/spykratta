/***************************************************************************
                          tkratta.h  -  description
                             -------------------
    begin                : Oct 12 2014
    copyright            : (C) 2003 by dr Jerzy Grebosz
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/

#ifndef TKRATTA_H
#define TKRATTA_H


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#ifdef KRATTA_PRESENT

const int how_many_kratta_segments = 9 ;


#include "Tkratta_crystal.h"

//class TIFJEvent;
class TRisingCalibratedEvent;

#include <vector>
#include "spectrum.h"
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"

class Tsi_detector;
//////////////////////////////////////////////////////////////////////
/**Class representing the KRATTA detector of the Rising
 *@author Jerzy Grebosz
 */
//////////////////////////////////////////////////////////////////////
class Tkratta : public Tfrs_element, public Tincrementer_donnor
{
    friend class Tkratta_crystal ;
protected:
    double distance ;
    vector<Tkratta_crystal *>  crystal ;     // segments of the kratta detector

public:
    Tkratta(string name);
    ~Tkratta();

    //-------------------------------
  vector< spectrum_abstr*>*  spectra_ptr()
  {
    return frs_spectra_ptr  ;
  }

    Tkratta_crystal *  give_crystal(unsigned nr)
    {
        if(nr >= (unsigned int) KRATTA_NR_OF_CRYSTALS) return nullptr;
        return crystal[nr] ;
    }
    //------------------------------
    void make_preloop_action(ifstream &) ;  // read the calibration factors, gates

    // taking the calibration factors
    void read_calibration_and_gates_from_disk();  // called form preloop function

    void analyse_current_event();
    void make_user_event_action(); // shop of events ?
    void make_postloop_action();  // saving the spectra ?




    inline double give_distance() {
        return distance ;
    }
    /** made in post loop, but also when the FRS ask for it (i.e. every 5 min) */
    void save_spectra();

    /** kratta will ask the Si detector for this information */

    int how_many_Si_hits();


    //------------------------------
protected:

    //for testing time

    void simulate_event();
    /** No descriptions */
    void create_my_spectra();

    int multiplicity_of_hits;

//     spectrum_2D  *matr_position_xy;

    spectrum_1D * spec_multiplicity;
    spectrum_1D * spec_fan ;
};
//////////////////////////////////////////////////////////////////////////////

#endif  // KRATTA_PRESENT
#endif // __CINT__

#endif
