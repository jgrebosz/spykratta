/***************************************************************************
                          tscaler.h  -  description
                             -------------------
    begin                : Tue May 20 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/


#ifndef TSCALER_H
#define TSCALER_H


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>

#include "TIFJEvent.h"
#include <time.h>
#include "Tincrementer_donnor.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**FRS scalers
  *@author Jurek Grebosz (IFJ Krakow, Poland)
  */
////////////////////////////////////////////////////////////////////////
class Tscaler : public Tfrs_element,  public Tincrementer_donnor
{
protected:
    int TIFJEvent::*data_ptr;
    int TIFJEvent::*time_of_the_measurement_in_seconds ;

    int previous_counts ;  // this we will need to exist outside this object, for users
    int last_current_counts ;  // here we store
    int current_counts ;
    int difference_value ;

    int last_incremented_channel ;

    int suwak;
    long long timestamps_FRS_at_beg_of_current_suwak_bin ;

    // seconds in channel zero
    int seconds_in_channel_zero ;

    spectrum_1D * spec_rel ;
    double right_edge;

    // sometime we want to konw how many we had in the last unit of time
    // (difference says only the difference since last event)
    int counts_diff_in_last_unit_of_time;

    int previous_time_of_the_measurement_in_seconds;
    int counts_at_begining_of_last_unit ; // second;

public:
    Tscaler(string _name, int TIFJEvent::*data,   int TIFJEvent::*seconds_) ;
    ~Tscaler();

    // calibration i calculation
    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates
    /** dead time scaler may ask other scalers for their results
     */
    int give_last_value();
    int give_curr_diff_value()
    {
        return difference_value ;
    }

protected:
    void create_my_spectra();

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif  // _CINT__
#endif
