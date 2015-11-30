/***************************************************************************
                          tkratta.cpp  -  description
                             -------------------
    begin                : Fri Oct 12 2014
    copyright            : (C) 2014 by dr Jerzy Grebosz
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/
#include "spectrum.h"

#include "Tkratta.h"
#include "Tkratta_crystal.h"
#ifdef KRATTA_PRESENT

#include "Tfrs.h"
#include "Tcluster.h"
#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h"

#include "Tsi_detector.h"


//*************************************************************************
Tkratta::Tkratta(string name): Tfrs_element(name)
{
    //  cout << "construcor of " << name_of_this_element << endl;

    // To know how many kratta modules we have - we must read the lookup table

    string geo_name = "calibration/geo_map.geo";
    ifstream plik(geo_name);
    if(!plik)
    {
        cout << "!!! Fatal error: I can't open file " << geo_name
             << "\nMost probably you do not have such a file in the ./calibration subdirectory of "
             << endl;
        system ("pwd");
        system ("ls *");
        system ("ls ./calibration/*");
        exit(1);
    }

    map<int, string> mapka;
    
    // first line is a comment
    string linijka;
    for(int nr = 0 ; getline(plik, linijka) ; ++nr)
    {
        if(linijka[0] == '#' ) {
            --nr;
            continue;
        }

        //cout << "Przeczytane " << linijka << endl;
        istringstream s(linijka.c_str());
        int fadc, chan;
        int modu;
       // int sign = 0;
        //bool flag = true;
        string label;

        s >> label>>  fadc >> chan ;

        //      >>  sign >> modu;
        //sign = nr % 3 ;
        modu = nr / 3;

        if(!s)
        {
            //cerr << "  Tlookup_table_kratta::read_from_disk --> error while reading file " << name << endl;
            break;
        }

        // powolanie do istnienia
        //cout << "loading to mapka do modul nr " << modu << ", label " << label << endl;
        mapka[modu] = label;
    }


    // -----------------------------------------------------------
    for(int i = 0 ; i < KRATTA_NR_OF_CRYSTALS ; ++i)
    {
        // if i is legal
        string lab = mapka[i];
        if(! lab.empty())
        {
            crystal.push_back(  new Tkratta_crystal(this, "kratta"+lab.substr(0,2), i)) ;
        }
    }

    
    
    
    named_pointer[name_of_this_element+"_multiplicity"] =
            Tnamed_pointer(&multiplicity_of_hits, 0, this) ;

    create_my_spectra();

}
//**************************************************************************
Tkratta::~Tkratta()
{
    save_spectra();
}
//*************************************************************************
//*****************************************************************
/** No descriptions */
void Tkratta::create_my_spectra()
{

    // Matrices ==============================================
    // map of the positions
    string folder = "kratta_folder/"  ;
    string name =   name_of_this_element + "_position_xy" ;

    //  /*   matr_position_xy = new spectrum_2D(
    //         name,
    //         name,
    //
    //         10, 0, 10, // KRATTA_NCOL,
    //         10, 0, 10, // KRATTA_NROW+1, 0, KRATTA_NROW,
    //         folder );
    //
    //     frs_spectra_ptr->push_back(matr_position_xy);*/

    //------------------------------------
    name = name_of_this_element + "_multiplicity"  ;
    spec_multiplicity = new spectrum_1D( name,
                                         name,
                                         KRATTA_NR_OF_CRYSTALS, 0, KRATTA_NR_OF_CRYSTALS,
                                         folder );
    frs_spectra_ptr->push_back(spec_multiplicity) ;


    name = name_of_this_element + "_fan"  ;
    spec_fan = new spectrum_1D( name,
                                name,
                                KRATTA_NR_OF_CRYSTALS, 0, KRATTA_NR_OF_CRYSTALS,
                                folder );
    frs_spectra_ptr->push_back(spec_fan) ;
}
//*************************************************************************
void Tkratta::make_preloop_action(ifstream &)
{
    // we do not use the stream here, because Cate has its own calibration file.

    cout << "Tkratta::make_preloop_action "  << endl;


    //     cout  << "Cala tablica kratta " << hex << &(event_unpacked->kratta) << dec << endl;
    for(unsigned int i = 0 ; i < crystal.size() ; i++)
    {
        crystal[i]->set_event_data_pointers((event_unpacked->kratta[i]));

        //         cout  << "Ta trojka tablica " << hex << event_unpacked->kratta[i] << dec << endl;
        crystal[i]->set_event_data_pointers((event_unpacked->kratta[i]));
    }


#if 0

    // Reading the distance of the Cate detector
    string geometry_file = "calibration/kratta_geometry.txt" ;

    try {

        ifstream plik_geometry(geometry_file.c_str() );
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


    //  kratta_segment k ;
    //  kratta_segment *k_ptr ;

    //------------------------------


    //cout << "After loop " << endl;



    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // very important:
    // calibrations and gates are read everytime we start the analysis

    read_calibration_and_gates_from_disk();
#endif
    cout << "End of Tkratta preloop " << endl;

}
//**********************************************************************
void Tkratta::read_calibration_and_gates_from_disk()
{
    // (by calling cluster preloop functions)

    cout << "\n\n\n\nReading Kratta calibration from the disk" << endl ;

    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    string cal_filename = "calibration/kratta_calibration.txt";
    try {
        ifstream plik(cal_filename.c_str() ) ; // ascii file
        if(!plik)
        {
            string mess = "I can't open kratta calibration file: "
                    + cal_filename  ;
            throw mess ;
        }

        // real reading the calibration -----------------
        for(unsigned int i = 0 ; i < crystal.size() ; i++)
        {
            cout << "reading calibration loop for kratta segment" << i << endl ;
            crystal[i]->make_preloop_action(plik) ;   // reference
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

    cout << "kratta calibration successfully read"
         << endl;



    //------------------------------------
    // we need to read the 1D gates for KRATTA itself

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

}
//********************************************************
//********************************************************
//********************************************************
void Tkratta::analyse_current_event()
{

    /*  char wzmocnienie;
//   typedef int cena;
    using cena = int;

  const cena  cen_1 = 40.8;      wolno dodać const
  cena  cen_2 { 30.6 };     // nadaje sie rowniez dla double (bo rozszerzenie a nie zawężenie
 */



    if(calculations_already_done) return ;

    multiplicity_of_hits = 0;

    //cout << "Cate analyse current event" << endl;
    for(unsigned int i = 0 ; i < crystal.size() ; i++)
    {
        crystal[i]->analyse_current_event();
        if(crystal[i]->fired() )
        {
            //      cout << "Kratta segment " << i+1 << " fired " << endl;
            ++multiplicity_of_hits;

            spec_fan->manually_increment( (int)i);  // they like (1-9)
            // int x_position = crystal[i]->give_x();
            // int y_position = crystal[i]->give_y();
            // matr_position_xy->manually_increment(x_position, y_position);
        }  // if this crystal fired
    } // for

    spec_multiplicity->manually_increment(multiplicity_of_hits);

    // finally
    calculations_already_done = true ;
}
//***********************************************************************
void  Tkratta::make_postloop_action()
{
    // cout << "F.Tkratta::make_postloop_action() " << endl ;

    // save all spectra made in Cate and in crystals
    save_spectra();


    // cout << "end of F.Tkratta::make_postloop_action() " << endl ;
}
//***********************************************************************
// packing with some artificial data
//
void Tkratta::simulate_event()
{
#ifdef NIGDY
    for(unsigned int i = 0 ; i < crystal.size() ; i++)
    {
        event_unpacked->kratta_detector[i].CsI_energy = (10*i);
        event_unpacked->kratta_detector[i].CsI_time  = (10*i) +1 ;
        event_unpacked->kratta_detector[i].Si_energy = 100*i ;
        event_unpacked->kratta_detector[i].Si_time = 100*i +1;

        event_unpacked->kratta_detector[i].pin_a = 210 + i ;
        event_unpacked->kratta_detector[i].pin_b = 220 + i ;
        event_unpacked->kratta_detector[i].pin_c = 240 + i ;
        event_unpacked->kratta_detector[i].pin_d = 250 + i ;
    }
#endif
}
//**********************************************************************
///** gives the geometrical position of the hit */
//double Tkratta::give_x()
//{
//  if( ! calculations_already_done) analyse_current_event();
//  return x_position ; // it is simulation, here should be real position
//}
////**********************************************************************
///** returns the y geomterical position of the hit on the Cate detector */
//double Tkratta::give_y()
//{
//  if( ! calculations_already_done) analyse_current_event();
//  return y_position ; // it is simulation, here should be real position
//}
//***************************************************************************
/**  Checking if we were able to determine X Y correctly */
//bool Tkratta::was_xy_ok()
//{
//  if( ! calculations_already_done) analyse_current_event();
//
//  return flag_exact_xy_ok ;
//}
//*****************************************************************************
/** made in post loop, but also when the FRS ask for it (i.e. every 5 min) */
void Tkratta::save_spectra()
{
    //     // saving Cate spectra
    //     // .....
    //
    //     // saving crystal spectra
    //     // .....
    //
    //     for(unsigned int i = 0 ; i < kratta_spectra_list.size() ; i++)
    //     {
    //         //cout << "Trying to save spectrum nr " <<  i << endl ;
    //         kratta_spectra_list[i]->save_to_disk() ;
    //     }

}
//***************************************************************
/** if only Si crystal fired */
//bool Tkratta::was_only_one_hit()
//{
//  return flag_only_one_hit ;
//}
//***************************************************************
/** if we had data from one and only one segment. (even if the data is outside of some gates) */
//bool Tkratta::segment_known()
//{
//  flag_one_hit ;
//}
//***************************************************************
/** was zero, one, or more kratta segment hit by the residue how_many_hits */
// int Tkratta::how_many_hits()
// {
//     return multiplicity_of_hits ;
// }
//***************************************************************
/** No descriptions */
//void Tkratta::incr_matr_dE_vs_E_gated_by_multiplicity(int nr)
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
//    //cout << "Tkratta Many hits " << dE.size() << endl;
//  }
//}
/** if it was element 1, 3, 7, 9 (starting from 1) */
//**********************************************************************************************
/** gives the geometrical position of the hit */
//**********************************************************************************************





#endif   // #ifdef KRATTA_PRESENT
