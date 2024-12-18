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

#include "TIFJEvent.h"
#include "TIFJEventProcessor.h"
// #include "Go4EventServer/TGo4TreeSource.h"
// #include "Go4EventServer/TGo4FileSource.h"
#include <cstring>
#include <cstdlib>

//int TIFJEvent::fgiZeroField[41]={0};

//******************************************************************************
TIFJEvent::TIFJEvent()
    : TjurekAbstractEventElement(string("Raw"))
{
// TRACE((12,"TIFJEvent::TIFJEvent()",__LINE__, __FILE__));
    TIFJEvent::Clear();

#ifdef CATE_PRESENT

    for(int i = 0; i < 9 ; i++)
    {
        cate_detector[i].CsI_energy = i * 10 + 1 ;
        cate_detector[i].CsI_time = i * 10 + 2 ;
        cate_detector[i].Si_time = i * 10 + 3 ;

//    KATE[i].CsI_energy = i*10 + 1 ;
//    KATE[i].CsI_time = i*10 + 2 ;
//    KATE[i].Si_time = i*10 + 3 ;

    }
#endif // CATE_PRESENT



}
//******************************************************************************
TIFJEvent::~TIFJEvent()
{
// TRACE((12,"TIFJEvent::~TIFJEvent()",__LINE__, __FILE__));

//fCateArray->Clear();
//delete fCateArray;

}
//******************************************************************************
int TIFJEvent::Fill()
{
//    TRACE((11,"TIFJEvent::Fill()",__LINE__, __FILE__));

    //TGo4TraceSingleton* tr=TGo4TraceSingleton::Instance();
    // check for different source types
    int rev = -1;


#ifdef NOGO4

    TIFJEventProcessor* proc;
    if(CheckEventSource("TIFJEventProcessor"))
    {
        proc = (TIFJEventProcessor*) GetEventSource();
        proc->BuildEbEvent(this);
        rev = 0;
    }
    else
    {
        rev = 1;
    }
#endif

    return rev;
}
//******************************************************************************
void  TIFJEvent::Clear(string /*t*/)
{
//     TRACE ( ( 11,"TIFJEvent::Clear()",__LINE__, __FILE__ ) );

//     cout << "clearing  inp event = event_unpacked = 0x" << hex << int(this) << endl;

    char *ptr = (char *) & begin_for_zeroing ;
    char *kon_ptr = (char *) & end_for_zeroing ;

    while(ptr < kon_ptr)
    {
        *(ptr++) = 0 ;
    }

#ifdef CATE_PRESENT

    for(int i = 0; i < 9; i++)
    {
        cate_detector[i].zeroing() ;
    }
#endif
}


//***********************************************************************************
       // not used anymore, as the tree for hect_kratta is available
void  TIFJEvent::save_as_semitree(string name_)
{
    static bool file_is_open { false};;

    if(!file_is_open )
    {
        // truncate the name
//         size_type result =  name.find(".lmd");
//         if(result != string::npos)
//         {
// 	    name.replace( [result+1]
//         }
//         else {
// 
//         }
        //---------------------
        plik.open(name_+"-bin", ios::binary);
        if(!plik)
        {
            cout << "TIFJEvent::save_as_semitree --> error while openinig the file " << name_ << endl;
            exit(22);
        }
        file_is_open = true;
    }

#ifdef HECTOR_PRESENT
    // data for Hector
//     int hector_tdc[32];
//     int hector_adc[32];
    plik.write( (char*) hector_tdc, sizeof(hector_tdc) );
    plik.write( (char*) hector_adc, sizeof(hector_tdc) );
#endif
#ifdef KRATTA_PRESENT
//     int kratta[KRATTA_NR_OF_CRYSTALS][3+3+3] ;   // 0,1,2 - signals, 3,4,5 - pedestals, 6,7,8 - times

   plik.write( (char*) kratta, sizeof(kratta) );
#endif
}


