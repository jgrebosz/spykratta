/***************************************************************************
 *   Copyright (C) 2010 by Jerzy Grebosz   *
 *   grebosz@dhcp-38-222   *
 ***************************************************************************/
#include "tbuffer_of_data_block.h"
#include <string.h>
//**************************************************************************
Tbuffer_of_data_block::Tbuffer_of_data_block()
{
    buffer = 0 ;
//     flag_all_read = false;
    block_number = 0 ;
    nr_of_file_already_analysed = 0 ;
    file_size = 1;
}
//**************************************************************************
Tbuffer_of_data_block::~Tbuffer_of_data_block()
{
    if(buffer) delete [] buffer;
}
//**************************************************************************
/*!
    \fn Tbuffer_of_data_block::read_in_next_block()
 */
int  Tbuffer_of_data_block::read_in_next_block()
{


    if(buffer != 0 && length_of_data_block > 0)
    {
        // zerowanie bufora po poprzednim bloku
        memset(buffer, 0x0, sizeof(char) *  length_of_data_block);
    }
top:
    length_of_data_block = 0;
    int when_to_stop = 999999999; //2000 ; //9999999 ;
    long long gdzie = 0 ;
    do
    {
        // Reading only the length value
        gdzie = plik.tellg();
        plik.read((char*) &length_of_data_block,   sizeof(int));


        if(plik.eof() || block_number > when_to_stop)
        {
            cout << "1 End of event file  while reading the block nr " << block_number << ", byte nr "
                 //<< gdzie
                 << endl;
            cout << "1 Read byte nr " << gdzie << " what is " << (100.0 * gdzie) / file_size  << "%  of " << file_size << endl;
            open_next_file_from_the_list();
            goto top;
        }
        if(plik.fail())
        {
            cout << "Error with the file reading " << endl;
            {
                Texception_input_file x(myFAIL);
                throw x;
            }
        } // end if
    }
    while(length_of_data_block <= 0   || length_of_data_block > 2000);
    //  the while loop avove is for situation of the lost synchronistaion of blocks
    // in the begining should be a length of the block, which is not a huge number. If we find
    // some 'realistic' number - we read the block, and then the other routines will search the
    // tokens of the block headers

//     if (length_of_data_block > 4000)
//     {
//         cout << "nONSENSE value of length_of_data_block = " << length_of_data_block << endl;
//         length_of_data_block = 40;
//     }

    gdzie = plik.tellg();

//     if (((double)gdzie)/file_size  > 0.30)
//     {
//         cout << "E Read byte nr " << gdzie << " what is " << (100.0 *gdzie)/file_size  << "%  of " << file_size<< endl;
//     }


    block_number++;

//   cout << "Reading next frame, length_of_data_block = " << length_of_data_block
//   << hex << ( length_of_data_block ) << dec
//   << endl;

//     length_of_data_block = 1000;   //FAKE !!!!!!!!!!!!!!!!!!

    if(buffer)
    {
        delete  []  buffer;
    }

    // reserving the bufer of the current length as a place for the following data
    buffer = new char[length_of_data_block];
//   cout << "buffer address = " << hex << ( int ) ( buffer ) << dec << endl;

    memset(buffer, 0x0, sizeof(char) *  length_of_data_block);
#ifdef NIGDY
    for(int i = 0  ; i < 10 /*length_of_data_block*/ ; i++)
    {
        cout << i << ")   --> " << hex << (unsigned short int) buf[i] << "<--,    " ;
        if(!((i + 1) % 10)) cout << endl;
    }
#endif

    memcpy(buffer, &length_of_data_block, sizeof(length_of_data_block));

    /*    cout << "After copying in " << endl;*/

//     for ( int i = 0  ; i < 10 /*length_of_data_block*/ ; i++ )
//     {
//         cout << i << ")   --> " << hex << ( unsigned short int ) buf[i] << "<--,    " ;
//         if(!((i+1) % 10)) cout << endl;
//     }
// cout << "Cont reading " << endl;
    gdzie = plik.tellg();
    plik.read(buffer + 4, length_of_data_block - 4);



    if(plik.eof() || block_number > when_to_stop)
    {
        if(block_number > when_to_stop)
        {
            cout << "Stopping after desired numbers of blocks " << block_number << endl;
        }

        cout << "After reading the consents of block - 2 End of event file  while reading the block nr " << block_number << ", byte nr " << gdzie
             << ", when_to_stop = " << when_to_stop
             << ", length_of_data_block = " << length_of_data_block
             << endl;

        cout << "2 Read byte nr " << gdzie << " what is " << (100.0 * gdzie) / file_size  << "%  of " << file_size << endl;

        open_next_file_from_the_list();
    }




    if(!plik)
    {
        cout << "Tbuffer_of_data_block::read_in_next_block()  ---> Error durig reading " << endl;
        Texception_input_file x(myFAIL);
        throw x  ;   // return -1;
    }
//     cout << "Start buf =" << hex<< int ( buffer )
//     << "End buf =" << hex<< int ( &buffer[length_of_data_block-1] )
//     << endl;

//     cout << "after reading the whole block " << endl;
//     for ( int i = 0  ; i < 10 /*length_of_data_block*/ ; i++ )
//     {
//         cout << i << ")   --> 0x" << hex << ( unsigned short int ) buffer[i] << "<--,    " ;
//         if ( ! ( ( i+1 ) % 10 ) ) cout << endl;
//     }

//     for ( int i = 0  ; i < 16 /*length_of_data_block*/ ; i+=4 )
//     {
//         unsigned int *ptr = ( unsigned int * ) &buffer[i];
//         cout << i << ")    0x" << hex << ( int ) ptr   << " value = " << *ptr << ",    " ;
//         if ( ! ( ( i+1 ) % 10 ) ) cout << endl;
//     }
//
//     cout << "& buffer = " << hex << ( int ) buffer << dec << endl;

    byte_read = 0 ;
    return   length_of_data_block;
}
//***************************************************************************
/*!
    \fn Tbuffer_of_data_block::open_file(string name)
 */
bool Tbuffer_of_data_block::open_file(string filename)
{

    if(filename[0] == '@')    // if this is a list of files to analyse
    {
        flag_list = true;
        cout << "This is a list of files. " << endl;
        string name_without_at = filename.substr(1);
        ifstream plik(name_without_at.c_str());
        if(!plik)
        {
            cout << "Can't open the file " << name_without_at << " with the list of files  --> called " << endl;
            return false;
        }

        // Readint the contents of the file with the list and creating a vector with these names.
        string single_name;
        for(; plik.good() ;)
        {
            plik >> single_name ;
            cout << "file on the list is: " << single_name << endl;

            if(!plik) break;
            list_of_names.push_back(single_name);
        }
        filename = list_of_names[0];
    }
    else  // if this is a name of the single file to analyse
    {
        flag_list = false;
        list_of_names.push_back(filename);  // put this name on the list
    }

    //  plik.close();
    plik.open(filename.c_str(), ios::binary);
    if(!plik)
    {
        cerr << "Can't open file " << filename << endl;
        return false;
    }

    plik.seekg(0, ios::end);
    file_size = plik.tellg();
    plik.seekg(0, ios::beg);
    long pocz = plik.tellg();
    file_size -= pocz;

    cout << "(Tbuffer_of_data_block::open_file)     File " << filename  << " successfully open for analysis " << endl;
    current_file = filename;
    return true;
}
//***************************************************************
bool Tbuffer_of_data_block::open_next_file_from_the_list()
{
    if(!flag_list)
    {
        cout << "The whole list file is analysed" << endl;
        Texception_input_file x(myEOF);
        throw x;
    }
    nr_of_file_already_analysed++;
    if(nr_of_file_already_analysed >= list_of_names.size())
    {
        cout << "All files are already analysed" << endl;
        Texception_input_file x(myEOF);
        throw x;
    }
    plik.close();
    cout << "$22 File nr " << nr_of_file_already_analysed << endl;
    if(nr_of_file_already_analysed >= list_of_names.size())
    {
        cout << "ALArm - bigger than the vector?" << endl;
    }

    cout
            << "Current file to analysis  has a name "
            << list_of_names[nr_of_file_already_analysed]
            << endl;

    // opening the data file to analyse
    plik.open(list_of_names[nr_of_file_already_analysed].c_str(),  ios::binary);
    if(!plik)
    {
        cerr << "Can't open file with events " << list_of_names[nr_of_file_already_analysed] << endl;
        Texception_input_file x(myFAIL);
        throw x;
    }
    cout << "File " << list_of_names[nr_of_file_already_analysed]
         << " successfully open for analysis " << endl;

    plik.seekg(0, ios::end);
    file_size = plik.tellg();
    plik.seekg(0, ios::beg);
    long pocz = plik.tellg();
    file_size -= pocz;

    current_file = list_of_names[nr_of_file_already_analysed];
    read_in_next_block();  // skiping info block?
    read_in_next_block();    // reading the real data block
// if successful,
    return true;
}
//*************************************************************************
/*!
    \fn Tbuffer_of_data_block::give_address(int byte)
 */
// char * Tbuffer_of_data_block::inspect_address ( int byte )
// {
//     if ( byte_read >= length_of_data_block )
//     {
//         cerr << "Buffer is over, I need to read the next one" << endl;
//         read_in_next_block();
//         return   &buffer[0];
//     }
//     else
//         return &buffer[byte];
// }
//*******************************************************************
char * Tbuffer_of_data_block::inspect_address()
{
    cout << "This function seems to be never used " << endl;
    if(byte_read >= length_of_data_block)
    {
        cerr << "Buffer is over, I need to read the next one" << endl;
        read_in_next_block();
        return   &buffer[0];
    }
    else
        return &buffer[byte_read];
}
//**************************************************************************
/*!
    \fn Tbuffer_of_data_block::info()
 */
void Tbuffer_of_data_block::info()
{
//   cout
//   << " Buffer info:    length_of_data_block= " << length_of_data_block
//   << " now  byte_read = " << byte_read << endl;
}
//**************************************************************************
