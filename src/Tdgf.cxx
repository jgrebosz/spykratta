#include "Tdgf.h"
#include "Tdgf_general_detector.h"
#include "Tfile_helper.h"

#include <dirent.h> // for scandir
#include <malloc.h> // for scandir

#include <sstream>
#include "Tnamed_pointer.h"

//*******************************************************
//*******************************************************
//         DGF class
//*******************************************************
//*******************************************************
Tdgf::Tdgf()                // constructor
{
    name_of_this_element = "dgf" ;


    //---------------------------------------------
    // constructing the current setup of DGF
    //---------------------------------------------


    // creating 8 general_detector detectors  ----------------------------

    // these arguments nr_slow, nr_fast
    // tell in which channel  hector_adc[32], hector_tdc[32] we send data
    // for this particular detector
    // for example  general_detector nr 6 can get data from adc[2], adc[14], tdc[8]


    general_detector.push_back(new Tdgf_general_detector(this, 0,  0, 0,  0, 0)) ;
    general_detector.push_back(new Tdgf_general_detector(this, 1,  1, 1, 1, 1)) ;
    general_detector.push_back(new Tdgf_general_detector(this, 2,  2, 2, 2, 2)) ;
    general_detector.push_back(new Tdgf_general_detector(this, 3,  3, 3, 3, 3)) ;
    general_detector.push_back(new Tdgf_general_detector(this, 4,  4, 4, 4, 4)) ;
    general_detector.push_back(new Tdgf_general_detector(this, 5,  5, 5, 5, 5)) ;
    general_detector.push_back(new Tdgf_general_detector(this, 6,  6, 6, 6, 6)) ;
    general_detector.push_back(new Tdgf_general_detector(this, 7,  7, 7, 7, 7)) ;

    create_my_spectra() ;




    named_pointer[name_of_this_element + "_synchro_data"]
    = Tnamed_pointer(&synchro_data, 0, this) ;



    // !!!!! Note : ADC incrementers for DGF are done near the spectra
    // in the function create_my_spectra()
}
//****************************************************************
void Tdgf::create_my_spectra()
{

    string folder = name_of_this_element  ;

    //=============== calibrated spectra (for Gold lines) =====
    // energy cal ------------------------------------

    string name = "" ;

    //- spectra of all ADC and TDCchannels
    for(unsigned int i = 0 ; i <  general_detector.size() ; i++)
    {
        ostringstream plik ;
        plik << "dgf_TDC_ch_"
             << (i < 10 ? "0" : "")
             << i
             ; // << ends ;   <----------- error for strings


        name = plik.str();

        spec_tdc_ch[i] = new spectrum_1D(
            name,
            name,
            8192, 0, 0xffff,
            folder);
        spectra_dgf.push_back(spec_tdc_ch[i]) ;

        // TDC time ------------------------------

        ostringstream plik2 ;
        plik2 << "dgf_ADC_ch_"
              << (i < 10 ? "0" : "")
              << i
              ; // << ends ;     <---------- error for strings

        //char * ptr = plik.str() ;

        name = plik2.str();

        spec_adc_ch[i] = new spectrum_1D(name,
                                         name,
                                         8192, 0, 0xffff,    // <--- full 16 bit word of data
                                         folder);
        spectra_dgf.push_back(spec_adc_ch[i]) ;




//-----------
        plik.str("");
        plik << "dgf_XIA_PSA_ch_"
             << (i < 10 ? "0" : "")
             << i
             ; // << ends ;   <----------- error for strings


        name = plik.str();

        spec_xia_psa_ch[i] = new spectrum_1D(
            name,
            name,
            8192, 0, 0xffff,
            folder);
        spectra_dgf.push_back(spec_xia_psa_ch[i]) ;

        //  ------------------------------

        plik.str("");
        plik << "dgf_USER_PSA_ch_"
             << (i < 10 ? "0" : "")
             << i
             ; // << ends ;     <---------- error for strings

        name = plik.str();

        spec_user_psa_ch[i] = new spectrum_1D(name,
                                              name,
                                              8192, 0, 0xffff,
                                              folder);
        spectra_dgf.push_back(spec_user_psa_ch[i]) ;



        //cout << "Created spectrum " << name << endl;


//     named_pointer[name]
//     = Tnamed_pointer(&dgf_ADC_channel[i], 0, this) ;

    }


    // which general_detector works and how often
    name =   name_of_this_element + "_synchro_data" ;

    spec_synchro    =
        new spectrum_1D(name,
                        name,
                        500, 0, 8000,
                        folder) ;

    spectra_dgf.push_back(spec_synchro) ;

}
//*************************************************************************
void Tdgf::make_preloop_action(TIFJEvent* event_unpacked ,
                               TIFJCalibratedEvent* event_calibrated)
{
    tmp_event_unpacked =    event_unpacked ;


    //cout << "Tdgf::make_preloop_action "  << endl;
    // first we inform where are the data coresponding to these devices

    //simulate_event();

    // loop over the enumeration type
    for(unsigned int i = 0 ; i < general_detector.size() ; i++)
    {
        // remember the source of data
        /* to compile
               general_detector[i]->set_event_data_pointers(

                 event_unpacked->dgf_adc_energy,
                 event_unpacked->dgf_tdc_trig_time,
                 event_unpacked->dgf_xiapsa,
                 event_unpacked->dgf_user_psa);
        */



    }


    // calibrations and gates are read everytime we start the analysis

    read_calibration_and_gates_from_disk();

    // result of dialog = what means GOOD event
//    read_good_crystal_specification();
}
//**********************************************************************
void Tdgf::read_calibration_and_gates_from_disk()
{

    return ;

    cout << "\n\n\n\nReading DGFcalibration from the disk" << endl ;

    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    string cal_filename = "calibration/dgf_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            string mess = "I can't open dgf calibration file: "
                          + cal_filename  ;
            throw mess ;
        }

        // real reading the calibration -----------------
        for(unsigned int i = 0 ; i < general_detector.size() ; i++)
        {
            // cout << "reading calibration loop for dgf general_detector "
            // << i <<  endl ;
            general_detector[i]->make_preloop_action(plik) ;   // reference
        }
    }  // end of try -----------------

    //---------------------
    catch(Tno_keyword_exception &capsule)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << capsule.message << endl ;
        exit(-1) ;
    }
    //----------------------
    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1) ; // is it healthy ?
    }
    //-------------------

    cout << "DGF calibration successfuly read"  << endl;
}
//********************************************************
//********************************************************
//********************************************************
void Tdgf::analyse_current_event()
{
    synchro_data = tmp_event_unpacked->dgf_synchro_data;
    spec_synchro->manually_increment(synchro_data);
    //cout << "in Tdgf::analyse_current_event(), value " << synchro_data << endl;


    /*

        for(unsigned int i = 0 ; i < general_detector.size()  ; i++)
        {
           int value = tmp_event_unpacked->dgf_tdc_trig_time[i] ;
           if(value)
           {
              spec_tdc_ch[i]->manually_increment(value) ;
           }


           value =  tmp_event_unpacked->dgf_adc_energy[i];
           if(value)
           {
              spec_adc_ch[i]->manually_increment(value) ;
           }



           value =  tmp_event_unpacked->dgf_xiapsa[i];
           if(value)
           {
              spec_xia_psa_ch[i]->manually_increment(value) ;
           }


          value =  tmp_event_unpacked->dgf_user_psa[i];
           if(value)
           {
              spec_user_psa_ch[i]->manually_increment(value) ;
           }

        }

        synchro_data = tmp_event_unpacked->dgf_synchro_data;
        spec_synchro->manually_increment(synchro_data);
    //cout << "in Tdgf::analyse_current_event(), value " << synchro_data << endl;


        reference_time_stamp = tmp_event_unpacked->dgf_tdc_trig_time[0] ;
        //-----------------------------------  general_detectors -----------------------

        for(unsigned int i = 0 ; i < general_detector.size() ; i++)
        {
          if(tmp_event_unpacked->dgf_channel_fired[i])
                    general_detector[i]->analyse_current_event() ;
        }

    */
}
//***********************************************************************
void  Tdgf::make_postloop_action()
{
    // cout << "F.Tdgf::make_postloop_action() " << endl ;

    cout <<  "Saving postloop  " << spectra_dgf.size()
         << " DGF spectra..." << endl;
    for(unsigned int i = 0 ; i < spectra_dgf.size() ; i++)
    {
        spectra_dgf[i]->save_to_disk() ;
    }
    cout <<  "Saving DGF spectra - done " << endl;

}
//***********************************************************************
void Tdgf::simulate_event()
{
#ifdef NIGDY
    for(unsigned int i = 0 ; i < detector.size() ; i++)
    {
        //cout << "Loop i =  " << i << endl;
        for(int cri = 0 ; cri < 7 ; cri++)
        {
            tmp_event_unpacked->Cluster_data[i][cri][0] =
                (100 * i) + (10 * cri) + 0 ; // energy

            tmp_event_unpacked->Cluster_data[i][cri][1] =
                (100 * i) + (10 * cri) + 1; // time
        }
        // remember the source of data
    }
    //cout << "After loop " << endl;
#endif

}
//**********************************************************************
bool Tdgf::save_selected_spectrum(string name)
{
    for(unsigned int i = 0 ; i < spectra_dgf.size() ; i++)
    {
        if(name == spectra_dgf[i]->give_name())
        {
            spectra_dgf[i]->save_to_disk() ;
            return true ;
        }
    }
    return false ; // was not here, perhaps in rising or frs ?
}
//*********************************************************************
int Tdgf::give_synchro_data()
{
    return (int) synchro_data;
}
//*************************************************************************

