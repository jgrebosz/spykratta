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

/**KRATTA detector consists of 9 segments. Each of them is a system of
 * Si and CsI detectors, which is just a crystal detector
 *
 */
#include <string>

#include "Tfrs_element.h"
#include "spectrum.h"
#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

class Tkratta;

/////////////////////////////////////////////////////////////////////////////
class Tkratta_crystal : public Tincrementer_donnor
{
protected:
    Tkratta * owner; //!
    string name_of_this_element ;

    int pd0_raw;                 // photodiode signals
    int pd1_raw;
    int pd2_raw;
    
    int pd0_time_raw;                 // photodiode time signals
    int pd1_time_raw;
    int pd2_time_raw;

    bool flag_fired;

    // calibration factors
    vector<double> pd0_calibr_factors;
    vector<double> pd1_calibr_factors;
    vector<double> pd2_calibr_factors;

    double pd0_cal;
    double pd1_cal;
    double pd2_cal;

    // ptr to his data in the TEvent.h
    int16_t * data;

    // spectra -----------------

    spectrum_1D* spec_pd0_raw;
    spectrum_1D* spec_pd1_raw;
    spectrum_1D* spec_pd2_raw;

    spectrum_1D* spec_pd0_time_raw;
    spectrum_1D* spec_pd1_time_raw;
    spectrum_1D* spec_pd2_time_raw;

    spectrum_1D* spec_pd0_cal;
    spectrum_1D* spec_pd1_cal;
    spectrum_1D* spec_pd2_cal;

    spectrum_1D* spec_fan_pattern_pds_raw;

    spectrum_1D *spec_pd0_pedestal_raw,
                *spec_pd1_pedestal_raw,
                *spec_pd2_pedestal_raw;

    spectrum_2D* spec_pd0_vs_pd1;
    // spectrum_2D* spec_pd1_vs_pd2; Not reliable signal pd2



public:

    Tkratta_crystal(Tkratta *ptr_owner, string name, int id_nr);
    virtual ~Tkratta_crystal();

    // Cate register of all spectra
    //vector< spectrum_abstr*>*  kratta_spectra_ptr();

    void make_preloop_action(ifstream & );

    /** the most important function,  executed for every event */
    void analyse_current_event() ; // return value: was Si hit or not ?



    /** where in the event starts data for this particular kratta_crystal */
    void set_event_data_pointers(int16_t ptr [3]);
    bool fired() {
        return flag_fired ;
    }
    int give_x() {
        return pos_x;
    }
    int give_y() {
        return pos_y;
    }


protected: // Protected methods
    void create_my_spectra();

protected: // Protected attributes
    /**  */


    /** Whis segment (numbering 0-n) */
    int segment_nr;
    int pos_x;   // geometric position of this crystal
    int pos_y;

    void read_gates();



private:



};
///////////////////////////////////////////////////////////////////////////////

#endif   //  KRATTA_PRESENT
#endif   // __CINT__

#endif
