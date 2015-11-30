/***************************************************************************
                          thector_BaF.cpp  -  description
                             -------------------
    begin                : Wed May 21 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/



#include "Thector_baf.h"

#ifdef HECTOR_PRESENT

#include "Tfile_helper.h"
#include "randomizer.h"

#include "Tnamed_pointer.h"
#include "Tself_gate_hec_descr.h"

#include "Tcrystal_xia.h"  // for doppler corr
#include "Ttarget.h"  // for doppler corr
#include "TIFJAnalysis.h" // for ntuple

/** does the user require some thereshold value? */
bool Thector_BaF::flag_good_fast_requires_threshold;
/** does the user require some thereshold value? */
bool Thector_BaF::flag_good_slow_requires_threshold;
/** does the user require some thereshold value? */
bool Thector_BaF::flag_good_time_requires_threshold;

bool Thector_BaF::flag_good_requires_not_led;

//------- En fast
int  Thector_BaF::fast_threshold  ;
//------- En slow
int    Thector_BaF::slow_threshold  ;
//------- time
int   Thector_BaF::good_time_threshold ;

// reading
bool  Thector_BaF::flag_increment_fast_cal_with_zero;
bool   Thector_BaF::flag_increment_slow_cal_with_zero;
bool   Thector_BaF::flag_increment_time_cal_with_zero;


// enums declared inside the Tcrystal class
type_of_tracking_requirements   Thector_BaF::tracking_position_on_target ;
type_of_lycca_requirements       Thector_BaF::tracking_position_on_lycca ;
type_of_doppler_algorithm       Thector_BaF::which_doppler_algorithm ;
bool   Thector_BaF::flag_beta_from_frs ; // which beta to use, frs or fixed
double Thector_BaF::beta_fixed_value ;  // if fixed, here it is
double Thector_BaF::energy_deposit_in_target ; // bec. beta on the other side
// of the target will be slower

Ttarget * Thector_BaF::target_ptr ;      // for faster acces of the doppler correction


#define dcout  if(0)cout

//*************************************************************************
// these arguments nr_slow, nr_fast tell us  in which channel of ADC this data
// was send.
// hector_adc[32], hector_tdc[32]
// for example  BaF nr 6 can get data from adc[2], adc[14], tdc[8]


Thector_BaF::Thector_BaF(Thector*ptr_owner, int nr_det,
                         int nr_fast, int nr_slow, int nr_time)
    :owner(ptr_owner), det_nr(nr_det),
    channel_nr_of_slow(nr_slow),
    channel_nr_of_fast(nr_fast),
    channel_nr_of_time(nr_time)

{
  name_of_this_element = "hector_BaF_" ;
  name_of_this_element +=  char('0' + det_nr) ;

	
  //....................................................
  // warning: The above pointers should point to the TOTAL spectra
  // which belong to  the Hector object. They do NOT exist now, because the
  // Hector constructor runs later than this constructor
  // So these pointers had to be set in the preloop

  spec_total_slow_cal = 0  ;
  spec_total_fast_cal= 0  ;

//   /*spec_total_slow_doppler= 0  ;
//   spec_total_fast_doppler= 0  ;*/

  matr_total_fast_vs_slow_cal= 0  ;
//   matr_total_fast_vs_slow_doppler = 0  ;


  spec_total_time_cal = 0;
  matr_total_time_vs_slow_cal = 0;

  //....................................................

  create_my_spectra();

	  named_pointer[name_of_this_element+"_detector_number_when_fired"]
  = Tnamed_pointer(&det_nr, &flag_fired, this) ;

	
  named_pointer[name_of_this_element+"_slow_raw"]
  = Tnamed_pointer(&slow_raw, 0, this) ;


  named_pointer[name_of_this_element+"_fast_raw"]
  = Tnamed_pointer(&fast_raw, 0, this) ;

  named_pointer[name_of_this_element+"_time_raw"]
  = Tnamed_pointer(&time_raw, 0, this) ;




  named_pointer[name_of_this_element+"_slow_cal"]
  = Tnamed_pointer(&slow_cal, &flag_fired, this) ;


  named_pointer[name_of_this_element+"_fast_cal"]
  = Tnamed_pointer(&fast_cal, &flag_fired, this) ;

  named_pointer[name_of_this_element+"_time_cal"]
  = Tnamed_pointer(&time_cal, &flag_fired, this) ;


  named_pointer[name_of_this_element+"_slow_cal_when_good"]
  = Tnamed_pointer(&slow_cal, &flag_good_data, this) ;


  named_pointer[name_of_this_element+"_fast_cal_when_good"]
  = Tnamed_pointer(&fast_cal, &flag_good_data, this) ;

  named_pointer[name_of_this_element+"_time_cal_when_good"]
  = Tnamed_pointer(&time_cal, &flag_good_data, this) ;


  //-------------------

/*
  named_pointer[name_of_this_element+"_fast_doppler_when_good"]
  = Tnamed_pointer(&fast_doppler, &flag_doppler_successful, this) ;

  named_pointer[name_of_this_element+"_slow_doppler_when_good"]
  = Tnamed_pointer(&slow_doppler,  &flag_doppler_successful, this) ;*/

  //  named_pointer[name_of_this_element+"_fast_doppler_when_ok"]
  //    = Tnamed_pointer(&fast_doppler, &flag_doppler_successful, this) ;
  //
  //  named_pointer[name_of_this_element+"_slow_doppler_when_ok"]
  //    = Tnamed_pointer(&slow_doppler,  &flag_doppler_successful, this) ;

  //-------------

  string entry_name =  "ALL_hector_BaF_slow_energy_cal_when_good" ;
  Tnamed_pointer::add_as_ALL(entry_name, &slow_cal, &flag_good_data, this);


  entry_name =  "ALL_hector_BaF_fast_energy_cal_when_good" ;
  Tnamed_pointer::add_as_ALL(entry_name, &fast_cal, &flag_good_data, this);

  entry_name =  "ALL_hector_BaF_time_cal_when_good" ;
  Tnamed_pointer::add_as_ALL(entry_name, &time_cal, &flag_good_data, this);



  //-------------
  //  entry_name =  "ALL_hector_BaF_slow_energy_cal_when_good_and_not_led" ;
  //  pos = named_pointer.find(entry_name);
  //
  //  if (pos != named_pointer.end ())
  //    pos->second.add_item(&slow_cal_not_led, &flag_good_data, this);
  //  else
  //    named_pointer[entry_name] =
  //      Tnamed_pointer(&slow_cal_not_led, &flag_good_data, this);
  //
  //
  //  entry_name =  "ALL_hector_BaF_fast_energy_cal_when_good_and_not_led" ;
  //  pos = named_pointer.find(entry_name);
  //
  //  if (pos != named_pointer.end ())
  //    pos->second.add_item(&fast_cal_not_led, &flag_good_data, this);
  //  else
  //    named_pointer[entry_name] =
  //      Tnamed_pointer(&fast_cal_not_led, &flag_good_data, this);
  //
  //
  //  entry_name =  "ALL_hector_BaF_time_cal_when_good_and_not_led" ;
  //  pos = named_pointer.find(entry_name);
  //
  //  if (pos != named_pointer.end ())
  //    pos->second.add_item(&time_cal_not_led, &flag_good_data, this);
  //  else
  //    named_pointer[entry_name] =
  //      Tnamed_pointer(&time_cal_not_led, &flag_good_data, this);
  //



  //========= doppler ================

//   entry_name =  "ALL_hector_BaF_slow_doppler_when_good" ;
//   Tnamed_pointer::add_as_ALL(entry_name, &slow_doppler, &flag_doppler_successful, this);
// 
// 
//   entry_name =  "ALL_hector_BaF_fast_doppler_when_good" ;
//   Tnamed_pointer::add_as_ALL(entry_name, &fast_doppler, &flag_doppler_successful, this);


}
//*************************************************************************
Thector_BaF::~Thector_BaF()
{}
//*************************************************************************
void Thector_BaF::create_my_spectra()
{
  string folder = "hector/" + name_of_this_element ;
  // ------SLOW  ----------------------------
  string name = name_of_this_element + "_slow_raw"  ;
  spec_slow_raw = new spectrum_1D( name,
                                   name,
                                   4095, 1, 4096,   // was 4096
                                   folder );
  hector_spectra_ptr()->push_back(spec_slow_raw) ;


  name = name_of_this_element + "_slow_raw_and_not_led"  ;
  spec_slow_raw_not_led = new spectrum_1D( name,
                          name,
                          4095, 1, 4096,   // was 4096
                          folder );
  hector_spectra_ptr()->push_back(spec_slow_raw_not_led) ;


  name = name_of_this_element + "_slow_raw_and_led"  ;
  spec_slow_raw_and_led = new spectrum_1D( name,
                          name,
                          4095, 1, 4096,   // was 4096
                          folder );
  hector_spectra_ptr()->push_back(spec_slow_raw_and_led) ;




  // ----------------------------------
  name = name_of_this_element + "_slow_cal_and_not_led"  ;
  spec_slow_cal_not_led = new spectrum_1D( name,
                          name,
                          4095, 1, 4096,
                          folder );
  hector_spectra_ptr()->push_back(spec_slow_cal_not_led) ;




  // ----------------------------------
//   name = name_of_this_element + "_slow_dopplered"  ;
//   spec_slow_doppler = new spectrum_1D( name,
//                                        name,
//                                        4095, 1, 4096,
//                                        folder );
//   hector_spectra_ptr()->push_back(spec_slow_doppler) ;
// 



  //------------- FAST -----------------------------------------
  name = name_of_this_element + "_fast_raw"  ;
  spec_fast_raw = new spectrum_1D( name,
                                   name,
                                   4095, 1, 4096,
                                   folder );
  hector_spectra_ptr()->push_back(spec_fast_raw) ;

  // ----------------------------------
  name = name_of_this_element + "_fast_cal_and_not_led"  ;
  spec_fast_cal_not_led = new spectrum_1D( name,
                          name,
                          4095, 1, 4096,
                          folder );
  hector_spectra_ptr()->push_back(spec_fast_cal_not_led) ;

#if 0
  // ----------------------------------
  name = name_of_this_element + "_fast_dopplered"  ;
  spec_fast_doppler = new spectrum_1D( name,
                                       name,
                                       4095, 1, 4096,
                                       folder );
  hector_spectra_ptr()->push_back(spec_fast_doppler) ;
#endif 




  //------------- time -----------------------------------------
  name = name_of_this_element + "_time_raw"  ;
  spec_time_raw =
    new spectrum_1D( name,
                     name,
                     4095, 1, 4096,
                     folder);
  hector_spectra_ptr()->push_back(spec_time_raw) ;


  //  time_gate_low = 0 ;
  //  time_gate_high = 4095 ;

  // ----------------------------------

  //#ifdef HECTOR_CALIBRATE_TIME
  name = name_of_this_element + "_time_cal"  ;
  spec_time_cal = new spectrum_1D( name,
                                   name,
                                   4095, 1, 4096,
                                   folder );
  hector_spectra_ptr()->push_back(spec_time_cal) ;
  //#endif

  // ----------------------------------
  // matrices
  //-----------------------------------

  name =   name_of_this_element + "_fast_vs_slow" ;

  matr_fast_vs_slow = new spectrum_2D( name,
                                       name,
                                       512, 1, 4096,
                                       512, 1, 4096,
                                       folder );

  hector_spectra_ptr()->push_back(matr_fast_vs_slow) ;

  //--------------
  name =   name_of_this_element + "_fast_vs_slow_cal" ;
  matr_fast_vs_slow_cal = new spectrum_2D( name,
                          name,
                          512, 1, 4096,
                          512, 1, 4096,
                          folder );

  hector_spectra_ptr()->push_back(matr_fast_vs_slow_cal) ;

  //--------------
  name =   name_of_this_element + "_time_vs_slow" ;
  matr_time_vs_slow = new spectrum_2D( name,
                                       name,
                                       512, 1, 4096,
                                       512, 1, 4096,
                                       folder );

  hector_spectra_ptr()->push_back(matr_time_vs_slow) ;

  //--------------

  name =   name_of_this_element + "_time_vs_slow_cal" ;
  matr_time_vs_slow_cal = new spectrum_2D( name,
                          name,
                          512, 1, 4096,
                          512, 1, 4096,
                          folder );

  hector_spectra_ptr()->push_back(matr_time_vs_slow_cal) ;



  // Adam invented this
  //--------------
  name =   name_of_this_element + "_fast_o_slow_const_vs_time" ;
  matr_fast_over_slow_vs_time = new spectrum_2D( name,
                                name,
                                512, 1, 4096,
                                512, 1, 4096,
                                folder );
  hector_spectra_ptr()->push_back( matr_fast_over_slow_vs_time) ;

}
//*************************************************************************
//************************************************************************
/** readinng calibration, gates, setting pointers */
void Thector_BaF::make_preloop_action(ifstream & s)
{
  //
  //  cout << "preloop action for "
  //       << name_of_this_element
  //       << endl ;
  //


  // why it is done here, look at the comment in the constructor
  spec_total_slow_cal = owner->spec_total_slow_cal ;
  spec_total_fast_cal= owner->spec_total_fast_cal;

//   spec_total_slow_doppler= owner->spec_total_slow_doppler;
//   spec_total_fast_doppler= owner->spec_total_fast_doppler;
  matr_total_fast_vs_slow_cal= owner->matr_total_fast_vs_slow_cal;
//   matr_total_fast_vs_slow_doppler = owner->matr_total_fast_vs_slow_doppler  ;


  spec_total_time_cal= owner->spec_total_time_cal;
  matr_total_time_vs_slow_cal=owner->matr_total_time_vs_slow_cal;


  //  check_legality_of_data_pointers();



  // cout << "Reading the calibration for the "
  //        << name_of_this_element
  //        << endl ;

  // in this file we look for a string
  int how_many_factors = 2 ;   // max linear calibration

  string slowo = name_of_this_element + "_slow_cal_factors" ;

  // first reading the energy calibration
  Tfile_helper::spot_in_file(s, slowo );

  slow_calibr_factors.clear() ;

  for(int i = 0 ; i < how_many_factors ; i++)
  {
    // otherwise we read the data
    double value ;
    s >> value ;

    if(!s)
    {
      string mess =
        "I found keyword '" + slowo +
        "' but error was in reading its value." ;
      throw mess ;
    }
    // if ok
    slow_calibr_factors.push_back(value ) ;
  }


  // reading the FAST calibration factors --------------

  slowo = name_of_this_element + "_fast_cal_factors" ;

  Tfile_helper::spot_in_file(s, slowo );
  fast_calibr_factors.clear();
  for(int i = 0 ; i < how_many_factors ; i++)
  {
    // otherwise we read the data
    double value ;
    s >> value ;

    if(!s)
    {
      string mess =
        "I found keyword '" + slowo
        + "' but error was in reading its value." ;
      throw mess ;
    }
    // if ok

    fast_calibr_factors.push_back(value ) ;
  }




  // reading the time calibration factors --------------

  slowo = name_of_this_element + "_time_cal_factors" ;

  Tfile_helper::spot_in_file(s, slowo );
  time_calibr_factors.clear() ;
  for(int i = 0 ; i < how_many_factors ; i++)
  {
    // otherwise we read the data
    double value ;
    s >> value ;

    if(!s)
    {
      string mess =
        "I found keyword '" + slowo
        + "' but error was in reading its value." ;
      throw mess ;
    }
    // if ok
    time_calibr_factors.push_back(value ) ;
  }


  // ------------------------------------------------
  // it is one for all (so far)
  slowo = "constant_factor_to_multiply_fast";
  constant_factor_to_multiply_fast
  = Tfile_helper::find_in_file(s, slowo);
  if(!s)
  {
    string mess =
      "I found keyword '" + slowo
      + "' but error was in reading its value." ;
    throw mess ;
  }

  // ---------------------
  // Geometry: phi, theta
  //----------------------

  // but this is in a different file....

  // cout << " reading the geometry angles  for telescope "
  //        << name_of_this_element
  //        << endl ;


  //-----------------

  // here we read the Geometry
  //


  string geometry_file = "calibration/hector_geometry.txt" ;

  try
  {

    ifstream plik_geometry(geometry_file.c_str() );
    if(! plik_geometry)
    {
      string mess = "I can't open  file: " + geometry_file  ;
      throw mess ;
    }
    slowo = name_of_this_element + "_phi_theta";

    Tfile_helper::spot_in_file(plik_geometry, slowo);
    plik_geometry >> zjedz >> phi_geom >> zjedz >> theta_geom
    >> zjedz >> distance_from_target ;
    if(!plik_geometry) throw 1;



    distance_from_target *= 10 ; // it was in [cm] and we need in milimeters !
    phi_geom_radians   =  M_PI * phi_geom / 180.0 ;
    theta_geom_radians =  M_PI * theta_geom / 180.0;

    // ============== recalculate phi and theta into x,y,z of the crystal
    crystal_position_x = distance_from_target * sin(theta_geom_radians) * cos(phi_geom_radians) ;
    crystal_position_y = distance_from_target * sin(theta_geom_radians) * sin(phi_geom_radians) ;
    double ttt = cos(theta_geom_radians);
    crystal_position_z = distance_from_target * ttt;



  }
  catch(string sss)
  {
    cerr << "Error while reading geometry file "
    << geometry_file
    << ":\n"
    << sss << endl  ;

    exit(-1) ; // is it healthy ?
  }

  catch(int p)
  {
    cerr << "\nError while reading (inside geometry file "
    << geometry_file
    << ") phi, theta and distance  for "
    << name_of_this_element
    << "\nI found keyword '"
    << slowo
    << "' but error was while reading its (double) values of phi "
    << "and theta."
    << endl  ;
    cerr << "NOTE: The old version of spy didn't want the 'distance' value\n"
    "Solution: Please take a copy the file "
    << "cracow_online/calibration/hector_geometry.txt\n"
    "and put in into your ./calibration   directory\n" << endl;

    exit(-1) ; // is it healthy ?

  }
  catch(Tno_keyword_exception &capsule)
  {
    cerr << "Error while reading geometry file "
    << geometry_file
    << ":\n"
    << capsule.message << endl ;
    exit(-1) ;
  }

  //-------------------------------------------------------
  // read the default gate on time ------------------------
  //-------------------------------------------------------

  //  string core_of_the_name = "gates/" + name_of_this_element ;
  //  string nam_gate = core_of_the_name + "_time_raw_gate_GATE.gate" ;
  //
  //  ifstream plik_gatesX(nam_gate.c_str() );

  //  if(! plik_gatesX)
  //  {
  //    string mess = "I can't open  file: " + nam_gate
  //      + "\nThis gate you can produce interactively using a 'cracow' viewer "
  //      "program" ;
  //    cout << mess
  //         << "\nI create a gate and assmue that the "
  //      "gate is wide open !!!!!!!!!!!!!! \a\a\a\n"
  //         << endl ;
  //
  //    ofstream pliknew(nam_gate.c_str() );
  //    pliknew   << time_gate_low << "   "    // lower limit
  //              << time_gate_high << endl ;
  //  }else{
  //
  //    double tmp1, tmp2 ;
  //    plik_gatesX >> zjedz >> tmp1     // lower limit
  //                >> zjedz >> tmp2 ;   // upper limit
  //
  //    time_gate_low = (int)tmp1 ;   // lower limit
  //    time_gate_high = (int)tmp2 ;
  //
  //    if(! plik_gatesX)
  //    {
  //      string mess = "Error while reading 2 numbers from the gate file: "
  //        + nam_gate  ;
  //      exit(-1) ;
  //    }
  //    plik_gatesX.close() ;
  //  }

}
//***************************************************************************
//***************************************************************************
/** // the most important fuction,  executed for every event */
void Thector_BaF::analyse_current_event()
{

  //  cout << "   analyse_current_event()          for "
  //    << name_of_this_element
  //    << endl ;

  time_raw = *ptr_to_time ;   // rising will ask for it
  slow_raw = *ptr_to_slow ;   // rising will ask for it
  fast_raw = *ptr_to_fast ;   // rising will ask for it



  flag_fired = flag_good_data  = false ;

  slow_cal_not_led = fast_cal_not_led = time_cal_not_led =
                                          slow_cal = fast_cal = time_cal =   0 ;
  slow_doppler = fast_doppler  = 0 ;   // doppler corrected values


  flag_doppler_successful = false ;


  // checking if it is something (not zero)
  if(fast_raw ||  slow_raw  || time_raw)     // if any of them had something
  {
    flag_fired = true ;

    if(RisingAnalysis_ptr->is_verbose_on() )
    {
      cout  << name_of_this_element
      << ", E slow=  " << slow_raw
      << ", E fast = " << fast_raw
      << ", T = " << time_raw
      <<endl;
    }

    // simple signals
    if(fast_raw)owner->spec_statistics->manually_increment((10*det_nr));   // there was fast
    if(slow_raw)owner->spec_statistics->manually_increment((10*det_nr) + 1);   // there was slow
    if(time_raw)owner->spec_statistics->manually_increment((10*det_nr) + 2);   // there was time
    // where the statistics of markers ? (in Hector object!)

    //===================================
    // incrementing the raw  spectra only when non zero
    // (Adam M. and Franco decided this)

    if(slow_raw)
      spec_slow_raw->manually_increment( slow_raw);

    if(fast_raw)
      spec_fast_raw->manually_increment( fast_raw);

    if(time_raw )
      spec_time_raw->manually_increment( time_raw);

    if(owner->not_led())
      spec_slow_raw_not_led->manually_increment( slow_raw);
    if(owner->and_led())
      spec_slow_raw_and_led->manually_increment( slow_raw);




    if( time_raw &&  slow_raw )
    {

      matr_fast_vs_slow->manually_increment( slow_raw, fast_raw);
      matr_time_vs_slow->manually_increment( slow_raw, time_raw);


      // Adam invented this, but does he want it calibrated ?

      matr_fast_over_slow_vs_time->manually_increment(
        double(time_raw),
        (constant_factor_to_multiply_fast *  (fast_raw)) / (slow_raw)
      );
    }
    // CALIBRATION -----------------------------------------


    // NOTE: so far the option of non incrementing calibrated
    // spectra when raw data is zero - is:
    //   a) disabled in the dialog window (cracow)
    //   b) not used here
    // it seams thea the condition that both of time and slow
    // must be non zero (as above if(...) )
    // is stronger

    if(slow_raw)
    {
      slow_cal =
        (slow_calibr_factors[1]  *  (slow_raw + randomizer::randomek()))
        + slow_calibr_factors[0]  ;

      if(owner->not_led())spec_slow_cal_not_led->manually_increment(slow_cal) ;
      spec_total_slow_cal->manually_increment(slow_cal) ;
    }


    // fast---------------------------
    if(fast_raw)
    {
      fast_cal =
        (fast_calibr_factors[1] * (fast_raw  + randomizer::randomek()))
        + fast_calibr_factors[0];

      if(owner->not_led())spec_fast_cal_not_led->manually_increment(fast_cal) ;
      matr_fast_vs_slow_cal->manually_increment( slow_cal,fast_cal);

      // totals
      spec_total_fast_cal->manually_increment(fast_cal) ;
      matr_total_fast_vs_slow_cal->manually_increment( slow_cal, fast_cal);

    }

    // time

    if(time_raw)
    {
      if(time_calibr_factors.size() >= 2)
      {
        time_cal = (time_calibr_factors[1]  *
                    (time_raw + randomizer::randomek()))
                   + time_calibr_factors[0]  ;

        spec_time_cal->manually_increment(time_cal) ;
        matr_time_vs_slow_cal->manually_increment( slow_cal,time_cal);

        // totals
        spec_total_time_cal->manually_increment(time_cal) ;
        matr_total_time_vs_slow_cal->manually_increment( slow_cal, time_cal);
      }
    }

    // Setting the good data flag

    if(
      (flag_good_fast_requires_threshold == false
       ||
       fast_cal > fast_threshold )
      &&
      (flag_good_slow_requires_threshold == false
       ||
       slow_cal > slow_threshold )
      &&
      (flag_good_time_requires_threshold == false
       ||
       time_cal > good_time_threshold )
      &&
      (flag_good_requires_not_led == false
       ||
       owner->not_led())
    )
    {
      flag_good_data = true ;
    }

    if(owner->not_led())
    {
      slow_cal_not_led = slow_cal;
      fast_cal_not_led = fast_cal;
      time_cal_not_led = time_cal;


#if 0
      // adding th the ntuple
      TIFJAnalysis::fxCaliEvent-> hect_ener_slow_cal_not_led[det_nr-1] =
        slow_cal_not_led;
      // adding to the ntuple
      TIFJAnalysis::fxCaliEvent-> hect_ener_fast_cal_not_led[det_nr-1] =
        fast_cal_not_led;
      TIFJAnalysis::fxCaliEvent->hect_time_cal_not_led[det_nr-1] =
        time_cal_not_led;
      // above -1 is because numbers are 1-8 but ntuple leaves are 0-7
	
#endif
    }


    //    if(time_gate_low <= time_raw   // lower limit
    //       &&
    //       time_raw <= time_gate_high)
    //    {
    //      if(owner->not_led())
    //        spec_slow_cal_not_led_gated_time->manually_increment(slow_cal) ;
    //    }


#if 0
    //############################################################
    // now the doppler correction, according to the same algorithm as in Rising
    //############################################################


    // NOTE !!!
    // at first we have to ask target if the parameters of this event
    // fullfill the requirements which were set in the options
    // (for example we want exact tracking, while in this event
    // we can have only estimated).
    // if these requirements are not fullfilled - we do not make
    // the doppler correction, event is rejected



    switch(which_doppler_algorithm)
    {
      //    case polar_coordinates:
      //      if(Tcrystal::target_ptr->is_doppler_possible()) doppler_corrector_polar();
      //      break ;
    case tracked_algorithm:
      if(Tcrystal_xia::target_ptr->is_doppler_possible()) doppler_corrector_TVector3();
      break;

    case basic_algorithm:
      doppler_corrector_basic();
      break;

    default:

      cout
      << "Error, the type of doppler correction algorithm was not decided "
      "in the file: option/doppler.options\n" << endl ;
      break ;
    }


    //    doppler_corrector_basic();

    if(flag_doppler_successful)
    {

      spec_slow_doppler->manually_increment(slow_doppler);
      spec_fast_doppler->manually_increment(fast_doppler);

      spec_total_slow_doppler->manually_increment(slow_doppler);
      spec_total_fast_doppler->manually_increment(fast_doppler);


      matr_total_fast_vs_slow_doppler->
      manually_increment(slow_doppler, fast_doppler);
    }
    // .......
#endif
  } // end of all fast/slow/time  are not zero

}
//****************************************************************************
//****************************************************************************
bool  Thector_BaF::check_selfgate(Tself_gate_abstract_descr *desc)
{

  // here we assume that this is really germ
  Tself_gate_hec_descr *d = (Tself_gate_hec_descr*)desc ;
  if(d->enable_energy_fast_gate)
  {
    if(fast_cal_not_led < d->en_fast_gate[0]
        ||
        fast_cal_not_led > d->en_fast_gate[1])
      return false ;
  }


  if(d->enable_energy_slow_gate)
  {
    if(slow_cal_not_led < d->en_slow_gate[0]
        ||
        slow_cal_not_led > d->en_slow_gate[1])
      return false ;
  }


  if(d->enable_time_gate)
  {
    if(time_cal_not_led < d->time_gate[0]
        ||
        time_cal_not_led > d->time_gate[1])
      return false ;
  }

  //---------------
  if(d->enable_geom_theta_gate)
  {
    if(theta_geom < d->geom_theta_gate[0]
        ||
        theta_geom > d->geom_theta_gate[1])
      return false;
  }

  if(d->enable_geom_phi_gate)
  {
    if(phi_geom < d->geom_phi_gate[0]
        ||
        phi_geom > d->geom_phi_gate[1])
      return false;
  }

  //---------------
  if(d->enable_gp_theta_gate)
  {
    if(theta_radians_between_gamma_vs_scattered_particle <  d->gp_theta_gate[0]
        ||
        theta_radians_between_gamma_vs_scattered_particle > d->gp_theta_gate[1])
      return false ;
  }

  if(d->enable_gp_phi_gate)
  {
    if(phi_radians_between_gamma_vs_scattered_particle  < d->gp_phi_gate[0]
        ||
        phi_radians_between_gamma_vs_scattered_particle > d->gp_phi_gate[1])
      return false;
  }

#ifdef NIGDY
  //------------ so far calculation if this multiplicity was not yet implemented
  if(enable_mult_in_cluster_gate)
  {
    if( d->mult_in_cluster_gate[0]
        d->mult_in_cluster_gate[1]
      )
    }
#endif

  return true ;// when EVETHING was successful
}
//****************************************************************************
//****************************************************************************
/** this function contains the algorithm of the doppler correction
    prepared by Sumit  */
//****************************************************************************
#ifdef NIGDY
#endif // NIGDY
//*******************************************************************
void Thector_BaF::set_doppler_corr_options
(int typ_track, int typ_lycca_pos, int typ_doppl_algor)
{
  tracking_position_on_target = (type_of_tracking_requirements) typ_track;
  tracking_position_on_lycca   = (type_of_lycca_requirements) typ_lycca_pos;
  which_doppler_algorithm = (type_of_doppler_algorithm)  typ_doppl_algor ;
}

//***************************************************************
/** Based on the simplest algorithm (from Hans-Juergen) */
void Thector_BaF::doppler_corrector_basic()
{
#if 0
  // this algorithm below is on the web page ===================
  double beta = target_ptr->give_beta_after_target();

  double doppler_factor =    ( 1.0 - beta * cos(theta_geom_radians))    // <--- theta of this crystal
                             / (sqrt(1.0 - beta * beta));


  fast_doppler = fast_cal_not_led * doppler_factor ;
  slow_doppler = slow_cal_not_led * doppler_factor ;

  dcout << "==== Gamma registered during this event =================================\n"
  << name_of_this_element
  << " (Geometrical phi " << phi_geom << "[deg],  rad= " << phi_geom_radians
  << " theta " << theta_geom << " [deg],  rad= " << theta_geom_radians << ")\n"

  << "\n: Gamma vector   \n\tdetector.x = " << detector.X()
  << ", (because crystal_position_x = " << crystal_position_x
  << " -  target x = " << target_ptr->give_x_tracked()
  << ")\n\tyv1 = " <<  detector.Y()
  << ", (because crystal_position_y = " << crystal_position_y
  << " -  target y = " << target_ptr->give_y_tracked()
  << ")\n\tzv1 = " <<  detector.Z()
  << ", (because  crystal_position_z = " << crystal_position_z
  //<< " -  target z = " << target_ptr->give_z()
  << ")" << endl ;


  dcout << "Doppler_factor = " <<  doppler_factor << endl;
  flag_doppler_successful = true ;
#endif 
}
//****************************************************************

/** this function contains the algorithm of the doppler correction
using the class TVector3 from ROOT library */
void Thector_BaF::doppler_corrector_TVector3()
{

#if 0

  //detector.SetMagThetaPhi(1, theta_geom_radians, phi_geom_radians);

  detector.SetXYZ(  crystal_position_x - target_ptr->give_x_tracked() ,
                    crystal_position_y - target_ptr->give_y_tracked() ,
                    crystal_position_z) ;


  dcout << "==== Gamma registered during this event =================================\n"
  << name_of_this_element
  << " (Geometrical phi " << phi_geom << "[deg],  rad= " << phi_geom_radians
  << " theta " << theta_geom << " [deg],  rad= " << theta_geom_radians << ")\n"
  << "\n: Gamma vector   \n\tdetector.x = " << detector.X()
  << ", (because crystal_position_x = " << crystal_position_x
  << " -  target x = " << target_ptr->give_x_tracked()
  << ")\n\tyv1 = " <<  detector.Y()
  << ", (because crystal_position_y = " << crystal_position_y
  << " -  target y = " << target_ptr->give_y_tracked()
  << ")\n\tzv1 = " <<  detector.Z()
  << ", (because  crystal_position_z = " << crystal_position_z
  //<< " -  target z = " << target_ptr->give_z()
  << ")" << endl ;


  theta_radians_between_gamma_vs_scattered_particle = detector.Angle(target_ptr->outgoing);
  double cos_theta_gamma_scattered_particle = cos(theta_radians_between_gamma_vs_scattered_particle);

  double beta = target_ptr->give_beta_after_target();

  dcout
  << "TVector3  cos_theta_gamma_scattered_particle " << cos_theta_gamma_scattered_particle
  << ", \ntheta_radians_between_gamma_vs_scattered_particle "
  << theta_radians_between_gamma_vs_scattered_particle
  << "\n, Beta = " << beta << endl;


  // using Alex way -------------------
  if(beta > 0.05  &&   beta < 1.0)
  {
    double doppler_factor=  ( 1.0 - (beta * cos_theta_gamma_scattered_particle) )
                            / (sqrt(1.0 - (beta * beta) ));

    dcout << "Doppler factor = " << doppler_factor << endl;

    // Now using this factor

    fast_doppler = fast_cal_not_led * doppler_factor;

    slow_doppler = slow_cal_not_led * doppler_factor ;


    flag_doppler_successful = true ;
  }
  else
  {
    flag_doppler_successful = false ;
  }
#endif

}
#endif  // if hector present


