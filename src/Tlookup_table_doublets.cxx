/***************************************************************************
                          Tlookup_table_doublets.cpp  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/


#include "Tlookup_table_doublets.h"
#include "Tfile_helper.h"


#include "Trising.h"
//**************************************************************************
Tlookup_table_doublets::Tlookup_table_doublets(int max_first, int max_second, string pref)
{
    max_group = max_first;
    max_item_nr = max_second;
    prefix_for_keyword = pref;

    //vector<> vvv( pair<int,int>(0,0) ) ;
    vector<pair<int, int> > vvv(max_item_nr + 1) ;

    for(unsigned int i = 0 ; i <= max_group ; i++)  // nieostra nierownosc
    {
        table.push_back(vvv) ;
    }
}
//**************************************************************************
Tlookup_table_doublets::~Tlookup_table_doublets()
{
}
//**************************************************************************
/** Reading the settings, which experimentator stored on the disk */
void Tlookup_table_doublets::read_from_disk(string name, string marks)
{

    for(unsigned int g = 0 ; g <= max_group  ; g++)       // nieostra
        for(unsigned int i = 0 ; i <= max_item_nr ; i++)   // nieostra
        {
            table[g][i].first = -1 ;
            table[g][i].second = -1 ;
        }

//  string name = "./mbs_settings/lookup_table.txt" ;
    ifstream plik(name.c_str());
    if(!plik)
    {
        cout << "!!! Fatal error: I can't open file " << name << endl;
        exit(1);
    }

//   string marks = Trising::cluster_characters;

    try
    {

        // loop over all germanium clusters
        for(unsigned int clus = 0 ; clus < marks.size() ; clus++)
        {
            for(int segm = 0 ; segm < 7 ; segm++)
            {
                string det_name = prefix_for_keyword + "_" + marks[clus] + "_" ;
                char znak = (char('1' + segm)) ;
                det_name  += znak ;


                // find the keyword of particular cristal
                // read the group
                // read the item

                unsigned int group =
                    (unsigned) Tfile_helper::find_in_file(plik, det_name) ;
                unsigned int item_nr;
                plik >> zjedz >> item_nr;

//        cout << "For geo = " << group << ", item " << item_nr
//        << " assigned combination  clus " <<clus << ", segm " << segm << endl;

                // store in the lookup table as a pair
                // first cluster nr
                // second crystal nr
                if(group <= max_group && item_nr <= max_item_nr)
                {
                    table[group][item_nr] = pair<int, int>(clus, segm);
                }
            }
        }
    } // end try
    catch(Tfile_helper_exception &k)
    {
        cout
                << "Error while reading " << name << "  " << k.message
                << endl;
        exit(1) ;
    }

//  for(int g = 0 ; g < max_group ; g++)
//    for(int i = 0 ; i < max_item_nr ; i++)
//    {
//      cout
//        << "Group " << g
//        << ", item " << i
//        << " gives signals for cluster nr " ;
//        if(table[g][i].first != -1)
//        {
//        cout << table[g][i].first
//        << " -> " << marks[table[g][i].first]
//        << " crystal "
//        << (table[g][i].second + 1)
//        << endl;
//        }else{
//          cout << "  <---- Not defined combination" << endl;
//          }
//    }

//  cout << "Lookup table successfully read " << endl;


}
//*************************************************************************
/** prepares the resutlts, if not found - returns bool = false
                If found = true, the you can call inline functions for
                getting the results */
bool Tlookup_table_doublets::current_combination(unsigned int group,
        unsigned int item,
        int *clus,
        int *crys)
{
    if(group > max_group  ||  item > max_item_nr) return false ;
    *clus = table[group][item].first;
    *crys = table[group][item].second;
    if(*clus < 0  ||  *crys < 0)
    {
        return false ;
    }
    return true ;
}
