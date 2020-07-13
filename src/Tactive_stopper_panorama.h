//
// C++ Interface: tactive_stopper_panorama
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef TACTIVE_STOPPER_PANORAMA_H
#define TACTIVE_STOPPER_PANORAMA_H

#include "experiment_def.h"


#ifdef MUN_ACTIVE_STOPPER_PRESENT

#include "Tfrs_element.h"
#include <Tincrementer_donnor.h>
#include "spectrum.h"

class Tactive_stopper_munich;
class Tpositron_absorber;

/**
This is a class which collects the data from absorber front, stoppers A, B, C, absorber rear
and prints the matrix showing the trajectory of implantations


@author dr. Jerzy Grebosz
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tactive_stopper_panorama : public Tfrs_element, public Tincrementer_donnor
{
public:
    Tactive_stopper_panorama(string name,
                             string front_absorber_name,
                             vector<string> stopper_names,
                             string rear_absorber_name);


    void analyse_current_event() ;
    void make_preloop_action(ifstream & s) {}



protected:
    Tpositron_absorber *front_absorber;
    vector<Tactive_stopper_munich *>stopper;
    Tpositron_absorber *rear_absorber;

    spectrum_2D * spec_x_panorama_impl;
    spectrum_2D * spec_x_panorama_decay;
    spectrum_2D * spec_y_panorama_impl;
    spectrum_2D * spec_y_panorama_decay;

    void create_my_spectra();

    // here we can make sums of other detectors
    double sum_energy_of_front_absorbers_plus_stoppers;
    double sum_energy_of_rear_absorbers_plus_stoppers;

    int multiplicity_of_front_absorbers_plus_stoppers;
    int multiplicity_of_rear_absorbers_plus_stoppers;
    int multiplicity_of_all_absorbers_plus_stoppers;

};

#endif // #ifdef MUN_ACTIVE_STOPPER_PRESENT


#endif
