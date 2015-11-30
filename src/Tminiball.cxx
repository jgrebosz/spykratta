#include "Tminiball.h"

#ifdef MINIBALL_PRESENT

#include <dirent.h> // for scandir
#include <malloc.h> // for scandir

#include <sstream>
#include "TIFJAnalysis.h"
#include "Tnamed_pointer.h"
TIFJCalibratedEvent* Tminiball::tmp_event_calibrated = 0;


//extern int local_mult_tmp ;

//*******************************************************
//*******************************************************
//         MINIBALL class
//*******************************************************
//*******************************************************
Tminiball::Tminiball()
{
    //cout << "\n\n\n\nConstructor of the Miniball Class =========================== " << endl ;
    Tminiball_cryostat::miniball_pointer = this ;
    Tminiball_elementary_det::miniball_pointer = this ;
    TIFJAnalysis* ptr_anal = dynamic_cast<TIFJAnalysis*>(TGo4Analysis::Instance());
    frs_pointer = ptr_anal->give_frs_ptr()  ;  // we will need if to ask traget for theta angles
//  hector_pointer = ptr_anal->give_hector_ptr()  ;  // we will need if to ask traget for theta angles


    //---------------------------------------------
    // constructing the current setup of Miniball
    //---------------------------------------------
    cryostat.push_back(new Tminiball_cryostat("miniball_1", 0));
    cryostat.push_back(new Tminiball_cryostat("miniball_2", 1));
    cryostat.push_back(new Tminiball_cryostat("miniball_3", 2));
    cryostat.push_back(new Tminiball_cryostat("miniball_4", 3));
    cryostat.push_back(new Tminiball_cryostat("miniball_5", 4));
    cryostat.push_back(new Tminiball_cryostat("miniball_6", 5));
    cryostat.push_back(new Tminiball_cryostat("miniball_7", 6));
    cryostat.push_back(new Tminiball_cryostat("miniball_8", 7));

    create_my_spectra() ;

    // conditional spectra are created/recreated during preloop

    named_pointer["Miniball_elementary_det_multiplicity"]
    = Tnamed_pointer(&Tminiball_elementary_det::multiplicity, 0, this) ;
    named_pointer["Miniball_all_miniball_cryostats_multiplicity"]
    = Tnamed_pointer(&Tminiball_cryostat::all_miniball_cryostats_multiplicity, 0, this) ;
//  named_pointer["Miniball_elementary_det_sum_energy_cal"]
//    = Tnamed_pointer(&Tminiball_elementary_det::sum_en4_cal, 0, this) ;
//  named_pointer["Miniball_elementary_det_sum_energy_doppler"]
//    = Tnamed_pointer(&Tminiball_elementary_det::sum_en4_dop, 0, this) ;




    named_pointer["mib_synchro_data"]
    = Tnamed_pointer(&mib_synchro_data, 0, this) ;

    /*-------------------------   This is now implemented to read in preloop time
        int aa;
        cout<< "max_fold ";
        cin >> aa;
        Tminiball_cryostat::max_fold=aa;

        cout<< "adjacent_mode ";
        cin >> aa;
        Tminiball_cryostat::adjacent_mode=aa;
        ---------------*/

}
//*************************************************************************
void Tminiball::create_my_spectra()
{

    string folder = "miniball_cryostats/" ;

    //=============== calibrated spectra (for Gold lines) =====
    // energy cal ------------------------------------
    string name_of_this_element = "Miniball totals";
    string name = "miniball_cryostat_total_core_energy_cal" ;
    spec_total_core_energy_cal = new spectrum_1D(
        name,
        name,
        4096, 0, 4096,
        folder);
    spectra_miniball.push_back(spec_total_core_energy_cal) ;

    // time ------------------------------

    name = "miniball_cryostat_total_time_cal" ;
    spec_total_time_cal = new spectrum_1D(name,
                                          name,
                                          4096, 0, 8192,
                                          folder);
    spectra_miniball.push_back(spec_total_time_cal) ;
    name = "miniball_cryostat_total_theta_mrad"  ;
    spec_total_theta_miliradians = new spectrum_1D(name,
            name,
            1000, 0, 100,
            folder);
    spectra_miniball.push_back(spec_total_theta_miliradians) ;
    name = "miniball_cryostat_total_core_energy_doppler"  ;
    spec_total_core_energy_doppler = new spectrum_1D(name,
            name,
            4096, 0, 4096,
            folder);
    spectra_miniball.push_back(spec_total_core_energy_doppler) ;

    // any other ?




    name = "miniball_cryostat_total_core_energy_cal_addbacked"  ;
    spec_total_core_energy_cal_addbacked = new spectrum_1D(name,
            name,
            4096, 0, 4096,
            folder);
    spectra_miniball.push_back(spec_total_core_energy_cal_addbacked) ;





    name = "miniball_cryostat_total_core_energy_doppler_addbacked"  ;
    spec_total_core_energy_dop_addbacked = new spectrum_1D(name,
            name,
            4096, 0, 4096,
            folder);
    spectra_miniball.push_back(spec_total_core_energy_dop_addbacked) ;




    // FAN spectrum of all elementary_dets----------------------------------------
    name = "miniball_cryostat_fan"  ;
    spec_fan = new spectrum_1D(
        name,
        name,
        1000, 0, 1000,
        folder);
    spectra_miniball.push_back(spec_fan) ;

    name = "miniball_cryostat_multiplicity"  ;
    spec_miniball_cryostats_multiplicity = new spectrum_1D(
        name,
        name,
        10, 0, 10,
        folder , "How many cryostats fired at a time (0-8)");
    spectra_miniball.push_back(spec_miniball_cryostats_multiplicity) ;


    name = "mib_data_for_synchronisation"  ;
    spec_mib_synchro = new spectrum_1D(
        name,
        name,
        512, 1, 8192,
        folder);
    spectra_miniball.push_back(spec_mib_synchro) ;

}
//*************************************************************************
void Tminiball::make_preloop_action(TIFJEvent* event_unpacked ,
                                    TIFJCalibratedEvent* event_calibrated)
{
#ifdef MINIBALL_PRESENT
    tmp_event_unpacked =    event_unpacked ;
    tmp_event_calibrated =    event_calibrated ;
    cout << "Tminiball::make_preloop_action "  << endl;

    Tminiball::tmp_event_calibrated->mib_energy_cal[0][1] = 77;  // = energy_calibrated[0];

    // first we inform where are the data coresponding to these devices
    Tminiball_elementary_det::target_ptr = frs_pointer->give_target_address() ;
    cate_ptr = Tminiball_elementary_det::target_ptr->give_cate_ptr() ;

    //simulate_event();

    // loop over the enumeration type
    for(unsigned int i = 0 ; i < cryostat.size() ; i++)
    {
        // cout << "miniball_cryostat nr  =  " << i << endl;
        // remember the source of data

//        int mib[8][3][7][8];    // 8 cryostats, 3 detectors, 7 singals, each 8 items on dgf block
//        bool mib_fired[8][3];

        cryostat[i]->set_event_data_pointers(event_unpacked->mib[i]);
    }

    //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    // very important:
    // calibrations and gates are read everytime we start the analysis


    read_calibration_and_gates_from_disk();
    read_good_elementary_det_specification();
    read_addback_options_for_cryostats();

// zeroing monitoring statistics for the watchdog
    memset(&fire_statistics[0][0], 0,  sizeof(fire_statistics));
#endif // #if MINIBALL_PRESENT
}
//**********************************************************************
void Tminiball::read_calibration_and_gates_from_disk()
{

    // (by calling miniball_cryostat preloop functions)
    cout << "\n\n\n\nReading Miniball calibration from the disk" << endl ;

    //--------------------------------------------
    // reading the calibration of all frs stuff
    // here we can open the calibration file
    //-------------------------------------------

    string cal_filename = "calibration/miniball_calibration.txt";
    try
    {
        ifstream plik(cal_filename.c_str()) ;    // ascii file
        if(!plik)
        {
            cout << "I can't open miniball calibration file: "
                 << cal_filename << endl ;

            string mess = "I can't open miniball calibration file: "
                          + cal_filename  ;
            throw mess ;
        }

        // real reading the calibration -----------------
        for(unsigned int i = 0 ; i < cryostat.size() ; i++)
        {
            cout << "reading calibration loop for Miniball cryostat " << i + 1 << endl ;
            cryostat[i]->make_preloop_action(plik) ; // reference
        }
    }
    catch(Tfile_helper_exception & k)
    {
        cerr << "Error while reading calibration file "
             << cal_filename
             << ":\n"
             << k.message << endl  ;
        exit(-1) ; // is it healthy ?
    }
    cout << "RISING calibration successfully read"
         << endl;
}
//********************************************************
//********************************************************
//********************************************************
void Tminiball::analyse_current_event()
{
#ifdef MINIBALL_PRESENT
    //elementary_det_which_fired.clear() ; // new event,
    Tminiball_cryostat::all_miniball_cryostats_multiplicity = 0 ;
    Tminiball_elementary_det::multiplicity = 0 ;
    Tminiball_elementary_det::sum_en4_cal = 0  ;
    Tminiball_elementary_det::sum_en4_dop = 0 ;




    //cout << "ANALIZING GE Tminiball::analyse_current_event() GE"<<endl;

    // here, for testing purposes, we want pack artificially to the
    // event
    //simulate_event();


    if((mib_synchro_data = tmp_event_unpacked->mib_synchro_data))   // <--- assigment
    {
        spec_mib_synchro->manually_increment(mib_synchro_data);
        //cout << "in Miniball synchro = " << mib_synchro_data << endl;
    }


//  cout << "============ Next event ================" << endl;

    for(unsigned cryo = 0; cryo < cryostat.size(); cryo++)
    {
        //cout << "Dane for cryo=" << cryo << " elementary_det=" << elem_det << endl ;

        cryostat[cryo]->  set_this_cryostat_multiplicity(0);
        cryostat[cryo]->  reset_variables();

        int mult_before_miniball_cryostat = Tminiball_elementary_det::multiplicity  ;
        int first_elementary_det = -1;
        int localmultiplicity = 0;
//    local_mult_tmp = 0 ;

        double E4max = -1e10; // in case of Miniball we do not have 20MeV channel
        //double E20min= 9999999;

        for(unsigned elem_det = 0; elem_det < 3 ; elem_det++)  // 3 segments (elem det) for cryostat
        {
            if(tmp_event_unpacked->mib_fired[cryo][elem_det] != 0)
            {
//                  cout << "Miniball cryostat " << cryo << ", elem_det= " << elem_det << " FIRED" << endl;
                fire_statistics[cryo][elem_det]++;  // for graphic monitoring
                cryostat[cryo]->GetCrystal(elem_det)->analyse_current_event();
                cryostat[cryo]->GetCrystal(elem_det)->increment_elementary_det_spectra_raw_and_fan();

                // incrementing the total spectrum (always!)
                cryostat[cryo]->GetCrystal(elem_det)->increment_elementary_det_spectra_cal_and_doppler();


                //
                if(cryostat[cryo]->GetCrystal(elem_det)->flag_good_data)
                {


                    if(flag_make_addback)   //  addback was asked, !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    {

                        if(cryostat[cryo]->GetCrystal(elem_det)->give_energy_calibrated(0) >= E4max)
                        {
                            E4max = cryostat[cryo]->GetCrystal(elem_det)->give_energy_calibrated(0);
                            first_elementary_det = (int)elem_det;

                        } // endif  E4max

//                cout << " Fired good:  det miniball_cryostat nr " << cryo << " elem_det " << elem_det
//                  << "E20 = "
//                                  << cryostat[cryo]->GetCrystal(elem_det)->give_energy20_calibrated()
//                                  << ", E4 = "
//                                  << cryostat[cryo]->GetCrystal(elem_det)->give_energy0_calibrated()
//                                  << endl;

                        localmultiplicity++;  // fired, ok, but not the maximum energy
//                local_mult_tmp ++;
                    }
                }  // end if good data


            } // endif elementary_det fired fired
            else
            {
                cryostat[cryo]->GetCrystal(elem_det)->zeroing_good_flags();
            }
        } // end for (elem_det)

// after workinig with the all elementary_det of this miniball_cryostat


        if(flag_make_addback)
        {
            if(localmultiplicity == 1)
            {
//              cout<<"SINGLE SPECTRUM "<< cryo <<" "<< first_elementary_det<<endl;
                cryostat[cryo]->GetCrystal(first_elementary_det)->increment_spect_total_addbacked_cal_and_doppler();
            }
            else if(localmultiplicity > 1 && localmultiplicity <= Tminiball_cryostat::max_fold)
            {
//              cout<<"ADDBACK SPECTRUM cryo= "<< cryo <<" crys= "<< first_elementary_det<<" mult ="<<localmultiplicity
//              << endl;

                if(E4max > Tminiball_cryostat::Eprimary_threshold)
                {
                    cryostat[cryo]->first_miniball_elementary_det_nb = first_elementary_det;
                    cryostat[cryo]->calculate_addback();
                }
            }
            else  {}   // otherwise, such event is discarded


        }
        else {}   // no addback, no work here



// some statistics
        //-------------------
        // at the end of miniball_cryostat, we may check the multiplicity
        if(Tminiball_elementary_det::multiplicity - mult_before_miniball_cryostat)
        {
            Tminiball_cryostat::all_miniball_cryostats_multiplicity++ ;
        }
        cryostat[cryo]->  set_this_cryostat_multiplicity(Tminiball_elementary_det::multiplicity - mult_before_miniball_cryostat) ;

    } // end of for miniball_cryostats

    spec_miniball_cryostats_multiplicity->manually_increment(Tminiball_cryostat::all_miniball_cryostats_multiplicity) ;
    user_spectra_loop();
    monitoring_statistics();
    if(Tminiball_cryostat::all_miniball_cryostats_multiplicity)
        {}//cout<<"TOTOAL MULT "<<Tminiball_cryostat::all_miniball_cryostats_multiplicity<<endl;
    //cout << "endf. Tminiball::analyse_current_event()" << endl;

    // for Puls Shape Analysis (PSA) we copy this to Fabio
    memcpy(tmp_event_calibrated->mib_raw, tmp_event_unpacked->mib, sizeof(tmp_event_unpacked->mib));

#endif // MINIBALL_PRESENT  
}
//***********************************************************************
void  Tminiball::make_postloop_action()
{
    // cout << "F.Tminiball::make_postloop_action() " << endl ;

    // save all spectra made in frs objects and in frs elements
    // (mw, sci, etc.)
    cout <<  "Saving Miniball " << spectra_miniball.size() << " spectra... " << endl;
    for(unsigned int i = 0 ; i < spectra_miniball.size() ; i++)
    {
        spectra_miniball[i]->save_to_disk() ;
    }
    cout <<  "Saving Miniball spectra - done " << endl;
}
//***********************************************************************
void Tminiball::simulate_event()
{
    for(unsigned int i = 0 ; i < cryostat.size() ; i++)
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
}
//**********************************************************************
/** No descriptions */
bool Tminiball::save_selected_spectrum(string name)
{
    for(unsigned int i = 0 ; i < spectra_miniball.size() ; i++)
    {
        if(name == spectra_miniball[i]->give_name())
        {
            spectra_miniball[i]->save_to_disk() ;
            return true ;
        }
    }
    return false ;  // was not here, perhaps in frs or hector?
}
//***********************************************************************
/** Depending on the status of hardware we can use different algorithms */
void Tminiball::read_doppler_corr_options()
{
}
//*******************************************************************
/** Rereading the definiton of the condiotinal (gated) spectra */
void Tminiball::read_def_conditional_spectra()   // called form preloop
{
    // here should be a function for retrieving the names.
    vector<string> names =
        Tfile_helper::find_files_in_directory("./definitions_user_spectra",
                ".user_definition");

    // loop which is looking into the directory for a specified definions *.
    for(unsigned nr = 0 ; nr < names.size() ; nr++)
    {
        cout << "User defined conditional file :" << names[nr] << endl;
        create_cond_spectrum(names[nr]);
    }
}
//*******************************************************************************
/** Create one conditional spectrum */
void Tminiball::create_cond_spectrum(string name)
{
#ifdef NIGDY
#endif
}
//********************************************************************************
/** where the user spectra can be incremented */
void Tminiball::user_spectra_loop()
{
#ifdef NIGDY
#endif
}
//**************************************************************
/******************************************** No descriptions */
int Tminiball::give_synchro_data()
{
    // now it is miniball_cryostat D 2
    return  tmp_event_unpacked->mib_synchro_data;
}
//**************************************************************
void Tminiball::read_good_elementary_det_specification()
{
    // reading from the file
    string pname =  "./options/good_miniball.txt" ;
    ifstream plik(pname.c_str());
    if(!plik)
    {
        cout << "Cant open file " << pname << endl;
        //------- En 4
        Tminiball_elementary_det::flag_good_core_energy_requires_threshold = true ;
        Tminiball_elementary_det::core_energy_threshold = 100 ;
        // everything else is zero (static)


        return ;
    }
    try
    {
        // reading


        Tminiball_elementary_det::flag_increment_segment_energy_cal_with_zero   =
            (bool) Tfile_helper::find_in_file(plik, "increment_segment_energy_cal_with_zero") ;
        Tminiball_elementary_det::flag_increment_time_cal_with_zero =
            (bool) Tfile_helper::find_in_file(plik, "increment_time_cal_with_zero") ;


        //------- En 4
        Tminiball_elementary_det::flag_good_core_energy_requires_threshold =
            (bool) Tfile_helper::find_in_file(plik, "good_core_energy_requires_threshold") ;
        Tminiball_elementary_det::core_energy_threshold =
            Tfile_helper::find_in_file(plik, "core_energy_threshold") ;
        // time -------
        Tminiball_elementary_det::flag_good_time_requires_threshold =
            (bool) Tfile_helper::find_in_file(plik, "good_time_requires_threshold") ;
        Tminiball_elementary_det::good_time_threshold =
            Tfile_helper::find_in_file(plik, "good_time_threshold") ;
    }
    catch(Tfile_helper_exception k)
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
        exit(1);
    }
}
//**********************************************************************
/** No descriptions */
void Tminiball::monitoring_statistics()
{

    // for fire_statistics[cryo][elem_det]++;  // for graphic monitoring
    static int events ;
    static time_t last = time(0) ;
    time_t now = time(0) ;
    events++ ;
    if(now - last > 60)
    {
        struct
        {
            int  moment;
            int real_seconds_of_measurement;
            int predicted_seconds;
            int nr_events;
        } opis ;

        opis.moment  = now ;
        opis.real_seconds_of_measurement = now - last ;
        opis.predicted_seconds = 60;
        opis.nr_events = events ;

        ofstream plik("mib_statistics.mon", ios::binary);
        plik.write((char*) &opis, sizeof(opis));
        plik.write((char*) &fire_statistics[0][0], sizeof(fire_statistics));
        struct tm * timeinfo;
        timeinfo = localtime(&now);
        ostringstream   robot ;
        robot << "Report at " <<  asctime(timeinfo) ;  //  << ends ; <--- error for strings
        events = 0 ;
        string  some_comment = robot.str() ;
        plik.write(some_comment.c_str() , some_comment.size() + 1);
        plik.close();
        memset(fire_statistics, 0 , sizeof(fire_statistics));
        last = now ;
    }
}
//*********************************************************************************************
/** This function reads the options defined in the dialog window of the
        cracow viewer. The result is given to the static members of
        the Tminiball_cryostat */
void Tminiball::read_addback_options_for_cryostats()
{
    // reading from the file
    string pname =  "./options/addback.option";  // the same file as for euroball cluster
    ifstream plik(pname.c_str());
    if(!plik)
    {
        cout << "Cant open file " << pname << endl;
        //------- En 4
        Tminiball_cryostat::max_fold = 2 ;
        Tminiball_cryostat::adjacent_mode = 2 ;
        Tminiball_cryostat::primary_seg_algorithm = 1;  // others do not exist yet
        Tminiball_cryostat::Eprimary_threshold = 0;

        return ;
    }
    try
    {
        // reading

        flag_make_addback =
            (bool) Tfile_helper::find_in_file(plik, "addback_on") ;

        Tminiball_cryostat::max_fold =
            (int) Tfile_helper::find_in_file(plik, "max_fold") ;

        Tminiball_cryostat::adjacent_mode   =
            (bool) Tfile_helper::find_in_file(plik, "adjacent_mode") ;

        Tminiball_cryostat::nonadjacent_mode  =
            (bool) Tfile_helper::find_in_file(plik, "nonadjacent_mode") ;

        Tminiball_cryostat::sum_all_mode     =
            (bool) Tfile_helper::find_in_file(plik, "sum_all_mode") ;


        Tminiball_cryostat::primary_seg_algorithm =
            (int) Tfile_helper::find_in_file(plik, "primary_seg_algorithm") ;

        try
        {

            Tminiball_cryostat::Eprimary_threshold =
                (int) Tfile_helper::find_in_file(plik, "Eprimary_threshold") ;

        }
        catch(...)
        {
            // it is quite new  option, so ignore when it is not present
            Tminiball_cryostat::Eprimary_threshold = 0 ;
        }
    }
    catch(Tfile_helper_exception k)
    {
        cout << "Error while reading " << pname << "\n" << k.message << endl;
        exit(1);
    }
}
//***************************************************************************************
#endif // MINIBALL_PRESENT

