#include "Tminiball_cryostat.h"
#include "Tminiball.h"
#include <algorithm>
#include <vector>
#include "Tnamed_pointer.h"
Tminiball *Tminiball_cryostat::miniball_pointer ; // static pointer
int Tminiball_cryostat::all_miniball_cryostats_multiplicity ;
int Tminiball_cryostat::max_fold;



bool Tminiball_cryostat::adjacent_mode;
bool Tminiball_cryostat::nonadjacent_mode;
bool Tminiball_cryostat::sum_all_mode;

int Tminiball_cryostat::primary_seg_algorithm ;
int Tminiball_cryostat::Eprimary_threshold;

//int local_mult_tmp ;
//***********************************************************************
Tminiball_cryostat::Tminiball_cryostat(string name, int nr) : name_of_this_element(name), id_cryostat(nr)
{
    //cout<< "TCLUSTER"<<endl;
    //  cout << "\n\nConstructor of " << _name
    //        << ", my addres is " << this << endl ;

    const int HOW_MANY_DETECTORS = 3 ;
    for(int i = 0 ; i < HOW_MANY_DETECTORS ; i++)
    {
        //cout << "before creating and pushing elementary_det " << i << endl ;
        crys.push_back(new Tminiball_elementary_det(this, i + 1));
        //cout << "Now " << cris.size()
        //     << " det are on the elementary_det list " << endl;
    }
    event_data_ptr = 0 ;  // to be set during pre_loop

    create_my_spectra();
    named_pointer[name_of_this_element + "_multiplicity"]
    = Tnamed_pointer(&this_cryostat_multiplicity, 0, this) ;

    // addbacks ------------

    named_pointer[name_of_this_element + "_core_energy_addback_cal"]
    = Tnamed_pointer(&core_energy_addback_cal, &flag_addback_incrementer_valid, this) ;


    named_pointer[name_of_this_element + "_core_energy_addback_doppler"]
    = Tnamed_pointer(&core_energy_addback_doppler, &flag_addback_incrementer_valid, this) ;


#ifdef NIGDY       // untill the addback is really invented, we do not confuse people with such incrementer

    // ALLs
    //------------------------------
    string entry_name =  "ALL_miniball_cryostat_core_energy_addback_cal_when_successful" ;
    Tmap_of_named_pointers::iterator   pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&core_energy_addback_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&core_energy_addback_cal, &flag_addback_incrementer_valid, this) ;


    //-------------
    entry_name =  "ALL_miniball_cryostat_core_energy_addback_doppler_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&core_energy_addback_doppler, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&core_energy_addback_doppler, &flag_addback_incrementer_valid, this) ;
#endif

}
//************************************************************************
void Tminiball_cryostat::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    miniball_spectra_ptr =
        miniball_pointer -> address_spectra_miniball() ;

    string folder = "cryostats/" + give_detector_name() ;


    string name = name_of_this_element ;
    name += "_core_energy_cal_addback"  ;
    spec_core_energy_addback_cal = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);


    name = name_of_this_element ;
    name += "_core_energy_doppler_corrected_addback"  ;
    spec_core_energy_addback_doppler = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);


    miniball_spectra_ptr->push_back(spec_core_energy_addback_cal) ;
    miniball_spectra_ptr->push_back(spec_core_energy_addback_doppler) ;

}
//**********************************************************************
void Tminiball_cryostat::analyse_current_event()
{

    // WARNING function  below is never called,
    // instead we jump directly from Tminiball into Tminiball_cryostat
    // (because we know which fired - from unpacking process)

    // there is a function  reset_variables() where some flags and variables
    // needed for addback are set

//  Tminiball::tmp_event_calibrated->mib_energy_cal[0][1] = 77;  // = energy_calibrated[0];

//miniball_pointer ->Tminiball::tmp_event_calibrated->mib_energy_cal[0][1] = 77;  // = energy_calibrated[0];

#ifdef NIGDY
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        crys[i]->analyse_current_event() ;
    }
#endif // NIGDY

}
//**************************************************************************
void  Tminiball_cryostat::make_preloop_action(ifstream & s)
{
    check_legality_of_data_pointers();
    //cout << "Reading the calibration for " << name_of_this_element << endl ;

    // in this file we look for a string

    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        crys[i]->make_preloop_action(s) ;
    }
    cout << "Preloop for cryostats successfully finished ..." << endl;
    // NOW WE CAN READ GATES

    // but this is in a different file....

    //cout << " reading the gates for " << name_of_this_element << " " << endl ;

#ifdef NIGDY
    string gates_file = "gates/cryostat_sum_gates.txt" ;
    try
    {
        ifstream plik_gates(gates_file.c_str());
        if(! plik_gates)
        {
            string mess = "I can't open  file: " + gates_file  ;
            throw mess ;
        }
        x_sum_gate[0] = find_in_file(plik_gates, name_of_this_element  + "_sumx_low");
        x_sum_gate[1] = find_in_file(plik_gates, name_of_this_element  + "_sumx_high");
        y_sum_gate[0] = find_in_file(plik_gates, name_of_this_element  + "_sumy_low");
        y_sum_gate[1] = find_in_file(plik_gates, name_of_this_element  + "_sumy_high");
    }
    catch(string sss)
    {
        cerr << "Error while reading calibration file "
             << gates_file
             << ":\n"
             << sss << endl  ;
        exit(-1) ; // is it healthy ?
    }
#endif
}
//**************************************************************************
void  Tminiball_cryostat::check_legality_of_data_pointers()
{
    // if any of data pointers was forgotten to set
    if(!event_data_ptr)
    {
        cout << "Error. in "
             << name_of_this_element
             << " object: You forgot to call its function:\n"
             << "Tminiball_cryostat::set_event_data_pointers "
             "\n   inside the Tminiball::make_preloop_action() "
             "\nPlease change it and recompile the program" << endl ; ;
        exit(-1) ;
    }
}
//**************************************************************************
void Tminiball_cryostat::set_event_data_pointers(int (*ptr)[7][8])
{

    // pointer to this place in the unpacked event,
    // where is the data for this cryostat
    event_data_ptr = ptr;
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        //      cout << "elementary_det nr " << i << endl ;
        //      cout << "Just to proove energy "
        //       << ((* event_data_ptr)[i][0])
        //       << ", time "
        //       << ((* event_data_ptr)[i][1])
        //       << endl ;
        crys[i]->set_event_ptr_for_elementary_det_data(event_data_ptr[i]) ;
    }
}
//**************************************************************************
void Tminiball_cryostat::calculate_addback()
{
    return ; // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    core_energy_addback_cal = 0;
    core_energy_addback_doppler = 0;

    flag_addback_incrementer_valid = 0;


    bool  flag_addback_performed = 0;
    int adjacent_situation = 0;
    int noadjacent_situation = 0;
    int previous_id = 5;
    int holes = 0;

    double doppler_factor;

    crys[first_miniball_elementary_det_nb]->calculate_chosen_doppler_algorithm();

    if(crys[first_miniball_elementary_det_nb]->flag_doppler_successful)
        doppler_factor = crys[first_miniball_elementary_det_nb]->doppler_factor;
    else doppler_factor = 0;



    //cout << "ADDING BACK "<<name_of_this_element<<endl;
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        if(crys[i]->flag_good_data)
        {
            if(!crys[previous_id]->flag_good_data) holes ++;

            core_energy_addback_cal +=  crys[i]->give_energy_calibrated(0) ;
            core_energy_addback_doppler +=  crys[i]->give_energy_calibrated(0) * doppler_factor;

            flag_addback_performed = true;

            //       cout << name_of_this_element <<  ", for cry " << i
            //        << "SUMMING  energ4 = " << crys[i]->give_energy4_calibrated()
            //         << ", energ4 doppler  = " << crys[i]->give_energy4_calibrated()
            ////          << " addback4MeV= " << addback4MeV
            ////          << " core_energy_addback_doppler= " << core_energy_addback_doppler
            //          << endl ;

            /*cout <<  i << " "

            <<" "<< crys[i]->give_energy20_calibrated()
            <<" "<< first_miniball_elementary_det_nb
            <<" "<< addback4MeV << endl; */
        }  // endif good data
        previous_id = i;
    } // end for elementary_dets

    //    cout << "FINALLY, " << name_of_this_element
    //                      << ", addback4MeV= " << addback4MeV
    //                      << ", core_energy_addback_doppler= " << core_energy_addback_doppler << endl ;



    if(flag_addback_performed)
    {
        //    spec_energy4_addback_sum_all->manually_increment( addback4MeV ) ;
        //    spec_energy4_addback_doppler_sum_all->manually_increment( core_energy_addback_doppler ) ;
        //    spec_energy20_addback_sum_all->manually_increment( addback20MeV ) ;
        //    spec_energy20_addback_doppler_sum_all->manually_increment( addback20MeV_doppler ) ;

        if(holes < 2 || crys[6]->flag_good_data)
        {
            adjacent_situation = 1;

            //        spec_energy4_addback_adjacent->manually_increment( addback4MeV ) ;
            //        spec_energy4_addback_doppler_adjacent->manually_increment( core_energy_addback_doppler ) ;
            //        spec_energy20_addback_adjacent->manually_increment( addback20MeV ) ;
            //        spec_energy20_addback_doppler_adjacent->manually_increment( addback20MeV_doppler ) ;
        }
        else
        {
            noadjacent_situation = 1;
            //        spec_energy4_addback_nonadjacent->manually_increment( addback4MeV ) ;
            //        spec_energy4_addback_doppler_nonadjacent->manually_increment( core_energy_addback_doppler ) ;
            //        spec_energy20_addback_nonadjacent->manually_increment( addback20MeV ) ;
            //        spec_energy20_addback_doppler_nonadjacent->manually_increment( addback20MeV_doppler ) ;
        }
    }





    if(flag_addback_performed
            &&
            (
                (adjacent_situation && adjacent_mode)
                || (noadjacent_situation && nonadjacent_mode)
                ||  sum_all_mode
            )
      )
    {
        flag_addback_incrementer_valid = true;

        spec_core_energy_addback_cal->manually_increment(core_energy_addback_cal) ;
        miniball_pointer->increm_spec_total_core_energy_calibrated_addbacked(core_energy_addback_cal) ;

        // incrementing ADDBACKED of the spectrum
        if(doppler_factor != 0)
        {
            spec_core_energy_addback_doppler->manually_increment(core_energy_addback_doppler) ;
            miniball_pointer->increm_spec_total_core_energy_doppler_addbacked(core_energy_addback_doppler);

        }



    }

    //cout << Tminiball_cryostat::adjacent_mode << " "<< Tminiball_cryostat::max_fold<<endl;

}
//**********************************************************************
//*********************************************************************
//*******************************************************************************************
/** No descriptions */
void Tminiball_cryostat:: reset_variables()
{

    core_energy_addback_cal = 0;
    core_energy_addback_doppler = 0;
    flag_addback_incrementer_valid = 0;

}
