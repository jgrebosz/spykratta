#ifndef FSOLVER_CLASS
#define FSOLVER_CLASS

#include "SystemsOfUnits.h"
#include "ConfigurationFile.h"
#include "Vectors.h"
#include "detData.h"
#include <iostream>
#include <vector>
#include <string>
#include <math.h>

using namespace NuclearPhysicsSystem;
using namespace std;

class ConfigurationFile;
//class detData;
#include <spectrum.h>
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TfastSolver : public Tfrs_element, public Tincrementer_donnor
{
public:
    TfastSolver(string name);
    TfastSolver(string name, double, double);
    TfastSolver(string name, double);
    TfastSolver(string name, char*);
    TfastSolver(string name, char*, double);
    TfastSolver(string name, string);
    TfastSolver(string name, string, double);
    ~TfastSolver();

    void analyse_current_event() {}
    void make_preloop_action(std::ifstream &);
private:
    void initGeometry();
    bool getQuadrupoleEntrance(double x, double y);
    bool getQuadrupoleExit();
    bool getDipoleEntrance();
    bool getDipoleExit();
    bool getFocalPlanePosition();
    double getFocalPlaneDispersion();
    double getTrajLength();

    void create_my_spectra();
    void free_reservations();
    void initial_zeroing_of_pointers();

public:
    bool findTrajectory(double xe, double ye, double xf);
    bool findATrajectory(double th, double ph, double xf);
    bool FindChamber(int n_det, int* origIndex, double* origData);
    bool FindChamber(int*, std::vector<detData*>&);

public:
    inline double getLength()
    {
        return traj_length;
    };
    inline double getRadius()
    {
        return curv_radius;
    };
    inline double getICEner()
    {
        return ic_energy;
    };
    inline double getIC_ADE()
    {
        return ic_a_de;
    };
    inline double getIC_ABE()
    {
        return ic_ab_de;
    };
    inline double getICPath()
    {
        return ic_path;
    };
    inline double getICRang()
    {
        return ic_range;
    };
    inline double getTheta()
    {
        return th_mcp;
    };
    inline double getPhi()
    {
        return ph_mcp;
    };
    inline double getTargetMcpDistance()
    {
        return target_mcp_distance;
    };

    inline Vector3D getPosDipoleIn()
    {
        return pos_dip_in;
    }
    inline Vector3D getPosDipoleOut()
    {
        return pos_dip_out;
    }

    inline Vector3D getPosQuadrupoleIn()
    {
        return pos_quad_in;
    }
    inline Vector3D getPosQuadrupoleOut()
    {
        return pos_quad_out;
    }
    inline Vector3D getPosFocalPlane()
    {
        return fp_position;
    }
    inline Vector3D getCenter()
    {
        return traj_centre;
    }

public:
    inline int  has_solved()
    {
        return solved_ev;
    };
    inline int  solved_chamber()
    {
        return solved_ch;
    };
    inline void not_solved()
    {
        solved_ev = 0;
        solved_ch = 0;
    };

private:
    bool  GoodSections(int n_det, int* origIndex, double* origData);
    bool  GoodSections(int*, std::vector<detData*>&);

private:
    bool  processChamber(int);
    bool  processChamber(int*, std::vector<detData*>&);




    // configuration
private:
    ConfigurationFile* theFile;
    bool gets_ratio;

    // magnetic fields
private:
    double    B_dipole;
    double    B_quadrupole;
    double    B_ratio;
    double    B_sqrt;
    
private:
    double   ConvertADC_Tesla_Dipole(double adc_dipole);
    double   ConvertADC_Tesla_Quadrupole(double adc_quadrupole);
    
    
public:
    inline double Get_B_dipole()
    {
    	return B_dipole;
    }
    
    // MCP start detector
private:
    double    x_mcp;
    double    y_mcp;
    double    angle_mcp;
    double    target_mcp_distance;
    Vector3D  position_mcp;
    double    th_mcp;
    double    ph_mcp;

    // focal plane
private:
    double x_fp;

    // entrance of the quadrupole
private:
    Vector3D   pos_quad_in;
    Vector3D   vel_quad_in;
    double     target_quad_distance;
    double     quad_radius;
    double     geom_radius;
    double     sqrt_radius;
    double     quad_length;

    // exit of the quadrupole
private:
    Vector3D   pos_quad_out;
    Vector3D   vel_quad_out;

    // coeff for transportation within the quadrupole
private:
    double     tg_alpha;
    double     tg_beta;
    double     coefA;
    double     coefB;
    double     coefC;
    double     coefD;
    double     coef_k;
    double     k_coef;

private:
    double     guess_radius;
    double     curv_radius;

    // dipole entrance
private:
    double     target_dipole_distance;
    double     quad_dip_distance;
    double     dip_in_m;
    double     dip_in_q;
    Vector3D   pos_dip_in;
    Vector3D   p_dip_in;
    Vector3D   vel_dip_in;
    double     dipole_height;
    double     target_dip_distance;

private:
    Vector3D   traj_centre;
    double     dip_out_m;
    double     dip_out_q;
    Vector3D   pos_dip_out;
    Vector3D   vel_dip_out;

private:
    double     fp_m;
    double     fp_q;
    Vector3D   fp_zero;
    Vector3D   fp_position;
    double     fp_coordinate;
    double     var_radius;
    double     fp_dispersion;
    double     tolerance;
    double     traj_length;
    double     c_ang;
    double     interc;


    // ionization chamber
private:
    double        ic_fp_distance;
    double        ic_depth;
    double        ic_width;
    int                   nSec;
    Vector3D*     ic_zero;
    double    *       ic_q;
    double    *       dE;

private:
    double     angular_tolerance;
    int        ic_threshold;
    int        ic_up_thresh;

    // 30/01/08 EF: we remove reference to the "original" format
    //              accepting data in "double" format rather than
    //              unsigned short int
    //              The addition of a random number to avoid binning effects
    //              is not needed here but should be done elsewhere!!!
    // unsigned short int* chamber_data;

private:
    double*       chamber_data;
    Vector3D*     x_min;
    Vector3D*     x_max;
    int               det_len;
    int               last_sec;
    double*       centre_section;

private:
    double      ic_energy;
    double      ic_a_de;
    double      ic_ab_de;
    double      ic_path;
    double      ic_range;

private:
    bool        solved_ev;
    int         solved_ch;

private:
    int* firstid;
    int* lastid;
    int* index1;
    int* index2;

//  vector<vector<spectrum_1D*> >  spec_side_de_raw;
    spectrum_1D   *spec_length;
    spectrum_1D   *spec_radius;
};

#endif /* SOLVER_CLASS */
