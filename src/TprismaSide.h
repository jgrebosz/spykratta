//////////////////////////////////////////////////////////////////
/// TprismaSide: in the actual data analysis, the Side detector
///  is used only to discard events in which it is giving
///  signal
//////////////////////////////////////////////////////////////////
#ifndef PRISMA_SIDE
#define PRISMA_SIDE

#include "TprismaDetector.h"
#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#include <vector>
#include <string>

using namespace std;
#include "Tincrementer_donnor.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TprismaSide : public TprismaDetector, public Tincrementer_donnor
{
public:
    TprismaSide(string name, int = 10, int = 4);   // default for Side: 10 detectors, 4 params each
    ~TprismaSide() { };

private:
    int  read_configuration();
    void init_data() {};
    void create_my_spectra();

public:
    int  validate_event(std::vector<detData*>&);
    void  process_event(std::vector<detData*>&) {};

    void analyse_current_event() {}
    void make_preloop_action(std::ifstream &) ;

    // parameters
private:
    int   ind_s[4];

    vector<vector<spectrum_1D*> >  spec_side_de_raw;
    spectrum_1D   *spec_multiplicity;
    bool flag_protest;

};
#endif //CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
#endif
