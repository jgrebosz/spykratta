#include "TprismaManager.h"

#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT

#include "RandomGenerator.h"
#include "TIFJEvent.h"

#include "TprismaIonCh.h"
#include "Tnamed_pointer.h"
//*****************************************************************************
TprismaManager::TprismaManager(string name) : Tfrs_element(name)
{
    theFile = NULL;

//manager_ndet  = 4

// ind_mcp   = 0
// ind_ppac  = 1
// ind_ionch = 2
// ind_side  = 3

//  theFile = new ConfigurationFile( conf_file.c_str() );
    ndet_mcp   = 0;
    ndet_ppac  = 0;
    ndet_ionch = 0;
    ndet_side  = 0;

    npar_mcp   = 0;
    npar_ppac  = 0;
    npar_ionch = 0;
    npar_side  = 0;

    if(read_configuration() > 0)     // this is always, (acutally does not read the disc )
        init_data();                    // this is always, (acutally does not read the disc )

    create_my_spectra();
}
//***********************************************************************************************
TprismaManager::TprismaManager(string name, int nmcp, int pmcp, int nppac, int pppac, int nic, int pic, int nsi, int psi):
    Tfrs_element(name)
{
    theFile = NULL;
//   theFile = new ConfigurationFile( conf_file.c_str() );

    ndet_mcp   = nmcp;
    ndet_ppac  = nppac;
    ndet_ionch = nic;
    ndet_side  = nsi;

    npar_mcp   = pmcp;
    npar_ppac  = pppac;
    npar_ionch = pic;
    npar_side  = psi;

//   if(read_configuration() > 0 )
//     init_data();
	 


    create_my_spectra();
}
//**********************************************************************
TprismaManager::~TprismaManager()
{
    delete theFile ; // jg
    delete [] lookup;

    delete  theDetectors[ind_mcp];
    delete  theDetectors[ind_ppac];
    delete   theDetectors[ind_ionch];
    delete theDetectors[ind_side];
    delete  theSolver ;
    delete  theZedCalculator;
    delete  theMassCalculator ;
}
//**************************************************************************************
void TprismaManager::create_my_spectra()
{
    string folder = "TprismaManager/";
    //--------------------
    string  name = name_of_this_element + "__a_over_q__vs__x_fp" ;
    spec_aoq_vs_x_fp  =
        new spectrum_2D(name, name,
                        1000, 0, 1,
                        1000, 200, 600,
                        folder,
                        "",
                        "X: " +
                        ((TprismaPPAC*)theDetectors[ind_ppac])->give_name() +
                        "__x_fp_in_meters_when_valid "
                        + "   Y:  " +
                        "__a_over_q_when_valid (uncal?)"    //     a_over_q_uncal())
                       );
    frs_spectra_ptr->push_back(spec_aoq_vs_x_fp) ;


    name = name_of_this_element + "__ic_E_energy" ;
    spec_ic_energy =
        new spectrum_1D(name, name,
                        1000, 0, 8000,
                        folder,
                        "Has already conditions on mcp",
                        "X: " +
                        ((TprismaPPAC*)theDetectors[ind_ppac])->give_name() +
                        "__ic_energy_when_valid "
                       );
    frs_spectra_ptr->push_back(spec_ic_energy) ;
    //--------------------


    name = name_of_this_element + "__beta" ;
    spec_beta =
        new spectrum_1D(name, name,
                        3000, 0, 0.3,
                        folder,
                        "v/c",
                        "X: _beta ..."
                       );
    frs_spectra_ptr->push_back(spec_beta) ;


    // sp[ectrum gated by TOF bec it is taken form the trajectory calculator
    name = name_of_this_element + "__total_dEa_vs_E_tof" ;
    spec_dE_vs_E_tof  =
        new spectrum_2D(name, name,
                        950, 0, 8000,
                        950, 0, 8000,
                        folder,
                        "",
                        "X:  " + name_of_this_element + "__total_E_cal_when_valid"
                        "   Y:  " + name_of_this_element + "__total_dEa_cal_when_valid"); // incrementor
    frs_spectra_ptr->push_back(spec_dE_vs_E_tof) ;

//     named_pointer[name_of_this_element + "__TKEL_total_kinetic_energy_loss"]
//     = Tnamed_pointer(&total_kinetic_energy_loss, &total_kinetic_energy_loss_ok, this) ;
}
//**********************************************************************

//**********************************************************************
void TprismaManager::init_data()
{
    theDetectors.resize(n_det);
    theEvent.resize(n_det);

    // init pointers to actual detectors
    theDetectors[ind_mcp]   = new TprismaMcp("mcp", ndet_mcp,   npar_mcp);
    theDetectors[ind_ppac]  = new TprismaPPAC("ppac", ndet_ppac,  npar_ppac);
    theDetectors[ind_ionch] = new TprismaIonCh("ionch", ndet_ionch, npar_ionch);
    theDetectors[ind_side]  = new TprismaSide("side", ndet_side,  npar_side);

    theSolver = new TfastSolver("trajectory");

    theZedCalculator  = new TzedCalculator("zedc");
    theMassCalculator = new TmassCalculator("mass", this);


//   valid_conf = 1;
//   for(int ii=0; ii<4; ii++ )
//   {
//     valid_conf   *=    theDetectors[lookup[ii]]->valid_configuration();
//   }
//   if( valid_conf < 1 )
//     cout << " Warning! Problem found in constructing the detectors ..." << endl;
}
//***********************************************************************************************
int TprismaManager::read_configuration()
{
    ////////////////////////////////////////////////////
    //// Basically only configuration files are read
    ////////////////////////////////////////////////////
//   mcp_conf    = string( theFile->get_string("mcp_conf",    "mcp.conf") );
//   ppac_conf   = string( theFile->get_string("ppac_conf",   "ppac.conf") );
//   ionch_conf  = string( theFile->get_string("ionch_conf",  "ionch.conf") );
//   side_conf   = string( theFile->get_string("side_conf",   "side.conf") );
//   solver_conf = string( theFile->get_string("solver_conf", "solver.conf") );
//   zed_conf    = string( theFile->get_string("zed_conf",    "zed.conf") );
//   mass_conf   = string( theFile->get_string("mass_conf",   "mass.conf") );

    n_det       =  4 ;        //theFile->get_int( "manager_ndet", 4);

    if(n_det < 4)
    {
        cout << " Error! PRISMA should have at least 4 detectors." << endl;
        return -1;
    }

    // use a temporary index
    int indici[4];

    indici[0] = 0 ;       // theFile->get_int("ind_mcp",   0);
    indici[1] = 1;            // theFile->get_int("ind_ppac",  1);
    indici[2] = 2;            // theFile->get_int("ind_ionch", 2);
    indici[3] =  3;       //theFile->get_int("ind_side",  3);

    // should check for inconsistencies
    for(int ii = 0; ii < 4; ii++)
    {
        if(indici[ii] > (n_det - 1))
        {
            cout << " Error with definition of index " << indici[ii] << "(" << ii << ")" << endl;
            return -1;
        }
        for(int jj = 0; jj < ii; jj++)
        {
            if(indici[ii] == indici[jj])
            {
                cout << " Error! Duplicated indexes "  << indici[ii] << "(" << ii << "), " <<
                     indici[jj] << "(" << jj << ")" << endl;
            }
        }
    }
    // everything OK, can store in "real" params
    ind_mcp   = indici[0];
    ind_ppac  = indici[1];
    ind_ionch = indici[2];
    ind_side  = indici[3];

    lookup = new int[4];
    lookup[0] = ind_mcp;
    lookup[1] = ind_ppac;
    lookup[2] = ind_ionch;
    lookup[3] = ind_side;

    // number of elements/parameters for the required detectors
    // read from file only if parameters have not been passed
    // as arguments of the constructor
    if(ndet_mcp < 1)
        ndet_mcp   = 1;         // theFile->get_int("ndet_mcp",    1);
    if(ndet_ppac < 1)
        ndet_ppac  = 10;            // theFile->get_int("ndet_ppac",  10);
    if(ndet_ionch < 1)
        ndet_ionch = 10;            // theFile->get_int("ndet_ionch", 10);
    if(ndet_side < 1)
        ndet_side  = 2;         // theFile->get_int("ndet_side",   2);

    if(npar_mcp < 1)
        npar_mcp   =  3;            //theFile->get_int("npar_mcp",    3);
    if(npar_ppac < 1)
        npar_ppac  = 8;         // theFile->get_int("npar_ppac",   8);
    if(npar_ionch < 1)
        npar_ionch = 4;         // theFile->get_int("npar_ionch",  4);
    if(npar_side < 1)
        npar_side  =  4;            // theFile->get_int("npar_side",   4);

    return 1;
}
//***********************************************************************************************
////////////////////////////////////////////////////////////////////
/// new_event: resets flags, stores pointers to data structures
////////////////////////////////////////////////////////////////////
void TprismaManager::new_event(std::vector<detData*>& mcp_data, std::vector<detData*>& ppac_data,
                               std::vector<detData*>& ionch_data, std::vector<detData*>& side_data)
{
    // resets solver
    theSolver->not_solved();

    // saves pointers
    theEvent[ind_mcp  ] = &mcp_data;
    theEvent[ind_ppac ] = &ppac_data;
    theEvent[ind_ionch] = &ionch_data;
    theEvent[ind_side ] = &side_data;


    // Look at the contents of ppac

//   cout << "In prisma manager_" << endl;
//   for(int i = 0 ; i < ppac_data.size() ; i++)
//   {
//  cout << i << "] ---> " <<  *(ppac_data[i]) << endl;
//   }

    for(int ii = 0; ii < 4; ii++)
    {
        theDetectors[lookup[ii]]->validate_event(*(theEvent[lookup[ii]]));
    }
    // valid event: Side should not fire
    valid_evt = theDetectors[ind_side]->valid_event();
}
//***********************************************************************************************
///////////////////////////////////////////////////////////////////////
/////// Methods to extract the relevant information from PRISMA
//////  detectors
///////////////////////////////////////////////////////////////////////
double TprismaManager::mcp_x()
{
    // should only process "good" events
    // for MCP we don't consider side!
    if(theDetectors[ind_mcp]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_mcp]->has_processed() < 1)
        theDetectors[ind_mcp]->process_event(*(theEvent[ind_mcp]));

    return ((TprismaMcp*)theDetectors[ind_mcp])->get_x_mcp();
}
//***********************************************************************************************
double TprismaManager::mcp_y()
{
    // should only process "good" events
    // for MCP we don't consider side!
    if(theDetectors[ind_mcp]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_mcp]->has_processed() < 1)
        theDetectors[ind_mcp]->process_event(*(theEvent[ind_mcp]));

    return ((TprismaMcp*)theDetectors[ind_mcp])->get_y_mcp();
}
//***********************************************************************************************
double TprismaManager::theta_c()
{
    // should only process "good" events
    // for MCP we don't consider side!
    if(theDetectors[ind_mcp]->valid_event() < 1)
        return -1000. * degree;

    if(theDetectors[ind_mcp]->has_processed() < 1)
        theDetectors[ind_mcp]->process_event(*(theEvent[ind_mcp]));

    return ((TprismaMcp*)theDetectors[ind_mcp])->get_theta_c();
}
//***********************************************************************************************
double TprismaManager::theta_b()
{
    // should only process "good" events
    // for MCP we don't consider side!
    if(theDetectors[ind_mcp]->valid_event() < 1)
        return -1000. * degree;

    if(theDetectors[ind_mcp]->has_processed() < 1)
        theDetectors[ind_mcp]->process_event(*(theEvent[ind_mcp]));

    return ((TprismaMcp*)theDetectors[ind_mcp])->get_theta_b();
}
//***********************************************************************************************
double TprismaManager::theta_a()
{
    // should only process "good" events
    // for MCP we don't consider side!
    if(theDetectors[ind_mcp]->valid_event() < 1)
        return -1000. * degree;

    if(theDetectors[ind_mcp]->has_processed() < 1)
        theDetectors[ind_mcp]->process_event(*(theEvent[ind_mcp]));

    return ((TprismaMcp*)theDetectors[ind_mcp])->get_theta_a();
}
//***********************************************************************************************
double TprismaManager::phi_c()
{
    // should only process "good" events
    // for MCP we don't consider side!
    if(theDetectors[ind_mcp]->valid_event() < 1)
        return -1000. * degree;

    if(theDetectors[ind_mcp]->has_processed() < 1)
        theDetectors[ind_mcp]->process_event(*(theEvent[ind_mcp]));

    return ((TprismaMcp*)theDetectors[ind_mcp])->get_phi_c();
}
//***********************************************************************************************
double TprismaManager::phi_b()
{
    // should only process "good" events
    // for MCP we don't consider side!
    if(theDetectors[ind_mcp]->valid_event() < 1)
        return -1000. * degree;

    if(theDetectors[ind_mcp]->has_processed() < 1)
        theDetectors[ind_mcp]->process_event(*(theEvent[ind_mcp]));

    return ((TprismaMcp*)theDetectors[ind_mcp])->get_phi_b();
}
//***********************************************************************************************
double TprismaManager::phi_a()
{
    // should only process "good" events
    // for MCP we don't consider side!
    if(theDetectors[ind_mcp]->valid_event() < 1)
        return -1000. * degree;

    if(theDetectors[ind_mcp]->has_processed() < 1)
        theDetectors[ind_mcp]->process_event(*(theEvent[ind_mcp]));

    return ((TprismaMcp*)theDetectors[ind_mcp])->get_phi_a();
}
//***********************************************************************************************
double TprismaManager::tof()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * ns;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_tof();
}
//***********************************************************************************************
// beta (not in %!)
double TprismaManager::tof_zero()
{
    if(tof() > 0.)
        return tof() * sqrt(1. - beta() * beta());
    else
        return -1.0;
}

//***********************************************************************************************
double TprismaManager::x_fp()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

//     double x = ((TprismaPPAC*)theDetectors[ind_ppac])->get_xfp() ;
//     if(x>0.01) cout << "in TprismaManager::x_fp() = " << x << endl;
    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_xfp();
}
//***********************************************************************************************
double TprismaManager::length()
{
    // should only process "good" events (no side)
    if(valid_evt < 1)
        return 0.;

    // ingredients to calculate trajectory should be good ones!
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    return theSolver->getLength();
}
//***********************************************************************************************
double TprismaManager::radius()
{
    // should only process "good" events (no side)
    if(valid_evt < 1)
        return 0.;

    // ingredients to calculate trajectory should be good ones!
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    return theSolver->getRadius();
}
//***********************************************************************************************

double TprismaManager::ic_energy()
{
    // should only process "good" events
    if(valid_evt < 1)
        return 0.;
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    // calibrations!
    if(theDetectors[ind_ionch]->valid_event() < 1)
        return 0.;

    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));

    // preliminary: trajectory reconstruction
    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    // now, validate trajectory through the ionization chamber
    int indici[4];

    if(theSolver->solved_chamber() < 1)
    {
        // indexes
        for(int ii = 0; ii < 4; ii++)
            indici[ii] = ((TprismaIonCh*)theDetectors[ind_ionch])->get_ind(ii);
        // finally, get results!
        theSolver->FindChamber(indici, *(theEvent[ind_ionch]));
    }
    return theSolver->getICEner();
}
//***********************************************************************************************
double TprismaManager::ic_energy_AB_DE()
{
    // should only process "good" events
    if(valid_evt < 1)
        return 0.;
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    // calibrations!
    if(theDetectors[ind_ionch]->valid_event() < 1)
        return 0.;

    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));

    // preliminary: trajectory reconstruction
    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    // now, validate trajectory through the ionization chamber
    int indici[4];

    if(theSolver->solved_chamber() < 1)
    {
        // indexes
        for(int ii = 0; ii < 4; ii++)
            indici[ii] = ((TprismaIonCh*)theDetectors[ind_ionch])->get_ind(ii);
        // finally, get results!
        theSolver->FindChamber(indici, *(theEvent[ind_ionch]));
    }
    return theSolver->getIC_ABE();
}
//***********************************************************************************************
double TprismaManager::ic_energy_A_DE()
{
    // should only process "good" events
    if(valid_evt < 1)
        return 0.;
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    // calibrations!
    if(theDetectors[ind_ionch]->valid_event() < 1)
        return 0.;

    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));

    // preliminary: trajectory reconstruction
    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    // now, validate trajectory through the ionization chamber
    int indici[4];

    if(theSolver->solved_chamber() < 1)
    {
        // indexes
        for(int ii = 0; ii < 4; ii++)
            indici[ii] = ((TprismaIonCh*)theDetectors[ind_ionch])->get_ind(ii);
        // finally, get results!
        theSolver->FindChamber(indici, *(theEvent[ind_ionch]));
    }
    return theSolver->getIC_ADE();
}
//***********************************************************************************************
double TprismaManager::path()
{
    // should only process "good" events
    if(valid_evt < 1)
        return 0.;
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    // calibrations!
    if(theDetectors[ind_ionch]->valid_event() < 1)
        return 0.;

    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));

    // preliminary: trajectory reconstruction
    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    // now, validate trajectory through the ionization chamber
    int indici[4];

    if(theSolver->solved_chamber() < 1)
    {
        // indexes
        for(int ii = 0; ii < 4; ii++)
            indici[ii] = ((TprismaIonCh*)theDetectors[ind_ionch])->get_ind(ii);
        // finally, get results!
        theSolver->FindChamber(indici, *(theEvent[ind_ionch]));
    }
    return theSolver->getICPath();
}
//***********************************************************************************************
double TprismaManager::range()
{
    // should only process "good" events
    if(valid_evt < 1)
        return 0.;
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    // calibrations!
    if(theDetectors[ind_ionch]->valid_event() < 1)
        return 0.;

    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));

    // preliminary: trajectory reconstruction
    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    // now, validate trajectory through the ionization chamber
    int indici[4];

    if(theSolver->solved_chamber() < 1)
    {
        // indexes
        for(int ii = 0; ii < 4; ii++)
            indici[ii] = ((TprismaIonCh*)theDetectors[ind_ionch])->get_ind(ii);
        // finally, get results!
        theSolver->FindChamber(indici, *(theEvent[ind_ionch]));
    }
    return theSolver->getICRang();
}
//***********************************************************************************************
// beta (not in %!)
double TprismaManager::beta()
{
    if((tof() > 0.) && (length() > 0.))
    {
        const double c_light = 29.97925 * cm / ns;
        return length() / (c_light * tof());
    }
    else
        return -1.0;
}
//***********************************************************************************************
double TprismaManager::gamma()
{
    double bb = beta();
    if(bb < 1.)
        return 1./sqrt(1.-bb*bb);
    else
        return -1.;
}
//***********************************************************************************************
// beta gamma = beta / sqrt( 1 - beta^2 )
double TprismaManager::betagamma()
{
    double bb = beta();
    double gg = gamma();
    if(bb < 1.0)
        return bb * gg;
    else
        return -1.0;
}
//***********************************************************************************************
// velocity (AGATA sdr)
Vector3D TprismaManager::velocity_a()
{
    if((tof() > 0.) && (length() >= 0.))
    {
        double theta = theta_a();
        double phi   = phi_a();
        const double c_light = 29.97925 * cm / ns;

        Vector3D dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

        return dir * beta() * c_light;
    }
    else
        return Vector3D(0., 0., 0.);
}
//***********************************************************************************************
// velocity (CLARA sdr)
Vector3D TprismaManager::velocity_c()
{
    if((tof() > 0.) && (length() >= 0.))
    {
        double theta = theta_c();
        double phi   = phi_c();
        const double c_light = 29.97925 * cm / ns;

        Vector3D dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

        return dir * beta() * c_light;
    }
    else
        return Vector3D(0., 0., 0.);
}
//***********************************************************************************************
// velocity (beam sdr)
Vector3D TprismaManager::velocity_b()
{
    if((tof() > 0.) && (length() >= 0.))
    {
        double theta = theta_b();
        double phi   = phi_b();
        const double c_light = 29.97925 * cm / ns;

        Vector3D dir(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));

        return dir * beta() * c_light;
    }
    else
        return Vector3D(0., 0., 0.);
}
//***********************************************************************************************
// velocity
Vector3D TprismaManager::velocity()
{
    return velocity_a();
}
//***********************************************************************************************
double TprismaManager::old_ic_energy()
{
    // should only process "good" events
    if(valid_evt < 1)
        return -1000. * keV;
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    // calibrations!
    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));

    // preliminary: trajectory reconstruction
    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    // now, validate trajectory through the ionization chamber
    int*    dum_ind;
    double* dum_icdata;
    int ndet, npar;
    int pos1 = 0, pos2 = 0;

    if(theSolver->solved_chamber() < 1)
    {
        // how many detectors/parameters?
        ndet = (*(theEvent[ind_ionch])).size();
        npar = -1;
        for(int ii = 0; ii < ndet; ii++)
        {
            if((*(theEvent[ind_ionch]))[ii]->is_valid())
                if((*(theEvent[ind_ionch]))[ii]->num_param() > npar)
                    npar = (*(theEvent[ind_ionch]))[ii]->num_param();
        }

        // prepare temporary array
        dum_ind    = new int[ndet];
        dum_icdata = new double[ndet * npar];

        // store values in temporary arrays
        for(int ii = 0; ii < ndet; ii++)
        {
            if((*(theEvent[ind_ionch]))[ii]->is_valid())
            {
                dum_ind[pos1++] = (*(theEvent[ind_ionch]))[ii]->num_det();
                for(int jj = 0; jj < npar; jj++)
                {
                    dum_icdata[pos2++] = (*(theEvent[ind_ionch]))[ii]->get(jj);
                }
            }
        }
        // finally, get results!
        theSolver->FindChamber(ndet, dum_ind, dum_icdata);
    }
    return theSolver->getICEner();
}
//***********************************************************************************************
double TprismaManager::old_path()
{
    // should only process "good" events
    if(valid_evt < 1)
        return -1000. * mm;
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    // calibrations!
    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));

    // preliminary: trajectory reconstruction
    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    // now, validate trajectory through the ionization chamber
    int*    dum_ind;
    double* dum_icdata;
    int ndet, npar;
    int pos1 = 0, pos2 = 0;

    if(theSolver->solved_chamber() < 1)
    {
        // how many detectors/parameters?
        ndet = (*(theEvent[ind_ionch])).size();
        npar = -1;
        for(int ii = 0; ii < ndet; ii++)
        {
            if((*(theEvent[ind_ionch]))[ii]->is_valid())
                if((*(theEvent[ind_ionch]))[ii]->num_param() > npar)
                    npar = (*(theEvent[ind_ionch]))[ii]->num_param();
        }

        // prepare temporary array
        dum_ind    = new int[ndet];
        dum_icdata = new double[ndet * npar];

        // store values in temporary arrays
        for(int ii = 0; ii < ndet; ii++)
        {
            if((*(theEvent[ind_ionch]))[ii]->is_valid())
            {
                dum_ind[pos1++] = (*(theEvent[ind_ionch]))[ii]->num_det();
                for(int jj = 0; jj < npar; jj++)
                {
                    dum_icdata[pos2++] = (*(theEvent[ind_ionch]))[ii]->get(jj);
                }
            }
        }
        // finally, get results!
        theSolver->FindChamber(ndet, dum_ind, dum_icdata);
    }
    return theSolver->getICPath();
}
//***********************************************************************************************
double TprismaManager::old_range()
{
    // should only process "good" events
    if(valid_evt < 1)
        return -1000. * mm;
    if(mcp_x() < -900.*mm)
        return 0.;
    if(mcp_y() < -900.*mm)
        return 0.;
    if(x_fp() < -900.*mm)
        return 0.;
    if(x_fp() > 1100.*mm)
        return 0.;

    // calibrations!
    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));

    // preliminary: trajectory reconstruction
    if(theSolver->has_solved() < 1)
        theSolver->findTrajectory(mcp_x(), mcp_y(), x_fp());

    // now, validate trajectory through the ionization chamber
    int*    dum_ind;
    double* dum_icdata;
    int ndet, npar;
    int pos1 = 0, pos2 = 0;

    if(theSolver->solved_chamber() < 1)
    {
        // how many detectors/parameters?
        ndet = (*(theEvent[ind_ionch])).size();
        npar = -1;
        for(int ii = 0; ii < ndet; ii++)
        {
            if((*(theEvent[ind_ionch]))[ii]->is_valid())
                if((*(theEvent[ind_ionch]))[ii]->num_param() > npar)
                    npar = (*(theEvent[ind_ionch]))[ii]->num_param();
        }

        // prepare temporary array
        dum_ind    = new int[ndet];
        dum_icdata = new double[ndet * npar];

        // store values in temporary arrays
        for(int ii = 0; ii < ndet; ii++)
        {
            if((*(theEvent[ind_ionch]))[ii]->is_valid())
            {
                dum_ind[pos1++] = (*(theEvent[ind_ionch]))[ii]->num_det();
                for(int jj = 0; jj < npar; jj++)
                {
                    dum_icdata[pos2++] = (*(theEvent[ind_ionch]))[ii]->get(jj);
                }
            }
        }
        // finally, get results!
        theSolver->FindChamber(ndet, dum_ind, dum_icdata);
    }
    return theSolver->getICRang();
}
//***********************************************************************************************
int TprismaManager::zed_deduced_from_pathE()
{
    //cout << "Zed_pathE " << endl;
    if(flag_zed_pathE_already_done == false)
    {
        zed_from_path = theZedCalculator->deduce_zed_from_bananas_on_matrix_E_vs_path(path(), ic_energy());
        flag_zed_pathE_already_done = true;
    }
    return zed_from_path;
}
//***********************************************************************************************
int TprismaManager::zed_deduced_from_EdE()
{
    //cout << "Zed_EdE " << endl;

    if(flag_zed_EdE_already_done == false)
    {
        zed_from_EdE = theZedCalculator->deduce_zed_from_bananas_on_matrix_dE_vs_E(ic_energy_AB_DE(), ic_energy());
        flag_zed_EdE_already_done = true;
    }
    return zed_from_EdE;
}
//***********************************************************************************************

// double TprismaManager::Mass_pathE()
// {
//
//     //return theMassCalculator->calculate_mass_from_a_over_q(zed_deduced_from_pathE(), ic_energy(), radius(), betagamma());
//     return theMassCalculator->give_mass_from_pathE();
// }
//***********************************************************************************************
// double TprismaManager::Mass_EdE()
// {
//     return theMassCalculator->calculate_mass_from_a_over_q(zed_deduced_from_EdE(), ic_energy(), radius(), betagamma());
// }
//***********************************************************************************************
int TprismaManager::mass_number()
{
    return theMassCalculator->give_mass_number();
}
//***********************************************************************************************
// int TprismaManager::mass_number_EdE()
// {
//     return theMassCalculator->mass_number(zed_deduced_from_EdE(), ic_energy(), radius(), betagamma(), x_fp());
// }
//***********************************************************************************************
double TprismaManager::a_over_q_uncal()
{
    return theMassCalculator->a_over_q_uncal(zed_deduced_from_pathE(), radius(), betagamma());
    //  return theMassCalculator->a_over_q_uncal( Zed(), radius(), beta() );
}
//***********************************************************************************************
double TprismaManager::a_over_q_uncal1()
{
    return theMassCalculator->a_over_q_uncal(zed_deduced_from_EdE(), radius(), betagamma());
    //  return theMassCalculator->a_over_q_uncal( Zed(), radius(), beta() );
}
//***********************************************************************************************
double TprismaManager::r_beta()
{
    return theMassCalculator->r_beta(zed_deduced_from_pathE(), radius(), betagamma());
}
//***********************************************************************************************
double TprismaManager::r_beta1()
{
    return theMassCalculator->r_beta(zed_deduced_from_EdE(), radius(), betagamma());
}
//***********************************************************************************************
// double TprismaManager::charge_state()
// {
//     return theMassCalculator-> deduce_charge_state(zed_deduced_from_pathE(), ic_energy(), radius(), betagamma());
// }
//***********************************************************************************************
// double TprismaManager::charge_state1()
// {
//     return theMassCalculator-> deduce_charge_state(zed_deduced_from_EdE(), ic_energy(), radius(), betagamma());
// }
//***********************************************************************************************
double TprismaManager::length_over_radius()
{
    if(radius() > 0.)
        return 1000.*length() / radius();
    else
        return -1.;
}
//***********************************************************************************************
double TprismaManager::energy_over_beta()
{
    if(beta() > 0.)
        return 0.005 * ic_energy() / (beta() * beta());
    else
        return -1.;
}
//***********************************************************************************************
int TprismaManager::mcp_err_code()
{
    return theDetectors[ind_mcp]->get_err_code();
}
//***********************************************************************************************
int TprismaManager::ppac_err_code()
{
    return theDetectors[ind_ppac]->get_err_code();
}
//***********************************************************************************************
int TprismaManager::ionch_err_code()
{
    return theDetectors[ind_ionch]->get_err_code();
}
//***********************************************************************************************
int TprismaManager::side_err_code()
{
    return theDetectors[ind_side]->get_err_code();
}
//***********************************************************************************************
/// partial PPAC information
int TprismaManager::get_ppac_section()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_section();
}
//***********************************************************************************************
double TprismaManager::get_xfp_lc()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_xfp_lc();
}
//***********************************************************************************************
double TprismaManager::get_xfp_rc()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_xfp_rc();
}
//***********************************************************************************************
double TprismaManager::get_xfp_xc()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_xfp_cathode();
}
//***********************************************************************************************
double TprismaManager::get_xfp_lu()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_xfp_lu();
}
//***********************************************************************************************
double TprismaManager::get_xfp_ru()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_xfp_ru();
}
//***********************************************************************************************
double TprismaManager::get_xfp_xu()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_xfp_xu();
}
//***********************************************************************************************
double TprismaManager::get_tof_xu()
{
    // should only process "good" events
    if(theDetectors[ind_ppac]->valid_event() < 1)
        return -1000. * mm;

    if(theDetectors[ind_ppac]->has_processed() < 1)
        theDetectors[ind_ppac]->process_event(*(theEvent[ind_ppac]));

    return ((TprismaPPAC*)theDetectors[ind_ppac])->get_tof_xu();
}
//***********************************************************************************************
double TprismaManager::get_calibrated_ic(int det, int sec)
{
    if(theDetectors[ind_ionch]->has_processed() < 1)
        theDetectors[ind_ionch]->process_event(*(theEvent[ind_ionch]));
    return ((TprismaIonCh*)theDetectors[ind_ionch])->get_calibrated_ic(det, sec);
}
//***********************************************************************************************
double TprismaManager::get_uncalibrated_ic(int det, int sec)
{
    return ((TprismaIonCh*)theDetectors[ind_ionch])->get_uncalibrated_ic(det, sec);
}
//***********************************************************************************************
#ifdef ABERRATIONS___bad_algorithm
double TprismaManager::Mass_aberrations()
{
    return theMassCalculator->Mass(Zed(), ic_energy(), radius(), betagamma(), mcp_x(), mcp_y(), x_fp());
}
//***********************************************************************************************
double TprismaManager::Mass1_aberrations()
{
    return theMassCalculator->Mass(Zed1(), ic_energy(), radius(), betagamma(), mcp_x(), mcp_y(), x_fp());
}
//***********************************************************************************************
int TprismaManager::mass_number_aberrations()
{
    return theMassCalculator->mass_number(Zed(), ic_energy(), radius(), betagamma(), x_fp(), mcp_x(), mcp_y());
}
//***********************************************************************************************
int TprismaManager::mass_number1_aberrations()
{
    return theMassCalculator->mass_number(Zed1(), ic_energy(), radius(), betagamma(), x_fp(), mcp_x(), mcp_y());
}
//***********************************************************************************************
double TprismaManager::Mass_Xmc(int zed, double value)
{
    return theMassCalculator->Correct_Xmc(zed, value);
}
//***********************************************************************************************
double TprismaManager::Mass_Ymc(int zed, double value)
{
    return theMassCalculator->Correct_Ymc(zed, value);
}
//***********************************************************************************************
double TprismaManager::Mass_Xfp(int zed, double value)
{
    return theMassCalculator->Correct_Xfp(zed, value);
}
#endif



//***********************************************************************************************
Vector3D TprismaManager::velocity_partner_a()
{
    const double amu = 931.5 * MeV;
    const double c_light = 29.97925 * cm / ns;

    Vector3D beam_momentum = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_momentum_a();
    Vector3D measured_momentum = amu * mass_number() * velocity_a();

    Vector3D partner_momentum = beam_momentum - measured_momentum;
    int partner_mass_number = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_mass_number() - mass_number();

    return partner_momentum / (c_light * amu * partner_mass_number);
}
#if 0   // not yet 
//***********************************************************************************************
Vector3D TprismaManager::velocity_partner_a1()
{
    const double amu = 931.5 * MeV;
    const double c_light = 29.97925 * cm / ns;

    Vector3D beam_momentum = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_momentum_a();
    Vector3D measured_momentum = amu * mass_number_EdE() * velocity_a();

    Vector3D partner_momentum = beam_momentum - measured_momentum;
    int partner_mass_number = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_mass_number() - mass_number_EdE();

    return partner_momentum / (c_light * amu * partner_mass_number);
}
#endif
//***********************************************************************************************
Vector3D TprismaManager::velocity_partner_b()
{
//     cout << "TprismaManager::velocity_partner_b__using_pathE()" << endl;

    cout << "TprismaManager::velocity_partner_b  do not call this function" << endl;

    double amu = 931.5 * MeV;
    const double c_light = 29.97925 * cm / ns;
    Vector3D beam_momentum = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_momentum_b();
    Vector3D velocity_measured = velocity_b() / 10.0;
//     cout << " veloc= " << veloc
//     << " veloc.abs() = " << abs(veloc) << endl;

    int mass_nr_measured = mass_number();

    /*    Vector3D measured_momentum = amu * mass_nr * veloc / c_light;   // [MeV/c]

        //Vector3D measured_momentum = amu * mass_number() * velocity_b();

        Vector3D partner_momentum = beam_momentum - measured_momentum;

      */
    int target_mass_number = ((TprismaMcp*)theDetectors[ind_mcp])->get_target_mass_number();
    int partner_mass_number = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_mass_number()
                              + target_mass_number -  mass_nr_measured;


    /*
        double partner_velocity = sqrt((partner_momentum * partner_momentum))  /
            (partner_mass_number * amu) ;
    // as Danielle asked

        double kinetic_g_upper =
            ((measured_momentum * measured_momentum) / (2.0 * amu * mass_nr));


        double kinetic_g_down =

            ((partner_momentum * partner_momentum) / (2 * partner_mass_number * amu) );

    //     cout
    //             << " (measured_momentum * measured_momentum) = " << (measured_momentum * measured_momentum)
    //             << " sqrt(measured_momentum * measured_momentum) = " << sqrt(measured_momentum * measured_momentum)
    //
    //             << "\nsqrt(veloc * veloc) = " << sqrt(veloc * veloc)
    //             << "   (2.0 * amu * mass_nr) = " << (2.0 * amu * mass_nr)
    //
    //             << "\n kinetic_g_upper = " << kinetic_g_upper
    //
    //             << "\nmass_number()=" << mass_number()
    //             << "\npartner_mass_number="<< partner_mass_number
    //             << "\npartner_mass_number * amu=" <<  partner_mass_number * amu
    //             << "\n kinetic_g_down = " << kinetic_g_down
    //             << "\npartner_velocity = " << partner_velocity
    //             << endl;

                    double kinetic_g = kinetic_g_upper + kinetic_g_down ;


        double kinetic_i =
            ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_energy() ;

        q_value = kinetic_i - kinetic_g ;
     */
//     cout << "       kinetic_i = " << kinetic_i
//          << ", kinetic_g= " << kinetic_g
//          << ", q_value = " << q_value
//          << endl;
//--------------------------------

    static double prepared = 0.5 * 1.672E-27 *(1.0/1.602e-19) * (1e-6) * 1e14 ;

    double measured_energy = mass_nr_measured * pow((abs(velocity_measured)), 2)  * prepared;

// cout << "E meas = " << E_measured << endl;

    double measured_momentum = sqrt(2. * measured_energy / (mass_nr_measured * amu));

    double theta = theta_a();
    double phi   = phi_a();

    Vector3D direction(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
    Vector3D measured_momentum_vector = measured_momentum * direction;
    Vector3D partner_momentum =     beam_momentum - measured_momentum_vector;
    double partner_energy = pow(abs(partner_momentum), 2) * partner_mass_number * amu / 2 ;
//total_kinetic_energy_loss = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_energy() - measured_energy - partner_energy ;

    double mass_beam = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_mass_number() * amu;
    double mass_target = target_mass_number * amu;
    double light_product_mass = mass_nr_measured * amu;

    double beam_energy = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_energy();

    total_kinetic_energy_loss = 1./(mass_beam+mass_target-light_product_mass) *
                                ((light_product_mass-mass_target)*beam_energy
                                 +(mass_beam+mass_target)*measured_energy
                                 - 2*sqrt(mass_beam*light_product_mass*measured_energy*beam_energy)*cos(theta));


    // Dcout <<  "Qreac = " <<  total_kinetic_energy_loss << '\t' << beam_energy << endl;


    total_kinetic_energy_loss_ok = true;
//cout << " total_kinetic_energy_loss = " << total_kinetic_energy_loss << endl;
// cout << "E measured_momentum2 = " << measured_momentum2
// << "\n measured_momentum2vec " << measured_momentum2vec
// << "  abs(measured_momentum2vec)= " << abs(measured_momentum2vec)
// << "\n partner_momentum2 = " << partner_momentum2 << " abs = " << abs(partner_momentum2)
// << "\n partner_energy = " << partner_energy
// << "\n q_value " << q_value
// << endl;

    return partner_momentum ;
}
//***************************************************************************************

#if 0
//***********************************************************************************************
Vector3D TprismaManager::velocity_partner_b1__using_EdE()
{
    double amu = 931.5 * MeV;
    const double c_light = 29.97925 * cm / ns;   // 299,792,458

    Vector3D beam_momentum = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_momentum_b();
    Vector3D measured_momentum = amu * mass_number_EdE() * velocity_b();

    Vector3D partner_momentum = beam_momentum - measured_momentum;
    int partner_mass_number = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_mass_number() - mass_number_EdE();

    return partner_momentum / (c_light * amu * partner_mass_number);
}
#endif
//***********************************************************************************************
Vector3D TprismaManager::velocity_partner_c()
{
    double amu = 931.5 * MeV;
    const double c_light = 29.97925 * cm / ns;

    Vector3D beam_momentum = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_momentum_c();
    Vector3D measured_momentum = amu * mass_number() * velocity_c();

    Vector3D partner_momentum = beam_momentum - measured_momentum;
    int partner_mass_number = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_mass_number() - mass_number();

    return partner_momentum / (c_light * amu * partner_mass_number);
}

#if 0
//***********************************************************************************************
Vector3D TprismaManager::velocity_partner_c1()
{
    double amu = 931.5 * MeV;
    const double c_light = 29.97925 * cm / ns;

    Vector3D beam_momentum = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_momentum_c();
    Vector3D measured_momentum = amu * mass_number_EdE() * velocity_c();

    Vector3D partner_momentum = beam_momentum - measured_momentum;
    int partner_mass_number = ((TprismaMcp*)theDetectors[ind_mcp])->get_beam_mass_number() - mass_number_EdE();

    return partner_momentum / (c_light * amu * partner_mass_number);
}
#endif

//***********************************************************************************************
Vector3D TprismaManager::velocity_partner()
{
    return velocity_partner_a();
}
#if 0
//***********************************************************************************************
Vector3D TprismaManager::velocity_partner1()
{
    return velocity_partner_a1();
}
#endif

//***********************************************************************************************
/** readinng calibration, gates, setting pointers */
void TprismaManager::make_preloop_action(ifstream & s)
{
    cout << " TprismaManager::make_preloop_action " << name_of_this_element << endl;


    try
    {
        threshold_for_general_raw_data =
            (int) Tfile_helper::find_in_file(s,  name_of_this_element  + "__threshold_for_general_raw_data");
    }
    catch(Tfile_helper_exception & x)
    {

        threshold_for_general_raw_data = 175 ; // defaut value

        cout << x.message
             << ",\n so the value "
             << threshold_for_general_raw_data
             << " is assumed " << endl;
        Tfile_helper::repair_the_stream(s);
    }


    for(int i = 0 ; i <  theDetectors.size() ; i++)
    {
        theDetectors[i]->make_preloop_action(s);
    }
    theSolver->make_preloop_action(s);
    theZedCalculator->make_preloop_action(s);
    theMassCalculator->make_preloop_action(s);


}
//********************************************************************************************
/** the most important fuction,  executed for every event */
void TprismaManager::analyse_current_event()         // return value: was Si hit or not ?
{
//     cout << " ===================== TprismaManager::analyse_current_event " << name_of_this_element << endl;
    flag_zed_pathE_already_done = false;
    flag_zed_EdE_already_done = false;
    total_kinetic_energy_loss = 0;
    total_kinetic_energy_loss_ok = false ;

    theZedCalculator->zeroing_variables();
    theMassCalculator->zeroing_variables();

    int numDet[4];
    int Counter[5] = { 0 };

    //#ifdef AF_PRISMA
    const int   pM = 0; //monitor
    const int   pA = 2; //MCP:x,y
    const int   pR = 6; //MWPPC:tof,xleft,xright,xcathode,y
    const int   pI = 4; //4 IC sections:A,B,C,D
    const int   pS = 4; //4 Side sections:A,B,C,D
    //#endif

    //=============================

    const int maxSections = 10;
    const double  c_light = 29.97925 * cm / ns;

#if 0

    memset(theMap, 0, sizeof(theMap));

    //  below I should generalize for a variable number of modules
    for(int ii = 0; ii < 3; ii++)
    {
        for(int jj = 0; jj < 32; jj++)
        {
            //cout << " LUT[" << ii << "]["<< jj<<"]= " <<  LUT[ii][jj] << endl;
            if(LUT[ii][jj] < 0) continue;
            theMap[ LUT[ii][jj] ] = Converter[ii].GetValue(jj);
            //if (ii==0 && jj>15 && jj<19)
            //  cout << "inside loop MCP " << theMap[ LUT[0][16] ] << "  Converter[0].GetValue(" << jj<< ") "  << Converter[ii].GetValue(jj) << endl;
        }
    }
#endif

    for(int zz = 0; zz < 4; zz++)
    {
        numDet[zz] = 0;
    }

    // below I count how many detectors have fired for each event
    //   for (int ii=60; ii<120; ii++)
    //   {
    //     cout << "ppac " <<ii-60 <<"= "<< event_unpacked ->thePrismaDataMap[ii] << endl;
    //   }

    int nParam[4][10] = {{0}};
    //memset(nParam, 0, sizeof(nParam));

    Counter[4]++;

    int tmp_thres = 250;
    for(int zz = 0; zz < 4; zz++)
    {
        if(zz == 3) // side detectors
        {
            tmp_thres = 250;
        }
        else tmp_thres = threshold_for_general_raw_data ; // 175;

        for(int jj = 0; jj < 10; jj++)
        {
            //nParam=0;
            for(int ii = 0; ii < 6; ii++)
            {
//                 if(zz == 3)
//                 {
//
//                     cout << event_unpacked ->thePrismaDataMap[60 * zz + 6 * jj + ii] << '\t';
//                 }
                if(event_unpacked ->thePrismaDataMap[60 * zz + 6 * jj + ii] > tmp_thres)
                {
                    nParam[zz][jj]++;
                }
            }
//             if(zz == 3)cout << "on row " << zz << " column " << jj
//                                 << ", nParam[zz][jj] = " << nParam[zz][jj]
//                                 << endl;
            //if (nParam) numDet[zz]++;
        }
        //if (numDet[zz]) Counter[zz]++;
    }

    for(int zz = 0; zz < 4; zz++)
    {
        for(int jj = 0; jj < 10; jj++)
        {
            if(nParam[zz][jj] > 0)
                numDet[zz]++;
        }
    }

    //cout << " N_ppac " << numDet[1] << endl;

    vector<detData*> theMcpData;
    theMcpData.clear();
    detData* aData;

    //for(int jj=0; jj<numDet[0]; jj++ ) {
    for(int jj = 0; jj < maxSections; jj++)
    {
        //index = theEvent->mcp( ii, 0 );
        if(nParam[0][jj] < 1) continue;
        theMcpData.push_back(new detData(jj, pA));
        aData = theMcpData.back();
        for(int ii = 0; ii < pA; ii++)
        {
            if(event_unpacked ->thePrismaDataMap[6 * jj + ii] > 0)
                aData->set(ii, event_unpacked ->thePrismaDataMap[6 * jj + ii] + Random::uniform() - 0.5);
        }
    }


    //   for(int jj=0; jj<numDet[0]; jj++ )
    //   {
    //     aData = theMcpData[jj];
    //     for(int ii=0; ii<pA; ii++ )
    //     {
    //       cout << "numDet= " <<aData->num_det() << " det_data= "<<aData->get(ii) << endl;
    //     }
    //   }
    //----------------------------
    vector<detData*> thePpacData;
    thePpacData.clear();

    //for(int jj=0; jj<numDet[1]; jj++ ) {
    for(int jj = 0; jj < maxSections; jj++)
    {
        if(nParam[1][jj] < 1) continue;
        thePpacData.push_back(new detData(jj, pR));
        aData = thePpacData.back();
        for(int ii = 0; ii < pR; ii++)
        {
            if(event_unpacked ->thePrismaDataMap[60 + 6 * jj + ii] > 0)
                aData->set(ii, event_unpacked ->thePrismaDataMap[60 + 6 * jj + ii] + Random::uniform() - 0.5);
        }
    }

    //for(int jj=0; jj<numDet[1]; jj++ ) {
    //  aData = thePpacData[jj];
    //  cout << "numm = " << numDet[1] << " " << nParam[1][aData->num_det()] << endl;
    //  for(int ii=0; ii<pR; ii++ )
    //    cout << "numDet= " <<aData->num_det() << " det_data= "<<aData->get(ii) << endl;
    //}
    //----------------------------
    vector<detData*> theIcData;
    theIcData.clear();

    //for(int jj=0; jj<numDet[2]; jj++ ) {
    for(int jj = 0; jj < maxSections; jj++)
    {

//         if(nParam[2][jj] < 1) continue;
        if(nParam[2][jj] < 1)
        {
            //cout << "nParam[2][" << jj << "] < 1, so continue "<< endl;
            continue;
        }
        theIcData.push_back(new detData(jj, pI));
        aData = theIcData.back();
        for(int ii = 0; ii < pI; ii++)
        {

//             cout << "Packing io event, sction " << jj << " ii= " << ii
//                  << " data = " << event_unpacked ->thePrismaDataMap[120 + 6 * jj + ii]
//                  << endl;
            if(event_unpacked ->thePrismaDataMap[120 + 6 * jj + ii] > 0)
                aData->set(ii, event_unpacked ->thePrismaDataMap[120 + 6 * jj + ii] + Random::uniform() - 0.5);
        }
    }
    //----------------------------
    vector<detData*> theSideData;
    theSideData.clear();

    //for( int jj=0; jj<numDet[3]; jj++ ) {
    for(int jj = 0; jj < maxSections; jj++)
    {
        //if(nParam[3][jj] < 1) continue;
        if(nParam[3][jj] < 1)
        {
//             cout << "Side   nParam[3][" << jj << "] < 1, so continue " << endl;
            continue;
        }
        else
        {
            //cout << "Side   nParam[3][" << jj << "] >= 1 " << endl;
        }


        theSideData.push_back(new detData(jj, pS));
        aData = theSideData.back();
        for(int ii = 0; ii < pS; ii++)
        {
            if(event_unpacked ->thePrismaDataMap[180 + 6 * jj + ii] > 0)
                aData->set(ii, event_unpacked ->thePrismaDataMap[180 + 6 * jj + ii] + Random::uniform() - 0.5);
        }
    }


    //   cout << " Testing new event " << endl;
    /*  for(int ii=0; ii<(int)theMcpData.size(); ii++ )
    {
    aData = theMcpData[ii];*/
    //     cout << endl << "New MCP " << (*aData) << endl;
    //   }
#if 0
    for(ii = 0; ii < (int)thePpacData.size(); ii++)
    {
        aData = thePpacData[ii];
        cout << (*aData) << endl;
    }

    for(ii = 0; ii < (int)theIcData.size(); ii++)
    {
        aData = theIcData[ii];
        cout << (*aData) << endl;
    }

    for(ii = 0; ii < (int)theSideData.size(); ii++)
    {
        aData = theSideData[ii];
        cout << (*aData) << endl;
    }
#endif

    new_event(theMcpData, thePpacData, theIcData, theSideData);

    //if( ( mcp_x() > -1000.*mm ) && ( mcp_y() > -1000.*mm ) ) {
    //  theEvent->set_theta    ( (unsigned short int)(10.*theta_c()/degree)  );
    //  theEvent->set_phi      ( (unsigned short int)(10.*phi_c()/degree)    );
    //  cout<< "Xmcp "<< mcp_x()/mm << endl;
    //  cout<< "Ymcp "<< mcp_y()/mm << endl;
    //}
    //if( x_fp() > 1.5*mm )
    //  cout<< "x_fp "<< x_fp()/mm << endl;
    ////if( tof() > 1.0*ns )
    ////  theEvent->set_mass     ( (unsigned short int)(10.*tof()/ns ) );

    //if( length() > 0. ) {
    //  theEvent->set_tra_len  ( (unsigned short int)(length()/mm) );
    //  theEvent->set_radius   ( (unsigned short int)(radius()/mm) );
    //  theEvent->set_energy   ( (unsigned short int)(ic_energy()) );
    //  theEvent->set_path     ( (unsigned short int)(path())      );
    //  theEvent->set_range    ( (unsigned short int)(range())     );
    //  theEvent->set_zed      ( (unsigned short int)(10*Zed() ) );
    //  theEvent->set_mass     ( (unsigned short int)(Mass()) );
    //  //      Mass();
    //  //    theEvent->set_mass       ( (unsigned short int)(Mass() ) );
    //  theEvent->set_eob      ( (unsigned short int)(energy_over_beta() ) );
    //  theEvent->set_aoq      ( (unsigned short int)(a_over_q_uncal() ) );
    //  theEvent->set_lor      ( (unsigned short int)(length_over_radius() ) );
    //  theEvent->set_rbe      ( (unsigned short int)(r_beta () ) );
    //}



    spec_dE_vs_E_tof->manually_increment(ic_energy(), ic_energy_A_DE());   // gated by tof
    theMassCalculator->analyse_current_event();
    
    spec_aoq_vs_x_fp->manually_increment(x_fp(),  a_over_q_uncal()) ;
    spec_ic_energy->manually_increment(ic_energy());
    spec_beta->manually_increment(beta());


    //------------------------- end of analysis, we can release the vectors of map data
    for(size_t nn = 0; nn < theMcpData.size(); nn++) delete theMcpData[nn];
    theMcpData.clear();
    for(size_t nn = 0; nn < thePpacData.size(); nn++) delete thePpacData[nn];
    thePpacData.clear();
    for(size_t nn = 0; nn < theIcData.size(); nn++) delete theIcData[nn];
    theIcData.clear();
    for(size_t nn = 0; nn < theSideData.size(); nn++) delete theSideData[nn];
    theSideData.clear();
    // return ; // true;
}

#endif // if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
//*******************************************************************
