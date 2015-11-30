#include "Tzet_calculator.h"
#include "Tfrs.h"
#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h" // for verbose mode flag

double zet_min_range = 0 ; //15 ;
double zet_max_range = 100 ; //75;

//***********************************************************************
//  constructor
Tzet_calculator::Tzet_calculator(string name_,
                                 string name_of_aoq_calculator,
                                 string name_of_music,
                                 Tsilicon_pin_diode* sil_pin_diode_ptr
                                ) : Tfrs_element(name_)
#ifdef PIN_DIODE
    ,  sil_pin_diode(sil_pin_diode_ptr)
#endif
{
    aoq_calc =  dynamic_cast<Taoq_calculator*>(owner->
                address_of_device(name_of_aoq_calculator));
    mus = dynamic_cast<Tmusic*>(owner->
                                address_of_device(name_of_music));

    if(mus == NULL)
    {
        cout << "Constructor of "
             << name_
             << "Error while recognizing object " << name_of_music << endl;
        exit(4);
    }
    // pointers to spectra
    create_my_spectra();
    primary_zet = 0 ;

    named_pointer[name_of_this_element + "_Z_when_ok"]
    = Tnamed_pointer(&zet , &zet_ok, this) ;



    named_pointer[name_of_this_element + "__Z_(corr_using_sci41)_when_ok"]
    = Tnamed_pointer(&zet_sci , &zet_sci_ok, this) ;

#ifdef TPC41_PRESENT

    named_pointer[name_of_this_element + "__Z_(corr_using_tpc)_when_ok"]
    = Tnamed_pointer(&zet_tpc , &zet_tpc_ok, this) ;
#endif // TPC41_PRESENT


    named_pointer[name_of_this_element + "__velocity_corrector"]
    = Tnamed_pointer(&vel_cor , 0, this) ;



#ifdef PIN_DIODE

    named_pointer[name_of_this_element + "_Z_from_pin_diode_when_ok"]
    = Tnamed_pointer(&zet_from_pin_diode ,
                     &zet_from_pin_diode_ok, this) ;
#endif

}
//***********************************************************************
void Tzet_calculator::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "isotope_identification/" ;

    string name =  name_of_this_element  ;
    spec_zet = new spectrum_1D(name,
                               name,
                               1000, 1, 100,
                               folder,
#ifdef  MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE
                               "Note: This value can be affected by  atmospheric pressure correction of dE in MUSIC. To disable such correction, see frs_calibration.txt",
#else
                               "",
#endif
                               name_of_this_element + "_Z_when_ok");
    frs_spectra_ptr->push_back(spec_zet) ;


#ifdef TPC41_PRESENT

    name =  name_of_this_element + "__Z_corr_by_tpc" ;
    spec_zet_tpc = new spectrum_1D(name,
                                   name,
                                   1000, 1, 100,
                                   folder,
#ifdef  MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE
                                   "Note: This value can be affected by  atmospheric pressure correction of dE in MUSIC. To disable such correction, see frs_calibration.txt",
#else
                                   "",
#endif
                                   name_of_this_element + "__Z_(corr_using_tpc)_when_ok");
    frs_spectra_ptr->push_back(spec_zet_tpc) ;

#endif


    //-------------------------------------
    // matrices
    // ------------------------------------

    name =  name_of_this_element + "_vs_aoq_sci_mw_corr" ;


    matr_zet_vs_aoq_sci_mw_corr  =
        new spectrum_2D(name,
                        name,
                        500, aoq_min, aoq_max, // 1, 4,
                        500, zet_min_range, zet_max_range,    // 1, 100
                        folder, "",
                        string("Y:") + name_of_this_element +
                        "_Z_when_ok"
                        + ",    X:" + aoq_calc->give_name() +
                        "_A_over_Q_(sci_mw_corr)_when_ok"
                       );

    frs_spectra_ptr->push_back(matr_zet_vs_aoq_sci_mw_corr) ;
    //------------------



    //------------------
    name =    name_of_this_element + "_vs_aoq_sci_sci_corr" ;


    matr_zet_vs_aoq_sci_sci_corr  =
        new spectrum_2D(name,
                        name,
                        500, aoq_min, aoq_max, // 1, 4,
                        500, zet_min_range, zet_max_range,    // 1, 100
                        folder, "",
                        string("Y:") + name_of_this_element +
                        "_Z_when_ok"
                        + ", X:" + aoq_calc->give_name() +
                        "_A_over_Q_(sci_sci_corr)_when_ok"
                       );

    frs_spectra_ptr->push_back(matr_zet_vs_aoq_sci_sci_corr) ;


#ifdef TPC41_PRESENT

    name =    name_of_this_element + "_vs_aoq_sci_tpc_corr" ;
    matr_zet_vs_aoq_sci_tpc_corr
    =  new spectrum_2D(name,
                       name,
                       500, aoq_min, aoq_max, // 1, 4,
                       500, zet_min_range, zet_max_range,    // 1, 100
                       folder, "",
                       string("Y:") + name_of_this_element +
                       "_Z_when_ok"
                       + ", X:" + aoq_calc->give_name() +
                       "_A_over_Q_(sci_tcp_corr)_when_ok"
                      );

    frs_spectra_ptr->push_back(matr_zet_vs_aoq_sci_tpc_corr) ;

#ifdef  TRACKING_IONISATION_CHAMBER_X

    name =    name_of_this_element + "_vs_aoq_tic_tpc_corr" ;
    matr_zet_vs_aoq_tic_tpc_corr
    =  new spectrum_2D(name,
                       name,
                       500, aoq_min, aoq_max, // 1, 4,
                       500, zet_min_range, zet_max_range,    // 1, 100
                       folder, "",
                       string("Y:") + name_of_this_element +
                       "_Z_when_ok"
                       + ", X:" + aoq_calc->give_name() +
                       "_A_over_Q_(tic_tcp_corr)_when_ok"
                      );

    frs_spectra_ptr->push_back(matr_zet_vs_aoq_tic_tpc_corr) ;
#endif
#endif


    name =    name_of_this_element + "_vs_aoq_tic_sci_corr" ;
    matr_zet_vs_aoq_tic_sci_corr
    =  new spectrum_2D(name,
                       name,
                       500, aoq_min, aoq_max, // 1, 4,
                       500, zet_min_range, zet_max_range,    // 1, 100
                       folder, "",
                       string("Y:") + name_of_this_element +
                       "_Z_when_ok"
                       + ", X:" + aoq_calc->give_name() +
                       "_A_over_Q_(tic_sci_corr)_when_ok"
                      );

    frs_spectra_ptr->push_back(matr_zet_vs_aoq_tic_sci_corr) ;




    name =    name_of_this_element + "_vs_aoq_tic_mw_corr" ;
    matr_zet_vs_aoq_tic_mw_corr
    =  new spectrum_2D(name,
                       name,
                       500, aoq_min, aoq_max, // 1, 4,
                       500, zet_min_range, zet_max_range,    // 1, 100
                       folder, "",
                       string("Y:") + name_of_this_element +
                       "_Z_when_ok"
                       + ", X:" + aoq_calc->give_name() +
                       "_A_over_Q_(tic_mw_corr)_when_ok"
                      );

    frs_spectra_ptr->push_back(matr_zet_vs_aoq_tic_mw_corr) ;


#ifdef TPC41_PRESENT

    name =    name_of_this_element + "_corr_tpc__vs__aoq_tic_tpc_corr" ;
    matr_zet_tpc_vs_aoq_tic_mw_corr
    =  new spectrum_2D(name,

                       name,
                       500, aoq_min, aoq_max, // 1, 4,
                       500, zet_min_range, zet_max_range,    // 1, 100
                       folder, "",
                       string("Y:") + name_of_this_element +
                       "_Z_corr_tpc_when_ok"
                       + ", X:" + aoq_calc->give_name() +
                       "_A_over_Q_(tic_tpc_corr)_when_ok"
                      );

    frs_spectra_ptr->push_back(matr_zet_tpc_vs_aoq_tic_mw_corr) ;

#endif // ifdef TPC41_PRESENT



    //   name_of_z_vs_aoq_matrix = name ;
    // because banana system will ask for this name
    // and the telescope program will regognize its bananas for
    // this matrix

#ifdef PIN_DIODE

    name =   name_of_this_element + "_using_pin_diode_vs_aoq" ;
    matr_zet_pin_vs_aoq  =
        new spectrum_2D(name,
                        name,
                        500, aoq_min, aoq_max, // 1, 4,
                        500, zet_min_range, zet_max_range,    // 1, 100
                        folder, "", "we do not use this anymore...");

    frs_spectra_ptr->push_back(matr_zet_pin_vs_aoq) ;
#endif
    //----------------------
}
//**********************************************************************
void Tzet_calculator::analyse_current_event()
{

    //   cout << "\nanalyse_current_event()  for "
    //       <<  name_of_this_element << "" << endl ;

    //--------------------------------------------------------------------
    //   Determination of Z
    //--------------------------------------------------------------------

    zet = 0;
    vel_cor = 0;
    zet_ok =  false ;
    zet_sci =  0;
    zet_sci_ok = false;

    // for dE taken from the silicon pin diode
#ifdef PIN_DIODE

    zet_from_pin_diode = 0 ;
    zet_from_pin_diode_ok  = false ;
#endif
    //--------------


    double  beta_tof_sc21_41 = aoq_calc->give_beta() ;
    if(mus->was_ok()  && aoq_calc->is_tof_ok()
            &&
            ((beta_tof_sc21_41 > 0.05)
             &&
             (beta_tof_sc21_41 < 1 /* 0.70*/))
      )
    {
        //cout << "Mus was ok, " << endl ;

        // In the expresion below they are used callibration factors,
        // which should be read from the calibration file

        vel_cor =
            velocity_corr_factor[0]
            + velocity_corr_factor[1] * beta_tof_sc21_41
            + velocity_corr_factor[2] * pow(beta_tof_sc21_41, 2)
            + velocity_corr_factor[3] * pow(beta_tof_sc21_41, 3) ;


        if(vel_cor > 0)
        {
            //cout << "Vel_cor  was > 0 " << endl ;
            zet = primary_zet * sqrt(mus->give_music_deposit_energy_corr()
                                     /
                                     vel_cor);

            if(zet_final.size() >= 3) // different than zero
            {
                zet = (zet_final[2] * zet * zet) + (zet_final[1] * zet) + zet_final[0]  ;
            }

            // cout << "vel_cor = " << vel_cor << endl ;
            // cout << "mus dE_cor = "
            // << mus->give_music_deposit_energy_corr() ;
            // cout << "zet = " << zet << endl ;

            /*------
              cout << "\nCalculating  zet = primary_z * sqrt(music_de_corr / vel_cor)\n"
              << " Zet = " << zet
              << ", because primary_zet = " <<  primary_zet

              << ", music_de_corr=" <<  music_de_corr
              << ", (music_de_corr / vel_cor) =" << (music_de_corr / vel_cor)
              << ", \nsqrt(music_de_corr / vel_cor) = "
              <<  sqrt(music_de_corr / vel_cor)

              << endl ;
              ------*/

            // end of calculations ===================================
            //========================================================

            if((zet > 0) && (zet < 100))
            {
                zet_ok =  true ;
                spec_zet->manually_increment(zet);
                //          cout << name_of_this_element  << ", zet = " << zet << endl ;

                TIFJAnalysis::fxCaliEvent->zet = zet;

                //        matr_zet_vs_aoq->manually_increment(
                //                    aoq_calc->give_aoq(), zet);

                //          matr_zet_vs_aoq_mw_corr->
                //manually_increment(aoq_calc->give_aoq_mw_corr(), zet);
                matr_zet_vs_aoq_sci_mw_corr->
                manually_increment(aoq_calc->give_aoq_mw_corrected(), zet);
                matr_zet_vs_aoq_sci_sci_corr->
                manually_increment(aoq_calc->give_aoq_sci_corrected(), zet);


#ifdef TPC41_PRESENT

                matr_zet_vs_aoq_sci_tpc_corr->
                manually_increment(aoq_calc->give_aoq_sci_tpc_corrected(), zet);
#ifdef  TRACKING_IONISATION_CHAMBER_X

                matr_zet_vs_aoq_tic_tpc_corr->
                manually_increment(aoq_calc->give_aoq_tic_tpc_corrected(), zet);
#endif
#endif


                matr_zet_vs_aoq_tic_sci_corr->manually_increment(aoq_calc->give_aoq_tic_sci_corrected(), zet);
                ;
                matr_zet_vs_aoq_tic_mw_corr->manually_increment(aoq_calc->give_aoq_tic_mw_corrected(), zet);
                ;
                // matr_zet_vs_aoq_tic_tpc_corr->manually_increment(aoq_calc->give_aoq_tic_tpc_corrected(), zet); ;


                if(RisingAnalysis_ptr->is_verbose_on())
                {
                    cout  << "FRS: (" << name_of_this_element
                          << "), Final calculation of Z was successful "
                          << ":  Z = " << zet
                          << ",   A/Q ="
                          <<  aoq_calc->give_aoq()
                          << endl;
                }

            } // endof zet
        }// end if vel_cor
    }// end of music ok
    else
    {

        if(RisingAnalysis_ptr->is_verbose_on())
        {
            cout  << "FRS: final calculation of Z was not possible because: " ;
            if(!mus->was_ok())
                cout << "\n\t\tmusic calculation was NOT ok " ;
            if(!aoq_calc->is_tof_ok())
                cout << "\n\t\tTOF calculation was NOT ok " ;
            if(!mus->was_ok()  && !aoq_calc->is_tof_ok())
            {
                cout << "\n\tIt was probably SYNCHRO event...(?) " ;
            }
            cout << endl;
        }
    }

    //%%%%%%%%%%%%%%%
    if(mus->give_music_deposit_energy_sci_corr() > -999
            &&
            aoq_calc->is_tof_ok()
            &&
            ((beta_tof_sc21_41 > 0.05)
             &&
             (beta_tof_sc21_41 < 1 /* 0.70*/))
      )
    {
        //cout << "Mus was ok, " << endl ;

        // In the expresion below they are used callibration factors,
        // which should be read from the calibration file

        vel_cor =
            velocity_corr_factor[0]
            + velocity_corr_factor[1] * beta_tof_sc21_41
            + velocity_corr_factor[2] * pow(beta_tof_sc21_41, 2)
            + velocity_corr_factor[3] * pow(beta_tof_sc21_41, 3) ;


        if(vel_cor > 0)
        {
            zet_sci = primary_zet * sqrt(mus->give_music_deposit_energy_sci_corr()
                                         /
                                         vel_cor);

            if(zet_final.size() >= 3) // different than zero
            {
                zet_sci = (zet_final[2] * zet_sci * zet_sci) + (zet_final[1] * zet_sci) + zet_final[0]  ;

            }
        }
        if(zet_sci > 0 && zet_sci < 100)
            zet_sci_ok = true;
    }

    //%%%%%%%%%%%%%%%%%%%%%%%

#ifdef TPC42_PRESENT
    //%%%%%%%%%%%%%%%
    if(mus->give_music_deposit_energy_tpc_corr() > -999
            &&
            aoq_calc->is_tof_ok()
            &&
            ((beta_tof_sc21_41 > 0.05)
             &&
             (beta_tof_sc21_41 < 1 /* 0.70*/))
      )
    {
        //cout << "Mus was ok, " << endl ;

        // In the expresion below they are used callibration factors,
        // which should be read from the calibration file

        vel_cor =
            velocity_corr_factor[0]
            + velocity_corr_factor[1] * beta_tof_sc21_41
            + velocity_corr_factor[2] * pow(beta_tof_sc21_41, 2)
            + velocity_corr_factor[3] * pow(beta_tof_sc21_41, 3) ;


        if(vel_cor > 0)
        {
            zet_tpc = primary_zet * sqrt(mus->give_music_deposit_energy_tpc_corr()
                                         /
                                         vel_cor);

            if(zet_final.size() >= 3) // different than zero
            {
                zet_tpc = (zet_final[2] * zet_tpc * zet_tpc) + (zet_final[1] * zet_tpc) + zet_final[0]  ;

            }
        }
        if(zet_tpc > 0 && zet_tpc < 100)
        {
            zet_tpc_ok = true;
            spec_zet_tpc->manually_increment(zet_tpc);

            matr_zet_tpc_vs_aoq_tic_mw_corr->
            manually_increment(aoq_calc->give_aoq_tic_tpc_corrected(), zet_tpc);

        }
    }
#endif

    //%%%%%%%%%%%%%%%%%%%%%%%


    calculations_already_done = true ;

}
//**********************************************************************
// read the calibration factors, gates
//**********************************************************************
void  Tzet_calculator::make_preloop_action(ifstream & s)
{
    // no data in event class,
    cout << "Reading the calibration for "
         << name_of_this_element << endl ;

    velocity_corr_factor.clear();

# ifdef MUSIC42_USED

    string keyword = name_of_this_element + "_velocity_corr_factor";

    velocity_corr_factor.push_back(
        Tfile_helper::find_in_file(s, keyword + "[0]"));
    velocity_corr_factor.push_back(
        Tfile_helper::find_in_file(s, keyword + "[1]"));
    velocity_corr_factor.push_back(
        Tfile_helper::find_in_file(s, keyword + "[2]"));
    velocity_corr_factor.push_back(
        Tfile_helper::find_in_file(s, keyword + "[3]"));


#else
    // old fashioned way
    velocity_corr_factor.push_back(
        Tfile_helper::find_in_file(s,  name_of_this_element +
                                   "_velocity_corr_factor[0]")) ;
    velocity_corr_factor.push_back(
        Tfile_helper::find_in_file(s, name_of_this_element +
                                   "_velocity_corr_factor[1]")) ;
    velocity_corr_factor.push_back(
        Tfile_helper::find_in_file(s, name_of_this_element +
                                   "_velocity_corr_factor[2]")) ;
    velocity_corr_factor.push_back(
        Tfile_helper::find_in_file(s, name_of_this_element +
                                   "_velocity_corr_factor[3]")) ;

#endif

    primary_zet = Tfile_helper::find_in_file(s,  "primary_zet");




    // for better adjusting zet we introduce the final linear calibarion of zet
    try
    {

        double tmp =
            Tfile_helper::find_in_file(
                s,
                name_of_this_element + "_final_calibration_factors");
        zet_final.push_back(tmp);  // ofset

        s >> zjedz >> tmp;
        zet_final.push_back(tmp);  // slope

        s >> zjedz >> tmp;
        zet_final.push_back(tmp);  // quadratic

    }
    catch(Tno_keyword_exception &e)
    {
        // repair the stream
        s.clear(s.rdstate() & ~(ios::eofbit | ios::failbit));
        zet_final.clear();
    }

}

//************************************************************************
/** doppler will ask */

//******************************************************************
//******************************************************************
