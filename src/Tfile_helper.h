/***************************************************************************
                          tfile_helper.h  -  description
                             -------------------
    begin                : Sat May 17 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl


    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! NOTE !!!!!!!!!!!!!!!!!!!!!!!!!!!
    There is a similar file in the cracow program, however it does not
    have a function:
   -  read_banana
    bool read_default_gate(string gate_name, double *low, double *high);
 ***************************************************************************/



#ifndef TFILE_HELPER_H
#define TFILE_HELPER_H

#include <fstream>
#include <string>
#include <vector>
using namespace std ;
// FOR GLOBAL FUNCITONS ( read gate, read banana)

//#include "TIFJAnalysis.h"  // for registering conditions
#include "TjurekPolyCond.h"


/**this class helps to search inside the configuration files, or callibration files
for some keywords. Such keywords are usually followed by the values of
the associated parameter
  *@author Jurek Grebosz (IFJ Krakow, Poland)
  */
// two classes to carry the exception
class Tfile_helper_exception
{
public:
    string message ;

};
class Tno_keyword_exception : public Tfile_helper_exception { } ;
class Treading_value_exception : public Tfile_helper_exception { } ;

#define typ_strumienia istream
//**********************************************************************
namespace File_helper
{

/** This function searches (in the given file) a keyword and then reads the
value which follows it. Then returns this value as the result.  */
double find_in_file(typ_strumienia& s, string slowo);

/** find a keyword, - just for positioning cursor  */
void spot_in_file(typ_strumienia& s, string slowo);
/** find in the given directory the files which names contain a filter substring */
vector<string> find_files_in_directory(string dir, string filter);

void repair_the_stream(typ_strumienia& s);

}
/////////////////////////////////////////////////////////////////////////
istream & zjedz(istream & plik);

bool read_default_gate(string gate_name, double *low, double *high);
bool read_banana(string gate_name, TjurekPolyCond *  *polygon);

// useful alias
namespace FH = File_helper;
namespace Tfile_helper = File_helper;
namespace Nfile_helper = File_helper;
#endif
