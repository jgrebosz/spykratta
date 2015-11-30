/***************************************************************************
                          tscaler_dt.h  -  description
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

#ifndef TSCALER_DT_H
#define TSCALER_DT_H


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__


#include <Tscaler.h>


/**
 *@author Jurek Grebosz (IFJ Krakow, Poland)
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tscaler_dt : public Tscaler
{
protected:
    int TIFJEvent::*data_two;
    int time_last ;  // we calculate the dead time only after one, full second (at least)
    int previous_accepted_triggers;
    int previous_free_triggers ;
public:
    Tscaler_dt(string name,
               int TIFJEvent::*one,
               int TIFJEvent::*two,
               int TIFJEvent::*seconds_);
    ~Tscaler_dt();

    void analyse_current_event();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif  // __CINT__

#endif

