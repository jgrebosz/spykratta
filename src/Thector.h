#ifndef _THECTOR_H_
#define _THECTOR_H_


#include "spectrum.h"
#include <vector>
#include <fstream>

#include "TIFJEvent.h"
#include "TIFJCalibratedEvent.h"

//#ifdef HECTOR_PRESENT


#include "Tincrementer_donnor.h"
//#include "Tone_signal_module.h"

#define  HECTOR_CALIBRATE_TIME    1
#include "Tfrs_element.h"

#include "Thector_baf.h"
template <typename TownerClass>
class Thector_BaF ;
///////////////////////////////////////////////////////////////////////////////
class Thector : public Tfrs_element, public Tincrementer_donnor
{
		// to give access to pointers to the total spectra

        using TmyBaF = Thector_BaF<Thector>;
        friend TmyBaF;


        int hector_ADC_channel[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS] ;
        int hector_TDC_channel[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS] ;
        int phoswich_ADC_channel[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS] ;
        int phoswich_TDC_channel[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS] ;


		//-------------------------------
		// elements of the hector
        vector< TmyBaF *  > BaF ;    // BaF of hector

        // vector<Tone_signal_module*> plastic_time;
        vector< TmyBaF * > phoswich_BaF ;    // phoswich

//        vector<Tone_signal_module*> digitizer_channels;
//        vector<TmyBaF *> digitizer_detector;


        //TIFJEvent* tmp_event_unpacked = nullptr;


        // spectrum_1D *spec_hec_synchro ;
        // spectrum_1D *spec_RF_raw;



        spectrum_1D
        *spec_total_slow_cal,
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

        Thector(string name);
		~Thector();
		void create_my_spectra() ;

		// function to get the pointer to list of the spectra
//		vector<spectrum_abstr*> *  address_spectra_hector()
//		{
//			return &spectra_hector ;
//		}

		//------------------------------
//		void make_preloop_action ( TIFJEvent* event_unpacked ,
//		                           TIFJCalibratedEvent* event_calibrated );
        void make_preloop_action(ifstream &);

		// taking the calibration factors
		void read_calibration_and_gates_from_disk();
		// above function is called form preloop function


        // will be called from Event Processor
        static void read_lookup_info_from_disk(int *HECTOR_ADC_GEO, int* HECTOR_TDC_GEO,
                                               int *PHOSWITCH_ADC_GEO, int *PHOSWITCH_TDC_GEO);
        //static void read_lookup_info_from_disk(int *HECTOR_ADC_GEO, int* HECTOR_TDC_GEO);


		//void make_step1_action();
		void analyse_current_event();
		void make_user_event_action(); // shop of events ?
		void make_postloop_action();  // saving the spectra ?

//		vector< spectrum_abstr*>*  give_spectra_pointer()
//		{ return &spectra_hector ; }

		/** No descriptions */
        //bool save_selected_spectrum ( string name );
//		bool and_led() { return marker_led_logical ;}
//		bool not_led() { return !marker_led_logical ;}
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

        spectrum_1D *spec_tdc_ch[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS] ;
		/**  */
        spectrum_1D *spec_adc_ch[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS] ;

        vector<spectrum_1D*>  spec_phoswich_tdc_ch;
        vector<spectrum_1D*>  spec_phoswich_adc_ch;

		/** for user defined source of the LED signal*/
//		int led_adc_nr,
//		led_tdc_nr,
//		led_adc_low,
//		led_adc_high,
//		led_tdc_low,
//		led_tdc_high;

 int multiplicity;
        spectrum_1D	*spec_muliplicity;
};
////////////////////////////////////////////////////////////////


//#endif  // HECTOR_PRESENT


#endif // _THECTOR_H_
