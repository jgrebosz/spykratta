/***************************************************************************
                          Tlookup_table_miniball.cpp  -  description
                             -------------------
    begin                :Nov 9 2004
    copyright            : (C) 2004 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "Tlookup_table_miniball.h"
#include "Tfile_helper.h"

#include <iostream>
using namespace std;

//**************************************************************************
Tlookup_table_miniball::Tlookup_table_miniball()
{
    max_crate = 3;
    max_slot = 22;
    max_channel = 3;


    triplet x ;
    vector<triplet > initial_row(max_channel + 1, x) ;
    vector<vector<triplet > > plaster(max_slot + 1, initial_row) ;

//  for(unsigned int i=0 ; i <= max_slot ; i++)    // nieostra nierownosc
//  {
//      plaster.push_back(line) ;
//  }

    for(unsigned int i = 0 ; i <= max_crate ; i++)  // nieostra nierownosc
    {
        table.push_back(plaster) ;
    }

}
//**************************************************************************
Tlookup_table_miniball::~Tlookup_table_miniball()
{
}
//**************************************************************************
/** Reading the settings, which experimentator stored on the disk */
void Tlookup_table_miniball::read_from_disk()
{
    for(unsigned int cr = 0 ; cr <= max_crate  ; cr++)       // nieostra
        for(unsigned int s = 0 ; s <= max_slot  ; s++)       // nieostra
            for(unsigned int ch = 0 ; ch <= max_channel ; ch++)   // nieostra
            {
                table[cr][s][ch].cryostat = -1 ;
                table[cr][s][ch].detector = -1 ;
                table[cr][s][ch].signal = -1 ;
            }

    string name = "./mbs_settings/lookup_table_miniball.txt" ;
    ifstream plik(name.c_str());
    if(!plik)
    {
        cout << "!!! Fatal error: I can't open file " << name
             << "\nMost probably you do not have such a file. If you do not use the Miniball branch, please \n"
             "disable all combinations of events with the MIB branch. \n"
             "(To do so, in the cracow viewer go to: Spy_options->Matching MBS event)"
             << endl;
        exit(1);
    }

    string marks = "ABC" ;

    try
    {

        // loop over all 8 minibal cryostats
        const unsigned int how_many_cryostats = 8 ;
        const unsigned int how_many_signals = 7 ;

        //================================================
        for(unsigned int cryos = 0 ; cryos < how_many_cryostats ; cryos++)
            for(unsigned int det = 0 ; det <  marks.size() ; det++)
                for(unsigned int sig = 0 ; sig < how_many_signals ; sig++)
                {
                    string det_name =
                        string("Miniball__Cryostat_") + (char('1' + cryos)) +
                        "__Detector_" + marks[det] +
                        "__Signal_" + (char('0' + sig))  ;
//        char znak = (char('1' + segm)) ;
//        det_name  += znak ;


                    // find the keyword of particular cristal
                    // read the crate
                    // read the slot

                    unsigned int crate =
                        (unsigned) Tfile_helper::find_in_file(plik, det_name) ;
                    unsigned int  slot;
                    plik >> zjedz >> slot ;
                    unsigned int  chan;
                    plik >> zjedz >> chan ;

                    if(!plik)
                    {
                        Tfile_helper_exception k ;
                        k.message =
                            "Error while reading the lookup table minibal data for  " + det_name + "\n";
                        throw k ;
                    }

                    // store in the lookup table as a pair
                    // first cluster nr
                    // second cryostat nr
                    if(crate <= max_crate && slot <= max_slot && slot <= max_slot)
                    {
                        table[crate][slot][chan] = triplet(cryos, det, sig);
                    }

                }// end for


    } // end try
    catch(Tfile_helper_exception &k)
    {
        cout
                << "Error while reading " << name << " \n\t" << k.message
                << endl;
        exit(1) ;
    }


//  for(int g = 0 ; g < 15 ; g++)
//    for(int i = 0 ; i < 7 ; i++)
//    {
//      cout
//        << "Group " << g
//        << ", item " << i
//        << " gives signals for cluster nr "
//        << table[g][i].first
//        << " -> " << marks[table[g][i].first]
//        << " cryostat "
//        << (table[g][i].second + 1)
//        << endl;
//    }
//

//  here we can try to read some Rising Clusters if somebody send it
// throug this channel

// TO DO ..................



//  cout << "Lookup table successfully read " << endl;


}
//*************************************************************************
/** prepares the resutlts, if not found - returns bool = false
        If found = true, the you can call inline functions for
        getting the results */
bool Tlookup_table_miniball::current_combination(unsigned int crate,
        unsigned int slot,
        unsigned int channel,
        int *pcryostat,
        int *pdetector,
        int *psignal)
{
    if(crate > max_crate  ||  slot > max_slot || channel > max_channel) return false ;
    *pcryostat = table[crate][slot][channel].cryostat;
    *pdetector = table[crate][slot][channel].detector;
    *psignal = table[crate][slot][channel].signal;

    if(*pcryostat < 0  ||  *pdetector < 0 || *psignal < 0)
    {
        return false ;
    }
    return true ;
}
