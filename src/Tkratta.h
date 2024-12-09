/***************************************************************************
                          tkratta.h  -  description
                             -------------------
    begin                : Oct 12 2014
    copyright            : (C) 2003 by dr Jerzy Grebosz
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/

#ifndef TKRATTA_H
#define TKRATTA_H


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#ifdef KRATTA_PRESENT

//const int how_many_kratta_segments = 9 ;


#include "Tkratta_crystal.h"

//class TIFJEvent;
class TRisingCalibratedEvent;

#include <vector>
#include "spectrum.h"
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"

//class Tsi_detector;
//////////////////////////////////////////////////////////////////////
/**Class representing the KRATTA detector (IFJ)
 *@author Jerzy Grebosz
 */
class Tkratta_crystal_plastic;
class Tkratta_crystal ;
//////////////////////////////////////////////////////////////////////
class Tkratta : public Tfrs_element, public Tincrementer_donnor
{
//    friend class Tkratta_crys_abstract ;
protected:
    double distance ;
    vector<Tkratta_crystal *>  crystal ;     // segments of the kratta detector

    vector<Tkratta_crystal_plastic *>  plastic;

//    int first_plastic_module_nr; // as the plastic and kratta are in the same vector
    // we need o know when one set is finishing and the other starts.

public:
    Tkratta(string name);
    ~Tkratta();

    //-------------------------------
    vector< spectrum_abstr*>*  spectra_ptr()
    {
        return frs_spectra_ptr  ;
    }

    Tkratta_crystal *  give_crystal(unsigned nr)
    {
        if(nr >= (unsigned int) KRATTA_NR_OF_CRYSTALS) return nullptr;
        return crystal[nr] ;
    }
    //------------------------------
    void make_preloop_action(ifstream &) ;  // read the calibration factors, gates

    // taking the calibration factors
    void read_calibration_and_gates_from_disk();  // called form preloop function

    void analyse_current_event();
    void make_user_event_action(); // shop of events ?
    void make_postloop_action();  // saving the spectra ?
    void learn_geometry_of_all_elements_of_kratta();

    inline double give_distance() {
        return distance ;
    }
    /** made in post loop, but also when the FRS ask for it (i.e. every 5 min) */
    void save_spectra();

    /** kratta will ask the Si detector for this information */
    //    int how_many_Si_hits();
    bool get_flag_only_one_good() {
        if(!calculations_already_done)    analyse_current_event();
        return flag_only_one_good;
    }
    double get_general_pd1_time_cal__when_only_one_good(){
        if(!calculations_already_done)    analyse_current_event();
        return general_pd1_time_cal__when_only_one_good;
    }
    int give_graph_max_x() { return graph_max_x;}
    int give_graph_min_x() { return graph_min_x;}
    int give_graph_max_y() { return graph_max_y;}
    int give_graph_min_y() { return graph_min_y;}

    spectrum_2D  *spec_geometry; // children will use it
    spectrum_2D  *spec_plastic_geometry; // children will use it
    spectrum_2D* spec_geometry_ratios;  // children will use it
    spectrum_2D* spec_geometry_from_scalers; // children will use it

    void zero_ratios_matrix()
    {
        spec_geometry_ratios->zeroing();
    }

    //------------------------------
protected:

    //for testing time


    void simulate_event();
    /** No descriptions */
    void create_my_spectra();

    int multiplicity_of_hits;
    int multiplicity_of_good_hits;
    int multiplicity_of_hits_in_plastic;
    int multiplicity_of_good_hits_in_plastic;

    spectrum_2D  *matr_kratta_vs_plastic;


    spectrum_1D * spec_multiplicity;
    spectrum_1D * spec_multiplicity_of_good;
    spectrum_1D * spec_plastic_multiplicity;
    spectrum_1D * spec_plastic_multiplicity_of_good;
    spectrum_1D * spec_fan;
    spectrum_1D * spec_fan_of_good;
    spectrum_1D * spec_plastic_fan;
    spectrum_1D * spec_plastic_fan_of_good;
    spectrum_1D * spec_plastic_scalers;
    spectrum_1D * spec_fan_ratios_plastic_over_kratta;


    int plastic_statistics[KRATTA_NR_OF_PLASTICS];
    int kratta_statistics[KRATTA_NR_OF_CRYSTALS];
    double ratio_statistics[KRATTA_NR_OF_PLASTICS];


    double general_pd1_time_cal__when_only_one_good;
    bool flag_only_one_good;
    spectrum_1D * spec_general_pd1_time_cal__when_only_one_good;

    double general_plastic_time_cal__when_only_one_good;
    bool flag_only_one_good_in_plastic;
    spectrum_1D * spec_general_plastic_time_cal__when_only_one_good;

    // sizes of matrices for geometry view

    int graph_max_x = -9999;
    int graph_min_x = +9999;
    int graph_max_y = -9999;
    int graph_min_y = +9999;
    int graph_margines = 10;
    int graph_columns = 1;
    int graph_rows = 1;

};
//////////////////////////////////////////////////////////////////////////////

#endif  // KRATTA_PRESENT
#endif // __CINT__

#endif
