/***************************************************************************
                          Tlookup_table_doublets.h  -  description
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

#ifndef TLOOKUP_TABLE_DOUBLETS_H
#define TLOOKUP_TABLE_DOUBLETS_H


#include <vector>
#include <string>
using namespace std;

/**Lookup table for mapping cables into clusters numbers
 *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
 */

// Warning: 'groups' and 'items' have a historical meaning because
// originally this lookuptable was used for VXI germanium signals
// now the naming does not matter

/////////////////////////////////////////////////////////////////////////////
class Tlookup_table_doublets
{
    vector< vector< pair<int, int> > > table;
    string prefix_for_keyword ;

public:
    Tlookup_table_doublets(int max_first, int max_second, string prefix);
    /** Reading the settings, which experimetnator stored on the disk */
    void read_from_disk(string name, string marks = "ABCDEFG");
    ~Tlookup_table_doublets();
    /** prepares the resutlts, if not found - retruns bool = false
                          If found = true, the you can call inline functions for
                          getting the results */
    bool current_combination(unsigned int group,
                             unsigned int item,
                             int *clus,
                             int *crys
                            );


protected: // Protected attributes
    /** max_group nr - for reservation and check purposes */
    unsigned int  max_group;
    /** For reservation and check purposes */
    unsigned int  max_item_nr;
};

#endif
