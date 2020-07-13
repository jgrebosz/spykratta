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

#ifndef TEBCALIBRATION_H
#define TEBCALIBRATION_H
//#include "Go4Event/TGo4EventCalibration.h"
//#include "TIFJLpar.h"
#include "TIFJPar.h"

/**
 * Class containing the calibration parameters for EB event example.
 */
class TIFJCalibration //: public TGo4EventCalibration
{
public:

    void SetParams();

    /** @link aggregationByValue */
    TIFJPar fxParams;

    TIFJCalibration() ;

    virtual ~TIFJCalibration() ;

//ClassDef(TIFJCalibration,1)


};
#endif //TEBCALIBRATION_H
