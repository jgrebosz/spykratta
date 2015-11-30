#include "Tmusic_8anodes.h"

#include "Tfrs.h"
#include "Tnamed_pointer.h"
//***********************************************************************
//  constructor
Tmusic_8anodes::Tmusic_8anodes(string _name,
                               int (TIFJEvent::*time_array_ptr)[EIGHT_ANODES],
                               int (TIFJEvent::*energy_array_ptr)[EIGHT_ANODES],
                               string name_of_related_focus_point,
                               string name_of_nearest_scintillator
#ifdef TPC42_PRESENT
                               ,   string  name_of_nearest_tpc_focal_plane
#endif
                              )
    : Tmusic(_name, name_of_related_focus_point
#ifdef TPC42_PRESENT
             , name_of_nearest_tpc_focal_plane
#endif
            ),
    time_array(time_array_ptr),
    energy_array(energy_array_ptr)
{

    // could be as argument, but let it be easier now
    const double di[EIGHT_ANODES] =
    {
        30.235, 80.235,
        130.235, 180.235,
        230.235, 280.235,
        330.235, 380.235
    };
    // what is it ?
    //const double mw_const[EIGHT_ANODES] = { 21, 20, 25, 25 } ;

    nearest_scintillator =
        dynamic_cast<Tscintillator*>(owner->address_of_device(name_of_nearest_scintillator)) ;



    for(int i = 0 ; i < EIGHT_ANODES ; i++)
    {
        dist_music_anode[i] = di[i] ;
        // multiwire_constant[i] = mw_const[i];
    }

    create_my_spectra();

    named_pointer[name_of_this_element + "_x"] = Tnamed_pointer(&music_x, 0, this) ;

    named_pointer[name_of_this_element + "_deposit_energy"]
    = Tnamed_pointer(&deposit_energy, 0, this) ;

    named_pointer[name_of_this_element + "_deposit_energy_corr_when_music_ok"]
    = Tnamed_pointer(&deposit_energy_corr, &music_ok, this) ;

    named_pointer[name_of_this_element + "_drift_time_ok"]
    = Tnamed_pointer(&drift_time_ok, 0, this) ;

    //   named_pointer[name_of_this_element+"_nr_of_correction_type"]
    //     = Tnamed_pointer(&how_to_correct, 0, this) ;

    named_pointer[name_of_this_element + "_x_mean"] =
        Tnamed_pointer(&music_x_mean, 0, this) ;

    //   named_pointer[name_of_this_element+"_deposit_energy_corr_when_music_ok"]
    //     = Tnamed_pointer(&deposit_energy_corr, &music_ok, this) ;

    named_pointer[name_of_this_element + "_deposit_energy_when_music_ok"]
    = Tnamed_pointer(&deposit_energy, &music_ok, this) ;


    named_pointer[name_of_this_element + "_deposit_energy_sci_corr_when_ok"]
    = Tnamed_pointer(&deposit_energy_corr_sci, &music_sci_corr_ok, this) ;

    //-----------------------------------------
    // to make ratio spectra - we use this trick, that we send this boolean
    // int he data_usable word
    named_pointer[name_of_this_element + "_ok_for_ratio_purposes"]
    = Tnamed_pointer(&music_ok, &music_ok, this) ;

}
//************************************************************************
void Tmusic_8anodes::create_my_spectra()
{
    // the spectra have to live all the time, so here we can create
    // them during  the constructor

    string folder = "music/" + name_of_this_element ;

    string name;
    for(int i = 0 ; i < EIGHT_ANODES ; i++)
    {
        name = name_of_this_element + "_anode_" + char('A' + i) + "_energy_raw" ;
        spec_energy[i] = new spectrum_1D(name,
                                         name,
                                         4096, 0, 4096,
                                         folder, "", name);
        frs_spectra_ptr->push_back(spec_energy[i]) ;

        named_pointer[name] = Tnamed_pointer(&energy[i], 0, this) ;

        //---
        name = name_of_this_element + "_anode_" + char('A' + i) + "_time_raw"  ;
        spec_time[i] = new spectrum_1D(name,
                                       name,
                                       2000, 0, 2000,
                                       folder, "some music chambers do not provide time signals", name);
        frs_spectra_ptr->push_back(spec_time[i]) ;

        named_pointer[name] = Tnamed_pointer(&time[i], 0, this) ;

        //---
        name = name_of_this_element + "_anode_" + char('A' + i) + "_x_when_ok" ;
        spec_x_musica_mw[i] = new spectrum_1D(name,
                                              name,
                                              200, -100, 100,
                                              folder, "", name);

        frs_spectra_ptr->push_back(spec_x_musica_mw[i]);
        named_pointer[name] = Tnamed_pointer(&x_musica[i], &x_musica_ok[i], this) ;

#ifdef MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED

        //---
        name = name_of_this_element + "_anode_" + char('A' + i) + "_y_when_ok" ;
        spec_y_musica_mw[i] = new spectrum_1D(name,
                                              name,
                                              200, -100, 100,
                                              folder, "", name);

        frs_spectra_ptr->push_back(spec_y_musica_mw[i]);

        //  named_pointer[name] = Tnamed_pointer(&y_musica[i], &y_musica_ok[i], this) ;

#endif


    }

    //---
    name = name_of_this_element + "_x_mw" ;
    spec_music_x_mw = new spectrum_1D(name,
                                      name,
                                      200, -100, 100,
                                      folder, "extrapolated from MW",
                                      name_of_this_element + "_x, (when MW correction is selected)");

    frs_spectra_ptr->push_back(spec_music_x_mw);


#ifdef MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED

    name = name_of_this_element + "_y_mw" ;
    spec_music_y_mw = new spectrum_1D(name,
                                      name,
                                      200, -100, 100,
                                      folder, "extrapolated from MW",
                                      name_of_this_element + "_y, (when MW correction is selected)");
    frs_spectra_ptr->push_back(spec_music_y_mw);
#endif



    //--------------
    name = name_of_this_element + "_deposit_energy_non_corr"  ;
    spec_music_deposit_energy = new spectrum_1D(name,
            name,
            4000, 0, 4000,
            folder, "before correction",
            name_of_this_element + "_deposit_energy_when_music_ok");
    frs_spectra_ptr->push_back(spec_music_deposit_energy) ;

    //--------
    name = name_of_this_element + "_deposit_energy_corr_self"  ;
    spec_music_deposit_energy_corr_self = new spectrum_1D(name,
            name,
            4000, 0, 4000,
            folder, "Active only if choosen so called \"self correction\" (other "
            "possibility is \"multiwire correction\" - corr_mwpc)" ,
            name_of_this_element +
            "_deposit_energy_corr_when_music_ok");
    // the same incrementer as below, because there  are two modes of working
    frs_spectra_ptr->push_back(spec_music_deposit_energy_corr_self) ;
    //---------
    name = name_of_this_element + "_deposit_energy_corr_mwpc"  ;
    spec_music_deposit_energy_corr_mwpc = new spectrum_1D(name,
            name,
            4000, 0, 4000,
            folder, "",
            name_of_this_element +
            "_deposit_energy_corr_when_music_ok");
    // the same incrementer as above, because there  are two modes of working
    frs_spectra_ptr->push_back(spec_music_deposit_energy_corr_mwpc) ;




    name = name_of_this_element + "_deposit_energy_corr_sci"  ;
    spec_music_deposit_energy_corr_sci = new spectrum_1D(name,
            name,
            4000, 0, 4000,
            folder, "",
            name_of_this_element +
            "_deposit_energy_corr_sci_when_music_ok");
    // the same incrementer as above, because there  are two modes of working
    frs_spectra_ptr->push_back(spec_music_deposit_energy_corr_sci) ;

#ifdef TPC42_PRESENT
    name = name_of_this_element + "_deposit_energy_corr_tpc"  ;
    spec_music_deposit_energy_corr_tpc = new spectrum_1D(name,
            name,
            4000, 0, 4000,
            folder, "",
            name_of_this_element +
            "_deposit_energy_corr_tpc_when_music_ok");
    // the same incrementer as above, because there  are two modes of working
    frs_spectra_ptr->push_back(spec_music_deposit_energy_corr_tpc) ;
#endif //  TPC42_PRESENT



    //---------
    name = name_of_this_element + "_root_mean_square_for_self_corr"  ;
    spec_music_rms = new spectrum_1D(name,
                                     name,
                                     100, 0, 100,
                                     folder,
                                     "Active only if choosen so called \"self correction\" (other "
                                     "possibility is \"multiwire correction\" - corr_mwpc)",
                                     noinc
                                    );
    frs_spectra_ptr->push_back(spec_music_rms) ;

    //-------------- 2D matrices

    // I do not know proper ranges of x_mean, so I put something safe

    name = name_of_this_element + "_dE__vs__x_mean_corrSelf"  ;
    matr_music_de_vs_x_mean__self = new spectrum_2D(name,
            name,
            200, -100, 100, // <--from sufit
            800, 0, 4000,
            folder, "",
            string("X: ") + name_of_this_element + "_x_mean"  //    music_x_mean,
            +  ", Y:" + name_of_this_element +
            "_deposit_energy_when_music_ok" //                    deposit_energy);
                                                   );
    frs_spectra_ptr->push_back(matr_music_de_vs_x_mean__self) ;

    //--------------
    name = name_of_this_element + "_x_mean_corrMwpc__vs__dE"  ;
    matr_music_x_mean_mwpc_vs_de = new spectrum_2D(name,
            name,
            800, 200, 4000,
            200, -100, 100,   // <- from sufit
            folder, "",
            string("X: ") + name_of_this_element +
            "_deposit_energy_when_music_ok"  // deposit_energy,
            + ", Y:" + name_of_this_element + "_x_mean" // music_x_mean
                                                  );

    frs_spectra_ptr->push_back(matr_music_x_mean_mwpc_vs_de);

#ifdef MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED
    name = name_of_this_element + "_y_mean_corrMwpc__vs__dE"  ;
    matr_music_y_mean_mwpc_vs_de = new spectrum_2D(name,
            name,
            800, 200, 4000,
            200, -100, 100,   // <- from sufit
            folder, "",
            string("X: ") + name_of_this_element +
            "_deposit_energy_when_music_ok"  // deposit_energy,
            + ", Y:" + name_of_this_element + "_y_mean" // music_x_mean
                                                  );
    frs_spectra_ptr->push_back(matr_music_y_mean_mwpc_vs_de);
#endif


#ifdef TPC42_PRESENT

    name = name_of_this_element + "_x_mean_corr_tpc__vs__dE"  ;
    matr_music_x_mean_tpc_vs_de = new spectrum_2D(name,
            name,
            800, 200, 4000,
            200, -100, 100,   // <- from sufit
            folder, "",
            string("X: ") + name_of_this_element +
            "_deposit_energy_when_music_ok"  // deposit_energy,
            + ", Y:" + name_of_this_element + "_x_mean_tpc" // music_x_mean
                                                 );

    frs_spectra_ptr->push_back(matr_music_x_mean_tpc_vs_de);


#endif

    name = name_of_this_element + "_x_mean_corr_sci__vs__dE"  ;
    matr_music_x_mean_scipc_vs_de = new spectrum_2D(name,
            name,
            800, 200, 4000,
            200, -100, 100,   // <- from sufit
            folder, "",
            string("X: ") + name_of_this_element +
            "_deposit_energy_sci_corr_when_music_ok"  // deposit_energy,
            + ", Y:" + name_of_this_element + "_x_mean" // music_x_mean
                                                   );

    frs_spectra_ptr->push_back(matr_music_x_mean_scipc_vs_de);


#ifdef MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE

    name = name_of_this_element + "_atmospheric_pressure_raw";
    spec_pressure_raw =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder, "Taken from the event", name);
    frs_spectra_ptr->push_back(spec_pressure_raw) ;


    name = name_of_this_element + "_atmospheric_pressure_cal";
    spec_pressure_cal =
        new spectrum_1D(name,
                        name,
                        4096, 0, 4096,
                        folder, "", name);
    frs_spectra_ptr->push_back(spec_pressure_cal) ;


    name = name_of_this_element + "_deposit_energy_corr_mwpc_without_atmospheric_pressure_corr"  ;
    spec_deposit_energy_corr_without_atmospheric_pressure_corr =
        new spectrum_1D(name,
                        name,
                        4000, 0, 4000,
                        folder, "",
                        name_of_this_element +
                        "_deposit_energy_corr_without_atmospheric_pressure_when_music_ok");
    // the same incrementer as above, because there  are two modes of working
    frs_spectra_ptr->push_back(spec_deposit_energy_corr_without_atmospheric_pressure_corr) ;


    name = name_of_this_element + "_deposit_energy_corr_sci_without_atmospheric_pressure_corr"  ;
    spec_deposit_energy_corr_sci_without_atmospheric_pressure_corr =
        new spectrum_1D(name,
                        name,
                        4000, 0, 4000,
                        folder, "",
                        name_of_this_element +
                        "_deposit_energy_corr_sci_without_atmospheric_pressure_when_music_ok");
    // the same incrementer as above, because there  are two modes of working
    frs_spectra_ptr->push_back(spec_deposit_energy_corr_sci_without_atmospheric_pressure_corr) ;

#ifdef TPC42_PRESENT

    name = name_of_this_element + "_deposit_energy_corr_tpc_without_atmospheric_pressure_corr"  ;
    spec_deposit_energy_corr_tpc_without_atmospheric_pressure_corr =
        new spectrum_1D(name,
                        name,
                        4000, 0, 4000,
                        folder, "",
                        name_of_this_element +
                        "_deposit_energy_corr_tpc_without_atmospheric_pressure_when_music_ok");
    // the same incrementer as above, because there  are two modes of working
    frs_spectra_ptr->push_back(spec_deposit_energy_corr_tpc_without_atmospheric_pressure_corr) ;
#endif

#endif


}
//**********************************************************************
void Tmusic_8anodes::analyse_current_event()
{
    //cout << "make calculations for " <<  name_of_this_element  << endl ;
    bool flag_pressure_calculations_already_done = false;

    // from the event
    for(int i = 0 ; i < EIGHT_ANODES ; i++)
    {
        time[i] = (event_unpacked->*time_array) [i] ;
        spec_time[i]->manually_increment(time[i]) ;

        energy[i] = (event_unpacked->*energy_array) [i] ;
        spec_energy[i]->manually_increment(energy[i]) ;

        x_musica_ok[i] = false; // we will set it below
    }

    //     energy[6] = energy[7] ; // cheating
    music_x = 0 ;
    //----------------------------------------------------------------------
    //    Energy loss
    //----------------------------------------------------------------------
    // calculating the energy loss as average of the ...
    music_ok =  false ;
    deposit_energy = 0;
    deposit_energy_corr = 0 ;


#ifdef MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE
    deposit_energy_corr_without_atmospheric_pressure = 0 ;
#endif
    // some anodes can be broken ????????????????
    // so below we make some trick. To calculate
    // geometrick mean we check if it has sensible data.
    // if yes - it participates in calculations
    // if no  - we put there 1

    // ask somebody if this is right !

    how_many_works = 0 ;
    for(int i = 0 ; i < EIGHT_ANODES ; i++)
    {
        if((energy[i] > 1) && (energy[i] < 4000))
        {
            anode_works[i] =  true ;
            how_many_works++ ;
        }
        else
        {
            anode_works[i] =   false ;
        }
    }

    if(how_many_works == EIGHT_ANODES)
    {
        //----------------------------------------------------------
        // this trick was invented by Maggie to avoid great numbers
        // in case of calculating the geometric mean

        double rrr[EIGHT_ANODES] = {0};
        for(int i = 0 ; i < EIGHT_ANODES ; i++)
        {
            rrr[i] =
                anode_works[i] ?  // if sensible value
                (energy[i] -  pedestal[i])    // contribute to the geometeic mean
                : 1 ;                                         // if not, make it transparent
        }

        double r[EIGHT_ANODES / 2] ;
        for(int i = 0 ; i < EIGHT_ANODES / 2 ; i++)
        {
            r[i] = rrr[2 * i] * rrr[2 * i + 1];
        }

        deposit_energy = sqrt(
                             sqrt(sqrt(r[0]) * sqrt(r[1]))
                             *
                             sqrt(sqrt(r[2]) * sqrt(r[3]))
                         ) ;

        //-------------------- end of trick -------------------------
        music_ok =  true ;
        // cout << "deposit_energy = " << deposit_energy << endl ;
        spec_music_deposit_energy->manually_increment(deposit_energy);
    }

    //----------------------------------------------------------------------
    //    Energy loss corrected for position
    //----------------------------------------------------------------------
    // My naive comment:
    // The energy loss had to be corrected, depending of this how
    // ion was flying. If it was paralele to the beam - it needs shorter
    // time than when it was with some angle.
    // so we must have correction.
    //
    // such correction depends on tracking of the ion.
    // It can be done on two ways:
    //
    // 1. tracking can be done by music itself, by analysing
    //    the drift time data from the anodes (Drift time calibration)
    //
    // 2. Tracking can be done by neighbouring MW, (this information
    //    was used by the focusing point (MW at s4)


    //------------------------------------------------------------
    //      (Drift time calibration) against (MW at s4)
    //------------------------------------------------------------

    drift_time_ok =  false;

    //-----------
    for(int i = 0 ; i < EIGHT_ANODES ; i++)//do i = 1, 4
    {
        if(time[i] > 0 && time[i] < 2000)
        {
            time_ok[i] =  true ;
        }
        else
        {
            time_ok[i] =  false ;
        }
    }

    // My trick (JG). We check only time_ok when its anode works. If it doesnt
    // we asumme that its time was ok.

    if((time_ok[0]  || !anode_works[0])
            &&
            (time_ok[1] || !anode_works[1])
            &&
            (time_ok[2] || !anode_works[2])
            &&
            (time_ok[3]) || !anode_works[3])
    {
        drift_time_ok =  true ;
    }

    //------------------------------------------------------------
    //         X - Position from drift times
    //------------------------------------------------------------
    for(int i = 0 ; i < EIGHT_ANODES ; i++)// do i = 1, 4
    {
        position[i] = -920 ;
        z_coordinate[i] = 0 ;                   // music 'z' coordinate of particular anode)
    }

    // this we cann calculate only if we were successful by
    // tracking of the ion in
    if(related_focus->was_x_ok()
            &&
            drift_time_ok
      )
    {
        for(int i = 0 ; i < EIGHT_ANODES ; i++)// do i = 1, 4
        {
            position[i] = time_offset[i] + (time[i] * time_slope[i]);
            z_coordinate[i] =
                distance + dist_music_anode[i]
                - (related_focus->give_distance_of_second_mw()) ;  // this is mw42

        }
    }

    //      Position at mw42 will be calculated
    deposit_energy_corr  =  0;
    double radius[EIGHT_ANODES] ;
    double  sum_x = 0;
    double  sum_z = 0;
    double  sum_z2 = 0;
    double  sum_xz = 0;
    double  rms = 0;
    double  mu_a = -10000 ;
    double  mu_b = 0 ;

    double corr = 0 ;

    for(int i = 0 ; i < EIGHT_ANODES ; i++)
        radius[i] = 0;

    //=====================================
    // two types of energy loss correction
    //=====================================
    // cout << "how to correct = " << how_to_correct << endl ;
    //   cout << "drift_time_ok="   << drift_time_ok << endl;

    //=====================================
    // So called: Self correction, almost never used
    //=====================================

    if(related_focus->was_x_ok()
            &&
            drift_time_ok
            &&
            how_to_correct ==  self_correction)    // "SELF" correction: tracked by music ====
    {
        //cout << "SELF correction because type_of_correction = 1 " << endl ;
        for(int i = 0 ; i < 4 ; i ++)
        {
            if(! anode_works[i])
                continue ; // not working will not participate
            sum_x = sum_x + position[i];
            sum_z  = sum_z + z_coordinate[i];
            sum_z2 = sum_z + z_coordinate[i] * z_coordinate[i];
            sum_xz = sum_xz + position[i] * z_coordinate[i];
        }

        //      Linear regression: x = a  +  b * z
        mu_b = (how_many_works * sum_xz - sum_z * sum_x)
               /
               (how_many_works * sum_z2 - sum_z * sum_z);
        mu_a = (sum_x - mu_b * sum_z) / how_many_works;

        //       cout << " mu_a =" <<  mu_a <<  " mu_b=" << mu_b << endl ;

        // RMS - Root Mean Square radius -----------------------
        //      RMS distance of 4 points from the line
        //  cout << " RMS calcul " << endl ;

        for(int i = 0 ; i < EIGHT_ANODES ; i++)
        {
            radius[i] = position[i] - mu_a - mu_b * z_coordinate[i];
            //cout << "position[i] " << position[i] << endl ;
            rms += anode_works[i] ? (radius[i] * radius[i]) : 0 ;
        }

        rms = sqrt(rms) * 10.0;
        spec_music_rms->manually_increment(rms);

        const double music_r = 50.0 ; // ?????????????????????????? Samit, what is it?

        if(rms < music_r)    // music_r  it is some constant
        {
            music_ang = mu_b  * 1000.0;      // in mrad
            music_x   = mu_a ;             // position at mw42    <--- why ?
            music_x_mean =  sum_x / how_many_works ;      // mean position

            //    correct geom. mean dE value with mean x position
            //    Position correction by self  = 1, by mwPC = 2
            corr = 0;
            if(music_ok)
            {
                // matrix to deduce 'a' factors for calibration
                matr_music_de_vs_x_mean__self->manually_increment(music_x_mean,
                        deposit_energy);

                corr = amusic_pos[0]
                       +
                       amusic_pos[1] * music_x_mean
                       +
                       amusic_pos[2] * pow(music_x_mean, 2)
                       +
                       amusic_pos[3] * pow(music_x_mean, 3)
                       +
                       amusic_pos[4] * pow(music_x_mean, 4)
                       +
                       amusic_pos[5] * pow(music_x_mean, 5);

                if(corr > 0.)
                {
                    corr =  amusic_pos[0] / corr;
                    deposit_energy_corr = deposit_energy  *  corr;
                }
                else
                {
                    music_ok =  false;
                    deposit_energy_corr = 0;
                    corr = 0;
                } // endif
                spec_music_deposit_energy_corr_self->manually_increment(deposit_energy_corr);
            }  // endif
        }   // endif
    }  // endif
    //=====================================
    // END of so called: Self correction, almost never used
    //=====================================



    //=====================================
    // So called: Multiwire position correction
    //=====================================

    //music_x1_mean = 0;
#ifdef MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED
    // local variables
    double  y_musica[EIGHT_ANODES] = { 0.0 };
    // calculated by scintillators, but we
    // will ask for that using the 'extrapolate' function from focusing point (s4)
    bool    y_musica_ok[EIGHT_ANODES] = { false };
    double music_y_mean = 0 ;
#endif // #ifdef MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED


    if(related_focus->was_x_ok()
            &&
            music_ok
            &&
            how_to_correct ==  mwpc_correction)   // type_of_correction 1 -  means "MWPC" correction =
    {
        //cout << "MWPC correction because type_of_correction = 2 " << endl ;
        // X music1 anode positions we calculate form MW
        for(int j = 0 ; j < EIGHT_ANODES ; j++)
        {
            /// cout << "calcultaing " << endl;
            // if(j==0)   cout << "NEW calculations--------------" << endl ;
            x_musica[j] = related_focus->
                          extrapolate_x_to_distance(distance + dist_music_anode[j]);
            x_musica_ok[j] = true;

            //  if(j==0)
            //      {
            //      cout << ", dist_music= " << distance
            //           << ", dist_musica[j] = " << dist_music_anode[j]
            //           << endl ;
            //      cout << " x_musica[" << j <<"] = "
            //           <<   x_musica[j] << endl ;
            //      }

            spec_x_musica_mw[j]->manually_increment(x_musica[j]);
            music_x_mean += x_musica[j];

#ifdef MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED
            if(flag_make_y_mw_correction)
            {
                y_musica[j] = related_focus->
                              extrapolate_y_to_distance(distance + dist_music_anode[j]);
                y_musica_ok[j] = true;

                spec_y_musica_mw[j]->manually_increment(y_musica[j]);
                music_y_mean += y_musica[j];
            }
#endif

        }
        music_x_mean /=   EIGHT_ANODES ;  // Summed just above, now dividing (arithmetic mean)
        music_x = related_focus->extrapolate_x_to_distance(distance);
        spec_music_x_mw->manually_increment(music_x);

        //------------------------
        // this is probably the matrix to produce the 'b' factors for calibration
        // at first we look at it, and then, after fitting the calibration factors
        // we can make real, true correction
        matr_music_x_mean_mwpc_vs_de->manually_increment(deposit_energy, music_x_mean);
        corr =
            bmusic_pos[0]
            +
            bmusic_pos[1] * music_x_mean
            +
            bmusic_pos[2] * pow(music_x_mean, 2)
            +
            bmusic_pos[3] * pow(music_x_mean, 3)
            +
            bmusic_pos[4] * pow(music_x_mean, 4)
            +
            bmusic_pos[5] * pow(music_x_mean, 5);

        if(corr > 0.)
        {
            corr =  bmusic_pos[0] / corr;
            deposit_energy_corr = deposit_energy * corr;
        }
        else
        {
            music_ok =  false  ;
        } // endif

#ifdef MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED
        if(flag_make_y_mw_correction)
        {
            music_y_mean /=   EIGHT_ANODES ;  // Summed just above, now dividing (arithmetic mean)
            double music_y = related_focus->extrapolate_y_to_distance(distance);
            spec_music_y_mw->manually_increment(music_y);

            //------------------------
            // this is probably the matrix to produce the 'b' factors for calibration
            // at first we look at it, and then, after fitting the calibration factors
            // we can make real, true correction
            matr_music_y_mean_mwpc_vs_de->manually_increment(deposit_energy, music_y_mean);
            double corr_y =
                b_y_music_pos[0]
                +
                b_y_music_pos[1] * music_y_mean
                +
                b_y_music_pos[2] * pow(music_y_mean, 2)
                +
                b_y_music_pos[3] * pow(music_y_mean, 3)
                +
                b_y_music_pos[4] * pow(music_y_mean, 4)
                +
                b_y_music_pos[5] * pow(music_y_mean, 5);

            //????????????????????
            if(corr_y > 0.)
            {
                corr_y =  b_y_music_pos[0] / corr_y;
                deposit_energy_corr = deposit_energy * corr_y * corr;   // cor in Y and in Y
            }
        } // end if flag_make_y_mw_correction

#endif




#ifdef MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE

        deposit_energy_corr_without_atmospheric_pressure = deposit_energy_corr;
        spec_deposit_energy_corr_without_atmospheric_pressure_corr->
        manually_increment(deposit_energy_corr_without_atmospheric_pressure);

        if(flag_make_atmospheric_pressure_correction_of_dE)
        {
            // reading raw value
            current_pressure_raw = *atmospheric_pressure_in_event;
            //       cout << " current_pressure_raw= " << current_pressure_raw
            //       << " deposit_energy_corr_without_atmospheric_pressure = "
            //       << deposit_energy_corr_without_atmospheric_pressure
            //       << " deposit_energy_corr= " << deposit_energy_corr
            //       << endl;
            spec_pressure_raw->manually_increment(current_pressure_raw);
            // calibrating it
            current_pressure_cal =
                (qua_pressure * current_pressure_raw * current_pressure_raw) +
                (slo_pressure * current_pressure_raw) +
                of_pressure;
            spec_pressure_cal->manually_increment(current_pressure_cal);

            if(current_pressure_raw && current_pressure_cal > 1)
            {
                deposit_energy_corr = deposit_energy_corr * (reference_pressure / current_pressure_cal);
            }
            flag_pressure_calculations_already_done = true;
        }
#endif

        spec_music_deposit_energy_corr_mwpc->manually_increment(deposit_energy_corr);
        //cout << "music_deposit_energy_corr " <<  music_deposit_energy_corr  << endl ;
    } //end if mwpc correction
    //=====================================
    // End of so called: Multiwire position correction
    //=====================================





    //------------------------------------------------------
    //Scintillator 41 can give x for correction
    //================================

    x_from_scintillator = -999;
    music_sci_corr_ok = false;
    if(nearest_scintillator)
    {
        if(nearest_scintillator->is_position_ok())
        {
            x_from_scintillator = nearest_scintillator->give_position();
            //------------------------

            matr_music_x_mean_scipc_vs_de->manually_increment(deposit_energy, x_from_scintillator);
            corr =
                bmusic_pos[0]
                +
                bmusic_pos[1] * x_from_scintillator
                +
                bmusic_pos[2] * pow(x_from_scintillator, 2)
                +
                bmusic_pos[3] * pow(x_from_scintillator, 3)
                +
                bmusic_pos[4] * pow(x_from_scintillator, 4)
                +
                bmusic_pos[5] * pow(x_from_scintillator, 5);
            music_sci_corr_ok  =  true  ;

            if(corr > 0.)
            {
                corr =  bmusic_pos[0] / corr;
                deposit_energy_corr_sci = deposit_energy * corr;
            }
            else
            {
                music_sci_corr_ok  =  false  ;
            } // endif

#ifdef MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE

            deposit_energy_corr_sci_without_atmospheric_pressure = deposit_energy_corr_sci;
            spec_deposit_energy_corr_sci_without_atmospheric_pressure_corr->
            manually_increment(deposit_energy_corr_sci_without_atmospheric_pressure);

            if(flag_make_atmospheric_pressure_correction_of_dE)
            {
                if(!flag_pressure_calculations_already_done)
                {
                    // reading raw value
                    current_pressure_raw = *atmospheric_pressure_in_event;
                    //       cout << " current_pressure_raw= " << current_pressure_raw
                    //       << " deposit_energy_corr_without_atmospheric_pressure = "
                    //       << deposit_energy_corr_without_atmospheric_pressure
                    //       << " deposit_energy_corr= " << deposit_energy_corr
                    //       << endl;
                    spec_pressure_raw->manually_increment(current_pressure_raw);
                    // calibrating it
                    current_pressure_cal =
                        (qua_pressure * current_pressure_raw * current_pressure_raw) +
                        (slo_pressure * current_pressure_raw) +
                        of_pressure;
                    spec_pressure_cal->manually_increment(current_pressure_cal);
                    flag_pressure_calculations_already_done = true;
                }
                if(current_pressure_raw && current_pressure_cal > 1)
                {
                    deposit_energy_corr_sci = deposit_energy_corr_sci * (reference_pressure / current_pressure_cal);
                }

            }
#endif
            spec_music_deposit_energy_corr_sci->manually_increment(deposit_energy_corr_sci);
        }
    }

    //#################################
    //------------------------------------------------------
    // Focal plane s4tpc can also give x for correction
    //================================
#ifdef TPC42_PRESENT
    // TPC correction

    if(related_tpc_focus->was_x_ok()
            &&
            music_ok)
    {
        //cout << "MWPC correction " << endl ;
        // X music1 anode positions we calculate form s4tpc
        double  music_x_mean_tpc = 0;
        double music_x_tpc;
        for(int j = 0 ; j < EIGHT_ANODES ; j++)
        {
            /// cout << "calcultaing " << endl;
            // if(j==0)   cout << "NEW calculations--------------" << endl ;
            x_musica[j] = related_tpc_focus->
                          extrapolate_x_to_distance(distance + dist_music_anode[j]);
            x_musica_ok[j] = true;

            //  if(j==0)
            //      {
            //      cout << ", dist_music= " << distance
            //           << ", dist_musica[j] = " << dist_music_anode[j]
            //           << endl ;
            //      cout << " x_musica[" << j <<"] = "
            //           <<   x_musica[j] << endl ;
            //      }

            //spec_x_musica_tpc[j]->manually_increment(x_musica[j]);
            music_x_mean_tpc += x_musica[j];
        }
        music_x_mean_tpc /=   EIGHT_ANODES ;  // Summed just above, now dividing (arithmetic mean)
        music_x_tpc = related_tpc_focus->extrapolate_x_to_distance(distance);
        //spec_music_x_tpc->manually_increment(music_x_tpc);

        //------------------------
        // this is probably the matrix to produce the 'c' factors for calibration
        // at first we look at it, and then, after fitting the calibration factors
        // we can make real, true correction
        matr_music_x_mean_tpc_vs_de->manually_increment(deposit_energy, music_x_mean_tpc);

        corr =
            cmusic_pos[0]
            +
            cmusic_pos[1] * music_x_mean_tpc
            +
            cmusic_pos[2] * pow(music_x_mean_tpc, 2)
            +
            cmusic_pos[3] * pow(music_x_mean_tpc, 3)
            +
            cmusic_pos[4] * pow(music_x_mean_tpc, 4)
            +
            cmusic_pos[5] * pow(music_x_mean_tpc, 5);

        if(corr > 0.)
        {
            corr =  bmusic_pos[0] / corr;
            deposit_energy_corr_tpc = deposit_energy * corr;
            music_tpc_corr_ok = true;
        }
        else
        {
            music_tpc_corr_ok =  false  ;
        } // endif

#ifdef MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE

        deposit_energy_corr_tpc_without_atmospheric_pressure = deposit_energy_corr_tpc;
        spec_deposit_energy_corr_tpc_without_atmospheric_pressure_corr->
        manually_increment(deposit_energy_corr_tpc_without_atmospheric_pressure);

        if(flag_make_atmospheric_pressure_correction_of_dE)
        {

            if(!flag_pressure_calculations_already_done)
            {
                // reading raw value
                current_pressure_raw = *atmospheric_pressure_in_event;
                //       cout << " current_pressure_raw= " << current_pressure_raw
                //       << " deposit_energy_corr_without_atmospheric_pressure = "
                //       << deposit_energy_corr_without_atmospheric_pressure
                //       << " deposit_energy_corr= " << deposit_energy_corr
                //       << endl;
                // was already incremented above:  spec_pressure_raw->manually_increment(current_pressure_raw);
                // calibrating it
                current_pressure_cal =
                    (qua_pressure * current_pressure_raw * current_pressure_raw) +
                    (slo_pressure * current_pressure_raw) +
                    of_pressure;
                spec_pressure_cal->manually_increment(current_pressure_cal);
                flag_pressure_calculations_already_done = true;
            }
            if(current_pressure_raw && current_pressure_cal > 1)
            {
                deposit_energy_corr_tpc = deposit_energy_corr_tpc * (reference_pressure / current_pressure_cal);
            }

        }
#endif

        spec_music_deposit_energy_corr_tpc->manually_increment(deposit_energy_corr_tpc);
        //cout << "music_deposit_energy_corr_tpc " <<  music_deposit_energy_corr_tpc  << endl ;
    } //end if TPC correction
#endif // TPC42_PRESENT

    //#################################

    calculations_already_done = true ;
}
//**************************************************************************
void  Tmusic_8anodes::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;
    distance = Tfile_helper::find_in_file(s, name_of_this_element
                                          + "_distance");

    // reading the caligration factors
    for(int i = 0 ; i < EIGHT_ANODES ; i++)
    {
        time_offset[i]  = Tfile_helper::find_in_file(s, name_of_this_element
                          + "_anode_"
                          + char('A' + i)
                          + "_time_offset");
        time_slope[i]  = Tfile_helper::find_in_file(s, name_of_this_element
                         + "_anode_"
                         + char('A' + i) + "_time_slope");
        pedestal[i]  = Tfile_helper::find_in_file(s, name_of_this_element
                       + "_anode_"
                       + char('A' + i) + "_energy_pedestal");
    }

    how_to_correct = static_cast<type_of_correction>
                     ((int) Tfile_helper::find_in_file(s, name_of_this_element
                             + "_correction_type_(self=1_or_mwpc=2)")
                     ) ;

    if(how_to_correct > 2 || how_to_correct < 0)
    {
        cout << "Wrong walue for " << string(name_of_this_element
                                             + "_correction_type_(self=1_or_mwpc=2)")
             << " - you may put there 1 o 2 "
             << endl ;
        exit(1) ;
    }


#ifdef  MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED
    flag_make_y_mw_correction = true;
#endif


    for(int i = 0 ; i < 6 ; i++)   // six degree of calibration
    {
        // calibration factors for correction "self_correction:"
        amusic_pos [i] = Tfile_helper::find_in_file(s, name_of_this_element
                         + "_corr_self_factor_"
                         + char('0' + i));

        bmusic_pos [i] = Tfile_helper::find_in_file(s, name_of_this_element
                         + "_corr_mwpc_factor_"
                         + char('0' + i));
#ifdef TPC42_PRESENT
        try
        {

            // calibration factors for correction "TPC_correction:"
            cmusic_pos [i] = Tfile_helper::find_in_file(s, name_of_this_element
                             + "_corr_tpc_factor_"
                             + char('0' + i));
        }
        catch(Tno_keyword_exception &e)
        {
            cmusic_pos [i] = 0 ;
            // to be able to continue the reading we need to repair
            Tfile_helper::repair_the_stream(s);
        }
#endif

#ifdef  MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED
        try
        {
            // calibration factors for x correction "mwpc_correction:"
            b_y_music_pos [i] = Tfile_helper::find_in_file(s, name_of_this_element
                                + "_corr_y_mwpc_factor_"
                                + char('0' + i));

        }
        catch(Tno_keyword_exception &e)
        {
            b_y_music_pos [i] = 0 ;
            flag_make_y_mw_correction = false;
            // to be able to continue the reading we need to repair
            Tfile_helper::repair_the_stream(s);
            // throw;
        }
#endif


    }  // end of for



#ifdef MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE

    // remembering the address in the event to take the atmospheric pressure

#ifdef MUN_ACTIVE_STOPPER_PRESENT
    // in case of Tin100 experiment it was here
    atmospheric_pressure_in_event = &event_unpacked->munich_multiplicity_module[0];
#else
    atmospheric_pressure_in_event = &event_unpacked->atmospheric_pressure_raw;
#endif

    try
    {
        flag_make_atmospheric_pressure_correction_of_dE =
            Tfile_helper::find_in_file(
                s, "flag_make_atmospheric_pressure_correction_of_musics_dE");

        reference_pressure =
            Tfile_helper::find_in_file(
                s, "reference_atmospheric_pressure__(used_for_dE_correction)");

        of_pressure = Tfile_helper::find_in_file(s, "calibration_factors_for_atmospheric_pressure");
        s >> zjedz >> slo_pressure
          >> zjedz >> qua_pressure ;

    }
    catch(Tno_keyword_exception &e)
    {
        // repair the stream
        s.clear(s.rdstate() & ~(ios::eofbit | ios::failbit));
        flag_make_atmospheric_pressure_correction_of_dE = 0;
        reference_pressure = 0 ;
    }




#endif

}
//***********************************************************************
