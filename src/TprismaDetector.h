///////////////////////////////////////////////////////////////////////
/// pure virtual class to define an interface
//////////////////////////////////////////////////////////////////////
#ifndef PRISMA_DET
#define PRISMA_DET

#include "experiment_def.h"
#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT

#include <vector>
#include <string>
#include "SystemsOfUnits.h"
#include "ConfigurationFile.h"
#include "banManager.h"
#include "calManager.h"
#include "detData.h"

#include "Tfrs_element.h"
#include <spectrum.h>

using namespace std;
using namespace NuclearPhysicsSystem;

class detData;
class calManager;
class banManager;
class ConfigurationFile;

class TprismaDetector : public Tfrs_element
{
public:
    TprismaDetector(string name, int, int, string): Tfrs_element(name)
    {
        theBanManager = NULL ;
        theFile = NULL;
    }
    TprismaDetector(string name): Tfrs_element(name)
    {
        theBanManager = NULL ;
        theFile = NULL;
    };
    virtual  ~TprismaDetector()
    {
        delete theBanManager;
        delete theFile;
    };

    // managers for banana gates and calibrations
protected:
    banManager*              theBanManager;
    std::vector<calManager*> theCalManager;


    // how many detectors/parameters
protected:
    int n_det;
    int n_par;

    // valid parameters?
protected:
    std::vector<int> valid;

    // error code: why was event not valid?
protected:
    int err_code;

    // valid configuration?
protected:
    int valid_conf;

    // valid event?
protected:
    bool valid_evt;

    // has processed event?
protected:
    int processed_event;

    // configuration is decoded from file
protected:
    ConfigurationFile* theFile;

    // threshold for validity of parameters (non-zero)
protected:
    double lower_th;

public:
    inline int get_ndet()
    {
        return n_det;
    };
    inline int get_npar()
    {
        return n_par;
    };

public:
    inline int get_err_code()
    {
        return err_code;
    };

protected:
    virtual void init_data() = 0;

    // event manipulation
    // (changes original data!)
public:
    virtual int  validate_event(std::vector<detData*>&) = 0;
    virtual void process_event(std::vector<detData*>&) = 0;

protected:
    virtual int read_configuration() = 0;

public:
    virtual inline int valid_configuration()
    {
        return valid_conf;
    };
    virtual inline int has_processed()
    {
        return processed_event;
    };
    virtual inline int valid_event()
    {
        return valid_evt;
    };

};

#endif  // CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT

#endif
