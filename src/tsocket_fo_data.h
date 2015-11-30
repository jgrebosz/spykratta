#ifndef TSOCKET_FO_DATA_H
#define TSOCKET_FO_DATA_H

/**
The class to obtain the data coming online from the experiment

@author Jerzy Grebosz
*/

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "i2o.h"

#define SUCCESS 0
#define ERROR   1

#define MAXDATASIZE 262144
// was 1000 below here
#define MAXEVENTWORDS 10000

#include <iostream>
#include <string>
using namespace std;

// should be always 4 bytes (4*8=32)
typedef uint32_t daq_word_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class Tsocket_fo_data
{
    int sockfd;
    char *buf;          // [MAXDATASIZE];
    char * buf_header;          // [sizeof ( I2O_SEND_TO_BU_MESSAGE_FRAME ) ];
    int bytes_already_used;
    int bytes_in_the_buffer;
    daq_word_t *event_buf ;
    string host_name_remembered ;
    string port_nr_text_remembered;

public:
    Tsocket_fo_data(string host, string port_txt)
    {
        buf = new char[MAXDATASIZE];
        buf_header  = new char [sizeof(I2O_SEND_TO_BU_MESSAGE_FRAME) ];
        event_buf = new daq_word_t[MAXEVENTWORDS];
        open_the_socket(host, port_txt);
        bytes_already_used = 0 ;
        bytes_in_the_buffer = 0 ;
    }
    //-----------------------------------------------
    ~Tsocket_fo_data()
    {
        close_exotic_socket();
        delete []  event_buf;
        delete  [] buf_header;
        delete [] buf;
    }
    //*************************************************************************
    int open_the_socket(string hostname, string port_txt);
    void get_next_buffer();
    void close_exotic_socket();
    int reads(int sd, char *buf, int maxb);
    daq_word_t*  give_next_event(int *how_many_words) ;
    int reopen_the_socket()
    {
        close_exotic_socket();
        cout << "Trying to reopen the socket " << endl;
        return open_the_socket(host_name_remembered, port_nr_text_remembered);
    }
};

#endif
