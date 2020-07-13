/***************************************************************************
                          tscaler_dt.cpp  -  description
                             -------------------
    begin                : Wed May 21 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Tscaler_dead_time.h"

Tscaler_dt::Tscaler_dt(string name,
                       int TIFJEvent::*one,
                       int TIFJEvent::*two,
                       int TIFJEvent::*seconds_)
    : Tscaler(name, one, seconds_), data_two(two)

{
    time_last = 0 ;
    previous_accepted_triggers = 0;
    previous_free_triggers = 0 ;
}
//***************************************************************************
Tscaler_dt::~Tscaler_dt()
{}

//**********************************************************************
void Tscaler_dt::analyse_current_event()
{
    //   cout << "analyse_current_event()  for "
    //           <<  name_of_this_element
    //            << endl ;

    // we calculate the dead time only after one second
    int time_now = event_unpacked->*time_of_the_measurement_in_seconds;
    if(time_now != time_last)  // is it really the next second ?
    {
        time_last = time_now ;
        // -------------------------------------------------------------
        int licznik =  event_unpacked->*data_ptr - previous_accepted_triggers ;  // accepted trigger
        int mian   =  event_unpacked->*data_two  - previous_free_triggers ; // free trigger

        //    cout << "Licznik = " << licznik << ", mianownik = " << mian << endl;
        int dead_time = 0 ;
        if(mian != 0)
        {
            dead_time = int(100.0 * licznik / mian) ; // dead time in procent
        }
        unsigned int channel_to_increment =
            event_unpacked->*time_of_the_measurement_in_seconds
            - seconds_in_channel_zero  ;


        if(channel_to_increment > 1001)  // there was some garbage
        {
            channel_to_increment = 0 ;
            seconds_in_channel_zero = event_unpacked->*time_of_the_measurement_in_seconds;
        }

        if(channel_to_increment > 998)       // was 960
        {
            // scroll by one minute
            int scroll_nr = 60 ;

            seconds_in_channel_zero += scroll_nr ;
            spec_rel->scroll_left_by_n_bins(scroll_nr);
            channel_to_increment -= scroll_nr ;
        }

        //    cout << "incrementing scaler " <<name_of_this_element
        //              << "channel " <<(int)channel_to_increment
        //              << ", with dead_time " << dead_time << endl ;

        // the first incrementation will be a nonsese !, so...

        spec_rel ->manually_inc_by((int)channel_to_increment,  dead_time) ;

        previous_counts = dead_time;   // ? not needed, but anyhow, if anybody ask for this dead time...
        previous_accepted_triggers  = event_unpacked->*data_ptr;     // accepted trigger
        previous_free_triggers  = event_unpacked->*data_two  ;         // free trigger

    }  // if this really the next second

    calculations_already_done = true ;

}
//**************************************************************************
