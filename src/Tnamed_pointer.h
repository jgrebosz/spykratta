/***************************************************************************
                          Tnamed_pointer.h  -  description
                             -------------------
    begin                : Wed Aug 13 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/
#ifndef TNAMED_POINTER_H
#define TNAMED_POINTER_H

#include <string>
#include <map>
#include <vector>
using namespace std;

class Tincrementer_donnor;
/**This is the class for wizard. It will read the names of the variables
which we will need on spectrum
  *@author dr. Jerzy Grebosz (IFJ Krakow, Poland)
  */
class Tuser_incrementer;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tnamed_pointer
{
    Tuser_incrementer * adr_user_incr { nullptr};
public:
    bool is_user_def_incrementer {false } ;
    bool flag_multi_entry ;

    enum Twhat_type
    {
        is_double = 1, is_int, is_bool,
        //is_vector_double, is_vector_int, is_vector_bool
    }  ;

    Twhat_type what_type ;
    union
    {
        const double *double_ptr ;
        const int    *int_ptr;
        const bool   *bool_ptr ;
    };
    bool *data_usable  ;  // ptr to the value telling if the data is good of bad (element not fired)
    Tincrementer_donnor *ptr_detector {nullptr}  ; // to know when X Y are comming from the same detector

    // for TOTAL entries
    struct vek_entry
    {
        Twhat_type v_what_type ;
        union
        {
            double *v_double_ptr ;
            int    *v_int_ptr;
            bool   *v_bool_ptr ;
        };
        bool *v_data_usable ;// ptr to the value telling if the data is good of bad (element not fired)
        Tincrementer_donnor *v_ptr_detector;
    };

    vector<vek_entry> vek ;

public:
    Tnamed_pointer();
    //~Tnamed_pointer();

    // null below means that no indicator, so the data is always usable
    // Otherwise there is a pointer to bool value telling:
    //    - this data is true (fired, good)
    //    - this data is false (not fired, not good)
    Tnamed_pointer(const int *ip, bool *data_usable, Tincrementer_donnor *det);
    Tnamed_pointer(const bool *b_ptr, bool *data_usable, Tincrementer_donnor *det);
    Tnamed_pointer(const double *p, bool *data_usable, Tincrementer_donnor *det);

    // user incrementer can be only 'double'
    Tnamed_pointer(double *p, bool *data_usable, Tincrementer_donnor *det,Tuser_incrementer* adr):
        Tnamed_pointer(p, data_usable, det)
    {
        is_user_def_incrementer = true;
        adr_user_incr = adr;
    }

    // this function will be used is the user incrementer is using the other user incrementer.
    // if so - during the evalustion time this user incrementer must ask the other to evaluate himself - first;
    Tuser_incrementer*  give_user_incr_adr() {
        return adr_user_incr;
    }

    bool is_user_incrementer() {
        return is_user_def_incrementer;
    }

    void add_item(int *ip, bool *data_usable, Tincrementer_donnor *det);
    void add_item(bool *b_ptr, bool *data_usable, Tincrementer_donnor *det);
    void add_item(double *p, bool *data_usable, Tincrementer_donnor *det);

    static
    void add_as_ALL(string entry_name, double *p, bool *data_usable, Tincrementer_donnor *det);
    static
    void add_as_ALL(string entry_name, int *p, bool *data_usable, Tincrementer_donnor *det);
    static
    void add_as_ALL(string entry_name, bool *p, bool *data_usable, Tincrementer_donnor *det);

    /** No descriptions */

    int is_in_AND_gate(double low, double high, bool obligatory);
    int is_in_OR_gate(double low, double high, bool obligatory);

protected: // Protected methods
    /** The first entry always is placed in single.
    In case of adding the next - we move single to the [0] elem of vector */
    void copy_single_to_vector();
};
/////////////////////////////////////////////////////////////////////
typedef map<string, Tnamed_pointer> Tmap_of_named_pointers;

extern map<string, Tnamed_pointer> named_pointer ;

#endif
