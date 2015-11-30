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

#include "mbs_listner.h"
using namespace std;
#include <iostream>

//********************************************************************************************
mbs_listner::mbs_listner()
{

    event_header_ptr  = nullptr;
    events_read = 0;
    //file_header = 0;
    buffer_header = 0;
}
//********************************************************************************************
mbs_listner::~mbs_listner()
{

}
//********************************************************************************************
int mbs_listner::open( MBS_StreamType type, string name )
{
//     cout << __func__ << endl;
    int result = f_evt_get_open ( type, ( char* ) name.c_str(),
                                  &input_channel,
                                  ( CHARS** ) NULL, //&file_header,   // Jesli noe NULL, to tu przyjdzie informacja o file, lub pli
                                  1, 	// co ktory blok danych
                                  0 );    	// nie uzywany parametr

    if ( result != GETEVT__SUCCESS )
    {
      cout << "Cant open the file " << name.c_str() << ": code for error is " << result
      << endl;
      string explanation ;
switch(result)
{
 
  case  GETEVT__FAILURE  :    explanation ="Failure   "; break ;
  case  GETEVT__FRAGMENT:    explanation ="FRAGMENT   "; break ;
  case  GETEVT__NOMORE:      explanation ="NO MORE:  "; break ;
  case  GETEVT__NOFILE:      explanation ="NO FILE   "; break ;
  case  GETEVT__NOSERVER    :  explanation ="NO SERVER  "; break ;
  case  GETEVT__RDERR:         explanation ="RD ERR:   "; break ;
  case  GETEVT__CLOSE_ERR   :  explanation ="CLOSE_ERR    "; break ;
  case  GETEVT__NOCHANNEL   :  explanation ="NO CHANNEL   "; break ;
  case  GETEVT__TIMEOUT     :  explanation ="TIMEOUT    "; break ;
  case  GETEVT__NOTAGFILE  :  explanation ="NO TAGFILE   "; break ;
  case  GETEVT__NOTAG      :  explanation ="NO TAG   "; break ;
  case  GETEVT__TAGRDERR   :  explanation ="TAG RD ERR  "; break ;
  case  GETEVT__TAGWRERR   :  explanation ="TAG WR ERR  "; break ;
  case  GETEVT__NOLMDFILE  :  explanation ="NO LMD FILE  "; break ;
  
}
      cout << "Explanation : " << explanation << endl;
        return result;
    }
    
    cout << "Succesfully opened: " << name.c_str() << ": " << result << endl;
    event_nr = 0;
    return GETEVT__SUCCESS;
}
//********************************************************************************************
int mbs_listner::get_next_event()
{
//     cout << __func__ << endl;

    int result = f_evt_get_event ( &input_channel, &event_header_ptr , ( INTS4** ) &buffer_header );
    if ( result != GETEVT__SUCCESS )
    {
        if(result == GETEVT__NOMORE) {
            //cout << "mbs_listner::get_next_event()  No more events " << endl;
            return result;
        }
        cout << "Some failure during f_evt_get_event"  << endl;
        return result;
    }
    ++event_nr;
    how_many_subs = 0 ;
    subev_nr = 0 ;
    return GETEVT__SUCCESS;   // czyli: 0
}
//********************************************************************************************
//s_ves10_1 *
int mbs_listner::get_next_subevent_if_any()
{
//     cout << __func__ << endl;
    if(how_many_subs == 0)
    {
        // so at first we ask how many data subs in this event
        how_many_subs =  f_evt_get_subevent (
                             ( s_ve10_1* ) event_header_ptr ,
                             0,                // <-- means just answer me: how many subs
                             ( INTS4** ) &subevent_header_ptr,
                             ( INTS4** ) &event_data_ptr,
                             &length_datawords
                         );
//         cout << "Nr of subevents = " << how_many_subs << endl;
        if(how_many_subs ==0)
        {
            // very strange
            cout << "mbs_listner::get_next_subevent_if_any() - Number of subevents should never be 0 " << endl;
            exit(22);
            //  return nullptr;
        }
    }

    if(how_many_subs > subev_nr)
    {
        ++subev_nr;
        int result = f_evt_get_subevent (
                         ( s_ve10_1* ) event_header_ptr ,
                         subev_nr,
                         ( INTS4** ) &subevent_header_ptr,
                         ( INTS4** ) &event_data_ptr,
                         &length_datawords
                     );


//         cout << "### subevent nr  "<<  subev_nr  << endl;
        if (result == GETEVT__SUCCESS )
        {

//             cout << "Sukces f_evt_get_subevent  wiec rozpakowanie eventu typu "
//                  << " l_dlen w bajtach = " << subevent_header_ptr->l_dlen
// 
//                  << " i_subtype= " << subevent_header_ptr->i_subtype
//                  << " i_type= " << subevent_header_ptr->i_type
// 
//                  << " h_control= " << (int) subevent_header_ptr->h_control
//                  << " h_subcrate= " << (int) subevent_header_ptr->h_subcrate
//                  << " procid= " << subevent_header_ptr->i_procid << endl;


        } else {
            cout << "Failure  f_evt_get_subevent  ev nr" << event_nr << "  subevent nr  "<<  subev_nr  << endl;

        }
        return result;
    }
    return GETEVT__NOMORE ;
}
//********************************************************************************************
void mbs_listner::close()
{
    f_evt_get_close ( &input_channel );
    //}
    //cout << "@ Closing file after analysing " << event_nr << " events "<< endl;
}
//********************************************************************************************
//********************************************************************************************



