#include "Tdegrader.h"


#include "Tfrs.h"
#include "Tnamed_pointer.h"

//extern int zet_min, zet_max;

//***********************************************************************
//  constructor
Tdegrader::Tdegrader(string name_,
                     string name_aoq_magnets, // <-- to take brho etc
                     string name_aoq_value,
                     string name_of_music,
                     string name_zet_object //
                    )
    :
    Tfrs_element(name_)
{
    aoq_object_for_brho = dynamic_cast<Taoq_calculator*>(owner->address_of_device(name_aoq_magnets));
    aoq_object_for_value = dynamic_cast<Taoq_calculator*>(owner->address_of_device(name_aoq_value));
    mus = dynamic_cast<Tmusic*>(owner->address_of_device(name_of_music));
    zet_object = dynamic_cast<Tzet_calculator*>(owner->address_of_device(name_zet_object));

    create_my_spectra();


    named_pointer[name_of_this_element + "_dE_beta_when_ok"]
    = Tnamed_pointer(&de_degrader_beta , &flag_degrader_ok, this) ;

    named_pointer[name_of_this_element + "_dE_gamma_when_ok"]
    = Tnamed_pointer(&de_degrader_gamma , &flag_degrader_ok, this) ;

    named_pointer[name_of_this_element + "_flag_degrader_ok"]
    = Tnamed_pointer(&flag_degrader_ok , 0, this) ;

}

//************************************************************************
void Tdegrader::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor
    string folder = name_of_this_element ;
    string name = name_of_this_element
                  + "_dE_gamma_vs_sc_21_position"  ;

    matr_degrader_gamma_vs_sc21_p =
        new spectrum_2D(name,
                        name,
                        200, -100, 100,
                        200, -100, 100,
                        folder, "",
                        string("X: sc21_position_when_ok") +
                        + "Y: " + name_of_this_element + "_dE_gamma_when_ok");

    frs_spectra_ptr->push_back(matr_degrader_gamma_vs_sc21_p) ;


    // ------------------------------
#ifdef NIGDY

    name = name_of_this_element + "_dE_beta_vs_aoq_nowe"  ;
    matr_de_degbeta_vs_aoq = new spectrum_2D(name,
            name,
            200, aoq_min, aoq_max,
            200, 0, 4000,
            folder) ;  // <-- from Sumit,
    //            but he mentiones something about making it dynamically
    //   because this depends of the type of the ion


    frs_spectra_ptr->push_back(matr_de_degbeta_vs_aoq) ;
#endif

    // marices

    // -------------------------------------
    name = name_of_this_element + "__aoq_vs_degrader_dE_gamma"  ;

    matr_aoq_vs_de_deggamma = new spectrum_2D(name,
            name,
            400, 0, 600,
            400, aoq_min, aoq_max,
            folder, "",
            string("X: ") +
            name_of_this_element + "_dE_gamma_when_ok" +
            " Y: " +
            aoq_object_for_value->give_name() +
            "_mw_corr, if ok. (If not ok, tries to use version _sci_corr)"  // aoq,
                                             ); // de_degrader_gamma);

    frs_spectra_ptr->push_back(matr_aoq_vs_de_deggamma) ;

    //-----------------

    name = name_of_this_element + "__zet_vs_degrader_dE_gamma"  ;

    matr_zet_vs_de_deggamma = new spectrum_2D(name,
            name,
            400, 0, 600,
            400, zet_min_range, zet_max_range,
            folder, "",
            string("X: ") +
            name_of_this_element + "_dE_gamma_when_ok" +
            " Y: " +
            zet_object->give_name()
                                             ); // de_degrader_gamma);

    frs_spectra_ptr->push_back(matr_zet_vs_de_deggamma) ;



    name = name_of_this_element + "_matr_de_music_corr_vs_dE_degamma"  ;
    matr_de_music_cor_vs_deggamma
    = new spectrum_2D(name,
                      name,
                      400, -200, 200,
                      400, 0, 4000,
                      folder, "",
                      string(" X: ")
                      + name_of_this_element + "_dE_gamma_when_ok" // de_degrader_gamma);
                      + ", Y: " + mus->give_name() + "_deposit_energy_corr_when_music_ok"
                     ) ;
    frs_spectra_ptr->push_back(matr_de_music_cor_vs_deggamma) ;
}
//**********************************************************************
void Tdegrader::analyse_current_event()
{
    //cout << "make_calculation for " <<  name_of_this_element << endl ;

    //cout << "before   Energy loss in Degrader" << endl ;
    flag_degrader_ok = false ;
    //---------------------------------------------------------------------
    //      Energy loss in Degrader from NP660(1999)87. (dE_degrader/q)
    //---------------------------------------------------------------------
    // This is an glgorithm of Pepe

    // in this algorithm we assume that aoq1 = aoq2
    // (aoq stands for A over Q)


    // degrader uses the "dummy" version of aoq, so only where is no multiwires
    double aoq = aoq_object_for_value->give_aoq() ;  // <-- real aoq (note: takes mw corr if possible,
    // if not - sci corrected, if not - zero

    double brho = aoq_object_for_brho->give_brho_sci() ;   // <--- dummy
    double gamma = 0 ;

    if(aoq > 0.005)
    {
        double fixed_factor = 931.5 ;
        gamma = sqrt(
                    1.0  +
                    (
                        0.10358125 *
                        pow(brho / aoq, 2)
                    )
                ) ;


        // this below was in the Sumit program
        //     double beta = brho / (aoq * 3.10713);
        //       beta = beta / sqrt(1 + beta * beta);


        // this formula was in the x$anl_id.ppl file and Chiara's manual

        // beta = sqrt(1 - 1/(pow(gamma, 2)));

        //-------------------------------------------
        // delta E in the degrader
        //-------------------------------------------



        de_degrader_beta = fixed_factor * (brho / beta - brho / beta);

        // this formula was in the x$anl_id.ppl file and Chiara's manual

        de_degrader_gamma =
            fixed_factor * aoq * (gamma -           // gamma before (11_21)
                                  aoq_object_for_value->give_gamma() // gamma 21_41
                                 );


        flag_degrader_ok = true ;

        double posit = aoq_object_for_brho->give_after_position();

        // marices
        matr_degrader_gamma_vs_sc21_p ->manually_increment(posit,
                de_degrader_gamma);


        //matr_de_degbeta_vs_aoq->manually_increment(aoq,  de_degrader_beta); ;    // not used
        matr_aoq_vs_de_deggamma->manually_increment(de_degrader_gamma, aoq);


        if(mus->was_ok())
        {

            matr_de_music_cor_vs_deggamma->manually_increment(de_degrader_gamma,
                    mus->give_music_deposit_energy_corr());
        }
        if(zet_object->is_zet_ok())
        {
            matr_zet_vs_de_deggamma ->manually_increment(de_degrader_gamma, zet_object->give_zet());
        }
    }
    else
    {
        gamma = 0;
        beta = 0;
        de_degrader_beta = 0;
        de_degrader_gamma = 0;
    } // endif

    calculations_already_done = true ;
}
//**************************************************************************
// read the calibration factors, gates
//**************************************************************************
void  Tdegrader::make_preloop_action(ifstream & s)
{
    // empty, no calibration, no gates

    // but we can check legality of pointersv
    check_legality_of_data_pointers(); // can throw exception
}
//***********************************************************************
