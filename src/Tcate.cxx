/***************************************************************************
                          tcate.cpp  -  description
                             -------------------
    begin                : Fri May 16 2003
    copyright            : (C) 2003 by dr Jerzy Grebosz
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/



#include "Tcate.h"
#include "Tcate_telescope.h"

#include "Tfrs.h"
#include "Tcluster.h"
#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h"

#include "Tsi_detector.h"


//*************************************************************************
Tcate::Tcate(string name, string name_of_silicon_at_target): Tfrs_element(name)
{


    if(name_of_silicon_at_target == ""
            ||
            name_of_silicon_at_target == "NONE"
            ||
            name_of_silicon_at_target == "none"
      )
    {
        related_silicon_at_target = 0 ;
    }
    else
    {
        related_silicon_at_target = dynamic_cast<Tsi_detector*>(owner->address_of_device(name_of_silicon_at_target)) ;
    }


    //  cout << "construcor of " << name_of_this_element << endl;
    // creating nine telescopes ----------------------------
#ifdef CATE_PRESENT

    telescope.push_back(new Tcate_telescope(this, "cate_segm_1", 0)) ;
    telescope.push_back(new Tcate_telescope(this, "cate_segm_2", 1)) ;
    telescope.push_back(new Tcate_telescope(this, "cate_segm_3", 2)) ;
    telescope.push_back(new Tcate_telescope(this, "cate_segm_4", 3)) ;
    telescope.push_back(new Tcate_telescope(this, "cate_segm_5", 4)) ;
    telescope.push_back(new Tcate_telescope(this, "cate_segm_6", 5)) ;
    telescope.push_back(new Tcate_telescope(this, "cate_segm_7", 6)) ;
    telescope.push_back(new Tcate_telescope(this, "cate_segm_8", 7)) ;
    telescope.push_back(new Tcate_telescope(this, "cate_segm_9", 8)) ;


    // the numbering is like this

    //   7   8   9
    //   4   5   6
    //   1   2   3

    // (looking from the target)
    // Pins are always named as (no matter if the real detector was rotated)
    //
    //  D     C
    //
    //     E
    //
    //  A     B


    // the position of the center of the telescope nr 5 is x=0, y=0

    named_pointer[name_of_this_element + "_flag_only_one_hit"] =
        Tnamed_pointer(&flag_only_one_hit, 0, this) ;

    named_pointer[name_of_this_element + "_flag_cate_xy_ok_and_only_one_hit"] =
        Tnamed_pointer(&flag_cate_xy_ok_and_only_one_hit, 0, this) ;


    named_pointer[name_of_this_element + "_id_of_fired"] =
        Tnamed_pointer(&id_of_fired, 0, this) ;

    named_pointer[name_of_this_element + "_position_x_when_only_one_hit_and_ok"] =
        Tnamed_pointer(&x_position, &flag_cate_xy_ok_and_only_one_hit, this) ;

    named_pointer[name_of_this_element + "_position_y_when_only_one_hit_and_ok"] =
        Tnamed_pointer(&y_position, &flag_cate_xy_ok_and_only_one_hit, this) ;



    named_pointer[name_of_this_element + "_position_x_corr_when_only_one_hit_and_ok"] =
        Tnamed_pointer(&x_position_corr, &flag_cate_xy_ok_and_only_one_hit, this) ;

    named_pointer[name_of_this_element + "_position_y_corr_when_only_one_hit_and_ok"] =
        Tnamed_pointer(&y_position_corr, &flag_cate_xy_ok_and_only_one_hit, this) ;



    named_pointer[name_of_this_element + "_multiplicity_of_CsI_hits"] =
        Tnamed_pointer(&multiplicity_of_CsI_hits, 0, this) ;

    named_pointer[name_of_this_element + "_multiplicity_of_Si_hits"] =
        Tnamed_pointer(&multiplicity_of_Si_hits, 0, this) ;


    named_pointer[name_of_this_element + "_id_of_CsI_fired_when_multiplicity_1"] =
        Tnamed_pointer(&id_of_CsI_fired_when_multiplicity_1, 0, this) ;

    named_pointer[name_of_this_element + "_id_of_Si_fired_when_multiplicity_1"] =
        Tnamed_pointer(&id_of_Si_fired_when_multiplicity_1, 0, this) ;






    named_pointer[name_of_this_element + "_flag_one_of_corner_segments_1_3_7_9_fired"]
    =
        Tnamed_pointer(&flag_one_of_corner_segments_1_3_7_9_fired, 0, this) ;

    named_pointer[name_of_this_element + "_flag_one_of_cross_segments_2_4_6_8_fired"]
    =
        Tnamed_pointer(&flag_one_of_cross_segments_2_4_6_8_fired, 0, this) ;

    named_pointer[name_of_this_element + "_dE_when_ok_and_only_one_hit"] =
        Tnamed_pointer(&dE, &flag_cate_dE_E_ok_and_only_one_hit, this) ;

    named_pointer[name_of_this_element + "_E_when_ok_and_only_one_hit"] =
        Tnamed_pointer(&E, &flag_cate_dE_E_ok_and_only_one_hit, this) ;

    named_pointer[name_of_this_element + "_CsI_time_calibrated_when_only_one_hit"] =
        Tnamed_pointer(&CsI_time_calibrated, &flag_only_one_hit, this);


    // for ratio purposes

    named_pointer[name_of_this_element + "_flag_cate_xy_ok_and_only_one_hit_for_rate_purposes"] =
        Tnamed_pointer(&flag_cate_xy_ok_and_only_one_hit,
                       &flag_cate_xy_ok_and_only_one_hit, this) ;

    create_my_spectra();
#endif

}
//**************************************************************************
Tcate::~Tcate()
{
    save_spectra();
}
//*************************************************************************
//*****************************************************************
/** No descriptions */
void Tcate::create_my_spectra()
{


    // Matrices ==============================================
    // map of the positions
    string folder = "cate_folder/"  ;
    string name =   name_of_this_element + "_position_xy" ;

    matr_position_xy = new spectrum_2D(
        name,
        name,
//                790, -79, 79,     // (3*50) + (2*4mm) (odstepu) => 158/2 => 79
//                790, -79, 79,     // -0.5, 0.5,
        395, -79, 79,     // (3*50) + (2*4mm) (odstepu) => 158/2 => 79
        395, -79, 79,     // -0.5, 0.5,
        folder);

    give_spectra_pointer() ->push_back(matr_position_xy) ;


    // map of the positions

    name =   name_of_this_element + "_position_xy_corrected" ;

    matr_position_xy_corr = new spectrum_2D(
        name,
        name,
//                790, -79, 79,     // (3*50) + (2*4mm) (odstepu) => 158/2 => 79
//                790, -79, 79,     // -0.5, 0.5,
        395, -79, 79,     // (3*50) + (2*4mm) (odstepu) => 158/2 => 79
        395, -79, 79,     // -0.5, 0.5,
        folder);

    give_spectra_pointer() ->push_back(matr_position_xy_corr) ;



    // TOTAL, to put banana on the scattered particle identification

    // the classical matrix for cate_telescope detectors deltaE / E

    name =   name_of_this_element + "_dE_vs_E" ;

    matr_total_dE_E = new spectrum_2D(name,
                                      name,
                                      600, 1, 4096,
                                      600, 1, 4096,
                                      folder);

    give_spectra_pointer()->push_back(matr_total_dE_E) ;




    name =  name_of_this_element +
            "_dE_vs_E_gated_crystal_multiplicityA" ;

    matr_total_dE_E_gated_by_crystal_multiplicityA
    = new spectrum_2D(name,
                      name,
                      100, 1, 4096,
                      100, 1, 4096,
                      folder);

    give_spectra_pointer()->push_back(matr_total_dE_E_gated_by_crystal_multiplicityA) ;



    name =   name_of_this_element +
             "_dE_vs_E_gated_crystal_multiplicityB" ;

    matr_total_dE_E_gated_by_crystal_multiplicityB
    = new spectrum_2D(name,
                      name,
                      100, 1, 4096,
                      100, 1, 4096,
                      folder);

    give_spectra_pointer()->push_back(matr_total_dE_E_gated_by_crystal_multiplicityB) ;


    name =  name_of_this_element +
            "_dE_vs_E_gated_crystal_multiplicityC" ;

    matr_total_dE_E_gated_by_crystal_multiplicityC
    = new spectrum_2D(name,
                      name,
                      100, 1, 4096,
                      100, 1, 4096,
                      folder);

    give_spectra_pointer()->push_back(matr_total_dE_E_gated_by_crystal_multiplicityC) ;



    // ----------------------------------
    // To put HJW gates
    // ----------------------------------
    name = name_of_this_element + "_total_CsI_time_cal"  ;
    spec_total_CsI_time_cal = new spectrum_1D(name,
            name,
            1024, 0, 4096,
            folder);
    give_spectra_pointer()->push_back(spec_total_CsI_time_cal) ;

//  // the corresponding gate
//  total_CsI_time_gate_low  = 1 ;   // lower limit
//  total_CsI_time_gate_high = 4095;



    name = name_of_this_element + "_total_Si_time_cal"  ;
    spec_total_Si_time_cal = new spectrum_1D(name,
            name,
            1024, 0, 4096,
            folder);
    give_spectra_pointer()->push_back(spec_total_Si_time_cal) ;


//  total_Si_time_gate_low  = 1 ;   // lower limit
//  total_Si_time_gate_high = 4095;


    //------------------------------------


    name = name_of_this_element + "_Si_multiplicity"  ;
    spec_Si_multiplicity = new spectrum_1D(name,
                                           name,
                                           10, 0, 10,
                                           folder);
    give_spectra_pointer()->push_back(spec_Si_multiplicity) ;


    name = name_of_this_element + "_CsI_multiplicity"  ;
    spec_CsI_multiplicity = new spectrum_1D(name,
                                            name,
                                            10, 0, 10,
                                            folder);
    give_spectra_pointer()->push_back(spec_CsI_multiplicity) ;



    name = name_of_this_element + "_fan"  ;
    spec_fan = new spectrum_1D(name,
                               name,
                               10, 0, 10,
                               folder);
    give_spectra_pointer()->push_back(spec_fan) ;





    name =   name_of_this_element +
             "_seg_5_dE_vs_crystal_multiplicity" ;

    matr_Si_E_vs_cluster_multiplicyty
    = new spectrum_2D(name,
                      name,
                      15, 0, 15,
                      600, 1, 4096,
                      folder);

    give_spectra_pointer()->push_back(matr_Si_E_vs_cluster_multiplicyty) ;


}

//*************************************************************************
void Tcate::make_preloop_action(ifstream &)
{
    // we do not use the stream here, because Cate has its own calibration file.

    //cout << "Tcate::make_preloop_action "  << endl;

    // Reading the distance of the Cate detector
    string geometry_file = "calibration/cate_geometry.txt" ;

    try
    {

        ifstream plik_geometry(geometry_file.c_str());
        if(! plik_geometry)
        {
            string mess = "I can't open  file: " + geometry_file  ;
            throw mess ;
        }
        string slowo = name_of_this_element + "_distance";
        distance = Tfile_helper::find_in_file(plik_geometry, slowo);
    }
    catch(Tfile_helper_exception &k)
    {
        cerr << "Error while reading geometry file "
             << geometry_file
             << ":\n"
             << k.message << endl  ;
        exit(1);
    }


//  cate_segment k ;
//  cate_segment *k_ptr ;

    //------------------------------
    for(unsigned int i = 0 ; i < telescope.size() ; i++)
    {
        // trick for vector
//      if(event_unpacked->cate_detector.size()-1 < i)
//      {
//        cate_segment m;
//        event_unpacked->cate_detector.push_back(m);
//        }

        // remember the source of data
//      cout << i << ") Address of cate segment " << & (event_unpacked->cate_detector[i]) << endl;
//      k     = event_unpacked->cate_detector[i] ;
//      k_ptr = &event_unpacked->cate_detector[i] ;
#ifdef CATE_PRESENT
        telescope[i]->set_event_data_pointers(& (event_unpacked->cate_detector[i]));
#endif // CATE_PRESENT
    }

    //cout << "After loop " << endl;



    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // very important:
    // calibrations and gates are read everytime we start the analysis

    read_calibration_and_gates_from_disk();

    cout << "End of Tcate preloop " << endl;

}
//**********************************************************************
void Tcate::read_calibration_and_gates_from_disk()
{
    // (by calling cluster preloop functions)
#ifdef CATE_PRESENT
    cout << "\n\n\n\nReading Cate calibration from the disk" << endl ;

    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    string cal_filename = "calibration/cate_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            string mess = "I can't open cate calibration file: "
                          + cal_filename  ;
            throw mess ;
        }

        // real reading the calibration -----------------
        for(unsigned int i = 0 ; i < telescope.size() ; i++)
        {
            cout << "reading calibration loop for cate segment" << i << endl ;
            telescope[i]->make_preloop_action(plik) ;   // reference
        }

    }  // end of try

    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ; // is it healthy ?
    }

    cout << "cate calibration successfully read"
         << endl;



    //------------------------------------
    // we need to read the 1D gates for CATE itself

    // this is:
    // -  the gate on the total calibrated time
    // -  the gate on the total calibrated energy
    //#ifdef NIGDY
    // reading gates
    // but this is in a different file....

    cout << " reading the gates for " << name_of_this_element << "  " << endl ;


    //-----------------


//  // Gate on the total CsI time --------------------
//
//  string nam_gate = string("gates/")
//    + name_of_this_element
//    + "_total_CsI_time_cal"
//    + "_gate_GATE.gate" ;
//
//  ifstream plik_gates(nam_gate.c_str() );
//  if(! plik_gates)
//    {
//   string mess = "I can't open  file: " + nam_gate  ;
//   // throw mess ;
//   cout << mess << "\nSo I assume that the gate is wide open !!!!!!!!!!! \a\a\a\n" << endl;
//   ofstream pliknew(nam_gate.c_str() );
//   pliknew <<  total_CsI_time_gate_low
//        << "   "
//        << total_CsI_time_gate_high
//        << "\t\t // left and right value for the gate" << endl;
//
//    }else{
//
//
//        plik_gates >> zjedz >> total_CsI_time_gate_low     // lower limit
//               >> zjedz >> total_CsI_time_gate_high ;   // upper limit
//
//   if(! plik_gates)
//     {
//       string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
//       exit(-1) ;
//     }
//   plik_gates.close() ;
//    }
//
//
//// gate on the total Si time ---------------------------
//  nam_gate = string("gates/")
//    + name_of_this_element
//    + "_total_Si_time_cal"
//    + "_gate_GATE.gate" ;
//
//  ifstream plik_gates2(nam_gate.c_str() );
//  if(! plik_gates2)
//    {
//   string mess = "I can't open  file: " + nam_gate  ;
//   // throw mess ;
//   cout << mess << "\nSo I assume that the gate is wide open !!!!!!!!!!! \a\a\a\n" << endl;
//   ofstream pliknew(nam_gate.c_str() );
//   pliknew <<  total_Si_time_gate_low
//        << "   "
//        << total_Si_time_gate_high
//        << "\t\t // left and right value for the gate" << endl;
//
//    }else{
//
//
//   plik_gates2 >> zjedz >> total_Si_time_gate_low     // lower limit
//         >> zjedz >> total_Si_time_gate_high ;   // upper limit
//
//   if(! plik_gates2)
//     {
//    string mess = "Error while reading 2 numbers from the gate file: " + nam_gate  ;
//    exit(-1) ;
//     }
//   plik_gates2.close() ;
//    }
#endif
}
//********************************************************
//********************************************************
//********************************************************
void Tcate::analyse_current_event()
{

    if(calculations_already_done) return ;
#ifdef CATE_PRESENT

    multiplicity_of_CsI_hits = 0;
    multiplicity_of_Si_hits = 0 ;

    flag_only_one_hit = false ;
    flag_cate_xy_ok_and_only_one_hit = false ;
    flag_cate_dE_E_ok_and_only_one_hit = false ;
    dE_E_ok  = false ;

    id_of_fired = -1 ;
    id_of_CsI_fired_when_multiplicity_1 = -1;
    id_of_Si_fired_when_multiplicity_1 = -1;


    flag_one_of_corner_segments_1_3_7_9_fired = false ;
    flag_one_of_cross_segments_2_4_6_8_fired = false ;

    E = 0 ;
    dE = 0 ;
    CsI_time_calibrated = 0 ;

    Si_energy_cal_from_si_det_at_target  = 0 ;

//cout << "Cate analyse current event" << endl;

    if(related_silicon_at_target)
    {
        Si_energy_cal_from_si_det_at_target  =
            related_silicon_at_target->give_Si_energy_cal() ;
    }



    for(unsigned int i = 0 ; i < telescope.size() ; i++)
        //for(unsigned int i = 4 ; i  < 5 ; i++)
    {
        telescope[i]->analyse_current_event();

//    if(telescope[i]->give_Si_and_CsI_fired_good()) // with this we had a "holes" on the xy plot.!

// this above is impossible with the new setup od CsI
        if(telescope[i]->   give_Si_segment_fired_good())          // this gives the nice plot without the holes
// ask Rady for details!!!
        {

            //      cout << "Cate segment " << i+1 << " fired " << endl;
            id_of_fired = i + 1 ; // start counting from 1-9
            id_of_Si_fired_when_multiplicity_1 = i ;

            spec_fan->manually_increment((int)(i + 1)); // they like (1-9)

            flag_exact_xy_ok = telescope[i]->was_xy_ok() ;
            if(flag_exact_xy_ok)
            {
                x_position = telescope[i]->give_x();
                y_position = telescope[i]->give_y();
                matr_position_xy->manually_increment(x_position, y_position);

                x_position_corr = telescope[i]->give_x_corr();
                y_position_corr = telescope[i]->give_y_corr();

                matr_position_xy_corr->manually_increment(x_position_corr, y_position_corr);


// Warning: if the calibration of pins is not good enough
// some hits of the telscope nr... can have xy from the region
// outside its own. For example telescope nr 5 may add a point on the
// matrix where we expect points from telscope nr 2

//        if(id_of_fired == 5 && (x_position < 30 || x_position > 30
//        || y_position < 35 || y_position > 30  ))
//        {
//          cout << "Exact position x = "
//              << x_position
//              << "y =" << y_position << endl;
//
//        }
            }
//     else
//      {
//        x_position = telescope[i]->give_central_x();
//        y_position = telescope[i]->give_central_y();
////        cout << "No Exact position, only center available  x = "
////            << x_position
////            << "y =" << y_position << endl;
//
//
//      }

        }  // if this telescope fired

        //----------------------------------------------------
        if(telescope[i]->   give_CsI_segment_fired_good())
        {
            id_of_CsI_fired_when_multiplicity_1 = i;
        }

    } // for



    spec_Si_multiplicity->manually_increment(multiplicity_of_Si_hits);
    spec_CsI_multiplicity->manually_increment(multiplicity_of_CsI_hits);

    // WARNING:   now it is inside the for loop. When we require only one
    // hit - it should (?) be outside

    // here we cold make some total spectra? or let the crystals do it
    if(multiplicity_of_Si_hits == 1 && multiplicity_of_CsI_hits == 1)
    {
        flag_only_one_hit = true ;
        //     cout << "cate: There was one hit " << endl;
        flag_cate_xy_ok_and_only_one_hit = flag_exact_xy_ok ; // and one hit

        flag_cate_dE_E_ok_and_only_one_hit = flag_only_one_hit  &&  dE_E_ok  ;

        if(RisingAnalysis_ptr->is_verbose_on())
        {
            cout  << name_of_this_element << ":  (Precisely) ONE segment fired " << endl;
        }

        // cate -----------------

//        TIFJAnalysis::fxCaliEvent->cate_dE = dE ;
//        TIFJAnalysis::fxCaliEvent->cate_E   = E;



#ifdef CATE_PRESENT
        TIFJAnalysis::fxCaliEvent->cate_x_pos = x_position_corr;
        TIFJAnalysis::fxCaliEvent->cate_y_pos = y_position_corr;

        // putting this below is
        TIFJAnalysis::fxCaliEvent->cate_segment_which_fired_nr
        = flag_cate_dE_E_ok_and_only_one_hit ? (id_of_fired - 1) : -1 ;     // franco wants 0-8

#endif


        switch(id_of_fired)
        {
        case 1:
        case 3:
        case 7:
        case 9:
            flag_one_of_corner_segments_1_3_7_9_fired = true ;
            break;
        case 2:
        case 4:
        case 6:
        case 8:
            flag_one_of_cross_segments_2_4_6_8_fired = false ;
            break;
        }
    }
    else
    {
        // cout << "There was MORE than one hit " << endl;
        if(multiplicity_of_CsI_hits != 1) id_of_CsI_fired_when_multiplicity_1 = -1;
        if(multiplicity_of_Si_hits != 1)  id_of_Si_fired_when_multiplicity_1 = -1;

    }

//  if( (!dE || !E)   &&  flag_cate_dE_E_ok_and_only_one_hit)
//  {
//    cout << "Cate dE E zero ,when flag  "
//            << flag_cate_dE_E_ok_and_only_one_hit
//                << endl;
//  }


    // finally
#endif // CATE PRESENT
    calculations_already_done = true ;
}
//***********************************************************************
void  Tcate::make_postloop_action()
{
    // cout << "F.Tcate::make_postloop_action() " << endl ;

    // save all spectra made in Cate and in telescopes
    save_spectra();


    // cout << "end of F.Tcate::make_postloop_action() " << endl ;
}
//***********************************************************************
// packing with some artificial data
//
void Tcate::simulate_event()
{
#ifdef NIGDY
    for(unsigned int i = 0 ; i < telescope.size() ; i++)
    {
        event_unpacked->cate_detector[i].CsI_energy = (10 * i);
        event_unpacked->cate_detector[i].CsI_time  = (10 * i) + 1 ;
        event_unpacked->cate_detector[i].Si_energy = 100 * i ;
        event_unpacked->cate_detector[i].Si_time = 100 * i + 1;

        event_unpacked->cate_detector[i].pin_a = 210 + i ;
        event_unpacked->cate_detector[i].pin_b = 220 + i ;
        event_unpacked->cate_detector[i].pin_c = 240 + i ;
        event_unpacked->cate_detector[i].pin_d = 250 + i ;
    }
#endif
}
//**********************************************************************
///** gives the geometrical position of the hit */
//double Tcate::give_x()
//{
//  if( ! calculations_already_done) analyse_current_event();
//  return x_position ; // it is simulation, here should be real position
//}
////**********************************************************************
///** returns the y geomterical position of the hit on the Cate detector */
//double Tcate::give_y()
//{
//  if( ! calculations_already_done) analyse_current_event();
//  return y_position ; // it is simulation, here should be real position
//}
//***************************************************************************
/**  Checking if we were able to determine X Y correctly */
//bool Tcate::was_xy_ok()
//{
//  if( ! calculations_already_done) analyse_current_event();
//
//  return flag_exact_xy_ok ;
//}
//*****************************************************************************
/** made in post loop, but also when the FRS ask for it (i.e. every 5 min) */
void Tcate::save_spectra()
{
    // saving Cate spectra
    // .....

    // saving telescope spectra
    // .....

    for(unsigned int i = 0 ; i < cate_spectra_list.size() ; i++)
    {
        //cout << "Trying to save spectrum nr " <<  i << endl ;
        cate_spectra_list[i]->save_to_disk() ;
    }

}
//***************************************************************
/** if only Si telescope fired */
//bool Tcate::was_only_one_hit()
//{
//  return flag_only_one_hit ;
//}
//***************************************************************
/** if we had data from one and only one segment. (even if the data is outside of some gates) */
//bool Tcate::segment_known()
//{
//  flag_one_hit ;
//}
//***************************************************************
/** was zero, one, or more cate segment hit by the residue how_many_hits */
int Tcate::how_many_Si_hits()
{
    return multiplicity_of_Si_hits ;
}
//***************************************************************
/** No descriptions */
//void Tcate::incr_matr_dE_vs_E_gated_by_multiplicity(int nr)
//{
//  if(dE.size() == 1
//  && E.size() == 1)
//  {
//    if(nr == 1)
//    {
//      matr_total_dE_E_gated_by_crystal_multiplicityA->manually_increment(E[0], dE[0]);
//    }
//    if(nr == 2)
//    {
//      matr_total_dE_E_gated_by_crystal_multiplicityB->manually_increment(E[0], dE[0]);
//    }
//    if(nr == 3)
//    {
//      matr_total_dE_E_gated_by_crystal_multiplicityC->manually_increment(E[0], dE[0]);
//    }
//  }
//  else if( dE.size() > 1)
//  {
//    //cout << "Tcate Many hits " << dE.size() << endl;
//  }
//}
/** if it was element 1, 3, 7, 9 (starting from 1) */
bool Tcate::corners()
{
    return (id_of_fired == 1 || id_of_fired == 3 || id_of_fired == 7 || id_of_fired == 9) ;
}
//**********************************************************************************************
/** gives the geometrical position of the hit */
//**********************************************************************************************
double Tcate::give_x()
{
    if(! calculations_already_done) analyse_current_event();
    //return 2.0 ; // it is simulation, here should be real position
    return x_position_corr ;
}
//**********************************************************************************************
/** returns the y geometrical position of the hit on the Cate detector */
double Tcate::give_y()
{
    if(! calculations_already_done) analyse_current_event();
    //return 3.0 ;
    return y_position_corr ; // it is simulation, here should be real position
}

//**********************************************************************************************
bool Tcate::give_xy_of_si_at_target(double *xx, double *yy)
{
    if(related_silicon_at_target->was_xy_ok())
    {
        *xx = related_silicon_at_target->give_x();
        *yy = related_silicon_at_target->give_y();
        return true;
    }
    else
        return false;
}
//*************************************************************************************
double Tcate::give_central_x_pos_of_segment_which_fired()
{
#ifdef CATE_PRESENT
    return telescope[id_of_fired]->give_central_x() ;
#else
    cout << "This function Tcate::give_central_x_pos_of_segment_which_fired() should be never called "
         << endl;
    return 0;
#endif
}
//*************************************************************************************
double Tcate::give_central_y_pos_of_segment_which_fired()
{
#ifdef CATE_PRESENT
    return telescope[id_of_fired]->give_central_y() ;
#else
    cout << "This function Tcate::give_central_x_pos_of_segment_which_fired() should be never called "
         << endl;
    return 0;
#endif
}
