/***************************************************************************
                          Tself_gate_ger_descr.h  -  description
                             -------------------
    begin                : Mon Sep 8 2003
    copyright            : (C) 2003 by dr. Jerzy Grebosz, IFJ Krakow, Poland
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/


#ifndef TSELF_GATE_GER_DESCR_H
#define TSELF_GATE_GER_DESCR_H

#include <string>
using namespace std;
#include <cstdlib>
#include "Tself_gate_abstract_descr.h"
#include "TjurekPolyCond.h"


//#include "Go4Analysis/TGo4Analysis.h"  // for registering conditions



/**Text file which describes the gate on the germanium crystal
  *@author dr. Jerzy Grebosz, IFJ Krakow, Poland
  */
//////////////////////////////////////////////////////////////////////////
class Tself_gate_ger_descr : public Tself_gate_abstract_descr
{
public:
  string name;

  bool enable_en4gate ;
  double en4_gate[2];

  bool enable_en20gate ;
  double en20_gate[2];

  bool enable_time_gate ;
  double time_gate[2];

  bool enable_LR_time_gate ;
  double LR_time_gate[2];
  
  bool enable_SR_time_gate ;
  double SR_time_gate[2];
  
  
    // For Marek Pfutzner 
  bool enable_xy_gamma_time_difference;
  bool flag_this_is_for_X_incrementer;
  double safe_for_x_time;
  
  // For Marek's polygon on energy-time
   // ---------------- for polygon gate on energy vs time (one of  three times)
  bool  enable_energy_time_polygon_gate;
  string energy_time_polygon_gate;
  
  enum which_time_for_banana { time_cal, LR_time_cal, SR_time_cal } ;
  which_time_for_banana    which_time;
  TjurekPolyCond * polygon ;
  
  
  bool enable_geom_theta_gate ;
  double geom_theta_gate[2];

  bool enable_geom_phi_gate ;
  double geom_phi_gate[2];

  // gamma - particle angle
  bool enable_gp_theta_gate ;
  double gp_theta_gate[2];

  bool enable_gp_phi_gate ;
  double gp_phi_gate[2];

  // this is the multiplicity inside the cluster where this crystal belongs
  bool enable_mult_in_cluster_gate ;
  double mult_in_cluster_gate[2];

  // this is the BGO inside the cluster where this crystal belongs
  bool enable_BGO_in_cluster_gate ;
  double BGO_in_cluster_gate[2];

  //---------------two virtual functions
  void this_is_x_incrementer()
  { flag_this_is_for_X_incrementer = true; safe_for_x_time = 0; }
  void this_is_y_incrementer()
  { flag_this_is_for_X_incrementer = false; }
  
  // true when we use the option xy difference
  bool not_for_1Dspectrum()
  {  return (enable_xy_gamma_time_difference ?  true : false) ;  }
   
  //----------------------------------
  Tself_gate_ger_descr();
  virtual ~Tself_gate_ger_descr() {} ;
  void read_definition_from(string pathed_name);
  void write_definitions(string path_only);
};

#endif
