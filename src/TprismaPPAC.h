//g++ -lstdc++ -L./lib -lPRISMA -I./include TprismaPPAC.cc
#ifndef PRISMA_PPAC
#define PRISMA_PPAC

#include "TprismaDetector.h"
#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#include <vector>
#include <string>

#include "Tfrs_element.h"

using namespace std;
#include "Tincrementer_donnor.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TprismaPPAC : public TprismaDetector, public Tincrementer_donnor
{
public:
    TprismaPPAC(string name, int = 10, int = 6);   // default for PPAC: 10 detectors, 6 params each
    ~TprismaPPAC();


    int  validate_event(std::vector<detData*>&);
    void  process_event(std::vector<detData*>&);
    void analyse_current_event() {}
    void make_preloop_action(std::ifstream &);

private:
    int  read_configuration();
    void init_data();
    void create_my_spectra();

    // parameters
    int   ind_yu;        // y up
    int   ind_yd;        // y down
    int   ind_xr;        // x right
    int   ind_xl;        // x left
    int   ind_cathode;        // x cathode
    int   ind_tof;       // tof

    // banana file(s)
    string ban_file;
    int    ban_res_x;
    int    ban_res_y;

    // calibration files

    string xl_file;        // x left
    string xr_file;        // x right
    string tof_file;       // tof
    string tof_ofile;      // tof offsets
    string xfp_file;       // x focal plane

    // the members which should be calculated event-by-event

    double ppac_xfp;
    bool xfp_ok;

    double ppac_yfp;
    bool yfp_ok;

    bool flag_y_is_in_banana_ok;                  // Raw values. Then it will be tested during configuration
//  double  det_0_cathode_for_y_purposes;

    double ppac_tof;
    bool tof_ok;  // validator

    // general calibration factors
    double xfp_gain;
    double xfp_offs;

    double xl_gain;
    double xl_offs;

    double xr_gain;
    double xr_offs;

    //   --- y ---
    double yfp_gain;
    double yfp_offs;

    double yu_gain;
    double yu_offs;

    double yd_gain;
    double yd_offs;



    double tof_gain;
    double tof_offs;

    double tof_ogain;
    double tof_ooffs;

    double tof_fgain;
    double tof_foffs;

    double xr_xl_gain;    // gain applied to xr-xl when computing xfp
    double xr_cathode_gain;    // gain applied to xr-cathode when computing xfp
    double cathode_xl_gain;    // gain applied to cathode-xl when computing xfp
    double check_x_gain;    // gain applied to xr-xl when validating event

    double yd_yu_gain;    // gain applied to yd - yu when computing yfp
    double yd_cathode_gain;    // gain applied to yd - cathode when computing yfp
    double cathode_yu_gain;    // gain applied to cathode - yu when computing yfp
    double check_y_gain;    // gain applied to yd_yu when validating event




    int ignore_banana;    // true: excludes banana check ...


    double raw_xl;  // xfp obtained through cathode-xl (uncalibrated)
    double raw_xr;  // xfp obtained through xr-cathode (uncalibrated)
    double raw_xf;  // xfp obtained through xr-xl (uncalibrated)
    double raw_tf;  // calibrated tof (section by section)


    double cal_xl;  // xfp obtained through cathode-xl (calibrated)
    double cal_xr;  // xfp obtained through xr-cathode (calibrated)
    double cal_xf;  // xfp obtained through xr-xl (calibrated)


    double cal_yu;  // yfp obtained through cathode-xl (calibrated)
    double cal_yd;  // yfp obtained through xr-cathode (calibrated)
    double cal_yf;  // yfp obtained through xr-xl (calibrated)



    int    valid_idx; // valid events have just a single section firing, store its number here

public:
    inline double get_xfp()
    {
        return ppac_xfp;
    };
    inline double get_tof()
    {
        return ppac_tof;
    };

    // number of the section (horizontal) which fired
public:
    inline int    get_section()
    {
        return valid_idx;
    };

    // "raw" (uncalibrated) values

    inline double get_xfp_lu()
    {
        return raw_xl;      // cathode-xl
    }
    inline double get_xfp_ru()
    {
        return raw_xr;      // xr-cathode
    }
    inline double get_xfp_xu()
    {
        return raw_xf;      // xr-xl
    }
    inline double get_tof_xu()
    {
        return raw_tf;      // tof
    }

    // calibrated values

    inline double get_xfp_lc()
    {
        return cal_xl;      // x_fp (cathode-xl only)
    }
    inline double get_xfp_rc()
    {
        return cal_xr;      // x_fp (xr-cathode only)
    }
    inline double get_xfp_cathode()
    {
        return cal_xf;      // x_fp (xr-xl only)
    }



protected:

    vector<spectrum_1D *>     spec_tof_raw;
    vector<spectrum_1D *>     spec_tof_cal;
    vector<spectrum_1D *>     spec_tof_cal_realigned;

    vector<spectrum_2D*>  spec_x_gain_vs_cath;

    spectrum_1D   *   spec_ppac_tof;   // finall, full
    spectrum_1D   *   spec_x_fp_mm;   // finall, full
    spectrum_1D   *   spec_y_fp_mm;   // finall, full

    spectrum_2D   *   spec_tof_final__vs__x_fp_mm;
    spectrum_2D   *   spec_y_fp_mm_vs_x_fp_mm;

    vector<spectrum_1D *>  spec_cathode_raw;
    vector<spectrum_1D *>  spec_x_left_raw;
    vector<spectrum_1D *>  spec_x_right_raw;

    vector<spectrum_1D *>  spec_y_up_raw;            
    vector<spectrum_1D *>  spec_y_down_raw;
    vector<spectrum_1D *>  spec_y_sum_raw;

    vector<int>  tof_raw ;// for array of 10 tofs
    vector<double>  tof_cal ;// for array of 10 tofs
    vector<double>  tof_cal_realigned ;// for array of 10 tofs

    vector<int>  cathode_raw ;//
    vector<double>  sumXleftRight_raw ;//
    vector<double>  sumYupDown_raw ;//     

    vector<TjurekPolyCond *> polygon ;
    vector<string>  polygon_name;
}
;
#endif  // CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#endif
