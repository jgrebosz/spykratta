cmake_minimum_required(VERSION 3.20)
project(spy)

#--------------------------------------------
# When DEBUG  uncomment this line below
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++17 -L./   -pthread -Wall -no-pie")

# when NODEBUG
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -L./   -pthread -Wall -no-pie")

#---------------------------------------------

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-format-nonliteral"  )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wno-class-memaccess" )

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op  -pedantic -Wall -Wextra -Wcast-align -Wcast-qual")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wold-style-cast -Woverloaded-virtual -Wsign-conversion -Wshadow")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}   -Wredundant-decls   -Wsign-promo")
# -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused")

# -Weffc++


#if I change the computer   e when I compile this program
#usually I need to make commands:
# cmake .
# rm CMakeCache.txt   <--- this is important
# cmake .
# make clean
# make all


message("my: info PROJECT_SOURCE_DIR points to " ${PROJECT_SOURCE_DIR})
#this second parameter below was taken from:  root-config --incdir
include_directories(
src/
/home/analiza/root_install/include
)



#NOTE: data files are available by JACOBI as /disk/CCB

#TO MAKE  ROOT/NOROOT VERSION - UNCOMMENT ONE OF THE FOLLOWING COMMANDS
link_directories(/home/analiza/root/root/lib)
link_directories(/home/analiza/root_install/lib)
#   if  CERN_ROOT_INSTALLED == true we can link with root functions
#link_libraries( ${PROJECT_SOURCE_DIR}/src/libgsievt.a rt Core Cint RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript
link_libraries( ${PROJECT_SOURCE_DIR}/src/libgsievt.a rt Core RIO Net Hist Graf Graf3d Gpad Tree Rint Postscript
Matrix Physics MathCore Thread Gui pthread
m dl
)


#       if CERN_ROOT_INSTALLED == false
# link_libraries( ${PROJECT_SOURCE_DIR}/src/libgsievt.a rt pthread m dl )



# I made this to make ROOT available in the program
#this is taken from the root progam   root-config  --ldflags --libs
#-m64 -L/home/analiza/root/root/lib -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic
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
    src/Tlookup_table_triple_universal.cxx src/Tnamed_pointer.cxx
#    src/Tone_signal_module.cxx
	src/Tone_signal_pattern_module.cxx src/Tself_gate_abstract_descr.cpp src/Tself_gate_ger_addback_descr.cpp 
        src/Tself_gate_agata_psa_descr.cxx
	src/Tself_gate_ger_descr.cpp src/Tself_gate_hec_descr.cpp src/Tself_gate_mib_descr.cpp src/Tsi_detector.cxx 
	src/Tsynchro_watchdog_curved.cxx src/Ttwo_signal_module.cxx src/Tuser_condition.cxx 
	src/Tuser_gated_spectrum_definition.cxx src/Tuser_incrementer.cxx src/Tuser_spectrum.cxx src/exotic.cpp src/spectrum.cxx
	src/tjurekabstracteventelement.cxx src/tjurekabstracteventprocessor.cpp src/user_spectrum_description.cxx 
        src/Tmultiwire.cxx  src/Tmicro_channel_plate.cxx src/tsocket_fo_data.cxx

        src/Vectors.cc src/ConfigurationFile.cc
	src/detData.cc src/calManager.cc src/banManager.cc src/banGate.cc src/calCoef.cc src/RandomGenerator.cc 
    src/tselfgate_type.cxx src/mbs_listner.cpp
    src/Thector.cxx 
    src/Tparis.cxx
#    src/Thector_baf.cxx   because it contains a template
        src/Tkratta.cpp src/Tkratta_crystal.cpp src/Tkratta_crystal_plastic.cpp
        src/Thec_kratta_tree.cpp src/Tself_gate_kratta_descr.cpp
        src/tsignal_processing.cpp
        src/v775.cpp
        src/v785.cpp
        src/v878.cpp
        src/v830.cpp
        src/DataDecoder.cpp
        src/v1724.cpp
        src/v1730B.cpp
         src/v1190.cpp
         src/v2495s.cpp
        src/LTable.cpp
        src/LTBranch.cpp
        src/Run.cpp
        src/LTRecord.cpp
        src/Tn_silicon_xy.cxx
        src/v2740.cpp
        )

install(TARGETS spy RUNTIME DESTINATION bin)
