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
#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;

//#include "TROOT.h"
//#include "TRint.h"
//#include "TApplication.h"
//#include "TH1.h"
//#include "TBrowser.h"
//#include "TCanvas.h"
//#include "TBenchmark.h"
#include "TIFJAnalysis.h"
//#include "Go4AnalysisClient/TGo4AnalysisClient.h"

#include "Tcracow_exception.h"
#include "lockspy_2.h"

void usage(); // print announcement

#ifdef NOGO4
TROOT go4application("The GO4", "This is the Go4");
#endif


#define kRemote 2
#define kBatch  1

#define WHAT  cout << "File " << __FILE__ << ", Line= " << __LINE__ << endl ;


// global variable
extern bool go4_gui_mode ;  // Take does not want to save *.spc files in Go4 mode

unsigned int starting_event; // global to be set in main and accessed from constructor of TGo4Analysis.cxx

//************************************************************************
//================== EB analysis main program ============================
//************************************************************************
int main(int argc, char **argv)
{
    // keep users from running multiple spies in one directory

#ifdef __DEBUG__
    cout << "This is a DEBUG version of the program " << endl;
#else
    cout << "This is a RELEASE version of the program " << endl;
    // setup_lock();
#endif

    // argv[0] program
    // argv[1] "-gui" when started by GUI.
    // argv[2] analysis name as specified in GUI
    // argv[3] hostname of GUI
    // argv[4] connector port of GUI
    //
    // If not "-gui", following args are free to application
    cout << "This version of SPY was fully recompiled "
         << __DATE__
         << "  "
         << __TIME__
         << endl;

    for(int i = 0; i < argc; i++) printf("%d %s\n", i, argv[i]);

    if(argc < 2)
    {
        usage(); // no argument: print usage
        exit(0);
    }
    int runningMode = kRemote;   // runningMode of analysis, kRemote or kBatch
    go4_gui_mode = true;
    if(strcmp(argv[1], "-gui") != 0)
    {
        runningMode = kBatch; // no GUI
        go4_gui_mode = false ;
    }

    // Now we know the running mode


//  TApplication     theApp("App", 0, 0); // init root application loop



    int maxevents = -1; // number of events (batch mode)
   
    string myfile;

    string tmp = "Current";
  // int len = tmp.size() ;
    myfile  = tmp;

    bool flag_strange_source = false ;

    string kawalek = string(argv[1]).substr(0, 4) ;
    if(kawalek == "-str"
            ||
            kawalek == "-tra"
            ||
            kawalek == "-soc"   // socket - for exotic experiment
            ||
            kawalek == "-rem"
      )
    {
        flag_strange_source = true ;
    }
    else
    {
        //================= Begin  EB analysis ==================================
        // Prepare the EB analysis
        // In batch mode the arguments are
        // argv[1] filename
        // argv[2] optional number of events or -1

        if(runningMode == kRemote)      // GUI analysis
        {
            //len = strlen(argv[2]);
            myfile = argv[2]; // build file names from analysis name
            //unsigned int iport = atoi(argv[4]);

        }
        else
        {
            // This is the normal analyze of the file

            // Perhaps there is a file name and somebody put *.lmd there?
            string name = argv[1];
            string::size_type pos = name.find(".lmd");
            if(pos != string::npos)
            {
                name.erase(pos);
            }

            //len = name.size();
            myfile = name.c_str() ;
            if(argc > 2)
                maxevents = atoi(argv[2]);

            if(argc > 3)
                starting_event = atoi(argv[3]);
        }
    }

    if(maxevents == -1) maxevents = 999999999;
//========================================================
    string mylmd = myfile; // input file with full path
    string myasf;
    string myraw;
    string mycal;
    string myusr;

    // strip any path information from output file name:
    string   lastname = mylmd;
    string   outname = lastname;

    // removing the path before
    string::size_type pos = lastname.rfind("/");
    if(pos != string::npos)
    {
        outname = lastname.substr(pos + 1, string::npos);
    }
    // if this is a command file with extension, we change the point into '_'
    pos = outname.rfind(".");
    if(pos != string::npos)
    {
        outname[pos] = '_';
    }

    myasf = outname;
    myraw = outname;
    mycal = outname;
    myusr = outname;

    // Now setup the EB analysis itself

    TIFJAnalysis*  myanalysis = 0 ;
    if(flag_strange_source == false)
    {
//       mylmd += ".lmd"; // input GSI listmode
        mylmd = argv[1];
        myasf += "_ASF_spectra"; // root autosave file
        myraw += "_RAW_tree"; // root raw events
        mycal += "_CAL_tree"; // root calibrated events
        myusr += "_USR_never_used";  // root usr

        myanalysis = new TIFJAnalysis(mylmd,
                                         myraw,
                                         mycal,
                                         myusr);

        // we do not want read in the old polygons etc
        string to_delete = myasf ;
        to_delete += ".root" ;
        unlink(to_delete.c_str());

        myanalysis->SetAutoSaveFile(myasf);
    }
    else
    {
        cout << "avgv[1] = " << argv[1] << endl;
        cout << "avgv[2] = " << argv[2] << endl;
        cout << "avgv[3] = " << argv[3] << endl;

        myanalysis = new TIFJAnalysis(string(argv[1]), string(argv[2]), string(argv[3]), "");
        unlink("Current_ASF_spectra.root");
        myanalysis->SetAutoSaveFile("Current_ASF_spectra");
//      myanalysis->SetAutoSaveFile("Current_ASF");
        // with the autosave file name "6003" was an error !!!!!!!!!!!!
    }

    myanalysis->SetAutoSaveInterval(200000);
    usage();

    // ===================== End EB analysis ================================
    // At this point we should have an analysis myanalysis

    if(runningMode == kBatch)
    {
        cout << "*  starting analysis in batch mode ...  " << endl;
        if(myanalysis->InitEventClasses())
        {
//      mybench.Start("Go4EventServer");

            //myanalysis->RunImplicitLoop(maxevents);

            myanalysis->run_jurek_ImplicitLoop(maxevents);

            delete myanalysis;
            cout << "\t Done (in a natural way) !" << endl;

            //gApplication->Terminate();
            // exit ( 0 );
        }
        else
        {
            cout << "Init event classes failed, aborting!" << endl;
            exit(-1);
        }
    }
    return 0;
}
//***********************************************************************************************
void usage()
{
    cout << endl;
    cout << "*****************************************" << endl;
    cout << "*    SPY                                " << endl;
    cout << "* the analysis program for IFJ PAN      " << endl;
    cout << "* experiments                           " << endl;
    cout << "*                                       " << endl;
    cout << "*                                       " << endl;
    cout << "* usage:                                " << endl;
    cout << "* 1. for ONline analysis                " << endl;
    cout << "*                                       " << endl;
    cout << "*   spy -rem   node_name   port_nr      " << endl;
//    cout << "*   spy -eve   node_name                *" <<endl;
//    cout << "*   spy -tra   node_name                *" <<endl;
//    cout << "*   spy -str   node_name                *" <<endl;
    cout << "*                                       " << endl;
    cout << "* 2. for OFFline analysis    " << endl;
    cout << "*                                       " << endl;
    cout << "*   spy filename [how many events]     " << endl;
    cout << "*   spy @filename_with_list_of_files_to_analyse.txt    " << endl;

    cout << "*****************************************" << endl;
}



