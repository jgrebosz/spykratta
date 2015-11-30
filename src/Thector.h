#ifndef _THECTOR_H_
#define _THECTOR_H_


#include "spectrum.h"
#include <vector>

#include "TIFJEvent.h"
#include "TIFJCalibratedEvent.h"

#ifdef HECTOR_PRESENT


#include "Tincrementer_donnor.h"

#define  HECTOR_CALIBRATE_TIME    1

class Thector_BaF ;
///////////////////////////////////////////////////////////////////////////////
class Thector : public Tincrementer_donnor
{
		// to give access to pointers to the total spectra
		friend class Thector_BaF ;
		bool marker_led_logical ;
		bool marker_eb_logical ;
		bool marker_led2_logical ;
		bool marker_cate_logical ;

		int marker_led_analog ;

		// for synchronisation purpose
		int hector_synchro_data;

		int hector_ADC_channel[32] ;
		int hector_TDC_channel[32] ;



		string name_of_this_element ;
		// all pointers to spectra for hector cristals and clusters
		// are registered also here - to make easier saving them
		// from time to time
		vector<spectrum_abstr*> spectra_hector ;

		vector<spectrum_abstr*> spectra_markers ;

		//-------------------------------
		// elements of the hector
		vector< Thector_BaF* > BaF ;    // BAF


		TIFJEvent* tmp_event_unpacked;


		spectrum_1D *spec_hec_synchro ;



		spectrum_1D	*spec_total_slow_cal,
		*spec_total_fast_cal,

		*spec_total_slow_doppler,
		*spec_total_fast_doppler;

		spectrum_2D	*matr_total_fast_vs_slow_cal,
		*matr_total_fast_vs_slow_doppler;


		//#ifdef HECTOR_CALIBRATE_TIME
		spectrum_1D   *spec_total_time_cal;
		spectrum_2D	*matr_total_time_vs_slow_cal;
		//#endif

		spectrum_1D *spec_statistics ; // which detectors fired


	public:

		Thector();
		~Thector();
		void create_my_spectra() ;

		// function to get the pointer to list of the spectra
		vector<spectrum_abstr*> *  address_spectra_hector()
		{
			return &spectra_hector ;
		}

		//------------------------------
		void make_preloop_action ( TIFJEvent* event_unpacked ,
		                           TIFJCalibratedEvent* event_calibrated );

		// taking the calibration factors
		void read_calibration_and_gates_from_disk();
		// above function is called form preloop function

		//void make_step1_action();
		void analyse_current_event();
		void make_user_event_action(); // shop of events ?
		void make_postloop_action();  // saving the spectra ?

		vector< spectrum_abstr*>*  give_spectra_pointer()
		{ return &spectra_hector ; }

		/** No descriptions */
		bool save_selected_spectrum ( string name );
		bool and_led() { return marker_led_logical ;}
		bool not_led() { return !marker_led_logical ;}
		/** No descriptions */
		int  give_synchro_data() ;
		//  /** For matrix produced in rising object */
		//  int give_time_BaF(int nr);

		//------------------------------
	protected:

		//for testing time
		void simulate_event();
		/** result of dialog = what means GOOD event */
		void read_good_crystal_specification();

		spectrum_1D *spec_tdc_ch[32] ;
		/**  */
		spectrum_1D *spec_adc_ch[32] ;

		/** for user defined source of the LED signal*/
		int led_adc_nr,
		led_tdc_nr,
		led_adc_low,
		led_adc_high,
		led_tdc_low,
		led_tdc_high;


};
////////////////////////////////////////////////////////////////

#endif  // HECTOR_PRESENT


#endif // _THECTOR_H_
