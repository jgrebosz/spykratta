//---------------------------------------------------------------
// here are definitions for particular experiment
#ifndef EXPDEF_H
#define EXPDEF_H

//#include <cstdint>

// To make Debug/NoDebug version go to the Project->Open Configuration
// To make ROOT / NOROOT - version -->go to the CmakeLists.txt file


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DO NOT CHANGE HERE, you may change 12 lines below this
#define RISING_STOPPED_BEAM_CAMPAIGN     1000
#define RISING_FAST_BEAM_CAMPAIGN        1100
#define G_FACTOR_OCTOBER_2005           1200
#define RISING_ACTIVE_STOPPER_BEAM_CAMPAIGN     1300
#define RISING_ACTIVE_STOPPER_100TIN                       1400
#define RISING_ACTIVE_STOPPER_APRIL_2008            1500
#define RISING_ACTIVE_STOPPER_JULY_2008            1600

#define EXOTIC_EXPERIMENT            1700
#define PISOLO2_EXPERIMENT            1800
#define PRISMA_EXPERIMENT            1900
#define IFJ_KRATTA_HECTOR_EXPERIMENT   2000

// #define STEPHANE_ASK_FOR_ORIGINAL_TIME_IN_THE_ROOT_TREE true
//==========================================================

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// HERE YOU MAKE CHANGES (you make the decision) !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// uncomment the one line which corresponds to your experimment and  coment the other lines

// #define CURRENT_EXPERIMENT_TYPE       EXOTIC_EXPERIMENT
//#define CURRENT_EXPERIMENT_TYPE       PISOLO2_EXPERIMENT
// #define CURRENT_EXPERIMENT_TYPE       PRISMA_EXPERIMENT

#define CURRENT_EXPERIMENT_TYPE 	IFJ_KRATTA_HECTOR_EXPERIMENT
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



// Now details:

//#################################################################
#if CURRENT_EXPERIMENT_TYPE==IFJ_KRATTA_HECTOR_EXPERIMENT
//#################################################################
#define MBS_DATAFILE
#define HECTOR_PRESENT
#define KRATTA_PRESENT
#define KRATTA_PLASTIC_PRESENT


//constexpr int KRATTA_NROW = 4;
//constexpr int KRATTA_NCOL = 4;

constexpr int KRATTA_NR_OF_CRYSTALS = 40;   // 40;
constexpr int KRATTA_NR_OF_PLASTICS = KRATTA_NR_OF_CRYSTALS * 4;

constexpr int NR_OF_SILICONS = 4;   // 32;

constexpr int PLASTIC_HOW_MANY_TDC_ADC_CHANNELS  {64 };
constexpr int NR_DIGITIZERS = 4; // (0-3) -> max nr of digitizers

#define CERN_ROOT_INSTALLED  false
// #define CERN_ROOT_INSTALLED  true
    
#define USER_INCREMENTERS_ENABLED

//#################################################################
#elif CURRENT_EXPERIMENT_TYPE==EXOTIC_EXPERIMENT
//#################################################################

// #define FINGER_SCINTILLATOR_PRESENT
//#define XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
// #define VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT     there was not!
//#define LONG_RANGE_SHORT_RANGE_TDC_USED

//#define SCI41_VERTIC_PRESENT
// if we use the old style Music with 4 electrods
//#define MUSICBIG_USED
//#define MUSIC42_USED
//#define TOF_21_42__AND_RELATED_AOQ_USED


//#define ACTIVE_STOPPER_PRESENT
//#define  ACTIVE_STOPPER2_PRESENT

// const unsigned int NR_OF_STOPPER_STRIPES_X = 16;
// const unsigned int NR_OF_STOPPER_STRIPES_Y = 16;
// const unsigned int HOW_MANY_STOPPER_MODULES = 3;  // plates of combined XY stripes



// #define THREE_TIMESTAMP_LONGWORDS_AT_BEGINNING_OF_FRS_SUBEVENT



// const unsigned int HOW_MANY_GE_CLUSTERS = 15;
// NOTE: The letters of corresponding clusters are defined in Trising::cluster_characters


#define NR_OF_EXOTIC_MODULES   16
#define NR_OF_EXOTIC_STRIPES_X  32
#define NR_OF_EXOTIC_SAMPLES  8


//####################################################################
//#################################################################
#elif CURRENT_EXPERIMENT_TYPE == PISOLO2_EXPERIMENT
//#################################################################

//#define OLD_PISOLO_DATA

// #define NR_OF_EXOTIC_MODULES   8
// #define NR_OF_EXOTIC_STRIPES_X  32
// #define NR_OF_EXOTIC_SAMPLES  8



//####################################################################
//####################################################################
//####################################################################
#elif CURRENT_EXPERIMENT_TYPE == G_FACTOR_OCTOBER_2005
//####################################################################

#define RISING_GERMANIUMS_PRESENT
//#define HECTOR_PRESENT
#define CATE_PRESENT
//#define MINIBALL_PRESENT
//#define FINGER_SCINTILLATOR_PRESENT
#define DGF_SEPARATE_BRANCH_PRESENT

#define XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
#define VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT
#define BGO_PRESENT
#define TIME_DIFF_USED

#define WE_WANT_ORIGINAL_DGF_TIME

#include <cstdlib>

const unsigned int HOW_MANY_GE_CLUSTERS = 8;
// NOTE: The letters of corresponding clusters are defined in Trising::cluster_characters
//---------------------------------------------------------------------
//#################################################################
#elif CURRENT_EXPERIMENT_TYPE==RISING_FAST_BEAM_CAMPAIGN
//#################################################################
#define RISING_GERMANIUMS_PRESENT
#define HECTOR_PRESENT
#define CATE_PRESENT

//#define MINIBALL_PRESENT

//#define FINGER_SCINTILLATOR_PRESENT
#define DGF_SEPARATE_BRANCH_PRESENT
#define SCI41_VERTIC_PRESENT

//#define XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT   - there was NOT
#define VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT
#define DOPPLER_CORRECTION_NEEDED

#define WE_WANT_FRS_SCALERS

const unsigned int HOW_MANY_GE_CLUSTERS = 15;
// NOTE: The letters of corresponding clusters are defined in Trising::cluster_characters

//#################################################################
#elif CURRENT_EXPERIMENT_TYPE==RISING_STOPPED_BEAM_CAMPAIGN
//#################################################################
#define RISING_GERMANIUMS_PRESENT
//#define HECTOR_PRESENT
//#define CATE_PRESENT
//#define MINIBALL_PRESENT
// #define FINGER_SCINTILLATOR_PRESENT
#define XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
// #define VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT     there was not!
#define LONG_RANGE_SHORT_RANGE_TDC_USED
#define MUSIC42_USED
#define TOF_21_42__AND_RELATED_AOQ_USED
#define SCI41_VERTIC_PRESENT
// Marek Pfutzner likes this below
//#define TIME_DIFF_USED

// Lucia wants to calibrate twice the energy
#define LUCIA_TWO_STEPS_ENERGY_CALIBRATION

const unsigned int HOW_MANY_GE_CLUSTERS = 15;
// NOTE: The letters of corresponding clusters are defined in Trising::cluster_characters



//#################################################################
#elif CURRENT_EXPERIMENT_TYPE==RISING_ACTIVE_STOPPER_100TIN
//#################################################################
#define RISING_GERMANIUMS_PRESENT
//#define HECTOR_PRESENT
//#define CATE_PRESENT
//#define MINIBALL_PRESENT
// #define FINGER_SCINTILLATOR_PRESENT
#define XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
// #define VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT     there was not!
#define LONG_RANGE_SHORT_RANGE_TDC_USED

// if we use the old style Music with 4 electrods
//#define MUSICBIG_USED
#define MUSIC42_USED
#define TOF_21_42__AND_RELATED_AOQ_USED


#define THREE_TIMESTAMP_LONGWORDS_AT_BEGINNING_OF_FRS_SUBEVENT

const unsigned int HOW_MANY_GE_CLUSTERS = 15;
// NOTE: The letters of corresponding clusters are defined in Trising::cluster_characters

#if 1   // put zero for Fabio TPC tests (he doesn't need active stopper spectra)

#define POSITRON_ABSORBER_PRESENT
#define POSITRON_ABSORBER_NR                         10
#define POSITRON_ABSORBER_STRIPS_NR         7

#define MUNICH_MATRIX_STRIPES_X         60
//#define MUNICH_MATRIX_STRIPES_Y         60

#define TRACKING_IONISATION_CHAMBER_X     192
#define TRACKING_IONISATION_CHAMBER_Y     64


#define MUN_ACTIVE_STOPPER_PRESENT
const unsigned int NR_OF_MUN_STOPPER_MODULES = 3;
const unsigned int NR_OF_MUN_STOPPER_STRIPES_X = 60; // no change
const unsigned int NR_OF_MUN_STOPPER_STRIPES_Y = 32;  // was 40
#endif

// below is a variable as walk-around when timestamp cable was not giving the data
// to the beggining of the FRS subevent (above) (this words were empty.
// Stephane says that almost the same data comes in dgf subevent (id=31)
// in the buffer header data - words 3,4,5
// #define NO_TITRIS_CABLE


#define TPC41_PRESENT
#define TPC42_PRESENT
#define TPC_NR_OF_DRIFT_TIMES   4
#define TPC_NR_OF_X_TIMES  2


#define SCI22_PRESENT
#define  TOF_22_41_PRESENT
#define  TOF_22_42_PRESENT
//#define  TOF_21_22_PRESENT   NOT!!!!!
#define  TOF_41_42_PRESENT
// #define SCI41_VERTIC_PRESENT                                  NOT !!!!!!!!!!!
#define TIN100_FRS_SCALERS
#define TOF_22_41__AND_RELATED_AOQ_USED


// for testing purposes Christoph H. wants this data in he Root Tree
#define  RAW_DATA_MUNICH_ACTIVE_STOPPER_IN_THE_ROOT_TREE
#define  RAW_DATA_MUNICH_TIC_IN_THE_ROOT_TREE

#define MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE

#define FRS_PATTERN_MODULE_PRESENT
#define ACTIVE_STOPPER_HIT_PATTERN  "munich_timer"


#define USER_INCREMENTERS_ENABLED
#define MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED


//#################################################################
#elif CURRENT_EXPERIMENT_TYPE==RISING_ACTIVE_STOPPER_APRIL_2008
//#################################################################
#define RISING_GERMANIUMS_PRESENT
//#define HECTOR_PRESENT
//#define CATE_PRESENT
//#define MINIBALL_PRESENT
// #define FINGER_SCINTILLATOR_PRESENT
#define XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
// #define VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT     there was not!
#define LONG_RANGE_SHORT_RANGE_TDC_USED

#define SCI41_VERTIC_PRESENT
// if we use the big, old style Music with 4 electrods
// #define MUSICBIG_USED
#define MUSIC42_USED
#define TOF_21_42__AND_RELATED_AOQ_USED


#define TPC41_PRESENT
#define TPC42_PRESENT
#define TPC_NR_OF_DRIFT_TIMES   4
#define TPC_NR_OF_X_TIMES  2


#define ACTIVE_STOPPER_PRESENT
#define  ACTIVE_STOPPER2_PRESENT


const unsigned int NR_OF_STOPPER_STRIPES_X = 16;
const unsigned int NR_OF_STOPPER_STRIPES_Y = 16;
const unsigned int HOW_MANY_STOPPER_MODULES = 3;  // plates of combined XY stripes

#define THREE_TIMESTAMP_LONGWORDS_AT_BEGINNING_OF_FRS_SUBEVENT

// below is a variable as walk-around when timestamp cable was not giving the data
// to the beggining of the FRS subevent (above) (this words were empty.
// Stephane says that almost the same data comes in dgf subevent (id=31)
// in the buffer header data - words 3,4,5
//#define NO_TITRIS_CABLE

const unsigned int HOW_MANY_GE_CLUSTERS = 15;
// NOTE: The letters of corresponding clusters are defined in Trising::cluster_characters

#define MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE

#define USER_INCREMENTERS_ENABLED

//#define MW51_USED
//#define MW71_USED
//#define MW81_USED
//#define MW21_USED


#define TPC21_PRESENT
#define TPC22_PRESENT

#define CASES_FOR_IMPLANTATION_TRIGGERS case 2: case 3:
#define CASES_FOR_DECAY_TRIGGERS   case 4: case 5: case 6:

#define FRS_PATTERN_MODULE_PRESENT
#define ACTIVE_STOPPER_HIT_PATTERN  "active_stopper_hit_pattern"


//#################################################################
#elif CURRENT_EXPERIMENT_TYPE==RISING_ACTIVE_STOPPER_JULY_2008
//#################################################################
#define RISING_GERMANIUMS_PRESENT
//#define HECTOR_PRESENT
//#define CATE_PRESENT
//#define MINIBALL_PRESENT
// #define FINGER_SCINTILLATOR_PRESENT
#define XIA_ELECTRONICS_FOR_GERMANIUMS_PRESENT
// #define VXI_ELECTRONICS_FOR_GERMANIUMS_PRESENT     there was not!
#define LONG_RANGE_SHORT_RANGE_TDC_USED

#define SCI41_VERTIC_PRESENT
// if we use the big, old style Music with 4 electrods
// #define MUSICBIG_USED
#define MUSIC42_USED
#define TOF_21_42__AND_RELATED_AOQ_USED


#define TPC41_PRESENT
#define TPC42_PRESENT
#define TPC_NR_OF_DRIFT_TIMES   4
#define TPC_NR_OF_X_TIMES  2


#define ACTIVE_STOPPER_PRESENT
#define  ACTIVE_STOPPER2_PRESENT


const unsigned int NR_OF_STOPPER_STRIPES_X = 16;
const unsigned int NR_OF_STOPPER_STRIPES_Y = 16;
const unsigned int HOW_MANY_STOPPER_MODULES = 3;  // plates of combined XY stripes

#define THREE_TIMESTAMP_LONGWORDS_AT_BEGINNING_OF_FRS_SUBEVENT

// Below is a variable as walk-around when timestamp cable was not giving the data
// to the beggining of the FRS subevent (above) (this words were empty.
// Stephane says that almost the same data comes in dgf subevent (id=31)
// in the buffer header data - words 3,4,5
#define NO_TITRIS_CABLE

const unsigned int HOW_MANY_GE_CLUSTERS = 15;
// NOTE: The letters of corresponding clusters are defined in Trising::cluster_characters

#define MUSIC_DEPOSIT_ENERGY_CORRECTED_BY_PRESSURE

#define USER_INCREMENTERS_ENABLED

//#define MW51_USED
//#define MW71_USED
//#define MW81_USED
//#define MW21_USED


#define TPC21_PRESENT
#define TPC22_PRESENT

#define CASES_FOR_IMPLANTATION_TRIGGERS case 2: case 7:
#define CASES_FOR_DECAY_TRIGGERS   case 3: case 4:

#define FRS_PATTERN_MODULE_PRESENT
#define ACTIVE_STOPPER_HIT_PATTERN  "active_stopper_hit_pattern"


// only for July2008 experiment
#define  ACTIVE_STOPPER3_PRESENT
#define  MULTIHIT_V1290_PRESENT
#define ACTIVE_STOPPERS_HAVE_TIME_INFORMATION
#define ACTIVE_STOPPERS_SECOND_AND_THIRD_HIT_IN_THE_TIME_GATE
//---------------------

#define MUSIC_Y_POSITION_CORRECTION_ALSO_APPLIED


//#################################################################
#elif CURRENT_EXPERIMENT_TYPE==PRISMA_EXPERIMENT
//#################################################################
#define USER_INCREMENTERS_ENABLED



#else   // Nothing ?  #################################################################
#error "Experiment version not defined properly"
#endif  // typ exper

//#################################################################


#endif  //guardian
