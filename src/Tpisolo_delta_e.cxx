#include "Tpisolo_delta_e.h"
#include "Tnamed_pointer.h"
#include "Tfrs.h"

#include "Tpisolo_tof.h"
#include "Tpisolo_si_final.h"

#if CURRENT_EXPERIMENT_TYPE == PISOLO2_EXPERIMENT
//***********************************************************************
//  constructor
Tpisolo_delta_e::Tpisolo_delta_e(string _name,
                                 int (TIFJEvent::*delta_E_ptr)[HOW_MANY_DELTA_E],
                                 int  TIFJEvent::*E_res,
                                 string name_of_tof,
                                 string name_of_final_si
                                ) : Tfrs_element(_name),
    deltaE_event(delta_E_ptr), dE_res_event(E_res)
{

    tof_ptr =   dynamic_cast<Tpisolo_tof*>(owner->address_of_device(name_of_tof)) ;
    if(!tof_ptr)
    {
        cout << "Not known object " << name_of_tof << endl;
        exit(8);
    }

    e_final_ptr  = dynamic_cast<Tpisolo_si_final*>(owner->address_of_device(name_of_final_si)) ;
    if(!e_final_ptr)
    {
        cout << "Not known object " << name_of_final_si << endl;
        exit(9);
    }

    create_my_spectra();

    named_pointer[name_of_this_element + "_dE_raw"]
    = Tnamed_pointer(&dE_raw, 0, this) ;

    named_pointer[name_of_this_element + "_dE_cal"]
    = Tnamed_pointer(&dE_cal ,   &dE_ok,  this) ;


    named_pointer[name_of_this_element + "_Esum_cal"]
    = Tnamed_pointer(&Esum_cal, 0, this) ;
    named_pointer[name_of_this_element + "_mass"]
    = Tnamed_pointer(&mass, 0, this) ;
}
//************************************************************************
//************************************************************************
void Tpisolo_delta_e::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "pisolo_deltaE/" + name_of_this_element ;

    //-----------
    for(int i = 0 ; i < HOW_MANY_DELTA_E ; i++)
    {
        string name = name_of_this_element  + "__deltaE"   ;
        name +=  char('0' + (i + 1))   ;

        string name_r = name  + "_raw" ;
        string name_c = name  + "_cal" ;
        spec_dE_raw[i] = new spectrum_1D(name_r, name_r,
                                         4095, 1, 4096,
                                         folder, "", name_r);
        frs_spectra_ptr->push_back(spec_dE_raw[i]) ;

        //-----------
        spec_dE_cal[i] = new spectrum_1D(name_c, name_c,
                                         4095, 1, 4096,
                                         folder, "", name_c);
        frs_spectra_ptr->push_back(spec_dE_cal[i]) ;

    }
    //-----------
    //-----------
    string name = name_of_this_element + "__Esum_raw"  ;
    spec_Esum_raw = new spectrum_1D(name, name,
                                    4095, 1, 4096,
                                    folder,
                                    "Esum = dE_raw +  Si_final__EN_raw",
                                    name_of_this_element + "_Esum_raw");
    frs_spectra_ptr->push_back(spec_Esum_raw) ;

    name = name_of_this_element + "__Esum_cal"  ;
    spec_Esum_cal = new spectrum_1D(name, name,
                                    8192, 1, 8191,
                                    folder,
                                    "Esum_cal = dE_cal  +   Si_final__EN_cal",
                                    name_of_this_element + "_Esum_cal");
    frs_spectra_ptr->push_back(spec_Esum_cal) ;

    //-----------
    name = name_of_this_element + "__dE_raw"  ;
    spec_dE_res_raw = new spectrum_1D(name, name,
                                      4095, 1, 4096,
                                      folder, "",
                                      name_of_this_element + "_dE_raw");
    frs_spectra_ptr->push_back(spec_dE_res_raw) ;
    //-----------
    name = name_of_this_element + "__dE_cal"  ;
    spec_dE_res_cal = new spectrum_1D(name, name,
                                      4095, 1, 4096,
                                      folder, "",
                                      name_of_this_element + "_dE_cal");
    frs_spectra_ptr->push_back(spec_dE_res_cal) ;

    name = name_of_this_element + "__mass"  ;
    spec_mass = new spectrum_1D(name, name,
                                1000, 1, 1000000,
                                folder, "mass  =  a * Esum_cal * (b - ToF1)^2",
                                name_of_this_element + "_mass");
    frs_spectra_ptr->push_back(spec_mass) ;
    //-------------------------------------- 2D ----

    name = name_of_this_element + "__matrix_dE_vs_Esum" ;
    spec_dE_vs_Esum = new spectrum_2D(name,
                                      name,
                                      500, 1, 8000,
                                      500, 1, 5000,
                                      folder, "note?",
                                      string("as X:") + name_of_this_element + "_Esum_cal" +
                                      ", as Y: " + name_of_this_element + "_dE_cal"
                                     );
    frs_spectra_ptr->push_back(spec_dE_vs_Esum) ;

    name = name_of_this_element + "__matrix_tof1_vs_Esum" ;
    spec_tof1_vs_Esum = new spectrum_2D(name,
                                        name,
                                        500, 1, 8000,
                                        500, 1, 6000,
                                        folder, "",
                                        string("as X:") + name_of_this_element + "_Esum_cal" +
                                        ", as Y: " + "pisolo_tof__tof1_raw"
                                       );
    frs_spectra_ptr->push_back(spec_tof1_vs_Esum) ;

    //---------------------
    name = name_of_this_element + "__matrix_tof1_vs_dE" ;
    spec_tof1_vs_dE = new spectrum_2D(name,
                                      name,
                                      500, 1, 6000,
                                      500, 1, 6000,
                                      folder, "",
                                      string("as X:") + name_of_this_element + "_dE_cal" +
                                      ", as Y: " + "pisolo_tof__tof1_cal"
                                     );
    frs_spectra_ptr->push_back(spec_tof1_vs_dE) ;

}
//**********************************************************************
void Tpisolo_delta_e::analyse_current_event()
{
    // cout << "analyse_current_event()  for "
    //         <<  name_of_this_element
    //          << endl ;



    dE_ok = false;
    for(int i = 0 ; i < HOW_MANY_DELTA_E ; i++)
    {

        deltaE_raw[i] =      // delta E, delta E1, delta E2
            (event_unpacked->*deltaE_event)[i];

        if(deltaE_raw[i])
        {
            spec_dE_raw[i] -> manually_increment(deltaE_raw[i]) ;
            deltaE_cal[i] = ((deltaE_raw[i] + randomizer::randomek()) * slope[i]) + offset[i] ;

            spec_dE_cal[i] ->manually_increment(deltaE_cal[i]) ;
        }
    }

    dE_raw =   event_unpacked->*dE_res_event;
    spec_dE_res_raw ->manually_increment(dE_raw) ;

    if(dE_raw)
    {
        dE_cal =
            ((dE_raw + randomizer::randomek()) * dE_slope) + dE_offset;
        dE_ok = true;
        spec_dE_res_cal ->manually_increment(dE_cal) ;
    }
    else dE_cal = 0 ;



    int EN = e_final_ptr->give_e_raw() ;
    Esum_raw = dE_raw + EN;
    spec_Esum_raw->manually_increment(Esum_raw);

    Esum_cal = dE_cal + (e_final_ptr->give_e_cal());
    spec_Esum_cal->manually_increment(Esum_cal);


    //   spec_dE_vs_E_raw->manually_increment(Er,   deltaE_raw[0] );
    spec_dE_vs_Esum->manually_increment(Esum_cal,   dE_cal);
    //   spec_dE_vs_E_cal->manually_increment(Esum_cal, deltaE_cal[0] );

    double  tof1_cal = tof_ptr->give_tof1_cal() ;
    spec_tof1_vs_Esum->manually_increment(Esum_cal,  tof1_cal);
    spec_tof1_vs_dE->manually_increment(dE_cal,   tof1_cal);

    mass =   mass_a_factor * Esum_cal * pow((mass_b_factor - tof1_cal), 2) ;  // mass = a * Esum * (b-ToF1) ^2
    spec_mass->manually_increment(mass);

    calculations_already_done = true ;
}
//**************************************************************************
void  Tpisolo_delta_e::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;
    // in this file we look for a string

    for(int i = 0 ; i < HOW_MANY_DELTA_E ; i++)
    {
        string name = name_of_this_element  + "_deltaE" ;
        name +=  char('0' + (i + 1))   ;

        slope[i]  = Tfile_helper::find_in_file(s,  name  + "_slope");
        offset[i] = Tfile_helper::find_in_file(s,  name  + "_offset");
    }
    dE_slope  = Tfile_helper::find_in_file(s,  name_of_this_element + "_dE_slope");
    dE_offset = Tfile_helper::find_in_file(s,  name_of_this_element  + "_dE_offset");

    mass_a_factor  = Tfile_helper::find_in_file(s,  name_of_this_element + "__mass_a_factor");
    mass_b_factor = Tfile_helper::find_in_file(s,  name_of_this_element  + "__mass_b_factor");

}
//***************************************************************
#endif  // CURRENT_EXPERIMENT_TYPE == PISOLO2_EXPERIMENT


