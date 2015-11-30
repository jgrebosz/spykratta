/***************************************************************************
                          Tlookup_table.cpp  -  description
                             -------------------
    begin                : Thu Jul 31 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/


#include "Tlookup_table.h"
#include "Tfile_helper.h"
#include "experiment_def.h"
#include "Trising.h"

//#define COTO  cout << "File: " << __FILE__ << ", line: " << __LINE__ << endl;

//**************************************************************************
Tlookup_table::Tlookup_table()
{
    max_group = 70;
    max_item_nr = 100 ;// Piotr says that it can be till 255;

    //vector<> vvv( pair<int,int>(0,0) ) ;
    vector<triplet_ger > vvv(max_item_nr + 1) ;

    for(unsigned int i = 0 ; i <= max_group ; i++)  // nieostra nierownosc
    {
        table.push_back(vvv) ;
    }
}
//**************************************************************************
Tlookup_table::~Tlookup_table()
{}
//**************************************************************************
/** Reading the settings, which experimentator stored on the disk */
void Tlookup_table::read_from_disk(string marks)
{
//   cout << "size of table g = " << table.size() << ", while max_group = " << max_group
//   << ", size of item = " << table[0].size() << ", while max_item = " <<  max_item_nr
//   << endl;

    for(unsigned int g = 0 ; g <= max_group  ; g++)       // nieostra???
        for(unsigned int i = 0 ; i <= max_item_nr ; i++)   // nieostra???
        {
            table[g][i].cluster = EMPTY_LOOKUP_POSITION ;
            table[g][i].crystal = EMPTY_LOOKUP_POSITION;
            table[g][i].signal = EMPTY_LOOKUP_POSITION;

//       if((g > (max_group - 3)) && (i > (max_item_nr - 3)))
//         cout << "writing into table[" << g << "][" << i << "]" << endl;
        }
//   cout << "Done " << __LINE__ << endl;
    string name = "./mbs_settings/lookup_table.txt" ;
    ifstream plik(name.c_str());
    if(!plik)
    {
        cout << "!!! Fatal error: I can't open file " << name << endl;
        exit(1);
    }

    //   string marks = Trising::cluster_characters ; // defined in: experiment_defsettings.h

    try
    {

        // at first we try to read old style keywords.
        // If such is not found, we try the new style

        // loop over all germanium clusters
        for(unsigned int clus = 0 ; clus < marks.size() ; clus++)
        {
            for(int segm = 0 ; segm < 7 ; segm++)
            {

                // Old style keyword
                // Cluster_A_1                   1     A
//         COTO
                string det_name = string("cluster_") + marks[clus] + "_" ;
                char znak = (char('1' + segm)) ;
                det_name  += znak ;

                // find the keyword of particular cristal
                // read the group
                // read the item

                // at first we try to read old style keywords.
                // If such is not found,  we try the new style
                try
                {
                    unsigned int group =
                        (unsigned) Tfile_helper::find_in_file(plik, det_name) ;

                    plik >> zjedz ;
                    char item = plik.peek();

                    if(!plik)
                    {
                        Tfile_helper_exception k ;
                        k.message =
                            "Error while reading the 'item symbol' (char)  for " + det_name ;
                        throw k ;
                    }
                    unsigned int item_nr = 0 ;
                    if(isdigit(item))   // Piotr says that if it is an digit - use it as the number
                    {
                        plik >> item_nr;
                        if(!plik)
                        {
                            Tfile_helper_exception k ;
                            k.message =
                                "Error while reading the 'item symbol' (char)  for " + det_name ;
                            throw k ;
                        }
                    }
                    else    // if it is a character  a,b,c,d,e,f, - recalculate it
                    {
                        plik >> item ;
                        if(!plik)
                        {
                            Tfile_helper_exception k ;
                            k.message =
                                "Error while reading the 'item symbol' (char)  for " + det_name ;
                            throw k ;
                        }
//             COTO
                        item_nr = (tolower(item) - 'a');
                        // *3 is because later we divide by 3 (in case of the old style)
                        // the new style is already corected - not divided by 3
                        item_nr *= 3;
                    }

                    // store in the lookup table
//           COTO
                    if(group <= max_group && item_nr <= max_item_nr)
                    {
//             COTO
                        table[group][item_nr] = triplet_ger(clus, segm, 0); // -1 czyli po staremu
                        if(item_nr + 1 <= max_item_nr)
                            table[group][item_nr + 1] = triplet_ger(clus, segm, 1); // -1 czyli po staremu
                        if(item_nr + 2 <= max_item_nr)
                            table[group][item_nr + 2] = triplet_ger(clus, segm, 2); // -1 czyli po staremu
//             COTO
                    }

                } // end try old style
                catch(Tno_keyword_exception &)
                {
                    // we have to repair the state of the string
                    //                                      cout << "State of the stream plik = " << hex << plik.rdstate() << dec << endl;
                    plik.clear() ;
                    //                                      cout << "After clearing State of the stream plik = " << hex << plik.rdstate() << dec << endl;

                    // then we try the new style
                    // constructing the  new style keyword

                    //Cluster_A_1_en4MeV                     1     A
                    //Cluster_A_1_en20MeV                   1    41
                    //Cluster_A_1_time                        1     A

                    string sig_suffix[3] = { "_en20MeV", "_en4MeV", "_time"};

                    for(int sig = 0 ; sig < 3 ; sig++)
                    {
                        string new_name = det_name + sig_suffix[sig] ;

                        try
                        {
                            unsigned int group =
                                (unsigned) Tfile_helper::find_in_file(plik, new_name) ;

                            unsigned int new_item;
                            plik >> new_item ;

                            if(!plik)
                            {
                                Tfile_helper_exception k ;
                                k.message =
                                    "Error while reading the 'item value for  " + new_name ;
                                throw k ;
                            }

                            // store in the lookup table as a pair
                            // first cluster nr
                            // second crystal nr
                            if(group <= max_group && new_item <= max_item_nr)
                            {
                                table[group][new_item] = triplet_ger(clus, segm, sig);   // why this -1 must be here...?
                            }
                        } // end try find file new
                        catch(Tno_keyword_exception)
                        {
                            cout << "The old style keyword ' " << det_name
                                 << " was not found, and also the new one: '" << new_name
                                 << "' as well " << endl;
                            throw;
                        }
                    } // end for
                } // end catch  Tno_keyword_exception &

            }// end for segm
        } // end for clust
        //=========================================================
        // here we can try to read the BGO information - which may (or not) be present


        for(unsigned int clus = 0 ; clus < marks.size() ; clus++)
        {


            string det_name = string("cluster_") + marks[clus] ;
            det_name  += + "_BGO"  ;

            // find the keyword of particular cristal
            // read the group
            // read the item

            try
            {
                //              cout << "Trying to find: " << det_name << endl;
                // TRICK: if the group will be less than 0 - this means that Piotr will give the next pair of numbers
                // by this we can later summ all the BGO crystals

                int group =
                    (int) Tfile_helper::find_in_file(plik, det_name) ;

                bool flag_read_more = true;
                bool will_be_summed = false;

                for(int i = 0 ; flag_read_more ; i++)
                {
                    //                  cout << "Top of the Loop over reading " << endl;
                    if(i > 0) // if this is the second or next pain
                    {
                        // reading the item
                        plik >> zjedz >> group ;
                    }

                    if(group < 0)
                    {
                        group = -group ;
                        will_be_summed = true;
                    }
                    else flag_read_more = false;

                    // reading the item
                    plik >> zjedz ;

                    int item_nr = 0 ;
                    plik >> item_nr;

                    if(!plik)
                    {
                        Tfile_helper_exception k ;
                        k.message =
                            "Error while reading the 'item_nr (int)  for " + det_name ;
                        throw k ;
                    }

                    if(item_nr < 0)
                    {
                        item_nr = -item_nr ;
                        will_be_summed = true;
                    }

                    // store in the lookup table

                    if(group <= (int)max_group && item_nr <= (int)max_item_nr)
                    {
                        //  en4, en20, time, but the BGO signal is something extra
                        // zero as the segm - if fake
                        if(will_be_summed)
                        {
                            // minus in the clus wil give the message to the unpacking routine, that it should
                            // not overwrite the previous value in the TIFJEvent.h but just add to it.
                            table[group][item_nr] = triplet_ger(-clus, 0, BGI_SIGNAL_NR);
                            //                    cout << "Remembered cumulation for BGO group = " << group
                            //                            << ", item = " << item_nr <<  endl;
                        }
                        else  // means the classical way
                        {
                            table[group][item_nr] = triplet_ger(clus, 0, BGI_SIGNAL_NR);
                        }
                    } // endif - max group, max item


                } // end of while - minus group


            } // end try old style
            catch(Tno_keyword_exception &)
            {
                cout << "In the Germanium lookup table, there is no BGO signal defined for "
                     << det_name << endl;

                // we have to repair the state of the string
                plik.clear() ;

            } // end catch  Tno_keyword_exception &

        } // end for clust


    } // end big try all

    catch(Tfile_helper_exception &k)
    {
        cout
                << "Error while reading " << name << "  " << k.message
                << endl;
        exit(1) ;
    }

    //  for(unsigned int g = 0 ; g < max_group ; g++)
    //    for(unsigned  int i = 0 ; i < max_item_nr ; i++)
    //    {
    //                      if(table[g][i].cluster == EMPTY_LOOKUP_POSITION) continue ;
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

    cout << "Clusters Lookup table successfully read " << endl;


}
//*************************************************************************
/** prepares the resutlts, if not found - returns bool = false
                If found = true, the you can call inline functions for
                getting the results */
bool Tlookup_table::current_combination(unsigned int group,
                                        unsigned int item,
                                        int *clus,
                                        int *crys,
                                        int *sign)
{
    if(group > max_group  ||  item > max_item_nr
            ||
            table[group][item].cluster  == EMPTY_LOOKUP_POSITION
            ||
            table[group][item].crystal == EMPTY_LOOKUP_POSITION) return false;   // nonsense values

    if(table[group][item].signal == -1)  // old style or new style
    {
        *crys = table[group][item].crystal / 3 ;  // old style
    }
    else *crys = table[group][item].crystal;

    *clus = table[group][item].cluster;
    *sign = table[group][item].signal;

    return true ;
}
