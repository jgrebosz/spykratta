#include "tsocket_fo_data.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>

//*************************************************************************
int Tsocket_fo_data::open_the_socket(string hostname, string port_txt)
{
    host_name_remembered = hostname ;
    port_nr_text_remembered = port_txt;

    struct hostent *he;
    struct sockaddr_in their_addr;
    struct sockaddr_in l_addr;
    //char *hostname;
    int port;
    int sock_opt = 1;
    //int nread;
    //cout << "F. Tsocket_fo_data::open_the_socket (string hostname, string port_txt) " << endl;
    //hostname = host.c_str();
    port = atoi(port_txt.c_str());

    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("open_the_socket:  Error from function 'socket' ");
        exit(1);
    }

    l_addr.sin_family = PF_INET;
    l_addr.sin_port = htons(0);
    l_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(& (l_addr.sin_zero), '\0', 8);

    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sock_opt, sizeof(int)) == -1)
    {
        perror("open_the_socket:  Error from function   'setsockopt' ");
        exit(2);
    }

    if(bind(sockfd, (struct sockaddr *) &l_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("open_the_socket:  Error from function  'bind'");
        exit(3);
    }

    //   cout << " Before  gehost by name\n"  << endl;

    if((he = gethostbyname(hostname.c_str())) == NULL)
    {
        perror("open_the_socket:  Error from function 'gethostbyname' ");
        exit(4);
    }
    //     cout << "success w gehost by name\n"  << endl;

    their_addr.sin_family = PF_INET;
    their_addr.sin_port = htons(port);
    their_addr.sin_addr = * ((struct in_addr *) he->h_addr);
    memset(& (their_addr.sin_zero), '\0', 8);

    int how_many = 100000000 ;
    for(int i = 0 ; i < how_many ; i++)
    {
        if(connect(sockfd, (struct sockaddr *) &their_addr, sizeof(struct sockaddr)) == -1)
        {
            //       perror ( "\nopen_the_socket:  Error from function 'connect' " );
            cerr << "open_socket:  " << hostname << ", can't reopen the socket, (perhaps DAQ does not give data now)..."
//             Error from function 'connect'  - try nr " << i
//                  << " (of " << how_many << ")"
                 << endl;
            sleep(5);
        }
        else
        {
            //       success = true;
            break;
        }
        if(i == (how_many - 3))
        {
            cerr << "Too many tries, it is hopeless..." << endl;
            exit(-6);
        }

    } // end for


    bytes_already_used = 0 ;
    bytes_in_the_buffer = 0 ;
    return 0 ; // success

}
//********************************************************************************************************************************************************************
void Tsocket_fo_data::get_next_buffer()
{
    bytes_already_used = 0 ;
    bytes_in_the_buffer = 0;
    // sequence or reading
    //     for ( ;; )
    {
        /* receive header */

        //         cout << "Reading the HEADER " << endl;
        int nread =  0;
        bool success = false;

        do
        {
            if((nread = reads(sockfd, buf_header, sizeof(I2O_SEND_TO_BU_MESSAGE_FRAME))) == ERROR)
            {
                cerr << "Error while calling reads (for the header)"
                << " perahaps the DAQ is stopped... "<< endl;
                //         return;
                reopen_the_socket();
            }
            else success = true;
        }
        while(!success);

        //         cout <<  "receive header        nread: " << nread << endl;

        I2O_SEND_TO_BU_MESSAGE_FRAME *i2omsg;
        //     int orgid;
        //     int *w1;

        i2omsg = (I2O_SEND_TO_BU_MESSAGE_FRAME *) buf_header;
        int buf_sz = i2omsg->PvtMessageFrame.StdMessageFrame.MessageSize << 2;

        //         cout <<  "buf_sz: = " <<  buf_sz << endl;

        int data_sz = buf_sz - sizeof(I2O_SEND_TO_BU_MESSAGE_FRAME);

        /* receive data ----------------------------------------------------------------------------------*/
        //         cout << "Reading the DATA " << endl;
        if((nread = reads(sockfd, buf, data_sz)) == ERROR)
        {
            cerr << "Error while calling reads  (for the data)" << endl;
            return ;
        }

        if(nread > 0)
        {
            //             cout << "afer 'reads DATA ' the return value is nread =" << nread << endl ;
        }
        else
        {
            // simulation
            //#define SIMULATION_SOCKET
#ifdef SIMULATION_SOCKET
            daq_word_t *abuf = (daq_word_t *) buf;

            abuf[0] = 0x23;
            abuf[1] = 0x34;
            abuf[2] = 0x45;
            abuf[3] = 0x56;
            abuf[4] = 0xffffffff;
            abuf[5] = 0x5;
            abuf[6] = 0x6;
            abuf[7] = 0x7;
            abuf[8] = 0x8;
            abuf[9] = 0xffffffff;
            nread = bytes_in_the_buffer = 10 * sizeof(abuf[0]);

#endif

        }
        bytes_in_the_buffer = nread;
    }
}
//*************************************************
void Tsocket_fo_data::close_exotic_socket()
{
    // closing the socket
    close(sockfd);
    //   return 0;
}
//***************************************************************************
int Tsocket_fo_data::reads(int sd, char *buf, int maxb)
{
    int nleft  = maxb;
    int nread;

    //     cout << "Tsocket_fo_data::reads" << endl;
    while(nleft > 0)
    {
        nread = recv(sd, buf, nleft, 0);
        if(nread < 0)
        {
            perror(" Tsocket_fo_data::reads    - error:  Cannot receive data ");
            return ERROR;
        }
        else if(nread == 0)
        {
            cout <<  "Tsocket_fo_data::reads - error: Connection closed by client\n"  << endl;  // the Mr. Toniolo hardware sometimes makes this error
            return ERROR;
        }
        //         cout << "recv:  nread = " << nread << endl;
        nleft -= nread;
        buf += nread;
    }
    bytes_already_used = 0 ;
    bytes_in_the_buffer = (maxb - nleft);
    //     cout << "end of function, return value =  " << (bytes_in_the_buffer)
    //     << "---------------------------------------------------" << endl;

    return bytes_in_the_buffer ; /* return >= 0 */    // how many bytes are read
}
//***************************************************************************************************************
daq_word_t*  Tsocket_fo_data::give_next_event(int *how_many_words)     // virtual
{
#ifdef NIGDY
    /*     Hi Jurek,
    I send you a compressed file which contains about 50MB of raw data taken
    using 2 ADC Caen V785 and 1 Scaler. I have attached the manual of the
    V785 ADC, that may be useful to understand the format of the data coming
    from the ADCs (see page 41). The Scaler can be ignored.
    The file zipped in data4jurek01.tgz has this format:

    File Header 32 bytes
    first data Block:
    first ADC Block (see output buffer section in Caen V785 Manual,
    page 41):
    Header 32 bits - first is 1a002000
    16 Data Words 32 bits
    End Of Block 32 bits - first is 1c000000
    second ADC Block (as before)
    Header 32 bits - first is 22002000
    16 Data Words 32 bits each
    End Of Block 32 bits - first is 2c7c0000
    Scaler Block
    ignore this...
    Trailer 32 bits, always ffffffff
    second data Block:
    ...
    ...
    File Trailer 24 bytes

    You can see the content of the file using a tool like octaldump (od),
    and the values I wrote are read with the command
    od -tx4 data_pc1203_r1.i0 | more

    I Hope you can use this data easily. Please write me some comments soon...
    Thanks a lot!

    ciao

    pietro

    ps. we have used V785, not V785N !

    #######################################################################




    */
#endif // NIGDY

    //     cout << "Getting the next event from the socket buffer and returning the pointer to the data"
    //     << endl;

    // reading something from the file into the buffer

    // skipping file header 40/32 bytes was done while opening the file
    //   long *event_ptr = ( long * ) &buf[0];

    int skok = sizeof(daq_word_t);

    int event_length = 0 ;
    //int nr_word = 0 ;
    for(event_length = 0 ;  ; event_length++)
    {
        // reading one long word?
        //        file.read ( ( char* ) ptr, skok );

        if(bytes_already_used >= bytes_in_the_buffer)
        {
            /*           cout << "bytes_already_used= " << bytes_already_used
                       << ", (bytes_in_the_buffer/sizeof(daq_word_t) = " << (bytes_in_the_buffer/sizeof(daq_word_t))
                       << endl;
            */           // buffer finished, we need the new delivery
            get_next_buffer();
            if(bytes_in_the_buffer == 0)
            {

                *how_many_words = 0 ;
                return NULL;
            }

            // what happens if the was an error during reading?


            //         if ( file.eof() )
            //         {
            //             *how_many_words = 0;
            //             TjurekEventEndException ex;
            //             throw ex;
            //         }
            //         if ( !file )
            //         {
            //             cout << "Error while reading event !!!!!!" << endl;
            //             exit ( 3 );
            //         }

        }
        daq_word_t * where_from = (daq_word_t*) &buf[bytes_already_used];
        event_buf[event_length] =  *where_from;
        /*        cout << "Data word = " << hex << event_buf[event_length]
                << dec << ", event_length="<< event_length << endl;*/
        bytes_already_used += skok;

        if(event_buf[event_length] == 0xffffffff /*|| event_length > 1000*/)
        {
            //             nr_of_current_event++;
            //             if ( nr_of_current_event > 236000 )
            //             {
            //                 cout << "End of event nr " << nr_of_current_event
            //                 << ", after " << event_length << " bytes" << endl;
            //             }

            *how_many_words = event_length ; // event_length / skok;
            break;
        }
        else
        {
            //             if ( nr_of_current_event > 236000 )
            //             {
            //                 cout << event_length << ")   read longword "
            //                 << hex << event_buf[event_length]  << " --->ev nr 0x" << event_length << dec << endl;
            //             }
        }
    }
    // setting some values (for example length of valid data)

    // returning the pointer to the memory where the event is now
    return (daq_word_t*) event_buf;
}







