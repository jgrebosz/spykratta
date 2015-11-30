//g++ -lstdc++ -L./lib -lPRISMA -I./include TprismaMcp.cc
#ifndef PRISMA_MCP
#define PRISMA_MCP

#include "TprismaDetector.h"
#include "TfastSolver.h"

#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT

#include <vector>
#include <string>
using namespace std;
// #include <spectrum.h>
#include "Tincrementer_donnor.h"
// ***************************************************************************************

class TprismaMcp : public TprismaDetector, public Tincrementer_donnor
{
public:
    TprismaMcp(string det_name,  int = 1, int = 3);  // default for Mcp: 1 detector, 3 parameters
    ~TprismaMcp();

private:
    int  read_configuration();
    void init_data();

public:
    int  validate_event(std::vector<detData*>&);
    void  process_event(std::vector<detData*>&);

    void analyse_current_event() {}
    void make_preloop_action(std::ifstream &);

private:
    void deform_mcp(std::vector<detData*>&);
    void rotate_mcp(double, std::vector<detData*>&);
    void get_angles();

    // relevant parameters
private:
    int   ind_xm;        // x mcp
    int   ind_ym;        // y mcp

    // banana file(s)
private:
    string ban_file;
    int    ban_res_x;
    int    ban_res_y;

    // calibration files
private:
    string xm_file;        // x mcp
    string ym_file;        // y mcp

private:
    double xm_gain;
    double xm_offs;
    double ym_gain;
    double ym_offs;

private:
    double theta_prisma;    // angle where PRISMA is positioned
    double theta_op_prisma ;// angular opening of PRISMA 
    double costh_p;
    double sinth_p;

private:
    double mcp_target_d;   // distance from target
    double mcp_angle;      // normally 135 degrees
    double costh_m;
    double sinth_m;

    // mixing coefficients for deformation
private:
    double mcp_mix_x[2];
    double mcp_mix_y[2];

    // rotation angle for Mcp
private:
    double theta_mcp;

    // the members which should be calculated event-by-event
private:

    int mcp_x_raw;
    int mcp_y_raw;

    double mcp_x;
    double mcp_y;

    bool xy_ok;

private:
    double mcp_theta_a;
    double mcp_phi_a;
    double mcp_theta_b;
    double mcp_phi_b;
    double mcp_theta_c;
    double mcp_phi_c;
    double mcp_theta_lab;
    double mcp_phi_lab;
    bool angles_ok ;

    // used only to calculate the velocity vector of the binary partner
private:
    int       beam_mass_number;
    int       beam_zed  ;
    double    beam_energy;
    double    beam_momentum;   // module
    Vector3D  beam_momentum_a;
    Vector3D  beam_momentum_b;
    Vector3D  beam_momentum_c;
    Vector3D  beam_momentum_lab;

private:
    double amu;

private:
    int ignore_banana;    // true: excludes banana check ...

public:
    inline double get_x_mcp()
    {
        return mcp_x;
    };
    inline double get_y_mcp()
    {
        return mcp_y;
    };

    // AGATA
public:
    inline double get_theta_a()
    {
        return mcp_theta_a;
    };
    inline double get_phi_a()
    {
        return mcp_phi_a;
    };

    // BEAM
public:
    inline double get_theta_b()
    {
        return mcp_theta_b;
    };
    inline double get_phi_b()
    {
        return mcp_phi_b;
    };

    // CLARA
public:
    inline double get_theta_c()
    {
        return mcp_theta_c;
    };
    inline double get_phi_c()
    {
        return mcp_phi_c;
    };
    
    
    // LABORATORY
public:
    inline double get_theta_lab()
    {
        return mcp_theta_lab;
    };
    inline double get_phi_lab()
    {
        return mcp_phi_lab;
    };

public:
    inline Vector3D get_beam_momentum_a()
    {
        return beam_momentum_a;
    };
    inline Vector3D get_beam_momentum_b()
    {
        return beam_momentum_b;
    };
    inline Vector3D get_beam_momentum_c()
    {
        return beam_momentum_c;
    };
    inline Vector3D get_beam_momentum_lab()
    {
        return beam_momentum_lab;
    };
    
public:
    inline int get_beam_mass_number()
    {
        return beam_mass_number;
    };
    
    inline int get_beam_zed()
    {
        return beam_zed;
    };

    
    inline double get_beam_energy()
    {
        return beam_energy;
    };

    inline int get_target_mass_number()
    {
        return target_mass_number;
    }
    
    inline int get_target_zed()
    {
        return target_zed;
    };
    inline double getPrismaAngle()
    {
        return theta_prisma;
    }
    inline double getPrismaOpening()
    {
        return theta_op_prisma;
    }

private:
    spectrum_1D   * spec_mcp_x_raw;
    spectrum_1D   * spec_mcp_y_raw;
    spectrum_1D   * spec_mcp_x_cal;
    spectrum_1D   * spec_mcp_y_cal;

    spectrum_2D   * spec_xy_cal;
    spectrum_2D   * spec_xy_raw;
    spectrum_2D   * spec_theta_phi_beam;
    spectrum_2D   * spec_theta_phi_c; 
    spectrum_2D   * spec_theta_phi_lab;
    
    spectrum_1D   * spec_phi_beam;
    spectrum_1D   * spec_theta_beam;
    spectrum_1D   * spec_phi_clara;
    spectrum_1D   * spec_theta_clara;
    spectrum_1D   * spec_phi_lab;
    spectrum_1D   * spec_theta_lab;

    TjurekPolyCond * polygon ;
    string polygon_name;
    int target_mass_number;
    int target_zed;

    void create_my_spectra();
};
#endif // CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#endif
