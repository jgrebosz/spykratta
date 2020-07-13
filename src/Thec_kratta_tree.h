#ifndef Thec_kratta_tree_H
#define Thec_kratta_tree_H
/**
@author Jerzy Grebosz

To be able ty compile together with the ROOT library, you need options, which can bye obtained by running command:
	root-config --glibs --cflags 
the result was ------------------------------------------------------------------

	-L/usr/local/root/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lGui -pthread -lm -ldl -rdynamic -pthread -m32 -I/usr/local/root/include

and I put this into src/Makefile.am    as: ----------------------------------------

	# I made this to make ROOT available in the program
	AM_LDFLAGS = -L/usr/local/root/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lGui -pthread -lm -ldl -rdynamic -pthread -m32 

	AM_CPPFLAGS = -I/usr/local/root/include


*/

#include "experiment_def.h"
#if CURRENT_EXPERIMENT_TYPE == IFJ_KRATTA_HECTOR_EXPERIMENT

class TFile;
class TTree;
const int mult = 5;
#include <string>
using namespace std;
#include "Tfrs_element.h"

////////////////////////////////////////////////////////////
class Thec_kratta_tree : public Tfrs_element
{
  TFile *hfile;
  TTree *tree;
  bool flag_file_and_tree_already_opened;

  std::string   root_filename;

  int16_t ( TIFJEvent::*hector_tdc_data_ptr )[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS];
  int16_t ( TIFJEvent::*hector_adc_data_ptr )[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS];
  int16_t ( TIFJEvent::*phoswich_tdc_data_ptr )[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS];
  int16_t ( TIFJEvent::*phoswich_adc_data_ptr )[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS];

  int16_t ( TIFJEvent::*kratta_data_ptr )[KRATTA_NR_OF_CRYSTALS][9];
  int16_t ( TIFJEvent::*plastic_data_ptr )[KRATTA_NR_OF_PLASTICS];

  struct {
         int16_t  hector_adc[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS];
         int16_t  hector_tdc[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS];
         int16_t  phoswich_adc[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS];
         int16_t  phoswich_tdc[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS];

		 int16_t  kratta_s0[KRATTA_NR_OF_CRYSTALS];
		 int16_t  kratta_s1[KRATTA_NR_OF_CRYSTALS];
		 int16_t  kratta_s2[KRATTA_NR_OF_CRYSTALS];

		 int16_t  kratta_p0[KRATTA_NR_OF_CRYSTALS];
		 int16_t  kratta_p1[KRATTA_NR_OF_CRYSTALS];
		 int16_t  kratta_p2[KRATTA_NR_OF_CRYSTALS];
		 
		 int16_t  kratta_t0[KRATTA_NR_OF_CRYSTALS];
		 int16_t  kratta_t1[KRATTA_NR_OF_CRYSTALS];
		 int16_t  kratta_t2[KRATTA_NR_OF_CRYSTALS];
		 

         int16_t  plastic_time[KRATTA_NR_OF_PLASTICS];
         //int16_t  plastic_energy[KRATTA_NR_OF_PLASTICS];

} store ;


public:
  Thec_kratta_tree(std::string who,
                    int16_t ( TIFJEvent::*hector_tdc_data_ptr_a )[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS],
                    int16_t ( TIFJEvent::*hector_adc_data_ptr_a )[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS],
                    int16_t ( TIFJEvent::*phoswich_tdc_data_ptr_a )[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS],
                    int16_t ( TIFJEvent::*phoswich_adc_data_ptr_a )[PLASTIC_HOW_MANY_TDC_ADC_CHANNELS],
                    int16_t ( TIFJEvent::*kratta_data_ptr_a ) [KRATTA_NR_OF_CRYSTALS][9],
                    int16_t ( TIFJEvent::*plastic_data_ptr_a ) [KRATTA_NR_OF_PLASTICS]
	     );

  void analyse_current_event();
  //void make_preloop_action(ifstream &)  {}
  void write_the_tree();
  ~Thec_kratta_tree();
	
 void make_preloop_action(ifstream &);   // read the calibration factors, gates;

};
#endif  // CURRENT_EXPERIMENT_TYPE==IFJ_KRATTA_HECTOR_EXPERIMENT
#endif
