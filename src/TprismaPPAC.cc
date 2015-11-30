#include "TprismaPPAC.h"
#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#include <sstream>
#include <iomanip>
#include <cstdlib>


#include "Tnamed_pointer.h"
//******************************************************************************

TprismaPPAC::TprismaPPAC(string name, int ndet, int npar) : TprismaDetector(name)
{
    n_det = ndet;
    n_par = npar;

//     theFile = new ConfigurationFile( conf_file.c_str() );
//     if(read_configuration() > 0 )
//       init_data();

    tof_raw.resize(n_det);
    tof_cal.resize(n_det);
    tof_cal_realigned.resize(n_det);
    cathode_raw.resize(n_det);
    sumXleftRight_raw.resize(n_det);

    sumYupDown_raw.resize(n_det);             // NOTE:Y is measured only by secton 0

	     my_selfgate_type =  Tselfgate_type::agata_psa; // fake, because..
    create_my_spectra();

    // incrementers ----------------------------------------
    named_pointer[name_of_this_element + "__x_fp_in_meters_when_valid"]
    = Tnamed_pointer(&ppac_xfp, &xfp_ok, this) ;

    named_pointer[name_of_this_element + "__y_fp_in_meters_when_valid"]
    = Tnamed_pointer(&ppac_yfp, &yfp_ok, this) ;

    named_pointer[name_of_this_element + "__tof_when_valid"]
    = Tnamed_pointer(&ppac_tof, &tof_ok, this) ;


    for(int i = 0 ; i < n_det ; i++)
    {
        ostringstream s;
        s << i ;
        string numerek = s.str();
        named_pointer[name_of_this_element + "__tof" + numerek + "_raw"]              = Tnamed_pointer(&tof_raw[i], NULL, this) ;
        named_pointer[name_of_this_element + "__tof" + numerek + "_cal"]              = Tnamed_pointer(&tof_cal[i], &valid_evt, this) ;
        named_pointer[name_of_this_element + "__tof" + numerek + "_cal_realigned"]    = Tnamed_pointer(&tof_cal_realigned[i], &valid_evt, this) ;
        named_pointer[name_of_this_element + "__cathode" + numerek + "_raw"]          = Tnamed_pointer(&cathode_raw[i], 0, this) ;
        named_pointer[name_of_this_element + "__x_sum_left_right" + numerek + "_raw"] = Tnamed_pointer(&sumXleftRight_raw[i], 0, this) ;
        named_pointer[name_of_this_element + "__y_sum_up_down_" + numerek + "_raw"]   = Tnamed_pointer(&sumYupDown_raw[i], 0, this) ;
    }
}
//******************************************************************************
TprismaPPAC::~TprismaPPAC()
{
    //   delete theBanManager;
    for(int i = 0; i < theCalManager.size(); i++)
    {
        delete theCalManager[i];
    }
    theCalManager.clear();
    valid.clear();
    //   delete theFile;
}
//******************************************************************************
void TprismaPPAC::create_my_spectra()
{
    string folder = "PRISMA_PPAC/";
    for(int i = 0 ; i < n_det ; i++)
    {
        ostringstream s;
        s << i ;
        string numerek = s.str();
        string name = name_of_this_element + "__tof" + numerek + "_raw";

        spectrum_1D * spec_tmp  = new spectrum_1D(name, name,
                4095, 1, 4096,
                folder,
                "Raw value ",
                name);  // incrementor name
        frs_spectra_ptr->push_back(spec_tmp) ;

        spec_tof_raw.push_back(spec_tmp);

        //------------------
        name = name_of_this_element + "__tof" + numerek + "_cal";
        spec_tmp  = new spectrum_1D(name, name,
                                    8192, -1000, 4096,
                                    folder,
                                    "",
                                    string("This spectrum is made just by calibrating the raw value. There is no such incrementer.\r ") +
                                    " However there is am incremetner called >" +
                                    name + "_when_valid<  which contains the similar data when the signal is considered as  'valid'");  // incrementor name
        frs_spectra_ptr->push_back(spec_tmp) ;
        spec_tof_cal.push_back(spec_tmp);

        //------------------
        name = name_of_this_element + "__tof" + numerek + "_cal_realigned" ;        // ;
        spec_tmp  = new spectrum_1D(name, name,
                                    4000, 0, 1000,
                                    folder,
                                    "",
                                    name + "_when_valid"); // incrementor name
        frs_spectra_ptr->push_back(spec_tmp) ;
        spec_tof_cal_realigned.push_back(spec_tmp);
        //-------------------

        name = name_of_this_element + "__cathode" + numerek + "_raw" ;
        spec_tmp  = new spectrum_1D(name, name,
                                    4095, 1, 4096,
                                    folder,
                                    "",
                                    name);  // incrementor name
        frs_spectra_ptr->push_back(spec_tmp) ;
        spec_cathode_raw.push_back(spec_tmp);
        //------------------
        name = name_of_this_element + "__x_left" + numerek + "_raw" ;
        spec_tmp  = new spectrum_1D(name, name,
                                    4095, 1, 4096,
                                    folder,
                                    "",
                                    name);  // incrementor name
        frs_spectra_ptr->push_back(spec_tmp) ;
        spec_x_left_raw.push_back(spec_tmp);
        //------------------
        name = name_of_this_element + "__x_right" + numerek + "_raw" ;
        spec_tmp  = new spectrum_1D(name, name,
                                    4095, 1, 4096,
                                    folder,
                                    "",
                                    name);  // incrementor name
        frs_spectra_ptr->push_back(spec_tmp) ;
        spec_x_right_raw.push_back(spec_tmp);
        //------------------
        name = name_of_this_element + "__y_up" + numerek + "_raw" ;
        spec_tmp  = new spectrum_1D(name, name,
                                    4095, 1, 4096,
                                    folder,
                                    "",
                                    name);  // incrementor name
        frs_spectra_ptr->push_back(spec_tmp) ;
        spec_y_up_raw.push_back(spec_tmp);
	//------------------
        name = name_of_this_element + "__y_down" + numerek + "_raw" ;
        spec_tmp  = new spectrum_1D(name, name,
                                    4095, 1, 4096,
                                    folder,
                                    "",
                                    name);  // incrementor name
        frs_spectra_ptr->push_back(spec_tmp) ;
        spec_y_down_raw.push_back(spec_tmp);
        //------------------
	name = name_of_this_element + "__y_sum" +numerek + "_raw";
        spec_tmp = new spectrum_1D(name,name,
                                   4095,1,4096,
				   folder,
				   "",
				   name);
        frs_spectra_ptr->push_back(spec_tmp);
	spec_y_sum_raw.push_back(spec_tmp);
	
        //---------------------------

        name = name_of_this_element + "__x_sum_left_right" + numerek + "__vs__cathode" + numerek + "_raw" ;
        spectrum_2D*  spec_tmp2  =
            new spectrum_2D(name, name,
                            950, 1500, 3500,
                            950, 1, 3000,
                            folder,
                            "",
                            "X: " + name_of_this_element + "__cathode" + numerek + "_raw"
                            "  Y: " + name_of_this_element + "_x_sum_left_right" + numerek + "_raw"
                           );  // incrementer name

        frs_spectra_ptr->push_back(spec_tmp2) ;
        spec_x_gain_vs_cath.push_back(spec_tmp2);

        polygon_name.push_back(name);



    } // end for over 10 sections

    string name;
    name = name_of_this_element + "__tof_final";
    spec_ppac_tof  = new spectrum_1D(name, name,
                                     8000, 0, 3,
                                     folder,
                                     "",
                                     name_of_this_element + "__tof_when_valid"); // incrementor name
    frs_spectra_ptr->push_back(spec_ppac_tof) ;
    //-----------
    name = name_of_this_element + "__x_fp";
    spec_x_fp_mm  = new spectrum_1D(name, name,
                                    4095, -0.5, 1.5,
                                    folder,
                                    "in meters",
                                    name);  // incrementor name
    frs_spectra_ptr->push_back(spec_x_fp_mm) ;
    //-----------
    name = name_of_this_element + "__y_fp";
    spec_y_fp_mm  = new spectrum_1D(name, name,
                                    2048, -0.05, 0.05,
                                    folder,
                                    "in meters",
                                    name);  // incrementor name
    frs_spectra_ptr->push_back(spec_y_fp_mm) ;


    //---------------- 2D   -----
    name = name_of_this_element + "__tof_vs_x_fp";
    spec_tof_final__vs__x_fp_mm  = new spectrum_2D(name, name,
            1000, 0, 1,
            800, 0, 0.5,
            folder,
            "",
            "X: " + name_of_this_element + "__x_fp_when_valid"
            + "   Y: " + name_of_this_element + "__tof_when_valid"
                                                  );  // incrementor name

    frs_spectra_ptr->push_back(spec_tof_final__vs__x_fp_mm) ;


    //---------------2D -----------
    name = name_of_this_element + "__y_fp_mm_vs_x_fp";
    spec_y_fp_mm_vs_x_fp_mm = new spectrum_2D(name,name,
            2048,0,1,
	    2048,-0.05,0.05,
	    folder,
	    "",
	    "X: " + name_of_this_element + "x_fp_when_valid"
	    + "  Y: " + name_of_this_element + "y_fp_when_valid");
	    
    frs_spectra_ptr->push_back(spec_y_fp_mm_vs_x_fp_mm); 

}
//**************************************************************************************
void TprismaPPAC::make_preloop_action(std::ifstream &  s)
{

    for(int i = 0 ; i < polygon_name.size() ; i++)
    {
        // reading the polygon
        TjurekPolyCond  *p;
        string name_pol = (polygon_name[i]) + "_polygon_good_events.poly" ;
        // remove polygon with this name if it already exists.



        if(!read_banana(name_pol, &p))
        {
            cerr << "During Reading-in the parameters for PPAC "
                 << name_of_this_element
                 << " it is impossible to read polygon gate: "
                 << name_pol
                 << "  - Most probably it does not exist (anymore?)"
                 << "!!! The polygon is going to be automaticaly created for you - set it position properly"
                 << endl;

            // creating it
            ofstream plik(("polygons/" + name_pol).c_str());
            if(!plik)         exit(1);

            plik <<
                 "1922.44		2272.33		 //    nr 0	   \n"
                 "1933.99		2152.23		 //    nr 1	   \n"
                 "1943.89		2116.9		 //    nr 2	   \n"
                 "2021.45		2120.43		 //    nr 3	   \n"
                 "1937.29		2014.46		 //    nr 4	   \n"
                 "1922.44		1950.88		 //    nr 5	   \n"
                 "1922.44		1364.5		 //    nr 6	   \n"
                 "1929.04		541.456		 //    nr 7	   \n"
                 "1886.14		488.47		 //    nr 8	   \n"
                 "1846.53		597.974		 //    nr 9	   \n"
                 "1848.18		1339.78		 //    nr 10	   \n"
                 "1848.18		1929.69		 //    nr 11	   \n"
                 "1742.57		1890.83		 //    nr 12	   \n"
                 "1854.79		2046.25		 //    nr 13	   \n"
                 "1877.89		2145.16		 //    nr 14	   \n"
                 "1897.69		2268.8		 //    nr 15	   \n"
                 << endl;
            //sleep(5);
            plik.close();
            if(! read_banana(name_pol, &p))
            {
                cout << "Could not create the polygon " << name_pol << " for you " << endl;
                exit(126);
            } // if not not
        }// if not
        polygon.push_back(p);
    }

    //    theFile = new ConfigurationFile( conf_file.c_str() );
    //   if(read_configuration() > 0 )

    valid_conf = 0;
    //////////////////////////////////
    // definition of the parameters
    //////////////////////////////////
    // preliminary check
    if(n_par < 6)
    {
        cout << " Error! Only " << n_par << " parameters defined, but PPAC has normally more ..." << endl;
        exit(78);
    }
    // use a temporary index
    int indici[6], ii, jj;

    indici[0] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_yu");
    indici[1] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_yd");
    indici[2] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_xl");
    indici[3] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_xr");
    indici[4] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_tof");
    indici[5] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_cathode");

    // should check for inconsistencies ----------------------------------------------------------
    for(ii = 0; ii < 6; ii++)
    {
        if(indici[ii] > (n_par - 1))
        {
            ostringstream t;
            t << " Error with definition of parameter " << indici[ii] << "(" << ii << ")";
            Tfile_helper_exception m;
            m.message = t.str();
            throw m;
        }
        for(jj = 0; jj < ii; jj++)
        {
            if(indici[ii] == indici[jj])
            {
                ostringstream t;
                t << " Error! Duplicated parameters "  << indici[ii] << "(" << ii << "), " <<
                  indici[jj] << "(" << jj << ")" ;
                Tfile_helper_exception m;
                m.message = t.str();
                throw m;

            }
        }
    }
    // everything OK, can store in "real" params ----------------------------------------------
    ind_yu   = indici[0];
    ind_yd   = indici[1];
    ind_xl   = indici[2];
    ind_xr   = indici[3];
    ind_tof  = indici[4];
    ind_cathode   = indici[5];

    //////////////////////////////////
    //// Banana files
    /////////////////////////////////
    //   ban_file = string( theFile->get_string("ppac_banana", "ppac_banana.ban") );
    //   ban_res_x = theFile->get_int("ban_res_x",  8192);
    //   ban_res_y = theFile->get_int("ban_res_y",  8192);

    //////////////////////////////////
    //// Calibration files
    /////////////////////////////////

    xfp_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xfp_gain");
    xfp_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xfp_offs");

    xl_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xl_gain");
    xl_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xl_offs");

    xr_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xr_gain");
    xr_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xr_offs");

    // ------------Y --------------
    yfp_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_yfp_gain");
    yfp_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_yfp_offs");

    yu_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_yu_gain");
    yu_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_yu_offs");

    yd_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_yd_gain");
    yd_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_yd_offs");
    //------------- ToF ------------------

    tof_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof_gain");
    tof_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof_offs");

    tof_ogain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof_ogain");
    tof_ooffs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof_ooffs");

    tof_fgain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof_fgain");
    tof_foffs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_tof_foffs");

    //////////////////////////////////////////////////
    ///////// useful gains ...                   /////
    //////////////////////////////////////////////////
    xr_xl_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xr_xl_gain");      // gain applied to xr-xl when computing xfp
    xr_cathode_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xr_cathode_gain");    // gain applied to xr-cathode when computing xfp
    cathode_xl_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_xl_gain");    // gain applied to cathode-xl when computing xfp
    check_x_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_check_x_gain");      // gain applied to xr-xl when validating event

    // --Y
    yd_yu_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_yd_yu_gain");      // gain applied to yd-yu when computing xfp
    yd_cathode_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_yd_cathode_gain");    // gain applied to yd-cathode when computing xfp
    cathode_yu_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_cathode_yu_gain");    // gain applied to cathode-yu when computing xfp
    check_y_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_check_y_gain");      // gain applied to yd-yu when validating event



    ignore_banana = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ignore_banana");

    //////////////////////////////////
    //// Threshold for data validity
    //////////////////////////////////
    lower_th = Tfile_helper::find_in_file(s,  name_of_this_element  + "_lower_th");  // equivalent to conditions found in GSORT

    valid_conf = 1;

    ///////////////////////////////////////
    /// Calibration files
    //////////////////////////////////////
    theCalManager.resize(n_par);     // "safe" allocation: not all the parameters will be actually calibrated
    // but Jurek will make the one for y calibration which was not used before. It was occupied by the "allignment parameters"


//   xl_file   = string( theFile->get_string( "xl_file", "x_left.cal") );
//   xr_file   = string( theFile->get_string( "xr_file", "x_right.cal") );
//   tof_file  = string( theFile->get_string( "tof_file", "tof.cal") );
//   tof_ofile = string( theFile->get_string( "tof_ofile", "tof_offsets.cal") );
//   xfp_file  = string( theFile->get_string( "xfp_file", "xfp.cal") );  // final focal plane calibration

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_xfp_calibration_data");
    theCalManager[ind_yu ] = new calManager(s , 10.0);

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_cathode_left_calibration_data");
    theCalManager[ind_xl ] = new calManager(s , 10.0);

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_cathode_right_calibration_data");
    theCalManager[ind_xr ] = new calManager(s, 10.0);

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_tof_calibration_data");
    theCalManager[ind_tof] = new calManager(s, 10.0);

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_alignment_offset_data");
    theCalManager[ind_yd ] = new calManager(s, 10.0);   // reminder: y will not be used in practice


    theCalManager[ind_yu ]->setGain(xfp_gain);
    theCalManager[ind_yu ]->setOffset(xfp_offs);

    theCalManager[ind_xl ]->setGain(xl_gain);
    theCalManager[ind_xl ]->setOffset(xl_offs);

    theCalManager[ind_xr ]->setGain(xr_gain);
    theCalManager[ind_xr ]->setOffset(xr_offs);


    theCalManager[ind_tof]->setGain(tof_gain);
    theCalManager[ind_tof]->setOffset(tof_offs);


    theCalManager[ind_yd ]->setGain(tof_ogain);
    theCalManager[ind_yd ]->setOffset(tof_ooffs);       // cheating


    cout << "finished reading parameters for " << name_of_this_element << endl;
}
//******************************************************************************
int TprismaPPAC::read_configuration()
{

#if 0
    valid_conf = 0;
    //////////////////////////////////
    // definition of the parameters
    //////////////////////////////////
    // preliminary check
    if(n_par < 6)
    {
        cout << " Error! Only " << n_par << " parameters defined, but PPAC has normally more ..." << endl;
        return valid_conf;
    }

    // use a temporary index
    int indici[6], ii, jj;

    indici[0] = theFile->get_int("ind_yu",  0);
    indici[1] = theFile->get_int("ind_yd",  1);
    indici[2] = theFile->get_int("ind_xl",  2);
    indici[3] = theFile->get_int("ind_xr",  3);
    indici[4] = theFile->get_int("ind_tof", 4);
    indici[5] = theFile->get_int("ind_xc",  5);

    // should check for inconsistencies
    for(ii = 0; ii < 6; ii++)
    {
        if(indici[ii] > (n_par - 1))
        {
            cout << " Error with definition of parameter " << indici[ii] << "(" << ii << ")" << endl;
            return valid_conf;
        }
        for(jj = 0; jj < ii; jj++)
        {
            if(indici[ii] == indici[jj])
            {
                cout << " Error! Duplicated parameters "  << indici[ii] << "(" << ii << "), " <<
                     indici[jj] << "(" << jj << ")" << endl;
            }
        }
    }
    // everything OK, can store in "real" params
    ind_yu   = indici[0];
    ind_yd   = indici[1];
    ind_xl   = indici[2];
    ind_xr   = indici[3];
    ind_tof  = indici[4];
    ind_xc   = indici[5];

    //////////////////////////////////
    //// Banana files
    /////////////////////////////////
    ban_file = string(theFile->get_string("ppac_banana", "ppac_banana.ban"));
    ban_res_x = theFile->get_int("ban_res_x",  8192);
    ban_res_y = theFile->get_int("ban_res_y",  8192);

    //////////////////////////////////
    //// Calibration files
    /////////////////////////////////
    xl_file   = string(theFile->get_string("xl_file", "x_left.cal"));
    xr_file   = string(theFile->get_string("xr_file", "x_right.cal"));
    tof_file  = string(theFile->get_string("tof_file", "tof.cal"));
    tof_ofile = string(theFile->get_string("tof_ofile", "tof_offsets.cal"));
    xfp_file  = string(theFile->get_string("xfp_file", "xfp.cal"));     // final focal plane calibration

    xfp_gain = theFile->get_double("xfp_gain", 1.);
    xfp_offs = theFile->get_double("xfp_offs", 0.);

    xl_gain = theFile->get_double("xl_gain", 1.);
    xl_offs = theFile->get_double("xl_offs", 0.);

    xr_gain = theFile->get_double("xr_gain", 1.);
    xr_offs = theFile->get_double("xr_offs", 0.);

    tof_gain = theFile->get_double("_tof_gain", 1.);
    tof_offs = theFile->get_double("tof_offs", 0.);

    tof_ogain = theFile->get_double("tof_ogain", 1.);
    tof_ooffs = theFile->get_double("tof_ooffs", 0.);

    tof_fgain = theFile->get_double("tof_fgain", 0.1);
    tof_foffs = theFile->get_double("tof_foffs", 0.);

    //////////////////////////////////////////////////
    ///////// useful gains ...                   /////
    //////////////////////////////////////////////////
    xr_xl_gain = theFile->get_double("xr_xl_gain", 0.25);     // gain applied to xr-xl when computing xfp
    xr_xc_gain = theFile->get_double("xr_xc_gain", 1.0);      // gain applied to xr-xc when computing xfp
    xc_xl_gain = theFile->get_double("xc_xl_gain", 1.0);      // gain applied to xc-xl when computing xfp
    check_gain = theFile->get_double("check_gain", 0.5);      // gain applied to xr-xl when validating event

    ignore_banana = theFile->get_int("ignore_banana", 0);

    //////////////////////////////////
    //// Threshold for data validity
    //////////////////////////////////
    lower_th = theFile->get_double("lower_th", 4.);  // equivalent to conditions found in GSORT

#endif

    valid_conf = 1;
    return valid_conf;
}
//********************************************************************************************
void TprismaPPAC::init_data()
{

#if 0
    ///////////////////////////////////////
    /// Banana(s)
    //////////////////////////////////////
    theBanManager = new banManager(ban_file, ban_res_x, ban_res_y);

    ///////////////////////////////////////
    /// Calibration files
    //////////////////////////////////////
    theCalManager.resize(n_par);     // "safe" allocation: not all the parameters will be actually calibrated

    theCalManager[ind_yu ] = new calManager(xfp_file);     // reminder: y will not be used in practice
    theCalManager[ind_xl ] = new calManager(xl_file);
    theCalManager[ind_xr ] = new calManager(xr_file);
    theCalManager[ind_tof] = new calManager(tof_file);
    theCalManager[ind_yd ] = new calManager(tof_ofile);    // reminder: y will not be used in practice

    theCalManager[ind_yu ]->setGain(xfp_gain);
    theCalManager[ind_yu ]->setOffset(xfp_offs);

    theCalManager[ind_xl ]->setGain(xl_gain);
    theCalManager[ind_xl ]->setOffset(xl_offs);

    theCalManager[ind_xr ]->setGain(xr_gain);
    theCalManager[ind_xr ]->setOffset(xr_offs);

    theCalManager[ind_tof]->setGain(tof_gain);
    theCalManager[ind_tof]->setOffset(tof_offs);

    theCalManager[ind_yd ]->setGain(tof_ogain);
    theCalManager[ind_yd ]->setOffset(tof_ooffs);
#endif

}
//***********************************************************************************
int TprismaPPAC::validate_event(std::vector<detData*>& theEvent)
{
    int ii, nvalid = 0;
    int over_th;
    detData* aData = 0;
    xfp_ok = false;
    tof_ok = false;

    flag_y_is_in_banana_ok = false;


    err_code = 0;

    // valid events should have:
    //  1) TOF
    // 2) x_cathode
    //  3) AT LEAST one of x_left, x_right
    //  4) banana to cut away noise!!!

    // at the beginning, nothing is valid ...
    valid.clear();
    valid_evt = 0;
    processed_event = 0;

    //////////////////////////////////////////////////////
    ///// Buffers to store raw/partially calibrated data
    ///// for initial stages of data analysis
    /////////////////////////////////////////////////////
    // dummy values;
    ppac_xfp = -1000.;
    ppac_tof = -1000.;
    cal_xf   = -1000.;
    cal_xr   = -1000.;
    cal_xl   = -1000.;

    raw_xl   = -1000.;
    raw_xr   = -1000.;
    raw_xf   = -1000.;
    raw_tf   = -1000.;

    valid_idx = -1;
    for(int i = 0 ; i < tof_raw.size() ; i++)
    {
        tof_raw[i] = 0;
        tof_cal[i] = 0;
        tof_cal_realigned[i] = 0;
        cathode_raw[i] = 0;
        sumXleftRight_raw[i] = 0;
    }
//     sumYupDown_raw = 0;
//     det_0_cathode_for_y_purposes = 0;


    // empty buffer -> not valid!
    if(theEvent.size() < 1)
    {
        err_code = 8;
        return -1;
    }

//     cout << "-------------------- In PPAC the event data is : " << endl;
//     for ( ii=0; ii<(int)theEvent.size(); ii++ )
//     {
//         cout << *(theEvent[ii]) ;   // endl is tere by default << endl;
//     }

    //====================================
    // start checking whether TOF exists ========
    //====================================
    for(ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
        int cur_det = aData->num_det();

        // raw tof
        tof_raw[cur_det] = (int)(*aData)[ind_tof] ;
        spec_tof_raw[cur_det]->manually_increment(tof_raw[cur_det]);




        // TOF calibrations
//      cout << "ind_taData->num_det() of = " << ind_tof << " while size is theCalManager.size() " << theCalManager.size()
//      << ", aData->num_det()="<<  aData->num_det()
//      << "raw tof is = "  << (*aData)[ind_tof]
//      << endl;
        int run = 0;

        double dummy_tof = theCalManager[ind_tof]->getCalValue(run, cur_det, (*aData)[ind_tof]);
        tof_cal[cur_det] = dummy_tof;
        spec_tof_cal[cur_det]->manually_increment(tof_cal[cur_det]);

//     cout << "dummy_tof = " << dummy_tof << endl;
        // if(ii == 1)
//         {
//         cout << "xxx tof_cal[" << valid[ii]<< "] ="  << tof_cal[valid[ii]] << endl;
//
//         }

        //===========================================================================
        // TOF realignment (plus conversion in ns)

//         cout << "dummy_tof = " << dummy_tof << endl;
        dummy_tof = theCalManager[ind_yd ]->getCalValue(run, cur_det, dummy_tof);     // this is cheating with the use of [ind_yd]
//         cout << "after realignment dummy_tof = " << dummy_tof << endl;
        //     COTO
        tof_cal_realigned[aData->num_det()] = dummy_tof;
        // COTO
        // cout << "xxx tof_cal_realigned[" << valid[ii]<< "] ="  << tof_cal_realigned[valid[ii]] << endl;
        spec_tof_cal_realigned[cur_det]->manually_increment(tof_cal_realigned[cur_det]);


        //&&&&&&&&&&&&&&&&&&&&&&&&&&&&

        if((*aData)[ind_tof] >= lower_th)
        {
            aData->set_valid(1);
            nvalid++;
        }
        else
            aData->set_valid(0);
    }

    if(nvalid < 1)
    {
        err_code = 16;
        return -1;   // nothing to do ...
    }

    // x_cathode should exist
    for(ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
        int cur_det = aData->num_det();

        if(!aData->is_valid()) continue;    // missing TOF

        cathode_raw[cur_det] = (int) (*aData)[ind_cathode];
        spec_cathode_raw[cur_det]->manually_increment(cathode_raw[cur_det]);

        if((*aData)[ind_cathode] <= lower_th)
        {
            aData->set_valid(0);
            nvalid--;
            continue;
        }
    }

    if(nvalid < 1)
    {
        err_code = 32;
        return -1;   // nothing to do ...
    }

    // now checks x_left, x_right
    for(ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
        int cur_det = aData->num_det();
//         cout
//             << " y up=" << (*aData)[ind_yu]
//             << " y dn=" << (*aData)[ind_yd]
//             << " X left= " << (*aData)[ind_xl]
//             << " X right =" << (*aData)[ind_xr]
//             << " tof =" << (*aData)[ind_tof]
//             << " Cathode  =" << (*aData)[ind_cathode]
//             << endl;


        if(!aData->is_valid()) continue;    // missing TOF or missing cathode

        spec_x_left_raw[cur_det]->manually_increment((*aData)[ind_xl]);
        spec_x_right_raw[cur_det]->manually_increment((*aData)[ind_xr]);


        over_th = 0;
        if((*aData)[ind_xl] > lower_th) over_th++;
        if((*aData)[ind_xr] > lower_th) over_th++;

        if(cur_det == 0)
        {
            if((*aData)[ind_yu] > lower_th) over_th++;
            if((*aData)[ind_yd] > lower_th) over_th++;
        }

        if(over_th < 1)
        {
            aData->set_valid(0);
            nvalid--;
            continue;
        }
    }

    if(nvalid < 1)
    {
        err_code = 64;
        return -1;   // nothing to do ...
    }

    // finally, X banana time!
    if(ignore_banana == 0)
    {
        for(ii = 0; ii < (int)theEvent.size(); ii++)
        {
            aData = theEvent[ii];
            if(!aData->is_valid()) continue;    // missing TOF or missing x info

            int cur_det = aData->num_det();
            // ------- X -------
            double xx = (*aData)[ind_cathode];
            sumXleftRight_raw[cur_det] = check_x_gain * ((*aData)[ind_xl] + (*aData)[ind_xr]);
            

//       cout << "xx = " << xx << ", yy = " << yy << endl;
            spec_x_gain_vs_cath[cur_det]->manually_increment(xx, sumXleftRight_raw[cur_det]) ;


            if(polygon[cur_det]->Test(xx,  sumXleftRight_raw[cur_det]) == false)
                // if( theBanManager->inside( aData->num_det(), (*aData)[ind_cathode], check_gain*((*aData)[ind_xl]+(*aData)[ind_xr]) ) < 1 )
            {
//                 cout << "Outside X banana for det nr " << cur_det << endl;
//                 cout << "X  Outisde the polygon gate is the poiint  ["
//                      << xx << ",  " <<  sumXleftRight_raw[cur_det]  << "]  "
//                      << endl;
                aData->set_valid(0);
                nvalid--;
                continue;
            }
            else
            {
//                 cout << "inside X banana for det nr " << cur_det << endl;
                // I need this because y inforation from det 0 does not have its own cathode
                // det_0_cathode_for_y_purposes = cathode_raw[cur_det] ;
            }
	    
	    // ------- Y -------
	    sumYupDown_raw[cur_det] = check_y_gain * ((*aData)[ind_yu] + (*aData)[ind_yd]);
	    
	    spec_y_up_raw[cur_det]->manually_increment((*aData)[ind_yu]);
            spec_y_down_raw[cur_det]->manually_increment((*aData)[ind_yd]);
	    spec_y_sum_raw[cur_det]->manually_increment(((*aData)[ind_yd]-(*aData)[ind_yu]+4096)/2);

        }


    } // end for event
    // -----------------------------------


    if(nvalid < 1)
    {
        err_code = 128;
        return -1;   // nothing to do ...
    }

    if(nvalid > 1)
    {
        err_code = 256;
        return -1;   // nothing to do ...
    }

    // fill up lookup table of valid detectors
    valid.clear();
    for(ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
        if(aData->is_valid())
        {
            valid.push_back(ii);
            valid_idx = aData->num_det();
            if((valid_idx < 0) || (valid_idx >= n_det))
            {
                cout << " --> Warning, inconsistent value (valid_idx = " << valid_idx << ")" << endl;
            }
        }
    }
    valid_evt = 1;
//     cout << "Valid PPAC event " << endl;
    return nvalid;
}
//**************************************************************************
void TprismaPPAC::process_event(std::vector<detData*>& theEvent)
{
    int ii, run = 0;
    detData* aData = 0;
    double dummy_xfp, dummy_tof;
    // ========================== TOF: we just need to calibrate using the coefficients
    ppac_tof = 0.;
    for(ii = 0; ii < (int)valid.size(); ii++)
    {
        aData = theEvent[valid[ii]];
        int cur_det = aData->num_det();

        //     cout << "cur_det has number " << cur_det << ", aData = " << (*aData) << endl;

        // TOF calibrations
//      cout << "ind_taData->num_det() of = " << ind_tof << " while size is theCalManager.size() " << theCalManager.size()
//      << ", aData->num_det()="<<  aData->num_det()
//      << "raw tof is = "  << (*aData)[ind_tof]
//      << endl;

        dummy_tof = theCalManager[ind_tof]->getCalValue(run, cur_det, (*aData)[ind_tof]);
        tof_cal[cur_det] = dummy_tof;
//         spec_tof_cal[cur_det]->manually_increment(tof_cal[cur_det]);


//     cout << "dummy_tof = " << dummy_tof << endl;
        // if(ii == 1)
//         {
//         cout << "xxx tof_cal[" << valid[ii]<< "] ="  << tof_cal[valid[ii]] << endl;
//
//         }
        // TOF realignment (plus conversion in ns)

//         cout << "dummy_tof = " << dummy_tof << endl;
        dummy_tof = theCalManager[ind_yd ]->getCalValue(run, cur_det, dummy_tof);
//         cout << "after realignment dummy_tof = " << dummy_tof << endl;
        //     COTO
        tof_cal_realigned[aData->num_det()] = dummy_tof;
        // COTO
        // cout << "xxx tof_cal_realigned[" << valid[ii]<< "] ="  << tof_cal_realigned[valid[ii]] << endl;
        //  spec_tof_cal_realigned[cur_det]->manually_increment(tof_cal_realigned[cur_det]);

        raw_tf = dummy_tof;
        ppac_tof += dummy_tof * ns;
        tof_ok = true;
    }
    // COTO
    ppac_tof = tof_fgain * ppac_tof + tof_foffs;
    spec_ppac_tof->manually_increment(ppac_tof);


    // ================= x_fp: we have to apply xfp_prisma (written in a more "readable" way)
    ppac_xfp = 0.;
    xfp_ok  = false;
    for(ii = 0; ii < (int)valid.size(); ii++)
    {
        aData = theEvent[valid[ii]];
        int cur_det = aData->num_det();
        dummy_xfp = 0.;

        // basic xfp_prisma
        // we performed the necessary checks elsewhere!!!
        if(((*aData)[ind_xl] > lower_th) && ((*aData)[ind_xr] > lower_th))
        {
            // both x_left, x_right
            dummy_xfp = xr_xl_gain * ((*aData)[ind_xr] - (*aData)[ind_xl]) + 2000.;
            // calibration
            dummy_xfp = theCalManager[ind_yu]->getCalValue(run, cur_det, dummy_xfp);

            cal_xf = dummy_xfp;
        }
        else if((*aData)[ind_xl] > lower_th)
        {
            // x_left, x_cath
            dummy_xfp = cathode_xl_gain * ((*aData)[ind_cathode] - (*aData)[ind_xl]) + 2000.;
            // calibration
            dummy_xfp = theCalManager[ind_xl]->getCalValue(run, cur_det, dummy_xfp);
            cal_xl = dummy_xfp;
        }
        else
        {
            dummy_xfp = xr_cathode_gain * ((*aData)[ind_xr] - (*aData)[ind_cathode]) + 2000.;
            // calibration
            dummy_xfp = theCalManager[ind_xr]->getCalValue(run, cur_det, dummy_xfp);
            cal_xr = dummy_xfp;
        }
        // finally, transform to mm!
        // remember that xfp coefficients were stored into y_up
        ppac_xfp += dummy_xfp * mm;
        xfp_ok = true;
    } // end 10 (valid) sections
    //   COTO

    if(xfp_ok)
    {
        spec_x_fp_mm->manually_increment(ppac_xfp);
//         if(ppac_xfp > 0.01) cout << "PPAC ppac_xfp " << ppac_xfp << endl;
        
        spec_tof_final__vs__x_fp_mm->manually_increment(ppac_xfp, ppac_tof);
    }
    //==================== Y ===================================
    // ================= y_fp: we have to apply yfp_prisma (written in a more "readable" way)
    ppac_yfp = 0.;
    yfp_ok = false;
    for(ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
//         aData = theEvent[valid[ii]];
        int cur_det = aData->num_det();
//         cout << "For Y calibration cur_det = " << cur_det << endl;

//        if(cur_det != 0) continue;    // NOTE: y infrmation is only in section nr 0

        double dummy_yfp = 0.;

        // basic yfp_prisma
        // we performed the necessary checks elsewhere!!!
        if(((*aData)[ind_yu] > lower_th) && ((*aData)[ind_yd] > lower_th))      // both y_up, y_down - are OK
        {
            dummy_yfp = yd_yu_gain * ((*aData)[ind_yd] - (*aData)[ind_yu]) ; // + 2000.;
            // calibration
//             cout << "Befire cakubration yfp " << dummy_yfp << endl;

            //dummy_yfp = theCalManager[ind_yu]->getCalValue( run, cur_det, dummy_yfp );


            cal_yf =  yfp_offs   + (dummy_yfp * yfp_gain) ;
//             cout << "After  cakubration yfp " << cal_yf << endl;
            //cal_yf = dummy_yfp;
            // finally, transform to mm!
            // remember that yfp coefficients were stored into y_up
            ppac_yfp = cal_yf   * mm;
            yfp_ok = true;
        }



#if 0
        else if((*aData)[ind_yu] > lower_th)        // y_up,  y_dn not exist  - so we use x_cath    // calibration the UP
        {
            dummy_yfp = cathode_yu_gain * ((*aData)[ind_cathode] - (*aData)[ind_yu]) + 2000.;
            // calibration
            dummy_yfp = theCalManager[ind_yu]->getCalValue(run, cur_det, dummy_yfp);
            cal_yu = dummy_yfp;
        }
        else                //  calibration the UP , while down does not exist
        {
            dummy_yfp = yd_cathode_gain * ((*aData)[ind_yd] - (*aData)[ind_cathode]) + 2000.;
            // calibration
            dummy_yfp = theCalManager[ind_yd]->getCalValue(run, cur_det, dummy_yfp);
            cal_yd = dummy_yfp;
        }
#endif // 0


    } // end 10 (valid) sections
    //   COTO

    if(yfp_ok)
    {
        spec_y_fp_mm->manually_increment(ppac_yfp);
    }

    if(yfp_ok && xfp_ok)
    {
        spec_y_fp_mm_vs_x_fp_mm->manually_increment(ppac_xfp,ppac_yfp);
    }

    processed_event = 1;
}
#endif // CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT


