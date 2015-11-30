#include "Thector.h"

#ifdef HECTOR_PRESENT

#include "Thector_baf.h"
#include "Tfile_helper.h"

#include <dirent.h> // for scandir
#include <malloc.h> // for scandir

#include <sstream>
#include "Tnamed_pointer.h"
#include "TIFJAnalysis.h"


//*******************************************************
//*******************************************************
//         HECTOR class
//*******************************************************
//*******************************************************
Thector::Thector()                // constructor
{
    name_of_this_element = "hector" ;


    //---------------------------------------------
    // constructing the current setup of Hector
    //---------------------------------------------

    led_adc_nr = 8 ;
    led_tdc_nr = 16;
    led_adc_low = 100;
    led_adc_high = 4000;
    led_tdc_low = 100;
    led_tdc_high = 4000;


    // creating 12 BaF detectors  ----------------------------

    // these arguments nr_slow, nr_fast
    // tell in which channel  hector_adc[32], hector_tdc[32] we send data
    // for this particular detector
    // for example  BaF nr 6 can get data from adc[2], adc[14], tdc[8]

  // note: Thector_BaF(Thector*ptr_owner, int nr_det, int nr_fast, int nr_slow, int nr_time)

    BaF.push_back ( new Thector_BaF ( this, 1,  17, 1, 1 ) ) ;
    BaF.push_back ( new Thector_BaF ( this, 2,  18, 2, 2 ) ) ;
    BaF.push_back ( new Thector_BaF ( this, 3,  19, 3, 3 ) ) ;
    BaF.push_back ( new Thector_BaF ( this, 4,  20, 4, 4 ) ) ;
    BaF.push_back ( new Thector_BaF ( this, 5,  21, 5, 5 ) ) ;
    BaF.push_back ( new Thector_BaF ( this, 6,  22, 6, 6 ) ) ;
    BaF.push_back ( new Thector_BaF ( this, 7,  23, 7, 7 ) ) ;
    BaF.push_back ( new Thector_BaF ( this, 8,  24, 8, 8 ) ) ;


    create_my_spectra() ;


    named_pointer[name_of_this_element+"_flag_marker_cate_logical"]
        = Tnamed_pointer ( &marker_cate_logical, 0, this ) ;

    named_pointer[name_of_this_element+"_flag_marker_eb_logical"]
        = Tnamed_pointer ( &marker_eb_logical, 0, this ) ;

    named_pointer[name_of_this_element+"_flag_marker_led_logical"]
        = Tnamed_pointer ( &marker_led_logical, 0, this ) ;

    named_pointer[name_of_this_element+"_marker_led_analog"]
        = Tnamed_pointer ( &marker_led_analog, 0, this ) ;


    named_pointer[name_of_this_element+"_flag_marker_led2_logical"]
        = Tnamed_pointer ( &marker_led2_logical, 0, this ) ;

    named_pointer[name_of_this_element+"_synchro_data"]
        = Tnamed_pointer ( &hector_synchro_data, 0, this ) ;


    // !!!!! Note : ADC iand TDC ncrementers are done near the spectra
    // in the function create_my_spectra()
}
//****************************************************************
Thector::~Thector()
{
    for ( unsigned int i = 0 ; i < spectra_hector.size() ; i++ )
    {
        delete spectra_hector[i];
    }
    for ( unsigned int i = 0 ; i < BaF.size() ; i++ )
    {
        delete BaF[i];
    }
}
//****************************************************************
void Thector::create_my_spectra()
{

    string folder = name_of_this_element  ;

    //=============== calibrated spectra (for Gold lines) =====
    // energy cal ------------------------------------

    string name = "" ;

    //- spectra of all ADC and TDCchannels
    for ( int i = 0 ; i < 32 ; i++ )
    {
        ostringstream plik ;
        plik << "hector_TDC_ch_"
             << ( i< 10 ? "0" : "" )
             << i
             ; // << ends ;   <----------- error for strings


        name = plik.str();

        spec_tdc_ch[i] = new spectrum_1D (
            name,
            name,
            4096, 0, 4096,
            folder );
        spectra_hector.push_back ( spec_tdc_ch[i] ) ;


        named_pointer[name]
            = Tnamed_pointer ( &hector_TDC_channel[i], 0, this ) ;


        // TDC time ------------------------------

        ostringstream plik2 ;
        plik2 << "hector_ADC_ch_"
              << ( i< 10 ? "0" : "" )
              << i
              ; // << ends ;     <---------- error for strings

        //char * ptr = plik.str() ;

        name = plik2.str();

        spec_adc_ch[i] = new spectrum_1D ( name,
                                           name,
                                           4096, 0, 4096,
                                           folder );
        spectra_hector.push_back ( spec_adc_ch[i] ) ;
        //cout << "Created spectrum " << name << endl;


        named_pointer[name]
            = Tnamed_pointer ( &hector_ADC_channel[i], 0, this ) ;


    }

    // markers spectra ------------------------------
    name = name_of_this_element + "_marker_LED_analog" ;
    spectra_markers.push_back ( new spectrum_1D ( name,
                                name,
                                4096, 0, 4096,
                                folder ) );
    spectra_hector.push_back ( spectra_markers[ spectra_markers.size()-1 ] ) ;

    // with marker EB
    name = name_of_this_element + "_marker_coinc_clusters" ;
    spectra_markers.push_back ( new spectrum_1D ( name,
                                name,
                                4096, 0, 4096,
                                folder ) );
    spectra_hector.push_back ( spectra_markers[ spectra_markers.size()-1 ] ) ;

    name = name_of_this_element + "_marker_multiplicity" ;
    spectra_markers.push_back ( new spectrum_1D ( name,
                                name,
                                4096, 0, 4096,
                                folder ) );
    spectra_hector.push_back ( spectra_markers[ spectra_markers.size()-1 ] ) ;

    // ------------------- Total spectra -------------------------

    name = name_of_this_element + "_total_slow_cal" ;
    spec_total_slow_cal = new spectrum_1D ( name,
                                            name,
                                            4096, 0, 4096,
                                            folder ) ;
    spectra_hector.push_back ( spec_total_slow_cal ) ;

    name = name_of_this_element + "_total_fast_cal" ;
    spec_total_fast_cal = new spectrum_1D ( name,
                                            name,
                                            4096, 0, 4096,
                                            folder );
    spectra_hector.push_back ( spec_total_fast_cal ) ;


#ifdef HECTOR_CALIBRATE_TIME

    name = name_of_this_element + "_total_time_cal" ;
    spec_total_time_cal = new spectrum_1D ( name,
                                            name,
                                            4096, 0, 4096,
                                            folder );
    spectra_hector.push_back ( spec_total_time_cal ) ;
#endif

    // ------------------ doppler corrected

    name = name_of_this_element + "_total_slow_doppler" ;
    spec_total_slow_doppler =
        new spectrum_1D ( name,
                          name,
                          4096, 0, 4096,
                          folder );
    spectra_hector.push_back ( spec_total_slow_doppler ) ;

    name = name_of_this_element + "_total_fast_doppler" ;
    spec_total_fast_doppler =
        new spectrum_1D ( name,
                          name,
                          4096, 0, 4096,
                          folder );
    spectra_hector.push_back ( spec_total_fast_doppler ) ;


    // Total matrices -----------------------

    name =   name_of_this_element + "_total_fast_vs_slow_cal" ;

    matr_total_fast_vs_slow_cal =
        new spectrum_2D ( name,
                          name,
                          256, 0, 4096,
                          256, 0, 4096,
                          folder );

    spectra_hector.push_back ( matr_total_fast_vs_slow_cal ) ;

    //-------------------------------------------

    name =    name_of_this_element + "_total_fast_vs_slow_doppler" ;

    matr_total_fast_vs_slow_doppler =
        new spectrum_2D ( name,
                          name,
                          256, 0, 4096,
                          256, 0, 4096,
                          folder );

    spectra_hector.push_back ( matr_total_fast_vs_slow_doppler ) ;

#ifdef HECTOR_CALIBRATE_TIME

    name =   name_of_this_element + "_total_time_vs_slow_cal" ;

    matr_total_time_vs_slow_cal =
        new spectrum_2D ( name,
                          name,
                          256, 0, 4096,
                          256, 0, 4096,
                          folder );

    spectra_hector.push_back ( matr_total_time_vs_slow_cal ) ;
#endif

    // which BaF works and how often
    name =   name_of_this_element + "_statistics" ;

    spec_statistics   =
        new spectrum_1D ( name,
                          name,
                          100, 0, 100,
                          folder ) ;

    spectra_hector.push_back ( spec_statistics ) ;


    //------------
    // which BaF works and how often
    name =   name_of_this_element + "_synchro_data" ;

    spec_hec_synchro  =
        new spectrum_1D ( name,
                          name,
                          500, 0, 8000,
                          folder ) ;
    spectra_hector.push_back ( spec_hec_synchro ) ;
}
//*************************************************************************
void Thector::make_preloop_action ( TIFJEvent* event_unpacked ,
                                    TIFJCalibratedEvent* event_calibrated )
{
    tmp_event_unpacked =    event_unpacked ;

    //cout << "Thector::make_preloop_action "  << endl;

//     TIFJAnalysis* ptr_anal = RisingAnalysis_ptr ;
    Thector_BaF::target_ptr = nullptr;   // TODO:   ptr_anal->give_frs_ptr()->give_target_address() ;


    // first we inform where are the data coresponding to these devices



    //simulate_event();

    // loop over the enumeration type
    for ( unsigned int i = 0 ; i < BaF.size() ; i++ )
    {
        // remember the source of data
        BaF[i]->set_event_data_pointers ( event_unpacked->hector_adc,
                                          event_unpacked->hector_tdc );
    }


    // calibrations and gates are read everytime we start the analysis

    read_calibration_and_gates_from_disk();

    // result of dialog = what means GOOD event
    read_good_crystal_specification();
}
//**********************************************************************
void Thector::read_calibration_and_gates_from_disk()
{

    cout << "\n\n\n\nReading Hector calibration from the disk" << endl ;

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

        // real reading the calibration -----------------
        for ( unsigned int i = 0 ; i < BaF.size() ; i++ )
        {
            // cout << "reading calibration loop for hector BaF "
            // << i <<  endl ;
            BaF[i]->make_preloop_action ( plik ) ;   // reference
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
//********************************************************
//********************************************************
void Thector::analyse_current_event()
{

//     cout << "Thector::" << __func__ << endl;

    for ( int i = 0 ; i < 32 ; i++ )
    {
        int value = hector_TDC_channel[i]= tmp_event_unpacked->hector_tdc[i] ;
        if ( value )
        {
//  		  cout << "-----TDC[" << i << "] " << " Value = " << value << endl;
            spec_tdc_ch[i]->manually_increment ( value ) ;
        }


        value = hector_ADC_channel[i]= tmp_event_unpacked->hector_adc[i];
        if ( value )
        {
// 		  cout << "ADC[" << i << "] " << " Value = " << value << endl;
            spec_adc_ch[i]->manually_increment ( value ) ;
        }

    }

    hector_synchro_data = hector_ADC_channel[6];
    spec_hec_synchro->manually_increment ( hector_synchro_data );

    // at first try to upack markers

    // =============================== markers ======================
    marker_led_analog = tmp_event_unpacked->hector_adc[led_adc_nr] ; // marker LED

    // now we make LED marker also when TDC nr 16 fired (OR of 2 sitiuations
    marker_led_logical =
        (
            led_adc_low <= marker_led_analog
            &&
            marker_led_analog <= led_adc_high
        )
        ||
        ( led_tdc_low <= tmp_event_unpacked->hector_tdc[led_tdc_nr]
          &&
          tmp_event_unpacked->hector_tdc[led_tdc_nr]  <= led_tdc_high ) ;


    if ( marker_led_logical && RisingAnalysis_ptr ->is_verbose_on() )
    {
        cout  << name_of_this_element << ", LED marker (logical)"
              <<endl;
    }

    //--------------------------------------------
    int value = tmp_event_unpacked->hector_adc[9];
    marker_eb_logical = ( 100 < value && value < 3800 );

    value = tmp_event_unpacked->hector_adc[16];
    marker_led2_logical = ( 100 < value && value < 3800 );

    value = tmp_event_unpacked->hector_adc[10];
    marker_cate_logical = ( 100 < value && value < 3800 );

    //-----------------------------------  BaF-----------------------

    for ( unsigned int i = 0 ; i < BaF.size() ; i++ )
    {
        BaF[i]->analyse_current_event() ;
    }


}
//***********************************************************************
void  Thector::make_postloop_action()
{
//     cout << "F.Thector::make_postloop_action() " << endl ;

//     cout <<  "Saving " << spectra_hector.size()    << " Hector spectra..." << endl;
    for ( unsigned int i = 0 ; i < spectra_hector.size() ; i++ )
    {
        spectra_hector[i]->save_to_disk() ;
    }
   // cout <<  "Saving Hector spectra - done " << endl;

}
//***********************************************************************
void Thector::simulate_event()
{
#ifdef NIGDY
    for ( unsigned int i = 0 ; i < detector.size() ; i++ )
    {
        //cout << "Loop i =  " << i << endl;
        for ( int cri = 0 ; cri < 7 ; cri++ )
        {
            tmp_event_unpacked->Cluster_data[i][cri][0] =
                ( 100*i ) + ( 10*cri ) +0 ; // energy

            tmp_event_unpacked->Cluster_data[i][cri][1] =
                ( 100*i ) + ( 10*cri ) +1; // time
        }
        // remember the source of data
    }
    //cout << "After loop " << endl;
#endif

}
//**********************************************************************
bool Thector::save_selected_spectrum ( string name )
{
    for ( unsigned int i = 0 ; i < spectra_hector.size() ; i++ )
    {
        if ( name == spectra_hector[i]->give_name() )
        {
	   //cout << "Saving selected spectrum " << name << endl;
            spectra_hector[i]->save_to_disk() ;
            return true ;
        }
    }
    return false ; // was not here, perhaps in rising or frs ?
}
//*********************************************************************
int Thector::give_synchro_data()
{
    return hector_synchro_data;
}
//*************************************************************************
/** result of dialog = what means GOOD event */
void Thector::read_good_crystal_specification()
{
    // defaults
    led_adc_nr = 8 ;
    led_tdc_nr = 16;
    led_adc_low = 100;
    led_adc_high = 4000;
    led_tdc_low = 100;
    led_tdc_high = 4000;

    // reading from the file
    string pname =  "./options/good_hector.txt" ;
    ifstream plik ( pname.c_str() );
    if ( !plik )
    {
        cout << "Can't open file " << pname << endl;

        //------- En fast
        Thector_BaF::flag_good_fast_requires_threshold = false ;
        Thector_BaF::fast_threshold = 100 ;

        //------- En slow
        Thector_BaF::flag_good_slow_requires_threshold = true ;
        Thector_BaF::slow_threshold = 100 ;

        //------- time
        Thector_BaF::flag_good_time_requires_threshold = true ;
        Thector_BaF::good_time_threshold = 100 ;

        // everything else is zero (static)
        return ;
    }

    try
    {

        // reading
        //    Thector_BaF::flag_increment_fast_cal_with_zero =
        //      (bool) Tfile_helper::find_in_file(plik, "increment_fast_cal_with_zero") ;
        //
        //    Thector_BaF::flag_increment_slow_cal_with_zero     =
        //      (bool) Tfile_helper::find_in_file(plik, "increment_slow_cal_with_zero") ;
        //
        //    Thector_BaF::flag_increment_time_cal_with_zero =
        //      (bool) Tfile_helper::find_in_file(plik, "increment_time_cal_with_zero") ;

        //---------- En fast
        Thector_BaF::flag_good_fast_requires_threshold  =
            ( bool ) Tfile_helper::find_in_file ( plik, "good_fast_requires_threshold" ) ;

        Thector_BaF::fast_threshold = ( int ) Tfile_helper::find_in_file ( plik, "fast_threshold" ) ;

        //------- En slow
        Thector_BaF::flag_good_slow_requires_threshold =
            ( bool ) Tfile_helper::find_in_file ( plik, "good_slow_requires_threshold" ) ;

        Thector_BaF::slow_threshold =
            ( int ) Tfile_helper::find_in_file ( plik, "slow_threshold" ) ;


        // time -------
        Thector_BaF::flag_good_time_requires_threshold =
            ( bool ) Tfile_helper::find_in_file ( plik, "good_time_requires_threshold" ) ;
        Thector_BaF::good_time_threshold = ( int )
                                           Tfile_helper::find_in_file ( plik, "good_time_threshold" ) ;

        // NOT LED
        Thector_BaF::flag_good_requires_not_led =
            ( bool ) Tfile_helper::find_in_file ( plik, "good_requires_not_led" ) ;

        try
        {
            // the new version
            led_adc_nr = ( int ) Tfile_helper::find_in_file ( plik, "led_adc_nr" );
            led_tdc_nr = ( int ) Tfile_helper::find_in_file ( plik, "led_tdc_nr" );
            led_adc_low = ( int ) Tfile_helper::find_in_file ( plik, "led_adc_low" );
            led_adc_high = ( int ) Tfile_helper::find_in_file ( plik, "led_adc_high" );
            led_tdc_low = ( int ) Tfile_helper::find_in_file ( plik, "led_tdc_low" );
            led_tdc_high = ( int ) Tfile_helper::find_in_file ( plik, "led_tdc_high" );
        }
        catch ( ... )
        {
            //  if this is old version, we use defaults
        }

    }
    catch ( Tfile_helper_exception k )
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
        exit ( 1 );
    }

}
//**********************************************************************

#endif   // HECTOR_PRESENT
