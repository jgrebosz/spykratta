/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 ***************************************************************************/
#ifndef TBUFFER_OF_DATA_BLOCK_H
#define TBUFFER_OF_DATA_BLOCK_H
/**
    @author Jerzy Grebosz <grebosz@dhcp-38-222>
*/
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;

enum error_nr_enum  { myEOF = -1,  myFAIL = -2 };
/////////////////////////////////////////////////////////////////////////////////////////////////////////
class Texception_input_file
{
public:
    error_nr_enum     error_nr;    // -1 EOF, -2 FAIL
    Texception_input_file(error_nr_enum er) : error_nr(er)
    {}
};
///////////////////////////////////////////////////////////////////////////////////////////////////////
class Tbuffer_of_data_block
{
    string current_file;
public:
    Tbuffer_of_data_block();

    ~Tbuffer_of_data_block();
    int  read_in_next_block();
    bool open_file(string name);

    vector<string> list_of_names;
    bool flag_list;
    int nr_of_file_already_analysed;
    bool open_next_file_from_the_list();

//        char * inspect_address ( int byte );
    char * inspect_address();

//         char * ga ( int byte, int size )   // to have it shorter
//         {
//             byte_read+= size;
//             if ( byte_read > length_of_data_block )
//             {
//                 cout << " F. ga ( int byte, int size ) --- byte_read > length_of_data_block  !!!!!!!!"<< endl;
//             }
//             return inspect_address ( byte );
//         }

    //---------------------------------------------------------------------------
    char * gg(int size)     // to have it shorter   (get new block of data)
    {
        if(byte_read >= length_of_data_block)
        {
//                cout << " F. ga ( int byte, int size ) --- byte_read > length_of_data_block  !!!!!!!!"<< endl;
            read_in_next_block();
        }
        char *ptr = &buffer[ byte_read ];
        byte_read += size;
        return ptr;
    }
    //-------------------------
    bool set_reading_point(int offs)
    {
        if(offs + byte_read >= length_of_data_block)
        {
            offs -= (length_of_data_block - byte_read);
            cout << " F. ga ( int byte, int size ) --- byte_read > length_of_data_block  !!!!!!!!" << endl;
            read_in_next_block();
        }
        byte_read = offs;
        return true;
    }
    //--------------------------------------------------------
    char * get_reading_point()
    {
//             return &buffer[byte_read];
        if(byte_read >= length_of_data_block)
        {
            cout << " F. ga ( int byte, int size ) --- byte_read > length_of_data_block  !!!!!!!!" << endl;
            read_in_next_block();
        }
        return   &buffer[ byte_read ];
    }
    //----------------------------------------------------------
    char * get_key_data()
    {
//             return &buffer[byte_read];
        if(byte_read >= length_of_data_block)
        {
//                 cout << " F. get_key_data () --- byte_read > length_of_data_block  !!!!!!!!"<< endl;
            read_in_next_block();
        }
        char *ptr = &buffer[ byte_read ];
        byte_read += 20; // size of the key
        return  ptr;
    }
    //******************************************
    int jump_bytes_read(int how_many)
    {
        if(byte_read + how_many >= length_of_data_block)
        {
            how_many -= (length_of_data_block - byte_read);
            read_in_next_block();
        }
        else byte_read += how_many;

        return byte_read;
    }
    //*******************************************
    int give_bytes_used()
    {
        return byte_read;
    }
    //*******************************************
    void jump_over_header()
    {
        // a header is 20 bytes
        jump_bytes_read(20);
    }
    //*******************************************
    void info();
    //*******************************************
    string give_current_filename()
    {
        return current_file ;
    }
    //*******************************************
protected:
    char * buffer;
    int length_of_data_block;   // in bytes
    ifstream plik;
    int byte_read;
    int block_number;
    long long file_size ;
//         bool flag_all_read;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
