//---------------------------------------------------------------

/*
make CFLAGS='-g -O0' CXXFLAGS='-g -O0' clean all
*/


#include "experiment_def.h"

#include "tjurekabstracteventprocessor.h"

#include "TIFJEvent.h"
#include "TIFJAnalysis.h"

#include "TIFJEventProcessor.h"


// #include "Go4EventServer/TjurekMbsEvent.h"

#include <iostream> // for test printouts
// #include <iomanip> // for test printouts
#include <sstream>
#include <algorithm>

using namespace std;

// #include "spectrum.h"
#include "Tfile_helper.h"
// #include "tsignal_processing.h"

// #include "Trising.h"  // for cluster characters
#include "Thector.h"  // for cluster characters


// #include "visitcard.h"
// #include "DataDecoder.h"
#include "v1724.h"
#include "v775.h"
#include "v785.h"
#include "v878.h"
#include "v879.h"
#include "v830.h"
#include "v1730B.h"
#include "v1190.h"
#include "v2740.h"

v1730B  d;  // decoder for digitizer
v775    d775 ("V775");  // decoder for digitizer v775 caen module
v785    d785 ("V785");  // decoder for digitizer v785 caen module
v879    d879 ("V879");  // decoder for digitizer v789 caen module
v1724   d1724; // decoder for module 1724
v830    d830;
v1190   d1190("V1190");
v2740   d2740("V2740");

#include "LTable.h"
LTable *ltb_kratta;
LTable *ltb_plastic;
LTable *ltb_paris;
LTable *ltb_silicon;

//#include "Tlookup_table_kratta.h"
#define FOUR_BRANCHES_VERSION   1
#define NEW_FRS_LOOKUP_TABLE   true

// for debugging purposes
#define POKAZ 0

//#define LOOKING 1
static const unsigned TIMESTAMP_QUEUE_MAX = 10; // was 10

//extern istream &zjedz(istream &plik);
#define DBG(x)     // cout << x << endl
//******************************************************************
TIFJEventProcessor::TIFJEventProcessor(string namearg) :
    TjurekAbstractEventProcessor(namearg)
  #if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
  ,
    lookup_xia_energies(22, 31, "Cluster_energy_xia"),
    lookup_xia_times(22, 31, "Cluster_time_xia")
  #endif

  #if   ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||         (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
      || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ) \
      || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008) \
      || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) )
  ,
    lookup_xia_energies(30, 4, "Cluster_energy_xia"),
    lookup_LR_times(22, 128, "Cluster_LR_time"),
    lookup_SR_times(22, 31, "Cluster_SR_time")
  #endif

  #ifdef  ACTIVE_STOPPER_PRESENT
  ,
    lookup_active_stopper(22, 31)
  #ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
  , lookup_active_stopper_time(22, 128)
  #endif
  #endif

  #ifdef  ACTIVE_STOPPER2_PRESENT
  ,
    lookup_active_stopper2(22, 31)
  #ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
  , lookup_active_stopper2_time(22, 128)
  #endif
  #endif

  #ifdef  ACTIVE_STOPPER3_PRESENT
  ,
    lookup_active_stopper3(22, 31)
  #ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
  , lookup_active_stopper3_time(22, 128)
  #endif
  #endif

  #ifdef  POSITRON_ABSORBER_PRESENT
  , lookup_absorber_0_1(22, 31)
  , lookup_absorber_2_9(2, 256)
  //    , lookup_absorber_rear(22, 31)
  , lookup_absorber_back_sides(22, 31)    // comming from VME

  #endif

  #ifdef  MUNICH_MATRIX_STRIPES_X
  //    ,  lookup_stopper_matrix(22, 31)
  #endif
  #ifdef  MUN_ACTIVE_STOPPER_PRESENT
  ,
    lookup_active_stopper_x(2, 200),    // comming from DSP (Gassiplex)
    lookup_active_stopper_y(22, 31),    // comming from VME readout
    lookup_multiplicity_module(22, 31)
  #endif

{
    //     TRACE((14,"TIFJEventProcessor::TIFJEventProcessor(string)",
    //            __LINE__,
    //            __FILE__));



    // setup_Piotr_Pawlowski_lookup_tables();


    flag_old_style_pulser_in_b7 = true; // during commisioninig experiment
    // after discovering first pulser in Master trigger word -
    // this flag will be switched automatically

    // we remember for private functions
    target_event = nullptr;
    // TjurekMbsEvent* input_event ;
    input_subevent = nullptr;
    input_event_data = nullptr; // one event buffer
    how_many_data_words = 0 ; // how many sensible data in the buffer
    memset(&vmeOne[0][0], 0, sizeof(vmeOne));
    memset(&vmeUser[0][0], 0, sizeof(vmeUser));

}
//******************************************************************
void TIFJEventProcessor::setup_Piotr_Pawlowski_lookup_tables(string data_fname_with_path)
{

    //cout << "we are at " << __func__ << endl;
    static string previous_path;

    string current_path_for_lookup_table = "./calibration/";
    auto pos = data_fname_with_path. rfind("/");
    if(pos == string::npos)
    {
        // if this from current directory, just use ./calibration
        current_path_for_lookup_table = "./calibration/";
    }
    else{
        current_path_for_lookup_table = data_fname_with_path.substr(0, pos+1);
        //cout << "sciezka = " << current_path_for_lookup_table << endl;

    }



    // if(previous_path == current_path_for_lookup_table ) return; // no work needed
    previous_path = current_path_for_lookup_table;

    // warning: it always make 'continuation' - so to make it
    // again, new version we need to reset it somehow;
    // Piotr promised to make some member function which
    // will make it.


    if(ltb_kratta != nullptr){
        delete ltb_kratta;
    }
    ltb_kratta = new LTable;

    string proper_ltb_file = find_proper_lookuptable_name("kratta.ltb",
                                                          current_path_for_lookup_table,
                                                          data_fname_with_path
                                                          );


    ltb_kratta->ReadFile((current_path_for_lookup_table + proper_ltb_file).c_str());
    //ltb_kratta->ReadFile((current_path_for_lookup_table + "kratta.ltb").c_str()); // lookup table

    //---------------------
    if(ltb_paris != nullptr){
        delete ltb_paris;
    }
    ltb_paris = new LTable;

    proper_ltb_file = find_proper_lookuptable_name("paris.ltb",
                                                   current_path_for_lookup_table,
                                                   data_fname_with_path
                                                   );
    ltb_paris->ReadFile((current_path_for_lookup_table + proper_ltb_file).c_str()); // lookup table
    //ltb_paris->ReadFile((current_path_for_lookup_table + "paris.ltb").c_str()); // lookup table

    //----------------------------- common for plastic and silicon --------------------
    if(ltb_plastic != nullptr){
        delete ltb_plastic;
    }
    ltb_plastic = new LTable;


    proper_ltb_file = find_proper_lookuptable_name("plastic.ltb",
                                                   current_path_for_lookup_table,
                                                   data_fname_with_path
                                                   );
    ltb_plastic->ReadFile((current_path_for_lookup_table + proper_ltb_file).c_str()); // lookup table
    ltb_plastic->Print();

    // ltb_plastic->ReadFile((current_path_for_lookup_table + "plastic.ltb").c_str()); // lookup table



    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // TODO: CZY TO BLAD?- UZYWAMY TU TEGO SAMEGO LTB_PLASTIC CO POWYZEJ !!!!!!!!!!!!!!!
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    // proper_ltb_file = find_proper_lookuptable_name("silicon.ltb",
    //                                                current_path_for_lookup_table,
    //                                                data_fname_with_path
    //                                                );

    // ltb_plastic->ReadFile((current_path_for_lookup_table + proper_ltb_file).c_str()); // lookup table

    //ltb_plastic->ReadFile((current_path_for_lookup_table + "silicon.ltb").c_str()); // lookup table

    // four silicons of Natalia ---------------------------------------------------------------------
    if(ltb_silicon != nullptr){
        delete ltb_silicon;
    }
    ltb_silicon = new LTable;
    proper_ltb_file = find_proper_lookuptable_name("silicon.ltb",
                                                   current_path_for_lookup_table,
                                                   data_fname_with_path
                                                   );

    ltb_silicon->ReadFile((current_path_for_lookup_table + proper_ltb_file).c_str()); // lookup table
    ltb_silicon->Print();

}
//******************************************************************
string TIFJEventProcessor::find_proper_lookuptable_name(string ltb_name,
                                                        string current_path_for_lookup_table,
                                                        string data_file_name_with_path)
{


    // cout << "working with " << ltb_name << endl;
    // making a filter
    auto kropka_pos =  ltb_name.rfind(".ltb");
    if(kropka_pos == string::npos)
    {
        cout << "Dear Programmer: in the name of lookuptable period is missing" << endl;
        exit(8);
    }
    string filter = ltb_name.substr(0, kropka_pos);

    // look into current path for all lookuptable of this kind
    auto vec_ltb_names = FH::find_files_in_directory(current_path_for_lookup_table, filter );
    if(vec_ltb_names.size() == 1)
    {
        return vec_ltb_names[0];
    }

    // sort alphabetically
    sort(vec_ltb_names.begin(), vec_ltb_names.end() );

    vec_ltb_names = remove_names_without_extension_ltb(vec_ltb_names);

    // extract run numbers from these files (making a map?)
    vector<int>  run_numbers;
    int run_number;
    for (unsigned int i = 0 ; i < vec_ltb_names.size(); ++i)
    {
        string one_name = vec_ltb_names[i];
        one_name = one_name.substr(kropka_pos);
        istringstream s(one_name);
        s >> run_number;
        if(!s) run_number = 0;
        run_numbers.push_back(run_number);
    }
    // check
    //    for(uint n = 0 ; n < run_numbers.size(); ++n)
    //    {
    //        cout << n << ") " << run_numbers[n] << " --> "
    //             << vec_ltb_names[n] << endl;
    //    }

    // extract the run number from data file
    auto data_run_pos = data_file_name_with_path.rfind("run");
    if(data_run_pos == string::npos)
    {
        cout <<  "No 'run' in a runfile name?" << data_file_name_with_path << endl;
        exit(9);
    }
    data_run_pos += 3; // 3 letters has string "run"
    istringstream sd(data_file_name_with_path.substr(data_run_pos));
    int data_run_nr;
    sd >> data_run_nr;
    // cout << "Current run has number " << endl;



    // check which lookuptable is suitable for this run number
    string found_ltb_name = ltb_name;
    for(uint n = 0 ; n < run_numbers.size(); ++n)
    {
        if(data_run_nr < run_numbers[n]) continue;

        //        cout << "For run number " << data_run_nr << " is higher or equal "
        //             << n << ") " << run_numbers[n] << " --> "
        //             << vec_ltb_names[n] << endl;

        found_ltb_name = vec_ltb_names[n];
    }

    // cout << "Proper ltb file is:" << found_ltb_name << endl;
    // return this lookuptable name
    return found_ltb_name;
}
//******************************************************************
vector<string> TIFJEventProcessor::remove_names_without_extension_ltb(vector<string> v)
{
    vector<string> vnew;
    for( size_t n = 0 ;  n < v.size() ; ++n )
    {
        //        cout<< v[n] << " pracujemy... z " << v[n] << endl;
        //        cout<< v[n].substr(v[n].size() - 4) << " substring " << v[n] << endl;

        if(v[n].substr(v[n].size() - 4) == ".ltb" )
        {
            vnew.push_back( v[n]);
        }else {
            //            cout << "nazwa opuszczona" << endl;
        }
    }
    return vnew;
}
//******************************************************************
TIFJEventProcessor::~TIFJEventProcessor()
{
    //     TRACE((14,"TIFJEventProcessor::~TIFJEventProcessor()",
    //            __LINE__,
    //            __FILE__));

}
//******************************************************************
// unpacking
// *****************************************************************
void TIFJEventProcessor::BuildEbEvent(
        TGo4EventSourceParameter *source_of_events, TIFJEvent *target)
{
    //    cout << "Function BuildEbEvent================================" << endl;

    source_of_events_ptr = source_of_events;
    target_event = target; // to remember for other private functions
    target_event->Clear();

#if 0
    input_event_data = source_of_events-> give_next_event(&how_many_data_words);
    if(!input_event_data) {
        cout << "BulidEvent in TIFJEventProcessor: no input event !" << endl;
        cout << "Event False" << endl;
        target_event->SetValid(false);
        return;
    }
    current_ev_nr = source_of_events->give_event_nr();
#endif



    int result = 0 ;

    source_of_events->give_next_event(&result);


    // perhaps we need a new lookuptable?
    if(source_of_events->is_new_lookup_table_required())
    {
        // extracting path and // installing new lookup table from this path
        setup_Piotr_Pawlowski_lookup_tables(
                    source_of_events->give_name_of_source()
                    );
        source_of_events->new_lookup_table_just_installed();
    }



    if(result != GETEVT__SUCCESS) {
        if(result == GETEVT__NOMORE) {
            cout << "Should never come here - the exception should be thrown before"
                 << endl;
            exit(12);
        } else {
            cout << "TIFJEventProcessor::BuildEbEvent : Some error while  mbs.get_next_event() " << endl;
            exit(11);
        }
    }


    current_ev_nr = source_of_events->give_event_nr()  ;
    //     cout << "# Event nr " << current_ev_nr << endl;


    if(unpack_the_CCB_event()) {
        target_event->SetValid(true);
        //         cout << "Correctly reconstructed event ------------" << endl;

        // not used anymore, as the tree for hect_kratta is available
        //	target_event->save_as_semitree(source_of_events->give_name_of_source());
    } else {
        target_event->SetValid(false);
        //         cout << "Event not constructed, wrong event -------" << endl;
    }


    //     cout << "end of event processor function." << endl;
}
//***********************************************************
bool  TIFJEventProcessor::unpack_kratta_hector_event(const_daq_word_t *  /*data*/, int /*how_many_words*/)
{
    cout << showbase << dec;

    // cout << "NEW kratta_hector EVENT ===\n";

    mbs_listner &mbs = *(source_of_events_ptr->give_mbs());
    int how_many_subevents = 99999;// at first we do not know

    // Loop over subevents of this event
    for(int subev_nr = 0; subev_nr < how_many_subevents ;  ++subev_nr)
    {
        int result = mbs.get_next_subevent_if_any();
        how_many_subevents = mbs.give_how_many_subs();

        // cout << "This event has " << how_many_subevents
        //      << " subevents (now is: " << subev_nr << ")" << endl;

        if(result  != GETEVT__SUCCESS)  // moze bardzej subtelnie? Podwody
        {
            if(result == GETEVT__NOMORE) {
                // cout << "Koniec eventu " << current_ev_nr
                //      << " bec. result is GETEVT__NOMORE = " << result << endl;
                break;
            } else {
                cout << "Very strange error because result is " << result << endl;
                exit(9);
            }
        }
        // cout << "------ Subevent nr " << subev_nr << " -----------------------" << endl;
        // recognising


        uint32_t *data = mbs.data();
        int length = mbs.length();
        if(length == 0) continue;

        //-----------------------------------------------
        // Recognise what subevent will be unpacked now
        //-----------------------------------------------
        //                cout << "Unpacking event nr " << current_ev_nr << " subevent nr " << subev_nr << endl;


        //                int s = mbs.subtype() ;
        //                int t = mbs.type();
        //                int p = mbs.procid();
        //                cout << "Subevent nr " << subev_nr << ", is:  subtype/type " << s << " / " << t << ", procid " << p
        //                     << ", h_control = " << (int) (mbs.control())
        //                     << ", length " << length << endl;


        if(mbs.subtype() == 36 && mbs.type() == 2800)
        {
            unpack_trigger_pattern(current_ev_nr, subev_nr, (uint32_t *) data, length);
        }
        else if(mbs.subtype() == 10 && mbs.type() == 1)
        {
            if(mbs.control() == 1){
                unpack_caen_v1724_kratta_digitizer(current_ev_nr, subev_nr, (uint32_t *) data, length);
            }
            else if(mbs.control() == 2){
                //                if(length > 0)
                //                    cout << "JEST" << endl;
                unpack_ccb_non_kratta_z_metryczkami(( (uint32_t *) data), true, length); // true means - it is a real HECTOR
            }
        }
        else if(mbs.subtype() == 34 && mbs.type() == 1) {
            unpack_34_1(current_ev_nr, subev_nr, (uint32_t *) data, length);
        } else if(mbs.subtype() == 36 && mbs.type() == 2700) {
            unpack_36_2700(current_ev_nr, subev_nr, (uint32_t *) data, length);
        } else if(mbs.subtype() == 32 && mbs.type() == 1130) {
            unpack_32_1130(current_ev_nr, subev_nr, (uint32_t *) data, length);
        } else if(mbs.subtype() == 36 && mbs.type() == 9494) {
            unpack_36_9494(current_ev_nr, subev_nr, (uint32_t *) data, length);
        } else {
            cout << "Nieznany typ eventu -  \n\n\n\n"
                 << "Type = " << mbs.type()
                 << " subtype " << mbs.subtype()
                 << endl;
            return false;
        }
    }   // end of for over subevents
    return true;
}
//***********************************************************
void TIFJEventProcessor::swap_nr_words(short int *pointer, int how_many)
{
    int tmp;
    //cout << "Swapping " << how_many  << " words " << endl;

    for(int i = 0; i < how_many; i += 2, pointer += 2) {
        tmp = *(pointer + 1);
        *(pointer + 1) = *(pointer);
        *(pointer) = tmp;
    }

}
//**********************************************************
//**********************************************************
/** Checks if the GER event is empty or it has some data */
bool TIFJEventProcessor::CheckForGerData(const_daq_word_t */*data*/, int /*length*/)
{
#ifdef RISING_GERMANIUMS_PRESENT
    struct vxi_word {
        unsigned data :
            16;
        unsigned group :
            8;
        unsigned item :
            6;
        unsigned two :
            2;
    };

    union {
        unsigned int raw;
        vxi_word vxi;
    };

    bool hasGerData = false;

    for(int i = 0; i < length / 2; i++) {
        raw = data[i];
        // ignore empty words
        if(!raw)
            continue;

        if(vxi.group == 0xf8) {
            // event number
        } else if(vxi.group == 0xf7) {
            // timing module
        } else {
            int co = vxi.item % 3; // is it: E20Mev, E4Mev, Time
            int item_crys = vxi.item / 3;

            if(co > 2) {
                // TODO complain
                continue;
            }

            int nr_cluster, nr_segment, nr_signal;
            if(lookup.current_combination(vxi.group - 1, item_crys,
                                          &nr_cluster, &nr_segment, &nr_signal)) {
                if(nr_cluster > 14) {
                    // TODO complain
                    continue;
                }
                if(nr_segment > 6) {
                    // TODO complain
                    continue;
                }
                hasGerData = true;
                break; // ??????????
            }
        }
    }

    if(hasGerData)
        ger_physics++;
    else
        ger_physics_empty++;

    return hasGerData;
#else
    return false;
#endif
}

//************************************************************
void TIFJEventProcessor::unpack_ger_subevent(const timestamped_subevent &/*t*/,
                                             TIFJEvent * /*target_event*/)
{}

//********************************************************************************************************************
void TIFJEventProcessor::unpack_frs_subevent(const timestamped_subevent &/*t*/,
                                             TIFJEvent */*te*/)
{
    cout << "Empty  function " << endl;

}

//using namespace Enum;

//*******************************************************************************************************************
void TIFJEventProcessor::distribute_frs_vme_words_to_event_members(
        TIFJEvent *  /*target_event_arg*/)
{

    if(frsOne_lookup_should_be_loaded) {
        read_frsOne_lookup_table();
    }

    for(unsigned int i = 0; i < frsOne_lookup_table.size(); i++) {
        frsOne_lookup_table[i].put_this_item(vmeOne);
    }

    if(frsUser_lookup_should_be_loaded)
        read_frsUser_lookup_table();
    for(unsigned int i = 0; i < frsUser_lookup_table.size(); i++) {
        frsUser_lookup_table[i].put_this_item(vmeUser);
    }

    // this is always - new and the old version - because we need this also in the event
    // as the vme signals

    for(int i = 0; i < 32; i++) {
        target_event->module_vmeOne_0[i] = vmeOne[0][i];
        target_event->module_vmeOne_1[i] = vmeOne[1][i];
        target_event->module_vmeOne_2[i] = vmeOne[2][i];
        target_event->module_vmeOne_3[i] = vmeOne[3][i];
        target_event->module_vmeOne_4[i] = vmeOne[4][i];

        target_event->module_vmeOne_5[i] = vmeOne[5][i];
        target_event->module_vmeOne_6[i] = vmeOne[6][i];
        target_event->module_vmeOne_7[i] = vmeOne[7][i];
        target_event->module_vmeOne_8[i] = vmeOne[8][i];
        target_event->module_vmeOne_9[i] = vmeOne[9][i];
        target_event->module_vmeOne_11[i] = vmeOne[11][i];
        target_event->module_vmeOne_13[i] = vmeOne[13][i];
        target_event->module_vmeOne_15[i] = vmeOne[15][i];
        target_event->module_vmeOne_16[i] = vmeOne[16][i];
        target_event->module_vmeOne_17[i] = vmeOne[17][i];
        target_event->module_vmeOne_18[i] = vmeOne[18][i];
        target_event->module_vmeOne_19[i] = vmeOne[19][i];
    }

    for(int i = 0; i < 32; i++) {
        target_event->module_vmeUser_5[i] = vmeUser[5][i];
        target_event->module_vmeUser_6[i] = vmeUser[6][i];
        target_event->module_vmeUser_8[i] = vmeUser[8][i];
        target_event->module_vmeUser_9[i] = vmeUser[9][i];
        target_event->module_vmeUser_11[i] = vmeUser[11][i];
        target_event->module_vmeUser_13[i] = vmeUser[13][i];
        target_event->module_vmeUser_15[i] = vmeUser[15][i];
        target_event->module_vmeUser_16[i] = vmeUser[16][i];
        target_event->module_vmeUser_17[i] = vmeUser[17][i];
        target_event->module_vmeUser_18[i] = vmeUser[18][i];
        target_event->module_vmeUser_19[i] = vmeUser[19][i];
    }

}
//***********************************************************************
void TIFJEventProcessor::unpack_hec_subevent(const timestamped_subevent &t,
                                             TIFJEvent *te)
{
#ifdef HECTOR_PRESENT
    // to have the knowledge when it happened
    //     te->timestamp_HEC = t.give_ts();
    //     te->HEC_time_since_beginning_of_spill =
    //         te->timestamp_HEC - time_stamp_beg_of_last_spill;

    unsigned int how_many_words = t.length();

    for(unsigned int i = 0; i < how_many_words; i++) {
        int word = t.give_word(i);

        int channel = (word >> 16) & 0xff;
        int data = word & 0x3fff; // mask to 12 bits

        if((word & 0xff000000) == 0x30000000)
            te->hector_tdc[channel] = data;
        else if((word & 0xff000000) == 0x38000000) {
            te->hector_adc[channel] = data;

        }
    }
#endif

    hec_subev_queue.pop();

    //   if( collect_time_stamp_statistics_spectra )
    //   {
    //     paczuszka.second = te->hector_adc[4];
    //     hec_val_que.push_back(paczuszka);
    //   }

}
//****************************************************************************
/** unpacking the DGF subevent */
void TIFJEventProcessor::unpack_dgf_subevent(const timestamped_subevent &  /*t*/,
                                             TIFJEvent * /*te*/)
{
    //cout << "Unpacking the DGF subevent " << endl;

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

    // new version mage for g-factor campaign ========================
    //================================================================
    //    cout << " unpacking next DGF subevent-----------------------------"
    //       << endl;

#define SHOW_DETECTORS 0
    // to have the knowledge when it happened
    target_event->timestamp_DGF = t.give_ts();
    target_event->DGF_time_since_beginning_of_spill =
            target_event->timestamp_DGF - time_stamp_beg_of_last_spill;

    // show ---------------------------------------------
    unsigned int dgf_subevent_length = t.length();
#ifdef NEVER

    //  cout << dgf_subevent_length << " words inside subevent " << endl;

    bool babe_found = false;
    // do not take these descriptions too seriously
    char *descriptions [100] = {"ID", "Timestamp 1",
                                "Timestamp 2", "Timestamp 3",
                                "Time calibrator"
                               };

    for(unsigned int i = 0; i < dgf_subevent_length; i++) {
        cout << "Word nr " << i
             << "\thex= 0x" << hex << setw(12) << t.give_word(i)
             << ", dec = " << dec << setw(12) << t.give_word(i);
        //        if(i==2)
        //            cout << " masked RUN TASK  = " << hex
        //                 <<  (t.give_word(i) - 0x1000) << dec;
        //         if(i < 4)
        //             cout  << "           " << descriptions[i] ;

        if(!babe_found) {
            vme_word word;
            word.raw_word = t.give_word(i);
            if(word.raw_word == 0xbabe0000) {
                babe_found = true;
            } else {
                cout
                        << " Geo= " << word.data_word.geo
                        << " code= " << word.data_word.code
                        << " channel= " << word.data_word.channel
                        << " data= " << word.data_word.data;
            }
        }
        cout << endl;
    }
#endif

    if(t.length() == 0) {
        dgf_subev_queue.pop();
        return; // empty subevent;
    }

    // synchro information
    // NOTE: here, instead the "debug" word
    // we placed the synchro time data

    //  cout << "in unpack_dgf_subevent, dgf synchro data "
    //       << t.give_word(0)
    //       << ", hex = 0x" << hex
    //       << t.give_word(0) << dec << ", truncated "
    //       << (t.give_word(0)  & 0x3fff)
    //       << endl ;

    // synchro data (so called time calibrator) comes as the word 4

    target_event->dgf_synchro_data = t.give_word(0) & 0x3fff;
    // Henning says that  if this is a synchro data the germanium
    // data may  have a lot of garbage, so ignore this contents

    if(target_event->dgf_synchro_data) {
        //      cout << "Have a synchro! = "
        //              << (t.give_word(0) & 0x3fff)
        //              << endl;
    }

    if(t.give_word(0) != 0x40000000) {
        //    cout << "DGF - Time calibrator event, so we skip the contents"
        //         << endl;
        dgf_subev_queue.pop();
        return;
    }

    // at first we unpack the TIMES information ########################
    // which is comming in the first part of the event.
    // (The energy comes in a different part, later)

    // loop over unknown nr of words

    vme_word word;
    // memset(&vme[0][0], 0, sizeof(vme) );

    unsigned int analysed_word = 1; // bec synchor time calibratoro
    //bool  time_to_finish= false ;

    // zero was used, now we start from 1
    for(unsigned int i = analysed_word;; i++, analysed_word++) {
        //    cout << "Unpacking vme word for time information " << i << endl;
        word.raw_word = t.give_word(i);
        if(!word.raw_word) {  // do not analyse empty words
            // TODO complain
            continue;
        }

        // Sometimes Henning is sending the extra word on the end of the VME block
        // with the pattern 0xbabe****
        if((word.raw_word & 0xffff0000) == 0xbabe0000) {
            analysed_word++; // prepare next word and jump out
            break; // end of the loop
        }

        switch(word.header_word.code)    // <-- it is common to all of types
            // of vme words
        {
        case 2: {
            //      cout << "Caen TDC Header .." << endl;
        }
            break;

        case 0:
            // sizes are hardcoded in the class definition
            if(word.data_word.geo < 22 && word.data_word.channel < 32) {

#if SHOW_DETECTORS
                cout << "Arrived TIME geo = " << word.data_word.geo
                     << ", chan = " << word.data_word.channel
                     << ", data = " << word.data_word.data;
#endif // SHOW_DETECTORS
                int clus = 0;
                int crys = 0;
                if(lookup_xia_times.current_combination(word.data_word.geo,
                                                        word.data_word.channel,
                                                        &clus,
                                                        &crys)) {

#if SHOW_DETECTORS
                    cout << " --- this is time for cluster "
                            //          << clus
                            //          << " crystal " << crys
                            //          << " which is "
                         << Trising::cluster_characters[clus] << "_"
                         << char('1' + crys) << endl;
#endif // SHOW_DETECTORS
                    // storing the time data
                    // no need to we reduce the  resolution from 64 KB to 8KB !!!
                    target_event->ger_xia_dgf[clus][crys][0] = word.data_word.data;
                    target_event->ger_xia_dgf_fired[clus][crys] = true;
                } else {
#if SHOW_DETECTORS
                    //          cout << "DGF unpacking. Warning: combination GEO = "
                    //          <<  word.data_word.geo
                    //          << ", CHANNEL = " << word.data_word.channel
                    //          << " was not specified in a lookup table "
                    //             "Cluster_time_xia" << endl;
#endif // SHOW_DETECTORS

                }
            }
            break;

        case 4:
            //      cout << "Footer  when i = .." << i << endl;

            break;
        case 6:
            //      cout << "'Not valid datum for this TDC when i = .."
            //           << i << endl;
            break;

        default :
            //      cout << "No time information  when i = .." << i << endl;
            break;
        }

    }

    //  cout << "analysed_word = " << analysed_word << endl;

    //  cout << "-------------Now we come to the energy ################"
    //       << endl;
    //###############################################################
    // The structure is like this

    // We will have many times the following sequence (one such sequence
    // describes electronic module which sends data)

    // BUFFER HEADER  (5 words)
    //  [0] - Number of words in this buffer  (comming from the particular
    //                                          module)
    //  [1] - Module number (this we need to tell to our lookup table) !!!!
    //  [2] - Format descriptor = RunTask + 0x1000  (says which version
    //        of data complicity we use 9,4,or 2 words)
    //  [3] - Buftime HI  (not important)
    //  [4] - Buftime MI  (not important)
    //  [5] - Buftime LO  (not important)
    //
    // EVENT HEADER  (3 words)
    //  [6] - Hit pattern (by this we calculate the 'channel' which
    //        fired (we need this for lookup table)
    //  [7] - Event time High   (not important)
    //  [8] - Event time Low   (not important)
    //
    // Now for each bit in the bit pattern we will have the data
    // from the 'channel'.
    // The length of this data depends on Format descriptor, see word [2] above.
    // If we choose length 2 the following structure looks like this
    // CHANNEL HEADER (if 2 words old_long)   (will come as many times
    // as many bits were set in the pattern [6]
    //  [ 9]  Fast trigger time  (so called DGF time, we do not rely on it
    //        too much)
    //  [10]  Energy - the essential data - result of the germanium
    //        detector conversion


    //  int how_many_modules = t.give_word(extra_word) & 0xffff ;
    // only the lower word. Upper can contain some extra information
    //
    //  // we have maximum 54 modules !!!!!!!!!!!!!!!!!!!!!!!!!
    //  if(how_many_modules > 54) how_many_modules = 54 ;
    // !!!!!!!!!!!!!!!!!!!!!!!! CHEATING !!!!!!!!!!!!
    //


    // position of the particular information in the incomming data
    // ------------------------
    // Warning: as I see Henning gave me now the structure which is one
    // word shorter. For the former
    //             dgf data there was an extra word nr [3] <--- how many
    //             events in this module.
    //             now it does not exist, so the calculation of indexes
    //             in the lines below are changed

    unsigned int index_of_beg_of_module = analysed_word;
    //-----------------------------------------------------------

    // see the instructtion for explanations
    int how_many_words_per_event = 777;

    //  cout << "How many modules = " << how_many_modules << endl;

    for(unsigned int module = 0; analysed_word < dgf_subevent_length; module++) {

        //    cout << "Unpacking next module, counter = " << module << endl;

        int index_of_beg_of_next_module = index_of_beg_of_module +
                t.give_word(index_of_beg_of_module);
        int index_of_module_number = index_of_beg_of_module + 1;
        int index_of_format_descriptor = index_of_beg_of_module + 2;
        int index_of_hit_pattern = index_of_beg_of_module + 6;
        int index_of_first_data = index_of_hit_pattern + 3;

        //    cout << "Checking current indexes "
        //         << "\nindex_of_beg_of_module " <<index_of_beg_of_module
        //         << "\nindex_of_beg_of_next_module "
        //        <<index_of_beg_of_next_module
        //         << "\nindex_of_format_descriptor  "
        //         << index_of_format_descriptor
        //         << "\nindex_of_hit_pattern " << index_of_hit_pattern
        //         << "\nindex_of_first_data " << index_of_first_data
        //         << "\n how_many_words_per_event= "
        //         << how_many_words_per_event
        //         << endl;


        int nr_of_words_in_this_module = t.give_word(index_of_beg_of_module);

        if(abs(nr_of_words_in_this_module) == 0) {
            //cout << " this is an empty module (words is = 0)" << endl;
            dgf_subev_queue.pop();
            return;

        }

        if(abs(nr_of_words_in_this_module) > 5000) {
            cout << "DGF 2 unpacking:  Nonsense length of the module event " << endl;
            dgf_subev_queue.pop();
            return;
        }

        //  // taking out the events of this module
        switch(t.give_word(index_of_format_descriptor)) {
        case 256:
        case 257:
        case 512:
        case 513:
        case 256+ 0x1000:
        case 257+ 0x1000:
        case 512+ 0x1000:
        case 513+ 0x1000:
            how_many_words_per_event = 9;
            break;
        case 258:
        case 514:
        case 258+ 0x1000:
        case 514+ 0x1000:
            how_many_words_per_event = 4;
            break;

        case 259:
        case 515:
        case 259+ 0x1000:
        case 515+ 0x1000:
            how_many_words_per_event = 2;
            break;
        default:

            if(module < 10) {  // for Henning error walkaroud
                cout << "Error during unpacking DGF data - unknown value of"
                        " CHANHEADLEN !!! \n , Format Descriptor is 0x"
                     << hex << t.give_word(index_of_format_descriptor)
                     << ", dec= " << dec << t.give_word(index_of_format_descriptor)
                     << " (at MBS module nr " << module << ")\n";
            }

            dgf_subev_queue.pop();
            return;
        } // end of switch format descriptor


        int module_nr = t.give_word(index_of_module_number);
        //    cout << " This module has a module_nr = " << module_nr << endl ;

        if(t.give_word(1 + analysed_word) != 1) {
            //      cout << "The nr of events in the data buffer is "
            //     "not ONE ???? \n" ;
        }
        analysed_word += 6 + 3; // size of buffer header + event header


        //      cout << "Searching next dgf 'Event header' (bit pattern),  nr = "
        //           << dgf_event << " -----------------------\n";
        // now we have to take bitpattern
        unsigned int p = t.give_word(index_of_hit_pattern);
        if(!p)
            break; // empty patern ?

        // cout << "bit pattern found = 0x" << hex << p << dec ;
        // we unpack the data depending what coding system was used
        switch(how_many_words_per_event) {
        default : { // means 4
            cout << "!!! Unrecognized nr of words in the channel cheader"
                    "CHANHEADLEN = "
                 << how_many_words_per_event << endl;

            dgf_subev_queue.pop();
            return;

        } // end of case - default

        case 2: // 2 words per event
        case 9: { // 9 words for the event
            // max channels are in the DGF module ? (according to Henning)
            const int max_channels_per_dgf_block = 4;

            for(int chann = 0; chann < max_channels_per_dgf_block; chann++) {
                // cout << "Loop over channels  bit pattern,   chann = "
                // << chann << "  ####\n";
                if((p >> chann) == 0)
                    break; // no more bits in the pattern?

                int piker = 1 << chann;
                if(p & piker) {
                    //cout << "   !!!! There is a data for this channel nr "
                    // << chann << "  !!!!!!!\n";
                    //cout << " this is module_nr = " << module_nr << endl;

                    int clus = -1, crys = -1;
                    if(lookup_xia_energies.current_combination(module_nr, chann,
                                                               &clus, &crys)) {
                        //
                        // int dgf[8][3][7][8];    // 8 cryostats, 3 detectors,
                        //                 7 singals, each 8 items on dgf block
                        // bool dgf_fired[8][3];

                        //---------------------
                        // take data for channel chann

                        // look below, nr of cryostat given by the lookup table
                        // has already -1
                        // so far we treated equally case 2 and case 9. Now is time
                        // to treat them separately

                        switch(how_many_words_per_event) {
                        case 9:
                            cout << "DGF unpacking. Length 9 is not implemented" << endl;
                            /* to compile -------------------------

                             // cout << "\t\ttdc_trig_time = "
                             // << t.give_word(index_of_first_data+1) << endl ;
                             // target_event-> dgf[nr_cryostat][nr_detector][nr_signal][0]
                             //                                = 6789 ;
                             // t.give_word(index_of_first_data+1) ;

                             // cout << "\t\tadc_energy = "
                             // << t.give_word(index_of_first_data+2) << endl ;
                             // target_event-> dgf[nr_cryostat][nr_detector][nr_signal][1]=
                             // t.give_word(index_of_first_data+2) ;

                             // cout << "\t\tpos of sig beg. = "
                             //  << t.give_word(index_of_first_data+3) << endl ;
                             target_event-> dgf[nr_cryostat][nr_detector][nr_signal][2] =
                             t.give_word(index_of_first_data+3) ;

                             // cout << "\t\tpos of the steepes slope = "
                             //  << t.give_word(index_of_first_data+4) << endl ;
                             target_event->dgf[nr_cryostat][nr_detector][nr_signal][3] =
                             t.give_word(index_of_first_data+4) ;

                             // cout << "\t\tmax height of mirror charge = "
                             // << t.give_word(index_of_first_data+5) << endl ;
                             target_event->dgf[nr_cryostat][nr_detector][nr_signal][4] =
                             t.give_word(index_of_first_data+5) ;

                             // cout << "\t\tpos of mirror charge maximum = "
                             // << t.give_word(index_of_first_data+6) << endl ;
                             target_event->dgf[nr_cryostat][nr_detector][nr_signal][5] =
                             t.give_word(index_of_first_data+6) ;

                             // cout << "\t\tpsa error code= "
                             // << t.give_word(index_of_first_data+7) << endl ;
                             target_event->dgf[nr_cryostat][nr_detector][nr_signal][6] =
                             t.give_word(index_of_first_data+7) ;

                             // cout << "\t\tposition signal end = "
                             // << t.give_word(index_of_first_data+8) << endl ;
                             target_event->dgf[nr_cryostat][nr_detector][nr_signal][7] =
                             t.give_word(index_of_first_data+8) ;

                             -------------- */
                            break;

                        case 2:

                            // Trigger TIME is nonsense, we had to take it from
                            // the first part of this function, dedicated to TDC

                            //cout << "\t\ttdc_trig_time = "
                            // << t.give_word(index_of_first_data+1) << endl ;
                            //    target_event->
                            // dgf[nr_cryostat][nr_detector][nr_signal][0] = 6789 ;
                            // t.give_word(index_of_first_data+1) ;

                            //ENERGY
#if SHOW_DETECTORS

                            cout << "adc_ENERGY " << " from module " << module_nr
                                 << ", chan= " << chann
                                 << ", data = " << (t.give_word(analysed_word + 1) >> 3);

                            cout << " --- This goes to cluster "
                                    //                << clus
                                    //                << ", crystal " << crys
                                    //                << " which is "
                                 << Trising::cluster_characters[clus] << "_"
                                 << char('1' + crys) << endl;
#endif // SHOW_DETECTORS
                            // we  divide by 8 (shifting 3 bits right) because the DGF
                            // electronics has resolution 64 KB
                            target_event->ger_xia_dgf[clus][crys][1] =
                                    t.give_word(analysed_word + 1) >> 3;

#if STEPHANE_ASK_FOR_ORIGINAL_TIME_IN_THE_ROOT_TREE == true
                            // oryginal time - Stephane wants this also (recently
                            // without shifting the bits)
                            target_event->ger_xia_dgf[clus][crys][2] =
                                    t.give_word(analysed_word + 0);  // >> 3;
#endif

                            target_event->ger_xia_dgf_fired[clus][crys] = true;
                            if(dgf_subevent_length > 30) {
                                // cout << "This is more complicated event" << endl;
                            }
                            break;
                        } // endof switch
                    } else {

#if SHOW_DETECTORS
                        cout << "DGF unpacking. Warning: combination module_nr = "
                             << module_nr
                             << ", CHANNEL = " << chann
                             << " was not specified in a lookup table Cluster_energy_xia !"
                             << endl;
#endif // SHOW_DETECTORS

                    }

                    //----------------------------------
                    index_of_first_data += how_many_words_per_event; // alway
                    analysed_word += how_many_words_per_event;
                }
            } // endof for chann

        } // end of case 9, case 2

        } // end of switch (what type of the channel header: 9,2 or 4 words


        if(t.give_word(index_of_beg_of_next_module) == 0)
            break; // next module is empty

        index_of_beg_of_module = index_of_beg_of_next_module;
        index_of_beg_of_next_module = 0;

    } // end for module

#endif // experiment g-factor
    //------------ on the end ------
    dgf_subev_queue.pop();

}
//****************************************************************************
/** unpacking the miniball subevent */
void TIFJEventProcessor::unpack_mib_subevent(const timestamped_subevent & /*t*/,
                                             TIFJEvent * /*te*/)
{
    //  cout << "\nUnpacking the mib subevent " << endl;

#ifdef MINIBALL_PRESENT

    // to have the knowledge when it happened
    target_event->timestamp_MIB = t.give_ts();
    target_event->MIB_time_since_beginning_of_spill =
            target_event->timestamp_MIB - time_stamp_beg_of_last_spill;

#ifdef NEVER
    // show ---------------------------------------------
    unsigned int how_many_data_words = t.length();

    cout << how_many_data_words << " words inside subevent " << endl;

    char *descriptions [100] = {"Nr of dgf modules",
                                "Nr of events in the data buffer",
                                "Synchro ?",
                                "Nr of words in this buffer   <---BUFFER HEADER",
                                "Crate / slot info", "Format descriptor",
                                "Run start time (high)",
                                "Run start time (middle)", "Run start time (low)",
                                "Hit pattern  <--( EVENT HEADER) ---",
                                "Event time (high word)",
                                "Event time (high word)"
                               };

    for(unsigned int i = 0; i < how_many_data_words; i++) {
        cout << "Word nr " << i
             << "\thex=" << hex << setw(8) << t.give_word(i)
             << ", dec = " << dec << setw(10) << t.give_word(i);
        if(i < 90)
            cout << "  " << descriptions[i];
        cout << endl;
    }
#endif

    if(t.length() == 0)
        return; // empty subevent;

    // unpacking------------------------------------------

    // synchro information
    // NOTE: here, instead the "debug" word
    // we placed the synchro time data

    //  cout << "in unpack_mib_subevent, mib synchro data "
    //       << t.give_word(2)
    //       << ", hex = 0x" << hex
    //       << t.give_word(2) << dec << ", truncated " << (t.give_word(2)  & 0x3fff)
    //       << endl ;

    target_event->mib_synchro_data = t.give_word(0) & 0x3fff;

    // at first we unpack the TIMES information ########################

    // loop over unknown nr of words

    vme_word word;
    // memset(&vme[0][0], 0, sizeof(vme) );

    int extra_word = 1; // bec synchor time calibratoro
    bool time_to_finish = false;

    // zero was used, now we start from 1
    for(unsigned int i = 1; !time_to_finish; i++, extra_word++) {
        //    cout << "Unpacking vme word for time information " << i << endl;
        word.raw_word = t.give_word(i);
        if(!word.raw_word) {  // do not analyse empty words
            // TODO complain
            continue;
        }

        switch(word.header_word.code)    // <-- it is common to all of
            // types of vme words
        {

        case 2: {
            //          cout << "Header .." << endl;
        }
            break;

        case 0:
            // sizes are hardcoded in the class definition
            if(word.data_word.geo < 22 && word.data_word.channel < 32) {
                //        cout << "arrived geo = " <<   word.data_word.geo
                //        << ", channel = " << word.data_word.channel
                //        << ", data = " << word.data_word.data
                //
                //        << " This is to " << (word.data_word.channel / 3) +1
                //        <<  char ('A' + (word.data_word.channel %3))
                //        << endl;

                //        vme[word.data_word.geo][word.data_word.channel]
                //          = word.data_word.data;
                target_event->mib[word.data_word.channel / 3][word.data_word.channel % 3][0][0] =
                        word.data_word.data;
            }
            break;

        case 4:
            //      cout << "Footer  when i = .." << i << endl;
            time_to_finish = true;
            break;
        default :
            //    cout << "No time information  when i = .." << i << endl;
            time_to_finish = true;
            break;
        }

    }

    // Sometimes Henning is sending the extra word on the end of the VME block
    // with the pattern 0xbabe****
    int coto = t.give_word(extra_word);
    if((coto & 0xffff0000) == 0xbabe0000) {
        extra_word++;
    }

    //  cout << "extra = " << extra_word << endl;

    //--------------------------Now we come to the energy ########################
    //###############################################################


    int how_many_modules =
            t.give_word(extra_word) & 0xffff;    // only the lower word. Upper can contain some extra information

    // we have maximum 54 modules !!!!!!!!!!!!!!!!!!!!!!!!!
    if(how_many_modules > 54)
        how_many_modules = 54; // !!!!!!!!!!!!!!!!!!!!!!!! CHEATING !!!!!!!!!!!!


    // position of the particular information in the incomming data ------------------------
    // Warning: as I see Henning gave me now the structure which is one word shorter. For the former
    //             dgf data there was an extra word nr [3] <--- how many events in this module.
    //             now it does not exist, so the calculation of indexes in the lines below are changed

    int index_of_beg_of_module = 3 + extra_word;
    //-----------------------------------------------------------

    // see the instructtion for explanations
    int how_many_words_per_event;

    //  cout << "How many modules = " << how_many_modules << endl;

    for(int module = 0; module < how_many_modules; module++) {

        int index_of_beg_of_next_module = index_of_beg_of_module + t.give_word(index_of_beg_of_module);
        int index_of_format_descriptor = index_of_beg_of_module + 2;
        //    int index_of_module_number         = index_of_beg_of_module + 1;

        int index_of_hit_pattern = index_of_beg_of_module + 6;
        int index_of_first_data = index_of_hit_pattern + 3;

        //    cout << "Test "
        //       << "\nindex_of_beg_of_module " <<index_of_beg_of_module
        //       << "\nindex_of_beg_of_next_module " <<index_of_beg_of_next_module
        //       << "\nindex_of_format_descriptor  " << index_of_format_descriptor
        //       << "\nindex_of_hit_pattern " << index_of_hit_pattern
        //       << "\nindex_of_first_data " << index_of_first_data
        //       << "\n how_many_words_per_event= " << how_many_words_per_event
        //       << endl;


        //    cout<< "Module nr " << module << " ===========================\n";


        int nr_of_words_in_this_module = t.give_word(index_of_beg_of_module);
        if(abs(nr_of_words_in_this_module) == 6) {
            //      cout << " this is an empty module (words is = 6)" << endl;
            index_of_beg_of_module += 1; // Henning does not write the Buffer header when it is just 6 ;
            continue;
        }
        if(abs(nr_of_words_in_this_module) > 5000) {
            //      cout << " Nonsense length of the module event " << endl;
            return;
        }

        //  // taking out the events of this module

        switch(t.give_word(index_of_format_descriptor) /*- 0x1000*/) {
        case 256:
        case 257:
        case 512:
        case 513:
        case 256+ 0x1000:
        case 257+ 0x1000:
        case 512+ 0x1000:
        case 513+ 0x1000:
            how_many_words_per_event = 9;
            break;
        case 258:
        case 514:
        case 258+ 0x1000:
        case 514+ 0x1000:
            how_many_words_per_event = 4;
            break;

        case 259:
        case 515:
        case 259+ 0x1000:
        case 515+ 0x1000:
            how_many_words_per_event = 2;
            break;
        default:

            if(module < 10) {  // for Henning error walkaroud
                cout << "Error during unpacking MINIBALL data - unknown value of CHANHEADLEN !!! \n"
                     << ", Format Descriptor is 0x"
                     << hex << t.give_word(index_of_format_descriptor)
                     << ", dec= " << dec << t.give_word(index_of_format_descriptor)
                     << " (at MINIBALL module nr " << module << ")\n";
            }

            // JUST to show for niGEL----------------
            // unsigned int how_many_data_words = t.length();
            //
            // cout << how_many_data_words << " words inside subevent " << endl;
            //
            // char * descriptions [100] = { "Nr of dgf modules", "Nr of events in the data buffer", "Synchro ?",
            //                               "Nr of words in this buffer   <---BUFFER HEADER",
            //                               "Crate / slot info", "Format descriptor",  "Run start time (high)",
            //                               "Run start time (middle)", "Run start time (low)",
            //                               "Hit pattern  <--( EVENT HEADER) ---",
            //                               "Event time (high word)", "Event time (high word)" } ;
            //
            // for(unsigned int i = 0; i < how_many_data_words; i++)
            // {
            //   cout << "Word nr " << i
            //   << "\thex=" << hex << setw(8) << t.give_word(i)
            //   << ", dec = " << dec << setw(10) << t.give_word(i);
            //   if(i < 90) cout  << "  " << descriptions[i] ;
            //   cout << endl;
            // }

            // END OF THE NIGEL SH


            mib_subev_queue.pop();
            return;
        }

        //    cout << "This module has to go to the detector crate/slot ...\n";

        int crate = (t.give_word(index_of_beg_of_module + 1) & 0xff00) >> 8;
        int slot = (t.give_word(index_of_beg_of_module + 1) & 0xff);
        //    cout << " this is crate = " << crate << ", slot = " << slot << endl ;


        if(t.give_word(1 + extra_word) != 1) {
            //      cout << "The nr of events in the data buffer is not ONE ???? \n" ;
        }

        for(unsigned int mib_event = 0; mib_event < t.give_word(1 + extra_word); mib_event++) {     //

            //      cout << "Event header nr = " << mib_event << " -----------------------\n";
            // now we have to take bitpattern
            unsigned int p = /* pattern[module] = */t.give_word(index_of_hit_pattern);
            if(!p)
                break; // empty patern ?

            //switch (what type of the channel header: 9,2 or 4 words
            switch(how_many_words_per_event) {
            default : { // means 4
                cout << "!!! Unrecognized nr of words in the channel cheader CHANHEADLEN = "
                     << how_many_words_per_event << endl;

                mib_subev_queue.pop();
                return;

            } // end of case - default

            case 2: // 2 words per event
            case 9: { // 9 words for the event

                const int max_channels_per_dgf_block = 4; // max channels are in the DGF module ? (according to Henning)
                int nr_cryostat, nr_detector, nr_signal;

                for(int i = 0; i < max_channels_per_dgf_block; i++) {
                    //          cout << "Loop over channels  bit pattern  i = " << i << "  ####\n";
                    if((p >> i) == 0)
                        break; // no more bits in the pattern?

                    int piker = 1 << i;
                    if(p & piker) {
                        //            cout << "   !!!! There is a data for this channel" << i << "  !!!!!!!\n";
                        //            cout << " this is crate = " << crate << ", slot = " << slot << endl ;
                        if(lookup_miniball.current_combination(crate, slot, i, &nr_cryostat, &nr_detector, &nr_signal)) {
                            //              cout << "This will go to the cryostat = " << nr_cryostat
                            //                << ", detector = " << nr_detector
                            //                << ", signal = " << nr_signal << endl;


                            //
                            // int mib[8][3][7][8];    // 8 cryostats, 3 detectors, 7 singals, each 8 items on dgf block
                            // bool mib_fired[8][3];

                            //---------------------
                            // take data for channel i

                            // look below, nr of cryostat given by the lookup table has already -1
                            target_event->mib_fired[nr_cryostat][nr_detector] = true;
                            switch(how_many_words_per_event) {
                            case 9:

                                //            cout << "\t\ttdc_trig_time = " << t.give_word(index_of_first_data+1) << endl ;
                                //              target_event->mib[nr_cryostat][nr_detector][nr_signal][0] = 6789 ; // t.give_word(index_of_first_data+1) ;

                                //            cout << "\t\tadc_energy = " << t.give_word(index_of_first_data+2) << endl ;
                                target_event->mib[nr_cryostat][nr_detector][nr_signal][1] = t.give_word(index_of_first_data + 2);

                                //            cout << "\t\tpos of sig beg. = " << t.give_word(index_of_first_data+3) << endl ;
                                target_event->mib[nr_cryostat][nr_detector][nr_signal][2] = t.give_word(index_of_first_data + 3);

                                //            cout << "\t\tpos of the steepes slope = " << t.give_word(index_of_first_data+4) << endl ;
                                target_event->mib[nr_cryostat][nr_detector][nr_signal][3] = t.give_word(index_of_first_data + 4);

                                //            cout << "\t\tmax height of mirror charge = " << t.give_word(index_of_first_data+5) << endl ;
                                target_event->mib[nr_cryostat][nr_detector][nr_signal][4] = t.give_word(index_of_first_data + 5);

                                //            cout << "\t\tpos of mirror charge maximum = " << t.give_word(index_of_first_data+6) << endl ;
                                target_event->mib[nr_cryostat][nr_detector][nr_signal][5] = t.give_word(index_of_first_data + 6);

                                //            cout << "\t\tpsa error code= " << t.give_word(index_of_first_data+7) << endl ;
                                target_event->mib[nr_cryostat][nr_detector][nr_signal][6] = t.give_word(index_of_first_data + 7);

                                //            cout << "\t\tposition signal end = " << t.give_word(index_of_first_data+8) << endl ;
                                target_event->mib[nr_cryostat][nr_detector][nr_signal][7] = t.give_word(index_of_first_data + 8);

                                break;

                            case 2:
                                //cout << "\t\ttdc_trig_time = " << t.give_word(index_of_first_data+1) << endl ;
                                //    target_event->mib[nr_cryostat][nr_detector][nr_signal][0] = 6789 ; // t.give_word(index_of_first_data+1) ;

                                //            cout << "\t\tadc_energy = " << t.give_word(index_of_first_data+1) << endl ;
                                target_event->mib[nr_cryostat][nr_detector][nr_signal][1] = t.give_word(index_of_first_data + 1);

                                break;
                            } // endof switch


                        } else {
                            // Piotr sends someof his signals through the MIB branch. They will appear here
                            //              cout << "   !!!! There is a data from crate = " << crate
                            //                   << ", slot = " << slot << ", channel = " << i<< endl ;
                            //              cout << "\t\tadc_energy = " << t.give_word(index_of_first_data+2) << endl ;

                            //              for (int m = 0 ;m < 7  ;m++)
                            //              {
                            //                cout << "["<< m << "] = " << t.give_word(index_of_first_data+m) << endl ;
                            //                }
                            //
                            //                cout << "This is the combination unknown to the lookup table !!!\n";

                            if(crate == 3) {  // our GE data come in the crate 3
                                const int first_slot = 21;
                                if(slot == first_slot || slot == (first_slot + 1)) {
                                    int where = ((slot - first_slot) * 4) + i;
                                    if(where < 10) {
                                        target_event->miniball_deliveres_cluster_energy[where] =
                                                (t.give_word(index_of_first_data + 2) >> 3);
                                    }
                                }
                            }// end if crate 3


                            //              else cout << "This is the combination unknown to the lookup table !!!\n";

                        } // end else - combination not from the lookup table of minballs

                        //----------------------------------
                        index_of_first_data += how_many_words_per_event; // alway
                    }
                } // endof for i

            } // end of case 9, case 2

            } // end of switch (what type of the channel header: 9,2 or 4 words

        } // end for event (one event in the buffer - Henning promises this)


        if(t.give_word(index_of_beg_of_next_module) == 0)
            break; // next module is empty

        index_of_beg_of_module = index_of_beg_of_next_module;
        index_of_beg_of_next_module = 0;

    } // end for module

#endif // MINIBALL_PRESENT
    //------------ on the end ------
    mib_subev_queue.pop();

}

//**************************************************************************
/** reading the types of events which are legal
 reading the time gates for time stamp system */

//************************************************************************
#if NIGDY

/** reading the types of events which are legal for 4 branches mode of MBS
 reading the time gates for time stamp system */
//************************************************************************
bool TIFJEventProcessor::read_in_4mbs_parameters(
        string nam_ger_minus_frs_timestamp_diff,
        string nam_frs_minus_hec_timestamp_diff,
        string nam_frs_minus_dgf_timestamp_diff,
        string nam_ger_minus_dgf_timestamp_diff,
        string nam_ger_minus_hec_timestamp_diff,
        string /*nam_dgf_minus_hec_timestamp_diff*/)
{

    string fname = "./mbs_settings/4_branches_legal_subevent_combinations.dat";
    ifstream plik(fname.c_str());

    type_of_matching = 0;
    how_many_branches = 4;

    if(!plik) {
        cout << "file does not exist, so I assume default state" << endl;
        flag_accept_single_frs = true;
        flag_accept_single_hec = true;
        flag_accept_single_ger = true;
        flag_accept_single_dgf = true;
        type_of_matching = 0;
        collect_time_stamp_statistics_spectra = true;
        return true;
    }

    try {
        type_of_matching = (int) FH::find_in_file(plik,
                                                  "type_of_matching");

        match_frs_ger_hec_dgf = FH::find_in_file(plik,
                                                 "match4_FRS_GER_HEC_DGF");

        match_frs_ger_dgf = FH::find_in_file(plik,
                                             "match4_FRS_GER_DGF");
        match_frs_hec_dgf = FH::find_in_file(plik,
                                             "match4_FRS_HEC_DGF");
        match_frs_ger_hec = FH::find_in_file(plik,
                                             "match4_FRS_GER_HEC");

        match_frs_ger = FH::find_in_file(plik, "match4_FRS_GER");
        match_frs_hec = FH::find_in_file(plik, "match4_FRS_HEC");
        match_frs_dgf = FH::find_in_file(plik, "match4_FRS_DGF");

        match_frs_when_single = FH::find_in_file(plik,
                                                 "match4_FRS_when_single");

        // matching to GER ------------

        match_ger_frs_hec_dgf = FH::find_in_file(plik,
                                                 "match4_GER_FRS_HEC_DGF");

        match_ger_frs_hec = FH::find_in_file(plik,
                                             "match4_GER_FRS_HEC");
        match_ger_frs_dgf = FH::find_in_file(plik,
                                             "match4_GER_FRS_DGF");
        match_ger_hec_dgf = FH::find_in_file(plik,
                                             "match4_GER_HEC_DGF");

        match_ger_frs = FH::find_in_file(plik, "match4_GER_FRS");
        match_ger_hec = FH::find_in_file(plik, "match4_GER_HEC");
        match_ger_dgf = FH::find_in_file(plik, "match4_GER_DGF");

        match_ger_when_single = FH::find_in_file(plik,
                                                 "match4_GER_when_single");

        flag_accept_single_frs = FH::find_in_file(plik, "alone4_FRS");
        flag_accept_single_ger = FH::find_in_file(plik, "alone4_GER");
        flag_accept_single_hec = FH::find_in_file(plik, "alone4_HEC");
        flag_accept_single_dgf = FH::find_in_file(plik, "alone4_DGF");

        plik.close();
    } catch(Tno_keyword_exception &m) {
        cout << "error while reading the file " << fname << "\n\t" << m.message
             << endl;
        throw;
    } catch(Treading_value_exception &m) {
        cout << "error while reading the file " << fname << "\n\t" << m.message
             << endl;
        throw;
    }
    plik.close();

    //===============================================================
    switch(type_of_matching) {
    case 1: // to FRS

        need_FRS = true; // surely !
        need_GER = (match_frs_ger_hec_dgf || match_frs_ger_hec
                    || match_frs_ger_dgf || match_frs_ger);
        need_HEC = (match_frs_ger_hec_dgf || match_frs_hec_dgf
                    || match_frs_ger_hec || match_frs_hec);
        need_DGF = (match_frs_ger_hec_dgf || match_frs_ger_dgf
                    || match_frs_hec_dgf || match_frs_dgf);
        break;

    case 2: // to GER
        need_GER = true;
        need_FRS = match_ger_frs_hec_dgf || match_ger_frs_dgf || match_ger_frs
                || match_ger_frs_hec;
        need_HEC = (match_ger_frs_hec_dgf || match_ger_hec_dgf
                    || match_ger_frs_hec || match_ger_hec);
        need_DGF = (match_ger_frs_hec_dgf || match_ger_frs_dgf
                    || match_ger_hec_dgf || match_ger_dgf);

        break;
    case 0:
        need_FRS = false;
        need_HEC = false;
        need_GER = false;
        need_DGF = false;
        break;

    }

    // from the another  file ===========================================
    fname = "./mbs_settings/collect_timestamp_diff_spectra.dat";
    plik.open(fname.c_str());
    if(!plik) {
        cout << "file does not exist, so I assume default state" << endl;
        collect_time_stamp_statistics_spectra = true;
    } else {
        try {
            collect_time_stamp_statistics_spectra = FH::find_in_file(
                        plik, "collect_time_stamp_statistics_spectra");
        } catch(Tfile_helper_exception &m) {
            cout << "Error while reading the file " << fname << "\n\t"
                 << m.message << endl;
            throw;
        }
    }

    // Now we read the gates for the diff spectra ==========================

    //----------------------
    int ger_minus_frs_low;
    int ger_minus_frs_high;

    if(!read_the_gates_for_diff_spectrum(nam_ger_minus_frs_timestamp_diff,
                                         &ger_minus_frs_low, &ger_minus_frs_high)) {
        // some warning about defaults;
    }

    //-----------------
    int frs_minus_hec_low;
    int frs_minus_hec_high;

    if(!read_the_gates_for_diff_spectrum(nam_frs_minus_hec_timestamp_diff,
                                         &frs_minus_hec_low, &frs_minus_hec_high)) {
        // some warning about defaults;
    }

    //-----------------
    int frs_minus_dgf_low;
    int frs_minus_dgf_high;

    if(!read_the_gates_for_diff_spectrum(nam_frs_minus_dgf_timestamp_diff,
                                         &frs_minus_dgf_low, &frs_minus_dgf_high)) {
        // some warning about defaults;
    }

    //--------------------
    int ger_minus_dgf_low;
    int ger_minus_dgf_high;

    if(!read_the_gates_for_diff_spectrum(nam_ger_minus_dgf_timestamp_diff,
                                         &ger_minus_dgf_low, &ger_minus_dgf_high)) {
        // some warning about defaults;
    }

    //--------------------
    int ger_minus_hec_low;
    int ger_minus_hec_high;

    if(!read_the_gates_for_diff_spectrum(nam_ger_minus_hec_timestamp_diff,
                                         &ger_minus_hec_low, &ger_minus_hec_high)) {
        // some warning about defaults;
    }

    // Note: there is no gates on the spectrum dgf_minus_hec


    // finally I need============================

    //------------------------------ for matching to FRS ---------------
    down_ger_minus_frs_gate = ger_minus_frs_low;
    up_ger_minus_frs_gate = ger_minus_frs_high;

    down_hec_minus_frs_gate = minimal(-frs_minus_hec_low, -frs_minus_hec_high);
    up_hec_minus_frs_gate = maximal(-frs_minus_hec_low, -frs_minus_hec_high);

    down_dgf_minus_frs_gate = minimal(-frs_minus_dgf_low, -frs_minus_dgf_high);
    up_dgf_minus_frs_gate = maximal(-frs_minus_dgf_low, -frs_minus_dgf_high);

    //------------------------------ for matching to GER ---------------

    down_frs_minus_ger_gate = minimal(-ger_minus_frs_low, -ger_minus_frs_high);
    up_frs_minus_ger_gate = maximal(-ger_minus_frs_low, -ger_minus_frs_high);

    down_hec_minus_ger_gate = minimal(-ger_minus_hec_low, -ger_minus_hec_high);
    up_hec_minus_ger_gate = maximal(-ger_minus_hec_low, -ger_minus_hec_high);

    down_dgf_minus_ger_gate = minimal(-ger_minus_dgf_low, -ger_minus_dgf_high);
    up_dgf_minus_ger_gate = maximal(-ger_minus_dgf_low, -ger_minus_dgf_high);

    return true;

}
#endif  // NIGDY
//*************************************************************************
void TIFJEventProcessor::postLoop()
{
    output_statistics_events();
#ifdef NIGDY
    if(collect_time_stamp_statistics_spectra) {
        queue_full_make_timestamp_spectra();

        spec_ger_minus_frs_timestamp_diff->save_to_disk();
        spec_frs_minus_hec_timestamp_diff->save_to_disk();
        spec_frs_minus_dgf_timestamp_diff->save_to_disk();
        spec_ger_minus_dgf_timestamp_diff ->save_to_disk();
        spec_ger_minus_hec_timestamp_diff->save_to_disk();
        spec_dgf_minus_hec_timestamp_diff->save_to_disk();
        spec_frs_minus_mib_timestamp_diff->save_to_disk();
        spec_ger_minus_mib_timestamp_diff ->save_to_disk();

    }
#endif // NIGDY
}
//**************************************************************************
/** Function for unpacking the FRS "status readout" */
void TIFJEventProcessor::trigger_14_block()
{
#ifdef NEVER
    cout << "This is trigger_14 status readout block " << endl;

    //-----------------------------------


#ifdef NEVER

    int ile = (input_subevent -> GetDlen() - 1) / 2;

    for(int i = 0; i < ile; i++) {
        cout << dec
             << i << " as old_long word = "
             << long_data[i]
                << "  , 0x"
                << hex << long_data[i]
                   << "  , old_long truncated = "
                   << dec << (long_data[i] & 0x3ffffff)

                   << dec
                   << endl;

    }
#endif // NEVER
    unsigned int length_of_real_data =
            ((input_subevent -> GetDlen() - 1) / 2) - 4;
    // 4 is because we skip type of the block (1) and time stamp (3)


    trigger_14_word word;

    //vme.clear();
    //cout << "This =" << this << endl;
    //   memset(&vme[0][0], 0, sizeof(vme) );

    daq_word_t *long_data =
            (daq_word_t *) input_subevent -> GetDataField();
    daq_word_t *ptr = &long_data[4];

    string fname = "spectra/last_trigger_14_block.txt";
    ofstream plik(fname.c_str());
    if(!plik) {
        cout << "Error while opening the file " << fname << endl;
        return;
    }

    for(unsigned int i = 0; i < length_of_real_data; i++, ptr++) {
        //      cout << "frs word nr " << i << " is "
        //           << hex << t.give_word(i) << dec << "\t\t";

        word.raw_word = *ptr;
        if(!word.raw_word) {  // if the word was empty;
            //cout << endl;
            continue;
        }

        switch(word.header_word.code)    // <-- it is common to all of types
            // of vme words
        {

        case 3: // this is custom value of the header
            plik << "Header "
                 << " Geo  = " << word.header_word.geo
                 << " forCATE  = " << word.header_word.forCATE
                 << ", full scale range = " << word.header_word.full_scale_range
                 << ", cnt = " << word.header_word.cnt
                 << endl;

            //i += word.header_word.cnt;

            break;

        case 0: // this is a data word
            plik << "  Data Word for "
                 << " Geo = " << word.data_word.geo
                 << ", chan= " << word.data_word.channel
                 << ", kill  = " << word.data_word.kill
                 << ", threshold = " << word.data_word.threshold
                 << endl;

            // sizes are hardcoded in the class definition
            if(word.data_word.geo < 22 && word.data_word.geo < 32) {
                //vme[word.data_word.geo][word.data_word.channel] =
                // word.data_word.data;
            }
            break;

        case 4:
            plik << "End of block "
                 << ", Geo= "
                 << word.end_of_block_word.geo
                 << ", Status register = "
                 << word.end_of_block_word.status_register
                 << ", 0x"
                 << hex << word.end_of_block_word.status_register
                 << dec
                 << endl;
            break;

        default :
            plik << "  unrecognized type of trigger 14 status word "
                 << endl;
            break;
        }

    }
    if(!plik) {
        cout << "Error while writing to the file " << fname << endl;
    }
    plik.close();

#endif

}
//************************************************************************
/** every event has its own number (stored inside the event structure)
 To estimate the ratio of analysed event the TAnalysis class wants to
 know this number quite often */
int *TIFJEventProcessor::give_address_of_mbs_event_number()
{
    return &current_ev_nr;
}

//********************************************************************
/** making the report of the timestamped subevent matching
 when the matching is made to the GER subevent */
void TIFJEventProcessor::make_report_ger_style()
{

    return; // because it is only for my debugging purposes

    //vector<time_stamp_value >::iterator it_ger, it_frs, it_hec;

    cout << "Making raport: time_matching_to_ger.raport" << endl;
    ofstream raport("./mbs_settings/time_matching_to_ger.raport");
    raport << "entries to   GER vector " << ger_time_stamps.size()
           << "\nentries to   FRS vector " << frs_time_stamps.size()
           << "\nentries to   HEC vector " << hec_time_stamps.size()
           << "\nentries to   DGF vector " << dgf_time_stamps.size()

           << endl;

    raport << "\n!!! Note: matching possibilites are shown "
              "after this long list below!!!\n" << endl;

    //#ifdef NEVER


    raport
            << "------- GER ------------------- FRS --------------------HEC------------------- DGF--------------"
            << endl;

    // showing the contents on the screen --------------------------------

    //  it_ger = ger_time_stamps.begin();
    //  it_frs = frs_time_stamps.begin();
    //  it_hec = hec_time_stamps.begin();

    for(unsigned int i = 0; i < ger_time_stamps.size(); i++) {
        raport << "[" << i << "] " << ger_time_stamps[i] << "    ";

        if(i < frs_time_stamps.size())
            raport << "  " << frs_time_stamps[i];
        else
            raport << " ------------";
        raport << "    ";

        if(i < hec_time_stamps.size())
            raport << "  " << hec_time_stamps[i];
        else
            raport << " ------------";

        if(i < dgf_time_stamps.size())
            raport << "  " << dgf_time_stamps[i];
        else
            raport << " ------------";

        raport << "\n";

    }
    raport << flush;

    //#endif

    raport << " trying to search suitable subevents\n";

    //list<time_stamp_value>::iterator it, it_prev, it_next;

    int lower_point_frs = -2000;
    int upper_point_frs = 2000;

    int lower_point_hec = -2000;
    int upper_point_hec = 2000;

    int lower_point_dgf = -2000;
    int upper_point_dgf = 2000;

    //  it_ger = ger_time_stamps.begin();
    //  it_frs = frs_time_stamps.begin();
    //  it_hec = hec_time_stamps.begin();

    // below is /4 because too long listing....

    // ------------------------------------ GER loop ---
    for(unsigned int i = 0; i < minimal(1000u, (ger_time_stamps.size())); i++) {

        raport << "\nGER[" << i << "] " << ger_time_stamps[i]
                  << "---------------------------------"
                  << ((ger_time_stamps[i].has_1Hz_flag()) ? " !!! 1Hz !!!" : " ");

        //raport << " finding a match in FRS ---------------------\n";

        //--------------------------------------------- FRS loop
        for(unsigned int f = 0; f < frs_time_stamps.size(); f++) {
            //if(it->has_1Hz_flag() ) continue;

            long long difference = frs_time_stamps[f] - ger_time_stamps[i];
            //if(abs(difference) < 20000) raport << " with FRS time_stamp["
            // << m << "]  "  << *it
            //             << "  difference is " << difference << "\n";

            if(difference >= lower_point_frs && difference <= upper_point_frs) {
                //              it_prev = it;
                //              it_prev--;
                //
                //              it_next = it;
                //              it_next++;

                raport
                        //<< "!!! possibly matching"
                        << "\n\t  FRS[" << f << "] " << frs_time_stamps[f]
                           // << " !!! "
                        << " (diff=" << difference << ") (prev: "
                        << ((f != 0) ? (frs_time_stamps[f - 1]
                                       - ger_time_stamps[i]) : 0) << ", next: "
                                                                  << (frs_time_stamps[f + 1] - ger_time_stamps[i])
                        << ") "
                        << ((frs_time_stamps[f].has_1Hz_flag()) ? "!!! 1Hz!!! "
                                                                : "")
                           // << "\n"
                           ;
            }
        }

        //raport  << "now searching match in HEC\n";


        // finding the match for HEC -----------------------------------------


        for(unsigned int a = 0; a < hec_time_stamps.size(); a++) {
            if(hec_time_stamps[a].has_1Hz_flag())
                continue;

            long long int difference = hec_time_stamps[a] - ger_time_stamps[i];
            //  if(abs(difference) < 40000) raport << " with HEC time_stamp["
            // << m << "]  "  << *it
            //      << "  difference is " << difference << "\n";


            if(difference >= lower_point_hec && difference <= upper_point_hec) {
                //              it_prev = it;
                //              it_prev--;
                //
                //              it_next = it;
                //              it_next++;

                raport
                        //<< "\n!!!!!! possibly matching"
                        << "\n\t\t  HEC[" << a << "] " << hec_time_stamps[a]
                           //<< " !!! "
                        << " (diff =" << difference << ") (prev: "
                        << ((a != 0) ? (hec_time_stamps[a - 1]
                                       - ger_time_stamps[i]) : 0) << ", next: "
                                                                  << (hec_time_stamps[a + 1] - ger_time_stamps[i]) << ")"
                                                                                                                   << ((hec_time_stamps[a].has_1Hz_flag()) ? "!!! 1Hz !!! "
                                                                                                                                                           : "")
                                                                                                                      //<< "\n"
                                                                                                                      ;
            }
        }

        //raport  << "now searching match in DGF\n";


        // finding the match for DGF -----------------------------------------


        for(unsigned int a = 0; a < dgf_time_stamps.size(); a++) {
            if(dgf_time_stamps[a].has_1Hz_flag())
                continue;

            long long int difference = dgf_time_stamps[a] - ger_time_stamps[i];
            //  if(abs(difference) < 40000) raport << " with DGF time_stamp["
            // << m << "]  "  << *it
            //      << "  difference is " << difference << "\n";


            if(difference >= lower_point_dgf && difference <= upper_point_dgf) {
                //              it_prev = it;
                //              it_prev--;
                //
                //              it_next = it;
                //              it_next++;

                raport
                        //<< "\n!!!!!! possibly matching"
                        << "\n\t\t  DGF[" << a << "] " << dgf_time_stamps[a]
                           //<< " !!! "
                        << " (diff =" << difference << ") (prev: "
                        << ((a != 0) ? (dgf_time_stamps[a - 1]
                                       - ger_time_stamps[i]) : 0) << ", next: "
                                                                  << (dgf_time_stamps[a + 1] - ger_time_stamps[i]) << ")"
                                                                                                                   << ((dgf_time_stamps[a].has_1Hz_flag()) ? "!!! 1Hz !!! "
                                                                                                                                                           : "")
                                                                                                                      //<< "\n"
                                                                                                                      ;
            }
        }

        raport << "\n";
    }

    raport << flush;
    raport.close();
    cout << "Finished raport: time_matching_to_ger.raport" << endl;

}
//********************************************************************
/** making the report of the timestamped subevent matching
 when the matching is made to the FRS subevent */
void TIFJEventProcessor::make_report_frs_style()
{

    return; // because we need it for debugging time

    cout << "Making raport: time_matching_to_frs.raport" << endl;

    vector<time_stamp_value>::iterator it_ger, it_frs, it_hec, it_dgf;

    //------------------------
    ofstream raport("./mbs_settings/time_matching_to_frs.raport");
    raport << "entries to   FRS vector " << frs_time_stamps.size()
           << "\nentries to   GER vector " << ger_time_stamps.size()
           << "\nentries to   HEC vector " << hec_time_stamps.size()
           << "\nentries to   DGF vector " << dgf_time_stamps.size() << endl;

    raport << "\n!!! Note: matching possibilites are shown "
              "after this long list below!!!\n" << endl;

    //raport  <<  " what really is there..." << endl;
    //  for(int i = 0 ; i < dgf_time_stamps.size() ; i++)
    //  {
    //    raport << "["<< i << "] "
    //           <<  dgf_time_stamps[i]
    //           << "    ";
    //}
    //raport << endl;


    //#ifdef NEVER
    raport
            << "------- FRS --------------- GER -------------------- HEC ------------------- DGF -----------------"
            << endl;

    // showing the contents on the screen --------------------------------

    it_frs = frs_time_stamps.begin();
    it_ger = ger_time_stamps.begin();
    it_hec = hec_time_stamps.begin();
    it_dgf = dgf_time_stamps.begin();

    for(unsigned int i = 0; it_frs != frs_time_stamps.end(); ++it_frs, i++) {
        raport << "[" << i << "] " << *it_frs << "    ";

        if(i < ger_time_stamps.size())
            raport << "  " << *it_ger;
        else
            raport << " ------------";

        raport << "    ";
        if(i < hec_time_stamps.size())
            raport << "  " << *it_hec;
        else
            raport << " ------------";

        raport << "    ";
        if(i < dgf_time_stamps.size())
            raport << "    " << *it_dgf;
        else
            raport << " ------------";

        raport << "\n";

        // we increment the pointers only if queues are not enpty
        if(it_ger != ger_time_stamps.end())
            it_ger++;
        if(it_hec != hec_time_stamps.end())
            it_hec++;
        if(it_dgf != dgf_time_stamps.end())
            it_dgf++;

    }
    raport << flush;
    //#endif


    raport << " trying to search suitable subevents\n";
    vector<time_stamp_value>::iterator it, it_prev, it_next;
    int lower_point_ger = -2000;
    int upper_point_ger = 2000;

    int lower_point_hec = -2000;
    int upper_point_hec = 2000;

    int lower_point_dgf = -2000;
    int upper_point_dgf = 2000;

    it_frs = frs_time_stamps.begin();
    it_ger = ger_time_stamps.begin();
    it_hec = hec_time_stamps.begin();
    it_dgf = dgf_time_stamps.begin();

    for(unsigned int i = 0; i < minimal(1000u, frs_time_stamps.size()); i++, it_frs++) {

        raport << "\nFRS[" << i << "] " << *it_frs
               << "---------------------------------"
               << ((it_frs->has_1Hz_flag()) ? " !!! 1Hz !!!" : " ");

        //raport << " finding a match in GER ---------------------\n";
        int m = 0;
        for(it = ger_time_stamps.begin(); it != ger_time_stamps.end(); it++, m++) {
            int difference = *it - *it_frs;
            //if(abs(difference) < 20000) raport << " with GER time_stamp["
            // << m << "]  "  << *it
            //             << "  difference is " << difference << "\n";

            if(difference >= lower_point_ger && difference <= upper_point_ger) {
                it_prev = it;
                it_prev--;

                it_next = it;
                it_next++;

                raport
                        //<< "!!! possibly matching"
                        << "\n\t  GER[" << m << "] " << *it
                           // << " !!! "
                        << " (diff=" << difference << ") (prev: "
                        << ((m != 0) ? (*(it_prev) - *it_frs) : 0)
                        << ", next: " << (*(it_next) - *it_frs) << ") "
                        << ((it->has_1Hz_flag()) ? " !!! 1Hz !!!" : " ")
                           // << "\n"
                           ;
            }
        }

        //raport  << "now searching match in HEC\n";


        // finding the match for HEC -----------------------------------------

        m = 0;
        for(it = hec_time_stamps.begin(); it != hec_time_stamps.end(); it++, m++) {
            int difference = *it - *it_frs;
            //  if(abs(difference) < 40000) raport << " with HEC time_stamp["
            // << m << "]  "  << *it
            //             << "  difference is " << difference << "\n";

            if(difference >= lower_point_hec && difference <= upper_point_hec) {
                it_prev = it;
                it_prev--;

                it_next = it;
                it_next++;

                raport
                        //<< "\n!!!!!! possibly matching"
                        << "\n\t\t  HEC[" << m << "] " << *it
                           //<< " !!! "
                        << " (diff =" << difference << ") (prev: "
                        << ((m != 0) ? (*it_prev - *it_frs) : 0) << ", next: "
                        << (*it_next - *it_frs) << ")"
                           //<< "\n"
                        << ((it->has_1Hz_flag()) ? " !!! 1Hz !!!" : " ")

                           ;
            }
        }

        //raport  << "now searching match in DGF\n";


        // finding the match for DGF -----------------------------------------

        m = 0;
        for(it = dgf_time_stamps.begin(); it != dgf_time_stamps.end(); it++, m++) {
            int difference = *it - *it_frs;
            //  if(abs(difference) < 40000) raport << " with DGFl time_stamp["
            // << m << "]  "  << *it
            //             << "  difference is " << difference << "\n";

            if(difference >= lower_point_dgf && difference <= upper_point_dgf) {
                it_prev = it;
                it_prev--;

                it_next = it;
                it_next++;

                raport
                        //<< "\n!!!!!! possibly matching"
                        << "\n\t\t  DGF[" << m << "] " << *it
                           //<< " !!! "
                        << " (diff =" << difference << ") (prev: "
                        << ((m != 0) ? (*it_prev - *it_frs) : 0) << ", next: "
                        << (*it_next - *it_frs) << ")"
                           //<< "\n"
                        << ((it->has_1Hz_flag()) ? " !!! 1Hz !!!" : " ")

                           ;
            }
        }

        raport << "\n";
    }

    // now make the statistics how much was not found !!!! (and which were alone)


    raport << "------------------" << endl;

    //  long long diff;
    //    for(unsigned f = 0; f < frs_val_que.size(); f++)
    //    for(unsigned a = 0; a < hec_val_que.size(); a++)
    //    {
    //
    //      diff = hec_val_que[a].first - frs_val_que[f].first;
    //
    //      raport << "frs timestamp = " << frs_val_que[f].first
    //        << " - value " << frs_val_que[f].second
    //        << "    hec timestamp = " << hec_val_que[a].first
    //        << " - value " << hec_val_que[a].second
    //        << "  TS difference " << diff
    //        << "  values a/b = "
    //        <<  ((hec_val_que[a].second) ?
    //                (frs_val_que[f].second / hec_val_que[f].second)
    //                :
    //                -999)
    //        << endl;
    //
    //       spec_synchro_xy->manually_increment(frs_val_que[f].second, hec_val_que[f].second);
    //
    //    }
    //
    //    frs_val_que.clear();
    //    hec_val_que.clear();

    raport << flush;
    raport.close();
    cout << "Finished raport:  time_matching_to_frs.raport" << endl;

}
//************************************************************************

#if 0  // NIGDY

/** When one of the 3 timestamps que is full, we make report and
 clean them all */
void TIFJEventProcessor::queue_full_make_timestamp_spectra()
{

    if(collect_time_stamp_statistics_spectra) {

        vector<time_stamp_value>::iterator it_ger, it_frs, it_hec, it_dgf,
                it_mib;

        //    cout << "updating the 'Time stamp distribution' spectra of "
        // "the incomming MBS subevents..."
        //          << "Warning: it is time consuming, so if you do not need "
        //             "it anymore - switch off this option "
        //          << endl;


        //#ifdef WE_WANT_REPORT
        if(type_of_matching == 1) {
            // matching to FRS
            make_report_frs_style();
        } else if(type_of_matching == 2) {
            make_report_ger_style();
        }

        //--------------- chwilowo -----------
        else {
            make_report_frs_style();
        }

        //#endif


        for(it_hec = hec_time_stamps.begin(); it_hec != hec_time_stamps.end(); ++it_hec) {
            // cout << "In raport Size of hec timestemp queue =" << hec_time_stamps.size() << endl;
            //--------------
            // GER - HEC

            for(it_ger = ger_time_stamps.begin(); it_ger
                != ger_time_stamps.end(); ++it_ger) {

                // non 1Hz
                //if(!it_ger->has_1Hz_flag() && !it_dgf->has_1Hz_flag() )
                {
                    spec_ger_minus_hec_timestamp_diff->manually_increment(
                                (int)(*it_ger - *it_hec));
                }
            }

            for(it_frs = frs_time_stamps.begin(); it_frs
                != frs_time_stamps.end(); ++it_frs) {
                // NON 1Hz trigger spectr
                //if(!it_frs->has_1Hz_flag() && !it_hec->has_1Hz_flag() )
                {
                    spec_frs_minus_hec_timestamp_diff->manually_increment(
                                (int)(*it_frs - *it_hec));
                }

            }

            // DGF - HEC spectrum
            for(it_dgf = dgf_time_stamps.begin(); it_dgf
                != dgf_time_stamps.end(); ++it_dgf) {
                // NON 1Hz trigger spectr
                //if(!it_dgf->has_1Hz_flag() && !it_dgf->has_1Hz_flag() )
                {
                    spec_dgf_minus_hec_timestamp_diff->manually_increment(
                                (int)(*it_dgf - *it_hec));
                }

            }
        }

        for(it_ger = ger_time_stamps.begin(); it_ger != ger_time_stamps.end(); ++it_ger) {

            for(it_frs = frs_time_stamps.begin(); it_frs
                != frs_time_stamps.end(); ++it_frs) {

                // non 1Hz
                //if(!it_frs->has_1Hz_flag() && !it_ger->has_1Hz_flag() )
                {
                    spec_ger_minus_frs_timestamp_diff->manually_increment(
                                (int)(*it_ger - *it_frs));
                }

            }
        }

        // DGF spectra =========================================

        for(it_dgf = dgf_time_stamps.begin(); it_dgf != dgf_time_stamps.end(); ++it_dgf) {

            for(it_frs = frs_time_stamps.begin(); it_frs
                != frs_time_stamps.end(); ++it_frs) {

                // non 1Hz
                //if(!it_frs->has_1Hz_flag() && !it_dgf->has_1Hz_flag() )
                {
                    spec_frs_minus_dgf_timestamp_diff->manually_increment(
                                (int)(*it_frs - *it_dgf));
                }

            }

            // GER - DGF

            for(it_ger = ger_time_stamps.begin(); it_ger
                != ger_time_stamps.end(); ++it_ger) {

                // non 1Hz
                //if(!it_ger->has_1Hz_flag() && !it_dgf->has_1Hz_flag() )
                {
                    spec_ger_minus_dgf_timestamp_diff->manually_increment(
                                (int)(*it_ger - *it_dgf));
                }

            }

        }

        // MIB spectra ==========================================

        for(it_mib = mib_time_stamps.begin(); it_mib != mib_time_stamps.end(); ++it_mib) {

            // FRS - MIB
            for(it_frs = frs_time_stamps.begin(); it_frs
                != frs_time_stamps.end(); ++it_frs) {

                // non 1Hz
                //if(!it_frs->has_1Hz_flag() && !it_mib->has_1Hz_flag() )
                {
                    spec_frs_minus_mib_timestamp_diff->manually_increment(
                                (int)(*it_frs - *it_mib));
                }

            }

            // GER - MIB

            for(it_ger = ger_time_stamps.begin(); it_ger
                != ger_time_stamps.end(); ++it_ger) {

                // non 1Hz
                //if(!it_ger->has_1Hz_flag() && !it_mib->has_1Hz_flag() )
                {
                    spec_ger_minus_mib_timestamp_diff->manually_increment(
                                (int)(*it_ger - *it_mib));
                }

            }

        }

        //    cout << "finished with 'Time stamp distribution spectra of MBS'"
        // << endl;
    } else {
        //cout << "Time stamp statistics - skipped" << endl;
    }

    hec_time_stamps.clear();
    ger_time_stamps.clear();
    frs_time_stamps.clear();
    dgf_time_stamps.clear();
    mib_time_stamps.clear();
}

//********************* preloop function ***********************************
void TIFJEventProcessor::remember_pointers_to_timestamped_spectra(
        spectrum_1D *s1, spectrum_1D *s2, spectrum_1D *s3, spectrum_1D *s4,
        spectrum_1D *s5, spectrum_1D *s6, spectrum_1D *s7, spectrum_1D *s8)
{
    spec_ger_minus_frs_timestamp_diff = s1;
    spec_frs_minus_hec_timestamp_diff = s2;
    spec_frs_minus_dgf_timestamp_diff = s3;
    spec_ger_minus_dgf_timestamp_diff = s4;
    spec_ger_minus_hec_timestamp_diff = s5;
    spec_dgf_minus_hec_timestamp_diff = s6;

    spec_frs_minus_mib_timestamp_diff = s7;
    spec_ger_minus_mib_timestamp_diff = s8;

}


#endif // NIGDY


//***************************************************************************
/** To have a chance to load the lookup table */
void TIFJEventProcessor::preLoop()
{
    // this funcion is a kind of preloop, so here
#ifdef RISING_GERMANIUMS_PRESENT
    lookup.read_from_disk();
#endif

#ifdef HECTOR_PRESENT
    Thector::read_lookup_info_from_disk(&HECTOR_ADC_GEO,
                                        &HECTOR_TDC_GEO,
                                        &PHOSWITCH_ADC_GEO,
                                        &PHOSWITCH_TDC_GEO);
#endif

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

    lookup_xia_times.read_from_disk("./mbs_settings/lookup_table.txt");
#endif

#ifdef  XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT

    lookup_xia_energies.read_from_disk("./mbs_settings/lookup_table.txt");
#endif

#if ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||         (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))

    lookup_LR_times.read_from_disk("./mbs_settings/lookup_table.txt");
    lookup_SR_times.read_from_disk("./mbs_settings/lookup_table.txt");
    read_ger_plastic_time_lookup_data();
#endif

    //# # # # # # # # # # # # # # # # # # # # # #########################################################
#ifdef ACTIVE_STOPPER_PRESENT

    // as there can be different nr of strinps for X and different for Y
    // here we make a trick. At first we read the lookup table for X only,
    // with the certain number of X stripes.

    // Later we do it once more, but skipping already read X items,
    // now with Y giving the right (different) nr of Y stripes

    vector<string> chain_one;
    for(unsigned int i = 0; i < HOW_MANY_STOPPER_MODULES; i++) {  // <-- so many of them

        chain_one.push_back("stopper_" + Tactive_stopper_full::give_symbol_nr(i) + "_");
        //chain_one.push_back("stopper_M_") ;
        //chain_one.push_back("stopper_R_") ;
    }

    vector<string> chain_two;
    chain_two.push_back("x_");    // only X strips <---

    vector<string> chain_three;

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

    vector<string> chain_three_time;
#endif

    for(unsigned int i = 0; i < NR_OF_STOPPER_STRIPES_X; i++) {  // <-- so many of them
        ostringstream s;
        s << setw(2) << setfill('0') << i;
        chain_three.push_back(s.str() + "_energy");

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        chain_three_time.push_back(s.str() + "_time");
#endif

    }

    //   cout << "Trying to read  " << "./mbs_settings/stopper_lookup_table.txt" << endl;

    lookup_active_stopper.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                         chain_one, chain_two, chain_three, false);

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

    lookup_active_stopper_time.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                              chain_one, chain_two, chain_three_time, false);
#endif

    // now we will do this for Y stripes, skipping the X stripes, because the nr of y stripes
    // can be different

    chain_two.clear();
    chain_two.push_back("x_skip_this_keyword");    // to skip over already read values
    chain_two.push_back("y_");

    chain_three.clear();
#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

    chain_three_time.clear();
#endif

    for(unsigned int i = 0; i < NR_OF_STOPPER_STRIPES_Y; i++) {
        ostringstream s;
        s << setw(2) << setfill('0') << i;

        chain_three.push_back(s.str() + "_energy");

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        chain_three_time.push_back(s.str() + "_time");
#endif

    }

    lookup_active_stopper.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                         chain_one, chain_two, chain_three, true);  // <-- true means: continuation

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

    lookup_active_stopper_time.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                              chain_one, chain_two, chain_three_time, true);  // <-- true means: continuation
#endif

#endif
    //## # # # # # # # # # # # # # # ##############################################################
#ifdef ACTIVE_STOPPER2_PRESENT

    {
        // local
        // as there can be different nr of strinps for X and different for Y
        // here we make a trick. At first we read the lookup table for X only,
        // with the certain number of X stripes.

        // Later we do it once more, but skipping already read X items,
        // now with Y giving the right (different) nr of Y stripes

        vector<string> chain_one;
        /*    chain_one.push_back("stopper2_L_");
         chain_one.push_back("stopper2_M_") ;
         chain_one.push_back("stopper2_R_") ;*/
        for(unsigned int i = 0; i < HOW_MANY_STOPPER_MODULES; i++) {  // <-- so many of them
            chain_one.push_back("stopper2_" + Tactive_stopper_full::give_symbol_nr(i) + "_");
            //chain_one.push_back("stopper_M_") ;
            //chain_one.push_back("stopper_R_") ;
        }

        vector<string> chain_two;
        chain_two.push_back("x_");    // only X strips <---

        vector<string> chain_three;
#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        vector<string> chain_three_time;
#endif

        for(unsigned int i = 0; i < NR_OF_STOPPER_STRIPES_X; i++) {  // <-- so many of them
            ostringstream s;
            s << setw(2) << setfill('0') << i;
            chain_three.push_back(s.str() + "_energy");

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

            chain_three_time.push_back(s.str() + "_time");
#endif

        }

        //     cout << "Trying to read  " << "./mbs_settings/stopper_lookup_table.txt" << endl;

        lookup_active_stopper2.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                              chain_one, chain_two, chain_three, false);
#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        lookup_active_stopper2_time.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                                   chain_one, chain_two, chain_three_time, false);
#endif

        // now we will do this for Y stripes, skipping the X stripes, because the nr of y stripes
        // can be different

        chain_two.clear();
        chain_two.push_back("x_skip_this_keyword");    // to skip over already read values
        chain_two.push_back("y_");
        chain_three.clear();
#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        chain_three_time.clear();
#endif

        for(unsigned int i = 0; i < NR_OF_STOPPER_STRIPES_Y; i++) {
            ostringstream s;
            s << setw(2) << setfill('0') << i;
            chain_three.push_back(s.str() + "_energy");

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

            chain_three_time.push_back(s.str() + "_time");
#endif

        }
        lookup_active_stopper2.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                              chain_one, chain_two, chain_three, true);  // <-- true means: continuation

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        lookup_active_stopper2_time.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                                   chain_one, chain_two, chain_three_time, true);
#endif

    } //local
#endif

    //-------------------
#ifdef ACTIVE_STOPPER3_PRESENT

    {
        // local
        // as there can be different nr of strinps for X and different for Y
        // here we make a trick. At first we read the lookup table for X only,
        // with the certain number of X stripes.

        // Later we do it once more, but skipping already read X items,
        // now with Y giving the right (different) nr of Y stripes

        vector<string> chain_one;
        /*    chain_one.push_back("stopper2_L_");
         chain_one.push_back("stopper2_M_") ;
         chain_one.push_back("stopper2_R_") ;*/
        for(unsigned int i = 0; i < HOW_MANY_STOPPER_MODULES; i++) {  // <-- so many of them
            chain_one.push_back("stopper3_" + Tactive_stopper_full::give_symbol_nr(i) + "_");
            //chain_one.push_back("stopper_M_") ;
            //chain_one.push_back("stopper_R_") ;
        }

        vector<string> chain_two;
        chain_two.push_back("x_");    // only X strips <---

        vector<string> chain_three;
#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        vector<string> chain_three_time;
#endif

        for(unsigned int i = 0; i < NR_OF_STOPPER_STRIPES_X; i++) {  // <-- so many of them
            ostringstream s;
            s << setw(2) << setfill('0') << i;
            chain_three.push_back(s.str() + "_energy");

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

            chain_three_time.push_back(s.str() + "_time");
#endif

        }

        //     cout << "Trying to read  " << "./mbs_settings/stopper_lookup_table.txt" << endl;

        lookup_active_stopper3.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                              chain_one, chain_two, chain_three, false);
#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        lookup_active_stopper3_time.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                                   chain_one, chain_two, chain_three_time, false);
#endif

        // now we will do this for Y stripes, skipping the X stripes, because the nr of y stripes
        // can be different

        chain_two.clear();
        chain_two.push_back("x_skip_this_keyword");    // to skip over already read values
        chain_two.push_back("y_");
        chain_three.clear();
#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        chain_three_time.clear();
#endif

        for(unsigned int i = 0; i < NR_OF_STOPPER_STRIPES_Y; i++) {
            ostringstream s;
            s << setw(2) << setfill('0') << i;
            chain_three.push_back(s.str() + "_energy");

#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

            chain_three_time.push_back(s.str() + "_time");
#endif

        }
        lookup_active_stopper3.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                              chain_one, chain_two, chain_three, true);  // <-- true means: continuation
#ifdef ACTIVE_STOPPERS_HAVE_TIME_INFORMATION

        lookup_active_stopper3_time.read_from_disk("./mbs_settings/stopper_lookup_table.txt",
                                                   chain_one, chain_two, chain_three_time, true);
#endif

    } //local
#endif

    //--------------------

#ifdef  POSITRON_ABSORBER_PRESENT
    {
        cout << "Trying to read  " << "./mbs_settings/tin100_lookup_table.txt" << endl;
        // at first we read the plates 0-1
        vector<string> chain_one;
        chain_one.push_back("absorber_front_");
        chain_one.push_back("absorber_rear_");

        vector<string> chain_two;
        for(unsigned int i = 0; i < 2; i++) {  // <-- so many of them
            ostringstream s;
            s << setw(2) << setfill('0') << i;
            chain_two.push_back(s.str());
        }

        vector<string> chain_three;
        for(unsigned int i = 0; i < POSITRON_ABSORBER_STRIPS_NR; i++) {  // <-- so many of them
            ostringstream s;
            s << "_strip_" << setw(2) << setfill('0') << i << "_energy";
            chain_three.push_back(s.str());
        }

        //   cout << "Trying to read  " << "./mbs_settings/stopper_lookup_table.txt" << endl;

        lookup_absorber_0_1.read_from_disk("./mbs_settings/tin100_lookup_table.txt",
                                           chain_one, chain_two, chain_three, false);

        // now we try to read the next 2-9 plates into different lookup table
        chain_two.clear();
        for(unsigned int i = 2; i < POSITRON_ABSORBER_NR; i++) {  // <-- so many of them
            ostringstream s;
            s << setw(2) << setfill('0') << i;
            chain_two.push_back(s.str());
        }
        lookup_absorber_2_9.read_from_disk("./mbs_settings/tin100_lookup_table.txt",
                                           chain_one, chain_two, chain_three, false,
                                           0, // optional default arguments needed if
                                           2, // absorbers nr start  from 2, not from 0 (see the for loop)
                                           0);

        // ============== lookup_absorber_back_sides
        // now we try to read the next 2-9 plates into different lookup table
        // at first we read the plates 0-1

        chain_two.clear();
        for(unsigned int i = 0; i < POSITRON_ABSORBER_NR; i++) {  // <-- so many of them
            ostringstream s;
            s << setw(2) << setfill('0') << i;
            chain_two.push_back(s.str());
        }
        chain_three.clear();
        chain_three.push_back("_back_side_energy");

        //   cout << "Trying to read  " << "./mbs_settings/stopper_lookup_table.txt" << endl;

        lookup_absorber_back_sides.read_from_disk("./mbs_settings/tin100_lookup_table.txt",
                                                  chain_one, chain_two, chain_three, false);

    } // local
#endif

#ifdef  MUNICH_MATRIX_STRIPES_X
#ifdef NIGDY   // because it is hardcoded as multiwire
    // as there can be different nr of strinps for X and different for Y
    // here we make a trick. At first we read the lookup table for X only,
    // with the certain number of X stripes.

    // Later we do it once more, but skipping already read X items,
    // now with Y giving the right (different) nr of Y stripes
    {
        // local
        vector<string> chain_one;
        chain_one.push_back("stopper_matrix");

        vector<string> chain_two;
        chain_two.push_back("_plate_x_strip_");    // only X strips <---

        vector<string> chain_three;
        for(unsigned int i = 0; i < MUNICH_MATRIX_STRIPES_X; i++) {  // <-- so many of them
            ostringstream s;
            s << setw(2) << setfill('0') << i << "_energy";
            chain_three.push_back(s.str());
        }

        //   cout << "Trying to read  " << "./mbs_settings/stopper_lookup_table.txt" << endl;

        .read_from_disk("./mbs_settings/tin100_lookup_table.txt",
                        chain_one, chain_two, chain_three, false);

        // now we will do this for Y stripes, skipping the X stripes, because the nr of y stripes
        // can be different

        chain_two.clear();
        chain_two.push_back("_skip_this_keyword");    // to skip over already read values
        chain_two.push_back("_plate_y_strip_");

        chain_three.clear();

        for(unsigned int i = 0; i < MUNICH_MATRIX_STRIPES_Y; i++) {
            ostringstream s;
            s << setw(2) << setfill('0') << i << "_energy";
            chain_three.push_back(s.str());
        }

        lookup_stopper_matrix.read_from_disk("./mbs_settings/tin100_lookup_table.txt",
                                             chain_one, chain_two, chain_three, true);  // <-- true means: continuation


    } // end of local
#endif  // NIGDY
#endif

#ifdef MUN_ACTIVE_STOPPER_PRESENT
    read_munich_lookup();
#endif

    // in the lookup table there is also information where
    // to find time calibrator singanl for two vxi crates of GER
    read_ger_synchro_group_item();

    frs_subevents = ger_subevents = hec_subevents = dgf_subevents
            = unknown_type_subevents = zero_type_subevents = frs_hec_event
            = frs_ger_event = frs_ger_hec_event = ger_hec_event
            = ger_frs_event = ger_frs_hec_event = ger_orphan
            = hec_orphan = frs_orphan = ger_empty = 0;

    ger_matched_single_event = frs_matched_single_event = 0;

#if ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||         (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008)\
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))

    frs_user_subevents = time_subevents = 0;

    statistics_tray0_counter =
            statistics_tray1_counter =
            statistics_tray2_counter = 0;
#endif

    counter_valid = 0;
    counter_put = 0;
    counter_try = 0;
    counter_total = 0;
    counter_nonsense = 0;

#if NEW_FRS_LOOKUP_TABLE == true
    // Note: lookup table for FRS will be loaded while alalysing
    // the first event (because we need address of the TIFJEvent object)
    // The pointes to the members would not be enough because we have
    // the arrays  (pointer to the member can not point to the element of the array)
    frsOne_lookup_should_be_loaded = true;
    frsUser_lookup_should_be_loaded = true;
#endif

    //#################################################################
#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
    //#################################################################
    //   vector< vector <int> > lookup_prisma;


    cout << "Reading  lookup_table for prisma " << endl;

    string fname("./calibration/prisma_lookup_table.txt");
    ifstream plik(fname.c_str());
    if(!plik) {
        cout << "Can't open  " << fname << endl;
        exit(23);
    }
    int geo = -1;
    int channel = -1;
    int array_nr = -1;
    while(plik) {
        plik >> zjedz >> geo
                >> zjedz >> channel
                >> zjedz  >> array_nr ;
        if(plik.eof()) break;

        //     cout  << "Read geo " << geo << ", ch = " << channel << ", array_nr=" << array_nr << endl;

        if(geo >= lookup_prisma.size()) {
            lookup_prisma.resize(geo + 1);
        }

        if(channel >= lookup_prisma[geo].size()) {
            lookup_prisma[geo].resize(channel + 1);
        }
        lookup_prisma[geo][channel] = array_nr;
    }
    //writing on screen for testing
    //   for(int i = 0 ; i < lookup_prisma.size() ; i++)
    //     for(int k = 0 ; k < lookup_prisma[i].size() ; k++)
    //     {
    //       //cout << "lookup_prisma [" << i << "][" << k << "] = " << lookup_prisma[i][k] << endl;
    //     }

#endif
}
//************************************************************************
/** No descriptions */
bool TIFJEventProcessor::read_the_gates_for_diff_spectrum(string name,
                                                          int *low, int *high)
{

    string fname = "./gates/" + name + "_gate_GATE.gate";
    ifstream plik(fname.c_str());
    if(!plik)
        cout << "I can not open file " << fname << endl;

    double tmp_double;

    plik >> zjedz >> tmp_double;
    *low = (int) tmp_double;

    plik >> zjedz >> tmp_double;
    *high = (int) tmp_double;

    if(!plik) {
        cout << "\a\a\aError while reading  file " << fname << endl;
        *low = -200;
        *high = 200;
        // saving such default gate
        ofstream out(fname.c_str());
        out << (*low) << "\t\t" << (*high) << endl;
        out.close();
    }

    plik.close();

    return true;
}
//*************************************************************************
/** No descriptions */

//#################################################################
#if ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||         (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008)\
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))

//#################################################################
void TIFJEventProcessor::output_statistics_events()
{
    ofstream out("./mbs_settings/matching_raport.txt");

    // to have any time information ================
    time_t rawtime = 0 ;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    out << asctime(timeinfo);

    out << "Up to now arrived:\n"
        << "\nFRS subevents = " << frs_subevents
        << "\nGER (by DGF XIA) subevents = " << ger_subevents

        << "    ( tray0= " << statistics_tray0_counter
        << ", tray1= " << statistics_tray1_counter
        << ", tray2= " << statistics_tray1_counter << ")\n"

        << "\nTIME subevents = " << time_subevents
        << "\nFRS USER subevents = " << frs_user_subevents
        << ", \n(unknown type subevents = " << unknown_type_subevents
        << ")\n";

    out.close();

    // specially for Piotr, raport about empty germanium events
    out.open("./empty_germanium_ratio.mon");
    out << ger_empty << "\n"
        << ger_subevents << "\n"
        << endl;
    out.close();

}

#else

void TIFJEventProcessor::output_statistics_events()
{

    return ;
#ifdef NIGDY
    ofstream out("./mbs_settings/matching_raport.txt");

    // to have any time information ================
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    out << asctime(timeinfo);

    //  out << "\n total: " << counter_total
    //    << " put: " << counter_put
    //    << " try: "<< counter_try
    //    << " valid: " << counter_valid
    //    << " nonsense: " << counter_nonsense
    //    << endl;

    out << "Up to now arrived:\n" << "\nfrs_subevents = " << frs_subevents
        << "\nger_subevents = " << ger_subevents << "    (in this "
        << ger_empty << " were empty)" << "\nhec_subevents = "
        << hec_subevents << "\ndgf_subevents = " << dgf_subevents
        << "\nmib_subevents = " << mib_subevents
           //<< "\n('zero type' subevents = " << zero_type_subevents  // <--- not meaningfull
        << ", \n(unknown type subevents = " << unknown_type_subevents
        << ")\n\n\nYou are working now in the mode: ";

    if(type_of_matching == 1) {
        out << " 'Match to FRS subevent' \n\nFrom these subevents "
               "the following events were created: \n\n";
        //      out   << "Events FRS+GER+HEC+DGF: " << frs_ger_hec_dgf_event
        //
        //        << "\nEvents FRS+GER+HEC        : " << frs_ger_hec_event
        //        << "\nEvents FRS+GER+   +DGF: " <<  frs_ger_dgf_event
        //        << "\nEvents FRS+   +HEC+DGF: " << frs_hec_dgf_event
        //
        //
        //        << "\nEvents FRS+GER    : " << frs_ger_event
        //        << "\nEvents FRS+    HEC: " << frs_hec_event
        //        << "\nEvents FRS+   +   +DGF : " << frs_dgf_event
        //
        //        << "\nEvents FRS        : " << frs_matched_single_event << "\n";

        if(how_many_branches == 5) {
            // make statistics  - has to be more tricky ....

            for(int m = 5; m > 0; m--) {
                //out << " Combinations of  " << m << " branches ----------------\n" ;

                for(unsigned int i = 0; i < sizeof(event_combination)
                    / sizeof(int); i++) {
                    // trick to display starting from the more
                    // complicated combinations to the simplest
                    int how_many_bits = ((1 & i) != 0) + ((2 & i) != 0) + ((4
                                                                            & i) != 0) + ((8 & i) != 0) + ((16 & i) != 0);

                    // out << "how_many_bits " << how_many_bits << ", m=" << m
                    //     << ", i= " << i << endl;
                    if(m != how_many_bits)
                        continue;
                    //out << "Dsiplay it  " << endl;


                    if((1 & i) == 0)
                        continue; // must be FRS
                    if(!event_combination[i])
                        continue; // we do not display zeros
                    out << "Events    FRS";
                    if(2 & i)
                        out << " + GER";
                    if(4 & i)
                        out << " + HEC";
                    if(8 & i)
                        out << " + DGF";
                    if(16 & i)
                        out << " + MIB";
                    out << " : " << event_combination[i] << "\n";
                } // for i
            } // f  or m
        }
        out << "\nNote: The " << ger_empty
            << " 'empty GER' subevents - even if matched correctly - will not create the valid event, so "
               "the real nr. of particular type of events can be smaller\n\n";

    } else if(type_of_matching == 2) {
        out << " 'Match to GER subevent' \n\nFrom these subevents "
               "the following were created: \n\n";

        //    out     << "Events GER+FRS+HEC+DGF: " << ger_frs_hec_dgf_event << endl
        //        << "Events GER+FRS+HEC: " << ger_frs_hec_event << endl
        //        << "\nEvents GER+FRS+   +DGF: " << ger_frs_dgf_event << endl
        //        << "\nEvents GER+   +HEC+DGF: " << ger_hec_dgf_event << endl
        //
        //
        //        << "\nEvents GER+FRS    : " << ger_frs_event << endl
        //        << "\nEvents GER+   +HEC: " << ger_hec_event << endl
        //        << "\nEvents GER+   +    +DGF: " << ger_dgf_event << endl
        //        << "\nEvents GER        : " << ger_matched_single_event << endl;

        if(how_many_branches == 5) {
            for(int m = 5; m > 0; m--) {
                //out << " Combinations of  " << m << " branches ----------------\n" ;

                for(unsigned int i = 0; i < sizeof(event_combination)
                    / sizeof(int); i++) {
                    // trick to display starting from the more complicated combinations
                    // to the simplest
                    int how_many_bits = ((1 & i) != 0) + ((2 & i) != 0) + ((4
                                                                            & i) != 0) + ((8 & i) != 0) + ((16 & i) != 0);

                    //out << "how_many_bits " << how_many_bits << ", m=" << m
                    //      << ", i= " << i << endl;
                    if(m != how_many_bits)
                        continue;
                    //out << "Dsiplay it  " << endl;


                    if((1 & i) == 0)
                        continue; // must be GER
                    if(!event_combination[i])
                        continue; // we do not display zeros

                    out << "Events    GER";
                    if(2 & i)
                        out << " + FRS";
                    if(4 & i)
                        out << " + HEC";
                    if(8 & i)
                        out << " + DGF";
                    if(16 & i)
                        out << " + MIB";
                    out << " : " << event_combination[i] << "\n";
                } // for i
            } // f  or m
        } // end if(how_many_branches == 5)
        out << "\nNote: The " << ger_empty
            << " 'empty GER' subevents - even if matched correctly - will not create the valid event, so "
               "the real nr. of particular type of events can be smaller\n\n";

    } else {
        out << "\nNote: The " << ger_empty
            << " 'empty GER' subevent - will not go to analysis as a valid event, so "
               "the real nr. of analysed GER events can be smaller\n";

        out << " 'No matching' \n\nSo from this were created: \n";

    }
    out << "Created  unmatched subevents: " << "\nGER: " << ger_orphan
        << "\nFRS: " << frs_orphan << "\nHEC: " << hec_orphan << "\nDGF: "
        << dgf_orphan << "\nMIB: " << mib_orphan << endl;

#if 0

    << endl << endl << endl << endl << endl
            << endl << "!!!! CAUTION CAUTION CAUTION !!!!" << endl
            << "UNMATCHED subevents are kept in memory FOREVER in singles mode!!!"
            << endl << "!!!! CAUTION CAUTION CAUTION !!!!" << endl
            << endl << endl << endl << endl
           #endif

               out << "\n\n\n(Current Queue sizes   FRS: " << frs_subev_queue.size()
            << " GER: " << ger_subev_queue.size() << " HEC: "
            << hec_subev_queue.size() << " DGF: " << dgf_subev_queue.size()
            << " MIB: " << mib_subev_queue.size() << ")" << endl;

    out.close();

    // specially for Piotr, raport about empty germanium events
    out.open("./empty_germanium_ratio.mon");
    out << ger_empty << "\n" << ger_subevents << "\n" << endl;
    out.close();
#endif // NIGDY

}
#endif // version for classical or STOPPED BEAM campaign
//*************************************************************************
//*************************************************************************
void TIFJEventProcessor::perhaps_one_queue_is_too_big()
{

    bool flag_big_frs = frs_subev_queue.size() > 40000;
    bool flag_big_ger = ger_subev_queue.size() > 40000;
    bool flag_big_hec = hec_subev_queue.size() > 40000;
    bool flag_big_dgf = dgf_subev_queue.size() > 40000;
    bool flag_big_mib = mib_subev_queue.size() > 40000;

    if(flag_big_frs || flag_big_ger || flag_big_hec || flag_big_dgf
            || flag_big_mib) {
        cout << "\nWARNING: Many events come from " << (flag_big_frs ? " FRS "
                                                                     : "") << (flag_big_ger ? " GER " : "")
             << (flag_big_hec ? " HEC " : "") << (flag_big_dgf ? " DGF "
                                                               : "") << (flag_big_mib ? " MIB " : "")
             << "\nwhile nothing comes from the branch "

             << ((frs_subev_queue.empty() && need_FRS) ? " FRS " : "")
             << ((ger_subev_queue.empty() && need_GER) ? " GER " : "")
             << ((hec_subev_queue.empty() && need_HEC) ? " HEC " : "")
             << ((dgf_subev_queue.empty() && need_DGF) ? " DGF " : "")
             << ((mib_subev_queue.empty() && need_MIB) ? " MIB " : "")

             << "\n\a\a\aPerhaps this branch stopped working ?\n"
             << "(or you specified a wrong subevent matching combination)\a\n"
             << endl;

        ofstream plik("./commands/synchronisation_lost.txt");
        plik << "Multi Branches Watchdog barks !\n"
             << "WARNING: Many events come from " << (flag_big_frs ? " FRS "
                                                                   : "") << (flag_big_ger ? " GER " : "")
             << (flag_big_hec ? " HEC " : "") << (flag_big_dgf ? " DGF "
                                                               : "") << (flag_big_mib ? " MIB " : "")
             << "\nwhile nothing comes from the branch "

             << ((frs_subev_queue.empty() && need_FRS) ? " FRS " : "")
             << ((ger_subev_queue.empty() && need_GER) ? " GER " : "")
             << ((hec_subev_queue.empty() && need_HEC) ? " HEC " : "")
             << ((dgf_subev_queue.empty() && need_DGF) ? " DGF " : "")
             << ((mib_subev_queue.empty() && need_MIB) ? " MIB " : "")

             << "\n!!! Perhaps this branch stopped working "
                "and does not deliver subevents anymore !!!\n"
             << "(Note: If this message aperars just after starting, "
                "it can mean that \n"
                "you specified a wrong combination of the subevent matching. \n"
                "In such a case -\n"
                " 1). Go to  Spy_analysis->Parameters for subevent matching. \n"
                " 2). Watch detailed report\n"
                " 3). Change the matching combinations\n" << endl;

        plik.close();

        if(flag_big_frs)
            frs_subev_queue.pop();
        if(flag_big_ger)
            ger_subev_queue.pop();
        if(flag_big_hec)
            hec_subev_queue.pop();
        if(flag_big_dgf)
            dgf_subev_queue.pop();
        if(flag_big_mib)
            mib_subev_queue.pop();

    }
}
//************************************************************************
/** for debuging purposes */
void TIFJEventProcessor::show_n_longwords(daq_word_t *buf, int how_many)
{
    for(int i = 0; i < how_many; i++) {
        cout << "nr " << i << ")   0x" << hex << buf[i] << "\t\t dec = " << dec
             << buf[i] << endl;
    }
}
//***************************************************************************
/** No descriptions */
int TIFJEventProcessor::matching_to_FRS(TIFJEvent *te, bool flag_ger,
                                        bool flag_hec, bool flag_dgf, bool flag_mib)
{

    if(frs_subev_queue.empty() || (flag_ger ? ger_subev_queue.empty() : false)
            || (flag_hec ? hec_subev_queue.empty() : false)
            || (flag_dgf ? dgf_subev_queue.empty() : false)
            || (flag_mib ? mib_subev_queue.empty() : false)) {
        // if one of the needed queues is empty - I
        // should not try another combinations, because
        // they could create non complete event
        // So: read more (sub?)events from input
        perhaps_one_queue_is_too_big();
        return take_another_subevent; // take_another_subevent ;
    }

    if((flag_ger ? ger_is_in_frs_gate() : true)
            && (flag_hec ? hec_is_in_frs_gate() : true)
            && (flag_dgf ? dgf_is_in_frs_gate() : true)
            && (flag_mib ? mib_is_in_frs_gate() : true)) {

        unpack_frs_subevent(frs_subev_queue.front(), te);
        if(flag_ger)
            unpack_ger_subevent(ger_subev_queue.front(), te);
        if(flag_hec)
            unpack_hec_subevent(hec_subev_queue.front(), te);
        if(flag_dgf)
            unpack_dgf_subevent(dgf_subev_queue.front(), te);
        if(flag_mib)
            unpack_mib_subevent(mib_subev_queue.front(), te);

        // make statistics  - has to be more tricky ....

        unsigned int signature = 1 // FRS
                + (flag_ger << 1) + (flag_hec << 2) + (flag_dgf << 3)
                + (flag_mib << 4);

        //    cout
        //        << "The matching was FRS "
        //        << (flag_ger? " GER " : "     ")
        //        << (flag_dgf? " DGF " : "     ")
        //        << (flag_mib? " MIB " : "     ")
        //        << (flag_hec? " HEC " : "     ")
        //        << endl;


        event_combination[signature]++; //    frs_ger_hec_dgf_event++;

        return event_ready; // true;
    }

    return try_another; // try another  ;
}
//**********************************************************************************
/** No descriptions */
int TIFJEventProcessor::matching_to_GER(TIFJEvent *te, bool flag_frs,
                                        bool flag_hec, bool flag_dgf, bool flag_mib)
{

    if(ger_subev_queue.empty() || (flag_frs ? frs_subev_queue.empty() : false)
            || (flag_hec ? hec_subev_queue.empty() : false)
            || (flag_dgf ? dgf_subev_queue.empty() : false)
            || (flag_mib ? mib_subev_queue.empty() : false)) {
        // if one of the needed ques is empty - I
        // should not try another combinations, because
        // they could create non complete event
        // So: read more (sub?)events from input
        perhaps_one_queue_is_too_big();
        return take_another_subevent; // take_another_subevent ;   /
    }

    if((flag_frs ? frs_is_in_ger_gate() : true)
            && (flag_hec ? hec_is_in_ger_gate() : true)
            && (flag_dgf ? dgf_is_in_ger_gate() : true)
            && (flag_mib ? mib_is_in_ger_gate() : true)) {
        //          DBG( "frs and HEC and DGF in their gates, "
        //               "taking this ger+frs+HEC+DGF event." );

        unpack_ger_subevent(ger_subev_queue.front(), te);
        if(flag_frs)
            unpack_frs_subevent(frs_subev_queue.front(), te);
        if(flag_hec)
            unpack_hec_subevent(hec_subev_queue.front(), te);
        if(flag_dgf)
            unpack_dgf_subevent(dgf_subev_queue.front(), te);
        if(flag_mib)
            unpack_mib_subevent(mib_subev_queue.front(), te);

        // make statistics  - has to be more tricky ....

        unsigned int signature = 1 // ger
                + (flag_frs << 1) + (flag_hec << 2) + (flag_dgf << 3)
                + (flag_mib << 4);

        event_combination[signature]++; //    ger_frs_hec_dgf_event++;

        return event_ready; // true;
    }

    return try_another; // try another  ;
}
//***************************************************************************
//***************************************************************************
/** Reconstruction of the event in case of 5 branches
 // (4 types of subevents: FRS, GER, HEC, DGF, mib) */
bool TIFJEventProcessor::try_to_5reconstruct_event(TIFJEvent *te)
{

    // cout << "Trying to reconstruct the event ##################################################" << endl;

    counter_try += 1;
    flag_ger_has_data = true;

    do {
        switch(type_of_matching) {
        case 1: {
            // match relatively to the FRS subevent
            // at first remove everything what is too old for this FRS subevent


#ifdef NEVER
            cout << "In the current combination ";

            if(!frs_subev_queue.empty()) {
                cout << "   FRS = " << frs_subev_queue.front();
            } else
                cout << "  FRS que empty ";

            if(!ger_subev_queue.empty()) {
                cout << "  GER = " << ger_subev_queue.front();
            } else
                cout << "  GER que empty ";

            if(!hec_subev_queue.empty()) {
                cout << "  HEC = " << hec_subev_queue.front();
            } else
                cout << "  HEC que empty ";

            if(!dgf_subev_queue.empty()) {
                cout << "  DGF = " << dgf_subev_queue.front();
            } else
                cout << "  DGF que empty ";

            //------------ differences

            if(!ger_subev_queue.empty() && !frs_subev_queue.empty()) {
                cout << "  GER_minus_FRS = "
                     << (ger_subev_queue.front() - frs_subev_queue.front());
            }
            if(!hec_subev_queue.empty() && !frs_subev_queue.empty()) {
                cout << "  HEC_minus_FRS = "
                     << (hec_subev_queue.front() - frs_subev_queue.front());

            }
            if(!dgf_subev_queue.empty() && !frs_subev_queue.empty()) {
                cout << "  DGF_minus_FRS = "
                     << (dgf_subev_queue.front() - frs_subev_queue.front());

            }
            cout << endl;

#endif

            if(ger_too_old_for_frs()) {
                DBG("GER too old for FRS, we have to reject this ger.");
                // if this on GER is too old for FRS subevent
                // throw it away, incremet statistics of not matched GER
                ger_subev_queue.pop();
                ger_orphan++;

                // and make the whole procedure of matching once more.
                // (continue the loop) As many times as all subev
                // which are too old will be removed
                continue;
            }

            if(hec_too_old_for_frs()) {
                // same as for germanium subvents
                DBG("HEC too old for FRS, we have to reject this hec.");

                hec_subev_queue.pop();
                hec_orphan++;
                continue;
            }

            if(dgf_too_old_for_frs()) {
                // same as for germanium subvents
                DBG("DGF too old for FRS, we have to reject this hec.");

                dgf_subev_queue.pop();
                dgf_orphan++;
                continue;
            }

            if(mib_too_old_for_frs()) {
                // same as for germanium subvents
                DBG("MIB too old for FRS, we have to reject this hec.");

                mib_subev_queue.pop();
                mib_orphan++;
                continue;
            }

            // -------------------------------------------------------------
            // nothing is too old, let's match ----------------------

            // 5 branches, when relatively to FRS -----------
            if(match_frs_ger_hec_dgf_mib) {
                switch(matching_to_FRS(te, 1, 1, 1, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            // -------- 4 branches only when relatively to FRS-----------

            if(match_frs_ger_hec_dgf) {
                switch(matching_to_FRS(te, 1, 1, 1, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            if(match_frs_ger_hec_mib) {
                switch(matching_to_FRS(te, 1, 1, 0, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            if(match_frs_ger_dgf_mib) {
                switch(matching_to_FRS(te, 1, 0, 1, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            if(match_frs_hec_dgf_mib) {
                switch(matching_to_FRS(te, 0, 1, 1, 1)) {
                case 0:
                    return false;
                case 1:
                    return true;
                case 2:
                    break;
                }
            }

            //== 3 =====================================================
            //        match_FRS_GER_HEC_xxx_xxx   1

            if(match_frs_ger_hec) {
                switch(matching_to_FRS(te, 1, 1, 0, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }
            //match_FRS_GER_xxx_DGF_xxx   1

            if(match_frs_ger_dgf) {
                switch(matching_to_FRS(te, 1, 0, 1, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }
            //match_FRS_xxx_HEC_DGF_xxx   1

            if(match_frs_hec_dgf) {
                switch(matching_to_FRS(te, 0, 1, 1, 0)) {
                case 0:
                    return false;
                case 1:
                    return true;
                case 2:
                    break;
                }
            }

            //match_FRS_GER_xxx_xxx_MIB   1
            if(match_frs_ger_mib) {
                switch(matching_to_FRS(te, 1, 0, 0, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            //match_FRS_xxx_HEC_xxx_MIB   1
            if(match_frs_hec_mib) {
                switch(matching_to_FRS(te, 0, 1, 0, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }
            //match_FRS_xxx_xxx_DGF_MIB   1
            if(match_frs_dgf_mib) {
                switch(matching_to_FRS(te, 0, 0, 1, 1)) {
                case 0:
                    return false;
                case 1:
                    return true;
                case 2:
                    break;
                }
            }

            // ------------------- doublets -------------------------------
            // -------- 2 branches only ---when relatively to FRS------------------------------
            //match_FRS_GER_xxx_xxx_xxx   1

            // if matching frs+ger+hec did not work / was disabled
            if(match_frs_ger) {
                switch(matching_to_FRS(te, 1, 0, 0, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            //match_FRS_xxx_HEC_xxx_xxx   1

            if(match_frs_hec) {
                switch(matching_to_FRS(te, 0, 1, 0, 0)) {
                case 0:
                    return false;
                case 1:
                    return true;
                case 2:
                    break;
                }
            }

            if(match_frs_dgf) {
                switch(matching_to_FRS(te, 0, 0, 1, 0)) {
                case 0:
                    return false;
                case 1:
                    return true;
                case 2:
                    break;
                }
            }
            //match_FRS_xxx_xxx_xxx_MIB   1
            if(match_frs_mib) {
                switch(matching_to_FRS(te, 0, 0, 0, 1)) {
                case 0:
                    return false;
                case 1:
                    return true;
                case 2:
                    break;
                }
            }

            // if we are here, that means that none of the above was
            // successful

            //------------------------------------------------------
            // here we are if everything is too young for FRS subevent
            // so: throw it away, increment statistics of not matched FRS

            if((need_GER && ger_subev_queue.empty())
                    || frs_subev_queue.empty() || (need_HEC
                                                   && hec_subev_queue.empty()) || (need_DGF
                                                                                   && dgf_subev_queue.empty()) || (need_MIB
                                                                                                                   && mib_subev_queue.empty())) {
                // nothing in the que to skip, so ask for more (sub?)events
                return false;
            } else if(match_frs_when_single) { // perhaps we want it as single ?
                unpack_frs_subevent(frs_subev_queue.front(), te);
                frs_matched_single_event++;
                event_combination[1]++; //  only frs is just here
                return true;
            } else {
                DBG("FRS is too old for any other, we have to throw it away, and try to match the next.");
                frs_subev_queue.pop();
                frs_orphan++;
                continue;
            }

            break; // we are never here
        }

            //#################################################################
            //#################################################################
            //#################################################################

        case 2: {
            // match relatively to the GER subevent
            // at first remove everything what is too old for this GER subevent


#ifdef NEVER
            cout << "In the current combination ";

            if(!ger_subev_queue.empty()) {
                cout << "   GER = " << ger_subev_queue.front();
            } else
                cout << "  GER que empty ";

            if(!frs_subev_queue.empty()) {
                cout << "  FRS = " << frs_subev_queue.front();
            } else
                cout << "  FRS que empty ";

            if(!hec_subev_queue.empty()) {
                cout << "  HEC = " << hec_subev_queue.front();
            } else
                cout << "  HEC que empty ";

            if(!dgf_subev_queue.empty()) {
                cout << "  DGF = " << dgf_subev_queue.front();
            } else
                cout << "  DGF que empty ";

            //------------ differences

            if(!frs_subev_queue.empty() && !ger_subev_queue.empty()) {
                cout << "  FRS_minus_GER = "
                     << (frs_subev_queue.front() - ger_subev_queue.front());
            }
            if(!hec_subev_queue.empty() && !ger_subev_queue.empty()) {
                cout << "  HEC_minus_GER = "
                     << (hec_subev_queue.front() - ger_subev_queue.front());

            }
            if(!dgf_subev_queue.empty() && !ger_subev_queue.empty()) {
                cout << "  DGF_minus_GER = "
                     << (dgf_subev_queue.front() - ger_subev_queue.front());

            }
            cout << endl;

#endif

            if(frs_too_old_for_ger()) {
                DBG("FRS too old for GER, we have to reject this frs.");
                // if this on FRS is too old for GER subevent
                // throw it away, incremet statistics of not matched FRS
                frs_subev_queue.pop();
                frs_orphan++;

                // and make the whole procedure of matching once more.
                // (continue the loop) As many times as all subev
                // which are too old will be removed
                continue;
            }

            if(hec_too_old_for_ger()) {
                // same as for frsmanium subvents
                DBG("HEC too old for GER, we have to reject this hec.");

                hec_subev_queue.pop();
                hec_orphan++;
                continue;
            }

            if(dgf_too_old_for_ger()) {
                // same as for frsmanium subvents
                DBG("DGF too old for GER, we have to reject this hec.");

                dgf_subev_queue.pop();
                dgf_orphan++;
                continue;
            }

            if(mib_too_old_for_ger()) {
                // same as for frsmanium subvents
                DBG("MIB too old for GER, we have to reject this hec.");

                mib_subev_queue.pop();
                mib_orphan++;
                continue;
            }

            // -------------------------------------------------------------
            // nothing is too old, let's match ----------------------

            // 5 branches, when relatively to GER -----------
            if(match_ger_frs_hec_dgf_mib) {
                switch(matching_to_GER(te, 1, 1, 1, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            // -------- 4 branches only when relatively to GER-----------

            if(match_ger_frs_hec_dgf) {
                switch(matching_to_GER(te, 1, 1, 1, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            if(match_ger_frs_hec_mib) {
                switch(matching_to_GER(te, 1, 1, 0, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            if(match_ger_frs_dgf_mib) {
                switch(matching_to_GER(te, 1, 0, 1, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            if(match_ger_hec_dgf_mib) {
                switch(matching_to_GER(te, 0, 1, 1, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            //== 3 =====================================================
            //        match_GER_FRS_HEC_xxx_xxx   1

            if(match_ger_frs_hec) {
                switch(matching_to_GER(te, 1, 1, 0, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }
            //match_GER_FRS_xxx_DGF_xxx   1

            if(match_ger_frs_dgf) {
                switch(matching_to_GER(te, 1, 0, 1, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }
            //match_GER_xxx_HEC_DGF_xxx   1

            if(match_ger_hec_dgf) {
                switch(matching_to_GER(te, 0, 1, 1, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            //match_GER_FRS_xxx_xxx_MIB   1
            if(match_ger_frs_mib) {
                switch(matching_to_GER(te, 1, 0, 0, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            //match_GER_xxx_HEC_xxx_MIB   1
            if(match_ger_hec_mib) {
                switch(matching_to_GER(te, 0, 1, 0, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }
            //match_GER_xxx_xxx_DGF_MIB   1
            if(match_ger_dgf_mib) {
                switch(matching_to_GER(te, 0, 0, 1, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            // ------------------- doublets -------------------------------
            // -------- 2 branches only ---when relatively to GER------------------------------
            //match_GER_FRS_xxx_xxx_xxx   1

            // if matching ger+frs+hec did not work / was disabled
            if(match_ger_frs) {
                switch(matching_to_GER(te, 1, 0, 0, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            //match_GER_xxx_HEC_xxx_xxx   1

            if(match_ger_hec) {
                switch(matching_to_GER(te, 0, 1, 0, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            if(match_ger_dgf) {
                switch(matching_to_GER(te, 0, 0, 1, 0)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }
            //match_GER_xxx_xxx_xxx_MIB   1
            if(match_ger_mib) {
                switch(matching_to_GER(te, 0, 0, 0, 1)) {
                case 0:
                    return false;
                case 1:
                    return flag_ger_has_data;
                case 2:
                    break;
                }
            }

            // if we are here, that means that none of the above was
            // successful

            //------------------------------------------------------
            // here we are if everything is too young for GER subevent
            // so: throw it away, increment statistics of not matched GER

            if((need_FRS && frs_subev_queue.empty())
                    || ger_subev_queue.empty() || (need_HEC
                                                   && hec_subev_queue.empty()) || (need_DGF
                                                                                   && dgf_subev_queue.empty()) || (need_MIB
                                                                                                                   && mib_subev_queue.empty())) {
                // nothing in the que to skip, so ask for more (sub?)events
                return false;
            } else if(match_ger_when_single) { // perhaps we want it as single ?
                unpack_ger_subevent(ger_subev_queue.front(), te);
                ger_matched_single_event++;
                event_combination[1]++; //  only ger is just here
                return true;
            } else {
                DBG("GER is too old for any other, we have to throw it away, and try to match the next.");
                ger_subev_queue.pop();
                ger_orphan++;
                continue;
            }

            break; // we are never here
        }

            //#################################################################
            //#################################################################
            //###########################################################
        case 0: { //  no matching at all
            bool something_to_do = false;
            //===========================
            if(flag_accept_single_frs && !frs_subev_queue.empty()) {
                unpack_frs_subevent(frs_subev_queue.front(), te);
                frs_orphan++;
                something_to_do = true;
            } else if(!flag_accept_single_frs)
                while(!frs_subev_queue.empty())
                    frs_subev_queue.pop();

            //===========================
            if(flag_accept_single_ger && !ger_subev_queue.empty()) {
                unpack_ger_subevent(ger_subev_queue.front(), te);
                ger_orphan++;
#ifdef JUREK

                something_to_do = true;
#else

                something_to_do = flag_ger_has_data;
#endif

            } else if(!flag_accept_single_ger)
                while(!ger_subev_queue.empty())
                    ger_subev_queue.pop();

            //===========================
            if(flag_accept_single_hec && !hec_subev_queue.empty()) {
                unpack_hec_subevent(hec_subev_queue.front(), te);
                hec_orphan++;
                something_to_do = true;
            } else if(!flag_accept_single_hec)
                while(!hec_subev_queue.empty())
                    hec_subev_queue.pop();

            //===========================
            if(flag_accept_single_dgf && !dgf_subev_queue.empty()) {
                unpack_dgf_subevent(dgf_subev_queue.front(), te);
                dgf_orphan++;
                something_to_do = true;
            } else if(!flag_accept_single_dgf)
                while(!dgf_subev_queue.empty())
                    dgf_subev_queue.pop();

            //===========================
            if(flag_accept_single_mib && !mib_subev_queue.empty()) {
                unpack_mib_subevent(mib_subev_queue.front(), te);
                mib_orphan++;
                something_to_do = true;
            } else if(!flag_accept_single_mib)
                while(!mib_subev_queue.empty())
                    mib_subev_queue.pop();

            return something_to_do;

        } // endof case
        } // end switch
    } while(1);
    return false; // we should never be here

}


#if 0  // NIGDY
//*****************************************************************************************
/** reading the types of events which are legal for 5 branches mode of MBS
 reading the time gates for time stamp system */
bool TIFJEventProcessor::read_in_5mbs_parameters(
        string nam_ger_minus_frs_timestamp_diff,
        string nam_frs_minus_hec_timestamp_diff,
        string nam_frs_minus_dgf_timestamp_diff,
        string nam_ger_minus_dgf_timestamp_diff,
        string nam_ger_minus_hec_timestamp_diff,
        string /*nam_dgf_minus_hec_timestamp_diff*/,
        string nam_frs_minus_mib_timestamp_diff,
        string nam_ger_minus_mib_timestamp_diff)
{

    string fname = "./mbs_settings/5_branches_legal_subevent_combinations.dat";
    ifstream plik(fname.c_str());

    type_of_matching = 0;
    how_many_branches = 5;

    if(!plik) {
        cout << "file does not exist, so I assume default state" << endl;
        flag_accept_single_frs = true;
        flag_accept_single_hec = true;
        flag_accept_single_ger = true;
        flag_accept_single_dgf = true;
        flag_accept_single_mib = true;
        type_of_matching = 0;
        collect_time_stamp_statistics_spectra = true;
        return true;
    }

    try {

        type_of_matching = (int) FH::find_in_file(plik,
                                                  "type_of_matching");

        // 5 branches, when relatively to frs ----------- ---
        match_frs_ger_hec_dgf_mib = FH::find_in_file(plik,
                                                     "match_FRS_GER_HEC_DGF_MIB");

        // -------- 4 branches only when relatively to frs---------------------------------
        match_frs_ger_hec_dgf = FH::find_in_file(plik,
                                                 "match_FRS_GER_HEC_DGF_xxx");
        match_frs_ger_hec_mib = FH::find_in_file(plik,
                                                 "match_FRS_GER_HEC_xxx_MIB");
        match_frs_ger_dgf_mib = FH::find_in_file(plik,
                                                 "match_FRS_GER_xxx_DGF_MIB");
        match_frs_hec_dgf_mib = FH::find_in_file(plik,
                                                 "match_FRS_xxx_HEC_DGF_MIB");
        // -------- 3 branches only when relatively to frs---------------------------------
        match_frs_ger_hec = FH::find_in_file(plik,
                                             "match_FRS_GER_HEC_xxx_xxx");
        match_frs_ger_dgf = FH::find_in_file(plik,
                                             "match_FRS_GER_xxx_DGF_xxx");
        match_frs_hec_dgf = FH::find_in_file(plik,
                                             "match_FRS_xxx_HEC_DGF_xxx");
        match_frs_ger_mib = FH::find_in_file(plik,
                                             "match_FRS_GER_xxx_xxx_MIB");
        match_frs_hec_mib = FH::find_in_file(plik,
                                             "match_FRS_xxx_HEC_xxx_MIB");
        match_frs_dgf_mib = FH::find_in_file(plik,
                                             "match_FRS_xxx_xxx_DGF_MIB");
        // -------- 2 branches only ---when relatively to frs------------------------------
        match_frs_ger = FH::find_in_file(plik,
                                         "match_FRS_GER_xxx_xxx_xxx");
        match_frs_hec = FH::find_in_file(plik,
                                         "match_FRS_xxx_HEC_xxx_xxx");
        match_frs_dgf = FH::find_in_file(plik,
                                         "match_FRS_xxx_xxx_DGF_xxx");
        match_frs_mib = FH::find_in_file(plik,
                                         "match_FRS_xxx_xxx_xxx_MIB");

        // -------- 1 branch,  ---when relatively to frs------------------------------
        //       match_frs  = FH::find_in_file(plik, "");
        match_frs_when_single = FH::find_in_file(plik,
                                                 "match_FRS_xxx_xxx_xxx_xxx"); //


        // matching to GER ------------

        match_ger_frs_hec_dgf_mib = FH::find_in_file(plik,
                                                     "match_GER_FRS_HEC_DGF_MIB");

        // -------- 4 branches only when relatively to ger---------------------------------
        match_ger_frs_hec_dgf = FH::find_in_file(plik,
                                                 "match_GER_FRS_HEC_DGF_xxx");
        match_ger_frs_hec_mib = FH::find_in_file(plik,
                                                 "match_GER_FRS_HEC_xxx_MIB");
        match_ger_frs_dgf_mib = FH::find_in_file(plik,
                                                 "match_GER_FRS_xxx_DGF_MIB");
        match_ger_hec_dgf_mib = FH::find_in_file(plik,
                                                 "match_GER_xxx_HEC_DGF_MIB");

        // -------- 3 branches only when relatively to ger---------------------------------
        match_ger_frs_hec = FH::find_in_file(plik,
                                             "match_GER_FRS_HEC_xxx_xxx");
        match_ger_frs_dgf = FH::find_in_file(plik,
                                             "match_GER_FRS_xxx_DGF_xxx");
        match_ger_hec_dgf = FH::find_in_file(plik,
                                             "match_GER_xxx_HEC_DGF_xxx");
        match_ger_frs_mib = FH::find_in_file(plik,
                                             "match_GER_FRS_xxx_xxx_MIB");
        match_ger_hec_mib = FH::find_in_file(plik,
                                             "match_GER_xxx_HEC_xxx_MIB");
        match_ger_dgf_mib = FH::find_in_file(plik,
                                             "match_GER_xxx_xxx_DGF_MIB");

        // -------- 2 branches only ---when relatively to ger------------------------------
        match_ger_frs = FH::find_in_file(plik,
                                         "match_GER_FRS_xxx_xxx_xxx");
        match_ger_hec = FH::find_in_file(plik,
                                         "match_GER_xxx_HEC_xxx_xxx");
        match_ger_dgf = FH::find_in_file(plik,
                                         "match_GER_xxx_xxx_DGF_xxx");
        match_ger_mib = FH::find_in_file(plik,
                                         "match_GER_xxx_xxx_xxx_MIB");

        // -------- 1 branch,  ---when relatively to ger------------------------------
        //      match_ger   = FH::find_in_file(plik, "");
        match_ger_when_single = FH::find_in_file(plik,
                                                 "match_GER_xxx_xxx_xxx_xxx");

        //====  No matching ===================

        flag_accept_single_frs = FH::find_in_file(plik,
                                                  "match5_FRS_alone");
        flag_accept_single_ger = FH::find_in_file(plik,
                                                  "match5_GER_alone");
        flag_accept_single_hec = FH::find_in_file(plik,
                                                  "match5_HEC_alone");
        flag_accept_single_dgf = FH::find_in_file(plik,
                                                  "match5_DGF_alone");
        flag_accept_single_mib = FH::find_in_file(plik,
                                                  "match5_MIB_alone");

        plik.close();
    } catch(Tno_keyword_exception &m) {
        cout << "error while reading the file " << fname << "\n\t" << m.message
             << endl;
        throw;
    } catch(Treading_value_exception &m) {
        cout << "error while reading the file " << fname << "\n\t" << m.message
             << endl;
        throw;
    }
    plik.close();

    // --------------------------------------------------------------------------------------------------


    //================= NEEDS ==============================================
    switch(type_of_matching) {
    case 1: // to FRS

        need_FRS = true; // surely !
        need_GER = // important only in case of FRS matching ?
                match_frs_ger_hec_dgf_mib || match_frs_ger_hec_dgf
                || match_frs_ger_hec_mib || match_frs_ger_dgf_mib
                || match_frs_ger_hec || match_frs_ger_dgf
                || match_frs_ger_mib || match_frs_ger;

        need_HEC = (match_frs_ger_hec_dgf_mib || match_frs_ger_hec_dgf
                    || match_frs_ger_hec_mib || match_frs_hec_dgf_mib
                    || match_frs_ger_hec || match_frs_hec_dgf || match_frs_hec_mib
                    || match_frs_hec);

        need_DGF = (match_frs_ger_hec_dgf_mib || match_frs_ger_hec_dgf
                    || match_frs_ger_dgf_mib || match_frs_hec_dgf_mib
                    || match_frs_ger_dgf || match_frs_hec_dgf || match_frs_dgf_mib
                    || match_frs_dgf);

        need_MIB = (match_frs_ger_hec_dgf_mib || match_frs_ger_hec_mib
                    || match_frs_ger_dgf_mib || match_frs_hec_dgf_mib
                    || match_frs_ger_mib || match_frs_hec_mib || match_frs_dgf_mib
                    || match_frs_mib);

        break;

    case 2: // to GER
        need_GER = true;
        need_FRS = // important only in case of GER matching ?
                match_ger_frs_hec_dgf_mib || match_ger_frs_hec_dgf
                || match_ger_frs_hec_mib || match_ger_frs_dgf_mib
                || match_ger_frs_hec || match_ger_frs_dgf
                || match_ger_frs_mib || match_ger_frs;

        need_HEC = (match_ger_frs_hec_dgf_mib || match_ger_frs_hec_dgf
                    || match_ger_frs_hec_mib || match_ger_hec_dgf_mib
                    || match_ger_frs_hec || match_ger_hec_dgf || match_ger_hec_mib
                    || match_ger_hec);

        need_DGF = (match_ger_frs_hec_dgf_mib || match_ger_frs_hec_dgf
                    || match_ger_frs_dgf_mib || match_ger_hec_dgf_mib
                    || match_ger_frs_dgf || match_ger_hec_dgf || match_ger_dgf_mib
                    || match_ger_dgf);

        need_MIB = (match_ger_frs_hec_dgf_mib || match_ger_frs_hec_mib
                    || match_ger_frs_dgf_mib || match_ger_hec_dgf_mib
                    || match_ger_frs_mib || match_ger_hec_mib || match_ger_dgf_mib
                    || match_ger_mib);

        break;

    case 0:
        need_FRS = flag_accept_single_frs;
        need_HEC = flag_accept_single_hec;
        need_GER = flag_accept_single_ger;
        need_DGF = flag_accept_single_dgf;
        need_MIB = flag_accept_single_mib;
        break;

    }

    if(need_MIB)
        lookup_miniball.read_from_disk();

    //    reading the gates --------------------------------------------------------


    // from the another  file ===========================================
    fname = "./mbs_settings/collect_timestamp_diff_spectra.dat";
    plik.open(fname.c_str());
    if(!plik) {
        cout << "file does not exist, so I assume default state" << endl;
        collect_time_stamp_statistics_spectra = true;
    } else {
        try {
            collect_time_stamp_statistics_spectra = FH::find_in_file(
                        plik, "collect_time_stamp_statistics_spectra");
        } catch(Tfile_helper_exception &m) {
            cout << "Error while reading the file " << fname << "\n\t"
                 << m.message << endl;
            throw;
        }
    }

    // Now we read the gates for the diff spectra ======================

    //----------------------
    int ger_minus_frs_low;
    int ger_minus_frs_high;

    if(!read_the_gates_for_diff_spectrum(nam_ger_minus_frs_timestamp_diff,
                                         &ger_minus_frs_low, &ger_minus_frs_high)) {
        // some warning about defaults;
    }

    //-----------------
    int frs_minus_hec_low;
    int frs_minus_hec_high;

    if(!read_the_gates_for_diff_spectrum(nam_frs_minus_hec_timestamp_diff,
                                         &frs_minus_hec_low, &frs_minus_hec_high)) {
        // some warning about defaults;
    }

    //-----------------
    int frs_minus_dgf_low;
    int frs_minus_dgf_high;

    if(!read_the_gates_for_diff_spectrum(nam_frs_minus_dgf_timestamp_diff,
                                         &frs_minus_dgf_low, &frs_minus_dgf_high)) {
        // some warning about defaults;
    }

    //--------------------
    int ger_minus_dgf_low;
    int ger_minus_dgf_high;

    if(!read_the_gates_for_diff_spectrum(nam_ger_minus_dgf_timestamp_diff,
                                         &ger_minus_dgf_low, &ger_minus_dgf_high)) {
        // some warning about defaults;
    }

    //--------------------
    int ger_minus_hec_low;
    int ger_minus_hec_high;

    if(!read_the_gates_for_diff_spectrum(nam_ger_minus_hec_timestamp_diff,
                                         &ger_minus_hec_low, &ger_minus_hec_high)) {
        // some warning about defaults;
    }

    // Note: there is no gates on the spectrum dgf_minus_hec


    //-----------------
    int frs_minus_mib_low;
    int frs_minus_mib_high;

    if(!read_the_gates_for_diff_spectrum(nam_frs_minus_mib_timestamp_diff,
                                         &frs_minus_mib_low, &frs_minus_mib_high)) {
        // some warning about defaults;
    }

    //--------------------
    int ger_minus_mib_low;
    int ger_minus_mib_high;

    if(!read_the_gates_for_diff_spectrum(nam_ger_minus_mib_timestamp_diff,
                                         &ger_minus_mib_low, &ger_minus_mib_high)) {
        // some warning about defaults;
    }

    // finally I need============================

    //------------------------------ for matching to FRS ---------------
    down_ger_minus_frs_gate = ger_minus_frs_low;
    up_ger_minus_frs_gate = ger_minus_frs_high;

    down_hec_minus_frs_gate = minimal(-frs_minus_hec_low, -frs_minus_hec_high);
    up_hec_minus_frs_gate = maximal(-frs_minus_hec_low, -frs_minus_hec_high);

    down_dgf_minus_frs_gate = minimal(-frs_minus_dgf_low, -frs_minus_dgf_high);
    up_dgf_minus_frs_gate = maximal(-frs_minus_dgf_low, -frs_minus_dgf_high);

    down_mib_minus_frs_gate = minimal(-frs_minus_mib_low, -frs_minus_mib_high);
    up_mib_minus_frs_gate = maximal(-frs_minus_mib_low, -frs_minus_mib_high);

    //------------------------------ for matching to GER ---------------

    down_frs_minus_ger_gate = minimal(-ger_minus_frs_low, -ger_minus_frs_high);
    up_frs_minus_ger_gate = maximal(-ger_minus_frs_low, -ger_minus_frs_high);

    down_hec_minus_ger_gate = minimal(-ger_minus_hec_low, -ger_minus_hec_high);
    up_hec_minus_ger_gate = maximal(-ger_minus_hec_low, -ger_minus_hec_high);

    down_dgf_minus_ger_gate = minimal(-ger_minus_dgf_low, -ger_minus_dgf_high);
    up_dgf_minus_ger_gate = maximal(-ger_minus_dgf_low, -ger_minus_dgf_high);

    down_mib_minus_ger_gate = minimal(-ger_minus_mib_low, -ger_minus_mib_high);
    up_mib_minus_ger_gate = maximal(-ger_minus_mib_low, -ger_minus_mib_high);

    return true;

}
#endif // NIGDY


/** No descriptions ***************************************************************/
// in the lookup table there is also information where
// to find time calibrator singanl for two vxi crates of GER
void TIFJEventProcessor::read_ger_synchro_group_item()
{

    string fname = "./mbs_settings/lookup_table.txt";
    ifstream plik(fname.c_str());
    if(!plik) {
        cout << "!!! Fatal error: I can't open file " << fname << endl;
        exit(1);
    }

    try {
        ger_synchro_vxi_crate1_group
                = (unsigned int) FH::find_in_file(plik,
                                                  "time_calibrator_vxi_crate1_group");
        ger_synchro_vxi_crate1_item
                = (unsigned int) FH::find_in_file(plik,
                                                  "time_calibrator_vxi_crate1_item");
        ger_synchro_vxi_crate2_group
                = (unsigned int) FH::find_in_file(plik,
                                                  "time_calibrator_vxi_crate2_group");
        ger_synchro_vxi_crate2_item
                = (unsigned int) FH::find_in_file(plik,
                                                  "time_calibrator_vxi_crate2_item");
    } catch(Tfile_helper_exception &m) {
        cout << "Error while reading the file " << fname << "\n\t" << m.message
             << endl;
        throw;
    }
}




//******************************************************************************************************
void TIFJEventProcessor::read_frsOne_lookup_table()
{
    // cout << "TIFJEventProcessor::read_frsOne_lookup_table() " << endl;
    frsOne_lookup_should_be_loaded = false;
    frsOne_lookup_table.clear();

    string fname = "./mbs_settings/frs_lookup_table.txt";

#if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
    fname = "./calibration/exotic_lookup_table.txt";

#elif  CURRENT_EXPERIMENT_TYPE==PISOLO2_EXPERIMENT


    fname = "./mbs_settings/pisolo_lookup_table.txt";
#endif




    ifstream plik(fname.c_str());

    if(!plik) {
        cout << "file " << fname << " does not exist." << endl;
        // some other more drastic actions
        exit(32);
    }

    try {
        // Reading from the file the data where from the particular signal will be comming.
        // it is put to the vector, and then, every event the loop over this vector
        // will do the unpacking.

        // without Macrodefiniction should be:
        // frs_lookup_table.push_back(frs_signal(plik, "mw11_x_right", &(target_event->mw11_x_right)));

        // this macrodefiniction uses the operator # to "stringify" a name of an argument
#define LLOne(wstaw)      frsOne_lookup_table.push_back(frs_signal(plik, #wstaw, &(target_event->wstaw)));

#ifdef  FRS_PATTERN_MODULE_PRESENT
        LLOne(frs_pattern_module);
#endif


#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
        LLOne(prisma_mcp_x);        //
        LLOne(prisma_mcp_y);        //
        LLOne(prisma_mcp_u);        //

#endif   // PRISMA_EXPERIMENT


#if CURRENT_EXPERIMENT_TYPE==PISOLO2_EXPERIMENT

        //LLOne ( pisolo_mcp01_x);
        //LLOne ( pisolo_mcp01_y);    // micro channel plate detectors measuring position
        //LLOne ( pisolo_mcp02_x);
        //LLOne (  pisolo_mcp02_y);

        //LLOne ( pisolo_si_target_en);   // EN is the energy released in the Si detector (near the target)

        LLOne(pisolo_deltaE[0]);        // ionisation chamber
        LLOne(pisolo_deltaE[1]);        // ionisation chamber
        LLOne(pisolo_dE_res);        // ionisation chamber fulle
        //int pisolo_e_res;      // residual energy released in the Ionisation Chamber (normally zero)

        LLOne(pisolo_si_final);    // final energy, wher the ion stops
        LLOne(pisolo_tof1);
        LLOne(pisolo_tof2);
        LLOne(pisolo_tof3);

        //   MONLR and MONUD are the two pairs of monitors (left-right)
        //     and (up-down). Each pair into one ADC input.
        LLOne(pisolo_mon_lr);
        LLOne(pisolo_mon_ud);

#endif

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

        LLOne(scaler_frs_sc42_left_or_right);
        LLOne(scaler_frs_sc42_left);
        LLOne(scaler_frs_sc42_right);
        LLOne(scaler_frs_sc43_left);
#endif



#ifdef RISING_GERMANIUMS_PRESENT



        LLOne(mw11_x_right);
        LLOne(mw11_x_left);
        LLOne(mw11_y_up);
        LLOne(mw11_y_down);
        LLOne(mw11_anode);    //

        LLOne(mw21_x_right);
        LLOne(mw21_x_left);
        LLOne(mw21_y_up);
        LLOne(mw21_y_down);
        LLOne(mw21_anode);

        LLOne(mw22_x_right);
        LLOne(mw22_x_left);
        LLOne(mw22_y_up);
        LLOne(mw22_y_down);
        LLOne(mw22_anode);

        LLOne(mw31_x_right);
        LLOne(mw31_x_left);
        LLOne(mw31_y_up);
        LLOne(mw31_y_down);
        LLOne(mw31_anode);

        LLOne(mw41_x_right);
        LLOne(mw41_x_left);
        LLOne(mw41_y_up);
        LLOne(mw41_y_down);
        LLOne(mw41_anode);

        LLOne(mw42_x_right);
        LLOne(mw42_x_left);
        LLOne(mw42_y_up);
        LLOne(mw42_y_down);
        LLOne(mw42_anode);
        //#endif    // def MW_USED

#ifdef MW51_USED

        LLOne(mw51_x_left);
        LLOne(mw51_x_right);
        LLOne(mw51_y_up);
        LLOne(mw51_y_down);
        LLOne(mw51_anode);
#endif

#ifdef MW71_USED

        LLOne(mw71_x_left);
        LLOne(mw71_x_right);
        LLOne(mw71_y_up);
        LLOne(mw71_y_down);
        LLOne(mw71_anode);
#endif

#ifdef MW81_USED

        LLOne(mw81_x_left);
        LLOne(mw81_x_right);
        LLOne(mw81_y_up);
        LLOne(mw81_y_down);
        LLOne(mw81_anode);
#endif

#ifdef MW82_USED

        LLOne(mw82_x_left);
        LLOne(mw82_x_right);
        LLOne(mw82_y_up);
        LLOne(mw82_y_down);
        LLOne(mw82_anode);
#endif

        // #ifdef TPC41_PRESENT
        //     LL(tpc41_drift_time_array_ptr[0]);
        //     LL(tpc41_drift_time_array_ptr[1]);
        //     LL(tpc41_drift_time_array_ptr[2]);
        //     LL(tpc41_drift_time_array_ptr[3]);
        //
        //
        //     LL(tpc41_left_time_array_ptr[0]);
        //     LL(tpc41_left_time_array_ptr[1]);
        //
        //     LL(tpc41_right_time_array_ptr[0]);
        //     LL(tpc41_right_time_array_ptr[1]);
        //
        //     LL(tpc41_drift_energy_array_ptr[0]);
        //     LL(tpc41_drift_energy_array_ptr[1]);
        //     LL(tpc41_drift_energy_array_ptr[2]);
        //     LL(tpc41_drift_energy_array_ptr[3]);
        //
        //     LL(tpc41_left_energy_array_ptr[0]);
        //     LL(tpc41_left_energy_array_ptr[1]);
        //
        //     LL(tpc41_right_energy_array_ptr[0]);
        //     LL(tpc41_right_energy_array_ptr[1]);
        //
        // #endif

        // #ifdef TPC42_PRESENT
        //     LL(tpc42_drift_time_array_ptr[0]);
        //     LL(tpc42_drift_time_array_ptr[1]);
        //     LL(tpc42_drift_time_array_ptr[2]);
        //     LL(tpc42_drift_time_array_ptr[3]);
        //
        //
        //     LL(tpc42_left_time_array_ptr[0]);
        //     LL(tpc42_left_time_array_ptr[1]);
        //
        //     LL(tpc42_right_time_array_ptr[0]);
        //     LL(tpc42_right_time_array_ptr[1]);
        //
        //     LL(tpc42_drift_energy_array_ptr[0]);
        //     LL(tpc42_drift_energy_array_ptr[1]);
        //     LL(tpc42_drift_energy_array_ptr[2]);
        //     LL(tpc42_drift_energy_array_ptr[3]);
        //
        //     LL(tpc42_left_energy_array_ptr[0]);
        //     LL(tpc42_left_energy_array_ptr[1]);
        //
        //     LL(tpc42_right_energy_array_ptr[0]);
        //     LL(tpc42_right_energy_array_ptr[1]);
        //
        // #endif


        LLOne(sc21_de_l);
        LLOne(sc21_de_r);

#ifdef SCI22_PRESENT

        LLOne(sc22_de_l);
        LLOne(sc22_de_r);
        LLOne(sc22_l_sc22_r_dt);
#endif

        LLOne(sc41_de_l);
        LLOne(sc41_de_r);

#ifdef SCI41_VERTIC_PRESENT

        LLOne(sc41_de_up);
        LLOne(sc41_de_dn);
#endif

        LLOne(sc42_de_l);
        LLOne(sc42_de_r);
        LLOne(sc42_l_sc42_r_dt);

        LLOne(sc43_de_l);
        LLOne(sc43_de_r);
        LLOne(sc43_l_sc43_r_dt);

#ifdef TOF_41_42_PRESENT

        LLOne(sc41_l_sc42_l_dt);    // tof
        LLOne(sc41_r_sc42_r_dt);    // tof
#endif

#ifdef TOF_41_43_PRESENT

        LLOne(sc41_l_sc43_l_dt);    // tof
        LLOne(sc41_r_sc43_r_dt);    // tof
#endif

#ifdef TOF_21_42__AND_RELATED_AOQ_USED

        LLOne(sc21_l_sc42_l_dt);    // tof
        LLOne(sc21_r_sc42_r_dt);    // tof
#endif

        LLOne(sc21_l_sc21_r_dt);
        LLOne(sc41_l_sc41_r_dt);

        LLOne(sc21_l_sc41_l_dt);
        LLOne(sc21_r_sc41_r_dt);

#ifdef SCI41_VERTIC_PRESENT

        LLOne(sc41_dt_up_dn);
#endif

#ifdef TOF_22_41_PRESENT

        LLOne(sc22_l_sc41_l_dt);
        LLOne(sc22_r_sc41_r_dt);
#endif

#ifdef TOF_22_42_PRESENT

        LLOne(sc22_l_sc42_l_dt);
        LLOne(sc22_r_sc42_r_dt);
#endif

#ifdef TOF_21_22_PRESENT

        LLOne(sc21_l_sc22_l_dt);
        LLOne(sc21_r_sc22_r_dt);
#endif

#ifdef TOF_41_42_PRESENT

        LLOne(sc41_l_sc42_l_dt);
        LLOne(sc41_r_sc42_r_dt);
#endif

        // music

#ifdef MUSICBIG_USED
        // in this case, it was in the other crate, so called "USER", so it is in the other lookup table
        // which is read in the next funciton (below)

#else   // new style musics, "Munich", 8 electrods
        LLOne(music_8_energy[0]);
        LLOne(music_8_energy[1]);
        LLOne(music_8_energy[2]);
        LLOne(music_8_energy[3]);
        LLOne(music_8_energy[4]);
        LLOne(music_8_energy[5]);
        LLOne(music_8_energy[6]);
        LLOne(music_8_energy[7]);

        LLOne(music_8_time[0]);
        LLOne(music_8_time[1]);
        LLOne(music_8_time[2]);
        LLOne(music_8_time[3]);
        LLOne(music_8_time[4]);
        LLOne(music_8_time[5]);
        LLOne(music_8_time[6]);
        LLOne(music_8_time[7]);
        // music
#ifdef MUSIC42_USED

        LLOne(music42_8_energy[0]);
        LLOne(music42_8_energy[1]);
        LLOne(music42_8_energy[2]);
        LLOne(music42_8_energy[3]);
        LLOne(music42_8_energy[4]);
        LLOne(music42_8_energy[5]);
        LLOne(music42_8_energy[6]);
        LLOne(music42_8_energy[7]);

        LLOne(music42_8_time[0]);
        LLOne(music42_8_time[1]);
        LLOne(music42_8_time[2]);
        LLOne(music42_8_time[3]);
        LLOne(music42_8_time[4]);
        LLOne(music42_8_time[5]);
        LLOne(music42_8_time[6]);
        LLOne(music42_8_time[7]);
#endif
#endif // newstyle musics
#ifdef CATE_PRESENT

        // remember - numbering of Cate segments here starts from 0 and goes to 8

        LLOne(cate_detector[0].pin_a);
        LLOne(cate_detector[0].pin_b);
        LLOne(cate_detector[0].Si_energy);
        LLOne(cate_detector[0].pin_c);
        LLOne(cate_detector[0].pin_d);

        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[1].pin_a);
        LLOne(cate_detector[1].pin_b);
        LLOne(cate_detector[1].Si_energy);
        LLOne(cate_detector[1].pin_c);
        LLOne(cate_detector[1].pin_d);

        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[2].pin_a);
        LLOne(cate_detector[2].pin_b);
        LLOne(cate_detector[2].Si_energy);
        LLOne(cate_detector[2].pin_c);
        LLOne(cate_detector[2].pin_d);

        //                                         channel 15 is empty
        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[3].pin_a);
        LLOne(cate_detector[3].pin_b);
        LLOne(cate_detector[3].Si_energy);
        LLOne(cate_detector[3].pin_c);
        LLOne(cate_detector[3].pin_d);

        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[4].pin_a);
        LLOne(cate_detector[4].pin_b);
        LLOne(cate_detector[4].Si_energy);
        LLOne(cate_detector[4].pin_c);
        LLOne(cate_detector[4].pin_d);

        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[5].pin_a);
        LLOne(cate_detector[5].pin_b);
        LLOne(cate_detector[5].Si_energy);
        LLOne(cate_detector[5].pin_c);
        LLOne(cate_detector[5].pin_d);

        // now GEO 15 (slot 15)

        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[6].pin_a);
        LLOne(cate_detector[6].pin_b);
        LLOne(cate_detector[6].Si_energy);
        LLOne(cate_detector[6].pin_c);
        LLOne(cate_detector[6].pin_d);

        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[7].pin_a);
        LLOne(cate_detector[7].pin_b);
        LLOne(cate_detector[7].Si_energy);
        LLOne(cate_detector[7].pin_c);
        LLOne(cate_detector[7].pin_d);

        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[8].pin_a);
        LLOne(cate_detector[8].pin_b);
        LLOne(cate_detector[8].Si_energy);
        LLOne(cate_detector[8].pin_c);
        LLOne(cate_detector[8].pin_d);

        // remember - numbering of Cate segments here starts from 0 and goes to 8
        LLOne(cate_detector[0].CsI_energy);
        LLOne(cate_detector[1].CsI_energy);
        LLOne(cate_detector[2].CsI_energy);
        LLOne(cate_detector[3].CsI_energy);
        LLOne(cate_detector[4].CsI_energy);
        LLOne(cate_detector[5].CsI_energy);
        LLOne(cate_detector[6].CsI_energy);
        LLOne(cate_detector[7].CsI_energy);
        LLOne(cate_detector[8].CsI_energy);

        LLOne(cate_detector[0].Si_time);
        LLOne(cate_detector[1].Si_time);
        LLOne(cate_detector[2].Si_time);
        LLOne(cate_detector[3].Si_time);
        LLOne(cate_detector[4].Si_time);
        LLOne(cate_detector[5].Si_time);
        LLOne(cate_detector[6].Si_time);
        LLOne(cate_detector[7].Si_time);
        LLOne(cate_detector[8].Si_time);

        LLOne(cate_detector[0].CsI_time);
        LLOne(cate_detector[1].CsI_time);
        LLOne(cate_detector[2].CsI_time);
        LLOne(cate_detector[3].CsI_time);
        LLOne(cate_detector[4].CsI_time);
        LLOne(cate_detector[5].CsI_time);
        LLOne(cate_detector[6].CsI_time);
        LLOne(cate_detector[7].CsI_time);
        LLOne(cate_detector[8].CsI_time);

#endif  // CATE_PRESENT
        // FINGER  at target position --------------------
        //  Carl Wheldon told me that:
        // in the place of target, for calibration purposes
        // can be set one Si segment

        // traget scintillator (kind of finger)
        LLOne(target_scintillator_x);
        LLOne(target_scintillator_y);

        //-------------------------------------------------

        //=============== clover =======================
#ifdef HECTOR_PRESENT

        LLOne(hector_to_vme_synchro_signal);
#endif

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

        LLOne(vxi_P_time_minus_Ge_time_OR);
        LLOne(xia_P_time_minus_Ge_time_OR);
#endif

        // #if ( (CURRENT_EXPERIMENT_TYPE != RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN)
        // && (CURRENT_EXPERIMENT_TYPE != RISING_ACTIVE_STOPPER_100TIN ))

#ifdef WE_WANT_FRS_SCALERS

        LLOne(scaler_stopped_beam_trigger);
        LLOne(scaler_frs_singles_trigger);
        LLOne(scaler_time_calibrator_trigger);
        LLOne(scaler_random_trigger);
        LLOne(scaler_fast_beam_trigger);
        LLOne(scaler_time_calibrator_analog_with_TAC);

        LLOne(scaler_frs_free_trigger);
        LLOne(scaler_frs_accepted_trigger);
        LLOne(scaler_frs_IC_old_dig);
        LLOne(scaler_frs_seetram_old_dig);
        LLOne(scaler_frs_seetram_new_dig);
        LLOne(scaler_frs_IC_new_dig);
        LLOne(scaler_frs_spill_counter);
        LLOne(scaler_frs_1Hz_clock);
        LLOne(scaler_frs_10Hz_clock);
        LLOne(scaler_frs_start_extraction);
        LLOne(scaler_frs_stop_extraction);

#endif

#ifdef  WE_WANT_TRIGGER_SCALERS

        // Trigger scalers ------------------------------------------------
        LLOne(scaler_trigger_sc41);
        LLOne(scaler_trigger_OR_triggers);
        LLOne(scaler_trigger_OR_triggers_singles_DGF);
        LLOne(scaler_trigger_OR_triggers_singles_VXI);
        LLOne(scaler_trigger_fast_clear);
        LLOne(scaler_singles_sc41);
        LLOne(scaler_singles_reduced_FRS_singles);
        LLOne(scaler_trigger_time_calibrator);
        LLOne(scaler_trigger_random_trigger);
        LLOne(scaler_trigger_fast_beam_trigger);
        LLOne(scaler_trigger_stopped_beam_trigger);
        LLOne(scaler_trigger_OR_tgammas);

#endif

#if CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005

        LLOne(scaler_frs_sc42_left_or_right);
        LLOne(scaler_frs_sc42_left);
        LLOne(scaler_frs_sc42_right);
        LLOne(scaler_frs_sc43_left);
#endif

#if (CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN)

#ifdef  WE_WANT_FRS_SCALERS

        LLOne(scaler_frs_sc21);
        LLOne(scaler_frs_sc41);
        LLOne(scaler_frs_sc01);
#endif
#endif

#ifdef TIN100_FRS_SCALERS

        LLOne(scaler_frs_free_trigger);
        LLOne(scaler_frs_accepted_trigger);
        LLOne(scaler_frs_spill_counter);
        LLOne(scaler_frs_1Hz_clock);
        LLOne(scaler_frs_10Hz_clock);
        LLOne(scaler_frs_sc01);
        LLOne(scaler_frs_IC_new_dig);
        LLOne(scaler_frs_sc21);
        LLOne(scaler_frs_sc41);
        LLOne(scaler_frs_seetram_old_dig);
        LLOne(scaler_frs_seetram_new_dig);
        LLOne(scaler_frs_IC_old_dig);
        LLOne(scaler_frs_start_extraction);
        LLOne(scaler_frs_stop_extraction);
        LLOne(scaler_sc1u);    // not used
        LLOne(scaler_frs_beam_before_digital_gated);
        LLOne(scaler_frs_100Hz);
        LLOne(scaler_frs_100Hz_outside_dead_time);
        LLOne(scaler_frs_decay_free);
        LLOne(scaler_frs_decay_accepted);
        LLOne(scaler_frs_implantation_free);
        LLOne(scaler_frs_implantation_accepted);
        LLOne(scaler_frs_sc22L);
        LLOne(scaler_frs_sc42L);

        LLOne(scaler_frs_1Mhz_free_running_clock);
        LLOne(scaler_frs_1Mhz_gated_with_spill_signal);
        LLOne(scaler_frs_1Mhz_gated_with_spill_signal_and_DAQ_busy);
        LLOne(scaler_frs_free_trigger_250us_to_gate_out_noise);

#endif


#endif   // def RISING_GERMANIUMS_PRESENT


        plik.close();
    } catch(Tno_keyword_exception &m) {
        cout << "Error while reading the lookup table file " << fname << "\n\t"
             << m.message << endl;
        exit(1);

    } catch(Treading_value_exception &m) {
        cout << "Error while reading the lookup table file " << fname << "\n\t"
             << m.message << endl;
        exit(2);
    }
    plik.close();
}
//************************************************************************************




//******************************************************************************************************
void TIFJEventProcessor::read_frsUser_lookup_table()
{
    frsUser_lookup_should_be_loaded = false;
    frsUser_lookup_table.clear();

    // NOTE: this is the same file for crate ONE and crate USER
    // but in the file we give only slot/channel  but not the crate nr...

    string fname = "./mbs_settings/frs_lookup_table.txt";
    ifstream plik(fname.c_str());

    if(!plik) {
        cout << "file " << fname << " does not exist." << endl;
        // some other more drastic actions
        return;
    }

    try {

        // Reading from the file the data where from the particular signal will be comming.
        // it is put to the vector, and then, every event the loop over this vector
        // will do the unpacking.


        // without Macrodefiniction should be:
        // frs_lookup_table.push_back(frs_signal(plik, "mw11_x_right", &(target_event->mw11_x_right)));

        // this macrodefiiction uses the operator # to "stringify" a name of an argument
#define LLUser(wstaw)      frsUser_lookup_table.push_back(frs_signal(plik, #wstaw, &(target_event->wstaw)));

#ifdef MUSICBIG_USED
        // these musics are in the second VME crate
        LLUser(musicBig41_energy[0]);
        LLUser(musicBig41_energy[1]);
        LLUser(musicBig41_energy[2]);
        LLUser(musicBig41_energy[3]);

        LLUser(musicBig41_time[0]);
        LLUser(musicBig41_time[1]);
        LLUser(musicBig41_time[2]);
        LLUser(musicBig41_time[3]);
        // music

#ifdef MUSIC42_USED

        LLUser(musicBig42_energy[0]);
        LLUser(musicBig42_energy[1]);
        LLUser(musicBig42_energy[2]);
        LLUser(musicBig42_energy[3]);

        LLUser(musicBig42_time[0]);
        LLUser(musicBig42_time[1]);
        LLUser(musicBig42_time[2]);
        LLUser(musicBig42_time[3]);
#endif // MUSIC42_USED
#endif

#ifdef TPC21_PRESENT

        LLUser(tpc21_drift_time_array_ptr[0]);
        LLUser(tpc21_drift_time_array_ptr[1]);
        LLUser(tpc21_drift_time_array_ptr[2]);
        LLUser(tpc21_drift_time_array_ptr[3]);

        LLUser(tpc21_left_time_array_ptr[0]);
        LLUser(tpc21_left_time_array_ptr[1]);

        LLUser(tpc21_right_time_array_ptr[0]);
        LLUser(tpc21_right_time_array_ptr[1]);

        LLUser(tpc21_drift_energy_array_ptr[0]);
        LLUser(tpc21_drift_energy_array_ptr[1]);
        LLUser(tpc21_drift_energy_array_ptr[2]);
        LLUser(tpc21_drift_energy_array_ptr[3]);

        LLUser(tpc21_left_energy_array_ptr[0]);
        LLUser(tpc21_left_energy_array_ptr[1]);

        LLUser(tpc21_right_energy_array_ptr[0]);
        LLUser(tpc21_right_energy_array_ptr[1]);

#endif

#ifdef TPC22_PRESENT

        LLUser(tpc22_drift_time_array_ptr[0]);
        LLUser(tpc22_drift_time_array_ptr[1]);
        LLUser(tpc22_drift_time_array_ptr[2]);
        LLUser(tpc22_drift_time_array_ptr[3]);

        LLUser(tpc22_left_time_array_ptr[0]);
        LLUser(tpc22_left_time_array_ptr[1]);

        LLUser(tpc22_right_time_array_ptr[0]);
        LLUser(tpc22_right_time_array_ptr[1]);

        LLUser(tpc22_drift_energy_array_ptr[0]);
        LLUser(tpc22_drift_energy_array_ptr[1]);
        LLUser(tpc22_drift_energy_array_ptr[2]);
        LLUser(tpc22_drift_energy_array_ptr[3]);

        LLUser(tpc22_left_energy_array_ptr[0]);
        LLUser(tpc22_left_energy_array_ptr[1]);

        LLUser(tpc22_right_energy_array_ptr[0]);
        LLUser(tpc22_right_energy_array_ptr[1]);

#endif

        //------------
#ifdef TPC41_PRESENT

        LLUser(tpc41_drift_time_array_ptr[0]);
        LLUser(tpc41_drift_time_array_ptr[1]);
        LLUser(tpc41_drift_time_array_ptr[2]);
        LLUser(tpc41_drift_time_array_ptr[3]);

        LLUser(tpc41_left_time_array_ptr[0]);
        LLUser(tpc41_left_time_array_ptr[1]);

        LLUser(tpc41_right_time_array_ptr[0]);
        LLUser(tpc41_right_time_array_ptr[1]);

        LLUser(tpc41_drift_energy_array_ptr[0]);
        LLUser(tpc41_drift_energy_array_ptr[1]);
        LLUser(tpc41_drift_energy_array_ptr[2]);
        LLUser(tpc41_drift_energy_array_ptr[3]);

        LLUser(tpc41_left_energy_array_ptr[0]);
        LLUser(tpc41_left_energy_array_ptr[1]);

        LLUser(tpc41_right_energy_array_ptr[0]);
        LLUser(tpc41_right_energy_array_ptr[1]);

#endif

#ifdef TPC42_PRESENT

        LLUser(tpc42_drift_time_array_ptr[0]);
        LLUser(tpc42_drift_time_array_ptr[1]);
        LLUser(tpc42_drift_time_array_ptr[2]);
        LLUser(tpc42_drift_time_array_ptr[3]);

        LLUser(tpc42_left_time_array_ptr[0]);
        LLUser(tpc42_left_time_array_ptr[1]);

        LLUser(tpc42_right_time_array_ptr[0]);
        LLUser(tpc42_right_time_array_ptr[1]);

        LLUser(tpc42_drift_energy_array_ptr[0]);
        LLUser(tpc42_drift_energy_array_ptr[1]);
        LLUser(tpc42_drift_energy_array_ptr[2]);
        LLUser(tpc42_drift_energy_array_ptr[3]);

        LLUser(tpc42_left_energy_array_ptr[0]);
        LLUser(tpc42_left_energy_array_ptr[1]);

        LLUser(tpc42_right_energy_array_ptr[0]);
        LLUser(tpc42_right_energy_array_ptr[1]);

#endif

#ifdef MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE
#ifdef MUN_ACTIVE_STOPPER_PRESENT
        // nothing, because it is hardcoded in ---> Tmusic_8anodes::make_preloop_action
#else

        LLUser(atmospheric_pressure_raw);
#endif
#endif

        plik.close();
    } catch(Tno_keyword_exception &m) {
        cout << "Error while reading the lookup table file " << fname << "\n\t"
             << m.message << endl;
        exit(1);

    } catch(Treading_value_exception &m) {
        cout << "Error while reading the lookup table file " << fname << "\n\t"
             << m.message << endl;
        exit(2);
    }
    plik.close();
}

#ifdef NIGDY
#endif  // NIGDY


//************************************************************************************
/** This is the function which reads new style of event - characteristic
 to the CCB. There is no timestamp matching, everything
 comes together as one event (with subevent parts) */
bool TIFJEventProcessor::unpack_the_CCB_event()
{
    //    cout << "===== function :unpack_the_CCB_event() " << counter_put << endl;

    static time_t last = 0;
    time_t now = time(nullptr);

    if(now - last >= 10) {
        if(now - last < 9999) {
            //      cout <<  "During last " << (now-last)
            //           << " seconds: GER subevents with Pulsers= " << ger_pulser
            //           << ", GER subevents whit 'Physics' with data = " << ger_physics
            //           << ", GER subevents whit 'Physics' empty= " << ger_physics_empty
            //             << endl;
            output_statistics_events(); // ---------------------
            //cout_statistics_events(cout);
        }
        ger_pulser = ger_physics = ger_physics_empty = 0;
        last = now;
    }

    // ======= Real work =======================================================
    // TjurekMbsSubEvent* ise;

    bool unpacked_successfuly = true;
    bool flag_was_any_frs = false;
    flag_vmeOne_zeroing_needed = flag_vmeUser_zeroing_needed = true;

    //################## unpacking subevents does not exist in "exotic"

    //       cout << "This is a frs_part to unpack " << endl;


    if(
        #if CURRENT_EXPERIMENT_TYPE==IFJ_KRATTA_HECTOR_EXPERIMENT
            ! unpack_kratta_hector_event(input_event_data, how_many_data_words)
        #else
        #error "Other experiments not listed here "
        #endif

            ) {
        unpacked_successfuly = false;
    } else {
        //    cout << "\n\n" << endl;
        flag_was_any_frs = true;
        frs_subevents++;
    }

    if(flag_was_any_frs)
        distribute_frs_vme_words_to_event_members(target_event);
    return unpacked_successfuly;

}
//*******************************************************************************
//************************************************************************************
//****************************************************************************************
//****************************************************************************************
//***************************************************************************************

//***************************************************************************************
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
/** No descriptions */
bool TIFJEventProcessor::unpack_multi_hit_tdc_v1290_from_raw_array(
        const_daq_word_t *data, int how_many_words)
{
    //   cout << " F.unpack_multi_hit_tdc_v1290_from_raw_array" << endl;
    /*
     cout << "To unpack there is " << dec << how_many_words << " words \n";

     for(int i = 0 ; i < how_many_words ; i++)
     {
     cout << "[" << i << "]  0x" << hex <<  data[i]
     //<< ",  dec = "
     <<  dec
     //<<  data[i]
     << endl;
     }
     cout << "End of buffer, lets try tu unpack this \n";
     */

    //   cout << "Unpacking the scalers report" << endl;
    /* Plamen wants this.
     According to the list, the word nmr 3 has 1Hz clock, so by its change we write the report on the disk.
     */
    static unsigned long last_1Hz_clock;

    if(data[3] != last_1Hz_clock) {
        last_1Hz_clock = data[3];
        ofstream plik("scalers_current_report.txt");
        for(int i = 0; i < how_many_words; i++) {
            plik
                    // << "[" << i << "]     "
                    << data[i] << endl;
        }
        plik.close();

    }

    /*
     if(word.header_word.geo == 10)  // was 10 for tin100 ???????????????????????????
     {

     target_event->scaler_free_trigger       = data[i+1]; // 1. Free (trigger box)
     target_event->scaler_accepted_trigger  = data[i+2];// 2. Active (last voie)
     target_event->scaler_spill_counter  = data[i+3];// 3. Spill counter
     target_event->scaler_100Hz_clock  = data[i+4]; // 4. 100 Hz free
     target_event->scaler_10Hz_clock = data[i+5]; // 5. 10 Hz free
     target_event->scaler_sc01 = data[i+6];  // 6. sc0
     target_event->scaler_sc42_left = data[i+7];// 7. sc42L
     target_event->scaler_sc21_right = data[i+8]; // 8. sc21T
     target_event->scaler_sc41_right = data[i+9]; // 9. sc41T
     target_event->scaler_seetram_old_dig = data[i+10]; // 10. seetram old digit
     target_event->scaler_seetram_new_dig = data[i+11];// 11. seetram new digit
     target_event->scaler_100_Hz_dead_time_veto = data[i+12];// 12. 100 Hz dead time veto
     target_event->scaler_start_extraction  = data[i+13]; // 13. Start extraction
     target_event->scaler_stop_extraction = data[i+14]; // 14. Stop extraction
     target_event->scaler_pile_up_beam_in_GFLT  = data[i+15];// 15. Pille up beam in GFLT
     target_event->scaler_pile_up_decay_in_GFLT  = data[i+16];   // 16. Pille up decay in GFLT
     target_event->scaler_implant_free  = data[i+17]; // 17. Implant free
     target_event->scaler_decay_free = data[i+18];// 18. Decay free
     target_event->scaler_implant_accepted  = data[i+19];// 19. Implant accepted
     target_event->scaler_decay_accepted  = data[i+20];// 20. Decay accepted

     // we jump over them in the outer loop, they are processed
     i += word.header_word.cnt;
     }

     */

#define SPEAK_VTIMING 0
#define DBGV  if(SPEAK_VTIMING)

    /////////////////////////////////////////
    class v1290_global_header_word
    {
    public:
        unsigned int geo :
            5;
        unsigned int evnet_nr :
            22;
        unsigned int code :
            5;
    };
    /////////////////////////////////////////
    class v1290_data_word
    {
    public:
        unsigned int time_data :
            21;
        unsigned int channel :
            5;
        unsigned int trailing_otherwise_leading_measurement :
            1;
        unsigned int code :
            5;
    };
    /////////////////////////////////////////
    class v1290_global_trailer_word
    {
    public:
        unsigned int geo :
            5;
        unsigned int word_count :
            16;
        unsigned int :
            3;
        //    unsigned int status_trigger_lost        : 1;
        //    unsigned int status_tdc_error        : 1;
        //    unsigned int status_output_buffer_overflow        : 1;
        unsigned int status_info :
            3; // instead of those 3 above
        unsigned int code :
            5;
    };
    /////////////////////////////////////////
    /////////////////////////////////////////
    union { //v1290_word
        daq_word_t raw_word;
        v1290_global_header_word header_word;
        v1290_data_word data_word;
        v1290_global_trailer_word trailer_word;
    };

    int geo = 0;

    // first are 2*32 scalers, plus header words
    for(int i = 68; i < how_many_words; i++) {
        DBGV
                cout << "Unpacking word " << i << ": 0x" << hex << data[i] << dec
                     << endl;
        raw_word = data[i];
        if(!raw_word) { // do not analyse empty words
            continue;
        }

        switch(header_word.code) { // <-- it is common to all of types of vme words
        case 0x8: {
            geo = header_word.geo;
            DBGV
                    cout << "Caen V1290 Global Header, containing geo =" << geo
                         << endl;
        }
            break;

        case 0: // Data word
            if(!geo)
                break; // without Global Header - we do not unpack

            if(geo < 22 /*was 22*/ && data_word.channel < 32) {

                DBGV
                        cout << "Arrived V1290 TIME   geo = " << geo << ", chan = "
                             << data_word.channel << ", data = "
                             << data_word.time_data << ", 'trailing_edge' = "
                             << data_word.trailing_otherwise_leading_measurement
                             << endl;

#ifdef MULTIHIT_V1290_PRESENT

                if(geo == 12) {
                    target_event->multihit_v1290[data_word.channel] = data_word.time_data >> 4;
                }
#endif
#ifdef NIGDY

                int clus = 0;
                int crys = 0;
                if(lookup_SR_times.current_combination(word.word775.data_word.geo,
                                                       word.word775.data_word.channel,
                                                       &clus,
                                                       &crys)) {

                    DBGV cout << " --- this is time for cluster "
                                 //          << clus
                                 //          << " crystal " << crys
                                 //          << " which is "
                              << Trising::cluster_characters[clus] << "_"
                              << char('1' + crys) << endl;

                    // storing the time data
                    // no need to we reduce the  resolution from 64 KB to 8KB !!!
                    target_event->ge_time_SR[clus][crys] = word.word775.data_word.data;
                    target_event->ger_xia_dgf_fired[clus][crys] = true;
                } else {
                    static int licznik;
                    if(!((licznik++) % 20000))
                        cout << "DGF unpacking. Warning: combination GEO = "
                             << word.word775.data_word.geo
                             << ", CHANNEL = " << word.word775.data_word.channel
                             << " was not specified in a lookup table Cluster_SR_time_xia"
                             << endl;

                }
#endif

            }
            break;

        case 0x10:
            if(!geo)
                break; // without Global Header - we do not unpack

            DBGV
                    cout << "Global trailer   when i = .." << i << endl;
        {
            // local scope
            static int how_many_errors;
            if(trailer_word.status_info) {
                how_many_errors++;
                static time_t last_message = time(NULL);
                if((time(NULL) - last_message > 5) && (how_many_errors
                                                       > 500)) { // every 5 seconds
                    last_message = time(NULL);
                    cout << "Error status in Caen v1290 block  " << endl;

                } // every 5 seconds
                return false;

            } else { // no error in status
                how_many_errors = 0;
            }
        } // locaal

            break;
        case 0x11:
            if(!geo)
                break; // without Global Header - we do not unpack

            DBGV
                    cout << "Global Trigger Time Tag  " << i << endl;
            //return false;
            break;

        default:
            if(!geo)
                break; // without Global Header - we do not unpack

            DBGV
                    cout << "Error - unknown code " << " in V1290    word nr = "
                         << i << endl;
            break;
        } // endof inner switch

    } // end of for i


    return true; // if successful
}

//****************************************************************************************

void *TIFJEventProcessor::GetInputEvent()  // virtual f. from the abstract class
{
    cout
            << "TIFJEventProcessor::GetInputEvent()  Getting the input event from the file, or from the data stream and "
               "returning the pointer to the event (EMPTY yet!!!!)" << endl;

    return NULL;
}
//****************************************************************************************
void TIFJEventProcessor::unpack_trigger_pattern(int /*event*/, int /*subevent*/, uint32_t *data, int /*length*/)
{
    //     cout << __func__ << endl;
    //output ( event, subevent, ( uint32_t* ) data, length );

    return;

    //if ( ( i_setyp==36 ) && ( i_sestyp == 2800 ) )

    int Trig_Patt = -10;
    Trig_Patt = data[0] ;//-> Data(0);
    cout << "Trigger pattern: " << Trig_Patt << endl;
    //cout << "Trigger pattern(oct): " << oct << Trig_Patt << ")" << endl;
    cout << "Trigger pattern (bin): ";
    if(Trig_Patt & 0x0001) cout << "1";
    else cout << "0";//1
    if(Trig_Patt & 0x0002) cout << "1";
    else cout << "0";//2
    if(Trig_Patt & 0x0004) cout << "1";
    else cout << "0";//4
    if(Trig_Patt & 0x0008) cout << "1";
    else cout << "0";//8
    if(Trig_Patt & 0x0010) cout << "1";
    else cout << "0";//16
    if(Trig_Patt & 0x0020) cout << "1";
    else cout << "0";//32
    if(Trig_Patt & 0x0040) cout << "1";
    else cout << "0";//64
    if(Trig_Patt & 0x0080) cout << "1";
    else cout << "0";//128
    if(Trig_Patt & 0x0100) cout << "1";
    else cout << "0";//256
    if(Trig_Patt & 0x0200) cout << "1";
    else cout << "0";//512 	<-
    if(Trig_Patt & 0x0400) cout << "1";
    else cout << "0";//1024	<-
    if(Trig_Patt & 0x0800) cout << "1";
    else cout << "0";//2048
    if(Trig_Patt & 0x1000) cout << "1";
    else cout << "0";
    if(Trig_Patt & 0x2000) cout << "1";
    else cout << "0";
    if(Trig_Patt & 0x4000) cout << "1";
    else cout << "0";
    if(Trig_Patt & 0x8000) cout << "1";
    else cout << "0";
    cout << endl;

}
//*****************************************************************************
// for a combination  (i_setyp == 36 && i_sestyp==1) - it is CAEN V1724
void TIFJEventProcessor::unpack_caen_v1724_kratta_digitizer(int /*event*/, int /*subevent*/, uint32_t *p_se, int length)
{
    // cout << "\n=============================\n"
    //      << __func__  << ",  subevent length = "<< length << endl;

    //constexpr int how_many_chan_for_pedestal = 30; // 50;

    if(length <= 0) {
        // cout << "Empty Event v1724!  " << endl;
        return;
    }

    // decoding  V1724 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1

    // for debugging purposes we want to see some words
    //    for(int i = 0 ; i < 160 ; ++i)
    //    {
    //        cout << "p_se[" << i << "] -> " << hex << *(p_se+i)
    //             << dec << ", dec= " << *(p_se+i) << endl;
    //        if(  (*(p_se+i) & 0xf0000000) == 0xa0000000) break;
    //    }

#ifdef NEW_WITHOUT_HECTOR_HERE
    if((p_se[0] >> 28)   == 0xa) { // new, mixed data
        //            cout << "Old only Kratta data" << endl;
    } else {
        //            cout << "New Kratta + Hector data" << endl;
        // analizuj hectora

        unpack_hector(p_se, true, length);   // true hector data, (not phoswich)

        // first word says how many longwords to skip to jump into kratta data
        // przeskocz od kratty
        int skip_over_hector = p_se[0];

        if(skip_over_hector) {
            //                cout << "JEST PRAWDZIWY PRZESKOK " << skip_over_hector << " slow "  << endl;
        } else {
            //                cout << "Nie ma danych hectora  " << endl;
        }
        d f g
                p_se += skip_over_hector + 1   ;
        length -= skip_over_hector     ;
        //        cout << "skipping " << skip_over_hector +1  +2 << " words " << endl;
    }
#endif

    if(length <= 1)       return; // no kratta data
    if(length > 15000)  // probably nonsense
        return;

    //#######################################
    // #if VISITING_CARD == 1



    // przeskok na razie jednej visiting card

    // int skip_vc_in_words = (sizeof(visitcard) - sizeof(uint32_t)) / sizeof(uint32_t);
    int nr_of_analysed_words = 0 ;

    for(int nr_visiting = 0 ; nr_of_analysed_words < length; ++nr_visiting)
        //for(int nr_visiting = 0 ; nr_visiting < 1; ++nr_visiting)
    {
        // cout << "Visitcard nr " << nr_visiting << endl;
        if(nr_visiting > 0)
        {
            //                    cout << "nr_of_analysed_words = "
            //                         << nr_of_analysed_words
            //                         << ", length = " << length
            //                         << endl;
            //exit(0);
        }

        Tvisitcard * visit_ptr = ( Tvisitcard *)(p_se + nr_of_analysed_words);

        uint32_t how_many_words = visit_ptr->size;

        //        cout << "Info from visitcard, Type = " << visit_ptr->type
        //              << ", name = " << visit_ptr->name << endl;



        //                cout << "Size of this visit block = "
        //                     << how_many_words
        //                     << ", after skipping visit= " << how_many_words - skip_vc_in_words
        //                     << endl;


        // cout << "Testing  NOWY_PIOTR linia " << __LINE__ << endl;
        if(visit_ptr->type == string("V1724"))
        {
            //cout << "Wykryta wizytowka digitizera V1724, " << visit_ptr->name<< endl;


            d1724.Load(&visit_ptr->data);
            //            d1724.Dump();

            /*
            uint8_t marker;
            uint32_t size;
            uint8_t channel_mask;
            uint16_t pattern;
            uint8_t  board_id;
        //    uint8_t  compression;
            uint32_t counter;
            uint32_t time;
            uint16_t * data;
            int buf_size;
            uint16_t * channel[8];

*/

            //            struct Tdigitizer
            //            {
            //                unsigned int  event_size : 28;
            //                unsigned int  header_code : 4;
            //                unsigned int  channel_mask_0_7 : 8;
            //                unsigned int  pattern_trig_opt : 16;
            //                unsigned int   : 2;
            //                unsigned int  bf : 1;
            //                unsigned int  board_id_geo : 5;
            //                unsigned int  event_counter : 24;
            //                unsigned int  channel_mask_8_15 : 8;
            //                unsigned int  trigger_time_tag : 32;
            //            };

            //            Tdigitizer * dig_ptr =  (Tdigitizer *) &visit_ptr->data;

            //            cout << "Digitizer 1724 header data: "
            //                 << ", event_size = " <<  dig_ptr->event_size
            //                 << ", header_code  [10] = " <<  dig_ptr->header_code
            //                 << "\nchannel_mask_0_7 0x" << hex << dig_ptr->channel_mask_0_7
            //                 << " channel_mask_8_15 0x" << hex << dig_ptr->channel_mask_8_15
            //                 << "\npattern_trig_opt " << hex << dig_ptr->pattern_trig_opt
            //                 << " board_id_geo " << hex << dig_ptr->board_id_geo
            //                 << " event_counter " << dec << dig_ptr->event_counter
            //                 << " trigger_time_tag " << hex << dig_ptr->trigger_time_tag
            //                 << " " << dec << endl;


            //int fadc_board = visit_ptr->name[4] - '0';   // should be fadcN (mod = 0-8)
            string from_where = visit_ptr->name;
            if(from_where.substr(0, 4) == "fadc")
            {
                //#define KRATTA_DATA_OTHERWISE_PLASTIC  true

                //#if KRATTA_DATA_OTHERWISE_PLASTIC


                auto current_board = ltb_kratta->FindBoard(visit_ptr->name);

                for(int ch = 0 ; ch < d1724.GetNchan() ; ++ch)
                {
                    if(d1724.Empty(ch))
                        continue;

                    for(int s = 0 ; s < 3; ++s)   // s is  0 = time, 1 -> amplitude  2-> pedestal
                    {
                        auto wart = d1724.GetData(ch, s);
                        if(wart != 0){
                            //                            cout << "Digitizer Channel " << ch
                            //                                 << ", signal " << s << " ==> value " << wart
                            //                                    //<< ", current_board " << current_board
                            //                                 << endl;


                            // NOT USED ANYMORE
                            // distributing to the unpacked event
                            //                        target_event->digitizer_data[(16*ch) + s] = wart / 4;       // int32 array, (for BaF)
                            //                        target_event->digitizer_int16data[(16*ch) + s] = wart / 4;  // short int array (for Tone_signal)
                            // because 0-15 is time, 16-31 qshort (fast), 32-48 qlong(slow)

                            if(ch > (current_board.Size()-1) )
                            {
                                current_board.Print() ;
                                cout << "ERROR: (A) TOO BIG INDEX ch = " << ch
                                     << ",  while the possible (for this board) in LUT is: [0 till "
                                     << current_board.Size()-1   << "]"
                                     << endl;
                            }

                            auto fch = current_board.FindChannel(ch);
                            if(fch.Size() != 1)
                            {
                                cout << "\nERROR: In lookup table for kratta there is no proper entry for " << visit_ptr->name
                                     << ", channel = " << ch << endl;
                                exit(88);

                            }
                            int id_kratty = fch[0]->GetID();
                            //int id_kratty = current_board[id_indeks]->GetID();

                            //int id_kratty = current_board[ch]->GetID();

                            // for ex. "pd1.time" we take the digit which must be here
                            int pd_kratty = fch[0]->GetVariable(0) [2] - '0';
                            //int pd_kratty = current_board[ch]->GetVariable(0) [2] - '0';


                            /*                       // it must be kratta, because only this data we loaded into LUT
                       string typ_urzadzenia = current_board[ch]->GetBranch();

                       cout << "LOOKUP_PIOTRA: "
                               "Board = " <<  visit_ptr->name << " / " << ch
                            << " --> typ_urzadzenia = " << typ_urzadzenia
                            << ", id  = " << id_kratty
                            << ", pd = " << pd_kratty
                            << endl;
*/

                            int to_which = (pd_kratty*3 + s);

                            //                        if(id_kratty == 0)
                            //                        {

                            //                            cout << "Kratta_" << id_kratty
                            //                                 << " pd" << pd_kratty
                            //                                 << ", signal type  s = "
                            //                                 << s
                            //                                 << " [0]->time, [1]->ampl, [2]->pedestal, value = "
                            //                                 << wart << ", so to_which matrix element= " << to_which
                            //                                 << endl;
                            //                        }

                            //wart = 100 * fadc_board + 10 * ch  + s;   // <-- fake for testing
                            target_event->kratta[id_kratty][to_which]  = wart; // energy value, pedestal, time;
                        }
                    }
                }
            } // endif "fadc"
            //#else   // KRATTA_DATA_OTHERWISE_PLASTIC
            else {

                auto current_board = ltb_plastic->FindBoard(visit_ptr->name);
                for(int ch = 0 ; ch < d1724.GetNchan() ; ++ch)
                {
                    if(d1724.Empty(ch))
                        continue;

                    auto wart = d1724.GetData(ch, 0);
                    if(wart != 0)
                    {
                        //                        cout << "Digitizer Channel " << ch
                        //                             << ", signal " << s << " ==> value " << wart << endl;

                        if(current_board.Size() == 0)
                        {
                            cout << "For signals coming from d1724 - the plastic lookup table is empty "
                                 << endl;
                            continue;
                        }
                        if(current_board[ch] == 0)
                        {
                            cout << "For d1724, Channel " << ch
                                 << " is not mentioned in a plastic lookup table " << endl;
                            continue;
                        }
                        int id_plastic = current_board[ch]->GetID();
                        cout << "v775 Kanal " << ch << " wartosc " << wart << endl;
                        // distributing to the event

                        cout << "Plastic data. id_plastic " << id_plastic << ", value = " << wart << endl;
                        target_event->plastic[id_plastic] = wart;     // time goes to 0. If in future we have energy, it will go to 1

                    }

                }
            } // endif else "fadc"
            //#endif

        }  // end wizytowki V1724

        else if(visit_ptr->type == string("V830"))
        {
            unpack_V830(visit_ptr);
            nr_of_analysed_words += how_many_words;
            continue;

        }
        else if(visit_ptr->type == string("V775"))
        {

            unpack_V775(visit_ptr);

            nr_of_analysed_words += how_many_words;   // UWAGA!!!!!1
            continue;

        }
        else if(visit_ptr->type == string("V879") || visit_ptr->type == string("V878"))
        {

            unpack_V879_878(visit_ptr);

            nr_of_analysed_words += how_many_words;
            continue;
        }
        else if(visit_ptr->type == string("V785"))
        {
            unpack_V785(visit_ptr);

            nr_of_analysed_words += how_many_words;
            continue;

        }

        else if(visit_ptr->type == string("V1190"))
        {
            // cout << "raz" << endl;
            unpack_V1190(visit_ptr);

            nr_of_analysed_words += how_many_words;
            continue;

        }
        else if (visit_ptr->type == string("RATE"))
        {
            // ignored visitin card "RATE"
        }
        else {
            static int licznik = 0 ;
            if(!(++licznik % 1000 ))
            {

                cout << __func__ << " Warning: Unexpected visiting card "
                     << visit_ptr->type << "--->   , origin: " << visit_ptr->name

                        // << " line:" << __LINE__
                     << ")"  << endl;
            }
        }



        nr_of_analysed_words += how_many_words;
    }// end for nr_of_visiting_cards
}
//*****************************************************************************

void TIFJEventProcessor::unpack_36_2800(int /*event*/, int /*subevent*/, uint32_t */*data*/, int /*length*/)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_10_1(int /*event*/, int /*subevent*/, uint32_t */*data*/, int /*length*/)
{
    cout << __func__ << " do tej pory nieużywana funkcja "<< endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_34_1(int /*event*/, int /*subevent*/, uint32_t */*data*/, int /*length*/)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_36_2700(int /*event*/, int /*subevent*/, uint32_t */*data*/, int /*length*/)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_32_1130(int /*event*/, int /*subevent*/, uint32_t */*data*/, int /*length*/)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_36_9494(int /*event*/, int /*subevent*/, uint32_t */*data*/, int /*length*/)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************
void TIFJEventProcessor::unpack_ccb_non_kratta_z_metryczkami(uint32_t *data_block, bool real_hector, int length)
{
    // cout << __func__ << (real_hector ? ": HECTOR" : ": Phoswich") << ",  length = " << length << endl;

    //#define VISITING_CARD 1    // 0 - no, old style;  1 - przskok, moje rozkowodanie,





    //    struct Tvisitcard
    //    {
    //        char type[8];           // type of VME board
    //        char name[8];           // specific name of the board
    //        uint32_t hwaddr;        // VME address of the board
    //        uint32_t size;          // size of the data block, starting from type
    //        uint32_t data;          // Should be always last!
    //    };


    // przeskok na razie jednej visiting card

    if(length > 15000)  // probably nonsense block of data
        return;

    int nr_of_analysed_words = 0 ;

    //int skip_vc_in_words = (sizeof(visitcard) - sizeof(uint32_t)) / sizeof(uint32_t);

    for(int nr_visiting = 0 ; nr_of_analysed_words < length; ++nr_visiting)
    {
        // cout << "Visitcard nr " << nr_visiting << endl;
        //        if(nr_visiting > 0)
        //        {
        //            cout << "nr_of_analysed_words = "
        //                 << nr_of_analysed_words
        //                 << ", length = " << length
        //                 << endl;
        //            //exit(0);
        //        }

        Tvisitcard * visit_ptr = ( Tvisitcard *)(data_block + nr_of_analysed_words);
        uint32_t how_many_words = visit_ptr->size;

        // cout << "Info from visitcard, Type = " << visit_ptr->type
        //      << ", name = " << visit_ptr->name
        //      << ",  how_many_words = " << how_many_words
        //      << endl;

        //                cout << "Size of this visit block = "
        //                     << how_many_words
        //                     << ", after skipping visit= " << how_many_words - skip_vc_in_words
        //                     << endl;

        //-----------------------------------------
        // Recognising what kind of data it is
        //-----------------------------------------

        if(visit_ptr->type == string("V1730B"))   // digitizer dla non-kratta signals
        {
            // cout << "Wykryte dane z digitizera V1730B" << endl;

            //                        struct Tdigitizer
            //                        {
            //                            unsigned int  event_size : 28;
            //                            unsigned int  header_code : 4;
            //                            unsigned int  channel_mask_0_7 : 8;
            //                            unsigned int  pattern_trig_opt : 16;
            //                            unsigned int   : 2;
            //                            unsigned int  bf : 1;
            //                            unsigned int  board_id_geo : 5;
            //                            unsigned int  event_counter : 24;
            //                            unsigned int  channel_mask_8_15 : 8;
            //                            unsigned int  trigger_time_tag : 32;
            //                        };

            //                        Tdigitizer * dig_ptr =  (Tdigitizer *) &visit_ptr->data;

            //                                    cout << "Digitizer header data: "
            //                                         << ", event_size = " <<  dig_ptr->event_size
            //                                         << ", header_code  [10] = " <<  dig_ptr->header_code
            //                                         << "\nchannel_mask_0_7 0x" << hex << dig_ptr->channel_mask_0_7
            //                                         << " channel_mask_8_15 0x" << hex << dig_ptr->channel_mask_8_15
            //                                         << "\npattern_trig_opt " << hex << dig_ptr->pattern_trig_opt
            //                                         << " board_id_geo " << hex << dig_ptr->board_id_geo
            //                                         << " event_counter " << dec << dig_ptr->event_counter
            //                                         << " trigger_time_tag " << hex << dig_ptr->trigger_time_tag
            //                                         << " " << endl;


            d.Load(&visit_ptr->data);
            //d.Dump();

            // There are two digitzers everyone gives part of data
            int nr_dig = 0;
            switch(visit_ptr->name[3])        //  "dig0" OR "dig1" ?
            {
            case '0':nr_dig = 0;
                break;
            case '1':nr_dig = 1;
                break;
            case '2':
                nr_dig = 2;
                break;
            case '3':
                nr_dig = 3;
                break;

            default:
            {
                static int counter = 0 ;
                if( (++counter % 1000) == 0 )
                {
                    cout << "error in title name of visiting card ["
                         << visit_ptr->name
                         << "], while max number should not be higher than " << (NR_DIGITIZERS-1)
                         << ". \nYou may change this parameter in experiment_def.h and recompile the spy program" << endl;
                    //                exit(100);
                }
            }

            }

            // We expect that in this block there is a data for Paris (from digitgiser)

            // cout << "Name of visiting card is " << visit_ptr->name << endl;
#define PARIS_NORMALY__OTHERWISE_PLASTIC true
            // sometimes they send plastics by this block of data
            // it is an exceptional situation. Then set false here

#if PARIS_NORMALY__OTHERWISE_PLASTIC

            // so this is paris

            auto current_board = ltb_paris->FindBoard(visit_ptr->name);
            // what if the Find was not succesful?

            double time_in_channel_0 = 0;


            if(current_board.Size() == 0)
            {
                static int licznik = 0;

                if((++licznik) %5000 == 0)
                {
                    cout << "Warning: For signals coming from digitizer  "
                         <<  visit_ptr->name
                          << " - the lookup table has no entries"
                          << endl;
                }
                //continue;
            } else
                for(int n = 0 ; n < d.GetNchan() ; ++n)
                {
                    if(d.Empty(n))
                        continue;

                    auto branch = current_board.FindChannel(n);
                    static int licznik;
                    if(branch.Size() != 1)
                    {
                        if( (++licznik % 5000) == 0 )
                            cout << "[B] For block v775, " << visit_ptr->name << ", a channel "
                                 << n
                                 << " is "<< (branch.Size() )
                                 << " times mentioned in a the lookup table "
                                 << ltb_paris->name
                                 << endl;

                        continue;
                    }

                    //
                    //czy to paris / labr / ref ==========================================================
                    // in case of "ref" it is only just the reference time to be subtracted
                    // later form other channels of time.

                    string branch_name = current_board.FindChannel(n)[0]->GetBranch();
                    // cout << "nazwa branchu = " << branch_name << endl;

                    bool flag_reference_time = false;
                    if( branch_name == string("ref"))
                        flag_reference_time = true;



                    int id_parysa  = current_board.FindChannel(n)[0]->GetID();
                    if(branch_name == "labr")
                    {
                        id_parysa += 28;
                    }

                    // Piotr says, that in case of "ref" the other s (fast, slow) are empty, so no harm
                    for(int s = 0 ; s < 3; ++s) // we need 3 first data signals (time, fast(short), slow(long)
                    {
                        auto wart = d.GetData(n, s);
                        if(wart != 0)
                        {
                            // distributing to the unpacked event


                            if(s == 0) // if this is a time...
                                // Note in case of time signal it should be
                                //subtracted by the contents of channel 0
                            {
                                if(n == 0)
                                {
                                    time_in_channel_0 = wart;
                                    //cout << "##CZAS REFERENCYJNY ##### time of chann 0 ==> " << wart << endl;
                                }
                                else{
                                    wart = wart - time_in_channel_0;

                                    //                                cout << " Ch " << n << ", co daje po przesunieciu (" << time_in_channel_0
                                    //                                     << ") time:  "
                                    //                                     << wart << endl;
                                }
                            }

                            if(s ==0)   // TIME
                            {
                                // inne wzmocnienia czasu dla parysa, inne dla LaBr
                                if(branch_name == "paris")
                                {
                                    wart = (wart *
                                            100
                                            )
                                            //+ 1000
                                            ;
                                }else // labr
                                {
                                    //wart = (wart * 100);
                                }
                            }
                            // else   // wzmocnienia dla energii
                            //                                {
                            //                                    wart = wart * 0.125;
                            //                                }
                            // double multiplier = (s != 0)? 0.125 :  100.0;
                            // double offset = (s != 0)? 0 :  1000.0;

                            if(nr_dig < NR_DIGITIZERS)
                            {
                                int dig_ch = (nr_dig *16*3) + (n * 3) + s ;
                                target_event->digitizer_double_data[dig_ch] = wart;  // short int array (for Tone_signal)

                                if(flag_reference_time == false) // true data, not just reference
                                {

                                    int where = id_parysa + (s * 32);    // which signal:
                                    // because 0-31 is time, 32-63 qshort (fast), 64-96 qlong(slow)
                                    // Paris takes directly from this array
                                    target_event->digitizer_data[where] = wart;       // int32 array, (for BaF)


                                    //                                if(s == 0){
                                    //                                    cout << "   po wstaw do event --> Channel n =" <<  n
                                    //                                         << "  (paris_" << id_parysa
                                    //                                         << "), " ;
                                    //                                    if(s ==0) cout << " time ";
                                    //                                    else if (s == 1) cout << " fast? ";
                                    //                                    else cout << " slow? ";

                                    //                                    cout << " ==> value "
                                    //                                         << wart << "  (po korekcji)  " << flush;
                                    //                                    cout << "KKK" << endl;
                                    //                                }
                                } // if not reference, but normal data
                            } // endif nr of Digitizers sensible

                        } // if wart
                    }// for s

                } // for n

#else    // PARIS_NORMALY__OTHERWISE_PLASTIC
            //@grupa3_25_02_85MeV.txt


            // so this is plastic
            auto current_board = ltb_plastic.FindBoard(visit_ptr->name);

            if(current_board.Size() == 0)
            {
                cout << "For signals coming from digit. "
                     <<  visit_ptr->name
                      << " - the plastic lookup table has no entries"
                      << endl;
                //continue;
            } else {
                for(int ch = 0 ; ch < d.GetNchan() ; ++ch)
                {
                    if(d.Empty(ch))
                        continue;

                    auto wart = d.GetData(ch, 0);
                    if(wart != 0 && wart < 100000)
                    {
                        cout << "Digitizer Channel " << ch
                             << ", signal " << s << " ==> value " << wart << endl;


                        if(ch > (current_board.Size()-1) )
                        {
                            current_board.Print() ;
                            cout << "ERROR: (B) TOO BIG INDEX ch = " << ch
                                 << ",  while the possible (for this board) in LUT is: 0-"
                                 << current_board.Size()-1
                                 << endl;
                        }

                        auto branch = current_board.FindChannel(ch);
                        if(branch.Size() != 1)
                        {
                            cout << "\nERROR: In lookup table for 'plastic' there is no proper entry for "
                                 << visit_ptr->name
                                 << ", channel = " << ch << endl;
                            exit(88);

                        }
                        //int id_kratty = branch[0]->GetID();

                        //                        if(current_board[ch] == 0)
                        //                        {
                        //                            cout << "For digit, Channel " << ch
                        //                                 << " is not mentioned in a plastic lookup table " << endl;
                        //                            continue;
                        //                        }
                        int id_plastic = branch->GetID();
                        //int id_plastic  = current_board.FindChannel(ch)[0]->GetID();

                        // cout << "v775 Kanal " << n << " wartosc " << wart << endl;
                        // distributing to the event

                        cout << "      Plastic data by DIG. id_plastic "
                             << id_plastic  << "("
                             << (id_plastic/4)
                             << "+"
                             << (id_plastic%4)
                             << "), value = " << wart << endl;
                        target_event->plastic[id_plastic] = wart;     // time goes to 0. If in future we have energy, it will go to 1
                    }
                }
            }


#endif
            nr_of_analysed_words += how_many_words;
            continue;

        }
        else if(visit_ptr->type == string("V775"))
        {

            unpack_V775(visit_ptr);

            nr_of_analysed_words += how_many_words;   // UWAGA!!!!!1
            continue;

        }
        else if(visit_ptr->type == string("V879") || visit_ptr->type == string("V878"))
        {

            unpack_V879_878(visit_ptr);

            nr_of_analysed_words += how_many_words;
            continue;
        }
        else if(visit_ptr->type == string("V785"))
        {
            unpack_V785(visit_ptr);


            nr_of_analysed_words += how_many_words;
            continue;

        }
        else if(visit_ptr->type == string("V830"))
        {
            unpack_V830(visit_ptr);

            nr_of_analysed_words += how_many_words;
            continue;

        }
        else if(visit_ptr->type == string("RATE"))
        {
            // Piotr says it is for his personal use.
            nr_of_analysed_words += how_many_words;
            continue;

        }
        else if(visit_ptr->type == string("V1190"))
        {
            // cout << "dwa" << endl;

            unpack_V1190(visit_ptr);

            nr_of_analysed_words += how_many_words;
            continue;

        }
        else if(visit_ptr->type == string("V2740"))
        {
            //cout << "V2740 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

            unpack_V2740(visit_ptr);

            nr_of_analysed_words += how_many_words;
            continue;

        }
        else{
            cout << "Jeszcze nie implementowane dekodowanie bloku typu "
                 << visit_ptr->type << endl;
            nr_of_analysed_words += how_many_words;

            continue;
        }


    }// end for visiting card




    // cout << "end of unpacking non kratta (hector?) " << endl;
}
//*****************************************************************************
uint32_t     TIFJEventProcessor::swap_int32(uint32_t   dana)
{
    union  un {
        uint32_t   d;
        unsigned char  c[4];
    } zrodlo, rezultat;

    zrodlo.d = dana;
    rezultat.c[0] = zrodlo.c[3];
    rezultat.c[1] = zrodlo.c[2];
    rezultat.c[2] = zrodlo.c[1];
    rezultat.c[3] = zrodlo.c[0];
    return rezultat.d ;
}
//************************************************************************************
void TIFJEventProcessor::unpack_V775(Tvisitcard *visit_ptr )
{

    cout << "\nWykryta wizytowka V775 o name "
         << visit_ptr->name << endl;

    //            if(visit_ptr->name == string("tdc4"))
    //            {

    //                cout << "Uwaga " << endl;
    //            }
    d775.Load(&visit_ptr->data);

    // This will be plastic or silicon data
    auto current_board = ltb_plastic->FindBoard(visit_ptr->name);

    for(int n = 0 ; n < d775.GetNchan() ; ++n)
    {
        auto wart = d775.GetData(n, 0);
        if(wart > 0)
        {
            //                    cout << "775 for channel " << n
            //                         << " there is a data: " << wart << endl;

            if(current_board.Size() == 0)
            {
                //static int licznik = 0;
                //if( (++licznik % 100) == 0)
                cout << "For signals coming from v775 "
                     << visit_ptr->name
                     << " - the plastic/silicon lookup table "
                     << " is empty "
                     << endl;
                continue;
            }


            auto branch = current_board.FindChannel(n);
            static int licznik;
            if(branch.Size() != 1)
            {
                if( (++licznik % 500) == 0 )
                    cout << "[A] Signal from the v775, " << visit_ptr->name << ", channel "
                         << n
                         << " this entry is "<< (branch.Size() )
                         << " [A] times mentioned in a plastic/silicon lookup table "
                            " (so this signal is ignored) "
                         << endl;

                continue;
            }
            // distributing to the event
            // cout << "v775 Kanal " << n << " wartosc " << wart << endl;
            //                    auto c =
            // branch.Print(); //   FindID()
            //                            const LTRecord* const *array;
            // auto array = branch[0];  //.GetArray();
            // auto id = array   ->GetID();

            auto id  = current_board.FindChannel(n)[0]->GetID();

            //                    if(id != id_plastic){
            //                        cout << "Roznica nie powinna sie zdarzyc" << endl;
            //                    }
            //                    int id_plastic = current_board[n]->GetID();


            //                    cout << "Plastic data by TDC. " << visit_ptr->name
            //                         << " id_plastic "
            //                         << id_plastic  << "("
            //                         << (id_plastic/4)
            //                         << "+"
            //                         << (id_plastic%4)
            //                         << "), value = " << wart << endl;
            //                    if(id_plastic/4 == 6)
            //                    {
            //                        cout << "stop" << endl;
            //                    }

            string branch_name =
                    current_board.FindChannel(n)[0]->GetBranch();
            if(branch_name == string("plastic"))
            {
                target_event->plastic[id] = wart;     // time goes to 0. If in future we have energy, it will go to 1
                cout << "775 to plastic[" <<id << "] goes <--- " << wart << endl;

            }
            else if(branch_name == string("silicon"))
            {
                string variable_name =
                        current_board.FindChannel(n)[0]-> GetVariable(0);
                int co = 1;
                int xy = 0;
                int ch = 0;

                if(variable_name == "time30")co = 1;
                else if(variable_name == "time80")co = 2;

                target_event->silicon[id][xy][ch][co] = wart;
                //    cout << "555to silicon[" <<id << "][" << co << "] goes <--- " << wart << endl;
            }

        }
    } // endfor

}
//*****************************************************************************************
void TIFJEventProcessor::unpack_V879_878(Tvisitcard *visit_ptr)
{

    //cout << "Wykryty V879, rozpakowuje jako 879, dane z " << visit_ptr->name << endl;

    d879.Load(&visit_ptr->data);
    // This will be plastic or silicon data
    auto current_board = ltb_plastic->FindBoard(visit_ptr->name);

    for(int n = 0 ; n < d879.GetNchan() ; ++n)
    {
        auto wart = d879.GetData(n, 0);
        if(wart > 0){
            // cout << "879 Kanal " << n << " wartosc " << wart << endl;
            // distributing to the event

            if(current_board.Size() == 0)
            {
                //static int licznik = 0;
                //if( (++licznik % 100) == 0)

                cout << "For signals coming from v879 "
                     << visit_ptr->name
                     << " - the plastic/silicon lookup table "
                     << " is empty "
                     << endl;
                continue;
            }

            auto branch = current_board.FindChannel(n);
            if(branch.Size() != 1)
            {
                cout << "For v879, " << visit_ptr->name << ", channel "
                     << n
                     << " is "<< (branch.Size() )
                     << " [B] times mentioned in a plastic/silicon lookup table "
                     << endl;

                continue;
            }


            // distributing to the event


            int id  = current_board.FindChannel(n)[0]->GetID();

            string branch_name =
                    current_board.FindChannel(n)[0]->GetBranch();
            if(branch_name == string("plastic"))
            {
                target_event->plastic[id] = wart;     // time goes to 0. If in future we have energy, it will go to 1
                cout << "?????? to plastic[" <<id << "] goes <--- " << wart << endl;

            }
            else if(branch_name == string("silicon"))
            {
                string variable_name =
                        current_board.FindChannel(n)[0]->GetVariable(0);
                int co = 0;

                int ch = 0;// jur_sil


                if(variable_name == "time30")co = 1;
                else if(variable_name == "time80")co = 2;
                else{
                    cout << "Katstrofa" ;
                    continue ;
                }

                int xy = ch % 16 ;// jur_sil
                target_event->silicon[id][xy][ch][co] = wart;

                //                        if(id > 15)
                //                        cout << "to silicon[" <<id << "][" << co << "] goes <--- "
                //                             << wart << endl;
            }

        } // end if
    }//end for
}
//*****************************************************************************************
void TIFJEventProcessor::unpack_V785(Tvisitcard *visit_ptr)
{
    // cout << "Wykryty V785, z informacja od " << visit_ptr->name<< endl;
    d785.Load(&visit_ptr->data);
    auto current_board = ltb_plastic->FindBoard(visit_ptr->name);

    for(int n = 0 ; n < d785.GetNchan() ; ++n)
    {
        auto wart = d785.GetData(n, 0);
        if(wart > 0){
            // cout << "785 Kanal " << n << " wartosc " << wart << endl;
            // distributing to the event

            if(current_board.Size() == 0)
            {
                //static int licznik = 0;
                //if( (++licznik % 100) == 0)
                cout << "For signals coming from v785 "
                     << visit_ptr->name
                     << " - the plastic/silicon lookup table "
                     << " is empty "
                     << endl;
                continue;
            }


            auto branch = current_board.FindChannel(n);
            if(branch.Size() != 1)
            {
                cout << "For v785, " << visit_ptr->name << ", channel "
                     << n
                     << " is "<< (branch.Size() )
                     << " [C] times mentioned in a plastic/silicon lookup table "
                     << endl;

                continue;
            }
            // distributing to the event
            // cout << "v785 Kanal " << n << " wartosc " << wart << endl;
            //                    auto c =
            // branch.Print(); //   FindID()
            //                            const LTRecord* const *array;
            // auto array = branch[0];  //.GetArray();
            // auto id = array   ->GetID();

            int id  = current_board.FindChannel(n)[0]->GetID();



            string branch_name =
                    current_board.FindChannel(n)[0]->GetBranch();
            if(branch_name == string("plastic"))
            {
                target_event->plastic[id] = wart;     // time goes to 0. If in future we have energy, it will go to 1
                cout << "?????? to plastic[" <<id << "] goes <--- " << wart << endl;

            }
            else if(branch_name == string("silicon"))
            {
                string variable_name =
                        current_board.FindChannel(n)[0]->GetVariable(0);
                int co = 0;


                int ch = 0; // jur_sil
                int xy = 0; // jur_sil

                if(variable_name == "amplitude")co = 0;
                else continue ;


                target_event->silicon[id][xy][ch][co] = wart;
                //cout << "t444o silicon[" <<id << "][" << co << "] goes <--- " << wart << endl;
            }

        } // endif
    } // end for

}
//**********************************************************************************************
void TIFJEventProcessor::unpack_V830(Tvisitcard *visit_ptr)
{
    //                cout << "Wykryty scaler V830 (w danych 'kratta') z danymi nazwanymi: "
    //                     << visit_ptr->name
    //                     << endl;

    //    static unsigned int licznik ;
    //    if( !( (++licznik) % 100))
    //        cout << "Wykryty scaler V830 z danymi nazwanymi: "
    //             << visit_ptr->name
    //             << endl;

    d830.Load(&visit_ptr->data);

    // auto current_board = visit_ptr->name;
    // usually scl0,1,2,3
    // cout << current_board << endl;
    int board = visit_ptr->name[3] - '0';

    for(int n = 0 ; n < d830.GetNchan() ; ++n)
    {
        auto wart = d830.GetData(n, 0);
        if(wart > 0){
            // cout << "d830 Kanal " << n << " wartosc " << wart << endl;
            // distributing to the event

            int chan = board*32 + n;

            // cout << "Plastic scaler  " << chan << ", value = " << wart << endl;
            if(chan <KRATTA_NR_OF_PLASTICS )
                target_event->plastic_scalers[chan] = wart;
        }
    }
}
//*****************************************************************************************
void TIFJEventProcessor::unpack_V1190(Tvisitcard *visit_ptr)
{
    //  return;
    // cout << "==========Wykryty V1190 " << __PRETTY_FUNCTION__ << endl;
    // cout << "==========Wykryty V1190, z informacja od " << visit_ptr->name<< endl;
    d1190.Load(&visit_ptr->data);
    auto current_board = ltb_plastic->FindBoard(visit_ptr->name);
    // current_board.Print();


    // cout << " ile kanalow od d1190 = " << d1190.GetNchan() << endl;
    for(int n = 0 ; n < d1190.GetNchan() ; ++n)
    {
        auto wart = d1190.GetData(n, 0);
        if(wart > 0){
            // cout << "v1190 Kanal " << n << " ma wartosc " << wart << endl;
            // distributing to the event

            if(current_board.Size() == 0)
            {
                static int licznik = 0;
                if( (++licznik % 10000) == 0)
                {
                    cout << "XXXX For signals coming from v1190 "
                         << visit_ptr->name
                         << " - the plastic lookup table "
                         << ltb_plastic->name
                         << " has no entry\nso I ignore this data "   // czyli" current_board.Size() jest zerowy, pomijam !"
                         << endl;
                }
                continue;
            }


            auto branch = current_board.FindChannel(n);

            if(branch.Size() != 1)
            {
                cout << "For V1190 "
                     << visit_ptr->name << ", channel "
                     << n
                     << " ma branch.Size() = " << (branch.Size() )
                     << " czyli rózna od 1 , czy to znaczy, że jest  "
                     << " pusty? [Dplast] times mentioned in a plastic/silicon lookup table "
                     << endl;

                continue;
            }
            // distributing to the event
            // powtorzenie?
            // cout << "v1190 Kanal " << n << " wartosc " << wart << endl;

            // cout << "Printing the branch: " << endl;
            // branch.Print(); //   FindID()
            //       const LTRecord* const *array;
            //             auto array = branch[0];  //.GetArray();
            //             auto id = array   ->GetID();

            int id  = current_board.FindChannel(n)[0]->GetID();


            string branch_name =
                    current_board.FindChannel(n)[0]->GetBranch();
            if(branch_name == string("plastic"))
            {

                target_event->plastic[id] = wart;     // time goes to 0. If in future we have energy, it will go to 1
                // cout << " [X1] The value " << wart
                //      << " is sent to target_event->plastic["
                //      <<id << "]"
                //     << " which is kratta nr id/4 = " << (id / 4)
                //     << ", quarter " << (id % 4)
                //     << endl;


            }
            else if(branch_name == string("silicon"))
            {
                string variable_name =
                        current_board.FindChannel(n)[0]->GetVariable(0);
                int co = 0;
                if(variable_name == "amplitude")co = 0;
                else continue ;

                int xy = 0;// jur_sil
                int ch = 0 ; // jur_sil

                target_event->silicon[id][xy][ch][co] = wart;
                // cout << " [Y] The value " << wart << " is sent to target_event->silicon["
                //         <<id << "][" << co << "] "  << endl;
            }

        } // endif
    } // end for

}
//**********************************************************************************************
//*****************************************************************************************
void TIFJEventProcessor::unpack_V2740(Tvisitcard *visit_ptr)
{

    // return;


    // cout << "==========Wykryty V2740, z informacja od " << visit_ptr->name<< endl;

    d2740.Load(&visit_ptr->data);
    // cout << "przed HEADER --------------------------------- LINE = " << __LINE__ << endl;
    // d2740.ShowHeader();

    auto current_board = ltb_silicon->FindBoard(visit_ptr->name);

    // cout << "ILe kanalow = d2740.GetNchan() =  " << d2740.GetNchan() << endl;
    for(int n = 0 ; n < d2740.GetNchan() ; ++n)
    {
        auto wart = d2740.GetData(n, 0);  // potem i tak jeeszcze raz
        // cout << "2740 Kanal " << n << " ma wartosc " << wart << endl;

        if(wart > 0){
            // cout << "2740 Kanal " << n << " ma niezerowa  wartosc " << wart << endl;
            // distributing to the event

            if(current_board.Size() == 0)
            {
                //static int licznik = 0;
                //if( (++licznik % 100) == 0)

                cout << "For signals coming from v2740 , name = "
                     << visit_ptr->name
                        // << current_board.
                     << " - the current board.Size()  is 0  "
                     << endl;
                continue;
            }

            // cout << "LINE = " << __LINE__ << endl;

            auto branch = current_board.FindChannel(n);
            // cout << "LINE = " << __LINE__ << endl;

            if(branch.Size() != 1)
            {
                cout << "For V2740 "
                     << visit_ptr->name << ", channel "
                     << n
                     << " branch.Size() = "<< (branch.Size() )
                     << " wiec "
                     << " pusty? [Dsiilic] times mentioned in a plastic/silicon lookup table "
                     << endl;

                continue;
            }
            // distributing to the event
            // powtorzenie?
            // cout << "v2740 Kanal " << n << " wartosc " << wart << endl;
            // cout << "LINE = " << __LINE__ << endl;

            // cout << "Printing the branch: " << endl;
            // branch.Print(); //   FindID()
            //       const LTRecord* const *array;
            // auto array = branch[0];  //.GetArray();
            // auto id = array   ->GetID();
            //cout << "gdzie = " << __LINE__ << endl;

            int id  = current_board.FindChannel(n)[0]->GetID();


            string branch_name =
                    current_board.FindChannel(n)[0]->GetBranch();
            // cout << "gdzie = " << __LINE__ << "  DDD branch_name = [" << branch_name << "]" << endl;

            if(branch_name == string("plastic"))
            {
                cout << "TUTAJ Plastic ??" << endl;
                target_event->plastic[id] = wart;     // time goes to 0. If in future we have energy, it will go to 1
                // cout << " [X2] The value " << wart
                //      << " is sent to target_event->plastic["
                //      <<id << "]"  << endl;

            }
            // else
            if(branch_name == string("silicon"))
            {
                // cout << " branch_name jest " << branch_name << endl;

                // auto channel  = current_board.FindChannel(n)[0];
                // auto ccc = channel->GetVariable(0);

                // string  variable_name = current_board.FindChannel(n)[0]->GetVariable(0);
                // cout << "--------------------------w galezi 'silicon' :variable_name = " << variable_name << endl;
                // cout << "gdzie = " << __LINE__ << endl;

                int nr_of_det = id / 1000 ;

                int xy =  (id - (nr_of_det * 1000) ) / 100;
                // xy = xy / 100;

                int stripe = id % 100 ;
                target_event->silicon_fired[nr_of_det] = true;

                for (int co = 0 ; co < 4 ; co++){

                    // 0 - time
                    // 1 - amplituta
                    // 2 - pedestal
                    // 3  - risetime

                    wart = d2740.GetData(n, co);

                    target_event->silicon[nr_of_det][xy][stripe][co] = wart;

                    // cout << " [Y] The value " << wart << " is sent to target_event->silicon["
                    //      << nr_of_det << "]["
                    //      << xy << "]["
                    //      << stripe << "]["
                    //      << co << "] "
                    //      << endl;
                    // cout << "juz " << endl;
                }
                int size = d2740.GetDataSize(n) - 4; // because skippinf above 4 signals (co)
                if(size)
                {
                    // cout << " there is a signal spectum " << endl;
                    for(int i = 0 ; i < size ; ++i)
                    {
                        double wart = d2740.GetData(n, i);
                        target_event->silicon_signals[nr_of_det][xy][stripe][i] = wart;

                        // if (i < 5 )
                        // {
                        //     cout << i << "), wart = " << wart ; // << endl;
                        //     cout << ", sprawdzam "   << target_event->silicon_signals[nr_of_det][xy][stripe][i]
                        //             << endl ;
                        // }
                    }
                    // cout << " [Y] These values shoud be sent to target_event->silicon["
                    //      << nr_of_det << "]["
                    //      << xy << "]["
                    //      << stripe << "]"
                    //      << endl;


                } // if size
                // int m = 24;
            }
            else{
                cout << "Nie rozpoznane branch name = " << branch_name << endl;
            }

        } // endif
    } // end for
    // cout << "KOniec f.cji " << __func__ << endl;
}
//**********************************************************************************************



// TIFJEventProcessor

