#ifndef ZED_CALC
#define ZED_CALC

#include <vector>
#include <string>

#include "banManager.h"
#include "ConfigurationFile.h"

using namespace std;
#include <spectrum.h>
#include "Tfrs_element.h"
#include "Tincrementer_donnor.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TzedCalculator : public Tfrs_element, public Tincrementer_donnor
{
public:
    TzedCalculator(string myname, string = "zed_calculator.conf");
    ~TzedCalculator();

    int  Zed_remove(double, double, bool type);

    int  deduce_zed_from_bananas_on_matrix_E_vs_path(double, double) ;
    int  deduce_zed_from_bananas_on_matrix_dE_vs_E(double, double);

    void analyse_current_event();
    void make_preloop_action(std::ifstream &) ;

    void zeroing_variables();
    int give_zed_universal()
    {
        return zed_universal;
    }
private:
    int z_min;
    int z_max;
    int fragment_type;
    int p_rem_max;
    int p_add_max;  
    

private:
//     int    ban_res_x;
//     int    ban_res_y;
//     string ban_file_base;
    string     zed_dE_vs_E_matrix_name ;  // tmp
    string     zed_E_vs_path_matrix_name; // tmp

private:
    double gain_energy;
    double gain_range;

private:
    vector< pair<TjurekPolyCond*, int> > zed_dE__banana_vector;
    vector< pair<TjurekPolyCond*, int> > zed_E_vs_path__banana_vector;
private:
    int zed_from_ic_value ;
    bool zed_from_ic_validator;

    int zed_from_path_value ;
    bool zed_from_path_validator;

    // only one of them is valid at the time.(depending on wish
    // in PrismaManager:  flag_zet_from_dE
    // To make it universal - below there is a universal Z value, taken from
    //     on e of those above
    int zed_universal;
    bool flag_zed_universal_ok;


    double energy_gained;
    double path_gained;
    double dE_gained;
    bool valid;


private:
    int multi_bananas;

    // configuration is decoded from file
protected:
    //ConfigurationFile* theFile;

private:
    int  read_configuration();
    void init_data();
    void create_my_spectra();

    spectrum_2D  *spec_zed_E_vs_path;
    spectrum_2D  *spec_zed_dE_vs_E;

    spectrum_1D  *spec_zed_from_ic;
    spectrum_1D  *spec_zed_from_path;
    spectrum_1D  *spec_zed_E;



}
;

#endif
