#ifndef _ZET_CALCULATOR_H_
#define _ZET_CALCULATOR_H_

// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "Tfrs_element.h"
#include "Tmusic.h"
#include "Taoq_calculator.h"
#include "Tsilicon_pin_diode.h"
#include "Tincrementer_donnor.h"
#include "spectrum.h"
#include <string>
#include <vector>
//////////////////////////////////////////////////////////////////
class Tzet_calculator : public Tfrs_element, public Tincrementer_donnor
{
    double zet;

#ifdef PIN_DIODE
    double  zet_from_pin_diode ;
    bool  zet_from_pin_diode_ok ;
    Tsilicon_pin_diode   * sil_pin_diode ;  // in case if music works wrong
    spectrum_2D   *matr_zet_pin_vs_aoq;
#endif

    double vel_cor ;
    bool zet_ok;
    double zet_sci ; // when dE from music is corrected by sci41
    bool zet_sci_ok;

#ifdef TPC42_PRESENT
    double zet_tpc;
    bool zet_tpc_ok;
#endif

    Taoq_calculator *aoq_calc ;
    Tmusic *mus ;

    // calibration

    // for final calculation of ZET we need some calibration factors
    vector<double> velocity_corr_factor ;
    double primary_zet ;

    // the velocity correction factors are hidden deaply inside algebraic formula, so they are not
    // very easy to work with. Therefore for poeople analysing the data
    // I introduce another, really final calibration which is applied in the very last moment
    // of calculations.
    // Note, if this factors do not exist in the frs_calibration.txt, this final calibration
    // is not performed, and we have the normal classical way of work
    vector<double>  zet_final;

    spectrum_1D *spec_zet;


    spectrum_2D
    *matr_zet_vs_aoq_sci_mw_corr ,
    * matr_zet_vs_aoq_sci_sci_corr;

#ifdef TPC41_PRESENT
    spectrum_2D  * matr_zet_vs_aoq_sci_tpc_corr;
    spectrum_1D *spec_zet_tpc;
    spectrum_2D  * matr_zet_tpc_vs_aoq_tic_mw_corr; // really it is not tic but tpc2*
#endif

#ifdef  TRACKING_IONISATION_CHAMBER_X
    spectrum_2D  * matr_zet_vs_aoq_tic_tpc_corr ;
#endif


    spectrum_2D  * matr_zet_vs_aoq_tic_sci_corr ;
    spectrum_2D  * matr_zet_vs_aoq_tic_mw_corr ;


public:
    //  constructor
    Tzet_calculator(string _name,
                    string name_of_aoq_calculator,
                    string name_of_music,
                    Tsilicon_pin_diode* sil_pin_diode_ptr = 0
                   );

    // read the calibration factors, gates
    void  make_preloop_action(ifstream &);

    // calculations-----------------------
    void analyse_current_event() ;

    // results for everybody who asks
    double give_zet()
    {
        if(calculations_already_done == false) analyse_current_event();
        return zet ;
    }
    double give_zet_sci()
    {
        if(calculations_already_done == false) analyse_current_event();
        return zet_sci ;
    }

    //  double give_aoq()    // charge state selector will ask here
    //    {
    //      if(calculations_already_done == false )
    //  analyse_current_event();
    //   return aoq_calc->give_aoq() ;
    //    }

    double is_zet_ok()
    {
        //cout << "in f.  is_zet_ok() " << zet_ok << endl ;
        if(!calculations_already_done) analyse_current_event();
        //cout << "result of f.  is_zet_ok() = " << zet_ok << endl ;
        return zet_ok ;
    }

    //   string give_name_of_z_vs_aoq_matrix()
    //   {
    //     return name_of_z_vs_aoq_matrix;
    //   }
    /** doppler will ask */
    inline double give_beta()
    {
        if(calculations_already_done == false) analyse_current_event();
        return aoq_calc->give_beta() ;
    }
    //---------------------
    double give_music_deposit_energy_non_corr()
    {
        if(calculations_already_done == false) analyse_current_event();
        return mus->give_deposit_energy_non_corr();
    }

private:
    void create_my_spectra();
};
/////////////////////////////////////////////////////////////////////////
#endif // __CINT__
#endif // _ZET_CALCULATOR_H_
