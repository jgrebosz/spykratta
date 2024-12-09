/***************************************************************************
                          Tkratta_crystal.h  -  description
                             -------------------
    begin                : Fri Oct 12 2014
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/
#ifndef TKRATTA_CRYSTAL_H
#define TKRATTA_CRYSTAL_H

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#ifdef KRATTA_PRESENT

/**KRATTA detector consists of many elementary detectors   */
#include <string>

#include "Tfrs_element.h"
#include "spectrum.h"
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

class Tkratta;

#if 0
class Tkratta_crys_abstract : public Tincrementer_donnor
{
protected:
    Tkratta * owner; //!
    string name_of_this_element ;
public:
    Tkratta_crys_abstract(Tkratta *ptr_owner, string name, int id_nr):
        owner(ptr_owner), name_of_this_element(name) {}

    virtual ~Tkratta_crys_abstract() {};

    void make_preloop_action(ifstream & ) ;

    /** the most important function,  executed for every event */
    void analyse_current_event() ; // return value: was Si hit or not ?



    /** where in the event starts data for this particular kratta_crystal */
    void set_event_data_pointers(int16_t ptr [3+3+3]);
    bool fired();
    bool fired_good() ;
    double give_pd1_time_cal();



    bool check_selfgate(Tself_gate_abstract_descr *desc);
};
#endif
/////////////////////////////////////////////////////////////////////////////
class Tkratta_crystal : public Tincrementer_donnor
{
protected:
    Tkratta * owner; //!
    string name_of_this_element ;
    int id;

    int pd0_amplitude_raw;
    int pd1_amplitude_raw;
    int pd2_amplitude_raw;

    double pd0_amplitude_cal;
    double pd1_amplitude_cal;
    double pd2_amplitude_cal;
    
    int pd0_time_raw;
    int pd1_time_raw;
    int pd2_time_raw;

    double pd0_time_cal;
    double pd1_time_cal;
    double pd2_time_cal;

    int pd0_pedestal_raw;
    int pd1_pedestal_raw;
    int pd2_pedestal_raw;

    int pd0_pedestal_cal;
    int pd1_pedestal_cal;
    int pd2_pedestal_cal;




    bool flag_fired, flag_good;

    // calibration factors
    vector<double> pd0_amplitude_calibr_factors;
    vector<double> pd1_amplitude_calibr_factors;
    vector<double> pd2_amplitude_calibr_factors;

    vector<double> pd0_time_calibr_factors;
    vector<double> pd1_time_calibr_factors;
    vector<double> pd2_time_calibr_factors;

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

    spectrum_1D* spec_pd0_amplitude_raw;
    spectrum_1D* spec_pd1_amplitude_raw;
    spectrum_1D* spec_pd2_amplitude_raw;

    spectrum_1D* spec_pd0_amplitude_cal;
    spectrum_1D* spec_pd1_amplitude_cal;
    spectrum_1D* spec_pd2_amplitude_cal;


    spectrum_1D* spec_pd0_time_raw;
    spectrum_1D* spec_pd1_time_raw;
    spectrum_1D* spec_pd2_time_raw;

    spectrum_1D* spec_pd0_time_cal;
    spectrum_1D* spec_pd1_time_cal;
    spectrum_1D* spec_pd2_time_cal;


    spectrum_1D* spec_fan_pattern_pds_raw;

    spectrum_1D *spec_pd0_pedestal_raw,
    *spec_pd1_pedestal_raw,
    *spec_pd2_pedestal_raw;

    spectrum_2D* spec_pd0_vs_pd1;
    spectrum_2D* spec_pd1_vs_pd2;
spectrum_2D* owner_spec_geometry = nullptr;




public:

    Tkratta_crystal(Tkratta *ptr_owner, string name, int id_nr);
    virtual ~Tkratta_crystal();

    // Cate register of all spectra
    //vector< spectrum_abstr*>*  kratta_spectra_ptr();

    virtual void make_preloop_action(ifstream & );

    /** the most important function,  executed for every event */
    virtual void analyse_current_event() ; // return value: was Si hit or not ?



    /** where in the event starts data for this particular kratta_crystal */
    virtual void set_event_data_pointers(int16_t ptr [3+3+3]);
    virtual bool fired() {
        return flag_fired ;
    }
    virtual bool fired_good() {
        return flag_good;
    }
    double give_x() {
        return crystal_position_x;
    }
    double give_y() {
        return crystal_position_y;
    }
    double give_z() {
        return crystal_position_z;
    }

//    int give_current_fan_statistics()
//    {
//        return current_fan_statistics;
//    }



    virtual double give_pd1_time_cal()
    { //if(!calculations_already_done)analyse_current_event();
        return pd1_time_cal;
    }

    virtual double give_pd0_time_cal()
    { //if(!calculations_already_done)analyse_current_event();
        return pd1_time_cal;
    }

    virtual
    bool check_selfgate(Tself_gate_abstract_descr *desc);

    // Data for GOOD kratta
    static bool  good_pd0_time_cal_requires_threshold;
    static bool  good_pd1_time_cal_requires_threshold;
    static bool  good_pd2_time_cal_requires_threshold;

    static double pd0_time_cal_lower_threshold;
    static double pd0_time_cal_upper_threshold;

    static double pd1_time_cal_lower_threshold;
    static double pd1_time_cal_upper_threshold;

    static double pd2_time_cal_lower_threshold;
    static double pd2_time_cal_upper_threshold;


    void read_geometry();
    string give_name() { return name_of_this_element ;}
protected: // Protected methods
    void create_my_spectra();

protected: // Protected attributes
    /**  */


    /** Whis segment (numbering 0-n) */
    //int segment_nr;
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

#endif   //  KRATTA_PRESENT
#endif   // __CINT__

#endif
