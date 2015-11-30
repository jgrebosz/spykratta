/***************************************************************************
                          Tincrementer_donnor.h  -  description
                             -------------------
    begin                : Thu Sep 11 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz
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

#ifndef TINCREmenter_DONNOR_H
#define TINCREmenter_DONNOR_H


/**Abstract class to mark the objects which may give variables to increment user defined spectra
  *@author dr Jerzy Grebosz
  */
#include <iostream>
using namespace std;


//#include "enum_selfgate_types.h"

#include "tselfgate_type.h"
#include "Tself_gate_abstract_descr.h"

/////////////////////////////////////////////////////////////////////
class Tincrementer_donnor
{
public:
    Tincrementer_donnor() :  my_selfgate_type(Tselfgate_type::Types::not_available ) {}  ;
    virtual ~Tincrementer_donnor();
    /** No descriptions */
    virtual bool check_selfgate(Tself_gate_abstract_descr * desc)
    {
		static int nr;
		static int how_often = 10000 ;
		if((++nr % how_often) == false){
            cout << "This is an abstract version of the check gate " << desc->give_name()  <<   " which was called !!!!  "    
				<< how_often
				<< endl;
				how_often += 1000;
				nr = 0 ;
		}
        return true ;
    }

    Tselfgate_type::Types   give_selfgate_type() {
        return my_selfgate_type.my_type;
    }
    bool selfgate_type_is_not_avaliable() {
        return my_selfgate_type.sg_is_not_available();
    }
protected:
    Tselfgate_type   my_selfgate_type;
};

#endif
