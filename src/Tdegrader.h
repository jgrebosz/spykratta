#ifndef _DEGRADER_H_
#define _DEGRADER_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__


#include "Tfrs_element.h"
#include "Tscintillator.h"
#include "Tfrs_element.h"
//#include "tof_measurement.h"
#include "Taoq_calculator.h"
#include "Tzet_calculator.h"
#include "Tmusic.h"

#include "spectrum.h"
#include <string>
#include <vector>

#include "Tincrementer_donnor.h"


//////////////////////////////////////////////////////////////////////////////
class Tdegrader : public Tfrs_element, public Tincrementer_donnor
{

    double
    de_degrader_beta,     // Sumit says it was for testing
    de_degrader_gamma ;

    bool flag_degrader_ok ;


    double gamma, beta ; // local to the Pepe algorithm


    //double zet, vel_cor ;
    //bool   zet_ok ;


    Taoq_calculator  *aoq_object_for_brho;
    Taoq_calculator  *aoq_object_for_value;

    Tzet_calculator  *zet_object;  // to make one matrix

    Tscintillator *sc ;
    Tmusic *mus ;


    // source data in the unpacked event

    // none



    spectrum_2D * matr_degrader_gamma_vs_sc21_p ;
    //  spectrum_2D * matr_de_degbeta_vs_aoq ;
    spectrum_2D * matr_aoq_vs_de_deggamma ;
    spectrum_2D * matr_zet_vs_de_deggamma ;

//  spectrum_2D * matr_de_deggamma_vs_aoq ;

    spectrum_2D * matr_de_music_cor_vs_deggamma ;



public:
    //  constructor
    Tdegrader(string _name,
              string name_aoq_magnets, // <-- to take brho etc
              string name_aoq_value,  // <-- to take calculated aoq
              // (Samit assumes that aoq_sc21_41 == aoq_sc11_21)
              // for incrementing a matrix matr_degrader_gamma_vs_sc21_p ;
              string name_music,  // for another matrix
              string name_zet_object // also for one matrix
             );


    // calibration i calculation
    void analyse_current_event() ;


    // read the calibration factors, gates
    void  make_preloop_action(ifstream &);


    //--------------------------------------------------
    // this is made in the moment of the preloop
    void where_the_data()
    {
        // no data in the event
    }

    // some other devices would like to know the tracking of the ion
    double give_dE_gamma()
    {
        if(!calculations_already_done) analyse_current_event();
        return de_degrader_gamma ;
    }

private:
    void create_my_spectra();
    void check_legality_of_data_pointers() {} ;

};

/////////////////////////////////////////////////////////////////////////
#endif // __CINT__

#endif // _DEGRADER_H_
