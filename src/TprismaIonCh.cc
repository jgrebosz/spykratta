#include "TprismaIonCh.h"
#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#include <sstream>
#include "Tnamed_pointer.h"
//****************************************************************
TprismaIonCh::TprismaIonCh(string name, int ndet, int npar) : TprismaDetector(name)
{
    n_det = ndet;
    n_par = npar;

    ionch_dE_raw_upper_limit = 9999;

    //   theFile = new ConfigurationFile( conf_file.c_str() );
    //   if( this->read_configuration() > 0 )
    //     this->init_data();

    create_my_spectra();

    //    named_pointer[name_of_this_element+"__x_fp_in_meters_when_valid"]
    //   = Tnamed_pointer(&ppac_xfp, &xfp_ok, this) ;
    //
    //   vector<double>   dE_A;   // just in first
    //   vector<double>   dE_AB;   // first and second
    //   vector<double>   dE_ABCD;   // all 4 elements
    //
    dE_A.resize(n_det);
    dE_AB.resize(n_det);
    dE_ABCD.resize(n_det);

    for(int i = 0 ; i < n_det ; i++)
    {
        ostringstream s;
        s << i ;
        string nr = s.str();
        named_pointer[name_of_this_element + "__dE_" + nr + "_cal_when_valid"]
        = Tnamed_pointer(&dE_A[i], &flag_valid, this) ;

        named_pointer[name_of_this_element + "__dEab_" + nr + "_cal_when_valid"]
        = Tnamed_pointer(&dE_AB[i], &flag_valid, this) ;

        named_pointer[name_of_this_element + "__dEabcd" + nr + "_cal_when_valid"]
        = Tnamed_pointer(&dE_ABCD[i], &flag_valid, this) ;
    }


    named_pointer[name_of_this_element + "__total_E_cal_when_valid"]
    = Tnamed_pointer(&ic_tot_E, &flag_valid, this) ;

    named_pointer[name_of_this_element + "__total_dEa_cal_when_valid"]
    = Tnamed_pointer(& ic_de[0] , &flag_valid, this) ;

    named_pointer[name_of_this_element + "__total_dEab_cal_when_valid"]
    = Tnamed_pointer(&id_de_ab , &flag_valid, this) ;

}
//****************************************************************
TprismaIonCh::~TprismaIonCh()
{
    for(int ii = 0; ii < theCalManager.size(); ii++)
        delete theCalManager[ii];
    theCalManager.clear();

    //   delete theFile;

    // was leak
    for(int ii = 0; ii < (int)uncalibratedIC.size(); ii++)
    {
        if(uncalibratedIC[ii])
            delete uncalibratedIC[ii];
    }
}
//****************************************************************
void TprismaIonCh::create_my_spectra()
{
    spectrum_1D * spec_tmp = NULL;
    ostringstream s;
    string name;
    string folder = "IonisationChamber/";
    string nri;

    spec_ic_de_raw.resize(n_det);
    spec_ic_de_cal.resize(n_det);

    for(int i = 0 ; i < n_det ; i++)
    {
        s.str("");
        s << i ;
        nri = s.str();
        for(int k = 0 ; k < 4 ; k++)
        {
            name =  name_of_this_element + "__dE_" + nri + "_" +  char(k + 'A') + "_raw";
            spec_tmp  = new spectrum_1D(name, name,
                                        4095, 1, 4096,
                                        folder,
                                        "",
                                        "no incrementor, use the calibrated version");  // incrementor name
            frs_spectra_ptr->push_back(spec_tmp) ;

            spec_ic_de_raw[i].push_back(spec_tmp);

            //.........
            name =  name_of_this_element + "__dE_" + nri + "_" +  char(k + 'A') + "_cal";
            spec_tmp  = new spectrum_1D(name, name,
                                        4095, 1, 4096,
                                        folder,
                                        "",
                                        name + "_when_valid"); // incrementor name
            frs_spectra_ptr->push_back(spec_tmp) ;
            spec_ic_de_cal[i].push_back(spec_tmp);

            //.........
        }// for k

        //--------------- 2D -------------------
        name =  name_of_this_element + "__dEa_vs_E_" + nri;

        spectrum_2D*  spec_tmp2  =
            new spectrum_2D(name, name,
                            950, 0, 8000,
                            950, 0, 8000,
                            folder,
                            "",
                            "X:  " + name_of_this_element + "__dEabcd_" + nri + "_cal_when_valid"
                            "   Y:  " + name_of_this_element + "__dEa_" + nri + "_cal_when_valid"
                           );  // incrementor name
        frs_spectra_ptr->push_back(spec_tmp2) ;
        spec__dEa_vs_E.push_back(spec_tmp2);

        name = name_of_this_element + "__dEab_vs_E_" + nri  ;
        spec_tmp2  =
            new spectrum_2D(name, name,
                            950, 0, 8000,
                            950, 0, 8000,
                            folder,
                            "",
                            "X:  " + name_of_this_element + "__dEabcd_" + nri + "_cal_when_valid"
                            "   Y:  " + name_of_this_element + "__dEab_" + nri + "_cal_when_valid"); // incrementor name
        frs_spectra_ptr->push_back(spec_tmp2) ;
        spec__dEab_vs_E.push_back(spec_tmp2);



        //polygon_name.push_back(name);



    } // for i
    //------------------
    name = name_of_this_element + "__total_dEa_vs_E" ;
    spec__total_dEa_vs_E  =
        new spectrum_2D(name, name,
                        950, 0, 8000,
                        950, 0, 8000,
                        folder,
                        "",
                        "X:  " + name_of_this_element + "__total_E_cal_when_valid"
                        "   Y:  " + name_of_this_element + "__total_dEa_cal_when_valid"); // incrementor

    frs_spectra_ptr->push_back(spec__total_dEa_vs_E) ;
    //------------------
    name = name_of_this_element + "__total_dEab_vs_E" ;
    spec__total_dEab_vs_E  =
        new spectrum_2D(name, name,
                        950, 0, 8000,
                        950, 0, 8000,
                        folder,
                        "",
                        "X:  " + name_of_this_element + "__total_E_cal_when_valid"
                        "   Y:  " + name_of_this_element + "__total_dEab_cal_when_valid"); // incrementor
    frs_spectra_ptr->push_back(spec__total_dEab_vs_E) ;
    
    
//     //------------------
//     name = name_of_this_element + "__total_dEab_vs_E_tot" ;
//     spec__total_dEa_vs_E_tot  =
//         new spectrum_2D(name, name,
//                         950, 0, 8000,
//                         950, 0, 8000,
//                         folder,
//                         "",
//                         "X:  " + name_of_this_element + "__total_E_cal_when_valid"
//                         "   Y:  " + name_of_this_element + "__total_dEab_cal_when_valid"); // incrementor
//     frs_spectra_ptr->push_back(spec__total_dEa_vs_E_tot) ;




    name = name_of_this_element + "__total_E" ;
    spec__total_E  =
        new spectrum_1D(name, name,
                        999, 0, 8000,
                        folder,
                        "",
                        "X:  " + name_of_this_element + "__total_E_cal_when_valid"
                       ); // incrementor
    frs_spectra_ptr->push_back(spec__total_E) ;

    name = name_of_this_element + "__rejected_ionch_event" ;
    spec__rejected_event_ionch  =
        new spectrum_1D(name, name,
                        2100, 0, 2100,
                        folder,
                        "",
                        "X:  " + name_of_this_element + "__rejected_ionch_event"
                       ); // incrementor
    frs_spectra_ptr->push_back(spec__rejected_event_ionch) ;

    
}
//****************************************************************
void TprismaIonCh::make_preloop_action(std::ifstream &s)
{
    valid_conf = 0;
    //////////////////////////////////
    // definition of the parameters
    //////////////////////////////////
    // preliminary check
    if(n_par < 4)
    {
        cout << " Error! Only " << n_par << " parameters defined, but Side has normally more ..." << endl;
        exit(79);
    }

    ind_s[0] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_s1");
    ind_s[1] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_s2");
    ind_s[2] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_s3");
    ind_s[3] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_s4");

    // should check for inconsistencies
    for(int ii = 0; ii < 4; ii++)
    {
        if(ind_s[ii] > (n_par - 1))
        {
            ostringstream t;
            t << " Error with definition of parameter " << ind_s[ii] << "(" << ii << ")" ;
            Tfile_helper_exception m;
            m.message = t.str();
            throw m;
        }
        for(int jj = 0; jj < ii; jj++)
        {
            if(ind_s[ii] == ind_s[jj])
            {
                ostringstream t;
                t << " Error! Duplicated parameters "  << ind_s[ii] << "(" << ii << "), " <<
                  ind_s[jj] << "(" << jj << ")" ;
                Tfile_helper_exception m;
                m.message = t.str();
                throw m;

            }
        }
    }
    // everything OK, can store in "real" params

    /////////////////////////////////////
    //// No banana, only calibration files
    ////////////////////////////////////


    ic_s1_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_s1_gain");
    ic_s1_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_s1_offs");

    ic_s2_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_s2_gain");
    ic_s2_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_s2_offs");

    ic_s3_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_s3_gain");
    ic_s3_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_s3_offs");

    ic_s4_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_s4_gain");
    ic_s4_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_s4_offs");

    //////////////////////////////////
    //// Threshold for data validity
    //////////////////////////////////
    lower_th = Tfile_helper::find_in_file(s,  name_of_this_element  + "_lower_th"); // equivalent to conditions found in GSORT

    threshold_before_cal = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_threshold_before_cal");  // by default thresholds applied AFTER calibration
    max_veto_id          = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_max_veto_id");          // veto on the sections up to this number (negative->no veto!)
    veto_threshold       = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_veto_threshold");       // threshold applied for this veto

    ionch_dE_raw_upper_limit = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_dE_raw_upper_limit");       // threshold applied for this veto


    valid_conf = 1;


    ///////////////////////////////////////
    /// Calibration files
    //////////////////////////////////////
    theCalManager.resize(n_par);     // "safe" allocation: not all the parameters will be actually calibrated

    //ic_s1_file  = string( theFile->get_string( "ic_s1_file", "ic_s1.cal") );

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_calibration_for_sectors_A");
    theCalManager[ind_s[0]] = new calManager(s, (double) n_det);

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_calibration_for_sectors_B");
    theCalManager[ind_s[1]] = new calManager(s, (double) n_det);

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_calibration_for_sectors_C");
    theCalManager[ind_s[2]] = new calManager(s, (double) n_det);

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_calibration_for_sectors_D");
    theCalManager[ind_s[3]] = new calManager(s, (double) n_det);

    theCalManager[ind_s[0]]->setGain(ic_s1_gain);
    theCalManager[ind_s[0]]->setOffset(ic_s1_offs);

    theCalManager[ind_s[1]]->setGain(ic_s2_gain);
    theCalManager[ind_s[1]]->setOffset(ic_s2_offs);

    theCalManager[ind_s[2]]->setGain(ic_s3_gain);
    theCalManager[ind_s[2]]->setOffset(ic_s3_offs);

    theCalManager[ind_s[3]]->setGain(ic_s4_gain);
    theCalManager[ind_s[3]]->setOffset(ic_s4_offs);


}
//****************************************************************
int TprismaIonCh::read_configuration()
{
    valid_conf = 0;
    //////////////////////////////////
    // definition of the parameters
    //////////////////////////////////
    // preliminary check
    if(n_par < 4)
    {
        cout << " Error! Only " << n_par << " parameters defined, but Side has normally more ..." << endl;
        return valid_conf;
    }

    ind_s[0] = theFile->get_int("ind_s1",  0);
    ind_s[1] = theFile->get_int("ind_s2",  1);
    ind_s[2] = theFile->get_int("ind_s3",  2);
    ind_s[3] = theFile->get_int("ind_s4",  3);

    // should check for inconsistencies
    for(int ii = 0; ii < 4; ii++)
    {
        if(ind_s[ii] > (n_par - 1))
        {
            cout << " Error with definition of parameter " << ind_s[ii] << "(" << ii << ")" << endl;
            return valid_conf;
        }
        for(int jj = 0; jj < ii; jj++)
        {
            if(ind_s[ii] == ind_s[jj])
            {
                cout << " Error! Duplicated parameters "  << ind_s[ii] << "(" << ii << "), " <<
                     ind_s[jj] << "(" << jj << ")" << endl;
            }
        }
    }
    // everything OK, can store in "real" params

    /////////////////////////////////////
    //// No banana, only calibration files
    ////////////////////////////////////
    ic_s1_file  = string(theFile->get_string("ic_s1_file", "ic_s1.cal"));
    ic_s2_file  = string(theFile->get_string("ic_s2_file", "ic_s2.cal"));
    ic_s3_file  = string(theFile->get_string("ic_s3_file", "ic_s3.cal"));
    ic_s4_file  = string(theFile->get_string("ic_s4_file", "ic_s4.cal"));

    ic_s1_gain = theFile->get_double("ic_s1_gain", 1.);
    ic_s1_offs = theFile->get_double("ic_s1_offs", 0.);

    ic_s2_gain = theFile->get_double("ic_s2_gain", 1.);
    ic_s2_offs = theFile->get_double("ic_s2_offs", 0.);

    ic_s3_gain = theFile->get_double("ic_s3_gain", 1.);
    ic_s3_offs = theFile->get_double("ic_s3_offs", 0.);

    ic_s4_gain = theFile->get_double("ic_s4_gain", 1.);
    ic_s4_offs = theFile->get_double("ic_s4_offs", 0.);

    //////////////////////////////////
    //// Threshold for data validity
    //////////////////////////////////
    lower_th = theFile->get_double("lower_th", 4.);  // equivalent to conditions found in GSORT

    threshold_before_cal = theFile->get_int("threshold_before_cal",  0);  // by default thresholds applied AFTER calibration
    max_veto_id          = theFile->get_int("max_veto_id",  -1);          // veto on the sections up to this number (negative->no veto!)
    veto_threshold       = theFile->get_int("veto_threshold", 120);       // threshold applied for this veto

    valid_conf = 1;
    return valid_conf;
}
//****************************************************************
void TprismaIonCh::init_data()
{
    ///////////////////////////////////////
    /// No banana(s)
    //////////////////////////////////////

    ///////////////////////////////////////
    /// Calibration files
    //////////////////////////////////////
    theCalManager.resize(n_par);     // "safe" allocation: not all the parameters will be actually calibrated

    theCalManager[ind_s[0]] = new calManager(ic_s1_file);
    theCalManager[ind_s[1]] = new calManager(ic_s2_file);
    theCalManager[ind_s[2]] = new calManager(ic_s3_file);
    theCalManager[ind_s[3]] = new calManager(ic_s4_file);

    theCalManager[ind_s[0]]->setGain(ic_s1_gain);
    theCalManager[ind_s[0]]->setOffset(ic_s1_offs);

    theCalManager[ind_s[1]]->setGain(ic_s2_gain);
    theCalManager[ind_s[1]]->setOffset(ic_s2_offs);

    theCalManager[ind_s[2]]->setGain(ic_s3_gain);
    theCalManager[ind_s[2]]->setOffset(ic_s3_offs);

    theCalManager[ind_s[3]]->setGain(ic_s4_gain);
    theCalManager[ind_s[3]]->setOffset(ic_s4_offs);

}
//****************************************************************
int TprismaIonCh::validate_event(std::vector<detData*>& theEvent)
{
    //////////////////////////////////////////////////////
    /// This validity check is merely a preliminary one.
    /// The real consistency check comes out of the
    /// trajectory reconstruction!
    //////////////////////////////////////////////////////
    flag_valid = false;
    detData* aData = 0;

    processed_event = 0;
    valid_evt = 0;
    ic_tot_E = 0.;
    int n_valid = 0;

    err_code = 0;

    for(int i = 0 ; i < dE_A.size() ; i++)
    {
        dE_A[i] = 0;
        dE_AB[i] = 0;
        dE_ABCD[i] = 0;
    }

    // empty buffer -> not valid!
    if(theEvent.size() < 1)
    {
        err_code = 512;
        spec__rejected_event_ionch->manually_increment(err_code);
//         cout << " event empty" << endl;
        return -1;
    }

    // Erase ic_de buffers
    for(int ii = 0; ii < 4; ii++)
        ic_de[ii] = 0.;

    // will store uncalibrated and calibrated values
    // here: erase previous values, if any ...
    for(int ii = 0; ii < (int)uncalibratedIC.size(); ii++)
    {
        if(uncalibratedIC[ii])
            delete uncalibratedIC[ii];
    }
    uncalibratedIC.resize(theEvent.size());


// cout << "new event" << endl;
    for(int ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
        int cur_det = aData->num_det();
        uncalibratedIC[ii] = new detData(cur_det, 4);        // <--- leak?

        for(int jj = 0; jj < 4; jj++)   // 4 data for every section of IC
        {
            uncalibratedIC[ii]->set(ind_s[jj], (*aData)[ind_s[jj]]);

//             if(cur_det == 2)
//                 cout << "ii = " << ii << " jj= " << jj << "  aData->num_det() = " <<  aData->num_det()
//                  << ", putting there data " << (*aData)[ind_s[jj]]
//                  << endl;

            spec_ic_de_raw[ cur_det ] [jj]->manually_increment((*aData)[ind_s[jj]]);

            if(threshold_before_cal > 0)
            {
                if((*aData)[ind_s[jj]] >= lower_th)
                    n_valid++;
                else
                    aData->set(ind_s[jj], 0.);
            }
            else
            {
                if((*aData)[ind_s[jj]] > 0.)
                    n_valid++;
            }
        }
    }

    if(n_valid < 1)
    {
        err_code = 1024;
        spec__rejected_event_ionch->manually_increment(err_code);
        return -1;
    }

    double cal_sect = 0.;
    double eff_thr = 0.;
    int run = 0;

    //   cout << "In validate   -- theEvent.size()= " << theEvent.size() << endl;
    for(int ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
        int num_det = aData->num_det();

        /////////////////////////////////////////////////////
        /// use first columns as veto!!!
        ///////////////////////////////////////
//      if(num_det > max_veto_id) continue;
//
//        for(int jj = 0; jj < 4; jj++)
//        {
//            if(threshold_before_cal > 0)
//            {
//                if((*aData)[ind_s[jj]] > veto_threshold)
//                {
//                    err_code = 2048;
//                    spec__rejected_event_ionch->manually_increment(err_code);
//                    return -1;
//                }
//            }
//            else
//            {
//                cal_sect = theCalManager[ind_s[jj]]->getCalValue(run, num_det, (*aData)[ind_s[jj]]);
//                if(cal_sect > veto_threshold)
//                {
//                    err_code = 2048;
//                    spec__rejected_event_ionch->manually_increment(err_code);
//                    return -1;
//                }
//            }
//        }
	/////////////////////////////////////////////////////
        /// use first row as veto!!!
        ///////////////////////////////////////
        if(max_veto_id<0) continue;
	//std::cout << "Event = " << (*aData)[ind_s[0]]  << std::endl;
        for(int jj = 0; jj < max_veto_id; jj++)
        {
            if(threshold_before_cal < 0)
            {
                if((*aData)[ind_s[jj]] < veto_threshold)
                {
                    err_code = 256;
		//    std::cout << "Event rejected because of threshold = " << veto_threshold << 
		//    "and event = " << (*aData)[ind_s[jj]]  << std::endl;
                    spec__rejected_event_ionch->manually_increment(err_code);
                    return -1;
                }
            }
            else
            {
                cal_sect = theCalManager[ind_s[jj]]->getCalValue(run, num_det, (*aData)[ind_s[jj]]);
                if(cal_sect < veto_threshold)
                {
                    err_code = 256;
		//    std::cout << "Event rejected because of threshold acal = " << veto_threshold << 
		//    "and event = " << cal_sect  << std::endl;
                    spec__rejected_event_ionch->manually_increment(err_code);
                    return -1;
                }
            }
        }
    }
    
#if 0    
// Calibrate data for E_dE matrix without condition on path reconstruction inside PRISMA
    for(int ii = 0; ii < (int)theEvent.size(); ii++)
    {
        double tot_energy = 0;
        aData = theEvent[ii];
        int num_det = aData->num_det();
	double energy_sec_a = get_A_DE();
        for(int jj = 0; jj < 4; jj++)
        {
	   tot_energy = get_calibrated_ic(num_det,jj);
	}
       spec__total_dEa_vs_E_tot->manually_increment(tot_energy,get_A_DE());
    }
#endif   // 0

    valid_evt = 1;
    spec__rejected_event_ionch->manually_increment(5);
    //   cout << "n_valid = " << n_valid << endl;
    return n_valid;
}
//****************************************************************
void TprismaIonCh::process_event(std::vector<detData*>& theEvent)
{
    int run = 0;
    detData* aData = 0;

    double cal_sect = 0.;

    // will store uncalibrated  and calibrated values
    // here: erase previous values, if any ...
    for(int ii = 0; ii < (int)calibratedIC.size(); ii++)
    {
        if(calibratedIC[ii])
            delete calibratedIC[ii];
    }
    //   calibratedIC.resize(theEvent.size());
    calibratedIC.clear();

    // apply the alignment coefficients
    // use the cycle to evaluate also E, DE_A, DE_AB
    // and store calibrated values
    // sections below threshold were already zeroed in the validation cycle ...

    //   int sss = theEvent.size();
    //   cout << "TprismaIonCh::process_event            theEvent.size()" << sss << endl;

    for(int ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];

        //     calibratedIC[ii] = new detData( aData->num_det(), 4 );
        calibratedIC.push_back(new detData(aData->num_det(), 4));
        if(calibratedIC.size() != (ii + 1))
        {
            cout << "strange !!!" << endl;
        }

        for(int jj = 0; jj < 4; jj++)
        {
            if((*aData)[ind_s[jj]] > 0.)
            {

                cal_sect = theCalManager[ind_s[jj]]->getCalValue(run, aData->num_det(), (*aData)[ind_s[jj]]);
                if(threshold_before_cal < 1)       // upper thereshold
                {
                    if(cal_sect < lower_th)
                        cal_sect = 0.;
                }

                if((*aData)[ind_s[jj]] >  ionch_dE_raw_upper_limit)  // raw data over upper thereshold     J.G.
                {
                    cal_sect = 0 ;
                }

                aData->set(ind_s[jj], cal_sect);

                calibratedIC[ii]->set(ind_s[jj], cal_sect);

                // spectra
//                          cout << "ii = " << ii << ", jj = " << jj
//                          << ", aData->num_det() = " << aData->num_det() << " cal value = " << cal_sect << endl;

                spec_ic_de_cal[ aData->num_det() ] [jj]->manually_increment(cal_sect);

                // only for this detector
                if(jj == 0)
                    dE_A[aData->num_det()] = cal_sect;   // just in first
                if(jj == 0 || jj == 1)
                {
                    dE_AB[aData->num_det()] += cal_sect;   // just in first
                }
                dE_ABCD[aData->num_det()] += cal_sect;  // all 4 elements


                // genarall for whole device
                ic_de[ind_s[jj]] += cal_sect;
                ic_tot_E         += cal_sect;
            } // if data
        } // for jj
    } // for ii

    //
    for(int i = 0 ; i < n_det ; i++)
    {
        spec__dEa_vs_E[i]->manually_increment(dE_ABCD[i],  dE_A[i]);
        spec__dEab_vs_E[i]->manually_increment(dE_ABCD[i],  dE_AB[i]);
    }

    id_de_ab = get_AB_DE();
    spec__total_dEa_vs_E->manually_increment(ic_tot_E,  get_A_DE());
    spec__total_dEab_vs_E->manually_increment(ic_tot_E, id_de_ab);

    spec__total_E->manually_increment(ic_tot_E);

    flag_valid = true;
    processed_event = 1;
}
//****************************************************************
double TprismaIonCh::get_calibrated_ic(int det, int sec)
{
    double dummy = -100.;

    if((sec < 0) || (sec > 3))
        return dummy;

    detData* aData = 0;

    for(int ii = 0; ii < (int)calibratedIC.size(); ii++)
    {
        aData = calibratedIC[ii];

        if(aData->num_det() == det)
            return (*aData)[ind_s[sec]];
    }
    return dummy;
}
//****************************************************************
double TprismaIonCh::get_uncalibrated_ic(int det, int sec)
{
    double dummy = -100.;

    if((sec < 0) || (sec > 3))
        return dummy;

    detData* aData = 0;

    for(int ii = 0; ii < (int)uncalibratedIC.size(); ii++)
    {
        aData = uncalibratedIC[ii];

        if(aData->num_det() == det)
            return (*aData)[ind_s[sec]];
    }
    return dummy;
}
//****************************************************************
double TprismaIonCh::get_AB_DE()
{
    double ab = 0.;
    detData* aData = 0;

    for(int ii = 0; ii < (int)calibratedIC.size(); ii++)
    {
        aData = calibratedIC[ii];
        for(int jj = 0; jj < 2; jj++)
        {
            if((*aData)[ind_s[jj]] > 0.)
                ab += (*aData)[ind_s[jj]];
        }
    }
    return ab;
}
//****************************************************************
double TprismaIonCh::get_A_DE()
{
    double a = 0.;
    detData* aData = 0;

    for(int ii = 0; ii < (int)calibratedIC.size(); ii++)
    {
        aData = calibratedIC[ii];
        if((*aData)[ind_s[0]] > 0.)
            a += (*aData)[ind_s[0]];
    }
    return a;
}
//****************************************************************
#endif // CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
