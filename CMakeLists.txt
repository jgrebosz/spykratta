cmake_minimum_required(VERSION 2.8)
project(spy)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -L./   -pthread -Wall")
message("moje info PROJECT_SOURCE_DIR points to " ${PROJECT_SOURCE_DIR})
#this second parameter below was taken from:  root-config --incdir
include_directories(
  src/    /usr/include/root
)

#TO MAKE  ROOT/NOROOT VERSION - UNCOMMENT ONE OF THE FOLLOWING COMMANDS

#   if  CERN_ROOT_INSTALLED == true we can link with root functions
link_libraries( ${PROJECT_SOURCE_DIR}/src/libgsievt.a rt  Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript
Matrix Physics MathCore Thread Gui pthread
m dl
)

#       if CERN_ROOT_INSTALLED == false
# link_libraries( ${PROJECT_SOURCE_DIR}/src/libgsievt.a rt pthread m dl )



# I made this to make ROOT available in the program
#this is taken from the root progam   root-config  --ldflags --libs
#-m64 -L/usr/lib/x86_64-linux-gnu -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic
#AM_CPPFLAGS = -pthread 


add_executable(spy src/MainUserAnalysis.cxx src/T32_signal_module.cxx src/TIFJAnalysis.cxx
	src/TIFJCalibrateFactory.cxx src/TIFJCalibrateProcessor.cxx src/TIFJCalibratedEvent.cxx 
	src/TIFJCalibration.cxx src/TIFJEvent.cxx src/TIFJEventFactory.cxx src/TIFJEventProcessor.cxx 
	src/Tcondition_description.cxx src/Tcracow_exception.cxx src/Texotic_one_stripe.cxx src/Texotic_one_stripe.h 
	src/Texotic_strip_detector.cxx src/Texotic_strip_detector.h src/Texotic_vector_strips.cxx 
	src/Texotic_vector_strips.h src/Tfile_helper.cxx src/Tframe_simple.cxx src/Tfrs.cxx src/Tfrs_element.cxx 
	src/Thistograms_jurek.cxx src/Tincrementer_donnor.cxx src/TjurekMbsEvent.cxx src/TjurekMbsSubEvent.cpp 
	src/TjurekPolyCond.cpp src/Tlookup_table.cxx src/Tlookup_table_doublets.cxx src/Tlookup_table_miniball.cxx 
	src/Tlookup_table_triple_universal.cxx src/Tnamed_pointer.cxx src/Tone_signal_module.cxx 
	src/Tone_signal_pattern_module.cxx src/Tself_gate_abstract_descr.cpp src/Tself_gate_ger_addback_descr.cpp 
	src/Tself_gate_agata_psa_descr.cxx 
	src/Tself_gate_ger_descr.cpp src/Tself_gate_hec_descr.cpp src/Tself_gate_mib_descr.cpp src/Tsi_detector.cxx 
	src/Tsynchro_watchdog_curved.cxx src/Ttwo_signal_module.cxx src/Tuser_condition.cxx 
	src/Tuser_gated_spectrum_definition.cxx src/Tuser_incrementer.cxx src/Tuser_spectrum.cxx src/exotic.cpp src/spectrum.cxx
	src/tjurekabstracteventelement.cxx src/tjurekabstracteventprocessor.cpp src/user_spectrum_description.cxx 
	src/Tmultiwire.cxx  src/Tmicro_channel_plate.cxx src/tsocket_fo_data.cxx src/Tpisolo_delta_e.cxx src/Tpisolo_si_final.cxx 
	src/Tpisolo_tof.cxx src/TprismaManager.cc   src/TprismaMcp.cc src/TprismaPPAC.cc src/TprismaIonCh.cc src/TprismaSide.cc 
	src/TfastSolver.cc src/TzedCalculator.cc src/TmassCalculator.cc src/Vectors.cc src/ConfigurationFile.cc
	src/detData.cc src/calManager.cc src/banManager.cc src/banGate.cc src/calCoef.cc src/RandomGenerator.cc 
	src/tselfgate_type.cxx src/mbs_listner.cpp src/Tlookup_table_kratta.cpp src/Thector.cxx src/Thector_baf.cxx 
	src/Tkratta.cpp src/Tkratta_crystal.cpp src/Thec_kratta_tree.cpp)

install(TARGETS spy RUNTIME DESTINATION bin)
