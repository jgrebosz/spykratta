#ifndef MASS_CALC
#define MASS_CALC

#include <vector>
#include <string>

#include "banManager.h"
#include "calManager.h"
#ifdef ABERRATIONS___bad_algorithm
#include "curveManager.h"
#endif
#include "ConfigurationFile.h"
#include "SystemsOfUnits.h"

using namespace std;
using namespace NuclearPhysicsSystem;
#include <spectrum.h>
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"

class TprismaManager;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TmassCalculator : public Tfrs_element, public Tincrementer_donnor
{
public:
    TmassCalculator(string myname,     TprismaManager *  ptr_prisma_manager_);
    ~TmassCalculator();


private:
    int z_min;
    int z_max;

    int q_min;
    int q_max;

    int a_min;
    int a_max;
    
    int a_start;
    int z_start;
    int fragment_type;
    int p_rem_max;
    int p_add_max;
    int n_rem_max;
    int n_add_max;
    int max_charge_states;
    

    int    ban_res_x;
    int    ban_res_y;
    //string ban_file_base;
    //string mas_file_base;
    //string chg_file_base;

    //int multi_bananas;

private:
    std::vector<banManager*> theBanManager;
    std::vector<banManager*> theMasManager;
    //std::vector<calManager*> theChargeManager;



    vector< vector< pair<TjurekPolyCond*, int> > > charge_state_bananas_vector_for_zed;

    vector< vector< pair<TjurekPolyCond*, int> > > massA_bananas_vector_for_zed;

    TprismaManager *ptr_prisma_manager;

    //////////////////////////////////////////////////
    /// Calibration: we assume a different alignment
    /// depending on the atomic number
    //////////////////////////////////////////////////
private:
    string      cal_file;
    calManager* theCalManager;

    double      a_over_q_offs;
    double      a_over_q_gain;
    double      r_fact;
    double      e_fact;
    double      B_dipole;

    const double inv_c_light;

private:
    double charge;
    bool charge_ok; // validator
//     bool charge_calc_already_done;

    double mass;
    bool mass_ok;

    bool flag_zed_from_dE_vs_E__otherwise_from_path ;
    //     bool mass_calc_already_done;

    int    mass_num;
    bool mass_num_ok;
// bool mass_num_calc_already_done;

    double a_over_q;
    bool a_over_q_ok;

    double total_kinetic_energy_loss;
    bool total_kinetic_energy_loss_ok;
    //string charge_file;

    // configuration is decoded from file
protected:
    ConfigurationFile* theFile;

#ifdef ABERRATIONS___bad_algorithm
    ////////////////////////////////////////////////
    /// corrections for aberrations
    ////////////////////////////////////////////////
private:
    string      xmc_file;
    string      ymc_file;
    string      xfp_file;

private:
    calManager* theXmcManager;
    calManager* theYmcManager;
    calManager* theXfpManager;

private:
    double      xmc_offs;
    double      xmc_gain;

private:
    double      ymc_offs;
    double      ymc_gain;

private:
    double      xfp_offs;
    double      xfp_gain;

private:
    string      xmc_spli;
    string      ymc_spli;
    string      xfp_spli;

private:
    curveManager* theXmcSpliManager;
    curveManager* theYmcSpliManager;
    curveManager* theXfpSpliManager;

private:
    int         xmc_uspl;    // 0 --> use polynomials; 1 --> use spline
    int         ymc_uspl;
    int         xfp_uspl;
#endif

private:
    int  read_configuration();
    void init_data();
    void create_my_spectra();

    //spectrum_1D  *spec_mass;

    spectrum_1D*    spec_aoq_uncal;
    spectrum_1D*    spec_aoq_cal;
    spectrum_1D*    spec_deduced_charged_states;
    spectrum_1D*    spec_mass;
    
    spectrum_1D* spec_radius_multiplied_by_velocity;

    spectrum_2D*    spec_charge_states__for_any_zed;
    spectrum_2D*    spec_q_react_calculator;
    vector<spectrum_2D*>  spec_charge_states__for_z;
    vector< vector<spectrum_2D*> >  spec_a_over_q_vs_xfp__for_z_for_q;

    vector< vector<spectrum_1D*> > spec_tkel__for_z_a;   // [z][a]
    vector< vector<spectrum_1D*> > spec_energy_used_q_reac__for_z_a   ;
    vector< vector<spectrum_1D*> > spec_angle_used_q_reac__for_z_a    ;
    vector< vector<spectrum_2D*> > spec_Ek_vs_theta__for_z_a    ;
    
    vector<spectrum_2D*>  spec_mass_vs_xfp__for_z;
    vector<spectrum_1D*>  spec_mass__for_z;



public:
    void  analyse_current_event() ;
    void  make_preloop_action(std::ifstream &) ;
    double    give_a_over_q_cal()
    {
        return a_over_q ;   // put condition to calculate
    }
    void zeroing_variables();
    int give_mass_number()
    {
        if(calculations_already_done == false)
        {
            analyse_current_event();
        }
        return mass_num ;
    }

    bool      is_mass_num_ok()
    {
        return  mass_num_ok;
    }
    double    a_over_q_uncal(int, double, double);
    double    r_beta(int, double, double);

    double q_reaction(int zed);
    
private:
    double    deduce_charge_state(int, double, double, double);
    int       deduce_mass_number(int, double, double, double, double);
    double    calculate_mass_from_a_over_q(int, double, double, double);
    void      make_A_bananas_on_the_matrices_A_vs_Xfp__for_z(int z);
    double    mass_ame(int z, int a);
    double    Energy_k(double mass);
    double    qreact_gs_to_gs(int z, int a);





    double    radius_multiplied_by_velocity ;
    double    energy_of_ion_chamber;
    double    ame_table[120][300];
    double    qreact_value[120][300];
#ifdef ABERRATIONS___bad_algorithm
public:
    double Correct_Xmc(int, double);
    double Correct_Ymc(int, double);
    double Correct_Xfp(int, double);

public:
    double Mass(int, double, double, double, double, double, double);
    int    mass_number(int, double, double, double, double, double, double);
#endif
};

#endif
