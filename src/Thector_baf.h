/********** *****************************************************************
						thector_general_detector.h  -  description
						-------------------
						begin                : Wed May 21 2003
						copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
						email                : jerzy.grebosz@ifj.edu.pl
***************************************************************************/


#ifndef THECTOR_BAF_H
#define THECTOR_BAF_H

#include "experiment_def.h" 
#ifdef HECTOR_PRESENT


#include "Thector.h"

// #include <TVector3.h>
#include <Vectors.h>
#include "Tincrementer_donnor.h"

#include "doppler_enums.h"
class Ttarget ;

/////////////////////////////////////////////////////////////////////////////////////////////////
class Thector_BaF : public Tincrementer_donnor
{
  friend class Thector ;
protected:
  Thector *owner ;
 static Ttarget * target_ptr ;    // for faster acces of the doppler correction
     
  bool flag_fired ;
  bool flag_good_data ;
  
  const int det_nr ;


  string name_of_this_element ;

  int slow_raw, fast_raw, time_raw;   // raw values
  double slow_cal, fast_cal, time_cal ; // callibrated values
  double slow_cal_not_led, fast_cal_not_led, time_cal_not_led ; // calibrated values
  double slow_doppler, fast_doppler ;   // doppler corrected values

  //===== position of the crystal ===============
  double phi_geom, theta_geom ;
  double
		phi_geom_radians,
		theta_geom_radians ;


  double distance_from_target ; // from the target

  double crystal_position_x, crystal_position_y, crystal_position_z ;
  bool flag_doppler_successful ;

  
  double theta_radians_between_gamma_vs_scattered_particle;
  double phi_radians_between_gamma_vs_scattered_particle;

	
  vector<double> slow_calibr_factors ;
  vector<double> fast_calibr_factors ;

//  int time_gate_low ;
//  int time_gate_high ;

     
  vector<double> time_calibr_factors ;   	// Adam does not want to calibrate
	// the times

  int channel_nr_of_slow;
  int channel_nr_of_fast;
  int channel_nr_of_time;
	
	
  // pointers to the data inside the event
  int16_t *ptr_to_slow ;
  int16_t *ptr_to_fast ;
  int16_t *ptr_to_time ;

  // spectra ----------------------------------
  spectrum_1D
    *spec_slow_raw,
    *spec_slow_raw_not_led,
    *spec_slow_raw_and_led,

    *spec_fast_raw,
    *spec_time_raw,
		
    *spec_slow_cal_not_led,
    *spec_fast_cal_not_led,

    *spec_slow_cal_not_led_gated_time;

//     *spec_slow_doppler,
//     *spec_fast_doppler;

  spectrum_2D
    * matr_fast_vs_slow,  
    * matr_fast_vs_slow_cal,
    * matr_fast_over_slow_vs_time,           // Adam invented this   
    * matr_time_vs_slow;
		
  spectrum_1D * spec_time_cal;
  spectrum_2D   *matr_time_vs_slow_cal;
	
  // pointers to the Total spectra, defined inside the hector object, and
  // shared by all BaF 		

  // warning, to not set this pointers in the constructor (or in
  // create_my_spectra) that would be too early, because the costructor of 
  // Hector runs later than the consructor of BaF
  spectrum_1D	*spec_total_slow_cal,
    *spec_total_fast_cal;
//     *spec_total_slow_doppler,
//     *spec_total_fast_doppler;

  spectrum_2D	*matr_total_fast_vs_slow_cal;
//     *matr_total_fast_vs_slow_doppler;

  spectrum_1D	*spec_total_time_cal;
  spectrum_2D	*matr_total_time_vs_slow_cal;


				
public:

  // these arguments nr_slow, nr_fast tell in which channel we send this data
  // hector_adc[32], hector_tdc[32]
  // for example  BaF nr 6 can get data from adc[2], adc[14], tdc[8]

  Thector_BaF(Thector*ptr_owner, int nr_det,
							int nr_fast, int nr_slow, int nr_time);
				
  ~Thector_BaF();
	
  vector< spectrum_abstr*>*  hector_spectra_ptr()
    {
			return owner->give_spectra_pointer() ;
    }
  /**  the most important fuction,  executed for every event */
	  
  /** readinng calibration, gates, setting pointers */
  void make_preloop_action(ifstream & );
  void analyse_current_event() ;

	
  /** where in the event are data for  this particular BaF*/
  void set_event_data_pointers(int16_t * hector_adc_ptr, int16_t *hector_tdc_ptr)
    {
			ptr_to_slow = &hector_adc_ptr[channel_nr_of_slow];
			ptr_to_fast = &hector_adc_ptr[channel_nr_of_fast];
			ptr_to_time = &hector_tdc_ptr[channel_nr_of_time];
    }

	bool  check_selfgate(Tself_gate_abstract_descr *desc);

	  static void set_doppler_corr_options
					(int typ_track, int typ_lycca_pos, int typ_doppl_algor);


	
  /** this function contains the algorithm of the doppler correction
using the class TVector3 from ROOT library */
  void doppler_corrector_TVector3();

//TVector3 detector ;
Vector3D detector ;


protected:
  void create_my_spectra() ;
  void doppler_corrector_basic();

  double constant_factor_to_multiply_fast  ;
  
  /** does the user require some thereshold value? */
  static bool flag_good_fast_requires_threshold;
/** does the user require some thereshold value? */
  static bool flag_good_slow_requires_threshold;
/** does the user require some thereshold value? */
  static bool flag_good_time_requires_threshold;

/** does the user require not led situation? */
  static bool flag_good_requires_not_led;

	/**------- En fast*/
  static int  fast_threshold ;
	/**------- En slow*/
  static int    slow_threshold;
	/**------- time */
  static int   good_time_threshold  ;

  static bool  flag_increment_fast_cal_with_zero;
  static bool   flag_increment_slow_cal_with_zero;
  static bool   flag_increment_time_cal_with_zero;

  	// option for doppler correction
  static type_of_tracking_requirements    tracking_position_on_target ;
  static type_of_lycca_requirements        tracking_position_on_lycca ;
  static type_of_doppler_algorithm        which_doppler_algorithm ;
  static bool flag_beta_from_frs ;     // which beta to use, frs or fixed
  static double beta_fixed_value ;     // if fixed, here it is
  static double energy_deposit_in_target ;// bec. beta on the other side of the target
  // will be slower
  
  
};
//////////////////////////////////////////////////////////////////////////////


#endif  // HECTOR_PRESENT

#endif
