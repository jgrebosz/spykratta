#include "Tcluster.h"
#include "Trising.h"
#include <algorithm>
#include <vector>
#include "Tnamed_pointer.h"
#include "Tself_gate_ger_addback_descr.h"

#include "TIFJAnalysis.h" // for verbose pointer

Trising *Tcluster::rising_pointer ; // static pointer
int Tcluster::all_clusters_multiplicity ;
int Tcluster::max_fold;

bool Tcluster::adjacent_mode;
bool Tcluster::nonadjacent_mode;
bool Tcluster::sum_all_mode;

int Tcluster::primary_seg_algorithm ;
int Tcluster::Eprimary_threshold;

// this below are not yet in use --------- for old vxi detectors was not important
double Tcluster::addback_option_max_acceptable_time_diff_to_the_fastest = 9999999;
bool Tcluster::addback_option_max_acceptable_time_diff_is_enabled = false;


//int local_mult_tmp ;
//***********************************************************************
Tcluster::Tcluster(string name, int nr) : name_of_this_element(name), id_cluster(nr)
{
    //cout<< "TCLUSTER"<<endl;
    //  cout << "\n\nConstructor of " << _name
    //        << ", my addres is " << this << endl ;

    const int HOW_MANY_CRISTALS = 7 ;
    for(int i = 0 ; i < HOW_MANY_CRISTALS ; i++)
    {
        //cout << "before creating and pushing crystal " << i << endl ;
        crys.push_back(new Tcrystal(this, i + 1));
        //cout << "Now " << cris.size()
        //     << " det are on the crystal list " << endl;
    }
    event_data_ptr = 0 ;  // to be set during pre_loop

    create_my_spectra();

    // addbacks ------------
    // the name of this element starts with cluster_crys and this
    // would allow to use the self gate for such named incrementer - we can not
    // allow this - there must be special incrementer for this
    // so here we remove this "crys" from the name
    string name_without_crys = name_of_this_element;
    string::size_type pos_cry = name_without_crys.rfind("crys_");
    if(pos_cry == string::npos)
    {
        cout << "Error - the name of the cluster "
             << name_of_this_element << " does not contain a substring 'crys_'"
             << endl;
        exit(0);
    }
    name_without_crys.erase(pos_cry, 5); // "crys_" has 5 characters


    named_pointer[name_without_crys + "_multiplicity"]
    = Tnamed_pointer(&this_cluster_multiplicity, 0, this) ;

#ifdef BGO_PRESENT
    named_pointer[name_without_crys + "_BGO_energy"]
    = Tnamed_pointer(&BGO_energy, 0, this) ;
#endif



    named_pointer[name_without_crys + "_addback_4MeV_cal"]
    = Tnamed_pointer(&addback4MeV_cal, &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_addback_20MeV_cal"]
    = Tnamed_pointer(&addback20MeV_cal, &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_addback_4MeV_doppler"]
    = Tnamed_pointer(&addback4MeV_doppler, &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_addback_20MeV_doppler"]
    = Tnamed_pointer(&addback20MeV_doppler, &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_addback_time_cal"]
    = Tnamed_pointer(&addback_time_cal, &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_atime_cal_of_the_first_crystal_minus_addback_time_cal"]
    = Tnamed_pointer(&time_cal_of_the_first_crystal_minus_addback_time_cal,
                     &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_LR_plus_addback_dt_time_cal"]
    = Tnamed_pointer(&LR_plus_addbacked_dt_time_cal, &flag_addback_incrementer_valid, this) ;

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    named_pointer[name_without_crys + "_time_LR_plus_addback_SR_plus_offsets_difference"]
    = Tnamed_pointer(&time_LR_plus_addbacked_SR_plus_offsets_difference,
                     &flag_addback_incrementer_valid, this) ;



    named_pointer[name_without_crys + "_addback_time_cal_minus_GeOR_cal_when_valid"]
    = Tnamed_pointer(&addback_time_cal_minus_GeOR_cal,
                     &flag_addback_incrementer_valid, this) ;
#endif
    // ALLs
    //------------------------------
    string entry_name =  "ALL_cluster_addback_time_cal_when_successful" ;
    Tmap_of_named_pointers::iterator   pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback_time_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback_time_cal, &flag_addback_incrementer_valid, this) ;


    //------------------------------
    entry_name =  "ALL_cluster_addback_4MeV_cal_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback4MeV_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback4MeV_cal, &flag_addback_incrementer_valid, this) ;


    entry_name =  "ALL_cluster_addback_20MeV_cal_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback20MeV_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback20MeV_cal, &flag_addback_incrementer_valid, this) ;

    //-------------
    entry_name =  "ALL_cluster_addback_4MeV_doppler_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback4MeV_doppler, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback4MeV_doppler, &flag_addback_incrementer_valid, this) ;


    entry_name =  "ALL_cluster_addback_20MeV_doppler_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback20MeV_doppler, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback20MeV_doppler, &flag_addback_incrementer_valid, this) ;

    //----------------

    entry_name =  "ALL_cluster_addback_time_cal_of_the_first_crystal_minus_addback_time_cal_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&time_cal_of_the_first_crystal_minus_addback_time_cal,
                             &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&time_cal_of_the_first_crystal_minus_addback_time_cal,
                           &flag_addback_incrementer_valid, this) ;

    entry_name =  "ALL_cluster_LR_plus_addback_dt_time_cal_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&LR_plus_addbacked_dt_time_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&LR_plus_addbacked_dt_time_cal, &flag_addback_incrementer_valid, this) ;

    //------------------

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

    entry_name =  "ALL_cluster_time_LR_plus_addback_SR_plus_offsets_difference_when_valid" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&time_LR_plus_addbacked_SR_plus_offsets_difference, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&time_LR_plus_addbacked_SR_plus_offsets_difference, &flag_addback_incrementer_valid, this) ;



    entry_name =  "ALL_cluster_addback_time_cal_minus_GeOR_cal_when_valid" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback_time_cal_minus_GeOR_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback_time_cal_minus_GeOR_cal, &flag_addback_incrementer_valid, this) ;
#endif

}
//************************************************************************
void Tcluster::create_my_spectra()
{
    //the spectra have to live all the time, so here we can create
    // them during  the constructor

    rising_spectra_ptr =
        rising_pointer -> address_spectra_rising() ;

    string folder = "clusters/" + give_detector_name() ;



    string name_without_crys = name_of_this_element;
    string::size_type pos_cry = name_without_crys.rfind("crys_");
    if(pos_cry == string::npos)
    {
        cout << "Error - the name of the cluster "
             << name_of_this_element << " does not contain a substring 'crys_'"
             << endl;
        exit(0);
    }
    name_without_crys.erase(pos_cry, 5); // "crys_" has 5 characters




    string name = name_without_crys ;
    name += "_energy4MeV_cal_addback"  ;
    spec_energy4_addback_cal = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);

    name = name_without_crys ;
    name += "_addback_time_cal"  ;
    spec_addback_time_cal = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);

    name = name_without_crys ;
    name += "_energy20MeV_cal_addback"  ;
    spec_energy20_addback_cal = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);

    name = name_without_crys ;
    name += "_energy4MeV_doppler_corrected_addback"  ;
    spec_energy4_addback_doppler = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);

    name = name_without_crys ;
    name += "_energy20MeV_doppler_corrected_addback"  ;
    spec_energy20_addback_doppler = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);

    rising_spectra_ptr->push_back(spec_energy4_addback_cal) ;
    rising_spectra_ptr->push_back(spec_energy20_addback_cal) ;
    rising_spectra_ptr->push_back(spec_addback_time_cal);   // was missing ?????????????
    rising_spectra_ptr->push_back(spec_energy4_addback_doppler) ;
    rising_spectra_ptr->push_back(spec_energy20_addback_doppler) ;

    // temporary for adjacent/nonadjacent situation


    name = name_of_this_element ;
    name += "_BGO_energy_raw"  ;
    spec_BGO_energy_raw = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);
    rising_spectra_ptr->push_back(spec_BGO_energy_raw) ;

    //
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    name = name_without_crys ;
    name += "_LR_plus_addbacked_dt_time_cal"  ;
    //cout << "Creating spectrum " << name << endl;

    spec_LR_plus_addbacked_dt_time_cal = new spectrum_1D(
        name,
        name,
        4096, 0, 4096,
        folder);
    rising_spectra_ptr->push_back(spec_LR_plus_addbacked_dt_time_cal) ;
    //-------------
    name = name_without_crys ;
    name += "_time_cal_of_the_first_crystal_minus_addback_time_cal"  ;
    //cout << "Creating spectrum " << name << endl;

    spec_time_cal_of_the_first_crystal_minus_addback_time_cal = new spectrum_1D(
        name,
        name,
        4096, 0, 4096,
        folder);
    rising_spectra_ptr->push_back(
        spec_time_cal_of_the_first_crystal_minus_addback_time_cal) ;


#endif


}
//**********************************************************************
void Tcluster::analyse_current_event()
{

    // WARNING function  below is never called,
    // instead we jump directly from Trising into Tcrystal
    // (because we know which fired - from unpacking process)

    // there is a function  reset_variables() where some flags and variables
    // needed for addback are set

#ifdef NIGDY
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        crys[i]->analyse_current_event() ;
    }
#endif // NIGDY

}
//**************************************************************************
void  Tcluster::make_preloop_action(istream & s)
{
    check_legality_of_data_pointers();
    //cout << "Reading the calibration for " << name_of_this_element << endl ;

    // in this file we look for a string

    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        crys[i]->make_preloop_action(s) ;
    }

    // NOW WE CAN READ GATES

    // but this is in a different file....

    //cout << " reading the gates for " << name_of_this_element << " " << endl ;

#ifdef NIGDY
    string gates_file = "gates/cluster_sum_gates.txt" ;
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
void  Tcluster::check_legality_of_data_pointers()
{
    //    // if any of data pointers was forgotten to set
    //    if(event_data_ptr == 0)
    //    {
    //        cout << "Error. in "
    //            << name_of_this_element
    //            << " object: You forgot to call its function:\n"
    //            << "Tcluster::set_event_data_pointers "
    //            "\n   inside the Trising::make_preloop_action() "
    //            "\nPlease change it and recompile the program" << endl ; ;
    //        exit(-1) ;
    //    }
}
//**************************************************************************
void Tcluster::set_event_data_pointers(int (* ptr)[7][3])
{

    //    // pointer to this place in the unpacked event,
    //    // where is the data for this cluster
    //    event_data_ptr = ptr;
    //    for(unsigned int i = 0 ; i < crys.size() ; i++)
    //    {
    //        //      cout << "crystal nr " << i << endl ;
    //        //      cout << "Just to proove energy "
    //        //       << ((* event_data_ptr)[i][0])
    //        //       << ", time "
    //        //       << ((* event_data_ptr)[i][1])
    //        //       << endl ;
    //        crys[i]->set_event_ptr_for_crystal_data( (*event_data_ptr)[i] ) ;
    //    }
}
//**************************************************************************
void Tcluster::calculate_addback()
{
    addback4MeV_cal = 0;
    addback20MeV_cal = 0;
    addback4MeV_doppler = 0;
    addback20MeV_doppler = 0;
    addback_time_cal = 0;
    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################
    time_cal_of_the_first_crystal_minus_addback_time_cal = 0;
    LR_plus_addbacked_dt_time_cal = 0;
#endif

    flag_addback_incrementer_valid = 0;

    bool  flag_addback_performed = 0;
    int adjacent_situation = 0;
    int noadjacent_situation = 0;
    int previous_id = 5;
    int holes = 0;

    double doppler_factor;

#ifdef DOPPLER_CORRECTION_NEEDED
    crys[first_crystal_nb]->calculate_chosen_doppler_algorithm();
#endif

    if(crys[first_crystal_nb]->flag_doppler_successful)
        doppler_factor = crys[first_crystal_nb]->doppler_factor;
    else doppler_factor = 0;

    //cout << "ADDING BACK "<<name_of_this_element<<endl;

    // Now we want that only those crystals participate in the addback, which time difference
    // to the fastest one - is no longer than some threshold
    // So at first wee need to find the fastest
    double fastest_time_cal = 9999999 ;
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        if(crys[i]->flag_good_data)
        {
            // in case of time, we take the highest one
            if(fastest_time_cal < crys[i]->give_time_calibrated())
            {
                fastest_time_cal = crys[i]->give_time_calibrated();
            }
        }
    }
// ------------
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        if(crys[i]->flag_good_data)
        {
            // is this time not longer, than the fastest
            if(addback_option_max_acceptable_time_diff_is_enabled == true
                    &&
                    (fastest_time_cal  - crys[i]->give_time_calibrated() > addback_option_max_acceptable_time_diff_to_the_fastest))
            {
                continue; // we do not want it in the addback "too late" participants
            }


            if(!crys[previous_id]->flag_good_data) holes ++;

            addback4MeV_cal +=  crys[i]->give_energy4_calibrated() ;
            addback20MeV_cal +=  crys[i]->give_energy20_calibrated() ;

            addback4MeV_doppler +=  crys[i]->give_energy4_calibrated() * doppler_factor;
            addback20MeV_doppler +=  crys[i]->give_energy20_calibrated() * doppler_factor;

            // in case of time, we take the highest one
            if(addback_time_cal < crys[i]->give_time_calibrated())
            {
                addback_time_cal = crys[i]->give_time_calibrated();

                //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
                //####################################################################
                // here remember the offset for LILI
                offset_of_the_local_fastest = crys[i]->give_calibrating_offset();
#endif

            }
            flag_addback_performed = true;

            // for addback selfgate we want to check the times of the crystals
            // participating in this addback.
            addback_participants.push_back(i);


            //         cout << name_of_this_element <<  ", for cry " << i
            //        << "SUMMING  energ4 = " << crys[i]->give_energy4_calibrated()
            //         << ", energ4 doppler  = " << crys[i]->give_energy4_calibrated()
            ////          << " addback4MeV= " << addback4MeV
            ////          << " addback4MeV_doppler= " << addback4MeV_doppler
            //          << endl ;

            /*cout <<  i << " "

            <<" "<< crys[i]->give_energy20_calibrated()
            <<" "<< first_crystal_nb
            <<" "<< addback4MeV << endl; */
        }  // endif good data
        previous_id = i;
    } // end for crystals



    if(flag_addback_performed)
    {
        if(holes < 2 || crys[6]->flag_good_data)
        {
            adjacent_situation = 1;
        }
        else
        {
            noadjacent_situation = 1;
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

        spec_energy4_addback_cal->manually_increment(addback4MeV_cal) ;
        spec_energy20_addback_cal->manually_increment(addback20MeV_cal) ;
        spec_addback_time_cal->manually_increment(addback_time_cal);

        if(RisingAnalysis_ptr->is_verbose_on())
        {
            cout << " -- " << name_of_this_element
                 << ", addback4MeV_cal= " << addback4MeV_cal
                 << ", addback_time_cal= " << addback_time_cal
                 << endl ;
        }


#ifdef VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT

        // loading to the n-tuple
        Trising::tmp_event_calibrated->en4_vxi_cal_addbacked[id_cluster] = addback4MeV_cal;
        Trising::tmp_event_calibrated->en20_vxi_cal_addbacked[id_cluster] = addback20MeV_cal;

        // sum energy
        rising_pointer->increm_spec_total_energy4_calibrated_addbacked(addback4MeV_cal) ;
        rising_pointer->increm_spec_total_energy20_calibrated_addbacked(addback20MeV_cal) ;

#ifdef DOPPLER_CORRECTION_NEEDED
        // incrementing ADDBACKED of the spectrum
        if(doppler_factor != 0)
        {
            spec_energy4_addback_doppler->manually_increment(addback4MeV_doppler) ;
            spec_energy20_addback_doppler->manually_increment(addback20MeV_doppler) ;

            rising_pointer->increm_spec_total_energy4_doppler_addbacked(addback4MeV_doppler);
            rising_pointer->increm_spec_total_energy20_doppler_addbacked(addback20MeV_doppler);
            if(RisingAnalysis_ptr->is_verbose_on())
            {
                cout << ", addback4MeV_doppler= " << addback4MeV_doppler << endl ;
            }
        }
#endif  // DOPPLER_CORRECTION_NEEDED

#endif  // VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT

    }

    //cout << Tcluster::adjacent_mode << " "<< Tcluster::max_fold<<endl;

}
//*******************************************************************************************
/** No descriptions */
void Tcluster:: reset_variables()
{

    addback4MeV_cal = 0;
    addback20MeV_cal = 0;
    addback4MeV_doppler = 0;
    addback20MeV_doppler = 0;
    addback_time_cal = 0; // here we will keep not the time of the primary, but the fastest
    // (of those participating in addback) (max value of time cal)

    secondary_crystal_nb = -1;   // this was not secondary - not calculated

    //####################################################################
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
    //####################################################################

    time_cal_of_the_first_crystal_minus_addback_time_cal = 0;
    LR_plus_addbacked_dt_time_cal = 0;
    offset_of_the_local_fastest = 0;
    time_LR_plus_addbacked_SR_plus_offsets_difference = 0;
    addback_time_cal_minus_GeOR_cal = 0;
#endif

    flag_addback_incrementer_valid = 0;
    addback_participants.clear();

}
//**********************************************************************************************
/** Read from the event the BGO energy */
#ifdef BGO_PRESENT
void Tcluster::analyse_BGO()
{
    BGO_energy = Trising::tmp_event_unpacked->Cluster_BGO_energy[id_cluster] ;
    spec_BGO_energy_raw->manually_increment(BGO_energy);
}
#endif
/******************************************************************************/
bool Tcluster::check_selfgate(Tself_gate_abstract_descr *desc)
{
    // Honestly it shoul not be posible to use this selfgate. Ther should be a special
    // self gate for cluster, while we are using the crystal selfgate. Perhaps I should block this
    // just by changing the name of the incremntor (not cluster_crys_M... but cluster_M)

    // here we assume that this is really germ cluster for addback
    Tself_gate_ger_addback_descr *d = (Tself_gate_ger_addback_descr*)desc ;


    if(d->enable_en4gate)
    {
        if(addback4MeV_cal < d->en4_gate[0]
                ||
                addback4MeV_cal > d->en4_gate[1])
            return false ;
    }

    if(d->enable_time_gate)
    {

#define FASTEST_CRYSTAL

#ifdef FASTEST_CRYSTAL
        // recent idea of Piotrek
        // NOTE addback_time_cal; // here we will keep not the time of the primary, but the fastest
        // (of those participating in addback) (max value of time cal)
        if(addback_time_cal < d->time_gate[0] || addback_time_cal > d->time_gate[1])
            return false ;

#else
        // another, old way
        // NOTE: here we should test the times of all crystals which participate
        // in the current addback

        for(unsigned int i = 0 ; i < addback_participants.size() ; i++)
        {
            double time_cal = crys[addback_participants[i]]->give_time_calibrated();

            if(time_cal < d->time_gate[0] || time_cal > d->time_gate[1])
                return false ;
        }
#endif

    }


    //=========================================
    // geometry of the current primary segment
    if(d->enable_geom_theta_gate)
    {
        double theta_geom = crys[first_crystal_nb]->give_theta_geom();
        if(theta_geom < d->geom_theta_gate[0] || theta_geom > d->geom_theta_gate[1])
            return false;
    }
    if(d->enable_geom_phi_gate)
    {
        double phi_geom = crys[first_crystal_nb]->give_phi_geom();
        if(phi_geom < d->geom_phi_gate[0] || phi_geom > d->geom_phi_gate[1])
            return false;
    }


    //=========================================
    // geometry of the current SECONDARY segment
    if(d->enable_geom_theta_gate_secondary)
    {
        double theta_geom = crys[secondary_crystal_nb]->give_theta_geom();
        if(theta_geom < d->geom_theta_gate_secondary[0]
                ||
                theta_geom > d->geom_theta_gate_secondary[1]
                ||
                secondary_crystal_nb == -1   // this was not secondary
          )
            return false;
    }
    if(d->enable_geom_phi_gate_secondary)
    {
        double phi_geom = crys[secondary_crystal_nb]->give_phi_geom();
        if(phi_geom < d->geom_phi_gate_secondary[0] || phi_geom > d->geom_phi_gate_secondary[1]
                ||
                secondary_crystal_nb == -1   // this was not secondary
          )
            return false;
    }


    //  it is possible that somebody sets the selfgate on the BGO signal
    //  which are the feature of the cluster (not  crystal). In such a case we
    //  test only BGO and multipilicyty - because, here in cluster only
    //  this is available


    if(d->enable_mult_in_cluster_gate)
    {
        int mult =  get_this_cluster_multiplicity();
        if(mult < d->mult_in_cluster_gate[0]
                ||
                mult > d->mult_in_cluster_gate[1]
          )
            return false ;
    }
#ifdef BGO_PRESENT

    if(d->enable_BGO_in_cluster_gate)
    {
        int BGO_energy =  get_this_cluster_BGO_energy();
        if(BGO_energy < d->BGO_in_cluster_gate[0]
                ||
                BGO_energy > d->BGO_in_cluster_gate[1]
          )
            return false ;
    }
#endif // def BGO_PRESENT

    return true ; // when EVRYTHING was successful
}
//******************************************************************************
#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
void Tcluster::analyse_LR_common_time_in_cluster_for_addback_purposes()
{
    if(!flag_addback_incrementer_valid)
        return;

    time_cal_of_the_first_crystal_minus_addback_time_cal =
        Tcrystal::time_of_the_crystal_which_first_fired - addback_time_cal;

    spec_time_cal_of_the_first_crystal_minus_addback_time_cal->
    manually_increment(time_cal_of_the_first_crystal_minus_addback_time_cal);

    LR_plus_addbacked_dt_time_cal =
        Trising::vxi_LR_time_calibrated_by_the_first_who_fired +
        time_cal_of_the_first_crystal_minus_addback_time_cal;

    spec_LR_plus_addbacked_dt_time_cal ->
    manually_increment(LR_plus_addbacked_dt_time_cal);

    //-------------
    double offset_of_first = Tcrystal::pointer_to_the_crystal_which_first_fired->give_calibrating_offset();

    time_LR_plus_addbacked_SR_plus_offsets_difference =
        Trising::vxi_LR_time_calibrated_by_the_first_who_fired    // LR
        +  time_cal_of_the_first_crystal_minus_addback_time_cal    // SR
        + (offset_of_the_local_fastest - offset_of_first);


    addback_time_cal_minus_GeOR_cal =  Trising::vxi_P_time_minus_Ge_time_OR_cal -  addback_time_cal;

    /*  LR_plus_addbacked_dt_time_olus_offset_difference  =
        Trising::vxi_LR_time_calibrated_by_the_first_who_fired +
        time_cal_of_the_first_crystal_minus_addback_time_cal +

        ;
    */

    if(RisingAnalysis_ptr->is_verbose_on())
    {
        cout << name_of_this_element
             << "-  Tcluster::calc addback. \n"
             " time_of_the_crystal_which_first_fired -  addback_time_cal ("
             << Tcrystal::time_of_the_crystal_which_first_fired
             << " - " <<  addback_time_cal
             << ") so time_cal_of_the_first_crystal_minus_addback_time_cal = "
             << time_cal_of_the_first_crystal_minus_addback_time_cal
             << endl;

        cout
        /*   << " vxi_LR_time_calibrated_by_the_first_who_fired = "
           << Trising::vxi_LR_time_calibrated_by_the_first_who_fired
           << ", time_cal_of_the_first_crystal_minus_addback_time_cal = "
           <<   time_cal_of_the_first_crystal_minus_addback_time_cal*/
                << "   the LR_plus_addbacked_dt_time_cal = "
                << LR_plus_addbacked_dt_time_cal
                << "\n time_LR_plus_addbacked_SR_plus_offsets_difference= "
                << time_LR_plus_addbacked_SR_plus_offsets_difference
                << endl;

        cout << endl;
    }
}
#endif

