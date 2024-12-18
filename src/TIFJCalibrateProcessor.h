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

#ifndef TEBCALIBRATEPROCESSOR_H
#define TEBCALIBRATEPROCESSOR_H

//#include "TRandom.h"
//#include "TMath.h"
//#include "TH1.h"
//#include "TH2.h"

//#include "TObjArray.h"
//#include "TCutG.h"


//#include "Go4Event/TGo4EventElement.h"

#include "tjurekabstracteventprocessor.h"

#include "TIFJEvent.h"
#include "TIFJCalibration.h"

//#include "Go4ConditionsBase/TGo4WinCond.h"
//#include "Go4ConditionsBase/TGo4PolyCond.h"
//#include "Go4ConditionsBase/TGo4CounterCond.h"
//#include "Go4ConditionsBase/TGo4CondArray.h"

//#include "Go4ConditionsBase/TGo4CondInside.h"

#include "spectrum.h"

class TIFJCalibratedEvent;

/**
 * Event processor to calibrate EBEvents into
 * EbCalibratedEvents
 * @author J. Adamczewski
 * @since 23-feb-2001
 */

////////////////////////////////////////////////////////////////////////////////////////////
class TIFJCalibrateProcessor : public TjurekAbstractEventProcessor // : public TGo4EventProcessor
{
public:
    int event_nr ;
    // #### Rising stuff ###############################################################
public:
    TIFJCalibrateProcessor() ;
    TIFJCalibrateProcessor(string  name);
    void BuildCalibratedEvent(TIFJCalibratedEvent* target);
    virtual ~TIFJCalibrateProcessor() ;
    /** No descriptions */
    void read_in_verbose_parameters();

    bool verbose_mode_enabled ;
    /** Flag telling that now we want to dump information about the current event */
    bool flag_verbose_events;
    int verbose_begins_with_event_nr ;
    int verbose_how_many_events ;
    /** many elements will ask if the verbose mode of displaying events is on */
    bool is_verbose_on()
    {
        return  flag_verbose_events;
    }
    /** No descriptions */
    void preLoop();

    //int read_calibration_parameters();
    //void read_in_polygon_gates();
    //void save_member_spectra();
    void postLoop()
    {
//         cout << "Empty TIFJCalibrateProcessor::postLoop()" << endl;
    }

    void Process()
    {
        cout << "Empty TIFJCalibrateProcessor::Process()" << endl;

    }
protected:
    //-------------------------------------------------------

//ClassDef(TIFJCalibrateProcessor,1)
};

#endif //TEBCALIBRATEPROCESSOR_H

