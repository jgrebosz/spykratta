/***************************************************************************
                          Tlookup_table_miniball.h  -  description
                             -------------------
    begin                : 9 nov 2004
    copyright            : (C) 2004 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#ifndef TLOOKUP_TABLE_MINIBALL_H
#define TLOOKUP_TABLE_MINIBALL_H


#include <vector>
using namespace std;
#include <cstdlib>
/**Lookup table for mapping cables into clusters numbers
 *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
 */
class triplet
{
public:
    short int cryostat, detector, signal;
    triplet(short c = -1, short d = -1, short s = -1) : cryostat(c), detector(d), signal(s) {} ;
};

/////////////////////////////////////////////////////////////////////////////
class Tlookup_table_miniball
{
    vector<vector< vector< triplet > > >table;

public:
    Tlookup_table_miniball();
    /** Reading the settings, which experimetnator stored on the disk */
    void read_from_disk();
    ~Tlookup_table_miniball();
    /** prepares the resutlts, if not found - retruns bool = false
          If found = true, the you can call inline functions for
          getting the results */
    bool current_combination(unsigned int crate,
                             unsigned int slot,
                             unsigned int channel,
                             int *cryostat,
                             int *detector,
                             int *signal
                            );


protected: // Protected attributes
    /** max_crate nr - for reservation and check purposes */
    unsigned int  max_crate;
    /** For reservation and check purposes */
    unsigned int  max_slot;
    unsigned int  max_channel;

};

#endif
