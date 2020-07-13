
// #if 0

#include "Thec_kratta_tree.h"

#include "experiment_def.h"
#if CURRENT_EXPERIMENT_TYPE == IFJ_KRATTA_HECTOR_EXPERIMENT


#if CERN_ROOT_INSTALLED == true

#include <TFile.h>
#include <TTree.h>

#include "experiment_def.h"
#include <sstream>
#include <iomanip>

#include "TIFJAnalysis.h"
extern TIFJAnalysis *RisingAnalysis_ptr;
// extern TIFJEvent * target_event;
//*********************************************************************
Thec_kratta_tree::Thec_kratta_tree(std::string my_name,
                                   int16_t (TIFJEvent::*hector_tdc_data_ptr_a)[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS],
                                   int16_t (TIFJEvent::*hector_adc_data_ptr_a)[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS],
                                    int16_t (TIFJEvent::*phoswich_tdc_data_ptr_a)[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS],
                                    int16_t (TIFJEvent::*phoswich_adc_data_ptr_a)[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS],
                                   int16_t (TIFJEvent::*kratta_data_ptr_a) [KRATTA_NR_OF_CRYSTALS][9],
                                    int16_t (TIFJEvent::*plastic_data_ptr_a) [KRATTA_NR_OF_PLASTICS]
                                  ):

    Tfrs_element(my_name),
    hector_tdc_data_ptr(hector_tdc_data_ptr_a),
    hector_adc_data_ptr(hector_adc_data_ptr_a),
    phoswich_tdc_data_ptr(phoswich_tdc_data_ptr_a),
    phoswich_adc_data_ptr(phoswich_adc_data_ptr_a),
    kratta_data_ptr(kratta_data_ptr_a),
    plastic_data_ptr(plastic_data_ptr_a)

{
}
//*********************************************************************
Thec_kratta_tree::~Thec_kratta_tree()
{
    if(flag_file_and_tree_already_opened) write_the_tree();
}
//***************************************************************************
void Thec_kratta_tree::make_preloop_action(ifstream &)  // read the calibration factors, gates()
{
  cout << "Thec_kratta_tree::make_preloop_action " << endl;
  
    if(RisingAnalysis_ptr->ntuple_RAW_is_collected) { // needed, bec. of TBranc

        if(flag_file_and_tree_already_opened == false) {
            // create a TFile object, using RECREATE to overwrite // if it exists

            // ask for name: either single lmd file or name of the @text file having a bulk of file names
            root_filename = RisingAnalysis_ptr->give_filename_of_list_of_lmds() ;

            if(root_filename.empty()) {
                root_filename = RisingAnalysis_ptr->give_current_filename();
            }

// if there is a path, remove it
            string::size_type where_slash = root_filename.find_last_of("/");
            if(where_slash != string::npos) {
                root_filename = root_filename.substr(where_slash+1);
            }


            // if there is an extension, remove it
            string::size_type where_dot = root_filename.find_last_of(".");
            if(where_dot != string::npos) {
                root_filename.erase(where_dot);
            }

            root_filename += "_RAW_tree.root" ;
//             TFile* f = new TFile("\\scieżka\\nazwa.root", "recreate"); - podwójne ukośniki, gdyż są one reinterpretowane.
            hfile = new TFile(root_filename.c_str(), "RECREATE");
            // this line must be below, and it will automatically attach to the TFile
            tree = new TTree(string("Hector_Kratta_RAW_tree").c_str() , "A ROOT tree");
            flag_file_and_tree_already_opened = true;
        }

        // adding the specific branches

        //  mult_casc = 2;
        // event_mult = 33;

        //tree->Branch("event_mult",&event_mult,"event_mult/I");
        //   tree->Branch("compound_spin",&compound_spin,"compound_spin/I");
        //  tree->Branch("fission",&fissn,"fission/I");

        //tree->Branch(string(owner_name+"_A[0]").c_str() , A[0][0], Form("A[%d][%d]/I", 0, 0) );



        // TIFJEvent *ev = event_unpacked;

        ostringstream s;
        s << "hector_adc[32]/s:hector_tdc[32]/s"
             << ":phoswich_adc[32]/s:phoswich_tdc[32]/s"
          << ":kratta_s1[" << 	KRATTA_NR_OF_CRYSTALS << "]/s1"
          << ":kratta_s2[" << 	KRATTA_NR_OF_CRYSTALS << "]/s"
          << ":kratta_s3[" << 	KRATTA_NR_OF_CRYSTALS << "]/s"

          << ":kratta_p1[" << 	KRATTA_NR_OF_CRYSTALS << "]/s1"
          << ":kratta_p2[" << 	KRATTA_NR_OF_CRYSTALS << "]/s"
          << ":kratta_p3[" << 	KRATTA_NR_OF_CRYSTALS << "]/s"

          << ":kratta_t1[" << 	KRATTA_NR_OF_CRYSTALS << "]/s"
          << ":kratta_t2[" << 	KRATTA_NR_OF_CRYSTALS << "]/s"
          << ":kratta_t3[" << 	KRATTA_NR_OF_CRYSTALS << "]/s"

//          << ":plastic_s1[" << 	KRATTA_NR_OF_PLASTICS << "]/s1"
//          << ":plastic_s2[" << 	KRATTA_NR_OF_PLASTICS << "]/s"
//          << ":plastic_s3[" << 	KRATTA_NR_OF_PLASTICS << "]/s"

//          << ":plastic_p1[" << 	KRATTA_NR_OF_PLASTICS << "]/s1"
//          << ":plastic_p2[" << 	KRATTA_NR_OF_PLASTICS << "]/s"
//          << ":plastic_p3[" << 	KRATTA_NR_OF_PLASTICS << "]/s"

//          << ":plastic_t1[" << 	KRATTA_NR_OF_PLASTICS << "]/s"
//          << ":plastic_t2[" << 	KRATTA_NR_OF_PLASTICS << "]/s"
//          << ":plastic_t3[" << 	KRATTA_NR_OF_PLASTICS << "]/s"   ;
          << ":plastic_time[" << 	KRATTA_NR_OF_PLASTICS << "]/s1";
//          << ":plastic_energy[" << 	KRATTA_NR_OF_PLASTICS << "]/s" ;

        string format = s.str();

        tree->Branch("event",
                     &store.hector_adc,
                     format.c_str());


        //tree->Branch("Ej",E, Form("E[%d]/F",mult));
        //  tree->Branch("Z",Z,Form("Z[%d]/I",mult));
        //  tree->Branch("Eleft",Eleft,Form("Eleft[%d]/F",mult));
        //  tree->Branch("Spin",Spin,Form("Spin[%d]/F",mult));
    }
}

//*********************************************************************
void Thec_kratta_tree::analyse_current_event()
{

    if(RisingAnalysis_ptr->ntuple_RAW_is_collected) {
        static int counter = 0 ;
        counter++;

// store.ppacA_x_left = event_unpacked->ppacA_x_left;
#define PUT(x)   store.x = event_unpacked->x;

        for(int i = 0 ; i < 32 ; ++i) {
            PUT(hector_adc[i]);
            PUT(hector_tdc[i]);
            PUT(phoswich_adc[i]);
            PUT(phoswich_tdc[i]);
        }

        for(int i = 0 ; i < KRATTA_NR_OF_CRYSTALS ; ++i) {
            store.kratta_s0[i] = event_unpacked->kratta[i][0];
            store.kratta_s1[i] = event_unpacked->kratta[i][1];
            store.kratta_s2[i] = event_unpacked->kratta[i][2];

            store.kratta_p0[i] = event_unpacked->kratta[i][3];
            store.kratta_p1[i] = event_unpacked->kratta[i][4];
            store.kratta_p2[i] = event_unpacked->kratta[i][5];

            store.kratta_t0[i] = event_unpacked->kratta[i][6];
            store.kratta_t1[i] = event_unpacked->kratta[i][7];
            store.kratta_t2[i] = event_unpacked->kratta[i][8];
        }


        for(int i = 0 ; i < KRATTA_NR_OF_PLASTICS ; ++i) {
            if(event_unpacked->plastic[i] < 20000)
            store.plastic_time[i] = event_unpacked->plastic[i];

//            if(store.plastic_time[i] )
//            cout << "wlozone do tree root plastic " << i
//                    << " time = " <<
//                    event_unpacked->plastic[i][0] << endl;
        }
        /*
        PUT(ppacA_x_right);
        PUT(ppacA_y_up);
        PUT(ppacA_y_down);
        PUT(ppacA_cathode);
        //...
        */


//#endif

        if(tree) {
            tree->Fill();

        } else {
            cout << __func__ << "Error: tree pointer is nullptr " << endl;
        }
        static time_t  last;

        time_t now = time(nullptr);

        if(now - last > 20)

//     if( !(counter % 100000))
        {
            cout << "NOTE: You asked to produce a 'RAW tree', " << root_filename
                 << "\n   I am  doing it, but this slows down the analysis..." << endl;

            last = now ;
            tree->OptimizeBaskets(); // Basia Wasilewska ask for this
        }

    } // endif
}
//*********************************************************************
void Thec_kratta_tree::write_the_tree()
{
    if(RisingAnalysis_ptr->ntuple_RAW_is_collected) {
        if(flag_file_and_tree_already_opened){
          cout << "Writing the tree" << endl;
            hfile->Write();
            cout << "After Writing the tree" << endl;
            
        }
    }
}
//*********************************************************************
// #endif

#endif  //CERN_ROOT_INSTALLED

#endif  // CURRENT_EXPERIMENT_TYPE == EXOTIC_EXPERIMENT



