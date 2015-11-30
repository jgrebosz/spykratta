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
    //#FADC CHAN -> MODULE    SIGNAL(Photodiode#)   OK_FLAG    LABEL
public:
    short int module;
    short int signal;
    bool   ok_flag;
    string label ;

    kwartet(short m = -1, short s = -1, bool f = false, string lab = "" ) : module {m}, signal {s}, ok_flag(f), label {lab}
    {} ;
};

/////////////////////////////////////////////////////////////////////////////
class Tlookup_table_kratta
{
    //vector<vector< vector< triplet > > >table;

    map<int, kwartet>   mapka;
    const int koding_factor = 16;
public:
    Tlookup_table_kratta();
    /** Reading the settings, which experimetnator stored on the disk */
    void read_from_disk(string filename);
    ~Tlookup_table_kratta();
    /** prepares the resutlts, if not found - retruns bool = false
    		If found = true, the you can call inline functions for
    		getting the results */
    kwartet current_combination(int fadc, int channel);


protected: // Protected attributes

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
