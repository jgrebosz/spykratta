/***************************************************************************
                          Tcondition_description.cpp  -  description
                             -------------------
    begin                : Mon May 19 2003
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

#include "Tcondition_description.h"
#include "Tfile_helper.h"
#include <fstream>

Tcondition_description::Tcondition_description()
{
}
Tcondition_description::~Tcondition_description()
{
}
//**************************************************************
/** No descriptions */
void Tcondition_description::read_in_description()
{


    item_1D_AND.clear() ;
    item_2D_AND.clear() ;
    item_1D_OR.clear() ;
    item_2D_OR.clear() ;
    other_condition_AND.clear() ;
    other_condition_OR.clear() ;
    other_condition_NAND.clear() ;
    other_condition_NOR.clear() ;

    string pat_name = "./conditions/" + name ;
    if(pat_name.find(".cnd") == string::npos)
    {
        pat_name += ".cnd" ;
    }
    ifstream plik(pat_name.c_str());
    if(!plik)
    {
        // cant open file warning
        cout << "Error while opening file: " << pat_name << endl;
        exit(1);
    }

    try
    {

        Tfile_helper::spot_in_file(plik, "1D_AND_gates");
        string klamra ;
        plik >> zjedz >> klamra ; // "{"

        Titem_1D  item_1D_tmp;

        do
        {

            plik >> zjedz >> item_1D_tmp.var_name;
            if(item_1D_tmp.var_name == "}" || !plik) break ;
            plik >> zjedz >> item_1D_tmp.obligatory_in_event ;
            plik >> zjedz >> item_1D_tmp.min_value;
            plik >> zjedz >> item_1D_tmp.max_value;

            item_1D_AND.push_back(item_1D_tmp) ;
        }
        while(plik);



        Tfile_helper::spot_in_file(plik, "1D_OR_gates");

        plik >> zjedz >> klamra ; // "{"



        do
        {

            plik >> zjedz >> item_1D_tmp.var_name;
            if(item_1D_tmp.var_name == "}" || !plik) break ;
            plik >> zjedz >> item_1D_tmp.obligatory_in_event ;
            plik >> zjedz >> item_1D_tmp.min_value;
            plik >> zjedz >> item_1D_tmp.max_value;

            item_1D_OR.push_back(item_1D_tmp) ;
        }
        while(plik);




        Tfile_helper::spot_in_file(plik, "2D_AND_gates");
        plik >> zjedz >> klamra ; // "{"

        Titem_2D  item_2D_tmp;
        do
        {
            plik >> zjedz >> item_2D_tmp.var_x_name;
            if(item_2D_tmp.var_x_name == "}" || !plik) break ;
            plik >> zjedz >> item_2D_tmp.var_y_name;
            plik >> zjedz >> item_2D_tmp.obligatory_in_event;
            plik >> zjedz >> item_2D_tmp.polygon_name;

            item_2D_AND.push_back(item_2D_tmp) ;
        }
        while(plik);



        Tfile_helper::spot_in_file(plik, "2D_OR_gates");
        plik >> zjedz >> klamra ; // "{"

        do
        {
            plik >> zjedz >> item_2D_tmp.var_x_name;
            if(item_2D_tmp.var_x_name == "}" || !plik) break ;
            plik >> zjedz >> item_2D_tmp.var_y_name;
            plik >> zjedz >> item_2D_tmp.obligatory_in_event;
            plik >> zjedz >> item_2D_tmp.polygon_name;

            item_2D_OR.push_back(item_2D_tmp) ;
        }
        while(plik);


        string fname ;

        Tfile_helper::spot_in_file(plik, "other_condition_AND");
        plik >> zjedz >> klamra ; // "{"
        do
        {
            plik >> zjedz >> fname;
            if(fname == "}" || !plik) break ;
            other_condition_AND.push_back(fname);
        }
        while(plik);

        Tfile_helper::spot_in_file(plik, "other_condition_OR");
        plik >> zjedz >> klamra ; // "{"
        do
        {
            plik >> zjedz >> fname;
            if(fname == "}" || !plik) break ;
            other_condition_OR.push_back(fname);
        }
        while(plik);

        Tfile_helper::spot_in_file(plik, "other_condition_NAND");
        plik >> zjedz >> klamra ; // "{"
        do
        {
            plik >> zjedz >> fname;
            if(fname == "}" || !plik) break ;
            other_condition_NAND.push_back(fname);
        }
        while(plik);

        Tfile_helper::spot_in_file(plik, "other_condition_NOR");
        plik >> zjedz >> klamra ; // "{"
        do
        {
            plik >> zjedz >> fname;
            if(fname == "}" || !plik) break ;
            other_condition_NOR.push_back(fname);
        }
        while(plik);

    }


    catch(Tfile_helper_exception &m)
    {
        cout << "Error during reading the file "
             << pat_name << " : "
             << m.message << endl;
        exit(1);
    }
}
//****************************************************************
/** Checking is done before working with any (all) user spectrum */
