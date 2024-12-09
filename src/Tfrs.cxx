#include "Tfrs.h"

#include <dirent.h> // for scandir
#include <malloc.h> // for scandir
#include <fstream>

#include <sstream>
using namespace std;

#include <ctime>


#include "Tcracow_exception.h"
#include "Tone_signal_module.h"
#include "Ttwo_signal_module.h"
#include "T32_signal_module.h"
#include "Tone_signal_pattern_module.h"
#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h"                   // for verbose mode flag
#include <algorithm>                                // for toupper

//#include "Tmultiwire.h"
#include "Tmicro_channel_plate.h"
#include "Tpisolo_delta_e.h"
#include "Tpisolo_si_final.h"
#include "Tpisolo_tof.h"

#include "Texotic_strip_detector.h"

#include "Thector.h"
#include "Tparis.h"
#include "Tkratta.h"
#include "Thec_kratta_tree.h"
#include "Tn_silicon_xy.h"

#include "TprismaManager.h"

int randomizer::randomizer_word  ;  // static

//*******************************************************
//*******************************************************
//         FRS class
//*******************************************************
//*******************************************************

Tfrs::Tfrs(TIFJAnalysis *ptr, std::string name)               // constructor
{
    // global
    RisingAnalysis_ptr = ptr;
    official_name = name;

    // filling the vector with the proper configuration
    // it must be here in the constructor, because they have
    // spectra, which have to be alive always

    // common to all of Tfrs_elements (to know where is
    //their register of the spectra)
    Tfrs_element::owner = this ;

    //---------------------------------------------
    // constructing the current setup of frs
    //---------------------------------------------

#ifdef     KRATTA_PRESENT
    module.push_back(kratta_ptr = new Tkratta("kratta"));



#if CERN_ROOT_INSTALLED == true
    module.push_back(new Thec_kratta_tree("hec_kratta_tree",
                                          &TIFJEvent::hector_tdc,
                                          &TIFJEvent::hector_adc,
                                          &TIFJEvent::phoswich_tdc,
                                          &TIFJEvent::phoswich_adc,
                                          &TIFJEvent::kratta,
                                          &TIFJEvent::plastic)
                    );
#endif // #if CERN_ROOT_INSTALLED == true

#endif

    // module.push_back((new Thector("hector") ));
    module.push_back((new Tparis("paris")));

    module.push_back(new Tn_silicon_xy<16> ("drac0", 0) );
    module.push_back(new Tn_silicon_xy<16> ("drac1", 1) );
    module.push_back(new Tn_silicon_xy<16> ("drac2", 2) );
    module.push_back(new Tn_silicon_xy<16> ("drac3", 3) );

#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT

// all particular detectors and calculators are insind of this object
    module.push_back(new TprismaManager("prisma"));

#endif

    // at first we define scalers object to collect
    // their "spectra"/ their graphs

    // this scaler gives the time information





#ifdef NR_OF_EXOTIC_MODULES
    for(int i = 0 ; i <  NR_OF_EXOTIC_MODULES / 2 ; i++)      // because one is having  X and Y plates
    {
        string dname("exotic_");
        dname = dname + char('A' + i) ;

        module.push_back(new Texotic_strip_detector(dname, i,
                         &TIFJEvent::exotic_data,
                         &TIFJEvent::exotic_data_fired
                                                   ));
    }
#endif // NR_OF_EXOTIC_MODULES

#if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
    module.push_back(new T32_signal_module<32> ("caen_v785", (&TIFJEvent::v785),
                     false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                     true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                     false));    //    bool flag_we_want_calibrate_signals_arg
    /*
    module.push_back ( new T32_signal_module<32> ( "v785_ch01", ( &TIFJEvent::v785_ch01 ),
                         false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                         true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                         false ) );  //    bool flag_we_want_calibrate_signals_arg
    module.push_back ( new T32_signal_module<32> ( "v785_ch02", ( &TIFJEvent::v785_ch02 ),
                         false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                         true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                         false ) );  //    bool flag_we_want_calibrate_signals_arg
    module.push_back ( new T32_signal_module<32> ( "v785_ch03", ( &TIFJEvent::v785_ch03 ),
                         false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                         true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                         false ) );  //    bool flag_we_want_calibrate_signals_arg
    module.push_back ( new T32_signal_module<32> ( "v785_ch04", ( &TIFJEvent::v785_ch04 ),
                         false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                         true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                         false ) );  //    bool flag_we_want_calibrate_signals_arg
    module.push_back ( new T32_signal_module<32> ( "v785_ch05", ( &TIFJEvent::v785_ch05 ),
                         false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                         true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                         false ) );  //    bool flag_we_want_calibrate_signals_arg
    */
#endif // if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT


#ifdef GSI_STUFF

    read_interesting_zet_aoq_range();


#ifdef STARE_PARTS_OF_FRS

#if (CURRENT_EXPERIMENT_TYPE ==RISING_FAST_BEAM_CAMPAIGN)

    module.push_back(new
                     Tscaler("scaler_seconds_1Hz_clock",
                             &TIFJEvent::scaler_ch_5_seconds,
                             &TIFJEvent::scaler_ch_5_seconds));
    // the other scalers use this time information

    module.push_back(scal_seetram =
                         new
    Tscaler("scaler_seetram_digitizer",
            &TIFJEvent::scaler_ch_3_seetram_digitizer,
            &TIFJEvent::scaler_ch_5_seconds));
#endif

#if (CURRENT_EXPERIMENT_TYPE ==RISING_FAST_BEAM_CAMPAIGN)

    module.push_back(scal_free_trig =
                         new
    Tscaler("scaler_free_triggers",
            &TIFJEvent::scaler_ch_1_free_triggers,
            &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_accepted_triggers",
                             &TIFJEvent::scaler_ch_2_accepted_triggers,
                             &TIFJEvent::scaler_ch_5_seconds));



    module.push_back(new
                     Tscaler("scaler_spills",
                             &TIFJEvent::scaler_ch_4_spills,
                             &TIFJEvent::scaler_ch_5_seconds));


    module.push_back(new
                     Tscaler("scaler_sci01_free",
                             &TIFJEvent::scaler_ch_6_sc01_free,
                             &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_sci21_free",
                             &TIFJEvent::scaler_ch_7_sc21_free,
                             &TIFJEvent::scaler_ch_5_seconds));



    module.push_back(scal_sc41 =
                         new
    Tscaler("scaler_sci41_free",
            &TIFJEvent::scaler_ch_8_sc41_free,
            &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_mw11_free",
                             &TIFJEvent::scaler_ch_9_mw11_anode_free,
                             &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_mw21_free",
                             &TIFJEvent::scaler_ch_10_mw21_anode_free,
                             &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_mw31_free",
                             &TIFJEvent::scaler_ch_11_mw31_anode_free,
                             &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_mw41_free",
                             &TIFJEvent::scaler_ch_12_mw41_anode_free,
                             &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_1MHz_clock",
                             &TIFJEvent::scaler_1MHz,
                             &TIFJEvent::scaler_ch_5_seconds));


    module.push_back(new
                     Tscaler_dt("scaler_dead_time",
                                &TIFJEvent::
                                scaler_ch_2_accepted_triggers,
                                &TIFJEvent::scaler_ch_1_free_triggers,
                                &TIFJEvent::scaler_ch_5_seconds));
#endif // FAST beam


    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################

    // ------------- new scalers for Nele --------------------------
    module.push_back(new
                     Tscaler("scaler_stopped_beam_trigger",
                             &TIFJEvent::scaler_stopped_beam_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_frs_singles_trigger",
                             &TIFJEvent::scaler_frs_singles_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_time_calibrator_trigger",
                             &TIFJEvent::scaler_time_calibrator_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_random_trigger",
                             &TIFJEvent::scaler_random_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_fast_beam_trigger",

                             &TIFJEvent::scaler_fast_beam_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_time_calibrator_analog_with_TAC",
                             &TIFJEvent::scaler_time_calibrator_analog_with_TAC,
                             &TIFJEvent::scaler_ch_5_seconds));

    // scaleres FRS crate
    module.push_back(new
                     Tscaler("scaler_frs_free_trigger",
                             &TIFJEvent::scaler_frs_free_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_accepted_trigger",
                             &TIFJEvent::scaler_frs_accepted_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_IC_old_dig",
                             &TIFJEvent::scaler_frs_IC_old_dig,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_seetram_old_dig",
                             &TIFJEvent::scaler_frs_seetram_old_dig,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_seetram_new_dig",
                             &TIFJEvent::scaler_frs_seetram_new_dig,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_IC_new_dig",
                             &TIFJEvent::scaler_frs_IC_new_dig,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_spill_counter",
                             &TIFJEvent::scaler_frs_spill_counter,
                             &TIFJEvent::scaler_ch_5_seconds));
    //module.push_back(new
    //                   Tscaler("scaler_frs_1Hz_clock",
    //                           &TIFJEvent::scaler_frs_1Hz_clock,
    //                           &TIFJEvent::scaler_ch_5_seconds));

    module.push_back(new
                     Tscaler("scaler_frs_1Hz_clock",
                             &TIFJEvent::scaler_frs_1Hz_clock,
                             &TIFJEvent::scaler_frs_1Hz_clock));


    module.push_back(new
                     Tscaler("scaler_frs_10Hz_clock",
                             &TIFJEvent::scaler_frs_10Hz_clock,
                             &TIFJEvent::scaler_frs_1Hz_clock));
    module.push_back(new
                     Tscaler("scaler_frs_start_extraction",
                             &TIFJEvent::scaler_frs_start_extraction,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_stop_extraction",
                             &TIFJEvent::scaler_frs_stop_extraction,
                             &TIFJEvent::scaler_ch_5_seconds));



    module.push_back(new
                     Tscaler("scaler_frs_sc42_left_or_right",
                             &TIFJEvent::scaler_frs_sc42_left_or_right,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_sc42_left",
                             &TIFJEvent::scaler_frs_sc42_left,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_sc42_right",
                             &TIFJEvent::scaler_frs_sc42_right,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_frs_sc43_left",
                             &TIFJEvent::scaler_frs_sc43_left,
                             &TIFJEvent::scaler_ch_5_seconds));
#endif  // G-factor experiment



    //#if (CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN
#ifdef  WE_WANT_FRS_SCALERS

    // here you have the choice to make more precisely
#define clocking  scaler_frs_1Hz_clock
    // #define clocking  scaler_frs_10Hz_clock

    // scalers FRS crate
    /* module.push_back(new
                      Tscaler("scaler_frs_free_trigger",
                              &TIFJEvent::scaler_frs_free_trigger,
                              &TIFJEvent::clocking));*/

    module.push_back(new
                     Tscaler("scaler_frs_accepted_trigger",
                             &TIFJEvent::scaler_frs_accepted_trigger,
                             &TIFJEvent::clocking));

    module.push_back(new
                     Tscaler_dt("scaler_live_time",
                                &TIFJEvent::scaler_frs_accepted_trigger,
                                &TIFJEvent::scaler_frs_free_trigger,
                                &TIFJEvent::clocking));

    module.push_back(new
                     Tscaler("scaler_frs_IC_old_dig",
                             &TIFJEvent::scaler_frs_IC_old_dig,
                             &TIFJEvent::clocking));
    module.push_back(new
                     Tscaler("scaler_frs_seetram_old_dig",
                             &TIFJEvent::scaler_frs_seetram_old_dig,
                             &TIFJEvent::clocking));
    module.push_back(new
                     Tscaler("scaler_frs_seetram_new_dig",
                             &TIFJEvent::scaler_frs_seetram_new_dig,
                             &TIFJEvent::clocking));
    module.push_back(new
                     Tscaler("scaler_frs_IC_new_dig",
                             &TIFJEvent::scaler_frs_IC_new_dig,
                             &TIFJEvent::clocking));
    module.push_back(new
                     Tscaler("scaler_frs_spill_counter",
                             &TIFJEvent::scaler_frs_spill_counter,
                             &TIFJEvent::clocking));

    module.push_back(new
                     Tscaler("scaler_frs_1Hz_clock",
                             &TIFJEvent::scaler_frs_1Hz_clock,
                             &TIFJEvent::scaler_frs_1Hz_clock));


    module.push_back(new
                     Tscaler("scaler_frs_10Hz_clock",
                             &TIFJEvent::scaler_frs_10Hz_clock,
                             &TIFJEvent::clocking));
    module.push_back(new
                     Tscaler("scaler_frs_start_extraction",
                             &TIFJEvent::scaler_frs_start_extraction,
                             &TIFJEvent::clocking));
    module.push_back(new
                     Tscaler("scaler_frs_stop_extraction",
                             &TIFJEvent::scaler_frs_stop_extraction,
                             &TIFJEvent::clocking));

    //                                        ------------
    module.push_back(new
                     Tscaler("scaler_frs_sc21",
                             &TIFJEvent::scaler_frs_sc21,
                             &TIFJEvent::clocking));
    module.push_back(new
                     Tscaler("scaler_frs_sc41",
                             &TIFJEvent::scaler_frs_sc41,
                             &TIFJEvent::clocking));
    module.push_back(new
                     Tscaler("scaler_frs_sc01",
                             &TIFJEvent::scaler_frs_sc01,
                             &TIFJEvent::clocking));
#endif

#if  ( (CURRENT_EXPERIMENT_TYPE != RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
&& (CURRENT_EXPERIMENT_TYPE != RISING_ACTIVE_STOPPER_APRIL_2008) \
&& (CURRENT_EXPERIMENT_TYPE != RISING_ACTIVE_STOPPER_JULY_2008) \
&& (CURRENT_EXPERIMENT_TYPE != RISING_ACTIVE_STOPPER_100TIN ))
    // some old scalers - I do not know from which experiment

#ifdef NEVER
    // Trigger scalers ------------------------------------------------
    module.push_back(new
                     Tscaler("scaler_trigger_sc41",
                             &TIFJEvent::scaler_trigger_sc41,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_OR_triggers",
                             &TIFJEvent::scaler_trigger_OR_triggers,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_OR_triggers_singles_DGF",
                             &TIFJEvent::scaler_trigger_OR_triggers_singles_DGF,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_OR_triggers_singles_VXI",
                             &TIFJEvent::
                             scaler_trigger_OR_triggers_singles_VXI,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_fast_clear",
                             &TIFJEvent::scaler_trigger_fast_clear,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_singles_sc41",
                             &TIFJEvent::scaler_singles_sc41,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_singles_reduced_FRS_singles",
                             &TIFJEvent::
                             scaler_singles_reduced_FRS_singles,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_time_calibrator",
                             &TIFJEvent::scaler_trigger_time_calibrator,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_random_trigger",
                             &TIFJEvent::scaler_trigger_random_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_fast_beam_trigger",
                             &TIFJEvent::
                             scaler_trigger_fast_beam_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_stopped_beam_trigger",
                             &TIFJEvent::
                             scaler_trigger_stopped_beam_trigger,
                             &TIFJEvent::scaler_ch_5_seconds));
    module.push_back(new
                     Tscaler("scaler_trigger_OR_tgammas",
                             &TIFJEvent::scaler_trigger_OR_tgammas,
                             &TIFJEvent::scaler_ch_5_seconds));
#endif // NEVER
#endif  // some old stuff



#if  ((CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN)\
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) \
|| (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))

    prepare_scalers_for_active_stopper();
#endif


#endif // ifdef STARE_PARTS_OF_FRS


    //====================================================
    // ------------- Multiwire Chambers ------------------
    //====================================================


    module.push_back(new
                     Tmultiwire("mw11",          // distance not used
                                &TIFJEvent::mw11_x_left,
                                &TIFJEvent::mw11_x_right,
                                &TIFJEvent::mw11_y_up  ,
                                &TIFJEvent::mw11_y_down,
                                &TIFJEvent::mw11_anode
                               ));



    module.push_back(new
                     Tmultiwire("mw21",          // was: 592.0,
                                &TIFJEvent::mw21_x_left,
                                &TIFJEvent::mw21_x_right,
                                &TIFJEvent::mw21_y_up  ,
                                &TIFJEvent::mw21_y_down,
                                &TIFJEvent::mw21_anode
                               ));


    module.push_back(new
                     Tmultiwire("mw22",
                                &TIFJEvent::mw22_x_left,
                                &TIFJEvent::mw22_x_right,
                                &TIFJEvent::mw22_y_up  ,
                                &TIFJEvent::mw22_y_down,
                                &TIFJEvent::mw22_anode
                               )) ;

    module.push_back(new
                     Tmultiwire("mw31",
                                &TIFJEvent::mw31_x_left,
                                &TIFJEvent::mw31_x_right,
                                &TIFJEvent::mw31_y_up  ,
                                &TIFJEvent::mw31_y_down,
                                &TIFJEvent::mw31_anode
                               )) ;


    module.push_back(mw41_ptr = new
    Tmultiwire("mw41",
               &TIFJEvent::mw41_x_left,
               &TIFJEvent::mw41_x_right,
               &TIFJEvent::mw41_y_up ,
               &TIFJEvent::mw41_y_down,
               &TIFJEvent::mw41_anode
              )) ;

    module.push_back(mw42_ptr = new
    Tmultiwire("mw42",
               &TIFJEvent::mw42_x_left,
               &TIFJEvent::mw42_x_right,
               &TIFJEvent::mw42_y_up  ,
               &TIFJEvent::mw42_y_down,
               &TIFJEvent::mw42_anode
              )) ;
#ifdef NIGDY

    module.push_back(new
                     Tmultiwire("mw43",
                                &TIFJEvent::mw43_x_left,
                                &TIFJEvent::mw43_x_right,
                                &TIFJEvent::mw43_y_up ,
                                &TIFJEvent::mw43_y_down,
                                &TIFJEvent::mw43_anode
                               )) ;
#endif // NIGDY



#ifdef MW51_USED

    module.push_back(new
                     Tmultiwire("mw51",
                                &TIFJEvent::mw51_x_left,
                                &TIFJEvent::mw51_x_right,
                                &TIFJEvent::mw51_y_up ,
                                &TIFJEvent::mw51_y_down,
                                &TIFJEvent::mw51_anode
                               )) ;
#endif
#ifdef MW71_USED

    module.push_back(new
                     Tmultiwire("mw71",
                                &TIFJEvent::mw71_x_left,
                                &TIFJEvent::mw71_x_right,
                                &TIFJEvent::mw71_y_up ,
                                &TIFJEvent::mw71_y_down,
                                &TIFJEvent::mw71_anode
                               )) ;
#endif

#ifdef MW81_USED

    module.push_back(new
                     Tmultiwire("mw81",
                                &TIFJEvent::mw81_x_left,
                                &TIFJEvent::mw81_x_right,
                                &TIFJEvent::mw81_y_up ,
                                &TIFJEvent::mw81_y_down,
                                &TIFJEvent::mw81_anode
                               )) ;
#endif

#ifdef MW82_USED

    module.push_back(new
                     Tmultiwire("mw82",
                                &TIFJEvent::mw82_x_left,
                                &TIFJEvent::mw82_x_right,
                                &TIFJEvent::mw82_y_up ,
                                &TIFJEvent::mw82_y_down,
                                &TIFJEvent::mw82_anode
                               )) ;
#endif



#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //===================================
    //  Bonn Magnet for precession
    //===================================

    module.push_back(new Tbonn_magnet("gfactor_magnet",
                                      &TIFJEvent::gfactor_magnet_up,
                                      &TIFJEvent::gfactor_magnet_dn));
#endif



#ifdef TRACKING_IONISATION_CHAMBER_X

    module.push_back(
        new Ttracking_ionisation_chamber("tic21",
                                         true, // false, // false = no decay events
                                         &TIFJEvent::tic22_data, // this 22 is on purpose
                                         &TIFJEvent::tic22_fired // this 22 is on purpose
                                        ));

    module.push_back(
        new Ttracking_ionisation_chamber("tic22",
                                         true, // false = no decay events
                                         &TIFJEvent::tic21_data,   // this 21 is on purpose
                                         &TIFJEvent::tic21_fired   // this 21 is on purpose
                                        ));

    // focus has two neighbours multiwires
    module.push_back(new
                     Tfocal_plane("s2tic",
                                  "tic21" , Tfocal_plane::tic,
                                  "tic22", Tfocal_plane::tic)) ;


#endif


#ifdef TPC21_PRESENT

    module.push_back(new Ttime_projection_chamber("tpc21",
                     &TIFJEvent::tpc21_drift_time_array_ptr,
                     &TIFJEvent::tpc21_left_time_array_ptr,
                     &TIFJEvent::tpc21_right_time_array_ptr,
                     &TIFJEvent::tpc21_drift_energy_array_ptr,
                     &TIFJEvent::tpc21_left_energy_array_ptr,
                     &TIFJEvent::tpc21_right_energy_array_ptr
                                                 )) ;
#endif

#ifdef TPC22_PRESENT

    module.push_back(new Ttime_projection_chamber("tpc22",
                     &TIFJEvent::tpc22_drift_time_array_ptr,
                     &TIFJEvent::tpc22_left_time_array_ptr,
                     &TIFJEvent::tpc22_right_time_array_ptr,
                     &TIFJEvent::tpc22_drift_energy_array_ptr,
                     &TIFJEvent::tpc22_left_energy_array_ptr,
                     &TIFJEvent::tpc22_right_energy_array_ptr
                                                 )) ;

    // focus has two neighbours multiwires
    module.push_back(new
                     Tfocal_plane("s2tpc",
                                  "tpc21" , Tfocal_plane::tpc,
                                  "tpc22", Tfocal_plane::tpc)) ;
#endif





    //====================================================
    // --------------- Focal planes -------------------
    //====================================================


    // focus has two neighbours multiwires
    module.push_back(new
                     Tfocal_plane("s2",  "mw21" ,
                                  Tfocal_plane::mw,
                                  "mw22",
                                  Tfocal_plane::mw
                                 )) ;


    module.push_back(s4_ptr =
                         new
    Tfocal_plane("s4",
                 "mw41",   Tfocal_plane::mw, // name of MW before it
                 "mw42",   Tfocal_plane::mw)) ;   // name of MW after it



#ifdef TPC41_PRESENT

    module.push_back(new Ttime_projection_chamber("tpc41",
                     &TIFJEvent::tpc41_drift_time_array_ptr,
                     &TIFJEvent::tpc41_left_time_array_ptr,
                     &TIFJEvent::tpc41_right_time_array_ptr,
                     &TIFJEvent::tpc41_drift_energy_array_ptr,
                     &TIFJEvent::tpc41_left_energy_array_ptr,
                     &TIFJEvent::tpc41_right_energy_array_ptr
                                                 )) ;
#endif

#ifdef TPC42_PRESENT

    module.push_back(new Ttime_projection_chamber("tpc42",
                     &TIFJEvent::tpc42_drift_time_array_ptr,
                     &TIFJEvent::tpc42_left_time_array_ptr,
                     &TIFJEvent::tpc42_right_time_array_ptr,
                     &TIFJEvent::tpc42_drift_energy_array_ptr,
                     &TIFJEvent::tpc42_left_energy_array_ptr,
                     &TIFJEvent::tpc42_right_energy_array_ptr
                                                 )) ;

    // focus has two neighbours multiwires
    module.push_back(new
                     Tfocal_plane("s4tpc",
                                  "tpc41" , Tfocal_plane::tpc,
                                  "tpc42", Tfocal_plane::tpc)) ;
#endif



    //====================================================
    // -----------------scintillators --------------------
    //====================================================
    //                  true - orientation horizontal (x)
    //                  false -orientation vertical (y)

    module.push_back(sci21_ptr =
                         new
    Tscintillator("sci21",
                  "s2",
                  true,
                  &TIFJEvent::sc21_de_l,
                  &TIFJEvent::sc21_de_r,
                  &TIFJEvent::sc21_l_sc21_r_dt));

#ifdef SCI22_PRESENT

    Tscintillator * sci22_ptr;
    module.push_back(sci22_ptr =
                         new
    Tscintillator("sci22",
                  "s2",
                  true,
                  &TIFJEvent::sc22_de_l,
                  &TIFJEvent::sc22_de_r,
                  &TIFJEvent::sc22_l_sc22_r_dt));
#endif

#ifdef FINGER_SCINTILLATOR_PRESENT
    //------------------ LAND finger detector----------------------
    // if we want this detector:
    // 1. Include this in the Makefile
    // 2. Unblock this in the unpacking routine (#ifdef NIGDY)
    // 3. Unblock this in the  TIFJEvent.h

    //  module.push_back(new
    //                   Tfinger("sci22_finger", "s2",
    //  &TIFJEvent::finger));

    module.push_back(new
                     Tfinger_tom("sci22finger", "s2",
                                 &TIFJEvent::sci_finger_time,
                                 &TIFJEvent::sci_finger_charge)
                    );
#endif

    module.push_back(new
                     Tscintillator("sci41horiz",
                                   "s4",  // &s[nr4],
                                   true,
                                   &TIFJEvent::sc41_de_l,
                                   &TIFJEvent::sc41_de_r,
                                   &TIFJEvent::sc41_l_sc41_r_dt));

#ifdef SCI41_VERTIC_PRESENT

    module.push_back(sci41LR_ptr =
                         new
    Tscintillator("sci41vertic",
                  "s4",
                  false,
                  &TIFJEvent::sc41_de_dn,
                  &TIFJEvent::sc41_de_up,
                  &TIFJEvent::sc41_dt_up_dn));   // vertical
#endif

    module.push_back(sci41UD_ptr =
                         new
    Tscintillator("sci42",
                  "s4",
                  true,
                  &TIFJEvent::sc42_de_l,
                  &TIFJEvent::sc42_de_r,
                  &TIFJEvent::sc42_l_sc42_r_dt));



    module.push_back(
        new
        Tscintillator("sci43",
                      "", //
                      true,
                      &TIFJEvent::sc43_de_l,
                      &TIFJEvent::sc43_de_r,
                      &TIFJEvent::sc43_l_sc43_r_dt));

    // kind of finger detector - of Carl Wheldon

    module.push_back(
        new
        Ttarget_scintillator("sci_at_target",
                             &TIFJEvent::target_scintillator_x,
                             &TIFJEvent::target_scintillator_y)
    );


    // #ifdef NIGDY

    module.push_back(new
                     Tsi_detector("si_detector_at_target",
                                  &TIFJEvent::target_Si));    // needed by CATE

    // #endif



    //===============================================
    //     time of flight measuring signals
    //===============================================
    module.push_back(new
                     Ttof_measurement("tof_21_41",
                                      &TIFJEvent::sc21_l_sc41_l_dt,
                                      &TIFJEvent::sc21_r_sc41_r_dt,
                                      sci21_ptr,  // default arg (null)
                                      sci41LR_ptr)  // default arg (null)
                    );


    //   module.push_back(
    // new
    //                    Ttof_measurement("tof_41_42",
    //                         &TIFJEvent::sc42_l_sc41_l_dt,
    //                         &TIFJEvent::sc42_r_sc41_r_dt  ) );

    module.push_back(new
                     Ttof_measurement("tof_41_43",
                                      &TIFJEvent::sc41_l_sc43_l_dt,
                                      &TIFJEvent::sc41_r_sc43_r_dt));

#ifdef TOF_21_42__AND_RELATED_AOQ_USED

    module.push_back(new
                     Ttof_measurement("tof_21_42",
                                      &TIFJEvent::sc21_l_sc42_l_dt,
                                      &TIFJEvent::sc21_r_sc42_r_dt));

#endif

#ifdef TOF_22_41_PRESENT

    module.push_back(new
                     Ttof_measurement("tof_22_41",
                                      &TIFJEvent::sc22_l_sc41_l_dt,
                                      &TIFJEvent::sc22_r_sc41_r_dt));


#endif

#ifdef TOF_22_42_PRESENT

    module.push_back(new
                     Ttof_measurement("tof_22_42",
                                      &TIFJEvent::sc22_l_sc42_l_dt,
                                      &TIFJEvent::sc22_r_sc42_r_dt));
#endif

#ifdef TOF_21_22_PRESENT

    module.push_back(new
                     Ttof_measurement("tof_21_22",
                                      &TIFJEvent::sc21_l_sc22_l_dt,
                                      &TIFJEvent::sc21_r_sc22_r_dt));
#endif




#ifdef TOF_41_42_PRESENT

    module.push_back(new
                     Ttof_measurement("tof_41_42",
                                      &TIFJEvent::sc41_l_sc42_l_dt,
                                      &TIFJEvent::sc41_r_sc42_r_dt));
#endif


    //==============================================
    //        music
    //==============================================
#ifdef MUSICBIG_USED

    module.push_back(new
                     Tmusic_4anodes("musicBig41" ,
                                    &TIFJEvent::musicBig41_time ,   // begin of array
                                    &TIFJEvent::musicBig41_energy,
                                    "s4"
#ifdef TPC42_PRESENT
                                    , "s4tpc"
#endif
                                   )
                    );

#ifdef MUSIC42_USED

    module.push_back(new
                     Tmusic_4anodes("musicBig42" ,
                                    &TIFJEvent::musicBig42_time ,   // begin of array
                                    &TIFJEvent::musicBig42_energy,
                                    "s4"
#ifdef TPC42_PRESENT
                                    , "s4tpc"
#endif
                                   )
                    );

#endif

#else //  MUSICBIG_USED


    module.push_back(
        new
        Tmusic_8anodes("music41" ,
                       &TIFJEvent::music_8_time ,
                       // begin of array
                       &TIFJEvent::music_8_energy,
                       "s4", "sci41horiz"
#ifdef TPC42_PRESENT
                       , "s4tpc"
#endif
                      )
    );

#ifdef MUSIC42_USED


    module.push_back(new
                     Tmusic_8anodes("music42" ,
                                    &TIFJEvent::music42_8_time ,
                                    // begin of array
                                    &TIFJEvent::music42_8_energy,
                                    "s4", "sci41horiz"
#ifdef TPC42_PRESENT
                                    , "s4tpc"
#endif
                                   )
                    );

#endif
#endif


    //==============================================
    // --------------------- AOQ
    //==============================================

    // at first we must prepare the vector with names of the magnets
    // which work at the region of this aoq calculatons
    vector<string> names_of_interesting_magnets;

    names_of_interesting_magnets.push_back("Ts3mu1");
    names_of_interesting_magnets.push_back("Ts3mu2") ;

    // done.

    // warning : this particular object defined below calculates
    // only rho and brho. for the degrader.
    // It can not calculate aoq, because there is no
    // time of flight meausrement between primary target and S2
    // but the degrader will use the final aoq
    module.push_back(
        new
        Taoq_calculator
        (
            "dummy_aoq_TA_S2",
            "",                 // nic,  // scintillator before, may be null
            "sci21",          // &sci[sci21],  // scintillator after
            "",                 // nic ,           // no tof object
            names_of_interesting_magnets,
            "NONE",// name_focal_plane_after
            "NONE",// name_focal_plane_tpc_after
            "NONE"// name_focal_plane_tic_before
        ));

    //---------------------
    // Another aoq object is real !
    // here we really WILL calculate the aoq


    names_of_interesting_magnets.clear() ;
    names_of_interesting_magnets.push_back("Ts4mu1");
    names_of_interesting_magnets.push_back("HFSmu1") ;



    module.push_back(
        aoq21_41 =
            new
    Taoq_calculator("aoq",
                    "sci21",
                    // scintillator before
                    "sci41horiz",
                    // sci after
                    "tof_21_41",        //
                    names_of_interesting_magnets,
                    "s4", // name_focal_plane_after
#ifdef TPC41_PRESENT
                    "s4tpc", // name_focal_plane_tpc_after
#else
                    "",
#endif

#ifdef TRACKING_IONISATION_CHAMBER_X
                    "s2tic"  // name_focal_plane_tic_before
#elif defined TPC22_PRESENT
                    "s2tpc"
#else
                    ""
#endif

                   )
    );



#ifdef TOF_22_41__AND_RELATED_AOQ_USED

    module.push_back(
        new
        Taoq_calculator("aoq22_41_",
                        "sci22",
                        // scintillator before
                        "sci41horiz",
                        // sci after
                        "tof_22_41",        //
                        names_of_interesting_magnets,
                        "s4", // name_focal_plane_after
#ifdef TPC41_PRESENT
                        "s4tpc", // name_focal_plane_tpc_after
#else
                        "",
#endif

#ifdef TRACKING_IONISATION_CHAMBER_X
                        "s2tic"  // name_focal_plane_tic_before
#elif defined TPC22_PRESENT
                        "s2tpc"
#else
                        ""
#endif
                       )
    );
#endif
    //##############################



#ifdef TOF_21_42__AND_RELATED_AOQ_USED

    module.push_back(aoq21_42 =
                         new
    Taoq_calculator("aoq21_42_",
                    "sci21",// scintillator before
                    "sci42",          // sci after
                    "tof_21_42",        //
                    names_of_interesting_magnets,
                    "s4", // name_focal_plane_after
#ifdef TPC41_PRESENT
                    "s4tpc", // name_focal_plane_tpc_after
#else
                    "",
#endif
#ifdef TRACKING_IONISATION_CHAMBER_X
                    "s2tic"  // name_focal_plane_tic_before
#elif defined TPC22_PRESENT
                    "s2tpc"
#else
                    ""
#endif
                   )
                    );

#endif

    //#################################################################
#ifdef FINGER_SCINTILLATOR_PRESENT
    //#################################################################

    // second version which takes time given by the finger detector
    module.push_back(
        aoq_calc =
            new
    Taoq_calculator("aoqF",
                    "sci22finger",// scintillator before
                    "sci41horiz",// sci after
                    "",
                    // empty string when Tof is measured by finger !!!
                    names_of_interesting_magnets,
                    "NONE"
                   )
    );
#endif


#ifdef TIC
    // at first we must prepare the vector with names of the magnets
    // which work at the region of this aoq calculatons
    vector<string> names_of_interesting_magnets;

    names_of_interesting_magnets.push_back("Ts3mu1");
    names_of_interesting_magnets.push_back("Ts3mu2") ;

    // done.

    // warning : this particular object defined below calculates
    // only rho and brho. for the degrader.
    // It can not calculate aoq, because there is no
    // time of flight meausrement between primary target and S2
    // but the degrader will use the final aoq
    module.push_back(
        new
        Taoq_calculator
        (
            "dummy_aoq_TA_S2",
            "",                 // nic,  // scintillator before, may be null
            "sci21",          // &sci[sci21],  // scintillator after
            "",                 // nic ,           // no tof object
            names_of_interesting_magnets,
            "NONE"
        ));

    //---------------------
    // Another aoq object is real !
    // here we really WILL calculate the aoq


    names_of_interesting_magnets.clear() ;
    names_of_interesting_magnets.push_back("Ts4mu1");
    names_of_interesting_magnets.push_back("HFSmu1") ;

    module.push_back(
        //aoq_calc =
        new
        Taoq_calculator("aoqTdc",
                        "sci21",   // scintillator before
                        "sci41horiz",   // sci after
                        "tof_21_41",        //
                        names_of_interesting_magnets,
                        "s4tdc"
                       )
    );
#endif

    //================================================
    // CATE (calorimeter telescope detector
    //================================================

    module.push_back(cate_ptr =
                         new Tcate("cate", "si_detector_at_target")) ;

    //================================================
    //   Rising  Target
    //================================================

    module.push_back(
        target_ptr =
            new
    Ttarget("target",        // name,  distance
            "s4",// name of the focus point from the left
            // (gives inf about position of the comming particle)
            "cate")  // name of cate, where from it takes value about
    // the position of the residue
    ) ;


    //============================
    // Zet may be affected by the atmospheric pressure, measuered by following block

#ifdef MUN_ACTIVE_STOPPER_PRESENT

    module.push_back(new T32_signal_module< (unsigned int) 32> ("munich_multiplicity",
                     (&TIFJEvent::munich_multiplicity_module),
                     true,// <--- please make the fun spectra
                     true ,  // <--- please make the signal spectra
                     true // make calibration (default is false)
                                                               ));
#endif

    //================================================
    //   finally, calculation of ZET
    //================================================

    module.push_back(zet_calc = new
    Tzet_calculator("zet",
                    "aoq",
#ifdef MUSICBIG_USED
                    "musicBig41"
#else
                    "music41"
#endif
                   ));


#ifdef MUSIC42_USED

    module.push_back(zet2_calc = new
    Tzet_calculator("zet2",
                    "aoq",
#ifdef MUSICBIG_USED
                    "musicBig42"
#else
                    "music42"
#endif
                   ));
#endif


    //================================================
    //---------------- Degrader ----------------------
    //================================================

    module.push_back(
        new
        Tdegrader("degrader",
                  "dummy_aoq_TA_S2",//*aoq_magnets, (here was calculated rho)
                  "aoq",        //   *aoq_value,  (here was calculated aoq)
#ifdef MUSICBIG_USED
                  "musicBig41",   // just to make one matrix
#else
                  "music41",   // just to make one matrix
#endif
                  "zet")) ;


    //#################################################################
#ifdef FINGER_SCINTILLATOR_PRESENT
    //#################################################################

    module.push_back(zet_calc = new
    Tzet_calculator("zetF",
                    "aoqF",
#ifdef MUSICBIG_USED
                    "musicBig41",   // just to make one matrix
#else
                    "music41"
#endif
                   ));
#endif


    module.push_back(
        synchro_object =
            new
    Tone_signal_module("frs_synchro_data",
                       &TIFJEvent::frs_synchro_data));

    //#################################################################
#if CURRENT_EXPERIMENT_TYPE==RISING_FAST_BEAM_CAMPAIGN
    //#################################################################

    module.push_back(
        new Tclover("clover",
                    &TIFJEvent::clover_energy,
                    &TIFJEvent::clover_time
                   )

    );
#endif



    //#################################################################
#ifdef ACTIVE_STOPPER_PRESENT
    //#################################################################
#endif

#ifdef ACTIVE_STOPPER2_PRESENT
#endif // ACTIVE_STOPPER2_PRESENT

#ifdef ACTIVE_STOPPER3_PRESENT
#endif // ACTIVE_STOPPER3_PRESENT





#ifdef MULTIHIT_V1290_PRESENT

    module.push_back(new T32_signal_module<32> ("multihit_v1290",
                     (&TIFJEvent::multihit_v1290),
                     false,   // is fan spectrum needed?
                     true, // <---- please make the spectra
                     false   // is calibration needed?
                                               ));
#endif








    /*******************************************************************************************
    // was used for my testing purposes ----------------
    module.push_back(   new Tframe_simple ("test_frame", &TIFJEvent::mw21_x_left)  );
    */

 #if 0   // Nie używane w CCB

    module.push_back(new T32_signal_module<32> ("module_vmeOne[2]", (&TIFJEvent::module_vmeOne_2),
                     false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                     true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                     false));    //    bool flag_we_want_calibrate_signals_arg



    module.push_back(new T32_signal_module<32> ("module_vmeOne[3]", (&TIFJEvent::module_vmeOne_3),
                     false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                     true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                     false));    //    bool flag_we_want_calibrate_signals_arg

    module.push_back(new T32_signal_module<32> ("module_vmeOne[4]", (&TIFJEvent::module_vmeOne_4),
                     false,  // bool flag_produce_fan_spectrum_arg , - false, // generally we do not want. Let the user make them as user defined
                     true, // <---- please make the spectra ( bool flag_produce_signal_spectra_arg,  - generally we do not want. Let the user make them as user defined)
                     false));   //      bool flag_we_want_calibrate_signals_arg

#endif

#endif  // GSI_STUFF ##########################################################################################################

#if CURRENT_EXPERIMENT_TYPE==PISOLO2_EXPERIMENT

    // MonL, MonR, MonU, MonD  //(left, right, up, down)
    module.push_back(new
                     Ttwo_signal_module("pisolo_mon",
                                        &TIFJEvent::pisolo_mon_lr,
                                        &TIFJEvent::pisolo_mon_ud,
                                        "lr", "ud"
                                       ));


    module.push_back(new
                     Tmicro_channel_plate("pisolo_mcp01",
                                          &TIFJEvent::pisolo_mcp01_x,
                                          &TIFJEvent::pisolo_mcp01_y
                                         ));

    module.push_back(new
                     Tmicro_channel_plate("pisolo_mcp02",
                                          &TIFJEvent::pisolo_mcp02_x,
                                          &TIFJEvent::pisolo_mcp02_y
                                         ));

    // module.push_back (      new
    //                           Tone_signal_module ( "pisolo_si_target",
    //                                                &TIFJEvent::pisolo_si_target_en,
    //                           "energy"    // description will be used for spectrum name and an incrementer name
    //                                              ) );



    module.push_back(new
                     Tpisolo_si_final("pisolo_si_final",
                                      &TIFJEvent::pisolo_si_final
                                     ));

    module.push_back(new
                     Tpisolo_tof("pisolo_tof_",
                                 &TIFJEvent::pisolo_tof1,
                                 &TIFJEvent::pisolo_tof2,
                                 &TIFJEvent::pisolo_tof3
                                ));

    module.push_back(new
                     Tpisolo_delta_e("pisolo_gas",
                                     &TIFJEvent::pisolo_deltaE,
                                     &TIFJEvent::pisolo_dE_res,
                                     "pisolo_tof_",  "pisolo_si_final"
                                    ));

#endif



#if 0   // Nie używane w CCB

//   module.push_back(new T32_signal_module<32> ("module_vmeOne[0]", (&TIFJEvent::module_vmeOne_0), true, true,      // <---- please make the spectra
//                      false));
//   module.push_back(new T32_signal_module<32> ("module_vmeOne[1]", (&TIFJEvent::module_vmeOne_1), true, true,      // <---- please make the spectra
//                      false));

    module.push_back(new T32_signal_module<32> ("module_vmeOne[5]", (&TIFJEvent::module_vmeOne_5), true, true,      // <---- please make the spectra
                     false));


    module.push_back(new T32_signal_module<32> ("module_vmeOne[6]", (&TIFJEvent::module_vmeOne_6), false, true, false));        // fan, spectra, claibration
    module.push_back(new T32_signal_module<32> ("module_vmeOne[7]", (&TIFJEvent::module_vmeOne_7), false, true, false));
    module.push_back(new T32_signal_module<32> ("module_vmeOne[8]", (&TIFJEvent::module_vmeOne_8), false, true, false));
    module.push_back(new T32_signal_module<32> ("module_vmeOne[9]", (&TIFJEvent::module_vmeOne_9), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeOne[11]", (&TIFJEvent::module_vmeOne_11), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeOne[13]", (&TIFJEvent::module_vmeOne_13), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeOne[15]", (&TIFJEvent::module_vmeOne_15), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeOne[16]", (&TIFJEvent::module_vmeOne_16), false, false, false));

    module.push_back(new T32_signal_module<32> ("module_vmeOne[17]", (&TIFJEvent::module_vmeOne_17), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeOne[18]", (&TIFJEvent::module_vmeOne_18), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeOne[19]", (&TIFJEvent::module_vmeOne_19), false, false, false));


    module.push_back(new T32_signal_module<32> ("module_vmeUser[5]", (&TIFJEvent::module_vmeUser_5), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[6]", (&TIFJEvent::module_vmeUser_6), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[8]", (&TIFJEvent::module_vmeUser_8), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[9]", (&TIFJEvent::module_vmeUser_9), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[11]", (&TIFJEvent::module_vmeUser_11), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[13]", (&TIFJEvent::module_vmeUser_13), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[15]", (&TIFJEvent::module_vmeUser_15), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[16]", (&TIFJEvent::module_vmeUser_16), false, false, false));

    module.push_back(new T32_signal_module<32> ("module_vmeUser[17]", (&TIFJEvent::module_vmeUser_17), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[18]", (&TIFJEvent::module_vmeUser_18), false, false, false));
    module.push_back(new T32_signal_module<32> ("module_vmeUser[19]", (&TIFJEvent::module_vmeUser_19), false, false, false));
#endif // 0

    create_your_spectra() ;
}
//*************************************************************************
void Tfrs::create_your_spectra()
{
    //   string name = "sci41horiz_and_vertic_sum_e"  ;
    //   spec_sum_e_41LRUD = new spectrum_1D(name,
    //                                       name,
    //                                       1024, 0, 8192);
    //   spectra_frs.push_back(spec_sum_e_41LRUD) ;
}
//**********************************************************************
void Tfrs::make_frs_preloop_action(
    TIFJEvent* ev_unp ,
    TIFJCalibratedEvent* event_calibrated_ptr)
{
    // now this adres if fixed
    Tfrs_element::event_unpacked = ev_unp ;
    event_calibrated = event_calibrated_ptr ;

    // very important:
    // calibrations and gates are read everytime we start the analysis
    read_calibration_and_gates_from_disk();
}
//**********************************************************************
void Tfrs::read_calibration_and_gates_from_disk()
{
    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    // If this is IFJ, there is no FRS calibration



    string cal_filename = "calibration/" + official_name + "_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;    // ascii file
        if(!plik)
        {
            string mess = "I can't open " + official_name + " calibration file: " +
                          cal_filename  ;
            throw mess ;
        }

        // here we assume that pointers to all the devices (multiwires,
        //   scintillators, are
        // in the module vector of pointers

        for(unsigned int i = 0 ; i < module.size() ; i++)
        {
            module[i]->make_preloop_action(plik) ;
        }
    }  // end of try
    catch(Tfile_helper_exception & k)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message << endl  ;

        exit(-1) ;    // is it healthy ?

    }
    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ;    // is it healthy ?
    }
    cout << "Calibrations successfully read   ----  " << endl;
}
//*********************************************************
//*********************************************************
void Tfrs::analyse_current_event()
{
    // this is real calibrating, (not just reading the calibration, which
    // had to be done earlier, in the preloop)

    //    static int nr_ev ;
    //
    //    cout << "\nFRS class  works on a new event === nr "
    //         << (nr_ev++) << " =====================" << endl ;


    if(RisingAnalysis_ptr->is_verbose_on())
    {
        //   cout  << "FRS   subevent arrived " << endl;
    }
    for(unsigned int i = 0 ; i < module.size() ; i++)
    {
        module[i]->clear_already_done_flag() ;
    }

    // here we can check what was forgotten (not calculated
    // in a chain to get Zet), we can make it now.

    for(unsigned int i = 0 ; i < module.size() ; i++)
    {
        if(!module[i]->was_it_calculated())
        {
            //      cout << "The "
            //         <<  module[i]->give_name()
            //         << " was 'forgotten', so doing this now  !!! "
            //         << endl ;
            module[i]->analyse_current_event();
        }
    }
}
//********************************************************************
void  Tfrs::make_postloop_action()
{
    // save all spectra made in frs objecti and in frs elements
    // (mw, sci, etc.)
    cout <<  official_name << " saving " << spectra_frs.size() << " spectra... " << endl;
    for(unsigned int i = 0 ; i < spectra_frs.size() ; i++)
    {
        spectra_frs[i]->save_to_disk() ;
    }

    cout <<  official_name << " saved  spectra - finished " << endl;
}
//*******************************************************************
//*******************************************************************
//*******************************************************************
Tfrs_element* Tfrs::address_of_device(string name)
{
    //  transform(name.begin(), name.end(), name.begin(),  toupper);
    for(unsigned i = 0 ; i < name.size() ; i++)
        name[i] = toupper(name[i]);

    // we keep traces of all scintillators, mutliwires etc. in one vector
    //cout << "Searching the address of the element " << name << endl;

    for(unsigned int i = 0 ; i < module.size() ; i++)
    {
        //cout << module[i]->give_name() << endl;
        string found = module[i]->give_name() ;
        //transform(found.begin(), found.end(), found.begin(),  toupper);
        for(unsigned k = 0 ; k < found.size() ; k++)
            found[k] = toupper(found[k]);

        if(name == found)
        {
            return module[i] ; //returning pointer to this device
        }
    }

    // here we are if such was not found

    // perhaps somebody put 0 device

    if(name == "" || name == "0" || name == "NONE"
      )
        return 0 ; //returning pointer to this device


    // really nothing

    cout << "Error during the constructor of the Tfrs:\n"
         << "You asked for an address of the device called " << name
         << " but it is not (yet) defined,\n"
         << "so its address can not be found. "
         << "Note: Till this place in the program, so far "
         " the following devices were defined: "
         << endl ;

    for(unsigned int i = 0 ; i < module.size() ; i++)
    {
        cout << module[i]->give_name() << ", " ;
    }
    cout << endl;

    exit(-2);
}
//*****************************************************************
/** this function decides how often make autosave of spectra */
void Tfrs::perhaps_autosave()

{
#ifdef NIGDY   // this was moved to run_implicit_loop
#endif

}
//*****************************************************************
/** To refresh more often spectra which are currently on the screen
of the cracow viewer */
//*****************************************************************
bool Tfrs::save_selected_spectrum(string name)
{
    for(unsigned int i = 0 ; i < spectra_frs.size() ; i++)
    {
        //cout << "Trying to save spectrum named: " << name << endl;
        if(name == spectra_frs[i]->give_name())
        {
            spectra_frs[i]->save_to_disk() ;
            return true ;
        }
    }
    return false ; // was not here, perhaps in rising or hector?
}
//*********************************************************************
/**************************************************** No descriptions */
/**********************************************************************/
void Tfrs::put_data_to_ntuple_tree()
{}
//*********************************************************************
