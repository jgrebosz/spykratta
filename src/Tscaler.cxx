/***************************************************************************
                          tscaler.cpp  -  description
                             -------------------
    begin                : Tue May 20 2003
    copyright            : (C) 2003 by Jurek Grebosz (IFJ Krakow, Poland)
    email                : jerzy.grebosz@ifj.edu.pl
 ********************************************** *****************************/

#include "Tscaler.h"
#include "Tnamed_pointer.h"


//#define KATIA_SEETRAM_CALIBRATION

// to make more precise measurement the scalers can work with 10Hz clock instead
// of 1Hz. This you can change in the file Tfrs.cxx
// by defining the following macros
// //#define clocking  scaler_frs_1Hz_clock
// #define clocking  scaler_frs_10Hz_clock
// Note: do not uncomment this here, this is just an explanation.


#ifdef KATIA_SEETRAM_CALIBRATION
ofstream plik;
#endif

//************************************************************************
Tscaler::Tscaler(string _name,
                 int TIFJEvent::*data,
                 int TIFJEvent::*seconds_
                ) : Tfrs_element(_name),
    data_ptr(data),
    time_of_the_measurement_in_seconds(seconds_)
{


#ifdef KATIA_SEETRAM_CALIBRATION
    static bool plik_opened = 0;

    if(!plik_opened)
    {
        plik.open("seetram_calibration_data.txt");
        plik_opened = true;
    }
#endif

    seconds_in_channel_zero = 0 ;
    previous_counts = 0 ;
    last_current_counts = 0 ;
    current_counts = 0 ;
    difference_value = 0 ;
    suwak = 0 ;

    counts_diff_in_last_unit_of_time = 0;
    previous_time_of_the_measurement_in_seconds = 0;


    create_my_spectra();

    named_pointer[name_of_this_element + "_previous_counts"]
    = Tnamed_pointer(&previous_counts, 0, this) ;

    named_pointer[name_of_this_element + "_current_counts"]
    = Tnamed_pointer(&current_counts, 0, this) ;

    named_pointer[name_of_this_element + "_difference_value"]
    = Tnamed_pointer(&difference_value, 0, this) ;


    named_pointer[name_of_this_element + "_counts_diff_in_last_unit_of_time"]
    = Tnamed_pointer(&counts_diff_in_last_unit_of_time, 0, this) ;

}
//************************************************************************
Tscaler::~Tscaler()
{}
//************************************************************************
void  Tscaler::create_my_spectra()
{

    //the spectra have to live all the time, so here we can create
    // them during  the constructor


    //   string folder = "scalers/" + name_of_this_element ;
    string folder = "scalers/" ;
    string comment = string("Unit is 1s. This graph will slowly move left, just like the paper tape") ;
    // relative -----------------------------------
    string name = name_of_this_element   ;
    spec_rel = new spectrum_1D(name, name,
                               //                              400, 0, 400,
                               1000, 0, 1000,
                               folder, comment);

    frs_spectra_ptr->push_back(spec_rel) ;

}
//#ifdef NIGDY
#if  1
//**********************************************************************
void Tscaler::analyse_current_event()
{
//   cout << "analyse_current_event()  for "
//            <<  name_of_this_element
//             << endl ;

    static bool flag_first_time = true ;
    // -------------------------------------------------------------
    current_counts =  event_unpacked->*data_ptr ;

    /*--- */
#ifdef KATIA_SEETRAM_CALIBRATION
    bool flag_is_block = false;
    if(

        /*
        name_of_this_element == "scaler_frs_sc01"
             || name_of_this_element == "scaler_frs_sc41"
             || name_of_this_element == "scaler_frs_seetram_old_dig"
             || name_of_this_element == "scaler_frs_seetram_new_dig"
             || name_of_this_element == "scaler_frs_IC_new_dig"
             || name_of_this_element == "scaler_frs_IC_old_dig"
        */
        name_of_this_element == "scaler_frs_accepted_trigger"
        || name_of_this_element == "scaler_frs_free_trigger"
        || name_of_this_element == "scaler_frs_seetram_new_dig"
        || name_of_this_element == "scaler_frs_seetram_old_dig"

    )
    {
        flag_is_block = true ;
    }
    /*---*/



    /*
    scaler_frs_accepted_trigger
    scaler_frs_free_trigger
    scaler_seetram_new_dig
    scaler_seeteram_old_dig

    */
    // sometime we want to know how many we had in the last unit of time
    // (difference says only the difference since last event)

    if(int time_diff = event_unpacked->*time_of_the_measurement_in_seconds -
                       previous_time_of_the_measurement_in_seconds > 0)
    {
        counts_diff_in_last_unit_of_time =
            (current_counts - counts_at_begining_of_last_unit) / time_diff;


        if(flag_is_block)
        {
            if(!plik)
            {
                cout << "Cannot write " << endl;
            }

            /*---
                 if(name_of_this_element == "scaler_frs_sc01")
                    plik << "sc01    " ;
                  else if(name_of_this_element == "scaler_frs_sc41")
                    plik << "sis     " ;
                  else if(name_of_this_element == "scaler_frs_seetram_old_dig")
                    plik << "seetram_old " ;
                  else if(name_of_this_element == "scaler_frs_seetram_new_dig")
                    plik << "seetram_new " ;
                  else if(name_of_this_element == "scaler_frs_IC_new_dig")
                    plik << "IC_new " ;
                  else if(name_of_this_element == "scaler_frs_IC_old_dig")
                    plik << "IC_old " ;
            */

            if(name_of_this_element == "scaler_frs_accepted_trigger")
                plik << "acc_trig    " ;
            else if(name_of_this_element == "scaler_frs_free_trigger")
                plik << "free_trig   " ;
            else if(name_of_this_element == "scaler_frs_seetram_old_dig")
                plik << "seetram_old " ;
            else if(name_of_this_element == "scaler_frs_seetram_new_dig")
                plik << "seetram_new " ;



            plik
            // << name_of_this_element
            //<< "\nChanging the time unit, diff counts = "
                    << "\t"

                    << counts_diff_in_last_unit_of_time
                    //<< "curr counts = " << current_counts
                    //<< ", counts_at_begining_of_last_unit = "<< counts_at_begining_of_last_unit
                    //<< " (now time is = "
                    << "\t"
                    <<  event_unpacked->*time_of_the_measurement_in_seconds
                    //<< " previously was " << previous_time_of_the_measurement_in_seconds
                    << "\t";

            //if(name_of_this_element == "scaler_frs_sc01") plik << endl;
            if(name_of_this_element == "scaler_frs_seetram_new_dig") plik << endl;

        }

        // here saving for Katja

        counts_at_begining_of_last_unit = current_counts;
        previous_time_of_the_measurement_in_seconds =
            event_unpacked->*time_of_the_measurement_in_seconds;

    }



    //   if(flag_is_block)
    //       {
    //        cout << "NO chacnde, diff counts = "
    //        << counts_diff_in_last_unit_of_time << endl;
    //       }

#endif  //  KATIA_SEETRAM_CALIBRATION



    if(current_counts > 0)
    {

//   cout << name_of_this_element << " = "  << current_counts << ", while time "
//   << event_unpacked->*time_of_the_measurement_in_seconds << endl;
//
//    if(name_of_this_element == "scaler_frs_free_trigger")
//    {
//    current_counts = current_counts ;  // fake for breakpoiint
//    }
//

        unsigned int channel_to_increment ;

        channel_to_increment =
            event_unpacked->*time_of_the_measurement_in_seconds -
            seconds_in_channel_zero  ;



        if(flag_first_time)
        {
            seconds_in_channel_zero = (unsigned)(event_unpacked->*time_of_the_measurement_in_seconds);
            flag_first_time = false ;
        }


        if(channel_to_increment > 1000)  // there was some garbage
            //    if(channel_to_increment > 100000 )  // there was some garbage
        {
            channel_to_increment = 0 ;
            seconds_in_channel_zero = (unsigned)(event_unpacked->*time_of_the_measurement_in_seconds);
        }

        if(channel_to_increment > 960)
            //    if(channel_to_increment > 99000)
        {
            // scroll by one minute
            int scroll_nr = 60 ;

            seconds_in_channel_zero += scroll_nr ;
            spec_rel->scroll_left_by_n_bins(scroll_nr);
            channel_to_increment -= scroll_nr ;
        }


        previous_counts = last_current_counts ;

        // the first incrementation will be a nonsese !, so...
        difference_value = (previous_counts != 0) ? current_counts - previous_counts  :  0 ;

        //    cout << "scaler= " <<name_of_this_element
        //    << ",  channel " <<(int)channel_to_increment
        //    << ", with difference_value " << difference_value << endl ;


        spec_rel ->manually_inc_by((int)channel_to_increment, difference_value) ;
        //  spec_rel ->manually_inc_by((int)suwak++, difference_value ) ;

        last_current_counts = current_counts;


    } // endif sensible y data
    calculations_already_done = true ;
}
//**************************************************************************
#else
//**********************************************************************
void Tscaler::analyse_current_event()
{
//   cout << "2 analyse_current_event()  for "
//            <<  name_of_this_element
//             << endl ;
    //  bool flag_digitizer = false;

    //  if(name_of_this_element.find("digitizer") != string::npos)
    //  {
    //    flag_digitizer = true;
    //  }

    // warning - this below is for EVERY scaler !!! ???
    //  static long long timestamps_FRS_at_beg_of_current_suwak_bin ;
    // -------------------------------------------------------------
    current_counts =  event_unpacked->*data_ptr ;

    if(current_counts > 0)
    {

        previous_counts = last_current_counts ;

        // the first incrementation will be a nonsese !, so...
        difference_value = (previous_counts != 0) ? current_counts - previous_counts  :  0 ;

        // cout << "now timestamp FRS " << event_unpacked->timestamp_FRS
        //       << ", last timestamp " << timestamps_FRS_at_beg_of_current_suwak_bin << endl;

        int seconds_difference = (event_unpacked->timestamp_FRS - timestamps_FRS_at_beg_of_current_suwak_bin) / 50000000L ;

        //    if(flag_digitizer)
        //    {
        //      int bre = 8 ;  // just a breakpoint
        //    }


        if(seconds_difference > 0)
        {

            //      if(flag_digitizer)
            //      {
            //        int bre = 8 ;  // just a breakpoint
            //      }


            suwak += (seconds_difference < 300) ? seconds_difference : 300 ;
            //      if(flag_digitizer)
            //      {
            //        cout << "Moved seconds by  " << seconds_difference << endl ;
            //      }
            timestamps_FRS_at_beg_of_current_suwak_bin = event_unpacked->timestamp_FRS ;

            if(suwak >= right_edge)
            {
                //        if(flag_digitizer)
                //        {
                //          cout << "Scrolling..."  << endl ;
                //        }
                spec_rel->scroll_left_by_n_bins(25);
                suwak -= 25;
            }

        }

        spec_rel ->manually_inc_by((int)suwak, difference_value) ;
        //    if(flag_digitizer)
        //    {
        //      cout << "scaler= " <<name_of_this_element
        //          << ",  suwak " <<(int)suwak
        //          << ", with difference_value " << difference_value << endl ;
        //    }

        last_current_counts = current_counts;




    } // endif sensible y data
    calculations_already_done = true ;
}
#endif // else nigdy

//**************************************************************************
void  Tscaler::make_preloop_action(ifstream & s)  // read the calibration factors, gates
{
    //cout << "preloop action for Reading the calibration for " << name_of_this_element << endl ;

    // remembering the start ?

    // ask what is ther right egde of the "paper"

    right_edge = spec_rel->give_max_chan_paper();

}
//***************************************************************
/** dead time scaler may ask other scalers for their results
 */
int Tscaler::give_last_value()
{
    if(!calculations_already_done) analyse_current_event();
    return last_current_counts ;
}
