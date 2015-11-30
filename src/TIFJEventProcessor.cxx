//---------------------------------------------------------------

/*
make CFLAGS='-g -O0' CXXFLAGS='-g -O0' clean all
*/


#include "experiment_def.h"

#include "tjurekabstracteventprocessor.h"

#include "TIFJEvent.h"
#include "TIFJAnalysis.h"

// #include "Go4EventServer/TjurekMbsEvent.h"

#include <iostream> // for test printouts
#include <iomanip> // for test printouts
#include <sstream>

using namespace std;

#include "spectrum.h"
#include "Tfile_helper.h"
#include "Trising.h"  // for cluster characters

#include "Tlookup_table_kratta.h"
#define FOUR_BRANCHES_VERSION   1
#define NEW_FRS_LOOKUP_TABLE   true

// for debugging purposes
#define POKAZ 0

//#define LOOKING 1
static const unsigned TIMESTAMP_QUEUE_MAX = 10; // was 10

extern istream &zjedz(istream &plik);
#define DBG(x)     // cout << x << endl
//******************************************************************
TIFJEventProcessor::TIFJEventProcessor(string name) :
    TjurekAbstractEventProcessor(name)
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


    lookup_kratta.read_from_disk("./calibration/geo_map.geo");

    flag_old_style_pulser_in_b7 = true; // during commisioninig experiment
    // after discovering first pulser in Master trigger word -
    // this flag will be switched automatically

    // we remember for private functions
    target_event = NULL;
    // TjurekMbsEvent* input_event ;
    input_subevent = NULL;
    input_event_data = NULL; // one event buffer
    how_many_data_words = 0 ; // how many sensible data in the buffer
    memset(&vmeOne[0][0], 0, sizeof(vmeOne));
    memset(&vmeUser[0][0], 0, sizeof(vmeUser));

}
//******************************************************************
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
    // TRACE((11,"TIFJEventProcessor::BuildEbEvent(TIFJevent*)"
    // ,__LINE__,
    // __FILE__));

    // cout << "Function BuildEbEvent================================" << endl;

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


    if(unpack_the_fast_beam_campaign_event()) {
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
bool  TIFJEventProcessor::unpack_kratta_hector_event(const_daq_word_t *data, int how_many_words)
{
    mbs_listner &mbs = *(source_of_events_ptr->give_mbs());
    for(int subev_nr = 0;  ;  ++subev_nr) {

        int result = mbs.get_next_subevent_if_any();

        if(result  != GETEVT__SUCCESS) { // moze bardzej subtelnie? Podwody

            if(result == GETEVT__NOMORE) {
//                cout << "Koniec eventu " << current_ev_nr
//                     << " bec. result is GETEVT__NOMORE = " << result << endl;
                break;
            } else {
                cout << "Very strange error because result is " << result << endl;
                exit(9);
            }
        }
        // recognising


        uint32_t *data = mbs.data();
        int length = mbs.length() ;

        // recognise what to unpack
//        cout << "Unpacking event nr " << current_ev_nr << " subevent nr " << subev_nr << endl;


//        int s = mbs.subtype() ;
//        int t = mbs.type();
//        cout << "Subevent  " << s << ",  " << t << ", length " << length << endl;


        if(mbs.subtype() == 36 && mbs.type() == 2800) {
            unpack_trigger_pattern(current_ev_nr, subev_nr, (uint32_t *) data, length);
        } else if(mbs.subtype() == 36 && mbs.type() == 1) { // rozkodowanie V1724 !!)
            unpack_caen_v1724(current_ev_nr, subev_nr, (uint32_t *) data, length);
        }

        else if(mbs.subtype() == 10 && mbs.type() == 1) {
            unpack_10_1(current_ev_nr, subev_nr, (uint32_t *) data, length);
        } else if(mbs.subtype() == 34 && mbs.type() == 1) {
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

    }   // while(1);


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
bool TIFJEventProcessor::CheckForGerData(const_daq_word_t *data, int length)
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
void TIFJEventProcessor::unpack_ger_subevent(const timestamped_subevent &t,
                                             TIFJEvent *target_event)
{}

//********************************************************************************************************************
void TIFJEventProcessor::unpack_frs_subevent(const timestamped_subevent &t,
                                             TIFJEvent *te)
{
    cout << "Empty  function " << endl;

}

//using namespace Enum;

//*******************************************************************************************************************
void TIFJEventProcessor::distribute_frs_vme_words_to_event_members(
        TIFJEvent *target_event)
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

    unsigned int how_many_data_words = t.length();

    for(unsigned int i = 0; i < how_many_data_words; i++) {
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
void TIFJEventProcessor::unpack_dgf_subevent(const timestamped_subevent &t,
                                             TIFJEvent *te)
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
void TIFJEventProcessor::unpack_mib_subevent(const timestamped_subevent &t,
                                             TIFJEvent *te)
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
/** reading the types of events which are legal for 4 branches mode of MBS
 reading the time gates for time stamp system */
//************************************************************************
bool TIFJEventProcessor::read_in_4mbs_parameters(
        string nam_ger_minus_frs_timestamp_diff,
        string nam_frs_minus_hec_timestamp_diff,
        string nam_frs_minus_dgf_timestamp_diff,
        string nam_ger_minus_dgf_timestamp_diff,
        string nam_ger_minus_hec_timestamp_diff,
        string nam_dgf_minus_hec_timestamp_diff)
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
//***************************************************************************
/** To have a chance to load the lookup table */
void TIFJEventProcessor::preLoop()
{
    // this funcion is a kind of preloop, so here
#ifdef RISING_GERMANIUMS_PRESENT
    lookup.read_from_disk();
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
//*****************************************************************************************
/** reading the types of events which are legal for 5 branches mode of MBS
 reading the time gates for time stamp system */
bool TIFJEventProcessor::read_in_5mbs_parameters(
        string nam_ger_minus_frs_timestamp_diff,
        string nam_frs_minus_hec_timestamp_diff,
        string nam_frs_minus_dgf_timestamp_diff,
        string nam_ger_minus_dgf_timestamp_diff,
        string nam_ger_minus_hec_timestamp_diff,
        string nam_dgf_minus_hec_timestamp_diff,
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
//************************************************************************************


/** This is the function which reads new style of event - characteristic
 to the fast beam campaign. There is no timestamp matching, everything
 comes together as one event (with subevent parts) */
bool TIFJEventProcessor::unpack_the_fast_beam_campaign_event()
{
    //cout << "===== function :unpack_the_fast_beam_campaign_event() "
    //  << counter_put << endl;

    static time_t last = 0;
    time_t now = time(0);

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

        #if CURRENT_EXPERIMENT_TYPE==PISOLO2_EXPERIMENT
            !unpack_pisolo_event     // Alberto Stefanini
            (input_event_data, how_many_data_words)
        #elif CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
            ! unpack_exotic_event_from_raw_array   // <------- for exotic block
            (input_event_data, how_many_data_words)
        #elif CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
            ! unpack_prisma_event(input_event_data, how_many_data_words)
        #elif CURRENT_EXPERIMENT_TYPE==IFJ_KRATTA_HECTOR_EXPERIMENT
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
/** unpacking the timing module subevent */
bool TIFJEventProcessor::unpack_timing_module_subevent(
        const_daq_word_t *long_data, int how_many_words)
{

#if ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||     \
    (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))

    // Henning says that here there are 4 Caen V775 TDCs (range 1.2 us)
    // and the old_long range CAEN TDC V767 - which has 128 channels in one card

#define SPEAK_TIMING 0
#define DBGT  if(SPEAK_TIMING)

    //   DBGT
    //   {
    //     cout << "To unpack there is " << how_many_words << " words \n";
    //
    //
    //     for(int i = 0 ; i < how_many_words ; i++)
    //   {
    //     cout << "[" << i << "]  0x" << hex <<  long_data[i]
    //       << ",  dec = " <<  dec <<  long_data[i] << endl;
    //
    //     }
    //   }


    /* is i header
     /////////////////////////////////////////
     class vme767_header_word
     {
     public:
     unsigned int evnet_nr: 12;
     unsigned int         : 9;
     //unsigned int header  : 1;
     unsigned int code    : 2;
     unsigned int         : 4;
     unsigned int geo     : 5 ;
     };
     /////////////////////////////////////////
     class vme767_data_word
     {
     public:
     unsigned int time_data  : 20;
     unsigned int edge       : 1;
     // unsigned int eob        : 1;
     // unsigned int header     : 1;
     unsigned int code    : 2;
     unsigned int start      : 1;
     unsigned int channel    : 7;
     };
     /////////////////////////////////////////
     class vme767_eob_word
     {
     public:
     unsigned int ev_counter   : 16 ;
     unsigned int  : 5 ;
     // unsigned int eob        : 1;
     // unsigned int header     : 1;
     unsigned int code    : 2;
     unsigned int  : 1 ;
     unsigned int status : 3 ;
     unsigned int geo     : 5 ;
     };
     /////////////////////////////////////////
     /////////////////////////////////////////
     union vme767_word
     {
     daq_word_t           raw_word ;
     vme767_header_word      header_word ;
     vme767_data_word        data_word ;
     vme767_eob_word         end_of_block_word ;
     };

     union general_vme_word
     {
     vme767_word word767;
     vme_word    word775;
     };

     --------------------*/

    // at first there are VME 775 VME data ########################
    // loop over unknown nr of words

    general_vme_word word;

    int counter775modules = 0;
    // zero was used, now we start from 1
    unsigned int current_geo = 0;

    enum typ_of_block {typ_vme775, typ_vme767};
    typ_of_block current_vme_blok = typ_vme775;

    for(int i = 0; i < how_many_words; i++) {
        DBGT cout << "Unpacking word " << i << ": 0x"
                  << hex << long_data[i] << dec << endl;
        word.word775.raw_word = long_data[i];
        if(!word.word775.raw_word) {  // do not analyse empty words
            continue;
        }

        // Sometimes Henning is sending the extra word on the end of the VME block
        // with the pattern 0xbabe****
        if((word.word775.raw_word & 0xffff0000) == 0xbabe0000) {
            DBGT cout << "babe word ==============" << endl;
            break; // end of data at all, so end of the loop
        }

        // at first there will be information from the FOUR  vme775 blocks and then
        // information from the vme767

        if(counter775modules >= 4)
            current_vme_blok = typ_vme767;

        switch(current_vme_blok) {
        case typ_vme775: //-----------------------------------------------------------------
            // Sometimes Henning is sending the extra word on the end of the VME block
            // with the pattern 0xbabe****

            switch(word.word775.header_word.code) {  // <-- it is common to all of types of vme words

            case 2: {
                DBGT cout << "Caen V775 TDC Header .." << endl;
                //counter775modules++;
            }
                break;

            case 0:
                // sizes are hardcoded in the class definition
                if(word.word775.data_word.geo < 22 && word.word775.data_word.channel < 32) {

                    DBGT cout << "Arrived TIME geo = " << word.word775.data_word.geo
                              << ", chan = " << word.word775.data_word.channel
                              << ", data = " << word.word775.data_word.data
                              << endl;

                    int clus = 0;
                    int crys = 0;
                    if(lookup_SR_times.current_combination(word.word775.data_word.geo,
                                                           word.word775.data_word.channel,
                                                           &clus,
                                                           &crys)) {

                        DBGT cout << " --- this is time for cluster "
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
                }
                break;

            case 4:
                DBGT cout << "Footer  when i = .." << i << endl;
                counter775modules++;
                break;
            case 6:
                DBGT cout << "'Not valid datum for this TDC when i = .." << i
                          << " vme module nr " << counter775modules << " didn't fired " << endl;
                counter775modules++;

                break;

            default :
                DBGT cout << "Error - unknown code in VME time information  when i = .." << i << endl;
                break;
            } // endof inner switch

            break;

            // ########################################################################
        case typ_vme767: //---------------------------------------------------------------------

            switch(word.word767.header_word.code) {  // <-- it is common to all of types of vme words

            case 2: {
                DBGT cout << "Caen TDC V767 Header ------------------------" << endl;
                current_geo = word.word767.header_word.geo;
            }
                break;

            case 0:
                // sizes are hardcoded in the class definition
                if(current_geo < 222 /*was 22*/ && word.word767.data_word.channel < 129) {

                    DBGT cout << "Arrived TIME geo = " << current_geo
                              << ", chan = " << word.word767.data_word.channel
                              << ", data = " << word.word767.data_word.time_data
                              << endl;

                    int clus = 0;
                    int crys = 0;
                    if(lookup_LR_times.current_combination(
                                current_geo,
                                word.word767.data_word.channel,
                                &clus,
                                &crys)) {

                        DBGT cout << " --- this is time for cluster "
                                     //          << clus
                                     //          << " crystal " << crys
                                     //          << " which is "
                                  << Trising::cluster_characters[clus] << "_"
                                  << char('1' + crys) << endl;

                        // storing the time data
                        // no need to we reduce the  resolution from 64 KB to 8KB !!!

                        // Note: this block can deliver more than one data for the same channel in the
                        // same event. We take only the first. (How we do check If this is the first occurence?
                        // We just check in in the target event if there is something
                        // already from this event (zero value means - this is the frist)

                        if(target_event->ge_time_LR[clus][crys] == 0) {
                            target_event->ge_time_LR[clus][crys] = word.word767.data_word.time_data;
                            target_event->ger_xia_dgf_fired[clus][crys] = true;
                        }
                    }

                    // if this was not found in the lookup table - possible that it
                    // was so called Plastic time information, which is also transmited
                    // in this block
                    else if((ger_plastic_LR_time_geo == current_geo)
                            &&
                            (ger_plastic_LR_time_channel == word.word767.data_word.channel)) {
                        target_event->Plastic_LR_time = word.word767.data_word.time_data;
                    }
#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE
                    else if((ger_Silicon_LR_trigger_time_geo == current_geo)
                            &&
                            (ger_Silicon_LR_trigger_time_channel == word.word767.data_word.channel)) {
                        // as it is a multi hit device, that means that in one event it can come 3 time.
                        // so we have to place it in one of the 3 elements of the array
                        int nr = 0;
                        if(target_event->Silicon_LR_trigger_time[0]) {  // <-- if already occupied
                            nr++;
                            if(target_event->Silicon_LR_trigger_time[1]) {  // <-- if already occupied
                                nr++;
                                if(target_event->Silicon_LR_trigger_time[2])    // <-- if already occupied
                                    nr++; // <-- this means more than 3 hit, forget it
                            }
                        }
                        // now we know the indeks
                        if(nr >= 0 && nr <= 2) {
                            target_event->Silicon_LR_trigger_time[nr] = word.word767.data_word.time_data;
                        } else {
                            cout << "Placing Silicon_LR_trigger_time   index 'nr' is out of range (" << nr << ")" << endl;
                        }
                    }
#endif
                    else {
                        //                              cout << "TIME unpacking. Warning: combination GEO = "
                        //                                 << current_geo
                        //                                 << ", CHANNEL = " << word.word767.data_word.channel
                        //                                 << " was not specified in a lookup table Cluster_LR_time" << endl;
                    }

                }
                break;

            case 1:
                DBGT cout << "Footer  when i = .." << i << endl;
            {
                // local scope
                static int how_many_errors;
                if(word.word767.end_of_block_word.status) {
                    how_many_errors++;
                    if(flag_accept_single_ger && flag_accept_single_dgf) {
                        static time_t last_message = time(NULL);
                        if((time(NULL) - last_message > 5) && (how_many_errors > 500)) {         // every 5 seconds
                            last_message = time(NULL);
                            cout << "Error status in Caen 767 block (which deliveres LR time). Please reset it"
                                 << endl;

                            //cout << "This is sorting online " << endl;

                            ofstream plik("./commands/synchronisation_lost.txt");
                            plik
                                    << "Long Range TDC status Watchdog barks !\n"
                                    << "The Status bitfield in NON ZERO.\n  "
                                    << "\nNote: - If you do not care about this (for example you are interested only in FRS)\n"
                                       "Please go:\n"
                                       "Spy_options->Matching of MBS subevent->Modify above parameters (wizard)->5 branches [next]->\n"
                                       "->Do not make matching [next]-> here please uncheck the fields 'GER' and 'DGF',\n"
                                       "Then finish the wizard (and update spy if it is running)"
                                    << endl;
                            plik.close();

                        } // every 5 seconds

                        return false;
                    }

                } else { // no error in status
                    how_many_errors = 0;
                }
            } // locaal
                break;
            case 3:
                DBGT cout << " 'Not valid datum for this TDC when i = .." << i << endl;
                break;

            default :
                DBGT cout << "no Code,  i = .." << i << endl;
                break;
            }

            break;

        }// end of  switch(current_vme_blok)

    } // end of for i
#endif
    return true;

}
//****************************************************************************************
/** This function is unpacking  directly  from the LMD data block  */
bool TIFJEventProcessor::unpack_pisolo_event(
        const_daq_word_t *data, int how_many_words)
{
    //   cout << " F.  unpack_pisolo_event" << endl;

#ifdef OLD_PISOLO_DATA

    return   unpack_OLD_pisolo_event(data, how_many_words);

#else

    // warning: this function should not destroy this was is done by other frs_user function


    vme_775_word word;
    if(flag_vmeOne_zeroing_needed) { // to zeroing before frs or frs_user unpacking function
        memset(&vmeOne[0][0], 0, sizeof(vmeOne));
        flag_vmeOne_zeroing_needed = false;
    }

    for(int i = 0; i < how_many_words; i++) {
        word.raw_word = data[i];

        if(!word.raw_word) { // do not analyse empty words
            // just skip, do not complain
            continue;
        }

        switch(word.header_word.code)  // <-- it is common to all of
            // types of vme words
        {

        case 2: // this is a header
            //cout << "This is the vme header ..........." << endl;
            // normally we do not care, but if it is geo 6...
#ifdef NIGDY

            if(word.header_word.geo == 6) {
                // this block nr 6 works with different convention.
                // Data words just follow header by many (32) raw data words
                for(unsigned int k = 0; k < word.header_word.cnt; k++) {
                    // each data word is comming from some channel, so we place it into vme array
                    //cout << "[" << k << "] " <<  data[1+i+k] << " "<< endl ;
                    vmeOne[word.header_word.geo][k] = data[i + k + 1];
                }
                //vme[word.header_word.geo][3] = 77;
                //cout << endl;

#ifdef NIGDY
                this was old style, now we do
                        it by lookup table
                        target_event->scaler_ch_1_free_triggers = data[i + 1];
                target_event->scaler_ch_2_accepted_triggers = data[i + 2];
                target_event->scaler_ch_3_seetram_digitizer = data[i + 3];
                target_event->scaler_ch_4_spills = data[i + 4];
                target_event->scaler_ch_5_seconds = data[i + 5];
                target_event->scaler_ch_6_sc01_free = data[i + 6];
                target_event->scaler_ch_7_sc21_free = data[i + 7];
                target_event->scaler_ch_8_sc41_free = data[i + 8];
#endif  // nigdy
                // after copying, we jump over them in the outer loop, they are processed
                i += word.header_word.cnt;
            }
#endif  // nigdy
            break;

        case 0:
            // sizes are hardcoded in the class definition
            //             cout << "Code is 0, data word" << endl;

            // loading into the vme table.
            if(word.data_word.geo < 22 && word.data_word.channel < 32) {
                vmeOne[word.data_word.geo][word.data_word.channel]
                        = word.data_word.data;
            }
            // trap for debugging
            //if (word.data_word.geo == 3 && (word.data_word.channel == 0))
        {

            //         cout << "Data word in the event,  Now:   vme["
            //         << word.data_word.geo << "][" << word.data_word.channel
            //         << "] has value = " << word.data_word.data << endl;
            //
            //         cout << " YES, raw value = " << hex << (word.raw_word & 0xffff)
            //         << dec << endl;
        }
            break;

        case 4:
            //cout << "Code 4 = End of block\n" << endl;
            break;

        case 6:
            //cout << "Code 6 = Not valid datum \n" << endl;
            break;

        default:
            cout << "Unknown VME Code  = " << word.header_word.code << ",  LINE= " << __LINE__ << endl;

            break;
        }
    }
#endif

    return true;
}

//****************************************************************************************
/** This function is unpacking not from the timestamped subevent, but just directly  from the MBS data block  */
bool TIFJEventProcessor::unpack_OLD_pisolo_event(
        const_daq_word_t *data, int how_many_words)
{
    //   cout << " F.  unpack_OLD_pisolo_event" << endl;
#if CURRENT_EXPERIMENT_TYPE    ==   PISOLO2_EXPERIMENT

    vme_775_word word;
    if(flag_vmeOne_zeroing_needed) { // to zeroing before frs or frs_user unpacking function
        memset(&vmeOne[0][0], 0, sizeof(vmeOne));
        flag_vmeOne_zeroing_needed = false;
    }


    // the Old Pisolo data  is 16 bit long  (shor int?)

    typedef unsigned short int   oldpisolo_t ;

    oldpisolo_t *buffer = (oldpisolo_t *) data;

    for(int i = 0; i < how_many_words; i++) {
        if(!buffer[i])
            continue;

        //      if(i == 13 || i == 14 )    cout << i << ") =======> " << showbase << hex<<  buffer[i] << dec << " dec = " << buffer[i]  << endl;;

        //if(i > 4)
        {
            //       cout << "Hurra " << endl;
            //       cout << "Hurra " << endl;
            //       cout << "Hurra " << endl;
            switch(i) {
            case 5:
                target_event->pisolo_mcp01_x = buffer[i];
                break;  // micro channel plate detectors measuring position
            case 6:
                target_event->pisolo_mcp01_y = buffer[i];
                break;
            case 7:
                target_event->pisolo_mcp02_x = buffer[i];
                break;
            case 8:
                target_event->pisolo_mcp02_y = buffer[i];
                break;

                //case 12: //9:
                //target_event->pisolo_si_target_en = buffer[i]; break;    // EN is the energy released in the Si detector

            case 10:
                target_event->pisolo_tof2 = buffer[i];
                break;
            case 11:
                target_event->pisolo_deltaE[0] = buffer[i];
                break;     // DE is the Delta E signal from the ionization chamber IC

            case 9:
                target_event->pisolo_si_final = buffer[i];
                break;    // E   in the Si detector
            case 12:
                target_event->pisolo_dE_res = buffer[i];     // Eres (gas)
                break;    // Eres is the residual energy released in the IC (normally zero)

            case 13:
                target_event->pisolo_mon_lr = buffer[i];
                break;
            case 14:
                target_event->pisolo_mon_ud = buffer[i];
                break;
            case 15:
                target_event->pisolo_tof1 = buffer[i];
                break;
            case 16:
                target_event->pisolo_tof3 = buffer[i];
                break;
            default:
                break;
            }
        }
    }
#endif
    return true;
}
//***************************************************************************************

//***************************************************************************************
bool TIFJEventProcessor::unpack_exotic_event_from_raw_array(
        const_daq_word_t *data, int how_many_words)
{
    //   cout << " F.  unpack_exotic_event_from_raw_array =====================================" << endl;
    // warning: this function should not destroy this was is done by other frs_user function

#if CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
    struct exotic_data_word {
        unsigned int data :
            12;
        unsigned int ovr :
            1;
        unsigned int not_used :
            3;
        unsigned int sample :
            4;
        unsigned int va_channel :
            5;
        unsigned int adc_channel :
            3;
        unsigned int event_counter :
            3;
        unsigned int data_word_identification :
            1;
    };

    struct exotic_header_word {
        unsigned int module :
            8;
        unsigned int modality_code :
            4;
        unsigned int trigger :
            4;
        unsigned int event_counter :
            14;
        unsigned int not_used_header_identification :
            1;
        unsigned int header_identification :
            1;
    };

    union exotic_word {
        exotic_data_word            data_word;
        exotic_header_word          header_word;
        unsigned long                   raw_word;
        vme_785_word    word785;   // defined in the header of this file
    };

    exotic_word word;

    if(flag_vmeOne_zeroing_needed) { // to zeroing before frs or frs_user unpacking function
        memset(&vmeOne[0][0], 0, sizeof(vmeOne));
        flag_vmeOne_zeroing_needed = false;
    }

    int exotic_block_number = 0 ;   // there can be more than one electronic block


    how_many_words = data[0] ;
    //   cout << "Nr of words is " << how_many_words << endl;
    int how_many_exotic_blocks = 1;

    for(int i = 1; i < how_many_words + 1 ; i++) {  // counter word was 'not counted'
        word.raw_word = data[i];
        if(!word.raw_word) { // do not analyse empty words
            // TODO complain
            continue;
        }

        //     cout << "word  " << i << ")   0x" << hex << word.raw_word << dec << endl;

        if(word.raw_word == 0xeeeeeeee  || word.raw_word == 0xffffffff) {
            //       cout << "word  " << i << ")   0x" << hex << word.raw_word << dec << endl;
            exotic_block_number++;
            //       cout << "exotic_block_number = " << exotic_block_number
            //       << " +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
            continue;
        }

        if(exotic_block_number < how_many_exotic_blocks) {
            switch(word.header_word.header_identification)  // <-- it is common to all of
                // types of vme words
            {

            case 1: // this is a header
                //       cout << "word " << i << ")   This is the exotic header ..........." << endl;
                //
                //             cout
                //                 << "HEADER WORD. Module = " << word.header_word.module
                //                 << ", modality_code = " << word.header_word.modality_code
                //                 << ", trigger = " << word.header_word.trigger
                //                 << ", event_counter = " << word.header_word.event_counter
                // //       << "================================================================="
                //                 << endl;

                break;

            case 0:
                // sizes are hardcoded in the class definition
                //       cout << "Code is 0, this is a data word" << endl;

                // if( word.data_word.adc_channel < 3)

                //       cout
                //       << "DATA WORD.  data = " << word.data_word.data
                //       << ", ovr = " << word.data_word.ovr
                //       << ", sample = " << word.data_word.sample
                //       << ", va_channel = " << word.data_word.va_channel
                //       << ", adc_channel = " << word.data_word.adc_channel
                //       << ", event_counter = " << word.data_word.event_counter
                //       << endl;

                int module_modified = word.data_word.adc_channel  + (8 * exotic_block_number);
                int sample = word.data_word.sample;
                if(sample > 7) {
                    sample -= 8;
                } else {
                    //         cout << "No need to subtract 8 ---------------------------------------------------------------------------------------------------------------" << endl;
                }
                if(
                        module_modified < NR_OF_EXOTIC_MODULES
                        &&  module_modified >= 0

                        && word.data_word.va_channel < NR_OF_EXOTIC_STRIPES_X
                        && word.data_word.va_channel >= 0

                        && (sample < NR_OF_EXOTIC_SAMPLES)
                        && (sample >= 0)
                        ) {
                    target_event->exotic_data[module_modified]
                            [word.data_word.va_channel]
                            [sample] = word.data_word.data;

                    target_event->exotic_data_fired[module_modified] = 1; // fired
                } else {

                    cout
                            << "TIFJEventProcessor::unpack_exotic_event_from_raw_array   -  Rejected event"
                            << "\n for electronic exotic block nr [0 - n] :  " << exotic_block_number
                            << "\nbecause \n" ;

                    if(module_modified >= NR_OF_EXOTIC_MODULES) {
                        cout << " module_modified = " << module_modified
                             << " while it should be smaller than " << NR_OF_EXOTIC_MODULES
                             << endl;
                    }

                    if(word.data_word.va_channel >= NR_OF_EXOTIC_STRIPES_X) {
                        cout << " va_channel = " << word.data_word.va_channel
                             << " while it should be smaller than " << NR_OF_EXOTIC_STRIPES_X
                             << endl;
                    }

                    if((sample) >  NR_OF_EXOTIC_SAMPLES) {
                        cout
                                << " sample = " << sample
                                << " while it should be smaller than " << NR_OF_EXOTIC_SAMPLES
                                << endl;
                    }

                    cout
                            << "\nmodule_modified = " << module_modified
                            <<  ", word.data_word.va_channel = " << word.data_word.va_channel
                             << ", word.data_word.sample = " << word.data_word.sample
                             << endl;

                    cout << " well? ... " << endl;
                }

                // trap for debugging
                //             if(word.data_word.geo == 3  && (word.data_word.channel == 0))
                //             {
                //
                //                 cout << "Data word in the event,  Now:   vme[" << word.data_word.geo << "]["
                //                 << word.data_word.channel << "] has value = "
                //                 << word.data_word.data
                //                 << endl;

                // //                 cout << " YES, raw value = " << hex << (word.raw_word & 0xffff )
                // //                 << dec << endl;
                //             }
                break;

            } // end switch
        } else { // this is not exotic card, but CAEN  775

            switch(word.word785.header_word.code) {  // <-- it is common to all of types of vme words

            case 2: {
                //    cout << "Caen V785 TDC Header .." << endl;
                //counter775modules++;
            }
                break;

            case 0:
                // sizes are hardcoded in the class definition
                if(word.word785.data_word.geo < 22 && word.word785.data_word.channel < 32) {


                    //           cout << "Arrived TIME geo = " << word.word785.data_word.geo
                    //           << ", chan = " << word.word785.data_word.channel
                    //           << ", data = " << word.word785.data_word.data
                    //           << endl;

                    int clus = 0;
                    int crys = 0;
                    if(true)
                        //           if ( lookup_SR_times.current_combination ( word.word775.data_word.geo,
                        //                word.word775.data_word.channel,
                        //                &clus,
                        //                &crys ) )
                    {
                        //
                        //              cout << " --- this is time for cluster "
                        //             //          << clus
                        //             //          << " crystal " << crys
                        //             //          << " which is "
                        //             << Trising::cluster_characters[clus] << "_"
                        //             << char ( '1' + crys ) << endl;
                        //
                        //             // storing the time data
                        //             // no need to we reduce the  resolution from 64 KB to 8KB !!!
                        target_event->v785[word.word785.data_word.channel] =  word.word785.data_word.data;

                    } else {
                        static int licznik;
                        //              if ( ! ( ( licznik++ ) %2 ) )
                        cout << "exotic unpacking of CAEN V785 Warning: combination GEO = "
                             << word.word785.data_word.geo
                             << ", CHANNEL = " << word.word785.data_word.channel
                             << " was not specified "
                                // "in a lookup table Cluster_SR_time_xia"
                             << endl;

                    }
                }
                break;

            case 4:
                //         cout << "Footer  when i = .." << i << endl;
                //         counter785modules++;
                break;
            case 6:
                //         cout << "'Not valid datum for this v785 "
                //  << endl;
                //         counter785modules++;

                break;

            default :
                cout << "Error - unknown code in VME time information  when i = .." << i << endl;
                break;
            } // endof inner switch
        } // end else 785
    }
    //     cout << "End of function ====================================" << endl;
    return true;
#else
    return false;
#endif   //CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT

}
//****************************************************************************************
/** No descriptions */
void TIFJEventProcessor::read_ger_plastic_time_lookup_data()
{
#if ((CURRENT_EXPERIMENT_TYPE == RISING_STOPPED_BEAM_CAMPAIGN) ||         (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008)\
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008)  \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ))

    string fname = "./mbs_settings/lookup_table.txt";
    ifstream plik(fname.c_str());
    if(!plik) {
        cout << "!!! Fatal error: I can't open file " << fname << endl;
        exit(1);
    }

    try {
        ger_plastic_LR_time_geo
                = (unsigned int) FH::find_in_file(plik, "Plastic_LR_time");
        plik >> zjedz >> ger_plastic_LR_time_channel;

        // TRAYs

        ge_plastic_time_tray0_geo = (unsigned int) FH::find_in_file(plik, "Plastic_xia_time_tray_0");
        plik >> zjedz >> ge_plastic_time_tray0_channel;

        ge_plastic_time_tray1_geo
                = (unsigned int) FH::find_in_file(plik, "Plastic_xia_time_tray_1");
        plik >> zjedz >> ge_plastic_time_tray1_channel;

        ge_plastic_time_tray2_geo
                = (unsigned int) FH::find_in_file(plik, "Plastic_xia_time_tray_2");
        plik >> zjedz >> ge_plastic_time_tray2_channel;

        try {
            // kind of dummy, Stephan sometimes sends new data
            ge_plastic_time_tray3_geo
                    = (unsigned int) FH::find_in_file(plik, "Plastic_xia_time_tray_3");
            plik >> zjedz >> ge_plastic_time_tray3_channel;
        } catch(...) {
            ge_plastic_time_tray3_geo = 0;
            ge_plastic_time_tray3_channel = 0;
        }

#ifdef ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE
        try {

            // in case if the previous keywords throwed the exception
            plik.clear(plik.rdstate() & ~(ios::failbit | ios::eofbit));

            ger_Silicon_LR_trigger_time_geo
                    = (unsigned int) FH::find_in_file(plik, "Silicon_LR_trigger_time");

            plik >> zjedz >> ger_Silicon_LR_trigger_time_channel;
        } catch(Tfile_helper_exception &m) {
            cout << "Error while reading the file " << fname << "\n\t"
                 << m.message << endl;
            ger_Silicon_LR_trigger_time_geo = 0;
            ger_Silicon_LR_trigger_time_channel = 0;
            exit(0);
        }
#endif

    } catch(Tfile_helper_exception &m) {
        cout << "Error while reading the file " << fname << "\n\t"
             << m.message << endl;
        throw;
    }
#endif

}
//****************************************************************************************
/** No descriptions */
bool TIFJEventProcessor::unpack_munich_sam3_subevent_from_raw_array(
        const_daq_word_t *data, int how_many_words)
{
    //   cout << " F.unpack_munich_sam3_subevent_from_raw_array" << endl;

    // return true;
#if(CURRENT_EXPERIMENT_TYPE==RISING_ACTIVE_STOPPER_APRIL_2008)\
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008)

    //   cout << "To unpack there is " << dec << how_many_words << " words \n";
    //   for(int i = 0 ; i < how_many_words ; i++)
    //   {
    //     cout << "[" << i << "]  0x" << hex <<  data[i]
    //     << ",  dec = " <<  dec <<  data[i] << endl;
    //   }


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

#elif CURRENT_EXPERIMENT_TYPE==RISING_ACTIVE_STOPPER_100TIN\

    //#ifdef TRACKING_IONISATION_CHAMBER_X


#define SPEAK_MUNICH 0
#define DBGM  if(SPEAK_MUNICH)

    DBGM cout << "To unpack unpack_munich_sam3_subevent_from_raw_array " << endl;

    DBGM {
        cout << "To unpack there is " << dec << how_many_words << " words \n";
        for(int i = 0; i < how_many_words; i++) {
            cout << "[" << i << "]  0x" << hex << data[i]
                 << ",  dec = " << dec << data[i] << endl;
        }
    }

    // find the "start word"
    const uint sim_start_word = ('T' << 24) + ('I' << 16) + ('C' << 8) + 'A';      // TICA = TIC Anfang
    const uint sim_stop_word = ('T' << 24) + ('I' << 16) + ('C' << 8) + 'E';      // TICE = TIC Ende

    DBGM cout << "start word is: " << hex << data[0] << dec << endl;

    if(data[0] != sim_start_word) {  // TICAnfang 4655434b)
        //DBGM
        cout << " !!!! ERROR: SIM3 munich subevent has a wrong start_word, skipping subevent " << endl;
        return true; // true, bec other subevent can be valid
    }
    // check if the last word is the "end word"  ??????????
    if(how_many_words == 2 && data[1] == sim_stop_word) {  // "TICE" - end
        static int counter;
        if(!(++counter % 500)) {
            cout << "Warning: so far there are " << counter
                 << " empty  SIM3 munich subevents," << endl;
        }
        return true; // true, bec other subevent can be valid
    }

    // take DSP_HEADER
    struct dsp_header {
        uint length :
            16; //  - Length event length excluding marking words
        uint trigger_counter :
            8;
        uint dsp_numer :
            8; //   - it contains dsp numer DSP
    };
    union {
        daq_word_t raw_data;
        dsp_header dh;
    };

    raw_data = data[1];

    DBGM cout << "dsp header contains ----> "
              << "dsp nr = 0x" << hex << dh.dsp_numer
              << ", trigg counter  = 0x" << hex << dh.trigger_counter
              << ", length (exclu marking words) = " << dec << dh.length
              << endl;

    // for loop over two modules, or untill the end word


    struct subevent_header {
        // additional_header_word
        uint length :
            8; //  - Length event length excluding marking words
        uint reserved :
            4; // to ignore
        uint devcode :
            4; // must be 0xa for GASSIPLEX boards

        // real subevent header
        uint bb_trigger_counter :
            8; //16;//  - Length event length excluding marking words
        uint ts :
            2; // 0 illega, 1 physics, 2 n/a, 3 = pedestal
        uint error :
            1;
        uint board :
            4; // board nr = GTB address of BB
        uint data_flag:
            1;
    };

    union {
        daq_word_t raw_data_seh;
        subevent_header seh;
    };

    int word = 2;
    // raw_data_seh = data[1];

    for(int module = 0; module < 2; module++) {  // module means another TIC device
        // take SEH subevent header
        raw_data_seh = data[word++];

        if(raw_data_seh == sim_stop_word
                ||
                data[word] == sim_stop_word) { // this is already the next word
            //       int m = 66;
            //     m = m+6;
            break;
        }

        DBGM cout << "subevent header contains ----> "
                  << "data_flag = 0x" << hex << seh.data_flag
                  << ", board = 0x" << hex << seh.board
                  << ", error = 0x" << hex << seh.error
                  << ", ts = 0x" << hex << seh.ts
                  << ", bb_trigger_counter = " << dec << seh.bb_trigger_counter
                  << endl;

        if(seh.data_flag == 0)
            continue; // no data for this module was sent (empty)
        // so the additional header word is also empty, this is why we just make the contine


        //  raw_data_ahw = data[word++];

        DBGM cout << "additional_header_word contains ----> "
                  << "devcode = 0x" << hex << seh.devcode
                  << ", length = " << dec << seh.length
                  << "+ 1 !!!"
                  << endl;

        if(seh.devcode != 0xa) {  // devcode should alway be 0xa  for real data (seh.data_flag is 1 )
            // DBGM
            cout << " Wrong structure of  'munich subevent'. "
                    " (The subeven devcode (seh.devcode) is not 0xa !, while seh.data_flag is true), ignoring this  subevent " << endl;
            //continue;
            return true; // false; otherwise all other subevent (frs  etc are discarded)
        }

        // +1 below is according the the description given to me by Michael
        // now we are analysing the short words

        //  unsigned short int *ptr = static_cast<unsigned short int *>(&data[word]);
        unsigned short int *ptr = (unsigned short int *)(&data[word]);

        uint d = 0;
        for(d = 0; d < seh.length + 1; d++, ptr++) {
            struct channel_bits_for_reordering {
                uint :
                8 + 4;
                uint two_bits :
                    2;
            };
            struct dane {
                uint value :
                    8;
                uint channel :
                    8;
            };
            union {
                uint dana_raw;
                dane ds;
                channel_bits_for_reordering reorder;
            };

            dana_raw = *ptr;
            // the channel have to be "translated"
            int orig_chann = ds.channel;

            //     cout << "BEFORE REORDER Data nr " << d << ") 0x"
            //       << hex << dana_raw << dec
            //       << ",      value = 0x" << ds.value
            //       << ", value dec = " << dec << ds.value
            //       << " for module = " << module
            //       << ", channel = " << ds.channel
            //       << ", orig_chann= " <<orig_chann
            //       << endl;


            int re = reorder.two_bits;

            switch(re) {
            case 2 :
                re = 0;
                break;
            case 3 :
                re = 1;
                break;
            case 1 :
                re = 2;
                break;
            case 0 :
                re = 3;
                break;
            }
            reorder.two_bits = re;

            DBGM cout << "AFTER REORDER Data nr " << d << ") 0x"
                      << hex << dana_raw << dec
                      << ",      value = 0x" << ds.value
                      << ", value dec = " << dec << ds.value
                      << " for module = " << module
                      << ", reordered channel = " << ds.channel
                      << ", original channel= " << orig_chann
                      << endl;

            //Michael reordering the wires
            int true_channel = ds.channel;
            if(ds.channel > 223)
                true_channel = (255 - ds.channel) + 224;

            if(ds.value) {  // i.e. non zero
                switch(module) {
                case 0 :
                    target_event->tic21_data[true_channel] = ds.value;
                    target_event->tic21_fired = true;
                    break;
                case 1 :
                    target_event->tic22_data[true_channel] = ds.value;
                    target_event->tic22_fired = true;
                    break;
                }

            }
        }// end for dane
        DBGM cout << "There was " << d << "data words " << endl;
        if(d % 2)
            d++;

        int how_much_to_skip = d / 2;
        DBGM cout << "so we need to skip longwords " << how_much_to_skip << endl;
        word += how_much_to_skip;

    } // end for modules
    DBGM cout << "here should be the stop word " << hex << data[word] << dec << endl;

#endif

    return true; // if successful
}
//****************************************************************************************
/** No descriptions */
int TIFJEventProcessor::decode_munich_absorber_data(const daq_word_t *data,
                                                    const uint sim_stop_word)
// unpack_munich_sam3_subevent_from_raw_array(const_daq_word_t *data,int how_many_words)
{
#ifdef POSITRON_ABSORBER_PRESENT

    //cout << " F.unpack_munich_sam3_subevent_from_raw_array======================" << endl;


#define DBGA  if(0)

    DBGA cout << "decode_munich_absorber_data " << endl;

    //   DBGA
    //   {
    //     cout << "To unpack there is " << dec << how_many_words << " words \n";
    //     for(int i = 0 ; i < how_many_words ; i++)
    //   {
    //     cout << "[" << i << "]  0x" << hex <<  data[i]
    //       << ",  dec = " <<  dec <<  data[i] << endl;
    //     }
    //   }

    // find the "start word"
    DBGA cout << "start word is: " << hex << data[0] << dec << endl;

    //   if(data[0] != 0x4655434b)
    //   {
    //     DBGA cout << " !!!! No start word, skipping subevent " << endl;
    //     // return false;
    //   }

    // check if the last word is the "end word"  ??????????
    if(data[1] == sim_stop_word) {
        DBGA cout << " !!!! Empty contents" << endl;
        return 1; // skip one word ("end word);
    }
    // take DSP_HEADER
    struct dsp_header {
        uint length :
            16; //  - Length event length excluding marking words
        uint trigger_counter :
            8;
        uint dsp_numer :
            8; //   - it contains dsp numer DSP
    };
    union {
        daq_word_t raw_data;
        dsp_header dh;
    };

    raw_data = data[1];

    DBGA cout << "dsp header contains ----> "
              << "dsp nr = 0x" << hex << dh.dsp_numer
              << ", trigg counter  = 0x" << hex << dh.trigger_counter
              << ", length (exclu marking words) = " << dec << dh.length
              << endl;

    // for loop over two modules, or untill the end word


    struct subevent_header {
        // additional_header_word
        uint length :
            8; //  - Length event length excluding marking words
        uint reserved :
            4; // to ignore
        uint devcode :
            4; // must be 0xa for GASSIPLEX boards

        // real subevent header
        uint bb_trigger_counter :
            8;//  - Length event length excluding marking words
        uint ts :
            2; // 0 illega, 1 physics, 2 n/a, 3 = pedestal
        uint error :
            1;
        uint board :
            4; // board nr = GTB address of BB
        uint data_flag:
            1;
    };

    union {
        daq_word_t raw_data_seh;
        subevent_header seh;
    };

    int word = 2;
    // raw_data_seh = data[1];

    for(int module = 0; module < 2; module++) {  // module means another TIC device
        // take SEH subevent header
        raw_data_seh = data[word++];
        // check if the last word is the "end word"  ??????????
        if(raw_data_seh == sim_stop_word) {
            DBGA cout << " !!!! No data for module " << module << endl;
            return word - 1; // skip words ("end word);
        }
        DBGA cout << "-----------Module nr " << module << endl;

        DBGA cout << "subevent header " << hex << raw_data_seh << "  contains ----> "
                  << "data_flag = 0x" << hex << seh.data_flag
                  << ", board = 0x" << hex << seh.board
                  << ", error = 0x" << hex << seh.error
                  << ", ts = 0x" << hex << seh.ts
                  << ", bb_trigger_counter = " << dec << seh.bb_trigger_counter
                  << endl;

        //  raw_data_ahw = data[word++];

        if(seh.data_flag == 0) {
            DBGA cout << "Data flag == 0, so skipping " << endl;
            continue; // empty module
        }
        DBGA cout << "additional_header_word contains ----> "
                  << "devcode = 0x" << hex << seh.devcode
                  << ", length = " << dec << seh.length
                  << "+ 1 !!!"
                  << endl;

        if(seh.devcode != 0xa) {
            DBGA cout << " The seh.devcode is not 0xa, skipping subevent " << endl;
            return false;
        }

        // +1 below is according the the decrition given to me by Michael
        // now we are analysing the short words

        //  unsigned short int *ptr = static_cast<unsigned short int *>(&data[word]);
        unsigned short int *ptr = (unsigned short int *)(&data[word]);

        uint d = 0;
        for(d = 0; d < seh.length + 1; d++, ptr++) {
            struct channel_bits_for_reordering {
                uint :
                8 + 4;
                uint two_bits :
                    2;
            };
            struct dane {
                uint value :
                    8;
                uint channel :
                    8;
            };
            union {
                uint dana_raw;
                dane ds;
                channel_bits_for_reordering reorder;
            };

            dana_raw = *ptr;
            // the channel have to be "translated"

            //int orig_chann = ds.channel ;

            //     cout << "BEFORE REORDER Data nr " << d << ") 0x"
            //       << hex << dana_raw << dec
            //       << ",      value = 0x" << ds.value
            //       << ", value dec = " << dec << ds.value
            //       << " for module = " << module
            //       << ", channel = " << ds.channel
            //       << ", orig_chann= " <<orig_chann
            //       << endl;


            int re = reorder.two_bits;

            switch(re) {
            case 2 :
                re = 0;
                break;
            case 3 :
                re = 1;
                break;
            case 1 :
                re = 2;
                break;
            case 0 :
                re = 3;
                break;
            }
            reorder.two_bits = re;

            DBGA
                    cout << "Data nr " << d << ") 0x"
                         << hex << dana_raw
                         << ",      value = 0x" << hex << ds.value
                         << ", (dec = " << dec << ds.value
                         << ") for module = " << module
                         << ", reordered channel = " << ds.channel
                            //<< ", original channel= " <<orig_chann
                         << ", brigde board address = " << seh.board
                         << endl;

            int front_or_rear = 0, x_y_plate = 0, stripe = 0;

            // if(ds.value) // i.e. non zero
            {
                switch(seh.board) {
                case 0: // this board gives data for the absorber

                    if(lookup_absorber_2_9.current_combination(
                                seh.board,
                                ds.channel,
                                &front_or_rear, &x_y_plate, & stripe)) {

                        DBGA
                                cout << " --- this is for front absorber  "
                                     << " Front_or_Rear = " << front_or_rear
                                     << ", plate= " << x_y_plate
                                     << ", stripe= " << stripe
                                     << ", data = " << ds.value
                                        //<< flush  ;  //
                                     << endl;

                        switch(front_or_rear) {
                        case 0:
                            target_event->positron_absorber_front[x_y_plate][stripe] = ds.value;
                            target_event->positron_absorber_front_fired[x_y_plate] = true;
                            break;

                        case 1:
                            target_event->positron_absorber_rear[x_y_plate][stripe] = ds.value;
                            target_event->positron_absorber_rear_fired[x_y_plate] = true;
                            break;
                        }

                    } else {
                        /*
                         cout
                         << "!!!! In the lookup table for absobrers there is no combination:  "
                         << " Module = " << module << ", Board ="
                         << seh.board << " Channel = " << ds.channel
                         << endl;
                         */
                    }
                    break;

                case 1: { // bridge board addres 0x1
                    int egzempl = 0;
                    x_y_plate = 0 , stripe = 0;
                    if(lookup_active_stopper_x.current_combination(
                                seh.board,
                                ds.channel,
                                &egzempl, &x_y_plate, & stripe)) {

                        DBGA
                                cout << " --- this is for lookup_active_stopper_x   "
                                     << "[Board= " << seh.board
                                     << ", channel= " << ds.channel
                                     << "] ==> Front_or_Rear = " << front_or_rear
                                     << ", plate= " << x_y_plate
                                     << ", stripe= " << stripe
                                     << ", data = " << ds.value
                                        //<< flush  ;  //
                                     << endl;

                        target_event->munich_active_stopper
                                [egzempl]
                                [(x_y_plate * NR_OF_MUN_STOPPER_STRIPES_X) + stripe] = ds.value;

                        target_event->munich_active_stopper_fired[egzempl][x_y_plate] = true;

                        break;

                    } // if lookup
                    else {
                        /*       cout
                         << "!!!! In the lookup table lookup_active_stopper_x    there is no combination:  "
                         << " Module = " << module << ", Board ="
                         << seh.board << " Channel = " << ds.channel
                         << endl;
                         */
                    }
                } // local

                    break;
                } // end if switch(board )

            } // if nonzero data
        }// end for data

        DBGA cout << "There was " << d << "data words " << endl;
        if(d % 2)
            d++;

        int how_much_to_skip = d / 2;
        DBGA cout << "so we need to skip longwords " << how_much_to_skip << endl;
        word += how_much_to_skip;

    } // end for modules
    DBGA cout << "here should be the stop word " << hex << data[word] << dec << endl;
    return word - 1; // if successful

#else

    return 0; // if successful
#endif

}
//****************************************************************************************
void TIFJEventProcessor::read_munich_lookup()
{
#ifdef MUN_ACTIVE_STOPPER_PRESENT

    // as there can be different nr of strinps for X and different for Y
    // here we make a trick. At first we read the lookup table for X only,
    // with the certain number of X stripes.

    // Later we do it once more, but skipping already read X items,
    // now with Y giving the right (different) nr of Y stripes

    // At first we read the X data - which comes from DSP Board

    vector<string> chain_one;
    for(unsigned int i = 0; i < NR_OF_MUN_STOPPER_MODULES; i++) {  // <-- so many of them
        chain_one.push_back(string("stopper") + char('A' + i) + "_");
    }

    vector<string> chain_two;
    chain_two.push_back("x_");    // only X strips <---

    vector<string> chain_three;
    for(unsigned int i = 0; i < NR_OF_MUN_STOPPER_STRIPES_X; i++) {  // <-- so many of them
        ostringstream s;
        s << setw(2) << setfill('0') << i << "_energy";
        chain_three.push_back(s.str());
    }

    //   cout << "Trying to read  " << "./mbs_settings/stopper_lookup_table.txt" << endl;

    lookup_active_stopper_x.read_from_disk("./mbs_settings/tin100_lookup_table.txt",
                                           chain_one, chain_two, chain_three, false);

    // Y stripes come from different electronics, so it is better to have
    // the different lookup table !!!!!!!!!!!


    chain_two.clear();
    chain_two.push_back("y_");

    chain_three.clear();

    for(unsigned int i = 0; i < NR_OF_MUN_STOPPER_STRIPES_Y; i++) {
        ostringstream s;
        s << setw(2) << setfill('0') << i << "_energy";
        chain_three.push_back(s.str());
    }

    lookup_active_stopper_y.read_from_disk("./mbs_settings/tin100_lookup_table.txt",
                                           chain_one, chain_two, chain_three, false);

    //=========== To neglect the noise =============

    fake_lookup_noise_to_neglect.clear();
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (5, 15));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (5, 31));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (7, 15));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (7, 31));

    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, 1));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, 3));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, 5));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, 7));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, 8));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, 10));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, 12));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, 14));
    for(int i = 16; i < 32; i++)
        fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (9, i));

    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (13, 14));
    fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (13, 15));
    for(int i = 30; i < 32; i++)
        fake_lookup_noise_to_neglect.push_back(pair<uint, uint> (13, i));

    //===============================
    //  lookup_multiplicity_module
    chain_one.clear();
    chain_one.push_back(string("munich_multiplicity_module"));
    chain_two.clear();
    chain_two.push_back("_first_channel");    // we ask only for channel 0, the next are 1:1
    //   for(unsigned int i = 0 ; i < 1 ; i++)
    //   {
    //     ostringstream s ;
    //     s << "_channel_" << setw(2) << setfill('0') << i ;
    //     chain_two.push_back(s.str()) ;
    //   }
    chain_three.clear();
    chain_three.push_back("_starts");
    lookup_multiplicity_module.read_from_disk("./mbs_settings/tin100_lookup_table.txt",
                                              chain_one, chain_two, chain_three, false);

#endif

}
//****************************************************************************************
bool TIFJEventProcessor::unpack_munich_timing_module_subevent(
        const daq_word_t *long_data, int how_many_words)
{
#if (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_100TIN ) \
    || (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_APRIL_2008 )

    // Henning says that here there are 4 Caen V775 TDCs (range 1.2 us)
    // and the old_long range CAEN TDC V767 - which has 128 channels in one card
#undef SPEAK_TIMING

#define SPEAK_TIMING 0
#define DBGT  if(SPEAK_TIMING)

    DBGT {
        cout << "To unpack there are:  " << how_many_words << " words \n";

        for(int i = 0; i < how_many_words; i++) {
            cout << "[" << i << "]  0x" << hex << long_data[i]
                    //       << ",  dec = "
                 << dec
                    //       <<  long_data[i]
                 << endl;
        }
    }

    /* is i header
     /////////////////////////////////////////
     class vme767_header_word
     {
     public:
     unsigned int evnet_nr: 12;
     unsigned int         : 9;
     //unsigned int header  : 1;
     unsigned int code    : 2;
     unsigned int         : 4;
     unsigned int geo     : 5 ;
     };
     /////////////////////////////////////////
     class vme767_data_word
     {
     public:
     unsigned int time_data  : 20;
     unsigned int edge       : 1;
     // unsigned int eob        : 1;
     // unsigned int header     : 1;
     unsigned int code    : 2;
     unsigned int start      : 1;
     unsigned int channel    : 7;
     };
     /////////////////////////////////////////
     class vme767_eob_word
     {
     public:
     unsigned int ev_counter   : 16 ;
     unsigned int  : 5 ;
     // unsigned int eob        : 1;
     // unsigned int header     : 1;
     unsigned int code    : 2;
     unsigned int  : 1 ;
     unsigned int status : 3 ;
     unsigned int geo     : 5 ;
     };
     /////////////////////////////////////////
     /////////////////////////////////////////
     union vme767_word
     {
     daq_word_t           raw_word ;
     vme767_header_word      header_word ;
     vme767_data_word        data_word ;
     vme767_eob_word         end_of_block_word ;
     };

     union general_vme_word
     {
     vme767_word word767;
     vme_word    word775;
     };

     --------------------*/

    // at first there are VME 775 VME data ########################
    // loop over unknown nr of words

    general_vme_word word;

    int counter775modules = 0;
    // zero was used, now we start from 1
    unsigned int current_geo = 0;

    enum typ_of_block {typ_vme775, typ_vme767};
    typ_of_block current_vme_blok = typ_vme775;

    /*
     // Trap, for debugging purposes
     if(how_many_words != 3 ||
     long_data[0] != 0x6000000
     || long_data[1] != 0x600000
     || long_data[2] != 0x600000
     )
     {
     cout
     << "There is more data, than just 3"
     << endl;
     }
     */

    for(int i = 0; i < how_many_words; i++) {
        DBGT cout << "Unpacking word " << i << ": 0x"
                  << hex << long_data[i] << dec << endl;
        word.word775.raw_word = long_data[i];
        if(!word.word775.raw_word) {  // do not analyse empty words
            continue;
        }

        // Sometimes Henning is sending the extra word on the end of the VME block
        // with the pattern 0xbabe****
        if((word.word775.raw_word & 0xffff0000) == 0xbabe0000) {
            DBGT cout << "babe word ==============" << endl;
            break; // end of data at all, so end of the loop
        }

        // at first there will be information from the 0NE  775 block
        // and then information from ONE  767

        if(counter775modules >= 1)
            current_vme_blok = typ_vme767;

        switch(current_vme_blok) {
        case typ_vme775: //-----------------------------------------------------------------
            // Sometimes Henning is sending the extra word on the end of the VME block
            // with the pattern 0xbabe****

            switch(word.word775.header_word.code) {  // <-- it is common to all of types of vme words
            case 2: {
                DBGT cout << "Caen V775 TDC Header .." << endl;
            }
                DBGT cout << "babe word ==============" << endl;
                break;

            case 0:
                // sizes are hardcoded in the class definition
                if(word.word775.data_word.geo < 22 && word.word775.data_word.channel < 32) {
                    DBGT cout << "Arrived TIME geo = " << word.word775.data_word.geo
                              << ", chan = " << word.word775.data_word.channel
                              << ", data = " << word.word775.data_word.data
                              << endl;

                    // Stephan says that it must be GEO = 20
                    if(word.word775.data_word.geo == 20) {
                        // storing the time data
                        // no need to we reduce the  resolution from 64 KB to 8KB !!!
                        target_event->munich_timer775[word.word775.data_word.channel ] =
                                word.word775.data_word.data;
                    }

                }
                break;

            case 4:
                DBGT cout << "Footer  when i = .." << i << endl;
                counter775modules++;
                break;

            case 6:
                DBGT cout << "'Not valid datum for this TDC when i = .." << i
                          << " vme module nr " << counter775modules << " didn't fired " << endl;
                counter775modules++;
                break;

            default :
                DBGT cout << "Error - unknown code in VME time information  when i = .." << i << endl;
                break;
            } // endof inner switch

            break;

            // ########################################################################
        case typ_vme767: //---------------------------------------------------------------------

            switch(word.word767.header_word.code) {  // <-- it is common to all of types of vme words

            case 2: {
                DBGT cout << "Caen TDC V767 Header ------------------------" << endl;
                current_geo = word.word767.header_word.geo;
            }
                break;

            case 0:
                // sizes are hardcoded in the class definition
                if(current_geo == 21 && word.word767.data_word.channel < 128) {

                    DBGT cout << "Arrived TIME geo = " << current_geo
                              << ", chan = " << word.word767.data_word.channel
                              << ", data = " << word.word767.data_word.time_data
                              << endl;

                    target_event->munich_timer767 [word.word767.data_word.channel] =
                            word.word767.data_word.time_data;

                } else {
                    cout << "F. unpack_munich_timing_module_subevent(). Unknown GEO = "
                         << current_geo << " arrived for V767" << endl;
                }
                break;

            case 1:
                DBGT cout << "Footer  when i = .." << i << endl;
            {
                // local scope
                static int how_many_errors;
                if(word.word767.end_of_block_word.status) {
                    how_many_errors++;
                    if(flag_accept_single_ger && flag_accept_single_dgf) {
                        static time_t last_message = time(NULL);
                        if((time(NULL) - last_message > 5) && (how_many_errors > 500)) {         // every 5 seconds
                            last_message = time(NULL);
                            cout << "Error status in Caen 767 block (which deliveres LR time). Please reset it"
                                 << endl;

                            //cout << "This is sorting online " << endl;

                            ofstream plik("./commands/synchronisation_lost.txt");
                            plik
                                    << "Long Range TDC status Watchdog barks !\n"
                                    << "The Status bitfield in NON ZERO.\n  "
                                    << "\nNote: - If you do not care about this (for example you are interested only in FRS)\n"
                                       "Please go:\n"
                                       "Spy_options->Matching of MBS subevent->Modify above parameters (wizard)->5 branches [next]->\n"
                                       "->Do not make matching [next]-> here please uncheck the fields 'GER' and 'DGF',\n"
                                       "Then finish the wizard (and update spy if it is running)"
                                    << endl;
                            plik.close();

                        } // every 5 seconds
                        DBGT cout << "end of unpacking the subevent ==============" << endl;
                        return false;
                    }

                } else { // no error in status
                    how_many_errors = 0;
                }
            } // locaal
                break;
            case 3:
                DBGT cout << " 'Not valid datum for this TDC when i = .." << i << endl;
                break;

            default :
                DBGT cout << "no Code,  i = .." << i << endl;
                break;
            }

            break;

        }// end of  switch(current_vme_blok)

    } // end of for i
    DBGT cout << "end of unpacking the subevent ==============" << endl;
#endif

    return true;

}
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

#if  (CURRENT_EXPERIMENT_TYPE == RISING_ACTIVE_STOPPER_JULY_2008)
//****************************************************************************************
bool TIFJEventProcessor::unpack_procid45_v767_and_775(
        const daq_word_t *long_data,
        int how_many_words)
{

    // Henning says that here there are 4 Caen V775 TDCs (range 1.2 us)
    // and the old_long range CAEN TDC V767 - which has 128 channels in one card
#undef SPEAK_TIMING

#define SPEAK_TIMING 0
#define DBGT  if(SPEAK_TIMING)

    DBGT {
        cout << "To unpack there are:  " << how_many_words << " words \n";
        for(int i = 0; i < how_many_words; i++) {
            cout << "[" << i << "]  0x" << hex << long_data[i]
                    //       << ",  dec = "
                 << dec
                    //       <<  long_data[i]
                 << endl;
        }
    }

    /* is i header
     /////////////////////////////////////////
     class vme767_header_word
     {
     public:
     unsigned int evnet_nr: 12;
     unsigned int         : 9;
     //unsigned int header  : 1;
     unsigned int code    : 2;
     unsigned int         : 4;
     unsigned int geo     : 5 ;
     };
     /////////////////////////////////////////
     class vme767_data_word
     {
     public:
     unsigned int time_data  : 20;
     unsigned int edge       : 1;
     // unsigned int eob        : 1;
     // unsigned int header     : 1;
     unsigned int code    : 2;
     unsigned int start      : 1;
     unsigned int channel    : 7;
     };
     /////////////////////////////////////////
     class vme767_eob_word
     {
     public:
     unsigned int ev_counter   : 16 ;
     unsigned int  : 5 ;
     // unsigned int eob        : 1;
     // unsigned int header     : 1;
     unsigned int code    : 2;
     unsigned int  : 1 ;
     unsigned int status : 3 ;
     unsigned int geo     : 5 ;
     };
     /////////////////////////////////////////
     /////////////////////////////////////////
     union vme767_word
     {
     daq_word_t           raw_word ;
     vme767_header_word      header_word ;
     vme767_data_word        data_word ;
     vme767_eob_word         end_of_block_word ;
     };

     union general_vme_word
     {
     vme767_word word767;
     vme_word    word775;
     };

     --------------------*/

    // at first there are VME 775 VME data ########################
    // loop over unknown nr of words

    general_vme_word word;

    //int counter775modules = 0;
    // zero was used, now we start from 1
    unsigned int current_geo = 0;

    //  enum  typ_of_block  { unknown, typ_vme775, typ_vme767 };
    //  typ_of_block        current_vme_blok = typ_vme775;

    /*
     // Trap, for debugging purposes
     if(how_many_words != 3 ||
     long_data[0] != 0x6000000
     || long_data[1] != 0x600000
     || long_data[2] != 0x600000
     )
     {
     cout
     << "There is more data, than just 3"
     << endl;
     }
     */

    enum enum_what_type_of_block {unknown, typ_vme767, typ_vme775} current_vme_blok;
    current_vme_blok = unknown;

    for(int i = 0; i < how_many_words; i++) {
        DBGT cout << "Unpacking word " << i << ": 0x"
                  << hex << long_data[i] << dec << endl;

        word.word775.raw_word = long_data[i];
        if(!word.word775.raw_word) {  // do not analyse empty words
            continue;
        }

        // Sometimes Henning is sending the extra word on the end of the VME block
        // with the pattern 0xbabe****
        if((word.word775.raw_word & 0xffff0000) == 0xbabe0000) {
            DBGT cout << "babe word ==============" << endl;
            break; // end of data at all, so end of the loop
        }

        // at first there will be information from the 0NE  775 block
        // and then information from ONE  767

        if(current_vme_blok == unknown) {

            // try to recognize the block by looking at the header code
            if(word.word775.header_word.code == 2) {  // <-- it is common to all of types of vme words
                current_vme_blok = typ_vme775;
            } else if(word.word767.header_word.code == 2) {
                current_vme_blok = typ_vme767;
            } else
                continue; // ???
        }

        switch(current_vme_blok) {
        case unknown:
        default :
            continue; // because unk

        case typ_vme775: //-----------------------------------------------------------------
            // Sometimes Henning is sending the extra word on the end of the VME block
            // with the pattern 0xbabe****

            switch(word.word775.header_word.code) {  // <-- it is common to all of types of vme words
            case 2:

                DBGT cout << "Caen V775 TDC Header .., geo = " << word.word775.header_word.geo << endl;
                break;

            case 0:
                // sizes are hardcoded in the class definition
                //        if(word.word775.data_word.geo < 22  &&word.word775.data_word.channel < 32)
            {
                DBGT cout << "Arrived V775 TIME geo = " << word.word775.data_word.geo
                          << ", chan = " << word.word775.data_word.channel
                          << ", data = " << word.word775.data_word.data
                          << endl;

                int module = 0;
                int x_y_plate = 0;
                int stripe = 0;
#ifdef ACTIVE_STOPPER_PRESENT
                // STOPPER A ==========================
                if(lookup_active_stopper_time.current_combination(
                            word.word775.data_word.geo,
                            word.word775.data_word.channel,
                            &module, &x_y_plate, & stripe)) {
                    DBGT
                            cout << " --- this is TIME information for stopper "
                                 << " det = " << module
                                 << ", x_y_plate= " << x_y_plate
                                 << ", stripe= " << stripe
                                 << ", data = " << word.word775.data_word.data
                                    //<< flush  ;  //
                                 << endl;

                    //#ifdef NIGDY
                    // storing the time data
                    target_event->active_stopper_time[module]
                            [(x_y_plate * NR_OF_STOPPER_STRIPES_X) + stripe] =
                            word.word775.data_word.data;
                    target_event->active_stopper_fired[module][x_y_plate] = true;
                    //#endif
                } else
#endif
#ifdef ACTIVE_STOPPER2_PRESENT
                    if(lookup_active_stopper2_time.current_combination(
                                word.word775.data_word.geo,
                                word.word775.data_word.channel,
                                &module, &x_y_plate, & stripe)) {
                        DBGT
                                cout << " --- this is TIME information for stopper2 "
                                     << " det = " << module
                                     << ", x_y_plate= " << x_y_plate
                                     << ", stripe= " << stripe
                                     << ", data = " << word.word775.data_word.data
                                        //<< flush  ;  //
                                     << endl;

                        // storing the time data
                        target_event->active_stopper2_time[module]
                                [(x_y_plate * NR_OF_STOPPER_STRIPES_X) + stripe] =
                                word.word775.data_word.data;
                        target_event->active_stopper2_fired[module][x_y_plate] = true;
                    } else
#endif
#ifdef ACTIVE_STOPPER3_PRESENT
                        if(lookup_active_stopper3_time.current_combination(
                                    word.word775.data_word.geo,
                                    word.word775.data_word.channel,
                                    &module, &x_y_plate, & stripe)) {
                            DBGT
                                    cout << " --- this is TIME information for stopper3 "
                                         << " det = " << module
                                         << ", x_y_plate= " << x_y_plate
                                         << ", stripe= " << stripe
                                         << ", data = " << word.word775.data_word.data
                                            //<< flush  ;  //
                                         << endl;

                            // storing the time data
                            target_event->active_stopper3_time[module]
                                    [(x_y_plate * NR_OF_STOPPER_STRIPES_X) + stripe] =
                                    word.word775.data_word.data;
                            target_event->active_stopper3_fired[module][x_y_plate] = true;
                        } else
#endif

                        {
                            static int licznik;
                            if(!((licznik++) % 1000))
                                cout << "unpacking Caen V775.  Combination   GEO = " << current_geo
                                     << ", channel = " << word.word775.data_word.channel
                                     << " is unknown for active stopper lookup table (time) " << endl;
                        }

            }// local
                break;

            case 4:
                DBGT cout << "Footer  when i = .." << i << endl;
                current_vme_blok = unknown; // so the next has to be recognized again
                //counter775modules++;
                break;

            case 6:
                DBGT cout << "'Not valid datum for this TDC when i = .." << i
                             //         << " vme module nr " << counter775modules
                             //         << " didn't fired "
                          << endl;
                //counter775modules++;
                break;

            default :
                DBGT cout << "Error - unknown code in VME time information  when i = .." << i << endl;
                break;
            } // endof inner switch

            break;

            // ########################################################################
        case typ_vme767: //---------------------------------------------------------------------

            switch(word.word767.header_word.code) {  // <-- it is common to all of types of vme words
            case 2: {
                current_geo = word.word767.header_word.geo;
                DBGT cout << "Caen TDC V767 Header ------------------------, geo = " << current_geo << endl;
            }
                break;

            case 0:
                //        if(current_geo <  25  && word.word767.data_word.channel <128)
            {
                // local

                DBGT cout << "Arrived TIME geo = " << current_geo
                          << ", chan = " << word.word767.data_word.channel
                          << ", data = " << word.word767.data_word.time_data
                          << endl;

                int module = 0;
                int x_y_plate = 0;
                int stripe = 0;

#ifdef ACTIVE_STOPPER_PRESENT
                // STOPPER A ==========================
                if(lookup_active_stopper_time.current_combination(
                            current_geo,
                            word.word767.data_word.channel,
                            &module, &x_y_plate, & stripe)) {
                    DBGT
                            cout << " --- this is TIME information for stopper "
                                 << " det = " << module
                                 << ", x_y_plate= " << x_y_plate
                                 << ", stripe= " << stripe
                                 << ", data = " << word.word767.data_word.time_data
                                    //<< flush  ;  //
                                 << endl;

                    // storing the time data
                    target_event->active_stopper_time[module]
                            [(x_y_plate * NR_OF_STOPPER_STRIPES_X) + stripe] =
                            word.word767.data_word.time_data;
                    target_event->active_stopper_fired[module][x_y_plate] = true;

                } else
#endif
#ifdef ACTIVE_STOPPER2_PRESENT
                    if(lookup_active_stopper2_time.current_combination(
                                current_geo,
                                word.word767.data_word.channel,
                                &module, &x_y_plate, & stripe)) {
                        DBGT
                                cout << " --- this is TIME information for stopper2 "
                                     << " det = " << module
                                     << ", x_y_plate= " << x_y_plate
                                     << ", stripe= " << stripe
                                     << ", data = " << word.word767.data_word.time_data
                                        //<< flush  ;  //
                                     << endl;

                        // storing the time data
                        target_event->active_stopper2_time[module]
                                [(x_y_plate * NR_OF_STOPPER_STRIPES_X) + stripe] =
                                word.word767.data_word.time_data;
                        target_event->active_stopper2_fired[module][x_y_plate] = true;
                    } else
#endif
#ifdef ACTIVE_STOPPER3_PRESENT
                        if(lookup_active_stopper3_time.current_combination(
                                    current_geo,
                                    word.word767.data_word.channel,
                                    &module, &x_y_plate, & stripe)) {
                            DBGT
                                    cout << " --- this is TIME information for stopper3 "
                                         << " det = " << module
                                         << ", x_y_plate= " << x_y_plate
                                         << ", stripe= " << stripe
                                         << ", data = " << word.word767.data_word.time_data
                                            //<< flush  ;  //
                                         << endl;

                            // storing the time data
                            target_event->active_stopper3_time[module]
                                    [(x_y_plate * NR_OF_STOPPER_STRIPES_X) + stripe] =
                                    word.word767.data_word.time_data;
                            target_event->active_stopper3_fired[module][x_y_plate] = true;
                        } else
#endif

                        {
                            static int licznik;
                            if(!((licznik++) % 1000))
                                cout << "unpacking Caen V767.  Combination   GEO = " << current_geo
                                     << ", channel = " << word.word767.data_word.channel
                                     << " is unknown for active stopper lookup table (time) " << endl;
                        }
            } // local
                break;

            case 1:
                DBGT cout << "Footer  when i = .." << i << endl;
                current_vme_blok = unknown; // so the next has to be recognized again
            {
                // local scope
                static int how_many_errors;
                if(word.word767.end_of_block_word.status) {
                    how_many_errors++;
                    if(flag_accept_single_ger && flag_accept_single_dgf) {
                        static time_t last_message = time(NULL);
                        if((time(NULL) - last_message > 5) && (how_many_errors > 500)) {         // every 5 seconds
                            last_message = time(NULL);
                            cout << "Error status in Caen 767 block (which deliveres LR time). Please reset it"
                                 << endl;

                            //cout << "This is sorting online " << endl;

                            ofstream plik("./commands/synchronisation_lost.txt");
                            plik
                                    << "Long Range TDC status Watchdog barks !\n"
                                    << "The Status bitfield in NON ZERO.\n  "
                                    << "\nNote: - If you do not care about this (for example you are interested only in FRS)\n"
                                       "Please go:\n"
                                       "Spy_options->Matching of MBS subevent->Modify above parameters (wizard)->5 branches [next]->\n"
                                       "->Do not make matching [next]-> here please uncheck the fields 'GER' and 'DGF',\n"
                                       "Then finish the wizard (and update spy if it is running)"
                                    << endl;
                            plik.close();

                        } // every 5 seconds
                        DBGT cout << "end of unpacking the subevent ==============" << endl;
                        return false;
                    }

                } else { // no error in status
                    how_many_errors = 0;
                }
            } // locaal
                break;
            case 3:
                DBGT cout << " 'Not valid datum for this TDC when i = .." << i << endl;
                break;

            default :
                DBGT cout << "no Code,  i = .." << i << endl;
                break;
            }

            break;

        }// end of  switch(current_vme_blok)

    } // end of for i
    DBGT cout << "end of unpacking the subevent type 45 ==============" << endl;
    return true;

}
//****************************************************************************************
#endif
void *TIFJEventProcessor::GetInputEvent()  // virtual f. from the abstract class
{
    cout
            << "TIFJEventProcessor::GetInputEvent()  Getting the input event from the file, or from the data stream and "
               "returning the pointer to the event (EMPTY yet!!!!)" << endl;

    return NULL;
}

//****************************************************************************************
//****************************************************************************************
#if CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT

/** This function is unpacking  directly  from the LMD data block  */
bool TIFJEventProcessor::unpack_prisma_event(const_daq_word_t *data, int how_many_words)
{
    //   cout << " F.  unpack_Prisma_event" << endl;

#if 0
    //  cout << "So far fake Prisma_event unpacking " << endl;
    return true;
#else

    // NOTE: This is a stuff coppied from pisolo function. It has to be written to adopt Prisma data


    // warning: this function should not destroy this was is done by other frs_user function


    vme_775_word word;
    if(flag_vmeOne_zeroing_needed) { // to zeroing before frs or frs_user unpacking function
        memset(&vmeOne[0][0], 0, sizeof(vmeOne));
        flag_vmeOne_zeroing_needed = false;
    }

    int header_geo = -1;
    for(int i = 1; i < how_many_words - 1; i++) { // seems that in the first word is a length of the block
        word.raw_word = data[i];

        if(!word.raw_word) { // do not analyse empty words
            // just skip, do not complain
            continue;
        }

        switch(word.header_word.code)  // <-- it is common to all of
            // types of vme words
        {
        case 2: // this is a header
            //       cout << "This is the vme header ..........."               // normally we do not care, but if it is geo 6...
            //       << " geo = " << word.header_word.geo
            //       << ", word nr " << i
            //       << endl;

            header_geo = word.header_word.geo;

#ifdef NIGDY            // ------------------------------------------------------------------------

            if(word.header_word.geo == 6) {
                // this block nr 6 works with different convention.
                // Data words just follow header by many (32) raw data words
                for(unsigned int k = 0; k < word.header_word.cnt; k++) {
                    // each data word is comming from some channel, so we place it into vme array
                    //cout << "[" << k << "] " <<  data[1+i+k] << " "<< endl ;
                    vmeOne[word.header_word.geo][k] = data[i + k + 1];
                }
                //vme[word.header_word.geo][3] = 77;
                //cout << endl;

                // after copying, we jump over them in the outer loop, they are processed
                i += word.header_word.cnt;
            }
#endif  // nigdy        // ------------------------------------------------------------------------
            break;

        case 0:
            // sizes are hardcoded in the class definition
            //       cout << "Code is 0, data word"
            //       << ", word nr " << i
            //       << endl;

            if(header_geo != word.data_word.geo) {
                // cout << "this can be the AGAVA block, we skip such a data" << endl;
                break;
            }

            // loading into the vme table.
            if(word.data_word.geo < 22 && word.data_word.channel < 32) {
                vmeOne[word.data_word.geo][word.data_word.channel]
                        = word.data_word.data;
            }
            // trap for debugging
            //             if (word.data_word.geo-5 == 1 && (word.data_word.channel == 8)  )
            //             {
            //
            //                         cout << "Data word in the event,  Now:   vme["
            //                         << word.data_word.geo-5 << "][" << word.data_word.channel
            //                         << "] has value = " << word.data_word.data << endl;
            //
            //
            // //                         cout << " YES, raw value = " << hex << (word.raw_word & 0xffff)
            // //                         << dec << endl;
            //             }



            if(word.data_word.geo == 0) break;  // geo  0 is for AGAVA which we skip
            if(word.data_word.geo > 0) word.data_word.geo -= 5;   // I do not know why

            if(word.data_word.geo >= lookup_prisma.size()) {
                static int cnt = 0 ;
                if(!((cnt++) % 300000)) {
                    cout  << "Unknown  geo = " <<  word.data_word.geo
                             // << ",  lookup_prisma.size() = " << lookup_prisma.size()
                          << endl;
                }
                break;
            }

            if(word.data_word.channel >= lookup_prisma[word.data_word.geo].size()) {
                cout  << "Unknown  channel " << word.data_word.channel
                      << ",  lookup_prisma[" << word.data_word.geo << "].size() = " << lookup_prisma[word.data_word.geo].size() << endl;
                break;
            }

            target_event->thePrismaDataMap[  lookup_prisma
                    [word.data_word.geo]
                    [word.data_word.channel] ] = word.data_word.data ;


            break;

        case 4:
            //       cout << "Code 4 = End of block\n"
            //       << ", word nr " << i
            //       << endl;
            break;

        case 6:
            //       cout << "Code 6 = Not valid datum \n" << endl;
            break;

        default: {
            static int cnt = 0 ;
            if(!((cnt++) % 25000)) {
                cout << "Unknown VME Code file (" << word.header_word.code
                     << ") in a data word"   //  "  (LINE= " << __LINE__ << ")"
                     << ".  Possibly a corrupted event in the data file: "
                     << RisingAnalysis_ptr->give_current_filename()
                     << endl;
            }
            return false;
        }
            break;
        }
    }
#endif

    return true;
}

//****************************************************************************************
#endif // PRISMA_EXPERIMENT


//*****************************************************************************
void TIFJEventProcessor::unpack_trigger_pattern(int event, int subevent, uint32_t *data, int length)
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
void TIFJEventProcessor::unpack_caen_v1724(int event, int subevent, uint32_t *p_se, int length)
{
    //cout << __func__ << endl;

    //bool empty_evt=0;

    // cout << "V1724,  length = "<< length << endl;

    //     int i_nevent = event;

    constexpr int how_many_chan_for_pedestal = 30; // 50;

    //bool fired[KRATTA_NR_OF_CRYSTALS][3]= {{0,0,0}};   // to zrobic w Tevent


    if(length > 0) {  // rozkodowanie V1724 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!1


        //         for(int i = 0 ; i < 10 ; ++i)
        //         {
        //             cout << i << ") -> " << hex << *(p_se+i) << endl;
        //         }


        if((p_se[0] >> 28)   == 0xa) { // new, mixed data
            // cout << "Old only Kratta data" << endl;
        } else {
//            cout << "New Kratta + Hector data" << endl;
            // analizuj hectora

            unpack_hector(p_se);

            // first word says how many longwords to skip to jump into kratta data
            // przeskocz od kratty
            int skip_over_hector = p_se[0];

            if(skip_over_hector) {
                //cout << "JEST PRAWDZIWY PRZESKOK " << endl;
            } else {
                //cout << "Nie ma danych hectora  " << endl;
            }

            p_se += skip_over_hector + 1;
            length -= skip_over_hector;
            cout << "skipping " << skip_over_hector +1  << " words " << endl;
        }
        int i_EVENT_SIZE0   = length;
        int addr_mod = 0;

        //         return;
        // petla dopoki (addr_mod < i_EVENT_SIZE0 - 1)
        do {


            int i_EVENT_SIZE    = (p_se[addr_mod + 0]) & 0xfffffff;
            char i_1010         = char((p_se[addr_mod + 0] >> 28) & 0xf);
            int i_CHANNEL_MASK  = (p_se[addr_mod + 1]) & 0xff;
            int i_PATTERN       = (p_se[addr_mod + 1] >> 8) & 0xffff;
            int i_RES           = (p_se[addr_mod + 1] >> 25) & 0x3;
            int i_BOARD_ID      = (p_se[addr_mod + 1] >> 27) & 0x1f;
            int i_EVENT_COUNTER = (p_se[addr_mod + 2]) & 0xffffff;
            int i_TRIG_TIME_TAG = (p_se[addr_mod + 3]);

            if(0) // just to avoid warning about the unused variables from above
            {
                cout  << "Nowa grupka danych  "
                      << "i_1010 = " <<  (int) i_1010
                      << "  i_EVENT_SIZE = "
                      << i_EVENT_SIZE
                      << " ze slowa danych " << hex << p_se[addr_mod+0] << dec
                      << ", length = " << length << ", while now we are at "  << addr_mod
                      << endl;



                cout << "BOARD_ID = " <<  i_BOARD_ID
                     <<  ", i_RES = " << i_RES
                      << ", i_PATTERN = "  << i_PATTERN
                      << " i_CHANNEL_MASK = " << i_CHANNEL_MASK << endl;

                cout << "i_EVENT_COUNTER = " <<  i_EVENT_COUNTER << endl;
                cout << "i_TRIG_TIME_TAG = " <<  i_TRIG_TIME_TAG << endl;
            } // end if 0
            if(i_EVENT_SIZE <= 0 ||  i_EVENT_SIZE > 20000) {
                cout << "Event size " << i_EVENT_SIZE<<  "  ????? ALARM " << endl;
                return;
            }

            int ich_indx = 0;
            for(int ich = 0; ich < 8; ich++) {
                if(i_CHANNEL_MASK & (1 << ich)) { //fired channel
                    ich_indx++;                    // zliczenie ile kanalow zadzialalo
                }
            }
            //             cout << "No. of fired channels " << ich_indx << endl;
            int n_sample = 0;
            if(ich_indx > 0) n_sample = 2 * (i_EVENT_SIZE - 4) / ich_indx;
            //             cout << "SAMPLE: "<< n_sample << endl;

            // do liczenia sredniej. czy to dobre miejsce - przeciez tu pracujemy z 8 kanalami !!!
            float value_min =  1.e9;   // minimalna wartosc sygnalu w przebiegu
            float value_max = -1.e9;
            unsigned int ch_max = 0;

            int time = 0;
            ich_indx = 0;
            for(int ich = 0; ich < 8; ich++) {

                //                 cout <<"czy jest sygnal z  Board_id = " << i_BOARD_ID << ", channel " << ich << endl;



                //cout << "przed petla  chmin= " << chmin << " chmax=" << chmax << endl;
                value_min =  1.e9;   // minimalna wartosc sygnalu w przebiegu
                value_max = -1.e9;
                if(i_CHANNEL_MASK & (1 << ich)) { //fired channel

                    //                      cout <<"Jest sygnal z  Board_id = " << i_BOARD_ID << ", channel " << ich << endl;
                    // fired, so no we try to decode it

                    kwartet t = lookup_kratta.current_combination(i_BOARD_ID, ich) ;// <-- Fake   // gg->GetIndexFromFadc(i_BOARD_ID, ich);

                    if(t.module  == -1) {
                        cerr << "Blad lookup table dla kratta - kombinacji " << i_BOARD_ID << " channel " << ich
                             << " nie przewidziano w lookup table  calibration/geo_map.geo"
                             << endl;
                        exit(0);
                    }
                    int mod  =  t.module ; // <---Fake   // gg->GetModuleNo(ggi);   // module number
                    int isig =  t.signal;  //  <--- Fake   //gg->GetSignalNo(ggi);   // signal nr  (kazdy modul daje 3 rodzaje sygnalow)

                    int flag_ok = t.ok_flag ;
                    if(!flag_ok) continue;    // we skip such data  which was disabled in the geo file by setting flag false.

                    string label = t.label ;

                    //                     cout
                    //                             << "  i_BOARD_ID = " << setw(2) << i_BOARD_ID
                    //                             << ", ich " << ich
                    //                             << " ---> mod " << setw(2) << mod
                    //                             << " isig " << setw(2) << isig
                    //                             << " flag= "<< flag_ok << "   " << label
                    //                             << endl;



                    if(mod < 0 || mod >= KRATTA_NR_OF_CRYSTALS) {
                        cout << "bad module = " << mod << " >= " << KRATTA_NR_OF_CRYSTALS << " \n" << endl;
                        continue;
                    }


                    //fired[mod][isig]=true;
                    int bin_nr = 1; // <-- nr kanlu w widmie przebiegu sygnalu

                    float pds = 0; // wyglada na to ze to pedestal liczony z pierwszych how_many_chan_for_pedestal   kanalow

                    vector<double> tabliczka;
                    // w jednym dlugim slowe danych sa dwa wynki pomiaru  (stad maskowanie ponizej)
                    for(int is = 0; is < n_sample / 2; is++) {
                        int i_data_lsb  = (p_se[addr_mod + 4 + ich_indx * n_sample / 2 + is]) & 0x3fff;


                        //                         if(is < 40)cout << "Ahist dla mod " << mod << ", sig " << isig
                        // 			  << "  [" << is << "] = "
                        // // 			  << ", kanal bin_nr " << bin_nr
                        // 			  << " setBinContent z wartoscia " << i_data_lsb << endl;	// hraw[mod][isig]->SetBinContent(bin_nr,float(i_data_lsb));

                        if(bin_nr <= how_many_chan_for_pedestal) pds += float(i_data_lsb); // do liczenia sredniego pedestalu
                        if(float(i_data_lsb) < value_min) value_min = float(i_data_lsb); // doliczenia min
                        if(float(i_data_lsb) > value_max) {
                            value_max = float(i_data_lsb); // do liczenia max
                            ch_max = bin_nr;
                        }
                        tabliczka.push_back(i_data_lsb);
                        bin_nr++;

                        int i_data_msb  = (p_se[addr_mod + 4 + ich_indx * n_sample / 2 + is] >> 16) & 0x3fff;
                        //                         if(is < 40) cout << "Bhist dla mod " << mod << ", sig " << isig
                        // // 			  << ", kanal bin_nr " << bin_nr
                        // 			  << "  [" << is << "] = "
                        // 			  << " setBinContent z wartoscia " << i_data_msb << endl; // hraw[mod][isig]->SetBinContent(bin_nr,float(i_data_msb));

                        if(bin_nr <= how_many_chan_for_pedestal) pds += float(i_data_msb);		// do liczenia sredniego pedestalu
                        if(float(i_data_msb) < value_min) value_min = float(i_data_msb);	// doliczenia min
                        if(float(i_data_msb) > value_max) {
                            value_max = float(i_data_msb);	// do liczenia max
                            ch_max = bin_nr;
                        }
                        tabliczka.push_back(i_data_msb);

                        bin_nr++;
                        //if(ich==2)printf("ch %1d is %4d %6d %6d\n",ich,is,i_data_msb,i_data_lsb);

                    }  // end for sample

                    if(ch_max == 0 || (ch_max > ( tabliczka.size() - 10))) {
                        // no maximu, or just near the end of the graph - we ignore it
                        flag_ok = false;
                    }

                    pds /= (double) how_many_chan_for_pedestal; // sredni pedestal

                    // Calculating the time - when the signal rises from pedestal to 10% of its maximum


                    // w jednym dlugim slowe danych sa dwa wynki pomiaru  (stad maskowanie ponizej)


                    double max_ampl = value_max - pds ;
                    const double rise20_percent_of_max  = 0.2 * max_ampl;
                    const double rise30_percent_of_max  = 0.3 * max_ampl;

                    //                  system("pwd");
                    static int licznik = 0;
                    ofstream  plik;
                    plik.open("input_data.bin", ios::app | ios::binary);
                    if(!plik) {
                        cout << "Error while opening input_data.txt" << endl;
                        exit(88);
                    }
                    
                    if(++licznik > 200) {
                        plik.close();
                        //exit(12);
                    }
                    
                    int dlug = tabliczka.size();
                    plik.write((char *) &dlug, sizeof(dlug));

                    for(unsigned int is = 0; is < tabliczka.size(); is++) {
                        tabliczka[is] -= pds;

                        plik.write((char *) &tabliczka[is], sizeof(tabliczka[is]));
                        // 	      plik << tabliczka[is] << endl;
                        // 		      cout << "  [" << is << "]" << "-->  " << tabliczka[is] ;
                        // 		      if(is % 10 ==0) cout << endl;
                    }
                    plik.close();


                    int ch2 = 0;
                    int ch3 = 0;
                    double y2 = 0 ;
                    double y3 = 0;

                    for(unsigned int is = how_many_chan_for_pedestal; is < ch_max; is++) {
                        // 		      cout << "Szukanie czasu channel is = [" << is << "] has value " <<   tabliczka[is]
                        // 			<< " max ampl = " << max_ampl
                        //                         << " ---> Czy (value  = " << tabliczka[is]
                        // 			<<  " >   rise_factor * (ampl) = " << rise_factor* (max_ampl)
                        //                              << endl;

                        if(ch2 == 0 && (tabliczka[is]) > rise20_percent_of_max) {
                            // 			  cout << "Znaleziony czas " << is << endl;
                            ch2 = is;

                            // now we prefer a mean of 5 channels
                            if(is > 2 && is < tabliczka.size() - 2) {
                                y2 = (tabliczka[is - 2] + tabliczka[is - 1] + tabliczka[is] + tabliczka[is + 1] + tabliczka[is + 2]) / 5.0;
                            } else {
                                y2 = tabliczka[is];
                            }

                        }

                        if((tabliczka[is]) > rise30_percent_of_max) {
                            //        cout << "Znaleziony czas " << is << endl;
                            ch3 = is;

                            if(is > 2 && is < tabliczka.size() - 2) {
                                y3 = (tabliczka[is - 2] + tabliczka[is - 1] + tabliczka[is] + tabliczka[is + 1] + tabliczka[is + 2]) / 5.0;
                            } else {
                                y3 = tabliczka[is];
                            }

                            break;
                        }

                    } // end for (is

                    if(!(ch2 && ch3)) {
                        flag_ok = false;
                    }

                    if(((y3 - y2) > 1)   && flag_ok) {
                        time = (y3 * ch2 - y2 * ch3) / (y3 - y2);
                        if(time <= how_many_chan_for_pedestal) {
                            time = 0 ;
                        }
                    }

                    if(time == 0) {
                        // cout << "Czas nieznaleziony " << endl;
                    }

                    ich_indx++;

                    //                     cout << "i_nevent= " << i_nevent << "  ich= " << ich << " pedestal pds= " << pds  << endl;
                    //                     cout << "Wlasciwy wynik - po petli  chmin= " << chmin << " chmax=" << chmax
                    //                          << " wartosc sygnalu  (chmax - pds) = " << (chmax - pds)
                    //                          << endl;


                    //if(verbose_mode_enabled)
                    // 		    {
                    //                     cout << " For Kratta module nr " <<  mod << ", sygnal = " << isig << " " << label
                    //                          << "  max value value is " << chmax << ", pedestal " << pds
                    //                          << " so (max - pedesal) = " << (chmax - pds)
                    //                          << " time is " << time
                    //                          << endl << endl;
                    //                     }

                    if(time) {
                        target_event->kratta[mod][isig] = value_max - pds ; // normal values 0-2
                        target_event->kratta[mod][3 + isig] = pds;		// pedestals are 3-5
                        target_event->kratta[mod][6 + isig]  = time;
                    }
                    // cout << endl;

                } else {
                    //cout << "Channel not fired " << endl;
                }

            }//for(int ich = 0;ich<8;ich++)


            addr_mod += i_EVENT_SIZE;
            //             cout << "Na koncu petli while - addr_mod " << addr_mod << "  i_EVENT_SIZE0 = " << i_EVENT_SIZE0 << " zwiekszenie o " << i_EVENT_SIZE << endl;
        } while(addr_mod < (i_EVENT_SIZE0 - 1));


        //goto lab;
        //i_nevent += 1;

        //         cout << "Pelny Event:   " << empty_evt << endl;
    } else { //empty evt
        /// xxx empty_evt= true;
                 cout << "Empty Event v1724!  " << endl;

    }

}
//*****************************************************************************

void TIFJEventProcessor::unpack_36_2800(int event, int subevent, uint32_t *data, int length)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_10_1(int event, int subevent, uint32_t *data, int length)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_34_1(int event, int subevent, uint32_t *data, int length)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_36_2700(int event, int subevent, uint32_t *data, int length)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_32_1130(int event, int subevent, uint32_t *data, int length)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************

void TIFJEventProcessor::unpack_36_9494(int event, int subevent, uint32_t *data, int length)
{
    //     cout << __func__ << endl;
}
//*****************************************************************************
void TIFJEventProcessor::unpack_hector(uint32_t *data)
{
    // cout << __func__ << endl;





#define FAKE 0
#if FAKE
    static ifstream plik;
    static bool flaga;
    if(!flaga) {
        plik.open("hector_data_0.dat", ios::binary);
        if(!plik) {
            cout << "imposible to open the hector data file" << endl;
            exit(33);
        }
        flaga = true;
    }

    uint32_t how_many_words = 0;


    vector<uint32_t> bufor;

    // if fake
    if(FAKE) {
        // read a first word (event length)file
        plik.read((char *) &how_many_words, sizeof(how_many_words));
        if(!plik) {
            cout << "Error while reading length of the fake  Hector file" << endl;
            exit(9);
        }
        //         cout << "How many words = " << how_many_words << hex << " " << how_many_words << dec << endl;

        how_many_words = swap_int32(how_many_words);
        //         cout << "How many words = " << how_many_words << hex << " " << how_many_words << dec << endl;

        for(int i = 0 ; i < how_many_words ; i++) {
            uint32_t d;
            plik.read((char *)  &d, sizeof(d));
            if(!plik) {
                cout << "Error while reading hector data" << endl;
                exit(91);
            }

            d = swap_int32(d);
            bufor.push_back(d);

        }
        data = bufor.data();
        //         cout << "buffor size = " << bufor.size() << endl;
    } // fake

#else
    uint32_t how_many_words = data[0];
#endif // 0


    //  VME 775 VME data ########################
    // loop over unknown nr of words

    general_vme_word word;
    //unsigned int current_geo = 0;

    for(unsigned int i = 1; i < how_many_words; ++i) { // 1 bec. we skip counter how many words
        //         cout << "Unpacking word " << i << ": 0x"
        //              << hex << data[i] << dec << endl;
        word.word775.raw_word = data[i];
        if(!word.word775.raw_word) {  // do not analyse empty words
            continue;
        }

        // Sometimes Henning is sending the extra word on the end of the VME block
        // with the pattern 0xbabe****
        if((word.word775.raw_word & 0xffff0000) == 0xbabe0000) {
            cout << "babe word ==============" << endl;
            break; // end of data at all, so end of the loop
        }


        switch(word.word775.header_word.code) {  // <-- it is common to all of types of vme words

        case 2: {
            //             cout << "Caen V775 TDC Header .." << endl;

        }
            break;

        case 0:
            // sizes are hardcoded in the class definition
            if(word.word775.data_word.geo < 22 && word.word775.data_word.channel < 32) {

                //                 cout << "Arrived  geo = " << word.word775.data_word.geo
                //                      << ", chan = " << word.word775.data_word.channel
                //                      << ", data = " << word.word775.data_word.data
                //                      << endl;

                //int clus = 0;
                //int crys = 0;

                // - Fast component on channels 1-8
                // - Slow component on channels 17-24
                // - Time information on TDC channels 1-8
                constexpr int HECTOR_TDC_GEO = 		8;
                constexpr int  HECTOR_ADC_GEO =	7;

                if(word.word775.data_word.geo == HECTOR_TDC_GEO) {
                    target_event->hector_tdc[word.word775.data_word.channel] = word.word775.data_word.data;

                } else if(word.word775.data_word.geo == HECTOR_ADC_GEO) {
                    target_event->hector_adc[word.word775.data_word.channel] = word.word775.data_word.data;
                }
            }
            break;

        case 4:
            //             cout << "Footer  when i = .." << i << endl;

            break;
        case 6:
            //             cout << "'Not valid datum for this TDC when i = .." << i
            //                  << endl;
            break;

        default :
            cout << "Error - unknown code in VME time information  when i = .." << i << endl;
            break;
        } // endof inner switch
    } // end of for i
    // cout << "end of unpackig hector" << endl;
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


//ClassImp(TIFJEventProcessor)
