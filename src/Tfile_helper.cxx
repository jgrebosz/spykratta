/***************************************************************************
                          tfile_helper.cpp  -  description
                             -------------------
    begin                : Sat May 17 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
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

#include "Tfile_helper.h"
#include <algorithm>
#include <dirent.h>
#include <iostream>


#include "TIFJAnalysis.h"
class TIFJAnalysis;
extern TIFJAnalysis *RisingAnalysis_ptr;

//****************************************************************************
/*../sample_lmd/80MeV_Pb200um_run0624.lmd
../sample_lmd/70MeV_Mylar120um_run0577.lmd*/
//****************************************************************************
/** No descriptions */
void File_helper::spot_in_file(typ_strumienia& s, string slowo) throw(Tno_keyword_exception)
{
    if(!s.good())
    {
        string mess =
            "I can't search the keyword \""
            + slowo
            + "\" in the file, because currently the stream is broken. Perhaps the previous use of this stream blocked it..." ;

        Tno_keyword_exception capsule ;
        capsule.message = mess ;
        throw capsule ;
    }
    s.seekg(0) ;    // rewind
    string arch_s = slowo ; // to have oryginal case for error message

    // to be independent of lower/upper case of keywords...
    //transform(slowo.begin(), slowo.end(), slowo.begin(),  tolower);
    for(unsigned i = 0 ; i < slowo.size() ; i++) slowo[i] = tolower(slowo[i]);

    //     cout << "Keyword to find is >>" << slowo <<"<<" << endl ;
    string word ;
    //-----------------
    while(s)
    {
        s >> zjedz >> word ;
        //transform(word.begin(), word.end(), word.begin(), tolower);
        for(unsigned i = 0 ; i < word.size() ; i++) word[i] = tolower(word[i]);

//     cout << "searching for slowo: >>" << slowo
//     << "<<, " << endl;
//     cout << " => while found word >>" << word << "<<"
//     << endl ;

        if(word == slowo) break ;    // found
    }
    //----------------
    if(!s)
    {
        string mess =
            "I can't find the keyword \""
            + arch_s
            + "\" in the file.(The search was case UNsensitive)" ;

        Tno_keyword_exception capsule ;
        capsule.message = mess ;
        throw capsule ;
    }
}
//****************************************************************************
/** This function searches (in the given file) a keyword and then reads the
    value which follows it. Then returns this value as the result.  */
double File_helper::find_in_file(typ_strumienia& s, string slowo) throw(
    Tno_keyword_exception, Treading_value_exception)
{

    s.seekg(0) ;    // rewind
    string arch_s = slowo ; // to have oryginal case for error message

    if(s.eof())
        cout << "Something wrong with the file caused EOF state. Put a new line on the end of it "
             << endl;

    // to be independent of lower/upper case of keywords...
    //transform(slowo.begin(), slowo.end(), slowo.begin(),  tolower);
    for(unsigned i = 0 ; i < slowo.size() ; i++) slowo[i] = tolower(slowo[i]);

//     cout << "Keyword to find is " << slowo << endl ;
    string word ;
    //-----------------
    while(s)
    {
        s >> zjedz >> word ;
        //transform(word.begin(), word.end(), word.begin(), tolower);
        for(unsigned i = 0 ; i < word.size() ; i++) word[i] = tolower(word[i]);

//         cout << "found word " << word << endl ;
        if(word == slowo) break ;
    }
    //----------------

    if(!s)
    {

        Tno_keyword_exception capsule ;
        capsule.message =  "I can't find keyword \""
                           + arch_s
                           + "\" in the file.(The search was case UNsensitive)" ;
        throw capsule ;
    }
// cout << "found word " << word << endl ;
    // otherwise we read the data
    double value ;
    s >> value ;

    if(!s)
    {


        Treading_value_exception capsule;

        capsule.message = "I found the keyword \""
                          + slowo
                          + "\", but an error occured during in reading its value." ;

        throw capsule ;

    }
    return value ;

}
//*************************************************************************
/** find in the given directory the files which names contain a filter substring */
vector<string> File_helper::find_files_in_directory(string dir, string filter)
{

    // here should be a function for retrieving the names.

    struct dirent **namelist;
    // You can deallocate the memory allocated for the array
    // by calling free(). Free each pointer in the array, and
    // then free the array itself.

    //cout << "direcory to scan is : " << dir_name << endl ;

    vector<string> nazwy ;

    int numer_of_entries = scandir(dir.c_str(), &namelist, 0, alphasort);

    if(numer_of_entries < 0)
    {
        perror("scandir");
        //int result =
        system(string("mkdir " + dir).c_str());        // <--- meant for people who do not have it yet (old versions of spy)
    }
    else
    {
        for(int nr = 0 ; nr < numer_of_entries ; nr++)
        {
            string filename = namelist[nr]->d_name ;
            if(filename.rfind(filter) != string::npos)
            {
                // cout << "Def file :" << filename << endl;
                nazwy.push_back(filename) ;
            }

        } // enf for nr_of_entry
    } // end else       (found list of files)



    // freeing the resources allocated by scandir
    for(int i = 0 ; i < numer_of_entries ; i++)
    {
        free(namelist[i]) ;
    }
    free(namelist);

    return nazwy ;


}
//************************************************************
// GLOBAL FUNCTIONS !!!
//**************************************************************************
/** Reading one 1D gate, result is stored into values poited by  arguments.
If gate does not exist - it is created wide open */
bool read_default_gate(string gate_name,
                       double *low,
                       double *high)
{
    // read the gate (the name is default)
    ifstream plik(gate_name.c_str());

    if(!plik)
    {
        cout << "Can't open file " << gate_name << endl ;
        // gate does not exist
    }

    plik >> zjedz >> *low
         >> zjedz >> *high ;

    if(plik)
    {
        *low = min(*low, *high) ;
        *high = max(*low, *high) ;
        return true ;
    }
    else
    {
        cout << "Can't read from the file " << gate_name << endl ;
    }
    return false ;
}
//***************************************************************************
/** Read the banana gate if possible, then register it in Root */
bool read_banana(string gate_name,  TjurekPolyCond   *polygon[])
{
    // read the gate (the name is default)
    string pathed_name = string("./polygons/") + gate_name ;
    ifstream plik(pathed_name.c_str());

    if(!plik)
    {
        cout << "Polygon file " << pathed_name  << " does not exist " << endl ;
        return false ;
    }

    //cout << "defining gate " <<  screen_name << endl ;
    vector<double> iksy ;
    vector<double> igreki ;

    //cout << "Reading the file " << nam << endl ;
    double x, y ;
    do
    {

        plik >> zjedz >> x ;
        plik >> zjedz >> y ;
        // cout << " Polygon point {" << x << ", " << y << "}" << endl ;

        if(!plik) break ;
        iksy.push_back(x);
        igreki.push_back(y);
    }
    while(plik);

    if(iksy.size() > 1)
    {
        iksy.push_back(iksy[0]);      // closing the polygon, because there is a bug in the ROOT
        igreki.push_back(igreki[0]);
    }

    // what do we have to do next time ? (after the second start ?)
    // at first remove it form the analysis

    // do{

    TjurekPolyCond *pointer =  RisingAnalysis_ptr -> give_ptr_to_polygon(gate_name) ;

    if(pointer != 0)
    {
        // such a root gate already exist in root memory,
        // so we have to delete it ???? ->> why ? some other user spectrum  could make it !

        // NOTE: now we can not remove, because several other conditions
        //  may use the same polygon gate - and they already remember
        // the address of this polygon

        // cout << "Yes, removing it ." << endl ;
        //         TGo4Analysis::Instance()->
        //                 RemoveAnalysisCondition  (gate_name.c_str()) ;
        // cout << "Polygon already existed, so deleting it "    << endl ;



        //no, we just have to change the name
        //gate_name += "A" ;
        *polygon = pointer ;
        pointer->SetValues(&iksy[0], &igreki[0], iksy.size()) ;
        pointer->Enable() ;
    }
    else
    {
        *polygon = new TjurekPolyCond(gate_name);
        //        (*polygon)->SetValues(iksy.begin(), igreki.begin(), iksy.size() ) ;
        (*polygon)->SetValues(&iksy[0], &igreki[0], iksy.size()) ;
        (*polygon)->Enable() ;
        RisingAnalysis_ptr->AddAnalysisCondition(*polygon);

        //break ; // not esisting
    }

    // }while(1) ;

    //*polygon = new TjurekPolyCond( (char *)( gate_name.c_str() ) );
    // (*polygon)->SetValues(iksy.begin(), igreki.begin(), iksy.size() ) ;
    //(*polygon)->Enable() ;
    //TGo4Analysis::Instance()->AddAnalysisCondition( *polygon);

    return true ;
}
//*****************************************************************************
namespace File_helper
{
void repair_the_stream(istream& s)
{
// repair the stream
    s.clear(s.rdstate() & ~(ios::eofbit | ios::failbit));
}

}

//****************************************************************************
