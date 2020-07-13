/***************************************************************************
                          Tcondition_description.h  -  description
                             -------------------
    begin                : Mon May 19 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/


#ifndef TCONDITION_DESCRIPTION_H
#define TCONDITION_DESCRIPTION_H

#include <string>
#include <vector>
#include <iostream>
using namespace std;
#include <cstdlib>

/**This class represents the one piece of the chain of condition.
  *@author Jurek Grebosz (IFJ Krakow, Poland)
  */

class Tcondition_description
{
//   friend class manager_user_spectra ;
//    friend class condition_wizard ;
protected:
public:
    struct Titem_1D
    {
        bool obligatory_in_event ;
        string var_name ;
        double min_value, max_value ;
    };
    struct Titem_2D
    {
        bool obligatory_in_event ;
        string var_x_name ;
        string var_y_name ;
        string polygon_name;
    };


    string name ;
    vector <Titem_1D> item_1D_AND;
    vector <Titem_2D> item_2D_AND;
    vector <Titem_1D> item_1D_OR;
    vector <Titem_2D> item_2D_OR;
    vector <string> other_condition_AND;
    vector <string> other_condition_OR;
    vector <string> other_condition_NAND;
    vector <string> other_condition_NOR;

public:
    Tcondition_description();
    ~Tcondition_description();
    /** No descriptions */
    void save_condition();
    /** No descriptions */
    void read_in_description();
    void set_name(string n)
    {
        name = n;
    }
    string give_name()
    {
        return name;
    }


};

#endif
