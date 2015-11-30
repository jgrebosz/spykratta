#ifndef MBS_C_TO_CPP_H
#define MBS_C_TO_CPP_H

#ifdef __cplusplus
extern "C" 
{
#endif

#include "typedefs.h"
#include "s_filhe_swap.h"
#include "s_bufhe_swap.h"
#include "s_ve10_1.h"
#include "s_ves10_1.h"
#include "s_evhe_swap.h"
#include "f_evt.h"

typedef enum
{
	MBS_File				= GETEVT__FILE,
	MBS_Stream				= GETEVT__STREAM,
	MBS_TransportServer		= GETEVT__TRANS,
	MBS_EventServer			= GETEVT__EVENT,
	MBS_RemoteEventServer	= GETEVT__REVSERV
} MBS_StreamType;

//! @brief interface for the subevent callback function
//! @param event the event number
//! @param subevent the subevent number 
//! @param data pointer to the array of long word data
//! @param length number of long words in the data array 
typedef void (*MBS_SubEventCallback)(int event, int subevent, INTU4* data, int length);

//! @brief this function will enter a never ending loop and will call the given callback function for each subevent it recieves
//! @param type specify whether to read a file (MBS_File) or a data stream (MBS_Stream for example)
//! @param name the name of the stream or file
//! @param callback the callback function that will handle the subevents
//! @param samples the MBS event API is configured to read only one event of 'samples' events
int mbs_Listener(MBS_StreamType type, const char *name, MBS_SubEventCallback callback, int samples);



  
#ifdef __cplusplus
} // extern "C"
#endif


#endif

