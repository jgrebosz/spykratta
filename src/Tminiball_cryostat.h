#ifndef _MINIBALL_CRYOSTAT_H_
#define _MINIBALL_CRYOSTAT_H_
#include "spectrum.h"
#include <vector>
#include "Tminiball_elementary_det.h"
#include "Tincrementer_donnor.h"
#include <string>
istream & zjedz(istream & plik);

//////////////////////////////////////////////////////////////////////////////
class Tminiball_cryostat : public Tincrementer_donnor
{
public:
    static Tminiball *miniball_pointer ;
    static int all_miniball_cryostats_multiplicity ;  // of cryostats (0-15)
    static int max_fold;

    // 3 modes
    static bool adjacent_mode;
    static bool nonadjacent_mode;
    static bool sum_all_mode;

    static int primary_seg_algorithm ;  // others do not exist yet
    /** for addback algorithm */
    static int Eprimary_threshold;

protected:
    int this_cryostat_multiplicity ;

    // this below is not a vector. This is only a pointer to a vector
    // which is in the parent class frs. There, we "register" all
    // spectra created in various miniball_detectors. By this - it is
    // easier to make automatic save

    // but of course the real owners of the spectra are particullar
    // miniball_detectors
    vector<spectrum_abstr*> *miniball_spectra_ptr ;

    string name_of_this_element ;

    vector<Tminiball_elementary_det*> crys ;    // seven miniball_elementary_dets of the cryostat

    // the pointer to this place in the unpacked event,
    // where is the data for this cryostat

    int (* event_data_ptr)[7][8];

public:
    Tminiball_cryostat(string name, int nr) ;
    //-----------------------------

    // read the calibration factors, gates
    void make_preloop_action(ifstream &);

    void analyse_current_event() ;
    string give_detector_name()
    {
        return name_of_this_element ;
    }
    Tminiball_elementary_det* GetCrystal(int cry) const
    {
        return crys[cry] ;
    }
    double core_energy_addback_cal;
    double core_energy_addback_doppler;

    int first_miniball_elementary_det_nb;

    bool flag_addback_incrementer_valid;



    // sets the pointer to this place in the unpacked event,
    // where is the data for this cryostat

    void set_event_data_pointers(int (*ptr)[7][8]);
    /** When we set gate on cate dE vs E matrix to gate the germanoum time  */
    void cryostat_do_increm_of_time_gated_cate();

    // Rising will call this function
    void set_this_cryostat_multiplicity(int n)
    {
        this_cryostat_multiplicity = n ;
    }
    // selfgate will call this function
    int get_this_cryostat_multiplicity()
    {
        return this_cryostat_multiplicity;
    }
    void calculate_addback();
    /** because we do not call function analyse_current event for non-fired cryostats */
    void  reset_variables();
protected:
    void create_my_spectra();
    void check_legality_of_data_pointers();
    int id_cryostat ;
    spectrum_1D * spec_core_energy_addback_cal ;
//  spectrum_1D * spec_energy20_addback_cal ;
    spectrum_1D * spec_core_energy_addback_doppler ;
//  spectrum_1D * spec_energy20_addback_doppler ;

};

/////////////////////////////////////////////////////////////////////////

#endif // _MINIBALL_CRYOSTAT_H_
