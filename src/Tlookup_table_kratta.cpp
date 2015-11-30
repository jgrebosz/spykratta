/***************************************************************************
                          Tlookup_table_kratta.cpp  -  description
                             -------------------
    begin                :Nov 9 2014
    copyright            : (C) 2014 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl

    It works with the kratta lookuptable file
    and stores the info in the map. (as the quartet data)
    The key to the map is coded from the two data fadc and channel
    so it is a one int value
    By this we avoid empty entries in the multidimmensional array.

    So far there is no Tfile_helper used - because in the conf file
    of kratta there are no keywords
***************************************************************************/

#include "Tlookup_table_kratta.h"
//#include "Tfile_helper.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include <cstdlib>

//**************************************************************************
Tlookup_table_kratta::Tlookup_table_kratta()
{

}
//**************************************************************************
Tlookup_table_kratta::~Tlookup_table_kratta()
{
}
//**************************************************************************
/** Reading the settings, which experimentator stored on the disk */
void Tlookup_table_kratta::read_from_disk(string name)
{

    //string name = "./geo_map.geo" ;
    ifstream plik(name.c_str());
    if(!plik)
    {
        cout << "!!! Fatal error: I can't open file " << name
             << "\nMost probably you do not have such a file in this location"
             << endl;
        exit(1);
    }

    // first line is a comment
    string linijka;
    for(int nr = 0  ; getline(plik, linijka) ; ++nr)
    {
        if(linijka[0] == '#' ) {
            --nr;
            continue;
        }

        //cout << "Przeczytane " << linijka << endl;
        istringstream s(linijka.c_str());
        int fadc, chan, modu, sign;
        bool flag = true;
        string label;

#if 1   // new spi 
        s >> label>>  fadc >> chan  ;
        sign = nr %3;
        modu = nr / 3;
#else	
        s >> fadc >> chan >> modu >> sign >> flag >> label ;
#endif

        if(!s)
        {
            cerr << "  Tlookup_table_kratta::read_from_disk --> error while reading file " << name << endl;
            return;
        }
        //kwartet tr(modu, sign, flag, label);
        // kodowanie
        int key = koding_key(fadc, chan) ;
        mapka[key] = kwartet(modu, sign, flag, label);
        cout << "Ladowanie lookup table - fadc = " << fadc << " chan= " << chan << " daja klucz " << key << endl;
    }

    //  cout << "Lookup table successfully read " << endl;

}
//*************************************************************************
/** prepares the resutlts, if not found - returns bool = false
        If found = true, the you can call inline functions for
        getting the results */
kwartet Tlookup_table_kratta::current_combination(int fadc, int channel )
{
    // cout << " Tlookup_table_kratta::current_combination. --> szukanie kombinacji dla fadc = " << fadc << ", chan = " << channel << endl;
    int klucz = koding_key(fadc, channel);
    kwartet t = mapka[klucz];   // a co jesli klucz nieleganlny? (daje pusty triplet a tam wartosci -1)
    return t;
}
