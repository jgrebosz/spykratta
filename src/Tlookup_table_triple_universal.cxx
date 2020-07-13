/***************************************************************************
                          Tlookup_table_triple_universal.cpp  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/


#include "Tlookup_table_triple_universal.h"
#include "Tfile_helper.h"
#include "experiment_def.h"
#include "Trising.h"


//**************************************************************************
Tlookup_table_triple_universal::  Tlookup_table_triple_universal(int max_g, int max_i)
{
    max_group = max_g;
    max_item_nr = max_i ;

    vector<triplet_det > vvv(max_item_nr + 1) ;

    for(unsigned int i = 0 ; i <= max_group ; i++)  // nieostra nierownosc
    {
        table.push_back(vvv) ;
    }
}
//**************************************************************************
Tlookup_table_triple_universal::~Tlookup_table_triple_universal()
{}
//**************************************************************************
/** Reading the settings, which experimentator stored on the disk */
void Tlookup_table_triple_universal::read_from_disk(string name,
        vector<string> chain_one,
        vector<string> chain_two,
        vector<string> chain_three, bool continuation_of_reading,
        int offset_clus,
        int offset_crys,
        int offset_sign)
{
    if(!continuation_of_reading)
    {
        for(unsigned int g = 0 ; g <= max_group  ; g++)       // nieostra
            for(unsigned int i = 0 ; i <= max_item_nr ; i++)   // nieostra
            {
                table[g][i].cluster = EMPTY_LOOKUP_POSITION ;
                table[g][i].crystal = EMPTY_LOOKUP_POSITION;
                table[g][i].signal = EMPTY_LOOKUP_POSITION;
            }
    }
    //string name = "./mbs_settings/lookup_table.txt" ;
    ifstream plik(name.c_str());
    if(!plik)
    {
        cout << "!!! Fatal error: I can't open file " << name << endl;
        exit(1);
    }

    //string marks = Trising::cluster_characters ; // defined in: experiment_defsettings.h


    // at first we try to read old style keywords.
    // If such is not found, we try the new style

    // loop over all germanium clusters
    for(unsigned int clus = 0 ; clus < chain_one.size() ; clus++)
    {
        for(unsigned int segm = 0; segm < chain_two.size() ; segm++)
        {
            for(unsigned int signal = 0 ; signal < chain_three.size() ; signal++)
            {
                string det_name = chain_one[clus] ;
                det_name += chain_two[segm];
                det_name += chain_three[signal];

                if(continuation_of_reading)
                {
                    // if one of the keywords is dummy, do not read here (this is usefull for "continuation")
                    string::size_type  pos = det_name.find("skip_this_keyword");
                    if(pos != string::npos)
                        continue;
                    //                     cout << "Nie ma substr = skip_this_keyword " << endl;

                }

                // find the keyword of particular cristal
                // read the group
                // read the item

                // at first we try to read old style keywords.
                // If such is not found,  we try the new style
                //                 cout << "Trying to read the keyword >>" << det_name << "<<" << endl;

                try
                {
                    unsigned int group =
                        (unsigned) Tfile_helper::find_in_file(plik, det_name) ;

                    unsigned int item;
                    plik >> item ;

                    if(!plik)
                    {
                        Tfile_helper_exception k ;
                        k.message =
                            "Error while reading the 'item value for  " + det_name ;
                        throw k ;
                    }

                    // store in the lookup table as a pair
                    // first cluster nr
                    // second crystal nr
                    if(group <= max_group && item <= max_item_nr)
                    {

                        table[group][item] = triplet_det(clus + offset_clus,
                                                         segm + offset_crys,
                                                         signal + offset_sign
                                                        );   // why this -1 must be here...?
                    }
                } // end try find file new
                catch(Tno_keyword_exception)
                {
                    cout << "The keyword: " << det_name
                         << " was not found " << endl;
                    exit(1);
                    // throw;
                } // end catch

            } // end for signal
        }// end for segm
    } // end for clust

    //  for(unsigned int g = 0 ; g < max_group ; g++)
    //    for(unsigned  int i = 0 ; i < max_item_nr ; i++)
    //    {
    //      if(table[g][i].cluster == EMPTY_LOOKUP_POSITION) continue ;
    //      cout
    //        << "Group " << g
    //        << ", item " << i
    //
    //        << " gives signals for ---> cluster nr "
    //        << table[g][i].cluster
    //        << " -> " << marks[table[g][i].cluster]
    //        << "  crystal : " << (table[g][i].crystal)
    //        << ",  signal : " <<  table[g][i].signal
    //        << endl;
    //
    //    }

    cout << " Lookup table successfully read " << endl;

}
//*************************************************************************
/** prepares the resutlts, if not found - returns bool = false
    If found = true, the you can call inline functions for
    getting the results */
bool Tlookup_table_triple_universal::current_combination(unsigned int group,
        unsigned int item,
        int *clus,
        int *crys,
        int *sign)
{
    if(group > max_group  ||  item > max_item_nr
            ||
            table[group][item].cluster  == EMPTY_LOOKUP_POSITION
            ||
            table[group][item].crystal == EMPTY_LOOKUP_POSITION)
        return false;   // nonsense values

    if(table[group][item].signal == -1)  // old style or new style
    {
        *crys = table[group][item].crystal / 3 ;  // old style
    }
    else
        *crys = table[group][item].crystal;

    *clus = table[group][item].cluster;
    *sign = table[group][item].signal;

    return true ;
}
