/***************************************************************************
                          user_spectrum_description.cpp  -  description
                             -------------------
    begin                : Fri Jul 25 2003
    copyright            : (C) 2003 by Dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/

#include "user_spectrum_description.h"
//#include "paths.h"
#include <iostream>
#include <fstream>
#include "Tfile_helper.h"


//*************************************************************************
user_spectrum_description::user_spectrum_description()
{

    name_of_spectrum = "user_" ;
    enabled = true ;

    dimmension = spec_1D;
    bins_x = 8192 ;
    beg_x = 0;
    end_x = 8192;

    bins_y = 256 ;
    beg_y = 0;
    end_y = 8192;
    policy_when_increm  = 0 ; // always

    flag_rotation = false;
    rotation_angle = 0 ;
    condition_name = "No_condition" ;

}
//**************************************************************************
user_spectrum_description::~user_spectrum_description()
{
}
//**************************************************************************
/** read the definitons from the disk */
void user_spectrum_description::read_from(string pathed_name)
{
    incrementers_x.clear();
    incrementers_y.clear();
    x_rot_cal_fact.clear();
    y_rot_cal_fact.clear();


    string file_name = pathed_name ;
    // path.user_def_spectra + name_of_spectrum + ".user_definition" ;
    ifstream plik(file_name.c_str());
    if(!plik)
    {
        cout << "Can't open the file "  << file_name
             << " for reading " << endl;
        return ;
    }

    try
    {

        Tfile_helper::spot_in_file(plik, "spectrum_name");
        plik >> name_of_spectrum ;   // will be without the path and extension
        enabled = (int)Tfile_helper::find_in_file(plik, "enabled");

        dimmension = (type_of_data) Tfile_helper::find_in_file(plik, "dimmension");

        bins_x = (int) Tfile_helper::find_in_file(plik, "bins_x");
        beg_x = Tfile_helper::find_in_file(plik, "beg_x");
        end_x = Tfile_helper::find_in_file(plik, "end_x");

        bins_y = (int) Tfile_helper::find_in_file(plik, "bins_y");
        beg_y = Tfile_helper::find_in_file(plik, "beg_y");
        end_y = Tfile_helper::find_in_file(plik, "end_y");



        // reading X incrementers-------------------------
        //Tfile_helper::spot_in_file(plik, "incrementers_x");

        try
        {
            Tfile_helper::spot_in_file(plik, "incrementers_x");
        }
        catch(...)
        {
            Tfile_helper::repair_the_stream(plik);
            Tfile_helper::spot_in_file(plik, "incrementors_x");      // old time it was "incementOrs"
        }

        string klamra ;
        plik >> zjedz >> klamra ; // "{";
        string wyraz, self_gate ;

        do
        {
            plik >> wyraz >> self_gate;
            if(wyraz == "}" || !plik) break ;
            incrementers_x.push_back(pair<string, string>(wyraz, self_gate)) ;
        }
        while(plik);

        // reading Y incrementers------------------------

        //Tfile_helper::spot_in_file(plik, "incrementers_y");
        try
        {
            Tfile_helper::spot_in_file(plik, "incrementers_y");
        }
        catch(...)
        {
            Tfile_helper::repair_the_stream(plik);
            Tfile_helper::spot_in_file(plik, "incrementors_y");      // old time it was "incementOrs"
        }
        plik >> zjedz >> klamra ; // "{";

        do
        {
            plik >> wyraz >> self_gate;
            if(wyraz == "}" || !plik) break ;
            incrementers_y.push_back(pair<string, string>(wyraz, self_gate)) ;
        }
        while(plik);

        policy_when_increm  = (int) Tfile_helper::find_in_file(plik, "policy_when_increment");

        Tfile_helper::spot_in_file(plik, "condition_name");
        plik >> condition_name ;
        //cout << "Read condition name = " << condition_name << endl ;

        try  // this may not exist in older definitions
        {
            flag_rotation = (bool) Tfile_helper::find_in_file(plik, "flag_rotation");
            rotation_angle = Tfile_helper::find_in_file(plik, "rotation_angle");

            // read vector of x rotated calibration factors
            double tmp;

            try{
            Tfile_helper::spot_in_file(plik, "x_rotated_calibration_factors");
            for(int i = 0 ; i < 3 ;++i){
                plik >> tmp;
                x_rot_cal_fact .push_back(tmp);
            }

            // read vector of x rotated calibration factors
            Tfile_helper::spot_in_file(plik, "y_rotated_calibration_factors");
            for(int i = 0 ; i < 3 ;++i){
                plik >> tmp;
                y_rot_cal_fact .push_back(tmp);
            }
            } catch(Tfile_helper_exception &m)
            {
                x_rot_cal_fact = { 0, 1, 0};
                y_rot_cal_fact = { 0, 1, 0};

            }

        }
        catch(Tfile_helper_exception &m)
        {
            flag_rotation = false;
            rotation_angle = 0.0;
            x_rot_cal_fact = { 0, 1, 0};
            y_rot_cal_fact = { 0, 1, 0};

        }

    }
    catch(Tfile_helper_exception &m)
    {
        cout << "Error during reading the file "
             << file_name << " : "
             << m.message << endl;
    }
}
//**************************************************************************
/** save the definition on the disk */
void user_spectrum_description::write_definitions(string path_only)
{

    cout << "This function is never used by the spy" << endl;
    string file_name = path_only + name_of_spectrum + ".user_definition" ;

    ofstream plik(file_name.c_str());
    if(!plik)
    {
        cout << "Can't open the file " << file_name
             << " for writing " << endl;
        return ;
    }

    plik
            << "// This is a definiton of the conditional spectrum\n"
            << "// enables are 0 or 1 which means false or true\n"
            << "// comments are marked using two slashes: // comment \n\n"
            << "spectrum_name\t\t " << name_of_spectrum << "\n"
            << "enabled\t\t" << enabled << "\n\n"


            << "dimmension\t\t" << dimmension
            << "\n//-----------------------------------------------------\n\n"

            << "bins_x\t\t " << bins_x <<  "\n"
            << "beg_x\t\t " << beg_x <<  "\t\t// left edge of the first bin\n"
            << "end_x\t\t " << end_x <<  "\t\t// right edge of the last bin\n\n"
            << "bins_y\t\t " << bins_y <<  "\n"
            << "beg_y\t\t " << beg_y <<  "\t\t// left edge of the first bin\n"
            << "end_y\t\t " << end_y <<  "\t\t// right edge of the last bin\n\n"

            << "flag_rotation\t\t " << flag_rotation <<  "\t\t// left edge of the first bin\n"
            << "rotation_angle\t\t " << rotation_angle <<  "\t\t// right edge of the last bin\n\n"
            << endl;


    plik << "\nincrementers_x                // and their self_gates\n{\n";
    for(unsigned i = 0 ; i < incrementers_x.size() ; i++)
    {
        plik << incrementers_x[i].first << "\t"
             << incrementers_x[i].second   << "\n" ;
    }
    plik << "}\n";

    plik << "\nincrementers_y               // and their self_gates\n{\n";
    for(unsigned i = 0 ; i < incrementers_y.size() ; i++)
    {
        plik <<  incrementers_y[i].first << "\t" << incrementers_y[i].second << "\n" ;
    }
    plik << "}\n\n";


    plik << "policy_when_increment\t"
         << policy_when_increm
         << "\t// 0= always, 1=only when from DIFFERENT detector, 2 = only when from SAME detectror"
         << "\n" ;


    plik << "x_rotated_calibration_factors" ;
    for( auto x : x_rot_cal_fact)
        plik << "  " << x  ;
    plik << "\n";

    plik << "y_rotated_calibration_factors" ;
    for( auto y : y_rot_cal_fact)
        plik << "  " << y  ;
    plik << "\n";

    if(condition_name.find(".cnd") == string::npos)
    {
        condition_name += ".cnd" ;
    }
    plik  << "\ncondition_name\t\t" << condition_name << endl ;





}
//*********************************************************************
/** No descriptions */
void user_spectrum_description::set_name(string s)
{
    name_of_spectrum = s ;
}
//********************************************************************
/** To know is somebody just changed some parameters so,
that we must re-create it (changed dimmension or binning) */
bool user_spectrum_description::are_parameters_identical(const user_spectrum_description & u) const
{
    if(dimmension != u.dimmension) return false ;
    if(dimmension == spec_1D)
    {
        return
            (
                bins_x == u.bins_x
                &&
                beg_x == u.beg_x
                &&
                end_x == u.end_x) ;
    }
    else
    {
        return
            (
                bins_x == u.bins_x
                &&
                beg_x == u.beg_x
                &&
                end_x == u.end_x
                &&
                bins_y == u.bins_y
                &&
                beg_y == u.beg_y
                &&
                end_y == u.end_y
                    &&
                    rotation_angle == u.rotation_angle
                    &&
                    flag_rotation == u.flag_rotation
            ) ;
    }

}
