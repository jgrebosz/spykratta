#include "TprismaMcp.h"

#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#include "Vectors.h"
#include "Tnamed_pointer.h"


const double PI = 3.14159265;
const double RADIANS_TO_DEGREES = 180.0 / (PI);
const double DEGREES_TO_RADIANS = (PI) /180.0;
//******************************************************************
TprismaMcp::TprismaMcp(string det_name,  int ndet, int npar) :   TprismaDetector(det_name)
{
    theBanManager = NULL ;// to avoid error during desruction
    n_det = ndet;
    n_par = npar;

//   theFile = new ConfigurationFile( conf_file.c_str() );
//   cout << "Conf file = " << conf_file << endl;
//   if( read_configuration() > 0 )
//     init_data();

	    my_selfgate_type =  Tselfgate_type::Types::german_crystal; // fake, because...

    create_my_spectra();

    //mcp_x_raw
	 	 	 // if the selfgate is diffenet than not_available

    named_pointer[name_of_this_element + "__x_raw"]
    = Tnamed_pointer(&mcp_x_raw, 0, this) ;

    named_pointer[name_of_this_element + "__y_raw"]
    = Tnamed_pointer(&mcp_y_raw, 0, this) ;

    named_pointer[name_of_this_element + "__x_cal_in_meters_when_valid"]
    = Tnamed_pointer(&mcp_x, &xy_ok, this) ;

    named_pointer[name_of_this_element + "__y_cal_in_meters_when_valid"]
    = Tnamed_pointer(&mcp_y, &xy_ok, this) ;
    // --- angles
    // Agata
    named_pointer[name_of_this_element + "__theta_radians_agata_when_valid"]
    = Tnamed_pointer(&mcp_theta_a, &angles_ok, this) ;

    named_pointer[name_of_this_element + "__phi_radians_agata_when_valid"]
    = Tnamed_pointer(&mcp_phi_a, &angles_ok, this) ;
    // beam
    named_pointer[name_of_this_element + "__theta_radians_beam_when_valid"]
    = Tnamed_pointer(&mcp_theta_b, &angles_ok, this) ;

    named_pointer[name_of_this_element + "__phi_radians_beam_when_valid"]
    = Tnamed_pointer(&mcp_phi_b, &angles_ok, this) ;
    // clara
    named_pointer[name_of_this_element + "__theta_radians_clara_when_valid"]
    = Tnamed_pointer(&mcp_theta_c, &angles_ok, this) ;

    named_pointer[name_of_this_element + "__phi_radians_clara_when_valid"]
    = Tnamed_pointer(&mcp_phi_c, &angles_ok, this) ;

}
//*****************************************************************
TprismaMcp::~TprismaMcp()
{
//   delete theBanManager;

    delete   theCalManager[ind_xm];
    delete theCalManager[ind_ym] ;

    theCalManager.clear();
    valid.clear();
//   delete theFile;
}
//********************************************************************
void TprismaMcp::create_my_spectra()
{
    string folder = "prisma_MCP/" + name_of_this_element ;
    //-----------
    string name = name_of_this_element + "__x_raw"  ;
    spec_mcp_x_raw = new spectrum_1D(name, name,
                                     4095, 1, 4096,
                                     folder,
                                     "",
                                     name_of_this_element + "__x_raw");
    frs_spectra_ptr->push_back(spec_mcp_x_raw) ;

    name = name_of_this_element + "__y_raw"  ;
    spec_mcp_y_raw = new spectrum_1D(name, name,
                                     4095, 1, 4096,
                                     folder,
                                     "",
                                     name_of_this_element + "__y_raw");
    frs_spectra_ptr->push_back(spec_mcp_y_raw) ;
    //------------------------
    name = name_of_this_element + "__xy_raw" ;
    spec_xy_raw = new spectrum_2D(name,
                                  name,
                                  500, 0, 4096,
                                  500, -0, 4096,
                                  folder, " raw values",
                                  string("as X:") + name_of_this_element + "__x_raw" +
                                  ", as Y: " + name_of_this_element + "__y_raw"
                                 );
    frs_spectra_ptr->push_back(spec_xy_raw) ;
    polygon_name = name;

    // ------------- cal -------------------
    name = name_of_this_element + "__x_cal_centimeters"  ;
    spec_mcp_x_cal = new spectrum_1D(name, name,
                                     1200, -6, 6,
                                     folder,
                                     "when inside 'good points' polygon (see: " + polygon_name + ".mat) - (if the option mcp_ignore_banana is  0)",
                                     name_of_this_element + "__x_cal_when_valid    (its value is in meters)");
    frs_spectra_ptr->push_back(spec_mcp_x_cal) ;

    name = name_of_this_element + "__y_cal_centimeters"  ;
    spec_mcp_y_cal = new spectrum_1D(name, name,
                                     1200, -6, 6,
                                     folder,
                                     "when inside 'good points' polygon (see: " + polygon_name + ".mat) - (if the option mcp_ignore_banana is  0)",
                                     name_of_this_element + "__y_cal_when_valid    (its value is in meters)");
    frs_spectra_ptr->push_back(spec_mcp_y_cal) ;


    //-------------------------------------- 2D ----

    name = name_of_this_element + "__xy_cal_in_centimeters" ;
    spec_xy_cal = new spectrum_2D(name,
                                  name,
                                  1200, -6, 6,
                                  1200, -6, 6,
                                  folder, "note?",
                                  string("as X:") + name_of_this_element + "__x_cal_when_valid    (its value is in meters)" +
                                  ", as Y: " + name_of_this_element + "__y_cal_when_valid    (its value is in meters)"
                                 );
    frs_spectra_ptr->push_back(spec_xy_cal) ;
    //-------------
    name = name_of_this_element + "__phi_beam"  ;
    spec_phi_beam = new spectrum_1D(name, name,
                                    3600, -180, 180,
                                    folder,
                                    "in degrees",
                                    name_of_this_element + "__phi_radians_beam_when_valid    (its was here recalcuted to degrees)");
    frs_spectra_ptr->push_back(spec_phi_beam) ;

    name = name_of_this_element + "__theta_beam"  ;
    spec_theta_beam = new spectrum_1D(name, name,
                                      1800, 0, 180,
                                      folder,
                                      "in degrees",
                                      name_of_this_element + "__theta_radians_beam_when_valid    (its was here recalcuted to degrees)");
    frs_spectra_ptr->push_back(spec_theta_beam) ;
    
    //--------------------------------2D----
    
    name = name_of_this_element + "_theta_phi_beam" ;
    
    spec_theta_phi_beam = new spectrum_2D(name,name,
                                 720,   0,180,
				1080,-180,180,
				folder, 
				  "in degrees",
                                  string("as X:") + name_of_this_element + "__theta_radians_beam_when_valid    (its was here recalcuted to degrees)" +
                                  ", as Y: " + name_of_this_element + "__phi_radians_beam_when_valid    (its was here recalcuted to degrees)"
                                 );
    frs_spectra_ptr->push_back(spec_theta_phi_beam) ;
    //------------------- Clara
    name = name_of_this_element + "__phi_clara"  ;
    spec_phi_clara = new spectrum_1D(name, name,
                                     3600, -180, 180,
                                     folder,
                                     "in degrees",
                                     name_of_this_element + "__phi_radians_clara_when_valid     (its was here recalcuted to degrees)");
    frs_spectra_ptr->push_back(spec_phi_clara) ;

    name = name_of_this_element + "__theta_clara"  ;
    spec_theta_clara = new spectrum_1D(name, name,
                                       1800, 0, 180,
                                       folder,
                                       "in degrees",
                                       name_of_this_element + "__theta_radians_clara_when_valid     (its was here recalcuted to degrees)");
    frs_spectra_ptr->push_back(spec_theta_clara) ;
    
    
    //------------------------------2D---
    
    name = name_of_this_element + "__theta_phi_clara";    
    spec_theta_phi_c = new spectrum_2D(name,name,
                                 720,   0,180,
				1080,-180,180,
				folder, 
				  "in degrees",
                                  string("as X:") + name_of_this_element + "__theta_radians_clara_when_valid    (its was here recalcuted to degrees)" +
                                  ", as Y: " + name_of_this_element + "__phi_radians_clara_when_valid    (its was here recalcuted to degrees)"
                                 );
    frs_spectra_ptr->push_back(spec_theta_phi_c) ;
    
    
        //------------------- Laboratory
    name = name_of_this_element + "__phi_lab"  ;
    spec_phi_lab = new spectrum_1D(name, name,
                                     3600, -180, 180,
                                     folder,
                                     "in degrees",
                                     name_of_this_element + "__phi_radians_laboratory_when_valid     (its was here recalcuted to degrees)");
    frs_spectra_ptr->push_back(spec_phi_lab) ;

    name = name_of_this_element + "__theta_lab"  ;
    spec_theta_lab = new spectrum_1D(name, name,
                                       1800, 0, 180,
                                       folder,
                                       "in degrees",
                                       name_of_this_element + "__theta_radians_laboratory_when_valid     (its was here recalcuted to degrees)");
    frs_spectra_ptr->push_back(spec_theta_lab) ;
    
    
    //------------------------------2D---
    
    name = name_of_this_element + "__theta_phi_lab";    
    spec_theta_phi_lab = new spectrum_2D(name,name,
                                 720,   0,90,
				1080,-35,35,
				folder, 
				  "in degrees",
                                  string("as X:") + name_of_this_element + "__theta_radians_laboratory_when_valid    (its was here recalcuted to degrees)" +
                                  ", as Y: " + name_of_this_element + "__phi_radians_laboratory_when_valid    (its was here recalcuted to degrees)"
                                 );
    frs_spectra_ptr->push_back(spec_theta_phi_lab) ;
    
    
}
//********************************************************************
void  TprismaMcp::make_preloop_action(std::ifstream & s)
{
    cout << "Reading the calibration for " << name_of_this_element << endl ;



    
    // reading the polygon
    string name_pol = (polygon_name) + "_polygon_good_points.poly" ;
    if(!read_banana(name_pol, &polygon))
    {
        cerr << "During Reading-in the parameters for PPAC "
             << name_of_this_element
             << " it is impossible to read polygon gate: "
             << name_pol
             << "  - Most probably it does not exist (anymore?)\n"
             << "!!! The polygon is going to be automaticaly created for you - set it position properly"
             << endl;

        // creating it
        ofstream plik(("polygons/" + name_pol).c_str());
        if(!plik)         exit(1);

        plik <<  "1200 3400\n 1000 1000\n3200 1000\n3200 3300\n" << endl;
        //sleep(5);
        plik.close();
        if(! read_banana(name_pol, &polygon))
        {
            cout << "Could not create the polygon " << name_pol << " for you " << endl;
            exit(125);
        };
    }

    valid_conf = 0;
    //////////////////////////////////
    // definition of the parameters
    //////////////////////////////////
    // preliminary check
    if(n_par < 2)
    {
        cout << " Error! Only " << n_par << " parameters defined, but Mcp has normally more ..." << endl;
        exit(-2);

//     return valid_conf;
    }

    // use a temporary index
    int indici[2];

//   indici[0] = theFile->get_int("ind_xm",  0);
//   indici[1] = theFile->get_int("ind_ym",  1);

    indici[0] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_xm");
    indici[1] = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ind_ym");


    // should check for inconsistencies
    for(int ii = 0; ii < 2; ii++)
    {
        if(indici[ii] > (n_par - 1))
        {
            cout << " Error with definition of parameter " << indici[ii] << "(" << ii << ")" << endl;
            exit(3);
//       return valid_conf;
        }
        for(int jj = 0; jj < ii; jj++)
        {
            if(indici[ii] == indici[jj])
            {
                cout << " Error! Duplicated parameters "  << indici[ii] << "(" << ii << "), " <<
                     indici[jj] << "(" << jj << ")" << endl;
            }
        }
    }
    // everything OK, can store in "real" params

    ind_xm   = indici[0];
    ind_ym   = indici[1];

    //////////////////////////////////
    //// Banana files
    /////////////////////////////////

    // made already on the top of the function

//   ban_file = string( theFile->get_string("mcp_banana", "mcp_banana.ban") );
//   ban_res_x = theFile->get_int("ban_res_x",  8192);
//   ban_res_y = theFile->get_int("ban_res_y",  8192);

    //////////////////////////////////
    //// Calibration files
    /////////////////////////////////
// xm_file  = string( theFile->get_string( "xm_file", "x_mcp.cal") );
    // ym_file  = string( theFile->get_string( "ym_file", "y_mcp.cal") );

    xm_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xm_gain");  //  theFile->get_double( "xm_gain", 1.);
    xm_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_xm_offs");   // theFile->get_double( "xm_offs", 0.);

    ym_gain = Tfile_helper::find_in_file(s,  name_of_this_element  + "_ym_gain");
    ym_offs = Tfile_helper::find_in_file(s,  name_of_this_element  + "_ym_offs");

    //////////////////////////////////
    //// Threshold for data validity
    //////////////////////////////////
    lower_th = Tfile_helper::find_in_file(s,  name_of_this_element  + "_lower_th"); // equivalent to conditions found in GSORT

    //////////////////////////////////
    //// Other parameters
    //////////////////////////////////
    mcp_mix_x[0] = Tfile_helper::find_in_file(s,  name_of_this_element  + "_mix_x_0");
    mcp_mix_x[1] = Tfile_helper::find_in_file(s,  name_of_this_element  + "_mix_x_1");
    mcp_mix_y[0] = Tfile_helper::find_in_file(s,  name_of_this_element  + "_mix_y_0");
    mcp_mix_y[1] = Tfile_helper::find_in_file(s,  name_of_this_element  + "_mix_y_1");

    theta_prisma    = Tfile_helper::find_in_file(s,  name_of_this_element  + "_angle_prisma") * degree;
    theta_op_prisma = Tfile_helper::find_in_file(s,  name_of_this_element  + "_prisma_angular_opening") * degree;
    theta_mcp       = Tfile_helper::find_in_file(s,  name_of_this_element  + "_rotation_mcp") * degree;
    mcp_target_d    = Tfile_helper::find_in_file(s,  name_of_this_element  + "_target_d") * mm;
    mcp_angle       = Tfile_helper::find_in_file(s,  name_of_this_element  + "_angle") * degree;

    ignore_banana = (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "_ignore_banana");

    costh_p = cos(theta_prisma);
//     sinth_p = sin(theta_prisma);

    costh_m = cos(mcp_angle);
    sinth_m = sin(mcp_angle);

    // used only to calculate the velocity of the binary partner, cannot find a better place for that ...
    beam_mass_number   = (int) Tfile_helper::find_in_file(s, name_of_this_element  + "_beam_mass_number"  );
    beam_zed           = (int) Tfile_helper::find_in_file(s, name_of_this_element  + "_beam_zed"          );
    beam_energy        =       Tfile_helper::find_in_file(s, name_of_this_element  + "_beam_energy"       ) * MeV;
    target_mass_number = (int) Tfile_helper::find_in_file(s, name_of_this_element  + "_target_mass_number");
    target_zed         = (int) Tfile_helper::find_in_file(s, name_of_this_element  + "_target_zed"        );
  
    valid_conf = 1;

    //------ init_data
    ///////////////////////////////////////
    /// Calibration files
    //////////////////////////////////////
    theCalManager.resize(n_par);     // "safe" allocation: not all the parameters will be actually calibrated

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_x_calibration_data");
    theCalManager[ind_xm] = new calManager(s, 1.0);

    Tfile_helper::spot_in_file(s,  name_of_this_element  + "_y_calibration_data");
    theCalManager[ind_ym] = new calManager(s, 1.0);

    theCalManager[ind_xm]->setGain(xm_gain);
    theCalManager[ind_xm]->setOffset(xm_offs);

    theCalManager[ind_ym]->setGain(ym_gain);
    theCalManager[ind_ym]->setOffset(ym_offs);

    // for the velocity of the partner
    amu = 931.49 * MeV;
    beam_momentum = sqrt(2. * beam_energy / (beam_mass_number * amu));

    beam_momentum_b = beam_momentum * Vector3D(0., 0., 1.);
    // these are the same since only x differs ...
    beam_momentum_a = beam_momentum * Vector3D(0., sinth_p, costh_p);
    beam_momentum_c = beam_momentum_a;

    cout << "Finished Reading the calibration for " << name_of_this_element << endl ;
    return ;
}
//********************************************************************
int TprismaMcp::read_configuration()
{
#if 0
    valid_conf = 0;
    //////////////////////////////////
    // definition of the parameters
    //////////////////////////////////
    // preliminary check
    if(n_par < 2)
    {
        cout << " Error! Only " << n_par << " parameters defined, but Mcp has normally more ..." << endl;
        return valid_conf;
    }

    // use a temporary index
    int indici[2];

    indici[0] = theFile->get_int("ind_xm",  0);
    indici[1] = theFile->get_int("ind_ym",  1);

    // should check for inconsistencies
    for(int ii = 0; ii < 2; ii++)
    {
        if(indici[ii] > (n_par - 1))
        {
            cout << " Error with definition of parameter " << indici[ii] << "(" << ii << ")" << endl;
            return valid_conf;
        }
        for(int jj = 0; jj < ii; jj++)
        {
            if(indici[ii] == indici[jj])
            {
                cout << " Error! Duplicated parameters "  << indici[ii] << "(" << ii << "), " <<
                     indici[jj] << "(" << jj << ")" << endl;
            }
        }
    }
    // everything OK, can store in "real" params
    ind_xm   = indici[0];
    ind_ym   = indici[1];

    //////////////////////////////////
    //// Banana files
    /////////////////////////////////
    ban_file = string(theFile->get_string("mcp_banana", "mcp_banana.ban"));
    ban_res_x = theFile->get_int("ban_res_x",  8192);
    ban_res_y = theFile->get_int("ban_res_y",  8192);

    //////////////////////////////////
    //// Calibration files
    /////////////////////////////////
    xm_file  = string(theFile->get_string("xm_file", "x_mcp.cal"));
    ym_file  = string(theFile->get_string("ym_file", "y_mcp.cal"));

    xm_gain = theFile->get_double("xm_gain", 1.);
    xm_offs = theFile->get_double("xm_offs", 0.);

    ym_gain = theFile->get_double("ym_gain", 1.);
    ym_offs = theFile->get_double("ym_offs", 0.);

    //////////////////////////////////
    //// Threshold for data validity
    //////////////////////////////////
    lower_th = theFile->get_double("lower_th", 4.);  // equivalent to conditions found in GSORT

    //////////////////////////////////
    //// Other parameters
    //////////////////////////////////
    mcp_mix_x[0] = theFile->get_double("mcp_mix_x_0", 1.);
    mcp_mix_x[1] = theFile->get_double("mcp_mix_x_1", 0.);
    mcp_mix_y[0] = theFile->get_double("mcp_mix_y_0", 0.);
    mcp_mix_y[1] = theFile->get_double("mcp_mix_y_1", 1.);

    theta_prisma = theFile->get_double("angle_prisma",  50.) * degree;
    theta_mcp    = theFile->get_double("rotation_mcp",   0.) * degree;
    mcp_target_d = theFile->get_double("mcp_target_d", 250.) * mm;
    mcp_angle    = theFile->get_double("mcp_angle",    135.) * degree;

    ignore_banana = theFile->get_int("ignore_banana", 0);

    costh_p = cos(theta_prisma);
    sinth_p = sin(theta_prisma);

    costh_m = cos(mcp_angle);
    sinth_m = sin(mcp_angle);

    // used only to calculate the velocity of the binary partner, cannot find a better place for that ...
    beam_mass_number = theFile->get_int("beam_mass_number", 82);
    beam_energy      = theFile->get_double("beam_energy", 490.) * MeV;

#endif // 0
    valid_conf = 1;
    return valid_conf;
}
//***************************************************************************************
void TprismaMcp::init_data()
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

    theCalManager[ind_xm] = new calManager(xm_file);
    theCalManager[ind_ym] = new calManager(ym_file);

    theCalManager[ind_xm]->setGain(xm_gain);
    theCalManager[ind_xm]->setOffset(xm_offs);

    theCalManager[ind_ym]->setGain(ym_gain);
    theCalManager[ind_ym]->setOffset(ym_offs);

    // for the velocity of the partner
    amu = 931.49 * MeV;
    beam_momentum = sqrt(2. * beam_energy / (beam_mass_number * amu));

    beam_momentum_b = beam_momentum * Vector3D(0., 0., 1.);
    // these are the same since only x differs ...
    beam_momentum_a = beam_momentum * Vector3D(0., sinth_p, costh_p);
    beam_momentum_c = beam_momentum_a;

#endif   // 0

}
//*****************************************************************************
int TprismaMcp::validate_event(std::vector<detData*>& theEvent)
{
    int nvalid = 0;
    detData* aData = 0;

    err_code = 0;
    xy_ok = false;
    angles_ok = false;

    // valid events should have multiplicity 1 and lie inside the banana:

    // at the beginning, nothing is valid ...
    valid.clear();
    processed_event = 0;
    valid_evt = 0;

    mcp_x_raw       = 0;
    mcp_y_raw       = 0;
    mcp_x       = 0.;   // cal
    mcp_y       = 0.;       // cal
    mcp_theta_b = 0.;
    mcp_phi_b   = 0.;
    mcp_theta_c = 0.;
    mcp_phi_c   = 0.;

    // empty buffer -> not valid!
    if(theEvent.size() < 1)
    {
        err_code = 1;
        return -1;
    }

    // banana time!

    for(int ii = 0; ii < (int)theEvent.size(); ii++)
    {
        aData = theEvent[ii];
        mcp_x_raw = (int)   (*aData)[ind_xm];
        mcp_y_raw = (int)   (*aData)[ind_ym];

        spec_xy_raw->manually_increment(mcp_x_raw, mcp_y_raw);
        spec_mcp_x_raw->manually_increment(mcp_x_raw);
        spec_mcp_y_raw->manually_increment(mcp_y_raw);

        if(polygon->Test(mcp_x_raw, mcp_y_raw) == false  && ignore_banana == 0)
            //if( theBanManager->inside( aData->num_det(), (*aData)[ind_xm], (*aData)[ind_ym] ) < 1 )
        {
            aData->set_valid(0);
            nvalid--;
            continue;
        }
        else
        {
            aData->set_valid(1);
            nvalid++;
            valid.push_back(ii);
            continue;
        }
    }


    if(nvalid < 1)
    {
        err_code = 2;
        return -1;   // nothing to do ...
    }
    if(nvalid > 1)
    {
        err_code = 4;
        return -1;   // should accept only multiplicity 1
    }

    // at this point, we can save temporary values of x, y
    for(int ii = 0; ii < (int)valid.size(); ii++)
    {
        aData = theEvent[valid[ii]];

        mcp_x += (*aData)[ind_xm];
        mcp_y += (*aData)[ind_ym];
    }
    valid_evt = nvalid;

    return nvalid;
}
//*****************************************************************************
void TprismaMcp::process_event(std::vector<detData*>& theEvent)
{
    int run = 0;
    detData* aData = 0;

    // first, deform Mcp
    deform_mcp(theEvent);


    // calibration follows converting everything in mm
    for(int ii = 0; ii < (int)valid.size(); ii++)
    {
        aData = theEvent[valid[ii]];

        if(ii == 1) cout << " TprismaMcp::process_event     Two times ?" << endl;
        aData->set(ind_xm,      theCalManager[ind_xm]->getCalValue(run, aData->num_det(), (*aData)[ind_xm]) * mm);
        aData->set(ind_ym,  theCalManager[ind_ym]->getCalValue(run, aData->num_det(), (*aData)[ind_ym]) * mm);

    }


    if(theta_mcp > 0.)
        rotate_mcp(theta_mcp, theEvent);

    // at this point, we can save values of x, y
    mcp_x = 0.;
    mcp_y = 0.;
    for(int ii = 0; ii < (int)valid.size(); ii++)
    {
        aData = theEvent[valid[ii]];
        mcp_x += (*aData)[ind_xm];
        mcp_y += (*aData)[ind_ym];
    }
    xy_ok = true;

    spec_mcp_x_cal->manually_increment(mcp_x * 100);
    spec_mcp_y_cal->manually_increment(mcp_y * 100);
    spec_xy_cal->manually_increment(mcp_x * 100, mcp_y * 100);

    get_angles();
    processed_event = 1;
}
//*****************************************************************************
void TprismaMcp::deform_mcp(std::vector<detData*>& theEvent)
{
    int run = 0;
    detData* aData = 0;
    double tx, ty;

    for(int ii = 0; ii < (int)valid.size(); ii++)
    {
        aData = theEvent[valid[ii]];

        tx = (*aData)[ind_xm];
        ty = (*aData)[ind_ym];

        aData->set(ind_xm, (mcp_mix_x[0] * tx + mcp_mix_x[1] * ty));
        aData->set(ind_ym, (mcp_mix_y[0] * tx + mcp_mix_y[1] * ty));
    }
}
//*****************************************************************************
void TprismaMcp::rotate_mcp(double angle, std::vector<detData*>& theEvent)
{
    if(angle == 0.) return;

    int run = 0;
    detData* aData = 0;
    double tx, ty;

    double costheta = cos(angle);
    double sintheta = sin(angle);

    for(int ii = 0; ii < (int)valid.size(); ii++)
    {
        aData = theEvent[valid[ii]];

        tx = (*aData)[ind_xm];
        ty = (*aData)[ind_ym];

        aData->set(ind_xm, (costheta * tx + sintheta * ty));
        aData->set(ind_ym, (-sintheta * tx + costheta * ty));
    }
}
//*****************************************************************************
void TprismaMcp::get_angles()
{
    double   dx, dy, dz;
    Vector3D dummy;

    // CLARA sdr (left-handed):
    // z --> entering PRISMA
    // x --> vertical, pointing topwards
    // y --> horizontal, pointing to the left (looking at PRISMA)
    dx =  mcp_y;
    dy = -mcp_x * sin(mcp_angle);
    dz =  mcp_x * cos(mcp_angle) + mcp_target_d;
	
    dummy = Vector3D(dx, dy, dz);

    // angles in CLARA sdr
    mcp_theta_c = dummy.theta();
    mcp_phi_c   = dummy.phi();
//     if ( mcp_phi_c < 0. )
//         mcp_phi_c += 360.*degree;

    spec_phi_clara->manually_increment(mcp_phi_c  * RADIANS_TO_DEGREES);
    spec_theta_clara->manually_increment(mcp_theta_c  * RADIANS_TO_DEGREES);
    spec_theta_phi_c->manually_increment(mcp_theta_c  * RADIANS_TO_DEGREES,mcp_phi_c  * RADIANS_TO_DEGREES);


    //--------------------------------------------------------------
    // AGATA sdr (right-handed):
    // z --> entering PRISMA
    // x --> vertical, pointing downwards
    // y --> horizontal, pointing to the left (looking at PRISMA)
    dx = -mcp_y;
    dy = -mcp_x * sin(mcp_angle);
    dz =  mcp_x * cos(mcp_angle) + mcp_target_d;

    dummy = Vector3D(dx, dy, dz);

    // angles in AGATA sdr
    mcp_theta_a = dummy.theta();
    mcp_phi_a   = dummy.phi();
//    if(mcp_phi_a < 0.)
//        mcp_phi_a += 360.*degree;

    // transform vector to PRISMA reference (right-handed)
    // z--> beam direction
    // PRISMA dispersion plane --> (x,z)
    // x axis pointing in direction of increasing coordinates in the focal plane
    // ==> y axis vertical pointing downwards
    dx = -dummy.Y() * costh_p + dummy.Z() * sinth_p;
    dy =  dummy.X();
    dz =  dummy.Y() * sinth_p + dummy.Z() * costh_p;

    dummy = Vector3D(dx, dy, dz);

    // angles in BEAM sdr   // values are in radians
    mcp_theta_b = dummy.theta();
    mcp_phi_b   = dummy.phi();
  //   if ( mcp_phi_b < 0. )
  //       mcp_phi_b += 360.*degree;

    spec_phi_beam->manually_increment(mcp_phi_b * RADIANS_TO_DEGREES);
    spec_theta_beam->manually_increment(mcp_theta_b * RADIANS_TO_DEGREES);
    spec_theta_phi_beam->manually_increment(mcp_theta_b  * RADIANS_TO_DEGREES,mcp_phi_b  * RADIANS_TO_DEGREES);
    
    
    
        //--------------------------------------------------------------
    // LAB sdr (right-handed):
    // Following Daniele tesi (see page 158)
    double prisma_pos_angle = getPrismaAngle();
    double dist_target_mcp = mcp_target_d;
    double mcp_rot_angle = mcp_angle;
    
//    cout << mcp_x << '\t' << dist_target_mcp << '\t' << mcp_y << endl;
    
    
    			
    dx = dist_target_mcp*sin(prisma_pos_angle)+sin(PI/2.- mcp_rot_angle - prisma_pos_angle) * mcp_x;	
    dy = dist_target_mcp*cos(prisma_pos_angle)-cos(PI/2.- mcp_rot_angle - prisma_pos_angle) * mcp_x;
    dz = mcp_y;	
    
    double norm = sqrt(pow(dx,2)+pow(dy,2)+pow(dz,2));

    // angles in laboratory sdr
    mcp_theta_lab = acos(dy/norm);
    mcp_phi_lab   = atan(dx/dz);
    
         if ( mcp_phi_lab < 0. )
         mcp_phi_lab += 180.*degree;
    mcp_phi_lab = -mcp_phi_lab +90.*degree;
    spec_phi_lab->manually_increment(mcp_phi_lab * RADIANS_TO_DEGREES);
    spec_theta_lab->manually_increment(mcp_theta_lab * RADIANS_TO_DEGREES);
    spec_theta_phi_lab->manually_increment(mcp_theta_lab  * RADIANS_TO_DEGREES,mcp_phi_lab  * RADIANS_TO_DEGREES);
    
    angles_ok = true;
}
//*****************************************************************************
#endif // CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT


