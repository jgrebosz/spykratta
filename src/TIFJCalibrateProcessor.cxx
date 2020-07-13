

//---------------------------------------------------------------
//        Go4 Framework release package V.1.0000
//                      24-May-2002
//---------------------------------------------------------------
//   The GSI Online Offline Object Oriented (Go4) Project
//   Experiment Data Processing at DVEE department, GSI
//---------------------------------------------------------------
//
//Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
//                    Planckstr. 1, 64291 Darmstadt, Germany
//Contact:            http://go4.gsi.de
//----------------------------------------------------------------
//This software can be used under the license agreements as stated
//in Go4License.txt file which is part of the distribution.
//----------------------------------------------------------------

/* Generated by Together */

#include "TIFJCalibrateProcessor.h"

#include "TIFJCalibratedEvent.h"
//#include "Go4Analysis/TGo4Analysis.h"
#include "TIFJAnalysis.h"
extern TIFJAnalysis *  RisingAnalysis_ptr ;


//******************************************

const int debug_level = 1 ;

#define Dcout(nnn) if(debug_level >= nnn) cout


// #define COTO  cout<<" I am in " << __FILE__ << " at line  "<<__LINE__<< endl ;
#define ANYKEY  { cout << "Stop.  Press: '1' and Enter " ; int iii ; cin >> iii ; }

//-------------------------------------------------------------------------
/*------------------------------------------------------------------------*/
istream & zjedz(istream & plik)
{
    char c = 'x' ;
    do
    {
        plik >> c  ;
        if(c == '/' && (plik.peek() == '/'))
        {
            std::string schowek;
            getline(plik, schowek);
        }
        else
        {
            plik.putback(c) ;
            return plik ;
        }
    }
    while(plik) ;
    return plik ;
}
/**************************************************************************/
//**************************************************************************
TIFJCalibrateProcessor::TIFJCalibrateProcessor(string  name_arg)
    : TjurekAbstractEventProcessor(name_arg)
{
//   TRACE((14,"TIFJCalibrateProcessor::TIFJCalibrateProcessor(string)",
//          __LINE__, __FILE__));

    event_nr = 0  ;

}
//**************************************************************************
/* default constructor */
TIFJCalibrateProcessor::TIFJCalibrateProcessor()
    : TjurekAbstractEventProcessor("EBCalibrateProcessor")
{
//   TRACE((14,"TIFJCalibrateProcessor::TIFJCalibrateProcessor()",
//          __LINE__, __FILE__));

//     cout << "The defaut constuctor for the class TIFJCalibrateProcessor() " << endl ;
    event_nr = 0;
}

//**********************************************************************************
/* destructor */
TIFJCalibrateProcessor::~TIFJCalibrateProcessor()
{
//   TRACE((14,"TIFJCalibrateProcessor::~TIFJCalibrateProcessor()",
//          __LINE__, __FILE__));

    //cout << "~TIFJCalibrateProcessor  Destructor begins" << endl ;

    // // WARNING: never delete the histograms which are added to the framework
    //   // with AddHistogram. They are adopted by the Go4 Folders!!!
    //   // causes SEGV if analysis is closed and initialized again!!!
    //   // the proper way is to remove the histograms from the framework...

    // cout << "Przed wywol w destruktorze " << endl ;
    //  dynamic_cast<TIFJAnalysis*> (TGo4Analysis::Instance())     ->analysis_submited(false) ;
    //cout << " End of destructor " <<endl ;
}
//************************************************************************

//***********************************************************************
//***********************************************************************
//***********************************************************************
// working on every event
//***********************************************************************
//***********************************************************************
void TIFJCalibrateProcessor::BuildCalibratedEvent(TIFJCalibratedEvent* /*target*/)
{
//   TRACE((11,"TIFJCalibrateProcessor::TIFJCalibrateProcessor(Int_t)",
//          __LINE__, __FILE__));

    Dcout(5)  << "\nFunction Step2 : Build CalibratedEvent%%%%%%%%%%%%%%%%%%%%%%%%" << endl ;

//     TIFJEvent* input = 0;
//     TIFJCalibration* cal = 0;
//     TIFJPar* ps_par = 0 ;

    //target->SetValid(kFALSE);       // events are not stored until kTRUE is set

//     input= ( TIFJEvent* ) GetInputEvent();
//     cal= ( TIFJCalibration* ) GetCalibration();

    //  target->set_value(-250);
    /*    if ( ! input->IsValid() )
        {
            //cout << "Not valid event " << endl;
            target->SetValid ( false );    // do not save the ntuple event
            return ;
        }
    */


    RisingAnalysis_ptr->nr_events_analysed =  ++event_nr ;

    // we check this true event number
    /** Flag telling that now we want to dump information about the current event */
    if(verbose_mode_enabled)
    {
        if(event_nr >= verbose_begins_with_event_nr
                &&
                event_nr < (verbose_begins_with_event_nr + verbose_how_many_events)
          )
        {
            flag_verbose_events = true;
        }
        else
        {
            flag_verbose_events = false ;
        }
        RisingAnalysis_ptr->set_verbose(flag_verbose_events) ;
    }

    //     cout << "\n---- created Valid Event nr " << (event_nr) << endl;

    static int period = 2000;
    static int last_print = time(NULL); ;
    if(!(event_nr % period)  || flag_verbose_events)
    {
        //    RisingAnalysis_ptr* ptr_anal =
        //    dynamic_cast<RisingAnalysis_ptr*> (TGo4Analysis::Instance());

        cout << "\n---- created Valid Event nr " << (event_nr)
             << " ----   " << RisingAnalysis_ptr->give_current_filename()
             << endl ;
        time_t now = time(NULL);
        if((now  - last_print <  5)  && (flag_verbose_events== false) )
        {
            period *= 2;
        }
        last_print = now;


        if(flag_verbose_events)
            cout << "VERBOSE MODE: "
            // "period =" << period
            << endl ;
    }

//     target-> ( TRUE );    // events are not stored until kTRUE is set

    //RisingAnalysis_ptr* ptr_anal = RisingAnalysis_ptr ;
    //    dynamic_cast<TIFJAnalysis*> (TGo4Analysis::Instance());


    // bool multiplicity_ok = ptr_anal ->rising.first_check_cluster_multiplicity();
    // ptr_anal ->frs.remember_rising_cluster_multiplicity(multiplicity_ok);
    // this multiplicity is also used for CATE

    RisingAnalysis_ptr -> make_calibration_step();   // here is also cate !
   // ptr_anal -> make_hector_calibration_step();   // here is also cate !
    
    
    RisingAnalysis_ptr -> put_data_to_ntuple();

    // if(! (event_nr % 50000)) save_member_spectra();

    return ;
}
//******************************************************************
//*******************************************************************
/** For Franco Camera wish to make dump of selected events */
void TIFJCalibrateProcessor::read_in_verbose_parameters()
{
    flag_verbose_events = false ;
    RisingAnalysis_ptr->set_verbose(flag_verbose_events) ;


    string nazwa = "commands/verbose_mode.command" ;
    ifstream plik(nazwa.c_str());
    if(plik)
    {
        try
        {
            verbose_mode_enabled =
                (bool) Tfile_helper::find_in_file(plik, "verbose_mode_enabled");

            verbose_begins_with_event_nr  =
                (int) Tfile_helper::find_in_file(plik,
                                                 "verbose_begins_with_event_nr");

            verbose_how_many_events =
                (int) Tfile_helper::find_in_file(plik,
                                                 "verbose_how_many_events");
        }
        catch(Tfile_helper_exception &m)
        {
            cout << "Error while trying to read the file " << nazwa
                 << "\n Reason is: " << m.message << endl;
            exit(1);
        }

    }
    else
    {
        verbose_begins_with_event_nr  = 0 ;
        verbose_how_many_events = 0;
        verbose_mode_enabled = false ;
    }

}

//****************************************************************************
// this function is called from TIFJAnalysis

//****************************************************************************
/** No descriptions */
void TIFJCalibrateProcessor::preLoop()
{
    read_in_verbose_parameters();
}
//**********************************************************


//ClassImp ( TIFJCalibrateProcessor )


