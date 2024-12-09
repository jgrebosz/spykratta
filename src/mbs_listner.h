/*
 * Copyright 2014 Jerzy Grebosz <email>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef MBS_LISTNER_H
#define MBS_LISTNER_H

#include "MBSc2cpp.h"
#include <string>
#include <stdint.h>

//typedef unsigned int32_t  uint32_t ;
class mbs_listner
{
protected:
    s_evt_channel 	input_channel;  
    s_filhe	  	*file_header;    
    s_bufhe		*buffer_header;
    int 		*event_header_ptr ;
     
    int events_read;
    int event_nr;

    // for getting subevents
    s_ves10_1 	*subevent_header_ptr;
    
    uint32_t 	*event_data_ptr;		// INTU4   
    int32_t	length_datawords;		// INTS4
    
    int how_many_subs;  	// in current event_data
    int subev_nr;           	// which nr of sub

public:
    mbs_listner();
    int open( MBS_StreamType type, std::string name );
    ~mbs_listner();
    int get_next_event();

    //s_ves10_1 *    
    int get_next_subevent_if_any(); // returning result 
    void close();
    
    int subtype() { return subevent_header_ptr->i_subtype; }
    int type() { return subevent_header_ptr->i_type; }
    int procid() { return subevent_header_ptr->i_procid; }
    char control() { return subevent_header_ptr->h_control; }
    uint32_t* data() { return event_data_ptr; }
    int32_t length() { return length_datawords; ; }
    int give_event_nr() { return event_nr;}
    int give_how_many_subs() { return how_many_subs; }
};

#endif // MBS_LISTNER_H
