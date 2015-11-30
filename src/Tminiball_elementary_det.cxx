#include "Tminiball_elementary_det.h"
#include "Tminiball_cryostat.h"


#include <sstream>
using namespace std;

#include <algorithm>
#include "Tminiball.h"
#include "Ttarget.h"
#include "Tfrs.h"
#include "Thector.h"
#include "Tnamed_pointer.h"
#include "Tself_gate_mib_descr.h"
#include "TIFJAnalysis.h" // for verbose mode

#include "TIFJCalibratedEvent.h"

Tminiball * Tminiball_elementary_det::miniball_pointer ; // static pointer
Ttarget * Tminiball_elementary_det::target_ptr ;      // for faster acces of the doppler correction

//int randomizer::randomizer_word  ;  // static

// enums declared inside the Tminiball_elementary_det class
type_of_tracking_requirements   Tminiball_elementary_det::tracking_position_on_target ;
type_of_cate_requirements       Tminiball_elementary_det::tracking_position_on_cate ;
type_of_doppler_algorithm       Tminiball_elementary_det::which_doppler_algorithm ;
bool   Tminiball_elementary_det::flag_beta_from_frs ; // which beta to use, frs or fixed
double Tminiball_elementary_det::beta_fixed_value ;  // if fixed, here it is
double Tminiball_elementary_det::energy_deposit_in_target ; // bec. beta on the other side
// of the target will be slower

int Tminiball_elementary_det::multiplicity ;
//int Tminiball_elementary_det::threshold_to_multiplicity ; // from this energy we will count it as "fired" !
double Tminiball_elementary_det::sum_en4_cal ;
double Tminiball_elementary_det::sum_en4_dop ;

bool Tminiball_elementary_det::flag_increment_segment_energy_cal_with_zero  ;
bool Tminiball_elementary_det::flag_increment_time_cal_with_zero  ;


bool Tminiball_elementary_det::flag_good_core_energy_requires_threshold  ;
double Tminiball_elementary_det::core_energy_threshold  ;

bool Tminiball_elementary_det::flag_good_time_requires_threshold  ;
double Tminiball_elementary_det::good_time_threshold ;


// static data for artitificaial europium lines
//double Tminiball_elementary_det::europ_spectrum[4096];
//bool Tminiball_elementary_det::flag_europ_read ;


//extern int local_mult_tmp ;

#define dcout   if(0)cout

//**************************************************************************
Tminiball_elementary_det::Tminiball_elementary_det(Tminiball_cryostat *parent_ptr, int nr): parent_cryostat(parent_ptr)
{
    //  cout << "constructor of elementary_det"
    //        << ", addres of parent is "
    //        << parent_cryostat
    //        << ", my address is " << this
    //        << endl;

    id_number = nr ;
    ostringstream sss ;
    sss << parent_cryostat->give_detector_name()
        << ""
        << ((char)('A' + (id_number - 1)));
    //<< id_number ; //    << ends ;  <------------------ was an error for string

    name_of_this_element = sss.str() ;



//  if(!flag_europ_read)
//  {
//      int fake;
//      ifstream plik("eu152.txt");
//      for(int i = 0 ; i < 4096 ; i++)
//      {
//          plik >> fake >> europ_spectrum[i] ;
//      }
//
//      flag_europ_read = true;
// }
    // to be able to increment total fan spectrum we must know which
    // cryostat it is

    // Take the last char from cryostat name and compare it with following
    // list
    string nazwa = parent_cryostat->give_detector_name();
    char last_char = nazwa[nazwa.size() - 1] ;

    cryostat_nr_from_0 = string("12345678").find(last_char) ;
//  cout << "This elementary_det has numbers " << cryostat_nr_from_0 << ", "
//      << id_number << endl;

    energy_data = 0  ;
    time_data = 0 ;

    doppler_factor = 1;
    create_my_spectra();


    named_pointer[name_of_this_element + "_core_energy_raw_when_fired"]
    = Tnamed_pointer(&energy_raw[0], &flag_fired, this) ;
    named_pointer[name_of_this_element + "_time_raw_when_fired"]
    = Tnamed_pointer(&time_raw, &flag_fired, this) ;
    named_pointer[name_of_this_element + "_core_energy_cal_when_fired"]
    = Tnamed_pointer(&energy_calibrated[0], &flag_fired, this) ;
    named_pointer[name_of_this_element + "_time_cal_when_fired"]
    = Tnamed_pointer(&time_calibrated, &flag_fired, this) ;
    named_pointer[name_of_this_element + "_core_energy_cal_when_good"]
    = Tnamed_pointer(&energy_calibrated[0], &flag_good_data, this) ;
    named_pointer[name_of_this_element + "_time_cal_when_good"]
    = Tnamed_pointer(&time_calibrated, &flag_good_data, this) ;
    named_pointer[name_of_this_element + "_core_energy_doppler_when_successful"]
    = Tnamed_pointer(&energy_doppler_corrected, &flag_doppler_successful, this) ;

    named_pointer[name_of_this_element + "_primary_segment_when_fired"]
    = Tnamed_pointer(&primary_segment, &flag_fired, this) ;



    //----------------------
    // TOTALS

    // find in the map the entry with such a name -
    // if there is not - create it
    // if there is already - open it and add to it

    string entry_name =  "ALL_miniball_cryostat_crys_core_energy_cal_when_good" ;
    Tmap_of_named_pointers::iterator pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&energy_calibrated[0], &flag_good_data, this) ;
    else
        named_pointer[entry_name] = Tnamed_pointer(&energy_calibrated[0], &flag_good_data, this) ;


    //----------------------------
    entry_name =  "ALL_miniball_cryostat_crys_time_cal_when_good" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&time_calibrated, &flag_good_data, this) ;
    else
        named_pointer[entry_name] = Tnamed_pointer(&time_calibrated, &flag_good_data, this) ;
    //------------------------------
    entry_name =  "ALL_miniball_cryostat_crys_core_energy_doppler_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&energy_doppler_corrected, &flag_doppler_successful, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&energy_doppler_corrected, &flag_doppler_successful, this) ;


}
//************************************************************************
void Tminiball_elementary_det::create_my_spectra()
{

    //cout << "before creating the spectra " << endl ;

    // the spectra have to live all the time, so here we can create
    // them during  the constructor

    vector<spectrum_abstr*> *miniball_spectra_ptr =
        miniball_pointer -> address_spectra_miniball() ;

    string folder = "miniball_cryostats/" + parent_cryostat->give_detector_name() ;

    // energy -------------------------------------
    string name = name_of_this_element ;
    for(int seg = 0 ; seg < 7 ; seg++)
    {
        name = name_of_this_element  + char('0' + seg);
        name += "_energy_raw"  ;

        spec_energy_raw[seg] = new spectrum_1D(
            name,
            name,
//          8192-1, 1, 2*16384,
            // Now signals raw energy is divided by 8  (64 K - > 8K)
            8191, 1, 8192, // here the binning must start from 0, because Carl Wheldon automatic routine expect this
            folder);
        miniball_spectra_ptr->push_back(spec_energy_raw[seg]) ;

        name = name_of_this_element  + char('0' + seg);

        // energy cal ------------------------------------

        name += "_energy_cal"  ;
        spec_energy_cal[seg] = new spectrum_1D(
            name,
            name,
            4096, 0, 4096,
            folder);
        miniball_spectra_ptr->push_back(spec_energy_cal[seg]) ;

    } //for seg



    name = name_of_this_element ;

    name += "_energy_dop"  ;
    spec_energy_doppler = new spectrum_1D(
        name,
        name,
        4096, 0, 4096,
        folder);
    miniball_spectra_ptr->push_back(spec_energy_doppler) ;


    // time ------------------------------------------
    name = name_of_this_element ;
    name += "_time_raw"  ;
    spec_time_raw = new spectrum_1D(name,
                                    name,
                                    8191, 1, 8192,
                                    folder);
    miniball_spectra_ptr->push_back(spec_time_raw) ;


    name = name_of_this_element ;
    name += "_time_cal"  ;
    spec_time_cal = new spectrum_1D(name,
                                    name,
                                    8192, 0, 8192,
                                    folder);
    miniball_spectra_ptr->push_back(spec_time_cal) ;
    name = name_of_this_element  + "_theta_mrad"  ;
    spec_theta_miliradians = new spectrum_1D(name,
            name,
            1000, 0, 100,
            folder);
    miniball_spectra_ptr->push_back(spec_theta_miliradians) ;



}
//**********************************************************************
void Tminiball_elementary_det::analyse_current_event()
{
//  cout << " analyse_current_event()  for "
//        << name_of_this_element
//        << endl ;

#ifdef MINIBALL_PRESENT

    // checking if it is something (not zero) -- is made during unpacking

//  zeroing_good_flags();

    //----------------
    // so it fired !
    flag_fired = true ;


    // At first the time, because it is only once
    // I do not know if it is necessery at all

    time_raw =  *time_data ;

    // time calibration --------------------------------
    time_calibrated = 0 ;
    double time_tmp = time_raw + randomizer::randomek() ;
    if(time_calibr_factors[2] == 0) // simple linear calibration
    {
        time_calibrated = time_calibr_factors[0]
                          + (time_tmp * time_calibr_factors[1]) ;
    }
    else for(unsigned int i = 0 ; i < time_calibr_factors .size() ; i++)
        {
            time_calibrated  += time_calibr_factors [i] * pow(time_tmp, (int)i) ;
        }

    energy_doppler_corrected = 0 ;

    // 7 signals from the 7 segments of the elementary detector
    primary_segment = 0 ;
    double max_energy_seg = 0 ;

    for(int seg = 0 ; seg < 7 ; seg++)
    {
        energy_raw[seg] = (energy_data[seg][1]  >> 3)  ;   // 64K --->8 K
//      cout << "Time data = energy_data[" << seg << "][0]  = "/d/rising01/rising00/dec2004/frs_calib0010.lmd
//          << energy_data[seg][0] << endl;


        if(RisingAnalysis_ptr->is_verbose_on())
        {
            cout  << name_of_this_element
                  << seg << "_energy_raw =  " << energy_raw[seg]
                  << endl;
        }

        /* Note: incrementing the raw spectra is done only
             when multiplicity in this cryostat is 1.
             all this is done from Tminiball class, by
             calling Tminiball_cryostat::calculate_single_spectrum()
             The "missing" instructions which were here - are moved just there
        */


        // energy calibration -------------------------------
        energy_calibrated[seg] = 0 ;
        double energy_tmp = (energy_raw[seg]) + randomizer::randomek() ;
        if(energy_calibr_factors[seg][2] == 0)
        {
            energy_calibrated[seg] = energy_calibr_factors[seg][0]
                                     + (energy_tmp * energy_calibr_factors[seg][1]) ;
        }
        else for(unsigned int i = 0 ; i < energy_calibr_factors[seg].size() ; i++)
            {
                energy_calibrated[seg] +=
                    energy_calibr_factors[seg][i] * pow(energy_tmp, (int)i) ;
            }
        if(seg != 0  && max_energy_seg < energy_calibrated[seg])
        {
            max_energy_seg =  energy_calibrated[seg] ;
            primary_segment = seg ;
        }




        // ---incr spectra
        // ---> fragment moved into the function
        //      Tminiball_cryostat::calculate_single_spectrum

        //================================
        // GOOD    only for core ?
        //================================
        if(seg == 0 &&

                (flag_good_core_energy_requires_threshold  ?
                 (energy_calibrated[0] >  core_energy_threshold)
                 :
                 true)
                &&
                (flag_good_time_requires_threshold  ?
                 (time_calibrated >  good_time_threshold)
                 :
                 true)
          )
        {
            flag_good_data = true;
            Tminiball_elementary_det::multiplicity++ ;
        }

        /*cout
            <<"Miniball " << name_of_this_element <<endl
            << energy_calibrated[seg]<< " " <<time_calibrated[seg]
            <<" "<<energy20_calibrated<<endl;
        */

//#ifdef NIGDY

        // ntuple, so far blocked
        Tminiball::tmp_event_calibrated->mib_energy_cal[cryostat_nr_from_0][id_number - 1] = energy_calibrated[0];
        Tminiball::tmp_event_calibrated->mib_time_cal[cryostat_nr_from_0][id_number - 1] = time_calibrated;
    }


#endif   // Minbiall_present

//  cout << " end of analyse_current_event()  for "
//        << name_of_this_element
//        << endl ;

}




//Del by KDevelop: //***********************************************************************************************************
//Del by KDevelop: void Tminiball_elementary_det::calculate_single_spectrum_old()
//Del by KDevelop: {
//Del by KDevelop: #ifdef NIGDY
//Del by KDevelop:   /*cout
//Del by KDevelop:     <<"Cluster " << cryostat_nr_from_0 <<" "<<id_number<<endl
//Del by KDevelop:     << energy_calibrated<<" "<<time_calibrated
//Del by KDevelop:     <<" "<<energy20_calibrated<<endl;
//Del by KDevelop:   */
//Del by KDevelop:
//Del by KDevelop:   int fan_offset = (100*cryostat_nr_from_0) +(10*id_number) ;
//Del by KDevelop:
//Del by KDevelop:
//Del by KDevelop:   if(*energy_data) spec_fan->manually_increment(fan_offset + 0);
//Del by KDevelop:   if(*time_data) spec_fan->manually_increment(fan_offset + 2);
//Del by KDevelop:   if(*energy_data
//Del by KDevelop:       && *time_data)
//Del by KDevelop:     spec_fan->manually_increment(fan_offset + 4);
//Del by KDevelop:
//Del by KDevelop:   spec_time_raw->manually_increment(time_raw )  ;
//Del by KDevelop:   spec_energy_raw->manually_increment(energy_raw);
//Del by KDevelop:   sum_en4_cal += energy_calibrated ;
//Del by KDevelop:
//Del by KDevelop:
//Del by KDevelop:   if(energy_raw !=0  || flag_increment_segment_energy_cal_with_zero)
//Del by KDevelop:  {
//Del by KDevelop:      spec_energy_cal->manually_increment(energy_calibrated ) ;
//Del by KDevelop:      miniball_pointer->increm_spec_total_energy_calibrated(energy_calibrated) ;
//Del by KDevelop:  }
//Del by KDevelop:
//Del by KDevelop:   if(time_raw !=0  || flag_increment_time_cal_with_zero)
//Del by KDevelop:  {
//Del by KDevelop:      spec_time_cal->manually_increment(time_calibrated )  ;
//Del by KDevelop:      miniball_pointer->increm_spec_total_time_calibrated(time_calibrated) ;
//Del by KDevelop:  }
//Del by KDevelop:   if( flag_good_data == true )
//Del by KDevelop:  {
//Del by KDevelop:      calculate_chosen_doppler_algorithm();
//Del by KDevelop:      if(flag_doppler_successful)
//Del by KDevelop:      {
//Del by KDevelop:          energy_doppler_corrected=energy_calibrated * doppler_factor;
//Del by KDevelop:
//Del by KDevelop:          spec_energy_doppler->manually_increment(energy_doppler_corrected);
//Del by KDevelop:          spec_total_energy_doppler->
//Del by KDevelop:              manually_increment(energy_doppler_corrected);
//Del by KDevelop:          sum_en4_dop += energy_doppler_corrected  ;
//Del by KDevelop:
//Del by KDevelop:          spec_theta_miliradians->
//Del by KDevelop:              manually_increment(theta_radians_between_gamma_vs_scattered_particle);
//Del by KDevelop:          spec_total_theta_miliradians->
//Del by KDevelop:              manually_increment(theta_radians_between_gamma_vs_scattered_particle);
//Del by KDevelop:      }
//Del by KDevelop:      else doppler_factor = 0 ;
//Del by KDevelop:  }
//Del by KDevelop:
//#endif
//}



//**************************************************************************
// This name of the fuction is not good. Should be:
//  chose_and_calculate_doppler()
void  Tminiball_elementary_det::calculate_chosen_doppler_algorithm()
{
    switch(which_doppler_algorithm)
    {
//  case polar_coordinates:
//      if(target_ptr->is_doppler_possible()) doppler_corrector_polar();
//      break ;
    case tracked_algorithm:
        //if(target_ptr->is_doppler_possible()) doppler_corrector_cartesian();
        if(target_ptr->is_doppler_possible()) doppler_corrector_TVector3();
        break;
    case basic_algorithm:
        doppler_corrector_basic();
        break;
    default:
        cout
                << "Error, the type of doppler correction algorithm was not selected "
                "in the file: option/doppler.options\n" << endl ;
        break ;
    }
}
//**************************************************************************
void  Tminiball_elementary_det::make_preloop_action(ifstream & s)
{
    check_legality_of_data_pointers();

    // total spectra are in the miniball object
    spec_total_theta_miliradians = miniball_pointer->spec_total_theta_miliradians ;
    spec_total_core_energy_doppler = miniball_pointer->spec_total_core_energy_doppler ;
    spec_fan = miniball_pointer->spec_fan;

    // cout << "Reading the calibration for the elementary_det "
    //        << name_of_this_element
    //        << endl ;

    // in this file we look for a string
    energy_calibr_factors.clear() ;
    energy_calibr_factors.resize(7);

    int how_many_factors = 3 ;


    string slowo ; // = name_of_this_element + "_energy_factors" ;

    for(int seg = 0 ; seg < 7 ; seg++)
    {

        string slowo = name_of_this_element +  char('0' + seg) + "_energy_factors" ;

        // first reading the energy calibration
        Tfile_helper::spot_in_file(s, slowo);
//  cout << "Reading the " << slowo << endl;

        energy_calibr_factors[seg].resize(how_many_factors);
        for(int i = 0 ; i < how_many_factors ; i++)
        {
            // otherwise we read the data
            double value ;
            s >> value ;
            if(!s)
            {
                Treading_value_exception capsule ;
                capsule.message =
                    "I found keyword '" + slowo
                    + "' but error was in reading its value." ;
                throw capsule ;
            }
            // if ok
//      cout << "pushing back the value " << value << " as factors[" << seg << "][" << i << "]" << endl;
//      cout << "energy_calibr_factors.size() = " << energy_calibr_factors.size() << endl;
//      cout << "energy_calibr_factors[" << seg<< "].size() = " << energy_calibr_factors[seg].size() << endl;
            energy_calibr_factors[seg][i] = value  ;
        }

    }
    // reading the time calibration factors --------------
    time_calibr_factors.clear() ;
    slowo = name_of_this_element + "_time_factors" ;
    Tfile_helper::spot_in_file(s, slowo);
    for(int i = 0 ; i < how_many_factors ; i++)
    {
        double value ;
        s >> value ;
        if(!s)
        {
            Treading_value_exception capsule ;
            capsule.message =
                "I found keyword '" + slowo
                + "' but error was in reading its value." ;
            throw capsule ;
        }
        // if ok
        time_calibr_factors.push_back(value) ;
    }




    // ---------------------------------------------------
    // Geometry: phi, theta
    //----------------------------------------------------

    // but this is in a different file....

    // cout << " reading the geometry angles  for elementary_det "
    //        << name_of_this_element
    //        << endl ;


    //-----------------

    // here we read the Geometry
    //


    string geometry_file = "calibration/miniball_geometry.txt" ;
    try
    {
        ifstream plik_geometry(geometry_file.c_str());
        if(! plik_geometry)
        {
            cout << "I can't open  file: " << geometry_file  ;
            exit(-1) ;
        }

        phi_geom.clear();
        phi_geom.reserve(7);

        theta_geom.clear();
        theta_geom.reserve(7);

        distance_from_target.clear();
        distance_from_target.reserve(7);

        phi_geom_radians.clear();
        theta_geom_radians.clear();
        phi_geom_radians.reserve(7);
        theta_geom_radians.reserve(7);

        // ============== recalculate phi and theta into x,y,z of the elementary_det
        elementary_det_position_x.clear();
        elementary_det_position_y.clear();
        elementary_det_position_z.clear();

        elementary_det_position_x.reserve(7);
        elementary_det_position_y.reserve(7);
        elementary_det_position_z.reserve(7);


        for(int seg = 0 ; seg < 7 ; seg++)
        {
            string keyword = name_of_this_element ;
            keyword += char('0' + seg);
            keyword +=  "_phi_theta_distance" ;
//          double p, t, d;
            Tfile_helper::spot_in_file(plik_geometry, keyword);
            plik_geometry >> zjedz >> phi_geom[seg]
                          >> zjedz >> theta_geom[seg]
                          >> zjedz >> distance_from_target[seg] ;


            if(!plik_geometry)
            {
                cout << "Error while reading (inside geometry file "
                     << geometry_file
                     << ") phi and theta and distance of"
                     << name_of_this_element
                     << "\nI found keyword '"
                     << keyword
                     << "' but error was while reading its (three) values"
                     << " of phi and theta."
                     << endl  ;
                exit(-1);
            }
//      phi_geom.push_back(p);.
//      theta_geom.push_back(p);.
//      distance_from_target.push_back(p);.

            //distance_from_target *= 10 ; // for rising clusters it was in [cm] so we needed in milimeters !
            phi_geom_radians[seg]   =  M_PI * phi_geom[seg] / 180.0 ;
            theta_geom_radians[seg] =  M_PI * theta_geom[seg] / 180.0;

            // ============== recalculate phi and theta into x,y,z of the elementary_det
            elementary_det_position_x[seg] =
                distance_from_target[seg] * sin(theta_geom_radians[seg]) * cos(phi_geom_radians[seg]) ;
            elementary_det_position_y[seg] =
                distance_from_target[seg] * sin(theta_geom_radians[seg]) * sin(phi_geom_radians[seg]) ;
            elementary_det_position_z[seg] =
                distance_from_target[seg] * cos(theta_geom_radians[seg]);

        }
    }
    catch(Tfile_helper_exception &k)
    {
        cerr << "Error while reading geometry file "
             << geometry_file
             << ":\n"
             << k.message << endl  ;
        exit(-1) ;
    }


    // doppler correction options are read by target and stored
    // in the static members of this clas (Tminiball_elementary_det)

    //give_simulated_Eu_152(4*8192);

//  cout << "Finished reading the calibration and geometry for " << name_of_this_element << endl;
}
//**********************************************************************
void Tminiball_elementary_det::set_event_ptr_for_elementary_det_data(int ptr[7][8])
{
    // now we can define local pointers
    energy_data = ptr ;
    time_data   = &ptr[0][0] ;  // trigtime word
}
//**********************************************************************
void Tminiball_elementary_det::check_legality_of_data_pointers()
{
    if(!energy_data ||
//      !energy20_data ||
            !time_data)
    {
        cerr
                << "Error. in "
                << name_of_this_element
                << " object: You forgot to call its function:\n"
                << "elementary_det::set_event_ptr_for_elementary_det_data(int ptr[3])"
                "\n inside the function "
                "Tminiball_cryostat::set_event_data_pointers(int (* ptr)[7][3])"
                "\nPlease change it and recompile the program"
                << endl ; ;
        exit(-2) ;
    }
}

//*******************************************************************
void Tminiball_elementary_det::set_doppler_corr_options
(int typ_track, int typ_cate_pos, int typ_doppl_algor)
{
    tracking_position_on_target = (type_of_tracking_requirements) typ_track;
    tracking_position_on_cate   = (type_of_cate_requirements) typ_cate_pos;
    which_doppler_algorithm = (type_of_doppler_algorithm)  typ_doppl_algor ;
}
//***************************************************************
/** Based on the simplest algorithm (from Hans-Juergen) */
void Tminiball_elementary_det::doppler_corrector_basic()
{
    // this algorithm below is on the web page ===================
    double beta = target_ptr->give_beta_after_target();

    //energy_doppler_corrected = energy_calibrated
    doppler_factor = (1.0 - beta * cos(theta_geom_radians[primary_segment]))
                     / (sqrt(1.0 - beta * beta));


    flag_doppler_successful = true ;
}
//*******************************************************************************
/** No descriptions */
void Tminiball_elementary_det::zeroing_good_flags()
{
    flag_fired =
        flag_good_data =
            flag_doppler_successful = false ;
}
/******************************************************************************/
bool Tminiball_elementary_det::check_selfgate(Tself_gate_abstract_descr *desc)
{

#ifdef NIGDY
    // this is the multiplicity inside the cryostat where this crystal  belongs
    bool enable_mult_in_cryostat_gate ;
    double mult_in_cryostat_gate[2];
#endif

    // here we assume that this is really germ
    Tself_gate_mib_descr *d = (Tself_gate_mib_descr*)desc ;
    if(d->enable_core_energy_gate)
    {
        if(energy_calibrated[0] < d->core_energy_gate[0]
                ||
                energy_calibrated[0] > d->core_energy_gate[1])
            return false ;
    }

    if(d->enable_time_gate)
    {
        if(time_calibrated < d->time_gate[0] || time_calibrated > d->time_gate[1])
            return false ;
    }
    if(d->enable_geom_theta_gate)
    {
        if(theta_geom[primary_segment] < d->geom_theta_gate[0] || theta_geom[primary_segment] > d->geom_theta_gate[1])
            return false;
    }
    if(d->enable_geom_phi_gate)
    {
        if(phi_geom[primary_segment] < d->geom_phi_gate[0] || phi_geom[primary_segment] > d->geom_phi_gate[1])
            return false;
    }
    if(d->enable_gp_theta_gate)
    {
        if(theta_radians_between_gamma_vs_scattered_particle <  d->gp_theta_gate[0]
                ||
                theta_radians_between_gamma_vs_scattered_particle > d->gp_theta_gate[1])
            return false ;
    }
    if(d->enable_gp_phi_gate)
    {
        if(phi_radians_between_gamma_vs_scattered_particle  < d->gp_phi_gate[0]
                ||
                phi_radians_between_gamma_vs_scattered_particle > d->gp_phi_gate[1])
            return false;
    }
    if(d->enable_mult_in_cryostat_gate)
    {
        int mult =  parent_cryostat->get_this_cryostat_multiplicity();
        if(mult < d->mult_in_cryostat_gate[0]
                ||
                mult > d->mult_in_cryostat_gate[1]
          )
            return false ;
    }
//#endif

    return true ; // when EVETHING was successful
}

//**************************************************************************************************
/** No descriptions */
void Tminiball_elementary_det::increment_elementary_det_spectra_raw_and_fan()
{
    /*cout
      <<  name_of_this_element <<endl
      << energy_calibrated<<" "<<time_calibrated
      <<" "<<energy20_calibrated<<endl;
    */
    int fan_offset = (100 * cryostat_nr_from_0) + (10 * id_number) ;
    if(*energy_data) spec_fan->manually_increment(fan_offset + 0);
    if(*time_data) spec_fan->manually_increment(fan_offset + 2);
    if(*energy_data &&
            *time_data)
        spec_fan->manually_increment(fan_offset + 4);


    spec_time_raw->manually_increment(time_raw)  ;
    for(int seg = 0 ; seg < 7 ; seg++)
        spec_energy_raw[seg]->manually_increment(energy_raw[seg]);


}
//*******************************************************************************************
/** No descriptions */
void Tminiball_elementary_det::increment_elementary_det_spectra_cal_and_doppler()
{
    /*cout
      <<"Cluster " << cryostat_nr_from_0 <<" "<<id_number<<endl
      << energy_calibrated<<" "<<time_calibrated
      <<" "<<energy20_calibrated<<endl;
    */

//  sum_en4_cal += energy_calibrated ;

    for(int seg = 0 ; seg < 7 ; seg++)
    {
        // incrementing calibrated versions of the spectrum

        if(energy_raw[seg] != 0  || flag_increment_segment_energy_cal_with_zero)
        {
            spec_energy_cal[seg]->manually_increment(energy_calibrated[seg]) ;

            if(seg == 0)   miniball_pointer->increm_spec_total_core_energy_calibrated(energy_calibrated[0]) ;
        }
    }

    if(time_raw != 0  || flag_increment_time_cal_with_zero)
    {
        spec_time_cal->manually_increment(time_calibrated)  ;
        miniball_pointer->increm_spec_total_time_calibrated(time_calibrated) ;
    }


    if(flag_good_data == true)
    {

        calculate_chosen_doppler_algorithm();
        if(flag_doppler_successful)
        {
            energy_doppler_corrected = energy_calibrated[0] * doppler_factor;
            spec_energy_doppler->manually_increment(energy_doppler_corrected);
            spec_total_core_energy_doppler-> manually_increment(energy_doppler_corrected);

            spec_theta_miliradians->
            manually_increment(theta_radians_between_gamma_vs_scattered_particle);
            spec_total_theta_miliradians->
            manually_increment(theta_radians_between_gamma_vs_scattered_particle);
        }
    }


}
//*******************************************************************************************
/** No descriptions */
void Tminiball_elementary_det::increment_spect_total_addbacked_cal_and_doppler()
{


    // incrementing calibrated versions of the spectrum

    if(energy_raw != 0  || flag_increment_segment_energy_cal_with_zero)
    {
        //spec_energy_cal->manually_increment(energy_calibrated ) ;
        miniball_pointer->increm_spec_total_core_energy_calibrated_addbacked(energy_calibrated[0]) ;

    }



// Doppler corrected spectra we do only when flag  GOOD is set

    if(flag_good_data == true)
    {

        calculate_chosen_doppler_algorithm();
        if(flag_doppler_successful)
        {
            energy_doppler_corrected = energy_calibrated[0] * doppler_factor;
            miniball_pointer->increm_spec_total_core_energy_doppler_addbacked(energy_doppler_corrected);
        }
    }

}
//****************************************************************************************
/** this function contains the algorithm of the doppler correction
        using the class TVector3 from ROOT library */
void Tminiball_elementary_det::doppler_corrector_TVector3()
{


}
//*********************************************************************************************************************
