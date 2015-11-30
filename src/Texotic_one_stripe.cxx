//
// C++ Implementation: Texotic_one_stripe
//
// Description:
//
//
// Author: dr. Jerzy Grebosz <jerzy.grebosz@ifj.edu.pl>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "Texotic_one_stripe.h"

#include "Tfile_helper.h"
#include "randomizer.h"
#include "Texotic_vector_strips.h"

//#include "Texotic_full.h"
#include "Tnamed_pointer.h"
#include <sstream>

#include "TIFJEvent.h"

#ifdef NIGDY
namespace exotic_namespace
{
// to distinguish two modes of operation implantation/decay
type_of_exotic_event   exotic_event_type;
}
using namespace exotic_namespace;
#endif

#include "Texotic_vector_strips.h"

#if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
// static
// not all samples need to participate in the mean value. Here below, the user may decide which does.
bool   Texotic_one_stripe::sample_participates_in_mean[NR_OF_EXOTIC_SAMPLES];

//********************************************************************
Texotic_one_stripe::Texotic_one_stripe(
    Texotic_vector_strips *ptr_owner,
    string name,
    int module_nr,
    int id_xy,
    int id_stripe_nr
)
    : Tfrs_element(name), Tincrementer_donnor(),
      owner_det(ptr_owner),
      name_of_this_element(name),
      id_module(module_nr),
      id_x_or_y(id_xy),
      id_stripe(id_stripe_nr)
{

    my_id_for_osciloscope_matrix =    /*( module_nr * 1000 ) + */ (10 *  id_stripe_nr);

    //     cout << "for " << name_of_this_element << ", the id = " << my_id_for_osciloscope_matrix
    //     << "           id_module = " << id_module << ",  id_x_or_y=" << id_x_or_y << ", id_stripe= " << id_stripe
    //     << endl;
    //
    data_ptr = 0; // NULL;

    named_pointer[name_of_this_element +
                  "_energy_cal_when_fired"]
    = Tnamed_pointer(&energy_calibrated, &flag_fired, this) ;

    //  pseudo FAN for H-J
    string entry_name =  "ALL_exotic_det_stripe_pseudo_fan" ;
    Tmap_of_named_pointers::iterator  pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&pseudo_fan, &flag_fired, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&pseudo_fan, &flag_fired, this) ;

    create_my_spectra();
}
//********************************************************************
Texotic_one_stripe::~Texotic_one_stripe()
{}
//********************************************************************
void Texotic_one_stripe::create_my_spectra()
{
    //    cout << "Texotic_one_stripe::create_my_spectra  for " << name_of_this_element << endl;
    string name = name_of_this_element + "_energy_raw"  ;
    string folder("stopper/");
    spec_energy_raw =  new spectrum_1D(name,
                                       name,
                                       (id_x_or_y ? 256 : 4096),  // x can be short (256 channels)
                                       0,
                                       (id_x_or_y ? 256 : 4096),  // x can be short (256 channels)
                                       folder, noraw);
    frs_spectra_ptr->push_back(spec_energy_raw) ;
    //    cout << " MIDDLE Texotic_one_stripe::create_my_spectra  for " << name_of_this_element << endl;


    name = name_of_this_element + "_energy_adjusted_non_cal"  ;
    spec_energy_adjusted =  new spectrum_1D(name,
                                            name,
                                            (id_x_or_y ? 256 : 4096),  // x can be short (256 channels)
                                            0,
                                            (id_x_or_y ? 256 : 4096),  // x can be short (256 channels)
                                            folder, "");
    frs_spectra_ptr->push_back(spec_energy_adjusted) ;
    // calibrated spectra ++++++++++++++++++++++++++++++++++++++++++

    name = name_of_this_element + "_energy_cal"  ;
    spec_energy_cal =
        new spectrum_1D(name,
                        name,
                        (id_x_or_y ? 256 : 4096),  // x can be short (256 channels)
                        0,
                        (id_x_or_y ? 256 : 4096),  // x can be short (256 channels)
                        folder, "", name_of_this_element +
                        "_energy_cal_when_fired");
    frs_spectra_ptr->push_back(spec_energy_cal) ;
    //    cout << "END Texotic_one_stripe::create_my_spectra  for " << name_of_this_element << endl;


}
//*********************************************************************
void Texotic_one_stripe::analyse_current_event_pass_1()
{
    //     cout << "analyse_current_event() for "
    //     << name_of_this_element << endl;

    energy_adjusted = 0 ;
    energy_calibrated =  0 ;

    energy_raw  =  *data_ptr ;

    // there are NR_OF_EXOTIC_SAMPLES samples

    double mean = 0 ;
    int how_many = 0 ;
    for(int i = 0 ; i < NR_OF_EXOTIC_SAMPLES ; i++)
    {
        increment_oscilloscope_matrix(my_id_for_osciloscope_matrix + i, data_ptr[i]);
        //         increment_oscilloscope_matrix ( 400, 500 );

        if(sample_participates_in_mean[i])
        {
            mean += data_ptr[i];
            how_many++;
            //             cout << "Sample " << i << " value = " << data_ptr[i] << endl;
        }
    }
    if(how_many)
    {
        mean /=  how_many;
    }
    //     cout << "mean value = " << mean << endl;

    // calculating the mean value

    energy_raw = int (mean);

    if(!energy_raw)
    {
        mark_not_fired();
        return;
    }

    // cout << "Absorber  one stripe event data "
    //   << "  id_module= " << id_module
    //   << " id_x_or_y= " <<  id_x_or_y
    //   << " id_stripe= " << id_stripe
    //   << ", value = " << dec << energy_raw << endl;

    flag_fired = true;
    spec_energy_raw->manually_increment(energy_raw);
    // Piotr wants to have the incrementer similar to this what
    // increments fan spectrum
    pseudo_fan = // (400*id_module) +(20*id_x_or_y) +
        id_stripe + 100 ;   // we avoid 0 ?


    //     if ( energy_raw )   cout << name_of_this_element
    //         << "  energy_raw = " << energy_raw;

}
//*********************************************************************
void Texotic_one_stripe::analyse_current_event_pass_2()
{
//       cout << "analyse_current_event() for "
//       << name_of_this_element << endl;

    energy_adjusted = energy_raw + jitter_correction ;
    spec_energy_adjusted->
    manually_increment(energy_adjusted);

    previous_energy_mean = energy_raw;


    // calibration of the energy -------------------------------
    energy_calibrated =
        (energy_cal_factors[1]  *
         (energy_raw + randomizer::randomek()))
        + energy_cal_factors[0]  ;

    spec_energy_cal->
    manually_increment(energy_calibrated);

// cout
// << " energy_cal_factors[0] = " << energy_cal_factors[0]
// << " energy_cal_factors[1] = " << energy_cal_factors[1]
// << "energy_raw = " << energy_raw<< " energy_adjusted= " << energy_adjusted
// << " energy_calibrated = " << energy_calibrated
// << endl;

}
//********************************************************************
//********************************************************************
void  Texotic_one_stripe::make_preloop_action(ifstream & plik)
{
    your_data_will_be_here(NULL);     // later must be set by a separate function

    // reading the calibration factors

    // in this file we look for a string
    int how_many_factors = 2 ;   // max linear calibration

    // implantation calibration factors ==============================
    string  slowo = name_of_this_element +
                    "_energy_cal_factors" ;

    Tfile_helper::spot_in_file(plik, slowo);

    energy_cal_factors.clear() ;
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        // otherwise we read the data
        double value ;
        plik >> zjedz >> value ;



        if(!plik)
        {
            string mess ;
            mess += "I found keyword: " + slowo
                    + ", but error was in reading its value." ;
            throw mess ;

        }
        // if ok
        energy_cal_factors.push_back(value) ;
    }

    if(energy_cal_factors.size() < 2)
    {
        string mess = "It should be at least 2 calibration factors "
                      " for (implantation) energy calibration of " +
                      name_of_this_element ;
        throw mess ;
    }

    /*
     cout << "Calibration factors read for" << name_of_this_element
     << " found with the keyword " << slowo << ", are:  " ;
      for(unsigned int i = 0 ; i < energy_cal_factors.size() ; i++)
        cout << "   " << energy_cal_factors[i] ;

      cout << "  <--- " << endl;
      */
}
//************************************************************************************
void  Texotic_one_stripe::increment_oscilloscope_matrix(int x, int y)
{
    owner_det-> increment_oscilloscope_matrix(x, y);
}
#endif  // #if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
