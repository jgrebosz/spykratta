/***************************************************************************
                          Tlookup_table.h  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
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

#ifndef TLOOKUP_TABLE_H
#define TLOOKUP_TABLE_H


#include <vector>
using namespace std;

/**Lookup table for mapping cables into clusters numbers
 *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
 */
class triplet_ger
{
public:
    short int cluster, crystal, signal;
    triplet_ger(short c = -1, short d = -1, short s = -1) : cluster(c), crystal(d), signal(s) {} ;
};

/////////////////////////////////////////////////////////////////////////////
class Tlookup_table
{
    vector< vector< triplet_ger > > table;
    pl
public:
    Tlookup_table();
    /** Reading the settings, which experimetnator stored on the disk */
    void read_from_disk();
    ~Tlookup_table();
    /** prepares the resutlts, if not found - retruns bool = false
          If found = true, the you can call inline functions for
          getting the results */
    bool current_combination(unsigned int group,
                             unsigned int item,
                             int *clus,
                             int *crys,
                             int *sign);


protected: // Protected attributes
    /** max_group nr - for reservation and check purposes */
    unsigned int  max_group;
    /** For reservation and check purposes */
    unsigned int  max_item_nr;
};

#endif
