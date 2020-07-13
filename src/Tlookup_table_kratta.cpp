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
#include "Tfile_helper.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>


#include <cstdlib>
#include <experiment_def.h>

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
    cout << __func__ << endl;
try{
    //string name = "./geo_map.geo" ;
    ifstream plik(name.c_str());
    if(!plik)
    {
        cout << "!!! Fatal error: I can't open file " << name
             << "\nMost probably you do not have such a file in this location"
             << endl;
        exit(1);
    }


#define LOOKUP_JUREK false

#if LOOKUP_JUREK == true
    // first line is a comment

    for(int m = 0 ; m < KRATTA_NR_OF_CRYSTALS ; ++m)
        for(int p = 0 ; p < 3  ; ++p)
        {
            cout << __LINE__ << endl;
            ostringstream n;
            n.fill('0');
            n << "kratta_" << setw(2) << m << "_pd" << p;

            string keyword = n.str();

            FH::spot_in_file(plik, keyword);
            //string label;
            int fadc, chan;
            plik
                    //>> label
                    >> zjedz >> fadc >> zjedz >> chan  ;
            if(!plik) break;
            // kodowanie
            int key = koding_key(fadc, chan) ;
            mapka[key] = kwartet(m, p, false, "puste");
            //cout << "Ladowanie kratta lookup table - fadc = " << fadc << " chan= " << chan << " daja klucz " << key << endl;           
        }


    for(int m = 0 ; m < KRATTA_NR_OF_PLASTICS ; ++m)
        for(int p = 0 ; p < 2  ; ++p) // just two signals
        {
            ostringstream n;
            n.fill('0');
            n << "plastic_module_" << setw(2) << m << "_pd" << p;

            string keyword = n.str();

            try{
            FH::spot_in_file(plik, keyword);

            int fadc, chan;
            plik
                    //>> label
                    >> zjedz >> fadc >> zjedz >> chan  ;
            if(!plik) break;
            // kodowanie
            int key = koding_key(fadc, chan) ;
            mapka[key] = kwartet(m, p, true, "puste"); // true means - plastic
//            cout << "Ladowanie kratta lookup table - Plastic,  fadc = " << fadc
//                 << " chan= " << chan << " daja klucz " << key << endl;
            }
            catch(Tno_keyword_exception & e)
            {
              //cout << e.message << endl;
              FH::repair_the_stream(plik);
            }
        }

#endif




    }catch(Tno_keyword_exception & e)
    {
     cout << e.message << " in lookup table file " << name << endl;
     exit(1);
    }
    catch(Treading_value_exception &e)
    {
        cout << e.message << " in lookup table file " << name << endl;
        exit(1);
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
