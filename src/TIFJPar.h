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
#ifndef SPAR_H
#define SPAR_H


//#include "MbsAPI/typedefs.h"
//#include "TObject.h"
class TIFJPar //: public TObject
{

public:
    /*
       Short_t l_tmin[6];
       Short_t l_tmax[6];
       Short_t l_amin[22];
       Short_t l_amax[22];
       Short_t l_qmin[2];
       Short_t l_qmax[2];

       Float_t r_almin[5];
       Float_t r_almax[5];
       Float_t r_ahmin[5];
       Float_t r_ahmax[5];
       Float_t r_thmin[10];
       Float_t r_thmax[10];
       Float_t r_tref_l[2];
       Float_t r_tref_h[2];
       Float_t r_ao[22];
       Float_t r_ag[22];
       Float_t r_aq[22];
       Float_t r_qo[2];
       Float_t r_qg[2];
       Float_t r_qq[2];
       Float_t r_qco[2];
       Float_t r_qcg[2];
       Float_t r_qcq[2];
       Float_t r_qf1_l1[2];
       Float_t r_qf2_l1[2];
       Float_t r_qf3_l1[2];
       Float_t r_qf4_l1[2];
       Float_t r_qf5_l1[2];
       Float_t r_qf1_l2[2];
       Float_t r_qf2_l2[2];
       Float_t r_qf3_l2[2];
       Float_t r_qf4_l2[2];
       Float_t r_qf5_l2[2];
       Float_t r_qf1_l3[2];
       Float_t r_qf2_l3[2];
       Float_t r_qf3_l3[2];
       Float_t r_qf4_l3[2];
       Float_t r_qf5_l3[2];
       Float_t r_qf1_h1[2];
       Float_t r_qf2_h1[2];
       Float_t r_qf3_h1[2];
       Float_t r_qf4_h1[2];
       Float_t r_qf5_h1[2];
       Float_t r_qf1_h2[2];
       Float_t r_qf2_h2[2];
       Float_t r_qf3_h2[2];
       Float_t r_qf4_h2[2];
       Float_t r_qf5_h2[2];
       Float_t r_qf1_h3[2];
       Float_t r_qf2_h3[2];
       Float_t r_qf3_h3[2];
       Float_t r_qf4_h3[2];
       Float_t r_qf5_h3[2];
       Float_t r_to[6];
       Float_t r_tg[6];
       Float_t r_tq[6];
       Float_t r_r1[152];
       Float_t r_r2[152];
       Float_t r_r3[152];
       Float_t r_r4[152];

       TIFJPar(){
        int ip;

         for(ip=0;ip<6;ip++)    l_tmin[ip]  =  0;
         for(ip=0;ip<6;ip++)    l_tmax[ip]  =  0;
         for(ip=0;ip<22;ip++)   l_amin[ip]  =  0;
         for(ip=0;ip<22;ip++)   l_amax[ip]  =  0;
         for(ip=0;ip<2;ip++)    l_qmin[ip]  =  0;
         for(ip=0;ip<2;ip++)    l_qmax[ip]  =  0;

         for(ip=0;ip<5;ip++)    r_almin[ip] =0.0;
         for(ip=0;ip<5;ip++)    r_almax[ip] =0.0;
         for(ip=0;ip<5;ip++)    r_ahmin[ip] =0.0;
         for(ip=0;ip<5;ip++)    r_ahmax[ip] =0.0;
         for(ip=0;ip<10;ip++)   r_thmin[ip] =0.0;
         for(ip=0;ip<10;ip++)   r_thmax[ip] =0.0;
         for(ip=0;ip<2;ip++)    r_tref_l[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_tref_h[ip]    =0.0;
         for(ip=0;ip<22;ip++)   r_ao[ip]    =0.0;
         for(ip=0;ip<22;ip++)   r_ag[ip]    =0.0;
         for(ip=0;ip<22;ip++)   r_aq[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qo[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qg[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qq[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qco[ip]   =0.0;
         for(ip=0;ip<2;ip++)    r_qcg[ip]   =0.0;
         for(ip=0;ip<2;ip++)    r_qcq[ip]   =0.0;
         for(ip=0;ip<2;ip++)    r_qf1_l1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf2_l1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf3_l1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf4_l1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf5_l1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf1_l2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf2_l2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf3_l2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf4_l2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf5_l2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf1_l3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf2_l3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf3_l3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf4_l3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf5_l3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf1_h1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf2_h1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf3_h1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf4_h1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf5_h1[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf1_h2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf2_h2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf3_h2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf4_h2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf5_h2[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf1_h3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf2_h3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf3_h3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf4_h3[ip]    =0.0;
         for(ip=0;ip<2;ip++)    r_qf5_h3[ip]    =0.0;
         for(ip=0;ip<6;ip++)    r_to[ip]        =0.0;
         for(ip=0;ip<6;ip++)    r_tg[ip]        =0.0;
         for(ip=0;ip<6;ip++)    r_tq[ip]        =0.0;
         for(ip=0;ip<152;ip++)  r_r1[ip]        =0.0;
         for(ip=0;ip<152;ip++)  r_r2[ip]        =0.0;
         for(ip=0;ip<152;ip++)  r_r3[ip]        =0.0;
         for(ip=0;ip<152;ip++)  r_r4[ip]        =0.0;
       }
       */
    virtual ~TIFJPar() {}



//ClassDef(TIFJPar,1)
};

//ClassImp(TIFJPar)


#endif //SPAR_H


