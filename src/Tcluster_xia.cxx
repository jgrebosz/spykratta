#include "Tcluster_xia.h"
#include "Trising.h"
#include <algorithm>
#include <vector>
#include "Tnamed_pointer.h"
//#include "Tself_gate_ger_descr.h"
#include "Tself_gate_ger_addback_descr.h"

#include "TIFJAnalysis.h" // for verbose

Trising *Tcluster_xia::rising_pointer ; // static pointer
int Tcluster_xia::all_clusters_multiplicity ;
int Tcluster_xia::max_fold;

bool Tcluster_xia::adjacent_mode;
bool Tcluster_xia::nonadjacent_mode;
bool Tcluster_xia::sum_all_mode;
double Tcluster_xia::addback_option_max_acceptable_time_diff_to_the_fastest;
bool Tcluster_xia::addback_option_max_acceptable_time_diff_is_enabled;

int Tcluster_xia::primary_seg_algorithm ;
int Tcluster_xia::Eprimary_threshold;

#ifdef DOPPLER_CORRECTION_NEEDED
spectrum_1D * Tcluster_xia::spec_total_xia_energy4_doppler_addbacked;
//spectrum_1D * Tcluster_xia::spec_total_xia_energy20_doppler_addbacked;
#endif // def DOPPLER_CORRECTION_NEEDED
//*************************************************************************************************
bool operator<(const fired_good_crystal & one, const fired_good_crystal & other)
{
    return one.time_calibrated < other.time_calibrated;
}


//int local_mult_tmp ;
//***********************************************************************
Tcluster_xia::Tcluster_xia(string name, int nr) : name_of_this_element(name), id_cluster(nr)
{
    //cout<< "TCLUSTER"<<endl;
    //  cout << "\n\nConstructor of " << _name
    //        << ", my addres is " << this << endl ;

    const int HOW_MANY_CRISTALS = 7 ;
    for(int i = 0 ; i < HOW_MANY_CRISTALS ; i++)
    {
        //cout << "before creating and pushing crystal " << i << endl ;
        crys.push_back(new Tcrystal_xia(this, i + 1));
        //cout << "Now " << cris.size()
        //     << " det are on the crystal list " << endl;
    }
    event_data_ptr = 0 ;  // to be set during pre_loop

    for(int i = 0 ; i < NR_MULTI_ADDBACKS ; i++)
    {
        selfgate_object[i].set_values(i, this);
    }


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

    string entry_name;
    Tmap_of_named_pointers::iterator   pos;
    // addbacks ------------
#ifdef OLD_ADDBACK
    named_pointer[name_without_crys + "_addback_4MeV_cal"]
    = Tnamed_pointer(&addback4MeV_cal, &flag_addback_incrementer_valid, this) ;

    //  named_pointer[name_without_crys+"_addback_20MeV_cal"]
    //    = Tnamed_pointer(&addback20MeV_cal, &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_addback_time_cal"]
    = Tnamed_pointer(&addback_time_cal, &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_addback_LR_time_cal"]
    = Tnamed_pointer(&addback_LR_time_cal, &flag_addback_incrementer_valid, this) ;

    named_pointer[name_without_crys + "_addback_SR_time_cal"]
    = Tnamed_pointer(&addback_SR_time_cal, &flag_addback_incrementer_valid, this) ;

#ifdef DOPPLER_CORRECTION_NEEDED

    named_pointer[name_without_crys + "_addback_4MeV_doppler"]
    = Tnamed_pointer(&addback4MeV_doppler, &flag_addback_incrementer_valid, this) ;

    //  named_pointer[name_without_crys+"_addback_20MeV_doppler"]
    //    = Tnamed_pointer(&addback20MeV_doppler, &flag_addback_incrementer_valid, this) ;
#endif // def DOPPLER_CORRECTION_NEEDED

    // ALLs
    //------------------------------
    entry_name =  "ALL_cluster_xia_addback_4MeV_cal_when_successful" ;
    //Tmap_of_named_pointers::iterator
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback4MeV_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback4MeV_cal, &flag_addback_incrementer_valid, this) ;



    entry_name =  "ALL_cluster_xia_addback_time_cal_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback_time_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback_time_cal, &flag_addback_incrementer_valid, this) ;

    entry_name =  "ALL_cluster_xia_addback_LR_time_cal_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback_LR_time_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback_LR_time_cal, &flag_addback_incrementer_valid, this) ;

    entry_name =  "ALL_cluster_xia_addback_SR_time_cal_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback_SR_time_cal, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback_SR_time_cal, &flag_addback_incrementer_valid, this) ;

#ifdef DOPPLER_CORRECTION_NEEDED

    //-------------
    entry_name =  "ALL_cluster_xia_addback_4MeV_doppler_when_successful" ;
    pos = named_pointer.find(entry_name);
    if(pos != named_pointer.end())
        pos->second.add_item(&addback4MeV_doppler, &flag_addback_incrementer_valid, this) ;
    else
        named_pointer[entry_name] =
            Tnamed_pointer(&addback4MeV_doppler, &flag_addback_incrementer_valid, this) ;


#endif // def DOPPLER_CORRECTION_NEEDED

#endif  // OLD_ADDBACK


#ifdef NIGDY
    // incrementers for multi_addback
    named_pointer[name_without_crys + "_multi_addback_nr1_4MeV_cal"]
    = Tnamed_pointer(&multi_addback_4MeV_cal[0], &flag_multi_addback_incrementer_valid[0], this) ;

    named_pointer[name_without_crys + "_multi_addback_nr2_4MeV_cal"]
    = Tnamed_pointer(&multi_addback_4MeV_cal[1], &flag_multi_addback_incrementer_valid[1], this) ;

    named_pointer[name_without_crys + "_multi_addback_nr3_4MeV_cal"]
    = Tnamed_pointer(&multi_addback_4MeV_cal[2], &flag_multi_addback_incrementer_valid[2], this) ;
#endif


    // NOTE: that below the incrementers do not have the pointer to 'this' but to the
    // special selfgate object. By this the selfgate will know which of the 3 possible multi_addback has
    // to be tested

    entry_name =  name_without_crys + "_multi_addback_4MeV_cal" ;
    pos = named_pointer.find(entry_name);

    for(int i = 0 ; i < NR_MULTI_ADDBACKS ; i++)
    {
        if(pos != named_pointer.end())
            pos->second.add_item(&multi_addback_4MeV_cal[i],
                                 &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&multi_addback_4MeV_cal[i],
                               &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
    }


    //-------------
    entry_name =  "ALL_cluster_xia_multi_addback_4MeV_cal_when_good" ;
    pos = named_pointer.find(entry_name);
    for(int i = 0 ; i < NR_MULTI_ADDBACKS ; i++)
    {
        if(pos != named_pointer.end())
            pos->second.add_item(&multi_addback_4MeV_cal[i],
                                 &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&multi_addback_4MeV_cal[i],
                               &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
    }




    entry_name =  name_without_crys + "_multi_addback_time_cal" ;
    for(int i = 0 ; i < NR_MULTI_ADDBACKS ; i++)
    {
        pos = named_pointer.find(entry_name);
        if(pos != named_pointer.end())
            pos->second.add_item(&multi_addback_time_cal[i],
                                 &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&multi_addback_time_cal[i],
                               &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
    }


    //-------------
    entry_name =  "ALL_cluster_xia_multi_addback_time_call_when_good" ;
    pos = named_pointer.find(entry_name);
    for(int i = 0 ; i < NR_MULTI_ADDBACKS ; i++)
    {
        if(pos != named_pointer.end())
            pos->second.add_item(&multi_addback_time_cal[i],
                                 &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&multi_addback_time_cal[i],
                               &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
    }





    entry_name =  name_without_crys + "_multi_addback_LR_time_cal" ;
    pos = named_pointer.find(entry_name);
    for(int i = 0 ; i < NR_MULTI_ADDBACKS ; i++)
    {

        if(pos != named_pointer.end())
            pos->second.add_item(&multi_addback_LR_time_cal[i],
                                 &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&multi_addback_LR_time_cal[i],
                               &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
    }


    entry_name =  name_without_crys + "_multi_addback_SR_time_cal" ;
    pos = named_pointer.find(entry_name);
    for(int i = 0 ; i < NR_MULTI_ADDBACKS ; i++)
    {

        if(pos != named_pointer.end())
            pos->second.add_item(&multi_addback_SR_time_cal[i],
                                 &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&multi_addback_SR_time_cal[i],
                               &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
    }


#ifdef DOPPLER_CORRECTION_NEEDED
    entry_name =  name_without_crys + "_multi_addback_4MeV_doppler" ;
    pos = named_pointer.find(entry_name);
    for(int i = 0 ; i < NR_MULTI_ADDBACKS ; i++)
    {
        if(pos != named_pointer.end())
            pos->second.add_item(&multi_addback4MeV_doppler[i],
                                 &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
        else
            named_pointer[entry_name] =
                Tnamed_pointer(&multi_addback4MeV_doppler[i],
                               &flag_multi_addback_incrementer_valid[i], &selfgate_object[i]) ;
    }
#endif // def DOPPLER_CORRECTION_NEEDED

}
//************************************************************************
void Tcluster_xia::create_my_spectra()
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
    rising_spectra_ptr->push_back(spec_energy4_addback_cal) ;



    name = name_without_crys ;
    name += "_addback_time_cal"  ;
    spec_addback_time_cal = new spectrum_1D(
        name,
        name,
        8191, 1, 8192,
        folder);

    rising_spectra_ptr->push_back(spec_addback_time_cal);

    name = name_without_crys ;
    name += "_addback_LR_time_cal"  ;
    spec_addback_LR_time_cal = new spectrum_1D(
        name,
        name,
        8191, 1, 8192,
        folder);

    rising_spectra_ptr->push_back(spec_addback_LR_time_cal);
    name = name_without_crys ;
    name += "_addback_SR_time_cal"  ;
    spec_addback_SR_time_cal = new spectrum_1D(
        name,
        name,
        8191, 1, 8192,
        folder);

    rising_spectra_ptr->push_back(spec_addback_LR_time_cal);

#ifdef DOPPLER_CORRECTION_NEEDED

    name = name_without_crys ;
    name += "_energy4MeV_doppler_corrected_addback"  ;
    spec_energy4_addback_doppler = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);
    rising_spectra_ptr->push_back(spec_energy4_addback_doppler) ;

    //  name = name_without_crys ;
    //  name += "_energy20MeV_doppler_corrected_addback"  ;
    //  spec_energy20_addback_doppler = new spectrum_1D(
    //    name,
    //    name,
    //    8192, 0, 8192,
    //    folder );
    //  rising_spectra_ptr->push_back(spec_energy20_addback_doppler) ;

    // TOTAL

    if(!spec_total_xia_energy4_doppler_addbacked)
    {

        name = "cluster_xia_total_energy4MeV_doppler_corrected_addback"  ;
        spec_total_xia_energy4_doppler_addbacked = new spectrum_1D(
            name,
            name,
            8192, 0, 8192,
            folder);
        rising_spectra_ptr->push_back(spec_total_xia_energy4_doppler_addbacked) ;
    }

    // if(!spec_total_xia_energy20_doppler_addbacked)
    // {
    //
    //  name = "cluster_xia_total_energy20MeV_doppler_corrected_addback"  ;
    //  spec_total_xia_energy20_doppler_addbacked = new spectrum_1D(
    //    name,
    //    name,
    //    8192, 0, 8192,
    //    folder );
    //  rising_spectra_ptr->push_back(spec_total_xia_energy20_doppler_addbacked) ;
    // }
#endif // def DOPPLER_CORRECTION_NEEDED


    // temporary for adjacent/nonadjacent situation

#ifdef BGO_PRESENT

    name = name_of_this_element ;
    name += "_BGO_energy_raw"  ;
    spec_BGO_energy_raw = new spectrum_1D(
        name,
        name,
        8192, 0, 8192,
        folder);
    rising_spectra_ptr->push_back(spec_BGO_energy_raw) ;
#endif
    //

}
//**********************************************************************
void Tcluster_xia::analyse_current_event()
{

    // WARNING function  below is never called,
    // instead we jump directly from Trising into Tcrystal_xia
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
void  Tcluster_xia::make_preloop_action(istream & s)
{
    //  check_legality_of_data_pointers();
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
//**************************************************************************
#define dcout if(RisingAnalysis_ptr->is_verbose_on() )cout
void Tcluster_xia::calculate_addback()
{
    addback4MeV_cal = 0;
    addback_time_cal = 0;
    addback_LR_time_cal = 0;
    addback_SR_time_cal = 0;

#ifdef DOPPLER_CORRECTION_NEEDED
    addback4MeV_doppler = 0;
#endif // def DOPPLER_CORRECTION_NEEDED

    flag_addback_incrementer_valid = 0;

    bool  flag_addback_performed = 0;
    int adjacent_situation = 0;
    int noadjacent_situation = 0;
    int previous_id = 5;
    int holes = 0;

    double doppler_factor;

#ifdef DOPPLER_CORRECTION_NEEDED
    crys[first_crystal_nb]->calculate_chosen_doppler_algorithm();
    if(crys[first_crystal_nb]->flag_doppler_successful)
        doppler_factor = crys[first_crystal_nb]->doppler_factor;
    else
        doppler_factor = 0;
#else
    doppler_factor = 0;
#endif

    dcout << "...........  classical ADDING BACK " << name_of_this_element << endl;

    // Now we want that only those crystals participate in the addback, which time difference
    // to the fastest one - is no longer than some threshold
    // So at first wee need to find the fastest
    double fastest_dgf_time = 9999999 ;
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        if(crys[i]->flag_good_data)
        {
            //cout << "[" << i << "] has good data and time cal = " << crys[i]->give_time_calibrated() << endl;
            // in case of time, we take the highest one
            if(fastest_dgf_time > crys[i]->give_time_calibrated())   // was < (time was going reversed ?)
            {
                fastest_dgf_time = crys[i]->give_time_calibrated();
            }
        }
    }

    dcout << "Fastest time was " << fastest_dgf_time << endl;

    // =================
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        if(crys[i]->flag_good_data)
        {
            // is this time not longer, than the fastest
            if(addback_option_max_acceptable_time_diff_is_enabled == true
                    &&
                    (fastest_dgf_time  - crys[i]->give_time_calibrated() > addback_option_max_acceptable_time_diff_to_the_fastest))
            {
                continue; // we do not want it in the addback "too late" participants
            }

            if(!crys[previous_id]->flag_good_data)
                holes ++;

            dcout << " Adding  [" << i + 1 << "]" << " E= " << crys[i]->give_energy4_calibrated()
                  << ", " ;
            addback4MeV_cal +=  crys[i]->give_energy4_calibrated() ;

#ifdef DOPPLER_CORRECTION_NEEDED

            addback4MeV_doppler +=  crys[i]->give_energy4_calibrated() * doppler_factor;
#endif // def DOPPLER_CORRECTION_NEEDED

            // in case of time, we take the highest one
            if(addback_time_cal < crys[i]->give_time_calibrated())
            {
                addback_time_cal = crys[i]->give_time_calibrated();
            }
#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
            // in case of time, we take the highest one
            if(addback_LR_time_cal < crys[i]->give_LR_time_calibrated())
            {
                addback_LR_time_cal = crys[i]->give_LR_time_calibrated();
            }
            // in case of time, we take the highest one
            if(addback_SR_time_cal < crys[i]->give_SR_time_calibrated())
            {
                addback_SR_time_cal = crys[i]->give_SR_time_calibrated();
            }
#endif

            flag_addback_performed = true;
            // for addback selfgate we want to check the times of the crystals
            // participating in this addback.
            //      addback_participants.push_back(i);


            // cout << name_of_this_element <<  ", for cry " << i
            // << "SUMMING  energ4 = " << crys[i]->give_energy4_calibrated()
            //         << ", energ4 doppler  = " << crys[i]->give_energy4_calibrated()
            // << " addback4MeV_cal= " << addback4MeV_cal
            ////          << " addback4MeV_doppler= " << addback4MeV_doppler
            // << endl ;

            /*cout <<  i << " "

            <<" "<< crys[i]->give_energy20_calibrated()
            <<" "<< first_crystal_nb
            <<" "<< addback4MeV << endl; */

        }  // endif good data -------------
        previous_id = i;
    } // end for crystals ---------------------------------------------------------------------------

    dcout << "\nFINALLY, " << name_of_this_element
          << ", addback4MeV_cal= " << addback4MeV_cal
#ifdef DOPPLER_CORRECTION_NEEDED
          << ", addback4MeV_doppler= " << addback4MeV_doppler
#endif // def DOPPLER_CORRECTION_NEEDED
          << endl ;

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
        dcout << "This is an accepted addback, adjacent =  "
              << adjacent_situation << endl;

        spec_energy4_addback_cal->manually_increment(addback4MeV_cal) ;
        spec_addback_time_cal->manually_increment(addback_time_cal);
        spec_addback_LR_time_cal->manually_increment(addback_SR_time_cal);
        spec_addback_SR_time_cal->manually_increment(addback_SR_time_cal);

#ifdef XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
        // loading to the n-tuple
        Trising::tmp_event_calibrated->en4_xia_cal_addbacked[id_cluster] = addback4MeV_cal;
#endif   // def XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT

        // sum energy
        rising_pointer->increm_spec_total_energy4_calibrated_addbacked(addback4MeV_cal) ;

#ifdef DOPPLER_CORRECTION_NEEDED
        // incrementing ADDBACKED of the spectrum
        if(doppler_factor != 0)
        {
            spec_energy4_addback_doppler->manually_increment(addback4MeV_doppler) ;
            spec_total_xia_energy4_doppler_addbacked->manually_increment(addback4MeV_doppler);
        }
#endif // def DOPPLER_CORRECTION_NEEDED

    }
    else
    {
        dcout << "This is an rejected addback"  << endl;
    }
    //cout << Tcluster_xia::adjacent_mode << " "<< Tcluster_xia::max_fold<<endl;
}
#undef dcout
//**********************************************************************
//*********************************************************************
//************************************************************************
/** No descriptions */
void Tcluster_xia:: reset_variables()
{
    addback4MeV_cal = 0;
    addback_time_cal = 0; // here we will keep not the time of the primary, but
    // the fastest (of those participating in addback) (max value of time cal)
    addback_LR_time_cal = 0; // here we will keep not the time of the primary, but
    // the fastest (of those participating in addback) (max value of time cal)
    addback_SR_time_cal = 0; // here we will keep not the time of the primary, but
    // the fastest (of those participating in addback) (max value of time cal)


#ifdef DOPPLER_CORRECTION_NEEDED
    addback4MeV_doppler = 0;
#endif // def DOPPLER_CORRECTION_NEEDED

    flag_addback_incrementer_valid = 0;
    //  addback_participants.clear();

    //=============
    for(int i = 0 ; i <  NR_MULTI_ADDBACKS; i++)
    {
        multi_addback_4MeV_cal[i] = 0;
        multi_addback_time_cal[i] = 0;
        multi_addback_LR_time_cal[i] = 0;
        multi_addback_SR_time_cal[i] = 0; // here we will keep not the time of the primary,
#ifdef DOPPLER_CORRECTION_NEEDED
        multi_addback4MeV_doppler[i] = 0;
#endif // def DOPPLER_CORRECTION_NEEDED
        multi_addback_primary_crystal_nb[i] = 0;
        flag_multi_addback_incrementer_valid[i] = false;  // validator
    }
    //============================== end of zeroing

}

#ifdef BGO_PRESENT

//**********************************************************************************
/** Read from the event the BGO energy */
void Tcluster_xia::analyse_BGO()
{
    // WARNING!!!!!  - THIS IS TAKEN FROM VXI, SO CHEATING

    BGO_energy = Trising::tmp_event_unpacked->Cluster_BGO_energy[id_cluster] ;
    spec_BGO_energy_raw->manually_increment(BGO_energy);
}

#endif // def BGO_PRESENT

/******************************************************************************/
bool Tcluster_xia::check_selfgate(Tself_gate_abstract_descr *desc)
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
        // NOTE addback_time_cal; // here we will keep not the time of the primary, but
        // the fastest (of those participating in addback) (max value of time cal)
        if(addback_time_cal < d->time_gate[0] || addback_time_cal > d->time_gate[1])
            return false ;

#else

        // NOTE: here we should test the times of all crystals which participate
        // in the current addback

        for(unsigned int i = 0 ; i < addback_participants.size() ; i++)
        {
            double time_cal = crys[addback_participants[i]]->give_time_calibrated();

            if(time_cal < d->time_gate[0] || time_cal > d->time_gate[1])
                return false ;
        }
#endif // FASTEST_CRYSTAL

    }

#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
    if(d->enable_LR_time_gate)
    {
#ifdef FASTEST_CRYSTAL
        // recent idea of Piotrek
        // NOTE addback_time_cal; // here we will keep not the time of the primary, but
        // the fastest (of those participating in addback) (max value of LR_time cal)
        if(addback_LR_time_cal < d->LR_time_gate[0]
                ||
                addback_LR_time_cal > d->LR_time_gate[1])
            return false ;

#else
        // NOTE: here we should test the times of all crystals which participate
        // in the current addback

        for(unsigned int i = 0 ; i < addback_participants.size() ; i++)
        {
            double LR_time_cal = crys[addback_participants[i]]->give_LR_time_calibrated();
            if(LR_time_cal < d->LR_time_gate[0]
                    ||
                    LR_time_cal > d->LR_time_gate[1])
                return false ;
        }
#endif

    }


    if(d->enable_SR_time_gate)
    {
#ifdef FASTEST_CRYSTAL
        // recent idea of Piotrek
        // NOTE addback_time_cal; // here we will keep not the time of the primary, but
        // the fastest (of those participating in addback) (max value of LR_time cal)
        if(addback_SR_time_cal < d->SR_time_gate[0]    ||
                addback_SR_time_cal > d->SR_time_gate[1])
            return false ;

#else
        // NOTE: here we should test the times of all crystals which participate
        // in the current addback

        for(unsigned int i = 0 ; i < addback_participants.size() ; i++)
        {
            double SR_time_cal = crys[addback_participants[i]]->give_SR_time_calibrated();

            if(SR_time_cal < d->SR_time_gate[0] || SR_time_cal > d->SR_time_gate[1])
                return false ;
        }
#endif // else  FASTEST_CRYSTAL

    }
#endif

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

    return true ; // when EVERYTHING was successful
}
//*******************************************************************************************************
void Tcluster_xia::calculate_multi_addback()
{

#define dcout if(RisingAnalysis_ptr->is_verbose_on() )cout

    static int display_counter ;
    dcout << "\n========== multi ADDING BACK, for " << name_of_this_element
          << "========= " << (display_counter++)
          << endl;

    vector<fired_good_crystal> active_crystal;
    active_crystal.clear();

    // 1. Loop over all the crystals to make the vector of all firing crystals
    for(unsigned int i = 0 ; i < crys.size() ; i++)
    {
        if(crys[i]->flag_good_data)   // this means fired and above some threshold
        {
            // is this time not longer, than the fastest
            fired_good_crystal f ;
            f.nr = i ;
            f.time_calibrated = crys[i]->give_time_calibrated();
            f.energy = crys[i]->give_time_calibrated();

            active_crystal.push_back(f);
        }
    }

    // 2. Sorting this vector according to the time

    //   cout << "Before sorting ------ " ;
    //   for(unsigned int i = 0 ; i < active_crystal.size() ; i++)
    //   {
    //     cout << "   " << i << ") crystal nr "
    //     <<  active_crystal[i].nr << " time= "
    //     << active_crystal[i].time_calibrated ;
    //   }
    //   cout << endl;
    sort(active_crystal.begin(), active_crystal.end());
    dcout << "Time sorted 'good' crystals ------> " ;

    if(RisingAnalysis_ptr->is_verbose_on())
    {
        for(unsigned int i = 0 ; i < active_crystal.size() ; i++)
        {
            cout
            //<< "   " << i << ") crystal nr "
                    << "   ["
                    <<  active_crystal[i].nr + 1 << "]  T= "
                    << active_crystal[i].time_calibrated
                    << " (E = "
                    << crys[active_crystal[i].nr]->give_energy4_calibrated() << ") ";
        }
        cout << endl;
    }


    //   if(active_crystal.size() > 3)
    //   {
    //    cout << "Multiplicity > 3 !!!!!" << endl;
    //   }
    // 3. Loop over this new vector to find the candidats for the addback.

    int how_many_elementary_addbacks = 0;
    vector<bool> crystal_fired;
    crystal_fired.resize(7);


    // loop over all active good %%%%%%%%%%%%%%%%%%%%%%%%%%
    for(unsigned int fastest_nr = 0 ; fastest_nr < active_crystal.size() ; fastest_nr++)
    {
        dcout << "Time interval: " ;
        // " nr in vector  = " << fastest_nr  << endl;

        int fastest_id = active_crystal[fastest_nr].nr; // used in LR, SR later
        double fastest_time = active_crystal[fastest_nr].time_calibrated ;
        int addb_multiplicity = 0;
        double primary_energy = 0 ;   // highest energy in the addback
        int primary_id = -1;  // ?

        bool adjacent_situation = false;
        bool noadjacent_situation = false;
        int previous_id = 5;
        int holes = 0;

        vector<int> addback_contributors_id;

        // loop over all following in the time interval ++++++++++++++++++++++++++++++
        unsigned int  current_nr = 0;
        for(current_nr = fastest_nr  ; current_nr < active_crystal.size() ; current_nr++)
        {
            //cout  << " Scanning interval, checkingi current_nr =   " << current_nr  << endl;
            int crystal_id = active_crystal[current_nr].nr;
            // 3.a. at first we look if the next crystal is in the time interval.
            //      if not, skip it, and try to take the next crystal

            if(addback_option_max_acceptable_time_diff_is_enabled == true
                    &&
                    ((active_crystal[current_nr].time_calibrated - fastest_time) >
                     addback_option_max_acceptable_time_diff_to_the_fastest))
            {
                //cout << "Next crystal is already outside the interval, lets check the just build addback..."
                // << endl;
                break ;  // we do not want it in the addback "too late" participants
            }
            // if yes - remember this and check if it is adjacent/nonadjecent
            // Loop over all in this interval
            addback_contributors_id.push_back(crystal_id);
            // there is a constrain about the fold - so we have to check it.
            addb_multiplicity++;

            // there is a constraind about the adjacent, nonadjacent, so we count holes

            crystal_fired[crystal_id] = true;

            // Summing the energy
            multi_addback_4MeV_cal[how_many_elementary_addbacks] +=
                crys[crystal_id]->give_energy4_calibrated() ;

            dcout << " Adding energy [" << crystal_id + 1 << "] "
                  << crys[crystal_id]->give_energy4_calibrated() ;

            if(primary_energy < crys[crystal_id]->give_energy4_calibrated())
            {
                primary_energy = crys[active_crystal[current_nr].nr]->give_energy4_calibrated() ;
                primary_id = crystal_id;  // ?
            }
        }     // endi of for over time interval  +++++++++++++++++++++++++++++++++++++
        dcout
        //<< "End of interval "
                << endl;


        if(addb_multiplicity > 1)
        {
            for(unsigned int i = 0 ; i < crystal_fired.size() ; i++)
            {
                if(i == 0) previous_id = 5;
                if(crystal_fired[i]  && !crystal_fired[previous_id])   // if previous not fired
                {
                    holes ++;
                    //cout << "HOLE Nr " << i << " fired, while " << previous_id << " didn't" << endl;
                }
                previous_id = i;
            }
            //cout << "Holes: " << holes << endl;
            if(holes < 2 || crystal_fired[6])
            {
                adjacent_situation = 1;
                dcout << "Adjacent situation " << endl;
            }
            else
            {
                noadjacent_situation = 1;
                dcout << "Non adjacent situation " << endl;
            }
        }



        fastest_nr = current_nr - 1; // because this outer loop will increment it anyway
        // here we know primary_id, and fastest_id
        if((addb_multiplicity <= max_fold && (addb_multiplicity > 1))   // if this was the correct addback !!!
                &&
                (
                    (adjacent_situation && adjacent_mode)
                    || (noadjacent_situation && nonadjacent_mode)
                    ||  sum_all_mode
                )
                &&
                (primary_energy >= Eprimary_threshold)
          )
        {
            // if all conditions are fulfilled for addback >>>>>>>>>>>>>>>>>>>>>>>>

            dcout << "In this interval Addback is succesfull: "
                  << "En4 = " << multi_addback_4MeV_cal[how_many_elementary_addbacks]
                  << ", fastest _time = " << fastest_time << endl;

#ifdef XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
            // loading to the n-tuple
            Trising::tmp_event_calibrated->en4_xia_cal_addbacked[id_cluster] =
                multi_addback_4MeV_cal[0];   // <--- Note, we put only the first (secont and
            // third are very seldom)
#endif   // def XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT

            // finally we should have the information fold, primary crystal (Max E), fastest crystal,
            // the result is copied to the multi_addback arrays

            multi_addback_time_cal[how_many_elementary_addbacks] = fastest_time;
#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
            // here we will keep not the time of the primary, but LR (SR) of fastest dgf crystal
            multi_addback_LR_time_cal[how_many_elementary_addbacks] =
                crys[fastest_id]->give_LR_time_calibrated();
            multi_addback_SR_time_cal[how_many_elementary_addbacks] =
                crys[fastest_id]->give_SR_time_calibrated();

#endif

            multi_addback_primary_crystal_nb[how_many_elementary_addbacks] = primary_id;
            flag_multi_addback_incrementer_valid[how_many_elementary_addbacks] = true;  // validator

            // loop over all participants to set a flag "used for addback"
            dcout << "Crystals flagged as 'participants of addback'  : "
                  << name_of_this_element << "   " ;
            for(unsigned int c = 0  ; c < addback_contributors_id.size() ; c++)
            {
                // Those crystals which were used for addback, have
                // the special flag_addback_participant
                crys[addback_contributors_id[c] ]->set_flag_participant_of_addback();
                dcout << " " << addback_contributors_id[c] + 1 << ", " ;
            }
            dcout << endl;


#ifdef DOPPLER_CORRECTION_NEEDED
            // doppler factor is calculated by the primary.
            double doppler_factor;

            crys[primary_id]->calculate_chosen_doppler_algorithm();
            if(crys[primary_id]->flag_doppler_successful)
                doppler_factor = crys[primary_id]->doppler_factor;
            else
                doppler_factor = 0;

            for(unsigned int c = 0  ; c < addback_contributors_id.size() ; c++)
            {
                multi_addback4MeV_doppler[how_many_elementary_addbacks] +=
                    crys[c]->give_energy4_calibrated() * doppler_factor;
            }
#endif // def DOPPLER_CORRECTION_NEEDED


            // incrementing the spectra ===========================

#ifdef NIGDY
            spec_energy4_multi_addback_cal->manually_increment(
                multi_addback_4MeV_cal[how_many_elementary_addbacks]) ;
            rising_pointer->increm_spec_total_energy4_calibrated_multi_addbacked(
                multi_addback_4MeV_cal[how_many_elementary_addbacks]);
            spec_multi_addback_time_cal->manually_increment(fastest_time);
#endif // NIGDY

#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED

#ifdef NIGDY
            spec_addback_LR_time_cal->manually_increment(
                multi_addback_LR_time_cal[how_many_elementary_addbacks]);
            spec_addback_SR_time_cal->manually_increment(
                multi_addback_SR_time_cal[how_many_elementary_addbacks]);
#endif  // NIGDY

#endif


#ifdef XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
            // loading to the n-tuple
            Trising::tmp_event_calibrated->en4_xia_cal_addbacked[id_cluster] =
                multi_addback_4MeV_cal[how_many_elementary_addbacks] ;
#endif   // def XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT


#ifdef DOPPLER_CORRECTION_NEEDED
            // incrementing ADDBACKED of the spectrum
            if(doppler_factor != 0)
            {
#ifdef NIGDY
                spec_energy4_multi_addback_doppler->manually_increment(
                    multi_addback4MeV_doppler[how_many_elementary_addbacks]) ;
                spec_total_xia_energy4_doppler_multi_addbacked->manually_increment(
                    multi_addback4MeV_doppler[how_many_elementary_addbacks]);
#endif  // NIGDY

            }
#endif // def DOPPLER_CORRECTION_NEEDED
            how_many_elementary_addbacks++;
        }
        else
        {
            // this interval does not fullfill the conditions, all participants rejected,
            // and can be used for non-addback spectra
            dcout << "The addback in this interval is not possible because:" ;

            if(addb_multiplicity > max_fold || (addb_multiplicity < 2))
                dcout << " Addback multiplicity = " << addb_multiplicity
                      //      << "  (max fold = "
                      //      << max_fold
                      << " " << endl;

            if(addb_multiplicity > 1)
            {
                if(adjacent_situation == false && adjacent_mode)
                    dcout << "     (You wanted the  Adjacent situation - and it is not!)" << endl;

                if(noadjacent_situation == false && nonadjacent_mode)
                    dcout << "(You wanted the  NONAdjacent situation - and it is adjanced!)" << endl;

                if(primary_energy < Eprimary_threshold)
                    dcout << "    Primary energy " << primary_energy
                          << " is less than the defined threshold " << Eprimary_threshold << " " << endl;
            }
        }
        //cout << "Bottom of the Loop over active crystals, fastest_nr = " << fastest_nr << endl;

    } // end of for over all active good crystals in the vector
    //  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    dcout << "========== End of multi ADDING BACK for "
          << name_of_this_element << endl;

    /*
      if(
      //multi_addback_4MeV_cal[0] !=  addback4MeV_cal
     //     ||
          flag_multi_addback_incrementer_valid[0] != flag_addback_incrementer_valid)
      {
        cout << "Skandal \nmulti_addback_4MeV_cal[0] =  " << multi_addback_4MeV_cal[0]
        << " flag_multi_addback_incrementer_valid[0]= " <<  flag_multi_addback_incrementer_valid[0]
        << "\naddback4MeV_cal= " << addback4MeV_cal
        << " flag_addback_incrementer_valid=" << flag_addback_incrementer_valid
        << endl;
        cout << endl;
      }
    */
    //   multi_addback_4MeV_cal[0] = 300; flag_multi_addback_incrementer_valid[0] = true;
    //   multi_addback_4MeV_cal[1] = 600; flag_multi_addback_incrementer_valid[1] = true;
    //   multi_addback_4MeV_cal[2] = 900; flag_multi_addback_incrementer_valid[2] = true;

}
//***************************************************************************************************
bool Tcluster_xia::for_multi_addback_selfgate::check_selfgate(Tself_gate_abstract_descr *desc)
{
    dcout
            << parent->name_of_this_element
            << ", $$$$$ Checking the addback selfgate for addback nr "
            << nr_of_addback
            << endl;

    // here we assume that this is really germ cluster for addback
    Tself_gate_ger_addback_descr *d = (Tself_gate_ger_addback_descr*)desc ;

    if(d->enable_en4gate)
    {
        dcout << "is " << (parent->multi_addback_4MeV_cal[nr_of_addback])
              << " in the gate? ";
        if(parent->multi_addback_4MeV_cal[nr_of_addback] < d->en4_gate[0]
                ||
                parent->multi_addback_4MeV_cal[nr_of_addback] > d->en4_gate[1])
        {
            dcout << " --- No" << endl;
            return false ;
        }
        dcout << " --- Yes!!!!" << endl;
    }

    if(d->enable_time_gate)
    {
        // recent idea of Piotrek
        // NOTE addback_time_cal; // here we will keep not the time of the primary, but
        // the fastest (of those participating in addback) (max value of time cal)
        if(parent->multi_addback_time_cal[nr_of_addback] < d->time_gate[0]
                ||
                parent->multi_addback_time_cal[nr_of_addback] > d->time_gate[1])
            return false ;
    }

    // banana gate according to Marek Pfutzner idea
    if(d->enable_energy_time_polygon_gate)
    {
        double selected_time = 0 ;
        switch(d->which_time)
        {
        default:
        case Tself_gate_ger_addback_descr::time_cal:
            selected_time = parent->multi_addback_time_cal[nr_of_addback];
            break;
#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED
        case Tself_gate_ger_addback_descr::LR_time_cal:
            selected_time = parent->multi_addback_LR_time_cal[nr_of_addback];
            break;
        case Tself_gate_ger_addback_descr::SR_time_cal:
            selected_time = parent->multi_addback_SR_time_cal[nr_of_addback];
            break;
#endif // LONG_RANGE_SHORT_RANGE_TDC_USED

        }

        if(d->polygon->Test(selected_time, parent->multi_addback_4MeV_cal[nr_of_addback]) == false)
        {
            //cout << " is outside " << endl;;
            return false;
        }
    }






#ifdef LONG_RANGE_SHORT_RANGE_TDC_USED

    if(d->enable_LR_time_gate)
    {
        // recent idea of Piotrek
        // NOTE addback_time_cal; // here we will keep not the time of the primary, but
        // the fastest (of those participating in addback) (max value of LR_time cal)
        if(parent->multi_addback_LR_time_cal[nr_of_addback] < d->LR_time_gate[0]
                ||
                parent->multi_addback_LR_time_cal[nr_of_addback] > d->LR_time_gate[1])
            return false ;
    }


    if(d->enable_SR_time_gate)
    {
        // recent idea of Piotrek
        // NOTE addback_time_cal; // here we will keep not the time of the primary, but
        // the fastest (of those participating in addback) (max value of LR_time cal)
        if(parent->multi_addback_SR_time_cal[nr_of_addback] < d->SR_time_gate[0]
                ||
                parent->multi_addback_SR_time_cal[nr_of_addback] > d->SR_time_gate[1])
            return false ;
    }
#endif

    //=========================================
    //=========================================
    // geometry of the current primary segment
    if(d->enable_geom_theta_gate)
    {
        double theta_geom = parent->crys[parent->
                                         multi_addback_primary_crystal_nb[nr_of_addback]]->give_theta_geom();
        if(theta_geom < d->geom_theta_gate[0] || theta_geom > d->geom_theta_gate[1])
            return false;
    }
    if(d->enable_geom_phi_gate)
    {
        double phi_geom = parent->crys[parent->
                                       multi_addback_primary_crystal_nb[nr_of_addback]]->give_phi_geom();
        if(phi_geom < d->geom_phi_gate[0] || phi_geom > d->geom_phi_gate[1])
            return false;
    }


    //  it is possible that somebody sets the selfgate on the BGO signal
    //  which are the feature of the cluster (not  crystal). In such a case we
    //  test only BGO and multipilicyty - because, here in cluster only
    //  this is available

    /*
      if(d->enable_mult_in_cluster_gate)
      {
        int mult =  parent->get_this_cluster_multiplicity();
        if( mult < d->mult_in_cluster_gate[0]
            ||
            mult > d->mult_in_cluster_gate[1]
          )
          return false ;
      }*/

#ifdef BGO_PRESENT

    if(d->enable_BGO_in_cluster_gate)
    {
        int BGO_energy =  parent->get_this_cluster_BGO_energy();
        if(BGO_energy < d->BGO_in_cluster_gate[0]
                ||
                BGO_energy > d->BGO_in_cluster_gate[1]
          )
            return false ;
    }
#endif // def BGO_PRESENT

    return true ; // when EVERYTHING was successful
}
#undef dcout

