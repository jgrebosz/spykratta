#ifndef PRISMA_MANAGER
#define PRISMA_MANAGER

#include "experiment_def.h"

#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#include "SystemsOfUnits.h"
#include "Vectors.h"
#include "TprismaDetector.h"
#include "TprismaMcp.h"
#include "TprismaPPAC.h"
#include "TprismaIonCh.h"
#include "TprismaSide.h"
#include "TfastSolver.h"
#include "TzedCalculator.h"
#include "TmassCalculator.h"
//#include "detData.h"

#include <vector>
#include <string>

using namespace std;
using namespace NuclearPhysicsSystem;

#include "Tfrs_element.h"
#include "spectrum.h"
//#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TprismaManager : public Tincrementer_donnor, public Tfrs_element
{
public:
    TprismaManager(string name);
    TprismaManager(string name, int, int, int, int, int, int, int, int);
    ~TprismaManager();

    /** readinng calibration, gates, setting pointers */
    void make_preloop_action(ifstream &) ;

    /** the most important fuction,  executed for every event */
    void analyse_current_event()  ; // return value: was Si hit or not ?

    TprismaMcp* give_mcp_ptr()
    {
        return ((TprismaMcp*)theDetectors[ind_mcp]);
    }

    TfastSolver* give_fastsolver_ptr()
    {
        return (TfastSolver*) theSolver;
    }

private:
    std::vector<TprismaDetector*>     theDetectors;
    TfastSolver*                          theSolver;
    TzedCalculator*                       theZedCalculator;
    TmassCalculator*                      theMassCalculator;

private:

    int   threshold_for_general_raw_data;  // to make data valid, one of 6 data words should be > this threshold
    int  ind_mcp;
    int  ind_ppac;
    int  ind_ionch;
    int  ind_side;
    int  n_det;
    int* lookup;

    int valid_conf;
    int valid_evt;

    string  mcp_conf;
    string  ppac_conf;
    string  ionch_conf;
    string  side_conf;
    string  solver_conf;
    string  zed_conf;
    string  mass_conf;

    int     ndet_mcp;
    int     ndet_ppac;
    int     ndet_ionch;
    int     ndet_side;

    int     npar_mcp;
    int     npar_ppac;
    int     npar_ionch;
    int     npar_side;

    std::vector<std::vector<detData*>*> theEvent;

    // configuration is decoded from file
    ConfigurationFile* theFile;

    int  read_configuration();
    void init_data();
    void create_my_spectra();

    spectrum_2D*    spec_aoq_vs_x_fp;
    spectrum_1D*    spec_beta;
    spectrum_2D*    spec_dE_vs_E_tof;


public:
    inline int valid_configuration()
    {
        return valid_conf;
    };
    inline int valid_event()
    {
        return valid_evt;
    };


    void new_event(std::vector<detData*>&, std::vector<detData*>&, std::vector<detData*>&, std::vector<detData*>&);

    double mcp_x();
    double mcp_y();
    double theta_a();
    double phi_a();
    double theta_b();
    double phi_b();
    double theta_c();
    double phi_c();

    double tof();
    double tof_zero();
    double x_fp();

    double length();
    double radius();

    double ic_energy();
    double path();
    double range();
    double ic_energy_AB_DE();
    double ic_energy_A_DE();

    double old_ic_energy();
    double old_path();
    double old_range();

    double beta();
    double betagamma();
    double gamma();

    Vector3D velocity();
    Vector3D velocity_a();
    Vector3D velocity_b();
    Vector3D velocity_c();

    inline Vector3D getPosDipoleIn()
    {
        return theSolver->getPosDipoleIn();
    }
    inline Vector3D getPosDipoleOut()
    {
        return theSolver->getPosDipoleOut();
    }
    inline Vector3D getPosQuadrupoleIn()
    {
        return theSolver->getPosQuadrupoleIn();
    }
    inline Vector3D getPosQuadrupoleOut()
    {
        return theSolver->getPosQuadrupoleOut();
    }
    inline Vector3D getPosFocalPlane()
    {
        return theSolver->getPosFocalPlane();
    }
    inline Vector3D getCenter()
    {
        return theSolver->getCenter();
    }

    int   zed_deduced_from_pathE();    //ban E Path
    bool  flag_zed_pathE_already_done;
    int   zed_from_path; // TODO : make it inkrementer

    int   zed_deduced_from_EdE();  //ban E DE_AB
    bool  flag_zed_EdE_already_done;
    int   zed_from_EdE; // TODO : make it inkrementer


    double Mass_pathE();
    double Mass_EdE();
    int    mass_number();
    //int    mass_number_EdE();
    double a_over_q_uncal();
    double a_over_q_uncal1();
    double r_beta();
    double r_beta1();


    double  total_kinetic_energy_loss;
    bool  total_kinetic_energy_loss_ok;

    Vector3D velocity_partner();
    //Vector3D velocity_partner1();
    Vector3D velocity_partner_a();
    Vector3D velocity_partner_b();
    Vector3D velocity_partner_c();
    //Vector3D velocity_partner_a1();
    //Vector3D velocity_partner_b1__using_EdE();
    //Vector3D velocity_partner_c1();


    double length_over_radius();
    double energy_over_beta();

    int    mcp_err_code();
    int    ppac_err_code();
    int    ionch_err_code();
    int    side_err_code();

    // partial PPAC data
    double get_xfp_lc();
    double get_xfp_rc();
    double get_xfp_xc();

    int    get_ppac_section();

    double get_xfp_lu();
    double get_xfp_ru();
    double get_xfp_xu();
    double get_tof_xu();

    double get_calibrated_ic(int, int);
    double get_uncalibrated_ic(int, int);

    double charge_state();
    double charge_state1();

    spectrum_1D *   spec_ic_energy;


#ifdef ABERRATIONS___bad_algorithm

// most probably it is wrong algorithm (acc. to Danielle)
public:
    double Mass_aberrations();
    double Mass1_aberrations();

    int    mass_number_aberrations();
    int    mass_number1_aberrations();

    double Mass_Xmc(int, double);
    double Mass_Ymc(int, double);
    double Mass_Xfp(int, double);

#endif
};

#endif //CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
/////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
