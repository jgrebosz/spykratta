/***************************************************************************
                          Tkratta_crystal.h  -  description
                             -------------------
    begin                : Fri Oct 12 2014
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/
#ifndef TKRATTA_CRYSTAL_PLASTIC_H
#define TKRATTA_CRYSTAL_PLASTIC_H



#ifdef KRATTA_PLASTIC_PRESENT



#define PLASTIC_HAS_ENERGY_DATA false

/**KRATTA detector consists of many elementary detectors   */
#include <string>

#include "Tfrs_element.h"
#include "spectrum.h"
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"
#include "Tkratta_crystal.h"

class Tkratta;

/////////////////////////////////////////////////////////////////////////////
class Tkratta_crystal_plastic : public Tincrementer_donnor
{

protected:
    Tkratta * owner; //!
    string name_of_this_element ;
    int my_id_nr;

#if PLASTIC_HAS_ENERGY_DATA
    int energy_raw;
    double energy_cal;
    vector<double> energy_calibr_factors;
    spectrum_1D* spec_energy_raw;
    spectrum_1D* spec_energy_cal;
    // Data for GOOD kratta
    static bool  good_energy_cal_requires_threshold;

    static double energy_cal_lower_threshold;
    static double energy_cal_upper_threshold;

     spectrum_1D* spec_fan;
#endif

    int time_raw;
    double time_cal;

    bool flag_fired, flag_good;

    // calibration factors

    vector<double> time_calibr_factors;


    double phi_geom, theta_geom ;
    double
          phi_geom_radians,
          theta_geom_radians ;
      double distance_from_target ; // from the target
      double crystal_position_x; // calculated from theta phi
      double crystal_position_y; // calculated from theta phi
      double crystal_position_z; // calculated from theta phi


    // ptr to his data in the TEvent.h
    int16_t * data;

    // spectra -----------------
    spectrum_1D* spec_time_raw;
    spectrum_1D* spec_time_cal;

    spectrum_2D* spec_geometry = nullptr;
    spectrum_2D* spec_geometry_ratios = nullptr;
    spectrum_2D* spec_geometry_from_scalers = nullptr;

public:

    Tkratta_crystal_plastic(Tkratta *ptr_owner, string name, int id_nr);
    virtual ~Tkratta_crystal_plastic();


    void make_preloop_action(ifstream & );
    void analyse_current_event();
    auto give_xpos() { return crystal_position_x; }
    auto give_ypos() { return crystal_position_y; }
    string give_name() { return name_of_this_element ;}

    void incr_ratios_matrix(int value)
    {
        spec_geometry_ratios->manually_increment_by(crystal_position_x, crystal_position_y, value);
    }


    void incr_geometry_from_scalers_matrix(int value)
    {
          spec_geometry_from_scalers->manually_increment_by(crystal_position_x, crystal_position_y, value);
    }

   /** where in the event starts data for this particular kratta_crystal */
    void set_event_data_pointers(int16_t *ptr );

    bool fired() {
        return flag_fired ;
    }
    bool fired_good() {
        return flag_good;
    }
//    int give_x() {
//        return pos_x;
//    }
//    int give_y() {
//        return pos_y;
//    }

    double give_time_cal()
    { //if(!calculations_already_done)analyse_current_event();
        return time_cal;
    }

    //bool check_selfgate(Tself_gate_abstract_descr *desc);

    // Data for GOOD kratta
    static bool  good_time_cal_requires_threshold;

    static double time_cal_lower_threshold;
    static double time_cal_upper_threshold;

    int quarter_x;
    int give_quarter() {
            return quarter_x;
        }


protected: // Protected methods
    void create_my_spectra();

protected: // Protected attributes
    /**  */


    /** Whis segment (numbering 0-n) */
 //int quarter;

//    int pos_x;   // geometric position of this crystal
//    int pos_y;

    void read_gates();

    void read_cal_factors_into(vector<double> & w, string slowo, ifstream & s)
    {

        constexpr int how_many_factors = 2;
        // first reading the energy calibration
        Tfile_helper::spot_in_file(s, slowo );

        w.clear() ;
        for(int i = 0 ; i < how_many_factors ; i++)
        {
            // otherwise we read the data
            double value ;
            s >> value ;

            if(!s) {
                string mess = "I found keyword:" + slowo
                        + ", but error was in reading its value." ;
                throw mess ;
            }
            // if ok
            w.push_back(value ) ;

        }

        if(w.size() < 2)
        {
            string mess = "It should be at least 2 calibration factors for " +
                    name_of_this_element + "_" + slowo;
            throw mess ;
        }

    }

private:

};
///////////////////////////////////////////////////////////////////////////////

#endif   //  KRATTA_PLASTIC_PRESENT


#endif
