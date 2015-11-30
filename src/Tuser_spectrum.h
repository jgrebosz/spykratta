/***************************************************************************
                          Tuser_spectrum.h  -  description
                             -------------------
    begin                : Wed Sep 3 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz, IFJ   Cracow
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#ifndef TUSER_SPECTRUM_H
#define TUSER_SPECTRUM_H

/**
 *@author dr Jerzy Grebosz, IFJ   Cracow
 */
#include <memory>   // for auto pointer

#include "user_spectrum_description.h"
#include "Tnamed_pointer.h"
#include "Tuser_gated_spectrum_definition.h"
#include "Tself_gate_abstract_descr.h"
class spectrum_abstr;
class Tuser_condition ;


//////////////////////////////////////////////////////////////////////////
struct Tincr_entry
{
    Tnamed_pointer nam_ptr ;
    Tself_gate_abstract_descr* self_gate_descr_ptr ; // to delete the new's

    Tincr_entry(Tnamed_pointer np,  Tself_gate_abstract_descr*  ptr)
        :   nam_ptr(np), self_gate_descr_ptr(ptr)
    {}
};
///////////////////////////////////////////////////////////////////////////
class Tuser_spectrum
{
    user_spectrum_description desc ; //!

    spectrum_abstr * spec_ptr ; //!

    vector<Tincr_entry> x_incr_addresses ;
    vector<Tincr_entry> y_incr_addresses ;
    Tuser_condition * cond_ptr ;
    bool * cond_result_ptr ;

    /** All selfgates used by this spectrum are listed here. As selfgates are
    created with new, the destructor of the Tuser_spectrum should remove them */
    map<string, Tself_gate_abstract_descr* > map_of_selfgates;

public:
    Tuser_spectrum();
    ~Tuser_spectrum();

    void create_the_spectrum();

    /** Read the definition prepared in the text file (for example by cracow) */
    void read_in_parameters(string s);

    user_spectrum_description   give_description();
    string give_name();
    /** converting the string names into addresses of data */
    void set_incrementers();
    void make_incrementations();
    void save_to_disk();
public: // Public attributes
    /**  */
    void remember_address_of_condition(Tuser_condition *t);
    /** Has X and Y incrementers */
    void make_incr_of_2D_spectrum();
    /** spectrum is 1D, so only X will be incremeted */
    void make_incr_of_1D_spectrum();
    /** No descriptions */
    void find_y_to_increment_with(double x_value, void *address);

protected: // Protected methods
    /** This function finds in the special vector the selfgate description
    If this description is on in the vector, reads it from the disk, stores
    in the vector and gives the address */
    Tself_gate_abstract_descr* address_of_self_gate(string sg_name);
    /** No descriptions */
    void clear_selfgates_map();

    /** in case if the name of the incrementer was not found, perhaps this name
    is on the list of the deprecated pointers */

public:
    static
    Tmap_of_named_pointers::iterator
    perhaps_deprecated_name_of_incrementer(Tmap_of_named_pointers & np, string str);

};

#endif
