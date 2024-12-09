#ifndef _TPARIS_H_
#define _TPARIS_H_


#include "spectrum.h"
#include <vector>
#include <fstream>

#include "TIFJEvent.h"
#include "TIFJCalibratedEvent.h"

//#ifdef HECTOR_PRESENT


#include "Tincrementer_donnor.h"
#include "Tone_signal_module.h"

#define  HECTOR_CALIBRATE_TIME    1
#include "Tfrs_element.h"

#include "Thector_baf.h"
template <typename TownerClass>
class Thector_BaF ;
///////////////////////////////////////////////////////////////////////////////
class Tparis : public Tfrs_element, public Tincrementer_donnor
{
		// to give access to pointers to the total spectra
        using TmyBaF = Thector_BaF<Tparis>;
        friend TmyBaF ;


        vector<Tone_signal_module<double> *> digitizer_channels;
        vector<TmyBaF *> digitizer_detector;


        spectrum_1D
        *spec_total_slow_cal,
        *spec_total_fast_cal;

//        *spec_total_slow_doppler,
//        *spec_total_fast_doppler
        ;

        spectrum_2D	*matr_total_fast_vs_slow_cal;
//        *matr_total_fast_vs_slow_doppler


        spectrum_1D *spec_total_time_cal;
        spectrum_2D	*matr_total_time_vs_slow_cal;

		spectrum_1D *spec_statistics ; // which detectors fired


	public:

        Tparis(string name);
        ~Tparis();
		void create_my_spectra() ;


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

		/** No descriptions */
		bool save_selected_spectrum ( string name );
		/** No descriptions */
		int  give_synchro_data() ;

		//------------------------------
	protected:

		//for testing time
		void simulate_event();
		/** result of dialog = what means GOOD event */
		void read_good_crystal_specification();



 int multiplicity;
        spectrum_1D	*spec_muliplicity;
};
////////////////////////////////////////////////////////////////


//#endif  // HECTOR_PRESENT


#endif // _THECTOR_H_
