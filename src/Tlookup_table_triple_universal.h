/***************************************************************************
                          Tlookup_table_triple_universal.h  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#ifndef TLOOKUP_TABLE_TRIPLE_UNIVERSAL_H
#define TLOOKUP_TABLE_TRIPLE_UNIVERSAL_H


#include <vector>
using namespace std;

#define EMPTY_LOOKUP_POSITION  -9992
#include <string>
using namespace std;


/**Lookup table for mapping cables into clusters numbers
 *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
 */
class triplet_det
{
public:
    short int cluster, crystal, signal;
    triplet_det(short c = -EMPTY_LOOKUP_POSITION,
                short d = -EMPTY_LOOKUP_POSITION,
                short s = -EMPTY_LOOKUP_POSITION) : cluster(c), crystal(d), signal(s) {} ;
};

/////////////////////////////////////////////////////////////////////////////
class Tlookup_table_triple_universal
{
    vector< vector< triplet_det > > table;

public:
    Tlookup_table_triple_universal(int max_g, int max_i);
    /** Reading the settings, which experimetnator stored on the disk */
    void read_from_disk(string file,
                        vector<string> chain_one_array,
                        vector<string> chain_two_array,
                        vector<string> chain_three_array,
                        bool continuation_of_reading,
                        int offset_clus = 0,
                        int offset_crys = 0,
                        int offset_sign = 0);
    // NOTE: here above are default arguments to make possible
    // reading values for custrer/crystats which do not necessary start
    // from 0.

    ~Tlookup_table_triple_universal();
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


//   vector<string> chain_one;
//   vector<string> chain_two;
//   vector<string> chain_three;
//
};

#endif
