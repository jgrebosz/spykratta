//---------------------------------------------------------------
//        Go4 Framework release package V.1.0000
//                      24-May-2002
//---------------------------------------------------------------
//   The GSI Online Offline Object Oriented (Go4) Project
//
//----------------------------------------------------------------
//This software can be used under the license agreements as stated
//in Go4License.txt file which is part of the distribution.
//----------------------------------------------------------------
/* Generated by Together */

#ifndef TEBANALYSIS_H
#define TEBANALYSIS_H

#include "experiment_def.h"

//#include "TH1.h"
//#include "TFile.h"

#include <string>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <map>  // for map of zeroing
#include <ctime> // zeroing time
#include "Tuser_incrementer.h"

//#include "Thector.h"
//#include "Tparis.h"

#include "mbs_listner.h"

// Fake version of Go4 function
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4EventProcessor
{
    std::string name;
public:
    TGo4EventProcessor(char *name_) : name(name_) {}
};
//////////////////////////////////////////////////////////////////////////////////////////////////
#include "TIFJEventFactory.h"
#include "TIFJCalibrateFactory.h"

class spectrum_abstr ;
class spectrum_1D;
class spectrum_2D;

class Tuser_spectrum ;
class Tuser_condition;
class Tuser_incrementer;


#include "Tfrs.h"
#include "Trising.h"
#include "Tsynchro_watchdog_curved.h"
#include "tsocket_fo_data.h"
class Tuser_condition;
class TjurekEventSourceException
{ }
;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TjurekEventEndException : public TjurekEventSourceException
{}
;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4EventSourceParameter
{
protected:
    int nr_of_current_event;
    bool new_lookup_table_required = true;

public:
    TGo4EventSourceParameter() : nr_of_current_event(0)
    {}

    virtual ~TGo4EventSourceParameter() {} // <-- needed for deleting derived class objects
    virtual const_daq_word_t* give_next_event(int *how_many_words) = 0;
    //virtual const_daq_word_t* give_next_subevent(){};
    virtual mbs_listner* give_mbs()  {
        return nullptr;
    }

    int give_event_nr()
    {
        return nr_of_current_event;
    }
    virtual std::string  give_name_of_source()=0;     

    virtual bool is_new_lookup_table_required()  // to install new lookup table
    {
        return new_lookup_table_required;
    }
    virtual void new_lookup_table_just_installed()  // to inform that just installed
    {
        new_lookup_table_required = false;
    }

//     {
//         return "abstract ???" ;
//     }
		virtual string give_filename_of_list_of_lmds() { return ""; }

};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4RevServParameter : public TGo4EventSourceParameter    // to jest rzeczywiscie gdy eventy przychodza z servera (np. pryzmy)
{
    string server_name;
    Tsocket_fo_data *exotic_socket;
public:
    TGo4RevServParameter(string host, string port)    // online name of the event server
    {
        // here will be calling of opening the socket
        cout << "TGo4RevServParameter : constructor, host = "
             << host << ", port=" << port << endl;
        exotic_socket = new Tsocket_fo_data(host, port);

    }
    ~TGo4RevServParameter()
    {
        delete exotic_socket ;
    }

    // getting the events _________________________________
    //virtual
    const_daq_word_t*  give_next_event(int *how_many_words)
    {

        const_daq_word_t *ptr = exotic_socket->give_next_event(how_many_words);

        if(*how_many_words) nr_of_current_event++;
        return ptr;

        //*how_many_words= 0;
        // return NULL;
    }

    std::string  give_name_of_source()
    {
        return server_name;
    }
};

//#########################################################################
enum error_nr_enum  { myEOF = -1,  myFAIL = -2 };
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class Texception_input_file
{
public:
    error_nr_enum     error_nr;    // -1 EOF, -2 FAIL
    Texception_input_file(error_nr_enum er) : error_nr(er)
    {}
}
;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4MbsFileParameter  : public TGo4EventSourceParameter   // to jest gdy eventy sa z pliku.
{
    // string filename;
    int starting_event_nr;
    ifstream file;
    int event_length_in_bytes;


    mbs_listner  mbs;


    //unsigned
    //char  data[10000000];
    vector<daq_word_t>  data_buffer;

    string current_file;  // from agata

    long long file_size ;
    //   int length_of_data_block;

public:
//   typedef unsigned int    buffer_word_t ;
//   typedef unsigned long   buffer_word ;
    bool open_file(string name);    // from agata

    vector<string> list_of_names;// from agata
    bool flag_list;// from agata
    unsigned int nr_of_file_already_analysed;// from agata
    bool open_next_file_from_the_list();// from agata
    
    string filename_with_list_of_lmds_list; // if it was a list file starting with @ sign
    string give_filename_of_list_of_lmds() { return filename_with_list_of_lmds_list; }
    
    int block_number;


    TGo4MbsFileParameter(string nam) :  starting_event_nr(0)
    {
        nr_of_file_already_analysed = 0 ;
				
        // opening the file
        if(!open_file(nam))
        {
            cout << "\n432 Error while opening the input file: " << nam << endl;
            cout << "I try to open it from directory " ;
            [[__maybe_unused__]] int result = system("pwd");
            exit(2);
        }
        else
        {
            cout << "\nSuccessful opening of the file to sort: " << current_file << endl;

            // skipping file header 32 bytes
            //file.read(data, 32);



#ifdef OLD_PISOLO_DATA
            // if this is OLD Pisolo data file, we have to skip first 32768 bytes
            int how_many_header_bytes_to_skip = 32768;
            file.seekg(how_many_header_bytes_to_skip);
            if(!file)
            {
                cout << "1. Error while  (skipping) file header of the data " << how_many_header_bytes_to_skip << "bytes"  << endl;
            }
#elif defined MBS_DATAFILE


#else
            // skipping file header 40 bytes
            if(data_buffer.size() < 40)   // ewe do not mind that it is long of int.
            {
                data_buffer.resize(40);
            }
//              char* where =  (char*) ( data_buffer.data());    // because old compiler does not know .data()  for vector
            char* where = (char*)(&data_buffer[0]);
            // skipping file header 40 bytes
            file.read(where, 10 * sizeof(daq_word_t));
            if(!file)
            {
                cout << "1. Error while reading (skipping) file header of the data 40 dwords" << endl;
            }
#endif

        }

    }
    ~TGo4MbsFileParameter()
    {
        file.close();
    }

    void SetStartEvent(int ev)
    {
        // positioning the
        starting_event_nr = ev; //
        nr_of_current_event = starting_event_nr;

        cout << "TODO!    skipping " << ev <<  " events" << endl;

    }

    // Getting the events  from the file_________________________________
    const_daq_word_t*  give_next_event(int *);     // virtual
    const_daq_word_t*  give_next_old_pisolo_event(int *how_many_words);

    const_daq_word_t*  give_next_mbs_event(int *how_many_words);
    mbs_listner* give_mbs() {
        return &mbs;
    }

    std::string  give_name_of_source()
    {
        return current_file;
    }
    bool is_new_lookup_table_required()  // to install new lookup table
    {
        return new_lookup_table_required;
    }
    void new_lookup_table_just_installed()  // to install new lookup table
    {
        new_lookup_table_required = false;
    }

};
//**************************************************
////////////////////////////////////////////////////////////////////////////////////
class TGo4FileStoreParameter
{
    string file_name;
public:
    TGo4FileStoreParameter(string nam, int /* val*/ = 0)
        : file_name(nam)
    {}

    void SetOverwriteMode(bool /* v */)
    {
        // empty so far
    }
};
////////////////////////////////////////////////////////////////////////////////////
// abstract class for event processor of particular step
class TGo4EventProcessorParameter
{}
;
////////////////////////////////////////////////////////////////////////////////////
class TGo4FileSourceParameter
{
public:
    TGo4FileSourceParameter(string /*name*/)
    {}
}
;
///////////////////////////////////////////////////////////////////////////////////////
class TjurekFactory
{
    string name;
public:
    TjurekFactory(string n) : name(n)
    {}
}
;
/////////////////////////////////////////////////////////////////////////////////////////////////
class TGo4AnalysisStep
{
    string name;
    bool enabled;

    TGo4EventSourceParameter *sourceptr; // <-- where from take the data
    TGo4FileStoreParameter *storeptr; // <--- where to put the result RAW root tree ( name_RAW.root)
    TjurekAbstractEventProcessor * procptr ;
    TjurekFactory *factoryptr;

public:

    TGo4AnalysisStep(
        string n,       // <--- name
        TjurekFactory *factory1,
        TGo4EventSourceParameter *sourcepar, // <-- where from take the data (file or remote server) pointer to the object
        TGo4FileStoreParameter *storepar, // <--- where to put the result RAW root tree ( name_RAW.root)
        TjurekAbstractEventProcessor * procpar) :  // NULL?
        name(n), sourceptr(sourcepar),storeptr(storepar), procptr(procpar),
        factoryptr(factory1)
    {
        enabled = true;
    }
    //----------------------------------
    void SetSourceEnabled(bool t)
    {
        enabled  = t;
    }
    void SetStoreEnabled(bool /*t*/)
    {}   // leave it true, even it will be disabled, otherwise the file
    // is not opened, and enabling later will not be possible - this is my observation JG

    void SetProcessEnabled(bool /*t*/)
    {}
    void SetErrorStopEnabled(bool /*t*/)
    {}

    void Process()
    {
        //        procptr->Process();
    }

};

//**************************************************
///////////////////////////////////////////////////////////////////////////////
class TIFJAnalysis  //: public TGo4Analysis
{

    TIFJEventProcessor *step1;
    TIFJCalibrateProcessor *step2;

    TIFJEvent *step1_output_event;
    TIFJCalibratedEvent *step2_output_event;

    void set_all_steps(TIFJEventProcessor *p1, TIFJCalibrateProcessor *p2,
                       TIFJEvent *targ1, TIFJCalibratedEvent *targ2)
    {
        step1 = p1;
        step2 = p2;
        step1_output_event = targ1;
        step2_output_event = targ2;
    }

public:
    TIFJAnalysis();
    TIFJAnalysis(string lmdname, string rawname,
                 string calname, string   userfname);

    virtual ~TIFJAnalysis() ;
    virtual int UserPreLoop();
    virtual int UserEventFunc();
    virtual int UserPostLoop();
    virtual bool InitEventClasses();
    void make_calibration_step()
    {
        frs.analyse_current_event();
#ifdef HECTOR_PRESENT	
//	hector.analyse_current_event();
#endif
    }

    void make_watchdog_step()
    {
#ifdef GSI_STUFF
        synchro_watchdog.analyse_current_event();
#endif

    }

    vector<spectrum_abstr*> spectra_local ;  // timestamp etc.

    vector<Tuser_spectrum*> spectra_user ;  //!    // user defined spectra
    vector<Tuser_condition*> user_condition ;  //!    // user defined conditions
    vector<Tuser_incrementer*> vector_of_user_incrementers; //!

    bool flag_sorting_from_file;


    /** This is the value which is stored in every event.
    We need it to estimate the ratio of how many events are analysed
    to all the events coming */
    int *ptr_to_mbs_nr_of_the_current_event; //!

    //string nam_hec_minus_ger_timestamp_diff ;

    static TIFJCalibratedEvent * fxCaliEvent;

    int nr_events_analysed;  // Cal Processor will set its value here

    void SetAutoSaveFile(string nazwa)
    {
        cout << "empty f. SetAutoSaveFile " << nazwa << endl;
    }

    void SetAutoSaveInterval(int /*value*/)
    { }


    TjurekPolyCond* give_ptr_to_polygon(string name);
    void AddAnalysisCondition(TjurekPolyCond* ptr)
    {
        list_of_polygons.push_back(ptr);
    }

    void RemoveHistogram(string name_root)
    {
        cout << "Empty f. RemoveHistogram() " << name_root << endl;
    }
    void AddHistogram(string name_root)
    {
        cout << "Empty f. AddHistogram() " << name_root << endl;
    }
    void AddHistogram(TH1* ptr_spec, const char*);

    //-----------------
private:
    bool flag_event_unpacked_and_is_valid ;
    vector<TGo4AnalysisStep *> steps_of_analysis;

    void AddAnalysisStep(TGo4AnalysisStep * step)
    {
        steps_of_analysis.push_back(step);
    }

    TGo4EventSourceParameter* source_of_events;   // null

    vector<TjurekPolyCond*> list_of_polygons;

    vector <TH1*> list_of_histograms;

    TjurekMbsEvent * fxMbsEvent;
    TIFJEvent * fxEbEvent;
    TIFJCalibrateProcessor* proc_remembered ;

    /** If some spectra are defined here in the analysis class */
    bool save_analysis_local_spectrum(string specname);



public:
    Tfrs * give_frs_ptr()
    {
        return &frs ;
    }



    vector<spectrum_abstr*> * local_spectra_pointer()
    {
        return  &spectra_local;
    }


    /** this is such a version which replaces the RunImplicitLoop,
    but is able to handle the pause and continue exceptions */
    int run_jurek_ImplicitLoop(int times);

    int print_ratios_every_n_events;            // initaially  = 5000, ten mdified
    time_t previous_print_ratios_time;
    int previous_print_event_nr;
    int last_ev_analsyed_long_term_nr;
    int last_ev_mbs_long_term_nr;

    void display_procent_ratios_and_speed(const int ev_nr);

    /** If the cracow viewer is just observing some spectra, we
    * will save them more often */
    void save_just_observed(bool unconditionally = false);
    /** sending an info message to the Cracow GUI */
    void info_to_cracow(string s, int i = -1);


    Tfrs          frs ;      //!     fragment separtor + target +cate

#ifdef HECTOR_PRESENT
#if 0
    Thector       hector ;   //!


    Thector * give_hector_ptr()
    {
        return &hector ;
    }
#endif



#if 0
       Tparis       paris ;   //!
    Tparis * give_paris_ptr()
    {
        return &paris ;
    }
#endif

#endif

    /** perhaps it was idle cycle ? */
    //     bool cycle_on_real_data;



    //---------------------
    void CloseAnalysis() ;  // hiding the one from the base class

    //transporter * ptr_transporter ; //!  -----------------------!
    TIFJCalibrateProcessor* give_call_processor_address() ;
    /** Cracow viewer may send request to delete list of spectra */
    void perhaps_delete_some_spectra();

    /** to checking if cracow gui is giving any pause/go command */
    void check_for_commands(int nr_events);

    /** No descriptions */
    inline void perhaps_info_of_events(int nr_events);
    void perhaps_conditions_report(bool par_force = false);

    /** In preLoop moment */
    void read_definitions_of_user_spectra();
    /** No descriptions */
    void create_user_spectrum(string name);

    /** Conditions live independently. One condition can be shared by many
    spectra */
    Tuser_condition* create_user_condition(string name);

    TIFJEvent* give_unpacked_event_ptr()
    {
        return fxEbEvent;
    }


    //    vector<Tuser_sorting_wish_abstract*> sorting_wish;

    string give_current_filename();
		
		string give_filename_of_list_of_lmds()
		{
			return source_of_events->give_filename_of_list_of_lmds() ;
		}
		Tuser_condition* give_ptr_to_cond(string);


    struct ntuple_entry_data
    {
        string variable_name ;                        //  1.  Name of the variable

        void   * variable_source_ptr ;             //  2.  Address of the variable
        bool   *validation_flag_ptr ;                //  3. when variable is usable

        void * variable_ntuple_ptr ;              //  4.  Address in the Tcalibrated event
        int type_of_data ;                           //  5. double=1, int = 2, bool = 3  ( make casting)
        bool must_be_ok ; // some data we do not want obligatory (for example sci xy pos)


        ntuple_entry_data(string nam, double *destin, bool obligatory_ok) :
            variable_name(nam),
            variable_ntuple_ptr(destin),
            type_of_data(1),    // double
            must_be_ok(obligatory_ok)
        {}

        ntuple_entry_data(string nam, int *destin, bool obligatory_ok) :
            variable_name(nam),
            variable_ntuple_ptr(destin),
            type_of_data(2),      // int
            must_be_ok(obligatory_ok)
        {}

        ntuple_entry_data(string nam, bool *destin, bool obligatory_ok) :
            variable_name(nam),
            variable_ntuple_ptr(destin),
            type_of_data(3),    // bool
            must_be_ok(obligatory_ok)
        {}



        ntuple_entry_data()
        {}
    }
    ;

    vector<ntuple_entry_data> ntuple_entry_vector ; // what to put into the ntuple

    
    void read_ntuple_options();
    void prepare_ntuple_addition();
    
    void put_data_to_ntuple();
    bool use_isotope_identifier_for_ntuple_events ;
    

    bool ntuple_RAW_is_collected;
    bool ntuple_CAL_is_collected;

    // this below is deprecated
    bool ntuple_mw_must_be_ok;
    bool ntuple_tof_must_be_ok ;
    bool ntuple_sci_pos_must_be_ok;
    bool ntuple_cate_xy_must_be_ok;
    bool ntuple_cate_energy_must_be_ok;

    // the new style, user def condition
    Tuser_condition *step2_cond_ptr;
    int nicto;
    bool *step2_cond_result_ptr; //!

    /** No descriptions */
    void incr_synchronisation_matices();
    /** No descriptions */
    void make_ratio_spectra();
    spectrum_1D
    * spec_rates1,
    *spec_rates2,
    *spec_rates3 ;

    /** No descriptions */
    void prepare_ratio_vector();

    struct single_ratio
    {
        string name ;
        bool *test_it;
        int nr_success;
        int spectrum_to_increment ;   // there may be more than one spectrum
        int channel_to_increment;
        string explanation;

        // constructor
        single_ratio(string name_of_value = "", int spec_to_increment = 0,
                     int chan_to_increment = 0, string explanation_text = "") :
            name(name_of_value),
            spectrum_to_increment(spec_to_increment),
            channel_to_increment(chan_to_increment),
            explanation(explanation_text)
        {
            nr_success = 0 ;
            if(explanation == "")
                explanation = name ;
        }

    };

    vector<single_ratio> ratio_vector ;

    //  /** Flag telling that now we want to dump information about the current event */

    bool general_flag_verbose_events;
    //  int verbose_begins_with_event_nr ;
    //  int verbose_how_many_events ;
    //

    //  /** For Franco Camera wish to make dump of selected events */
    //  void read_in_verbose_parameters();
    /** many elements will ask if the verbose mode of displaying events
    * is on */
    bool is_verbose_on()
    {
        return  general_flag_verbose_events;
    }
    void set_verbose(bool b)
    {
        general_flag_verbose_events = b ;
    }

    /** No descriptions */
    void create_my_spectra();
    /** No descriptions */
    int read_how_many_branches();
    /** No descriptions */
    void read_autosave_time();
    int autosave_period_in_minutes;

    /** This function calculates the value of all the user defined conditions.
    * It may be used by the user defined spectra, but also as the decision
    * if the event should go to the ntuple. */
    void check_all_user_def_conditions();

    /** This function checks if the user wanted any condtion on
    * the step2 ntuple
    * and sets a special pointer */
    void prepare_condition_for_ntuple(string condition_name);
    /** No descriptions */
    void user_batch();

    /** This member remembers the time of the last zeroing */
    map<string, time_t> map_of_zeroing;
    void store_time_of_zeroing_spectrum(string name, time_t when)
    {
        map_of_zeroing[name] = when;
    }
    void save_times_of_zeroing_on_disk();
    void restore_times_of_zeroing_from_disk();

    void read_in__user_incrementers();
    void create_user_incrementer(string name_of_description);

    void create_file_with_incrementers();
    //--------------------------------
    void PreLoop()
    {
//         cout << "f. TIFJAnalysis::PreLoop() " << endl;
        step1->preLoop();
        step2->preLoop();
        UserPreLoop();
    }
    //--------------------------------
    void MainCycle()
    {
        // cout << "f. TIFJAnalysis::MainCycle() " << endl;

        //         1. rozpakowanie - step1
        step1->BuildEbEvent(source_of_events, step1_output_event);
        //         2. kalibracja - step2
        step2->BuildCalibratedEvent(step2_output_event);
        //         3. user functions - step2a
        // go4 was calling it automatiacally, here in exotic - we must make it on our own
        UserEventFunc();
    }

    //--------------------------------
    void PostLoop()
    {
        //cout << "Empty f. TIFJAnalysis::PostLoop() " << endl;

//         cout << "f. TIFJAnalysis::PostLoop() " << endl;
        step1->postLoop();
        step2->postLoop();
        UserPostLoop();
    }


    TH1 * GetHistogram(string name)
    {
        //     cout << "Empty f. TIFJAnalysis::GetHistogram()  for " << name << endl;
        for(unsigned int i = 0 ; i < list_of_histograms.size() ; i++)
        {
            if(list_of_histograms[i]->GetName() == name)
            {
                //         cout << "Found !!!" << endl;
                return list_of_histograms[i];
            }
        }
        // not found
        //     cout << "F. TIFJAnalysis::GetHistogram()  for " << name
        //     << " - such a spectrum is not found on the list "
        //     << endl;
        return nullptr;
    }

    //    ClassDef(TIFJAnalysis,1)

};

///////////////////////////////////////////////////////////
extern TIFJAnalysis *  RisingAnalysis_ptr ;
extern unsigned int starting_event; // global to be set in main and accessed from constructor of TGo4Analysis.cxx

#endif //TEBANALYSIS_H


