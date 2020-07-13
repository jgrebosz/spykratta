/***************************************************************************
                          Tlookup_table_kratta.h  -  description
                             -------------------
    begin                : 4 oct 20014
    copyright            : (C) 2014 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#ifndef TLOOKUP_TABLE_KRATTA_H
#define TLOOKUP_TABLE_KRATTA_H


#include <vector>
#include <string>
#include <map>
//#include <pair>

using namespace std;

/**Lookup table for mapping cables into kratta modules
 *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
 */
struct kwartet
{
    //#  FADC CHAN ----> MODULE    SIGNAL(Photodiode#)   is_it_plastic?    LABEL
public:
    short int detector_id;  // i.e. kratta_01
    short int subdetector;   // i.e. pd1
    bool   flag_plastic;
    string label ;

    kwartet(short m = -1, short ch = -1, bool f = false, string lab = "" ) :
        detector_id {m}, subdetector {ch}, flag_plastic(f), label {lab}
    {} ;
};

/////////////////////////////////////////////////////////////////////////////
class Tlookup_table_kratta
{
    map<int, kwartet>  mapka;  // vector of maps

    const int koding_factor = 16;
public:
    Tlookup_table_kratta();
    /** Reading the settings, which experimenter stored on the disk */
    void read_from_disk(string filename);
    ~Tlookup_table_kratta();
    /** prepares the results, if not found - returns bool = false
    		If found = true, the you can call inline functions for
    		getting the results */
    kwartet current_combination(int fadc, int channel);

protected: // Protected attributes
    int current_active_map_in_the_atlas;
    void focus_on_map_nr(int nr) { current_active_map_in_the_atlas = nr;}


    int koding_key(int f, int c) {
        return (f << koding_factor) + c;
    }

    // chyba nie uzywane nigdy
    pair<int, int>  dekoding_key(int k)
    {
        pair<int, int>  p;
        p.first = k >> koding_factor ;
        p.second = 0xff & k;   // ff is 16 bits (bec. ..)
        return p;
    }
};

#endif
