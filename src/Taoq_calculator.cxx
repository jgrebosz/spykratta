#include "Taoq_calculator.h"
#include "Tfrs.h"
#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h"

/*
NOTE: In case of the 100Sn experiment we were using so called TIC (time ionisation chamber)
to obtain position at s2. Only during this experiment.
Later, for Zsolt experiment in April 2008 we introduced the TPC (Time projection chamber) doing
the same job.
There is no need to change the code, because aoq_calculator does not work directly with them
but with the focal plane defined by these devices ("s2tic" or "s2tpc").
Only change are the names of the spectra and names of the incrementers
 To make this more flexible the following string has been defined
*/
std::string TICorTPC =
#ifdef  TPC22_PRESENT
    "tpc"
#else
    "tic"
#endif
    ;


// to magnify interesting region of aoq matrices
double aoq_min = 1 ; //1.5 ;
double aoq_max = 5;
//**********************************************************************
//  constructor
Taoq_calculator::Taoq_calculator(
    string name_of,
    string name_of_sci_before,
    string name_of_sci_after,
    string name_of_tof,          //  tof_measurement *_tof,
    vector<string> magn_names,
    string name_focal_plane_after,
    string name_focal_plane_tpc_after,
    string name_focal_plane_tic_before)
    :
    Tfrs_element(name_of),
    magnet_names(magn_names)
{
    foc_after = 0 ;
    sci_before =
        dynamic_cast<Tscintillator*>(owner->address_of_device(name_of_sci_before)) ;
    sci_after =
        dynamic_cast<Tscintillator*>(owner->address_of_device(name_of_sci_after)) ;
    tof =
        dynamic_cast<Ttof_measurement*>(owner->address_of_device(name_of_tof)) ;

    foc_after = dynamic_cast<Tfocal_plane*>(owner->address_of_device(name_focal_plane_after)) ;
    foc_tpc_after = dynamic_cast<Tfocal_plane*>(owner->address_of_device(name_focal_plane_tpc_after)) ;
    foc_tic_before = dynamic_cast<Tfocal_plane*>(owner->address_of_device(name_focal_plane_tic_before)) ;



    flag_this_is_real_aoq = (sci_before != NULL) ;
    create_my_spectra();
}
//************************************************************************
void Taoq_calculator::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    // sci_before -----  we do not create spectra for dummy !!!
    if(sci_before  && sci_after)   // it is possible that sci_before was given 0, then dummy aoq
    {
        string name =  name_of_this_element  ;
        string folder = "isotope_identification/"  ;
        string name_focus_point = sci_before -> give_rel_focus_name() ;
        // New way of making ------------

        name =  name_of_this_element + "_sci_sci_corr" ;
        spec_aoq_sci_sci_corr = new spectrum_1D(name,
                                                name,
                                                5000, aoq_min, aoq_max,
                                                folder,
                                                "",
                                                name_of_this_element + "_A_over_Q_(sci_sci_corr)_when_ok"   // aoq_sci_corrected
                                               );
        frs_spectra_ptr->push_back(spec_aoq_sci_sci_corr) ;

        name =  name_of_this_element + "_mw_corr" ;
        spec_aoq_sci_mw_corr = new spectrum_1D(name,
                                               name,
                                               5000, aoq_min, aoq_max,
                                               folder, "",
                                               name_of_this_element + "_A_over_Q_mw_corr_when_ok" // incrementer
                                              );
        frs_spectra_ptr->push_back(spec_aoq_sci_mw_corr) ;


        name =  name_of_this_element + "_" + TICorTPC + "_tpc_corr" ;
        spec_aoq_tic_tpc_corr = new spectrum_1D(name,
                                                name,
                                                5000, aoq_min, aoq_max,
                                                folder, "",
                                                name_of_this_element + "_A_over_Q_(" + TICorTPC + "_tpc_corr)_when_ok" // incrementer
                                               );
        frs_spectra_ptr->push_back(spec_aoq_tic_tpc_corr) ;


        name =  name_of_this_element + "_" + TICorTPC + "_mw_corr" ;
        spec_aoq_tic_mw_corr = new spectrum_1D(name,
                                               name,
                                               5000, aoq_min, aoq_max,
                                               folder, "",
                                               name_of_this_element + "_A_over_Q_(" + TICorTPC + "_mw_corr)_when_ok" // incrementer
                                              );
        frs_spectra_ptr->push_back(spec_aoq_tic_mw_corr) ;



        name =  name_of_this_element + "_sci_tpc_corr" ;
        spec_aoq_sci_tpc_corr = new spectrum_1D(name,
                                                name,
                                                5000, aoq_min, aoq_max,
                                                folder, "",
                                                name_of_this_element + "_A_over_Q_(sci_tpc_corr)_when_ok" // incrementer
                                               );
        frs_spectra_ptr->push_back(spec_aoq_sci_tpc_corr) ;




        name =  name_of_this_element + "_" + TICorTPC + "_sci_corr" ;
        spec_aoq_tic_sci_corr = new spectrum_1D(name,
                                                name,
                                                5000, aoq_min, aoq_max,
                                                folder, "",
                                                name_of_this_element + "_A_over_Q_(" + TICorTPC + "_sci_corr)_when_ok" // incrementer
                                               );
        frs_spectra_ptr->push_back(spec_aoq_tic_sci_corr) ;

        // Matrices --------------------
        name =  name_focus_point
                + "_vs_" + name_of_this_element + "_sci_corrected" ;

        matr_sc_before_vs_aoq_sci_corr
        = new spectrum_2D(name,
                          name,
                          200,  aoq_min, aoq_max,
                          200, -100., 100.,
                          folder, "",
                          string("X: ") + name_of_this_element + "_A_over_Q_sci_corr_when_ok"   // aoq_sci_corrected
                          + ", Y: " +  sci_before->give_name() + "_position_when_ok"   // sci_position_before
                         );
        frs_spectra_ptr->push_back(matr_sc_before_vs_aoq_sci_corr) ;
        //-------------------
        name =
            (
                foc_after ?
                foc_after->give_name()
                :
                (sci_after -> give_rel_focus_name())
            )
            + "_vs_" + name_of_this_element + "_sci_corrected" ;

        matr_sc_after_vs_aoq_sci_corr
        = new spectrum_2D(name,
                          name,
                          200,  aoq_min, aoq_max,
                          200, -100., 100.,
                          folder, "",
                          string("X: ") + name_of_this_element + "_A_over_Q_sci_corr_when_ok"   // aoq_sci_corrected,
                          + ", Y: " +  sci_after->give_name() + "_position_when_ok"   // sci_position_after
                         );
        frs_spectra_ptr->push_back(matr_sc_after_vs_aoq_sci_corr) ;
        //--------------------
        name =  name_focus_point
                + "_vs_" + name_of_this_element + "_mw_corrected" ;
        matr_sc_before_vs_aoq_mw_corr
        = new spectrum_2D(name,
                          name,
                          200,  aoq_min, aoq_max,
                          200, -100., 100.,
                          folder, "",
                          string("X: ") + name_of_this_element + "_A_over_Q_mw_corr_when_ok"   // aoq_sci_corrected
                          + ", Y: " +  sci_before->give_name() + "_position_when_ok"   // sci_position_before
                         );
        frs_spectra_ptr->push_back(matr_sc_before_vs_aoq_mw_corr) ;

        name =
            (
                foc_after ?
                foc_after->give_name()
                :
                (sci_after -> give_rel_focus_name())
            )
            + "_vs_" + name_of_this_element + "_mw_corrected" ;

        matr_sc_after_vs_aoq_mw_corr
        = new spectrum_2D(name,  name,
                          200,  aoq_min, aoq_max,
                          200, -100., 100.,
                          folder, "",
                          string("X: ") + name_of_this_element + "_A_over_Q_mw_corr_when_ok"  // aoq_mw_corrected
                          + ", Y: " +  sci_after->give_name() + "_position_when_ok"   // sci_position after
                         );
        frs_spectra_ptr->push_back(matr_sc_after_vs_aoq_mw_corr) ;


        named_pointer[name_of_this_element + "_A_over_Q_mw_corr_when_ok"]    //obsolete, for comaptibility
        = Tnamed_pointer(&aoq_sci_mw_corrected , &aoq_sci_mw_corrected_ok, this) ;

        named_pointer[name_of_this_element + "_A_over_Q_sci_corr_when_ok"]   // obsolete, for comaptibility
        = Tnamed_pointer(&aoq_sci_sci_corrected , &aoq_sci_sci_corrected_ok, this) ;

        //--------

        named_pointer[name_of_this_element + "_A_over_Q_(sci_mw_corr)_when_ok"]
        = Tnamed_pointer(&aoq_sci_mw_corrected , &aoq_sci_mw_corrected_ok, this) ;  // new style



        named_pointer[name_of_this_element + "_A_over_Q_(sci_sci_corr)_when_ok"]    // new style
        = Tnamed_pointer(&aoq_sci_sci_corrected , &aoq_sci_sci_corrected_ok, this) ;


        named_pointer[name_of_this_element + "_A_over_Q_(" + TICorTPC + "_tpc_corr)_when_ok"]
        = Tnamed_pointer(&aoq_tic_tpc_corrected , &aoq_tic_tpc_corrected_ok, this) ;

        named_pointer[name_of_this_element + "_A_over_Q_(sci_tpc_corr)_when_ok"]
        = Tnamed_pointer(&aoq_sci_tpc_corrected , &aoq_sci_tpc_corrected_ok, this) ;

        named_pointer[name_of_this_element + "_A_over_Q_(" + TICorTPC + "_mw_corr)_when_ok"]
        = Tnamed_pointer(&aoq_tic_mw_corrected , &aoq_tic_mw_corrected_ok, this) ;

        named_pointer[name_of_this_element + "_A_over_Q_(" + TICorTPC + "_sci_corr)_when_ok"]
        = Tnamed_pointer(&aoq_tic_sci_corrected , &aoq_tic_sci_corrected_ok, this) ;



        named_pointer[name_of_this_element + "_A_over_Q_(sci_sci_corr)_additionally_linearized_when_ok"]    // new style
        = Tnamed_pointer(&aoq_sci_sci_corrected_linearized , &aoq_sci_sci_corrected_ok, this) ;

        named_pointer[name_of_this_element + "_A_over_Q_(sci_mw_corr)_additionally_linearized_when_ok"]
        = Tnamed_pointer(&aoq_sci_mw_corrected_linearized , &aoq_sci_mw_corrected_ok, this) ;  // new style

        named_pointer[name_of_this_element + "_A_over_Q_(sci_tpc_corr)_additionally_linearized_when_ok"]
        = Tnamed_pointer(&aoq_sci_tpc_corrected_linearized , &aoq_sci_tpc_corrected_ok, this) ;

#ifdef  TPC22_PRESENT

        named_pointer[name_of_this_element + "_A_over_Q_(tpc_tpc_"
                      "_and_angle_at_s2tpc_corrected)_when_ok"]
        = Tnamed_pointer(&aoq_tic_tpc_and_angle_at_s2tpc_corrected ,
                         &aoq_tic_tpc_and_angle_at_s2tpc_corrected_ok, this) ;

#endif

    }  // end of if sci_before and sci_after exitst

    named_pointer[name_of_this_element + "_brho_sci_sci"]
    = Tnamed_pointer(&brho_sci_sci , 0, this) ;

    named_pointer[name_of_this_element + "_brho_sci_mw"]
    = Tnamed_pointer(&brho_sci_mw , 0, this) ;


}
//**********************************************************************
void Taoq_calculator::analyse_current_event()
{

    //    cout << "analyse_current_event()  for "
    //     <<  name_of_this_element << "" << endl ;


    aoq_sci_sci_corrected = 0 ;
    aoq_sci_sci_corrected_ok  = false;

    aoq_sci_mw_corrected_ok = false;
    aoq_sci_mw_corrected = 0;

    aoq_sci_tpc_corrected_ok = false;
    aoq_sci_tpc_corrected = 0;

    aoq_tic_tpc_corrected_ok = false;
    aoq_tic_tpc_corrected = 0;

    aoq_tic_mw_corrected_ok = false;
    aoq_tic_mw_corrected = 0;

    my_beta = 0;
    my_bega = 0 ;
    flag_tof_ok = false;

    aoq_sci_sci_corrected_linearized  = 0;
    aoq_sci_mw_corrected_linearized = 0;
    aoq_sci_tpc_corrected_linearized = 0;

#ifdef  TPC22_PRESENT

    aoq_tic_tpc_and_angle_at_s2tpc_corrected = 0 ;
    aoq_tic_tpc_and_angle_at_s2tpc_corrected_ok = false ;
#endif
    // ########################################
    // At first we calculate the SCI SCI version of Brho
    //########################################
    //=============================================
    // searching the position before
    // NOTE:: for dummy calculation (for degrader) we do not have the sci before - then we use 0
    double sci_position_before = 0;
    bool sci_position_before_is_possible = false ;

    if(flag_this_is_real_aoq)    // this flag is true when: (sci_before) ;
    {
        if(sci_before)  // ------------- sc21 is used -----------------
        {
            if(sci_before->is_position_ok())
            {
                sci_position_before = sci_before->give_position() ;
                sci_position_before_is_possible = true;
            }
        }
    }  // end of this is real

    // scintillator after must always exist
    double sci_position_after =  0 ;
    bool sci_position_after_possible = sci_after->is_position_ok();
    if(sci_position_after_possible)
    {
        sci_position_after    = sci_after->give_position() ;

    }// if not fired, we have zero


    brho_sci_sci = brho_zero
                   *
                   (1 -
                    (
                        (
                            sci_position_after - magnification * sci_position_before
                        )  / (1000 * dispersion)
                    )
                   ) ;



    // ########################################
    // We calculate the SCI MULTIWIRE  version of Brho
    //########################################

    // now position taken from the multiwire ===================================

    // note: for the correction we need the focus point
    //       but the scintillator has it related  focus point
    // so we can get it indirectly
    double mw_position_after = 0;
    bool mw_position_after_possible = true ;
    if(foc_after == 0)  // if this is  normal  situation, we ask scintillator which focal plane he uses, and focal plane gives MW position
    {
        mw_position_after =
            sci_after->give_position_extrapolated_by_focal_plane(&mw_position_after_possible) ;
    }
    else // if  we want to  use the focal plane, which we directly specified  [ for example] hybrid (mw+sci)
    {
        mw_position_after_possible =  foc_after->was_x_ok();
        mw_position_after =  foc_after->give_x() ;
    }


    if(mw_position_after_possible)
    {
        brho_sci_mw =  brho_zero
                       *
                       (1 -
                        (
                            (
                                mw_position_after - magnification * sci_position_before
                            )
                            /
                            (1000 * dispersion)
                        )
                       ) ;

    }
    else
    {
        brho_sci_mw = -999999;
    }
    // brho_mw = rho_mw * mean_field ;



    // ########################################
    // we calculate the TIC_TPC version of Brho
    //########################################
    double tic_position_before = 0;
    bool tic_position_before_is_possible = false ;

    bool tpc_position_after_possible =  false;
    double tpc_position_after =  -999;
    if(foc_tpc_after)
    {
        // now position taken from the ... ===================================

        tpc_position_after_possible =  foc_tpc_after->was_x_ok();
        tpc_position_after =  foc_tpc_after->give_x() ;

        if(foc_tic_before)
            if(foc_tic_before->was_x_ok())
            {
                tic_position_before = foc_tic_before->give_x() ;
                tic_position_before_is_possible = true;
            }

        if(tpc_position_after_possible && tic_position_before_is_possible)
        {
            brho_tic_tpc =  brho_zero
                            *
                            (1 -
                             (
                                 (
                                     tpc_position_after - magnification * tic_position_before
                                 )
                                 /
                                 (1000 * dispersion)
                             )
                            ) ;
        }
        else
        {
            brho_tic_tpc = -999999;
        }
    }
    //##################



    // ########################################
    // Now we calculate the SCI_TPC version of Brho
    //########################################
    // all positons were already obtained befroe, so we can just use it
    if(foc_tpc_after)
    {

        if(tpc_position_after_possible && sci_position_before_is_possible)
        {
            brho_sci_tpc =  brho_zero
                            *
                            (1 -
                             (
                                 (
                                     tpc_position_after - magnification * sci_position_before
                                 )
                                 /
                                 (1000 * dispersion)
                             )
                            ) ;
        }
        else
        {
            brho_sci_tpc = -999999;
        }
    }
    //##################


    // ########################################
    // Now we calculate the TIC SCI version of Brho
    //########################################
    // all positons were already obtained befroe, so we can just use it

    if(sci_position_after_possible && tic_position_before_is_possible)
    {

        brho_tic_sci =  brho_zero
                        *
                        (1 -
                         (
                             (
                                 sci_position_after - magnification * tic_position_before
                             )
                             /
                             (1000 * dispersion)
                         )
                        ) ;
    }
    else
    {
        brho_tic_sci = -999999;
    }

    //##################


    // ########################################
    // Now we calculate the TIC MW version of Brho
    //########################################
    // all positons were already obtained befroe, so we can just use it

    if(mw_position_after_possible && tic_position_before_is_possible)
    {
        brho_tic_mw =  brho_zero
                       *
                       (1 -
                        (
                            (
                                mw_position_after - magnification * tic_position_before
                            )
                            /
                            (1000 * dispersion)
                        )
                       ) ;
    }
    else
    {
        brho_tic_mw = -999999;
    }

    //############### end of Brho calculations ####################



    //======================================================================
    // this above is necessery for the dummy and for real aoq calculator
    // the dummy is used only for degrader - which needs 'brho'
    //======================================================================

    if(flag_this_is_real_aoq)
    {

        //  cout << "analyse_current_event()  for "
        //      <<  name_of_this_element << "" << endl ;

        if(tof)   // if tof object exists
        {
            if(tof->is_tof_ok())
            {
                my_beta = tof->give_beta();
                my_bega = tof->give_beta_gamma();
                my_gamma = tof->give_gamma();

                flag_tof_ok = true;
            }

            if(flag_tof_ok && my_beta > 0.05 && my_beta < 1)
            {

                //====================================
                // SCI SCI correction (not so good!)==============
                //====================================
                if(sci_position_before_is_possible && sci_position_after_possible)   // before must be always
                {

                    //             cout << "\t\t\tin aoq SCI position after = " << sci_position_after << endl;

                    aoq_sci_sci_corrected = brho_sci_sci / (my_bega * 3.10713) ;
                    aoq_sci_sci_corrected = aoq_sci_sci_corrected - (corr_slope_s4 * sci_position_after) - corr_offset_s4 ;
                    aoq_sci_sci_corrected_ok = true ;

                    spec_aoq_sci_sci_corr->manually_increment(aoq_sci_sci_corrected) ;
                    matr_sc_before_vs_aoq_sci_corr->manually_increment(aoq_sci_sci_corrected, sci_position_before) ;
                    matr_sc_after_vs_aoq_sci_corr->manually_increment(aoq_sci_sci_corrected, sci_position_after) ;

                    for(uint i = 0 ; i < linearization_factors.size(); i++)
                    {
                        if(!linearization_factors[i])
                            continue;
                        aoq_sci_sci_corrected_linearized += linearization_factors[i] * pow(aoq_sci_sci_corrected, double(i)) ;
                    }
                }
                //====================================
                // SCI MW correction (very good) ================
                // note: for the correction we need the focus point
                //       but the scintillator has it related  focus point
                // so we can get it indirectly
                //====================================
                if(sci_position_before_is_possible && mw_position_after_possible)
                {
                    //                     cout << "\t\t\tin aoq MW_position_after = " << mw_position_after << endl;

                    aoq_sci_mw_corrected = brho_sci_mw / (my_bega * 3.10713) ;
                    aoq_sci_mw_corrected = aoq_sci_mw_corrected - (corr_slope_s4 * mw_position_after) - corr_offset_s4 ;
                    aoq_sci_mw_corrected_ok = true ;

                    spec_aoq_sci_mw_corr->manually_increment(aoq_sci_mw_corrected) ;
                    matr_sc_before_vs_aoq_mw_corr->manually_increment(aoq_sci_mw_corrected, sci_position_before) ;
                    matr_sc_after_vs_aoq_mw_corr->manually_increment(aoq_sci_mw_corrected, mw_position_after) ;

                    //sss ;
                    for(uint i = 0 ; i < linearization_factors.size(); i++)
                    {
                        if(!linearization_factors[i])
                            continue;
                        aoq_sci_mw_corrected_linearized += linearization_factors[i] * pow(aoq_sci_mw_corrected, double(i)) ;
                    }

                    //%%%%%%%%%%%%%%%%%%%
                    //     putting into the n-tuple
                    TIFJAnalysis::fxCaliEvent->aoq_sci_corr = aoq_sci_sci_corrected;
                    TIFJAnalysis::fxCaliEvent->aoq_mw_corr = aoq_sci_mw_corrected;
                    //%%%%%%%%%%%%%%%%%%
                }

#ifdef NIGDY
                by mistake it was twice(second time is below)
                //====================================
                // TIC TPC correction (very good) ================
                //====================================

                if(tic_position_before_is_possible && tpc_position_after_possible)
                {
                    aoq_tic_tpc_corrected = brho_tic_tpc / (my_bega * 3.10713) ;
                    aoq_tic_tpc_corrected = aoq_tic_tpc_corrected - (corr_slope_s4 * tpc_position_after) - corr_offset_s4 ;
                    aoq_tic_tpc_corrected_ok = true ;
                    spec_aoq_tic_tpc_corr->manually_increment(aoq_tic_tpc_corrected) ;

                }
#endif

                //====================================
                // SCI/TPC correction (very good) ================
                //====================================
                if(sci_position_before_is_possible &&  tpc_position_after_possible)
                {
                    aoq_sci_tpc_corrected = brho_sci_tpc / (my_bega * 3.10713) ;
                    aoq_sci_tpc_corrected = aoq_sci_tpc_corrected - (corr_slope_s4 * tpc_position_after) - corr_offset_s4 ;
                    aoq_sci_tpc_corrected_ok = true ;
                    spec_aoq_sci_tpc_corr->manually_increment(aoq_sci_tpc_corrected) ;


                    for(uint i = 0 ; i < linearization_factors.size(); i++)
                    {
                        if(!linearization_factors[i])
                            continue;
                        aoq_sci_tpc_corrected_linearized += linearization_factors[i] * pow(aoq_sci_tpc_corrected, double(i)) ;
                    }

                }

                //====================================
                // TIC_TPC correction (very good) ================
                //====================================
                if(tic_position_before_is_possible &&  tpc_position_after_possible)
                {
                    aoq_tic_tpc_corrected = brho_tic_tpc / (my_bega * 3.10713) ;
                    aoq_tic_tpc_corrected = aoq_tic_tpc_corrected - (corr_slope_s4 * tpc_position_after) - corr_offset_s4 ;
                    aoq_tic_tpc_corrected_ok = true ;
                    spec_aoq_tic_tpc_corr->manually_increment(aoq_tic_tpc_corrected) ;

#ifdef  TPC22_PRESENT

                    if(ang_x_corr_factor.size() == 2)
                    {

                        aoq_tic_tpc_and_angle_at_s2tpc_corrected =
                            //                             aoq_tic_tpc_corrected ;   <----- fake
                            aoq_tic_tpc_corrected *
                            (ang_x_corr_factor[1] *  foc_tic_before->give_angle_x()
                             +
                             ang_x_corr_factor[0]);
                        aoq_tic_tpc_and_angle_at_s2tpc_corrected_ok = true;

                    }
#endif

                }

                //====================================
                // TIC_SCI correction  ================
                //====================================
                if(tic_position_before_is_possible &&  sci_position_after_possible)
                {
                    aoq_tic_sci_corrected = brho_tic_sci / (my_bega * 3.10713) ;
                    aoq_tic_sci_corrected = aoq_tic_sci_corrected - (corr_slope_s4 * sci_position_after) - corr_offset_s4 ;
                    aoq_tic_sci_corrected_ok = true ;
                    spec_aoq_tic_sci_corr->manually_increment(aoq_tic_sci_corrected) ;

                }
                //====================================
                // TIC_MW correction  ================
                //====================================
                if(tic_position_before_is_possible &&  mw_position_after_possible)
                {
                    aoq_tic_mw_corrected = brho_tic_mw / (my_bega * 3.10713) ;
                    aoq_tic_mw_corrected = aoq_tic_mw_corrected - (corr_slope_s4 * mw_position_after) - corr_offset_s4 ;
                    aoq_tic_mw_corrected_ok = true ;
                    spec_aoq_tic_mw_corr->manually_increment(aoq_tic_mw_corrected) ;
                }


            } // endif beta
        } // endif sci and tof exists
    } // if this is real

    calculations_already_done = true ;
    // cout << name_of_this_element  << " -> aoq= "  << give_aoq() << endl;
}
//**************************************************************************
// read the calibration factors, gates
//**************************************************************************
void  Taoq_calculator::make_preloop_action(ifstream & s)
{

    // no data in event class,

    //  use_mw_pos_instead_sci_pos_at_s2  =  1;
    //  use_mw_pos_instead_sci_pos_at_s4  =  1;

    cout << "Reading the calibration for " << name_of_this_element << endl ;

    //  bool flag_finger_is_used = false;
    //
    //  if(this_is_real_aoq && sci_finger_before)
    //  {
    //       flag_finger_is_used = true;
    //  }

    //  if(sci_before && sci_after && tof)
    if(flag_this_is_real_aoq)
    {
        // this part will not be called for 'dummy'



        //----------
        try // this Nami corrections may not apear in the file
        {
            corr_offset_s2 = Tfile_helper::find_in_file(s, name_of_this_element
                             + "_corr_offset_s2");
            corr_slope_s2  = Tfile_helper::find_in_file(s, name_of_this_element
                             + "_corr_slope_s2");
            corr_offset_s4 = Tfile_helper::find_in_file(s, name_of_this_element
                             + "_corr_offset_s4");
            corr_slope_s4  = Tfile_helper::find_in_file(s, name_of_this_element
                             + "_corr_slope_s4");
        }
        catch(...)
        {
            s.clear() ;
            corr_offset_s2 =
                corr_slope_s2  =
                    corr_offset_s4 =
                        corr_slope_s4 = 0;
        }
    }

    // data about MAGNETS. Their names are in the vector
    magn_field.clear();
    magn_radius.clear();

    for(unsigned int i = 0 ; i <  magnet_names.size() ; i++)
    {
        // create the names
        magn_field.push_back(Tfile_helper::find_in_file(s,
                             magnet_names[i]
                             + "_field"));
        magn_radius.push_back(Tfile_helper::find_in_file(s,
                              magnet_names[i]
                              + "_radius"));
        //   cout << "Magnet nr " << i << endl;

    }

    // old, previous way - was OK
    effective_radius = 0 ;
    mean_field = 0 ;
    brho_zero = 0 ;

    unsigned int i = 0 ;
    for(i = 0 ; i < magn_radius.size() ; i++)
    {
        effective_radius += magn_radius[i] ;
        mean_field += magn_field[i] ;
        brho_zero += (magn_radius[i] * magn_field[i]) ;
    }

    if(i > 0)
    {
        effective_radius /= i ;  // arythmetic mean
        mean_field /= i ;        // also
        brho_zero /= i ;
    }
    // sum of all the magnet names ---------------------
    string all_magnets ;
    for(unsigned int i = 0 ; i <  magnet_names.size() ; i++)
    {
        all_magnets += string(magnet_names[i] + "_") ;
    }

    dispersion = Tfile_helper::find_in_file(s,
                                            all_magnets
                                            + "dispersion") ;
    magnification = Tfile_helper::find_in_file(s,
                    all_magnets
                    + "magnification") ;

    //============================
    // for the final linearization (Lucia asked for this)
    try
    {
        linearization_factors.clear();
        string slowo = name_of_this_element + "_linearization_factors";

        // first reading the energy calibration
        Tfile_helper::spot_in_file(s, slowo);
        for(int i = 0; i < 5 ; i++) // max
        {
            // otherwise we read the data
            double value;
            s  >> value;
            if(!s)
            {
                Treading_value_exception capsule;
                capsule.message =
                    "I found keyword '" + slowo
                    + "' but error was in reading its value.";
                throw capsule;
            }
            // if ok
            linearization_factors.push_back(value);
        }
    }
    catch(...)
    {
        // this is not obligatory, so if it does not exist, no problem
        // the stream has to be repaired
        s.clear(s.rdstate() & ~(ios::eofbit | ios::failbit));
    }


#ifdef  TPC22_PRESENT
    // newly introduced calibration of the angle
    // sometimes people need this to correct AoQ by the angle
    ang_x_corr_factor.clear();

    // at first for X
    try
    {
        double tmp = Tfile_helper::find_in_file(s, name_of_this_element
                                                + "_angle_x_at_s2tpc__corr_factors");

        ang_x_corr_factor.push_back(tmp);  // offset
        s >> zjedz >> tmp;
        ang_x_corr_factor.push_back(tmp);  // slope

    }
    catch(Tno_keyword_exception &e)
    {
        ang_x_corr_factor.clear();
        // to be able to continue the reading we need to repair
        Tfile_helper::repair_the_stream(s);

    }
#endif


}
//******************************************************************
