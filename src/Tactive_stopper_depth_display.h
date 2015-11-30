#ifndef Tactive_stopper_depth_display_H_
#define Tactive_stopper_depth_display_H_


// this conditional compilation below had to be introduced
// because the dictionary was wrongly created when I introduced
// the poiters to the members of the TIFJEvent class
#ifndef  __CINT__

#include "experiment_def.h"
#ifdef MUN_ACTIVE_STOPPER_PRESENT

#include "Tfrs_element.h"
#include "spectrum.h"
#include <string>
#include <sstream>


#include "TIFJEvent.h"
#include "Tincrementer_donnor.h"




//class focus_point ;
//typedef unsigned int calkow;
typedef int calkow;

const int how_many_775_channels = 32;
const int how_many_767_channels = 128;

//////////////////////////////////////////////////////////////////////////////
class Tactive_stopper_depth_display : public Tfrs_element , public Tincrementer_donnor
{
    int signal_767data[how_many_767_channels];
    int signal_775data[how_many_775_channels];

    bool please_increment_channel[22] ;

    // pointer to members of event class
    // it must be done like that, because the event does not
    // exist yet. It will be produced by the factory
    // in the moment of running the analysis.

    int (TIFJEvent::*signal_767ptr)[how_many_767_channels];
    int (TIFJEvent::*signal_775ptr)[how_many_775_channels];

public:
    //  constructor
    Tactive_stopper_depth_display(string _name,
                                  int (TIFJEvent::*first_ptr)[how_many_767_channels],
                                  int (TIFJEvent::*second_ptr)[how_many_775_channels]
                                 );

    void analyse_current_event()  ;
    void make_preloop_action(ifstream &) ;   // read the calibration factors, gates

protected:
    void create_my_spectra();
    spectrum_1D * spec_depth_implantation;
    spectrum_1D * spec_depth_decay;
};
#endif // ifdef MUN_ACTIVE_STOPPER_PRESENT

#endif // CINT

#endif // 
