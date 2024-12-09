#include "Tparis.h"


//#ifdef HECTOR_PRESENT

#include "Thector_baf.h"
#include "Tfile_helper.h"

#include <dirent.h> // for scandir
#include <malloc.h> // for scandir

#include <sstream>
#include <iomanip>
#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h"


//*******************************************************
//*******************************************************
//         PARIS class
//*******************************************************
//*******************************************************
Tparis::Tparis(string name) : Tfrs_element(name)               // constructor
{

    my_selfgate_type =  Tselfgate_type::hector;

    //---------------------------------------------
    // constructing the current setup of PARIS  (Hector)
    //---------------------------------------------




    // without any cosmetics, just digitizer spectra
    vector<string> signal { "time", "fast", "slow" };
    for(int d = 0 ; d < 2 ; ++d){
        for(int n = 0 ; n < 16 ; ++n){
            for(int sig = 0 ; sig < 3 ; ++sig){
                ostringstream s ;
                s << "digitizer_0" << d << "_ch_" << setfill('0') << setw(2) << n;

                digitizer_channels.push_back(
                            new Tone_signal_module<double>(s.str(), nullptr, signal[sig])
                            ) ;
            }
        }
    }


    // the same what above distributed to the "BaF" detectors
    constexpr int HOW_MANY_PARIS_ELEMENTS = 32;

    for(int n = 0 ; n < HOW_MANY_PARIS_ELEMENTS ; ++n){
        ostringstream s ;
        // s << "digitizer_detector_" << setfill('0') << setw(2) << n;
        digitizer_detector.push_back(
                    new Thector_BaF<Tparis>
                                     ( this, // owner
                                       "paris_", // name
                                       n, // nr_det
                                                       n+64, // slow (qlong)
                                                       n+32,   // fast  (qshort)
                                                       n,  // time
                                                       300));
    }

    create_my_spectra() ;

    named_pointer[name_of_this_element+"_multiplicity"]
            = Tnamed_pointer ( &multiplicity, nullptr, this ) ;


    // !!!!! Note : ADC iand TDC ncrementers are done near the spectra
    // in the function create_my_spectra()
}
//****************************************************************
Tparis::~Tparis()
{

    for(auto f : digitizer_channels)delete f;
    for(auto f : digitizer_detector)delete f;



}
//****************************************************************
void Tparis::create_my_spectra()
{

    string folder = name_of_this_element  ;


    //=============== calibrated spectra (for Gold lines) =====
    // energy cal ------------------------------------

    string name = "" ;

    
    // ------------------- Total spectra -------------------------

    name = name_of_this_element + "_total_slow_cal" ;
    spec_total_slow_cal = new spectrum_1D ( name,
                                            4096, 0, 4096,
                                            folder, "",
                                            "ALL_paris_BaF_slow_energy_cal_when_good"
                                            ) ;
    frs_spectra_ptr->push_back ( spec_total_slow_cal ) ;

    name = name_of_this_element + "_total_fast_cal" ;
    spec_total_fast_cal = new spectrum_1D ( name,
                                            4096, 0, 4096,
                                            folder, "",
                                            "ALL_paris_BaF_fast_energy_cal_when_good");
    frs_spectra_ptr->push_back ( spec_total_fast_cal ) ;


#ifdef HECTOR_CALIBRATE_TIME

    name = name_of_this_element + "_total_time_cal" ;
    spec_total_time_cal = new spectrum_1D ( name,
                                            4096, 0, 4096,
                                            folder, "",
                                            "ALL_paris_BaF_time_cal_when_good"
                                            );
    frs_spectra_ptr->push_back ( spec_total_time_cal ) ;
#endif

    // ------------------ doppler corrected
#if 0
    name = name_of_this_element + "_total_slow_doppler" ;
    spec_total_slow_doppler =
            new spectrum_1D ( name,
                              4096, 0, 4096,
                              folder, "",
                              "ALL_paris_BaF_slow_doppler_when_good");
    frs_spectra_ptr->push_back ( spec_total_slow_doppler ) ;

    name = name_of_this_element + "_total_fast_doppler" ;
    spec_total_fast_doppler =
            new spectrum_1D ( name,
                              4096, 0, 4096,
                              folder, "",
                              "ALL_paris_BaF_fast_doppler_when_good");
    frs_spectra_ptr->push_back ( spec_total_fast_doppler ) ;

#endif // 0

//    //---- RF

//    name = name_of_this_element + "_RF_raw" ;
//    spec_RF_raw =
//            new spectrum_1D ( name,
//                              4096, 0, 4096,
//                              folder, "", "hector_ADC_channel[18]" );
//    frs_spectra_ptr->push_back ( spec_RF_raw ) ;

    // Total matrices -----------------------

    name =   name_of_this_element + "_total_fast_vs_slow_cal" ;

    matr_total_fast_vs_slow_cal =
            new spectrum_2D ( name,
                              256, 0, 4096,
                              256, 0, 4096,
                              folder, "",
                              "X: ALL_paris_BaF_slow_energy_cal_when_good"
                              "\nY: ALL_paris_BaF_fast_energy_cal_when_good");

    frs_spectra_ptr->push_back ( matr_total_fast_vs_slow_cal ) ;

    //-------------------------------------------

//    name =    name_of_this_element + "_total_fast_vs_slow_doppler" ;

//    matr_total_fast_vs_slow_doppler =
//            new spectrum_2D ( name,
//                              256, 0, 4096,
//                              256, 0, 4096,
//                              folder, "",
//                              "X: ALL_paris_BaF_slow_doppler_when_good"
//                              "\nY: " "ALL_paris_BaF_fast_doppler_when_good");

//    frs_spectra_ptr->push_back ( matr_total_fast_vs_slow_doppler ) ;

#ifdef HECTOR_CALIBRATE_TIME

    name =   name_of_this_element + "_total_time_vs_slow_cal" ;

    matr_total_time_vs_slow_cal =
            new spectrum_2D ( name,
                              256, 0, 4096,
                              256, 0, 4096,
                              folder , "",
                              "X: ALL_paris_BaF_slow_energy_cal_when_good"
                              "\nY: ALL_paris_BaF_time_cal_when_good");

    frs_spectra_ptr->push_back ( matr_total_time_vs_slow_cal ) ;
#endif




    // which BaF works and how often
    name =   name_of_this_element + "_statistics_fan" ;

    spec_statistics   =
            new spectrum_1D ( name,
                              700, 0, 700,
                              folder,
                              "0=fast, 1 = slow, 2 = time, 3 = fast&time, 4 = slow&time, 5-fast&slow, 6 fast&slow&time (From ch. 300 is the phoswich info)",
                              "no incrementer exist, ask Jurek for making a 'pseudo-fan' ") ;


    frs_spectra_ptr->push_back ( spec_statistics ) ;


//    //------------
//    // which BaF works and how often
//    name =   name_of_this_element + "_synchro_data" ;

//    spec_hec_synchro  =
//            new spectrum_1D ( name,
//                              500, 0, 8000,
//                              folder , "", "hector_ADC_channel[6]") ;
//    frs_spectra_ptr->push_back ( spec_hec_synchro ) ;



    name =   name_of_this_element + "_BaF_multiplicity" ;

    spec_muliplicity   =
            new spectrum_1D ( name,
                              29, 0, 29,
                              folder, "",
                              name_of_this_element+"_multiplicity") ;

    frs_spectra_ptr->push_back ( spec_muliplicity ) ;


}
//*************************************************************************
void Tparis::make_preloop_action(ifstream & file)
//                make_preloop_action ( TIFJEvent* event_unpacked ,
//                                    TIFJCalibratedEvent* /*event_calibrated*/ )
{


    //cout << "Tparis::make_preloop_action "  << endl;

    // TIFJAnalysis* ptr_anal = RisingAnalysis_ptr ;
    //TmyBaF ::target_ptr = nullptr;   // TODO:   ptr_anal->give_frs_ptr()->give_target_address() ;



    // Instead of Lookup Table ----------------------------------------


    // first we inform where are the data coresponding to these devices
    //simulate_event();

    // loop over the enumeration type
//    for ( unsigned int i = 0 ; i < BaF.size() ; i++ )
//    {
//        // remember the source of data
//        BaF[i]->set_event_data_pointers ( event_unpacked->hector_adc,
//                                          event_unpacked->hector_tdc );
//    }

//    for ( unsigned int i = 0 ; i < plastic_time.size() ; i++ )
//    {
//        plastic_time[i]->signal_ptr  = event_unpacked->hector_tdc + 16 + i;
//    }
    for ( unsigned int i = 0 ; i < digitizer_channels.size() ; ++i )
    {
        digitizer_channels[i]->signal_ptr  = event_unpacked->digitizer_double_data + i;
    }

//    for(auto f : phoswich_BaF)
//        f->set_event_data_pointers ( event_unpacked->phoswich_adc,
//                                     event_unpacked->phoswich_tdc );


    for ( unsigned int i = 0 ; i < digitizer_detector.size() ; ++i )
    {
        // digitizer detectors are TmyBaF
        // cout << "Sending parameters adr = " << hex << event_unpacked->digitizer_data;
        digitizer_detector[i]->set_event_digitizer_data_pointers (
                                             event_unpacked->digitizer_data);
    }

    // calibrations and gates are read everytime we start the analysis
    read_calibration_and_gates_from_disk();

    // result of dialog = what means GOOD event
    read_good_crystal_specification();
}
//**********************************************************************
void Tparis::read_calibration_and_gates_from_disk()
{

    cout << "\n\n\n\nReading Paris calibration from the disk" << endl ;

    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    string cal_filename = "calibration/paris_calibration.txt";
    try
    {
        ifstream plik ( cal_filename.c_str() ) ; // ascii file
        if ( !plik )
        {
            string mess = "I can't open hector calibration file: "
                    + cal_filename  ;
            throw mess ;
        }



        for ( unsigned int i = 0 ; i < digitizer_detector.size() ; ++i )
        {
            // digitizer detectors are TmyBaF
            // cout << "Sending parameters adr = " << hex << event_unpacked->digitizer_data;
            digitizer_detector[i]->make_preloop_action ( plik ) ;
        }

    }  // end of try -----------------

    //---------------------
    catch ( Tno_keyword_exception &capsule )
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << capsule.message << endl ;
        exit ( -1 ) ;
    }
    //----------------------
    catch ( string sss )
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit ( -1 ) ; // is it healthy ?
    }
    //-------------------

    cout << "HECTOR calibration successfully read"  << endl;
}
//********************************************************
//**********************************************************************
void Tparis::read_lookup_info_from_disk(int *HECTOR_ADC_GEO, int* HECTOR_TDC_GEO,
                                         int *PHOSWITCH_ADC_GEO, int* PHOSWITCH_TDC_GEO
                                         )
{

    cout << "\n\n\n\nReading Hector lookup info from the disk" << endl ;

    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    string cal_filename = "calibration/hector_calibration.txt";
    try
    {
        ifstream plik ( cal_filename.c_str() ) ; // ascii file
        if ( !plik )
        {
            string mess = "I can't open hector calibration file: "
                    + cal_filename  ;
            throw mess ;
        }

        // real reading the lookup -----------------

        *HECTOR_ADC_GEO = FH::find_in_file(plik, "HECTOR_ADC_GEO");
        *HECTOR_TDC_GEO = FH::find_in_file(plik, "HECTOR_TDC_GEO");

        *PHOSWITCH_ADC_GEO = FH::find_in_file(plik, "PHOSWITCH_ADC_GEO");
        *PHOSWITCH_TDC_GEO = FH::find_in_file(plik, "PHOSWITCH_TDC_GEO");


    }  // end of try -----------------

    //---------------------
    catch ( Tno_keyword_exception &capsule )
    {
        cerr << "Error while reading lookup info from file "
             << cal_filename
             << ":\n"
             << capsule.message << endl ;
        exit ( -1 ) ;
    }
    //----------------------
    catch ( string sss )
    {
        cerr << "Error while reading geo info from calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit ( -1 ) ; // is it healthy ?
    }
    //-------------------

    cout << "HECTOR lookup info successfully read"  << endl;
}
//********************************************************
//********************************************************
void Tparis::analyse_current_event()
{

    // cout << "Tparis::" << __func__ << endl;
    multiplicity = 0 ;

//    for ( int i = 0 ; i < PLASTIC_HOW_MANY_TDC_ADC_CHANNELS ; i++ )
//    {



    spec_muliplicity->manually_increment(multiplicity);

//    for ( unsigned int i = 0 ; i < plastic_time.size() ; i++ )
//    {
//        plastic_time[i]->analyse_current_event();
//    }

    for (auto d: digitizer_channels)     d->analyse_current_event() ;
    for (auto d: digitizer_detector)     d->analyse_current_event() ;


}
//***********************************************************************
void  Tparis::make_postloop_action()
{
    //     cout << "F.Tparis::make_postloop_action() " << endl ;

    // cout <<  "Saving Hector spectra - done " << endl;
}
//***********************************************************************
void Tparis::simulate_event()
{
#ifdef NIGDY
    for ( unsigned int i = 0 ; i < detector.size() ; i++ )
    {
        //cout << "Loop i =  " << i << endl;
        for ( int cri = 0 ; cri < 7 ; cri++ )
        {
            event_unpacked->Cluster_data[i][cri][0] =
                    ( 100*i ) + ( 10*cri ) +0 ; // energy

            event_unpacked->Cluster_data[i][cri][1] =
                    ( 100*i ) + ( 10*cri ) +1; // time
        }
        // remember the source of data
    }
    //cout << "After loop " << endl;
#endif

}
//**********************************************************************
//bool Tparis::save_selected_spectrum ( string name )
//{
//    for ( unsigned int i = 0 ; i < spectra_hector.size() ; i++ )
//    {
//        if ( name == spectra_hector[i]->give_name() )
//        {
//            //cout << "Saving selected spectrum " << name << endl;
//            spectra_hector[i]->save_to_disk() ;
//            return true ;
//        }
//    }

//    return false ; // was not here, perhaps in rising or frs ?
//}
//*********************************************************************
//int Tparis::give_synchro_data()
//{
//    return hector_synchro_data;
//}
//*************************************************************************
/** result of dialog = what means GOOD event */
void Tparis::read_good_crystal_specification()
{
    // defaults
//    led_adc_nr = 8 ;
//    led_tdc_nr = 16;
//    led_adc_low = 100;
//    led_adc_high = 4000;
//    led_tdc_low = 100;
//    led_tdc_high = 4000;

    // reading from the file
    string pname =  "./options/good_paris.txt" ;
    ifstream plik ( pname.c_str() );
    if ( !plik )
    {
        cout << "Can't open file " << pname << endl;

        //------- En fast
       TmyBaF::flag_good_fast_requires_threshold = false ;
       TmyBaF::fast_threshold = 100 ;

        //------- En slow
       TmyBaF::flag_good_slow_requires_threshold = true ;
       TmyBaF::slow_threshold = 100 ;

        //------- time
       TmyBaF::flag_good_time_requires_threshold = true ;
       TmyBaF::good_time_threshold = 100 ;

        // everything else is zero (static)
        return ;
    }

    try
    {

        // reading
        //   TmyBaF::flag_increment_fast_cal_with_zero =
        //      (bool) Tfile_helper::find_in_file(plik, "increment_fast_cal_with_zero") ;
        //
        //   TmyBaF::flag_increment_slow_cal_with_zero     =
        //      (bool) Tfile_helper::find_in_file(plik, "increment_slow_cal_with_zero") ;
        //
        //   TmyBaF::flag_increment_time_cal_with_zero =
        //      (bool) Tfile_helper::find_in_file(plik, "increment_time_cal_with_zero") ;

        //---------- En fast
       TmyBaF::flag_good_fast_requires_threshold  =
                ( bool ) Tfile_helper::find_in_file ( plik, "good_fast_requires_threshold" ) ;

       TmyBaF::fast_threshold = ( int ) Tfile_helper::find_in_file ( plik, "fast_threshold" ) ;

        //------- En slow
       TmyBaF::flag_good_slow_requires_threshold =
                ( bool ) Tfile_helper::find_in_file ( plik, "good_slow_requires_threshold" ) ;

       TmyBaF::slow_threshold =
                ( int ) Tfile_helper::find_in_file ( plik, "slow_threshold" ) ;


        // time -------
       TmyBaF::flag_good_time_requires_threshold =
                ( bool ) Tfile_helper::find_in_file ( plik, "good_time_requires_threshold" ) ;
       TmyBaF::good_time_threshold = ( int )
                Tfile_helper::find_in_file ( plik, "good_time_threshold" ) ;

        //        // NOT LED
        //       TmyBaF::flag_good_requires_not_led =
        //            ( bool ) Tfile_helper::find_in_file ( plik, "good_requires_not_led" ) ;

        //        try
        //        {
        //            // the new version
        //            led_adc_nr = ( int ) Tfile_helper::find_in_file ( plik, "led_adc_nr" );
        //            led_tdc_nr = ( int ) Tfile_helper::find_in_file ( plik, "led_tdc_nr" );
        //            led_adc_low = ( int ) Tfile_helper::find_in_file ( plik, "led_adc_low" );
        //            led_adc_high = ( int ) Tfile_helper::find_in_file ( plik, "led_adc_high" );
        //            led_tdc_low = ( int ) Tfile_helper::find_in_file ( plik, "led_tdc_low" );
        //            led_tdc_high = ( int ) Tfile_helper::find_in_file ( plik, "led_tdc_high" );
        //        }
        //        catch ( ... )
        //        {
        //            //  if this is old version, we use defaults
        //        }

    }
    catch ( Tfile_helper_exception k )
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
        exit ( 1 );
    }

}
//**********************************************************************

//#endif

