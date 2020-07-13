#include "Thector.h"

#ifdef HECTOR_PRESENT

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
//         HECTOR class
//*******************************************************
//*******************************************************
Thector::Thector(string name) : Tfrs_element(name)               // constructor
{

    my_selfgate_type =  Tselfgate_type::hector;

    //---------------------------------------------
    // constructing the current setup of Hector
    //---------------------------------------------


#if 0
    // creating 12 BaF detectors  ----------------------------

    // these arguments nr_slow, nr_fast
    // tell in which channel  hector_adc[32], hector_tdc[32] we send data
    // for this particular detector
    // for example  BaF nr 6 can get data from adc[2], adc[14], tdc[8]

    // note:TmyBaF(Thector*ptr_owner, int nr_det, int nr_slow, int nr_fast, int nr_time)

    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 1,  17, 1, 1 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 2,  18, 2, 2 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 3,  19, 3, 3 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 4,  20, 4, 4 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 5,  21, 5, 5 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 6,  22, 6, 6 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 7,  23, 7, 7 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 8,  24, 8, 8 , 0) ) ;

    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 9,  25, 9, 9 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 10,  26, 10, 10 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 11,  27, 11, 11 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 12,  28, 12, 12 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 13,  29, 13, 13 , 0) ) ;
    BaF.push_back ( newTmyBaF ( this, "hector_BaF_", 14,  30, 14, 14 , 0) ) ;


#endif


#if 0
    // note: TmyBaF(Thector*ptr_owner, int nr_det, int nr_slow, int nr_fast, int nr_time)
    // phoswtich part
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 0,  16, 0, 0 , 300) ) ;
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 1,  17, 1, 1, 300 ) ) ;
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 2,  18, 2, 2, 300 ) ) ;
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 3,  19, 3, 3, 300 ) ) ;
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 4,  20, 4, 4, 300 ) ) ;
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 5,  21, 5, 5, 300 ) ) ;
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 6,  22, 6, 6, 300 ) ) ;
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 7,  23, 7, 7, 300 ) ) ;
    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 8,  24, 8, 8, 300 ) ) ;

    phoswich_BaF.push_back ( new TmyBaF ( this, "phoswich_", 9,  25, 9, 9, 300 ) ) ;

    spec_phoswich_tdc_ch.resize(32);
    spec_phoswich_adc_ch.resize(32);
#endif
    create_my_spectra() ;

    named_pointer[name_of_this_element+"_multiplicity"]
            = Tnamed_pointer ( &multiplicity, nullptr, this ) ;


    // !!!!! Note : ADC iand TDC ncrementers are done near the spectra
    // in the function create_my_spectra()
}
//****************************************************************
Thector::~Thector()
{

    for ( unsigned int i = 0 ; i < BaF.size() ; i++ )
    {
        delete BaF[i];
    }
    for(auto f : phoswich_BaF)delete f;

}
//****************************************************************
void Thector::create_my_spectra()
{

    string folder = name_of_this_element  ;

    //=============== calibrated spectra (for Gold lines) =====
    // energy cal ------------------------------------

    string name = "" ;

    //- spectra of all ADC and TDCchannels
    spec_phoswich_adc_ch.resize(PLASTIC_HOW_MANY_TDC_ADC_CHANNELS);
    spec_phoswich_tdc_ch.resize(PLASTIC_HOW_MANY_TDC_ADC_CHANNELS);

    for ( int i = 0 ; i < PLASTIC_HOW_MANY_TDC_ADC_CHANNELS ; i++ )
    {
        string det_name = "hector_";
        string nr = ( i< 10 ? "0" : "" ) + to_string(i);

        name = det_name + "TDC_ch_" + nr ;
        spec_tdc_ch[i] = new spectrum_1D (
                    name,
                    4096, 0, 4096,
                    folder, "", name );
        frs_spectra_ptr->push_back ( spec_tdc_ch[i] ) ;

        named_pointer[name]
                = Tnamed_pointer ( &hector_TDC_channel[i], 0, this ) ;


        // TDC time ------------------------------
        name = det_name + "ADC_ch_" + nr;
        spec_adc_ch[i] = new spectrum_1D ( name,
                                           4096, 0, 4096,
                                           folder, "", name );
        frs_spectra_ptr->push_back ( spec_adc_ch[i] ) ;
        //cout << "Created spectrum " << name << endl;

        named_pointer[name]
                = Tnamed_pointer ( &hector_ADC_channel[i], 0, this ) ;


        // make the same with Phoswich adc, tdc
        // -------------
        det_name = "phoswich_";
        name = det_name + "TDC_ch_" + nr;
        spec_phoswich_tdc_ch[i] = new spectrum_1D ( name,
                                                    4096, 0, 4096,
                                                    folder, "", name );
        frs_spectra_ptr->push_back ( spec_phoswich_tdc_ch[i] ) ;
        named_pointer[name] =
                Tnamed_pointer ( &phoswich_TDC_channel[i], 0, this ) ;

        //--------------

        name = det_name + "ADC_ch_" + nr;

        spec_phoswich_adc_ch[i] = new spectrum_1D ( name,
                                                    4096, 0, 4096,
                                                    folder , "", name);
        frs_spectra_ptr->push_back ( spec_phoswich_adc_ch[i] ) ;
        named_pointer[name] =
                Tnamed_pointer ( &phoswich_ADC_channel[i], 0, this ) ;

    }



    
    // ------------------- Total spectra -------------------------

    name = name_of_this_element + "_total_slow_cal" ;
    spec_total_slow_cal = new spectrum_1D ( name,
                                            4096, 0, 4096,
                                            folder, "",
                                            "ALL_hector_BaF_slow_energy_cal_when_good"
                                            ) ;
    frs_spectra_ptr->push_back ( spec_total_slow_cal ) ;

    name = name_of_this_element + "_total_fast_cal" ;
    spec_total_fast_cal = new spectrum_1D ( name,
                                            4096, 0, 4096,
                                            folder, "",
                                            "ALL_hector_BaF_fast_energy_cal_when_good");
    frs_spectra_ptr->push_back ( spec_total_fast_cal ) ;


#ifdef HECTOR_CALIBRATE_TIME

    name = name_of_this_element + "_total_time_cal" ;
    spec_total_time_cal = new spectrum_1D ( name,
                                            4096, 0, 4096,
                                            folder, "",
                                            "ALL_hector_BaF_time_cal_when_good"
                                            );
    frs_spectra_ptr->push_back ( spec_total_time_cal ) ;
#endif

    // ------------------ doppler corrected

    name = name_of_this_element + "_total_slow_doppler" ;
    spec_total_slow_doppler =
            new spectrum_1D ( name,
                              4096, 0, 4096,
                              folder, "",
                              "ALL_hector_BaF_slow_doppler_when_good");
    frs_spectra_ptr->push_back ( spec_total_slow_doppler ) ;

    name = name_of_this_element + "_total_fast_doppler" ;
    spec_total_fast_doppler =
            new spectrum_1D ( name,
                              4096, 0, 4096,
                              folder, "",
                              "ALL_hector_BaF_fast_doppler_when_good");
    frs_spectra_ptr->push_back ( spec_total_fast_doppler ) ;



    // Total matrices -----------------------

    name =   name_of_this_element + "_total_fast_vs_slow_cal" ;

    matr_total_fast_vs_slow_cal =
            new spectrum_2D ( name,
                              256, 0, 4096,
                              256, 0, 4096,
                              folder, "",
                              "X: ALL_hector_BaF_slow_energy_cal_when_good"
                              "\nY: ALL_hector_BaF_fast_energy_cal_when_good");

    frs_spectra_ptr->push_back ( matr_total_fast_vs_slow_cal ) ;

    //-------------------------------------------

    name =    name_of_this_element + "_total_fast_vs_slow_doppler" ;

    matr_total_fast_vs_slow_doppler =
            new spectrum_2D ( name,
                              256, 0, 4096,
                              256, 0, 4096,
                              folder, "",
                              "X: ALL_hector_BaF_slow_doppler_when_good"
                              "\nY: " "ALL_hector_BaF_fast_doppler_when_good");

    frs_spectra_ptr->push_back ( matr_total_fast_vs_slow_doppler ) ;

#ifdef HECTOR_CALIBRATE_TIME

    name =   name_of_this_element + "_total_time_vs_slow_cal" ;

    matr_total_time_vs_slow_cal =
            new spectrum_2D ( name,
                              256, 0, 4096,
                              256, 0, 4096,
                              folder , "",
                              "X: ALL_hector_BaF_slow_energy_cal_when_good"
                              "\nY: ALL_hector_BaF_time_cal_when_good");

    frs_spectra_ptr->push_back ( matr_total_time_vs_slow_cal ) ;
#endif




    // which BaF works and how often
    name =   name_of_this_element + "_and_phoswich_statistics_fan" ;

    spec_statistics   =
            new spectrum_1D ( name,
                              700, 0, 700,
                              folder,
                              "0=fast, 1 = slow, 2 = time, 3 = fast&time, 4 = slow&time, 5-fast&slow, 6 fast&slow&time (From ch. 300 is the phoswich info)",
                              "no incrementer exist, ask Jurek for making a 'pseudo-fan' ") ;


    frs_spectra_ptr->push_back ( spec_statistics ) ;



    name =   name_of_this_element + "_BaF_multiplicity" ;

    spec_muliplicity   =
            new spectrum_1D ( name,
                              20, 0, 20,
                              folder, "",
                              name_of_this_element+"_multiplicity") ;

    frs_spectra_ptr->push_back ( spec_muliplicity ) ;


}
//*************************************************************************
void Thector::make_preloop_action(ifstream & file)
//                make_preloop_action ( TIFJEvent* event_unpacked ,
//                                    TIFJCalibratedEvent* /*event_calibrated*/ )
{


    //cout << "Thector::make_preloop_action "  << endl;


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



//    for(auto f : phoswich_BaF)
//        f->set_event_data_pointers ( event_unpacked->phoswich_adc,
//                                     event_unpacked->phoswich_tdc );

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

        for(auto f : phoswich_BaF)f->make_preloop_action ( plik ) ;

//        for ( unsigned int i = 0 ; i < digitizer_detector.size() ; ++i )
//        {
//            // digitizer detectors are TmyBaF
//            // cout << "Sending parameters adr = " << hex << event_unpacked->digitizer_data;
//            digitizer_detector[i]->make_preloop_action ( plik ) ;
//        }

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
void Thector::read_lookup_info_from_disk(int *HECTOR_ADC_GEO, int* HECTOR_TDC_GEO,
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
void Thector::analyse_current_event()
{

    // cout << "Thector::" << __func__ << endl;
    multiplicity = 0 ;

    for ( int i = 0 ; i < PLASTIC_HOW_MANY_TDC_ADC_CHANNELS ; i++ )
    {

        int value = hector_TDC_channel[i]= event_unpacked->hector_tdc[i] ;
        if ( value )
        {
            //  		  cout << "-----TDC[" << i << "] " << " Value = " << value << endl;
            spec_tdc_ch[i]->manually_increment ( value ) ;
        }


        value = hector_ADC_channel[i]= event_unpacked->hector_adc[i];
        if ( value )
        {
            // 		  cout << "ADC[" << i << "] " << " Value = " << value << endl;
            spec_adc_ch[i]->manually_increment ( value ) ;
        }

    }
    //--------------------
    for(unsigned int i = 0  ; i < spec_phoswich_tdc_ch.size() ; ++i)
    {
        int value = phoswich_TDC_channel[i]= event_unpacked->phoswich_tdc[i] ;
        if ( value )
        {
            //cout << "-----TDC[" << i << "] " << " Value = " << value << endl;
            spec_phoswich_tdc_ch[i]->manually_increment ( value ) ;
        }
    }
    //--------------------------------
    for(unsigned int i = 0  ; i < spec_phoswich_adc_ch.size() ; ++i)
    {
        int value = phoswich_ADC_channel[i]= event_unpacked->phoswich_adc[i] ;
        if ( value )
        {
            //  	  cout << "-----TDC[" << i << "] " << " Value = " << value << endl;
            spec_phoswich_adc_ch[i]->manually_increment ( value ) ;
        }
    }


    //-----------------------------------  BaF-----------------------

    for ( unsigned int i = 0 ; i < BaF.size() ; i++ )
    {
        BaF[i]->analyse_current_event() ;
    }

    spec_muliplicity->manually_increment(multiplicity);

//    for ( unsigned int i = 0 ; i < plastic_time.size() ; i++ )
//    {
//        plastic_time[i]->analyse_current_event();
//    }

    for (auto f: phoswich_BaF)     f->analyse_current_event() ;
//    for (auto d: digitizer_channels)     d->analyse_current_event() ;
//    for (auto d: digitizer_detector)     d->analyse_current_event() ;


}
//***********************************************************************
void  Thector::make_postloop_action()
{
    //     cout << "F.Thector::make_postloop_action() " << endl ;

    //     cout <<  "Saving " << spectra_hector.size()    << " Hector spectra..." << endl;
//    for ( unsigned int i = 0 ; i < spectra_hector.size() ; i++ )
//    {
//        spectra_hector[i]->save_to_disk() ;
//    }


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
//bool Thector::save_selected_spectrum ( string name )
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
//int Thector::give_synchro_data()
//{
//    return hector_synchro_data;
//}
//*************************************************************************
/** result of dialog = what means GOOD event */
void Thector::read_good_crystal_specification()
{
    // defaults

    // reading from the file
    string pname =  "./options/good_hector.txt" ;
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

    }
    catch ( Tfile_helper_exception k )
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
        exit ( 1 );
    }

}
//**********************************************************************

#endif   // HECTOR_PRESENT
