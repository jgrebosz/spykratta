#ifndef _one_signal_module_H_
#define _one_signal_module_H_

#include <cstdint>


#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>

// #include "TIFJEvent.h"
#include "Tincrementer_donnor.h"



//////////////////////////////////////////////////////////////////////////////
/// \brief The Tone_signal_module class
//template <typename Type = uint16_t>


template <typename T = uint16_t>
class Tone_signal_module : public Tfrs_element , public Tincrementer_donnor
{

    double double_signal_data;
    //T signed_signal_data;

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

public:
//    int16_t TIFJEvent::*signal_ptr;
    T *signal_ptr;
protected:
    spectrum_1D * spec_first ;
    string signal_description;
public:
    //  constructor
    Tone_signal_module(string _name, T *first_ptr, string description = "signal") ;

    void analyse_current_event() ;
    void make_preloop_action(ifstream &);   // read the calibration factors, gates
    double give_value()
    {
        return double_signal_data;
    }

protected:
    void create_my_spectra();

};

/////////////////////////////////////////////////////////////////////////
#include <Tone_signal_module.cxx>

#endif // _one_signal_module_H_
