/***************************************************************************
                          doppler_enums.h  -  description
                             -------------------
    begin                : Wed Jun 9 2004
    copyright            : (C) 2004 by dr. Jerzy Grebosz
    email                : jerzy.grebosz@ifj.edu.pl
 ***************************************************************************/
#ifndef _DOPPLER_ENUMS_
#define  _DOPPLER_ENUMS_

/** Depending of the status of mutliwires at s4 */
enum type_of_tracking_requirements
  {
    tracking_obligatory = 1,
    no_tracking,
    tracking_if_available,
    tracking_from_the_Si_at_target
  };
  // tracking_position_on_target ;
  //-------------------


//** Depending of the status of mutliwires at s4 */
enum type_of_lycca_requirements
  {
    position_obligatory = 1,
    only_segment_nr,
    no_lycca_info,
    position_if_available
  };
  //position_on_lycca ;

  
enum type_of_doppler_algorithm
  {
    tracked_algorithm =1,  basic_algorithm
  };

#endif   //  _DOPPLER_ENUMS_
