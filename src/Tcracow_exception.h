/***************************************************************************
                          tcracow_exception.h  -  description
                             -------------------
    begin                : Sat Jun 21 2003
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

#ifndef TCRACOW_EXCEPTION_H
#define TCRACOW_EXCEPTION_H

#include <string>
using namespace std;

/**this is a class to send anny message from the cracow viewer to the spy program
  *@author Jurek Grebosz (IFJ Krakow, Poland)
  */

class Tcracow_exception
{
public:
    Tcracow_exception();
    ~Tcracow_exception();
public: // Public attributes
    /**  */
    string message;
};

#endif
