#ifndef _n_silicon_xy_H_
#define _n_silicon_xy_H_



#include <cstdlib>
#include <cstring>  // for memset

#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>
#include <sstream>
#include <iomanip>
#include "Tnamed_pointer.h"


#include <iomanip>

#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

// #define COTO  cout << "File: " << __FILE__ << ", line: " << __LINE__ << ", "; // endl;
// #define COTO  cout << "line: " << __LINE__ << ", "; // endl;

//class focus_point ;
//typedef unsigned int calkow;
typedef int calkow;
//////////////////////////////////////////////////////////////////////////////
template <calkow nr_of_channels>
class Tn_silicon_xy : public Tfrs_element , public Tincrementer_donnor
{

    static constexpr int nr_of_signals = 4;
    static constexpr int nr_of_strips = 16;     // 2 - is vertical/horizonatak x/y
    static constexpr int risetime_nr = 3;

    int signal_data[2][nr_of_strips][nr_of_signals];

    double signal_data_cal[2][nr_of_strips][nr_of_signals];
    vector<double>  cal_factors [2][nr_of_strips][nr_of_signals];

    double rise_time_cal[2][nr_of_strips];
    bool channel_has_fired_fully[2][nr_of_strips];

    int silicon_signal_data[2][nr_of_strips][512];


    int *signal_ptr;
    int nr = 0 ; // which nr of the silicon detector vector
    int threshold_of_raw = 0;
    int x_multpilicity, y_multpilicity;

public:
    //  constructor
    Tn_silicon_xy(string _name, int nr_);

    void analyse_current_event()  ;
    void make_preloop_action(ifstream &) ;   // read the calibration factors, gates

protected:
    void create_my_spectra();

    spectrum_1D * spec_chan[2][nr_of_channels][nr_of_signals];
    spectrum_1D * spec_chan_cal[2][nr_of_channels][nr_of_signals];
    spectrum_1D * spec_digitiser_signal[2][nr_of_channels];

    spectrum_2D * spec_xy_raw;
    spectrum_2D * spec_adc_vs_rise_time_cal[2][nr_of_channels];
    spectrum_2D * spec_X_adc_vs_rise_time_cal;
    spectrum_2D * spec_Y_adc_vs_rise_time_cal;

    // spectrum_2D * spec_X_ch_ampl_vs_rise_time_cal[nr_of_channels];
    // spectrum_2D * spec_Y_ch_ampl_vs_rise_time_cal[nr_of_channels];

    spectrum_1D * spec_fan;
    //bool flag_atmospheric_pressure_block;
};
////////////////////////////////////////////////////////////////////////////


//***********************************************************************
//  constructor
template <calkow nr_of_channels>
Tn_silicon_xy<nr_of_channels>::Tn_silicon_xy(string _name, int nr_arg)
    : Tfrs_element(_name),
    signal_ptr(nullptr),  nr(nr_arg)
//      flag_produce_fan_spectrum(flag_produce_fan_spectrum_arg),
//      flag_produce_signal_spectra(flag_produce_signal_spectra_arg),
//      flag_we_want_calibrate_signals(flag_we_want_calibrate_signals_arg)
{
    //   cout << "For "
    //   << name_of_this_element
    //   << " flag_we_want_calibrate_signals is "
    //   << flag_we_want_calibrate_signals << endl;
    //    flag_atmospheric_pressure_block = false;




    named_pointer[name_of_this_element + "_x_multiplicity"] = Tnamed_pointer(&x_multpilicity, 0, this) ;
    named_pointer[name_of_this_element + "_y_multiplicity"] = Tnamed_pointer(&y_multpilicity, 0, this) ;

    // other incrementers are defined in f. create_my_spectra
    create_my_spectra();

}
//************************************************************
template <calkow nr_of_channels>
void Tn_silicon_xy<nr_of_channels>::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor
    string folder = "silicons/" + name_of_this_element ;
    //-------------- for testing,  spectra of all ADC and TDCchannels

    // fan spectrum
    int zakres = 2000;
    spec_fan = new spectrum_1D(
        name_of_this_element + "_fan",
        zakres, 0, zakres,
        folder, " [X starts from 0, Y from 1000]     stripe nr * 10, then: \n 0- time, 1- amplitude, 2- pedestal, 3- rise-time  ", "X: No_such_incrementer_defined");
    frs_spectra_ptr->push_back(spec_fan) ;




    string signal_name[] = {"time", "amplitude", "pedestal", "risetime"};
    int len_mat = 64;
    int bin_mat = 16*len_mat;
    int len_y = 5000;

    for(int xy = 0 ; xy < 2 ; xy++){


        for(int i = 0 ; i < nr_of_channels ; i++)
        {
            // incrementers------------------


            ostringstream plik ;
            plik << name_of_this_element
                 << (xy? "_y" : "_x")
                 << "_"
                 << setfill('0')
                 << setw(2)
                 << i;

            string  inkr_name = plik.str() + "_rise_time_cal_when_fired";
            // named_pointer[inkr_name] = Tnamed_pointer(&rise_time_cal[i],
            //                                           &channel_has_fired_fully[i],
            //                                           this) ;

            string nam = plik.str() + "_ampl__vs__rise_time_cal"  ;
            string nam_y = plik.str() + "_amplitude_cal";


            spec_adc_vs_rise_time_cal[xy][i] = new spectrum_2D(nam,
                                                               bin_mat,
                                                               0, len_mat,
                                                               1000,
                                                               1, len_y,
                                                               folder, "",

                                                               string("X: ") +
                                                                   name_of_this_element +
                                                                   Tnamed_pointer::give_incrementer_name(&rise_time_cal[i])
                                                                   +
                                                                   "\nY: " + nam_y );

            frs_spectra_ptr->push_back(spec_adc_vs_rise_time_cal[xy][i]) ;





            for(int s = 0 ; s < nr_of_signals ; s++)
            {
                // incrementers
                ostringstream plik ;
                plik << name_of_this_element
                     //<< "_"
                     << (xy? "_y" : "_x")
                     << "_"
                     << setfill('0')
                     << setw(2)
                     << i << "_" << signal_name[s] ;

                string name = plik.str();
                named_pointer[name+"_raw"] = Tnamed_pointer(&signal_data[xy][i][s], 0, this) ;


                // spectra


                //      cout << "creating spectra for channel nr " << i   << " for [" << name << "] while nr_of_channels = " << nr_of_channels << endl;
                int bins = 4*4095;
                int length =  16*4096;
                if(s == 3)
                {
                    bins = 1024;
                    length =  bins;
                }
                spec_chan[xy][i][s] = new spectrum_1D(name + "_raw",
                                                      bins, 1, length,
                                                      folder, "",
                                                      "X: " + name + "_raw"
                                                      );
                frs_spectra_ptr->push_back(spec_chan[xy][i][s]) ;

                string name_cal = name + "_cal";
                //          named_pointer[name_cal ] = Tnamed_pointer(&signal_data_cal[i][s], 0, this) ;

                spec_chan_cal[xy][i][s] = new spectrum_1D(name_cal,
                                                          4095, 1, 4096,
                                                          folder, "",
                                                          "X: " + name_cal);
                frs_spectra_ptr->push_back(spec_chan_cal[xy][i][s]) ;

            }
            // signals from digitizer --------------------------------

            // incrementers
            ostringstream plik2 ;
            plik2 << name_of_this_element
                 //<< "_"
                 << (xy? "_y" : "_x")
                 << "_"
                 << setfill('0')
                 << setw(2)
                 << i  ;

            // string name =

            string name_moj = plik2.str();


            spec_digitiser_signal[xy][i] = new spectrum_1D(name_moj + "_digitiser_signal",
                                                           512, 0, 512,
                                                           folder, "",
                                                           "X: " + name_moj + "_digitiser_signal"
                                                           );
            frs_spectra_ptr->push_back(spec_digitiser_signal[xy][i]) ;

        }

    }
    // TODO tworzenie widma XY

    string name = name_of_this_element + "_map_xy_raw"  ;
    spec_xy_raw = new spectrum_2D(name,
                                  nr_of_channels,
                                  0.0, nr_of_channels,
                                  nr_of_channels,
                                  0.0, nr_of_channels,
                                  folder, "",
                                  string("X: No_such_incrementer_defined\n") +
                                      "Y: No_such_incrementer_defined");
    frs_spectra_ptr->push_back(spec_xy_raw) ;

    //=========================
    for(int i = 0 ; i < nr_of_channels ; i++)
    {


    }




    // SUMARYCZNE wszystkie paski-----------------
    string nam = name_of_this_element + "_ALL_X_ampl__vs__rise_time_cal"  ;
    spec_X_adc_vs_rise_time_cal = new spectrum_2D(nam,
                                                  bin_mat,
                                                  0, len_mat,
                                                  1000,
                                                  1, len_y,
                                                  folder, "",
                                                  "X: No_such_incrementer_defined\n"
                                                  "Y: No_such_incrementer_defined");
    frs_spectra_ptr->push_back(spec_X_adc_vs_rise_time_cal) ;


    nam = name_of_this_element + "_ALL_Y_ampl__vs__rise_time_cal"  ;

    spec_Y_adc_vs_rise_time_cal = new spectrum_2D(nam,
                                                  bin_mat,
                                                  0, len_mat,
                                                  1000,
                                                  1, len_y,
                                                  folder, "",
                                                  "X: No_such_incrementer_defined\n"
                                                  "Y: No_such_incrementer_defined"
                                                  );

    frs_spectra_ptr->push_back(spec_Y_adc_vs_rise_time_cal) ;

}
//*************************************************************
template <calkow nr_of_channels>
void Tn_silicon_xy<nr_of_channels>::make_preloop_action(ifstream &)
{

    // as almost nobody needs the calibration(only munich people for Sn100 experiment
    // so the calibration will be in the file related to this module


    //    signal_ptr = &event_unpacked->silicon[0][0];
    signal_ptr = &event_unpacked->silicon[nr][0][0][0];


    cout << "Reading the calibration for " << name_of_this_element << endl ;

    for(int xy = 0 ; xy < 2; xy++)
        for(int i = 0 ; i < nr_of_channels ; i++)
            for(int s = 0 ; s < nr_of_signals ; s++)
                cal_factors [xy][i][s].clear();


    //    string cal_filename = "calibration/" + name_of_this_element +  "_calibration.txt";
    string cal_filename = "calibration/" + string("drac") +  "_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;  // ascii file
        if(!plik)
        {
            string mess = "I can't open calibration file: "
                          + cal_filename  ;
            throw mess ;
        }
        vector<string> signame = { "time", "amplitude", "pedestal", "risetime" };
        for(int xy = 0 ; xy < 2 ; xy++)
            for(int s = 0 ; s < nr_of_signals ; s++)
            {
                if(s == 2) continue;
                for(int ch = 0 ; ch < nr_of_channels ; ch++)
                {
                    string znak = xy == 0 ? "_x" : "_y" ;
                    ostringstream slowo;
                    slowo <<  name_of_this_element + znak +
                                 "_channel_" << setw(2) << setfill('0') << ch
                          << "_" << signame[s]
                          <<  "_calibr_factors" ;

                    double tmp =
                        FH::find_in_file(plik,  slowo.str());
                    cal_factors [xy][ch][s].push_back(tmp);  // ofset

                    plik >> zjedz >> tmp;
                    if(!plik) {
                        Treading_value_exception a;
                        a.message = "  error while reading slope value for:  " +
                                    slowo.str();
                        throw a;
                    }
                    cal_factors [xy][ch][s].push_back(tmp);  // slope

                    //            plik >> zjedz >> tmp;
                    //            cal_factors [i][s].push_back(tmp);  // quadratic
                }// end for

                string name = "threshold_of_sensible_raw_value";
                double tmp =
                    FH::find_in_file(plik,  name);
                threshold_of_raw = tmp;
            }

    }  // end of try
    catch(Tno_keyword_exception &k)
    {
        cerr << "Error while reading the calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
            ; // is it healthy ?
    }
    catch(Treading_value_exception &k)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message
             << endl  ;
        exit(-1)
            ; // is it healthy ?
    }
    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << sss << endl  ;
        exit(-1)
            ; // is it healthy ?
    }




    //   cout << "End of reaadin calibration" << name_of_this_element
    //   << ", flag_we_want_calibrate_signals= " << flag_we_want_calibrate_signals
    //   << endl;
}
/////////////////////////////////////////////////////////////////////////


//*************************************************************
template <calkow nr_of_channels>
void Tn_silicon_xy<nr_of_channels>::analyse_current_event()
{
    constexpr int TESTING_DIVISION = 4;  // because there is no proper amplitude calibration yet

    if(    event_unpacked->silicon_fired[nr]){

        // cout << "analyse_current_event()  for "
        //      <<  name_of_this_element
        //      << endl ;
        // cout << "------------------ fired " << endl ;
    }else {
        // cout << " not fired " << endl;
        return;
    }

    memcpy(signal_data, event_unpacked->silicon[nr], sizeof(signal_data)) ;

    memcpy(silicon_signal_data, event_unpacked->silicon_signals[nr], sizeof(silicon_signal_data));

    bool flag_any_data_in_the_event = false;
    x_multpilicity = 0 ;
    y_multpilicity = 0 ;

    for(int xy = 0 ; xy < 2 ; xy++)
        for(int ch = 0 ; ch < nr_of_channels ; ch++)
        {
            channel_has_fired_fully[xy][ch] = false;
            int how_many_signals_has_this_channel = 0;
            rise_time_cal[xy][ch] = 0;

            for(int s = 0 ; s < nr_of_signals ; ++s)
            {

                // signal_data[xy][ch][s] = *(signal_ptr + (nr_of_signals*ch) + s);
                signal_data_cal[xy][ch][s] = 0;


                if(signal_data[xy][ch][s])
                {
                    // cout << name_of_this_element
                    //      << " has  signal data [" << xy << "]["<< ch << "][" << s << "] = "
                    //      <<  signal_data[xy][ch][s]
                    //      << endl;
                }

                // if(s==0)
                // {  // PO CO? - is time non zero?

                //     if(signal_data[xy][ch][0] < threshold_of_raw) signal_data[xy][ch][0] = 0;
                // }

                bool have_ch_s_data = (signal_data[xy][ch][s] != 0);
                //        signal_data_cal[i] = 0;

                if(!have_ch_s_data)
                    continue;

                if(signal_data[xy][ch][s] != 0) how_many_signals_has_this_channel++;

                flag_any_data_in_the_event = flag_any_data_in_the_event || have_ch_s_data;
                if((signal_data[xy][ch][s] != 0) )
                {
                    spec_chan[xy][ch][s]->manually_increment(signal_data[xy][ch][s]);

                    // if(s ==0)
                    // {
                    //     cout
                    //         << "Incrementing spectrum "
                    //         << name_of_this_element
                    //         << "  with silicon[" << xy << "][" << ch
                    //         << "][" << s << "] = "
                    //         <<  signal_data[xy][ch][s]
                    //         << endl;
                    // }
                }

                //      COTO;
                if(have_ch_s_data)
                {
                    spec_fan->manually_increment(( xy* 1000) + (ch*10) +s);
                }

                // calibrating -------------------
                if(s != 2)  // not for pedestal
                {
                    double tmp = signal_data[xy][ch][s] + randomizer::randomek() ;
                    signal_data_cal[xy][ch][s] =
                        (cal_factors [xy][ch][s][1] * tmp) +
                        (cal_factors [xy][ch][s][0]);

                    // cout << "after calibratnion  " << name_of_this_element
                    //      << "   " << signal_data[xy][ch][s] << " ----> is ["
                    //      << xy << "][" << ch << "][" << s <<  "] "
                    //      << signal_data_cal[xy][ch][s] << endl;
                    spec_chan_cal[xy][ch][s]->manually_increment(signal_data_cal[xy][ch][s]);

                }

            }// end for s

            if(silicon_signal_data[xy][ch][0])
            {

                // clear all spectrum
                spec_digitiser_signal[xy][ch]->zeroing();

                for(int i = 0 ; i < 512 ; ++i)
                {
                    // if ( i < 5 )
                    //     cout << name_of_this_element
                    //          << ", channel " <<  i << " of spectrum silicon_signal_data["
                    //          << xy
                    //          << "][" << ch<< "] = "
                    //          << silicon_signal_data[xy][ch][i]
                    //          << endl;
                    // spec_digitiser_signal[xy][ch]->manually_inc_by(i, i);
                    spec_digitiser_signal[xy][ch]->manually_inc_by(i, silicon_signal_data[xy][ch][i]);
                }

                // int nic = 8;

            }

            if(how_many_signals_has_this_channel == nr_of_signals)
            {
                channel_has_fired_fully[xy][ch] = true;

                if(ch < (nr_of_channels/2)) x_multpilicity++;
                else y_multpilicity++;

                rise_time_cal[xy][ch] = fabs(signal_data_cal[xy][ch][risetime_nr]) ;

                spec_adc_vs_rise_time_cal[xy][ch]->manually_increment(
                    rise_time_cal[xy][ch],  // x:  rise_time
                    signal_data_cal[xy][ch][1] / TESTING_DIVISION //: y ampl
                    );


                if(xy == 0 )   // X or Y
                {
                    spec_X_adc_vs_rise_time_cal->manually_increment(
                        rise_time_cal[xy][ch],  // x:  tim80 - tim30
                        signal_data_cal[xy][ch][1] / TESTING_DIVISION //: y ampl
                        );

                }
                else {
                    spec_Y_adc_vs_rise_time_cal->manually_increment(
                        rise_time_cal[xy][ch],  // x:  tim80 - tim30
                        signal_data_cal[xy][ch][1] / TESTING_DIVISION //: y ampl
                        );
                }
            }

        } // end for ch (all strips)


    // Incrementing xy map ---------------------------------
    if(flag_any_data_in_the_event)
    {
        // finding max X
        double x = 0;
        //int value_max_x = -1;
        double y = 0;
        //int value_max_y = -1;

// bool flag_x_OK {false};
// bool flag_y_OK {false};


#if 0
        int which_signal = 0 ; // <--amplitude
        int fired_x = 0 ;
        int fired_y = 0 ;
    // badziemy liczyc srednia z nr paska
        for(int xy = 0 ; xy < 2 ; xy++)
            for(int ch = 0 ; ch < nr_of_channels ;++ch)
            {

                if(xy == 0){
                    if(signal_data[xy][ch][which_signal]
                        && signal_data[xy][ch][1] > threshold_of_raw
                        && signal_data[xy][ch][2] > threshold_of_raw
                        ){
                        x += ch;
                        fired_x++;
                        // cout << "fired x channel " << ch << endl;
                    }
                }else{
                    if(signal_data[xy][ch][which_signal]
                        && signal_data[xy][ch][1] > threshold_of_raw
                        && signal_data[xy][ch][2] > threshold_of_raw
                        )
                    {
                        y += ch;
                        fired_y++;
                        // cout << "fired y channel " << ch << endl;

                    }
                }
            }
        if(fired_x)
        {
            // x /= fired_x;
            flag_x_OK = true;
        }
        if(fired_y){
            // y /= fired_y;
            flag_y_OK = true;
        }
#else
        x_multpilicity = 0 ;
        y_multpilicity = 0;

        for(int xy = 0 ; xy < 2 ; xy++)
            for(int ch = 0 ; ch < nr_of_channels ;++ch)
            {

                if(xy == 0){
                    if(
                        signal_data[xy][ch][1] > threshold_of_raw   // amplit
                        )
                    {
                        x = ch;
                        // fired_x++;
                        x_multpilicity++;
                        // cout << "fired x channel " << ch << " x_multpilicity " << x_multpilicity << endl;
                    }
                }else{
                    if(
                        (signal_data[xy][ch][1] > threshold_of_raw)
                        &&
                        (signal_data[xy][ch][0] > 143) // threshold_of_raw  CZASOWO
                        )
                    {
                        y = ch;
                        // fired_y++;
                        y_multpilicity++;
                        // cout << "fired y channel " << ch << " y_multpilicity " << y_multpilicity << endl;

                    }
                }
            }
#endif




        // macierz xy --------------------------
        if(x_multpilicity == 1 && y_multpilicity == 1)
        // if(flag_x_OK && flag_y_OK)
        {
            spec_xy_raw->manually_increment(x, y);

            // cout
            //     << "---> Incrementing matrix "
            //     << name_of_this_element
            //     << " at [" << x << "][" << y << "] "
            //     << endl;
        }
    }

    calculations_already_done = true ;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif // _n_silicon_xy_H_
