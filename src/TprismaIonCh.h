//////////////////////////////////////////////////////////////////
/// prismaIC: calibrates and produces total energy, DE_A, DE_AB
//////////////////////////////////////////////////////////////////
#ifndef PRISMA_IC
#define PRISMA_IC

#include "TprismaDetector.h"
#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#include <vector>
#include <string>

using namespace std;
#include "Tincrementer_donnor.h"
// *********************************************************************************************
class TprismaIonCh : public TprismaDetector, public Tincrementer_donnor
{
public:
    TprismaIonCh(string name, int = 10, int = 4);   // default for IC: 10 detectors, 4 params each
    ~TprismaIonCh();

private:
    int  read_configuration();
    void init_data();
    void create_my_spectra();

public:
    int  validate_event(std::vector<detData*>&);
    void  process_event(std::vector<detData*>&);

    // parameters
private:
    int   ind_s[4];

private:
    double ic_s1_gain;
    double ic_s1_offs;
    double ic_s2_gain;
    double ic_s2_offs;
    double ic_s3_gain;
    double ic_s3_offs;
    double ic_s4_gain;
    double ic_s4_offs;

private:
    string ic_s1_file;
    string ic_s2_file;
    string ic_s3_file;
    string ic_s4_file;

private:
    double ic_tot_E;
    double ic_de[4];
    double id_de_ab;
private:
    std::vector<detData*> calibratedIC;
    std::vector<detData*> uncalibratedIC;

private:
    int threshold_before_cal;
    int veto_threshold;
    int max_veto_id;

    // sometimes on the end of raw spectum there is an "overflow peak".
    // This helps it to skip such situations
    int ionch_dE_raw_upper_limit;


public:
    inline double get_tot_E()
    {
        return ic_tot_E;
    };
//  inline double get_A_DE  () { return ic_de[0];             };
//  inline double get_AB_DE () { return ic_de[0] + ic_de[1];  };
    double get_A_DE();
    double get_AB_DE();
    inline int get_ind(int);
    inline double get_cal(int);

public:
    double get_calibrated_ic(int, int);
    double get_uncalibrated_ic(int, int);

    void analyse_current_event() {}
    void make_preloop_action(std::ifstream &);

protected:
    vector<vector<spectrum_1D*> >  spec_ic_de_raw;
    vector<vector<spectrum_1D*> >  spec_ic_de_cal;

    vector<spectrum_2D*>   spec__dEa_vs_E;
    vector<spectrum_2D*>   spec__dEab_vs_E;
   

    spectrum_2D*   spec__total_dEa_vs_E;
    spectrum_2D*   spec__total_dEab_vs_E;
    //spectrum_2D*   spec__total_dEa_vs_E_tot;
    spectrum_1D*   spec__total_E;
    spectrum_1D*   spec__rejected_event_ionch;
    // delta for each of 10 kolumns
    vector<double>   dE_A;   // just in first
    vector<double>   dE_AB;   // first and second
    vector<double>   dE_ABCD;   // all 4 elements
    bool flag_valid ;
};
//*****************************************************************
inline int TprismaIonCh::get_ind(int ii)
{
    if((ii < 0) || (ii > 3))
        return -1;
    else
        return ind_s[ii];
}
#endif // CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT

#endif
